# Capítulo 1 — Ferramentas Essenciais

> Livro didático baseado em *Windows Internals, 7th Edition*.
> Atualizado para Windows 11 com exemplos práticos em C++ moderno.
> Repositório: [ring0-study](https://github.com/cnthigu/ring0-study)

> Navegação: [Anterior](capitulo-01-07-objetos-handles-seguranca.md) | [Índice](README.md)

---

## Por que ferramentas importam

Entender teoria é necessário. Mas o que transforma teoria em habilidade real é a capacidade de **observar o sistema funcionando**. Este capítulo apresenta as ferramentas que você vai usar ao longo do livro para inspecionar processos, memória, handles, drivers, e o próprio kernel.

Todas as ferramentas mencionadas aqui são gratuitas. A maioria é da suite Sysinternals da Microsoft.

---

## Process Explorer

O substituto definitivo do Task Manager. Onde o Task Manager mostra quatro colunas, o Process Explorer mostra centenas — e você entende o que cada uma significa depois de ler este livro.

**Instalação**: baixe do pacote Sysinternals ou da Microsoft Store (Sysinternals Suite).

**Configuração inicial obrigatória**: configure o symbol server para ver nomes de funções nas stack traces:
```
Options → Configure Symbols
Symbol path: srv*C:\Symbols*https://msdl.microsoft.com/download/symbols
```

### O que o Process Explorer mostra que o Task Manager não mostra

| Feature | Task Manager | Process Explorer |
|---------|-------------|-----------------|
| Hierarquia de processos (árvore) | Não | Sim |
| DLLs carregadas por processo | Não | Sim |
| Handles abertos (arquivos, mutexes, etc.) | Não | Sim |
| Conexões de rede por processo | Não | Sim (aba TCP/IP) |
| Token de segurança / integridade | Não | Sim |
| Job objects | Não | Sim |
| Strings encontradas em memória | Não | Sim |
| Verificação com VirusTotal | Não | Sim (integrado) |
| Cores por tipo de processo | Básico | Detalhado |

### Cores no Process Explorer

```
Rosa/vermelho claro  → Processo hospeda serviços do Windows
Azul claro           → Processo do usuário atual
Verde brilhante      → Processo recém-criado
Vermelho            → Processo sendo encerrado
Roxo                → Processo com imagem compactada (packed) ou protegida (PPL)
Branco              → Processo de outro usuário
Cinza               → Processo suspenso
```

### Experimento: encontrando handles vazados

1. Abra Process Explorer
2. View → Lower Pane → Handles
3. Clique em qualquer processo
4. Observe a lista de handles: arquivos abertos, eventos, mutexes, ports
5. Clique com direito em qualquer handle → Close Handle (fecha o handle manualmente — útil para "destravar" arquivos)

---

## Process Monitor (ProcMon)

Enquanto Process Explorer é uma foto, o **Process Monitor** é um vídeo. Ele captura **em tempo real** toda atividade de:
- Arquivo (criar, abrir, ler, escrever, deletar)
- Registro (ler, escrever, criar chave, deletar)
- Rede (conexões TCP/UDP)
- Processo (criar, encerrar, carregar DLL)

```
Uso clássico: diagnóstico de "arquivo não encontrado"

Um programa falha com "arquivo não encontrado".
Abra ProcMon → filtre pelo nome do processo.
Observe quais caminhos o programa tenta abrir (Result = NAME NOT FOUND).
O programa está procurando o arquivo no lugar errado — agora você sabe onde.
```

### Filtros do ProcMon

ProcMon captura decenas de milhares de eventos por segundo. Sem filtros, é inutilizável. Configure assim:

```
Filter → Filter (Ctrl+L)
  Process Name → contains → notepad → Include
  Result       → is       → ACCESS DENIED → Include
  Path         → contains → .dll → Exclude  (reduz ruído de carregamento de DLL)
```

**Salvar como .pml**: quando compartilhar uma captura com colegas, salve em formato PML — é comprimido e mantém todos os metadados.

---

## WinDbg Preview

O WinDbg é o debugger do Windows. Diferente de debuggers como Visual Studio Debugger ou gdb (que são focados em user mode), o WinDbg pode depurar:

- **Processos em user mode** (como qualquer debugger)
- **O kernel do Windows** via debug remoto (kernel debugging)
- **Crash dumps** (análise post-mortem de BSODs)
- **Time-travel debugging**: gravar a execução e navegar para frente e para trás

**Instalação**: Microsoft Store → "WinDbg Preview" (é gratuito, substitui o WinDbg clássico do SDK).

### Configurando símbolos

Sem símbolos, o WinDbg mostra endereços hexadecimais onde deveria mostrar nomes de funções. Configure uma vez:

```
File → Settings → Debugging Settings
Symbol search path: srv*C:\Symbols*https://msdl.microsoft.com/download/symbols
```

O símbolo server da Microsoft tem PDBs para ntoskrnl.exe, hal.dll, ntdll.dll, e todos os outros binários do sistema. Na primeira vez que você inspecionar um módulo, os símbolos são baixados automaticamente.

### Comandos essenciais do WinDbg

```windbg
# Listar todos os processos (kernel debug)
!process 0 0

# Ver detalhes de um processo específico
!process <endereço> 7

# Listar threads de um processo
!thread

# Ver o call stack da thread atual
k          # stack básica
kv         # stack com parâmetros
kb         # stack com endereços de retorno

# Procurar um símbolo
x nt!*Create*Process*

# Ver tipo de uma estrutura do kernel
dt nt!_EPROCESS
dt nt!_ETHREAD

# Ver valor de um campo de estrutura
dt nt!_EPROCESS <endereço>

# Analisar crash dump
!analyze -v

# Ver todos os drivers carregados
lm        # list modules
lmDvmntfs # detalhes do driver ntfs.sys

# Desmontar instrução
u <endereço>

# Inspecionar memória
dd <endereço>      # dump em DWORDs (4 bytes)
dq <endereço>      # dump em QWORDs (8 bytes)
da <endereço>      # dump como string ANSI
du <endereço>      # dump como string Unicode
```

### Debugando um crash dump de BSOD

Quando o Windows trava, ele escreve um dump em `C:\Windows\MEMORY.DMP` (ou `Minidump\*.dmp`). Para analisar:

```
1. Abra WinDbg Preview
2. File → Open Dump File → selecione o .dmp
3. No console, execute: !analyze -v
4. WinDbg mostra: qual módulo causou o crash, o call stack no momento,
   o bugcheck code, e frequentemente uma sugestão do que foi errado.
```

Exemplo de saída de `!analyze -v`:
```
BUGCHECK_CODE: 3b (SYSTEM_SERVICE_EXCEPTION)
PROCESS_NAME: meudriver.exe
MODULE_NAME: meudriver
IMAGE_NAME: meudriver.sys
STACK_TEXT:
  nt!KiSystemServiceCopyEnd
  nt!KiServiceLinkage
  meudriver!MinhaFuncao+0x47  ← onde o crash ocorreu
  ...
FOLLOWUP_NAME: MachineOwner
```

---

## Kernel Debugging Local

Para inspecionar o kernel do seu próprio Windows sem uma segunda máquina:

**Passo 1**: Habilite o debugging local (requer reinicialização):
```
bcdedit /debug on
bcdedit /dbgsettings local
```

**Passo 2**: Reinicie

**Passo 3**: No WinDbg Preview, selecione "Attach to kernel" → "Local"

No modo de debugging local, você **não pode pausar o kernel** (isso travaria a máquina), mas pode ler estruturas, inspecionar processos, e executar a maioria dos comandos de read-only.

Para debugging completo com breakpoints, você precisa de uma segunda máquina (física ou VM) conectada via cabo USB, rede, ou serial.

---

## Performance Monitor e Resource Monitor

Para análise de desempenho:

**Resource Monitor** (`resmon.exe`): visão em tempo real de CPU, memória, disco e rede por processo. Mais detalhado que o Task Manager, mas mais simples que o WinDbg.

**Performance Monitor** (`perfmon.exe`): coleta contadores de performance ao longo do tempo, gera gráficos e relatórios. Tem acesso a centenas de contadores do sistema:
- `Process\% Processor Time` — uso de CPU por processo
- `Memory\Available MBytes` — RAM disponível
- `PhysicalDisk\Disk Read Bytes/sec` — throughput de leitura de disco
- `System\Context Switches/sec` — quantos context switches por segundo

Para investigar picos de CPU ou memória, crie um Data Collector Set que captura 60 segundos de contadores quando disparado por um evento específico.

---

## AccessChk (Sysinternals)

Ferramenta de linha de comando para verificar permissões de qualquer objeto do Windows:

```powershell
# Quem pode escrever no diretório System32?
accesschk -d C:\Windows\System32

# Que serviços podem ser controlados por usuários normais?
accesschk -ucqv Users *

# Que chaves de registro Todos podem escrever?
accesschk -wk HKLM\

# Ver ACL de um processo específico (requer PID)
accesschk -p <PID>
```

AccessChk é essencial para auditoria de segurança: revela misconfigurações como arquivos de sistema que usuários normais podem modificar (um vetor clássico de privilege escalation).

---

## Autoruns

Todo mecanismo de persistência no Windows (programas que iniciam automaticamente) está documentado em uma categoria do **Autoruns**:

```
Categorias que o Autoruns verifica:
  Logon              → HKCU/HKLM Run keys, Startup folder
  Services           → Serviços do Windows
  Drivers            → Drivers registrados em HKLM\SYSTEM
  Scheduled Tasks    → Tarefas agendadas
  Browser Extensions → Add-ons de browsers
  DLL Hijacks        → DLLs que podem ser substituídas
  Codecs             → Codecs de mídia
  Boot Execute       → Processos que rodam durante o boot
  ... (mais 20 categorias)
```

Autoruns verifica a assinatura digital de cada item e pode comparar contra VirusTotal. Itens sem assinatura aparecem em amarelo; itens com assinatura inválida em vermelho.

Malware quase inevitavelmente aparece no Autoruns — e muitas análises de incidente começam aqui.

---

## Fluxo de trabalho: investigando um processo suspeito

Este é o workflow que você vai usar repetidamente:

```
1. Abra Process Explorer
   → O processo tem uma cor incomum? (roxo, sem ícone, nome estranho?)
   → Parent process faz sentido? (cmd.exe criando iexplore.exe é suspeito)
   → O caminho do executável é legítimo? (sistema em System32, não em Temp)
   → Clique direito → Check VirusTotal

2. Verifique as DLLs carregadas (View → Lower Pane → DLLs)
   → Há DLLs em caminhos incomuns?
   → Há DLLs sem assinatura?

3. Verifique as handles abertas (View → Lower Pane → Handles)
   → Quais arquivos estão abertos?
   → Há connections de rede abertas?
   → Há mutexes com nomes suspeitos?

4. Abra ProcMon, filtre pelo processo
   → Quais arquivos está lendo/escrevendo?
   → Quais chaves de registro está acessando?
   → Está se conectando a endereços IP externos?

5. Se ainda suspeito, abra no WinDbg
   → Attach to process
   → !analyze ou examinação manual das threads e pilhas
```

---

## Resumo: ferramentas por situação

| Situação | Ferramenta |
|----------|-----------|
| "Qual processo está usando este arquivo?" | Process Explorer → Handles, ou ProcMon |
| "O que este programa faz ao iniciar?" | ProcMon com filtro no processo |
| "Por que este serviço está falhando?" | Autoruns + Event Viewer |
| "O que está rodando automaticamente?" | Autoruns |
| "Quem tem permissão de escrita aqui?" | AccessChk |
| "Por que houve um BSOD?" | WinDbg + análise de dump |
| "Por que a memória está alta?" | Process Explorer + VMMap |
| "Este processo é malware?" | Process Explorer + VirusTotal + ProcMon |
| "Que syscalls este programa faz?" | API Monitor ou WinDbg |

---

*Fim do Capítulo 1. Próximo: [Capítulo 2 — Arquitetura do Sistema](../capitulo-02.md)*
