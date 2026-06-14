# Capítulo 1 — Processos e Jobs

> Livro didático baseado em *Windows Internals, 7th Edition*.
> Atualizado para Windows 11 com exemplos práticos em C++ moderno.
> Repositório: [ring0-study](https://github.com/cnthigu/ring0-study)

> Navegação: [Anterior](capitulo-01-02-apis-windows.md) | [Índice](README.md) | [Próximo](capitulo-01-04-threads.md)

---

## Processo vs. Programa — a distinção fundamental

Um **programa** é estático: é um arquivo PE (Portable Executable) em disco com código e dados. Um **processo** é dinâmico: é a instância em execução desse programa, com todos os recursos que ele usa naquele momento.

Você pode ter dez instâncias do Notepad rodando. São dez processos diferentes — cada um com seu próprio espaço de memória, suas próprias handles abertas, seu próprio estado. Mas todos compartilham o mesmo arquivo `notepad.exe` em disco.

Esta distinção parece óbvia, mas tem implicações profundas em segurança: se um processo é comprometido, o invasor tem acesso aos recursos *daquele processo* — não necessariamente aos de outros processos rodando na mesma máquina (o isolamento de memória que o Windows implementa garante isso).

---

## O que um processo contém

No nível mais alto, um processo Windows é composto por:

```
┌─────────────────────────────────────────────────────────┐
│                      PROCESSO                           │
│                                                         │
│  ┌─────────────────────┐  ┌──────────────────────────┐ │
│  │  Espaço de endereços│  │  Tabela de handles       │ │
│  │  virtual privado    │  │  (arquivos, mutexes,     │ │
│  │  (128 TB em 64-bit) │  │   threads, processos...) │ │
│  └─────────────────────┘  └──────────────────────────┘ │
│                                                         │
│  ┌─────────────────────┐  ┌──────────────────────────┐ │
│  │  Token de segurança │  │  PID (Process ID)        │ │
│  │  (usuário, grupos,  │  │  + pelo menos 1 thread   │ │
│  │   privilégios)      │  │  de execução             │ │
│  └─────────────────────┘  └──────────────────────────┘ │
└─────────────────────────────────────────────────────────┘
```

### Espaço de endereços virtual privado

Cada processo acredita que tem todo o espaço de memória para si. Em sistemas 64-bit com Windows 11, esse espaço é de **128 TB** no lado do usuário. Na prática, a maioria dos processos usa alguns GB — o resto fica reservado mas não comprometido.

"Privado" é a palavra-chave: código em um processo não pode ler ou escrever na memória de outro processo diretamente. Para compartilhar dados, processos precisam usar mecanismos explícitos: memória compartilhada (file mapping objects), pipes, sockets, etc.

### Tabela de handles

Um handle é um identificador opaco para um objeto do kernel — arquivo, thread, processo, mutex, semáforo, evento. Cada processo tem sua própria tabela de handles. Handles de um processo não funcionam diretamente em outro (embora possam ser duplicados com `DuplicateHandle`).

```cpp
// Handle para um arquivo — o valor numérico (ex: 0x34) é específico deste processo
HANDLE hArquivo = CreateFileW(
    L"C:\\temp\\log.txt",
    GENERIC_WRITE,
    FILE_SHARE_READ,
    nullptr,
    CREATE_ALWAYS,
    FILE_ATTRIBUTE_NORMAL,
    nullptr
);

// Ao fechar, o kernel libera o objeto se não há mais referências
CloseHandle(hArquivo);
```

### Token de segurança

O token define *quem* é o processo: o usuário logado, os grupos aos quais pertence, e os privilégios disponíveis (ex: `SeDebugPrivilege` para debug de processos de outros usuários, `SeLoadDriverPrivilege` para carregar drivers).

O token é verificado pelo kernel em toda operação que acessa um objeto protegido. Se o token não tem permissão, a operação falha com `ACCESS_DENIED`. Veremos tokens em profundidade no Capítulo 7 (Segurança).

---

## Hierarquia de processos e re-parentificação

No Windows, todo processo tem um campo `ParentProcessId`. Mas essa informação **não é atualizada** se o processo pai morrer — é só o ID registrado no momento da criação. Isso cria situações interessantes:

```
cmd.exe (PID 1000)
  └── powershell.exe (PID 1100)  ← pai = 1000
        └── notepad.exe (PID 1200) ← pai = 1100

[powershell.exe (1100) é encerrado]

notepad.exe (1200) ainda existe, mas seu pai (1100) não existe mais.
No Process Explorer, notepad.exe aparece sem pai.
```

Além disso, alguns processos fazem **re-parentificação intencional**. Por exemplo, quando você eleva um processo via UAC, o consent.exe cria o novo processo em nome do usuário, mas informa ao Windows que o pai é o processo original — para que o Explorer mostre a árvore corretamente.

> **Experimento:** Abra o Process Explorer, vá em View → Show Lower Pane → Handles. Clique num processo e veja todas as handles abertas — arquivos, keys de registro, objetos de sincronização. Isso mostra exatamente quais recursos aquele processo está segurando.

### Como inspecionar processos com código

```cpp
#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>

void listarProcessos() {
    // Cria um "snapshot" (foto instantânea) de todos os processos
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap == INVALID_HANDLE_VALUE) return;

    PROCESSENTRY32W pe = { sizeof(pe) };
    
    if (Process32FirstW(hSnap, &pe)) {
        do {
            wprintf(L"PID: %6u | PPID: %6u | %s\n",
                pe.th32ProcessID,
                pe.th32ParentProcessID,
                pe.szExeFile);
        } while (Process32NextW(hSnap, &pe));
    }
    
    CloseHandle(hSnap);
}
```

Execute isso e você verá a mesma informação que o tlist.exe e o Process Explorer mostram — porque eles usam exatamente essas mesmas APIs (ou a equivalente NtQuerySystemInformation da Native API, que é mais eficiente).

---

## Criando processos

A forma oficial de criar um processo é `CreateProcess`:

```cpp
#include <windows.h>
#include <stdio.h>

int main() {
    STARTUPINFOW si = { sizeof(si) };
    PROCESS_INFORMATION pi = {};
    
    // Cria o Notepad como processo filho
    BOOL ok = CreateProcessW(
        L"C:\\Windows\\System32\\notepad.exe",  // nome do executável
        nullptr,          // linha de comando (nullptr = só o executável)
        nullptr,          // security attributes do processo
        nullptr,          // security attributes da thread principal
        FALSE,            // herdar handles do pai?
        0,                // flags de criação
        nullptr,          // ambiente (nullptr = herdar do pai)
        nullptr,          // diretório de trabalho (nullptr = herdar)
        &si,              // startup info
        &pi               // recebe PID, TID, handles
    );
    
    if (ok) {
        wprintf(L"Processo criado: PID=%u, TID=%u\n",
            pi.dwProcessId, pi.dwThreadId);
        
        // Aguarda o processo terminar (timeout = 5 segundos)
        WaitForSingleObject(pi.hProcess, 5000);
        
        // SEMPRE feche as handles retornadas
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
    } else {
        wprintf(L"Erro: %u\n", GetLastError());
    }
    
    return 0;
}
```

> **Por que precisamos fechar `pi.hProcess` e `pi.hThread`?**
>
> `CreateProcess` abre handles para o novo processo e sua thread principal, incrementando o contador de referências desses objetos no kernel. Se você não chamar `CloseHandle`, o objeto processo nunca é destruído pelo kernel — mesmo após o processo terminar. Isso é um handle leak. Ferramentas como Process Explorer mostram o count de handles de cada processo; um número crescente indefinidamente é sinal de problema.

### Flags de criação importantes

```cpp
// Flags mais usadas no parâmetro dwCreationFlags:
CREATE_NEW_CONSOLE        // abre janela de console própria
CREATE_SUSPENDED          // processo criado mas thread principal suspensa
                          // (útil para injeção de código antes da execução)
CREATE_NO_WINDOW          // sem janela (para processos de console em background)
DETACHED_PROCESS          // sem console associado
HIGH_PRIORITY_CLASS       // alta prioridade de CPU
CREATE_NEW_PROCESS_GROUP  // grupo de processos separado (para Ctrl+C handling)
```

---

## Jobs — grupos gerenciados de processos

O Windows não tem, nativamente, a árvore de processos hierárquica do Unix (onde `kill -9` num pai mata todos os filhos). Para preencher essa lacuna — e ir além — o Windows tem **Jobs**.

Um Job object permite agrupar um ou mais processos e aplicar **limites e controles** ao grupo como um todo:

```
Job Object: "MeuSandbox"
  ├── chrome.exe (PID 2000)
  │     ├── chrome.exe (PID 2100) [renderer]
  │     └── chrome.exe (PID 2200) [GPU process]
  └── notepad.exe (PID 3000)

Aplicar ao Job:
  - Limite de memória: 4 GB total
  - Limite de CPU: 50% de um core
  - Sem accesso à rede
  - Terminar todos ao fechar o job
```

O Chrome usa jobs extensivamente para seus processos de renderer — isolamento não é só questão de segurança, mas também de controle de recursos.

```cpp
#include <windows.h>
#include <stdio.h>

int main() {
    // Cria um job object
    HANDLE hJob = CreateJobObjectW(nullptr, L"MeuJob");
    
    // Configura limites: terminar todos os processos ao fechar o job
    JOBOBJECT_EXTENDED_LIMIT_INFORMATION limites = {};
    limites.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
    SetInformationJobObject(hJob, JobObjectExtendedLimitInformation,
                            &limites, sizeof(limites));
    
    // Cria o processo filho
    STARTUPINFOW si = { sizeof(si) };
    PROCESS_INFORMATION pi = {};
    CreateProcessW(L"C:\\Windows\\System32\\cmd.exe",
                   nullptr, nullptr, nullptr,
                   FALSE, CREATE_SUSPENDED, nullptr, nullptr, &si, &pi);
    
    // Associa o processo ao job ANTES de resumir
    AssignProcessToJobObject(hJob, pi.hProcess);
    
    // Agora sim, resume a thread principal
    ResumeThread(pi.hThread);
    
    // Quando hJob for fechado (CloseHandle ou quando o processo pai sair),
    // cmd.exe e todos os filhos que ele criou serão encerrados.
    
    printf("Pressione Enter para terminar o job e encerrar cmd.exe...\n");
    getchar();
    
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    CloseHandle(hJob);   // Isso dispara JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE
    
    return 0;
}
```

> **Por que `CREATE_SUSPENDED`?** Você precisa associar o processo ao job *antes* que ele crie processos filhos, senão os filhos ficam fora do job. Criar suspenso e depois `AssignProcessToJobObject` + `ResumeThread` garante a ordem correta.

### Jobs e o Windows Sandbox

A partir do Windows 10, o **Windows Sandbox** e o **Windows Defender Application Guard** usam jobs em combinação com containers de virtualização (baseados em Hyper-V) para isolar aplicações. Mesmo browsers modernos usam jobs para garantir que processos renderer não escapem do controle do processo principal.

---

## Visualizando processos e jobs

O **Process Explorer** do Sysinternals é sua ferramenta primária para investigar processos:

- Processos coloridos em **rosa**: hospedam serviços do Windows
- Processos coloridos em **azul**: processos do seu usuário
- Processos em **roxo**: processos protegidos (PPL — Protected Process Light)

Para ver jobs: vá em View → Lower Pane → Job, ou abra as propriedades de um processo e veja a aba Job.

A aba **TCP/IP** do Process Explorer mostra todas as conexões de rede por processo — útil para descobrir que processo está fazendo conexões inesperadas.

---

*Próximo: [Threads](capitulo-01-04-threads.md)*
