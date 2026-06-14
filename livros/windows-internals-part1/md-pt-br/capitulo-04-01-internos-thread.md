# Capítulo 4 — Internos de Thread: ETHREAD, KTHREAD e TEB

> Livro didático baseado em *Windows Internals, 7th Edition*.
> Atualizado para Windows 11 com exemplos práticos em C++ moderno.

> Navegação: [Índice Cap.4](capitulo-04.md) | [Próximo](capitulo-04-02-scheduling.md)

---

## A estrutura ETHREAD

Assim como processos têm EPROCESS, threads têm **ETHREAD (Executive Thread)**:

```
ETHREAD (Executive Thread)
  ├── KTHREAD (Kernel Thread)  ← scheduling e contexto de CPU
  │     ├── DISPATCHER_HEADER  ← permite WaitForSingleObject em thread
  │     ├── KernelStack        ← base da pilha de kernel mode
  │     ├── InitialStack       ← topo da pilha de kernel mode
  │     ├── State              ← Ready, Running, Waiting, etc.
  │     ├── Priority           ← prioridade atual
  │     ├── BasePriority       ← prioridade base
  │     ├── Quantum            ← fatia de tempo restante
  │     ├── WaitStatus         ← resultado do wait
  │     └── ApcState           ← APCs pendentes
  ├── Cid (CLIENT_ID)          ← PID + TID
  ├── ThreadsProcess           ← ponteiro para EPROCESS pai
  ├── Win32StartAddress        ← função passada para CreateThread
  ├── StartAddress             ← ponto de entrada real (RtlUserThreadStart)
  ├── IrpList                  ← IRPs pendentes desta thread
  └── Win32Thread (TEB*)       ← TEB em user mode
```

```
kd> dt nt!_ETHREAD <endereço>    ← ver campos do ETHREAD
kd> dt nt!_KTHREAD <endereço>    ← ver campos do KTHREAD (via Tcb)
kd> !thread <endereço>           ← visão amigável de uma thread
kd> !process <pid> 7             ← processo com todas as threads detalhadas
```

---

## Estados de uma thread

O KTHREAD mantém o estado atual de cada thread via o campo `State`:

```
Estados possíveis:
  Initialized  → thread recém-criada, ainda não está no sistema de scheduling
  Ready        → aguardando uma CPU para executar (na fila de prontos)
  Running      → executando em uma CPU agora
  Standby      → selecionada para ser a próxima a rodar numa CPU específica
  Terminated   → finalizou execução, aguardando liberação de recursos
  Waiting      → bloqueada aguardando um objeto (evento, mutex, timer, I/O)
  Transition   → prontapara rodar mas stack de kernel mode foi paginada
  DeferredReady → pronta mas ainda não adicionada à fila de prontos (NUMA)
  GateWaitObsolete → (obsoleto, não usado atualmente)
```

A transição mais comum:
```
CreateThread → Initialized → Ready → Running ↔ Waiting
                                         ↓
                                      Terminated
```

---

## TEB: Thread Environment Block

Cada thread tem seu próprio TEB (em user mode address space), apontado pelo registrador `GS` em x64 (`FS` em x86):

```cpp
// GS:[0x00] = ponteiro para o próprio TEB
// GS:[0x08] = StackBase (topo da pilha — endereço mais alto)
// GS:[0x10] = StackLimit (base da pilha — endereço mais baixo)
// GS:[0x30] = ponteiro para o PEB
// GS:[0x48] = TID (Thread ID)
// GS:[0x1478] = slots de TLS estático (__declspec(thread))

// Acessar o TEB diretamente:
NtCurrentTeb()  // retorna ponteiro para o TEB da thread atual

// Exemplo de uso sem syscall:
DWORD GetCurrentThreadId_fast() {
    return NtCurrentTeb()->ClientId.UniqueThread;
    // NENHUMA syscall — leitura direta da memória user-mode
}
```

Campos importantes do TEB:

| Offset (x64) | Campo | Uso |
|-------------|-------|-----|
| `+0x000` | `NtTib.StackBase` | Limite superior da pilha (endereço mais alto) |
| `+0x008` | `NtTib.StackLimit` | Limite inferior da pilha (endereço mais baixo) |
| `+0x030` | `ProcessEnvironmentBlock` | Ponteiro para o PEB do processo |
| `+0x034` | `LastErrorValue` | `GetLastError()` lê daqui |
| `+0x038` | `CountOfOwnedCriticalSections` | Para detecção de deadlock |
| `+0x048` | `RealClientId.UniqueThread` | TID |
| `+0x1478` | `TlsSlots[64]` | TLS estático |
| `+0x1480` | `TlsExpansionSlots` | TLS dinâmico (via TlsAlloc) |

---

## Criando threads

```cpp
#include <windows.h>
#include <stdio.h>

DWORD WINAPI WorkerThread(LPVOID param) {
    auto numero = (int)(intptr_t)param;
    printf("Thread %d: TID=%u\n", numero, GetCurrentThreadId());
    return numero * 2;  // exit code
}

int main() {
    HANDLE threads[4];
    
    for (int i = 0; i < 4; i++) {
        DWORD tid;
        threads[i] = CreateThread(
            nullptr,              // security attrs
            0,                    // stack size (0 = padrão 1MB ou conforme imagem PE)
            WorkerThread,         // entry point
            (LPVOID)(intptr_t)i, // parâmetro
            0,                    // flags (0 = começa rodando; CREATE_SUSPENDED = suspensa)
            &tid                  // TID de saída
        );
    }
    
    WaitForMultipleObjects(4, threads, TRUE, INFINITE);
    
    for (int i = 0; i < 4; i++) {
        DWORD exitCode;
        GetExitCodeThread(threads[i], &exitCode);
        printf("Thread %d retornou: %u\n", i, exitCode);
        CloseHandle(threads[i]);
    }
}
```

### CreateRemoteThread: injeção de thread

`CreateRemoteThread` cria uma thread em **outro processo**:

```cpp
// Injeção de DLL via CreateRemoteThread (técnica clássica)
HANDLE hTarget = OpenProcess(PROCESS_ALL_ACCESS, FALSE, targetPid);

// Aloca memória no processo alvo para o nome da DLL
SIZE_T dllPathLen = (wcslen(dllPath) + 1) * sizeof(wchar_t);
LPVOID remoteMem = VirtualAllocEx(hTarget, nullptr, dllPathLen,
                                   MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
WriteProcessMemory(hTarget, remoteMem, dllPath, dllPathLen, nullptr);

// Cria thread no processo alvo que chama LoadLibraryW com nosso path
HANDLE hThread = CreateRemoteThread(
    hTarget,
    nullptr,
    0,
    (LPTHREAD_START_ROUTINE)GetProcAddress(
        GetModuleHandleW(L"kernel32.dll"), "LoadLibraryW"),
    remoteMem,
    0, nullptr
);

WaitForSingleObject(hThread, INFINITE);
VirtualFreeEx(hTarget, remoteMem, 0, MEM_RELEASE);
CloseHandle(hThread);
CloseHandle(hTarget);
```

> Esta técnica requer `PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE` no handle alvo. Em processos PPL, isso é bloqueado. Antivírus monitoram `CreateRemoteThread` como indicador de injeção.

---

## Thread Stack: guard pages e crescimento automático

A pilha padrão de uma thread tem 1 MB. O Windows usa **guard pages** para detectar estouro de pilha antes que ele cause dano:

```
Pilha em memória (x64, cresce para baixo):
  0x0000 ... (endereço baixo)
  ┌─────────────┐ ← StackLimit (limite atual de acesso)
  │ Guard page  │ ← PAGE_GUARD — acesso aqui gera exceção STATUS_GUARD_PAGE_VIOLATION
  ├─────────────┤   → kernel trata a exceção, aumenta StackLimit, move guard page
  │   Pilha     │ ← área acessível
  │   em uso    │
  │             │
  ├─────────────┤ ← RSP (Stack Pointer atual)
  │  Disponível │
  │  (não usado)│
  ├─────────────┤ ← StackBase (endereço mais alto — topo inicial)
  │  Não commit │ ← MEM_RESERVE mas não MEM_COMMIT (não usa RAM ainda)
  └─────────────┘
  0xffff ... (endereço alto)
```

Quando o ESP/RSP toca a guard page, o kernel automaticamente commit mais memória e move a guard page para baixo — até o limite máximo (1 MB por padrão). Se exceder: `EXCEPTION_STACK_OVERFLOW`.

```cpp
// Alterar o tamanho da pilha de uma thread:
CreateThread(nullptr,
             4 * 1024 * 1024,  // 4 MB de pilha
             WorkerThread, nullptr, STACK_SIZE_PARAM_IS_A_RESERVATION, nullptr);
```

---

## Contexto de Thread para Debugging

```
kd> .thread <endereço ETHREAD>    ← trocar contexto para esta thread
kd> k                              ← stack da thread (user + kernel)
kd> kb                             ← stack com parâmetros de cada frame
kd> r                              ← registradores atuais desta thread

# Para ver o stack de todas as threads de um processo:
kd> !process <pid> 7   ← inclui stack de cada thread
```

No Process Explorer, clique com direito em qualquer thread → Stack para ver o call stack completo (user mode + kernel mode se símbolos estiverem configurados).

---

*Próximo: [Scheduling de Threads](capitulo-04-02-scheduling.md)*
