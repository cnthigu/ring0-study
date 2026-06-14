# Capítulo 2 — VBS: A Arquitetura de Segurança Baseada em Virtualização

> Livro didático baseado em *Windows Internals, 7th Edition*.
> Atualizado para Windows 11 com exemplos práticos em C++ moderno.
> Repositório: [ring0-study](https://github.com/cnthigu/ring0-study)

> Navegação: [Anterior](capitulo-02-03-portabilidade-smp.md) | [Índice Cap.2](capitulo-02.md) | [Próximo](capitulo-02-05-subsistemas-ntdll.md)

---

## O problema que o Ring 0 sozinho não resolve

No Capítulo 1 introduzimos o VBS superficialmente. Aqui vamos entender a arquitetura em profundidade, porque ela é central na forma como o Windows 11 funciona.

O problema fundamental é este: Ring 0 tem poder absoluto. Qualquer código que chegue ao kernel — seja um driver legítimo, um rootkit, ou código injetado via exploit — tem acesso irrestrito a toda a memória, pode modificar tabelas de página, pode desabilitar verificações de segurança.

Isso significa que comprometer Ring 0 é comprometer tudo. Não há supervisor, não há árbitro. Um rootkit em Ring 0 pode:
- Esconder processos do Task Manager modificando listas do kernel
- Interceptar senhas antes de chegarem ao lsass.exe lendo memória
- Desabilitar verificações de assinatura de drivers para carregar mais malware
- Burlar o antivírus modificando os callbacks que o AV registrou

A solução: **adicionar uma camada ainda mais privilegiada que supervisiona o kernel**.

---

## A hierarquia de privilégios com VBS

```
┌──────────────────────────────────────────────────────────┐
│                   HARDWARE                               │
│             Intel VT-x / AMD-V (VMX)                     │
├──────────────────────────────────────────────────────────┤
│                   HYPER-V                                │
│          (VMX root mode — mais privilegiado)             │
│                                                          │
│  Controla quais páginas são acessíveis a quem (SLAT)     │
│  Gerencia VTLs e transições entre eles                   │
├────────────────────────┬─────────────────────────────────┤
│   VTL 0 (Normal World) │  VTL 1 (Secure World)           │
│                        │                                  │
│  Ring 0: ntoskrnl.exe  │  Ring 0: securekernel.exe        │
│  Ring 3: apps normais  │  Ring 3: Trustlets               │
│                        │         (lsaiso.exe, etc.)       │
└────────────────────────┴─────────────────────────────────┘
```

A relação de privilégio:
- **Hyper-V** pode ver e controlar tudo
- **VTL 1 Ring 0** (Secure Kernel) pode ver e controlar VTL 0, mas não pode controlar o Hyper-V
- **VTL 0 Ring 0** (kernel Windows normal) não pode ver ou modificar VTL 1
- **VTL 0 Ring 3** (apps) não pode ver o kernel, como sempre

O ponto crítico: **o Secure Kernel em VTL 1 supervisiona o kernel Windows em VTL 0**. Mesmo que um atacante comprometa completamente o kernel do Windows (Ring 0, VTL 0), o Secure Kernel continua protegido e pode limitar o que o kernel comprometido consegue fazer.

---

## SLAT: Segunda Camada de Tradução de Endereços

O Hyper-V usa **SLAT (Second Level Address Translation)** — chamado de EPT em Intel e NPT em AMD — para controlar quais páginas de memória física são acessíveis de VTL 0.

Normalmente, uma tabela de páginas de VTL 0 traduz:
```
Endereço Virtual → Endereço Físico
```

Com SLAT, há uma segunda camada:
```
Endereço Virtual (VTL 0)
  → Endereço Físico "Guest" (que VTL 0 acredita ser o físico)
     → Endereço Físico Real (controlado pelo Hyper-V via SLAT)
```

O Secure Kernel usa SLAT para:

1. **Marcar certas páginas como não-acessíveis de VTL 0**: a memória onde lsaiso.exe armazena hashes de senha é inacessível ao kernel do VTL 0 — mesmo que o kernel esteja comprometido e tente ler aquela memória diretamente.

2. **HVCI (Hypervisor-Protected Code Integrity)**: o Secure Kernel usa SLAT para garantir que nenhuma página no kernel seja simultaneamente writeable e executável. Isso impossibilita injeção de código no kernel, mesmo com permissão de driver.

3. **Proteger o próprio Hyper-V e Secure Kernel**: páginas de memória usadas pelo Hyper-V e VTL 1 são simplesmente removidas do mapa de memória de VTL 0 via SLAT.

---

## Isolated User Mode (IUM)

VTL 1 tem dois modos:
- **Ring 0** (Secure Kernel): `securekernel.exe`
- **Ring 3** (IUM — Isolated User Mode): onde Trustlets rodam

IUM é um ambiente user mode restrito. Ele usa versões especiais das bibliotecas padrão:
- `iumdll.dll` (equivalente ao `ntdll.dll` de VTL 0)
- `iumbase.dll` (equivalente ao `kernelbase.dll`)

As syscalls de IUM têm o bit mais alto setado (`0x8000xxxx`), distinguindo-as das syscalls normais de VTL 0. O Secure Kernel valida essas chamadas independentemente.

---

## Trustlets: aplicações autorizadas em VTL 1

Não é qualquer aplicação que pode rodar em VTL 1. Apenas **Trustlets** — binários especialmente assinados pela Microsoft.

Propriedades de Trustlets:
- Têm um ID único hardcoded no Secure Kernel
- Devem ser assinados com uma chave especial
- Qualquer modificação invalida a assinatura, impedindo execução
- O Secure Kernel rejeita qualquer Trustlet não reconhecido

Trustlets atuais incluem:
- `lsaiso.exe` — o LSA Isolado (Credential Guard)
- `vmsup.exe` — suporte a VMs protegidas (em Server)
- Componentes internos de VBS

Você não pode criar um Trustlet sem acesso à chave de assinatura da Microsoft — esse é o ponto. A superfície de ataque de VTL 1 é propositalmente mínima.

---

## I/O MMU: bloqueando DMA malicioso

Há um vetor de ataque que SLAT sozinho não resolve: DMA (Direct Memory Access). Dispositivos de hardware — especificamente periféricos PCIe — podem fazer leituras e escritas diretamente na RAM física, **bypassando as tabelas de página do processador**.

Um dispositivo malicioso (ou um driver que controla um dispositivo malicioso) poderia fazer DMA para as páginas físicas do Secure Kernel ou do Hyper-V, mesmo que essas páginas sejam invisíveis via SLAT.

A solução é o **I/O MMU** (também chamado IOMMU — Input-Output MMU): um chip separado que intercepta operações DMA e aplica uma segunda camada de tradução. O Hyper-V programa o I/O MMU para que dispositivos só possam acessar as regiões físicas de memória que têm permissão.

```
Driver (VTL 0)
  → DMA request para dispositivo PCIe
    → Dispositivo tenta acessar RAM física
      → I/O MMU intercepta
        → Verifica permissões programadas pelo Hyper-V
          → Permite apenas acesso às páginas autorizadas
```

Sem I/O MMU (IOMMU), não é possível implementar VBS completo de forma segura. Por isso Windows 11 requer IOMMU no hardware.

---

## O ciclo de boot com VBS

O boot é onde a cadeia de confiança é estabelecida:

```
1. UEFI Firmware verifica assinatura do bootloader
   ↓
2. bootmgr.efi carrega → verifica assinaturas de todos os componentes
   ↓
3. Hyper-V é o PRIMEIRO componente a ser carregado
   → Programa o SLAT com permissões iniciais
   → Programa o IOMMU
   ↓
4. Hyper-V coloca o sistema em VTL 1
5. bootloader executa novamente em VTL 1
6. Secure Kernel (securekernel.exe) é carregado em VTL 1
   → Configura políticas de SLAT mais específicas
   → Carrega Trustlets se necessário
   ↓
7. VTL é dropado para VTL 0
8. Kernel Windows normal (ntoskrnl.exe) inicia em VTL 0
   → Agora está "enjaulado" pelo SLAT/IOMMU configurado pelo Secure Kernel
   ↓
9. Sistema inicializa normalmente
```

Qualquer tentativa de modificar o Secure Kernel ou o Hyper-V após esse processo requer permissão do próprio Hyper-V — que o kernel de VTL 0 não tem.

---

## Verificando VBS no seu sistema

```powershell
# PowerShell: status do VBS e HVCI
Get-CimInstance -ClassName Win32_DeviceGuard -Namespace root\Microsoft\Windows\DeviceGuard |
    Select-Object -Property VirtualizationBasedSecurityStatus, HypervisorEnforcedCodeIntegrityStatus
```

No WinDbg com kernel debug:
```
kd> !vbs
kd> !securekernel   (se VTL 1 estiver ativo)
```

Ou via `msinfo32.exe` → System Summary → procure "Virtualization-based security".

---

## O que isso significa para desenvolvimento

### Para drivers

- Seu driver **não pode** alocar memória executável com `MmAllocatePool` e escrever nela depois com HVCI ativo. A sequência "aloca memória, escreve código, executa" é bloqueada.
- Drivers precisam ser HVCI-compatible: código e dados em regiões separadas, sem self-modifying code.
- Verifique com `HVCIcompat.exe` do WDK antes de submeter.

### Para ferramentas de segurança

- Leitura de memória de `lsass.exe` (técnica do Mimikatz) é bloqueada pelo Credential Guard — os dados sensíveis estão em VTL 1.
- Hooks de kernel (alterando tabelas de ponteiros de função) são detectados pelo HyperGuard.
- Análise de malware com Windows 11 + VBS tem uma superfície de ataque muito menor.

### Para pesquisa de segurança

- Vulnerabilidades de kernel que antes seriam "game over" agora precisam de um segundo passo: escapar do "VTL 0 jail" — muito mais difícil.
- A fronteira de pesquisa se move para: vulnerabilidades no Secure Kernel, no Hyper-V, e no processo de boot.

---

*Próximo: [Subsistemas de Ambiente e Ntdll.dll](capitulo-02-05-subsistemas-ntdll.md)*
