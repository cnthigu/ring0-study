# Capítulo 4 — Threads

> *Tradução do livro Windows Internals, 7ª Edição*  
> *Pavel Yosifovich, Alex Ionescu, Mark E. Russinovich, David A. Solomon*  
> *Microsoft Press, 2017*

---

Este capítulo explica as estruturas de dados e os algoritmos que lidam com threads e agendamento de threads (thread scheduling) no Windows. A primeira seção mostra como criar threads. Em seguida, os internos das threads e do agendamento são descritos. O capítulo conclui com uma discussão sobre pools de threads (thread pools).

## Criando threads

A função de criação mais simples em modo usuário é `CreateThread`. Essa função cria uma thread no processo atual, aceitando os seguintes argumentos:

- **Uma estrutura de atributos de segurança opcional** — Especifica o descritor de segurança a ser anexado à thread recém-criada. Também especifica se o handle da thread deve ser criado como herdável.
- **Um tamanho de pilha opcional** — Se zero for especificado, um padrão é obtido do cabeçalho do executável.
- **Um ponteiro de função** — Serve como ponto de entrada para a execução da nova thread.
- **Um argumento opcional** — Para passar à função da thread.
- **Flags opcionais** — Um controla se a thread começa suspensa (`CREATE_SUSPENDED`). O outro controla a interpretação do argumento de tamanho de pilha.

Uma função estendida de criação de thread é `CreateRemoteThread`. Essa função aceita um argumento extra (o primeiro), que é um handle para um processo alvo onde a thread deve ser criada. Você pode usar essa função para injetar uma thread em outro processo. Um uso comum dessa técnica é um depurador forçar uma quebra em um processo depurado.

A função final digna de menção é `CreateRemoteThreadEx`, que é um superconjunto de `CreateThread` e `CreateRemoteThread`. De fato, a implementação de `CreateThread` e `CreateRemoteThread` simplesmente chama `CreateRemoteThreadEx` com os padrões apropriados.

> **Rode no Visual Studio**
> `CreateThread` cria uma thread no processo atual. O ponteiro de funcao e o argumento permitem passar contexto para ela.
>
> ```cpp
> #include <windows.h>
> #include <stdio.h>
>
> DWORD WINAPI MinhaThread(LPVOID arg) {
>     int numero = *(int*)arg;
>     printf("Thread rodando. TID: %lu | Argumento: %d\n",
>         GetCurrentThreadId(), numero);
>     return 0;
> }
>
> int main() {
>     int valor = 42;
>     HANDLE hThread = CreateThread(NULL, 0, MinhaThread, &valor, 0, NULL);
>
>     if (hThread) {
>         WaitForSingleObject(hThread, INFINITE);
>         CloseHandle(hThread);
>     }
>
>     return 0;
> }
> ```

Criar uma thread em modo kernel é feito com a função `PsCreateSystemThread` (documentada no WDK). Isso é útil para drivers que precisam de trabalho independente a ser processado dentro do processo do sistema. Sair da função de uma thread do kernel não destrói automaticamente o objeto thread. Em vez disso, os drivers devem chamar `PsTerminateSystemThread` de dentro da função da thread para terminar adequadamente a thread.

## Internos da thread

### Estruturas de dados

No nível do sistema operacional, uma thread Windows é representada por um objeto thread executivo. O objeto thread executivo encapsula uma estrutura ETHREAD, que por sua vez contém uma estrutura KTHREAD como seu primeiro membro.

A estrutura KTHREAD contém informações que o kernel Windows precisa para realizar funções de agendamento, sincronização e contabilidade de tempo. O primeiro membro do ETHREAD é chamado Tcb (thread control block), que é uma estrutura do tipo KTHREAD.

O TEB (thread environment block), ilustrado na Figura 4-3, é uma das estruturas de dados que existe no espaço de endereçamento do processo (ao contrário do espaço do sistema). Internamente, é composto por um cabeçalho chamado Thread Information Block (TIB). O TEB armazena informações de contexto para o carregador de imagens e várias DLLs do Windows. Como esses componentes rodam em modo usuário, eles precisam de uma estrutura de dados gravável a partir do modo usuário — por isso essa estrutura existe no espaço de endereçamento do processo.

#### EXPERIMENTO: Exibindo estruturas ETHREAD e KTHREAD

Você pode exibir as estruturas ETHREAD e KTHREAD com o comando `dt` no depurador do kernel:

```
lkd> dt nt!_ethread
+0x000 Tcb : _KTHREAD
+0x5d8 CreateTime : _LARGE_INTEGER
+0x5e0 ExitTime : _LARGE_INTEGER
...
+0x7b8 Silo : Ptr64 _EJOB

lkd> dt nt!_kthread
+0x000 Header : _DISPATCHER_HEADER
+0x028 InitialStack : Ptr64 Void
+0x030 StackLimit : Ptr64 Void
+0x038 StackBase : Ptr64 Void
+0x048 CycleTime : Uint8B
```

#### EXPERIMENTO: Usando o comando !thread do depurador do kernel

O comando `!thread` exibe um subconjunto das informações nas estruturas de dados da thread. Para exibir informações de thread, use o comando `!process` (que exibe todas as threads de um processo) ou o comando `!thread` com o endereço de um objeto thread:

```
lkd> !process 0 0 explorer.exe
lkd> !process ffffe00018c817c0 2
```

Cada thread mostra seu endereço (ETHREAD), ID do cliente (Cid), endereço do TEB e estado da thread (a maioria deve estar em estado de espera, com o motivo entre parênteses).

#### EXPERIMENTO: Examinando o TEB

Você pode despejar a estrutura TEB com o comando `!teb` no depurador do kernel ou de modo usuário. Para ver o TEB com um depurador de modo usuário, abra o WinDbg, abra o Notepad.exe e emita o comando `!teb`:

```
0:000> !teb
TEB at 000000ef125c1000
ExceptionList: 0000000000000000
StackBase:     000000ef12290000
StackLimit:    000000ef1227f000
ClientId:      00000000000021bc . 0000000000001b74
PEB Address:   000000ef125c0000
LastErrorValue: 0
```

### Nascimento de uma thread

O ciclo de vida de uma thread começa quando um processo cria uma nova thread. A solicitação filtra para o executive do Windows, onde o gerenciador de processos aloca espaço para um objeto thread e chama o kernel para inicializar o bloco de controle de thread (KTHREAD). As seguintes etapas são realizadas dentro de `CreateRemoteThreadEx` em Kernel32.dll:

1. A função converte os parâmetros da Windows API para flags nativas.
2. Constrói uma lista de atributos com dois entries: ID do cliente e endereço TEB.
3. Determina se a thread é criada no processo chamador ou em outro processo.
4. Chama `NtCreateThreadEx` (em Ntdll.dll) para fazer a transição para o executive em modo kernel.
5. `NtCreateThreadEx` (dentro do executive) cria e inicializa o contexto de thread em modo usuário.
6. `CreateRemoteThreadEx` aloca um contexto de ativação para a thread.
7. A menos que o chamador criou a thread com o flag `CREATE_SUSPENDED`, a thread é retomada.
8. O handle da thread e o ID da thread são retornados ao chamador.

### Limitações em threads de processos protegidos

Como discutido no Capítulo 3, processos protegidos (clássicos ou PPL) têm várias limitações em termos de quais direitos de acesso serão concedidos. As únicas permissões concedidas são `THREAD_SUSPEND_RESUME` e `THREAD_SET/QUERY_LIMITED_INFORMATION`. Isso garante que o código real em execução dentro do processo protegido não possa ser sequestrado por funções padrão do Windows.

## Agendamento de threads

### Visão geral do agendamento do Windows

O Windows implementa um sistema de agendamento preemptivo orientado a prioridade (priority-driven, preemptive scheduling system). Ao menos uma das threads de maior prioridade executáveis (prontas) sempre roda, com a ressalva de que certas threads de alta prioridade prontas para rodar podem ser limitadas pelos processadores em que podem ser executadas — fenômeno chamado **afinidade de processador** (processor affinity).

Após uma thread ser selecionada para rodar, ela roda por um período de tempo chamado **quantum**. Um quantum é o tempo que uma thread tem permissão de rodar antes de outra thread na mesma prioridade ter sua vez.

Uma thread pode não completar seu quantum porque o Windows implementa um agendador preemptivo. Se outra thread com prioridade mais alta ficar pronta para rodar, a thread atualmente em execução pode ser preemptada antes de terminar seu quantum.

O código de agendamento do Windows está implementado no kernel. Não há um único módulo ou rotina "agendador". O código está espalhado pelo kernel em locais onde ocorrem eventos relacionados ao agendamento. As rotinas que realizam essas funções são coletivamente chamadas de **despachante** (dispatcher) do kernel.

Os seguintes eventos podem requerer despacho de threads:

- Uma thread torna-se pronta para executar — por exemplo, foi recém-criada ou saiu de um estado de espera.
- Uma thread deixa o estado de execução porque seu quantum terminou, ela terminou, cedeu a execução ou entrou em estado de espera.
- A prioridade de uma thread muda.
- A afinidade de processador de uma thread muda.

A cada uma dessas junções, o Windows deve determinar qual thread deve rodar em seguida. Após um processador lógico selecionar uma nova thread para rodar, ele eventualmente realiza uma **troca de contexto** (context switch). Uma troca de contexto é o procedimento de salvar o estado volátil do processador associado a uma thread em execução, carregar o estado volátil de outra thread e iniciar a execução da nova thread.

O Windows agenda na granularidade de thread. Nenhuma consideração é dada a qual processo a thread pertence. Por exemplo, se o processo A tem 10 threads executáveis e o processo B tem 2 threads executáveis, e todas as 12 threads estão na mesma prioridade, cada thread receberia teoricamente um doze avos do tempo de CPU.

### Níveis de prioridade

O Windows usa 32 níveis de prioridade internamente, variando de 0 a 31:

- **Dezesseis níveis em tempo real** (16 a 31)
- **Dezesseis níveis variáveis** (0 a 15), onde o nível 0 é reservado para a thread de página zero.

A Windows API organiza os processos por **classe de prioridade** (priority class):

| Classe | Valor interno |
|--------|--------------|
| Real-Time | 4 |
| High | 3 |
| Above Normal | 6 |
| Normal | 2 |
| Below Normal | 5 |
| Idle | 1 |

A Windows API então atribui uma **prioridade relativa** das threads individuais dentro desses processos (os números representam um delta de prioridade aplicado à prioridade base do processo):

| Prioridade relativa | Delta |
|---------------------|-------|
| Time-Critical | +15 |
| Highest | +2 |
| Above-Normal | +1 |
| Normal | 0 |
| Below-Normal | -1 |
| Lowest | -2 |
| Idle | -15 |

Enquanto um processo tem apenas um único valor de prioridade base, cada thread tem dois valores de prioridade: atual (dinâmica) e base. As decisões de agendamento são feitas com base na prioridade atual. Em certas circunstâncias, o sistema aumenta a prioridade de threads no intervalo dinâmico (1 a 15) por breves períodos. O Windows **nunca** ajusta a prioridade de threads no intervalo em tempo real (16 a 31).

#### Prioridades em tempo real

Você deve ter o privilégio de aumento de prioridade de agendamento (`SeIncreaseBasePriorityPrivilege`) para entrar no intervalo em tempo real. Observe que muitas threads de sistema importantes do Windows rodam no intervalo de prioridade em tempo real, portanto, se as threads passarem tempo excessivo nesse intervalo, podem bloquear funções críticas do sistema.

> **Nota:** O nome "tempo real" não implica que o Windows seja um SO em tempo real no sentido comum do termo. O Windows não fornece instalações de SO em tempo real reais, como latência de interrupção garantida. O termo tempo real realmente significa apenas "mais alto que todos os outros".

### Estados de thread

Antes de examinar os algoritmos de agendamento, você deve entender os vários estados de execução em que uma thread pode estar:

- **Ready (Pronta)** — Uma thread no estado pronto está aguardando para executar. Quando busca uma thread para executar, o despachante considera apenas threads no estado pronto.
- **Deferred Ready (Pronta Diferida)** — Usado para threads que foram selecionadas para rodar em um processador específico mas ainda não começaram a rodar lá. Esse estado existe para que o kernel possa minimizar o tempo que o bloqueio por processador no banco de dados de agendamento é mantido.
- **Standby (Em espera)** — Uma thread nesse estado foi selecionada para rodar em seguida em um processador específico. Apenas uma thread pode estar no estado standby para cada processador. Observe que uma thread pode ser preemptada do estado standby antes de executar.
- **Running (Em execução)** — Após o despachante realizar uma troca de contexto para uma thread, ela entra no estado em execução. Continua até seu quantum terminar, ser preemptada, terminar, ceder a execução ou voluntariamente entrar em estado de espera.
- **Waiting (Esperando)** — Uma thread pode entrar no estado de espera de várias formas: voluntariamente esperar por um objeto, o SO esperar em nome da thread, ou um subsistema de ambiente direcionar a thread a se suspender.
- **Transition (Transição)** — Uma thread entra no estado de transição se está pronta para execução, mas sua pilha do kernel está paginada fora da memória.
- **Terminated (Terminada)** — Quando uma thread termina de executar, entra nesse estado.
- **Initialized (Inicializada)** — Usado internamente enquanto uma thread está sendo criada.

### Banco de dados do despachante

Para tomar decisões de agendamento de threads, o kernel mantém um conjunto de estruturas de dados coletivamente conhecidas como **banco de dados do despachante** (dispatcher database). O banco de dados do despachante rastreia quais threads estão aguardando para executar e quais processadores estão executando quais threads.

Sistemas multiprocessadores Windows têm **filas de pronto por processador** (per-processor dispatcher ready queues) e **filas de pronto de grupo compartilhado** (shared processor group queues). Em cada fila há 32 prioridades. Para agilizar a seleção, o Windows mantém um **resumo de pronto** (ready summary) de 32 bits, onde cada bit definido indica uma ou mais threads na fila de pronto para aquele nível de prioridade.

#### EXPERIMENTO: Visualizando threads prontas

Você pode ver a lista de threads prontas com o comando `!ready` do depurador de kernel, que exibe a thread ou lista de threads prontas para rodar em cada nível de prioridade.

### Quantum

Um quantum é a quantidade de tempo que uma thread tem permissão para rodar antes de o Windows verificar se outra thread na mesma prioridade está esperando para rodar.

- **Sistemas cliente Windows:** as threads rodam por dois intervalos de clock por padrão.
- **Sistemas servidor:** as threads rodam por 12 intervalos de clock por padrão.

A lógica para o valor padrão mais longo em sistemas servidor é minimizar a troca de contexto (context switching). Com um quantum mais longo, aplicativos de servidor que acordam por causa de uma solicitação do cliente têm uma chance melhor de completar a solicitação e voltar ao estado de espera antes de seu quantum terminar.

O quantum é medido em ciclos de clock de CPU, não em ticks de clock. Internamente, uma unidade de quantum é representada como um terço de um tick de clock: em sistemas cliente, o valor de redefinição de quantum é 6 (2 * 3) e em sistemas servidor é 36 (12 * 3) por padrão.

#### Quantums variáveis

Quando quantums variáveis são habilitados (padrão em sistemas cliente), o processo em primeiro plano (foreground) — aquele contendo a thread que possui a janela de primeiro plano na área de trabalho — recebe quantums triplicados. Threads no processo em primeiro plano rodam com um quantum de seis ticks de clock, enquanto threads em outros processos têm o quantum cliente padrão de dois ticks de clock.

A separação de prioridade é armazenada no valor de registro `Win32PrioritySeparation` em `HKLM\SYSTEM\CurrentControlSet\Control\PriorityControl`.

### Boosts de prioridade

O agendador do Windows aumenta periodicamente a prioridade atual (dinâmica) de threads por meio de um mecanismo interno de boost de prioridade. Os cenários de boost incluem:

**Boosts devido a eventos do agendador/despachante (redução de latência):**
- Um APC (Asynchronous Procedure Call) é enfileirado para uma thread.
- Um evento é definido ou pulsado.
- Um mutex é liberado ou abandonado.
- Um processo sai.
- Um semáforo é liberado.

**Boosts de conclusão de I/O (redução de latência):** O Windows dá boosts temporários de prioridade após a conclusão de certas operações de I/O. Os valores de boost recomendados do WDK são:

| Dispositivo | Boost recomendado |
|-------------|------------------|
| Disco | IO_DISK_INCREMENT (1) |
| Rede | IO_NETWORK_INCREMENT (2) |
| Serial | IO_SERIAL_INCREMENT (2) |
| Som | IO_SOUND_INCREMENT (8) |
| Teclado/Mouse | IO_KEYBOARD_INCREMENT / IO_MOUSE_INCREMENT (6) |

**Boosts de interface de usuário (UI):** Threads que possuem janelas recebem um boost adicional de 2 quando acordam por causa de atividade de janelamento, como a chegada de mensagens de janela.

**Boosts para evitar inanição de CPU:** Uma vez por segundo, a thread do balance set manager escaneia as filas de pronto em busca de threads que ficaram no estado pronto por aproximadamente 4 segundos. Se encontrar tal thread, o balance set manager aumenta a prioridade da thread para 15 e define o alvo de quantum para um equivalente de 3 unidades de quantum de CPU.

> **Nota:** O Windows nunca aumenta a prioridade de threads no intervalo em tempo real (16 a 31). Portanto, o agendamento é sempre previsível em relação a outras threads nesse intervalo.

**Autoboost:** O Autoboost é um framework voltado ao problema de inversão de prioridade. A ideia é rastrear proprietários de bloqueios e garantes de bloqueios de tal forma que permita o aumento das prioridades das threads apropriadas para permitir que as threads progridam. A implementação atual usa o framework Autoboost para pushlocks e guarded mutexes.

**MMCSS (Multimedia Class Scheduler Service):** O driver MMCSS (mmcss.sys) garante a reprodução de multimídia sem falhas para aplicativos que se registram nele. Aplicativos cliente podem se registrar com o MMCSS chamando `AvSetMmThreadCharacteristics`. Por padrão, threads de multimídia recebem 80% do tempo de CPU disponível, enquanto outras threads recebem 20%.

### Troca de contexto

Uma troca de contexto típica requer salvar e recarregar:

- Ponteiro de instrução
- Ponteiro de pilha do kernel
- Um ponteiro para o espaço de endereçamento em que a thread roda (o diretório de tabela de página do processo)

O kernel salva essas informações da thread antiga empurrando-as para a pilha do kernel em modo kernel da thread atual, atualizando o ponteiro de pilha e salvando o ponteiro de pilha na estrutura KTHREAD da thread antiga. O ponteiro de pilha do kernel é então definido para a pilha do kernel da nova thread e o contexto da nova thread é carregado.

**Direct Switch:** Introduzido no Windows 8, o Direct Switch permite que uma thread doe seu quantum e boost para outra thread, que é então imediatamente agendada no mesmo processador. Em cenários síncronos cliente/servidor, isso pode produzir melhorias significativas de throughput. O Direct Switch é atualmente usado em:

- Chamadas à API `SignalObjectAndWait`
- ALPC (Advanced Local Procedure Call)
- Chamadas síncronas de RPC (Remote Procedure Call)
- Chamadas remotas de COM (MTA para MTA apenas)

### Cenários de agendamento

**Troca voluntária:** Uma thread pode voluntariamente ceder o uso do processador entrando em estado de espera em algum objeto, chamando funções de espera como `WaitForSingleObject` ou `WaitForMultipleObjects`.

**Preempção:** Uma thread de prioridade mais baixa é preemptada quando uma thread de prioridade mais alta fica pronta para rodar. Quando uma thread é preemptada, ela é colocada no início da fila de pronto para a prioridade em que estava rodando (não no final — ela pode completar seu quantum quando a thread preemptante terminar).

**Fim de quantum:** Quando a thread em execução esgota seu quantum de CPU, o Windows deve determinar se a prioridade da thread deve ser decrementada e se outra thread deve ser agendada no processador. Se a prioridade for reduzida, o Windows procura uma thread mais apropriada para agendar. Se não for reduzida e houver outras threads na fila de pronto com a mesma prioridade, o Windows seleciona a próxima thread e move a thread anteriormente em execução para o final dessa fila.

**Terminação:** Quando uma thread termina de rodar, ela passa do estado em execução para o estado terminado.

### Threads ociosas (Idle threads)

Quando nenhuma thread executável existe em uma CPU, o Windows despacha a thread ociosa (idle thread) dessa CPU. Cada CPU tem sua própria thread ociosa dedicada. Todas as threads ociosas pertencem ao processo ocioso (idle process), com PID 0.

A thread ociosa de cada processador é encontrada via um ponteiro no PRCB daquele processador. A thread ociosa executa em prioridade 0, que é sempre o nível de prioridade mais baixo, e nunca está em nenhuma fila de pronto — ela simplesmente roda quando não há mais nada para rodar.

### Suspensão de thread

As threads podem ser suspensas e retomadas explicitamente com as funções de API `SuspendThread` e `ResumeThread`. Cada thread tem uma contagem de suspensão (suspend count), que é incrementada por suspensão e decrementada por retomada. Se a contagem é 0, a thread está livre para executar.

O Windows 8.1 e Server 2012 R2 adicionaram um mecanismo chamado **Lightweight Suspend** para permitir a suspensão de uma thread que está em estado de espera sem usar o mecanismo APC, manipulando diretamente o objeto da thread na memória.

### Congelamento profundo (Deep freeze)

O congelamento é um mecanismo pelo qual os processos entram em estado suspenso que não pode ser alterado chamando `ResumeThread` nas threads do processo. Isso é útil quando o sistema precisa suspender um aplicativo UWP (quando vai para o segundo plano). O congelamento profundo (Deep freeze) adiciona outra restrição: threads recém-criadas no processo também não podem iniciar.

## Seleção de thread

Sempre que um processador lógico precisa escolher a próxima thread para rodar, ele chama a função do agendador `KiSelectNextThread`. Quando não há thread pronta, a thread ociosa é selecionada. O **agendador ocioso** (idle scheduler) verifica se o agendamento ocioso foi habilitado — se sim, a thread ociosa começa a escanear as filas de pronto de outros processadores.

## Sistemas multiprocessadores

### Conjuntos de pacotes e conjuntos SMT

O Windows usa cinco campos no KPRCB para determinar as decisões corretas de agendamento ao lidar com topologias de processador lógico:

- `CoresPerPhysicalProcessor` — Determinado a partir do CPUID e arredondado para uma potência de 2.
- `LogicalProcessorsPerCore` — Determina se o processador lógico faz parte de um conjunto SMT.
- `PackageProcessorSet` — A máscara de afinidade descrevendo quais outros processadores lógicos neste grupo pertencem ao mesmo processador físico.
- `CoreProcessorSet` — Conecta outros processadores lógicos ao mesmo núcleo (também chamado de conjunto SMT).
- `GroupSetMember` — Define qual bitmask dentro do grupo de processadores atual identifica este processador lógico.

### Sistemas NUMA

Em sistemas NUMA (Non-Uniform Memory Architecture), os processadores são agrupados em unidades menores chamadas nós. O kernel mantém informações sobre cada nó em uma estrutura de dados chamada KNODE. O Windows tenta melhorar o desempenho agendando threads em processadores que estão no mesmo nó que a memória sendo usada.

### Atribuição de grupo de processador

O Windows limita o número atual de grupos de processadores a 20, com cada grupo contendo até 64 processadores lógicos. A rotina `KePerformGroupConfiguration` atribui processadores a grupos durante a inicialização.

### Afinidade

Cada thread tem uma máscara de afinidade que especifica os processadores nos quais a thread tem permissão de rodar. Por padrão, todos os processos (e portanto todas as threads) começam com uma máscara de afinidade igual ao conjunto de todos os processadores ativos em seu grupo atribuído.

A afinidade pode ser alterada através de:
- `SetThreadAffinityMask` para definir a afinidade de uma thread individual.
- `SetProcessAffinityMask` para definir a afinidade de todas as threads em um processo.

> **Rode no Visual Studio**
> O exemplo abaixo cria uma thread, define sua prioridade e fixa sua afinidade ao processador 0.
>
> ```cpp
> #include <windows.h>
> #include <stdio.h>
>
> DWORD WINAPI MinhaThread(LPVOID arg) {
>     printf("Thread no processador: %lu | Prioridade: %d\n",
>         GetCurrentProcessorNumber(),
>         GetThreadPriority(GetCurrentThread()));
>     return 0;
> }
>
> int main() {
>     HANDLE hThread = CreateThread(NULL, 0, MinhaThread, NULL, CREATE_SUSPENDED, NULL);
>
>     SetThreadPriority(hThread, THREAD_PRIORITY_ABOVE_NORMAL);
>     SetThreadAffinityMask(hThread, 1); // fixar no processador 0
>
>     ResumeThread(hThread);
>     WaitForSingleObject(hThread, INFINITE);
>     CloseHandle(hThread);
>
>     return 0;
> }
> ```
- Tornando um processo membro de um job que tem uma máscara de afinidade de job definida.
- Especificando uma máscara de afinidade no cabeçalho da imagem ao compilar o aplicativo.

### Processador ideal e último processador

Cada thread tem três números de CPU armazenados no bloco de controle de thread do kernel:

- **Processador ideal** — O processador preferido em que esta thread deve rodar.
- **Último processador** — O processador em que a thread rodou pela última vez.
- **Próximo processador** — O processador em que a thread estará ou já está rodando.

O processador ideal para uma thread é escolhido quando ela é criada usando uma semente no bloco de controle do processo. A semente é incrementada cada vez que uma thread é criada, de forma que o processador ideal para cada nova thread no processo rotaciona pelos processadores disponíveis.

### CPU Sets

O Windows 10 e Server 2016 introduzem um mecanismo chamado **CPU sets** — uma forma de afinidade que você pode definir para uso pelo sistema como um todo (incluindo atividade de threads do sistema), processos e até threads individuais. A API documentada inclui:

- `GetSystemCpuSetInformation` — Retorna um array de `SYSTEM_CPU_SET_INFORMATION` com dados para cada CPU set.
- `SetProcessDefaultCpuSets` — Define CPU sets padrão para um processo.
- `SetThreadSelectedCpuSets` — Define um CPU set para uma thread específica.

## Seleção de processador

Quando uma thread fica pronta para rodar, a função do agendador `KiDeferredReadyThread` é chamada. O Windows primeiro olha para o processador ideal da thread e depois calcula o conjunto de processadores ociosos dentro da máscara de afinidade difícil da thread.

**Quando há processadores ociosos:** O Windows escolhe o processador ocioso mais apropriado considerando: processadores que estão no mesmo nó NUMA do processador ideal, processadores do mesmo conjunto SMT, o último processador em que a thread rodou, e o processador atual.

**Quando não há processadores ociosos:** O Windows deve decidir se deve preemptar a thread atualmente em execução, comparando os ranks das duas threads (um número interno de agendamento que indica o poder relativo de uma thread).

## Agendamento heterogêneo (big.LITTLE)

O Windows 10 introduziu a capacidade de distinguir entre núcleos de diferentes capacidades em processadores ARM e agendar threads com base no tamanho do núcleo e na política, incluindo o status de primeiro plano da thread, sua prioridade e seu tempo de execução esperado.

## Agendamento baseado em grupo

O Windows 8 e Server 2012 introduziram um mecanismo de agendamento baseado em grupo, construído em torno do conceito de um **grupo de agendamento** (scheduling group, KSCHEDULING_GROUP). Um grupo de agendamento mantém uma política, parâmetros de agendamento e uma lista de blocos de controle de agendamento do kernel (KSCBs).

Um parâmetro importante mantido por um grupo de agendamento é chamado **rank** — uma prioridade de agendamento de todo o grupo de threads. Um rank de valor 0 é o mais alto. Um número de rank mais alto significa que o grupo usou mais tempo de CPU e, portanto, é menos provável de receber mais tempo de CPU. O rank sempre supera a prioridade.

### Dynamic Fair Share Scheduling (DFSS)

O DFSS é um mecanismo que pode ser usado para distribuir de forma justa o tempo de CPU entre sessões rodando em uma máquina. Ele impede que uma sessão monopolize potencialmente a CPU se algumas threads rodando sob essa sessão tiverem prioridade relativamente alta. É habilitado por padrão em um sistema Windows Server que tem a função Remote Desktop.

Para o DFSS ser habilitado, o valor de registro `EnableCpuQuota` deve ser definido como um valor diferente de zero nas chaves de quota. Se habilitado, a variável global `PsCpuFairShareEnabled` é definida como TRUE, o que faz todas as threads pertencerem a grupos de agendamento (exceto processos da sessão 0).

### Limites de taxa de CPU

A infraestrutura de grupo de agendamento pode ser usada de forma mais granular usando um objeto job. Usando `SetInformationJobObject` com `JobObjectCpuRateControlInformation`, você pode aplicar uma das três configurações:

- **Taxa de CPU** — Um valor entre 1 e 10000 representando uma porcentagem multiplicada por 100.
- **Baseado em peso** — Um valor entre 1 e 9, relativo ao peso de outros jobs.
- **Taxas mínima e máxima de CPU** — Especificadas de forma similar à primeira opção.

## Adição e substituição dinâmica de processadores

Sistemas servidor modernos suportam a adição de processadores (bem como sua substituição) enquanto a máquina ainda está rodando. O suporte a processador dinâmico é fornecido através da HAL, que notifica o kernel de um novo processador no sistema através da função `KeStartDynamicProcessor`.

Aplicativos não aproveitam um processador adicionado dinamicamente por padrão. Eles devem solicitar isso usando `SetProcessAffinityUpdateMode` e `QueryProcessAffinityUpdateMode`.

## Worker factories (pools de threads)

Worker factories são o mecanismo interno usado para implementar pools de threads em modo usuário. Essa funcionalidade de pool de threads do kernel no Windows é gerenciada por um tipo de gerenciador de objetos chamado TpWorkerFactory, bem como quatro chamadas de sistema nativas para gerenciar a factory e seus workers.

O worker factory criará uma nova thread sempre que todas as seguintes condições forem atendidas:
- A criação dinâmica de threads está habilitada.
- O número de workers disponíveis é menor que o número máximo de workers configurado para a factory (padrão de 500).
- A worker factory tem objetos vinculados ou uma thread foi ativada no pool.
- Há IRPs pendentes associados a uma thread worker.

Além disso, threads são terminadas sempre que ficaram ociosas — ou seja, não processaram nenhum item de trabalho — por mais de 10 segundos (por padrão).

### Criação da worker factory

O suporte à worker factory é meramente um wrapper para gerenciar tarefas mundanas. Muito da lógica do novo código de pool de threads permanece no lado do Ntdll.dll dessa arquitetura. Não é o código da worker factory que fornece a escalabilidade, espera interna e eficiência do processamento de trabalho — é um componente muito mais antigo do Windows: **portas de conclusão de I/O** (I/O completion ports) ou, mais corretamente, **filas do kernel** (KQUEUE).

De fato, ao criar uma worker factory, uma porta de conclusão de I/O deve ter sido criada pelo modo usuário, e o handle precisa ser passado. É através dessa porta de conclusão de I/O que a implementação em modo usuário enfileirará e aguardará trabalho.

## Conclusão

Este capítulo examinou a estrutura das threads, como elas são criadas e gerenciadas e como o Windows decide quais threads devem rodar, por quanto tempo e em qual processador ou processadores. No próximo capítulo, você verá um dos aspectos mais importantes de qualquer SO: o gerenciamento de memória.
