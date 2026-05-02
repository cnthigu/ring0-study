# Capitulo 6 - Lendo e escrevendo memoria do game

> Titulo original: *Reading from and Writing to Game Memory*

> Navegacao: [Anterior](capitulo-05.md) | [Indice](README.md) | [Proximo](capitulo-07.md)

## Topicos

- Obter Process Identifier (PID) e handle do processo
- Ler/escrever memoria com `ReadProcessMemory` e `WriteProcessMemory`
- Memory protection e atributos de pages
- Bypass de Address Space Layout Randomization (ASLR)

## Abertura

Capitulos anteriores discutiram como memoria e estruturada e como
escanear/modificar com Cheat Engine e OllyDbg. Trabalhar com
memoria e essencial quando voce comeca a escrever bots; o seu
codigo precisa saber fazer isso.

Este capitulo aprofunda os detalhes de manipulacao de memoria no
nivel de codigo. Primeiro, voce aprende a localizar e obter handles
para processos de game. Depois, a usar esses handles para ler e
escrever memoria, tanto a partir de processos remotos quanto de
codigo injetado. Para fechar, bypasses de uma certa tecnica de
protecao de memoria, com um pequeno exemplo de code injection. O
codigo de exemplo esta em `GameHackingExamples/Chapter6_AccessingMemory`.

> NOTA: quando o livro fala em API functions, refere-se a Windows
> API, salvo aviso. Se nenhum header e mencionado, suponha
> `Windows.h`.

## Obtendo o Process Identifier do game

Para ler ou escrever memoria de um game, voce precisa do *process
identifier* (PID), um numero que identifica unicamente um processo
ativo. Se o game tem janela visivel, voce pega o PID via
`GetWindowThreadProcessId()`. Essa funcao recebe o handle da janela
no primeiro parametro e devolve o PID no segundo. O handle da
janela vem de `FindWindow()`, passando o titulo da janela (texto na
taskbar) como segundo parametro, como na Listagem 6-1.

```cpp
HWND myWindow =
    FindWindow(NULL, "Title of the game window here");
DWORD PID;
GetWindowThreadProcessId(myWindow, &PID);
```

> Listagem 6-1: pegando o handle da janela para obter o PID.

Com o handle, basta declarar onde armazenar o PID e chamar
`GetWindowThreadProcessId()`.

Se o game nao tem janela ou o nome da janela nao e previsivel, da
para enumerar processos e procurar pelo nome do binario. A
Listagem 6-2 usa `CreateToolhelp32Snapshot()`, `Process32First()` e
`Process32Next()` de `tlhelp32.h`.

```cpp
#include <tlhelp32.h>

PROCESSENTRY32 entry;
entry.dwSize = sizeof(PROCESSENTRY32);
HANDLE snapshot =
    CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
if (Process32First(snapshot, &entry) == TRUE) {
    while (Process32Next(snapshot, &entry) == TRUE) {
        std::wstring binPath = entry.szExeFile;
        if (binPath.find(L"game.exe") != std::wstring::npos) {
            printf("game pid is %d\n", entry.th32ProcessID);
            break;
        }
    }
}
CloseHandle(snapshot);
```

> Listagem 6-2: pegando o PID sem saber o titulo da janela.

Pode parecer mais complexo, mas e basicamente um `for` canonico:
`CreateToolhelp32Snapshot()` cria a lista; `entry` e o iterador;
`Process32First()` inicializa, `Process32Next()` incrementa. O
return boolean de `Process32Next()` e o comparator. O codigo itera
sobre todos os processos, procura por `game.exe` no path do binario
e imprime o PID.

## Obtendo handles de processo

Com o PID, voce pega um handle do proprio processo via
`OpenProcess()`. Essa funcao permite obter handles com niveis de
acesso necessarios para ler/escrever memoria. E crucial em game
hacking, porque qualquer funcao que opere num processo precisa de
um handle com o acesso adequado.

Prototipo de `OpenProcess()`:

```cpp
HANDLE OpenProcess(DWORD DesiredAccess, BOOL InheritHandle, DWORD ProcessId);
```

`DesiredAccess` espera uma ou uma combinacao de access flags. As
mais comuns em game hacking:

- `PROCESS_VM_OPERATION`: handle util com `VirtualAllocEx()`,
  `VirtualFreeEx()` e `VirtualProtectEx()` para alocar, liberar e
  proteger chunks de memoria.
- `PROCESS_VM_READ`: handle util com `ReadProcessMemory()`.
- `PROCESS_VM_WRITE`: util com `WriteProcessMemory()`, mas tambem
  precisa de `PROCESS_VM_OPERATION`. Combine via
  `PROCESS_VM_OPERATION | PROCESS_VM_WRITE` em `DesiredAccess`.
- `PROCESS_CREATE_THREAD`: handle util com `CreateRemoteThread()`.
- `PROCESS_ALL_ACCESS`: handle pode fazer qualquer coisa. Evite, ja
  que so processos com debug privileges podem usar e tem problemas
  de compatibilidade em Windows mais antigos.

`InheritHandle` em geral pode ser `false`. `ProcessId` espera o PID
do processo.

### Trabalhando com OpenProcess()

Exemplo:

```cpp
DWORD PID = getGamePID();
HANDLE process = OpenProcess(
    PROCESS_VM_OPERATION |
        PROCESS_VM_READ |
        PROCESS_VM_WRITE,
    FALSE,
    PID
);
if (process == INVALID_HANDLE_VALUE) {                       // (1)
    printf("Failed to open PID %d, error code %d",
           PID, GetLastError());
}
```

Primeiro, `getGamePID()` busca o PID. (Voce escreve essa funcao;
pode ser uma das listagens 6-1 e 6-2 transformada em funcao.) Em
seguida, `OpenProcess()` recebe tres flags: `PROCESS_VM_OPERATION`
da permissao de operacao em memoria, e os outros dois combinados
dao read e write. O exemplo trata erro em (1), mas, se o PID
estiver certo, as flags forem validas e seu codigo rodar com
permissoes iguais ou maiores que o game (Run As Admin), nao deve
falhar.

Quando terminar de usar um handle, libere com `CloseHandle()`:

```cpp
CloseHandle(process);
```

Da para reaproveitar handles a vontade. Deixe aberto ate terminar
ou ate o bot encerrar.

## Acessando memoria

A Windows API expoe duas funcoes cruciais para acesso de memoria:
`ReadProcessMemory()` e `WriteProcessMemory()`. Voce manipula
memoria do game externamente com elas.

### ReadProcessMemory() e WriteProcessMemory()

Os prototipos (Listagem 6-3) sao bem parecidos.

```cpp
BOOL ReadProcessMemory(
    HANDLE Process, LPVOID Address,
    LPVOID Buffer, DWORD Size,
    DWORD *NumberOfBytesRead
);

BOOL WriteProcessMemory(
    HANDLE Process, LPVOID Address,
    LPCVOID Buffer, DWORD Size,
    DWORD *NumberOfBytesWritten
);
```

> Listagem 6-3: prototipos de `ReadProcessMemory()` e
> `WriteProcessMemory()`.

`Process` e o handle, `Address` e o endereco-alvo. Em leitura,
`Buffer` recebe o dado lido. Em escrita, aponta para o dado a ser
escrito. Em ambos, `Size` e o tamanho de `Buffer` em bytes. O
ultimo parametro retorna opcionalmente o numero de bytes
acessados; pode ser `NULL`. Salvo falha, esse valor deve ser igual
a `Size`.

### Acessando um valor em memoria

A Listagem 6-4 mostra um uso pratico:

```cpp
DWORD val;
ReadProcessMemory(proc, adr, &val, sizeof(DWORD), 0);
printf("Current mem value is %d\n", val);
val++;
WriteProcessMemory(proc, adr, &val, sizeof(DWORD), 0);
ReadProcessMemory(proc, adr, &val, sizeof(DWORD), 0);
printf("New mem value is confirmed as %d\n", val);
```

> Listagem 6-4: leitura e escrita de memoria de processo via
> Windows API.

Antes deste codigo, voce precisa do PID e do endereco (`adr`).
`ReadProcessMemory()` armazena o valor lido em `val`. O codigo
incrementa e escreve com `WriteProcessMemory()`. Em seguida, le de
novo para confirmar. Note que `val` nao e um buffer; passar `&val`
funciona porque pode ser ponteiro para qualquer estrutura
estatica, desde que `Size` bata.

### Funcoes de memoria via templates

A Listagem 6-4 hardcoda `DWORD`. Para ser versatil, use templates:

```cpp
template<typename T>
T readMemory(HANDLE proc, LPVOID adr) {
    T val;
    ReadProcessMemory(proc, adr, &val, sizeof(T), NULL);
    return val;
}

template<typename T>
void writeMemory(HANDLE proc, LPVOID adr, T val) {
    WriteProcessMemory(proc, adr, &val, sizeof(T), NULL);
}
```

> Listagem 6-5: funcoes genericas de memoria.

Permitem acessar qualquer tipo:

```cpp
DWORD value = readMemory<DWORD>(proc, adr);
writeMemory<DWORD>(proc, adr, value++);
```

> Listagem 6-6: chamando funcoes templated.

Compare com a Listagem 6-4: o resultado e o mesmo, mas o tipo e
parametrizavel.

## Memory protection

Quando memoria e alocada por um game (ou qualquer programa), ela
fica em uma *page*. Em x86 Windows, pages sao chunks de 4096 bytes.
Como toda memoria precisa estar em uma page, a unidade minima de
alocacao e 4096 bytes. O SO pode colocar chunks menores como
subset de uma page com espaco livre, em uma page nova ou em duas
pages contiguas com mesmos atributos.

Chunks de 4096+ bytes ocupam `n` pages, onde
`n = ceil(memory_size / 4096)`. O SO geralmente reusa pages
existentes, mas aloca novas conforme necessario.

> NOTA: e possivel um chunk grande ocupar `n + 1` pages, ja que nao
> ha garantia de que ele comeca exatamente no inicio da page.

Cada page tem um conjunto de atributos. A maioria e transparente em
user mode, mas um e fundamental em game hacking: *protection*.

### Tipos de memory protection no x86 Windows

As tecnicas de leitura ate aqui assumem `PAGE_READWRITE`. Isso
vale para variavel, mas outros dados ficam em pages com outros
tipos de protecao. Tabela 6-1:

> Tabela 6-1: tipos de memory protection
>
> | Tipo | Valor | Read | Write | Execute | Especial |
> |---|---|---|---|---|---|
> | `PAGE_NOACCESS` | `0x01` | nao | nao | nao | |
> | `PAGE_READONLY` | `0x02` | sim | nao | nao | |
> | `PAGE_READWRITE` | `0x04` | sim | sim | nao | |
> | `PAGE_WRITECOPY` | `0x08` | sim | sim | nao | copy on write |
> | `PAGE_EXECUTE` | `0x10` | nao | nao | sim | |
> | `PAGE_EXECUTE_READ` | `0x20` | sim | nao | sim | |
> | `PAGE_EXECUTE_READWRITE` | `0x40` | sim | sim | sim | |
> | `PAGE_EXECUTE_WRITECOPY` | `0x80` | sim | sim | sim | copy on write |
> | `PAGE_GUARD` | `0x100` | nao | nao | nao | guard page |

Strings constantes, por exemplo, ficam em geral em `PAGE_READONLY`.
Outros dados constantes (VF tables, header PE inteiro do modulo)
tambem em pages read-only. Codigo assembly fica em
`PAGE_EXECUTE_READ`.

A maioria dos tipos envolve combinacoes de read/write/execute. Por
ora, tipos especiais podem ser ignorados; os basicos vao dominar
suas aventuras.

### Mudando memory protection

As vezes voce precisa acessar memoria de modo proibido pela
protecao da page. A Windows API tem `VirtualProtectEx()` para isso:

```cpp
BOOL VirtualProtectEx(
    HANDLE Process, LPVOID Address,
    DWORD Size, DWORD NewProtect,
    PDWORD OldProtect
);
```

`Process`, `Address` e `Size` recebem entradas iguais as de
`ReadProcessMemory()`/`WriteProcessMemory()`. `NewProtect`
especifica as novas flags. `OldProtect` opcionalmente recebe as
flags antigas.

> ### Boxe: tipos especiais de protection
>
> Dois tipos da Tabela 6-1 incluem **copy-on-write**. Quando varios
> processos tem pages identicas (DLLs do sistema mapeadas, por
> exemplo), copy-on-write economiza memoria fisica: o dado real
> existe em um lugar fisico unico, e o SO mapeia virtualmente
> todas as pages para ele. Se um dos processos faz mudanca, o SO
> faz uma copia fisica, aplica a mudanca e remapeia. As pages
> trocam: `PAGE_WRITECOPY` vira `PAGE_READWRITE`,
> `PAGE_EXECUTE_WRITECOPY` vira `PAGE_EXECUTE_READWRITE`. Sem uso
> game hacking-especifico claro, mas vale conhecer.
>
> Pages tambem podem ter **guard protection**. Guarded pages tem
> uma protecao secundaria, ex.: `PAGE_GUARD | PAGE_READONLY`.
> Quando o programa acessa uma guarded page, o SO joga uma excecao
> `STATUS_GUARD_PAGE_VIOLATION`. Apos tratar, o flag PAGE_GUARD
> some, deixando so a secundaria. O SO usa isso para expandir a
> call stack dinamicamente colocando uma guarded page no topo e
> alocando mais memoria quando ela e atingida. Em game hacking, da
> para usar guarded pages como tripwire detectando se o game
> tentou ler seu codigo na memoria.

A protecao mais granular e por page, ou seja, `VirtualProtectEx()`
seta a nova protecao em cada page entre `Address` e
`Address + Size - 1`.

> NOTA: existe `VirtualProtect()`, irma de `VirtualProtectEx()`.
> Funciona igual, mas opera apenas no processo que chama; nao
> recebe handle.

Sugestao: encapsule em template para reuso:

```cpp
template<typename T>
DWORD protectMemory(HANDLE proc, LPVOID adr, DWORD prot) {
    DWORD oldProt;
    VirtualProtectEx(proc, adr, sizeof(T), prot, &oldProt);
    return oldProt;
}
```

> Listagem 6-7: helper generico para mudar protection.

Para escrever um `DWORD` em uma page sem write permission:

```cpp
protectMemory<DWORD>(process, address, PAGE_READWRITE);
writeMemory<DWORD>(process, address, newValue);
```

Boa pratica: deixar a mudanca de protection ativa o menor tempo
possivel e restaurar a original assim que terminar. E menos
eficiente, mas evita que o game detecte seu bot (por exemplo,
percebendo paginas de codigo que viraram writable).

```cpp
DWORD oldProt =
    protectMemory<DWORD>(process, address, PAGE_READWRITE);
writeMemory<DWORD>(process, address, newValue);
protectMemory<DWORD>(process, address, oldProt);
```

Uma observacao final: quando voce mexe em memoria, o game pode
acessar a mesma memoria ao mesmo tempo. Se a nova protection nao
e compativel com a original, o processo do game leva
`ACCESS_VIOLATION` e crasha. Por exemplo, mudando de
`PAGE_EXECUTE` para `PAGE_READWRITE`, o game pode tentar executar
codigo numa page que nao e mais executable. Nesse caso, prefira
`PAGE_EXECUTE_READWRITE` para que voce escreva enquanto o game
ainda executa.

## Address Space Layout Randomization (ASLR)

Ate aqui descrevemos enderecos como inteiros estaticos que so
mudam quando o binario muda. Vale para Windows XP e anteriores.
Em sistemas mais novos, enderecos sao estaticos *relativos* ao
*base address* do binario, porque o sistema habilita ASLR para
binarios suportados. Quando o binario e compilado com suporte a
ASLR (default no MSVC++ 2010 e em muitos outros compiladores), o
base address pode ser diferente a cada execucao. Binarios sem
ASLR sempre tem base `0x400000`.

> NOTA: como ASLR nao funcionava no XP, vamos chamar `0x400000` de
> *XP-base*.

### Desabilitando ASLR para simplificar dev

Para simplificar o desenvolvimento, desabilite ASLR e use
enderecos com XP-base. No Visual Studio Command Prompt:

```text
> editbin /DYNAMICBASE:NO "C:\path\to\game.exe"
```

Para reabilitar:

```text
> editbin /DYNAMICBASE "C:\path\to\game.exe"
```

### Bypass de ASLR em producao

Desabilitar ASLR e ok em dev, mas nao da para pedir isso ao usuario
final. Em vez disso, escreva uma funcao que rebase enderecos em
runtime. Com enderecos no XP-base:

```cpp
DWORD rebase(DWORD address, DWORD newBase) {
    DWORD diff = address - 0x400000;
    return diff + newBase;
}
```

Sabendo o `newBase`, voce ignora o ASLR rebaseando os enderecos.
Para achar o `newBase`, use `GetModuleHandle()`. Quando o
parametro e `NULL`, retorna o handle do binario principal do
processo. O tipo de retorno e `HMODULE`, mas o valor e o endereco
onde o binario esta mapeado. Como voce esta procurando o base
address de outro processo, precisa executar `GetModuleHandle()` no
contexto desse outro processo.

Para isso, use `CreateRemoteThread()`, que spawna threads e executa
codigo em processo remoto:

```cpp
HANDLE CreateRemoteThread(
    HANDLE Process,
    LPSECURITY_ATTRIBUTES ThreadAttributes,
    DWORD StackSize,
    LPTHREAD_START_ROUTINE StartAddress,
    LPVOID Param,
    DWORD CreationFlags,
    LPDWORD ThreadId
);
```

> Listagem 6-8: funcao que spawna thread remota.

A thread comeca em `StartAddress`, tratado como funcao de um
parametro com `Param` como entrada e setando o return value como
*exit code* da thread. Combinando: aponte `StartAddress` para
`GetModuleHandle()` e `Param` para `NULL`. Use
`WaitForSingleObject()` para esperar a thread terminar e
`GetExitCodeThread()` para pegar o base address de retorno.

Codigo completo:

```cpp
DWORD newBase;

HMODULE k32 = GetModuleHandle("kernel32.dll");

LPVOID funcAdr = GetProcAddress(k32, "GetModuleHandleA");
if (!funcAdr)
    funcAdr = GetProcAddress(k32, "GetModuleHandleW");

HANDLE thread =
    CreateRemoteThread(process, NULL, NULL,
        (LPTHREAD_START_ROUTINE)funcAdr,
        NULL, NULL, NULL);

WaitForSingleObject(thread, INFINITE);

GetExitCodeThread(thread, &newBase);

CloseHandle(thread);
```

> Listagem 6-9: descobrindo o base address do game via Windows API.

`GetModuleHandle()` faz parte de `kernel32.dll`, que tem o mesmo
base address em todo processo. Com isso, o codigo pega o endereco
de `kernel32.dll` no proprio bot, descobre o endereco de
`GetModuleHandle()` via `GetProcAddress()`, e como esse endereco e
o mesmo no game, chama `CreateRemoteThread()` apontando para ele.
Depois espera a thread, captura o exit code e tem o `newBase`.

## Fechando

Voce ja sabe manipular memoria a partir do seu proprio codigo. Os
proximos capitulos aplicam as skills das Partes 1 e 2 a games
reais. Esses fundamentos sao essenciais; se estiver tendo
dificuldade, brinque com o codigo de exemplo enquanto revisa.

A forma como a Listagem 6-9 trapaceia o game para executar
`GetModuleHandle()` e uma forma de *code injection*. E so um
vislumbre. No Capitulo 7, mergulhamos a fundo.
