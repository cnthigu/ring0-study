# Capítulo 7 — Modelo de Segurança do Windows

> Livro didático baseado em *Windows Internals, 7th Edition*.
> Atualizado para Windows 11 com exemplos práticos em C++ moderno.

> Navegação: [Índice Cap.7](capitulo-07.md) | [Próximo](capitulo-07-02-tokens-privilegios.md)

---

## Os componentes de segurança do Windows

O modelo de segurança do Windows é implementado por vários componentes que trabalham juntos:

```
Componentes de Segurança:

  USER MODE:
  ├── Lsass.exe (Local Security Authority Subsystem Service)
  │     ├── lsasrv.dll       ← núcleo do LSA
  │     ├── msv1_0.dll       ← pacote NTLM
  │     ├── kerberos.dll     ← pacote Kerberos
  │     ├── wdigest.dll      ← pacote WDigest (legacy)
  │     └── tspkg.dll        ← CredSSP (RDP)
  └── Winlogon.exe
        └── LogonUI.exe      ← interface gráfica de login

  KERNEL MODE:
  ├── SRM (Security Reference Monitor) — em ntoskrnl.exe
  │     → verifica se um token tem acesso a um objeto
  │     → implementa SeAccessCheck, SePrivilegeCheck
  ├── Kernel object ACL checking
  └── Win32k.sys (GDI/User segurança de janelas)
```

O fluxo básico de verificação de acesso:

```
1. Thread tenta abrir um objeto (arquivo, chave de registro, processo...)
2. ObOpenObjectByName chama SRM: "este token tem acesso?"
3. SRM compara:
   - Grupos e SIDs do token com as ACEs do DACL do objeto
   - Nível de integridade MIC (Mandatory Integrity Control)
   - Restrições do token (se token restrito)
4. Retorna: GRANT ou DENY
```

---

## SID: Security Identifier

Cada usuário, grupo e identidade especial é representado por um **SID** — um identificador único imutável:

```
Formato de SID:
S-1-5-21-<domínio>-<RID>

Exemplos comuns:
S-1-1-0           → Everyone (Todos)
S-1-5-18          → NT AUTHORITY\SYSTEM
S-1-5-19          → NT AUTHORITY\LOCAL SERVICE
S-1-5-20          → NT AUTHORITY\NETWORK SERVICE
S-1-5-21-X-X-X-500 → Administrador local da máquina
S-1-5-21-X-X-X-501 → Guest local
S-1-5-21-X-X-X-1000+ → Usuários criados pelo administrador
S-1-16-0          → Integridade Untrusted
S-1-16-4096       → Integridade Low
S-1-16-8192       → Integridade Medium (usuário padrão)
S-1-16-12288      → Integridade High (administrador elevado)
S-1-16-16384      → Integridade System
```

```cpp
// Converter SID para string legível:
PSID pSid; // obtido de alguma forma
LPWSTR sidStr;
ConvertSidToStringSidW(pSid, &sidStr);
wprintf(L"SID: %s\n", sidStr);
LocalFree(sidStr);

// Converter string para SID:
PSID pSid2;
ConvertStringSidToSidW(L"S-1-5-18", &pSid2);
// ... usar pSid2 ...
FreeSid(pSid2);
```

---

## Access Tokens: a identidade de um processo/thread

Cada processo tem um **Access Token** que define sua identidade de segurança:

```
Token de Acesso (simplificado):
  ├── UserAndGroups[]        ← SID do usuário + SIDs de grupos
  │     ├── User SID + atributos
  │     ├── Everyone (S-1-1-0)
  │     ├── Grupos do domínio
  │     └── Grupos de sessão (Logon SID, etc.)
  ├── Privileges[]           ← lista de privilégios e seus estados
  ├── PrimaryGroupSid        ← grupo primário (para objetos criados)
  ├── DefaultDacl            ← DACL padrão para objetos criados
  ├── RestrictedSids[]       ← SIDs de restrição (token restrito)
  ├── TokenType              ← Primary (processo) ou Impersonation (thread)
  ├── ImpersonationLevel     ← SecurityIdentification, Delegation, etc.
  ├── MandatoryPolicy        ← política de integridade MIC
  └── IntegrityLevel         ← SID de nível de integridade (S-1-16-*)
```

```cpp
// Abrir o token do processo atual:
HANDLE hToken;
OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken);

// Ler o IntegrityLevel (MIC level):
DWORD size;
GetTokenInformation(hToken, TokenIntegrityLevel, nullptr, 0, &size);
auto pil = (TOKEN_MANDATORY_LABEL*)malloc(size);
GetTokenInformation(hToken, TokenIntegrityLevel, pil, size, &size);

DWORD rid = *GetSidSubAuthority(pil->Label.Sid, 0);
const wchar_t* level =
    rid >= SECURITY_MANDATORY_SYSTEM_RID    ? L"System" :
    rid >= SECURITY_MANDATORY_HIGH_RID      ? L"High" :
    rid >= SECURITY_MANDATORY_MEDIUM_RID    ? L"Medium" :
    rid >= SECURITY_MANDATORY_LOW_RID       ? L"Low" : L"Untrusted";
wprintf(L"Integrity Level: %s (RID=0x%x)\n", level, rid);

free(pil);
CloseHandle(hToken);
```

---

## DACL e ACEs: controle de acesso discricionário

O acesso a cada objeto protegível é controlado por uma **DACL (Discretionary Access Control List)** — uma lista de **ACEs (Access Control Entries)**:

```
Security Descriptor de um arquivo:
  ├── Owner SID        ← quem é o dono do objeto
  ├── Primary Group    ← grupo primário (legado Unix)
  ├── DACL             ← lista de quem pode fazer o quê
  │     ├── ACE 1: Allow  DOMAIN\UserA     ReadData | ExecuteFile
  │     ├── ACE 2: Allow  BUILTIN\Admins  FullControl
  │     └── ACE 3: Deny   Everyone        WriteData
  └── SACL             ← lista de auditoria (System ACL)
        └── ACE 1: Audit Everyone  SuccessAndFailure  WriteData

Algoritmo de verificação (SeAccessCheck):
  1. Se token é SYSTEM: acesso concedido (exceto objetos com DACL explícita)
  2. Se token é Owner: acesso a ReadControl + WriteDAC concedido
  3. Percorrer DACL de cima para baixo:
     a. Se ACE é DENY e o SID do ACE está no token: NEGAR imediatamente
     b. Se ACE é ALLOW e o SID do ACE está no token: acumular direitos
  4. Verificar se direitos acumulados satisfazem o acesso solicitado
  5. Verificar MIC: nível do objeto ≤ nível do token?
```

```cpp
// Ver o Security Descriptor de um arquivo:
PSECURITY_DESCRIPTOR pSD;
PACL pDacl;
PSID pOwner;
BOOL bOwnerDefault;

GetNamedSecurityInfoW(
    L"C:\\meuarquivo.txt",
    SE_FILE_OBJECT,
    OWNER_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION,
    &pOwner, nullptr, &pDacl, nullptr, &pSD);

// Converter para formato legível:
LPWSTR sdStr;
ConvertSecurityDescriptorToStringSecurityDescriptorW(
    pSD, SDDL_REVISION_1,
    OWNER_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION,
    &sdStr, nullptr);
wprintf(L"SDDL: %s\n", sdStr);

LocalFree(sdStr);
LocalFree(pSD);
```

---

## SACL e Auditoria

A **SACL (System Access Control List)** define quais acessos são auditados e geram eventos de segurança (Event ID 4663, 4656, etc.):

```cpp
// Configurar auditoria de arquivo (requer SeSecurityPrivilege):
HANDLE hToken;
OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken);

// Ativar SeSecurityPrivilege:
TOKEN_PRIVILEGES tp = {};
tp.PrivilegeCount = 1;
LookupPrivilegeValueW(nullptr, SE_SECURITY_NAME, &tp.Privileges[0].Luid);
tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
AdjustTokenPrivileges(hToken, FALSE, &tp, 0, nullptr, nullptr);
CloseHandle(hToken);

// Criar SACL que audita acesso de escrita por qualquer pessoa:
EXPLICIT_ACCESS ea = {};
ea.grfAccessPermissions = FILE_WRITE_DATA;
ea.grfAccessMode = SET_AUDIT_FAILURE;  // auditar falhas de escrita
ea.grfInheritance = NO_INHERITANCE;
BuildTrusteeWithSidW(&ea.Trustee,
    (PSID)&(SID){1, 1, {0,0,0,0,0,1}, {0}}); // Everyone

PACL pSacl;
SetEntriesInAclW(1, &ea, nullptr, &pSacl);

SetNamedSecurityInfoW(
    L"C:\\arquivo-auditado.txt",
    SE_FILE_OBJECT,
    SACL_SECURITY_INFORMATION,
    nullptr, nullptr, nullptr, pSacl);
LocalFree(pSacl);
```

---

## Impersonation: executar como outro usuário

**Impersonation** permite que uma thread execute temporariamente como um usuário diferente:

```cpp
// Servidor que atende requests de clientes via named pipe:
// Após conectar um cliente, impersonar o cliente para verificar acesso:

BOOL ok = ImpersonateNamedPipeClient(hPipe);
// Agora a thread tem o token do cliente

// Fazer operações no contexto do cliente:
HANDLE hFile = CreateFileW(L"C:\\dados-do-cliente.txt",
    GENERIC_READ, 0, nullptr, OPEN_EXISTING, 0, nullptr);
// O acesso será verificado com o token DO CLIENTE, não do servidor

// Reverter para o token original:
RevertToSelf();

// Impersonation via token explícito:
HANDLE hClientToken;
OpenThreadToken(GetCurrentThread(), TOKEN_ALL_ACCESS, FALSE, &hClientToken);
// ... obter ou duplicar um token ...
SetThreadToken(nullptr, hClientToken);  // nullptr = thread atual
// ... executar como o token alvo ...
RevertToSelf();
CloseHandle(hClientToken);
```

---

## Object Manager e segurança de objetos kernel

Cada objeto kernel (HANDLE) tem um Security Descriptor. O Object Manager verifica acesso ao abrir handles:

```
Objeto HANDLE no Windows:
  → NÃO é um ponteiro — é um índice na Handle Table do processo
  → Cada entrada na Handle Table tem: ponteiro para objeto + máscara de acesso

Abertura de objeto:
  ObOpenObjectByName → verifica token da thread vs DACL do objeto
  → Se GRANT: cria entrada na Handle Table com access mask solicitado
  → Handle retornado encapsula: qual objeto + quais direitos

Uso do handle:
  WriteFile(hFile, ...) → I/O Manager verifica: handle tem FILE_WRITE_DATA?
  → Se não: ACCESS_DENIED mesmo que você tenha o handle

  // Portanto: handles são "capabilities com direitos limitados"
  HANDLE hReadOnly = CreateFileW(..., GENERIC_READ, ...);
  // hReadOnly não pode ser usado para escrita MESMO que você tenha DACL de escrita
```

---

*Próximo: [Tokens, Privilégios e UAC](capitulo-07-02-tokens-privilegios.md)*
