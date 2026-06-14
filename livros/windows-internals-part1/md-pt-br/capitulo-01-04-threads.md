# Capítulo 1 — Threads

> Livro didático baseado em *Windows Internals, 7th Edition*.
> Atualizado para Windows 11 com exemplos práticos em C++ moderno.
> Repositório: [ring0-study](https://github.com/cnthigu/ring0-study)

> Navegação: [Anterior](capitulo-01-03-processos-jobs.md) | [Índice](README.md) | [Próximo](capitulo-01-05-memoria-virtual.md)

---

## O que é uma thread?

Se um processo é o *container* de recursos, a **thread** é a *unidade de execução*. Um processo sem threads é inútil — não há código rodando. O Windows não agenda processos; ele agenda **threads**.

Pense assim: o processo define *o que* existe (espaço de memória, handles, identidade de segurança), e as threads definem *o que está acontecendo* (qual código está sendo executado agora).

Cada thread tem:

```
┌──────────────────────────────────────────────────────┐
│                       THREAD                         │
│                                                      │
│  ┌──────────────────────┐  ┌───────────────────────┐ │
│  │  Contexto do         │  │  Thread ID (TID)      │ │
│  │  processador         │  │  (no mesmo namespace  │ │
│  │  (registradores,     │  │   que PIDs)           │ │
│  │   ponteiro de pilha, │  └───────────────────────┘ │
│  │   RIP/EIP, RFLAGS)  │                             │
│  └──────────────────────┘  ┌───────────────────────┐ │
│                             │  Pilha em kernel mode  │ │
│  ┌──────────────────────┐  │  (separada da pilha   │ │
│  │  Pilha em user mode  │  │   em user mode)        │ │
│  │  (padrão: 1 MB)      │  └───────────────────────┘ │
│  └──────────────────────┘                             │
│                             ┌───────────────────────┐ │
│  ┌──────────────────────┐  │  TLS (Thread Local     │ │
│  │  Token de segurança  │  │  Storage) — opcional   │ │
│  │  (opcional — para    │  └───────────────────────┘ │
│  │   impersonation)     │                             │
│  └──────────────────────┘                             │
└──────────────────────────────────────────────────────┘
```

---

## O contexto do processador

O "contexto" de uma thread é o conjunto completo de registradores do processador no momento em que a thread foi interrompida. Isso inclui:

- **RIP** (x64): o instruction pointer — onde a thread estava executando
- **RSP** (x64): o stack pointer — o topo da pilha atual
- **RFLAGS**: flags de status (resultado de comparações, overflow, etc.)
- **Registradores de propósito geral**: RAX, RBX, RCX, RDX, RSI, RDI, R8-R15
- **Registradores XMM/YMM/ZMM**: para operações SIMD (vetoriais)
- **Registradores de segmento**: CS, DS, SS, ES, FS, GS (menos relevantes em 64-bit)

Quando o scheduler decide trocar de thread A para thread B, ele salva o contexto de A na estrutura `KTHREAD` no kernel e carrega o contexto de B de volta nos registradores do processador. Isso é um **context switch**. O processo é transparente para as threads — elas não sabem que foram pausadas.

O bloco de contexto que o Windows usa internamente é a estrutura `CONTEXT` (documentada no WDK). Você pode inspecioná-la com:

```
kd> dt nt!_CONTEXT
   +0x000 ContextFlags     : Uint4B
   +0x008 Dr0              : Uint8B
   ...
   +0x078 Rax              : Uint8B
   +0x080 Rcx              : Uint8B
   ...
   +0x0f8 Rip              : Uint8B
   +0x100 EFlags           : Uint4B
   ...
```

---

## Criando threads

```cpp
#include <windows.h>
#include <stdio.h>

// Função que a nova thread vai executar
DWORD WINAPI funcaoThread(LPVOID param) {
    int numero = (int)(intptr_t)param;
    printf("Thread %d rodando no TID %u\n", numero, GetCurrentThreadId());
    
    // Simula trabalho
    Sleep(1000 * numero);
    
    printf("Thread %d terminando\n", numero);
    return 0;
}

int main() {
    const int NUM_THREADS = 4;
    HANDLE handles[NUM_THREADS];
    
    for (int i = 0; i < NUM_THREADS; i++) {
        handles[i] = CreateThread(
            nullptr,                    // security attributes
            0,                          // stack size (0 = padrão: 1 MB)
            funcaoThread,               // função de entrada
            (LPVOID)(intptr_t)(i + 1),  // parâmetro passado para a função
            0,                          // flags (0 = começa rodando)
            nullptr                     // ponteiro para receber o TID
        );
    }
    
    // Aguarda todas as threads terminarem
    WaitForMultipleObjects(NUM_THREADS, handles, TRUE, INFINITE);
    
    for (int i = 0; i < NUM_THREADS; i++) {
        CloseHandle(handles[i]);
    }
    
    return 0;
}
```

> **Prefira `std::thread` ou thread pools em código de produção.** `CreateThread` é a API Win32 bruta. Em C++ moderno, `std::thread` e `std::async` são mais seguros. Para alta performance com muitas threads curtas, use o **Windows Thread Pool** (`CreateThreadpoolWork`, `SubmitThreadpoolWork`) — ele gerencia um pool de threads automaticamente, evitando o overhead de criar e destruir threads constantemente.

### Thread Local Storage (TLS)

TLS é um mecanismo para que cada thread tenha sua própria cópia de uma variável global — sem precisar de sincronização. Útil para variáveis de estado por-thread como `errno` da biblioteca C.

```cpp
// Declaração de variável TLS com keyword do compilador (mais simples)
__declspec(thread) int contadorLocal = 0;

DWORD WINAPI workerThread(LPVOID) {
    // Cada thread tem sua própria cópia de contadorLocal
    contadorLocal = GetCurrentThreadId();
    Sleep(100);
    // Imprime o valor que ETA thread setou — não há race condition
    printf("TID %u: contadorLocal = %d\n", GetCurrentThreadId(), contadorLocal);
    return 0;
}
```

Internamente, o compilador implementa `__declspec(thread)` usando slots no bloco TEB (Thread Environment Block) — uma estrutura por thread no espaço de endereços do processo.

---

## Duas pilhas por thread

Um detalhe importante: toda thread tem **duas pilhas separadas**:

1. **Pilha em user mode** (padrão 1 MB): usada quando a thread executa código de aplicação
2. **Pilha em kernel mode** (tipicamente 64 KB a 256 KB): alocada quando a thread entra no kernel via syscall

Quando sua thread chama `ReadFile`, ela entra no kernel (via syscall instruction). O processador troca o stack pointer para apontar para a pilha kernel da thread, executa o código do kernel, e ao retornar troca de volta para a pilha user mode. As duas pilhas coexistem — elas apenas ficam ativas em momentos diferentes.

Esta separação é fundamental para segurança: código em user mode não pode "ver" a pilha kernel. Mesmo que um exploit em user mode corrumpa a pilha user mode completamente, a pilha kernel (que pode conter dados sensíveis do OS) permanece íntegra.

---

## Fibers — threads cooperativas em user mode

Fibers são threads "levíssimas" gerenciadas completamente em user mode, invisíveis ao kernel. O kernel as vê como uma única thread normal.

```cpp
// Conceito de fibers (simplificado)
LPVOID fiber1 = CreateFiber(0, funcaoFiber1, nullptr);
LPVOID fiber2 = CreateFiber(0, funcaoFiber2, nullptr);

// Thread principal se converte em fiber
ConvertThreadToFiber(nullptr);

// Troca entre fibers explicitamente (sem envolver o kernel)
SwitchToFiber(fiber1);
// ... fiber1 roda até chamar SwitchToFiber(fiber2) ...
SwitchToFiber(fiber2);
```

> **Por que geralmente não usar fibers:**
> - O kernel não sabe que existem → APCs (Async Procedure Calls) e exceções podem ter comportamento inesperado
> - TLS é compartilhado entre fibers na mesma thread (FLS existe, mas não resolve tudo)
> - Fibers são cooperativas: se uma fiber fizer uma I/O bloqueante, toda a thread bloqueia
> - Em quase todos os casos, thread pools modernos fazem melhor o que fibers tentam fazer

Fibers existem principalmente por razões históricas e para portar código de outros sistemas operacionais. Evite-as em código novo.

---

## User-Mode Scheduling (UMS)

UMS threads (disponíveis apenas em Windows 64-bit) são uma tentativa de combinar as vantagens de fibers (troca de contexto em user mode) com a visibilidade do kernel para threads normais. UMS threads têm:

- Estado próprio no kernel (visíveis ao scheduler)
- Capacidade de fazer syscalls bloqueantes sem bloquear outras UMS threads
- Troca de contexto em user mode quando possível

Na prática, UMS foi introduzido no Windows 7 mas teve adoção mínima. Tecnologias modernas como corotinas C++20, `async/await` .NET e os thread pools do Windows resolvem os mesmos problemas de forma mais elegante. UMS é mencionado aqui porque você pode encontrá-lo no código do Windows e em material antigo.

---

## Threads compartilham o espaço de memória do processo

Todas as threads de um processo compartilham o **mesmo espaço de endereços virtual**. Isso é tanto poder quanto responsabilidade:

```cpp
// Variável global: TODAS as threads do processo veem a mesma
int contadorGlobal = 0;

DWORD WINAPI incrementa(LPVOID) {
    for (int i = 0; i < 1000000; i++) {
        contadorGlobal++;  // RACE CONDITION: não é atômico!
    }
    return 0;
}

// Correção: use operações atômicas
#include <atomic>
std::atomic<int> contadorAtomico = 0;

DWORD WINAPI incrementaSeguro(LPVOID) {
    for (int i = 0; i < 1000000; i++) {
        contadorAtomico.fetch_add(1, std::memory_order_relaxed);
    }
    return 0;
}
```

O scheduler pode interromper uma thread *no meio* de uma instrução de alto nível como `contadorGlobal++`. Em assembly x64, isso vira:
```asm
mov eax, [contadorGlobal]   ; lê o valor
inc eax                      ; incrementa
mov [contadorGlobal], eax   ; escreve de volta
```

Se o scheduler interrompe a thread entre a leitura e a escrita, e outra thread também lê/escreve o mesmo valor, você perde incrementos. Este é o race condition clássico — e é por isso que sincronização (mutexes, semáforos, spinlocks, variáveis atômicas) é necessária quando threads compartilham dados mutáveis.

---

## Thread vs. Process: quando usar cada um?

| Situação | Preferir |
|----------|---------|
| Tarefas paralelas que compartilham dados | Threads (mais eficiente) |
| Isolamento de falhas (um crash não afeta o resto) | Processos separados |
| Overhead de comunicação mínimo | Threads |
| Limites de recursos independentes | Processos + Jobs |
| Plugins de terceiros não confiáveis | Processos separados |
| Servidores web de alta performance | Thread pool (processos podem usar mais memória) |

O Chrome, por exemplo, usa uma combinação: o processo principal (browser) gerencia UI, e processos renderer separados hospedam abas. Se uma aba trava, não derruba o browser inteiro. Dentro de cada processo, múltiplas threads executam tarefas em paralelo.

---

*Próximo: [Memória Virtual e Modos do Processador](capitulo-01-05-memoria-virtual.md)*
