# Capítulo 3 — Job Objects em Profundidade

> Livro didático baseado em *Windows Internals, 7th Edition*.
> Atualizado para Windows 11 com exemplos práticos em C++ moderno.

> Navegação: [Anterior](capitulo-03-05-image-loader.md) | [Índice Cap.3](capitulo-03.md)

---

## Jobs: o que process trees do Unix não fazem

No Unix, processos formam uma árvore natural — `kill -9` num pai pode matar a subárvore inteira. No Windows, a "árvore" de processos é informacional apenas (o campo `ParentProcessId` em EPROCESS) e não tem semântica de controle.

O **Job Object** é o mecanismo do Windows para agrupar processos com controle real. Um Job:
- Agrupa um ou mais processos (e todos os processos que eles criarem, automaticamente)
- Aplica limites e políticas ao grupo como um todo
- Permite terminar todos os processos do grupo de uma vez
- Rastreia estatísticas agregadas (tempo de CPU total, memória total)

```
EJOB (kernel)
  ├── JobMembers (lista de EPROCESS no job)
  ├── BasicLimitInformation
  │     ├── MaximumWorkingSetSize
  │     ├── ActiveProcessLimit (máximo de processos)
  │     ├── PriorityClass
  │     └── LimitFlags
  ├── ExtendedLimitInformation
  │     ├── JobMemoryLimit (total de memória para todos os processos)
  │     └── IoInfo (contadores de I/O)
  └── SecurityLimitInformation
        ├── SecurityLimitFlags
        └── JobToken (token restrito para processos no job)
```

---

## Criando e configurando Jobs

```cpp
#include <windows.h>
#include <stdio.h>

int main() {
    // 1. Criar o job object
    HANDLE hJob = CreateJobObjectW(nullptr, L"MeuJob");
    
    // 2. Configurar limites estendidos
    JOBOBJECT_EXTENDED_LIMIT_INFORMATION limites = {};
    
    // Terminar todos os processos quando o handle do job fechar
    limites.BasicLimitInformation.LimitFlags |= JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
    
    // Limitar uso de memória total do job
    limites.JobMemoryLimit = 512ULL * 1024 * 1024; // 512 MB total
    limites.BasicLimitInformation.LimitFlags |= JOB_OBJECT_LIMIT_JOB_MEMORY;
    
    // Limitar número de processos ativos
    limites.BasicLimitInformation.ActiveProcessLimit = 10;
    limites.BasicLimitInformation.LimitFlags |= JOB_OBJECT_LIMIT_ACTIVE_PROCESS;
    
    // Limitar prioridade
    limites.BasicLimitInformation.PriorityClass = BELOW_NORMAL_PRIORITY_CLASS;
    limites.BasicLimitInformation.LimitFlags |= JOB_OBJECT_LIMIT_PRIORITY_CLASS;
    
    SetInformationJobObject(hJob, JobObjectExtendedLimitInformation,
                            &limites, sizeof(limites));
    
    // 3. Configurar limite de CPU (novo no Windows 8)
    JOBOBJECT_CPU_RATE_CONTROL_INFORMATION cpuCtrl = {};
    cpuCtrl.ControlFlags = JOB_OBJECT_CPU_RATE_CONTROL_ENABLE |
                           JOB_OBJECT_CPU_RATE_CONTROL_HARD_CAP;
    cpuCtrl.CpuRate = 2500; // 25% de uma CPU (em unidades de 1/100%)
    SetInformationJobObject(hJob, JobObjectCpuRateControlInformation,
                            &cpuCtrl, sizeof(cpuCtrl));
    
    // 4. Criar processo filho e atribuir ao job
    STARTUPINFOW si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    
    CreateProcessW(L"C:\\Windows\\System32\\cmd.exe",
                   nullptr, nullptr, nullptr, FALSE,
                   CREATE_SUSPENDED,  // suspende para atribuir ao job primeiro
                   nullptr, nullptr, &si, &pi);
    
    AssignProcessToJobObject(hJob, pi.hProcess);
    ResumeThread(pi.hThread);
    
    // 5. Aguardar todos os processos do job terminarem
    WaitForSingleObject(hJob, INFINITE);
    
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    CloseHandle(hJob); // dispara KILL_ON_JOB_CLOSE → mata todos os processos restantes
    
    return 0;
}
```

---

## Hierarquia de Jobs (Windows 8+)

Antes do Windows 8, um processo só podia pertencer a um job. A partir do Windows 8, jobs podem ser aninhados hierarquicamente:

```
Job Pai "Sandbox"
  ├── Job Filho "Renderer"  
  │     ├── chrome.exe renderer (PID 2100)
  │     └── chrome.exe GPU (PID 2200)
  └── Job Filho "Browser"
        └── chrome.exe main (PID 2000)
```

Limites mais restritivos prevalecem. Se o Job Pai limita memória total a 2GB e o Job Filho a 1GB, o filho nunca pode exceder 1GB (o limite mais restritivo).

Esta hierarquia é usada extensivamente por:
- **Windows Sandbox** (Microsoft Defender Application Guard)
- **Docker/Containers no Windows**: cada container é um job hierárquico
- **WSL 2**: o ambiente Linux fica num job separado

---

## Notificações de Job via IOCP

Jobs podem enviar notificações assíncronas via **I/O Completion Ports**:

```cpp
// Associar um IOCP ao job para notificações
HANDLE hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 1);

JOBOBJECT_ASSOCIATE_COMPLETION_PORT assoc;
assoc.CompletionKey = (PVOID)42; // chave para identificar este job
assoc.CompletionPort = hIOCP;
SetInformationJobObject(hJob, JobObjectAssociateCompletionPortInformation,
                        &assoc, sizeof(assoc));

// Thread que processa notificações:
DWORD msgCode;
ULONG_PTR key;
LPOVERLAPPED ov;

while (GetQueuedCompletionStatus(hIOCP, &msgCode, &key, &ov, INFINITE)) {
    switch (msgCode) {
    case JOB_OBJECT_MSG_NEW_PROCESS:
        wprintf(L"Novo processo no job: PID %llu\n", (ULONG64)ov);
        break;
    case JOB_OBJECT_MSG_EXIT_PROCESS:
        wprintf(L"Processo saiu: PID %llu\n", (ULONG64)ov);
        break;
    case JOB_OBJECT_MSG_ACTIVE_PROCESS_ZERO:
        wprintf(L"Todos os processos do job terminaram\n");
        break;
    case JOB_OBJECT_MSG_JOB_MEMORY_LIMIT:
        wprintf(L"Limite de memória atingido!\n");
        break;
    }
}
```

---

## Jobs e sandboxing no Windows 11

O Windows Sandbox usa jobs em combinação com:
- Virtualização Hyper-V (VM separada)
- AppContainers (restrições de namespace)
- Jobs (para controle de recursos e encerramento limpo)

Quando você fecha o Windows Sandbox, o sistema:
1. Fecha o handle do job
2. `JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE` dispara → todos os processos são terminados
3. A VM Hyper-V é destruída
4. Toda alteração feita no Sandbox se vai — não há persistência

Para criar uma sandbox simples com jobs em C++:

```cpp
JOBOBJECT_SECURITY_LIMIT_INFORMATION secLimites = {};
secLimites.SecurityLimitFlags = 
    JOB_OBJECT_SECURITY_RESTRICTED_TOKEN |        // token restrito
    JOB_OBJECT_SECURITY_NO_ADMIN |               // sem privilégios admin
    JOB_OBJECT_SECURITY_FILTER_TOKENS;           // filtro de token

JOBOBJECT_BASIC_UI_RESTRICTIONS uiRestrict = {};
uiRestrict.UIRestrictionsClass = 
    JOB_OBJECT_UILIMIT_EXITWINDOWS |    // não pode desligar o sistema
    JOB_OBJECT_UILIMIT_HANDLES |        // não pode usar handles de outras janelas
    JOB_OBJECT_UILIMIT_READCLIPBOARD |  // não pode ler clipboard
    JOB_OBJECT_UILIMIT_WRITECLIPBOARD | // não pode escrever no clipboard
    JOB_OBJECT_UILIMIT_SYSTEMPARAMETERS | // não pode mudar parâmetros do sistema
    JOB_OBJECT_UILIMIT_DISPLAYSETTINGS;  // não pode mudar resolução etc.

SetInformationJobObject(hJob, JobObjectSecurityLimitInformation, &secLimites, sizeof(secLimites));
SetInformationJobObject(hJob, JobObjectBasicUIRestrictions, &uiRestrict, sizeof(uiRestrict));
```

---

## Consultando estatísticas de um Job

```cpp
// Estatísticas de um job em execução
JOBOBJECT_BASIC_ACCOUNTING_INFORMATION stats;
QueryInformationJobObject(hJob, JobObjectBasicAccountingInformation,
                          &stats, sizeof(stats), nullptr);

wprintf(L"Total CPU user mode: %llu ns\n",   stats.TotalUserTime.QuadPart * 100);
wprintf(L"Total CPU kernel mode: %llu ns\n", stats.TotalKernelTime.QuadPart * 100);
wprintf(L"Total page faults: %u\n",          stats.TotalPageFaultCount);
wprintf(L"Total processos criados: %u\n",    stats.TotalProcesses);
wprintf(L"Processos ativos agora: %u\n",     stats.ActiveProcesses);
wprintf(L"Processos terminados por limite: %u\n", stats.TotalTerminatedProcesses);
```

---

## Job vs AppContainer vs Silo (Windows Server)

| Mecanismo | Nível de isolamento | Uso principal |
|-----------|--------------------|----|
| **Job Object** | Limites de recursos, encerramento em massa | CI/CD, sandboxes, contenção de recursos |
| **AppContainer** | Namespace de objetos, rede, FS restrito | Apps UWP, Edge |
| **Server Silo** | Container completo (registry, network, FS) | Docker/Windows Containers |

Silos (usados por containers Windows) são extensões de Jobs que isolam namespaces de kernel inteiros. Um processo num Silo vê seu próprio registro, seus próprios devices — uma visão filtrada do sistema.

---

*Fim do Capítulo 3. Próximo: [Capítulo 4 — Threads](capitulo-04.md)*
