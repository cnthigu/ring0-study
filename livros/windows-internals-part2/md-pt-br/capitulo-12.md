# Capitulo 12 - Inicializacao e Desligamento

> *Traducao do livro Windows Internals, Part 2, Seventh Edition*
> *Andrea Allievi, Alex Ionescu, Mark E. Russinovich, David A. Solomon*
> *Microsoft Press, 2022*

---

Este capitulo descreve o fluxo de operacoes que ocorre quando o sistema inicia e desliga, e os componentes do sistema operacional envolvidos no fluxo de boot. O capitulo tambem analisa as novas tecnologias trazidas pelo UEFI, como Secure Boot, Measured Boot e Secure Launch.

---

## Processo de boot

O processo de inicializacao do Windows e dividido em varias fases, cada uma responsavel por inicializar componentes especificos do sistema.

### O boot UEFI

A maioria dos sistemas modernos usa **UEFI (Unified Extensible Firmware Interface)** como firmware de inicializacao. O UEFI substituiu o BIOS tradicional e oferece:

- Interface de programacao padronizada para acesso a hardware durante o boot.
- Suporte a discos GPT (GUID Partition Table) com volumes maiores que 2 TB.
- Boot seguro (Secure Boot).
- Ambiente de pre-boot mais sofisticado.
- Boot mais rapido graças a inicializacao paralela de hardware.

A sequencia de boot UEFI e:

1. **Phase SEC (Security):** O firmware realiza verificacoes de integridade basicas do hardware e inicializa os registradores da CPU em um estado conhecido.

2. **Phase PEI (Pre-EFI Initialization):** Inicializa a memoria RAM e os controladores criticos. O codigo PEI tem acesso muito limitado a hardware.

3. **Phase DXE (Driver Execution Environment):** A maioria do hardware e inicializado nesta fase. Os UEFI drivers sao carregados e executados. A interface UEFI completa se torna disponivel.

4. **Phase BDS (Boot Device Selection):** O firmware verifica as opcoes de boot configuradas (variaveis UEFI) e tenta inicializar o sistema a partir do dispositivo de boot selecionado.

5. **Carregamento do bootloader:** O bootloader (como `bootmgfw.efi` do Windows) e carregado do dispositivo de boot.

### O processo de boot BIOS

Em sistemas mais antigos com BIOS tradicional:

1. A **POST (Power-On Self Test)** verifica o hardware.
2. O BIOS le o **MBR (Master Boot Record)** do primeiro setor do disco de boot.
3. O codigo no MBR localiza e carrega o **Volume Boot Record (VBR)** da particao ativa.
4. O VBR carrega o arquivo bootloader (`bootmgr` para Windows Vista+).

### Secure Boot

O **Secure Boot** e uma especificacao da UEFI que garante que apenas software confiavel seja executado durante o boot. Ele funciona assim:

1. O firmware UEFI tem um banco de dados de certificados confiados (`db`) e um banco de dados de certificados revogados (`dbx`).
2. Antes de carregar qualquer EFI executable (como o bootloader do Windows), o firmware verifica a assinatura digital do executavel contra os certificados em `db`.
3. Se a assinatura for valida e o certificado nao estiver em `dbx`, o executavel e carregado.
4. Se a assinatura for invalida ou o executavel nao estiver assinado, o boot e rejeitado.

O Windows Boot Manager e assinado pela Microsoft. Portanto, em um sistema com Secure Boot habilitado, apenas versoes assinadas do bootloader do Windows podem ser carregadas.

Isso impede que bootloaders maliciosos (bootloaders de rootkits) sejam carregados durante o processo de boot - mesmo se um atacante tiver acesso fisico ao disco.

### O Windows Boot Manager

O **Windows Boot Manager** (arquivo `bootmgfw.efi` em sistemas UEFI, `bootmgr` em sistemas BIOS) e o primeiro componente do Windows a ser carregado pelo firmware. Suas responsabilidades incluem:

- Ler a **BCD (Boot Configuration Data)** - o banco de dados que contem as configuracoes de boot.
- Exibir o menu de boot se houver mais de uma entrada de boot.
- Carregar e executar o OS Loader para a entrada de boot selecionada.
- Implementar politicas de boot seguro (verificar hashes de componentes de boot).

#### Boot Configuration Data (BCD)

A **BCD** substitui o arquivo `boot.ini` usado no Windows XP e versoes anteriores. A BCD e uma estrutura similar ao registro, armazenada em:

- Em sistemas UEFI: Arquivo `\EFI\Microsoft\Boot\BCD` na particao EFI.
- Em sistemas BIOS: Arquivo `\Boot\BCD` na particao de sistema ativa.

A BCD pode ser editada com a ferramenta `bcdedit.exe`:

```
bcdedit /enum all        <- Lista todas as entradas de boot
bcdedit /set {default} debug on  <- Habilita debugging do kernel
bcdedit /set {default} testsigning on  <- Habilita drivers nao assinados (para dev)
```

### O menu de boot

Se houver mais de uma entrada de boot na BCD (ou se o usuario pressionar F8/F11 durante o boot), o Windows Boot Manager exibe um menu de boot grafico.

A partir do Windows 8, o menu de boot e renderizado em modo grafico de alta resolucao pelo proprio Windows Boot Manager (usando os recursos grafiticos do UEFI GOP ou VBE), em vez do menu de texto legado.

### Launching a boot application

Quando uma entrada de boot e selecionada, o Boot Manager carrega e executa o executavel EFI correspondente - normalmente o **Windows OS Loader** (`winload.efi` em UEFI, `winload.exe` em BIOS).

Antes de transferir o controle, o Boot Manager pode executar varias verificacoes de seguranca:

- Verificar o hash do OS Loader contra valores armazenados na BCD ou no TPM.
- Verificar politicas de integridade de boot.
- Configurar o ambiente de pre-launch para o Secure Launch.

### Measured Boot

O **Measured Boot** e um recurso que registra criptograficamente informacoes sobre o estado do sistema durante o processo de boot. Cada componente carregado (firmware UEFI, bootloader, OS Loader, kernel, drivers de boot) e "medido" - seu hash e registrado em um **PCR (Platform Configuration Register)** no TPM.

Os PCRs sao acumulativos - cada nova medicao estende o PCR existente:

```
PCR_novo = hash(PCR_atual || hash_do_componente)
```

Isso significa que o valor final do PCR reflete toda a sequencia de boot. Se qualquer componente mudar (ou for substituido por um componente malicioso), o valor do PCR sera diferente.

As medicoes do Measured Boot podem ser usadas para:

- **Attestation remoto:** Um servidor pode verificar o log de medicoes do boot e o estado do TPM para confirmar que o sistema iniciou de forma integra.
- **Selagem de segredos do TPM:** Chaves criptograficas podem ser "seladas" a um conjunto especifico de valores de PCR, tornando-as inacessiveis se o sistema boot em um estado diferente (por exemplo, se um rootkit modificou o bootloader).

### Trusted Execution (Boot seguro do kernel)

O **Trusted Execution** e o termo geral para o conjunto de verificacoes que o OS Loader realiza antes de executar o kernel do Windows. Inclui:

- Verificacao de assinatura do kernel (`ntoskrnl.exe`).
- Verificacao de assinatura de todos os drivers de boot carregados.
- Verificacao de politica de integridade de codigo (quando HVCI esta habilitado).

---

## O OS Loader do Windows

O **Windows OS Loader** (`winload.efi` em UEFI) e responsavel por carregar todos os componentes necessarios para inicializar o kernel Windows:

1. **Carrega o kernel (`ntoskrnl.exe`)** e a **HAL (`hal.dll`)** na memoria.
2. **Carrega os drivers de boot** - drivers com tipo de start = Boot que sao necessarios para acessar o dispositivo de armazenamento que contem o sistema.
3. **Carrega o hive SYSTEM do registro** - necessario para o kernel inicializar os servicos.
4. **Configura a memoria inicial** - cria as estruturas de paginas iniciais.
5. **Transfere o controle** para o ponto de entrada do kernel.

### Boot a partir de iSCSI

O Windows suporta boot a partir de um dispositivo de armazenamento iSCSI (storage de rede). Nesse cenario, o OS Loader inicializa o stack de rede minimo necessario para acessar o volume de boot via rede.

---

## O loader do hypervisor

Se o Hyper-V estiver habilitado, o OS Loader tambem carrega o hypervisor antes de transferir o controle ao kernel. A sequencia e:

1. O OS Loader carrega o hypervisor (`hvix64.exe` para Intel ou `hvax64.exe` para AMD).
2. O hypervisor e iniciado, inicializando as estruturas de virtualizacao.
3. O kernel Windows e carregado como uma particao raiz dentro do hypervisor.

### Politica de inicializacao do VSM

Se a Virtualization-Based Security (VBS) estiver habilitada, o loader tambem carrega o **Secure Kernel** (`securekernel.exe`) e o configura no VTL 1.

A sequencia exata e:

1. Hypervisor e iniciado.
2. O VTL 1 e habilitado pelo hypervisor.
3. O Secure Kernel e carregado no VTL 1.
4. O Secure Kernel inicializa suas proprias estruturas.
5. O kernel normal (`ntoskrnl.exe`) e iniciado no VTL 0.

### O Secure Launch

O **Secure Launch** (tambem chamado de DRTM - Dynamic Root of Trust for Measurement) e uma tecnologia que permite re-estabelecer uma raiz de confianca criptografica em um ponto especifico durante o processo de boot, em vez de depender apenas da cadeia de boot desde o firmware.

Isso e util porque:

- O firmware UEFI e muito complexo e pode conter vulnerabilidades.
- O Secure Launch permite que o Windows estabeleca confianca sem depender da integridade completa do firmware.

Em Intel, o Secure Launch usa a instrucao `GETSEC[SENTER]` (parte do Intel TXT - Trusted Execution Technology).

---

## Inicializando o kernel e os subsistemas do executive

Quando o kernel recebe o controle do OS Loader, ele executa a funcao `KiSystemStartup`. Esta funcao:

1. Inicializa os processadores restantes (em sistemas SMP, os processadores secundarios sao inicializados nesta fase).
2. Inicializa o hardware de CPU (IDT, GDT, TSS).
3. Inicializa o gerenciador de interrupcoes (IRQL, APIC).
4. Chama `KiInitializeKernel` para inicializar o nucleo do kernel.
5. Inicia a fase 0 de inicializacao do executive.
6. Cria a thread principal do sistema (System thread).
7. Inicia a fase 1 de inicializacao.

### Inicializacao do kernel na fase 1

A **fase 1 de inicializacao do kernel** e mais elaborada. A sequencia geral e:

1. **HAL:** A Hardware Abstraction Layer e completamente inicializada.
2. **Gerenciador de memoria:** As estruturas de paginas, zonas de memoria, e outros mecanismos sao configurados.
3. **Object Manager:** O namespace de objetos e inicializado (`\` como raiz).
4. **Security Reference Monitor (SRM):** O modulo de seguranca do kernel e inicializado.
5. **Process Manager:** O processo System e os processos iniciais sao criados.
6. **I/O Manager:** O framework de I/O, drivers e PnP Manager sao inicializados.
7. **Registro:** Os hives do registro sao carregados.
8. **Executive Objects:** Os outros componentes do executive sao inicializados (Pool Manager, Cache Manager, etc.).
9. **Session Manager (Smss.exe):** O primeiro processo usuario e criado.

> **Rode no Visual Studio**
> Voce pode ver quando cada componente do kernel foi inicializado via o ETW ou analisando o log de inicializacao do sistema. Este codigo le o tempo de boot do sistema a partir da API:
>
> ```cpp
> #include <windows.h>
> #include <stdio.h>
>
> int main() {
>     // Tempo desde o ultimo boot em milissegundos
>     ULONGLONG uptime = GetTickCount64();
>
>     ULONGLONG dias   = uptime / (1000ULL * 60 * 60 * 24);
>     ULONGLONG horas  = (uptime / (1000ULL * 60 * 60)) % 24;
>     ULONGLONG minutos = (uptime / (1000ULL * 60)) % 60;
>     ULONGLONG segundos = (uptime / 1000ULL) % 60;
>
>     printf("Sistema ativo ha: %llud %lluh %llum %llus\n",
>         dias, horas, minutos, segundos);
>
>     // Hora do ultimo boot
>     SYSTEMTIME st;
>     GetSystemTime(&st);
>     FILETIME ft;
>     SystemTimeToFileTime(&st, &ft);
>
>     ULARGE_INTEGER agora;
>     agora.LowPart  = ft.dwLowDateTime;
>     agora.HighPart = ft.dwHighDateTime;
>
>     // Boot time = agora - uptime (convertido para unidades de 100ns)
>     ULONGLONG bootTimeRaw = agora.QuadPart - (uptime * 10000ULL);
>
>     FILETIME ftBoot;
>     ftBoot.dwLowDateTime  = (DWORD)bootTimeRaw;
>     ftBoot.dwHighDateTime = (DWORD)(bootTimeRaw >> 32);
>
>     SYSTEMTIME stBoot;
>     FileTimeToSystemTime(&ftBoot, &stBoot);
>     printf("Ultimo boot: %02d/%02d/%04d %02d:%02d:%02d (UTC)\n",
>         stBoot.wDay, stBoot.wMonth, stBoot.wYear,
>         stBoot.wHour, stBoot.wMinute, stBoot.wSecond);
>
>     return 0;
> }
> ```

---

## Smss, Csrss e Wininit

### Session Manager (Smss.exe)

O **Session Manager** (`smss.exe`) e o primeiro processo usuario criado pelo kernel. Ele roda na sessao 0 com privilégios de Sistema e e responsavel por:

1. **Carregar subsistemas nativos:** Incluindo o `csrss.exe` (subsistema Win32).
2. **Configurar o espaco de enderecamento:** Inicializa o Memory Manager com a configuracao de espaco de enderecamento definida no registro.
3. **Criar pagefile(s):** Le a configuracao de paginacao do registro e cria os arquivos de paginacao.
4. **Executar processos de inicializacao:** Executa os programas listados em `HKLM\SYSTEM\CurrentControlSet\Control\Session Manager\BootExecute` (como `autochk.exe` para verificacao de disco).
5. **Criar sessoes adicionais:** Para cada nova sessao de usuario, o Smss cria uma instancia filho de si mesmo que inicializa o ambiente da sessao.

### Client/Server Runtime Subsystem (Csrss.exe)

O **Csrss.exe** implementa o subsistema Win32 em modo usuario. Cada sessao tem seu proprio processo `csrss.exe`. Suas responsabilidades incluem:

- Gerenciamento de consoles (janelas de terminal CMD).
- Criacao e destruicao de processos e threads Win32.
- Comunicacao com o driver Win32 kernel (`win32k.sys`).
- Gerenciamento de handles de processo e thread no subsistema.

### Wininit.exe

O **Wininit.exe** e o processo de inicializacao do Windows para a sessao 0. Ele:

1. Inicia o **Local Security Authority Subsystem Service (Lsass.exe)**.
2. Inicia o **Service Control Manager (Services.exe)**.
3. Inicia o **Local Session Manager (Lsm.exe)**.

---

## ReadyBoot

O **ReadyBoot** e um mecanismo de otimizacao de boot que aprende quais arquivos sao necessarios durante o boot e os pre-carrega na memoria durante os primeiros 60 segundos antes do login.

O ReadyBoot:

- Monitora os acessos a arquivo durante as ultimas sessoes de boot.
- Cria um arquivo de cache (`.dat`) com os dados dos arquivos mais acessados durante o boot.
- Na proxima inicializacao, le esses dados antecipadamente enquanto o sistema esta inicializando outros componentes.

O ReadyBoot e gerenciado pelo servico **Superfetch/SysMain** e seus dados sao armazenados em:

`%SystemRoot%\Prefetch\ReadyBoot\`

---

## Imagens que iniciam automaticamente

Varios tipos de programas podem ser configurados para iniciar automaticamente com o Windows:

| Mecanismo | Local | Descricao |
|-----------|-------|-----------|
| **Servicos** | `HKLM\SYSTEM\CurrentControlSet\Services` | Servicos do Windows (auto-start) |
| **Run/RunOnce** | `HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\Run` | Executaveis para todos os usuarios |
| **Run/RunOnce** | `HKCU\SOFTWARE\Microsoft\Windows\CurrentVersion\Run` | Executaveis para o usuario atual |
| **Startup folder** | `%ProgramData%\Microsoft\Windows\Start Menu\Programs\StartUp` | Atalhos para todos os usuarios |
| **Startup folder** | `%AppData%\Microsoft\Windows\Start Menu\Programs\Startup` | Atalhos para o usuario atual |
| **Task Scheduler** | `\Microsoft\Windows\` | Tarefas agendadas para boot/login |
| **Winlogon** | `HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Winlogon` | Processos como Userinit, Shell |
| **AppInit_DLLs** | `HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Windows` | DLLs carregadas em todo processo (descontinuado) |

> **Rode no Visual Studio**
> Este exemplo lista as entradas de auto-inicio do registro para o usuario atual.
>
> ```cpp
> #include <windows.h>
> #include <stdio.h>
>
> void listarAutoStart(HKEY hRoot, const wchar_t* caminho) {
>     HKEY hKey;
>     if (RegOpenKeyExW(hRoot, caminho, 0, KEY_READ, &hKey) != ERROR_SUCCESS)
>         return;
>
>     wprintf(L"\n[%s\\%s]\n",
>         hRoot == HKEY_LOCAL_MACHINE ? L"HKLM" : L"HKCU", caminho);
>
>     DWORD indice = 0;
>     WCHAR nome[256], valor[1024];
>     DWORD tamNome, tamValor, tipo;
>
>     while (true) {
>         tamNome = 256; tamValor = sizeof(valor);
>         LONG res = RegEnumValueW(hKey, indice++, nome, &tamNome,
>             NULL, &tipo, (BYTE*)valor, &tamValor);
>         if (res != ERROR_SUCCESS) break;
>         wprintf(L"  %-30s => %s\n", nome, valor);
>     }
>
>     RegCloseKey(hKey);
> }
>
> int main() {
>     const wchar_t* caminho = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";
>     listarAutoStart(HKEY_LOCAL_MACHINE, caminho);
>     listarAutoStart(HKEY_CURRENT_USER, caminho);
>     return 0;
> }
> ```

---

## Desligamento

O processo de desligamento do Windows e tao importante quanto o boot - dados nao gravados precisam ser confirmados, servicos precisam ser notificados e o hardware precisa ser colocado em um estado seguro.

### Sequencia de desligamento

Quando um usuario inicia um desligamento:

1. **Csrss.exe notifica todos os processos GUI** para salvar seu estado e fechar.
2. **O timeout de desligamento** aguarda os processos fechar (configuravel, padrao 5 segundos).
3. **Processos que nao respondem** recebem `TerminateProcess`.
4. **Smss.exe notifica os subsistemas** para encerrar.
5. **O SCM para todos os servicos** em ordem de dependencia reversa.
6. **O kernel executa o desligamento final:**
   - Chama os handlers de desligamento registrados.
   - Esvazia o cache de arquivo sujo para o disco.
   - Desmonta os volumes.
   - Executa a fase de desligamento do hardware (via HAL/ACPI).

### Tipos de desligamento

| Tipo | Descricao |
|------|-----------|
| **Logoff** | Encerra a sessao do usuario, mantem o sistema rodando |
| **Shutdown** | Para o sistema operacional e desliga o hardware |
| **Restart** | Para o sistema operacional e reinicia |
| **Hibernate** | Salva o estado da RAM no disco e desliga |
| **Sleep** | Suspende o hardware em baixo consumo (S3/S1) |
| **Hybrid sleep** | Combina sleep e hibernate |
| **Fast Startup** | Hybrid shutdown seguido de boot usando o estado salvo |

---

## Hibernacao e Fast Startup

### Hibernacao

A **hibernacao** salva o conteudo completo da memoria RAM em um arquivo chamado `hiberfil.sys` na raiz do volume de sistema e desliga o computador. Na proxima inicializacao, o conteudo da memoria e restaurado do arquivo, retornando ao estado exato anterior ao desligamento.

Vantagens sobre desligamento normal:
- Retoma de forma muito mais rapida que boot completo.
- Nenhuma perda de trabalho em progresso.

O arquivo `hiberfil.sys` tem o mesmo tamanho que a RAM instalada (ou 75% dela com compressao).

### Fast Startup (Inicializacao Rapida)

O **Fast Startup** e uma variante de hibernacao introduzida no Windows 8 que acelera o processo de boot:

1. Ao "desligar", o Windows:
   - Faz logoff de todos os usuarios (sem salvar seu estado).
   - Coloca apenas o nucleo do OS (kernel + sessao 0) em hibernacao no `hiberfil.sys`.
2. Na proxima inicializacao, o OS Loader:
   - Verifica se existe um `hiberfil.sys` valido.
   - Restaura o estado do kernel da hibernacao (muito mais rapido que inicializar do zero).
   - O usuario faz login normalmente.

O Fast Startup reduz tipicamente o tempo de boot em 50-70% em SSDs modernos.

**Limitacoes do Fast Startup:**

- Drivers nao sao reinicializados entre boots - pode causar problemas se hardware foi adicionado/removido.
- Dualboot com Linux pode ter problemas de acesso ao volume Windows (que permanece "montado" do ponto de vista do NTFS).
- O evento de boot completo (`EventID 12` em System) nao e registrado para boots com Fast Startup.

---

## Windows Recovery Environment (WinRE)

O **Windows Recovery Environment** (WinRE, anteriormente conhecido como Windows PE) e uma versao minima do Windows usada para recuperacao do sistema quando o Windows principal nao pode inicializar.

O WinRE esta armazenado em uma particao de recuperacao dedicada (tipicamente com a flag `Recovery` no GPT). Ele pode ser acessado:

- Automaticamente quando o Windows detecta falhas de boot repetidas.
- Pressionando F8/F11 durante o boot.
- A partir de um USB de recuperacao.

O WinRE oferece as seguintes ferramentas de recuperacao:

| Ferramenta | Descricao |
|-----------|-----------|
| **Reiniciar** | Simplesmente reinicia o computador |
| **Restauracao do sistema** | Restaura para um ponto de restauracao anterior |
| **Recuperacao de imagem** | Restaura o Windows a partir de um backup de imagem |
| **Reparo de inicializacao** | Tenta reparar automaticamente problemas de boot |
| **Prompt de comando** | Acesso a linha de comando para diagnostico manual |
| **Configuracoes de inicializacao** | Modifica configuracoes de boot (Safe Mode, etc.) |
| **Desinstalar atualizacoes** | Remove as ultimas atualizacoes instaladas |

### Como o WinRE detecta falhas de boot

O Windows conta o numero de tentativas de boot malsucedidas. Se o contador atingir um limite (geralmente 3), o Windows Boot Manager redireciona automaticamente o boot para o WinRE.

O contador e armazenado em:

`HKLM\SYSTEM\CurrentControlSet\Control\Session Manager\BootExecute`

E redefinido para zero quando um boot e bem-sucedido (usuario faz login).

---

## Modo de seguranca (Safe Mode)

O **modo de seguranca** e uma opcao de boot especial que inicializa o Windows com um conjunto minimo de drivers e servicos. E usado para diagnosticar problemas causados por drivers ou software defeituosos.

Para entrar em modo de seguranca, o usuario pode:

- Pressionar F8 durante o boot (em sistemas BIOS legados).
- Nas configuracoes de inicializacao do WinRE.
- Via `bcdedit /set {default} safeboot minimal`.

### Carregamento de drivers em modo de seguranca

Em modo de seguranca, o Windows carrega apenas os drivers essenciais:

- Drivers de boot criticos (storage, rede basica se Safe Mode with Networking).
- Drivers de video VGA minimos (sem driver grafico completo).
- Nenhum servico de terceiros e iniciado.

A lista de drivers carregados em modo de seguranca e controlada pelo valor `safeboot` na chave do driver no registro:

- Drivers com `safeboot = minimal`: Carregados no modo de seguranca basico.
- Drivers com `safeboot = network`: Carregados apenas no modo de seguranca com rede.
- Drivers sem `safeboot`: Nao carregados em nenhum modo de seguranca.

### Programas de usuario cientes do modo de seguranca

Os aplicativos podem detectar se o sistema esta em modo de seguranca usando a API `GetSystemMetrics(SM_CLEANBOOT)`, que retorna:

- `0`: Boot normal.
- `1`: Safe Mode.
- `2`: Safe Mode with Networking.

```cpp
#include <windows.h>
#include <stdio.h>

int main() {
    int modoSeguro = GetSystemMetrics(SM_CLEANBOOT);
    switch (modoSeguro) {
    case 0: printf("Boot normal\n"); break;
    case 1: printf("Modo de Seguranca (sem rede)\n"); break;
    case 2: printf("Modo de Seguranca com Rede\n"); break;
    }
    return 0;
}
```

### Arquivo de status de boot

O Windows mantem um **arquivo de status de boot** (`%SystemRoot%\bootstat.dat`) que registra informacoes sobre o ultimo boot:

- Se o boot foi bem-sucedido.
- O tipo de boot (normal, hibernacao, safe mode).
- O timestamp do boot.

O SCM e o Winlogon usam esse arquivo para determinar se o sistema foi inicializado com sucesso e se a configuracao "Last Known Good" deve ser atualizada.

---

## Conclusao

Este capitulo examinou o complexo processo de inicializacao e desligamento do Windows.

O processo de boot moderno comeca com o firmware UEFI verificando a integridade de cada componente carregado via Secure Boot. O Windows Boot Manager le a configuracao de boot da BCD e carrega o OS Loader. O OS Loader, por sua vez, carrega o kernel, a HAL e os drivers de boot essenciais, verificando as assinaturas digitais de cada um.

Se o Hyper-V e VBS estiverem habilitados, o loader tambem inicializa o hypervisor e o Secure Kernel antes de transferir o controle ao kernel Windows. Essa sequencia garante que a raiz de confianca criptografica seja estabelecida cedo no processo de boot.

Apos o kernel assumir o controle, ele inicializa sistematicamente todos os subsistemas do executive, criando o processo System e eventualmente o primeiro processo usuario, Smss.exe. O Smss cria o subsistema Win32 (Csrss), inicializa a sessao de usuario e inicia o processo de inicializacao (Wininit), que por sua vez inicia os servicos criticos como Lsass e o SCM.

O processo de desligamento e uma versao reversa desse processo, garantindo que todos os dados sujos sejam gravados no disco, que os servicos sejam parados em ordem e que o hardware seja colocado em um estado seguro.

O Fast Startup e a hibernacao otimizam os ciclos de boot/desligamento armazenando o estado do kernel no disco, permitindo inicializacoes muito mais rapidas sem comprometer a seguranca ou a integridade dos dados.

Com isso, concluimos a traducao do Windows Internals Part 2. Este livro fornece uma compreensao profunda de como o Windows funciona internamente - do hardware ao usuario, do boot ao desligamento. Com esse conhecimento, voce estara bem preparado para trabalhar com desenvolvimento de sistemas Windows em C++, diagnosticar problemas complexos de sistema e desenvolver software de seguranca sofisticado.
