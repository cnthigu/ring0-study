# Capítulo 1 — Objetos, Handles e Segurança

> Livro didático baseado em *Windows Internals, 7th Edition*.
> Atualizado para Windows 11 com exemplos práticos em C++ moderno.
> Repositório: [ring0-study](https://github.com/cnthigu/ring0-study)

> Navegação: [Anterior](capitulo-01-06-hipervisor-vbs.md) | [Índice](README.md) | [Próximo](capitulo-01-08-ferramentas.md)

---

## Tudo é um objeto

No Windows, quase todos os recursos gerenciados pelo kernel são implementados como **objetos** — estruturas de dados com um conjunto bem definido de operações e um sistema uniforme de contagem de referências, nomens, e controle de acesso.

Processos, threads, arquivos, eventos, mutexes, semáforos, seções de memória compartilhada, portas de comunicação, tokens de segurança — todos são objetos do kernel gerenciados pelo **Object Manager** (ntoskrnl.exe).

Por que objetos? Porque isso dá ao kernel quatro propriedades valiosas de uma só vez:

1. **Contagem de referências**: um objeto só é destruído quando ninguém mais o referencia. Elimina a classe inteira de erros use-after-free no nível do kernel.
2. **Namespace uniforme**: objetos podem ser nomeados no Object Manager namespace (ex: `\BaseNamedObjects\MinhaFila`) e descobertos por qualquer processo que saiba o nome.
3. **Segurança uniforme**: todo objeto pode ter um Security Descriptor. O mesmo mecanismo que protege arquivos também protege processos, threads e mutexes.
4. **Quotas**: o Object Manager pode limitar quantos objetos de cada tipo um processo pode criar — prevenindo DoS por esgotamento de recursos.

---

## Handles: a janela para os objetos

Código em user mode nunca tem um ponteiro direto para um objeto do kernel — isso seria um desastre de segurança (e impossível com HVCI). Em vez disso, o processo recebe um **handle**: um índice na tabela de handles do processo, que o kernel usa para encontrar o objeto real.

```
Processo (user mode)                   Kernel
┌─────────────────────────┐      ┌──────────────────────────────┐
│  Tabela de Handles       │      │  Object Manager              │
│                          │      │                              │
│  Handle 0x4 → ─────────────────→  EPROCESS (PID=1234)        │
│  Handle 0x8 → ─────────────────→  FILE_OBJECT ("log.txt")    │
│  Handle 0xC → ─────────────────→  KEVENT (evento de sync)    │
└─────────────────────────┘      └──────────────────────────────┘
```

Um handle válido em um processo **não funciona em outro processo** — a tabela de handles é por processo. O valor numérico 0x4 num processo pode apontar para um arquivo, enquanto o mesmo 0x4 em outro processo aponta para um mutex ou não existe.

### Propriedades de um handle

Cada entrada na tabela de handles tem:
- **Ponteiro para o objeto** (endereço kernel)
- **Access rights**: quais operações este handle pode executar (`GENERIC_READ`, `PROCESS_TERMINATE`, `FILE_WRITE_DATA`, etc.)
- **Flag de herança**: se filhos herdam este handle
- **Flag de proteção**: se o handle pode ser fechado acidentalmente com `CloseHandle`

### O custo de não fechar handles

```cpp
// BUG: handle leak
for (int i = 0; i < 100000; i++) {
    HANDLE h = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, 4);
    // Esqueceu de chamar CloseHandle(h)
    // O kernel mantém 100.000 referencias ao objeto EPROCESS de PID=4
}

// Correto:
for (int i = 0; i < 100000; i++) {
    HANDLE h = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, 4);
    if (h) {
        // ... usa h ...
        CloseHandle(h);  // decrementa o ref count
    }
}
```

Handle leaks não causam crash imediato, mas esgotam a tabela de handles do processo (que tem limite) e mantêm objetos vivos desnecessariamente. Em produção, use RAII:

```cpp
// RAII wrapper para HANDLE — fecha automaticamente ao sair do escopo
struct HandleRAII {
    HANDLE h;
    explicit HandleRAII(HANDLE h) : h(h) {}
    ~HandleRAII() { if (h && h != INVALID_HANDLE_VALUE) CloseHandle(h); }
    HandleRAII(const HandleRAII&) = delete;
    HandleRAII& operator=(const HandleRAII&) = delete;
    operator HANDLE() const { return h; }
};

// Uso:
HandleRAII proc(OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, targetPid));
if (!proc) { /* erro */ }
// ... usa proc como HANDLE ...
// CloseHandle chamado automaticamente ao sair do escopo
```

---

## O modelo de segurança do Windows

O Windows implementa **DAC (Discretionary Access Control)** — proprietários de recursos controlam quem pode acessá-los.

Cada objeto protegido tem um **Security Descriptor** contendo:
- **Owner SID**: quem possui o objeto
- **DACL** (Discretionary ACL): lista de ACEs (Access Control Entries) que definem quem pode fazer o quê
- **SACL** (System ACL): para auditoria — gera eventos no Event Log quando acessado

### SIDs — Security Identifiers

Toda entidade de segurança (usuário, grupo, computador, serviço) tem um SID único e imutável. Mesmo que um usuário seja renomeado, seu SID permanece o mesmo.

```
S-1-5-21-3623811015-3361044348-30300820-1013
│ │ │  └─────────────────────────────────────── RID (1013 = usuário específico)
│ │ └──────────────────────────────────────────── domínio/máquina (identificador único)
│ └────────────────────────────────────────────── authority (5 = NT Authority)
└──────────────────────────────────────────────── revision (sempre 1)
```

SIDs "bem conhecidos" (well-known SIDs):
```
S-1-1-0          Everyone
S-1-5-18         SYSTEM
S-1-5-19         LOCAL SERVICE
S-1-5-20         NETWORK SERVICE
S-1-5-32-544     Administrators
S-1-5-32-545     Users
```

```cpp
// Obtendo o SID do usuário atual
HANDLE hToken;
OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken);

DWORD dwSize = 0;
GetTokenInformation(hToken, TokenUser, nullptr, 0, &dwSize);
auto pTokenUser = (TOKEN_USER*)malloc(dwSize);
GetTokenInformation(hToken, TokenUser, pTokenUser, dwSize, &dwSize);

LPWSTR strSid;
ConvertSidToStringSidW(pTokenUser->User.Sid, &strSid);
wprintf(L"SID: %s\n", strSid);

LocalFree(strSid);
free(pTokenUser);
CloseHandle(hToken);
```

### Tokens de acesso

Quando um usuário faz login, o subsistema de segurança cria um **token de acesso** que representa sua identidade. O token é associado ao processo e contém:

- **SID do usuário**
- **SIDs dos grupos** (incluindo grupos de domínio)
- **Privilégios** (SeDebugPrivilege, SeBackupPrivilege, SeLoadDriverPrivilege, etc.)
- **Nível de integridade** (MIC)
- **Restrições** (para tokens restritos / sandboxes)

Todo acesso a objetos é verificado contra o token: o kernel compara os SIDs no token com as ACEs na DACL do objeto.

### Verificando permissões manualmente

```cpp
// Verificando se temos acesso a um arquivo antes de tentar abri-lo
SECURITY_INFORMATION secInfo = DACL_SECURITY_INFORMATION | OWNER_SECURITY_INFORMATION;
DWORD dwSize = 0;

// Obter o tamanho necessário
GetFileSecurity(L"C:\\Windows\\System32\\ntoskrnl.exe", secInfo, nullptr, 0, &dwSize);
auto pSD = (SECURITY_DESCRIPTOR*)malloc(dwSize);
GetFileSecurity(L"C:\\Windows\\System32\\ntoskrnl.exe", secInfo, pSD, dwSize, &dwSize);

HANDLE hToken;
OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY | TOKEN_DUPLICATE, &hToken);

// AccessCheck precisa de um token de impersonation
HANDLE hImpToken;
DuplicateToken(hToken, SecurityImpersonation, &hImpToken);

GENERIC_MAPPING mapping = {};
mapping.GenericRead = FILE_GENERIC_READ;
mapping.GenericWrite = FILE_GENERIC_WRITE;
mapping.GenericExecute = FILE_GENERIC_EXECUTE;
mapping.GenericAll = FILE_ALL_ACCESS;

DWORD desiredAccess = GENERIC_READ;
MapGenericMask(&desiredAccess, &mapping);

PRIVILEGE_SET privs = {};
DWORD privSize = sizeof(privs);
DWORD grantedAccess = 0;
BOOL accessGranted = FALSE;

AccessCheck(pSD, hImpToken, desiredAccess, &mapping,
            &privs, &privSize, &grantedAccess, &accessGranted);

wprintf(L"Acesso de leitura: %s\n", accessGranted ? L"PERMITIDO" : L"NEGADO");

CloseHandle(hImpToken);
CloseHandle(hToken);
free(pSD);
```

---

## MIC — Mandatory Integrity Control

Além do DAC, o Windows Vista introduziu o **MIC** — um segundo mecanismo de controle de acesso baseado em *níveis de integridade*:

| Nível | Nome | Quem tem |
|-------|------|---------|
| 0 | Untrusted | Processos sob restrição máxima |
| 4096 (0x1000) | Low | Internet Explorer em Protected Mode, downloads |
| 8192 (0x2000) | Medium | Usuários normais (padrão) |
| 12288 (0x3000) | High | Processos elevados (UAC) |
| 16384 (0x4000) | System | SYSTEM, serviços críticos |
| 20480 (0x5000) | Protected | Processos PPL (antivírus, LSA) |

A regra do MIC: **um processo de nível mais baixo não pode escrever em objetos de nível mais alto** — mesmo que a DACL permita. Um processo em Medium não pode modificar arquivos de SYSTEM. Um processo em Low (browser) não pode injetar código em um processo Medium.

Isso é por que browsers modernos rodam partes críticas em processos de integridade Low: mesmo que o renderer seja exploitado, o código malicioso fica preso nesse nível e não pode escalar para modificar o sistema.

---

## AppContainers — sandboxing de apps UWP

Apps da Microsoft Store (UWP) rodam em **AppContainers** — uma sandbox ainda mais restrita que MIC:

```
AppContainer cria:
  • Namespace de objetos privado (não pode ver objetos de outros processos)
  • Network isolation (acesso à rede controlado por capabilities)
  • Acesso ao sistema de arquivos restrito a pastas específicas
  • SID único do AppContainer (ex: S-1-15-2-...)
  • Nível de integridade Low
```

Mesmo com o nível de integridade Low e sem `SeDebugPrivilege`, um AppContainer é mais restrito: ele tem sua própria "bucket" de acesso que o separa completamente de outros AppContainers e de processos normais.

Você pode inspecionar se um processo está em AppContainer com `AccessChk` (Sysinternals) ou no Process Explorer: propriedades do processo → Security → "Is this a sandboxed process?"

---

## Registro do Windows

O **Registro** é o banco de dados centralizado de configurações do Windows. É estruturado em uma hierarquia de chaves (análogas a diretórios) e valores.

Raízes principais:

| Raiz | Abreviação | Conteúdo |
|------|-----------|---------|
| `HKEY_LOCAL_MACHINE` | HKLM | Configurações de toda a máquina |
| `HKEY_CURRENT_USER` | HKCU | Configurações do usuário atual |
| `HKEY_USERS` | HKU | Configurações de todos os usuários |
| `HKEY_CLASSES_ROOT` | HKCR | Tipos de arquivo, COM classes (merge de HKLM e HKCU) |
| `HKEY_CURRENT_CONFIG` | HKCC | Hardware atual (muda a cada boot) |

As chaves ficam armazenadas em arquivos chamados **hives** em `C:\Windows\System32\config\`:
- `SYSTEM` → HKLM\SYSTEM
- `SOFTWARE` → HKLM\SOFTWARE
- `SAM` → HKLM\SAM (banco de senhas locais, inacessível em runtime por ACL)
- `SECURITY` → HKLM\SECURITY

```cpp
// Lendo um valor do registro
HKEY hKey;
LONG res = RegOpenKeyExW(
    HKEY_LOCAL_MACHINE,
    L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",
    0, KEY_READ, &hKey
);

if (res == ERROR_SUCCESS) {
    wchar_t buildNumber[64];
    DWORD size = sizeof(buildNumber);
    DWORD type;
    
    RegQueryValueExW(hKey, L"CurrentBuildNumber",
                     nullptr, &type, (BYTE*)buildNumber, &size);
    wprintf(L"Build: %s\n", buildNumber);
    RegCloseKey(hKey);
}
```

O registro é protegido pelas mesmas ACLs que arquivos — `HKLM\SAM` requer privilégio SYSTEM para ser lido, por exemplo. Malware frequentemente tenta persistência escrevendo em `HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\Run` ou equivalentes.

---

## Unicode e strings no Windows

O Windows NT foi projetado desde o início com suporte completo a Unicode. Internamente, **todas as strings são UTF-16LE** (caracteres de 16 bits, little-endian).

Por que UTF-16 e não UTF-8? Em 1993, quando o NT foi projetado, a expectativa era que todos os caracteres do mundo coubessem em 16 bits (o que é verdade para o BMP — Basic Multilingual Plane, suficiente para praticamente todos os idiomas em uso). UTF-8 era uma ideia relativamente nova e não amplamente adotada.

```cpp
// No Windows, L"..." cria strings UTF-16LE
const wchar_t* caminho = L"C:\\Usuários\\João\\Documentos";
// wchar_t tem 2 bytes no Windows (diferente de Linux onde tem 4 bytes)

// Convertendo UTF-8 para UTF-16
std::string utf8 = "Olá Mundo";
int tamanho = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, nullptr, 0);
std::wstring utf16(tamanho, 0);
MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, &utf16[0], tamanho);

// Convertendo UTF-16 para UTF-8
int tamanhoUtf8 = WideCharToMultiByte(CP_UTF8, 0, utf16.c_str(), -1, nullptr, 0, nullptr, nullptr);
std::string resultado(tamanhoUtf8, 0);
WideCharToMultiByte(CP_UTF8, 0, utf16.c_str(), -1, &resultado[0], tamanhoUtf8, nullptr, nullptr);
```

### Tipos de strings no mundo Windows

| Tipo | Codificação | Onde aparece |
|------|------------|-------------|
| `LPSTR` / `LPCSTR` | ANSI/MBCS | APIs legadas com sufixo A |
| `LPWSTR` / `LPCWSTR` | UTF-16LE | APIs Win32 com sufixo W (use sempre) |
| `BSTR` | UTF-16LE + tamanho prefixado | COM/OLE |
| `HSTRING` | UTF-16LE imutável | WinRT |
| `std::string` | Depende (geralmente UTF-8) | C++ padrão |
| `std::wstring` | UTF-16LE no Windows | C++ no Windows |

> **Regra prática**: em código Win32 nativo, use `std::wstring` e APIs com sufixo W. Converta para/de UTF-8 apenas nas fronteiras com sistemas externos (rede, arquivos de configuração) usando `MultiByteToWideChar`/`WideCharToMultiByte` com `CP_UTF8`.

---

*Próximo: [Ferramentas Essenciais](capitulo-01-08-ferramentas.md)*
