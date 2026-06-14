# Capítulo 2 — Executive, Kernel, HAL e Drivers

> Livro didático baseado em *Windows Internals, 7th Edition*.
> Atualizado para Windows 11 com exemplos práticos em C++ moderno.
> Repositório: [ring0-study](https://github.com/cnthigu/ring0-study)

> Navegação: [Anterior](capitulo-02-05-subsistemas-ntdll.md) | [Índice Cap.2](capitulo-02.md) | [Próximo](capitulo-02-07-processos-sistema.md)

---

## O Executive: os grandes subsistemas do kernel

O **Executive** é a camada superior de `ntoskrnl.exe`. Ele implementa os grandes serviços do OS e é a parte com a qual drivers e código de kernel mode interagem mais diretamente.

### Componentes do Executive e seus prefixos

| Prefixo | Componente | Responsabilidade |
|---------|-----------|-----------------|
| `Ob` | Object Manager | Criar, gerenciar e destruir objetos do kernel |
| `Ps` | Process Manager | Processos e threads (complementa o Kernel) |
| `Mm` | Memory Manager | Memória virtual, page faults, working set |
| `Io` | I/O Manager | Despacho de I/O para drivers via IRPs |
| `Se` | Security Reference Monitor | Verificação de ACLs e tokens em tempo de execução |
| `Cm` | Configuration Manager | O Registro do Windows |
| `Pnp` / `Pi` | Plug and Play Manager | Detecção de hardware, carregamento de drivers |
| `Po` | Power Manager | Suspensão, hibernação, gerenciamento de energia |
| `Cc` | Cache Manager | Cache de arquivo em RAM para performance de I/O |
| `Ex` | Executive Support | Pool allocators, interlocked ops, fast mutexes |
| `Rtl` | Runtime Library | Strings, aritmética, segurança (compartilhado kernel/user) |

As funções "privadas" de cada componente usam o prefixo com `p` ou `i`:
- `Psp...` → funções internas do Process Manager
- `Iop...` → funções internas do I/O Manager
- `Mi...` → funções internas do Memory Manager

```
kd> x nt!Ps*  ← lista todas as funções do Process Manager
kd> x nt!Mm*  ← todas do Memory Manager
kd> x nt!Io*  ← todas do I/O Manager
```

### Funções exportadas vs internas

O Executive exporta funções em três "níveis":

**1. Exportadas para user mode via ntdll**: `NtCreateFile`, `NtCreateProcess`, `NtAllocateVirtualMemory`, etc. Mais de 450 funções. São as syscalls.

**2. Exportadas para drivers (documentadas no WDK)**: `IoCreateDevice`, `ExAllocatePool2`, `KeInitializeEvent`, `MmMapLockedPages`, etc. São as APIs que você usa ao escrever drivers.

**3. Não exportadas (internas)**: `IopCompleteRequest`, `PspCreateProcess`, `MiAllocateMappedView`. Usadas internamente, não disponíveis para drivers. Podem ser encontradas nos símbolos públicos com `kd> x nt!Iop*`.

---

## O Kernel: mecanismos, não políticas

O **Kernel** (camada inferior de `ntoskrnl.exe`) implementa os mecanismos fundamentais que tudo mais usa. A distinção filosófica do livro: o Kernel implementa **mecanismos**, o Executive implementa **políticas**.

Exemplo: o Kernel sabe como fazer context switch entre threads (mecanismo). O Executive define a política de *quando* fazer context switch — qual thread tem maior prioridade, quanto tempo cada thread roda (quantum), etc.

### Objetos de Controle (Control Objects)

Objetos do Kernel usados para controlar o fluxo de execução:

- **APC (Asynchronous Procedure Call)**: permite que código seja injetado na execução de uma thread, assincronamente. Usado pelo I/O manager para completar I/O na thread que iniciou a operação.

- **DPC (Deferred Procedure Call)**: rotinas diferidas que rodam em alta prioridade (IRQL = DISPATCH_LEVEL) após interrupções de hardware. Drivers usam DPCs para adiar trabalho pesado que não pode ser feito dentro da ISR (Interrupt Service Routine).

- **Interrupt Object**: registra handlers para linhas de interrupção específicas.

### Objetos Dispatcher (Synchronization Objects)

Objetos que controlam sincronização e scheduling:

| Objeto | Uso |
|--------|-----|
| `KEVENT` | Sinaliza que algo aconteceu (event) |
| `KMUTEX` (Mutant) | Exclusão mútua (só uma thread por vez) |
| `KSEMAPHORE` | Semáforo contador |
| `KTIMER` | Timer periódico ou one-shot |
| `KTHREAD` | Representa um thread para o scheduler |

Threads chamam `KeWaitForSingleObject` ou `KeWaitForMultipleObjects` para aguardar esses objetos. O scheduler coloca a thread em estado wait até que o objeto seja sinalizado.

### KPCR e KPRCB: dados per-CPU

O kernel mantém uma estrutura por processador chamada **KPCR (Kernel Processor Control Region)**. É acessível via o registrador GS em x64 (`gs:[0]`).

O KPCR contém:
- Ponteiro para IDT (Interrupt Descriptor Table) do processador
- Ponteiro para GDT (Global Descriptor Table)
- IRQL atual do processador
- O KPRCB embutido

O **KPRCB (Kernel Processor Control Block)** é privado do kernel e contém:
- Thread atual, próxima thread, e idle thread do processador
- Ready queues para cada nível de prioridade (64 prioridades no Windows)
- Fila de DPCs pendentes
- Estatísticas: contagem de I/Os, tempo de DPC, contagem de interrupts
- Velocidade do processador em MHz (detectada no boot)
- Topologia NUMA e cache topology

```
kd> dt nt!_KPCR @$pcr      ← ver KPCR do processador atual
kd> !prcb                   ← ver resumo do KPRCB
kd> dt nt!_KPRCB fffff803`c3b23180 MHz  ← campo específico do KPRCB
```

---

## Hardware Abstraction Layer (HAL)

O HAL (`hal.dll`) é a única parte do sistema operacional que toca o hardware físico diretamente. Tudo mais — kernel, executive, drivers — chama funções do HAL.

O HAL abstrai:
- **Controladores de interrupção**: APIC vs outros layouts
- **Timers de alta resolução**: QueryPerformanceCounter, KeQueryPerformanceCounter
- **DMA controllers**: programação de transfers de DMA
- **PCI/ACPI hardware**: leitura de configuração de dispositivos
- **Multiprocessor spin operations**: PAUSE instruction, cache coherency

Por que isso importa para drivers? Porque código de driver que chama HAL em vez de acessar portas I/O diretamente funciona em diferentes hardware sem recompilação.

```c
// ERRADO: acessa hardware diretamente (não portável)
__outbyte(0x70, 0x0A);  // escreve no RTC

// CERTO: usa HAL (portável)
HalGetBusData(PCIConfiguration, busNumber, slotNumber, &config, sizeof(config));
```

### HAL Extensions

No Windows 10/11, hardware muito especializado pode precisar de código além do HAL padrão. Em vez de uma HAL customizada por hardware (como antigamente), existe o mecanismo de **HAL Extensions**: DLLs separadas carregadas pelo boot loader.

Exemplos:
- `HalExtPL080.dll` — controlador DMA PL080 (ARM/IoT)
- `HalExtIntcLpioDMA.dll` — DMA em plataformas Intel de baixo consumo

HAL Extensions requerem um certificado especial e aprovação da Microsoft — não são escritas por terceiros arbitrariamente.

---

## Drivers: tipos e modelos

### Tipos de driver por função

| Tipo | Exemplo | Função |
|------|---------|--------|
| Hardware device drivers | `nvlddmkm.sys` (NVIDIA), `ndis.sys` | Controla hardware físico via HAL |
| File system drivers | `ntfs.sys`, `refs.sys` | Traduz I/O de arquivo para operações de disco |
| File system filter drivers | antivírus, encryption (BitLocker) | Interceta I/O antes/depois de FS drivers |
| Network redirectors | `mrxsmb.sys` (SMB client) | Traduz I/O de arquivo para tráfego de rede |
| Protocol drivers | `tcpip.sys`, `netbt.sys` | Implementa protocolos de rede |
| Software drivers | Process Explorer, Process Monitor | Funcionalidades kernel-mode para tools user-mode |

### Contextos de execução de um driver

Um driver pode receber chamadas em três contextos diferentes:

1. **Contexto do thread do usuário**: quando o user chama `ReadFile`, a thread do usuário entra no kernel e executa código do driver. O driver tem acesso ao espaço de endereços do processo atual.

2. **Thread do sistema**: quando o kernel (ex: Plug and Play Manager) chama o driver para configuração. Não há processo de usuário.

3. **Contexto de interrupção**: quando o hardware gera uma interrupção, a execução ocorre na thread que estava rodando — que pode ser qualquer thread. Não pode fazer chamadas que bloqueiam!

Esta variação de contexto é uma das fontes mais comuns de bugs em drivers.

### WDM (Windows Driver Model)

Introduzido no Windows 2000, WDM adicionou suporte a Plug and Play e gerenciamento de energia. Três tipos de drivers WDM:

**Bus Drivers**: representam o barramento (PCI, USB, I2C). Enumeram os dispositivos conectados ao barramento. Geralmente fornecidos pela Microsoft (`pci.sys`, `usbhub.sys`).

**Function Drivers**: o driver principal de um dispositivo. Implementa a funcionalidade do dispositivo. Geralmente fornecido pelo fabricante do hardware.

**Filter Drivers**: opcionais, colocados acima ou abaixo do function driver para adicionar funcionalidade ou corrigir comportamento do hardware.

```
IRP (I/O Request Packet)
  ↓
Upper Filter Driver (opcional)
  ↓
Function Driver (driver principal)
  ↓
Lower Filter Driver (opcional)
  ↓
Bus Driver
  ↓
Hardware
```

### WDF (Windows Driver Foundation)

WDF é uma abstração sobre WDM que simplifica muito a escrita de drivers:

**KMDF (Kernel-Mode Driver Framework)**:
- Gerencia automaticamente o ciclo de vida de objetos (sem necessidade de `DriverUnload` manual para a maioria dos recursos)
- Power management automático
- Tratamento de Plug and Play simplificado
- Mais de 200 linhas de WDM podem virar 1 chamada KMDF

```c
// KMDF: criar um dispositivo é simples
WDF_OBJECT_ATTRIBUTES attrs;
WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&attrs, MINHA_ESTRUTURA);
WdfDeviceCreate(&deviceInit, &attrs, &meuDispositivo);
// KMDF cuida de cleanup, power, PnP automaticamente
```

**UMDF (User-Mode Driver Framework)**:
- Drivers executam como processos user mode (mais seguro — um crash não derruba o sistema)
- Para dispositivos de alta latência: USB, câmeras, impressoras
- V2.0 (Windows 8.1+) tem o mesmo modelo de programação do KMDF

**Universal Windows Drivers**:
- Drivers que usam apenas APIs presentes em todos os device forms do Windows (desktop, Xbox, IoT)
- Binário compilado para x64 funciona em qualquer hardware x64 com Windows
- Submetidos ao hardware certification via Partner Center

---

## A convenção de nomes do Executive

Entender o padrão de nomenclatura ajuda a decifrar nomes no WinDbg:

```
<Prefixo><Operação><Objeto>

ExAllocatePool2  = Ex (Executive) + Allocate (operação) + Pool (objeto)
IoCreateDevice   = Io (I/O Mgr)  + Create    (operação) + Device (objeto)
PsCreateSystemThread = Ps (Process) + Create + SystemThread
MmAllocateContiguousMemory = Mm (Memory) + Allocate + ContiguousMemory
KeInitializeSpinLock = Ke (Kernel) + Initialize + SpinLock
SeAccessCheck    = Se (Security) + AccessCheck (operação combinada)
```

Funções "internas" (não-exportadas) usam variações:
- `Ps` → `Psp` (private) ou `Psi` (internal): `PspCreateProcess`
- `Io` → `Iop`: `IopCompleteRequest`
- `Ke` → `Ki`: `KiDispatchInterrupt`

---

*Próximo: [Processos de Sistema](capitulo-02-07-processos-sistema.md)*
