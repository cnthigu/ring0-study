# Capítulo 4 — APCs, DPCs e IRQL

> Livro didático baseado em *Windows Internals, 7th Edition*.
> Atualizado para Windows 11 com exemplos práticos em C++ moderno.

> Navegação: [Anterior](capitulo-04-02-scheduling.md) | [Índice Cap.4](capitulo-04.md) | [Próximo](capitulo-04-04-wait-sync.md)

---

## IRQL: Interrupt Request Level

O conceito mais fundamental do kernel do Windows para entender APCs e DPCs é o **IRQL** — Interrupt Request Level.

O IRQL não é uma propriedade de thread, é uma propriedade **do processador atual**. Ele controla quais interrupções podem preemtar o código que está rodando agora:

```
IRQL  Nome          Significado
──────────────────────────────────────────────────────────
31    HIGH          Reservado (NMI, power failure)
...
 2    DPC/DISPATCH  DPCs rodam aqui; page faults são proibidos
 1    APC           APCs de kernel mode rodam aqui
 0    PASSIVE       Código normal de user mode e kernel mode

IRQLs de hardware (variam por sistema, geralmente 3-26):
  → Interrupções de dispositivos
  → Cada IRQ de hardware tem um IRQL associado
```

A regra de ouro: **código rodando em IRQL alto só pode ser preemptado por código em IRQL ainda mais alto**.

```cpp
// Em código de kernel/driver:
KIRQL oldIrql;
KeRaiseIrql(DISPATCH_LEVEL, &oldIrql);  // eleva IRQL para 2
// ... código que não pode dormir aqui ...
KeLowerIrql(oldIrql);                   // restaura IRQL anterior
```

### O que é proibido em IRQL >= DISPATCH_LEVEL

Em IRQL 2 (DISPATCH_LEVEL) ou superior:
- **Page faults são fatais** — toda memória acessada deve estar em NonPagedPool
- **Esperas por objetos dispatcher são proibidas** — não pode chamar KeWaitForSingleObject
- **Alocação de memória paginável é proibida** — apenas NonPagedPool
- A thread corrente não pode ser agendada de fora — o scheduler não preempta em IRQL >= DISPATCH

---

## APC: Asynchronous Procedure Call

APCs são mecanismos para executar código no contexto de uma thread específica. São chamadas de função "injetadas" em uma thread, executadas quando a thread está em estado alertável.

### Dois tipos de APC

```
APC de Kernel Mode (KIRQL = APC_LEVEL = 1):
  → Executado com IRQL elevado para 1
  → Preempta a thread mas não pode ser bloqueado pela thread
  → Usado pelo I/O Manager para completar operações de I/O assíncronas

APC de User Mode (KIRQL = PASSIVE_LEVEL = 0):
  → Executado quando a thread está em estado "alertável"
  → A thread PODE evitar o APC simplesmente não entrando em estado alertável
  → Usado por: ReadFileEx/WriteFileEx completion routines, QueueUserAPC
```

### User-mode APCs e estado alertável

```cpp
#include <windows.h>

// Completion routine — será chamada como APC quando o I/O completar
VOID WINAPI ReadCompletionRoutine(
    DWORD dwErrorCode,
    DWORD dwBytesTransferred,
    LPOVERLAPPED pOverlapped)
{
    printf("I/O completou: %u bytes\n", dwBytesTransferred);
}

// Para receber APCs, a thread precisa entrar em "alertable wait"
// Funções que colocam a thread em estado alertável:
SleepEx(0, TRUE);              // alertable sleep
WaitForSingleObjectEx(h, INFINITE, TRUE);  // bAlertable = TRUE
MsgWaitForMultipleObjectsEx(..., MWMO_ALERTABLE);
```

### QueueUserAPC: injetar código em outra thread

```cpp
// APC function — executada no contexto da thread alvo
VOID NTAPI MeuAPC(ULONG_PTR param) {
    wprintf(L"APC executada, param=%llu\n", param);
}

// Injetar APC na thread alvo
HANDLE hThread = ...; // handle para thread alvo com THREAD_SET_CONTEXT
QueueUserAPC(MeuAPC, hThread, (ULONG_PTR)42);
// O APC só roda quando a thread entrar em estado alertável!
```

> Esta é a base de técnicas de injeção de código "APC injection" — mais furtiva que CreateRemoteThread porque não cria uma nova thread visível. Antivírus modernos monitoram QueueUserAPC entre processos.

### Como o kernel usa APCs para completar I/O

```
Sequência de I/O assíncrono com APC:
  1. Thread chama ReadFileEx com completion routine
  2. I/O Manager inicia a leitura em disco (DMA ou PIO)
  3. Thread continua executando (não bloqueia)
  4. Thread chama SleepEx(0, TRUE) → estado alertável
  5. Hardware completa a leitura → gera interrupção → DPC
  6. DPC do I/O Manager enfileira um APC na thread original
  7. Quando IRQL cai para APC_LEVEL (1), o APC roda
  8. APC chama a completion routine com os resultados
```

---

## DPC: Deferred Procedure Call

DPCs são o mecanismo para adiar trabalho de alta prioridade (interrupções de hardware) para um momento ligeiramente posterior, em IRQL = DISPATCH_LEVEL (2).

O problema que DPCs resolvem: interrupções de hardware rodam em IRQL muito alto e precisam ser rápidas. Não dá para fazer processamento longo nelas. A solução: a interrupção faz o mínimo e enfileira um DPC para o trabalho real.

```
Sequência típica com DPC:
  1. Hardware gera interrupção (IRQL elevado, ex: IRQL 6 para NIC)
  2. ISR (Interrupt Service Routine) do driver roda em IRQL 6:
     a. Lê status do hardware
     b. Limpa a interrupção no hardware
     c. Enfileira um DPC: IoRequestDpc() ou KeInsertQueueDpc()
     d. Retorna RAPIDAMENTE
  3. IRQL cai de volta para DISPATCH_LEVEL (2)
  4. DPC roda em IRQL 2:
     a. Processa os dados que vieram do hardware
     b. Completa IRPs (I/O Request Packets)
     c. Notifica threads que aguardavam este I/O
  5. IRQL cai para PASSIVE_LEVEL (0)
  6. APCs pendentes rodam (se houver)
```

```c
// Em código de driver (C, não C++):
KDPC meuDpc;

// Inicializar DPC
KeInitializeDpc(&meuDpc, MinhaRotinaDpc, contexto);

// Enfileirar DPC (chamado da ISR, em IRQL alto):
KeInsertQueueDpc(&meuDpc, arg1, arg2);

// A rotina DPC em si (roda em IRQL = DISPATCH_LEVEL):
VOID MinhaRotinaDpc(
    PKDPC Dpc,
    PVOID Context,
    PVOID SystemArgument1,
    PVOID SystemArgument2)
{
    // Processar dados — memória deve ser NonPaged!
    // NÃO pode: dormir, esperar por objects, alocar PagedPool
    // PODE: KeInsertQueueApc, IoCompleteRequest, KeSetEvent
}
```

### DPC Timer: executar código periodicamente em IRQL 2

```c
KTIMER timer;
KDPC timerDpc;

KeInitializeTimer(&timer);
KeInitializeDpc(&timerDpc, TimerCallback, contexto);

LARGE_INTEGER dueTime;
dueTime.QuadPart = -10000000LL; // 1 segundo (em unidades de 100ns negativos)

// Repetir a cada 1 segundo:
KeSetTimerEx(&timer, dueTime,
             1000,        // período em ms para repetição
             &timerDpc);  // DPC a disparar
```

---

## A hierarquia de interrupção completa

```
IRQL  Tipo de código              Pode ser preemptado por
────────────────────────────────────────────────────────
31    NMI/Machine Check           Nada
...
26+   Hardware (IRQ mais alto)    IRQL 27+
...
 3+   Hardware (IRQ baixo)        IRQLs superiores + hardware
 2    DPC/DISPATCH               Hardware + NMI
 1    APC                         DPCs + hardware
 0    PASSIVE (user mode, maioria do kernel)  Tudo acima
```

O scheduler roda em IRQL DISPATCH_LEVEL. É por isso que código em PASSIVE_LEVEL pode ser preemptado pelo scheduler (que eleva IRQL para 2), mas código já em DISPATCH_LEVEL não pode.

---

## Diagnóstico com WinDbg

```
kd> !irql           ← IRQL atual do processador
kd> !dpc            ← DPCs pendentes
kd> !apc            ← APCs pendentes (por thread)
kd> !stacks 2       ← mostrar threads rodando em DISPATCH_LEVEL ou superior
```

Para investigar um travamento de sistema:
```
kd> !analyze -v     ← análise automática de crash dump
→ Frequentemente revela DPC ou APC rodando em IRQL errado
→ "IRQL_NOT_LESS_OR_EQUAL" = acesso a memória paginável em IRQL >= 2
→ "DRIVER_IRQL_NOT_LESS_OR_EQUAL" = idem, causado por driver específico
```

---

## Resumo da relação entre IRQL, APC e DPC

```
                    Pode ser preemptado por:
Código              IRQL    DPC?    APC?    Thread Switch?
──────────────────────────────────────────────────────────
User mode / PASSIVE  0      Sim     Sim     Sim
Kernel APC level     1      Sim     Não     Não
DPC / Dispatch       2      Não     Não     Não
ISR (hardware)       3+     Não     Não     Não
```

A regra prática para desenvolvedores de driver: **eleve IRQL apenas pelo mínimo de tempo necessário**, e **nunca acesse memória paginável com IRQL >= 2**.

---

*Próximo: [Wait States e Sincronização](capitulo-04-04-wait-sync.md)*
