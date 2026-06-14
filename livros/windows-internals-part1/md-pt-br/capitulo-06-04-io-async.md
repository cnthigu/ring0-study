# Capítulo 6 — I/O Assíncrono e I/O Completion Ports

> Livro didático baseado em *Windows Internals, 7th Edition*.
> Atualizado para Windows 11 com exemplos práticos em C++ moderno.

> Navegação: [Anterior](capitulo-06-03-pnp-power.md) | [Índice Cap.6](capitulo-06.md)

---

## Modos de I/O: síncrono vs. assíncrono

O Windows suporta múltiplos modelos de I/O, cada um com trade-offs diferentes:

```
I/O Síncrono (padrão):
  ReadFile() → thread bloqueia até I/O completar → retorna
  ✓ Simples de programar
  ✗ Thread fica bloqueada — não pode fazer outra coisa

I/O Assíncrono com OVERLAPPED:
  ReadFile() com OVERLAPPED → retorna imediatamente (possivelmente FALSE + ERROR_IO_PENDING)
  Thread continua trabalhando
  GetOverlappedResult() ou WaitForSingleObject() para esperar conclusão
  ✓ Thread pode fazer outro trabalho enquanto espera
  ✗ Mais complexo

I/O Assíncrono com Completion Routines (APC-based):
  ReadFileEx() com callback → I/O Manager chama callback via APC quando completa
  ✓ Modelo de callback natural
  ✗ Thread precisa estar em estado alertável

I/O Completion Ports (IOCP):
  N threads aguardando port → I/O Manager despacha completions automaticamente
  ✓ Escalabilidade máxima — servidores de alto desempenho
  ✗ Modelo mais complexo
```

---

## OVERLAPPED: o struct base do I/O assíncrono

```cpp
#include <windows.h>

// Abrir arquivo para I/O assíncrono (obrigatório: FILE_FLAG_OVERLAPPED)
HANDLE hFile = CreateFileW(
    L"arquivo.bin",
    GENERIC_READ,
    FILE_SHARE_READ,
    nullptr,
    OPEN_EXISTING,
    FILE_FLAG_OVERLAPPED,  // ESSENCIAL para I/O assíncrono
    nullptr);

// OVERLAPPED: struct que descreve cada operação assíncrona
OVERLAPPED ov = {};
ov.Offset     = 0;          // byte offset no arquivo (parte baixa)
ov.OffsetHigh = 0;          // byte offset (parte alta, para arquivos > 4 GB)
// ov.hEvent pode ser um event handle para aguardar conclusão

char buffer[4096] = {};
DWORD bytesRead;

// Iniciar leitura assíncrona:
BOOL result = ReadFile(hFile, buffer, sizeof(buffer), &bytesRead, &ov);
if (!result && GetLastError() == ERROR_IO_PENDING) {
    // I/O em andamento — podemos fazer outra coisa aqui
    DoOtherWork();
    
    // Aguardar conclusão:
    GetOverlappedResult(hFile, &ov, &bytesRead, TRUE); // TRUE = bloquear até completar
    printf("Lidos: %u bytes\n", bytesRead);
}

CloseHandle(hFile);
```

---

## I/O Completion Ports: escalabilidade para servidores

O **IOCP (I/O Completion Port)** é o mecanismo mais eficiente para servidores que lidam com muitas conexões simultâneas. Em vez de uma thread por conexão, o IOCP usa um pool fixo de threads que processa completions conforme chegam:

```
Arquitetura IOCP:

  [Handle 1 (socket, arquivo)]  ]
  [Handle 2 (socket, arquivo)]  } associados ao IOCP
  [Handle N (socket, arquivo)]  ]
           ↓ I/O completa
  ┌────────────────────────────┐
  │  IOCP Completion Queue     │ ← kernel enfileira completions aqui
  └────────────────────────────┘
           ↓ GetQueuedCompletionStatus
  [Thread 1] ← pega completion e processa
  [Thread 2] ← pega completion e processa
  [Thread 3] ← pega completion e processa
  (pool fixo de threads — geralmente = número de CPUs)
```

```cpp
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

// 1. Criar o IOCP:
HANDLE hIOCP = CreateIoCompletionPort(
    INVALID_HANDLE_VALUE,  // não associar a nenhum handle ainda
    nullptr, 0,
    0);  // 0 = número de threads = número de CPUs lógicas

// 2. Associar sockets ao IOCP:
struct PerHandleContext {
    SOCKET socket;
    // ... dados por conexão ...
};

auto AssociarAoIOCP = [&](SOCKET sock) {
    auto ctx = new PerHandleContext{ sock };
    CreateIoCompletionPort((HANDLE)sock, hIOCP,
        (ULONG_PTR)ctx,  // completion key = ponteiro para contexto
        0);
};

// 3. Pool de threads worker:
struct PerIoContext {
    OVERLAPPED ov;           // DEVE ser o primeiro campo
    char buffer[8192];
    WSABUF wsaBuf;
    DWORD flags;
};

auto WorkerThread = [&]() {
    while (true) {
        DWORD bytesTransferred;
        PerHandleContext* handleCtx;
        PerIoContext* ioCtx;
        
        BOOL ok = GetQueuedCompletionStatus(
            hIOCP,
            &bytesTransferred,
            (PULONG_PTR)&handleCtx,
            (LPOVERLAPPED*)&ioCtx,
            INFINITE);
        
        if (!ok || bytesTransferred == 0) {
            // Conexão fechada ou erro
            closesocket(handleCtx->socket);
            delete handleCtx;
            delete ioCtx;
            continue;
        }
        
        // Processar os dados recebidos:
        ProcessarDados(ioCtx->buffer, bytesTransferred);
        
        // Iniciar próxima leitura assíncrona:
        auto novoCtx = new PerIoContext{};
        novoCtx->wsaBuf.buf = novoCtx->buffer;
        novoCtx->wsaBuf.len = sizeof(novoCtx->buffer);
        WSARecv(handleCtx->socket, &novoCtx->wsaBuf, 1,
                nullptr, &novoCtx->flags,
                &novoCtx->ov, nullptr);
    }
};

// Lançar N threads worker (tipicamente número de CPUs):
SYSTEM_INFO si;
GetSystemInfo(&si);
for (DWORD i = 0; i < si.dwNumberOfProcessors; i++) {
    std::thread(WorkerThread).detach();
}
```

---

## PostQueuedCompletionStatus: trabalho customizado no IOCP

O IOCP pode receber não só completions de I/O, mas qualquer notificação customizada:

```cpp
// Enfileirar uma "fake completion" para comunicar com as worker threads:
struct MeuPacoteDeTrabalho {
    OVERLAPPED ov;     // obrigatório
    TipoDeTrabalho tipo;
    void* dados;
};

auto pacote = new MeuPacoteDeTrabalho{ {}, TRABALHO_CUSTOMIZADO, minhaInfo };

// Enviar para o IOCP:
PostQueuedCompletionStatus(hIOCP,
    0,                // bytes transferidos (customizável)
    (ULONG_PTR)ctx,   // completion key
    &pacote->ov);     // overlapped

// As worker threads receberão via GetQueuedCompletionStatus
// e podem distinguir por: completion key, bytes, ou campos no OVERLAPPED
```

Esse padrão é usado para desligar as worker threads graciosamente:

```cpp
// Sinal de shutdown para N threads:
for (DWORD i = 0; i < nThreads; i++) {
    PostQueuedCompletionStatus(hIOCP, 0, SHUTDOWN_KEY, nullptr);
}
// Cada thread, ao receber completion key == SHUTDOWN_KEY, encerra
```

---

## Cancelamento de I/O

```cpp
// Cancelar TODAS as operações de I/O pendentes de uma thread no arquivo:
CancelIo(hFile);  // cancela apenas as iniciadas pela thread atual

// Cancelar TODAS as operações (de qualquer thread) em um arquivo:
CancelIoEx(hFile, nullptr);  // nullptr = cancelar todas

// Cancelar uma operação específica (pelo ponteiro do OVERLAPPED):
CancelIoEx(hFile, &ov);

// Verificar se foi cancelada:
DWORD err = GetLastError(); // após GetOverlappedResult falhar
if (err == ERROR_OPERATION_ABORTED) {
    // Operação foi cancelada explicitamente
}
```

---

## Comparando modelos de I/O: qual usar?

```
Aplicação simples, leitura de arquivo único:
  → I/O síncrono (ReadFile sem OVERLAPPED)
  → Simplicidade máxima, sem overhead

Aplicação com múltiplas operações em paralelo, mesma thread:
  → OVERLAPPED com WaitForMultipleObjects
  → Thread gerencia ~63 operações simultâneas

Servidor com centenas/milhares de conexões:
  → IOCP com thread pool fixo
  → Padrão de facto para servidores Windows de alto desempenho
  → HTTP.sys (servidor web do kernel) usa IOCP internamente
  → ASP.NET, IIS, SQL Server usam IOCP

Processamento de I/O com pipeline/transformações:
  → ReadFileEx com APC completion routine
  → Modelo de callback natural para pipelines
```

---

## HTTP.sys: servidor web no kernel

O Windows tem um servidor HTTP **no kernel** (`http.sys`), que é a base do IIS e do Windows Communication Foundation:

```
http.sys:
  → Recebe conexões TCP na porta 80/443 no kernel
  → Parseia headers HTTP
  → Enfileira requests para o processo servidor (user mode) via IOCP
  → Kernel mode → sem overhead de transição user/kernel por request
  → TLS offload e cache de respostas no kernel

URL reservations (netsh http):
  netsh http add urlacl url=http://+:80/minhaapi/ user=DOMAIN\ServiceAccount
  → Permite que processo user-mode sem admin escute em portas privilegiadas via http.sys
```

---

*Fim do Capítulo 6. Próximo: [Capítulo 7 — Segurança](capitulo-07.md)*
