# Capítulo 5 — Gerenciamento de Memória: Visão Geral

> Livro didático baseado em *Windows Internals, 7th Edition*.
> Atualizado para Windows 11 com exemplos práticos em C++ moderno.

> Navegação: [Índice Cap.5](capitulo-05.md) | [Próximo](capitulo-05-02-alocacao.md)

---

## O Memory Manager: componente central do kernel

O **Memory Manager** (prefixo `Mm` no kernel) é responsável por tudo relacionado à memória:
- Mapeamento de memória virtual → física (page tables)
- Alocação e liberação de memória virtual (`VirtualAlloc`, `NtAllocateVirtualMemory`)
- Gerenciamento do page file (swap)
- Seções de arquivos mapeados em memória (`CreateFileMapping`)
- Working sets e paginação de memória física
- Proteção de páginas (read-only, execute, no-access, etc.)
- Copy-on-Write (COW) para DLLs e fork implícito

---

## O espaço de endereços virtual no Windows 11 x64

No Windows 11 x64, cada processo tem um espaço de endereços virtual de **128 TB user mode + 128 TB kernel mode** = 256 TB totais (com LA57 de 5 níveis de paginação: 128 PB!):

```
Espaço de Endereços Virtual x64 (padrão, 4 níveis de paginação):

0x0000000000000000  ┌─────────────────────────────────────────────────────────
                    │ NULL pointer guard (64 KB de no-access)
0x0000000000010000  ├─────────────────────────────────────────────────────────
                    │
                    │  USER MODE (128 TB)
                    │  ├── Executável mapeado (texto, dados, BSS)
                    │  ├── DLLs carregadas (kernel32, ntdll, etc.)
                    │  ├── Heap(s) do processo
                    │  ├── Stack(s) das threads
                    │  ├── Áreas VirtualAlloc explícitas
                    │  └── Arquivos mapeados em memória
                    │
0x00007FFFFFFFFFFF  ├─────────────────────────────────────────────────────────
                    │ (espaço inacessível — endereços canônicos inválidos)
                    │  ↑ User space limit
0xFFFF080000000000  ├─────────────────────────────────────────────────────────
                    │
                    │  KERNEL MODE (128 TB)
                    │  ├── Kernel code (ntoskrnl.exe)
                    │  ├── HAL
                    │  ├── Drivers (.sys)
                    │  ├── NonPaged Pool (memória sempre em RAM)
                    │  ├── Paged Pool (pode ser paginada)
                    │  ├── System cache (file cache do sistema)
                    │  └── KPCR, KPRCB (estruturas por CPU)
                    │
0xFFFFFFFFFFFFFFFF  └─────────────────────────────────────────────────────────
```

O modo x86 (32-bit) tem apenas 4 GB total: 2 GB user + 2 GB kernel (ou 3+1 com `/3GB`).

---

## Memória virtual vs. memória física

A memória virtual é uma abstração. Um endereço virtual 0x00401000 não corresponde diretamente ao endereço físico — o processador (com ajuda do kernel) traduz via **page tables**:

```
Endereço Virtual (64-bit)
  Bits 47..39: PML4E index (512 entradas)
  Bits 38..30: PDPTE index (512 entradas)
  Bits 29..21: PDE index   (512 entradas)
  Bits 20..12: PTE index   (512 entradas)
  Bits 11..0:  Page offset (4096 bytes)

CPU usa CR3 para encontrar o PML4:
  CR3 → PML4[bits 47..39] → PDPT[bits 38..30]
       → PD[bits 29..21] → PT[bits 20..12] → Frame físico
       + offset = Endereço Físico

  TLB (Translation Lookaside Buffer) cacheia traduções recentes
  TLB miss → 4 acessos à memória (muito mais lento)
```

Cada processo tem seu **próprio** conjunto de page tables (CR3 diferente). Por isso, o endereço virtual 0x00401000 no processo A aponta para um frame físico diferente do processo B.

---

## Estados de uma página de memória

Cada página (4 KB) de memória virtual está em um de três estados:

```
Estado      Significado                         Uso de RAM?
──────────────────────────────────────────────────────────
Free        Não comprometida, não reservada     Não
Reserved    Reservada (VAD existe), sem físico  Não
Committed   Comprometida — pode ser:
  In RAM    Mapeada para frame físico           Sim
  In Swap   Paginada para page file             No (swap)
```

O conceito chave: **reservar** é grátis — apenas registra a intenção de usar. **Commitar** consome "commit charge" (RAM + swap disponível). A página só vai para RAM quando é **acessada** pela primeira vez (page fault).

```cpp
// Reserve 1 GB de espaço de endereços (não usa RAM):
void* pBase = VirtualAlloc(nullptr, 1ULL * 1024 * 1024 * 1024,
                           MEM_RESERVE, PAGE_NOACCESS);

// Commitar apenas os primeiros 4 KB (usa RAM quando acessado):
void* pFirst = VirtualAlloc(pBase, 4096,
                            MEM_COMMIT, PAGE_READWRITE);
// Agora pFirst[0] = 0 → page fault → kernel aloca frame físico → continua

// Liberar tudo (decommit + unreserve):
VirtualFree(pBase, 0, MEM_RELEASE);
```

---

## VAD: Virtual Address Descriptor

O kernel rastreia o uso do espaço de endereços de cada processo via uma **árvore AVL de VADs** (Virtual Address Descriptors). Cada VAD descreve uma região de memória virtual:

```
VAD fields:
  StartingVpn  ← página inicial (endereço / 4096)
  EndingVpn    ← página final
  u.VadFlags:
    PrivateMemory  = 1 → VirtualAlloc; = 0 → arquivo mapeado
    Protection     → PAGE_READWRITE, PAGE_EXECUTE, etc.
    NoChange       → proteção não pode ser alterada (VirtualProtect bloqueado)
  Subsection     ← para arquivos mapeados: aponta para o arquivo
  Parent, LeftChild, RightChild ← árvore AVL
```

```
WinDbg:
kd> !vad <endereço de EPROCESS>    ← árvore VAD completa do processo
kd> !vad <endereço de EPROCESS> 1  ← incluindo endereços físicos
```

---

## Proteção de memória: flags de página

```cpp
// Flags de proteção mais comuns:
PAGE_NOACCESS        → qualquer acesso gera exceção
PAGE_READONLY        → apenas leitura
PAGE_READWRITE       → leitura e escrita
PAGE_EXECUTE         → apenas execução (sem leitura/escrita)
PAGE_EXECUTE_READ    → leitura + execução (sem escrita)
PAGE_EXECUTE_READWRITE → tudo — evitar em código seguro!
PAGE_WRITECOPY       → copy-on-write em escrita
PAGE_GUARD           → primeira access gera exceção (usado para pilhas)

// Modificar proteção de uma região:
DWORD oldProt;
VirtualProtect(ptr, size, PAGE_READONLY, &oldProt);
```

**DEP (Data Execution Prevention)**: processo com DEP ativado recebe exceção ao tentar executar código em páginas que não têm `PAGE_EXECUTE*`. Isso bloqueia exploits de buffer overflow clássicos.

---

## Page Faults: tipos e tratamento

Um page fault ocorre quando a CPU acessa um endereço virtual que não tem um frame físico mapeado. O kernel trata o fault e retoma a thread:

```
Tipos de page fault:
  1. Hard fault (demand paging)
     → Página commitada mas nunca acessada (zero-fill on demand)
     → Kernel aloca frame físico, zera a página, mapeia
     → ~microssegundos

  2. Soft fault (soft page fault)
     → Página estava em standby list (foi paginada mas ainda está em RAM)
     → Kernel remapeia o frame existente sem I/O de disco
     → ~microssegundos

  3. Hard fault (page-in from swap)
     → Página foi paginada para o page file
     → I/O de disco necessário para ler de volta
     → ~milissegundos (100-1000x mais lento)

  4. Copy-on-Write fault
     → Thread tenta escrever em página compartilhada (DLL, mmap)
     → Kernel cria cópia privada para este processo
     → RAM duplicada para este processo

  5. Invalid (AV — Access Violation)
     → Endereço não mapeado, ou proteção violada
     → EXCEPTION_ACCESS_VIOLATION para a aplicação
```

---

## Kernel Pools: NonPaged e Paged

O kernel aloca memória de seus próprios "heaps":

```
NonPaged Pool:
  → Memória que NUNCA é paginada para disco
  → Obrigatório para código que roda em IRQL >= DISPATCH_LEVEL
  → Mais escasso — use com moderação
  → ExAllocatePool(NonPagedPool, size)

Paged Pool:
  → Pode ser paginada para disco quando há pressão de memória
  → Apenas para código que roda em IRQL < DISPATCH_LEVEL
  → Mais abundante
  → ExAllocatePool(PagedPool, size)

Session Pool:
  → Pool privado de cada sessão de logon (Win32k.sys)
  → Separado para isolar sessões de usuário
```

```
WinDbg — ver uso dos pools:
kd> !poolused    ← uso total por tag de pool
kd> !pool <addr> ← identificar que alocação está neste endereço
kd> !poolfind MmSt 2  ← encontrar alocações com tag específica
```

---

## System Cache: file cache do Windows

Diferente do Linux onde o page cache é o mesmo que a memória de processo, o Windows tem um **System Cache** separado: uma região do espaço kernel onde arquivos são mapeados para leitura/escrita buffered.

```
Sem cache (FILE_FLAG_NO_BUFFERING):
  Aplicação → ReadFile → DMA → Buffer da Aplicação
  Exige alinhamento a setor (512B/4096B) e tamanho múltiplo

Com cache (padrão):
  Aplicação → ReadFile → CcCopyRead → System Cache (Kernel VA) → RAM
  Se não estiver em cache → page fault → lê do disco para cache
```

O cache do sistema e a RAM disponível para aplicações competem pelo mesmo pool físico — o Memory Manager equilibra dinamicamente (working set trimming).

---

*Próximo: [Alocação de Memória: VirtualAlloc, Heaps e Large Pages](capitulo-05-02-alocacao.md)*
