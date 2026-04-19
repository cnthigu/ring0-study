# Capítulo 3 — Processos e Jobs

> *Tradução do livro Windows Internals, 7ª Edição*  
> *Pavel Yosifovich, Alex Ionescu, Mark E. Russinovich, David A. Solomon*  
> *Microsoft Press, 2017*

---

Neste capítulo, explicaremos as estruturas de dados e algoritmos que lidam com processos e jobs no Windows. Primeiro, faremos uma visão geral da criação de processos. Em seguida, examinaremos as estruturas internas que compõem um processo. Depois disso, analisaremos os processos protegidos e como eles diferem dos não protegidos. Em seguida, delinearemos as etapas envolvidas na criação de um processo (e sua thread inicial). O capítulo conclui com uma descrição de jobs.

## Criando um processo

A Windows API fornece várias funções para criar processos. A mais simples é `CreateProcess`, que tenta criar um processo com o mesmo token de acesso que o processo criador. Se um token diferente for necessário, `CreateProcessAsUser` pode ser usada, que aceita um argumento extra — um handle para um objeto token que foi obtido de alguma forma (por exemplo, chamando a função `LogonUser`).

Outras funções de criação de processo incluem `CreateProcessWithTokenW` e `CreateProcessWithLogonW` (ambas parte de advapi32.dll). Ambas chamam o serviço Secondary Logon (seclogon.dll, hospedado em SvcHost.exe) fazendo uma chamada de Procedimento Remoto (RPC) para realizar a criação real do processo.

Em última análise, todos esses caminhos de execução levam a uma função interna comum, `CreateProcessInternal`, que inicia o trabalho real de criação de um processo Windows em modo usuário. Se tudo correr bem, `CreateProcessInternal` chama `NtCreateUserProcess` em Ntdll.dll para fazer a transição para o modo kernel.

> **Rode no Visual Studio**
> `CreateProcess` cria um novo processo e sua thread inicial. O exemplo abaixo abre o Notepad e aguarda ele fechar.
>
> ```cpp
> #include <windows.h>
> #include <stdio.h>
>
> int main() {
>     STARTUPINFOW si = { sizeof(si) };
>     PROCESS_INFORMATION pi = {};
>
>     BOOL ok = CreateProcessW(
>         L"C:\\Windows\\System32\\notepad.exe",
>         NULL,
>         NULL, NULL,
>         FALSE,
>         0,
>         NULL, NULL,
>         &si, &pi
>     );
>
>     if (ok) {
>         printf("Processo criado. PID: %lu | TID: %lu\n", pi.dwProcessId, pi.dwThreadId);
>         WaitForSingleObject(pi.hProcess, INFINITE);
>         printf("Notepad fechado.\n");
>         CloseHandle(pi.hProcess);
>         CloseHandle(pi.hThread);
>     }
>
>     return 0;
> }
> ```

### Argumentos das funções CreateProcess*

Um processo criado a partir do modo usuário é sempre criado com uma thread dentro dele. Os argumentos importantes para as funções `CreateProcess*` incluem:

- Para `CreateProcessAsUser` e `CreateProcessWithTokenW`: o handle do token sob o qual o novo processo deve ser executado.
- O caminho do executável e os argumentos da linha de comando.
- Atributos de segurança opcionais aplicados ao novo processo e objeto thread.
- Um flag booleano indicando se todos os handles no processo criador marcados como herdáveis devem ser herdados pelo novo processo.
- Vários flags que afetam a criação do processo:
  - `CREATE_SUSPENDED` — Cria a thread inicial do novo processo em estado suspenso.
  - `DEBUG_PROCESS` — O processo criador se declara como depurador, criando o novo processo sob seu controle.
  - `EXTENDED_STARTUPINFO_PRESENT` — A estrutura estendida `STARTUPINFOEX` é fornecida em vez de `STARTUPINFO`.
- Um bloco de ambiente opcional para o novo processo.
- Um diretório atual opcional para o novo processo.
- Uma estrutura `STARTUPINFO` ou `STARTUPINFOEX`.
- Uma estrutura `PROCESS_INFORMATION` que é a saída de uma criação de processo bem-sucedida, contendo o novo ID de processo único, o novo ID de thread único, um handle para o novo processo e um handle para a nova thread.

### Criando processos modernos do Windows

Criar um processo de aplicativo moderno (UWP/imersivo) requer mais do que apenas chamar `CreateProcess` com o caminho executável correto. Há alguns argumentos obrigatórios de linha de comando, além de um atributo de processo não documentado com uma chave chamada `PROC_THREAD_ATTRIBUTE_PACKAGE_FULL_NAME` com o valor definido como o nome completo do pacote do store app.

A Windows API inclui uma interface COM chamada `IApplicationActivationManager`, implementada por uma classe COM com um CLSID chamado `CLSID_ApplicationActivationManager`. Um dos métodos na interface é `ActivateApplication`, que pode ser usado para iniciar um store app.

### Criando outros tipos de processos

O Executive inclui suporte para tipos adicionais de processos que devem ser iniciados contornando a Windows API, como processos nativos, processos mínimos (minimal processes) ou processos Pico. Por exemplo, o Session Manager (Smss) é um exemplo de imagem nativa. Como é criado diretamente pelo kernel, obviamente não usa a API `CreateProcess`, mas em vez disso chama diretamente `NtCreateUserProcess`.

Em última análise, independentemente do caminho — seja um cmdlet PowerShell WMI ou um driver do kernel — todas as maneiras de criar um processo terminarão nas rotinas internas `PspAllocateProcess` e `PspInsertProcess`.

## Internos do processo

Cada processo Windows é representado por uma estrutura de processo executivo (EPROCESS). Além de conter muitos atributos relacionados a um processo, um EPROCESS contém e aponta para várias outras estruturas de dados relacionadas.

O EPROCESS e a maioria das suas estruturas de dados relacionadas existem no espaço de endereçamento do sistema. Uma exceção é o Process Environment Block (PEB), que existe no espaço de endereçamento do processo (usuário), pois contém informações acessadas pelo código em modo usuário.

Para cada processo que executa um programa Windows, o processo do subsistema Windows (Csrss) mantém uma estrutura paralela chamada `CSR_PROCESS`. Além disso, a parte em modo kernel do subsistema Windows (Win32k.sys) mantém uma estrutura de dados por processo, `W32PROCESS`, que é criada na primeira vez que uma thread chama uma função USER ou GDI do Windows implementada em modo kernel.

### EXPERIMENTO: Exibindo o formato de uma estrutura EPROCESS

Para uma lista dos campos que compõem uma estrutura EPROCESS e seus deslocamentos em hexadecimal, digite `dt nt!_eprocess` no depurador do kernel:

```
lkd> dt nt!_eprocess
+0x000 Pcb : _KPROCESS
+0x2d8 ProcessLock : _EX_PUSH_LOCK
+0x2e8 UniqueProcessId : Ptr64 Void
+0x2f0 ActiveProcessLinks : _LIST_ENTRY
...
+0x3a8 Win32Process : Ptr64 Void
+0x3b0 Job : Ptr64 _EJOB
...
+0x418 ObjectTable : Ptr64 _HANDLE_TABLE
+0x420 DebugPort : Ptr64 Void
+0x428 WoW64Process : Ptr64 _EWOW64PROCESS
```

O primeiro membro desta estrutura (Pcb) é uma estrutura incorporada do tipo KPROCESS. É onde os dados de agendamento e contabilidade de tempo são armazenados.

O PEB reside no espaço de endereçamento em modo usuário do processo que descreve. Contém informações necessárias pelo carregador de imagens (image loader), pelo gerenciador de heap e por outros componentes Windows que precisam acessá-las a partir do modo usuário.

### EXPERIMENTO: Usando o comando !process do depurador do kernel

O comando `!process` do depurador do kernel exibe um subconjunto das informações em um objeto de processo e suas estruturas associadas:

```
lkd> !process
PROCESS ffffe0011c3243c0
    SessionId: 2  Cid: 0e38    Peb: 5f2f1de000  ParentCid: 0f08
    Image: windbg.exe
    VadRoot ffffe0011badae60  Vads 117  Clone 0  Private 3563.
    Token    ffffc000a13f39a0
    ElapsedTime 00:00:20.772
    BasePriority 8
    CommitCharge 3994
```

## Processos protegidos

No modelo de segurança do Windows, qualquer processo rodando com um token que contenha o privilégio de debug (como uma conta de administrador) pode solicitar qualquer direito de acesso que deseje a qualquer outro processo rodando na máquina.

Esse comportamento lógico conflita com os requisitos de gerenciamento de direitos digitais (DRM) impostos pela indústria de mídia. Para suportar a reprodução confiável e protegida de conteúdo como mídia Blu-ray, o Windows Vista e o Windows Server 2008 introduziram **processos protegidos** (protected processes). Esses processos existem ao lado dos processos Windows normais, mas adicionam restrições significativas aos direitos de acesso que outros processos no sistema podem solicitar.

O único direito de acesso concedido para processos protegidos é `PROCESS_QUERY/SET_LIMITED_INFORMATION`, `PROCESS_TERMINATE` e `PROCESS_SUSPEND_RESUME`. Certos direitos de acesso também são desabilitados para threads rodando dentro de processos protegidos.

O Processo de Áudio (Audiodg.exe) é um processo protegido porque conteúdo musical protegido pode ser decodificado por ele. O processo WER (Windows Error Reporting) cliente (Werfaultsecure.exe) também pode rodar protegido porque precisa ter acesso a processos protegidos caso um deles trave. Finalmente, o próprio processo System é protegido para proteger a integridade de todos os handles do kernel.

### Protected Process Light (PPL)

A partir do Windows 8.1 e Windows Server 2012 R2, uma extensão ao modelo de processo protegido foi introduzida, chamada **Protected Process Light (PPL)**.

PPLs são protegidos no mesmo sentido que os processos protegidos clássicos. No entanto, o modelo PPL adiciona uma dimensão adicional à qualidade de ser protegido: valores de atributo. Os diferentes **Signers** têm diferentes níveis de confiança, o que por sua vez resulta em certos PPLs sendo mais ou menos protegidos do que outros PPLs.

Os valores legais para o flag de proteção armazenado na estrutura EPROCESS são:

| Tipo | Descrição | Exemplos |
|------|-----------|---------|
| PsProtectedSignerWinSystem | Nível mais alto, para o processo System | System, Memory Compression |
| PsProtectedSignerWinTcb | Processos críticos conhecidos pelo kernel | smss.exe, csrss.exe, services.exe |
| PsProtectedSignerWindows | Componentes do Windows | sppsvc.exe |
| PsProtectedSignerAntimalware | Software antimalware | MsMpEng.exe |
| PsProtectedSignerLsa | Autenticação de segurança local | lsass.exe |
| PsProtectedSignerCodeGen | Runtime .NET | |
| PsProtectedSignerAuthenticode | Aplicativos assinados | |
| PsProtectedSignerNone | Sem proteção | |

O Code Integrity do Windows entende dois OIDs especiais de uso de chave estendida (EKU) que podem ser codificados em um certificado digital de assinatura de código: `1.3.6.1.4.1.311.10.3.22` e `1.3.6.4.1.311.10.3.20`.

O nível de proteção de um processo também impacta quais DLLs ele terá permissão de carregar. Cada processo recebe um "Nível de Assinatura" armazenado no campo `SignatureLevel` do EPROCESS. Por exemplo, um processo com "WinTcb" como assinante do executável só carregará DLLs assinadas como "Windows" ou superior.

#### EXPERIMENTO: Visualizando processos protegidos no Process Explorer

Execute o Process Explorer e selecione a caixa de seleção Proteção (Protection) na aba Process Image para visualizar a coluna Protection. Classifique pela coluna Protection em ordem decrescente e role para o topo para ver todos os processos protegidos com seu tipo de proteção.

### Suporte a PPL de terceiros

O mecanismo PPL estende as possibilidades de proteção para processos além dos executáveis criados exclusivamente pela Microsoft. Um exemplo comum é o software antimalware (AM). Um produto AM típico consiste em três componentes principais:

1. Um driver do kernel que intercepta solicitações de I/O para o sistema de arquivos e/ou rede.
2. Um serviço em modo usuário que configura as políticas do driver.
3. Um processo GUI em modo usuário que comunica informações ao usuário.

Para habilitar PPL para software AM, o driver do kernel AM precisa ter um driver ELAM (Early-Launch Anti Malware) correspondente. Tais drivers requerem um certificado especial anti-malware fornecido pela Microsoft. Uma vez instalado, o driver pode conter uma seção de recurso personalizada chamada `ELAMCERTIFICATEINFO`. Um exemplo canônico é o próprio AM da Microsoft, o Windows Defender. Seu serviço no Windows 10 (MsMpEng.exe) é assinado com o certificado anti-malware para melhor proteção.

## Processos mínimos e Pico

### Processos mínimos

Quando um flag específico é fornecido à função `NtCreateProcessEx` e o chamador está em modo kernel, a função causa a execução da API `PsCreateMinimalProcess`. Um processo mínimo é criado sem muitas das estruturas vistas anteriormente:

- Nenhum espaço de endereçamento em modo usuário será configurado — sem PEB e estruturas relacionadas.
- Nenhuma NTDLL será mapeada no processo.
- Nenhum objeto de seção estará vinculado ao processo — nenhum arquivo de imagem executável está associado.
- O flag Minimal será definido no EPROCESS, fazendo com que todas as threads se tornem threads mínimas.

O Windows 10 tem pelo menos dois processos mínimos: o processo System e o processo Memory Compression. Pode haver um terceiro, o processo Secure System, se a Virtualization-Based Security estiver habilitada.

### Processos Pico

Processos Pico assumem um papel mais importante ao permitir que um componente especial, chamado Pico Provider, controle a maioria dos aspectos de sua execução de uma perspectiva de sistema operacional. Esse nível de controle permite que tal provedor emule o comportamento de um kernel de sistema operacional completamente diferente, sem que o binário em modo usuário subjacente tenha consciência de que está rodando em um sistema operacional baseado em Windows.

Para suportar a existência de processos Pico no sistema, um provedor deve primeiro estar presente. Tal provedor pode ser registrado com a API `PsRegisterPicoProvider`. No Windows 10 versão 1607, um tal provedor está presente: Lxss.sys e seu parceiro LxCore.sys — o componente do Windows Subsystem for Linux (WSL).

Quando um Pico provider chama a API de registro, ele recebe um conjunto de ponteiros de função que permitem criar e gerenciar processos Pico:

- Funções para criar processos e threads Pico.
- Funções para obter e definir o contexto de um processo ou thread Pico.
- Funções para obter e definir a estrutura de contexto de CPU (CONTEXT) de uma thread Pico.
- Funções para terminar, suspender e retomar threads e processos Pico.

Um segundo conjunto de ponteiros de função é transferido — callbacks chamados quando atividades de interesse são realizadas por uma thread ou processo Pico:

- Um callback quando uma thread Pico faz uma chamada de sistema usando a instrução SYSCALL.
- Um callback quando uma exceção é levantada de uma thread Pico.
- Um callback quando alguém solicita a terminação de um processo Pico.
- Um callback quando uma thread ou processo Pico termina inesperadamente.

## Trustlets (processos seguros)

O Windows contém novos recursos de segurança baseados em virtualização (VBS), como Device Guard e Credential Guard, que aproveitam o hypervisor. Um desses recursos, o Credential Guard, roda em um novo ambiente Isolated User Mode (IUM) que, embora ainda sem privilégios (ring 3), tem um virtual trust level de 1 (VTL 1).

### Estrutura do Trustlet

Trustlets são arquivos PE Windows regulares, mas contêm algumas propriedades específicas de IUM:

- Podem importar apenas de um conjunto limitado de DLLs do sistema Windows.
- Podem importar de uma DLL de sistema específica de IUM chamada Iumbase, que fornece a Base IUM System API.
- Contêm uma seção PE chamada `.tPolicy` com uma variável global exportada chamada `s_IumPolicyMetadata`.
- São assinados com um certificado que contém o IUM EKU (`1.3.6.1.4.311.10.3.37`).

### Metadados de política do Trustlet

Os metadados de política incluem várias opções para configurar quão "acessível" o Trustlet será a partir do VTL 0. Inclui opções como permitir depuração, suporte a crash dump e acesso a seções seguras.

### Trustlets integrados do sistema

No momento da escrita, o Windows 10 contém cinco Trustlets diferentes, identificados por seus números de identidade:

| ID | Nome | Descrição |
|----|------|-----------|
| 0 | Secure Kernel | O próprio Secure Kernel |
| 1 | LsaIso.exe | Credential Guard e Key Guard |
| 2 | Vmsp.exe | Proteção de VM (Hyper-V shielded) |
| 3 | Unknown | Uso futuro |
| 4 | BioIso.exe | Biometria segura (Windows Hello) |

### Identidade do Trustlet

Trustlets têm múltiplas formas de identidade:

- **Identificador do Trustlet (Trustlet ID)** — Um inteiro codificado nos metadados de política.
- **Instância do Trustlet** — Um número aleatório criptograficamente seguro de 16 bytes gerado pelo Secure Kernel.
- **Collaboration ID** — Usado quando um Trustlet gostaria de permitir que outros Trustlets compartilhem acesso ao mesmo blob de Armazenamento Seguro.
- **Security version (SVN)** — Usado para Trustlets que requerem prova criptográfica forte de proveniência.

### Chamadas de sistema acessíveis ao Trustlet

O Secure Kernel fornece um subconjunto (menos de 50) de todas as centenas de chamadas de sistema que um aplicativo normal pode usar. Estas incluem:

- APIs de Worker Factory e Thread para suportar o Thread Pool API (usado por RPC).
- API de Informação do Processo para suporte a slots TLS.
- APIs de Evento, Semáforo, Espera e Conclusão para sincronização.
- APIs ALPC para RPC local.
- API de Informações do Sistema para informações básicas do sistema.
- APIs de Alocação de Memória Virtual para o Heap Manager em modo usuário.
- APIs de Seção para o Carregador.
- API de Controle de Trace para ETW.

## Fluxo do CreateProcess

A criação de um processo Windows consiste em várias etapas realizadas em três partes do sistema operacional: a biblioteca do lado do cliente Windows Kernel32.dll, o executive do Windows e o processo do subsistema Windows (Csrss).

### Etapa 1: Convertendo e validando parâmetros e flags

Antes de abrir a imagem executável para rodar, `CreateProcessInternalW` realiza as seguintes etapas:

1. A classe de prioridade para o novo processo é especificada como bits independentes. Há seis classes de prioridade de processo: Idle (4), Below Normal (6), Normal (8), Above Normal (10), High (13) e Real-time (24).
2. Se nenhuma classe de prioridade for especificada, o padrão é Normal. Se Real-time for especificado sem privilégio suficiente, High é usado em vez disso.
3. Se as flags de criação especificam que o processo será depurado, Kernel32 inicia uma conexão com o código de depuração nativo em Ntdll.dll.
4. A lista de atributos fornecida pelo usuário é convertida do formato do subsistema Windows para o formato nativo.
5. Os atributos de segurança para o processo e thread inicial são convertidos para sua representação interna (estruturas `OBJECT_ATTRIBUTES`).

### Etapa 2: Abrindo a imagem a ser executada

1. `NtCreateUserProcess` primeiro valida os argumentos.
2. O próximo estágio é encontrar a imagem Windows apropriada que executará o arquivo executável especificado pelo chamador e criar um objeto de seção para mapeá-lo no espaço de endereçamento do novo processo.
3. Se o processo precisar ser criado protegido, ele também verifica a política de assinatura.
4. Se o processo for um Trustlet, o objeto de seção deve ser criado com um flag especial que permite ao secure kernel usá-lo.

A árvore de decisão que `CreateProcessInternalW` percorre para executar uma imagem inclui:
- Se for um aplicativo MS-DOS (.exe, .com, .pif): usa Ntvdm.exe.
- Se o arquivo tiver extensão .bat ou .cmd: usa Cmd.exe.
- Se for um executável Win16: usa Ntvdm.exe.

### Etapa 3: Criando o objeto de processo executivo do Windows

`NtCreateUserProcess` chama a função interna do sistema `PspAllocateProcess`. Esta sub-etapa inclui:

**3A: Configurando o objeto EPROCESS**
- Herdar a afinidade do processo pai.
- Herdar as prioridades de I/O e de página do processo pai.
- Criar o token de acesso primário do processo (uma duplicata do token primário de seu pai).
- Inicializar o espaço de endereçamento do processo.

**3B: Criando o espaço de endereçamento inicial do processo**
O espaço de endereçamento inicial consiste em:
- Diretório de páginas
- Página de hyperspace
- Página de bitmap VAD
- Lista do working set

**3C: Criando a estrutura de processo do kernel (KPROCESS)**
Realizada por `KeInitializeProcess`, que inicializa:
- A lista duplamente encadeada que conecta todas as threads do processo.
- O quantum padrão inicial do processo.
- A prioridade base do processo.
- A afinidade de processador padrão para as threads no processo.

**3D: Concluindo a configuração do espaço de endereçamento**
- O gerenciador de memória virtual define o valor do último tempo de trimming do processo.
- A seção (criada ao abrir o arquivo de imagem) agora é mapeada no espaço de endereçamento do novo processo.
- O PEB é criado e inicializado.
- Ntdll.dll é mapeada no processo.

**3E: Configurando o PEB**
`NtCreateUserProcess` chama `MmCreatePeb`, que primeiro mapeia as tabelas NLS em todo o sistema no espaço de endereçamento do processo e depois chama `MiCreatePebOrTeb` para alocar uma página para o PEB.

**3F: Concluindo a configuração do objeto de processo executivo**
Realizado por `PspInsertProcess`:
- Se a auditoria de processos em todo o sistema estiver habilitada, a criação do processo é escrita no log de eventos de Segurança.
- O novo objeto de processo é inserido no final da lista de processos ativos do Windows (`PsActiveProcessHead`).
- O objeto de processo é inserido na tabela de handles do processo.

### Etapa 4: Criando a thread inicial e sua pilha e contexto

A rotina `PspAllocateThread` realiza as seguintes etapas:

1. Impede que threads UMS sejam criadas em processos Wow64.
2. Um objeto de thread executivo é criado e inicializado.
3. Vários IDs de thread e listas usadas por LPC, Gerenciamento de I/O e Executive são inicializados.
4. O tempo de criação da thread é definido e seu TID é criado.
5. Uma pilha e um contexto para a thread são configurados.
6. O TEB (thread environment block) é alocado para a nova thread.
7. `KeInitThread` é chamado para configurar a estrutura KTHREAD.

### Etapa 5: Realizando a inicialização específica do subsistema Windows

Uma vez que `NtCreateUserProcess` retorna com um código de sucesso, `CreateProcessInternalW` realiza operações específicas do subsistema Windows:

1. São feitas verificações para saber se o Windows deve permitir a execução do executável, incluindo validar a versão da imagem no cabeçalho.
2. O banco de dados de compatibilidade de aplicativos é consultado para verificar se há uma entrada para o processo.
3. Uma mensagem para o subsistema Windows é construída com base nas informações coletadas e enviada ao Csrss.

Quando recebe esta mensagem, o subsistema Windows realiza as seguintes etapas:

1. `CsrCreateProcess` duplica um handle para o processo e thread.
2. A estrutura de processo Csrss (`CSR_PROCESS`) é alocada.
3. A porta de exceção do novo processo é definida para ser a porta de função geral do subsistema Windows.
4. A estrutura de thread Csrss (`CSR_THREAD`) é alocada e inicializada.
5. O nível de desligamento do processo é definido para 0x280, o nível de desligamento padrão.

### Etapa 6: Iniciando a execução da thread inicial

Neste ponto, o ambiente do processo foi determinado, os recursos para suas threads foram alocados, o processo tem uma thread e o subsistema Windows sabe sobre o novo processo. A menos que o chamador especificou o flag `CREATE_SUSPENDED`, a thread inicial agora é retomada.

### Etapa 7: Realizando a inicialização do processo no contexto do novo processo

A nova thread começa sua vida rodando a rotina de inicialização de thread em modo kernel `KiStartUserThread`, que então chama a rotina de thread inicial do sistema, `PspUserThreadStartup`. Esta realiza as seguintes ações:

1. Instala uma cadeia de exceções (em x86).
2. Abaixa o IRQL para PASSIVE_LEVEL (0).
3. Verifica se o processo é um debuggee e, se sim, envia eventos de debug.
4. Verifica se a pré-busca (prefetching) do aplicativo está habilitada.
5. Configura o contexto thunk inicial para rodar a rotina de inicialização do image loader (`LdrInitializeThunk` em Ntdll.dll).

O `LdrInitializeThunk` inicializa o loader, o gerenciador de heap, tabelas NLS, arrays TLS e FLS, e estruturas de seção crítica. Em seguida, carrega quaisquer DLLs necessárias e chama os pontos de entrada de DLL com o código de função `DLL_PROCESS_ATTACH`.

Uma vez que a função retorna, `NtContinue` restaura o novo contexto de usuário e retorna ao modo usuário. A execução da thread realmente começa com `RtlUserThreadStart`.

## Terminando um processo

Um processo pode sair graciosamente chamando a função `ExitProcess`. O termo graciosamente significa que as DLLs carregadas no processo têm a chance de fazer algum trabalho sendo notificadas da saída do processo usando uma chamada à sua função `DllMain` com `DLL_PROCESS_DETACH`.

Uma terminação forçada (sem graça) de um processo é possível usando a função `TerminateProcess`, que pode ser chamada de fora do processo. Isso é usado pelo Process Explorer e Task Manager quando solicitado pelo usuário.

Em qualquer forma que um processo deixe de existir, nunca pode haver vazamentos. Toda a memória privada do processo é liberada automaticamente pelo kernel, o espaço de endereçamento é destruído, todos os handles para objetos do kernel são fechados, etc.

## Image loader (carregador de imagens)

O image loader vive na DLL de sistema em modo usuário Ntdll.dll. O que torna esse código especial é a garantia de que sempre estará presente no processo em execução (Ntdll.dll é sempre carregada) e que é o primeiro pedaço de código a rodar em modo usuário como parte de um novo processo.

Algumas das principais tarefas que o loader é responsável incluem:

- Inicializar o estado em modo usuário para o aplicativo, como criar o heap inicial e configurar os slots TLS e FLS.
- Analisar a tabela de importação (IAT) do aplicativo para procurar todas as DLLs que requer.
- Carregar e descarregar DLLs em tempo de execução.
- Tratar arquivos de manifesto necessários para suporte Windows Side-by-Side (SxS) e arquivos MUI.
- Ler o banco de dados de compatibilidade de aplicativos para quaisquer shims.
- Habilitar suporte para API Sets e redirecionamento de API.

### Inicialização antecipada do processo

Quando um processo inicia, o loader realiza os seguintes passos:

1. Inicializar o banco de dados do loader no PEB.
2. Inicializar as tabelas National Language Support (NLS) para o processo.
3. Construir o nome do caminho da imagem para o aplicativo.
4. Consultar a chave Image File Execution Options (IFEO) do registro para o aplicativo.
5. Inicializar o gerenciador de heap para o processo e criar o primeiro heap do processo.
6. Abrir o diretório de objeto `\KnownDlls` e construir o caminho de DLLs conhecidas.
7. Determinar o diretório atual do processo, o caminho do sistema e o caminho de carga padrão.
8. Construir a primeira entrada da tabela de dados do loader para Ntdll.dll e inseri-la no banco de dados do módulo.
9. Se habilitado, inicializar o Application Verifier.
10. Inicializar o loader paralelo para carregar dependências usando o pool de threads.

### Resolução de nomes de DLL e redirecionamento

Quando se resolve dependências binárias, o modelo básico de aplicativo Windows localiza arquivos em um caminho de busca. Para prevenir riscos de segurança associados à substituição de binários, um recurso conhecido como **modo de busca segura de DLL** (safe DLL search mode) foi adicionado e está habilitado por padrão para todos os processos. Sob o modo de busca segura, a ordem de busca é:

1. O diretório a partir do qual o aplicativo foi lançado
2. O diretório nativo do sistema Windows (por exemplo, `C:\Windows\System32`)
3. O diretório do sistema Windows de 16 bits (por exemplo, `C:\Windows\System`)
4. O diretório Windows (por exemplo, `C:\Windows`)
5. O diretório atual no momento do lançamento do aplicativo
6. Quaisquer diretórios especificados pela variável de ambiente `%PATH%`

### Redirecionamento de nomes de DLL

Antes de tentar resolver uma string de nome de DLL para um arquivo, o loader tenta aplicar regras de redirecionamento:

- **Redirecionamento MinWin API Set** — Permite que diferentes versões ou edições do Windows alterem o binário que exporta uma determinada API do sistema de maneira transparente para os aplicativos.
- **Redirecionamento .LOCAL** — Permite que aplicativos redirecionem todas as cargas de um nome base de DLL específico para uma cópia local da DLL no diretório do aplicativo.
- **Redirecionamento Fusion (SxS)** — Permite que os componentes expressem informações de dependência binária mais detalhadas (geralmente informações de versão) incorporando recursos binários conhecidos como manifestos.
- **Redirecionamento de DLL Conhecida** — Um mecanismo que mapeia nomes base de DLL específicos para arquivos no diretório do sistema.

### Banco de dados de módulos carregados

O loader mantém uma lista de todos os módulos (DLLs, bem como o executável primário) que foram carregados por um processo. Essas informações são armazenadas no PEB — especificamente, em uma subestrutura identificada por `Ldr` e chamada `PEB_LDR_DATA`. No estrutura, o loader mantém três listas duplamente encadeadas, todas contendo as mesmas informações mas ordenadas diferentemente (por ordem de carga, localização de memória ou ordem de inicialização).

Além disso, o loader também mantém duas árvores rubro-negras que permitem ao algoritmo de busca rodar em tempo logarítmico:
- A primeira é ordenada por endereço base.
- A segunda é ordenada pelo hash do nome do módulo.

### Análise de importações

Durante esta etapa, o loader:

1. Carrega cada DLL referenciada na tabela de importação da imagem executável do processo.
2. Verifica se a DLL já foi carregada verificando o banco de dados de módulos.
3. Verifica se o kernel a carregou em outro lugar — isso é chamado de relocação.
4. Cria uma entrada da tabela de dados do loader para esta DLL e a insere no banco de dados.
5. Para cada nome importado, o loader analisa a tabela de exportação da DLL importada e tenta localizar uma correspondência.

### Pós-inicialização do processo

Após os importações necessárias serem carregadas:

1. Todos os inicializadores de DLL estão sendo executados — este é o passo que chama a rotina `DllMain` para cada DLL.
2. Se a imagem usar quaisquer slots TLS, chama seu inicializador TLS.
3. Escreve um evento ETW indicando que o processo foi carregado com sucesso.

## SwitchBack

À medida que cada nova versão do Windows corrige bugs em funções de API existentes, é criado um risco de compatibilidade de aplicativos para cada mudança. O Windows usa uma tecnologia chamada **SwitchBack**, implementada no loader, que permite que desenvolvedores de software embutam um GUID específico para a versão do Windows que estão visando em seu manifesto executável.

Os GUIDs definidos pelo Windows atualmente representam configurações de compatibilidade para cada versão do Windows Vista:

```
{e2011457-1546-43c5-a5fe-008deee3d3f0}  Windows Vista
{35138b9a-5d96-4fbd-8e2d-a2440225f93a}  Windows 7
{4a2f28e3-53b9-4441-ba9c-d69d4a4a6e38}  Windows 8
{1f676c76-80e1-4239-95bb-83d0f6d0da78}  Windows 8.1
{8e0f7a12-bfb3-4fe8-b9a5-48fd50a15a9a}  Windows 10
```

Exemplo de entrada de manifesto que define compatibilidade para Windows 10:

```xml
<compatibility xmlns="urn:schemas-microsoft-com:compatibility.v1">
  <application>
    <!-- Windows 10 -->
    <supportedOS Id="{8e0f7a12-bfb3-4fe8-b9a5-48fd50a15a9a}" />
  </application>
</compatibility>
```

## API Sets

Enquanto o SwitchBack usa redirecionamento de API para cenários específicos de compatibilidade de aplicativos, há um mecanismo de redirecionamento muito mais abrangente usado no Windows para todos os aplicativos, chamado **API Sets**. Seu propósito é habilitar a categorização refinada das APIs do Windows em sub-DLLs em vez de ter grandes DLLs de múltiplos propósitos.

Quando o gerenciador de processos é inicializado, ele chama a função `PspInitializeApiSetMap`, que é responsável por criar um objeto de seção da tabela de redirecionamento de API Set, armazenada em `%SystemRoot%\System32\ApiSetSchema.dll`. A DLL não contém código executável, mas tem uma seção chamada `.apiset` que contém dados de mapeamento do API Set.

Sempre que um novo processo inicia, o gerenciador de processos mapeia o objeto de seção no espaço de endereçamento do processo e define o campo `ApiSetMap` no PEB do processo para apontar para o endereço base onde o objeto de seção foi mapeado.

A função `LdrpApplyFileNameRedirection` do loader também verifica dados de redirecionamento de API Set sempre que uma nova biblioteca de importação com um nome que começa com `API-` é carregada. A tabela de API Set é organizada por biblioteca, com cada entrada descrevendo em qual DLL lógica a função pode ser encontrada.

## Jobs

Um job é um objeto de kernel nomeável, com segurança e compartilhável que permite o controle de um ou mais processos como um grupo. Um processo pode ser membro de qualquer número de jobs, embora o caso típico seja apenas um. A associação de um processo com um objeto job não pode ser quebrada, e todos os processos criados pelo processo e seus descendentes são associados ao mesmo objeto job.

Jobs desempenham um papel significativo em vários mecanismos do sistema:

- Gerenciam aplicativos modernos (processos UWP) — de fato, todo aplicativo moderno está rodando sob um job.
- São usados para implementar o suporte a Windows Containers, através de um mecanismo chamado server silo.
- São a principal forma através da qual o Desktop Activity Moderator (DAM) gerencia o throttling e outros comportamentos de indução de ociosidade.
- Permitem a definição e gerenciamento de grupos de agendamento para o Dynamic Fair-Share Scheduling (DFSS).

### Limites do job

Alguns dos limites relacionados a CPU, memória e I/O que você pode especificar para um job incluem:

- **Número máximo de processos ativos** — Limita o número de processos existentes simultaneamente no job.
- **Limite de tempo de CPU em modo usuário em todo o job** — Limita a quantidade máxima de tempo de CPU em modo usuário que os processos no job podem consumir.
- **Limite de tempo de CPU em modo usuário por processo** — Permite que cada processo no job acumule apenas uma quantidade máxima fixa de tempo de CPU em modo usuário.
- **Afinidade de processador do job** — Define a máscara de afinidade de processador para cada processo no job.
- **Classe de prioridade do processo do job** — Define a classe de prioridade para cada processo no job.
- **Controle de taxa de CPU** — Define a quantidade máxima de tempo de CPU que o job pode usar antes de ser forçado a throttling.
- **Controle de taxa de largura de banda de rede** — Define a largura de banda de saída máxima para todo o job antes de o throttling entrar em vigor.
- **Controle de taxa de largura de banda de I/O de disco** — O mesmo que o controle de taxa de rede, mas aplicado ao I/O de disco.

### Trabalhando com um job

Um objeto job é criado usando a API `CreateJobObject`. Para adicionar um processo a um job, chame `AssignProcessToJobObject`. A API mais interessante para jobs é `SetInformationJobObject`, que permite a configuração dos vários limites e configurações mencionados. Os valores podem ser lidos de volta com `QueryInformationJobObject`.

> **Rode no Visual Studio**
> O exemplo abaixo cria um job, abre o Notepad dentro dele com um limite de CPU, e consulta informacoes do job.
>
> ```cpp
> #include <windows.h>
> #include <stdio.h>
>
> int main() {
>     HANDLE hJob = CreateJobObjectW(NULL, L"MeuJob");
>
>     // Limita o job a 1 processo ativo
>     JOBOBJECT_BASIC_LIMIT_INFORMATION limite = {};
>     limite.ActiveProcessLimit = 1;
>     limite.LimitFlags = JOB_OBJECT_LIMIT_ACTIVE_PROCESS;
>     SetInformationJobObject(hJob, JobObjectBasicLimitInformation, &limite, sizeof(limite));
>
>     // Cria o Notepad suspenso para poder associar ao job antes de rodar
>     STARTUPINFOW si = { sizeof(si) };
>     PROCESS_INFORMATION pi = {};
>     CreateProcessW(L"C:\\Windows\\System32\\notepad.exe",
>         NULL, NULL, NULL, FALSE,
>         CREATE_SUSPENDED, NULL, NULL, &si, &pi);
>
>     AssignProcessToJobObject(hJob, pi.hProcess);
>     ResumeThread(pi.hThread);
>
>     printf("Notepad rodando no job. PID: %lu\n", pi.dwProcessId);
>
>     // Consulta quantos processos estao no job
>     JOBOBJECT_BASIC_ACCOUNTING_INFORMATION info = {};
>     QueryInformationJobObject(hJob, JobObjectBasicAccountingInformation, &info, sizeof(info), NULL);
>     printf("Processos ativos no job: %lu\n", info.ActiveProcesses);
>
>     WaitForSingleObject(pi.hProcess, INFINITE);
>     CloseHandle(pi.hProcess);
>     CloseHandle(pi.hThread);
>     CloseHandle(hJob);
>
>     return 0;
> }
> ```

### Jobs aninhados

A partir do Windows 8 e Windows Server 2012, um processo pode ser associado a múltiplos jobs, criando efetivamente uma hierarquia de jobs. Os limites de job para um job filho não podem ser mais permissivos do que seu pai, mas podem ser mais restritivos.

O accounting de recursos para um job pai inclui os recursos agregados usados por seus processos gerenciados diretos e todos os processos em jobs filhos. Quando um job é encerrado (`TerminateJobObject`), todos os processos no job e nos jobs filhos são encerrados, começando pelos jobs filhos no fundo da hierarquia.

#### EXPERIMENTO: Visualizando o objeto job

Para ver se um processo está associado a um job, use o comando `!process` do depurador do kernel ou o Process Explorer. No Process Explorer, abra as Opções, escolha Configurar Cores e marque a entrada Jobs para ver processos gerenciados por jobs destacados em marrom.

Para obter um resumo do job no depurador do kernel:

```
lkd> !job <endereço_do_job>
```

Use o flag 2 após o comando `!job` para ver uma lista dos processos que fazem parte do job:

```
lkd> !job <endereço_do_job> 2
```

## Windows Containers (silos de servidor)

A ascensão da computação em nuvem barata e ubíqua levou a uma revolução na Internet, onde construir serviços online e/ou back-ends de servidor para aplicativos móveis é tão fácil quanto clicar em um botão em um dos muitos provedores de nuvem. Microsoft trouxe o Docker para o Windows 10 como parte da Anniversary Update. Ele pode funcionar em dois modos:

1. Implantando um aplicativo em um contêiner Hyper-V pesado, mas totalmente isolado.
2. Implantando um aplicativo em um contêiner server silo, leve e com isolamento de SO.

### Objetos job e silos

A capacidade de criar um silo está associada a um número de subclasses não documentadas como parte da API `SetJobObjectInformation`. Em outras palavras, um silo é essencialmente um super-job, com regras e capacidades adicionais. Jobs que também funcionam como silos são chamados de **hybrid jobs** pelo sistema.

### Isolamento do silo

O primeiro elemento que define um server silo é a existência de um objeto de diretório raiz customizado do gerenciador de objetos (`\`). A capacidade de um server silo ter sua própria raiz significa que todo acesso a qualquer objeto nomeado pode ser controlado. Isso é feito de uma de três formas:

- Criando uma nova cópia de um objeto existente para fornecer acesso alternativo a ele de dentro do silo.
- Criando um link simbólico para um objeto existente para fornecer acesso direto a ele.
- Criando um objeto totalmente novo que existe apenas dentro do silo.

Essa capacidade inicial é então combinada com o serviço Virtual Machine Compute (Vmcompute) (usado pelo Docker), que interage com componentes adicionais:

- Um arquivo WIM de imagem base do Windows chamado **base OS** — Fornece uma cópia separada do sistema operacional (imagem Server Core ou Nano Server).
- A biblioteca Ntdll.dll do SO host — Sobrescreve a da imagem do SO base.
- Um sistema de arquivos virtual em sandbox fornecido pelo driver de filtro Wcifs.sys.
- Um registro virtual em sandbox fornecido pelo componente do kernel VReg.
- O Session Manager (Smss.exe) — Agora usado para criar sessões de serviço ou console adicionais.

### Limites de isolamento do silo

Cada server silo conterá seu próprio isolado:

- **Micro shared user data (SILO_USER_SHARED_DATA)** — Contém o caminho do sistema personalizado, ID de sessão, PID de primeiro plano e tipo/suite de produto.
- **Namespace raiz de diretório de objeto** — Com seu próprio symlink `\SystemRoot`, diretório `\Device`, etc.
- **Mapeamento de API Set** — Baseado no API Set schema do WIM do SO base.
- **Sessão de logon** — Associada com o LUID SYSTEM e Anônimo local, mais o LUID de uma conta de serviço virtual descrevendo o usuário no silo.
- **Contextos de rastreamento ETW e logger** — Para isolar operações ETW ao silo.

### Contextos de silo

Outros componentes dentro do kernel, bem como drivers (incluindo de terceiros), podem adicionar dados contextuais a silos usando a API `PsCreateSiloContext`. O sistema fornece 32 índices de slot de armazenamento em todo o sistema integrados, mais 256 slots de expansão.

Quando um server silo é criado, ele recebe seu próprio array de armazenamento local de silo (SLS), muito como uma thread tem thread-local storage (TLS). Drivers que desejam ser notificados quando novos silos são criados podem usar as APIs de monitor de silo:

- `PsRegisterSiloMonitor` — Registra um monitor de silo.
- `PsStartSiloMonitor` — Inicia o monitor.
- `PsUnregisterSiloMonitor` — Cancela o registro do monitor.

### Criação de um server silo

Quando um server silo é criado, um objeto de job é usado primeiro. Isso é feito através da API padrão `CreateJobObject`, que foi modificada como parte da Anniversary Update para agora ter um ID de job associado (JID). O JID vem do mesmo pool de números que o PID e TID de processo e thread.

Após criar o job, `SetInformationJobObject` é usado com a classe de informação de criação de silo. Após a conversão do silo em server silo, o kernel inicializa a estrutura `ESERVERSILO_GLOBALS` e os monitores de silo registrados recebem notificações para adicionar seus próprios dados de contexto de silo.

O passo final é "inicializar" o server silo com uma nova sessão de serviço. Isso é feito através de uma mensagem ALPC enviada ao `SmApiPort` do Smss. O Smss executará seus deveres habituais, como iniciar Csrss.exe, Wininit.exe, Lsass.exe, etc.

## Conclusão

Este capítulo examinou a estrutura dos processos, incluindo a forma como os processos são criados e destruídos. Vimos como os jobs podem ser usados para gerenciar um grupo de processos como uma unidade e como os server silos podem ser usados para introduzir suporte a containers nas versões do Windows Server. O próximo capítulo investiga as threads — sua estrutura e operação, como são agendadas para execução e as várias formas pelas quais podem ser manipuladas e usadas.
