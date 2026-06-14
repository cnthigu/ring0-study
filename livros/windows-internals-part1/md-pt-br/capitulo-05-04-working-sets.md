# Capítulo 5 — Working Sets, Paginação e Memória Física

> Livro didático baseado em *Windows Internals, 7th Edition*.
> Atualizado para Windows 11 com exemplos práticos em C++ moderno.

> Navegação: [Anterior](capitulo-05-03-shared-memory.md) | [Índice Cap.5](capitulo-05.md)

---

## A hierarquia de memória física no Windows

O Memory Manager mantém um banco de dados de cada frame físico (Page Frame Number database — PFN DB). Cada frame pode estar em uma das seguintes listas:

```
PFN Database Lists:
  ┌─────────────────────────────────────────────────────┐
  │  Working Set (de um processo)                       │
  │  → Frames atualmente mapeados neste processo        │
  │  → O processo pode acessar sem page fault           │
  └─────────────────────────────────────────────────────┘
         ↓ (quando o MM decide paginar)
  ┌─────────────────────────────────────────────────────┐
  │  Standby List (por prioridade 0-7)                  │
  │  → Frames desmapeados mas ainda com dados válidos   │
  │  → "Soft fault" — remapear é barato (sem disco)    │
  └─────────────────────────────────────────────────────┘
         ↓ (quando o conteúdo foi modificado)
  ┌─────────────────────────────────────────────────────┐
  │  Modified List                                      │
  │  → Frames com dados "sujos" que precisam ir ao swap │
  │  → Modified Page Writer os escreve para page file   │
  └─────────────────────────────────────────────────────┘
         ↓ (após escrita no swap)
  ┌─────────────────────────────────────────────────────┐
  │  Free List / Zero List                              │
  │  → Frames disponíveis para novos processos          │
  │  → Zero Page Thread zera os frames em background    │
  └─────────────────────────────────────────────────────┘
```

---

## Working Set: a RAM de um processo

O **Working Set** de um processo é o conjunto de páginas atualmente mapeadas em RAM. O kernel gerencia o tamanho do working set usando limites:

```cpp
// Ver working set atual e limites:
PROCESS_MEMORY_COUNTERS_EX pmc = { sizeof(pmc) };
GetProcessMemoryInfo(GetCurrentProcess(),
    (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));

wprintf(L"Working Set atual: %zu MB\n", pmc.WorkingSetSize / (1024*1024));
wprintf(L"Peak Working Set: %zu MB\n",  pmc.PeakWorkingSetSize / (1024*1024));

// Modificar limites do working set:
SIZE_T minWS = 10 * 1024 * 1024;   // 10 MB mínimo
SIZE_T maxWS = 200 * 1024 * 1024;  // 200 MB máximo
SetProcessWorkingSetSize(GetCurrentProcess(), minWS, maxWS);
```

Quando há pressão de memória no sistema, o **Working Set Manager** (rodando em um timer DPC) escolhe processos para **trim** — remove páginas do working set para a standby list, liberando RAM para outros.

---

## Memory Compression (Windows 10+)

No Windows 10 e 11, o sistema de memória tem um componente adicional: **Memory Compression**. Em vez de paginar para disco quando a RAM está cheia, o kernel comprime páginas e as mantém em RAM:

```
Sem compressão (Windows 7/8):
  Falta de RAM → Escrever para page file → Ler do disco (lento)
  
Com compressão (Windows 10/11):
  Falta de RAM → Comprimir em RAM (processo "Memory Compression" PID ~X)
               → Manter comprimido em RAM (rápido!)
               → Só vai para disco se comprimido ainda não couber
```

O processo `Memory Compression` (sem executável visível — é um "minimal process") aparece no Task Manager usando RAM — essa é a memória comprimida. A RAM total usada pelo sistema = working sets + kernel + comprimido.

```
WinDbg:
kd> !memlist   ← distribuição de RAM pelas listas do PFN DB
kd> !poolused  ← uso do kernel pool
kd> !wsle      ← Working Set List Entries do processo atual
```

---

## Page File: o swap do Windows

O Page File (`pagefile.sys`, geralmente em C:\) armazena páginas que foram retiradas da RAM:

```
Configuração recomendada no Windows 11:
  - Gerenciamento automático: deixe o Windows decidir o tamanho
  - Ou: mínimo = RAM física, máximo = 2-3x RAM física
  - NÃO desabilite (mesmo com muita RAM) — o kernel usa para minidumps e outros fins

O Page File tem múltiplas funções:
  1. Backing store para memória privada anônima (MEM_COMMIT)
  2. Backing store para memória compartilhada anônima (CreateFileMapping com INVALID_HANDLE_VALUE)
  3. Armazenamento de crash dumps (partial)
  4. PrefetchMap para paginas de inicialização
```

---

## Pinning de memória: VirtualLock

Para evitar que páginas sejam paginadas (útil para dados criptográficos ou código de baixa latência):

```cpp
// Requer SeIncreaseWorkingSetPrivilege ou limite alto de working set
void* sensitiveMem = VirtualAlloc(nullptr, 4096, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);

// Impedir paginação desta memória:
VirtualLock(sensitiveMem, 4096);

// ... usar para dados sensíveis (chaves, tokens) ...

// Zerar ANTES de liberar (para não deixar dados na standby list):
SecureZeroMemory(sensitiveMem, 4096);

VirtualUnlock(sensitiveMem, 4096);
VirtualFree(sensitiveMem, 0, MEM_RELEASE);
```

---

## NUMA e alocação de memória

Em sistemas NUMA, alocar memória no nó "errado" custa 2-3x mais tempo de acesso:

```cpp
// Alocar memória especificamente no nó NUMA onde a thread rodará:
ULONG numaNode = 0;
GetNumaProcessorNodeEx(&processorNumber, &numaNode);

void* p = VirtualAllocExNuma(
    GetCurrentProcess(),
    nullptr,
    SIZE,
    MEM_COMMIT | MEM_RESERVE,
    PAGE_READWRITE,
    numaNode);  // alocar no nó local da CPU atual
```

---

## Diagnóstico de memória com RAMMap e VMMap

**RAMMap** (Sysinternals) mostra a distribuição de toda a RAM física:

```
Use Counts:
  Process Private → RAM de processos (working sets privados)
  Mapped File     → System cache (arquivos mapeados)
  Shareable       → DLLs e seções compartilhadas
  Page Table      → overhead das page tables
  Paged Pool      → kernel Paged Pool
  Nonpaged Pool   → kernel NonPaged Pool
  Free            → RAM disponível (inclui standby/free lists)
```

**VMMap** (Sysinternals) mostra o mapa virtual de um processo específico:

```
Tipo          Significado
Image         DLLs/EXEs mapeados (compartilhados entre processos)
Mapped        Arquivos mapeados (CreateFileMapping)
Private Data  Memória privada (heap, stack, VirtualAlloc anônimo)
Shareable     Seções shared anônimas
```

---

## Pool de processos: o impacto do Memory Manager no sistema

O Memory Manager equilibra continuamente:
1. **Working set trimming**: quando RAM < threshold, remove páginas de processos
2. **Page writing**: Modified Page Writer escreve páginas sujas no swap
3. **Zero Page Thread**: zera frames livres em background (idle priority)
4. **Memory Compression Thread**: comprime standby list quando RAM < threshold

Se você vê alta atividade de disco sem I/O óbvio de aplicativos, é quase certo que é paginação. Diagnosticar:

```
Performance Monitor → Add counters:
  Memory\Page Faults/sec       → page faults totais (inclui soft faults)
  Memory\Page Reads/sec        → page-ins do disco (hard faults de leitura)
  Memory\Pages Output/sec      → writes para page file
  Memory\Available MBytes      → RAM livre + standby de alta prioridade
  Memory\Committed Bytes       → commit charge total
  Memory\Commit Limit          → limite máximo de commit (RAM + swap)
  Process(*)\Working Set       → RAM de cada processo
```

---

*Fim do Capítulo 5. Próximo: [Capítulo 6 — Sistema de I/O](capitulo-06.md)*
