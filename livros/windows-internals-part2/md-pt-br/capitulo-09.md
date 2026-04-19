# Capitulo 9 - Tecnologias de Virtualizacao

> *Traducao do livro Windows Internals, Part 2, Seventh Edition*
> *Andrea Allievi, Alex Ionescu, Mark E. Russinovich, David A. Solomon*
> *Microsoft Press, 2022*

---

Este capitulo descreve como o Windows usa as tecnologias de virtualizacao expostas pelos processadores modernos para permitir que usuarios criem e usem multiplas maquinas virtuais no mesmo sistema. A virtualizacao tambem e usada extensivamente pelo Windows para fornecer um novo nivel de seguranca. Portanto, o Secure Kernel e o Isolated User Mode sao amplamente discutidos neste capitulo.

---

## O hypervisor do Windows

O **hypervisor do Windows** e o componente central que permite a virtualizacao no Windows. Ele e implementado no arquivo `hvix64.exe` (em sistemas Intel) ou `hvax64.exe` (em sistemas AMD) e e carregado pelo Windows Boot Loader antes do kernel do Windows.

O hypervisor do Windows e um **hypervisor do Tipo 1** (bare-metal hypervisor), o que significa que ele roda diretamente no hardware, abaixo do sistema operacional host. Isso contrasta com hypervisors do Tipo 2 (como o VMware Workstation em modo legado), que rodam sobre um sistema operacional host.

### Particoes, processos e threads

O hypervisor organiza a execucao em unidades chamadas **particoes** (partitions):

- **Particao raiz (root partition):** A particao privilegiada onde o Windows host roda. So existe uma particao raiz por sistema.
- **Particoes filhas (child partitions):** Particoes onde as maquinas virtuais rodam. Cada VM e uma particao filha separada.

Dentro de cada particao, o hypervisor mantem seus proprios conceitos de processos e threads - chamados de **VPs (Virtual Processors)** - que mapeiam para os processadores logicos fisicos do sistema.

### Inicializacao do hypervisor

O hypervisor e inicializado durante o processo de boot antes do kernel Windows ser carregado. A sequencia de inicializacao e:

1. O Windows Boot Manager verifica se o Hyper-V deve ser habilitado.
2. O hypervisor loader (`hvloader.dll` no contexto do loader) e carregado.
3. O hypervisor configura as estruturas de controle de virtualizacao (`VMCS` em Intel, `VMCB` em AMD).
4. O hypervisor habilita o modo VM (`VMXON` em Intel, `VMRUN` em AMD).
5. O Windows host comeca a rodar como uma particao raiz privilegiada.
6. O kernel do Windows e carregado na particao raiz.

### O gerenciador de memoria do hypervisor

O hypervisor implementa seu proprio gerenciador de memoria, separado do gerenciador de memoria do Windows. Ele usa **Extended Page Tables (EPT)** em Intel ou **Nested Page Tables (NPT)** em AMD para:

- Mapear enderecos fisicos de convidados (GPA - Guest Physical Addresses) para enderecos fisicos do host (HPA - Host Physical Addresses).
- Impor restricoes de acesso a memoria entre particoes.
- Implementar o isolamento de memoria necessario para a seguranca baseada em virtualizacao (VBS).

### Agendadores do Hyper-V

O Hyper-V implementa varios tipos de agendadores para gerenciar como os processadores virtuais das particoes filhas sao mapeados para os processadores fisicos:

**Classic scheduler:** O agendador original que agenda VPs em qualquer processador logico disponivel. Vulneravel a ataques Spectre entre VMs em sistemas com Hyper-Threading.

**Core scheduler:** Introduzido como resposta ao Spectre variante 2. Garante que os dois VPs de uma VM que compartilha um nucleo fisico rodem nos dois threads logicos do mesmo nucleo. Isso evita que VMs maliciosas usem side-channels via compartilhamento de branch predictor entre threads do mesmo nucleo.

**Root scheduler:** Permite que o kernel Windows host tome decisoes de agendamento para as particoes filhas, em vez do hypervisor. Isso permite que o Windows use sua logica de agendamento sofisticada (DFSS, grupos de agendamento, etc.) para as VMs.

### Hypercalls e o TLFS do hypervisor

A interface principal entre o Windows host/convidado e o hypervisor e por meio de **hypercalls** - o equivalente de syscalls para o hypervisor. O hypervisor define uma especificacao chamada **Top-Level Functional Specification (TLFS)** que documenta todas as hypercalls disponiveis.

As hypercalls mais comuns incluem:

| Hypercall | Descricao |
|-----------|-----------|
| `HvCallFlushVirtualAddressSpace` | Invalida entradas TLB para um espaco de enderecamento |
| `HvCallSendSyntheticClusterIpi` | Envia um IPI sintetico para um conjunto de VPs |
| `HvCallNotifyLongSpinWait` | Notifica o hypervisor de uma espera longa em spinlock |
| `HvCallGetVpRegisters` | Le registradores de um VP |
| `HvCallSetVpRegisters` | Define registradores de um VP |

> **Rode no Visual Studio**
> O hypervisor pode ser detectado por meio do CPUID. A folha 0x40000000 retorna informacoes do hypervisor se presente.
>
> ```cpp
> #include <windows.h>
> #include <intrin.h>
> #include <stdio.h>
>
> int main() {
>     int info[4] = {};
>
>     // CPUID folha 1: bit 31 de ECX indica presenca de hypervisor
>     __cpuid(info, 1);
>     BOOL temHypervisor = (info[2] >> 31) & 1;
>     printf("Hypervisor presente: %s\n", temHypervisor ? "sim" : "nao");
>
>     if (temHypervisor) {
>         // CPUID folha 0x40000000: identidade do hypervisor
>         __cpuid(info, 0x40000000);
>         char vendor[13] = {};
>         memcpy(vendor + 0, &info[1], 4);
>         memcpy(vendor + 4, &info[2], 4);
>         memcpy(vendor + 8, &info[3], 4);
>         printf("Vendor do hypervisor: %.12s\n", vendor);
>         // Microsoft Hyper-V retorna "Microsoft Hv"
>     }
>
>     return 0;
> }
> ```

### Intercepts

Um **intercept** e um mecanismo pelo qual o hypervisor pode capturar e processar operacoes que uma VM convidada esta tentando executar. Quando um intercept e configurado para um determinado tipo de operacao (como leitura de um registrador de controle, execucao de uma instrucao privilegiada, acesso a um endereco fisico especifico), o hypervisor recebe controle em vez de deixar a VM executar a operacao diretamente.

Os intercepts mais comuns no Hyper-V incluem:

- **I/O port intercepts:** Capturam acessos a portas de I/O - usados para emular dispositivos.
- **MSR intercepts:** Capturam leituras/escritas de Model Specific Registers (MSRs) especificos.
- **CPUID intercepts:** Permitem ao hypervisor modificar o que o CPUID retorna para VMs.
- **Memory intercepts (EPT violations):** Acionados quando uma VM acessa memoria mapeada de forma especial pelo hypervisor.

### O controlador de interrupcoes sintetico (SynIC)

O **Synthetic Interrupt Controller (SynIC)** e uma extensao do controlador de interrupcoes implementada pelo Hyper-V. Ele fornece um mecanismo de comunicacao eficiente entre VMs e o hypervisor.

O SynIC expoe 16 **SINTs** (Synthetic Interrupt Sources) que podem ser usados para enviar interrupcoes sinteticas para VPs especificos. O mecanismo mais importante do SynIC e o **VMBus**, descrito a seguir.

---

## A pilha de virtualizacao

A pilha de virtualizacao do Windows consiste em varios componentes em modo usuario e modo kernel que trabalham juntos para gerenciar VMs.

### Servicos e processos worker do gerenciador de VM

O componente principal em modo usuario e o **Virtual Machine Management Service (VMMS)** - `vmms.exe`. Este servico e responsavel por:

- Criar, iniciar, pausar e destruir VMs.
- Gerenciar o estado persistente das VMs (arquivos `.vmcx` de configuracao e `.vmrs` de estado em execucao).
- Expor a interface WMI e o PowerShell para gerenciamento de VMs.

Para cada VM em execucao, o VMMS cria um **VM Worker Process** - `vmwp.exe`. Este processo representa a VM no espaco de modo usuario e e responsavel por:

- Emular dispositivos de hardware sinteticos.
- Processar acessos de I/O da VM.
- Comunicar-se com o hypervisor via hypercalls.

### O driver VID e o gerenciador de memoria da pilha de virtualizacao

O **VID (Virtualization Infrastructure Driver)** - `vid.sys` - e um driver em modo kernel que atua como intermediario entre os componentes em modo usuario (como `vmwp.exe`) e o hypervisor.

O VID e responsavel por:

- Criar e gerenciar particoes do hypervisor.
- Alocar memoria fisica para VMs.
- Mapear a memoria da VM no espaco de enderecamento do processo worker (`vmwp.exe`).
- Processar intercepts do hypervisor em nome da VM.

### O nascimento de uma VM

Quando um usuario inicia uma nova VM, a sequencia de eventos e:

1. O VMMS recebe a solicitacao de inicializacao da VM.
2. O VMMS cria um processo `vmwp.exe` para a VM.
3. O `vmwp.exe` comunica com o VID para criar uma nova particao do hypervisor.
4. O VID faz uma hypercall para o hypervisor para criar a particao.
5. O hypervisor aloca os recursos necessarios e retorna um handle de particao.
6. A memoria fisica e alocada e mapeada para a VM.
7. Os dispositivos virtuais sao inicializados (disco virtual, adaptador de rede virtual, etc.).
8. O boot loader virtual e carregado e o sistema convidado e inicializado.

### VMBus

O **VMBus** e um canal de comunicacao de alto desempenho entre a particao raiz e as particoes filhas (VMs). Ele substitui a emulacao de hardware lenta (como emulacao de portas de I/O) por uma interface compartilhada de memoria de alta velocidade.

O VMBus usa os seguintes mecanismos:

- **Shared memory (Grant Table):** Blocos de memoria fisica compartilhados entre a particao raiz e a VM convidada.
- **Ring buffers:** Buffers circulares na memoria compartilhada para comunicacao bidirecional.
- **Interrupt notification via SynIC:** O SynIC e usado para notificar o outro lado quando ha dados disponiveis no ring buffer.

Os dispositivos VMBus mais comuns sao:

| Dispositivo VMBus | Descricao |
|-------------------|-----------|
| `storvsc` | Armazenamento sintetico (disco virtual) |
| `netvsc` | Rede sintetica |
| `vmbus` | Canal de controle VMBus |
| `hvsock` | Sockets Hyper-V |

### Suporte a hardware virtual

O Hyper-V emula ou passa adiante varios tipos de hardware para VMs:

**Hardware emulado (legacy):** Dispositivos que o Hyper-V emula completamente, independente do suporte do convidado. Inclui adaptadores de rede legados, controladores de disco IDE, etc. A emulacao completa e lenta, mas garante compatibilidade com sistemas operacionais sem drivers VMBus.

**Hardware sintetico:** Dispositivos que usam o VMBus e requerem drivers especificos no sistema convidado (chamados de **enlightenments**). Muito mais rapidos que hardware emulado porque evitam a emulacao de I/O porta a porta.

**Passthrough de hardware:** Dispositivos fisicos atribuidos diretamente a uma VM, sem emulacao. O hypervisor usa o **Device Assignment** (baseado em IOMMU) para isolar o acesso do dispositivo a particao correta.

---

## Seguranca baseada em virtualizacao (VBS)

A **Virtualization-Based Security (VBS)** e um conjunto de recursos de seguranca que usa o hypervisor para isolar partes criticas do sistema, tornando-as inacessiveis mesmo para codigo malicioso em modo kernel.

### Virtual Trust Levels (VTLs) e o Virtual Secure Mode (VSM)

Com VBS habilitado, o hypervisor introduce o conceito de **Virtual Trust Levels (VTLs)**:

- **VTL 0:** Onde o kernel Windows normal e os aplicativos rodam. E o ambiente "normal".
- **VTL 1:** Onde o Secure Kernel e os componentes Isolated User Mode (IUM) rodam. Tem maior privilegio que o VTL 0 no contexto da VBS.

A diferenca fundamental entre VTLs e niveis de privilegio (ring 0/ring 3) e:

- **Niveis de ring** controlam quais instrucoes privilegiadas podem ser executadas.
- **VTLs** controlam o **isolamento** - o VTL 1 pode ver e modificar a memoria do VTL 0, mas o VTL 0 nao pode ver ou modificar a memoria do VTL 1.

O conjunto de tecnologias que usam VTLs e chamado de **Virtual Secure Mode (VSM)**.

### Servicos fornecidos pelo VSM

O VSM fornece os seguintes servicos criticos:

- **Secure Memory (SLAT-backed isolation):** Paginas protegidas no VTL 1 nao podem ser mapeadas nem lidas pelo VTL 0, mesmo por codigo kernel privilegiado.
- **Secure Kernel Code Integrity:** O codigo do kernel do VTL 0 e verificado antes de ser executado.
- **Credential Guard:** Credenciais de autenticacao (hashes NTLM, tickets Kerberos) sao armazenadas no processo `LsaIso.exe` no VTL 1, inacessiveis ao VTL 0.

---

## O Secure Kernel

O **Secure Kernel** e um kernel minimalista que roda em VTL 1 (ring 0 do VTL 1). Ele e implementado em `securekernel.exe` e e carregado durante o boot antes do kernel Windows normal.

O Secure Kernel e muito menor e mais simples que o kernel Windows normal:

- Nao suporta device drivers.
- Nao implementa o sistema de arquivos.
- Nao gerencia processos de usuario completos.

Seu papel principal e ser um ponto de confianca para:

- Verificar a integridade do codigo do kernel VTL 0.
- Proteger recursos criticos (como chaves criptograficas do Credential Guard).
- Responder a syscalls seguras do VTL 0 para operacoes criticas.

### Interrupcoes virtuais

O Secure Kernel usa o mecanismo de **Virtual Interrupt Controller (HVIC)** do hypervisor para receber e processar interrupcoes de forma isolada. As interrupcoes no VTL 1 nao podem ser interceptadas ou injetadas por codigo VTL 0.

### Intercepts seguros

Os **secure intercepts** permitem que o Secure Kernel monitore certas operacoes do kernel VTL 0:

- Quando o kernel VTL 0 tenta modificar tabelas de paginas, o Secure Kernel pode verificar se a modificacao e legítima.
- Quando um novo driver e carregado no VTL 0, o Secure Kernel verifica a assinatura digital do driver antes de permitir o carregamento.

Esse e o mecanismo central do **HVCI (Hypervisor-Enforced Code Integrity)**.

### Chamadas de sistema seguras (VSM system calls)

O kernel VTL 0 pode fazer chamadas para o Secure Kernel via uma interface chamada **VSM system calls** (ou secure calls). Essas chamadas transitam entre VTL 0 e VTL 1 usando um mecanismo especial implementado pelo hypervisor.

As secure calls sao usadas para:

- Solicitar que o Secure Kernel proteja uma pagina de memoria (marque-a como pertencente ao VTL 1).
- Derivar chaves criptograficas protegidas pelo hardware.
- Verificar assinaturas digitais.

### Threads e agendamento seguros

Cada thread que existe no VTL 0 tem um **contexto de thread VTL 1** correspondente gerenciado pelo Secure Kernel. Quando ocorre uma transicao de VTL 0 para VTL 1 (via secure call ou intercept), o contexto de registro da thread e salvo/restaurado pelo hypervisor.

O Secure Kernel tem seu proprio conjunto de threads do sistema que rodam em VTL 1. Esses threads sao completamente isolados das threads do kernel VTL 0.

### Hypervisor Enforced Code Integrity (HVCI)

O **HVCI** e uma das funcionalidades mais poderosas do VBS. Com o HVCI habilitado:

1. O kernel VTL 0 nao pode alocar memoria executavel e gravavel ao mesmo tempo (`PAGE_EXECUTE_READWRITE` e proibido).
2. Antes de marcar qualquer pagina como executavel, o Secure Kernel verifica se a pagina contem codigo assinado por uma autoridade confiavel.
3. Toda tecnica de injcao de codigo em modo kernel que depende de alocar nova memoria executavel e bloqueada.

Isso efetivamente impede a maioria dos rootkits tradicionais que funcionavam injetando codigo no kernel.

### Virtualizacao de runtime UEFI

Para garantir que o UEFI (firmware do sistema) nao comprometa a integridade do sistema apos o boot, o Hyper-V virtualiza o acesso da UEFI aos recursos do sistema. O codigo da UEFI continua rodando, mas em um ambiente controlado onde seus acessos a memoria e registradores podem ser monitorados e restritos.

### Inicializacao do VSM

A sequencia de inicializacao do VSM durante o boot e:

1. O hypervisor e carregado e habilitado.
2. O Secure Kernel (`securekernel.exe`) e carregado pelo boot loader no contexto VTL 1.
3. O Secure Kernel inicializa sua propria IDT, gerenciador de memoria e estruturas de sincronizacao.
4. O Secure Kernel configura os intercepts para monitorar o kernel VTL 0.
5. O kernel Windows normal (`ntoskrnl.exe`) e carregado e executado em VTL 0.
6. Durante a inicializacao do kernel, o VBS e completamente configurado.

---

## Isolated User Mode (IUM)

O **Isolated User Mode (IUM)** e o ambiente de modo usuario que roda em VTL 1. E o equivalente do espaco de enderecamento de usuario do Windows normal, mas isolado por VTL.

Os processos que rodam no IUM sao chamados de **Trustlets** (descritos no Capitulo 3 do Part 1 e expandidos aqui). Trustlets sao binarios PE normais, mas com restricoes especiais:

- Podem importar apenas de um conjunto limitado de DLLs do sistema.
- Podem importar da DLL especifica do IUM chamada `Iumbase.dll`.
- Sao assinados com um certificado especial que contem o IUM EKU.

### Criacao de Trustlets

Quando um Trustlet e criado:

1. O kernel VTL 0 solicita ao Secure Kernel a criacao de um processo Trustlet.
2. O Secure Kernel verifica a assinatura do binario.
3. O Secure Kernel cria o processo no espaco de enderecamento do IUM (VTL 1).
4. O processo recebe seu proprio espaco de enderecamento privado em VTL 1.

### Dispositivos seguros

O IUM suporta o conceito de **dispositivos seguros** - dispositivos de hardware que podem ser acessados exclusivamente pelo IUM, sem acesso pelo kernel VTL 0. Isso permite, por exemplo:

- Acesso seguro a chips TPM.
- Acesso a enclaves de hardware (como Intel SGX ou AMD SEV).
- Gerenciamento de chaves criptograficas protegidas por hardware.

### Enclaves baseados em VBS

O Windows 10 Creators Update introduziu suporte a **VBS-based enclaves** - uma implementacao de enclaves de memoria segura usando VBS em vez de hardware especifico (como Intel SGX). Os enclaves VBS:

- Sao processos Trustlet especiais no IUM.
- Protegem o codigo e dados do enclave contra acesso pelo kernel normal.
- Fornecem attestation (comprovacao de autenticidade) via o Secure Kernel.

A Windows API para enclaves inclui:

```cpp
// Cria um enclave
LPVOID VirtualAllocExEnclaveAddr = CreateEnclave(
    hProcess,     // processo onde criar
    NULL,         // endereco base sugerido
    dwSize,       // tamanho do enclave
    dwInitialCommitment, // comprometimento inicial
    ENCLAVE_TYPE_VBS,    // tipo VBS
    lpEnclaveInformation, // informacoes do enclave
    dwInfoLength,
    lpEnclaveError
);
```

> **Rode no Visual Studio**
> `IsEnclaveTypeSupported` verifica se o tipo de enclave VBS e suportado pelo sistema.
>
> ```cpp
> #include <windows.h>
> #include <stdio.h>
>
> int main() {
>     // Verifica suporte a enclave VBS
>     BOOL vbs = IsEnclaveTypeSupported(ENCLAVE_TYPE_VBS);
>     printf("Enclaves VBS suportados: %s\n", vbs ? "sim" : "nao");
>
>     // Verifica suporte a enclave SGX
>     BOOL sgx = IsEnclaveTypeSupported(ENCLAVE_TYPE_SGX);
>     printf("Enclaves SGX suportados: %s\n", sgx ? "sim" : "nao");
>
>     return 0;
> }
> ```

### Attestation em tempo de execucao do System Guard

O **System Guard Runtime Attestation** e um mecanismo pelo qual o Windows pode provar criptograficamente que o sistema esta em um estado de seguranca conhecido. Ele usa o TPM (Trusted Platform Module) e o Secure Kernel para gerar medidas criptograficas do estado do sistema.

O processo de attestation funciona assim:

1. O Secure Kernel coleta medidas do estado do kernel VTL 0 (paginas de codigo, estruturas criticas, etc.).
2. Essas medidas sao assinadas usando uma chave protegida pelo TPM.
3. A attestation resultante pode ser verificada por um servidor remoto para confirmar que o sistema nao foi comprometido.

---

## Conclusao

Este capitulo explorou as tecnologias de virtualizacao que o Windows usa tanto para criar maquinas virtuais completas quanto para fortalecer a seguranca do proprio sistema.

O hypervisor do Windows (Hyper-V) fornece a base para executar multiplas VMs no mesmo hardware fisico com alto desempenho, graças ao VMBus e ao suporte de hardware sintetico. O VBS e o Secure Kernel vao alem, usando a virtualizacao para criar uma barreira de seguranca que protege componentes criticos mesmo contra ataques que comprometem o kernel principal.

As tecnologias como HVCI, Credential Guard e System Guard Attestation representam o estado da arte em seguranca de SO, usando o hypervisor como uma Trusted Computing Base (TCB) menor e mais facil de auditar do que o kernel Windows completo.

No proximo capitulo, examinaremos os mecanismos de gerenciamento, diagnostico e rastreamento do Windows - incluindo o registro, servicos, WMI e ETW.
