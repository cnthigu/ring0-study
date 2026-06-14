# Capítulo 2 — Processos de Sistema

> Livro didático baseado em *Windows Internals, 7th Edition*.
> Atualizado para Windows 11 com exemplos práticos em C++ moderno.
> Repositório: [ring0-study](https://github.com/cnthigu/ring0-study)

> Navegação: [Anterior](capitulo-02-06-executive-kernel-hal.md) | [Índice Cap.2](capitulo-02.md)

---

## A árvore de processos do sistema

Todo Windows 11 em operação normal tem um conjunto fixo de processos de sistema. Visualize com Process Explorer (View → Process Tree):

```
[System Process] (PID 0) — não é um processo real
[System] (PID 4)
  └── Memory Compression (PID variável)
  └── Secure System (PID variável, se VBS ativo)
smss.exe (PID ~xxx)
  └── csrss.exe [Session 0]
  └── wininit.exe [Session 0]
        ├── services.exe
        │     ├── svchost.exe (LocalSystemNetworkRestricted)
        │     ├── svchost.exe (LocalService)
        │     ├── svchost.exe (NetworkService)
        │     └── ... (dezenas de instâncias)
        └── lsass.exe
              └── (lsaiso.exe — se Credential Guard ativo)
smss.exe [Session 1] → spawna sessão do usuário
  └── csrss.exe [Session 1]
  └── winlogon.exe [Session 1]
        └── userinit.exe → explorer.exe
```

---

## Processos especiais (Minimal Processes)

Estes "processos" são diferentes de processos normais — não executam uma imagem PE de user mode:

### Idle Process (PID 0)

Não é um processo real — é um artifício de contabilidade. Cada CPU tem um thread de Idle que roda quando não há nada mais para fazer. O tempo do sistema reportado como "System Idle Process" é o tempo total que as CPUs passaram sem trabalho.

- **PID 0** por convenção (handle inválido)
- Um thread de Idle por CPU lógica
- Nunca aparece no Process Explorer como processo "normal"

### System Process (PID 4)

O processo de sistema contém a maioria das threads de kernel mode e possui a maioria dos handles de objetos de kernel que o OS usa internamente.

Por que um processo e não threads "soltas"? Para ter um lugar onde o kernel possa alocar handles — qualquer objeto do kernel criado pelo sistema precisa pertencer a algum processo para contabilidade de handles.

```
kd> !process 4 0    ← ver o System process
kd> !process 4 7    ← ver com threads e handles
```

### Secure System Process

Presente quando VBS está ativo. Contém o espaço de endereços do Secure Kernel (VTL 1). Visível no Process Explorer como um processo com ícone de escudo.

### Memory Compression Process

Introduzido no Windows 10. O Memory Manager comprime páginas de memória que foram pouco usadas recentemente em vez de paginá-las para disco — RAM comprimida é muito mais rápida de recuperar do que disco.

As páginas comprimidas vivem no espaço de endereços deste processo. Você vê no Task Manager como "SysMain" (o serviço) consumindo RAM — na verdade é o espaço de compressão.

---

## Smss.exe — Session Manager

O **Session Manager** (`smss.exe`) é o **primeiro processo user mode** criado pelo kernel. Não tem pai — o kernel o cria diretamente.

`smss.exe` é uma **native image** (link apenas contra ntdll.dll, sem Win32). Isso é necessário porque o Windows subsystem ainda não existe quando ele inicia.

### O que Smss.exe faz

**1. Executa AutoChk**: antes de montar partições, verifica a integridade do sistema de arquivos (`autochk.exe`). Também é native image.

**2. Define variáveis de ambiente do sistema**: lê de `HKLM\SYSTEM\CurrentControlSet\Control\Session Manager\Environment`.

**3. Popula o DOS Device namespace**: cria os links simbólicos `C:`, `D:`, etc. apontando para os volumes reais.

**4. Inicia os subsistemas**: lê de `HKLM\SYSTEM\CurrentControlSet\Control\Session Manager\SubSystems`. O Windows subsystem (`csrss.exe`) é iniciado aqui.

**5. Cria sessions adicionais**: quando um novo usuário faz login (ou via Remote Desktop), Smss cria uma cópia de si mesmo por sessão, que por sua vez cria `csrss.exe` e `winlogon.exe` para aquela sessão.

**6. AutoStart applications**: entradas em `HKLM\SYSTEM\CurrentControlSet\Control\Session Manager\BootExecute`.

---

## Csrss.exe — Windows Subsystem Process

Uma instância por sessão. **Processo crítico** — o sistema crasha imediatamente se morrer.

Em Session 0 (serviços): gerencia processos de console de serviços.
Em Session 1+ (usuários): gerencia janelas de console, input de teclado para apps console, e atualizações de cursor.

O Csrss da sessão interativa também hospeda a "raw input thread" (em `winsrv.dll`) — a thread que lê teclado e mouse do driver de input e distribui para as aplicações corretas.

---

## Wininit.exe — Inicializador da Session 0

Cria os processos críticos da sessão de sistema:

1. **services.exe** — Service Control Manager
2. **lsass.exe** — Local Security Authority
3. **lsm.exe** — Local Session Manager (gerencia sessões do Terminal Services)

Wininit também seta a sessão 0 como "Session 0 isolation" — garante que serviços em session 0 não possam interagir com a sessão do usuário (prevenção de "shatter attacks").

---

## Winlogon.exe — Gerenciador de Login

Uma instância por sessão de usuário interativa. Responsável por:

- **Sequência de Secure Attention (Ctrl+Alt+Del)**: GINA ou Credential Providers
- **Autenticação**: delega para `lsass.exe` via SSPI
- **Carregamento do profile do usuário**: `HKCU`, pasta AppData
- **Criação da sessão do usuário**: lança `userinit.exe` → `explorer.exe`
- **Tela de lock**: monitora inatividade, exibe screensaver/lock screen

O Winlogon não autentica usuários diretamente — delega para Credential Providers (que implementam métodos como senha, PIN, biometria, smartcard) e para o LSASS.

---

## Services.exe — Service Control Manager (SCM)

Responsável por gerenciar o ciclo de vida de todos os Windows Services:

- **Inicia serviços**: lê de `HKLM\SYSTEM\CurrentControlSet\Services`
- **Para e pausa serviços**: via APIs de controle de serviços
- **Gerencia dependências**: se Serviço B depende de Serviço A, garante a ordem de início
- **Reinicia serviços com falha**: configurável por serviço (nenhuma ação, reiniciar, rodar um programa)

```powershell
# Ver todos os serviços e status
Get-Service | Sort-Object Status | Format-Table Name, Status, StartType

# Ver dependências de um serviço
Get-Service -Name wuauserv -DependentServices
Get-Service -Name wuauserv -RequiredServices
```

### Svchost.exe — Generic Service Host

A maioria dos serviços do Windows não tem seu próprio executável — eles são DLLs hospedadas por `svchost.exe`. Uma instância de svchost pode hospedar múltiplos serviços que compartilham o mesmo contexto de segurança.

No Windows 10/11 (com >= 3.5 GB RAM), cada serviço roda em seu próprio processo svchost separado. Isso melhora o isolamento: se um serviço trava, não derruba os outros.

```powershell
# Ver quais serviços cada svchost hospeda
Get-WmiObject -Class Win32_Service | 
    Where-Object {$_.PathName -like "*svchost*"} |
    Format-Table Name, PathName, StartMode
```

---

## Lsass.exe — Local Security Authority

O guardião da segurança do sistema. Responsável por:

- **Autenticação**: valida credenciais via provedores de autenticação (NTLM, Kerberos, biometria)
- **Tokens**: cria tokens de acesso para sessões autenticadas
- **Política de segurança local**: senhas mínimas, lockout, auditoria
- **Cache de credenciais**: hashes de senha, tickets Kerberos

`lsass.exe` roda como SYSTEM e tem privilégios extremamente altos. Historicamente era o alvo principal do Mimikatz — dump de memória do lsass revelava hashes de senha.

Com **Credential Guard** ativo, `lsaiso.exe` (LSA Isolated) rodando em VTL 1 armazena as credenciais sensíveis. O `lsass.exe` de VTL 0 vira um proxy — não tem os segredos, apenas solicita operações ao `lsaiso.exe` via Secure Calls.

---

## Explorer.exe — O Shell do Windows

Tecnicamente não é um "processo de sistema" no sentido estrito, mas é o processo que cria a experiência visual do Windows. Criado por `userinit.exe` após login bem-sucedido:

```
winlogon.exe
  → cria userinit.exe
      → lança explorer.exe
          → cria a taskbar, desktop, ícones
          → shell namespace (My Computer, Network, etc.)
```

Se o Explorer trava, ele pode ser reiniciado sem logout — basta Task Manager → Run → `explorer.exe`. Isso mostra que a sessão do usuário não depende do Explorer para existir; ele apenas fornece a UI.

---

## Experimento: observando o boot em WinDbg

Com kernel debugging ativo e uma conexão de debug, você pode acompanhar a criação dos processos de sistema durante o boot:

```
kd> !process 0 0    ← lista todos os processos no momento
kd> bp nt!PspCreateProcess  ← breakpoint em toda criação de processo
g
(o debugger para em cada novo processo criado)
kd> du @rcx   ← nome do processo sendo criado (simplificado)
```

Você verá a sequência: System → smss → csrss → wininit → services/lsass/winlogon → svchost → explorer.

---

*Fim do Capítulo 2. Próximo: [Capítulo 3 — Processos e Jobs](capitulo-03.md)*
