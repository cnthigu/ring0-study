# Capítulo 2 — Visão Geral da Arquitetura

> Livro didático baseado em *Windows Internals, 7th Edition*.
> Atualizado para Windows 11 com exemplos práticos em C++ moderno.
> Repositório: [ring0-study](https://github.com/cnthigu/ring0-study)

> Navegação: [Anterior](capitulo-02-01-design-goals.md) | [Índice Cap.2](capitulo-02.md) | [Próximo](capitulo-02-03-portabilidade-smp.md)

---

## O mapa do sistema

Antes de detalhar cada componente, é essencial ter uma visão geral de como tudo se encaixa. A arquitetura do Windows tem três grandes regiões separadas por barreiras de privilégio:

```
┌─────────────────────────────────────────────────────────────────────┐
│                         USER MODE (Ring 3)                          │
│                                                                     │
│  ┌──────────────┐  ┌───────────────┐  ┌───────────────────────┐    │
│  │  Processos   │  │  Processos de │  │  Processos de         │    │
│  │  de Usuário  │  │  Serviço      │  │  Sistema              │    │
│  │  (.exe)      │  │  (svchost.exe)│  │  (smss, csrss, lsass) │    │
│  └──────┬───────┘  └───────┬───────┘  └───────────────────────┘    │
│         │                  │                                        │
│  ┌──────┴──────────────────┴──────────────────────────────────┐     │
│  │          Subsystem DLLs                                    │     │
│  │  kernel32.dll │ advapi32.dll │ user32.dll │ ntdll.dll      │     │
│  └─────────────────────────────────────────────────┬──────────┘     │
├─────────────────────────────────────────────────────┼───────────────┤
│                   KERNEL MODE (Ring 0)              │syscall        │
│                                                     ▼               │
│  ┌──────────────────────────────────────────────────────────────┐   │
│  │                    EXECUTIVE (ntoskrnl.exe)                  │   │
│  │  Object Mgr │ Process Mgr │ Memory Mgr │ I/O Mgr │ Security │   │
│  ├──────────────────────────────────────────────────────────────┤   │
│  │                    KERNEL (ntoskrnl.exe)                     │   │
│  │  Thread scheduling │ Interrupts │ Exceptions │ SMP sync      │   │
│  ├──────────────────────────────────────────────────────────────┤   │
│  │  Device Drivers (.sys)     │   Win32k.sys (janelas, GDI)    │   │
│  ├──────────────────────────────────────────────────────────────┤   │
│  │                    HAL (hal.dll)                             │   │
│  └──────────────────────────────────────────────────────────────┘   │
├─────────────────────────────────────────────────────────────────────┤
│                    HYPER-V (Ring -1 / VMX root)                     │
│              VTL 0: Windows normal │ VTL 1: Secure Kernel           │
└─────────────────────────────────────────────────────────────────────┘
```

---

## Os quatro tipos de processos em user mode

### 1. Processos de usuário (User Processes)

O que você normalmente cria: aplicações Win32 (64-bit ou 32-bit), apps UWP/WinRT, aplicações .NET. Cada um roda em seu espaço de endereços privado com um token de segurança.

No Windows 11 moderno, praticamente todos são 64-bit ou 32-bit sobre WoW64 (a camada de compatibilidade para código 32-bit em sistema 64-bit). Aplicações 16-bit (DOS/Windows 3.1) não são mais suportadas em Windows 11 64-bit.

### 2. Processos de Serviço (Service Processes)

Processos que hospedam Windows Services — programas que rodam em background, independente de login de usuário. São gerenciados pelo **Service Control Manager (SCM)**.

Exemplos:
- `svchost.exe` — processo genérico que hospeda múltiplos serviços em uma única instância (econômico em memória)
- `spoolsv.exe` — Print Spooler
- `SearchIndexer.exe` — Windows Search

Serviços podem rodar como SYSTEM, LOCAL SERVICE, ou NETWORK SERVICE — contas de sistema com permissões controladas.

### 3. Processos de Sistema (System Processes)

Processos fixos que fazem parte da infraestrutura do OS. **Não são serviços** — não são gerenciados pelo SCM. São criados diretamente pelo kernel durante o boot.

- `smss.exe` — Session Manager: primeiro processo user mode criado pelo kernel
- `csrss.exe` — Client/Server Runtime Subsystem: o processo do Windows subsystem
- `wininit.exe` — inicialização da Session 0
- `winlogon.exe` — gerencia o processo de login
- `lsass.exe` — Local Security Authority: autentica usuários, mantém políticas de segurança

### 4. Processos de Subsistema de Ambiente (Environment Subsystem Processes)

Processos que implementam a "personalidade" do OS para diferentes ambientes. Historicamente incluíam subsistemas POSIX e OS/2. Hoje, o único subsistema relevante é o Windows (implementado pelo `csrss.exe`).

O WSL (Windows Subsystem for Linux) usa uma abordagem diferente — Pico providers — que veremos adiante.

---

## Por que aplicações não chamam o kernel diretamente?

Esta é uma das perguntas mais importantes para entender a arquitetura. Quando seu código C++ chama `CreateFile`, ele **não** chama o kernel diretamente. A cadeia é:

```
Seu código
  │
  ▼
kernel32.dll → CreateFileW()
  │  (traduz para chamada da Native API)
  ▼
ntdll.dll → NtCreateFile()
  │  (executa a instrução syscall)
  ▼
[transição para kernel mode]
  │
  ▼
ntoskrnl.exe → NtCreateFile() [versão do kernel]
  │  (valida parâmetros, chama I/O Manager)
  ▼
I/O Manager → IRP (I/O Request Packet)
  │
  ▼
File System Driver (NTFS.sys) → executa a operação
```

Por que essa indireção? Várias razões:

1. **Versioning**: a Microsoft pode mudar a implementação interna de `NtCreateFile` sem quebrar código que chama `CreateFileW`. O contrato público é a API documentada, não os internos.

2. **Tradução**: `CreateFileW` faz muito mais do que simplesmente chamar `NtCreateFile`. Ela expande variáveis de ambiente no caminho, converte flags de compatibilidade, e trata erros de forma padronizada para o subsistema Windows.

3. **Subsistemas diferentes**: um programa POSIX chamaria `open()` na DLL do subsistema POSIX, que por sua vez chamaria `NtCreateFile`. A camada do kernel é a mesma para todos os subsistemas.

4. **Segurança**: o kernel valida **todos** os parâmetros que chegam de user mode. Ele não confia em nada. Mesmo que você "quebre" a validação da `kernel32.dll`, o kernel revalida.

---

## Os componentes do kernel mode

### Executive (ntoskrnl.exe — camada superior)

A camada executive implementa os grandes subsistemas do OS:

| Componente | Responsabilidade |
|-----------|-----------------|
| Object Manager | Cria, gerencia e destrói objetos do kernel (arquivos, processos, threads, eventos...) |
| Process Manager | Criação e encerramento de processos e threads |
| Memory Manager | Memória virtual, paginação, gerenciamento de RAM física |
| I/O Manager | Despacha I/O para drivers; implementa o modelo de IRPs |
| Security Reference Monitor (SRM) | Verifica ACLs, tokens, e impõe políticas de segurança |
| Configuration Manager | Implementa o Registro do Windows |
| Plug and Play Manager | Detecta hardware, carrega drivers correspondentes |
| Power Manager | Coordena eventos de energia, suspensão, hibernação |
| Cache Manager | Cache de disco em memória RAM para performance de I/O |

Cada um desses componentes tem seu próprio prefixo de função:

```
Ex...  → Executive support (ExAllocatePool, ExAcquireFastMutex)
Ob...  → Object Manager (ObCreateObject, ObReferenceObject)
Ps...  → Process/Thread Manager (PsCreateSystemThread, PsLookupProcessByProcessId)
Mm...  → Memory Manager (MmAllocatePool, MmMapLockedPages)
Io...  → I/O Manager (IoCreateDevice, IoBuildSynchronousFsdRequest)
Se...  → Security (SeAccessCheck, SeCreateToken)
Cm...  → Configuration Manager/Registry (CmRegisterCallback)
Po...  → Power Manager (PoRequestPowerIrp)
Cc...  → Cache Manager (CcInitializeCacheMap)
```

### Kernel (ntoskrnl.exe — camada inferior)

A camada kernel é mais baixo nível e mais específica de arquitetura:

- **Thread scheduling**: decide qual thread roda em qual processador
- **Dispatcher**: quando uma thread deve ser acordada ou dormida
- **Interrupt handling**: responde a interrupções de hardware
- **Exception dispatching**: trata exceções (divisão por zero, access violation)
- **Synchronization**: spinlocks, mutexes de baixo nível para SMP

Funções da camada kernel têm prefixo `Ke`:
```
KeInitializeSpinLock, KeAcquireSpinLock, KeReleaseSpinLock
KeWaitForSingleObject, KeSetEvent, KeReleaseSemaphore
KiDispatchInterrupt (interno)
```

### Win32k.sys

O subsistema gráfico do Windows roda em kernel mode. Isso é controverso mas pragmático: mover o gerenciador de janelas e o GDI para user mode seria mais seguro, mas o custo de performance seria inaceitável para aplicações gráficas intensivas.

Win32k.sys contém:
- **Window Manager**: gerencia janelas, posicionamento, rendering, input do teclado/mouse
- **GDI (Graphics Device Interface)**: primitivas de desenho (linhas, texto, bitmaps)
- **Wrappers para DirectX**: que fica em `dxgkrnl.sys`

No Windows 10/11, Win32k.sys foi dividido em módulos:
- `Win32kBase.sys` — funcionalidade base comum a todos os devices
- `Win32kFull.sys` — funcionalidades desktop completas (carregado em PCs/laptops)
- `Win32kMin.sys` — versão mínima para dispositivos sem display tradicional (IoT)

Isso reduz a superfície de ataque — dispositivos que não precisam de janelas não carregam o código que as implementa.

### HAL (hal.dll)

O **Hardware Abstraction Layer** isola o resto do kernel das particularidades do hardware físico. Funções como controle de interrupts, timers de alta resolução, e DMA são implementadas no HAL, não diretamente no kernel.

Resultado: o mesmo `ntoskrnl.exe` roda em hardware com APIC, IOAPIC, e diferentes configurações de barramento — o HAL se adapta.

### Drivers (.sys)

Módulos de kernel mode carregados dinamicamente que implementam suporte a hardware, sistemas de arquivos, protocolos de rede, e outros serviços. Veremos em detalhe na seção de drivers.

---

## Subsystem DLLs: a ponte entre mundos

As Subsystem DLLs (kernel32.dll, advapi32.dll, user32.dll, gdi32.dll, etc.) são a **interface pública** do Windows. Elas:

1. Expõem a API documentada que você usa no seu código
2. Validam parâmetros do ponto de vista do subsistema
3. Traduzem para chamadas da Native API (ntdll.dll)
4. Ocasionalmente se comunicam com o processo subsistema (csrss.exe) via ALPC

Um ponto importante: quando uma função da Win32 é "apenas" user mode (como `GetCurrentProcessId`), ela não faz nenhuma syscall. O PID é cached no PEB (Process Environment Block) — uma estrutura em user mode que o kernel preenche na criação do processo. `GetCurrentProcessId` simplesmente lê um campo nessa estrutura.

```cpp
// GetCurrentProcessId() é basicamente isso (simplificado):
DWORD GetCurrentProcessId() {
    return NtCurrentTeb()->ClientId.UniqueProcess;
    // TEB = Thread Environment Block, sempre acessível via GS:[0x30]
    // Nenhuma syscall necessária!
}
```

Isso é uma otimização importante: funções chamadas milhões de vezes por segundo não devem cruzar a barreira user/kernel.

---

*Próximo: [Portabilidade, SMP e Escalabilidade](capitulo-02-03-portabilidade-smp.md)*
