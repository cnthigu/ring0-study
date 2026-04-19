# Capítulo 1 — Conceitos e Ferramentas

> *Tradução do livro Windows Internals, 7ª Edição*  
> *Pavel Yosifovich, Alex Ionescu, Mark E. Russinovich, David A. Solomon*  
> *Microsoft Press, 2017*

---

Neste capítulo, apresentaremos os principais conceitos e termos do sistema operacional (SO) Microsoft Windows que usaremos ao longo deste livro, como a Windows API, processos, threads, memória virtual (virtual memory), modo kernel e modo usuário (kernel mode e user mode), objetos, handles, segurança e o registro (registry). Também apresentaremos as ferramentas que você pode usar para explorar os internos do Windows, como o depurador do kernel (kernel debugger), o Monitor de Desempenho (Performance Monitor) e as principais ferramentas do Windows Sysinternals (http://www.microsoft.com/technet/sysinternals). Além disso, explicaremos como você pode usar o Windows Driver Kit (WDK) e o Windows Software Development Kit (SDK) como recursos para encontrar mais informações sobre os internos do Windows.

Certifique-se de entender tudo neste capítulo; o restante do livro foi escrito presumindo que você o fez.

## Versões do sistema operacional Windows

Este livro cobre a versão mais recente dos sistemas operacionais cliente e servidor Microsoft Windows: Windows 10 (32 bits em x86 e ARM, e versão 64 bits em x64) e Windows Server 2012 R2 (que existe apenas em versão 64 bits). Salvo indicação específica, o texto se aplica a todas as versões. Como informação de contexto, a Tabela 1-1 lista os nomes dos produtos Windows, seu número de versão interno e sua data de lançamento.

**TABELA 1-1 Lançamentos do sistema operacional Windows**

Os números de versão parecem ter se desviado de um caminho bem definido a partir do Windows 7. Seu número de versão era 6.1 e não 7. Por causa da popularidade do Windows XP, quando o Windows Vista aumentou o número de versão para 6.0, alguns aplicativos falharam em detectar o SO correto porque os desenvolvedores verificavam números principais maiores ou iguais a 5 e números secundários maiores ou iguais a 1, o que não era o caso do Windows Vista. Tendo aprendido a lição, a Microsoft optou por manter o número de versão principal como 6 e o número secundário como 2 (maior que 1) para minimizar tais incompatibilidades. No entanto, com o Windows 10, o número de versão foi atualizado para 10.0.

> **Nota**  
> A partir do Windows 8, a função da Windows API `GetVersionEx` retorna o número de versão do SO como 6.2 (Windows 8) por padrão, independentemente do SO real. (A função também foi declarada como obsoleta.) Isso é feito para minimizar problemas de compatibilidade, mas também como um indicador de que verificar a versão do SO não é a melhor abordagem na maioria dos casos. Isso porque alguns componentes podem ser instalados fora de banda, sem coincidir com um lançamento oficial do Windows. Mesmo assim, se você precisar da versão real do SO, pode obtê-la indiretamente usando a função `VerifyVersionInfo` ou as APIs auxiliares de versão mais recentes, como `IsWindows8OrGreater`, `IsWindows8Point1OrGreater`, `IsWindows10OrGreater`, `IsWindowsServer` e similares. Além disso, a compatibilidade do SO pode ser indicada no manifesto do executável, o que altera os resultados dessa função. (Veja o Capítulo 8, "Mecanismos do sistema", em Windows Internals Parte 2 para detalhes.)

Você pode visualizar as informações de versão do Windows usando a ferramenta de linha de comando `ver` ou graficamente executando `winver`.

> **Rode no Visual Studio**
> O trecho abaixo mostra como detectar a versão do Windows da forma correta, usando as helpers modernas em vez de `GetVersionEx`.
>
> ```cpp
> #include <windows.h>
> #include <VersionHelpers.h>
> #include <stdio.h>
>
> int main() {
>     if (IsWindows10OrGreater())
>         printf("Windows 10 ou superior\n");
>     else if (IsWindows8Point1OrGreater())
>         printf("Windows 8.1\n");
>     else
>         printf("Versao anterior ao Windows 8.1\n");
>
>     return 0;
> }
> ```

### Windows 10 e versões futuras do Windows

Com o Windows 10, a Microsoft declarou que atualizará o Windows em um ritmo mais rápido do que antes. Não haverá um "Windows 11" oficial; em vez disso, o Windows Update (ou outro modelo de manutenção empresarial) atualizará o Windows 10 existente para uma nova versão. No momento da escrita, duas dessas atualizações já ocorreram: em novembro de 2015 (também conhecida como versão 1511, referindo-se ao ano e mês de manutenção) e em julho de 2016 (versão 1607, também conhecida pelo nome de marketing Anniversary Update).

> **Nota**  
> Internamente, a Microsoft ainda constrói versões do Windows em ondas. Por exemplo, o lançamento inicial do Windows 10 tinha o codinome Threshold 1, enquanto a atualização de novembro de 2015 foi chamada de Threshold 2. As próximas três fases de atualização são chamadas de Redstone 1 (versão 1607), seguida de Redstone 2 e Redstone 3.

### Windows 10 e OneCore

Ao longo dos anos, diversas variantes do Windows evoluíram. Além do Windows convencional para PCs, existe o console de jogos Xbox 360, que roda uma derivação do Windows 2000. O Windows Phone 7 roda uma variante baseada no Windows CE (o SO de tempo real da Microsoft). Manter e estender todas essas bases de código é claramente difícil. Por isso, a Microsoft decidiu convergir os kernels e os binários de suporte da plataforma base em um único componente. Isso começou com o Windows 8 e o Windows Phone 8 compartilhando um kernel (e o Windows 8.1 e Windows Phone 8.1 tendo uma Windows Runtime API convergida). Com o Windows 10, a convergência está completa; essa plataforma compartilhada é conhecida como **OneCore**, e roda em PCs, telefones, no console Xbox One, no HoloLens e em dispositivos de Internet das Coisas (IoT) como o Raspberry Pi 2.

Claramente, todos esses fatores de forma de dispositivos são muito diferentes uns dos outros. Algumas funcionalidades simplesmente não existem em alguns dispositivos. Por exemplo, suportar mouse ou teclado físico em um dispositivo HoloLens pode não fazer sentido, então você não pode esperar que essas partes estejam presentes na versão do Windows 10 para tal dispositivo. Mas o kernel, os drivers e os binários da plataforma base são essencialmente os mesmos (com configurações baseadas em registro e/ou políticas onde fazem sentido por razões de desempenho ou outras). Você verá um exemplo dessa política na seção "API Sets" no Capítulo 3, "Processos e jobs".

Este livro mergulha nos internos do kernel OneCore, em qualquer dispositivo em que esteja rodando. Os experimentos do livro, no entanto, são direcionados a uma máquina desktop com mouse e teclado, principalmente por conveniência, pois não é fácil (e às vezes é oficialmente impossível) realizar os experimentos em outros dispositivos como telefones ou o Xbox One.

## Conceitos e termos fundamentais

As seções a seguir apresentam os conceitos mais fundamentais do Windows, que são essenciais para entender os tópicos discutidos no restante do livro. Muitos dos conceitos, como processos, threads e memória virtual, são discutidos em extensão nos capítulos subsequentes.

### Windows API

A interface de programação de aplicativos do Windows (Windows API) é a interface de programação de sistema em modo usuário (user mode) para a família de SOs Windows. Antes da introdução das versões de 64 bits do Windows, a interface de programação para as versões de 32 bits era chamada de Win32 API para distingui-la da API original de 16 bits do Windows. Neste livro, o termo Windows API refere-se tanto às interfaces de programação de 32 bits quanto de 64 bits do Windows.

> **Nota**  
> Às vezes usamos o termo Win32 API no lugar de Windows API. De qualquer forma, ainda se refere às variantes de 32 e 64 bits.

#### Variantes da Windows API

A Windows API originalmente consistia apenas em funções no estilo C. Hoje, milhares dessas funções existem para uso pelos desenvolvedores. C era a escolha natural na época da criação do Windows porque era o mínimo denominador comum (podia ser acessado de outras linguagens também) e era suficientemente de baixo nível para expor os serviços do SO. A desvantagem era o número enorme de funções aliado à falta de consistência de nomenclatura e agrupamentos lógicos (por exemplo, namespaces do C++). Um resultado dessas dificuldades foi que algumas APIs mais recentes usam um mecanismo diferente: o Component Object Model (COM).

O COM foi originalmente criado para permitir que aplicativos do Microsoft Office se comunicassem e trocassem dados entre documentos (como incorporar um gráfico do Excel em um documento do Word ou em uma apresentação do PowerPoint). Essa capacidade é chamada de Object Linking and Embedding (OLE). O OLE foi originalmente implementado usando um mecanismo de mensagens antigo do Windows chamado Dynamic Data Exchange (DDE). O DDE era inerentemente limitado, razão pela qual uma nova forma de comunicação foi desenvolvida: o COM. De fato, o COM inicialmente era chamado de OLE 2, lançado ao público por volta de 1993.

O COM é baseado em dois princípios fundamentais. Primeiro, os clientes se comunicam com objetos (às vezes chamados de objetos servidores COM) por meio de interfaces — contratos bem definidos com um conjunto de métodos logicamente relacionados agrupados sob o mecanismo de despacho de tabela virtual, que também é uma maneira comum de os compiladores C++ implementarem o despacho de funções virtuais. Isso resulta em compatibilidade binária e eliminação de problemas de name mangling do compilador. Consequentemente, é possível chamar esses métodos de muitas linguagens (e compiladores), como C, C++, Visual Basic, linguagens .NET, Delphi e outras. O segundo princípio é que a implementação do componente é carregada dinamicamente em vez de ser estaticamente vinculada ao cliente. O termo servidor COM tipicamente se refere a uma Dynamic Link Library (DLL) ou um executável (EXE) onde as classes COM são implementadas.

> **Nota**  
> Exemplos de APIs acessadas por COM incluem DirectShow, Windows Media Foundation, DirectX, DirectComposition, Windows Imaging Component (WIC) e o Background Intelligent Transfer Service (BITS).

#### O Windows Runtime

O Windows 8 introduziu uma nova API e runtime de suporte chamada Windows Runtime (às vezes abreviada WinRT, não confundir com Windows RT, a versão descontinuada do Windows baseada em ARM). O Windows Runtime consiste em serviços de plataforma destinados particularmente a desenvolvedores de aplicativos para os chamados Windows Apps (anteriormente conhecidos como Metro Apps, Modern Apps, Immersive Apps e Windows Store Apps). Os Windows Apps podem ter como alvo múltiplos fatores de forma de dispositivos, desde pequenos dispositivos IoT até telefones, tablets, laptops, desktops e até dispositivos como o Xbox One e o Microsoft HoloLens.

Do ponto de vista da API, o WinRT é construído sobre o COM, adicionando várias extensões à infraestrutura COM base. Por exemplo, metadados de tipo completos estão disponíveis no WinRT (armazenados em arquivos WINMD e baseados no formato de metadados do .NET). Do ponto de vista do design da API, é muito mais coeso do que as funções clássicas da Windows API, com hierarquias de namespace, nomenclatura consistente e padrões programáticos.

#### O .NET Framework

O .NET Framework faz parte do Windows. O .NET Framework consiste em dois componentes principais:

- **O Common Language Runtime (CLR)** — Este é o mecanismo de execução para .NET e inclui um compilador Just In Time (JIT) que traduz instruções do Common Intermediate Language (CIL) para a linguagem de máquina do hardware de CPU subjacente, um coletor de lixo (garbage collector), verificação de tipos, segurança de acesso a código e muito mais. É implementado como um servidor em processo (DLL) COM e usa vários recursos fornecidos pela Windows API.

- **A .NET Framework Class Library (FCL)** — Esta é uma grande coleção de tipos que implementam funcionalidades tipicamente necessárias por aplicativos cliente e servidor, como serviços de interface de usuário, rede, acesso a banco de dados e muito mais.

### Serviços, funções e rotinas

Vários termos na documentação de usuário e programação do Windows têm diferentes significados em diferentes contextos. Por exemplo, a palavra serviço pode se referir a uma rotina chamável no SO, um device driver ou um processo servidor. A lista a seguir descreve o que certos termos significam neste livro:

- **Funções da Windows API** — São sub-rotinas documentadas e chamáveis na Windows API. Exemplos incluem `CreateProcess`, `CreateFile` e `GetMessage`.

- **Serviços nativos do sistema (ou chamadas de sistema)** — São os serviços não documentados e subjacentes no SO que são chamáveis a partir do modo usuário. Por exemplo, `NtCreateUserProcess` é o serviço interno do sistema que a função `CreateProcess` do Windows chama para criar um novo processo.

- **Funções de suporte do kernel (ou rotinas)** — São as sub-rotinas dentro do SO Windows que só podem ser chamadas a partir do modo kernel. Por exemplo, `ExAllocatePoolWithTag` é a rotina que os device drivers chamam para alocar memória a partir dos heaps do sistema Windows (chamados de pools).

- **Serviços do Windows** — São processos iniciados pelo gerenciador de controle de serviços (service control manager) do Windows. Por exemplo, o serviço Task Scheduler roda em um processo em modo usuário que suporta o comando `schtasks`.

- **Dynamic link libraries (DLLs)** — São sub-rotinas chamáveis vinculadas juntas como um arquivo binário que pode ser carregado dinamicamente por aplicativos que usam as sub-rotinas. Exemplos incluem Msvcrt.dll (a biblioteca de tempo de execução C) e Kernel32.dll (uma das bibliotecas de subsistema da Windows API).

### Processos

Embora programas e processos pareçam similares na superfície, eles são fundamentalmente diferentes. Um programa é uma sequência estática de instruções, enquanto um processo é um contêiner para um conjunto de recursos usados ao executar a instância do programa. No nível mais alto de abstração, um processo Windows compreende:

- **Um espaço de endereçamento virtual privado (private virtual address space)** — Um conjunto de endereços de memória virtual que o processo pode usar.
- **Um programa executável** — Define o código e os dados iniciais e é mapeado no espaço de endereçamento virtual do processo.
- **Uma lista de handles abertos** — Mapeiam para vários recursos do sistema, como semáforos, objetos de sincronização e arquivos acessíveis a todas as threads do processo.
- **Um contexto de segurança** — Um token de acesso (access token) que identifica o usuário, grupos de segurança, privilégios, atributos, declarações, capacidades, estado de virtualização do UAC, sessão e estado de conta de usuário limitado associados ao processo.
- **Um ID de processo** — Um identificador único, que internamente faz parte de um identificador chamado client ID.
- **Pelo menos uma thread de execução** — Embora um processo "vazio" seja possível, ele é (na maioria das vezes) inútil.

> **Rode no Visual Studio**
> Cada processo tem um ID único. Veja como obter o PID do processo atual e abrir um handle para ele.
>
> ```cpp
> #include <windows.h>
> #include <stdio.h>
>
> int main() {
>     DWORD pid = GetCurrentProcessId();
>     printf("PID deste processo: %lu\n", pid);
>
>     HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
>     if (hProcess != NULL) {
>         printf("Handle aberto: %p\n", hProcess);
>         CloseHandle(hProcess);
>     }
>
>     return 0;
> }
> ```

#### EXPERIMENTO: Visualizando informações de processo com o Task Manager

O Task Manager do Windows oferece uma lista rápida dos processos no sistema. Você pode iniciar o Task Manager de quatro formas:

1. Pressione Ctrl+Shift+Esc.
2. Clique com o botão direito na barra de tarefas e clique em Iniciar Gerenciador de Tarefas.
3. Pressione Ctrl+Alt+Delete e clique no botão Iniciar Gerenciador de Tarefas.
4. Inicie o executável Taskmgr.exe.

Na primeira vez que o Task Manager aparecer, ele estará no modo "menos detalhes", onde apenas os processos que têm uma janela de nível superior visível são mostrados. Clique no botão expansor Mais Detalhes para mostrar a visualização completa. A aba Processos deve ser selecionada por padrão.

A aba Processos mostra a lista de processos, com quatro colunas: CPU, Memória, Disco e Rede. Você pode mostrar mais colunas clicando com o botão direito no cabeçalho. As colunas disponíveis incluem Nome do Processo (Imagem), ID do Processo, Tipo, Status, Publicador e Linha de Comando.

Para obter ainda mais detalhes do processo, clique na aba Detalhes. A aba Detalhes mostra processos de forma mais compacta, sem mostrar as janelas criadas pelos processos, e fornece colunas de informações mais diversas.

Algumas colunas-chave são:

- **Threads** — Mostra o número de threads em cada processo. Este número deve ser normalmente pelo menos um. Se um processo mostrar zero threads, geralmente significa que o processo não pode ser excluído por algum motivo — provavelmente por causa de algum código de driver com bugs.
- **Handles** — Mostra o número de handles para objetos do kernel abertos por threads rodando dentro do processo.
- **Status** — Para processos sem interface de usuário, *Running* (Em execução) deve ser o caso normal. *Suspended* (Suspenso) ocorre quando todas as threads do processo estão em estado suspenso. Para Windows Apps, *Suspended* normalmente ocorre quando o app perde o status de primeiro plano ao ser minimizado pelo usuário — esses processos são suspensos após 5 segundos para não consumir CPU ou recursos de rede. O terceiro valor possível é *Not Responding* (Sem Resposta), o que pode acontecer se uma thread do processo que criou a interface de usuário não verificou sua fila de mensagens por pelo menos 5 segundos.

Cada processo também aponta para seu processo pai ou criador. Se o pai não existir mais, essa informação não é atualizada — portanto, é possível que um processo faça referência a um pai inexistente.

#### EXPERIMENTO: Visualizando a árvore de processos

Um atributo único sobre um processo que a maioria das ferramentas não exibe é o ID do processo pai ou criador. Você pode usar a ferramenta Tlist.exe nas Ferramentas de Depuração para Windows para mostrar a árvore de processos usando a opção `/t`:

```
tlist /t
```

A lista indenta cada processo para mostrar sua relação pai/filho. Processos cujos pais não estão vivos ficam alinhados à esquerda porque, mesmo que um avô exista, não há como encontrar essa relação. O Windows mantém apenas o ID do processo criador, não um link de volta ao criador do criador.

O Process Explorer, do Sysinternals, mostra mais detalhes sobre processos e threads do que qualquer outra ferramenta disponível. Alguns dos recursos únicos do Process Explorer incluem:

- Token de segurança de um processo, como listas de grupos e privilégios e o estado de virtualização
- Destaque para mostrar mudanças na lista de processos, threads, DLLs e handles
- Uma lista de serviços dentro de processos hospedadores de serviços, incluindo o nome de exibição e descrição
- Uma lista de atributos adicionais do processo, como políticas de mitigação e seu nível de proteção
- Processos que fazem parte de um job e detalhes do job
- Processos hospedando aplicativos .NET e detalhes específicos do .NET
- O tempo de início para processos e threads
- Uma lista completa de arquivos mapeados na memória (não apenas DLLs)

### Threads

Uma thread (fio de execução) é uma entidade dentro de um processo que o Windows agenda para execução. Sem ela, o programa do processo não pode rodar. Uma thread inclui os seguintes componentes essenciais:

- O conteúdo de um conjunto de registradores de CPU representando o estado do processador
- Duas pilhas (stacks) — uma para uso da thread durante a execução em modo kernel e outra para execução em modo usuário
- Uma área de armazenamento privada chamada thread-local storage (TLS) para uso por subsistemas, bibliotecas de tempo de execução e DLLs
- Um identificador único chamado thread ID (parte de uma estrutura interna chamada client ID; IDs de processos e IDs de threads são gerados a partir do mesmo namespace, então nunca se sobrepõem)

> **Rode no Visual Studio**
> Processos e threads compartilham o mesmo namespace de IDs, mas nunca se sobrepõem. Veja os dois lado a lado.
>
> ```cpp
> #include <windows.h>
> #include <stdio.h>
>
> int main() {
>     DWORD pid = GetCurrentProcessId();
>     DWORD tid = GetCurrentThreadId();
>     printf("PID: %lu | TID: %lu\n", pid, tid);
>     return 0;
> }
> ```

Os registradores voláteis, as stacks e a área de armazenamento privada são chamados de **contexto** da thread. Como essas informações são diferentes para cada arquitetura de máquina em que o Windows roda, essa estrutura é, por necessidade, específica da arquitetura.

Como trocar a execução de uma thread para outra envolve o agendador do kernel (kernel scheduler), pode ser uma operação cara. O Windows implementa dois mecanismos para reduzir esse custo: **fibers** e **user-mode scheduling (UMS)**.

#### Fibers

Fibers permitem que um aplicativo agende seus próprios fios de execução em vez de depender do mecanismo de agendamento baseado em prioridade integrado ao Windows. Fibers são frequentemente chamadas de threads leves (lightweight threads). Em termos de agendamento, elas são invisíveis para o kernel porque são implementadas em modo usuário em Kernel32.dll.

Para usar fibers, você primeiro faz uma chamada à função `ConvertThreadToFiber` do Windows. Essa função converte a thread em uma fiber em execução. Em seguida, a fiber recém-convertida pode criar fibers adicionais por meio da função `CreateFiber`. Ao contrário de uma thread, uma fiber não começa a execução até que seja manualmente selecionada por meio de uma chamada à função `SwitchToFiber`.

> **Nota**  
> Usar fibers geralmente não é uma boa ideia. Isso porque elas são invisíveis para o kernel e têm problemas como compartilhamento de thread local storage (TLS). Fibers ligadas a I/O terão desempenho ruim independentemente. Além disso, fibers não podem rodar simultaneamente em mais de um processador e estão limitadas a multitarefa cooperativa apenas.

#### Threads de agendamento em modo usuário (UMS)

Threads de User-mode scheduling (UMS), disponíveis apenas em versões de 64 bits do Windows, fornecem as mesmas vantagens básicas das fibers — com apenas algumas das desvantagens. Threads UMS têm seu próprio estado de thread do kernel e, portanto, são visíveis para o kernel, o que permite que múltiplas threads UMS emitam chamadas de sistema bloqueantes e compartilhem e disputem recursos. Quando duas ou mais threads UMS precisam realizar trabalho em modo usuário, elas podem periodicamente trocar contextos de execução em modo usuário em vez de envolver o agendador.

### Jobs

O Windows fornece uma extensão ao modelo de processo chamada **job**. A função principal de um objeto job é permitir o gerenciamento e manipulação de grupos de processos como uma unidade. Um objeto job permite controlar certos atributos e fornece limites para o processo ou processos associados ao job. Em certos aspectos, o objeto job compensa a falta de uma árvore de processos estruturada no Windows — e em muitos aspectos é mais poderoso do que uma árvore de processos no estilo UNIX.

Você aprenderá muito mais sobre a estrutura interna de processos e jobs no Capítulo 3 e sobre threads e algoritmos de agendamento de threads no Capítulo 4.

### Memória virtual

O Windows implementa um sistema de memória virtual baseado em um espaço de endereçamento plano (linear) que fornece a cada processo a ilusão de ter seu próprio e grande espaço de endereçamento privado. A memória virtual fornece uma visão lógica da memória que pode não corresponder ao seu layout físico. Em tempo de execução, o gerenciador de memória (memory manager) — com assistência do hardware — traduz, ou mapeia, os endereços virtuais em endereços físicos, onde os dados são realmente armazenados.

Como a maioria dos sistemas tem muito menos memória física do que a memória virtual total em uso pelos processos em execução, o gerenciador de memória transfere, ou pagina (pages), parte do conteúdo da memória para o disco. Paginar dados para o disco libera memória física para que possa ser usada por outros processos ou pelo próprio SO. Quando uma thread acessa um endereço virtual que foi paginado para o disco, o gerenciador de memória virtual carrega as informações de volta para a memória a partir do disco.

O tamanho do espaço de endereçamento virtual varia para cada plataforma de hardware:

- Em sistemas x86 de 32 bits: máximo teórico de 4 GB. Por padrão, o Windows aloca a metade inferior (endereços 0x00000000 a 0x7FFFFFFF) para processos e a metade superior (0x80000000 a 0xFFFFFFFF) para uso próprio do SO protegido.
- Em sistemas de 64 bits: Windows 8.1, Server 2012 R2 e sistemas posteriores fornecem 128 TB de espaço de endereçamento para processos.

> **Rode no Visual Studio**
> `VirtualAlloc` reserva e comita páginas no espaço de endereçamento virtual do processo. O endereço retornado é um endereço virtual, não físico.
>
> ```cpp
> #include <windows.h>
> #include <stdio.h>
>
> int main() {
>     SIZE_T tamanho = 4096; // 4 KB - tamanho de uma pagina
>     LPVOID endereco = VirtualAlloc(NULL, tamanho, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
>
>     if (endereco != NULL) {
>         printf("Endereco virtual alocado: %p\n", endereco);
>         VirtualFree(endereco, 0, MEM_RELEASE);
>         printf("Memoria liberada.\n");
>     }
>
>     return 0;
> }
> ```

### Modo kernel vs. modo usuário

Para proteger aplicativos de usuário de acessar e/ou modificar dados críticos do SO, o Windows usa dois modos de acesso do processador: **modo usuário (user mode)** e **modo kernel (kernel mode)**. O código de aplicativos de usuário roda em modo usuário, enquanto o código do SO (como serviços do sistema e device drivers) roda em modo kernel. O modo kernel refere-se a um modo de execução em um processador que concede acesso a toda a memória do sistema e a todas as instruções de CPU.

> **Nota**  
> As arquiteturas dos processadores x86 e x64 definem quatro níveis de privilégio (ou rings) para proteger o código e os dados do sistema. O Windows usa o nível de privilégio 0 (ou ring 0) para o modo kernel e o nível de privilégio 3 (ou ring 3) para o modo usuário. O motivo pelo qual o Windows usa apenas dois níveis é que algumas arquiteturas de hardware, como ARM hoje e MIPS/Alpha no passado, implementaram apenas dois níveis de privilégio.

Embora cada processo Windows tenha seu próprio espaço de memória privado, o SO em modo kernel e o código de device driver compartilham um único espaço de endereçamento virtual. Cada página na memória virtual é marcada para indicar em qual modo o processador deve estar para ler e/ou escrever a página. Páginas no espaço do sistema só podem ser acessadas a partir do modo kernel, enquanto todas as páginas no espaço de endereçamento do usuário são acessíveis a partir do modo usuário e do modo kernel.

O Windows não fornece nenhuma proteção para memória do sistema privada de leitura/gravação sendo usada por componentes rodando em modo kernel. Em outras palavras, uma vez em modo kernel, o código do SO e de device drivers tem acesso completo à memória do espaço do sistema e pode contornar a segurança do Windows. Isso enfatiza a necessidade de permanecer vigilante ao carregar um device driver de terceiros, especialmente se não estiver assinado, porque uma vez em modo kernel, o driver tem acesso completo a todos os dados do SO.

Em versões de 64 bits e ARM do Windows 8.1, a política de assinatura de código em modo kernel (KMCS) determina que todos os device drivers devem ser assinados com uma chave criptográfica atribuída por uma das principais autoridades de certificação de código. No Windows 10, Microsoft implementou uma mudança ainda mais significativa: todos os novos drivers do Windows 10 devem ser assinados por apenas duas das autoridades de certificação aceitas com um certificado SHA-2 Extended Validation (EV). Uma vez assinado com EV, o driver de hardware deve ser submetido à Microsoft através do portal System Device (SysDev) para assinatura de atestação.

Como você verá no Capítulo 2, "Arquitetura do sistema", aplicativos de usuário mudam de modo usuário para modo kernel quando fazem uma chamada de serviço do sistema. O uso de uma instrução especial do processador aciona a transição do modo usuário para o modo kernel e faz com que o processador entre no código de despacho de serviço do sistema no kernel. Antes de retornar o controle para a thread do usuário, o modo do processador é revertido para o modo usuário. Dessa forma, o SO se protege e protege seus dados de perusal e modificação por processos de usuário.

#### EXPERIMENTO: Modo kernel vs. modo usuário

Você pode usar o Monitor de Desempenho (Performance Monitor) para ver quanto tempo seu sistema gasta executando em modo kernel versus em modo usuário:

1. Abra o menu Iniciar e digite "Executar Monitor de Desempenho" para executar o Performance Monitor.
2. Selecione o nó Monitor de Desempenho em Performance/Ferramentas de Monitoramento na árvore à esquerda.
3. Para excluir o contador padrão mostrando o tempo total de CPU, clique no botão Excluir na barra de ferramentas.
4. Clique no botão Adicionar (+) na barra de ferramentas.
5. Expanda a seção de contador Processador, clique no contador % Privileged Time e, enquanto mantém pressionada a tecla Ctrl, clique no contador % User Time.
6. Clique em Adicionar e depois em OK.
7. Abra um prompt de comando e digite `dir \\%computername%\c$ /s` para executar uma varredura do diretório da sua unidade C.

Você também pode ver isso rapidamente usando o Task Manager: clique na aba Desempenho, clique com o botão direito no gráfico de CPU e selecione Mostrar Tempos do Kernel.

### Hypervisor

Mudanças recentes nos modelos de aplicativos e software, como a introdução de serviços baseados em nuvem e a onipresença de dispositivos IoT, resultaram na necessidade de sistemas operacionais e fornecedores de hardware criarem formas mais eficientes de virtualizar outros SOs convidados no hardware host da máquina.

Para fornecer tais serviços de virtualização, quase todas as soluções modernas empregam o uso de um **hypervisor**, que é um componente especializado e altamente privilegiado que permite a virtualização e isolamento de todos os recursos na máquina. O Hyper-V é um exemplo de tal hypervisor. Produtos concorrentes como Xen, KVM, VMware e VirtualBox implementam seus próprios hypervisors.

Devido à sua natureza altamente privilegiada, um hypervisor tem uma vantagem distinta: além de simplesmente executar múltiplas instâncias convidadas de outros sistemas operacionais, ele pode proteger e monitorar uma única instância host para oferecer garantias além do que o kernel fornece. No Windows 10, a Microsoft agora aproveita o hypervisor Hyper-V para fornecer um novo conjunto de serviços conhecido como **virtualization-based security (VBS)**:

- **Device Guard** — Fornece Hypervisor Code Integrity (HVCI) para garantias mais fortes de assinatura de código.
- **Hyper Guard** — Protege estruturas de dados e código relacionados ao kernel e ao hypervisor.
- **Credential Guard** — Previne acesso não autorizado a credenciais de contas de domínio e segredos.
- **Application Guard** — Fornece um sandbox ainda mais forte para o navegador Microsoft Edge.
- **Host Guardian e Shielded Fabric** — Aproveitam um TPM virtual (v-TPM) para proteger uma máquina virtual da infraestrutura em que está rodando.

A vantagem principal de todas essas tecnologias é que, ao contrário das melhorias de segurança anteriores baseadas no kernel, elas não são vulneráveis a drivers maliciosos ou mal escritos. Isso é possível devido à implementação de Virtual Trust Levels (VTLs) pelo hypervisor. Como o SO normal e seus componentes estão em um modo menos privilegiado (VTL 0), mas essas tecnologias VBS rodam no VTL 1 (um privilégio maior), eles não podem ser afetados mesmo por código em modo kernel.

### Firmware

O Windows cada vez mais depende da segurança do SO e de seu kernel, e o último agora depende da proteção do hypervisor. O que, então, fornece uma cadeia raiz de confiança que pode garantir um processo de boot sem comprometimento? Em sistemas modernos com Windows 8 e posteriores, isso fica sob a responsabilidade do firmware do sistema, que deve ser baseado em UEFI em sistemas certificados.

Como parte do padrão UEFI, uma implementação de boot seguro (secure boot) com fortes garantias e requisitos em torno das qualidades de assinatura do software relacionado ao boot deve estar presente. Por meio desse processo de verificação, os componentes do Windows têm garantia de carregamento seguro desde o início do processo de boot.

### Terminal Services e múltiplas sessões

Terminal Services refere-se ao suporte no Windows para múltiplas sessões de usuário interativas em um único sistema. Com o Windows Terminal Services, um usuário remoto pode estabelecer uma sessão em outra máquina, fazer login e executar aplicativos no servidor.

A primeira sessão é considerada a sessão de serviços, ou sessão zero (session zero), e contém processos de hospedagem de serviços do sistema. A primeira sessão de login no console físico da máquina é a sessão um, e sessões adicionais podem ser criadas por meio do programa de conexão de desktop remoto (Mstsc.exe) ou por meio do recurso de troca rápida de usuário (fast user switching).

Sistemas Windows Server suportam duas conexões remotas simultâneas para facilitar o gerenciamento remoto. Também suportam mais de duas sessões remotas se devidamente licenciados e configurados como servidor de terminal.

### Objetos e handles

No SO Windows, um **objeto do kernel** (kernel object) é uma instância única em tempo de execução de um tipo de objeto definido estaticamente. Um tipo de objeto compreende um tipo de dados definido pelo sistema, funções que operam em instâncias do tipo de dados e um conjunto de atributos do objeto.

A diferença mais fundamental entre um objeto e uma estrutura de dados comum é que a estrutura interna de um objeto é opaca. Você deve chamar um serviço de objeto para obter ou colocar dados em um objeto — não pode ler ou alterar diretamente os dados dentro de um objeto.

Objetos, com a ajuda de um componente do kernel chamado gerenciador de objetos (object manager), fornecem um meio conveniente de realizar as seguintes quatro tarefas importantes do SO:

1. Fornecer nomes legíveis para recursos do sistema
2. Compartilhar recursos e dados entre processos
3. Proteger recursos de acesso não autorizado
4. Rastreamento de referências, que permite ao sistema reconhecer quando um objeto não está mais em uso para que possa ser desalocado automaticamente

Nem todas as estruturas de dados no SO Windows são objetos. Apenas dados que precisam ser compartilhados, protegidos, nomeados ou tornados visíveis a programas em modo usuário são colocados em objetos.

### Segurança

O Windows foi projetado desde o início para ser seguro. As principais capacidades de segurança do Windows incluem:

- Proteção discricionária (por necessidade de saber) e obrigatória para todos os objetos compartilháveis do sistema, como arquivos, diretórios, processos, threads, etc.
- Auditoria de segurança para responsabilização de sujeitos ou usuários e as ações que eles iniciam
- Autenticação de usuário no login
- Prevenção de um usuário de acessar recursos não inicializados que outro usuário desalocou

O Windows tem três formas de controle de acesso sobre objetos:

- **Controle de acesso discricionário (Discretionary access control)** — O mecanismo de proteção pelo qual proprietários de objetos concedem ou negam acesso a outros. Quando usuários fazem login, recebem um conjunto de credenciais de segurança. Com o Windows Server 2012 e Windows 8, essa forma de controle discricionário é aprimorada com controle de acesso baseado em atributos (Dynamic Access Control).

- **Controle de acesso privilegiado (Privileged access control)** — Necessário quando o controle de acesso discricionário não é suficiente. Permite que um administrador acesse objetos protegidos quando o proprietário não está disponível (por exemplo, ao assumir a propriedade de arquivos quando um funcionário sai da empresa).

- **Controle de integridade obrigatório (Mandatory integrity control)** — Necessário quando um nível adicional de controle de segurança é necessário para proteger objetos sendo acessados de dentro da mesma conta de usuário. É usado, por exemplo, para isolar o Internet Explorer em Modo Protegido das configurações de um usuário.

A partir do Windows 8, um sandbox chamado **AppContainer** é usado para hospedar Windows Apps, fornecendo isolamento em relação a outros AppContainers e processos que não são Windows Apps. Um exemplo canônico é o navegador Microsoft Edge, que roda dentro de um AppContainer e, assim, fornece melhor proteção contra código malicioso rodando dentro de seus limites.

### Registro (Registry)

O registro é o banco de dados do sistema que contém as informações necessárias para inicializar e configurar o sistema, configurações de software em todo o sistema que controlam a operação do Windows, o banco de dados de segurança e as configurações de per-usuário.

Embora muitos usuários e administradores do Windows nunca precisem olhar diretamente para o registro (porque você pode visualizar ou alterar a maioria das configurações de configuração com utilitários administrativos padrão), ele ainda é uma fonte útil de informações sobre os internos do Windows porque contém muitas configurações que afetam o desempenho e o comportamento do sistema.

A maioria das chaves de registro referidas neste livro está sob o hive de configuração em todo o sistema, `HKEY_LOCAL_MACHINE`, que abreviaremos como HKLM.

> **Cuidado**
> Se você decidir alterar diretamente as configurações do registro, deve ter extremo cuidado. Qualquer alteração pode afetar adversamente o desempenho do sistema ou, pior, fazer o sistema falhar ao inicializar com sucesso.

> **Rode no Visual Studio**
> Lendo o nome do produto Windows direto do registro, abaixo de `HKLM`.
>
> ```cpp
> #include <windows.h>
> #include <stdio.h>
>
> int main() {
>     HKEY hKey;
>     if (RegOpenKeyExW(HKEY_LOCAL_MACHINE,
>         L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",
>         0, KEY_READ, &hKey) == ERROR_SUCCESS) {
>
>         WCHAR valor[256];
>         DWORD tamanho = sizeof(valor);
>         RegQueryValueExW(hKey, L"ProductName", NULL, NULL, (LPBYTE)valor, &tamanho);
>         printf("Sistema: %ls\n", valor);
>         RegCloseKey(hKey);
>     }
>
>     return 0;
> }
> ```

### Unicode

O Windows se diferencia da maioria dos outros sistemas operacionais porque a maioria das strings de texto internas são armazenadas e processadas como caracteres Unicode de 16 bits (tecnicamente UTF-16LE). Unicode é um padrão de conjunto de caracteres internacional que define valores únicos para a maioria dos conjuntos de caracteres conhecidos do mundo.

Como muitos aplicativos lidam com strings de caracteres ANSI de 8 bits (byte único), muitas funções do Windows que aceitam parâmetros de string têm dois pontos de entrada: uma versão Unicode (wide, 16 bits) e uma versão ANSI (narrow, 8 bits). Se você chamar a versão narrow de uma função do Windows, há um leve impacto no desempenho enquanto os parâmetros de string de entrada são convertidos para Unicode antes de serem processados pelo sistema.

A função `CreateFile` da Windows API, por exemplo, não é uma função por si só; em vez disso, é uma macro que se expande para uma de duas funções: `CreateFileA` (ANSI) ou `CreateFileW` (Unicode, onde W significa wide). A expansão é baseada em uma constante de compilação chamada `UNICODE`.

> **Rode no Visual Studio**
> `CreateFile` é na verdade uma macro. Com `UNICODE` definido vira `CreateFileW`, sem ele vira `CreateFileA`. Veja a versão wide explícita:
>
> ```cpp
> #include <windows.h>
> #include <stdio.h>
>
> int main() {
>     // Chamando CreateFileW diretamente (versao Unicode/wide)
>     HANDLE hArquivo = CreateFileW(
>         L"teste.txt",
>         GENERIC_WRITE,
>         0,
>         NULL,
>         CREATE_ALWAYS,
>         FILE_ATTRIBUTE_NORMAL,
>         NULL
>     );
>
>     if (hArquivo != INVALID_HANDLE_VALUE) {
>         printf("Arquivo criado com CreateFileW\n");
>         CloseHandle(hArquivo);
>         DeleteFileW(L"teste.txt");
>     }
>
>     return 0;
> }
> ```

#### EXPERIMENTO: Visualizando funções exportadas

Neste experimento, você usará a ferramenta Dependency Walker para visualizar funções exportadas de uma DLL de subsistema do Windows:

1. Baixe o Dependency Walker em http://www.dependencywalker.com.
2. Execute a ferramenta (depends.exe). Depois abra o menu Arquivo, escolha Abrir, navegue até a pasta `C:\Windows\System32`, localize o arquivo `kernel32.dll` e clique em Abrir.
3. Na segunda visualização do topo à direita, você verá as funções exportadas disponíveis em kernel32.dll.
4. Clique no cabeçalho da lista de funções para ordenar por nome e localize `CreateFileA`. Você encontrará `CreateFileW` um pouco mais abaixo. Assim como esse par, a maioria das funções que têm pelo menos um argumento de tipo string são, na verdade, pares de funções.

## Explorando os internos do Windows

### Monitor de Desempenho e Monitor de Recursos

O Monitor de Desempenho (Performance Monitor), acessível na pasta Ferramentas Administrativas no Painel de Controle ou digitando `perfmon` na caixa de diálogo Executar, fornece mais informações sobre como seu sistema está operando do que qualquer outro utilitário único. Inclui centenas de contadores base e extensíveis para vários objetos.

O Windows também inclui um utilitário Monitor de Recursos (Resource Monitor), acessível a partir do menu Iniciar ou da aba Desempenho do Task Manager, que mostra quatro recursos primários do sistema: CPU, disco, rede e memória.

- A aba **CPU** exibe informações sobre o uso de CPU por processo, além de incluir uma exibição separada de serviços e seu uso de CPU associado.
- A aba **Memória** exibe um gráfico de barras de memória física mostrando a organização atual da memória física em: reservada para hardware, em uso, modificada, em espera ou livre.
- A aba **Disco** exibe informações por arquivo para I/O, facilitando a identificação dos arquivos mais acessados no sistema.
- A aba **Rede** exibe as conexões de rede ativas, os processos que as possuem e quanto de dados está passando por elas.

### Depuração do kernel

A depuração do kernel (kernel debugging) significa examinar estruturas de dados internas do kernel e/ou percorrer funções no kernel. É uma maneira útil de investigar os internos do Windows porque você pode exibir informações internas do sistema não disponíveis por nenhuma outra ferramenta.

#### Símbolos para depuração do kernel

Arquivos de símbolo (symbol files) contêm os nomes de funções e variáveis e o layout e formato das estruturas de dados. Eles são gerados pelo linker e usados por depuradores para referenciar e exibir esses nomes durante uma sessão de depuração.

Para usar os símbolos, você pode usar o servidor de símbolos da Microsoft sob demanda usando uma sintaxe especial para o caminho de símbolo:

```
srv*c:\symbols*http://msdl.microsoft.com/download/symbols
```

Essa sintaxe faz com que as ferramentas de depuração carreguem os símbolos necessários do servidor de símbolos na Internet e mantenham uma cópia local na pasta `C:\symbols`.

#### Ferramentas de Depuração para Windows (Debugging Tools for Windows)

O pacote Ferramentas de Depuração para Windows contém ferramentas avançadas de depuração. As versões mais recentes estão incluídas como parte do Windows SDK. Há quatro depuradores incluídos nas ferramentas: `cdb`, `ntsd`, `kd` e `WinDbg`:

- `cdb` e `ntsd` são depuradores de modo usuário baseados em interface de console.
- `kd` é um depurador de modo kernel baseado em interface de console.
- `WinDbg` pode ser usado como depurador de modo usuário ou modo kernel, mas não ambos ao mesmo tempo. Fornece uma GUI.

Há dois tipos de depuração disponíveis:

- **Depuração de modo usuário** — Permite anexar a um processo de modo usuário e examinar e/ou alterar a memória do processo. Pode ser invasiva (usando `DebugActiveProcess`) ou não invasiva (usando apenas `OpenProcess`).

- **Depuração de modo kernel** — Permite abrir um arquivo de dump de falha (crash dump), conectar-se a um sistema vivo em execução ou usar a depuração local do kernel (local kernel debugging). Para a depuração local do kernel com WinDbg, certifique-se de que o sistema está definido para o modo de depuração (por exemplo, executando `msconfig.exe`, clicando na aba Boot, selecionando Opções Avançadas, selecionando Depuração e reiniciando o Windows).

#### EXPERIMENTO: Exibindo informações de tipo para estruturas do kernel

Para exibir a lista de estruturas do kernel cujas informações de tipo estão incluídas nos símbolos do kernel, digite `dt nt!_*` no depurador do kernel. Você pode também usar o comando `dt` para pesquisar estruturas específicas usando curinga:

```
lkd> dt nt!_*interrupt*
```

E formatar uma estrutura específica:

```
lkd> dt nt!_KINTERRUPT
```

#### Ferramenta LiveKd

O LiveKd é uma ferramenta gratuita do Sysinternals que permite usar os depuradores de kernel padrão da Microsoft para examinar o sistema em execução sem precisar inicializar o sistema em modo de depuração.

### Windows Software Development Kit (SDK)

O Windows SDK está disponível para download gratuito em https://developer.microsoft.com/en-US/windows/downloads/windows-10-sdk. Além das Ferramentas de Depuração para Windows, ele contém os arquivos de cabeçalho C e as bibliotecas necessárias para compilar e vincular aplicativos Windows.

### Windows Driver Kit (WDK)

O WDK também está disponível para download gratuito. Embora o WDK seja voltado para desenvolvedores de device drivers, é uma fonte abundante de informações sobre os internos do Windows. O WDK contém arquivos de cabeçalho (em particular, `ntddk.h`, `ntifs.h` e `wdm.h`) que definem estruturas de dados internas chave e constantes, bem como interfaces para muitas rotinas internas do sistema.

### Ferramentas Sysinternals

Muitos experimentos neste livro usam ferramentas freeware que você pode baixar do Sysinternals. As ferramentas mais populares incluem Process Explorer e Process Monitor. Observe que muitos desses utilitários envolvem a instalação e execução de device drivers em modo kernel e, portanto, requerem privilégios de administrador.

Como as ferramentas do Sysinternals são atualizadas frequentemente, certifique-se de ter a versão mais recente. Para ser notificado de atualizações das ferramentas, você pode seguir o Sysinternals Site Blog.

## Conclusão

Este capítulo apresentou os principais conceitos técnicos e termos do Windows que serão usados ao longo do livro. Também ofereceu um vislumbre das muitas ferramentas úteis disponíveis para explorar os internos do Windows. Agora você está pronto para começar sua exploração do design interno do sistema, começando com uma visão geral da arquitetura do sistema e seus principais componentes.
