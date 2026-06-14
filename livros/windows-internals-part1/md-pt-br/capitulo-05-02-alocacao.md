# Capítulo 5 — Alocação de Memória: VirtualAlloc, Heaps e Large Pages

> Livro didático baseado em *Windows Internals, 7th Edition*.
> Atualizado para Windows 11 com exemplos práticos em C++ moderno.

> Navegação: [Anterior](capitulo-05-01-virtual-memory.md) | [Índice Cap.5](capitulo-05.md) | [Próximo](capitulo-05-03-shared-memory.md)

---

## VirtualAlloc: a API de baixo nível

`VirtualAlloc` é a API mais próxima do Memory Manager. Tudo mais (`malloc`, `new`, `HeapAlloc`) é construído sobre ela.

```cpp
#include <windows.h>

// Padrão: Reserve + Commit em um passo
void* p = VirtualAlloc(
    nullptr,          // endereço preferido (nullptr = deixe o sistema escolher)
    64 * 1024,        // tamanho (arredondado para cima para múltiplo de 64 KB)
    MEM_COMMIT | MEM_RESERVE,  // estado
    PAGE_READWRITE    // proteção
);
if (!p) {
    // GetLastError() para diagnóstico
    // ERROR_COMMITMENT_LIMIT → commit charge esgotado (sem RAM e sem swap)
}

// Usar a memória:
memset(p, 0, 64 * 1024);

// Liberar:
VirtualFree(p, 0, MEM_RELEASE);  // libera tudo (reserve + commit)
```

### Granularidade importante

```
Granularidade de reserva: 64 KB (SYSTEM_INFO.dwAllocationGranularity)
  → VirtualAlloc com endereço específico é arredondado para baixo ao múltiplo de 64 KB
  → Por isso, reservar 1 byte na prática reserva 64 KB

Granularidade de commit: 4 KB (tamanho de página)
  → Commit e decommit operam em múltiplos de 4 KB
```

```cpp
// Padrão Reserve + Commit gradual (útil para buffers que crescem):
const size_t MAX_SIZE = 1ULL * 1024 * 1024 * 1024; // 1 GB reservado
const size_t PAGE = 4096;

void* base = VirtualAlloc(nullptr, MAX_SIZE, MEM_RESERVE, PAGE_NOACCESS);
size_t comitted = 0;

// Commitar apenas o que precisamos agora:
auto GrowBy = [&](size_t bytes) -> bool {
    size_t newSize = comitted + bytes;
    newSize = (newSize + PAGE - 1) & ~(PAGE - 1); // arredondar para cima
    if (newSize > MAX_SIZE) return false;
    void* result = VirtualAlloc(
        (char*)base + comitted, newSize - comitted,
        MEM_COMMIT, PAGE_READWRITE);
    if (!result) return false;
    comitted = newSize;
    return true;
};

GrowBy(64 * 1024); // commitar 64 KB inicialmente
// ... usar base[0..comitted-1] ...
GrowBy(1024 * 1024); // crescer mais 1 MB

VirtualFree(base, 0, MEM_RELEASE);
```

---

## Consultando o estado da memória: VirtualQuery

```cpp
MEMORY_BASIC_INFORMATION mbi;
void* addr = (void*)0x00401000;

VirtualQuery(addr, &mbi, sizeof(mbi));

// mbi.BaseAddress = início da região
// mbi.RegionSize  = tamanho total desta região com as mesmas características
// mbi.State       = MEM_FREE, MEM_RESERVE, MEM_COMMIT
// mbi.Protect     = PAGE_READWRITE, PAGE_EXECUTE_READ, etc.
// mbi.Type        = MEM_PRIVATE, MEM_MAPPED, MEM_IMAGE

// Enumerar TODO o espaço de endereços de um processo:
void EnumerateVAS(HANDLE hProcess) {
    char* addr = nullptr;
    MEMORY_BASIC_INFORMATION mbi;
    while (VirtualQueryEx(hProcess, addr, &mbi, sizeof(mbi)) == sizeof(mbi)) {
        wprintf(L"[%p - %p] State=%s Type=%s Prot=%x Size=%zu KB\n",
            mbi.BaseAddress,
            (char*)mbi.BaseAddress + mbi.RegionSize,
            mbi.State == MEM_FREE ? L"Free" :
            mbi.State == MEM_RESERVE ? L"Reserved" : L"Committed",
            mbi.Type == MEM_IMAGE ? L"Image" :
            mbi.Type == MEM_MAPPED ? L"Mapped" : L"Private",
            mbi.Protect,
            mbi.RegionSize / 1024);
        addr += mbi.RegionSize;
        if (addr == nullptr) break; // overflow → fim do espaço
    }
}
```

---

## Heap Manager: malloc em user mode

O Heap Manager do Windows implementa `HeapAlloc`/`HeapFree` (equivalentes a `malloc`/`free`) sobre `VirtualAlloc`. Cada processo tem pelo menos um heap padrão — acessível via `GetProcessHeap()`.

```cpp
HANDLE hHeap = GetProcessHeap();  // heap padrão do processo

// Alocar:
void* p = HeapAlloc(hHeap, 0, 256);        // 0 = sem flags especiais
void* pZero = HeapAlloc(hHeap, HEAP_ZERO_MEMORY, 256);  // zerado

// Realocar:
p = HeapReAlloc(hHeap, 0, p, 512);

// Tamanho real:
SIZE_T real = HeapSize(hHeap, 0, p);  // pode ser maior que o solicitado

// Liberar:
HeapFree(hHeap, 0, p);

// Criar heap privado (isolado, com lock próprio):
HANDLE hPrivate = HeapCreate(0, 0, 0);
void* q = HeapAlloc(hPrivate, 0, 100);
HeapFree(hPrivate, 0, q);
HeapDestroy(hPrivate);  // libera tudo de uma vez
```

### Segment Heap (Windows 10 2004+)

O Windows 10 introduziu o **Segment Heap**, ativado por padrão em processos UWP e opcionalmente em processos Win32:

```
Segment Heap vs. NT Heap:
  NT Heap:      algoritmo clássico, FreeList[128], chunks coalescidos
  Segment Heap: componentes especializados por tamanho
    ├── Low Fragmentation Heap (LFH): tamanhos ≤ 512 bytes
    ├── Variable Size (VS): tamanhos 512 B a 128 KB
    ├── Backend Heap: tamanhos maiores, direto de VirtualAlloc
    └── Large Page Alloc: > 512 KB

Vantagens do Segment Heap:
  → Menor fragmentação
  → Melhor desempenho em alocações pequenas (LFH por padrão desde o início)
  → Isolamento de heaps por componente
```

Para habilitar em processo Win32:
```xml
<!-- no manifest do executável: -->
<heapType xmlns="http://schemas.microsoft.com/SMI/2016/WindowsSettings">SegmentHeap</heapType>
```

---

## Large Pages: 2 MB em vez de 4 KB

Para workloads que acessam grandes áreas de memória (bancos de dados, HPC), **Large Pages** de 2 MB reduzem drasticamente a pressão no TLB:

```
TLB com páginas de 4 KB, 512 MB de dados:
  512 MB / 4 KB = 131.072 páginas → 131.072 entradas no TLB necessárias
  TLB tem ~2048 entradas → MUITOS TLB misses

TLB com Large Pages de 2 MB, 512 MB de dados:
  512 MB / 2 MB = 256 páginas → 256 entradas no TLB
  → TLB fica quente → ganho de 5-30% em workloads de memória intensiva
```

```cpp
// Requer privilégio SeLockMemoryPrivilege (apenas admins)
// e o privilégio deve estar ATIVADO no token

// 1. Ativar o privilégio no token
HANDLE hToken;
OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken);

TOKEN_PRIVILEGES tp;
tp.PrivilegeCount = 1;
LookupPrivilegeValueW(nullptr, SE_LOCK_MEMORY_NAME, &tp.Privileges[0].Luid);
tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
AdjustTokenPrivileges(hToken, FALSE, &tp, 0, nullptr, nullptr);
CloseHandle(hToken);

// 2. Alocar com Large Pages
SIZE_T largePageSize = GetLargePageMinimum();  // tipicamente 2 MB
if (largePageSize == 0) {
    // Hardware não suporta large pages
}

void* p = VirtualAlloc(nullptr,
    largePageSize * 10,   // 20 MB
    MEM_COMMIT | MEM_RESERVE | MEM_LARGE_PAGES,
    PAGE_READWRITE);
// Nota: Large Pages são sempre committed (não podem ser paginadas!)
```

---

## AWE: Address Windowing Extensions (32-bit legado)

No modo 32-bit (4 GB de espaço), AWE permite acessar mais RAM física que o espaço de endereços suporta, usando janelas mapeadas:

```cpp
// Em x64 com 128 TB de espaço, AWE é raramente necessário
// Mas ainda existente para compatibilidade:

ULONG_PTR nPages = 1024;   // 4 MB
ULONG_PTR pfnArray[1024];  // array de page frame numbers

AllocateUserPhysicalPages(GetCurrentProcess(), &nPages, pfnArray);

void* window = VirtualAlloc(nullptr, 4 * 1024 * 1024,
                             MEM_RESERVE | MEM_PHYSICAL, PAGE_READWRITE);

MapUserPhysicalPages(window, nPages, pfnArray);  // mapear a janela
// usar window[...] para acessar as páginas físicas
MapUserPhysicalPages(window, nPages, nullptr);   // desmapear

FreeUserPhysicalPages(GetCurrentProcess(), &nPages, pfnArray);
VirtualFree(window, 0, MEM_RELEASE);
```

---

## Performance: medindo uso de memória

```cpp
// Working Set = páginas atualmente em RAM para este processo:
PROCESS_MEMORY_COUNTERS_EX pmc;
pmc.cb = sizeof(pmc);
GetProcessMemoryInfo(GetCurrentProcess(),
    (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));

wprintf(L"Working Set: %zu MB\n",   pmc.WorkingSetSize / (1024*1024));
wprintf(L"Peak WS: %zu MB\n",       pmc.PeakWorkingSetSize / (1024*1024));
wprintf(L"Private bytes: %zu MB\n", pmc.PrivateUsage / (1024*1024));
wprintf(L"Page faults: %u\n",       pmc.PageFaultCount);
```

```
Process Explorer:
  Working Set   → RAM atual do processo
  Private Bytes → memória privada (não compartilhada com outros processos)
  Virtual Size  → total de espaço virtual reservado/commitado
  Peak WS       → máximo histórico de RAM

RAM Tools → diagnóstico de memória no sistema
  RAMMap (Sysinternals): distribuição detalhada de RAM por tipo
  VMMap (Sysinternals): mapa visual do espaço de endereços de um processo
```

---

*Próximo: [Memória Compartilhada e Seções](capitulo-05-03-shared-memory.md)*
