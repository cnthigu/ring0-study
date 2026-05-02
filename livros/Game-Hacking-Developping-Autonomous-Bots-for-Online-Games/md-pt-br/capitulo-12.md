# Capitulo 12 - Permanecendo escondido

> Titulo original: *Staying Hidden*

> Navegacao: [Anterior](capitulo-11.md) | [Indice](README.md)

## Topicos

- Anti-cheat suites em destaque (PunkBuster, ESEA, VAC,
  GameGuard, Warden)
- Reduzindo e mascarando o footprint do bot
- Detectando debuggers e tecnicas anti-debugging
- Vencendo SBD, screenshots, binary validation, rootkit e
  heuristics

## Abertura

Game hacking e jogo de gato e rato em constante evolucao: hackers
e devs tentam subverter um ao outro. Enquanto houver bots, as
empresas vao buscar barrar avancos e banir botters. Em vez de
deixar os games inerentemente mais dificeis de hackear, as
empresas focam em *deteccao*.

As maiores tem suites sofisticadas chamadas *anti-cheat software*.
Vamos cobrir as mais comuns e depois ensinar formas eficazes de
escapar.

## Anti-cheat software em destaque

As suites mais conhecidas usam metodos parecidos com antivirus:
escaneiam por bots e marcam como ameaca. Algumas sao dinamicas:
mudam comportamento conforme o game protegido. Os devs de
anti-cheat ainda monitoram bypass software e patcham. Sempre
pesquise a fundo a anti-cheat que voce vai enfrentar.

Quando uma suite detecta, *flegga* a conta para ban. A cada
algumas semanas, as empresas executam *ban waves*. Bans em onda
sao mais lucrativos: o botter joga, fica vinculado e tende a
comprar nova conta apos banido depois de semanas, em vez de horas.

Vamos focar em cinco: *PunkBuster*, *ESEA Anti-Cheat*, *Valve
Anti-Cheat (VAC)*, *GameGuard* e *Warden*.

### PunkBuster

PunkBuster, da Even Balance Inc., e o pioneiro. Comum em FPS:
Medal of Honor, Far Cry 3, varios Battlefield. Usa varios metodos;
os mais formidaveis sao *signature-based detection (SBD)*,
*screenshots* e *hash validation*. Tambem famoso por *hardware
bans* (banem o computador, salvando fingerprint dos seriais e
bloqueando logins de qualquer maquina que case).

#### Signature-based detection

PunkBuster escaneia memoria de todos os processos do sistema
quando um game protegido roda, buscando byte patterns unicos de
cheat software (signatures). Casou? Banido. Roda em user mode com
`NtQueryVirtualMemory()`, as vezes a partir de varios processos
ocultos.

SBD e cego ao contexto e tem uma falha fatal: *false positives*.
Em 23 de marco de 2008, hackers spamaram chats publicos com uma
string que PunkBuster identificava como signature de bot. SBD
escaneia memoria sem entender contexto, entao todos os players
nesses chats foram flagrados. Milhares de players honestos
banidos sem motivo.

Em novembro de 2013, milhares de players de Battlefield 4 foram
banidos por engano apos PunkBuster receber uma signature ruim.
Restauraram as contas, mas mostra a agressividade. Apos esses
casos, PunkBuster reduziu falsos positivos checando signatures
apenas em offsets binarios pre-definidos.

#### Screenshots

PunkBuster periodicamente tira screenshots do player e manda para
o server. Detec metodo fraco; comunidade especula que serve para
embasar bans contestados.

#### Hash validation

Cria hashes criptograficos das executaveis do game na maquina e
compara com hashes do server. Bate? OK. Nao bate? Flag. So nos
binarios em disco, nao em memoria.

### ESEA Anti-Cheat

Usado pela E-Sports Entertainment Association, principalmente na
liga de Counter-Strike: Global Offensive. Diferente do PunkBuster,
quase nao gera falsos positivos e e altamente eficaz.

Recursos parecidos com PunkBuster, com uma diferenca importante:
o SBD roda em *kernel-mode driver*, usando
`MmGetPhysicalMemoryRanges()`, `ZwOpenSection()` e
`ZwMapViewOfSection()`. Isso deixa o sistema quase imune a *memory
spoofing* (forma comum de derrotar SBD), porque hookar funcoes
chamadas de driver e bem mais dificil.

### VAC

Valve Anti-Cheat. Aplicado aos games da Valve e a varios outros na
plataforma Steam. Usa SBD e hash validation parecidos com
PunkBuster, alem de *DNS cache scans* e *binary validation*.

#### DNS cache scans

DNS converte nomes de dominio em IPs; o cache fica na maquina.
Quando o SBD detecta cheat, VAC escaneia o DNS cache do player
buscando dominios associados a sites de cheat. Nao se sabe se um
DNS cache positivo e *necessario* para banir, ou se so reforca o
flag do SBD.

> NOTA: para ver seu DNS cache, `ipconfig /displaydns` no prompt.
> O VAC olha tudo isso.

#### Binary validation

Para impedir adulteracao em memoria, VAC compara hashes do codigo
em memoria com os do binario em disco. Detecta IAT hooks, jump
hooks e code hooking. Mismatch? Flag.

Eficaz, mas a primeira implementacao falhou: em julho de 2010,
banou 12000 players de Call of Duty. O modulo nao considerou um
update do Steam: os bytes em memoria nao batiam com os do binario
recem-atualizado.

#### Falsos positivos

Versao inicial banou players por "memoria com defeito" e por
usarem Cedega (rodava games Windows em Linux). Em abril de 2004,
banou alguns milhares por glitch server-side. Em junho de 2011 e
fevereiro de 2014, banimentos em massa em Team Fortress 2 e
Counter-Strike por bugs nao revelados. Como PunkBuster, e bem
agressivo.

### GameGuard

Da INCA Internet, usado em MMORPGs (Lineage II, Cabal Online,
Ragnarok Online). Alem de SBD moderado, usa *rootkits* para
*proativamente impedir* cheat software de rodar.

#### Rootkit em user mode

Hookea funcoes da Windows API no entry point mais baixo
(geralmente em funcoes nao documentadas de `ntdll.dll`,
`user32.dll`, `kernel32.dll`). Resumo do que bloqueia:

- `NtOpenProcess()`: bloqueia `OpenProcess()` no game.
- `NtProtectVirtualMemory()`: bloqueia `VirtualProtect()` /
  `VirtualProtectEx()` no game.
- `NtReadVirtualMemory()` e `NtWriteVirtualMemory()`: bloqueiam
  `ReadProcessMemory()` / `WriteProcessMemory()` no game.
- `NtSuspendProcess()` e `NtSuspendThread()`: bloqueiam tentativas
  de suspender o GameGuard.
- `NtTerminateProcess()` e `NtTerminateThread()`: bloqueiam
  tentativas de matar o GameGuard.
- `PostMessage()`, `SendMessage()` e `SendInput()`: bloqueiam input
  programatico no game.
- `SetWindowsHookEx()`: impede hooks globais de mouse/teclado.
- `CreateProcessInternal()`: detecta e hookea novos processos
  automaticamente.
- `GetProcAddress()`, `LoadLibraryEx()` e `MapViewOfFileEx()`:
  impedem injecao de DLLs no game ou no GameGuard.

#### Rootkit em kernel mode

Tambem ha rootkit driver-based para barrar bots em kernel. Mesmas
capacidades, hookando `ZwProtectVirtualMemory()`,
`ZwReadVirtualMemory()`, `ZwWriteVirtualMemory()`, `SendInput()` e
afins.

### Warden

Da Blizzard, exclusivo para games dela. De longe o mais avancado.
Dificil dizer exatamente o que faz, porque baixa codigo dinamico em
runtime (shellcode compilado) com dois papeis tipicos:

- Detectar bots.
- Mandar *heartbeat* periodico para o server. O valor enviado nao
  e pre-definido, e sim gerado por algum subset do codigo de
  deteccao.

Se o heartbeat falha ou vem errado, o server sabe que o Warden foi
desligado/adulterado. Voce nao consegue desligar a deteccao
mantendo o heartbeat. Pior: voce nao sabe do que esta se
escondendo, e mesmo escapando hoje, amanha o codigo muda.

> ### Boxe: o halting problem
>
> Um bot que conseguisse desligar o codigo de deteccao do Warden
> mantendo o heartbeat resolveria o *halting problem*, que Alan
> Turing provou impossivel em 1936. O halting problem e
> determinar, com algoritmo generico, se um programa termina ou
> roda para sempre. Como Warden faz duas tarefas no mesmo
> shellcode, escrever algoritmo generico que desabilite uma e
> mantenha a outra e variante do halting problem.

## Gerenciando o footprint do bot

Se voce vai distribuir bots, ira encontrar pelo menos uma das
suites e precisara escapar. A dificuldade vai de trivial a
extremamente dura, dependendo do *footprint*.

Footprint e quantas caracteristicas unicas e detectaveis seu bot
tem. Bot que hookea 100 funcoes e mais facil de detectar que um
que hookea 10: mais mudancas de codigo, mais coisa para a suite
encontrar. Tambem entram detalhes da UI: dialog boxes com titulos
especificos viram alvo. Mesmo raciocinio para nomes de processo
e arquivos.

### Minimizando o footprint

Se o bot depende muito de hooks, evite hookar codigo do game e
foque em Windows API. Hookar Windows API e bem comum (incluindo
software legitimo); a suite nao pode presumir bot.

Se o bot tem UI bem definida, mascare: tire as strings das
barras/botoes e mostre imagens com texto. Para nomes de processo
ou arquivo, use nomes genericos e copie o bot para um diretorio
randomizado a cada execucao.

### Mascarando o footprint

Minimizar e o ideal, mas voce tambem pode *ofuscar* o bot. Boa
ofuscacao impede tanto devs de anti-cheat de entender quanto
outros devs de bot copiarem suas funcionalidades, e dificulta
crackear se voce vende.

Tipo comum: *packing*. Voce cifra o executavel e o esconde dentro
de outro. Quando o container roda, o packed e descriptografado e
executado em memoria. Analise estatica vira impossivel; debug fica
muito mais dificil. Packers comuns: UPX, Armadillo, Themida,
ASPack.

### Ensinando o bot a detectar debuggers

Mesmo packed, se um analista debuga, descobre. Bots usam tecnicas
*anti-debugging* para mudar comportamento ao detectar debugger.

#### CheckRemoteDebuggerPresent()

```cpp
bool IsRemoteDebuggerPresent() {
    BOOL dbg = false;
    CheckRemoteDebuggerPresent(GetCurrentProcess(), &dbg);
    return dbg;
}
```

Direto e simples; tambem facil de o debugger evadir.

#### Checando interrupt handlers

Interrupts sao sinais que o processador manda para acionar handler
no kernel. Sao normalmente gerados por hardware, mas software pode
gerar com `INT`. O kernel deixa interrupts `0x2D` e `0x03`
acionarem user-mode interrupt handlers (exception handlers).

Quando um debugger seta breakpoint numa instrucao, troca a
instrucao por um breakpoint (`INT 0x03`). Quando esse INT executa,
o debugger e notificado via exception handler, lida, repoe o
codigo original e segue. Diante de interrupt nao reconhecido,
alguns debuggers passam silencioso, sem disparar outros handlers.

Detecte gerando interrupts dentro de exception handlers (Listagem
12-1):

```cpp
inline bool Has2DBreakpointHandler() {
    __try { __asm INT 0x2D }
    __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
    return true;
}

inline bool Has03BreakpointHandler() {
    __try { __asm INT 0x03 }
    __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
    return true;
}
```

Em execucao normal, os interrupts disparam os exception handlers a
volta. Sob debug, alguns debuggers interceptam silencioso, e os
handlers nao disparam: indica debugger.

#### Checando hardware breakpoints

Debuggers podem usar os debug registers do processador (hardware
breakpoints), gravando o endereco da instrucao em um dos quatro
registers. Quando o endereco executa, o debugger e notificado.
Detectar:

```cpp
bool HasHardwareBreakpoints() {
    CONTEXT ctx = {0};
    ctx.ContextFlags = CONTEXT_DEBUG_REGISTERS;
    auto hThread = GetCurrentThread();
    if (GetThreadContext(hThread, &ctx) == 0)
        return false;
    return (ctx.Dr0 != 0 || ctx.Dr1 != 0 ||
            ctx.Dr2 != 0 || ctx.Dr3 != 0);
}
```

#### Imprimindo debug strings

`OutputDebugString()` imprime log na console do debugger. Sem
debugger, retorna com error code. Com debugger, retorna sem error.
Logo:

```cpp
inline bool CanCallOutputDebugString() {
    SetLastError(0);
    OutputDebugStringA("test");
    return (GetLastError() == 0);
}
```

Direto, mas tambem facil de evadir.

#### Checando handlers de DBG_RIPEXCEPTION

Debuggers tem handlers que pegam exceptions com codigo
`DBG_RIPEXCEPTION` cegamente. Detectar (igual a Listagem 12-1):

```cpp
#define DBG_RIPEXCEPTION 0x40010007

inline bool hasRIPExceptionHandler() {
    __try { RaiseException(DBG_RIPEXCEPTION, 0, 0, 0); }
    __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
    return true;
}
```

#### Cronometrando rotinas criticas

Se o analista debuga, ele coloca breakpoint e single-stepa em
codigo critico. Single-step demora bem mais. Detecte tempos:

```cpp
auto startTime = GetTickCount();
protectMemory<>(...);
if (GetTickCount() - startTime >= 100)
    debuggerDetectedGoConfuseIt();
```

Se a protecao de memoria leva mais de 100ms, e debugger.

#### Detectando debug drivers

Alguns debuggers carregam kernel-mode drivers. Tente abrir handle
para os device names conhecidos:

```cpp
bool DebuggerDriversPresent() {
    const char drivers[9][20] = {
        "\\\\.\\EXTREM",      "\\\\.\\ICEEXT",
        "\\\\.\\NDBGMSG.VXD", "\\\\.\\RING0",
        "\\\\.\\SIWVID",      "\\\\.\\SYSER",
        "\\\\.\\TRW",         "\\\\.\\SYSERBOOT",
        "\0"
    };
    for (int i = 0; drivers[i][0] != '\0'; i++) {
        auto h = CreateFileA(drivers[i], 0, 0, 0, OPEN_EXISTING, 0, 0);
        if (h != INVALID_HANDLE_VALUE) {
            CloseHandle(h);
            return true;
        }
    }
    return false;
}
```

Diferente das tecnicas anteriores, abrir handle nao garante que o
debugger esta atachado *ao seu bot* especificamente.

### Tecnicas anti-debugging

Detectado o debugger, voce pode obfuscar o control flow.

#### Crashar o debugger

```cpp
OutputDebugString("%s%s%s%s");
```

A string tem format specifiers e o OllyDbg v1.10 passa para
`printf()` sem extras: o debugger crasha. Funciona so contra
OllyDbg.

#### Loop infinito inevitavel

Sobrecarregue o sistema ate o analista fechar tudo:

```cpp
void SelfDestruct() {
    std::vector<char*> explosion;
    while (true)
        explosion.push_back(new char[10000]);
}
```

Adiciona elementos ate estourar a memoria.

#### Stack overflow

Para confundir mais, encadeie funcoes que estouram a stack
indiretamente:

```cpp
#include <random>

typedef void (*_recurse)();

void recurse1(); void recurse2();
void recurse3(); void recurse4();
void recurse5();

_recurse recfuncs[5] = {
    &recurse1, &recurse2, &recurse3,
    &recurse4, &recurse5
};

void recurse1() { recfuncs[rand() % 5](); }
void recurse2() { recfuncs[(rand() % 3) + 2](); }
void recurse3() {
    if (rand() % 100 < 50) recurse1();
    else                   recfuncs[(rand() % 3) + 1]();
}
void recurse4() { recfuncs[rand() % 2](); }
void recurse5() {
    for (int i = 0; i < 100; i++)
        if (rand() % 50 == 1)
            recfuncs[i % 5]();
    recurse5();
}
// chamar qualquer uma dispara o overflow
```

Recursoes aleatorias e infinitas estouram a call stack.
Indiretamente, dificulta para o analista pausar e examinar.

#### Causar BSOD

Mais agressivo: setar o processo como *critical* e dar `exit()`,
forcando Blue Screen of Death.

```cpp
void BSODBaby() {
    typedef long (WINAPI *RtlSetProcessIsCritical)(
        BOOLEAN New, BOOLEAN *Old, BOOLEAN NeedScb);

    auto ntdll = LoadLibraryA("ntdll.dll");
    if (ntdll) {
        auto SetProcessIsCritical = (RtlSetProcessIsCritical)
            GetProcAddress(ntdll, "RtlSetProcessIsCritical");
        if (SetProcessIsCritical)
            SetProcessIsCritical(1, 0, 0);
    }
}

BSODBaby();
exit(1);
```

Modo "do mal":

```cpp
BSODBaby();
OutputDebugString("%s%s%s%s");
recurse1();
exit(1);
```

Dispara BSOD, crasha OllyDbg v1.10, estoura a stack e finaliza.
Se uma das tecnicas falha, ainda restam as outras.

## Vencendo signature-based detection

Mesmo com obfuscacao, SBD nao cai facil. Os engenheiros que
analisam bots e escrevem signatures sao bons; ofuscacao incomoda
marginalmente.

Para realmente derrotar, voce precisa subverter o codigo de
deteccao. Exige saber exatamente como o SBD opera. PunkBuster, por
exemplo, usa `NtQueryVirtualMemory()`. Voce pode injetar codigo
nos processos do PunkBuster com hook nessa funcao:

```cpp
NTSTATUS onNtQueryVirtualMemory(
    HANDLE  process,    PVOID  baseAddress,
    MEMORY_INFORMATION_CLASS memoryInformationClass,
    PVOID   buffer,     ULONG  numberOfBytes,
    PULONG  numberOfBytesRead)
{
    // se o scan e na hook DLL, esconda
    if ((process == INVALID_HANDLE_VALUE ||
         process == GetCurrentProcess()) &&
        baseAddress >= MY_HOOK_DLL_BASE &&
        baseAddress <= MY_HOOK_DLL_BASE_PLUS_SIZE)
        return STATUS_ACCESS_DENIED;                 // (1)

    auto ret = origNtQueryVirtualMemory(
        process, baseAddress,
        memoryInformationClass,
        buffer,  numberOfBytes, numberOfBytesRead);

    // se o scan e no bot, zera o buffer
    if (GetProcessId(process) == MY_BOT_PROCESS)
        ZeroMemory(buffer, numberOfBytesRead);       // (2)

    return ret;
}
```

(1) retorna `STATUS_ACCESS_DENIED` quando o scan tenta ler memoria
da hook DLL. (2) zera o buffer quando o scan e no processo do bot.
Mostrei dois caminhos; voce pode tambem trocar o buffer por
sequencia random.

So funciona para SBD em user mode (PunkBuster, VAC). SBD em driver
(ESEA) ou imprevisivel (Warden) e bem mais cabuloso.

Nesses casos, elimine signatures unicos. Se voce distribui para
mais de uma duzia de pessoas, dificil eliminar tudo. Para
desorientar:

- Compile o bot com compiladores diferentes.
- Mude opcoes de otimizacao.
- Alterne entre `__fastcall` e `__cdecl`.
- Empacote (pack) com packers diferentes.
- Alterne static e dynamic linking de runtime libs.

Cada combinacao gera assembly diferente, mas ha limite de
incarnacoes. Eventualmente as suites cobrem tudo.

Fora obfuscacao e mutacao, nao ha muitas alternativas para SBD
avancado. Voce pode implementar o bot num driver ou criar rootkit
kernel-mode para esconder, mas nao e infalivel.

> NOTA: implementar bot em driver ou rootkit foge do escopo. Cada
> assunto e livro inteiro. Recomendo "The Rootkit Arsenal: Escape
> and Evasion in The Dark Corners of The System" de Bill Blunden
> (Jones & Bartlett Learning, 2009).

Alguns hackers cobrem todas as bases (todas as funcoes de leitura
de memoria, toda a filesystem API), mas ainda caem com Warden.
Recomendacao do autor: fique longe de Warden e Blizzard.

## Vencendo screenshots

Usaram screenshots? Sorte sua: e facil. So nao deixe o bot ser
visto.

Mantenha UI minima e nao mude nada visivelmente no game client. Se
seu bot precisa de HUD ou UI distinta, intercepte o codigo do
screenshot e esconda enquanto a foto e tirada.

Em algumas versoes do PunkBuster, `GetSystemTimeAsFileTime()` e
chamada logo antes do screenshot. Hookando:

```cpp
void onGetSystemTimeAsFileTime(LPFILETIME systemTimeAsFileTime) {
    myBot->hideUI(2000); // esconde UI por 2 segundos
    origGetSystemTimeAsFileTime(systemTimeAsFileTime);
}
```

Use as tecnicas de hooking do Capitulo 8, escreva `hideUI()` e
chame antes da execucao continuar.

## Vencendo binary validation

Solucao mais simples: nao colocar hooks dentro de binarios do
game. Jump hooks e IAT hooks em Windows API sao comuns; prefira
sempre. Se inevitavel, intercepte o scan e *spoofie* o resultado.

Como SBD, binary validation costuma usar `NtQueryVirtualMemory()`.
Comece hookando:

```cpp
NTSTATUS onNtQueryVirtualMemory(
    HANDLE  process, PVOID baseAddress,
    MEMORY_INFORMATION_CLASS memoryInformationClass,
    PVOID   buffer,  ULONG numberOfBytes,
    PULONG  numberOfBytesRead)
{
    auto ret = origNtQueryVirtualMemory(
        process, baseAddress,
        memoryInformationClass,
        buffer,  numberOfBytes, numberOfBytesRead);
    // codigo malandro aqui
    return ret;
}
```

Dentro do hook, monitore se o scan toca em memoria modificada.
Listagem 12-2 (assume um unico hook, com `HOOK_*` definidos):

```cpp
bool currentProcess =
    process == INVALID_HANDLE_VALUE ||
    process == GetCurrentProcess();

auto endAddress = baseAddress + numberOfBytesRead - 1;
bool containsHook =
    (HOOK_START_ADDRESS >= baseAddress &&
     HOOK_START_ADDRESS <= endAddress) ||
    (HOOK_END_ADDRESS   >= baseAddress &&
     HOOK_END_ADDRESS   <= endAddress);

if (currentProcess && containsHook) {       // (1)
    // esconder o hook
}
```

> Listagem 12-2: detectando scan na regiao hookada.

Quando ambos sao `true` (1), atualiza o buffer para o codigo
original. Voce precisa saber onde o codigo modificado esta dentro
do bloco escaneado (o bloco pode cobrir so um pedaco):

```cpp
int readStart, writeStart;
if (HOOK_START_ADDRESS >= baseAddress) {
    readStart  = 0;
    writeStart = HOOK_START_ADDRESS - baseAddress;
} else {
    readStart  = baseAddress - HOOK_START_ADDRESS;
    writeStart = baseAddress;
}

int readEnd;
if (HOOK_END_ADDRESS <= endAddress)
    readEnd = HOOK_LENGTH - readStart - 1;
else
    readEnd = endAddress - HOOK_START_ADDRESS;
```

Spoof:

```cpp
char* replaceBuffer = (char*)buffer;
for (; readStart <= readEnd; readStart++, writeStart++)
    replaceBuffer[writeStart] = HOOK_ORIG_DATA[readStart];
```

Versao completa:

```cpp
NTSTATUS onNtQueryVirtualMemory(
    HANDLE  process, PVOID baseAddress,
    MEMORY_INFORMATION_CLASS memoryInformationClass,
    PVOID   buffer,  ULONG numberOfBytes,
    PULONG  numberOfBytesRead)
{
    auto ret = origNtQueryVirtualMemory(
        process, baseAddress,
        memoryInformationClass,
        buffer,  numberOfBytes, numberOfBytesRead);

    bool currentProcess =
        process == INVALID_HANDLE_VALUE ||
        process == GetCurrentProcess();

    auto endAddress = baseAddress + numberOfBytesRead - 1;
    bool containsHook =
        (HOOK_START_ADDRESS >= baseAddress &&
         HOOK_START_ADDRESS <= endAddress) ||
        (HOOK_END_ADDRESS   >= baseAddress &&
         HOOK_END_ADDRESS   <= endAddress);

    if (currentProcess && containsHook) {
        int readStart, writeStart;
        if (HOOK_START_ADDRESS >= baseAddress) {
            readStart  = 0;
            writeStart = HOOK_START_ADDRESS - baseAddress;
        } else {
            readStart  = baseAddress - HOOK_START_ADDRESS;
            writeStart = baseAddress;
        }

        int readEnd;
        if (HOOK_END_ADDRESS <= endAddress)
            readEnd = HOOK_LENGTH - readStart - 1;
        else
            readEnd = endAddress - HOOK_START_ADDRESS;

        char* replaceBuffer = (char*)buffer;
        for (; readStart <= readEnd; readStart++, writeStart++)
            replaceBuffer[writeStart] = HOOK_ORIG_DATA[readStart];
    }

    return ret;
}
```

Para multiplos hooks, generalize para rastrear todas as regioes
modificadas.

## Vencendo um anti-cheat rootkit

GameGuard e similares vem com user-mode rootkits que detectam *e
proativamente impedem* execucao de bot. Em vez de pensar fora da
caixa, *copie a caixa* e trabalhe dentro da copia.

Por exemplo, para escrever memoria voce chama
`WriteProcessMemory()` (em `kernel32.dll`), que internamente chama
`NtWriteVirtualMemory()` (em `ntdll.dll`). GameGuard hookea
`ntdll.NtWriteVirtualMemory()`. Mas se `NtWriteVirtualMemory()`
vier de, digamos, `ntdll_copy.dll`, GameGuard nao a hookea.

```cpp
copyFile("ntdll.dll", "ntdll_copy.dll");
auto module = LoadLibrary("ntdll_copy.dll");

typedef NTSTATUS (WINAPI* _NtWriteVirtualMemory)(
    HANDLE, PVOID, PVOID, ULONG, PULONG);

auto myWriteVirtualMemory = (_NtWriteVirtualMemory)
    GetProcAddress(module, "NtWriteVirtualMemory");

myWriteVirtualMemory(process, address, data, length, &writtenlength);
```

Importa `NtWriteVirtualMemory()` da copia com nome
`myWriteVirtualMemory()`. Mesmo codigo, lib diferente.

So funciona se voce chamar a funcao no entry point mais baixo. Se
copiar `kernel32.dll` e importar `WriteProcessMemory()`, ainda
caira no `ntdll.NtWriteVirtualMemory()` hookado.

## Vencendo heuristics

Alem dos mecanismos client-side, empresas usam *heuristics
server-side*, monitorando comportamento. Aprendem a distinguir
humano de bot via machine learning. As decisoes sao internas e
incompreensiveis para humanos: dificil saber o que dispara.

Voce nao precisa entender; basta *agir como humano*. Comportamentos
distintos:

- **Intervalos entre acoes**: bots agem rapido demais ou em
  intervalos fixos. Tenha cooldown razoavel e randomizacao.
- **Repeticao de path**: lista de waypoints exatos pixel a pixel
  denuncia. Humanos andam imprevisivel e visitam pontos extras.
  Ande para localizacoes random dentro de raio do alvo, e
  randomize a ordem.
- **Jogo irreal**: bots botam centenas de horas seguidas.
  Recomende limites (oito horas e ja muito), e advirta que sete
  dias direto na mesma rotina dispara alarmes.
- **Acuracia perfeita**: bots fazem mil headshots seguidos sem
  desperdicar tiro, ou cada skill shot perfeito. Humano nao faz.
  Erre de proposito as vezes.

Em geral, use senso comum: nao faca o bot fazer o que humano nao
faz, nao deixe ele fazer a mesma coisa por tempo demais.

## Fechando

Hackers e devs vivem em batalha de inteligencia constante. Hackers
acham caminhos para subverter; devs aprimoram deteccao. Com
determinacao, o que tem aqui ajuda a derrotar qualquer anti-cheat
que voce encontre.
