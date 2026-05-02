# Capitulo 7 - Code Injection

> Titulo original: *Code Injection*

> Navegacao: [Anterior](capitulo-06.md) | [Indice](README.md) | [Proximo](capitulo-08.md)

## Topicos

- Construir code caves em assembly e shellcode
- Thread injection e thread hijacking
- Injetar DLLs com `LoadLibrary` para controle total
- Acesso direto a memoria a partir de DLL injetada
- Bypass de ASLR de dentro do processo

## Abertura

Imagine entrar no escritorio de uma empresa de games, sentar e
comecar a adicionar codigo no client. Imagine fazer isso em
qualquer game, quando quiser, com qualquer funcionalidade. Quase
todo gamer tem ideias para melhorar um game, mas pra eles isso e
sonho. Voce sabe que sonhos sao para serem realizados, e agora que
ja conhece um pouco de memoria, esta pronto para jogar as regras
fora. Com code injection, na pratica voce vira tao poderoso quanto
qualquer dev do game.

*Code injection* e uma forma de forcar qualquer processo a
executar codigo estrangeiro dentro do proprio espaco de memoria e
contexto de execucao. Vimos uma pincelada em "Bypassing ASLR in
Production" no Capitulo 6, com `CreateRemoteThread()`, mas aquele
exemplo so arranhou a superficie. Na primeira parte deste capitulo
voce vai aprender a criar code caves, injetar threads e hijack
thread execution para forcar games a executarem snippets de
assembly. Na segunda parte, vamos injetar binarios completos
direto nos games, fazendo-os executar programas inteiros que voce
escreveu.

## Injetando code caves com thread injection

O primeiro passo para injetar codigo em outro processo e escrever
codigo assembly position-agnostic, conhecido como *shellcode*, na
forma de um array de bytes. Voce escreve shellcode em processos
remotos para formar *code caves*, que servem de entry point para
uma nova thread que voce quer que o game execute. Apos criar uma
code cave, da para executar via *thread injection* ou *thread
hijacking*. Vamos ver thread injection nesta secao e thread
hijacking depois.

> Codigo de exemplo em
> `GameHackingExamples/Chapter7_CodeInjection`. Abra
> `main-codeInjection.cpp` para acompanhar enquanto construimos uma
> versao simplificada de `injectCodeUsingThreadInjection()`.

### Construindo uma code cave em assembly

No Capitulo 6, fizemos thread injection para chamar
`GetModuleHandle()` via `CreateRemoteThread()` e obter um handle.
Naquele caso, `GetModuleHandle()` agiu como code cave: ja tinha o
formato adequado para servir de entry point de uma thread nova.
Mas thread injection nem sempre e tao facil.

Imagine que voce quer chamar a seguinte funcao do game a partir de
um bot externo:

```cpp
DWORD __cdecl someFunction(int times, const char* string);
```

Algumas coisas dificultam: tem dois parametros, entao voce precisa
montar uma cave que prepare a stack e faca a call.
`CreateRemoteThread()` deixa passar um argumento para a cave (via
`ESP`), mas o outro precisa ser hardcoded. Hardcodar o primeiro,
`times`, e o mais simples. Tambem precisa garantir que a cave
limpa a stack.

> NOTA: lembre que ao bypass-ar o ASLR no Capitulo 6,
> `CreateRemoteThread()` foi usado para iniciar uma thread executando
> codigo arbitrario num endereco e passando um unico parametro. Por
> isso esses exemplos passam um parametro pela stack.

A code cave para injetar a chamada a `someFunction` ficaria mais ou
menos:

```nasm
PUSH DWORD PTR:[ESP+0x4]   ; pega o segundo arg da stack
PUSH times
CALL someFunction
ADD ESP, 0x8
RETN
```

Quase perfeito, mas da para simplificar. `CALL` aceita ou um
registrador com endereco absoluto ou um inteiro immediate como
offset relativo ao return address. Para evitar calculo de offset,
use registrador. A Listagem 7-1 mostra a versao position-agnostic.

```nasm
PUSH DWORD PTR:[ESP+0x4]   ; pega o segundo arg da stack
PUSH times
MOV EAX, someFunction
CALL EAX
ADD ESP, 0x8
RETN
```

> Listagem 7-1: code cave para chamar `someFunction`.

Como o caller sabe que a funcao chamada vai sobrescrever `EAX` com
o return value, da para usar `EAX` para guardar o endereco
absoluto de `someFunction`.

### Traduzindo o assembly para shellcode

Code caves sao escritas na memoria de outro processo, entao nao da
para escrever direto em assembly. Voce escreve byte por byte. Nao
ha jeito padrao de saber quais bytes correspondem a cada
instrucao, mas tem alguns truques. Meu predileto: compilar um app
C++ vazio com o assembly numa funcao e usar OllyDbg para inspecionar
a funcao. Alternativa: abrir o OllyDbg em qualquer processo e
varrer o disassembly ate achar os bytes que precisa. Vai funcionar
porque code caves devem ser simples e usar instrucoes muito comuns.
Existem tabelas de opcodes na internet, mas costumam ser dificeis
de ler.

Sabendo os bytes, monte o shellcode em C++. A Listagem 7-2 mostra
o esqueleto correspondente a Listagem 7-1.

```cpp
BYTE codeCave[20] = {
    0xFF, 0x74, 0x24, 0x04,       // PUSH DWORD PTR:[ESP+0x4]
    0x68, 0x00, 0x00, 0x00, 0x00, // PUSH 0
    0xB8, 0x00, 0x00, 0x00, 0x00, // MOV EAX, 0x0
    0xFF, 0xD0,                   // CALL EAX
    0x83, 0xC4, 0x08,             // ADD ESP, 0x08
    0xC3                          // RETN
};
```

> Listagem 7-2: esqueleto de shellcode.

Os dois grupos de quatro bytes `0x00` sao placeholders para
`times` e o endereco de `someFunction`. Em runtime, voce preenche
com `memcpy()`:

```cpp
memcpy(&codeCave[5],  &times,             4);
memcpy(&codeCave[10], &addressOfSomeFunc, 4);
```

> Listagem 7-3: inserindo `times` e o endereco de `someFunction`.

`times` e um `int` de 4 bytes, e o endereco e 32 bits. Eles vao em
`codeCave[5..8]` e `codeCave[10..13]`, respectivamente.

### Escrevendo a code cave na memoria

Com o shellcode pronto, use `VirtualAllocEx()` e
`WriteProcessMemory()` para colocar dentro do processo alvo.
Listagem 7-4:

```cpp
int stringlen = strlen(string) + 1; // +1 para null terminator
int cavelen   = sizeof(codeCave);
int fulllen   = stringlen + cavelen;                          // (1)

auto remoteString =                                           // (2)
    VirtualAllocEx(process, 0, fulllen, MEM_COMMIT, PAGE_EXECUTE);

auto remoteCave =                                             // (3)
    (LPVOID)((DWORD)remoteString + stringlen);

WriteProcessMemory(process, remoteString, string,   stringlen, NULL);  // (4)
WriteProcessMemory(process, remoteCave,   codeCave, cavelen,   NULL);  // (5)
```

> Listagem 7-4: gravando o shellcode na code cave.

Primeiro, calcula `fulllen` (1) com bytes para a string e a cave.
Em seguida, `VirtualAllocEx()` aloca `fulllen` bytes em `process`
com protection `PAGE_EXECUTE` (use `0` e `MEM_COMMIT` para o
segundo e quarto parametros), guardando o endereco em
`remoteString` (2). Soma `stringlen` em `remoteString` para definir
o inicio da cave em `remoteCave` (3) (a cave fica logo apos a
string). Por fim, `WriteProcessMemory()` preenche o buffer com a
string (4) e o codigo (5).

A Tabela 7-1 mostra um memory dump da cave assumindo alocacao em
`0x030000`, `someFunction = 0xDEADBEEF`, `times = 5`,
`string = "injected!"`.

> Tabela 7-1: memory dump da code cave
>
> | Address | Code representation | Raw data | Significado |
> |---|---|---|---|
> | `0x030000` | `remoteString[0..4]` | `0x69 0x6E 0x6A 0x65 0x63` | `injec` |
> | `0x030005` | `remoteString[5..9]` | `0x74 0x65 0x64 0x0A 0x00` | `ted!\0` |
> | `0x03000A` | `remoteCave[0..3]` | `0xFF 0x74 0x24 0x04` | `PUSH DWORD PTR[ESP+0x4]` |
> | `0x03000E` | `remoteCave[4..8]` | `0x68 0x05 0x00 0x00 0x00` | `PUSH 0x05` |
> | `0x030013` | `remoteCave[9..13]` | `0xB8 0xEF 0xBE 0xAD 0xDE` | `MOV EAX, 0xDEADBEEF` |
> | `0x030018` | `remoteCave[14..15]` | `0xFF 0xD0` | `CALL EAX` |
> | `0x03001A` | `remoteCave[16..18]` | `0x83 0xC4 0x08` | `ADD ESP, 0x08` |
> | `0x03001D` | `remoteCave[19]` | `0xC3` | `RETN` |

Voce ve `injected!` em `0x030000`, `times` em `0x03000E` e o
endereco de `someFunction` em `0x030014`.

### Executando a code cave com thread injection

Com a cave gravada, falta executar. No exemplo:

```cpp
HANDLE thread = CreateRemoteThread(process, NULL, NULL,
                    (LPTHREAD_START_ROUTINE)remoteCave,
                    remoteString, NULL, NULL);
WaitForSingleObject(thread, INFINITE);
CloseHandle(thread);
VirtualFreeEx(process, remoteString, fulllen, MEM_RELEASE);
```

`CreateRemoteThread()`, `WaitForSingleObject()` e `CloseHandle()`
injetam e executam. `VirtualFreeEx()` libera a memoria alocada,
apagando os rastros do bot. Na pratica, sempre cheque os retornos:
se `VirtualAllocEx()` retornar `0x00000000`, a alocacao falhou; se
ignorar, `WriteProcessMemory()` tambem falha e
`CreateRemoteThread()` comeca a executar em `0x00000000`,
crashando o game. Geralmente essas funcoes so falham se o handle
do processo nao tiver as access flags certas.

## Hijack do main thread para executar code caves

As vezes a code cave precisa estar em sync com o main thread do
game. Suspender o main thread ate a cave terminar e lento. Mais
rapido: forcar a propria thread a executar o codigo, pratica
chamada *thread hijacking*.

> Acompanhe pelo `main-codeInjection.cpp` (versao simplificada de
> `injectCodeUsingThreadHijacking()`).

### Construindo a code cave para hijack

Diferente de thread injection, voce nao sabe o que a thread esta
executando quando voce hijack. Voce precisa salvar o estado da
thread no inicio da cave e restaurar no fim. O shellcode precisa
ser envelopado:

```nasm
PUSHAD                  ; salva os general registers na stack
PUSHFD                  ; salva EFLAGS na stack
; shellcode aqui
POPFD                   ; restaura EFLAGS
POPAD                   ; restaura registradores
; retoma a thread sem usar registradores aqui
```

> Listagem 7-5: framework do code cave para thread hijacking.

Para chamar a mesma `someFunction`, voce poderia usar um shellcode
parecido com o da Listagem 7-2, mas sem `CreateRemoteThread()`,
nao ha argumento via stack: empilhe `string` igual a `times`. A
parte que faz a call:

```nasm
PUSH string
PUSH times
MOV EAX, someFunction
CALL EAX
ADD ESP, 0x8
```

> Listagem 7-6: skeleton de assembly para chamar `someFunction`.

A unica diferenca da Listagem 7-1 e que aqui `string` e empilhada
explicitamente e nao tem `RETN`. Voce nao retorna porque quer que
a thread volte ao que fazia antes do hijack.

Para retomar a execucao normal, a cave precisa pular para o `EIP`
original sem usar registradores. Use `GetThreadContext()` para
pegar o `EIP`, preenchendo o skeleton em C++. Depois, empurre na
stack e faca return. A Listagem 7-7 mostra como a cave termina:

```nasm
PUSH originalEIP
RETN
```

> Listagem 7-7: pulando para `EIP` indiretamente.

`RETN` pula para o topo da stack; entao `PUSH originalEIP` seguido
de `RETN` faz o trabalho. Use isso em vez de jump direto, ja que
jumps exigem calculo de offset.

Juntando, a cave completa fica:

```nasm
; salvar estado
PUSHAD
PUSHFD

; payload
PUSH string
PUSH times
MOV EAX, someFunction
CALL EAX
ADD ESP, 0x8

; restaurar estado
POPFD
POPAD

; un-hijack: retomar a thread
PUSH originalEIP
RETN
```

Em seguida, traduza para bytes (igual a "Traduzindo o assembly para
shellcode") e gere o array.

### Gerando o shellcode skeleton e alocando memoria

```cpp
BYTE codeCave[31] = {
    0x60,                         // PUSHAD
    0x9C,                         // PUSHFD
    0x68, 0x00, 0x00, 0x00, 0x00, // PUSH 0  (string)
    0x68, 0x00, 0x00, 0x00, 0x00, // PUSH 0  (times)
    0xB8, 0x00, 0x00, 0x00, 0x00, // MOV EAX, 0x0
    0xFF, 0xD0,                   // CALL EAX
    0x83, 0xC4, 0x08,             // ADD ESP, 0x08
    0x9D,                         // POPFD
    0x61,                         // POPAD
    0x68, 0x00, 0x00, 0x00, 0x00, // PUSH 0  (originalEIP)
    0xC3                          // RETN
};

memcpy(&codeCave[3],  &remoteString,        4);
memcpy(&codeCave[8],  &times,               4);
memcpy(&codeCave[13], &func,                4);
memcpy(&codeCave[25], &threadContext.Eip,   4);
```

> Listagem 7-8: array de shellcode para thread hijacking.

Como na Listagem 7-3, `memcpy()` insere as variaveis no skeleton.
Diferenca: `times` e `func` ja sao conhecidos imediatamente, mas
`remoteString` so existe apos a alocacao e `threadContext.Eip` so
apos congelar a thread. Faca a alocacao antes do freeze para
manter a thread suspensa o minimo possivel:

```cpp
int stringlen = strlen(string) + 1;
int cavelen   = sizeof(codeCave);
int fulllen   = stringlen + cavelen;

auto remoteString =
    VirtualAllocEx(process, 0, fulllen, MEM_COMMIT, PAGE_EXECUTE);
auto remoteCave =
    (LPVOID)((DWORD)remoteString + stringlen);
```

(Igual ao thread injection, da para reaproveitar.)

### Encontrando e congelando o main thread

O codigo para congelar o main thread e mais delicado. Primeiro,
voce pega o thread identifier (parecido com PID) usando
`CreateToolhelp32Snapshot()`, `Thread32First()` e `Thread32Next()`
de `TlHelp32.h`. Um processo pode ter varias threads, mas o
exemplo assume que a primeira criada pelo game e a que precisa ser
hijacked:

```cpp
DWORD GetProcessThreadID(HANDLE Process) {
    THREADENTRY32 entry;
    entry.dwSize = sizeof(THREADENTRY32);
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (Thread32First(snapshot, &entry) == TRUE) {
        DWORD PID = GetProcessId(Process);
        while (Thread32Next(snapshot, &entry) == TRUE) {
            if (entry.th32OwnerProcessID == PID) {
                CloseHandle(snapshot);
                return entry.th32ThreadID;
            }
        }
    }
    CloseHandle(snapshot);
    return NULL;
}
```

Itera todas as threads e devolve a primeira que pertence ao PID
do game. Em seguida, pegue o estado dos registradores:

```cpp
HANDLE thread = OpenThread(
    (THREAD_GET_CONTEXT | THREAD_SUSPEND_RESUME | THREAD_SET_CONTEXT),
    false, threadID);

SuspendThread(thread);

CONTEXT threadContext;
threadContext.ContextFlags = CONTEXT_CONTROL;
GetThreadContext(thread, &threadContext);
```

`OpenThread()` da o handle, `SuspendThread()` congela e
`GetThreadContext()` pega os registradores. Apos isso, os
`memcpy()` da Listagem 7-8 ja tem todas as variaveis para gerar
o shellcode.

A escrita na memoria fica igual a Listagem 7-4:

```cpp
WriteProcessMemory(process, remoteString, string,   stringlen, NULL);
WriteProcessMemory(process, remoteCave,   codeCave, cavelen,   NULL);
```

Com a cave pronta, basta apontar o `EIP` da thread para o inicio
da cave e retomar:

```cpp
threadContext.Eip = (DWORD)remoteCave;
threadContext.ContextFlags = CONTEXT_CONTROL;
SetThreadContext(thread, &threadContext);
ResumeThread(thread);
```

A thread retoma na cave; pelo formato da cave, ela nao percebe
mudanca: salva estado, executa payload, restaura, volta ao codigo
original.

Ao injetar codigo, lembre-se: cuidado com qual data sua cave
toca. Se voce monta uma cave que chama internal functions do game
para criar um network packet, garanta que qualquer global que essas
funcoes tocam (packet buffer, packet position marker, etc) seja
restaurada. Voce nao sabe o que o game esta fazendo no momento da
cave; pode estar chamando a mesma funcao!

## Injetando DLLs para controle total

Code caves sao poderosas, mas escrever shellcode na mao nao escala.
Seria bem mais conveniente injetar codigo C++. Da, mas o processo
e bem mais complexo: precisa compilar para assembly, empacotar
position-agnostic, mapear dependencias externas, mapear tudo na
memoria e executar a partir de um entry point.

Sorte que o Windows ja resolve tudo isso. Compilando um projeto
C++ como dynamic library, voce produz um binario auto-contido e
position-agnostic chamado *Dynamic Link Library* (DLL). Em
seguida, com uma combinacao de thread injection ou hijacking e
`LoadLibrary()`, voce mapeia a DLL na memoria do game.

> Acompanhe `main-codeInjection.cpp` em
> `GameHackingExamples/Chapter7_CodeInjection` e `dllmain.cpp` em
> `GameHackingExamples/Chapter7_CodeInjection_DLL`. Olhe a funcao
> `LoadDLL()`.

### Enganando o processo a carregar sua DLL

Com uma code cave, voce trapaceia o processo remoto a chamar
`LoadLibrary()` na sua DLL, efetivamente carregando codigo
estrangeiro. Como `LoadLibrary()` recebe um unico parametro:

```cpp
wchar_t* dllName = L"c:\\something.dll";
int namelen = wcslen(dllName) + 1;
LPVOID remoteString =
    VirtualAllocEx(process, NULL, namelen * 2, MEM_COMMIT, PAGE_EXECUTE);
WriteProcessMemory(process, remoteString, dllName, namelen * 2, NULL);

HMODULE k32 = GetModuleHandleA("kernel32.dll");
LPVOID funcAdr = GetProcAddress(k32, "LoadLibraryW");

HANDLE thread =
    CreateRemoteThread(process, NULL, NULL,
        (LPTHREAD_START_ROUTINE)funcAdr,
        remoteString, NULL, NULL);

WaitForSingleObject(thread, INFINITE);
CloseHandle(thread);
```

E uma mistura do thread injection do bypass de ASLR com a code
cave de `someFunction`. Como `LoadLibrary` precisa de string
pointer, voce injeta o nome da DLL na memoria do game. Apos
injetar a thread, `LoadLibrary` carrega a DLL, colocando codigo
estrangeiro dentro do game.

> NOTA: de a sua DLL um nome unico, tipo
> `MySuperBotV2Hook.dll`. Nomes genericos como `Hook.dll` ou
> `Injected.dll` sao perigosos. Se conflitarem com uma DLL ja
> carregada, `LoadLibrary()` assume que e a mesma e nao carrega.

Quando a DLL e carregada, o entry point (`DllMain()`) e executado
com motivo `DLL_PROCESS_ATTACH`. Quando o processo e morto ou
`FreeLibrary()` e chamado na DLL, o entry point e executado com
motivo `DLL_PROCESS_DETACH`. Tratar:

```cpp
BOOL APIENTRY DllMain(HMODULE hModule,
                      DWORD  ul_reason_for_call,
                      LPVOID lpReserved) {
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH:
            printf("DLL attached!\n");
            break;
        case DLL_PROCESS_DETACH:
            printf("DLL detached!\n");
            break;
    }
    return TRUE;
}
```

Atencao: o entry point de uma DLL roda dentro de um *loader lock*,
um lock global usado por todas as funcoes que leem/modificam a
lista de modulos do processo. Esse lock e usado por
`GetModuleHandle()`, `GetModuleFileName()`, `Module32First()` e
`Module32Next()`, ou seja, rodar codigo nao trivial direto no
entry point pode dar deadlock. Sempre que precisar rodar codigo
substantivo apos o attach, faca em uma nova thread:

```cpp
DWORD WINAPI runBot(LPVOID lpParam) {
    // sua logica de bot
    return 1;
}

// dentro de DllMain() no case DLL_PROCESS_ATTACH:
auto thread = CreateThread(NULL, 0, &runBot, NULL, 0, NULL);
CloseHandle(thread);
```

Em `runBot()` voce executa o codigo do bot dentro do game,
podendo manipular memoria diretamente com typecasts.

Ao injetar DLLs, cuide das dependencias. Se sua DLL depende de
DLLs nao padrao, ou injete essas DLLs no game antes ou coloque
numa pasta que `LoadLibrary()` procure (qualquer pasta no
`PATH`). A primeira opcao so funciona se essas DLLs nao tiverem
dependencias proprias; a segunda e enrolada e sujeita a colisao
de nomes. Melhor estrategia: linkar todas as bibliotecas
externas estaticamente para que sejam compiladas direto na sua
DLL.

### Acessando memoria de uma DLL injetada

Quando voce esta injetado, process handles e API functions
atrapalham. Como o game compartilha o mesmo address space que o
codigo injetado, da para acessar a memoria do game diretamente.
Por exemplo, para acessar um `DWORD`:

```cpp
DWORD value = *((DWORD*)adr); // le um DWORD em adr
*((DWORD*)adr) = 1234;        // escreve 1234 em DWORD adr
```

Typecast in place funciona, mas e mais limpo abstrair, igual aos
wrappers da Windows API:

```cpp
template<typename T>
T readMemory(LPVOID adr) {
    return *((T*)adr);
}

template<typename T>
void writeMemory(LPVOID adr, T val) {
    *((T*)adr) = val;
}
```

Uso similar ao das funcoes do Capitulo 6:

```cpp
DWORD value = readMemory<DWORD>(adr);
writeMemory<DWORD>(adr, value++);
```

Sao quase identicas as do Capitulo 6, mas dispensam o handle do
processo. Para deixar ainda mais flexivel, crie `pointMemory()`:

```cpp
template<typename T>
T* pointMemory(LPVOID adr) {
    return ((T*)adr);
}
```

Pula o dereference e devolve o ponteiro. Voce le e escreve
dereferenciando voce mesmo:

```cpp
DWORD* pValue = pointMemory<DWORD>(adr); // aponta
DWORD  value  = *pValue;                 // 'le'
(*pValue)++;                             // 'incrementa e escreve'
```

Com `pointMemory()`, da para eliminar `readMemory()` e
`writeMemory()` em muitos casos, deixando o codigo bem mais
direto.

### Bypass de ASLR a partir da DLL injetada

Como o codigo ja esta injetado, nao precisa thread remota para
pegar o base address. Chame `GetModuleHandle()` direto:

```cpp
DWORD newBase = (DWORD)GetModuleHandle(NULL);
```

Mais rapido ainda: usar o segmento de memoria `FS` do game (outro
superpoder do codigo injetado). Esse segmento aponta para uma
estrutura chamada *Thread Environment Block* (TEB), e em offset
`0x30` do TEB ha um ponteiro para a *Process Environment Block*
(PEB). Em offset `0x8` do PEB esta o base address do main module.
Com inline assembly:

```cpp
DWORD newBase;
__asm {
    MOV EAX, DWORD PTR FS:[0x30]
    MOV EAX, DWORD PTR DS:[EAX+0x8]
    MOV newBase, EAX
}
```

A primeira instrucao guarda o endereco do PEB em `EAX`. A segunda
le o base address do main module e guarda em `EAX`. A ultima copia
para `newBase`.

## Fechando

No Capitulo 6, voce viu como ler memoria remotamente e como uma
DLL injetada acessa direto via ponteiros. Este capitulo mostrou
como injetar todo tipo de codigo, do byte code puro a binarios
C++ inteiros. No proximo, voce vai ver o que da para fazer com
todo esse poder dentro do address space do game. Se voce achou
injection legal, vai amar combinar C++ injetado com manipulacao
de control flow.

O codigo de exemplo deste capitulo tem PoC de tudo o que vimos. Se
algo ainda esta confuso, brinque com ele para ver tudo em acao.
