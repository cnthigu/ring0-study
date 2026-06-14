# Capítulo 7 — Tokens, Privilégios e UAC

> Livro didático baseado em *Windows Internals, 7th Edition*.
> Atualizado para Windows 11 com exemplos práticos em C++ moderno.

> Navegação: [Anterior](capitulo-07-01-modelo-seguranca.md) | [Índice Cap.7](capitulo-07.md) | [Próximo](capitulo-07-03-logon-autenticacao.md)

---

## Privilégios: poderes especiais além do ACL

ACLs controlam acesso a **objetos**. **Privilégios** controlam ações **do sistema** que vão além de objetos individuais:

```
Privilégios importantes (por nome de constante e displayname):

SeDebugPrivilege              → Debug Programs
  → permite OpenProcess em qualquer processo (mesmo PPL parcialmente)
  → base de debuggers, ferramentas de análise

SeBackupPrivilege             → Back up files and directories
  → ignora DACLs de arquivo para leitura
  → permite backup mesmo sem permissão de Read nos arquivos

SeRestorePrivilege            → Restore files and directories
  → ignora DACLs para escrita, pode mudar owner

SeLoadDriverPrivilege         → Load and unload device drivers
  → permite carregar drivers de kernel
  → extremamente perigoso — base de escalada de privilégio

SeTcbPrivilege                → Act as part of the operating system
  → "Trusted Computer Base" — pode criar qualquer tipo de token
  → usado por Lsass para criar tokens de logon

SeImpersonatePrivilege        → Impersonate a client after authentication
  → pode impersonar tokens de outros usuários
  → técnica de escalada: "Potato" attacks (PrintSpoofer, JuicyPotato)

SeAssignPrimaryTokenPrivilege → Replace a process level token
  → pode associar qualquer token primário a um processo

SeLockMemoryPrivilege         → Lock pages in memory
  → VirtualLock / AWE / Large Pages

SeSecurityPrivilege           → Manage auditing and security log
  → modificar SACLs, acessar Security Event Log

SeCreateSymbolicLinkPrivilege → Create symbolic links
  → criar symlinks sem ser admin (em alguns cenários)
```

Privilégios têm dois estados: **habilitado** e **desabilitado**. Um token pode ter um privilégio mas desabilitado — e só funciona se explicitamente habilitado:

```cpp
// Habilitar um privilégio:
BOOL EnablePrivilege(HANDLE hToken, LPCWSTR privilegeName) {
    TOKEN_PRIVILEGES tp = {};
    tp.PrivilegeCount = 1;
    
    if (!LookupPrivilegeValueW(nullptr, privilegeName, &tp.Privileges[0].Luid))
        return FALSE;
    
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    
    if (!AdjustTokenPrivileges(hToken, FALSE, &tp, 0, nullptr, nullptr))
        return FALSE;
    
    return GetLastError() != ERROR_NOT_ALL_ASSIGNED;
}

// Uso:
HANDLE hToken;
OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
EnablePrivilege(hToken, SE_DEBUG_NAME);
CloseHandle(hToken);
```

---

## UAC: User Account Control

O **UAC** é o sistema que permite que administradores executem como usuários padrão na maior parte do tempo, e elevem apenas quando necessário.

### O Split Token

Quando um administrador faz logon no Windows, o Lsass cria **dois tokens**:

```
Admin faz logon → Lsass cria:

Token 1 (Filtered/Standard Token):
  → Grupos: Users, Everyone, Logon SID (SEM "Administrators", SEM "BUILTIN\Admins")
  → Privilégios: apenas os básicos (sem SeDebug, SeBackup, etc.)
  → IntegrityLevel: Medium
  → Este token é usado para explorer.exe e apps normais

Token 2 (Full Token / Elevated Token):
  → Grupos: TODOS os grupos incluindo Administrators
  → Privilégios: todos os privilégios do admin habilitados
  → IntegrityLevel: High
  → Só usado quando o usuário clica "Sim" no prompt UAC
```

```
Como o UAC funciona quando você clica "Executar como administrador":

  1. Shell cria processo filho com ShellExecuteEx e verb "runas"
  2. AppInfo service (um serviço SYSTEM) recebe a requisição
  3. AppInfo exibe o prompt UAC via Consent.exe (processo SYSTEM em Desktop isolado)
  4. Usuário clica "Sim" → AppInfo usa CreateProcessAsUser com o elevated token
  5. O novo processo recebe o token elevado (High integrity, admins group ativo)
```

### Detectar se o processo está elevado

```cpp
// Verificar se o processo atual está rodando com token elevado:
BOOL IsElevated() {
    HANDLE hToken;
    TOKEN_ELEVATION elevation;
    DWORD size;
    
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
        return FALSE;
    
    BOOL result = GetTokenInformation(hToken, TokenElevation,
                                      &elevation, sizeof(elevation), &size);
    CloseHandle(hToken);
    
    return result && elevation.TokenIsElevated;
}

// Verificar tipo de elevação:
TOKEN_ELEVATION_TYPE elevType;
GetTokenInformation(hToken, TokenElevationType, &elevType, sizeof(elevType), &size);
// TokenElevationTypeDefault → não admin, ou UAC desabilitado
// TokenElevationTypeFull    → admin com token elevado (Full token)
// TokenElevationTypeLimited → admin com token filtrado (Standard token)
```

---

## Token Restrito: sandboxing via CreateRestrictedToken

`CreateRestrictedToken` cria um novo token com direitos **reduzidos** — subconjunto do token original:

```cpp
// Criar token restrito para lançar processo em sandbox:
HANDLE hOriginalToken;
OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &hOriginalToken);

// SIDs a negar (mesmo que o ACL permita, o token nega):
SID_AND_ATTRIBUTES sidsToDisable[1];
// Desabilitar o grupo Administrators:
SID adminSid = { SID_REVISION, 2, {0,0,0,0,0,5}, {32, 544} }; // BUILTIN\Administrators
sidsToDisable[0].Sid = &adminSid;
sidsToDisable[0].Attributes = 0;

// Privilégios a remover:
LUID_AND_ATTRIBUTES privsToDelete[1];
LookupPrivilegeValueW(nullptr, SE_DEBUG_NAME, &privsToDelete[0].Luid);
privsToDelete[0].Attributes = 0;

HANDLE hRestrictedToken;
CreateRestrictedToken(
    hOriginalToken,
    DISABLE_MAX_PRIVILEGE,  // remover todos os privilégios
    1, sidsToDisable,       // grupos a desabilitar
    1, privsToDelete,       // privilégios a remover
    0, nullptr,             // SIDs restritivos extras
    &hRestrictedToken);

// Lançar processo com token restrito:
STARTUPINFOW si = { sizeof(si) };
PROCESS_INFORMATION pi;
CreateProcessAsUserW(hRestrictedToken, L"programa-sandbox.exe",
    nullptr, nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi);

CloseHandle(hRestrictedToken);
CloseHandle(hOriginalToken);
```

---

## Integridade Mandatória (MIC) em detalhe

O MIC adiciona uma camada de segurança **além das ACLs**. Mesmo que um processo tenha direitos de ACL, ele é bloqueado se o nível de integridade for insuficiente:

```
Regra MIC (No Write Up):
  → Um processo de integridade BAIXA não pode escrever em objetos de integridade MÉDIA ou ALTA
  → Mesmo que o DACL permita!

  Exemplo: processo de Medium integrity não pode modificar processo de High integrity
  (base da proteção de anti-cheat e anti-tamper)

Regra MIC (No Read Up — opcional):
  → No Write Up é padrão; No Read Up é configurável por objeto
  → AppContainers usam No Read Up para isolamento mais forte

Configurar integridade de um objeto:
  LABEL_SECURITY_INFORMATION
  SetNamedSecurityInfo com SACL contendo mandatory label
```

```cpp
// Criar arquivo com integridade Low (sandbox pode criar, mas não modificar depois):
void SetLowIntegrity(LPCWSTR filePath) {
    PSECURITY_DESCRIPTOR pSD;
    ConvertStringSecurityDescriptorToSecurityDescriptorW(
        L"S:(ML;;NW;;;LW)",  // SACL: Mandatory Label, No Write, Low integrity
        SDDL_REVISION_1, &pSD, nullptr);
    
    PACL pSacl;
    BOOL saclPresent, saclDefaulted;
    GetSecurityDescriptorSacl(pSD, &saclPresent, &pSacl, &saclDefaulted);
    
    SetNamedSecurityInfoW((LPWSTR)filePath, SE_FILE_OBJECT,
        LABEL_SECURITY_INFORMATION, nullptr, nullptr, nullptr, pSacl);
    
    LocalFree(pSD);
}
```

---

## Impersonation e Escalada de Privilégio

O `SeImpersonatePrivilege` é particularmente importante no contexto de segurança:

**Serviços** têm esse privilégio por padrão. Se um serviço pode ser comprometido para criar um token de outro usuário e impersoná-lo, o atacante escala para o nível daquele usuário.

As técnicas "Potato" (RottenPotato, JuicyPotato, PrintSpoofer) exploram justamente isso:
1. Processo com SeImpersonatePrivilege
2. Força SYSTEM a se conectar via pipe ou COM
3. Captura e usa o token de SYSTEM via `ImpersonateNamedPipeClient`

Mitigação: serviços não deveriam rodar como SYSTEM — preferir LOCAL SERVICE / NETWORK SERVICE com SeImpersonatePrivilege removido quando não necessário.

---

*Próximo: [Logon e Autenticação](capitulo-07-03-logon-autenticacao.md)*
