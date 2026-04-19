# Capítulo 7 — Segurança

> *Tradução do livro Windows Internals, 7ª Edição*  
> *Pavel Yosifovich, Alex Ionescu, Mark E. Russinovich, David A. Solomon*  
> *Microsoft Press, 2017*

---

Prevenir o acesso não autorizado a dados sensíveis é essencial em qualquer ambiente onde vários usuários tenham acesso aos mesmos recursos físicos ou de rede. Neste capítulo, explicamos como cada aspecto do design e implementação do Microsoft Windows foi influenciado de alguma forma pelos requisitos rigorosos de fornecer segurança robusta.

## Classificações de segurança

### Trusted Computer System Evaluation Criteria (TCSEC)

O TCSEC, comumente referido como o Orange Book, define níveis de classificação de segurança. A classificação C2 é considerada suficiente e a mais alta classificação prática para um SO de uso geral. Os requisitos-chave para uma classificação de segurança C2 são:

- **Instalação de logon seguro** — Os usuários devem ser capazes de ser identificados de forma única e só devem ter acesso ao computador após autenticação.
- **Controle de acesso discricionário** — O proprietário de um recurso pode determinar quem pode acessá-lo e o que pode fazer com ele.
- **Auditoria de segurança** — Capacidade de detectar e registrar eventos relacionados à segurança.
- **Proteção de reutilização de objeto** — Impede que usuários vejam dados que outro usuário excluiu ou acessem memória que outro usuário usou e liberou.

O Windows também atende a dois requisitos de segurança de nível B: funcionalidade de caminho confiável (prevenção contra Trojans via Ctrl+Alt+Delete) e gerenciamento de instalação confiável (suporte a funções administrativas separadas).

### Common Criteria (CC)

Em 1996, os EUA, Reino Unido, Alemanha, França, Canadá e Holanda lançaram a especificação Common Criteria for Information Technology Security Evaluation (CCITSE). O CC é mais flexível que as classificações TCSEC e inclui o conceito de Perfis de Proteção (PP) e Alvos de Segurança (ST).

Windows 10 e Windows Server 2012 R2 alcançaram certificação Common Criteria EAL 4+ em junho de 2016.

## Componentes do sistema de segurança

Os principais componentes e bancos de dados que implementam a segurança do Windows incluem:

- **Security Reference Monitor (SRM)** — Componente no executive do Windows responsável por definir a estrutura de dados de token de acesso, realizar verificações de acesso de segurança em objetos, manipular privilégios e gerar mensagens de auditoria de segurança.
- **Local Security Authority Subsystem Service (Lsass)** — Processo em modo usuário responsável pela política de segurança do sistema local, autenticação de usuários e envio de mensagens de auditoria de segurança ao log de eventos.
- **LSAIso.exe** — Usado pelo Lsass (se configurado) para armazenar hashes de token de usuários em vez de mantê-los na memória do Lsass. Como o Lsaiso.exe é um Trustlet rodando em VTL 1, nenhum processo normal pode acessar seu espaço de endereçamento.
- **Lsass policy database** — Banco de dados contendo configurações da política de segurança local, armazenado no registro em `HKLM\SECURITY`.
- **Security Accounts Manager (SAM)** — Serviço responsável por gerenciar o banco de dados que contém os usuários e grupos definidos na máquina local.
- **SAM database** — Banco de dados armazenado no registro em `HKLM\SAM`.
- **Active Directory** — Serviço de diretório que armazena informações sobre objetos em um domínio.
- **Pacotes de autenticação** — DLLs que implementam a política de autenticação do Windows.
- **Winlogon** — Processo em modo usuário responsável por responder ao SAS e gerenciar sessões de logon interativas.
- **LogonUI** — Apresenta a interface de logon aos usuários e usa credential providers para obter credenciais.
- **Credential Providers (CPs)** — Objetos COM que rodam no processo LogonUI para obter credenciais do usuário.
- **Netlogon** — Serviço Windows que estabelece o canal seguro para um controlador de domínio.
- **KSecDD** — Biblioteca em modo kernel de funções que implementam interfaces ALPC para componentes de segurança.
- **AppLocker** — Mecanismo que permite aos administradores especificar quais arquivos executáveis, DLLs e scripts podem ser usados por usuários e grupos especificados.

## Segurança baseada em virtualização (VBS)

O Windows 10 e o Server 2016 incluem uma arquitetura de segurança baseada em virtualização (VBS) que habilita um nível adicional ortogonal de confiança: o virtual trust level (VTL). Os principais componentes VBS incluem:

- **Hypervisor-Based Code Integrity (HVCI) e Kernel-Mode Code Integrity (KMCI)** — Potencializam o Device Guard.
- **LSA (Lsass.exe) e isolated LSA (LsaIso.exe)** — Potencializam o Credential Guard.

O código de modo usuário e de kernel normal roda em VTL 0 e não tem conhecimento da existência do VTL 1. Qualquer coisa colocada em VTL 1 está oculta e inacessível ao código VTL 0.

## Credential Guard

O Credential Guard protege as credenciais de autenticação de um usuário armazenando-as no processo LsaIso.exe, que roda como um Trustlet em VTL 1. As credenciais que o Credential Guard protege incluem:

- **Senha** — A credencial primária usada por usuários interativos.
- **NT one-way function (NT OWF)** — Um hash usado por componentes legados para identificar o usuário usando o protocolo NTLM.
- **Ticket-granting ticket (TGT)** — O equivalente do NTOWF quando é usado um mecanismo de autenticação remota mais moderno: Kerberos.

### Comunicação segura

O processo LsaIso.exe se comunica com o Lsass usando ALPC (Advanced Local Procedure Call) via RPC local. O protocolo de comunicação usa criptografia para garantir que o Lsass não possa interceptar as credenciais protegidas.

### Bloqueio UEFI

Para evitar que um administrador não presente fisicamente (como malware com direitos de administrador) desabilite o Credential Guard, o Secure Boot e o UEFI podem ser aproveitados para persistir a configuração do Credential Guard. Quando ativado com Bloqueio UEFI, uma variável de runtime EFI é escrita na memória do firmware.

## Device Guard

O Device Guard é um conjunto de recursos de segurança que usa virtualização de hardware para isolar o serviço de integridade de código dos drivers e programas. Enquanto o Credential Guard protege credenciais, o Device Guard protege o sistema contra código não confiável que é executado no kernel.

O Device Guard usa a **Hypervisor Code Integrity (HVCI)** para garantir que o código que é executado no kernel seja apenas código confiável e verificado. Com HVCI habilitado, o kernel não pode ser modificado para executar código arbitrário.

## Protegendo objetos

### Verificações de acesso

Quando uma thread tenta abrir um handle para um objeto, o sistema de segurança verifica o descritor de segurança do objeto para determinar se a thread tem o acesso desejado. O SRM realiza essas verificações com base no token de acesso da thread e na lista de controle de acesso discricionário (DACL) do objeto.

### Identificadores de segurança (SIDs)

Um SID é um valor único que identifica uma conta de usuário, conta de grupo ou sessão de logon. Um SID tem um formato binário que inclui:

- Número de revisão
- Valor de identificador de autoridade
- Até oito subautoridades relativas

Exemplos de SIDs bem conhecidos:
- S-1-5-18 — Conta SYSTEM
- S-1-1-0 — Grupo Everyone (Todos)
- S-1-5-32-544 — Grupo Administrators

### Contas de serviço virtual

O Windows suporta **virtual service accounts**, que são contas de serviço geradas automaticamente. Uma conta de serviço virtual tem um SID único e um nome exclusivo baseado no nome do serviço. Elas não requerem gerenciamento de senha porque o sistema gerencia automaticamente as credenciais da conta.

### Descritores de segurança e controle de acesso

Todo objeto protegível no Windows tem um **descritor de segurança** associado que contém:

- **SID do proprietário** — O SID do proprietário do objeto.
- **SID do grupo** — Para compatibilidade POSIX.
- **DACL (Discretionary Access Control List)** — Uma lista de controles de acesso discricionária que determina quais contas têm quais acessos ao objeto.
- **SACL (System Access Control List)** — Uma lista de controle de acesso do sistema que determina quais operações são auditadas.

### Dynamic Access Control (DAC)

O Windows Server 2012 e o Windows 8 introduziram o Dynamic Access Control, que permite o controle de acesso baseado em atributos (attribute-based access control). Em vez de identificar usuários e grupos individuais, os recursos podem definir as condições de acesso com base em atributos do usuário, do dispositivo ou do recurso.

### Direitos de conta e privilégios

Os privilégios controlam o acesso às operações relacionadas ao sistema. Os privilégios são atribuídos a usuários e grupos. Alguns exemplos de privilégios:

- `SeDebugPrivilege` — Permite depurar processos que o usuário não possui.
- `SeLoadDriverPrivilege` — Permite carregar e descarregar device drivers.
- `SeShutdownPrivilege` — Permite desligar o sistema.
- `SeBackupPrivilege` — Permite fazer backup de arquivos e diretórios independentemente das permissões.
- `SeTakeOwnershipPrivilege` — Permite assumir a propriedade de objetos.

### Super privilégios

Certos privilégios são tão poderosos que concedem ao usuário efetivamente acesso irrestrito ao sistema. O mais poderoso é `SeDebugPrivilege`, que permite ao usuário abrir handles a processos que de outra forma seriam inacessíveis.

> **Rode no Visual Studio (como Administrador)**
> O exemplo abaixo habilita `SeDebugPrivilege` no processo atual usando `AdjustTokenPrivileges`.
>
> ```cpp
> #include <windows.h>
> #include <stdio.h>
>
> BOOL HabilitarPrivilegio(LPCWSTR nomePrivilegio) {
>     HANDLE hToken;
>     OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
>
>     LUID luid;
>     LookupPrivilegeValueW(NULL, nomePrivilegio, &luid);
>
>     TOKEN_PRIVILEGES tp = {};
>     tp.PrivilegeCount = 1;
>     tp.Privileges[0].Luid = luid;
>     tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
>
>     BOOL ok = AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);
>     CloseHandle(hToken);
>     return ok && GetLastError() == ERROR_SUCCESS;
> }
>
> int main() {
>     if (HabilitarPrivilegio(SE_DEBUG_NAME))
>         printf("SeDebugPrivilege habilitado com sucesso\n");
>     else
>         printf("Falhou - rode como Administrador\n");
>
>     return 0;
> }
> ```

### Tokens de acesso de processos e threads

Cada processo e thread tem um **token de acesso** que contém informações de segurança sobre o usuário ou serviço que executou o processo ou thread. O token de acesso inclui:

- O SID do usuário
- Os SIDs dos grupos aos quais o usuário pertence
- A lista de privilégios do usuário
- Várias outras informações de segurança

> **Rode no Visual Studio**
> `OpenProcessToken` + `GetTokenInformation` inspecionam o token do processo atual e mostram o SID do usuario convertido em nome legivel com `LookupAccountSid`.
>
> ```cpp
> #include <windows.h>
> #include <sddl.h>
> #include <stdio.h>
>
> int main() {
>     HANDLE hToken;
>     OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken);
>
>     // Obtem o tamanho necessario
>     DWORD tamanho = 0;
>     GetTokenInformation(hToken, TokenUser, NULL, 0, &tamanho);
>
>     TOKEN_USER* pUser = (TOKEN_USER*)malloc(tamanho);
>     GetTokenInformation(hToken, TokenUser, pUser, tamanho, &tamanho);
>
>     // Converte SID para string legivel
>     LPWSTR sidStr = NULL;
>     ConvertSidToStringSidW(pUser->User.Sid, &sidStr);
>     printf("SID do usuario: %ls\n", sidStr);
>
>     // Obtem nome do usuario a partir do SID
>     WCHAR nome[256] = {}, dominio[256] = {};
>     DWORD nNome = 256, nDominio = 256;
>     SID_NAME_USE tipo;
>     LookupAccountSidW(NULL, pUser->User.Sid, nome, &nNome, dominio, &nDominio, &tipo);
>     printf("Usuario: %ls\\%ls\n", dominio, nome);
>
>     LocalFree(sidStr);
>     free(pUser);
>     CloseHandle(hToken);
>     return 0;
> }
> ```

## Auditoria de segurança

### Auditoria de acesso a objetos

A auditoria de segurança registra eventos de segurança no log de eventos de segurança. A auditoria de acesso a objetos rastreia tentativas de acessar objetos protegíveis (como arquivos e chaves de registro).

### Política de auditoria global

A política de auditoria global define quais categorias de eventos de segurança são registradas. As categorias incluem logon/logoff, acesso a objetos, uso de privilégios, mudança de política, etc.

### Configurações de Política de Auditoria Avançada

O Windows Vista introduziu as Configurações de Política de Auditoria Avançada, que fornecem um conjunto muito mais granular de categorias e subcategorias de auditoria do que a política de auditoria global anterior.

## AppContainers

O Windows 8 introduziu um novo modelo de sandbox chamado **AppContainer**. Os AppContainers fornecem isolamento entre apps e entre um app e o restante do sistema. O app roda com um token que contém o SID do AppContainer, que limita os recursos que o app pode acessar.

### Visão geral dos apps UWP

Os apps UWP (Universal Windows Platform) rodam dentro de AppContainers. Quando um app UWP tenta acessar um recurso, o sistema verifica as capacidades do app no token do AppContainer. Se o app não tiver a capacidade necessária, o acesso é negado.

## Logon

### Inicialização do Winlogon

O Winlogon.exe é responsável por gerenciar sessões de logon interativas. Durante a inicialização:

1. O Winlogon registra o SAS (Secure Attention Sequence — Ctrl+Alt+Delete).
2. Lança o LogonUI para exibir a interface de logon.
3. Coordena com o Lsass para autenticar o usuário.

### Etapas de logon do usuário

1. O usuário pressiona Ctrl+Alt+Delete (SAS).
2. O Winlogon detecta o SAS e lança o LogonUI.
3. O LogonUI usa um Credential Provider para obter as credenciais do usuário.
4. As credenciais são passadas para o Lsass para autenticação.
5. O Lsass usa o pacote de autenticação apropriado (como Kerberos ou NTLM).
6. Após autenticação bem-sucedida, o Lsass cria um token de acesso.
7. O Winlogon usa o token de acesso para criar o primeiro processo do usuário (normalmente Explorer.exe via Userinit.exe).

### Autenticação assegurada

O Windows implementa a **autenticação assegurada** para garantir que os usuários estejam se autenticando de forma segura. Isso inclui mecanismos como Kerberos armado (que vincula a autenticação do usuário a um dispositivo específico) e Credential Guard.

### Windows Biometric Framework

O Windows Biometric Framework (WBF) fornece uma infraestrutura para hardware e software biométrico. O WBF inclui:

- Um serviço biométrico do Windows (WBS) que gerencia dispositivos biométricos.
- Drivers para hardware biométrico.
- APIs para aplicativos que usam biometria.

### Windows Hello

O **Windows Hello** é o sistema de autenticação biométrica do Windows 10. Ele suporta reconhecimento facial, impressão digital e reconhecimento de íris. Com o Windows Hello, os usuários nunca precisam digitar uma senha, removendo o risco de roubo de senha.

## User Account Control (UAC) e virtualização

### Virtualização de sistema de arquivos e registro

O UAC usa virtualização para que aplicativos mais antigos que esperavam ter acesso de gravação a locais protegidos do sistema possam continuar funcionando sem exigir acesso de administrador real. Quando um aplicativo não elevado tenta gravar em um local protegido do sistema de arquivos ou do registro, o acesso é redirecionado para um local virtual específico do usuário.

### Elevação

Quando um usuário tenta executar uma operação que requer elevação de privilégio, o UAC exibe um prompt pedindo confirmação. Se o usuário confirmar, o processo é executado com um token de acesso elevado.

## Mitigações de exploits

### Políticas de mitigação de processos

O Windows suporta políticas de mitigação de processos que permitem que aplicativos se protejam contra classes específicas de exploits:

- **Data Execution Prevention (DEP)** — Impede a execução de código em páginas de dados.
- **Address Space Layout Randomization (ASLR)** — Randomiza os endereços virtuais dos módulos carregados para dificultar ataques.
- **Control Flow Guard (CFG)** — Verifica os alvos de chamadas de função indiretas para prevenir exploits que desviam o fluxo de controle.
- **Stack protections** — Proteções de pilha para detectar estouro de buffer.
- **Arbitrary Code Guard (ACG)** — Impede a criação de novos executáveis em memória ou a modificação de executáveis existentes.
- **Code Integrity Guard** — Restringe o carregamento de imagens às assinadas pelo Windows ou pela Microsoft Store.

### Control Flow Integrity (CFI)

A **Control Flow Integrity** é uma classe de mitigações que restringem os tipos de transferências de controle que um programa pode executar. O CFG é a implementação da CFI no Windows e valida destinos de chamadas de função indiretas em relação a um conjunto de destinos válidos calculados pelo compilador.

## Identificação de aplicativos (AppLocker)

O **AppLocker** permite que administradores especifiquem quais aplicativos os usuários podem executar. As regras do AppLocker podem ser baseadas em:

- **Caminho** — Permite ou nega baseado no caminho do arquivo.
- **Hash de arquivo** — Permite ou nega com base em um hash criptográfico do arquivo.
- **Assinatura digital** — Permite ou nega com base na assinatura digital do arquivo.

### Software Restriction Policies (SRP)

As **Software Restriction Policies** são o mecanismo predecessor do AppLocker. As SRPs são aplicadas usando o Local Security Policy editor. Ao contrário do AppLocker, as SRPs não suportam regras baseadas em assinatura digital e não têm a capacidade de gerar logs de eventos.

## Kernel Patch Protection

### PatchGuard

O **PatchGuard** é uma tecnologia que detecta e responde a modificações não autorizadas do kernel em sistemas Windows de 64 bits. O PatchGuard protege várias estruturas críticas do kernel:

| Elemento protegido | Descrição |
|--------------------|-----------|
| Tabela de despacho de serviço do sistema (SSDT) | Tabela de ponteiros para funções de sistema |
| Interrupção global (GDT, IDT) | Tabelas de descritor de segmento e interrupção |
| MSRs | Registradores específicos do modelo, como LSTAR e SYSENTER |
| Imagem do kernel | Código e dados do kernel em memória |
| Pilha de threads do kernel | Conteúdo das pilhas de threads do sistema |

Quando o PatchGuard detecta modificações não autorizadas, ele grava no sistema com o código de bug check **0x109 (CRITICAL_STRUCTURE_CORRUPTION)**.

O PatchGuard não é habilitado quando o sistema inicializa em modo de depuração com uma conexão de depuração remota ativa.

### HyperGuard

O **HyperGuard** é uma tecnologia complementar ao PatchGuard que roda em VTL 1 e pode detectar modificações no kernel feitas por código malicioso rodando em VTL 0 (nível do kernel). Ao contrário do PatchGuard, o HyperGuard:

- Não precisa depender de ofuscação para segurança.
- Não precisa operar de forma não determinística.
- Pode detectar uma maior variedade de ataques.

Quando o HyperGuard detecta uma inconsistência, ele trava o sistema com o código **0x18C (HYPERGUARD_VIOLATION)**.

Elementos protegidos pelo HyperGuard incluem:

| Elemento protegido | Descrição |
|--------------------|-----------|
| Páginas de código do kernel | Verificações de hash das páginas de código do kernel |
| MSRs | Registradores específicos do modelo críticos |
| Estruturas internas do hypervisor | Estruturas que o hypervisor usa internamente |
| Dados críticos de VTL 1 | Tabelas de chamadas seguras e callbacks |

Em sistemas com VBS habilitado, a **Non-Privileged Instruction Execution Prevention (NPIEP)** também mitiga o vazamento de informações do KASLR por meio das instruções SGDT, SIDT e SLDT.

## Conclusão

O Windows fornece um conjunto abrangente de funções de segurança que atendem aos principais requisitos de agências governamentais e instalações comerciais. As principais funcionalidades de segurança incluem:

- Um modelo robusto de controle de acesso baseado em tokens de acesso e descritores de segurança.
- Uma infraestrutura de autenticação extensível com suporte para Kerberos, NTLM e autenticação biométrica.
- Credential Guard e Device Guard que aproveitam a virtualização para proteger credenciais e integridade de código.
- AppContainer para isolar apps UWP e User Account Control para gerenciar elevação de privilégio.
- Mitigações de exploits como DEP, ASLR e CFG.
- PatchGuard e HyperGuard para detectar e responder a modificações não autorizadas do kernel.

No próximo capítulo do Part 2 do livro, serão abordados vários mecanismos que estão espalhados por todo o sistema Windows.
