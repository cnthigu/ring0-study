# Capítulo 3 — Criação de Processos

> Livro didático baseado em *Windows Internals, 7th Edition*.
> Atualizado para Windows 11 com exemplos práticos em C++ moderno.

> Navegação: [Índice Cap.3](capitulo-03.md) | [Próximo](capitulo-03-02-internos-processo.md)

---

## A família CreateProcess

O Windows oferece várias funções para criar processos, cada uma para um cenário diferente:

```
CreateProcess          → mesmo token do processo criador
CreateProcessAsUser    → token fornecido pelo caller (requer SeAssignPrimaryTokenPrivilege)
CreateProcessWithTokenW → token fornecido, requisitos de privilégio diferentes
CreateProcessWithLogonW → credentials de usuário → loga e cria processo em um passo
ShellExecute(Ex)       → qualquer arquivo (não só executáveis), usa extensões de arquivo
```

Internamente, todas convergem para `CreateProcessInternal` em kernel32.dll, que chama `NtCreateUserProcess` em ntdll.dll para fazer a transição kernel-mode:

```
ShellExecuteEx(L"C:\\doc.txt")
  → descobre que .txt abre com notepad.exe via HKCR
  → CreateProcessW(L"notepad.exe C:\\doc.txt")
    → CreateProcessInternal
      → NtCreateUserProcess (ntdll.dll — stub de syscall)
        → NtCreateUserProcess (ntoskrnl.exe — implementação)
          → PspAllocateProcess
          → PspInsertProcess
```

### CreateProcessWithLogonW e o serviço SecLogon

`CreateProcessWithLogonW` não cria o processo diretamente — faz uma chamada RPC para o **serviço SecLogon** (seclogon.dll em svchost.exe), que tem os privilégios necessários para logar o usuário e criar o processo com o novo token. Isso é o que `runas.exe` usa internamente.

```cpp
// runas /user:dominio\usuario programa.exe
// internamente faz:
CreateProcessWithLogonW(
    L"usuario",           // usuário
    L"dominio",           // domínio
    L"senha",             // senha
    LOGON_WITH_PROFILE,   // carregar profile do usuário
    nullptr,
    L"C:\\programa.exe",
    CREATE_NEW_CONSOLE,
    nullptr, nullptr,
    &si, &pi
);
```

---

## Argumentos importantes de CreateProcess

```cpp
#include <windows.h>
#include <wchar.h>

int main() {
    STARTUPINFOEXW siex = {};
    siex.StartupInfo.cb = sizeof(siex);
    
    // Atributos de processo/thread (STARTUPINFOEX)
    SIZE_T attrSize = 0;
    InitializeProcThreadAttributeList(nullptr, 1, 0, &attrSize);
    auto attrList = (LPPROC_THREAD_ATTRIBUTE_LIST)malloc(attrSize);
    InitializeProcThreadAttributeList(attrList, 1, 0, &attrSize);
    siex.lpAttributeList = attrList;
    
    PROCESS_INFORMATION pi = {};
    
    BOOL ok = CreateProcessW(
        L"C:\\Windows\\System32\\cmd.exe",  // lpApplicationName
        nullptr,                             // lpCommandLine
        nullptr,                             // lpProcessAttributes
        nullptr,                             // lpThreadAttributes
        FALSE,                               // bInheritHandles
        CREATE_NEW_CONSOLE |
        EXTENDED_STARTUPINFO_PRESENT,        // dwCreationFlags
        nullptr,                             // lpEnvironment
        nullptr,                             // lpCurrentDirectory
        (LPSTARTUPINFOW)&siex,               // lpStartupInfo
        &pi                                  // lpProcessInformation
    );
    
    if (ok) {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
    
    DeleteProcThreadAttributeList(attrList);
    free(attrList);
    return 0;
}
```

### Flags de criação mais importantes

| Flag | Efeito |
|------|--------|
| `CREATE_SUSPENDED` | Thread principal criada suspensa; use `ResumeThread` para iniciar |
| `CREATE_NEW_CONSOLE` | Nova janela de console |
| `DEBUG_PROCESS` | Caller vira debugger do novo processo |
| `CREATE_NO_WINDOW` | Sem janela (para processos console em background) |
| `DETACHED_PROCESS` | Sem console associado |
| `INHERIT_PARENT_AFFINITY` | Herda afinidade de CPU do pai |
| `EXTENDED_STARTUPINFO_PRESENT` | `lpStartupInfo` aponta para `STARTUPINFOEXW` |

### STARTUPINFO e herança de handles

A flag `bInheritHandles = TRUE` faz o kernel copiar para o filho todos os handles do pai marcados como **heritable**. Isso é o mecanismo padrão para passar stdin/stdout para processos filhos:

```cpp
// Criando pipe para stdin/stdout do filho
HANDLE hReadStdin, hWriteStdin;
HANDLE hReadStdout, hWriteStdout;

SECURITY_ATTRIBUTES sa = { sizeof(sa), nullptr, TRUE }; // handles heritable
CreatePipe(&hReadStdin, &hWriteStdin, &sa, 0);
CreatePipe(&hReadStdout, &hWriteStdout, &sa, 0);

STARTUPINFOW si = { sizeof(si) };
si.dwFlags = STARTF_USESTDHANDLES;
si.hStdInput  = hReadStdin;
si.hStdOutput = hWriteStdout;
si.hStdError  = hWriteStdout;

PROCESS_INFORMATION pi;
CreateProcessW(L"C:\\prog.exe", nullptr, nullptr, nullptr,
               TRUE, 0, nullptr, nullptr, &si, &pi);
// Agora você pode ler stdout do filho via hReadStdout
// e escrever no stdin via hWriteStdin
```

---

## Processos modernos (UWP/Store Apps)

Aplicações da Microsoft Store exigem um atributo adicional via `UpdateProcThreadAttribute`:

```cpp
// Lançar um app UWP via API COM
#include <shobjidl.h>
#include <appmodel.h>

IApplicationActivationManager* pActivationManager;
CoCreateInstance(CLSID_ApplicationActivationManager, nullptr, 
                 CLSCTX_LOCAL_SERVER,
                 IID_PPV_ARGS(&pActivationManager));

DWORD pid;
pActivationManager->ActivateApplication(
    L"Microsoft.WindowsCalculator_8wekyb3d8bbwe!App",  // AppUserModelId
    nullptr,   // argumentos
    AO_NONE,
    &pid
);
pActivationManager->Release();
```

Apps UWP têm restrições adicionais impostas pelo OS:
- Rodam em AppContainer (sandbox)
- Só podem usar APIs declaradas no manifesto do pacote
- Gerenciadas pelo PLM (Process Lifetime Manager) — suspensas quando em background

---

## Tipos especiais de processos

### Native processes (imagens nativas)

Executáveis que linkam apenas contra ntdll.dll, sem subsistema Windows. Exemplos: `smss.exe`, `autochk.exe`. Criados via `NtCreateUserProcess` diretamente.

O kernel rejeita tentativas de criar native images via a API Win32 — `CreateProcessInternal` verifica o tipo de subsistema no header PE e bloqueia.

### Minimal processes

Processos sem uma imagem PE de user mode — são "containers vazios" no kernel com espaço de endereços mas sem código de aplicação. Usados pelo:
- System process (PID 4)
- Memory Compression process
- Secure System process (VTL 1)

Criados via `NtCreateProcessEx` com flags especiais, acessíveis apenas para callers kernel-mode.

### Pico processes

Processos criados e gerenciados por um Pico Provider (driver). O WSL usa isso para hospedar binários ELF Linux nativamente. O kernel repassa syscalls, exceptions e page faults ao provider registrado.

```c
// Em um Pico Provider (driver):
PS_PICO_PROVIDER_ROUTINES rotinas = { ... };
PsRegisterPicoProvider(&rotinas, &callbacks);

// Depois, para criar um processo Pico:
PspCreatePicoProcess(...);  // função não exportada, acessível via interface do provider
```

---

## A saída de CreateProcess: PROCESS_INFORMATION

```cpp
PROCESS_INFORMATION pi;
// Após CreateProcess bem-sucedido:
pi.hProcess    // handle para o novo processo
pi.hThread     // handle para a thread principal
pi.dwProcessId // PID
pi.dwThreadId  // TID da thread principal
```

**Sempre feche os handles.** `CreateProcess` abre handles com contagem de referência incrementada — o objeto processo só é destruído quando não há mais handles E o processo terminou.

```cpp
// Pattern correto:
if (CreateProcessW(..., &pi)) {
    // Use pi.hProcess e pi.hThread se necessário (WaitForSingleObject, etc.)
    WaitForSingleObject(pi.hProcess, INFINITE);
    
    DWORD exitCode;
    GetExitCodeProcess(pi.hProcess, &exitCode);
    
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
}
```

---

*Próximo: [Internos do Processo](capitulo-03-02-internos-processo.md)*
