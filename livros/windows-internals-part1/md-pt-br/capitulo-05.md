# Capítulo 5 — Gerenciamento de Memória

> *Tradução do livro Windows Internals, 7ª Edição*  
> *Pavel Yosifovich, Alex Ionescu, Mark E. Russinovich, David A. Solomon*  
> *Microsoft Press, 2017*

---

Neste capítulo, você aprenderá como o Windows implementa a memória virtual e como ele gerencia o subconjunto de memória virtual mantido na memória física. Também descreveremos a estrutura interna e os componentes que compõem o gerenciador de memória, incluindo estruturas de dados e algoritmos-chave.

## Introdução ao gerenciador de memória

Por padrão, o tamanho virtual de um processo no Windows de 32 bits é de 2 GB. Se a imagem for marcada especificamente como compatível com espaço de endereçamento grande (large address space-aware) e o sistema for inicializado com uma opção especial, um processo de 32 bits pode crescer até 3 GB no Windows de 32 bits e até 4 GB no Windows de 64 bits. O tamanho do espaço de endereçamento virtual do processo no Windows 8 de 64 bits e Server 2012 é de 8.192 GB (8 TB) e no Windows 8.1 de 64 bits (e posterior) e Server 2012 R2 (e posterior), é de 128 TB.

O gerenciador de memória tem duas tarefas primárias:

- **Traduzir, ou mapear**, o espaço de endereçamento virtual de um processo para a memória física, de forma que quando uma thread lê ou escreve no espaço de endereçamento virtual, o endereço físico correto seja referenciado. O subconjunto do espaço de endereçamento virtual de um processo que é fisicamente residente é chamado de **working set** (conjunto de trabalho).
- **Paginar** parte do conteúdo da memória para o disco quando ela fica supercomprometida — isto é, quando threads tentam usar mais memória física do que está disponível — e trazer o conteúdo de volta para a memória física quando necessário.

Além disso, o gerenciador de memória fornece um conjunto central de serviços sobre os quais os subsistemas de ambiente do Windows são construídos, incluindo arquivos mapeados em memória (internamente chamados de objetos de seção), memória copy-on-write e suporte a aplicativos que usam espaços de endereçamento grandes e esparsos.

## Componentes do gerenciador de memória

O gerenciador de memória faz parte do executive do Windows e portanto existe no arquivo Ntoskrnl.exe. É o maior componente no executive, o que indica sua importância e complexidade. O gerenciador de memória consiste nos seguintes componentes:

- Um conjunto de serviços executivos do sistema para alocar, desalocar e gerenciar memória virtual.
- Um handler de trap para falhas de acesso e de tradução inválida para resolver exceções de gerenciamento de memória detectadas por hardware.
- Seis rotinas de nível superior principais, cada uma rodando em uma das seis threads do kernel no processo System:
  - **Balance set manager** (`KeBalanceSetManager`, prioridade 17) — Chama o working set manager uma vez por segundo e quando a memória livre cai abaixo de um certo limiar.
  - **Process/stack swapper** (`KeSwapProcessOrStack`, prioridade 23) — Realiza operações de entrada e saída de processo e de pilha de thread do kernel.
  - **Modified page writer** (`MiModifiedPageWriter`, prioridade 18) — Escreve páginas sujas na lista modificada de volta para os arquivos de paginação apropriados.
  - **Mapped page writer** (`MiMappedPageWriter`, prioridade 18) — Escreve páginas sujas em arquivos mapeados para o disco ou armazenamento remoto.
  - **Segment dereference thread** (`MiDereferenceSegmentThread`, prioridade 19) — Responsável pela redução do cache e pelo crescimento e encolhimento do arquivo de páginas.
  - **Zero page thread** (`MiZeroPageThread`, prioridade 0) — Zera as páginas na lista livre para que um cache de páginas zero esteja disponível para satisfazer futuras falhas de página de demanda zero.

## Páginas grandes e pequenas

O gerenciamento de memória é feito em blocos distintos chamados **páginas** (pages). Os processadores nos quais o Windows roda suportam dois tamanhos de página: pequenas e grandes.

| Arquitetura | Tamanho de página pequena | Tamanho de página grande |
|-------------|--------------------------|--------------------------|
| x86 | 4 KB | 4 MB (2 MB com PAE) |
| x64 | 4 KB | 2 MB |
| ARM | 4 KB | 4 MB |

A principal vantagem de páginas grandes é a velocidade de tradução de endereços para referências a dados dentro da página grande, pois uma única entrada no TLB (translation look-aside buffer) cobre toda a página. O Windows 10 versão 1607 x64 e Server 2016 também suportam **huge pages** (páginas enormes) de 1 GB.

## Examinando o uso de memória

A aba Performance no Task Manager do Windows e o Process Explorer do Sysinternals mostram informações detalhadas sobre o uso de memória física e virtual. O comando `!vm` no depurador do kernel também mostra informações básicas de gerenciamento de memória.

## Serviços fornecidos pelo gerenciador de memória

A Windows API tem quatro grupos de funções para gerenciar memória em aplicativos:

- **Virtual API** — A API de nível mais baixo para alocações e desalocações gerais de memória. Sempre funciona com granularidade de página. Funções incluem `VirtualAlloc`, `VirtualFree`, `VirtualProtect`, `VirtualLock`, etc.
- **Heap API** — Fornece funções para pequenas alocações (tipicamente menos de uma página). Usa a Virtual API internamente. Funções incluem `HeapAlloc`, `HeapFree`, `HeapCreate`, `HeapReAlloc`, etc.
- **Local/Global APIs** — Sobras do Windows de 16 bits, agora implementadas usando a Heap API.
- **Arquivos mapeados em memória** — Permite mapear arquivos como memória e/ou compartilhar memória entre processos cooperantes. Funções incluem `CreateFileMapping`, `OpenFileMapping`, `MapViewOfFile`, etc.

## Estados de página e alocações de memória

As páginas no espaço de endereçamento virtual de um processo são **livres**, **reservadas**, **confirmadas** (committed) ou **compartilháveis**:

- **Páginas confirmadas (committed)** — Também referidas como páginas privadas. Não podem ser compartilhadas com outros processos. Alocadas através de `VirtualAlloc`.
- **Páginas compartilháveis (shareable)** — Geralmente mapeadas para uma visão de uma seção. Podem ser compartilhadas com outros processos.
- **Páginas reservadas** — Faixa de espaço de endereçamento virtual reservado para uso futuro, consumindo recursos mínimos do sistema.
- **Páginas livres** — Não têm associação com nenhum armazenamento.

Páginas confirmadas privadas são criadas com inicialização zero quando acessadas pela primeira vez (páginas de demanda zero). Páginas privadas confirmadas podem ser escritas automaticamente para o arquivo de paginação pelo sistema operacional se necessário.

> **Rode no Visual Studio**
> `VirtualQuery` inspeciona o estado de uma regiao do espaco de enderecamento. O exemplo aloca memoria, consulta o estado e depois muda a protecao com `VirtualProtect`.
>
> ```cpp
> #include <windows.h>
> #include <stdio.h>
>
> int main() {
>     LPVOID ptr = VirtualAlloc(NULL, 4096, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
>
>     MEMORY_BASIC_INFORMATION mbi;
>     VirtualQuery(ptr, &mbi, sizeof(mbi));
>
>     printf("Endereco base : %p\n", mbi.BaseAddress);
>     printf("Tamanho       : %zu bytes\n", mbi.RegionSize);
>     printf("Estado        : %s\n", mbi.State == MEM_COMMIT ? "COMMIT" : "RESERVE");
>     printf("Protecao      : 0x%lX\n", mbi.Protect); // 0x04 = PAGE_READWRITE
>
>     // Muda para somente leitura
>     DWORD protAnterior;
>     VirtualProtect(ptr, 4096, PAGE_READONLY, &protAnterior);
>     VirtualQuery(ptr, &mbi, sizeof(mbi));
>     printf("Nova protecao : 0x%lX\n", mbi.Protect); // 0x02 = PAGE_READONLY
>
>     VirtualFree(ptr, 0, MEM_RELEASE);
>     return 0;
> }
> ```

## Encargo de confirmação e limite de confirmação

O gerenciador de memória rastreia o uso de memória virtual privada confirmada de forma global, denominado **encargo de confirmação** (commit charge). Existe um limite em todo o sistema, chamado **limite de confirmação** (commit limit), sobre a quantidade de memória virtual confirmada que pode existir a qualquer momento. Este limite corresponde ao tamanho total atual de todos os arquivos de paginação mais a quantidade de RAM utilizável pelo SO.

## Bloqueio de memória

As páginas podem ser bloqueadas na memória de duas formas:

- Aplicativos Windows podem chamar `VirtualLock` para bloquear páginas no working set de seu processo.
- Device drivers podem chamar `MmProbeAndLockPages`, `MmLockPagableCodeSection`, `MmLockPagableDataSection` ou `MmLockPagableSectionByHandle`.

## Memória compartilhada e arquivos mapeados

O Windows fornece um mecanismo para compartilhar memória entre processos e o SO. Os primitivos subjacentes no gerenciador de memória usados para implementar memória compartilhada são chamados **objetos de seção** (section objects), que são expostos como objetos de mapeamento de arquivo na Windows API.

Uma seção pode ser conectada a um arquivo aberto em disco (chamado de arquivo mapeado) ou a memória confirmada (para fornecer memória compartilhada). Seções mapeadas para memória confirmada são chamadas de **seções respaldadas por arquivo de página** (page-file-backed sections).

> **Rode no Visual Studio**
> `CreateFileMapping` + `MapViewOfFile` criam uma regiao de memoria compartilhada entre processos. O exemplo abaixo escreve uma string e la de volta.
>
> ```cpp
> #include <windows.h>
> #include <stdio.h>
>
> int main() {
>     // Cria secao respaldada pelo arquivo de paginacao (memoria compartilhada)
>     HANDLE hMap = CreateFileMappingW(
>         INVALID_HANDLE_VALUE,
>         NULL,
>         PAGE_READWRITE,
>         0, 256,
>         L"MinhaSessaoCompartilhada"
>     );
>
>     LPVOID ptr = MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, 256);
>
>     // Escreve na memoria compartilhada
>     lstrcpyW((LPWSTR)ptr, L"dados compartilhados entre processos");
>     printf("Escrito: %ls\n", (LPWSTR)ptr);
>
>     UnmapViewOfFile(ptr);
>     CloseHandle(hMap);
>     return 0;
> }
> ```

## Protegendo a memória

O Windows fornece proteção de memória de quatro maneiras principais:

1. Todas as estruturas de dados e pools de memória de todo o sistema usados por componentes do sistema em modo kernel só podem ser acessados no modo kernel.
2. Cada processo tem um espaço de endereçamento separado e privado, protegido do acesso por qualquer thread pertencente a outro processo.
3. Todos os processadores suportados pelo Windows fornecem alguma forma de proteção de memória controlada por hardware, como leitura/escrita, somente leitura, etc.
4. Objetos de seção de memória compartilhada têm ACLs padrão do Windows que são verificadas quando os processos tentam abri-los.

### Data Execution Prevention (DEP)

O DEP, ou proteção de página no-execute (NX), causa uma falha de acesso quando há uma tentativa de transferir controle para uma instrução em uma página marcada como "sem execução". No Windows de 64 bits, a proteção de execução é sempre aplicada a todos os processos de 64 bits e device drivers. O modo de aplicação pode ser configurado como `AlwaysOff`, `OptIn` (padrão para sistemas cliente), `OptOut` (padrão para servidores) ou `AlwaysOn`.

### Copy-on-write (Cópia na escrita)

Copy-on-write é uma otimização que o gerenciador de memória usa para conservar memória física. Quando um processo mapeia uma visão copy-on-write de um objeto de seção contendo páginas de leitura/escrita, o gerenciador de memória adia a cópia das páginas até que a página seja escrita.

Quando uma thread escreve em uma página copy-on-write:
1. Uma nova página de leitura/escrita é alocada na memória física.
2. O conteúdo da página original é copiado para a nova página.
3. As informações de mapeamento de página neste processo são atualizadas para apontar para o novo local.
4. A exceção é descartada, fazendo com que a instrução que gerou a falha seja re-executada com sucesso.

### Address Windowing Extensions (AWE)

O AWE permite que um processo aloque mais memória física do que pode ser representada em seu espaço de endereçamento virtual. Funciona em três etapas:

1. Alocar a memória física a ser usada com `AllocateUserPhysicalPages`.
2. Criar regiões do espaço de endereçamento virtual para atuar como janelas com `VirtualAlloc` e o flag `MEM_PHYSICAL`.
3. Mapear porções da região física alocada em uma das regiões virtuais com `MapUserPhysicalPages`.

## Heaps em modo kernel (pools de memória do sistema)

Na inicialização do sistema, o gerenciador de memória cria dois pools de memória de tamanho dinâmico:

- **Pool não paginado (Non-paged pool)** — Consiste em intervalos de endereços virtuais do sistema garantidos de residir na memória física em todos os momentos. Podem ser acessados a qualquer momento sem incorrer em falha de página.
- **Pool paginado (Paged pool)** — Uma região de memória virtual no espaço do sistema que pode ser paginada para dentro e para fora do sistema. Device drivers que não precisam acessar a memória no nível DPC/dispatch ou acima podem usar o pool paginado.

### Tamanhos do pool

O pool não paginado começa com um tamanho inicial de 3% da RAM do sistema. O Windows escolhe dinamicamente o tamanho máximo dos pools. O pool não paginado pode crescer até 75% da RAM ou 128 GB (o que for menor). O pool paginado pode crescer até 10 GB em sistemas de 64 bits.

### Listas look-aside

O Windows fornece um mecanismo de alocação de memória rápido chamado **listas look-aside** (look-aside lists). Ao contrário dos pools gerais, uma lista look-aside contém apenas blocos de tamanho fixo. As alocações de listas look-aside são mais rápidas porque não usam spinlocks. O sistema ajusta automaticamente o número de buffers liberados armazenados conforme a frequência de alocação.

## Gerenciador de heap

O gerenciador de heap gerencia alocações dentro de áreas de memória maiores reservadas usando as funções de alocação de memória com granularidade de página. A granularidade de alocação no gerenciador de heap é de 8 bytes em sistemas de 32 bits e de 16 bytes em sistemas de 64 bits.

### Heaps de processo

> **Rode no Visual Studio**
> `HeapAlloc` aloca do heap do processo — mais eficiente que `VirtualAlloc` para pequenas alocacoes, pois nao precisa de granularidade de pagina.
>
> ```cpp
> #include <windows.h>
> #include <stdio.h>
>
> int main() {
>     HANDLE hHeap = GetProcessHeap(); // heap padrao do processo
>
>     // Aloca 64 bytes no heap
>     LPVOID ptr = HeapAlloc(hHeap, HEAP_ZERO_MEMORY, 64);
>     if (ptr) {
>         printf("Alocado no heap: %p\n", ptr);
>
>         // Consulta o tamanho real alocado
>         SIZE_T tamanho = HeapSize(hHeap, 0, ptr);
>         printf("Tamanho real alocado: %zu bytes\n", tamanho);
>
>         HeapFree(hHeap, 0, ptr);
>         printf("Memoria liberada.\n");
>     }
>
>     return 0;
> }
> ```

Cada processo tem pelo menos um heap: o **heap de processo padrão** (default process heap). O heap padrão é criado na inicialização do processo e nunca é excluído durante a vida do processo. Seu tamanho padrão é de 1 MB, mas pode ser aumentado com o flag de linker `/HEAP`. Uma aplicação UWP inclui pelo menos três heaps: o heap padrão, um heap compartilhado para passar argumentos grandes ao Csrss.exe, e um heap criado pela biblioteca de tempo de execução C.

### Tipos de heap

**O NT heap:** É estruturado em duas camadas — uma camada de front-end e o back-end do heap. O back-end lida com a funcionalidade básica, incluindo gerenciamento de blocos dentro de segmentos. A camada de front-end opcional é o **Low-Fragmentation Heap (LFH)**.

**O heap de segmento:** Introduzido no Windows 10. Para tamanhos pequenos (≤ 16.368 bytes), usa o alocador LFH. Para tamanhos ≤ 128 KB (não atendidos pelo LFH), usa o alocador VS (Variable Size). Alocações > 508 KB vão diretamente ao gerenciador de memória (`VirtualAlloc`). O heap de segmento tem uma pegada menor de metadados, sendo adequado para dispositivos com pouca memória.

Por padrão, o heap de segmento é usado por todos os apps UWP e alguns processos do sistema (csrss.exe, lsass.exe, runtimebroker.exe, services.exe, smss.exe e svchost.exe).

### O Low-Fragmentation Heap (LFH)

O LFH evita a fragmentação gerenciando blocos alocados em intervalos de tamanho predeterminados chamados buckets. Quando um processo aloca memória do heap, o LFH escolhe o bucket que mapeia para o menor bloco grande o suficiente para conter o tamanho necessário. O menor bloco é de 8 bytes. O 128º bucket (o último) é usado para alocações entre 15.873 e 16.384 bytes.

## Layouts do espaço de endereçamento virtual

### Layouts do espaço de endereçamento x86

Em sistemas x86, o espaço de endereçamento virtual total tem um máximo teórico de 4 GB. Por padrão, o Windows aloca a metade inferior (0x00000000 a 0x7FFFFFFF) para processos e a metade superior (0x80000000 a 0xFFFFFFFF) para o uso de memória do SO protegido.

O Windows suporta a opção de inicialização `increaseuserva` no Boot Configuration Database, que dá aos processos executando programas especialmente marcados a capacidade de usar até 3 GB de espaço de endereçamento privado, deixando 1 GB para o SO.

### Layout do espaço de endereçamento de 64 bits

O Windows de 64 bits fornece um espaço de endereçamento muito maior para processos: 128 TB no Windows 8.1, Server 2012 R2 e sistemas posteriores.

### Espaço do sistema x86

O espaço do sistema no Windows de 32 bits PAE inclui:
- **Non-paged pool** — Para alocações que devem permanecer na memória física.
- **Paged pool** — Para alocações que podem ser paginadas.
- **Entradas de tabela de página do sistema** — Mapeamentos usados para diversos fins do sistema.
- **Cache do sistema** — Espaço de endereçamento usado pelo gerenciador de cache.
- **Código e dados de modo kernel** — As imagens do kernel e do HAL.
- **Espaço de sessão** — Dados de sessão compartilhados entre processos dentro de uma sessão.

## Tradução de endereços

### Tradução de endereços virtuais x86

Usando estruturas de dados que o gerenciador de memória cria e mantém chamadas **tabelas de página** (page tables), a CPU traduz endereços virtuais em endereços físicos. Cada página do espaço de endereçamento virtual está associada a uma estrutura no espaço do sistema chamada **entrada de tabela de página** (page table entry, PTE), que contém o endereço físico para o qual o endereço virtual é mapeado.

No modo PAE (Physical Address Extension) do x86, a sequência de tradução de endereço virtual para físico é:

1. Os dois bits mais significativos do endereço virtual (bits 30 e 31) fornecem um índice na PDPT (Page Directory Pointer Table), que tem quatro entradas.
2. 9 bits selecionam uma entrada no diretório de página (PDE), que aponta para a tabela de página.
3. 9 bits selecionam uma entrada na tabela de página (PTE), que aponta para o endereço físico inicial da página.
4. Os 12 bits inferiores do endereço virtual são adicionados ao endereço apontado pela PTE para fornecer o endereço físico final.

### Translation Look-aside Buffer (TLB)

Como fazer três buscas adicionais na memória para cada referência a um endereço virtual quadruplicaria a largura de banda necessária para a memória, resultando em desempenho ruim, todas as CPUs armazenam em cache as traduções de endereços para que acessos repetidos aos mesmos endereços não precisem ser repetidamente traduzidos. Este cache é chamado **translation lookaside buffer (TLB)**. Se um PTE do sistema tiver o bit global definido (como feito pelo Windows para páginas do espaço do sistema visíveis a todos os processos), a entrada do TLB não é invalidada em trocas de contexto de processo.

### Tradução de endereços virtuais x64

No x64, o endereçamento virtual usa 4 níveis de tabelas de página (PML4, PDPT, PD, PT), cada uma com 512 entradas de 8 bytes. Os 9 bits mais significativos do endereço virtual selecionam uma entrada na tabela de nível superior (PML4), e assim por diante.

## Tratamento de falha de página

Quando um thread acessa um endereço virtual que não está mapeado para um endereço físico, a CPU gera uma exceção chamada **falha de página** (page fault). O gerenciador de memória tem um handler de trap que resolve essas exceções.

As falhas de página podem ser **soft** (suaves) — a página está na memória mas não está no working set do processo — ou **hard** (duras) — a página precisa ser lida do disco. Falhas de página duras são muito mais caras que falhas suaves.

### PTEs inválidos

Se um PTE é inválido (bit de validade está limpo), pode representar diferentes estados:

- **Demanda zero** — A página nunca foi acessada e precisa ser preenchida com zeros.
- **Arquivo de paginação** — A página está no arquivo de paginação e precisa ser lida de volta.
- **Arquivo mapeado** — A página pertence a um arquivo mapeado e precisa ser lida do arquivo.
- **Lista de transição** — A página está em uma lista de transição (uma das listas de página do gerenciador de memória).

### PTEs protótipos

Para facilitar o compartilhamento de páginas entre processos (como para DLLs carregadas por múltiplos processos), o gerenciador de memória usa **PTEs protótipos** (prototype PTEs). Cada seção de arquivo mapeado tem um array de PTEs protótipos, um por página na seção. Quando vários processos mapeiam a mesma seção, cada processo aponta seu PTE de processo para o PTE protótipo correspondente.

## Arquivos de página (Page files)

O Windows usa um ou mais arquivos de página para armazenar páginas virtuais que não cabem na memória física. O tamanho total de todos os arquivos de página, mais a quantidade de RAM, define o limite de confirmação do sistema.

### Encargo de confirmação e tamanho do arquivo de página

O encargo de confirmação do sistema mede o total de páginas privadas confirmadas em todos os processos, mais o espaço privado em uso no arquivo de paginação. O sistema pode expandir automaticamente um ou mais arquivos de paginação se não estiverem em seu tamanho máximo configurado.

## Pilhas

### Pilhas de modo usuário

Como descrito no Capítulo 3, quando uma thread é criada, uma pilha é criada reservando uma porção contígua do espaço de endereçamento do processo. O tamanho padrão é 1 MB, mas pode ser substituído com as chamadas `CreateThread` e `CreateRemoteThread(Ex)`. Por padrão, a primeira página na pilha é confirmada e a próxima página é marcada como uma **guard page** que expande a pilha conforme necessário.

### Pilhas de modo kernel

Cada thread também tem uma pilha de modo kernel. A pilha de modo kernel tem um tamanho fixo especificado em tempo de compilação. Em sistemas de 64 bits, o tamanho da pilha de modo kernel padrão é de 24 KB.

## Descritores de endereços virtuais (VADs)

O gerenciador de memória usa uma estrutura de dados eficiente chamada **VAD tree** (árvore de descritores de endereços virtuais) para manter o controle de quais regiões do espaço de endereçamento estão em uso em cada processo. A árvore VAD é uma árvore AVL (self-balancing) onde cada nó descreve uma faixa contígua de regiões reservadas no espaço de endereçamento de um processo.

## Objetos de seção

**Objetos de seção** (section objects) são o mecanismo de memória compartilhada fundamental. Um objeto de seção pode ser conectado a um arquivo no disco ou a memória confirmada. O objeto de seção é o mecanismo que o carregador de imagens usa para mapear executáveis e DLLs, e que o gerenciador de cache usa para acessar dados em arquivos em cache.

A estrutura interna de um objeto de seção inclui:
- **Área de controle** (control area) — Descreve o objeto de seção e aponta para seu segmento e a lista de visões mapeadas.
- **Segmento** (segment) — Contém informações sobre o arquivo subjacente ou espaço de arquivo de página.
- **Subseções** (subsections) — Descrevem partes individuais do arquivo mapeado.

## Working sets

Um subconjunto de páginas virtuais residentes na memória física é chamado de working set. Há três tipos de working sets:

- **Working sets de processo** — Contêm as páginas referenciadas por threads dentro de um único processo.
- **Working set do sistema** — Contém o subconjunto residente do código de sistema paginável (por exemplo, Ntoskrnl.exe e drivers), pool paginado e o cache do sistema.
- **Working set de sessão** — Cada sessão tem um working set que contém o subconjunto residente das estruturas de dados de sessão em modo kernel.

### Demand paging (Paginação por demanda)

O gerenciador de memória do Windows usa um algoritmo de paginação por demanda com clustering para carregar páginas na memória. Quando uma thread recebe uma falha de página, o gerenciador de memória carrega na memória a página com falha mais um pequeno número de páginas antes e/ou depois dela. Para falhas de página que referenciam páginas de dados em imagens, o tamanho do cluster é três páginas. Para todas as outras falhas de página, o tamanho do cluster é sete páginas.

### Prefetcher lógico e ReadyBoot

O prefetcher lógico monitora os dados e o código acessados durante inicializações de boot e de aplicativos e usa essas informações no início de uma inicialização ou partida de aplicativo subsequente para ler o código e os dados. O prefetcher monitora os primeiros 10 segundos da inicialização do aplicativo. Para boot, o prefetcher rastreia por padrão desde o início do sistema até 30 segundos após o início do shell do usuário.

### Política de colocação

Quando uma nova página é adicionada ao working set de um processo, o gerenciador de memória deve escolher qual página física usar. O gerenciador de memória usa uma política de colocação baseada no **banco de dados de número de quadro de página** (PFN database) para determinar qual página física usar.

### Gerenciamento de working set

O working set manager usa um algoritmo de envelhecimento (aging) para acompanhar quais páginas no working set foram referenciadas recentemente. O working set manager também usa as informações do bit de acesso de hardware para determinar se uma página foi referenciada. Quando o gerenciador de memória precisa liberar páginas para satisfazer demandas de memória, ele remove páginas de working sets que estão acima de seu tamanho máximo configurado.

## Banco de dados de número de quadro de página (PFN database)

O **banco de dados PFN** (PFN database) é a estrutura de dados principal do gerenciador de memória que descreve o estado de cada página física de memória no sistema. Cada entrada da PFN database inclui informações como o estado atual da página (ativa, modificada, em espera, etc.), um referência ao PTE que está mapeando essa página física, e informações de contagem de referências.

## Dinâmica da lista de páginas

O gerenciador de memória organiza as páginas físicas em várias listas:

- **Lista livre (Free)** — Páginas que foram liberadas mas que ainda contêm dados (não zeradas).
- **Lista zerada (Zeroed)** — Páginas que foram zeradas e estão prontas para uso.
- **Lista de espera (Standby)** — Páginas que fazem parte do cache do sistema (ex: pages de arquivos mapeados que não fazem parte do working set ativo de nenhum processo).
- **Lista modificada (Modified)** — Páginas que fazem parte do working set de um processo mas foram modificadas e precisam ser escritas no arquivo de paginação ou arquivo mapeado antes de serem reutilizadas.
- **Lista modificada sem gravação (Modified No Write)** — Semelhante à lista modificada, mas estas páginas são paginadas para o arquivo de paginação somente quando o sistema fica sem memória.
- **Páginas ativas** — Páginas atualmente no working set de um processo ou de uso especial do sistema.
- **Lista inválida/ruim (Bad)** — Páginas que foram identificadas como com falha de hardware.

### Prioridade de página

Cada página tem uma prioridade associada a ela que indica sua "idade relativa". As prioridades de página variam de 0 (mais baixa) a 7 (mais alta). Quando o gerenciador de memória precisa liberar páginas, ele começa com as páginas de prioridade mais baixa na lista de espera.

### Escritor de página modificada e escritor de página mapeada

O **escritor de página modificada** (modified page writer) escreve páginas sujas de volta para o arquivo de paginação apropriado. O **escritor de página mapeada** (mapped page writer) escreve páginas sujas em arquivos mapeados para o disco ou armazenamento remoto.

## Limites de memória física

O Windows suporta quantidades variáveis de memória física dependendo da versão e edição:
- Windows 10 Home: 128 GB
- Windows 10 Pro/Enterprise: 2 TB
- Windows Server 2016 Datacenter: 24 TB

## Compressão de memória

O Windows 10 introduziu a compressão de memória como um mecanismo para aumentar a quantidade efetiva de memória física disponível. Em vez de escrever páginas diretamente no disco quando a memória fica escassa, o gerenciador de memória pode primeiro comprimir as páginas e mantê-las na memória.

O processo de compressão usa o **Store Manager**, que gerencia um ou mais stores (armazéns). Cada store contém páginas comprimidas organizadas em regiões de 128 KB. A compressão é realizada na thread atual com baixa prioridade de CPU (7) para minimizar a interferência.

A partir do Windows 10 Versão 1607, um novo processo dedicado chamado **Memory Compression** é usado para armazenar páginas comprimidas. Este é um processo mínimo, que não carrega nenhuma DLL.

## Partições de memória

O Windows 10 (versões de 64 bits apenas) e o Windows Server 2016 fornecem controle de memória por meio de **Partições de Memória** (Memory Partitions). Uma partição de memória consiste em suas próprias estruturas de gerenciamento de memória, como listas de páginas, encargo de confirmação, working set, etc., mas isoladas de outras partições.

Uma partição sempre existe, chamada de **Partição do Sistema** (System Partition), e representa o sistema como um todo. Partições de memória podem ser criadas a partir do modo usuário ou do modo kernel usando a função interna `NtCreatePartition`. Os manipuladores reais de partição são realizados com a função `NtManagePartition`.

## Combinação de memória

A **combinação de memória** (memory combining) é um mecanismo pelo qual o gerenciador de memória encontra páginas duplicadas na RAM e as combina em uma, usando copy-on-write para garantir que as modificações subsequentes funcionem corretamente.

O processo de combinação é acionado pela API nativa `NtSetSystemInformation` com a classe `SystemCombinePhysicalMemoryInformation`. O chamador deve ter o privilégio `SeProfileSingleProcessPrivilege`.

## Enclaves de memória (Memory enclaves)

O Windows 10 Creators Update introduziu o suporte a **enclaves de memória** (memory enclaves) usando a tecnologia Intel SGX (Software Guard Extensions). Um enclave é uma região protegida dentro do espaço de endereçamento de um processo que é protegida de acesso pelo SO e outros processos.

## Gerenciamento de memória proativo (SuperFetch)

O **SuperFetch** é um componente do Windows que tenta antecipar quais páginas um processo provavelmente precisará e as pré-carrega na memória antes de serem necessárias. O SuperFetch mantém rastreia os padrões de acesso de memória dos processos ao longo do tempo e usa essas informações para fazer previsões sobre uso futuro.

O **ReadyBoost** e o **ReadyDrive** são extensões do SuperFetch que usam mídia flash (como unidades USB ou SSDs) como um cache de nível intermediário entre a RAM e os discos rígidos mais lentos.

## Conclusão

Este capítulo examinou como o Windows implementa e gerencia a memória virtual, incluindo como as páginas físicas são alocadas, paginadas, compartilhadas e protegidas. No próximo capítulo, exploraremos o sistema de I/O do Windows — como ele funciona e como se integra com os device drivers.
