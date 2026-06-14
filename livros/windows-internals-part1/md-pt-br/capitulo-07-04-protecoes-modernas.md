# Capítulo 7 — Proteções de Segurança Modernas no Windows 11

> Livro didático baseado em *Windows Internals, 7th Edition*.
> Atualizado para Windows 11 com exemplos práticos em C++ moderno.

> Navegação: [Anterior](capitulo-07-03-logon-autenticacao.md) | [Índice Cap.7](capitulo-07.md)

---

## A estratégia de defesa do Windows 11

O Windows 11 adota uma estratégia de **Defense in Depth** (defesa em profundidade), com múltiplas camadas de proteção:

```
Camadas de segurança do Windows 11 (de baixo para cima):

  Hardware:
  ├── UEFI Secure Boot         → garante que só código assinado inicializa
  ├── TPM 2.0                  → chaves criptográficas, medições de boot
  └── CPU com VT-x/AMD-V       → suporte a virtualização para VBS

  Firmware/Hypervisor:
  ├── HVCI / Memory Integrity  → kernel apenas com código assinado e verificado
  ├── Credential Guard         → credenciais isoladas em VTL1
  └── VBS (Virtualization Based Security)

  Kernel:
  ├── KPP / PatchGuard         → detecta modificações não autorizadas no kernel
  ├── DSE (Driver Signature Enforcement) → apenas drivers assinados
  ├── KASLR / KPTI             → randomização e isolamento do kernel
  └── CFG (Control Flow Guard) → proteção de fluxo de controle

  User Mode:
  ├── ASLR / High Entropy      → randomização de endereços
  ├── DEP                      → páginas de dados não executáveis
  ├── SafeSEH / SEHOP          → proteção de exception handlers
  ├── ACG (Arbitrary Code Guard) → Edge/Chromium não pode executar JIT em páginas injetadas
  ├── CIG (Code Integrity Guard)  → processo só carrega código assinado pela MS
  └── PPL / Protected Processes   → processos críticos com proteção especial
```

---

## PatchGuard / Kernel Patch Protection (KPP)

O **PatchGuard** é um mecanismo que detecta modificações não autorizadas em estruturas críticas do kernel:

```
O que PatchGuard monitora:
  ├── IDT (Interrupt Descriptor Table)
  ├── GDT (Global Descriptor Table)
  ├── MSRs críticos (LSTAR, SYSENTER)
  ├── SSDT (System Service Descriptor Table)
  ├── Código do kernel (ntoskrnl.exe, hal.dll, CI.dll)
  ├── DR7 (Debug Register — global breakpoints)
  └── Certas estruturas de dados críticas

Quando modificação detectada:
  → BSOD com código 0x109 CRITICAL_STRUCTURE_CORRUPTION

Método de detecção:
  → Timer DPC rodando em intervalos aleatórios (não previsíveis)
  → Checksum criptográfico das estruturas monitoradas
  → Salvo em forma ofuscada para dificultar bypass
```

PatchGuard explica por que **hooks de SSDT** — uma técnica clássica de antivírus — foram abandonados. Antivírus modernos usam o mecanismo oficial de callbacks do kernel (`PsSetCreateProcessNotifyRoutine`, `ObRegisterCallbacks`, etc.).

---

## Driver Signature Enforcement (DSE)

No Windows 11 x64, **todos os drivers de kernel devem ser assinados digitalmente**:

```
Níveis de assinatura de driver:

1. Assinatura de desenvolvedor (para teste):
   → Habilitar modo de teste: bcdedit /set testsigning on
   → Driver assinado com certificado auto-gerado
   → NÃO funciona em hardware real com Secure Boot + HVCI

2. Assinatura WHQL (Windows Hardware Quality Labs):
   → Submetido e testado pela Microsoft
   → Recebia certificado da MS via Hardware Dev Center
   → Necessário para distribuição no Windows Update

3. Extended Validation (EV) Code Signing:
   → Certificado de alta confiança de CA pública
   → Necessário para assinar drivers para Windows 10/11
   → Combinado com attestation signing na Microsoft

4. Attestation Signing (Windows 10+):
   → Driver submetido para o Microsoft Hardware Dev Center
   → Microsoft verifica e assina sem processo WHQL completo
   → Para drivers de períodos de desenvolvimento
```

```
Verificar se DSE está ativo:
  bcdedit /enum {current} | findstr integritycheck
  
  Desabilitar (admin + Secure Boot desligado):
  bcdedit /set nointegritychecks on   (funciona sem HVCI)
```

---

## HVCI: Hypervisor-Protected Code Integrity

O **HVCI (Hypervisor-Protected Code Integrity)** é a proteção mais forte contra carregamento de código malicioso no kernel:

```
HVCI (Memory Integrity) em ação:

Sem HVCI:
  Kernel em VTL0 controla suas próprias page tables
  → Código malicioso em VTL0 pode tornar página de dados executável
  → Driver não assinado pode ser carregado se DSE for bypassado

Com HVCI:
  Hypervisor (VTL1) controla as page tables do kernel (VTL0)
  → Kernel NÃO pode tornar páginas executáveis sem aprovação do hypervisor
  → Para carregar código no kernel: hypervisor verifica a assinatura ANTES de mapear
  → Mesmo com kernel comprometido: não há como executar código não assinado

Consequência:
  → Qualquer vulnerabilidade no kernel que permita escrever em memória
    ainda não consegue executar código sem assinar o código e obter
    aprovação do hypervisor
  → Elimina toda uma classe de exploits de kernel
```

---

## AppContainers e Low Integrity Processes

Os **AppContainers** são o modelo de sandbox do Windows para aplicativos UWP e Edge:

```
AppContainer Sandbox:
  ├── Token de baixa integridade (Low Integrity Level)
  ├── Token com Capabilities (lista de permissões declaradas)
  ├── Namespace isolado de objetos kernel (pipes, mutexes, sections privados)
  ├── Acesso a filesystem restrito:
  │     → Apenas App Local Data folder
  │     → Apenas pastas explicitamente permitidas
  │     → Broker process para acesso a outras localizações
  ├── Acesso a rede restrito por Capabilities:
  │     → internetClient: pode fazer conexões outbound
  │     → internetClientServer: pode também receber
  │     → privateNetworkClientServer: rede local
  └── Capabilities declaradas no manifest:
        → microphone, camera, location, contacts, etc.
```

```
Comparação de isolamento:
  Processo Normal   → MIC Medium, acesso ao filesystem do usuário
  AppContainer      → MIC Low + Capabilities, filesystem isolado
  LowBox Token      → variante programática do AppContainer
  Server Silo       → container completo com namespace de kernel separado
```

---

## Control Flow Guard (CFG) e CET

**CFG** impede que um exploit que corrompeu um ponteiro de função salte para código arbitrário:

```
Sem CFG:
  Buffer overflow → sobrescreve ponteiro de função → salta para shellcode
  ✓ Exploit funciona

Com CFG:
  Toda chamada indireta (call reg, call [mem]) → verificação automática
  → CPU/software checa: este endereço é um ponto de entrada válido de função?
  → Se não: exceção → processo termina
  → O shellcode não está marcado como ponto de entrada válido

CET (Control-flow Enforcement Technology) - hardware:
  → Shadow Stack: CPU mantém pilha separada (não acessível a código normal)
  → Cada CALL empurra endereço de retorno na shadow stack
  → RET verifica se o endereço bate com a shadow stack
  → ROP (Return Oriented Programming) fica efetivamente inviável
  → Disponível em Intel Tiger Lake+ e AMD Zen 3+
  → Windows 11 habilita CET automaticamente quando hardware suporta
```

---

## Windows Defender: AntiMalware Scan Interface (AMSI)

O **AMSI** permite que o Windows Defender (e outros antivírus) escaneiem conteúdo em tempo de execução — incluindo scripts PowerShell, VBScript e macros Office:

```
Fluxo AMSI:

  PowerShell.exe
    │ script.ps1 com obfuscação
    ▼
  Antes de executar o script:
  powershell.dll chama AmsiScanBuffer(script)
    ▼
  AMSI Provider (Windows Defender):
  → desofusca → escaneia → verifica contra assinaturas
    ├── AMSI_RESULT_CLEAN: executar normalmente
    └── AMSI_RESULT_DETECTED: bloquear, gerar alerta

Integração:
  PowerShell, Windows Script Host, Office macros, .NET
  Todos chamam AMSI antes de executar conteúdo dinâmico

Bypass tentativas (detectadas e corrigidas):
  → Patch da tabela de AmsiScanBuffer em memória para retornar CLEAN
  → Reflexive loading via AppDomain para evitar scan
  → (Cada bypass foi endereçado em updates subsequentes)
```

---

## Smart App Control e Reputation

O Windows 11 introduz o **Smart App Control** — baseado em reputação na nuvem:

```
Smart App Control:
  → Todo executável novo é verificado contra cloud da Microsoft
  → Se tem boa reputação (usado por muitos sem problemas): permitido
  → Se tem reputação desconhecida: bloqueado ou aviso
  → Se é malicioso (detecção cloud): bloqueado
  
  Integração com Windows Defender Application Control (WDAC)
  Impede a classe de "first use" do malware:
  → Mesmo que o malware seja novo e desconhecido localmente
  → Cloud consulta telemetria de bilhões de PCs Windows
```

---

## ETW: Event Tracing for Windows (segurança)

O **ETW** é a base de telemetria de segurança do Windows — usado por EDR/XDR:

```
Provedores ETW de segurança relevantes:
  Microsoft-Windows-Kernel-Process   → criação/término de processos e threads
  Microsoft-Windows-Kernel-File      → operações de arquivo
  Microsoft-Windows-Kernel-Network   → conexões de rede
  Microsoft-Windows-Kernel-Registry  → acesso ao registro
  Microsoft-Windows-Security-Auditing → eventos do security event log
  Microsoft-Antimalware-Engine       → eventos do Windows Defender
  Microsoft-Windows-DotNETRuntime    → CLR + JIT (para detectar .NET malware)
```

```cpp
// Consumir eventos ETW programaticamente (sessão em tempo real):
EVENT_TRACE_PROPERTIES* props = ...; // configurar sessão ETW
StartTrace(&hTrace, L"MinhaSessionETW", props);

// Registrar handler:
TRACE_LOGFILE_HEADER header = {};
header.LoggerName = L"MinhaSessionETW";
header.ProcessTraceMode = PROCESS_TRACE_MODE_REAL_TIME | PROCESS_TRACE_MODE_EVENT_RECORD;
header.EventRecordCallback = MeuHandlerDeEvento;

TRACEHANDLE hConsumer = OpenTrace(&header);
ProcessTrace(&hConsumer, 1, nullptr, nullptr);
```

---

*Fim do Capítulo 7.*
