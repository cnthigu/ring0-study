# Capítulo 2 — Arquitetura do Sistema

> *Tradução do livro Windows Internals, 7ª Edição*  
> *Pavel Yosifovich, Alex Ionescu, Mark E. Russinovich, David A. Solomon*  
> *Microsoft Press, 2017*

---

Agora que você aprendeu os termos, conceitos e ferramentas com os quais precisa estar familiarizado, é hora de começar a explorar os objetivos de design interno e a estrutura do sistema operacional Microsoft Windows. Este capítulo explica a arquitetura geral do sistema — os componentes-chave, como eles interagem entre si e o contexto em que rodam. Para fornecer uma estrutura para entender os internos do Windows, vamos primeiro revisar os requisitos e objetivos que moldaram o design e especificação originais do sistema.

## Requisitos e objetivos de design

Os seguintes requisitos guiaram a especificação do Windows NT em 1989:

- Fornecer um SO de 32 bits verdadeiro, preemptivo, reentrante e com memória virtual.
- Rodar em múltiplas arquiteturas e plataformas de hardware.
- Rodar e escalar bem em sistemas de multiprocessamento simétrico (SMP).
- Ser uma ótima plataforma de computação distribuída, tanto como cliente de rede quanto como servidor.
- Rodar a maioria dos aplicativos existentes de 16 bits para MS-DOS e Microsoft Windows 3.1.
- Atender aos requisitos governamentais de conformidade com POSIX 1003.1.
- Atender aos requisitos governamentais e da indústria para segurança de SO.
- Ser facilmente adaptável ao mercado global com suporte a Unicode.

Para guiar os milhares de decisões que tinham que ser tomadas, a equipe de design do Windows NT adotou os seguintes objetivos de design:

- **Extensibilidade** — O código deve ser escrito para crescer e mudar confortavelmente à medida que os requisitos de mercado mudem.
- **Portabilidade** — O sistema deve ser capaz de rodar em múltiplas arquiteturas de hardware e deve ser capaz de ser movido com relativa facilidade para novas arquiteturas.
- **Confiabilidade e robustez** — O sistema deve se proteger tanto de mau funcionamento interno quanto de adulteração externa. Aplicativos não devem ser capazes de prejudicar o SO ou outros aplicativos.
- **Compatibilidade** — Embora o Windows NT devesse estender a tecnologia existente, sua interface de usuário e APIs deveriam ser compatíveis com versões anteriores do Windows e com o MS-DOS.
- **Desempenho** — Dentro das restrições dos outros objetivos de design, o sistema deve ser tão rápido e responsivo quanto possível em cada plataforma de hardware.

## Modelo do sistema operacional

Na maioria dos sistemas operacionais multiusuário, os aplicativos são separados do próprio SO. O código do kernel do SO roda em um modo privilegiado do processador (chamado modo kernel neste livro), com acesso a dados do sistema e ao hardware. O código do aplicativo roda em um modo não privilegiado (chamado modo usuário), com um conjunto limitado de interfaces disponíveis, acesso limitado a dados do sistema e sem acesso direto ao hardware.

Quando um programa em modo usuário chama um serviço do sistema, o processador executa uma instrução especial que muda a thread chamadora para o modo kernel. Quando o serviço do sistema é concluído, o SO muda o contexto da thread de volta para o modo usuário e permite que o chamador continue.

O Windows é semelhante à maioria dos sistemas UNIX no sentido de que é um SO monolítico — a maior parte do código do SO e de device drivers compartilha o mesmo espaço de memória protegido do modo kernel. O Windows aborda os riscos disso por meio de mecanismos como WHQL e KMCS, além de tecnologias adicionais de proteção do kernel como a segurança baseada em virtualização (VBS) e os recursos Device Guard e Hyper Guard.

Os componentes em modo kernel do Windows também incorporam princípios básicos de design orientado a objetos. Em geral, eles não acessam as estruturas de dados uns dos outros diretamente para acessar informações mantidas por componentes individuais. Em vez disso, usam interfaces formais para passar parâmetros e acessar e/ou modificar estruturas de dados.

Apesar de seu uso abrangente de objetos para representar recursos compartilhados do sistema, o Windows não é um sistema orientado a objetos no sentido estrito. A maior parte do código do SO em modo kernel é escrita em C por razões de portabilidade.

## Visão geral da arquitetura

Os quatro tipos básicos de processos em modo usuário são:

- **Processos de usuário** — Podem ser: Windows de 32 ou 64 bits, Windows 3.1 de 16 bits, MS-DOS de 16 bits, ou POSIX de 32 ou 64 bits.
- **Processos de serviço** — São processos que hospedam serviços do Windows, como o Task Scheduler e o serviço Print Spooler.
- **Processos do sistema** — São processos fixos como o processo de logon e o Session Manager.
- **Processos do servidor do subsistema de ambiente** — Implementam parte do suporte para o ambiente do SO apresentado ao usuário e ao programador.

No Windows, os aplicativos de usuário não chamam os serviços nativos do SO Windows diretamente. Em vez disso, eles passam por uma ou mais subsystem DLLs. O papel das subsystem DLLs é traduzir uma função documentada nos serviços nativos internos (geralmente não documentados) do sistema implementados principalmente em Ntdll.dll.

Os componentes em modo kernel do Windows incluem:

- **Executive** — Contém os serviços base do SO, como gerenciamento de memória, gerenciamento de processos e threads, segurança, I/O, rede e comunicação entre processos.
- **Kernel do Windows** — Consiste em funções de SO de baixo nível, como agendamento de threads, despacho de interrupções e exceções, e sincronização multiprocessador.
- **Device drivers** — Inclui tanto device drivers de hardware quanto drivers que não estão diretamente relacionados ao hardware, como drivers de sistema de arquivos e de rede.
- **A Hardware Abstraction Layer (HAL)** — Uma camada de código que isola o kernel, os device drivers e o restante do executive do Windows de diferenças de hardware específicas da plataforma.
- **O sistema de janelamento e gráficos** — Implementa as funções da interface gráfica do usuário (GUI), como o gerenciamento de janelas, controles de interface de usuário e desenho.
- **A camada do hypervisor** — Composta por um único componente: o próprio hypervisor.

## Portabilidade

O Windows foi projetado para rodar em uma variedade de arquiteturas de hardware. O Windows consegue portabilidade entre arquiteturas e plataformas de hardware de duas formas:

**Por meio de um design em camadas** — O Windows usa um design em camadas, com as partes de baixo nível do sistema que são específicas de arquitetura de processador ou de plataforma isoladas em módulos separados. Os dois componentes-chave que fornecem portabilidade ao SO são o kernel (contido em Ntoskrnl.exe) e a HAL (contida em Hal.dll).

**Por meio do uso de C** — A grande maioria do Windows é escrita em C, com algumas partes em C++. A linguagem Assembly é usada apenas para as partes do SO que precisam se comunicar diretamente com o hardware do sistema (como o handler de trap de interrupção) ou que são extremamente sensíveis ao desempenho.

## Multiprocessamento simétrico

O Windows é um SO de multiprocessamento simétrico (SMP). Não há um processador mestre — o SO bem como as threads de usuário podem ser agendados para rodar em qualquer processador. Todos os processadores compartilham um único espaço de memória. Esse modelo contrasta com o multiprocessamento assimétrico (ASMP), no qual o SO tipicamente seleciona um processador para executar o código do kernel do SO enquanto outros processadores executam apenas código de usuário.

O Windows também suporta quatro tipos modernos de sistemas multiprocessador:

- **Multicore** — Sistemas com múltiplos núcleos físicos no mesmo pacote.
- **Simultaneous Multi-Threaded (SMT)** — Sistemas com Hyper-Threading da Intel ou tecnologia SMT da AMD, que fornecem dois processadores lógicos para cada núcleo físico.
- **Heterogêneo** — Sistemas ARM com tecnologia big.LITTLE, que combina núcleos de processadores mais lentos e de baixo consumo com núcleos mais rápidos.
- **Non-Uniform Memory Access (NUMA)** — Os processadores são agrupados em unidades menores chamadas nós, cada um com seus próprios processadores e memória.

O Windows implementa um construto de ordem superior chamado **grupo de processadores** (processor group) para suportar sistemas maiores. Um grupo de processadores é um conjunto de processadores que podem todos ser definidos por uma única máscara de afinidade (affinity bitmask). O sistema suporta atualmente até 20 grupos, com um máximo de 640 processadores lógicos.

> **Rode no Visual Studio**
> `GetSystemInfo` retorna informações sobre a plataforma: numero de processadores, tamanho de pagina e arquitetura do processador.
>
> ```cpp
> #include <windows.h>
> #include <stdio.h>
>
> int main() {
>     SYSTEM_INFO info;
>     GetSystemInfo(&info);
>
>     printf("Processadores logicos : %lu\n", info.dwNumberOfProcessors);
>     printf("Tamanho de pagina     : %lu bytes\n", info.dwPageSize);
>     printf("Arquitetura           : %u\n", info.wProcessorArchitecture);
>     // 9 = x64, 5 = ARM, 12 = ARM64, 0 = x86
>
>     return 0;
> }
> ```

## Escalabilidade

O Windows incorpora vários recursos cruciais para seu sucesso como SO multiprocessador:

- A capacidade de rodar código do SO em qualquer processador disponível e em múltiplos processadores ao mesmo tempo.
- Múltiplas threads de execução dentro de um único processo, cada uma das quais pode ser executada simultaneamente em diferentes processadores.
- Sincronização refinada dentro do kernel (como spinlocks, queued spinlocks e pushlocks) para permitir que mais componentes rodem simultaneamente em múltiplos processadores.
- Mecanismos de programação como I/O completion ports que facilitam a implementação eficiente de processos de servidor multi-thread que podem escalar bem em sistemas multiprocessador.

## Diferenças entre versões cliente e servidor

O Windows é vendido tanto em pacotes de varejo cliente quanto servidor. As versões cliente e servidor diferem em:

- Preços baseados em núcleos para as edições Datacenter e Standard do Server 2016.
- O número total de processadores lógicos suportados.
- A quantidade de memória física suportada.
- O número de conexões de rede simultâneas suportadas.
- Suporte a recursos como BitLocker, boot em VHD, AppLocker, Hyper-V, etc.

Embora haja várias edições de varejo cliente e servidor do Windows, elas compartilham um conjunto comum de arquivos de sistema central, incluindo a imagem do kernel Ntoskrnl.exe, as bibliotecas HAL, os device drivers e os utilitários e DLLs do sistema base.

Como o sistema sabe qual edição foi inicializada? Consultando os valores do registro `ProductType` e `ProductSuite` sob a chave `HKLM\SYSTEM\CurrentControlSet\Control\ProductOptions`.

Os sistemas servidor são otimizados por padrão para throughput do sistema como servidores de aplicativos de alto desempenho, enquanto a versão cliente é otimizada para tempo de resposta para uso interativo de desktop.

> **Rode no Visual Studio**
> O sistema armazena o tipo do produto no registro. `ProductType` retorna "WinNT" para cliente, "ServerNT" para servidor dedicado e "LanmanNT" para controlador de dominio.
>
> ```cpp
> #include <windows.h>
> #include <stdio.h>
>
> int main() {
>     HKEY hKey;
>     if (RegOpenKeyExW(HKEY_LOCAL_MACHINE,
>         L"SYSTEM\\CurrentControlSet\\Control\\ProductOptions",
>         0, KEY_READ, &hKey) == ERROR_SUCCESS) {
>
>         WCHAR tipo[64];
>         DWORD tamanho = sizeof(tipo);
>         RegQueryValueExW(hKey, L"ProductType", NULL, NULL, (LPBYTE)tipo, &tamanho);
>         printf("ProductType: %ls\n", tipo);
>         RegCloseKey(hKey);
>     }
>
>     return 0;
> }
> ```

## Visão geral da arquitetura de segurança baseada em virtualização (VBS)

A separação entre modo usuário e modo kernel protege o SO do código em modo usuário. No entanto, se um código em modo kernel indesejado entrar no sistema (devido a uma vulnerabilidade não corrigida ou porque o usuário foi induzido a instalar um driver malicioso), o sistema fica essencialmente comprometido porque todo código em modo kernel tem acesso completo ao sistema inteiro.

As tecnologias que aproveitam o hypervisor para fornecer garantias adicionais contra ataques constituem um conjunto de capacidades de **segurança baseada em virtualização (VBS)**, estendendo a separação natural baseada em privilégios do processador por meio da introdução de Virtual Trust Levels (VTLs).

Com VBS habilitado, um VTL de nível 1 agora está presente, que contém seu próprio **secure kernel** rodando no modo privilegiado do processador (ring 0 no x86/x64). Similarmente, existe um ambiente de modo usuário em tempo de execução chamado **Isolated User Mode (IUM)**, que roda no modo não privilegiado (ring 3).

Nessa arquitetura, o secure kernel é seu próprio binário separado, encontrado com o nome `securekernel.exe` no disco. O IUM usa bibliotecas especiais: `Iumdll.dll` (a versão VTL 1 de Ntdll.dll) e `Iumbase.dll` (a versão VTL 1 de Kernelbase.dll).

O VTL 1 tem acesso ao VTL 0 mas é isolado de modificações provenientes do VTL 0. Uma forma simples de pensar sobre isso: os níveis de privilégio (usuário vs. kernel) impõem poder. Os VTLs, por outro lado, impõem isolamento.

Apenas uma classe especial de binários assinados, chamados **Trustlets**, tem permissão para executar no VTL 1. Cada Trustlet tem um identificador único e assinatura, e o secure kernel tem conhecimento codificado de quais Trustlets foram criados.

## Componentes-chave do sistema

### Subsistemas de ambiente e DLLs de subsistema

O papel de um subsistema de ambiente é expor algum subconjunto dos serviços base do executive do Windows para programas de aplicativos. Cada imagem executável (.exe) está vinculada a exatamente um e somente um subsistema.

O subsistema Windows consiste nos seguintes componentes principais:

- Para cada sessão, uma instância do processo do subsistema de ambiente (Csrss.exe) carrega quatro DLLs (Basesrv.dll, Winsrv.dll, Sxssrv.dll e Csrsrv.dll) que contêm suporte para várias tarefas de manutenção.
- Um device driver em modo kernel (Win32k.sys) que contém o gerenciador de janelas (window manager) e a Graphics Device Interface (GDI).
- O processo host do console (Conhost.exe), que fornece suporte para aplicativos de console.
- O Desktop Window Manager (Dwm.exe), que permite a composição de renderização de janelas visíveis em uma única superfície.
- DLLs de subsistema (como Kernel32.dll, Advapi32.dll, User32.dll e Gdi32.dll) que traduzem as funções documentadas da Windows API nos serviços de sistema em modo kernel em Ntoskrnl.exe e Win32k.sys.

#### Windows 10 e Win32k.sys

Os requisitos básicos de gerenciamento de janelas para dispositivos baseados em Windows 10 variam consideravelmente dependendo do dispositivo. Por isso, a funcionalidade do Win32k.sys foi dividida entre vários módulos do kernel:

- Em telefones (Windows Mobile 10): Win32k.sys carrega Win32kMin.sys e Win32kBase.sys.
- Em sistemas desktop completos: Win32k.sys carrega Win32kBase.sys e Win32kFull.sys.
- Em certos sistemas IoT: Win32k.sys pode precisar apenas de Win32kBase.sys.

#### Provedor Pico e o Subsistema Windows para Linux (WSL)

O modelo tradicional de subsistema tem duas desvantagens técnicas importantes que dificultam o uso amplo de binários não-Windows. Para resolver isso, a Microsoft implementou o **modelo Pico**.

Sob esse modelo, um **Pico provider** é um driver de modo kernel personalizado que recebe acesso a interfaces especializadas do kernel por meio da API `PsRegisterPicoProvider`. Com o Windows 10 versão 1607, um tal Pico provider está presente: Lxss.sys e seu parceiro Lxcore.sys — o componente do Windows Subsystem for Linux (WSL).

Os processos Pico rodando sob o Pico provider do WSL são muito diferentes dos processos normais do Windows — não possuem, por exemplo, a Ntdll.dll que é sempre carregada em processos normais. Em vez disso, sua memória contém estruturas como um vDSO, uma imagem especial vista apenas em sistemas Linux/BSD.

#### Console host

A partir do Windows 8, o processo Conhost.exe é gerado a partir do processo baseado em console (em vez de a partir do Csrss.exe, como no Windows 7) pelo driver do console (`\Windows\System32\Drivers\ConDrv.sys`). O processo que usa o console se comunica com o Conhost.exe por meio do driver do console, enviando tipos de I/O como leitura, escrita e controle de I/O.

### Ntdll.dll

O Ntdll.dll é uma biblioteca especial de suporte ao sistema usada principalmente por DLLs de subsistema e aplicativos nativos. Ele contém dois tipos de funções:

- **Stubs de despacho de serviços do sistema** para os serviços do executive do Windows — Há mais de 450 tais funções, como `NtCreateFile`, `NtSetEvent`, etc.
- **Funções de suporte interno** usadas por subsistemas, DLLs de subsistema e outras imagens nativas.

Para cada uma das funções de serviço do sistema, o Ntdll.dll contém um ponto de entrada com o mesmo nome. O código dentro da função contém a instrução específica da arquitetura que causa uma transição para o modo kernel para invocar o despachante de serviço do sistema.

#### EXPERIMENTO: Visualizando o código do despachante de serviço do sistema

Abra a versão do WinDbg que corresponde à arquitetura do seu sistema. Em seguida, abra o menu Arquivo e selecione Abrir Executável. Navegue até `%SystemRoot%\System32` e selecione Notepad.exe.

Defina um breakpoint em NtCreateFile dentro de Ntdll.dll:
```
bp ntdll!ntcreatefile
```

Entre o comando `g` ou pressione F5 para deixar o Notepad continuar a execução. O despachante deve parar. Em seguida, use o comando `u` para ver algumas instruções à frente:

```
00007ffa'9f4e5b10  mov r10,rcx
00007ffa'9f4e5b13  mov eax,55h          ; número do serviço do sistema
00007ffa'9f4e5b22  syscall              ; transição para modo kernel
00007ffa'9f4e5b24  ret
```

O registro EAX é definido com o número do serviço do sistema (55 hex neste caso). Então note a instrução `syscall`, que é o que causa a transição do processador para o modo kernel.

### Executive

O executive do Windows é a camada superior do Ntoskrnl.exe. Inclui os seguintes componentes principais:

- **Gerenciador de configuração (Configuration manager)** — Responsável por implementar e gerenciar o registro do sistema.
- **Gerenciador de processos (Process manager)** — Cria e termina processos e threads.
- **Security Reference Monitor (SRM)** — Aplica políticas de segurança no computador local.
- **Gerenciador de I/O (I/O manager)** — Implementa I/O independente de dispositivo e é responsável por despachar para os device drivers apropriados.
- **Gerenciador de Plug and Play (PnP manager)** — Determina quais drivers são necessários para suportar um dispositivo específico e carrega esses drivers.
- **Gerenciador de energia (Power manager)** — Coordena eventos de energia e gera notificações de I/O de gerenciamento de energia para device drivers.
- **Gerenciador de memória (Memory manager)** — Implementa memória virtual, um esquema de gerenciamento de memória que fornece um grande espaço de endereçamento privado para cada processo.
- **Gerenciador de cache (Cache manager)** — Melhora o desempenho do I/O baseado em arquivo fazendo com que dados de disco recentemente referenciados residam na memória principal para acesso rápido.

O executive também contém quatro grupos principais de funções de suporte:

- **Gerenciador de objetos (Object manager)** — Cria, gerencia e exclui objetos do executive do Windows.
- **Facilidade ALPC (Asynchronous LPC)** — Passa mensagens entre um processo cliente e um processo servidor no mesmo computador.
- **Funções da biblioteca de tempo de execução (Run-time library)** — Incluem processamento de strings, operações aritméticas, conversão de tipos de dados e processamento de estruturas de segurança.
- **Rotinas de suporte do executive** — Incluem alocação de memória do sistema, acesso de memória interligado e tipos especiais de mecanismos de sincronização.

### Kernel

O kernel consiste em um conjunto de funções em Ntoskrnl.exe que fornece mecanismos fundamentais. Esses incluem serviços de agendamento de threads e sincronização, usados pelos componentes do executive, e suporte de baixo nível específico da arquitetura de hardware, como despacho de interrupções e exceções.

O kernel separa a si mesmo do restante do executive implementando mecanismos do SO e evitando tomar decisões de política. Ele delega quase todas as decisões de política ao executive, com exceção do agendamento e despacho de threads.

#### Objetos do kernel

O kernel implementa um conjunto de objetos mais simples, chamados **objetos do kernel**, que ajudam o kernel a controlar o processamento central e suportar a criação de objetos do executive.

- **Objetos de controle** — Estabelecem semântica para controlar várias funções do SO, incluindo o objeto APC (Asynchronous Procedure Call), o objeto DPC (Deferred Procedure Call) e os objetos de interrupção.
- **Objetos despachantes (Dispatcher objects)** — Incorporam capacidades de sincronização que alteram ou afetam o agendamento de threads. Incluem a thread do kernel, mutex (chamado mutant na terminologia do kernel), evento, par de eventos do kernel, semáforo, timer e timer aguardável.

#### Região de controle de processador do kernel (KPCR) e bloco de controle (KPRCB)

O kernel usa uma estrutura de dados chamada **kernel processor control region (KPCR)** para armazenar dados específicos do processador. O KPCR contém informações básicas como a interrupt dispatch table (IDT), task state segment (TSS) e global descriptor table (GDT) do processador.

O KPCR também contém uma estrutura de dados incorporada chamada **kernel processor control block (KPRCB)**, que é uma estrutura privada usada apenas pelo código do kernel em Ntoskrnl.exe. Contém informações de agendamento, o banco de dados do despachante, filas DPC, informações de CPU e NUMA, tamanhos de cache e informações de contabilidade de tempo.

#### EXPERIMENTO: Visualizando o KPCR e KPRCB

Você pode visualizar o conteúdo do KPCR e KPRCB usando os comandos `!pcr` e `!prcb` do depurador do kernel:

```
lkd> dt nt!_KPCR @$pcr
lkd> !prcb
```

Para determinar a velocidade do processador como detectada na inicialização:

```
lkd> dt nt!_KPRCB fffff803c3b23180 MHz
```

### Hardware Abstraction Layer (HAL)

A HAL é um módulo carregável em modo kernel (Hal.dll) que fornece a interface de baixo nível para a plataforma de hardware na qual o Windows está rodando. Ela oculta detalhes dependentes de hardware como interfaces de I/O, controladores de interrupção e mecanismos de comunicação multiprocessador.

Em vez de acessar o hardware diretamente, os componentes internos do Windows e os device drivers escritos por usuários mantêm a portabilidade chamando rotinas da HAL quando precisam de informações dependentes da plataforma.

Em máquinas x64 e ARM, há apenas uma imagem HAL, chamada Hal.dll. O Windows agora suporta módulos conhecidos como **HAL extensions**, que são DLLs adicionais no disco que o boot loader pode carregar se hardware específico que as requeira for necessário. Criar HAL extensions requer colaboração com a Microsoft e tais arquivos devem ser assinados com um certificado especial de extensão HAL disponível apenas para fornecedores de hardware.

### Device drivers

Device drivers são módulos carregáveis em modo kernel (arquivos tipicamente com extensão .sys) que fazem interface entre o gerenciador de I/O e o hardware relevante. Eles rodam em modo kernel em um de três contextos:

- No contexto da thread de usuário que iniciou uma função de I/O.
- No contexto de uma thread de sistema em modo kernel.
- Como resultado de uma interrupção.

Há vários tipos de device drivers:

- **Hardware device drivers** — Usam a HAL para manipular hardware para escrever saída ou recuperar entrada de um dispositivo físico ou rede.
- **File system drivers** — Aceitam solicitações de I/O orientadas a arquivo e as traduzem em solicitações de I/O vinculadas a um dispositivo específico.
- **File system filter drivers** — Interceptam solicitações de I/O e realizam algum processamento de valor agregado antes de passar o I/O para a próxima camada.
- **Network redirectors and servers** — São file system drivers que transmitem/recebem solicitações de I/O de arquivo para uma máquina na rede.
- **Protocol drivers** — Implementam um protocolo de rede como TCP/IP, NetBEUI e IPX/SPX.
- **Kernel streaming filter drivers** — São encadeados para realizar processamento de sinal em fluxos de dados, como gravação ou exibição de áudio e vídeo.
- **Software drivers** — Módulos do kernel que realizam operações que só podem ser feitas em modo kernel em nome de algum processo em modo usuário.

#### Windows Driver Model (WDM)

O WDM fornece um modelo de driver comum. Do ponto de vista do WDM, há três tipos de drivers:

- **Bus drivers** — Atendem um controlador de barramento, adaptador, bridge ou qualquer dispositivo que tenha dispositivos filhos. São drivers obrigatórios.
- **Function drivers** — São o driver de dispositivo principal e fornecem a interface operacional para seu dispositivo.
- **Filter drivers** — São usados para adicionar funcionalidade a um dispositivo ou driver existente.

#### Windows Driver Foundation (WDF)

O WDF simplifica o desenvolvimento de drivers Windows fornecendo dois frameworks:

- **Kernel-Mode Driver Framework (KMDF)** — Fornece uma interface simples para WDM e oculta sua complexidade do escritor de driver. Em alguns casos, mais de 200 linhas de código WDM podem ser substituídas por uma única chamada de função KMDF.
- **User-Mode Driver Framework (UMDF)** — Permite que certas classes de drivers sejam implementadas como drivers em modo usuário. Se um driver UMDF travar, o processo morre e geralmente é reiniciado, sem desestabilizar o sistema.

O WDF foi disponibilizado como código aberto (open source) pela Microsoft e está disponível no GitHub em https://github.com/Microsoft/Windows-Driver-Frameworks.

#### Drivers universais do Windows (Universal Windows drivers)

A partir do Windows 10, os Universal Windows drivers podem ser escritos uma vez e compartilhar APIs e Device Driver Interfaces (DDIs) em dispositivos que variam desde dispositivos IoT a telefones, HoloLens e Xbox One até laptops e desktops.

## Processos do sistema

Os seguintes processos do sistema aparecem em todo sistema Windows 10:

- **Processo Idle** — Contém uma thread por CPU para contabilizar o tempo de CPU ocioso.
- **Processo System** — Contém a maioria das threads do sistema em modo kernel e handles.
- **Processo Secure System** — Contém o espaço de endereçamento do secure kernel no VTL 1, se estiver rodando.
- **Processo Memory Compression** — Contém o working set comprimido de processos em modo usuário.
- **Session Manager (Smss.exe)** — Primeiro processo em modo usuário criado no sistema.
- **Subsistema Windows (Csrss.exe)** — Processo do subsistema Client/Server Runtime.
- **Inicialização da sessão 0 (Wininit.exe)** — Realiza funções de inicialização do sistema.
- **Processo de logon (Winlogon.exe)** — Trata logons e logoffs de usuário interativos.
- **Service Control Manager (Services.exe)** — Responsável por iniciar, parar e interagir com processos de serviço.
- **Local Security Authentication Service (Lsass.exe)** — Serviço de autenticação de segurança local.

### Session Manager (Smss.exe)

O Session Manager é o primeiro processo em modo usuário criado no sistema, criado diretamente pelo kernel. É criado como um Protected Process Light (PPL).

As principais etapas de inicialização do Smss.exe incluem:

1. Criar porta ALPC chamada `\SmApiPort` para receber comandos.
2. Criar diretório objeto raiz `\Sessions` no namespace do gerenciador de objetos.
3. Executar os programas da lista BootExecute (por padrão, Autochk.exe, que realiza uma verificação de disco).
4. Inicializar o restante do registro (hives HKLM software, SAM e security).
5. Inicializar arquivo(s) de paginação com base nas configurações do registro.
6. Criar uma instância do Smss.exe para inicializar a sessão 0 (sessão não interativa).
7. Criar uma instância do Smss.exe para inicializar a sessão 1 (sessão interativa).

### Processo de inicialização do Windows (Wininit.exe)

O Wininit.exe realiza as seguintes funções de inicialização do sistema:

1. Cria uma janela station (sempre chamada Winsta0) e dois desktops (Winlogon e Default) para processos rodarem na sessão 0.
2. Inicializa a chave de criptografia de máquina LSA.
3. Cria o Service Control Manager (SCM ou Services.exe).
4. Inicia o Local Security Authentication Subsystem Service (Lsass.exe) e, se o Credential Guard estiver habilitado, o Isolated LSA Trustlet (Lsaiso.exe).

### Serviços e Service Control Manager (SCM)

Serviços são como processos daemon do Linux — podem ser configurados para iniciar automaticamente na inicialização do sistema sem requerer um logon interativo. O Service Control Manager (SCM) é um processo especial do sistema rodando a imagem `%SystemRoot%\System32\Services.exe` que é responsável por iniciar, parar e interagir com processos de serviço.

Os serviços têm três nomes: o nome do processo visível no sistema, o nome interno no registro e o nome de exibição mostrado na ferramenta administrativa Serviços. Os serviços são definidos no registro sob `HKLM\SYSTEM\CurrentControlSet\Services`.

#### EXPERIMENTO: Listando serviços instalados

Para listar os serviços instalados, abra o Painel de Controle, selecione Ferramentas Administrativas e selecione Serviços. Alternativamente, clique em Iniciar e execute `services.msc`.

Para ver os serviços dentro de processos de serviço usando o Process Explorer: processos hospedando um ou mais serviços são destacados por padrão em rosa. Se você clicar duas vezes em um processo que hospeda serviços, você verá uma aba Serviços listando os serviços dentro do processo.

### Winlogon, LogonUI e Userinit

O processo de logon do Windows (`%SystemRoot%\System32\Winlogon.exe`) trata logons e logoffs de usuário interativos. O Winlogon é notificado de uma solicitação de logon de usuário quando o usuário entra na sequência de atenção segura (SAS) — o padrão no Windows é Ctrl+Alt+Delete.

Os aspectos de identificação e autenticação do processo de logon são implementados por meio de DLLs chamadas **credential providers**. Os provedores de credenciais padrão do Windows implementam as interfaces de autenticação padrão: senha e cartão inteligente. O Windows 10 fornece um provedor de credenciais biométrico: reconhecimento facial, conhecido como **Windows Hello**.

Após o nome de usuário e senha serem capturados, eles são enviados ao Local Security Authentication Service (Lsass.exe) para serem autenticados. Após autenticação bem-sucedida, o Lsass.exe gera um **objeto de token de acesso** (access token) que contém o perfil de segurança do usuário.

O Userinit.exe realiza alguma inicialização do ambiente do usuário, como executar o script de login e reestabelecer conexões de rede. Em seguida, ele procura no registro o valor Shell e cria um processo para rodar o shell definido pelo sistema (por padrão, Explorer.exe). Então o Userinit.exe sai — é por isso que o Explorer é mostrado sem pai.

## Conclusão

Este capítulo ofereceu uma visão ampla da arquitetura geral do sistema do Windows. Examinou os componentes-chave do Windows e mostrou como eles se inter-relacionam. No próximo capítulo, examinaremos com mais detalhes os processos, que são uma das entidades mais básicas no Windows.
