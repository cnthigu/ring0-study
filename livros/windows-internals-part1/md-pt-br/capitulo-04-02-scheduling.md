# Capítulo 4 — Scheduling de Threads: Prioridades e Quantum

> Livro didático baseado em *Windows Internals, 7th Edition*.
> Atualizado para Windows 11 com exemplos práticos em C++ moderno.

> Navegação: [Anterior](capitulo-04-01-internos-thread.md) | [Índice Cap.4](capitulo-04.md) | [Próximo](capitulo-04-03-apcs-dpcs.md)

---

## O escalonador do Windows: preemptivo e baseado em prioridades

O Windows usa um escalonador **preemptivo baseado em prioridades**:
- A thread de maior prioridade pronta para executar sempre executa
- Se duas threads têm a mesma prioridade, elas compartilham a CPU em rodízio (round-robin) com **quantum**
- Uma thread de alta prioridade que fica pronta preempta imediatamente a thread atual, mesmo no meio de um quantum

```
32 níveis de prioridade (0 a 31):

31  REALTIME ─────────────────────── ]  IRQL = PASSIVE_LEVEL (user mode)
30  REALTIME                          ]  Afeta sistema inteiro — use com cuidado!
29  REALTIME                          ]
28  REALTIME
...
16  REALTIME mínimo

15  HIGHEST ──────────────────────── ] DYNAMIC range (normal threads)
14  ABOVE_NORMAL_HIGH
13  ABOVE_NORMAL
...
 8  NORMAL (prioridade padrão)
...
 1  IDLE (roda só quando nada mais precisa da CPU)

 0  ZERO — reservado para o Zero Page Thread do Memory Manager
```

### Duas classes de prioridade

| Range | Nome | Quem usa |
|-------|------|---------|
| 0–15 | **Dynamic** | Processos de usuário — ajustada dinamicamente pelo kernel |
| 16–31 | **Realtime** | Drivers, threads de sistema críticas — NÃO é ajustada automaticamente |

> Cuidado: atribuir prioridade realtime (16+) a uma thread de usuário pode tornar o sistema não responsivo — a thread monopoliza a CPU e impede até as threads do kernel de processar I/O de mouse/teclado.

---

## Prioridade de processo + prioridade de thread = prioridade final

A prioridade final de uma thread é a combinação de:
1. **Priority Class** do processo (IDLE, BELOW_NORMAL, NORMAL, ABOVE_NORMAL, HIGH, REALTIME)
2. **Thread Priority** relativa ao processo (-2 a +2, ou THREAD_PRIORITY_TIME_CRITICAL / THREAD_PRIORITY_IDLE)

```
Priority Class              Base Priority
──────────────────────────────────────────
IDLE_PRIORITY_CLASS         4
BELOW_NORMAL_PRIORITY_CLASS 6
NORMAL_PRIORITY_CLASS       8     ← padrão
ABOVE_NORMAL_PRIORITY_CLASS 10
HIGH_PRIORITY_CLASS         13
REALTIME_PRIORITY_CLASS     24

Thread Priority Offset      Ajuste
──────────────────────────────────
THREAD_PRIORITY_LOWEST      -2
THREAD_PRIORITY_BELOW_NORMAL -1
THREAD_PRIORITY_NORMAL       0    ← padrão
THREAD_PRIORITY_ABOVE_NORMAL +1
THREAD_PRIORITY_HIGHEST      +2
THREAD_PRIORITY_TIME_CRITICAL → nível máximo da classe (15 ou 31)
THREAD_PRIORITY_IDLE         → nível mínimo da classe (1 ou 16)
```

```cpp
// Configurar prioridade de processo e thread
SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);

// Resultado: processo HIGH (base 13) + thread +1 = prioridade 14

// Ler prioridade atual:
int prio = GetThreadPriority(GetCurrentThread()); // retorna -2 a +2 ou valores especiais
DWORD classe = GetPriorityClass(GetCurrentProcess());
```

---

## Quantum: a fatia de tempo de cada thread

O **quantum** é o tempo que uma thread pode usar a CPU antes de ceder para outra thread da mesma prioridade.

No Windows 11 (workstation), o quantum padrão é **~15ms** (dois ticks de relógio de 7.5ms cada).

Valores de quantum:

| Configuração | Quantum | Uso típico |
|-------------|---------|-----------|
| Workstation (padrão) | 2 × clock tick (~20ms) | Desktop interativo |
| Server | 12 × clock tick (~120ms) | Throughput — menos troca de contexto |

```
O quantum é medido em clock ticks, não em ms absolutas.
timeBeginPeriod(1) reduz o tick para ~1ms → quantum menor.

Verificar via Registry:
HKLM\SYSTEM\CurrentControlSet\Control\PriorityControl
  PerfBoostMode    → boost de prioridade para foreground
  Win32PrioritySeparation → quantum/foreground boost
```

```cpp
// timeBeginPeriod(1): usado por jogos para reduzir latência
// MAS também aumenta consumo de energia e degrada sleep accuracy do sistema inteiro
#include <timeapi.h>
#pragma comment(lib, "winmm.lib")

timeBeginPeriod(1);  // clock a 1ms — afeta TODO o sistema enquanto ativo
// ... código sensível ao tempo ...
timeEndPeriod(1);    // restaurar
```

---

## Boosts de prioridade dinâmicos

Para manter a interface responsiva, o kernel aplica **boosts temporários de prioridade** em certas situações:

### 1. Foreground Boost
Quando uma janela recebe foco, as threads do processo passam a ter quantum aumentado (em workstations). No Process Explorer, você verá processos em foreground com mais CPU mesmo tendo a mesma prioridade.

### 2. I/O Completion Boost
Quando uma thread acorda depois de esperar por I/O, recebe um boost de prioridade temporário. O boost varia por tipo de I/O:

```
Tipo de I/O          Boost
────────────────────────────
Disco               +1
Rede                +2
Teclado             +6
Mouse               +6
Áudio               +8
```

Isso garante que threads esperando por eventos de usuário acordem rapidamente quando os eventos chegam.

### 3. Starvation Protection (Priority Boost Anti-Starvation)
Se uma thread não executa por ~4 segundos (configurable), o kernel aplica um boost para prioridade 15 temporariamente para que ela tenha chance de executar. Depois do boost, volta à prioridade base.

```cpp
// Desabilitar boosts para uma thread (para benchmarks reproducíveis):
SetThreadPriorityBoost(GetCurrentThread(), TRUE);  // TRUE = desabilitar boost
```

---

## Ready Queues: como o dispatcher funciona

Cada nível de prioridade (0–31) tem uma fila de threads prontas para executar. Por CPU:

```
Dispatcher Database (por CPU, via KPRCB):
  ReadyListHead[32]   ← array de listas duplamente ligadas
  ReadySummary        ← bitmask: bit N = ReadyListHead[N] não vazio

Ao agendar uma nova thread:
  1. Verificar ReadySummary para encontrar a fila mais alta não-vazia
  2. Retirar a primeira thread daquela fila
  3. Disparar contexto switch

Ao adicionar thread pronta:
  1. Inserir na ReadyListHead[prioridade]
  2. Setar bit correspondente em ReadySummary
  3. Se prioridade > thread atual → preemptar imediatamente
```

---

## Contexto Switch: o que realmente acontece

Quando o scheduler troca de thread, ele salva e restaura o **contexto de CPU** completo:

```
Contexto salvo/restaurado em cada context switch (x64):
  ├── General Registers: RAX, RBX, RCX, RDX, RSI, RDI, RBP
  ├── RSP (Stack Pointer)
  ├── R8-R15 (extended registers x64)
  ├── RIP (Instruction Pointer) — onde retomar
  ├── RFLAGS (flags de condição)
  ├── Floating Point/SSE: XMM0-XMM15, MXCSR
  ├── Segment Registers: CS, DS, ES, FS, GS
  ├── CR3 (Page Directory Base) — se troca de processo também
  └── Debug Registers: DR0-DR7 (se breakpoints de HW ativos)
```

Custo aproximado de um context switch: **1-10 µs** (variável com cache state, NUMA, etc.)

```cpp
// Forçar uma troca de contexto voluntária:
SwitchToThread();  // cede para outra thread de MESMA prioridade (round-robin)
Sleep(0);          // similar — cede por pelo menos um quantum
Sleep(1);          // dorme por pelo menos 1 clock tick (~15ms com timer padrão)
```

---

## NUMA e Scheduling em Sistemas Multi-CPU

Em sistemas NUMA (Non-Uniform Memory Access), acessar memória de outro nó é 2-3x mais lento. O scheduler do Windows tenta manter threads no mesmo nó NUMA onde sua memória foi alocada:

```cpp
// Criar thread em um nó NUMA específico (para workloads NUMA-aware):
HANDLE hThread = CreateRemoteThreadEx(
    GetCurrentProcess(),
    nullptr, 0,
    WorkerThread, nullptr,
    0,    // flags
    nullptr,
    nullptr  // LPPROC_THREAD_ATTRIBUTE_LIST — pode incluir processor affinity
);

// Afinidade: limitar thread a certos núcleos
DWORD_PTR affinity = (1ULL << 0) | (1ULL << 1); // apenas CPUs 0 e 1
SetThreadAffinityMask(hThread, affinity);

// Prioridade ideal para um nó NUMA:
SetThreadIdealProcessorEx(hThread, &idealProcessor, &oldIdeal);
```

---

## Verificando scheduling em tempo real

```
WinDbg:
kd> !pcr               ← mostra KPCR do processador atual
kd> !prcb              ← KPRCB — inclui CurrentThread, NextThread, IdleThread
kd> !ready             ← mostra todas as threads prontas, por prioridade
kd> !locked            ← mostra objetos dispatcher bloqueados

Process Explorer:
  → Coluna "Priority" em threads mostra prioridade atual
  → Coluna "Context Switches" mostra quantas vezes a thread foi agendada
  → Coluna "CPU Time" mostra tempo total de CPU

PowerShell (monitorar prioridade de processo):
  Get-Process | Sort-Object CPU -Descending | Select-Object -First 10 Name, PriorityClass, CPU
```

---

*Próximo: [APCs, DPCs e IRQL](capitulo-04-03-apcs-dpcs.md)*
