# Capítulo 1 — Hipervisor e Virtualization Based Security (VBS)

> Livro didático baseado em *Windows Internals, 7th Edition*.
> Atualizado para Windows 11 com exemplos práticos em C++ moderno.
> Repositório: [ring0-study](https://github.com/cnthigu/ring0-study)

> Navegação: [Anterior](capitulo-01-05-memoria-virtual.md) | [Índice](README.md) | [Próximo](capitulo-01-07-objetos-handles-seguranca.md)

---

## Por que precisamos de uma camada abaixo do kernel?

Durante décadas, o kernel do Windows em Ring 0 era o deus absoluto do sistema. Qualquer código rodando em Ring 0 — um driver legítimo, um rootkit, ou código exploitado — tinha controle total. Um driver mal-intencionado podia modificar tabelas de páginas, desabilitar verificações de segurança, ou ler a memória de qualquer processo, incluindo o processo do antivírus que tentava detectá-lo.

Este era o problema fundamental: **Ring 0 não tem supervisor**. Qualquer atacante que conseguia executar código no kernel tinha o mesmo poder que o próprio Windows.

A solução foi introduzir uma camada ainda mais privilegiada: o **hipervisor**.

---

## Hyper-V: o hipervisor da Microsoft

O **Hyper-V** é um hipervisor Tipo 1 (bare-metal) — roda diretamente no hardware, abaixo de qualquer sistema operacional. Quando habilitado, **o próprio Windows passa a rodar dentro de uma VM gerenciada pelo Hyper-V**.

```
┌────────────────────────────────────────────────────────────┐
│                     HARDWARE (CPU, RAM, NIC...)            │
├────────────────────────────────────────────────────────────┤
│                     HYPER-V (Ring -1 / VMX root)           │
│  • Controla quem pode acessar quais recursos de hardware   │
│  • Define e impõe políticas de memória para todas as VMs   │
│  • Implementa Virtual Trust Levels (VTLs)                  │
├─────────────────────────────────┬──────────────────────────┤
│     VTL 0 (Normal World)        │   VTL 1 (Secure World)   │
│  ┌─────────────────────────┐   │  ┌────────────────────┐  │
│  │  Windows 11 (Ring 0)    │   │  │  Secure Kernel     │  │
│  │  ntoskrnl.exe, drivers  │   │  │  (skci.dll,        │  │
│  │                         │   │  │  lsaiso.exe)       │  │
│  ├─────────────────────────┤   │  └────────────────────┘  │
│  │  Ring 3 — Aplicações    │   │                           │
│  └─────────────────────────┘   │                           │
└─────────────────────────────────┴──────────────────────────┘
```

Mesmo o kernel do Windows em Ring 0 agora tem um supervisor: o Hyper-V. O kernel não pode mais modificar tabelas de página arbitrariamente sem que o hipervisor aprove.

> **No Windows 11, o Hyper-V está sempre ativo** quando o hardware suporta (Intel VT-x ou AMD-V, e IOMMU). Não é necessário ter Hyper-V "habilitado" para usar VMs — isso é uma configuração separada que habilita as ferramentas de gerenciamento. A tecnologia de virtualização subjacente sempre roda.

---

## Virtual Trust Levels (VTLs)

O Hyper-V introduz os **VTLs (Virtual Trust Levels)** — uma hierarquia de confiança independente dos rings do processador:

- **VTL 0** (Normal World): onde o Windows convencional roda. Ring 0 aqui é o kernel normal. Ring 3 são as aplicações.
- **VTL 1** (Secure World): onde o **Secure Kernel** roda. Código aqui tem mais privilégio que qualquer coisa no VTL 0 — incluindo o kernel do Windows.

A regra fundamental: **VTL 1 pode ver e controlar VTL 0, mas VTL 0 nunca pode ver o interior de VTL 1**. Um rootkit em Ring 0 do VTL 0 não consegue ler a memória de VTL 1, modificar suas tabelas de página, ou interromper suas operações.

---

## Virtualization Based Security (VBS)

O **VBS** é o conjunto de tecnologias de segurança que rodam em VTL 1, aproveitando o isolamento fornecido pelo Hyper-V. No Windows 11, várias features do VBS estão habilitadas por padrão:

### 1. HVCI — Hypervisor-Protected Code Integrity

Antigamente, um atacante podia desabilitar a verificação de integridade de drivers modificando variáveis no kernel. Com HVCI:

- Toda página de memória marcada como executável no kernel é **aprovada pelo hipervisor**
- O hipervisor impõe políticas de memória: uma página não pode ser simultaneamente writeable E executável (W⊕X)
- Mesmo um driver com credenciais válidas não pode modificar o código de outros drivers em memória
- O Hyper-V mantém as "Second Level Address Translation" (SLAT/EPT) tables que o kernel não pode modificar

Resultado: injeção de código no kernel (a técnica favorita de rootkits) se torna praticamente impossível.

### 2. Credential Guard

**Credential Guard** move o processo `lsass.exe` (que armazena hashes de senhas e tickets Kerberos) para um processo isolado em VTL 1 chamado `lsaiso.exe` (LSA Isolated):

```
VTL 0:
  lsass.exe — proxy, recebe requisições
      |
      | (chamada entre VTLs via Secure Call)
      ▼
VTL 1:
  lsaiso.exe — processa credenciais, retorna apenas o necessário
               Memória completamente inacessível de VTL 0
```

Ferramentas como Mimikatz funcionavam porque podiam ler a memória do `lsass.exe` diretamente de um processo com SeDebugPrivilege. Com Credential Guard, não há nada para ler em `lsass.exe` — as credenciais reais estão em VTL 1.

### 3. HyperGuard (KPP Aprimorado)

O **Kernel Patch Protection (PatchGuard)** clássico detectava modificações na SSDT e outras estruturas críticas periodicamente — havia janelas de tempo onde um rootkit podia agir. O HyperGuard usa o hipervisor para verificação contínua e instantânea: qualquer modificação não autorizada é detectada imediatamente na próxima transição de VTL.

### 4. Application Guard (WDAG)

O **Windows Defender Application Guard** vai além: ele abre o Edge (e documentos Office suspeitos) em uma VM Hyper-V completamente separada. O arquivo pode ser malicioso ao ponto de exploitar o browser — mas o malware fica contido dentro de uma VM efêmera que é descartada ao fechar a sessão.

---

## Firmware: UEFI e Secure Boot

A cadeia de confiança começa antes do sistema operacional iniciar:

```
Energia ligada
      │
      ▼
UEFI Firmware (armazenado na SPI Flash da placa-mãe)
      │ verifica assinatura digital
      ▼
Boot Manager (bootmgr.efi) — assinado pela Microsoft
      │ verifica assinatura
      ▼
Windows Boot Loader (winload.efi) — assinado
      │ verifica assinatura
      ▼
Windows Kernel (ntoskrnl.exe) — assinado
      │ verifica assinatura
      ▼
Drivers (*.sys) — devem ter assinatura EV + Microsoft WHQL
```

Se **qualquer elemento dessa cadeia** for modificado sem assinatura válida, o **Secure Boot** impede a inicialização. Bootkits — malware que infecta o setor de boot ou o bootloader — se tornaram praticamente extintos em hardware moderno com Secure Boot.

O Windows 11 exige:
- **UEFI** (não BIOS legado)
- **Secure Boot habilitado** (não pode ser desabilitado sem acesso físico ao firmware)
- **TPM 2.0** — para armazenar chaves do Secure Boot e servir como âncora de confiança de hardware

### TPM 2.0 — Trusted Platform Module

O TPM é um chip dedicado (ou implementação em firmware) que:

- **Armazena chaves criptográficas** que nunca saem do chip em texto claro
- **Faz medições (PCRs)** de cada estágio do boot — qualquer alteração nos arquivos de boot é detectada
- **Suporta BitLocker**: a chave de criptografia do disco só é liberada se o TPM verificar que o sistema bootou sem alterações
- **Suporta Windows Hello**: credenciais biométricas e PIN são protegidos pelo TPM

Em WinDbg, você pode ver o estado do VBS:
```
kd> !vbs
Virtual Based Security (VBS)
  VBS: Enabled
  Hypervisor enforced Code Integrity: Enabled
  Credential Guard: Enabled
```

---

## Sessions e Terminal Services

Um conceito relacionado mas frequentemente confundido com segurança: **sessões**.

O Windows suporta múltiplos usuários logados simultaneamente (Terminal Services / Remote Desktop). Cada sessão tem:
- Um ID de sessão único (Session 0, 1, 2, ...)
- Espaço de objetos do namespace separado (para evitar conflitos entre sessões)
- Desktop(s) separado(s)

**Session 0** é especial: é onde os serviços do Windows rodam. Desde o Vista, serviços e aplicações do usuário rodam em sessões diferentes — isso previne o ataque "Shatter attack" onde código malicioso em user mode mandava mensagens de janela para um serviço privilegiado.

```cpp
// Descobrindo a sessão atual do processo
DWORD sessionId;
ProcessIdToSessionId(GetCurrentProcessId(), &sessionId);
printf("Session: %u\n", sessionId); // provavelmente 1 ou 2 para um usuário normal
```

---

## O que isso significa na prática para developers

Se você escreve drivers, precisa:

1. **Certificado EV + WHQL**: em Windows 11 (64-bit), drivers sem assinatura da Microsoft não carregam. O driver precisa passar pelo processo de certificação WHQL ou ser assinado pelo Cross-Signing Program.

2. **Driver Verifier**: ferramenta que habilita verificações extras em runtime para encontrar bugs em drivers (pool overruns, invalid IRQLs, etc.).

3. **HVCI compatibility**: seu driver não pode alocar memória writeable+executável simultaneamente. Precisa ser compatível com HVCI — pode ser verificado com a ferramenta `HVCIcompat.exe`.

Se você escreve ferramentas de segurança:

1. Qualquer ferramenta que precise ler memória de outros processos precisa de `SeDebugPrivilege` — e mesmo assim não pode ler VTL 1.
2. Ferramentas de dump de credenciais (legítimas ou não) são fundamentalmente bloqueadas pelo Credential Guard.
3. Análise de malware em máquina real com Windows 11 HVCI tem muito menos superfície de ataque para o malware exploitar.

---

*Próximo: [Objetos, Handles e Segurança](capitulo-01-07-objetos-handles-seguranca.md)*
