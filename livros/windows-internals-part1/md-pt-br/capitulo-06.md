# Capítulo 6 — Sistema de I/O

> *Tradução do livro Windows Internals, 7ª Edição*  
> *Pavel Yosifovich, Alex Ionescu, Mark E. Russinovich, David A. Solomon*  
> *Microsoft Press, 2017*

---

O sistema de I/O do Windows consiste em vários componentes executivos que, juntos, gerenciam dispositivos de hardware e fornecem interfaces para dispositivos de hardware para aplicativos e o sistema. Este capítulo lista os objetivos de design do sistema de I/O, examina a estrutura e os componentes do sistema de I/O e os vários tipos de device drivers. Discute as estruturas de dados-chave que descrevem dispositivos, device drivers e solicitações de I/O, após o que descreve as etapas necessárias para completar solicitações de I/O conforme elas se movem pelo sistema.

## Componentes do sistema de I/O

Os objetivos de design para o sistema de I/O do Windows são fornecer uma abstração de dispositivos com os seguintes recursos:

- Segurança uniforme e nomenclatura em dispositivos para proteger recursos compartilháveis.
- I/O assíncrono de alto desempenho baseado em pacotes para permitir a implementação de aplicativos escaláveis.
- Serviços que permitem que drivers sejam escritos em linguagem de alto nível e facilmente portados entre diferentes arquiteturas de máquina.
- Camadas e extensibilidade para permitir a adição de drivers que modificam transparentemente o comportamento de outros drivers ou dispositivos.
- Carregamento e descarregamento dinâmico de device drivers para que drivers possam ser carregados sob demanda.
- Suporte para Plug and Play, onde o sistema localiza e instala drivers para hardware recém-detectado.
- Suporte para gerenciamento de energia para que o sistema ou dispositivos individuais possam entrar em estados de baixo consumo.
- Suporte para vários sistemas de arquivos instaláveis, incluindo FAT, CDFS, UDF, ReFS e NTFS.
- Suporte a Windows Management Instrumentation (WMI) e diagnóstico.

Os componentes executivos do sistema de I/O incluem:

- **O I/O manager** — O coração do sistema de I/O. Conecta aplicativos e componentes do sistema a dispositivos virtuais, lógicos e físicos, e define a infraestrutura que suporta device drivers.
- **Device drivers** — Fornecem uma interface de I/O para um tipo específico de dispositivo. Recebem comandos roteados pelo I/O manager e informam o I/O manager quando esses comandos são concluídos.
- **O PnP manager** — Trabalha com o I/O manager e um tipo de device driver chamado bus driver para orientar a alocação de recursos de hardware e detectar e responder à chegada e remoção de dispositivos de hardware.
- **O power manager** — Trabalha com o I/O manager e o PnP manager para orientar o sistema e device drivers individuais através de transições de estado de energia.
- **O HAL** — Isola drivers das especificidades do processador e do controlador de interrupção fornecendo APIs que ocultam diferenças entre plataformas.

## O I/O manager

O I/O manager é o núcleo do sistema de I/O. É um sistema **orientado a pacotes** — a maioria das solicitações de I/O é representada por um **I/O Request Packet (IRP)**, que é uma estrutura de dados que contém informações que descrevem completamente uma solicitação de I/O. O IRP viaja de um componente do sistema de I/O para outro.

O I/O manager cria um IRP na memória para representar uma operação de I/O, passando um ponteiro para o IRP ao driver correto e descartando o pacote quando a operação de I/O é concluída.

Além de criar e descartar IRPs, o I/O manager fornece código comum a diferentes drivers que eles podem chamar para realizar seu processamento de I/O. Por exemplo, o I/O manager fornece uma função que permite que um driver chame outros drivers.

## Processamento típico de I/O

A maioria das solicitações de I/O começa com um aplicativo executando uma função relacionada a I/O (por exemplo, lendo dados de um dispositivo). Uma chamada típica do cliente em modo usuário chama as funções `CreateFile` ou `CreateFile2` para obter um handle para um arquivo virtual.

O SO abstrai todas as solicitações de I/O como operações em um arquivo virtual porque o I/O manager só tem conhecimento de arquivos. O diretório do gerenciador de objetos chamado `GLOBAL??` contém nomes simbólicos (como C:) para todos os dispositivos. Todos os nomes no diretório `GLOBAL??` são candidatos a argumentos para `CreateFile(2)`.

> **Rode no Visual Studio**
> I/O sincrono basico: cria um arquivo, escreve dados e le de volta. A thread aguarda cada operacao terminar antes de continuar.
>
> ```cpp
> #include <windows.h>
> #include <stdio.h>
>
> int main() {
>     HANDLE hArquivo = CreateFileW(L"teste_io.txt",
>         GENERIC_WRITE | GENERIC_READ,
>         0, NULL, CREATE_ALWAYS,
>         FILE_ATTRIBUTE_NORMAL, NULL);
>
>     // Escreve
>     const char dados[] = "I/O sincrono via WriteFile";
>     DWORD escritos = 0;
>     WriteFile(hArquivo, dados, sizeof(dados) - 1, &escritos, NULL);
>     printf("Escritos: %lu bytes\n", escritos);
>
>     // Volta ao inicio do arquivo e le
>     SetFilePointer(hArquivo, 0, NULL, FILE_BEGIN);
>     char buffer[64] = {};
>     DWORD lidos = 0;
>     ReadFile(hArquivo, buffer, sizeof(buffer) - 1, &lidos, NULL);
>     printf("Lidos: %lu bytes | Conteudo: %s\n", lidos, buffer);
>
>     CloseHandle(hArquivo);
>     DeleteFileW(L"teste_io.txt");
>     return 0;
> }
> ```

## Interrupt Request Levels (IRQLs) e Deferred Procedure Calls (DPCs)

### Interrupt Request Levels (IRQLs)

O IRQL tem dois significados distintos:
- Uma prioridade atribuída a uma fonte de interrupção de um dispositivo de hardware.
- Cada CPU tem seu próprio valor de IRQL que deve ser considerado um registrador da CPU.

A regra fundamental dos IRQLs é que o código com IRQL mais alto pode preemptar o código rodando em IRQL mais baixo.

Os IRQLs mais importantes para discussões relacionadas a I/O são:

| IRQL | Nível | Descrição |
|------|-------|-----------|
| Passive (0) | `PASSIVE_LEVEL` | IRQL normal onde o agendador do kernel trabalha normalmente |
| Dispatch/DPC (2) | `DISPATCH_LEVEL` | O IRQL em que o agendador do kernel trabalha. Neste nível, a thread tem efetivamente quantum infinito |
| Device IRQL (3–26 em x86; 3–12 em x64 e ARM) | `DIRQL` | Níveis atribuídos a interrupções de hardware |

Código rodando no nível IRQL 2 ou acima tem restrições importantes: não pode esperar em objetos despachantes do kernel, e não pode incorrer em falhas de página (porque tratamento de falhas de página requer uma troca de contexto). Isso significa que o código nesse nível só pode acessar memória não paginada (non-paged pool).

### Deferred Procedure Calls (DPCs)

Um DPC é um objeto que encapsula uma chamada a uma função no nível `DPC_LEVEL` (2). DPCs existem principalmente para processamento pós-interrupção — uma ISR típica faz o trabalho mínimo possível (lê o estado do dispositivo e diz a ele para parar o sinal de interrupção) e então adia o processamento adicional para um IRQL mais baixo (2) enfileirando um DPC.

O termo *Deferred* (Diferido) significa que o DPC não será executado imediatamente — ele não pode ser executado porque o IRQL atual é maior que 2. Entretanto, quando a ISR retorna, se não houver interrupções pendentes esperando para ser atendidas, o IRQL da CPU cai para 2 e os DPCs acumulados são executados.

## Device drivers

### Tipos de device drivers

O Windows suporta uma ampla gama de tipos de device drivers e ambientes de programação. A classificação mais ampla de um driver é se ele é um driver em modo usuário ou modo kernel.

**Drivers em modo usuário:**
- Drivers de impressora do subsistema Windows.
- Drivers UMDF (User-Mode Driver Framework) — drivers de dispositivos de hardware que rodam em modo usuário.

**Tipos de drivers em modo kernel:**
- **File-system drivers** — Aceitam solicitações de I/O a arquivos e satisfazem as solicitações emitindo suas próprias solicitações mais explícitas para drivers de dispositivo de armazenamento em massa ou de rede.
- **Plug and Play drivers** — Trabalham com hardware e se integram com o power manager e o PnP manager do Windows. Incluem drivers para dispositivos de armazenamento em massa, adaptadores de vídeo, dispositivos de entrada e adaptadores de rede.
- **Non-Plug and Play drivers** — Incluem extensões de kernel que não integram com o PnP manager ou o power manager porque geralmente não gerenciam hardware real.

### WDM drivers

Os drivers WDM aderem ao Windows Driver Model (WDM). Há três tipos de drivers WDM:

- **Bus drivers** — Gerenciam controladores de barramento ou adaptadores.
- **Function drivers** — Gerenciam o lado funcional de um dispositivo.
- **Filter drivers** — Opcionais, implementados acima ou abaixo dos function drivers para modificar o comportamento do dispositivo ou driver.

### Estrutura de um driver

Um driver é uma imagem PE (Portable Executable) carregável, tipicamente com extensão `.sys`. Além das seções normais de código e dados que um executável pode ter, um driver tem as seguintes características:

- **Routine DriverEntry** — Ponto de entrada chamado pelo I/O manager quando o driver é carregado. O driver registra outras rotinas de entrada-saída (dispatch routines).
- **Dispatch routines** — Funções que o driver registra no objeto driver para lidar com solicitações de I/O específicas (por exemplo, `IRP_MJ_READ`, `IRP_MJ_WRITE`, `IRP_MJ_DEVICE_CONTROL`).
- **StartIO routine** — Uma rotina chamada pelo I/O manager quando o dispositivo deve iniciar o processamento de uma solicitação.
- **ISR (Interrupt Service Routine)** — Rotina chamada quando o dispositivo sinaliza uma interrupção.
- **DPC routine** — Executada após uma ISR para completar processamento pós-interrupção.
- **Completion routines** — Chamadas quando uma solicitação de I/O em uma pilha em camadas completa um nível.

### Objetos driver e objetos dispositivo

O I/O manager representa drivers carregados com **objetos driver** (driver objects) e dispositivos controlados por esses drivers com **objetos dispositivo** (device objects). Quando um driver é carregado, o I/O manager cria um objeto driver para representá-lo. O driver então cria um ou mais objetos dispositivo durante sua inicialização.

## I/O request packets (IRPs)

Um IRP é a principal maneira pela qual o I/O manager representa uma solicitação de I/O. Quando uma thread chama um serviço de I/O, o I/O manager constrói um IRP para representar a operação conforme ela progride pelo sistema de I/O.

Um IRP consiste em um **header fixo** (parte fixa) e um ou mais **locais de pilha** (stack locations). Cada local de pilha identifica a função de I/O a ser executada e os parâmetros para essa função para um driver específico na pilha de drivers. Conforme o IRP se move para baixo na pilha de drivers, cada driver usa seu próprio local de pilha no IRP.

### EXPERIMENTO: Visualizando IRPs com o depurador do kernel

Você pode visualizar um IRP com o comando `!irp` do depurador do kernel, passando o endereço de uma estrutura IRP:

```
lkd> !irp <endereço_do_IRP>
```

O resultado mostra todos os locais de pilha do IRP, os objetos dispositivo e driver envolvidos, e as rotinas de conclusão.

## Tipos de I/O

O Windows suporta vários tipos de I/O:

### I/O síncrono e assíncrono

Em **I/O síncrono**, a thread de chamada aguarda a conclusão da operação de I/O antes de continuar. No **I/O assíncrono**, a thread de chamada pode continuar executando enquanto a operação de I/O está em andamento. O I/O assíncrono é essencial para aplicativos de servidor escaláveis. Aplicativos verificam a conclusão de operações de I/O assíncronas usando mecanismos como `GetOverlappedResult`, eventos, ou portas de conclusão de I/O.

> **Rode no Visual Studio**
> I/O assincrono com `OVERLAPPED`: a thread continua rodando enquanto a operacao ocorre e verifica o resultado depois com `GetOverlappedResult`.
>
> ```cpp
> #include <windows.h>
> #include <stdio.h>
>
> int main() {
>     HANDLE hArquivo = CreateFileW(L"teste_async.txt",
>         GENERIC_WRITE | GENERIC_READ,
>         0, NULL, CREATE_ALWAYS,
>         FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, // essencial para async
>         NULL);
>
>     // Escreve de forma assincrona
>     OVERLAPPED ov = {};
>     ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
>
>     const char dados[] = "I/O assincrono com OVERLAPPED";
>     WriteFile(hArquivo, dados, sizeof(dados) - 1, NULL, &ov);
>
>     printf("Operacao de escrita iniciada, thread continua rodando...\n");
>
>     // Aguarda conclusao
>     DWORD escritos = 0;
>     GetOverlappedResult(hArquivo, &ov, &escritos, TRUE);
>     printf("Escrita concluida: %lu bytes\n", escritos);
>
>     CloseHandle(ov.hEvent);
>     CloseHandle(hArquivo);
>     DeleteFileW(L"teste_async.txt");
>     return 0;
> }
> ```

### I/O mapeado, em buffer e direto

- **I/O mapeado** — O I/O manager mapeia o buffer fornecido pelo usuário no espaço de endereçamento do sistema para que seja acessível em qualquer contexto de thread.
- **I/O em buffer** — O I/O manager copia o buffer de modo usuário em um buffer de modo kernel alocado para a duração da solicitação de I/O.
- **I/O direto** — O I/O manager usa uma MDL (Memory Descriptor List) para descrever as páginas físicas do buffer de modo usuário para que o driver possa acessá-las diretamente.

### Fast I/O

O fast I/O é um mecanismo que permite que certos tipos de I/O contornem o sistema de IRP normal para operações onde a latência de criar e processar um IRP seria muito alta. O fast I/O é especialmente útil para operações de I/O de arquivo com cache.

### I/O de dispersão/coleta (scatter/gather)

O I/O de dispersão/coleta permite que um único pedido de I/O transfira dados de/para múltiplos buffers não contíguos em memória. `ReadFileScatter` e `WriteFileGather` são as funções da Windows API correspondentes.

## Portas de conclusão de I/O

Uma **porta de conclusão de I/O** (I/O completion port) é um objeto do kernel que fornece um mecanismo eficiente para gerenciar grandes números de operações de I/O assíncronas concorrentes. As portas de conclusão de I/O são baseadas nas filas do kernel (KQUEUE), que fornecem funcionalidade de despacho de trabalho thread-safe e eficiente.

Um pool de threads de trabalhadores aguarda na porta de conclusão de I/O por operações de I/O concluídas. Quando uma operação é concluída, um worker thread é acordado para processá-la. O sistema garante que apenas um número configurado de threads esteja ativo ao mesmo tempo, limitando a sobrecarga de troca de contexto.

## Priorização de I/O

O Windows suporta três tipos de priorização de I/O:

- **Largura de banda de I/O** — Permite que drivers gerenciem largura de banda de I/O por processo para melhorar a qualidade de serviço.
- **Prioridade de I/O** — Cinco prioridades de I/O definidas pelo sistema que são atribuídas a IRPs individuais:
  - `IoPriorityVeryLow` — Para processos idle e operações em segundo plano.
  - `IoPriorityLow` — Para processos de baixa prioridade.
  - `IoPriorityNormal` — Para operações de I/O regulares.
  - `IoPriorityHigh` — Reservado para o sistema.
  - `IoPriorityCritical` — Reservado para o sistema para operações críticas.
- **Reserva de largura de banda** (não implementada na versão coberta por este livro).

## Driver Verifier

O **Driver Verifier** é um utilitário que ajuda os desenvolvedores de device drivers a encontrar bugs como estouros de buffer, vazamentos de memória, uso imprópido de APIs do kernel, etc. Ele implementa verificações opcionais de integridade de drivers em modo kernel e código.

O Driver Verifier pode ser ativado usando a ferramenta `Verifier.exe` ou `Verifier.cmd`. Quando habilitado, pode detectar:

- Verificações de memória (verificação de pools especiais, rastreamento de pool).
- Verificações de I/O (verificação de IRP aprimorada, incluindo verificação de pilha de IRP).
- Verificações relacionadas ao DMA (verificação de DMA).
- Verificações de bloqueio (verificação de aquisição de spinlock).
- E muitas mais.

## O gerenciador de Plug and Play (PnP manager)

O PnP manager executa as seguintes funções:

- **Enumeração de dispositivos** — Detecta dispositivos de hardware e cria objetos de nó de dispositivo (devnodes) para representar cada dispositivo no sistema.
- **Gerenciamento de recursos** — Determina quais recursos de hardware (IRQ, DMA, portas de I/O, intervalos de memória) cada dispositivo requer e resolve conflitos de recursos.
- **Gerenciamento de driver** — Carrega e descarrega drivers conforme os dispositivos são adicionados/removidos.

### Pilhas de dispositivo

O PnP manager constrói uma **pilha de dispositivos** (device stack) para cada dispositivo no sistema. Uma pilha de dispositivos é uma lista ordenada de objetos dispositivo que representam diferentes camadas de drivers para um dispositivo.

A pilha de dispositivo típica de baixo para cima é:
1. Bus driver — Gerencia o barramento ao qual o dispositivo está conectado.
2. Bus filter drivers (opcional) — Filtros abaixo do function driver.
3. Function driver — O driver principal do dispositivo.
4. Device filter drivers (opcional) — Filtros acima do function driver.

### Instalação de driver

O processo de instalação de driver envolve:

1. O PnP manager detecta um novo dispositivo e determina seu hardware ID.
2. O PnP manager procura em sua lista de diretórios de driver por um driver adequado usando o hardware ID.
3. Se um driver é encontrado, o PnP manager o carrega e inicia o dispositivo.
4. Se nenhum driver é encontrado, o Windows exibe um balão de notificação ou alerta o usuário.

## O gerenciador de energia (Power manager)

O gerenciador de energia é o componente executivo que controla as políticas de gerenciamento de energia do sistema. O power manager é responsável por:

- Gerenciar transições de estado de energia do sistema (S0 - funcionando, S1–S4 - sleep, S5 - desligado).
- Enviar notificações de mudança de estado de energia para drivers de dispositivos.
- Coordenar o nível de desempenho da CPU (P-states) com o driver do processador de energia.

### Connected Standby e Modern Standby

Dispositivos móveis suportam um estado especial chamado **Connected Standby** (também chamado de Modern Standby). Neste estado, o sistema parece estar em repouso do ponto de vista do usuário, mas ainda mantém conectividade de rede. Este estado foi introduzido no Windows 8 para dispositivos tablet.

### Power Management Framework (PoFx)

O **Power Management Framework (PoFx)** é um novo framework de gerenciamento de energia introduzido no Windows 8. Ele fornece suporte para gerenciamento de energia de componente individual (por contraste com o gerenciamento de energia de dispositivo inteiro). Isso permite que diferentes componentes de um dispositivo (por exemplo, um processador de sinal digital versus um rádio) sejam colocados em estados de baixo consumo independentemente.

### Solicitações de disponibilidade de energia

O Windows suporta **objetos de solicitação de energia** (power request objects) que permitem que aplicativos e drivers solicitem que o sistema não entre em determinados estados de baixo consumo. Os quatro tipos de solicitação são:

- **Solicitação de sistema** — Impede que o sistema entre automaticamente em sono devido ao temporizador de inatividade.
- **Solicitação de exibição** — O mesmo que a solicitação de sistema, mas para o monitor.
- **Solicitação away-mode** — Mantém o computador no modo totalmente ativo, mas com a exibição e cartão de som desligados.
- **Solicitação de execução necessária** — Para apps UWP continuarem executando mesmo se o gerenciador de ciclo de vida do processo normalmente os terminaria.

## Conclusão

O sistema de I/O define o modelo de processamento de I/O no Windows e executa funções comuns a múltiplos drivers. Suas principais responsabilidades são criar IRPs representando solicitações de I/O e guiar os pacotes através de vários drivers, retornando resultados ao chamador quando um I/O é concluído.

Device drivers incluem não apenas drivers de hardware tradicionais, mas também drivers de sistema de arquivos, rede e filtro em camadas. Todos os drivers têm uma estrutura comum e se comunicam entre si e com o I/O manager usando mecanismos comuns.

Finalmente, o papel do PnP manager é trabalhar com device drivers para detectar dinamicamente dispositivos de hardware e construir uma árvore de dispositivos interna que orienta a enumeração de dispositivos de hardware e a instalação de drivers. O power manager trabalha com device drivers para mover dispositivos para estados de baixo consumo quando aplicável para conservar energia e prolongar a vida da bateria.
