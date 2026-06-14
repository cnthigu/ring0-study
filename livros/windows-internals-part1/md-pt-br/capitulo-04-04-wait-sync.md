# Capítulo 4 — Wait States e Sincronização

> Livro didático baseado em *Windows Internals, 7th Edition*.
> Atualizado para Windows 11 com exemplos práticos em C++ moderno.

> Navegação: [Anterior](capitulo-04-03-apcs-dpcs.md) | [Índice Cap.4](capitulo-04.md)

---

## Como threads aguardam: o sistema de Wait do kernel

Quando uma thread chama `WaitForSingleObject`, ela não fica em loop — ela é **bloqueada pelo kernel** e removida da ready queue até que o objeto seja sinalizado. Isso é chamado de **wait state**.

```
Thread em RUNNING chama WaitForSingleObject(hEvent, INFINITE):
  1. Kernel verifica: objeto já está sinalizado?
     → Sim: retorna imediatamente, sem bloquear
     → Não: continua para passo 2
  2. Kernel cria uma KWAIT_BLOCK na stack da thread
  3. Thread é removida da ready queue
  4. Thread passa para estado Waiting
  5. CPU é entregue para outra thread (context switch)
  ...
  (algum tempo depois)
  ...
  6. Outro código sinaliza o evento (SetEvent)
  7. Kernel percorre a wait list do evento
  8. Encontra nossa thread na lista
  9. Thread volta para estado Ready
  10. Quando a CPU estiver disponível: estado Running
  11. WaitForSingleObject retorna WAIT_OBJECT_0
```

---

## Dispatcher Objects: objetos sinalizáveis

Todo objeto do kernel que suporta WaitForSingleObject tem um **DISPATCHER_HEADER** como primeiro campo. Esse header contém:
- `Type`: tipo do objeto (evento, mutex, semáforo, etc.)
- `SignalState`: estado sinalizado/não-sinalizado
- `WaitListHead`: lista das threads aguardando este objeto

```
Objetos dispatcher (todos usáveis com WaitForSingleObject):
  ├── Event           → SetEvent / ResetEvent / PulseEvent
  ├── Mutex/Mutant    → ownership exclusivo
  ├── Semaphore       → contador (N threads simultâneas)
  ├── Timer (waitable) → KeSetTimer / CreateWaitableTimer
  ├── Thread          → sinalizado quando a thread termina
  ├── Process         → sinalizado quando o processo termina
  ├── Job             → sinalizado quando todos os processos terminam
  └── File (certos)   → I/O assíncrono completado
```

---

## Events: manual-reset vs auto-reset

```cpp
#include <windows.h>
#include <vector>
#include <thread>

// Manual-reset event: permanece sinalizado até ResetEvent
HANDLE hManual = CreateEventW(nullptr,
    TRUE,   // bManualReset = TRUE → manual reset
    FALSE,  // bInitialState = não sinalizado
    L"MeuEventoManual");

// Auto-reset event: reseta automaticamente após liberar UMA thread
HANDLE hAuto = CreateEventW(nullptr,
    FALSE,  // bManualReset = FALSE → auto reset
    FALSE,
    nullptr);

// Exemplo: broadcast (manual) vs one-shot (auto)
void producer_manual() {
    // Sinaliza o evento → TODAS as threads que aguardam são liberadas
    SetEvent(hManual);
    // hManual permanece sinalizado até alguém chamar ResetEvent
}

void producer_auto() {
    // Sinaliza o evento → APENAS UMA thread é liberada
    SetEvent(hAuto);
    // hAuto volta a não-sinalizado automaticamente
}

// Exemplo prático: pipeline produtor/consumidor com auto-reset event
class WorkQueue {
    HANDLE m_event;
    CRITICAL_SECTION m_cs;
    std::vector<int> m_items;
public:
    WorkQueue() {
        m_event = CreateEventW(nullptr, FALSE, FALSE, nullptr);
        InitializeCriticalSection(&m_cs);
    }
    ~WorkQueue() {
        CloseHandle(m_event);
        DeleteCriticalSection(&m_cs);
    }
    void Push(int item) {
        EnterCriticalSection(&m_cs);
        m_items.push_back(item);
        LeaveCriticalSection(&m_cs);
        SetEvent(m_event);  // acorda UM consumidor
    }
    bool Pop(int* item, DWORD timeout = INFINITE) {
        if (WaitForSingleObject(m_event, timeout) != WAIT_OBJECT_0)
            return false;
        EnterCriticalSection(&m_cs);
        if (!m_items.empty()) {
            *item = m_items.front();
            m_items.erase(m_items.begin());
        }
        LeaveCriticalSection(&m_cs);
        return true;
    }
};
```

---

## Mutex: exclusão mútua com ownership

```cpp
// Mutex tem dono — só a thread que adquiriu pode liberar
HANDLE hMutex = CreateMutexW(nullptr, FALSE, L"MeuMutex");

// Aguardar + adquirir:
WaitForSingleObject(hMutex, INFINITE);
// → a thread agora é "dona" do mutex

// Seção crítica
DoProtectedWork();

// Liberar:
ReleaseMutex(hMutex);
// → próxima thread na wait list adquire ownership

CloseHandle(hMutex);
```

**Abandonment detection**: se a thread dona de um mutex terminar sem chamar ReleaseMutex, o próximo Wait retorna `WAIT_ABANDONED`. Isso indica um bug — dados protegidos pelo mutex podem estar em estado inconsistente.

```cpp
DWORD result = WaitForSingleObject(hMutex, INFINITE);
if (result == WAIT_ABANDONED) {
    // Thread anterior morreu com o mutex adquirido
    // Estado dos dados protegidos é INCERTO
    // Normalmente: abort ou recovery
}
```

---

## Semáforo: controle de recursos

```cpp
// Semáforo permite N threads simultâneas
HANDLE hSem = CreateSemaphoreW(nullptr,
    3,   // lInitialCount = 3 (começa com 3 "slots" disponíveis)
    3,   // lMaximumCount = 3 (máximo de 3 threads simultâneas)
    nullptr);

// Adquirir um slot (decrementa contador):
WaitForSingleObject(hSem, INFINITE);
// → contador vai de 3→2, ou 2→1, ou 1→0 (bloqueia se já for 0)

DoLimitedResourceWork();

// Liberar o slot (incrementa contador):
ReleaseSemaphore(hSem, 1, nullptr);
// → contador incrementa, libera uma thread bloqueada se houver
```

Uso clássico: limitar o número de conexões simultâneas a um banco de dados, ou controlar acesso a um pool de recursos.

---

## WaitForMultipleObjects: aguardar vários objetos

```cpp
HANDLE handles[3] = { hThread1, hThread2, hEvent };

// Aguardar QUALQUER um dos objetos ser sinalizado:
DWORD idx = WaitForMultipleObjects(3, handles, FALSE, INFINITE);
// retorna WAIT_OBJECT_0 + índice → handles[idx - WAIT_OBJECT_0]

// Aguardar TODOS os objetos serem sinalizados:
DWORD result = WaitForMultipleObjects(3, handles, TRUE, INFINITE);

// Com timeout:
DWORD result = WaitForMultipleObjects(3, handles, FALSE, 5000); // 5 segundos
if (result == WAIT_TIMEOUT) {
    // Nenhum objeto ficou sinalizado em 5 segundos
}
```

> Limite: `WaitForMultipleObjects` aceita no máximo **MAXIMUM_WAIT_OBJECTS = 64** handles. Para aguardar mais objetos, use I/O Completion Ports ou Thread Pool.

---

## CRITICAL_SECTION: mutex de user mode (rápido)

```cpp
CRITICAL_SECTION cs;
InitializeCriticalSection(&cs);

// Adquirir (sem syscall se não houver contention):
EnterCriticalSection(&cs);
// → se não houver contention: loop de spin (~10-100 spins) em user mode
// → se houver contention: event de kernel para bloquear (syscall)

DoProtectedWork();

LeaveCriticalSection(&cs);
DeleteCriticalSection(&cs);

// Versão com try (não bloqueia):
if (TryEnterCriticalSection(&cs)) {
    DoProtectedWork();
    LeaveCriticalSection(&cs);
} else {
    // Alguém já tem o CS — fazer outra coisa
}
```

A `CRITICAL_SECTION` é muito mais rápida que um mutex kernel em casos sem contention porque:
1. Mantém um **spin count** — tenta adquirir em user mode (sem syscall) por N iterações
2. Só cria um kernel event e faz syscall se o spin falhar
3. Ideal para seções muito curtas (microssegundos)

---

## Slim Reader/Writer Lock (SRW): leitura concorrente

```cpp
SRWLOCK srwLock = SRWLOCK_INIT;

// Leitura (múltiplas threads simultâneas):
AcquireSRWLockShared(&srwLock);
ReadData();
ReleaseSRWLockShared(&srwLock);

// Escrita (exclusiva — bloqueia toda leitura e escrita):
AcquireSRWLockExclusive(&srwLock);
WriteData();
ReleaseSRWLockExclusive(&srwLock);
```

SRW Lock é mais leve que CRITICAL_SECTION porque:
- Cabe em um único ponteiro (não aloca memória separada)
- Sem recursão — uma thread não pode adquirir a mesma SRW lock duas vezes
- Ideal para dados que são lidos com frequência e escritos raramente

---

## Condition Variables: padrão produtor/consumidor moderno

```cpp
CRITICAL_SECTION cs;
CONDITION_VARIABLE cv;
std::deque<int> fila;

InitializeCriticalSection(&cs);
InitializeConditionVariable(&cv);

// Thread produtora:
void Produce(int item) {
    EnterCriticalSection(&cs);
    fila.push_back(item);
    LeaveCriticalSection(&cs);
    WakeConditionVariable(&cv);  // acorda UM consumidor
    // WakeAllConditionVariable(&cv) para acordar TODOS
}

// Thread consumidora:
void Consume() {
    EnterCriticalSection(&cs);
    while (fila.empty()) {
        // Atomicamente: libera cs e bloqueia na cv
        SleepConditionVariableCS(&cv, &cs, INFINITE);
        // Quando acorda: cs está adquirida novamente
    }
    int item = fila.front();
    fila.pop_front();
    LeaveCriticalSection(&cs);
    Process(item);
}
```

---

## Waitable Timer: temporizador preciso

```cpp
HANDLE hTimer = CreateWaitableTimerW(nullptr, FALSE, nullptr); // auto-reset

// Disparar em 2 segundos, depois a cada 1 segundo:
LARGE_INTEGER dueTime;
dueTime.QuadPart = -20000000LL; // 2 segundos (unidades de 100ns negativo = relativo)

SetWaitableTimer(hTimer,
    &dueTime,
    1000,     // período em ms (0 = one-shot)
    nullptr,  // completion routine (ou nullptr)
    nullptr,
    FALSE);   // fRestore (wake from sleep?)

while (true) {
    WaitForSingleObject(hTimer, INFINITE);
    DoPeriodicWork();
}

CloseHandle(hTimer);
```

---

## Diagnóstico de deadlocks e waits

```
WinDbg — analisar deadlock:
kd> !locks        ← mostra CRITICAL_SECTIONs adquiridas
kd> !handle 0 f   ← todos os handles do processo
kd> !thread       ← estado de todas as threads (Waiting em quê?)

Para ver wait chain completa (ferramenta gráfica):
Resource Monitor → CPU → Analyze Wait Chain
→ mostra grafo de quem está esperando por quem
→ ciclos no grafo = deadlock
```

```cpp
// Prevenir deadlock: sempre adquirir locks na mesma ordem
// NUNCA faça:
// Thread A: AcquireA → AcquireB
// Thread B: AcquireB → AcquireA  ← deadlock garantido!

// SEMPRE: definir ordem global
// Thread A: AcquireA → AcquireB
// Thread B: AcquireA → AcquireB  ← correto
```

---

*Fim do Capítulo 4. Próximo: [Capítulo 5 — Gerenciamento de Memória](capitulo-05.md)*
