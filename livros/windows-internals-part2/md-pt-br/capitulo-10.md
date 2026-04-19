# Capitulo 10 - Gerenciamento, Diagnostico e Rastreamento

> *Traducao do livro Windows Internals, Part 2, Seventh Edition*
> *Andrea Allievi, Alex Ionescu, Mark E. Russinovich, David A. Solomon*
> *Microsoft Press, 2022*

---

Este capitulo detalha os mecanismos fundamentais implementados no sistema operacional para gerenciamento, configuracao e diagnostico. Em particular, sao apresentados o registro do Windows, os servicos do Windows, o WMI e o agendamento de tarefas, juntamente com servicos de diagnostico como o Event Tracing for Windows (ETW) e o DTrace.

---

## O Registro

O **registro do Windows** e um banco de dados hierarquico que armazena informacoes de configuracao de baixo nivel para o sistema operacional e para as aplicacoes que optam por usa-lo.

### Visualizando e alterando o registro

As principais ferramentas para trabalhar com o registro sao:

- **Regedit.exe:** Editor de registro grafico incluido no Windows.
- **Reg.exe:** Ferramenta de linha de comando para gerenciar o registro.
- **PowerShell:** Cmdlets como `Get-ItemProperty` e `Set-ItemProperty`.
- **API de programacao:** `RegOpenKeyExW`, `RegQueryValueExW`, `RegSetValueExW`, etc.
- **WMI:** Classe `StdRegProv` para acesso remoto ao registro.

### Uso do registro

O registro e usado por:

- O kernel e os drivers para armazenar configuracoes.
- Os servicos do Windows para armazenar configuracoes e estado.
- Aplicativos para armazenar preferencias e dados de instalacao.
- Componentes de seguranca (politicas, ACLs, etc.).
- O sistema de plug-and-play para persistir informacoes de dispositivos.

### Tipos de dados do registro

| Tipo | Descricao |
|------|-----------|
| `REG_SZ` | String Unicode terminada em null |
| `REG_EXPAND_SZ` | String com variaveis de ambiente expandiveis |
| `REG_MULTI_SZ` | Multiplas strings separadas por null |
| `REG_DWORD` | Inteiro de 32 bits |
| `REG_QWORD` | Inteiro de 64 bits |
| `REG_BINARY` | Dados binarios brutos |
| `REG_NONE` | Sem tipo definido |

### Estrutura logica do registro

O registro e organizado em uma estrutura hierarquica de **chaves** (analogas a pastas) e **valores** (analogos a arquivos). As chaves de nivel raiz sao:

| Hive raiz | Descricao |
|-----------|-----------|
| `HKEY_LOCAL_MACHINE (HKLM)` | Configuracoes de todo o sistema |
| `HKEY_CURRENT_USER (HKCU)` | Configuracoes do usuario atual |
| `HKEY_USERS (HKU)` | Perfis de todos os usuarios |
| `HKEY_CLASSES_ROOT (HKCR)` | Associacoes de arquivo e COM |
| `HKEY_CURRENT_CONFIG (HKCC)` | Perfil de hardware atual |

Internamente, o registro e organizado em **hives** - arquivos no disco que armazenam partes da hierarquia do registro. Os hives mais importantes sao:

| Hive | Arquivo no disco | Conteudo |
|------|-----------------|---------|
| `HKLM\SYSTEM` | `%SystemRoot%\System32\config\SYSTEM` | Configuracoes do sistema de boot |
| `HKLM\SOFTWARE` | `%SystemRoot%\System32\config\SOFTWARE` | Configuracoes de software |
| `HKLM\SAM` | `%SystemRoot%\System32\config\SAM` | Banco de dados de contas locais |
| `HKLM\SECURITY` | `%SystemRoot%\System32\config\SECURITY` | Politicas de seguranca |
| `HKCU` | `%USERPROFILE%\NTUSER.DAT` | Configuracoes do usuario |
| `HKU\.DEFAULT` | `%SystemRoot%\System32\config\DEFAULT` | Perfil padrao |

> **Rode no Visual Studio**
> Este exemplo le o nome do produto Windows do registro e imprime na tela.
>
> ```cpp
> #include <windows.h>
> #include <stdio.h>
>
> int main() {
>     HKEY hKey;
>     LONG res = RegOpenKeyExW(HKEY_LOCAL_MACHINE,
>         L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",
>         0, KEY_READ, &hKey);
>
>     if (res == ERROR_SUCCESS) {
>         WCHAR nome[256];
>         DWORD tamanho = sizeof(nome);
>         DWORD tipo;
>
>         res = RegQueryValueExW(hKey, L"ProductName",
>             NULL, &tipo, (BYTE*)nome, &tamanho);
>
>         if (res == ERROR_SUCCESS)
>             wprintf(L"Produto: %s\n", nome);
>
>         RegCloseKey(hKey);
>     }
>     return 0;
> }
> ```

### Application Hives

O Windows 8 introduziu os **Application Hives** - hives de registro privados para aplicativos empacotados (UWP). Esses hives sao isolados do registro do sistema e de outros aplicativos, fornecendo uma view privada do registro para cada aplicativo.

Os application hives sao armazenados por padrao em:
`%LocalAppData%\Packages\<PackageFullName>\SystemAppData\Helium\`

### Registro Transacional (TxR)

O **Transactional Registry (TxR)** permite que operacoes de registro sejam agrupadas em transacoes atomicas. Isso significa que um grupo de operacoes de registro pode ser confirmado ou revertido em conjunto, garantindo a consistencia dos dados.

TxR usa o **Kernel Transaction Manager (KTM)** para coordenar transacoes que abrangem tanto o sistema de arquivos (via TxF) quanto o registro.

```cpp
// Exemplo de uso de TxR
HANDLE hTransaction = CreateTransaction(NULL, NULL, 0, 0, 0, 0, NULL);
HKEY hKey;
RegCreateKeyTransactedW(HKEY_CURRENT_USER, L"Software\\Teste",
    0, NULL, 0, KEY_READ | KEY_WRITE, NULL, &hKey, NULL, hTransaction, NULL);
// ... operacoes de registro ...
CommitTransaction(hTransaction); // ou RollbackTransaction
CloseHandle(hTransaction);
```

### Monitoramento de atividade do registro

O Process Monitor da Sysinternals e a ferramenta padrao para monitorar acessos ao registro em tempo real. Ele captura cada operacao de registro (abertura, leitura, escrita, exclusao) junto com o processo que fez a operacao, o resultado e o stack trace.

Internamente, o Process Monitor usa o **ETW** (Event Tracing for Windows) para capturar eventos de registro.

### Internos do registro

Internamente, o registro e armazenado em um formato binario especifico:

- **Cell:** A unidade basica de alocacao dentro de um hive. Cada tipo de dado (chave, valor, string, etc.) ocupa um ou mais cells.
- **Base Block:** O cabecalho de um arquivo de hive, contendo assinatura, versao, checksums e informacoes de recuperacao.
- **Root Cell:** O cell que representa a chave raiz de um hive.
- **Key Node (NK Cell):** Representa uma chave do registro. Contem ponteiros para subchaves, valores, dados de seguranca e o nome da chave.
- **Value (VK Cell):** Representa um valor do registro. Contem o nome do valor, tipo e dados.

O formato usa offsets relativos ao inicio do hive em vez de ponteiros absolutos, o que facilita o carregamento do hive em diferentes enderecos de memoria.

### Reorganizacao do hive

Com o tempo, as operacoes de registro criam fragmentacao nos arquivos de hive (pois cells sao alocadas e liberadas). O Windows desfragmenta periodicamente os hives do registro em um processo chamado **hive compaction** para manter o desempenho.

### Namespace do registro e operacoes

O Object Manager do Windows expoe o namespace do registro como um namespace de objetos. As chaves raiz como `HKEY_LOCAL_MACHINE` sao na verdade pseudonomes que mapeiam para objetos no namespace:

- `HKLM` -> `\Registry\Machine`
- `HKCU` -> `\Registry\User\<SID>`
- `HKU` -> `\Registry\User`

### Armazenamento estavel

O Windows garante a durabilidade do registro usando um mecanismo chamado **log de hive**. Cada hive tem um arquivo de log associado (`.LOG`, `.LOG1`, `.LOG2`) onde as mudancas sao escritas antes de serem aplicadas ao arquivo de hive principal. Se o sistema travar durante uma escrita, as mudancas podem ser reaplicadas a partir do log na proxima inicializacao.

### Filtragem do registro

O Windows suporta **filtros do registro** - um mecanismo pelo qual drivers podem interceptar e potencialmente modificar operacoes do registro. Os filtros de registro sao usados por:

- Software de seguranca (antivirus, EDR) para monitorar acessos suspeitos.
- Software de auditoria.
- O proprio sistema para implementar virtualizacao e redirecionamento.

### Virtualizacao do registro

Para compatibilidade com aplicativos mais antigos que tentam escrever em partes do registro que requerem privilégios administrativos, o Windows implementa **virtualizacao do registro**. Quando um processo sem privilegios tenta escrever em `HKLM\SOFTWARE`, a escrita e redirecionada para `HKCU\VirtualStore\Machine\Software` de forma transparente.

### Otimizacoes do registro

O Windows implementa varias otimizacoes para melhorar o desempenho do registro:

- **Cache de chaves:** A maioria das chaves frequentemente acessadas sao mantidas em cache na memoria.
- **Batch writes:** Escritas no registro sao agregadas antes de serem persistidas no disco.
- **Map-based access:** O hive e mapeado na memoria do kernel usando a interface do gerenciador de cache para I/O eficiente.

---

## Servicos do Windows

Os **servicos do Windows** sao processos de longa duracao que realizam funcoes especificas do sistema e que podem ser iniciados automaticamente durante o boot, sem interacao do usuario.

### Aplicativos de servico

Um aplicativo de servico e um executavel Win32 que implementa a interface de servico especificada pela API de servico do Windows. Um servico tipico:

1. Registra seu ponto de entrada principal com o SCM via `StartServiceCtrlDispatcher`.
2. Implementa uma funcao de controle de servico via `RegisterServiceCtrlHandlerEx`.
3. Notifica o SCM sobre mudancas de estado via `SetServiceStatus`.

A estrutura basica de um servico e:

```cpp
#include <windows.h>
#include <stdio.h>

SERVICE_STATUS_HANDLE g_hStatusHandle;
SERVICE_STATUS        g_serviceStatus;

VOID WINAPI ServiceCtrlHandler(DWORD dwControl) {
    switch (dwControl) {
    case SERVICE_CONTROL_STOP:
        g_serviceStatus.dwCurrentState = SERVICE_STOP_PENDING;
        SetServiceStatus(g_hStatusHandle, &g_serviceStatus);
        // ... sinalizar para parar ...
        break;
    }
}

VOID WINAPI ServiceMain(DWORD argc, LPTSTR* argv) {
    g_hStatusHandle = RegisterServiceCtrlHandlerW(L"MeuServico", ServiceCtrlHandler);

    g_serviceStatus.dwServiceType             = SERVICE_WIN32_OWN_PROCESS;
    g_serviceStatus.dwCurrentState            = SERVICE_RUNNING;
    g_serviceStatus.dwControlsAccepted        = SERVICE_ACCEPT_STOP;
    g_serviceStatus.dwWin32ExitCode           = NO_ERROR;
    g_serviceStatus.dwCheckPoint              = 0;
    g_serviceStatus.dwWaitHint               = 0;
    SetServiceStatus(g_hStatusHandle, &g_serviceStatus);

    // ... logica principal do servico ...

    g_serviceStatus.dwCurrentState = SERVICE_STOPPED;
    SetServiceStatus(g_hStatusHandle, &g_serviceStatus);
}

int main() {
    SERVICE_TABLE_ENTRYW serviceTable[] = {
        { L"MeuServico", ServiceMain },
        { NULL, NULL }
    };
    StartServiceCtrlDispatcherW(serviceTable);
    return 0;
}
```

### Contas de servico

Os servicos rodam no contexto de uma conta de usuario especifica. As contas de servico mais comuns sao:

| Conta | Descricao |
|-------|-----------|
| `LocalSystem` | Conta mais privilegiada - acesso total ao sistema local e na rede como computador$ |
| `NetworkService` | Privilegios de usuario limitados, acessa a rede como computador$ |
| `LocalService` | Privilegios de usuario limitados, acessa a rede anonimamente |
| Conta gerenciada de servico (gMSA) | Conta de dominio com gerenciamento automatico de senha |
| Conta de usuario especifica | Uma conta de usuario normal ou de dominio |

A partir do Windows Vista, o Windows introduziu o conceito de **servico virtual account** - contas criadas automaticamente para cada servico, com privilegios minimos e nome no formato `NT Service\<ServiceName>`.

### O Service Control Manager (SCM)

O **Service Control Manager (SCM)** e o componente do sistema que gerencia os servicos do Windows. O SCM:

- Mantem o banco de dados de servicos instalados no registro (`HKLM\SYSTEM\CurrentControlSet\Services`).
- Inicia servicos no boot (conforme sua configuracao de startup).
- Responde a comandos de controle (iniciar, parar, pausar).
- Monitora e responde a falhas de servico.

O SCM roda no processo `services.exe`.

#### Tipos de inicializacao de servico

| Tipo | Valor | Descricao |
|------|-------|-----------|
| Boot | 0 | Iniciado pelo loader durante o boot (antes do kernel estar totalmente iniciado) |
| System | 1 | Iniciado pelo kernel durante a fase de inicializacao do sistema |
| Auto | 2 | Iniciado automaticamente pelo SCM quando o sistema inicia |
| Demand | 3 | Iniciado manualmente ou por outro servico |
| Disabled | 4 | Nao pode ser iniciado |

> **Rode no Visual Studio**
> Este exemplo enumera todos os servicos em execucao e imprime seus nomes.
>
> ```cpp
> #include <windows.h>
> #include <winsvc.h>
> #include <stdio.h>
>
> int main() {
>     SC_HANDLE hSCM = OpenSCManagerW(NULL, NULL, SC_MANAGER_ENUMERATE_SERVICE);
>     if (!hSCM) { printf("Erro: %lu\n", GetLastError()); return 1; }
>
>     DWORD bytesNecessarios = 0, numServicos = 0, resumeHandle = 0;
>     EnumServicesStatusExW(hSCM, SC_ENUM_PROCESS_INFO, SERVICE_WIN32,
>         SERVICE_ACTIVE, NULL, 0, &bytesNecessarios, &numServicos, &resumeHandle, NULL);
>
>     BYTE* buffer = (BYTE*)malloc(bytesNecessarios);
>     EnumServicesStatusExW(hSCM, SC_ENUM_PROCESS_INFO, SERVICE_WIN32,
>         SERVICE_ACTIVE, buffer, bytesNecessarios,
>         &bytesNecessarios, &numServicos, &resumeHandle, NULL);
>
>     ENUM_SERVICE_STATUS_PROCESSW* servicos = (ENUM_SERVICE_STATUS_PROCESSW*)buffer;
>     printf("Servicos em execucao (%lu):\n", numServicos);
>     for (DWORD i = 0; i < numServicos; i++)
>         wprintf(L"  [%5lu] %s\n",
>             servicos[i].ServiceStatusProcess.dwProcessId,
>             servicos[i].lpServiceName);
>
>     free(buffer);
>     CloseServiceHandle(hSCM);
>     return 0;
> }
> ```

### Servicos com inicio automatico

Durante o boot, o SCM le o banco de dados de servicos e inicia os servicos com tipo de inicio `Auto`. O processo e dividido em grupos de servico definidos em:

`HKLM\SYSTEM\CurrentControlSet\Control\ServiceGroupOrder`

Servicos dentro do mesmo grupo podem depender uns dos outros - o SCM honra essas dependencias ao determinar a ordem de inicializacao.

### Servicos com inicio automatico atrasado

Para melhorar o tempo de boot, o Windows suporta **delayed auto-start** - servicos que sao iniciados automaticamente, mas somente apos o desktop do usuario ter sido carregado e o sistema estar em estado estavel. Esses servicos sao marcados com `DelayedAutoStart = 1` em sua chave de registro.

### Servicos com inicio acionado por evento (triggered-start)

O Windows 7 introduziu **triggered-start services** - servicos que sao iniciados automaticamente em resposta a eventos especificos, sem necessidade de rodar permanentemente. Isso permite melhor consumo de recursos.

Os triggers suportados incluem:

- Presenca de um dispositivo (baseado em Device Interface Class GUID).
- Chegada de um pacote de rede especifico.
- Entrada/saida no dominio.
- Mudanca de politica de grupo.
- Evento ETW especifico.
- Abertura de uma porta de firewall.

### Erros de inicializacao e "Last Known Good"

Quando um servico critico falha ao iniciar, o Windows pode usar a configuracao **Last Known Good** para reverter para um conjunto de configuracoes anterior que funcionou com sucesso.

O Windows considera um boot bem-sucedido quando um usuario faz login com sucesso. Ate esse ponto, as mudancas de configuracao do registro nao sao consideradas "aceitas" e o sistema pode reverter para a configuracao anterior se algo falhar.

### Falhas de servico

O SCM pode ser configurado para responder automaticamente a falhas de servico. As acoes possiveis sao:

- **Nenhuma:** O servico nao e reiniciado.
- **Reiniciar o servico:** O SCM tenta reiniciar o servico.
- **Executar um programa:** O SCM executa um programa especificado.
- **Reiniciar o computador:** O sistema e reiniciado.

Essas acoes podem ser configuradas para as primeira, segunda e subsequentes falhas, com um intervalo configuravel.

### Processos de servico compartilhados (SvcHost)

Para economizar recursos, muitos servicos do Windows rodam no mesmo processo hospedeiro - o `svchost.exe`. O `svchost.exe` pode hospedar multiplos servicos, cada um implementado como uma DLL.

Os grupos de servicos hospedados juntos sao configurados em:

`HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Svchost`

Cada entrada nessa chave define um grupo de servicos que compartilham um processo `svchost.exe`.

### Tags de servico

Para ajudar a identificar qual DLL em um `svchost.exe` compartilhado e responsavel por que atividade, o Windows usa **service tags** - um valor numerico unico atribuido a cada thread de servico dentro de um processo compartilhado.

Ferramentas como o Process Explorer mostram service tags para ajudar a diagnosticar problemas.

---

## Agendamento de tarefas e UBPM

O **Agendador de Tarefas** (Task Scheduler) e um servico do Windows que permite agendar a execucao de programas em horarios especificos, em resposta a eventos ou em intervalos regulares.

### O Agendador de Tarefas

O Agendador de Tarefas e implementado pelo servico `Schedule` (que roda em `svchost.exe`) e expoe uma interface COM para criar e gerenciar tarefas. As tarefas sao armazenadas em:

- Banco de dados interno: `%SystemRoot%\System32\Tasks`
- Registro: `HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Schedule\TaskCache`

Os **gatilhos** (triggers) de uma tarefa definem quando ela deve rodar:

| Gatilho | Descricao |
|---------|-----------|
| Horario | Em um horario especifico (uma vez, diariamente, semanalmente, mensalmente) |
| Inicializacao | Quando o sistema inicia |
| Login | Quando um usuario faz login |
| Evento | Em resposta a um evento do log de eventos |
| Inatividade | Quando o sistema fica inativo por um periodo especificado |
| Conexao de sessao | Quando um usuario se conecta a uma sessao |
| Criacao de registro | Quando uma chave especifica e criada no registro |

### Unified Background Process Manager (UBPM)

O **UBPM** e um componente introduzido no Windows 8 que gerencia processos e tarefas em segundo plano de forma mais eficiente, considerando restricoes de energia e responsividade do sistema.

O UBPM e o "motor" real por tras do Agendador de Tarefas em sistemas modernos, otimizando quando e como as tarefas em segundo plano executam com base em:

- Estado da bateria e modo de energia.
- Uso atual de CPU e memoria.
- Presenca do usuario no sistema.
- Conectividade de rede.

---

## Windows Management Instrumentation (WMI)

O **WMI** e a implementacao da Microsoft do **Web-Based Enterprise Management (WBEM)**, um padrao de gerenciamento de sistemas. O WMI fornece uma interface unificada para consultar e gerenciar informacoes sobre o sistema e os aplicativos instalados.

### Arquitetura WMI

A arquitetura WMI consiste em:

- **WMI Service (winmgmt):** O nucleo do WMI, rodando em `svchost.exe`.
- **CIM Object Manager (CIMOM):** Gerencia o repositorio de objetos CIM.
- **Providers:** Componentes que fornecem dados sobre aspectos especificos do sistema.
- **MOF (Managed Object Format):** Linguagem de definicao de classes.

### Providers WMI

Os **WMI providers** sao DLLs (ou executaveis, no caso de providers desacoplados) que fornecem dados sobre aspectos especificos do sistema. Por exemplo:

- `Win32_Process`: Informacoes sobre processos em execucao.
- `Win32_Service`: Informacoes sobre servicos.
- `Win32_Disk`: Informacoes sobre discos.
- `Win32_NetworkAdapter`: Informacoes sobre adaptadores de rede.

### Consultando o WMI

O WMI suporta consultas via **WQL (WMI Query Language)**, que e um subconjunto do SQL:

```cpp
#include <windows.h>
#include <wbemidl.h>
#include <comdef.h>
#include <stdio.h>
#pragma comment(lib, "wbemuuid.lib")

int main() {
    CoInitializeEx(NULL, COINIT_MULTITHREADED);
    CoInitializeSecurity(NULL, -1, NULL, NULL,
        RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE,
        NULL, EOAC_NONE, NULL);

    IWbemLocator* pLoc = NULL;
    CoCreateInstance(CLSID_WbemLocator, NULL, CLSCTX_INPROC_SERVER,
        IID_IWbemLocator, (void**)&pLoc);

    IWbemServices* pSvc = NULL;
    pLoc->ConnectServer(_bstr_t(L"ROOT\\CIMV2"), NULL, NULL, NULL, 0, NULL, NULL, &pSvc);

    IEnumWbemClassObject* pEnum = NULL;
    pSvc->ExecQuery(
        _bstr_t("WQL"),
        _bstr_t("SELECT Name, ProcessId FROM Win32_Process"),
        WBEM_FLAG_FORWARD_ONLY, NULL, &pEnum);

    IWbemClassObject* pObj = NULL;
    ULONG retornados = 0;
    while (pEnum->Next(WBEM_INFINITE, 1, &pObj, &retornados) == S_OK) {
        VARIANT vName, vPid;
        pObj->Get(L"Name", 0, &vName, NULL, NULL);
        pObj->Get(L"ProcessId", 0, &vPid, NULL, NULL);
        wprintf(L"PID %5u: %s\n", vPid.uintVal, vName.bstrVal);
        VariantClear(&vName); VariantClear(&vPid);
        pObj->Release();
    }

    pEnum->Release(); pSvc->Release(); pLoc->Release();
    CoUninitialize();
    return 0;
}
```

> **Rode no Visual Studio**
> Uma forma muito mais simples de consultar o WMI e via PowerShell ou wmic.exe:
>
> ```cpp
> #include <windows.h>
> #include <stdio.h>
>
> int main() {
>     // Executa wmic para listar processos
>     STARTUPINFOW si = { sizeof(si) };
>     PROCESS_INFORMATION pi = {};
>     WCHAR cmd[] = L"wmic process get Name,ProcessId /format:csv";
>
>     CreateProcessW(NULL, cmd, NULL, NULL, FALSE, CREATE_NO_WINDOW,
>         NULL, NULL, &si, &pi);
>     WaitForSingleObject(pi.hProcess, 5000);
>     CloseHandle(pi.hProcess);
>     CloseHandle(pi.hThread);
>     return 0;
> }
> ```

---

## Event Tracing for Windows (ETW)

O **ETW** e o sistema de rastreamento de eventos de alta performance do Windows. Ele foi projetado para ter impacto minimo no desempenho do sistema mesmo quando ativo, tornando-o adequado para uso em sistemas de producao.

### O modelo ETW

O ETW e organizado em tres componentes:

1. **Providers:** Componentes do sistema (drivers, servicos, aplicativos) que geram eventos.
2. **Controllers:** Ferramentas que configuram sessoes de rastreamento e habilitam providers.
3. **Consumers:** Ferramentas que processam os eventos coletados.

### Inicializacao do ETW

O ETW e inicializado durante o processo de boot, antes do kernel estar completamente inicializado. Isso permite capturar eventos de boot que de outra forma seriam perdidos.

O ETW usa um buffer circular em memoria para armazenar eventos antes de escrevê-los em um arquivo de log. Cada sessao ETW tem seus proprios buffers.

### Sessoes ETW

Uma **sessao ETW** e uma configuracao de coleta de eventos que especifica:

- Quais providers estao habilitados.
- Para quais niveis de severidade (critico, erro, aviso, informativo, verbose).
- Para quais palavras-chave (categorias de eventos).
- Tamanho e numero de buffers.
- Arquivo de log de saida (opcional).

O Windows suporta ate 64 sessoes de rastreamento simultaneas. Algumas sessoes sao reservadas pelo sistema:

| Sessao | Descricao |
|--------|-----------|
| `NT Kernel Logger` | A sessao do kernel para eventos do sistema |
| `Circular Kernel Context Logger` | Buffer circular sempre ativo para diagnostico |
| `EventLog-Security` | Eventos de seguranca para o Event Log |

### Providers ETW

Um **provider ETW** e um componente que pode gerar eventos. Cada provider e identificado por um **GUID**. Os tipos de providers sao:

- **Classic providers (WPP):** O formato legado usando macros WPP.
- **Manifest providers:** Providers modernos que descrevem seus eventos em um arquivo XML de manifesto.
- **TraceLogging providers:** Um formato simplificado introduzido no Windows 8.1.

### Gerando eventos

Para gerar eventos em modo usuario, um provider usa a API `EventWrite`:

```cpp
#include <windows.h>
#include <evntprov.h>
#include <stdio.h>

// GUID do provider (normalmente gerado com uuidgen)
static const GUID ProviderGuid = 
    { 0x12345678, 0x1234, 0x1234, { 0x12, 0x34, 0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc } };

REGHANDLE g_hProvider;

int main() {
    // Registra o provider
    EventRegister(&ProviderGuid, NULL, NULL, &g_hProvider);

    // Define e escreve um evento
    EVENT_DESCRIPTOR evDesc;
    EventDescCreate(&evDesc, 1, 0, 0, 4 /*INFORMATION*/, 0, 0, 0);

    WCHAR mensagem[] = L"Evento de teste ETW";
    EVENT_DATA_DESCRIPTOR dados;
    EventDataDescCreate(&dados, mensagem, (ULONG)((wcslen(mensagem) + 1) * sizeof(WCHAR)));

    EventWrite(g_hProvider, &evDesc, 1, &dados);
    printf("Evento ETW gerado.\n");

    EventUnregister(g_hProvider);
    return 0;
}
```

> **Rode no Visual Studio**
> Para capturar eventos ETW em tempo real, use o `logman` ou o `tracelog` na linha de comando, ou o **Windows Performance Recorder (WPR)**:
>
> ```cpp
> // Inicia uma sessao ETW via linha de comando (rode como admin):
> // logman start "minha-sessao" -p {GUID-DO-PROVIDER} -o trace.etl -ets
> // logman stop "minha-sessao" -ets
> // tracerpt trace.etl -o relatorio.xml
>
> #include <windows.h>
> #include <stdio.h>
>
> int main() {
>     // Abre o Event Log de aplicacoes e le o ultimo evento
>     HANDLE hLog = OpenEventLogW(NULL, L"Application");
>     if (!hLog) { printf("Erro: %lu\n", GetLastError()); return 1; }
>
>     DWORD bytesLidos = 0, bytesNecessarios = 0;
>     BYTE buffer[4096];
>     if (ReadEventLogW(hLog, EVENTLOG_BACKWARDS_READ | EVENTLOG_SEQUENTIAL_READ,
>             0, buffer, sizeof(buffer), &bytesLidos, &bytesNecessarios)) {
>         EVENTLOGRECORD* rec = (EVENTLOGRECORD*)buffer;
>         printf("Ultimo evento - ID: %lu, Categoria: %u\n",
>             rec->EventID & 0xFFFF, rec->EventCategory);
>     }
>
>     CloseEventLog(hLog);
>     return 0;
> }
> ```

### Thread de logger ETW

O **ETW Logger thread** e uma thread de sistema dedicada que e acordada periodicamente para esvaziar os buffers de memoria para o arquivo de log em disco. Isso garante que os buffers nao transbordem mesmo quando muitos eventos sao gerados rapidamente.

### Consumindo eventos

Os eventos ETW podem ser consumidos de duas formas:

1. **Tempo real:** Um consumer se inscreve em uma sessao ao vivo e recebe eventos a medida que sao gerados.
2. **Arquivo de log:** Os eventos sao gravados em um arquivo `.etl` que pode ser analisado posteriormente.

As ferramentas para analise de logs ETW incluem:

- **Windows Performance Analyzer (WPA):** Ferramenta grafica de analise do Windows SDK.
- **PerfView:** Ferramenta de analise de desempenho e memoria da Microsoft.
- **tracerpt.exe:** Ferramenta de linha de comando incluida no Windows.
- **xperf:** Ferramenta de linha de comando do Windows Performance Toolkit.

### Loggers do sistema (System loggers)

O Windows mantem varias sessoes ETW sempre ativas, chamadas de **system loggers**:

- **NT Kernel Logger:** Captura eventos do kernel como operacoes de processo, thread, I/O de arquivo, pagefaults, etc.
- **Circular Kernel Context Logger (CKCL):** Um buffer circular menor sempre ativo que captura os eventos mais recentes do kernel. Usado para diagnostico quando ocorre um crash.
- **SiRepairETW:** Monitoramento de integridade do sistema.

---

## Rastreamento dinamico (DTrace)

O Windows 10 versao 1903 introduziu suporte ao **DTrace** - a ferramenta de rastreamento dinamico originalmente desenvolvida pela Sun Microsystems para Solaris e depois portada para outros sistemas Unix.

O DTrace permite que os administradores e desenvolvedores criem scripts de rastreamento personalizados usando a linguagem D, sem necessidade de compilar e instalar modulos de kernel separados.

### Arquitetura interna do DTrace

A implementacao do DTrace no Windows consiste em:

- **Agente DTrace (`dtrace.exe`):** A ferramenta de linha de comando que compila e executa scripts D.
- **Driver do kernel DTrace (`DTrace.sys`):** O driver que implementa a infraestrutura de probing no kernel.
- **Biblioteca DTrace:** Fornece probes predefinidos para varios subsistemas.

O DTrace no Windows usa o ETW como mecanismo subjacente para receber eventos do kernel, fornecendo uma camada de abstracao adicional.

### Biblioteca de tipos DTrace

O DTrace inclui definicoes de tipos para estruturas internas do Windows como `KTHREAD`, `EPROCESS`, etc. Isso permite que scripts D acessem campos especificos das estruturas do kernel por nome.

---

## Windows Error Reporting (WER)

O **WER** e o servico do Windows responsavel por coletar, analisar e reportar erros e crashes de aplicativos e do sistema operacional.

### Crashes de aplicativos em modo usuario

Quando um aplicativo em modo usuario sofre um crash (excecao nao tratada), a sequencia de eventos e:

1. A excecao nao tratada chega ao ultimo handler da cadeia de SEH.
2. O `ntdll.dll` invoca o frame `UnhandledExceptionFilter`.
3. Este frame notifica o debugger (se houver um conectado).
4. Se nenhum debugger estiver conectado, o WER e invocado.
5. O WER cria um **minidump** do processo que travou.
6. O WER exibe uma caixa de dialogo (ou suprime silenciosamente, dependendo da configuracao).
7. O WER envia o relatorio de crash para os servidores da Microsoft (se o usuario consentiu).

O WER usa um processo separado (`WerFault.exe` ou `WerFaultSecure.exe`) para criar o minidump. Isso e necessario porque o processo que travou pode estar em estado corrupto e nao pode ser confiado para criar seu proprio dump.

### Crashes de modo kernel (BSODs)

Quando o kernel sofre um crash irreparavel, ele executa `KeBugCheckEx`, que:

1. Salva o estado atual do sistema.
2. Desabilita todas as interrupcoes.
3. Exibe o **Blue Screen of Death (BSOD)** com informacoes do erro.
4. Cria um arquivo de dump do sistema (dependendo da configuracao):
   - **Small memory dump (minidump):** Os dados mais basicos (~256 KB).
   - **Kernel memory dump:** Toda a memoria do kernel.
   - **Complete memory dump:** Toda a memoria fisica.
   - **Automatic memory dump:** Similar ao kernel dump, mas adaptativo.

Os codigos de bugcheck mais comuns incluem:

| Codigo | Nome | Descricao |
|--------|------|-----------|
| `0x0000000A` | `IRQL_NOT_LESS_OR_EQUAL` | Acesso a memoria em IRQL muito alto |
| `0x0000001E` | `KMODE_EXCEPTION_NOT_HANDLED` | Excecao nao tratada no kernel |
| `0x0000003B` | `SYSTEM_SERVICE_EXCEPTION` | Excecao em uma syscall |
| `0x0000007B` | `INACCESSIBLE_BOOT_DEVICE` | Dispositivo de boot inacessivel |
| `0x000000EF` | `CRITICAL_PROCESS_DIED` | Processo critico terminou |
| `0xC0000005` | `STATUS_ACCESS_VIOLATION` | Violacao de acesso |

---

## Flags globais

O Windows tem um conjunto de **flags globais** (global flags) que podem ser usados para habilitar varias opcoes de depuracao e diagnostico do sistema. As flags globais sao armazenadas em `HKLM\SYSTEM\CurrentControlSet\Control\Session Manager\GlobalFlag`.

A ferramenta `gflags.exe` (parte do Windows SDK) fornece uma interface grafica e de linha de comando para gerenciar flags globais.

Algumas flags uteis incluem:

| Flag | Nome | Descricao |
|------|------|-----------|
| `0x00000020` | `FLG_HEAP_ENABLE_FREE_CHECK` | Verifica integridade do heap em Free |
| `0x00000100` | `FLG_HEAP_VALIDATE_PARAMETERS` | Valida parametros de funcoes de heap |
| `0x00001000` | `FLG_USER_STACK_TRACE_DB` | Banco de dados de trace de pilha de usuario |
| `0x00002000` | `FLG_KERNEL_STACK_TRACE_DB` | Banco de dados de trace de pilha do kernel |
| `0x00010000` | `FLG_ENABLE_KDEBUG_SYMBOL_LOAD` | Carregamento de simbolos de kernel |
| `0x02000000` | `FLG_ENABLE_CLOSE_EXCEPTIONS` | Excecoes em operacoes de Close |

---

## Kernel shims

O Windows inclui um mecanismo de compatibilidade chamado **kernel shims** (ou driver shims) que permite que o sistema aplique correcoes de compatibilidade a nivel de kernel sem modificar os drivers ou aplicativos originais.

### Inicializacao do shim engine

O shim engine e inicializado durante o processo de boot. Ele le a **shim database** (arquivo `.sdb`) que contem as definicoes dos shims a serem aplicados.

### O banco de dados de shims

O banco de dados de shims do sistema fica em:

`%SystemRoot%\AppPatch\sysmain.sdb`

Cada entrada no banco de dados mapeia um executavel especifico (identificado por nome, tamanho, checksum) para um conjunto de shims a serem aplicados quando esse executavel rodar.

### Driver shims e Device shims

Alem dos shims para aplicativos em modo usuario, o Windows suporta:

- **Driver shims:** Correcoes aplicadas a drivers de kernel especificos para garantir compatibilidade com versoes mais novas do Windows.
- **Device shims:** Correcoes aplicadas a dispositivos ou classes de dispositivos especificos para resolver problemas de compatibilidade de hardware.

---

## Conclusao

Este capitulo examinou os mecanismos fundamentais que o Windows usa para configuracao, gerenciamento e diagnostico do sistema.

O registro e a espinha dorsal de toda a configuracao do Windows - desde configuracoes de boot de baixo nivel ate preferencias de usuario de alto nivel. Sua arquitetura de hives com logging transacional garante durabilidade mesmo em caso de falhas.

Os servicos do Windows fornecem um modelo robusto para componentes de sistema de longa duracao, com suporte a diferentes contas de seguranca, politicas de reinicio em caso de falha e o sistema de triggered-start para economia de recursos.

O ETW e o mecanismo de rastreamento mais poderoso do Windows, permitindo visibilidade profunda no comportamento do sistema com impacto minimo de desempenho. Ferramentas como o WPA e o PerfView podem processar traces ETW para diagnosticar praticamente qualquer problema de desempenho ou comportamento.

O WER fecha o ciclo de diagnostico coletando e reportando crashes automaticamente, tornando possivel para a Microsoft identificar e corrigir bugs que afetam muitos usuarios.

No proximo capitulo, examinaremos como o Windows gerencia o cache de I/O e os sistemas de arquivos, incluindo NTFS e ReFS.
