# Capítulo 3 — Internos do Processo: EPROCESS, KPROCESS e PEB

> Livro didático baseado em *Windows Internals, 7th Edition*.
> Atualizado para Windows 11 com exemplos práticos em C++ moderno.

> Navegação: [Anterior](capitulo-03-01-criacao-processo.md) | [Índice Cap.3](capitulo-03.md) | [Próximo](capitulo-03-03-protected-processes.md)

---

## A hierarquia de estruturas de um processo

Todo processo Windows vivo no kernel é representado por uma coleção de estruturas aninhadas:

```
EPROCESS (Executive Process)          ← o objeto processo no kernel
  ├── KPROCESS (Kernel Process)        ← scheduling e contabilidade
  │     └── DISPATCHER_HEADER          ← permite wait em objetos de processo
  ├── PEB (Process Environment Block)  ← em user mode address space
  ├── ObjectTable (handle table)       ← handles abertos pelo processo
  ├── Token                            ← identidade de segurança
  ├── VadRoot (VAD tree)               ← mapa de memória virtual
  ├── Job pointer                      ← se associado a um Job
  └── ... (mais de 100 campos)
```

Estruturas paralelas mantidas por outros subsistemas:
- **CSR_PROCESS** em csrss.exe (subsistema Windows)
- **W32PROCESS** em win32k.sys (criado quando user32.dll é carregada)
- **DXGPROCESS** em dxgkrnl.sys (DirectX, quando apps gráficas iniciam)

---

## EPROCESS: o objeto processo no Executive

```
kd> dt nt!_EPROCESS ffffe001`1c324300
   +0x000 Pcb                  : _KPROCESS        ← scheduling
   +0x2d8 ProcessLock          : _EX_PUSH_LOCK    ← lock do objeto
   +0x2e8 UniqueProcessId      : 0x0e38 (PID)
   +0x2f0 ActiveProcessLinks   : _LIST_ENTRY       ← lista ligada de todos os processos
   +0x340 Flags                : flags de estado
   +0x418 ObjectTable          : _HANDLE_TABLE*    ← tabela de handles
   +0x420 DebugPort            : ponteiro para debug port
   +0x428 WoW64Process         : _EWOW64PROCESS*  ← se processo 32-bit em 64-bit
   +0x3a8 Win32Process         : W32PROCESS*      ← para apps com janela
   +0x3b0 Job                  : _EJOB*           ← job object se associado
   +0x4b8 Token                : Token de segurança
   +0x520 WorkingSetPage       : working set
```

Campos mais úteis para debug:
- `ActiveProcessLinks`: lista duplamente ligada de todos os EPROCESS do sistema
- `UniqueProcessId`: o PID (mesmo namespace de TIDs)
- `Token`: ponteiro para o token de segurança (ExFastRef — ponteiro + ref count comprimido)
- `ImageFileName`: nome do executável (15 chars, truncado)
- `SeAuditProcessCreationInfo`: caminho completo do executável

```
kd> !process 0 0           ← lista todos os processos (campos básicos)
kd> !process 0xe38 7       ← detalha processo por PID, com threads
kd> dt nt!_EPROCESS <addr> ← dump de todos os campos
```

---

## KPROCESS: a visão do scheduler

O primeiro campo de EPROCESS é um `KPROCESS` embarcado (o EPROCESS "é um" KPROCESS do ponto de vista do kernel):

```
kd> dt nt!_KPROCESS
   +0x000 Header               : _DISPATCHER_HEADER  ← permite WaitForSingleObject
   +0x018 ProfileListHead      : _LIST_ENTRY
   +0x028 DirectoryTableBase   : UINT8B   ← endereço físico do PML4 (page table raiz)
   +0x030 ThreadListHead       : _LIST_ENTRY ← lista de ETHREADs deste processo
   +0x26c KernelTime           : tempo em kernel mode
   +0x270 UserTime             : tempo em user mode
   +0x2c8 InstrumentationCallback : callback para instrumentação
```

**`DirectoryTableBase`** é o campo mais fundamental: é o endereço físico do CR3 do processo — a página raiz das tabelas de página. Quando o scheduler troca de processo, ele carrega este valor no registrador CR3, mudando assim todo o mapeamento de memória virtual.

```
kd> dt nt!_KPROCESS <addr> DirectoryTableBase
→ 0x00000000`38b3e000   ← endereço físico do PML4 deste processo
```

---

## PEB: Process Environment Block

O PEB é a única estrutura principal do processo que existe em **user mode address space** (não no kernel). Isso é necessário porque várias funções de user mode precisam acessar informações do processo sem fazer syscalls.

Endereço: apontado por `TEB.ProcessEnvironmentBlock` (acessível via `gs:[0x60]` em x64).

```
kd> !peb
PEB at 00000000`5f2f1de000
  InheritedAddressSpace:    No
  ReadImageFileExecOptions: No
  BeingDebugged:            No
  ImageBaseAddress:         00007ff7`4a230000    ← onde o EXE está mapeado
  Ldr:                      00007ff9`d0e63580    ← loader data
  SubSystemData:            0000000000000000
  ProcessHeap:              000001ac`50ea0000    ← heap padrão do processo
  NtGlobalFlag:             00000000
  NumberOfProcessors:       12
  OSMajorVersion:           10
  OSMinorVersion:           0
  OSBuildNumber:            22621               ← build do Windows
  ...
  ImageFileName:            notepad.exe
  CommandLine:              "notepad.exe"
  Environment:              00000239`f3a23a90   ← bloco de variáveis de ambiente
```

### Campos importantes do PEB

| Campo | Uso |
|-------|-----|
| `ImageBaseAddress` | Base do executável (.exe) em memória |
| `Ldr` | `PEB_LDR_DATA` — lista de DLLs carregadas |
| `ProcessHeap` | Handle do heap padrão (`GetProcessHeap()` retorna isso) |
| `BeingDebugged` | Flag — `IsDebuggerPresent()` lê este campo |
| `NtGlobalFlag` | Flags de diagnóstico (habilitado por `gflags.exe`) |
| `ProcessParameters` | Linha de comando, diretório atual, stdout/stdin handles |
| `TlsBitmap` | TLS slot allocation bitmap |
| `NumberOfProcessors` | Número de CPUs (usado por `GetSystemInfo`) |
| `OSBuildNumber` | Build do Windows — usado para detecção de versão |

### Enumerando DLLs via PEB.Ldr

```cpp
#include <windows.h>
#include <winternl.h>

void listarDLLsViaPEB() {
    // Obter PEB via TEB
    PEB* peb = NtCurrentTeb()->ProcessEnvironmentBlock;
    PEB_LDR_DATA* ldr = peb->Ldr;
    
    // InMemoryOrderModuleList: lista de LDR_DATA_TABLE_ENTRY
    LIST_ENTRY* head = &ldr->InMemoryOrderModuleList;
    LIST_ENTRY* entry = head->Flink;
    
    while (entry != head) {
        LDR_DATA_TABLE_ENTRY* mod = CONTAINING_RECORD(
            entry, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);
        
        wprintf(L"Base: %p  %wZ\n",
            mod->DllBase,
            &mod->FullDllName);
        
        entry = entry->Flink;
    }
}
```

Isso é o mesmo que o `EnumProcessModules` da Win32, mas sem passar pelo kernel. Antivírus, injetores de DLL e ferramentas de anti-cheat frequentemente inspecionam o PEB.Ldr diretamente.

> **Técnica de ocultação de DLL ("dll unlinking")**: malware pode remover uma entrada da lista InMemoryOrderModuleList para esconder uma DLL da inspeção via PEB. Ferramentas de análise mais sofisticadas usam as estruturas do kernel (EPROCESS.Ldr via kernel debug ou VAD tree) em vez do PEB para detectar isso.

---

## TEB: Thread Environment Block

Cada thread tem seu próprio TEB (apontado por `gs:[0]` em x64, `fs:[0]` em x86):

```cpp
// Campos importantes do TEB
NT_TIB NtTib;              // Stack base/limit, TEB self pointer
PVOID EnvironmentPointer;
CLIENT_ID ClientId;        // PID e TID desta thread
PVOID ActiveRpcHandle;
PVOID ThreadLocalStoragePointer;  // TLS dinâmico
PEB* ProcessEnvironmentBlock;     // ponteiro para o PEB do processo
ULONG LastErrorValue;      // GetLastError() lê daqui — sem syscall!
ULONG CountOfOwnedCriticalSections;
// ... mais campos ...
BYTE TlsSlots[64*8];       // TLS estático (__declspec(thread))
```

`GetLastError()` é outra função que não faz syscall — lê `TEB.LastErrorValue`. O kernel define este valor após cada chamada de sistema que falha.

---

## Criando e inspecionando processos com código

```cpp
#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <wchar.h>

void inspecionarProcesso(DWORD pid) {
    HANDLE hProc = OpenProcess(
        PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
        FALSE, pid
    );
    if (!hProc) return;
    
    // Nome do executável
    wchar_t exePath[MAX_PATH];
    DWORD size = MAX_PATH;
    QueryFullProcessImageNameW(hProc, 0, exePath, &size);
    wprintf(L"Executável: %s\n", exePath);
    
    // Uso de memória
    PROCESS_MEMORY_COUNTERS_EX pmc = { sizeof(pmc) };
    GetProcessMemoryInfo(hProc, (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
    wprintf(L"Working Set: %zu KB\n", pmc.WorkingSetSize / 1024);
    wprintf(L"Private Bytes: %zu KB\n", pmc.PrivateUsage / 1024);
    
    // DLLs carregadas
    HMODULE mods[1024];
    DWORD needed;
    if (EnumProcessModulesEx(hProc, mods, sizeof(mods), &needed, LIST_MODULES_ALL)) {
        for (DWORD i = 0; i < needed / sizeof(HMODULE); i++) {
            wchar_t modName[MAX_PATH];
            GetModuleFileNameExW(hProc, mods[i], modName, MAX_PATH);
            wprintf(L"  DLL: %s\n", modName);
        }
    }
    
    CloseHandle(hProc);
}
```

---

*Próximo: [Processos Protegidos e PPL](capitulo-03-03-protected-processes.md)*
