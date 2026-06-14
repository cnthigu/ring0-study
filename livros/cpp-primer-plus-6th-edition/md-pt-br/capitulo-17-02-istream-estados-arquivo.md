# Capítulo 17 — Entrada, Saída e Arquivos (parte 2)

> Tradução não oficial de *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Navegação: [Anterior](capitulo-17-01-streams-entrada-saida.md) | [Índice](README.md) | [Próximo](capitulo-17-03-arquivo-avancado-resumo.md)

## Entrada com `cin`

Agora é hora de tratar da entrada e de obter dados para um programa. O objeto `cin` representa a entrada padrão como um fluxo de bytes. Normalmente, você gera esse fluxo de caracteres pelo teclado. Se você digitar a sequência de caracteres `2011`, o objeto `cin` extrai esses caracteres do fluxo de entrada. Você pode pretender que essa entrada seja parte de uma string, ser um valor `int`, ser um valor `float` ou ser algum outro tipo. Portanto, a extração também envolve conversão de tipo. O objeto `cin`, guiado pelo tipo da variável designada para receber o valor, deve usar seus métodos para converter essa sequência de caracteres para o tipo de valor pretendido.

Tipicamente, você usa `cin` da seguinte forma:

```cpp
cin >> receptor_do_valor;
```

Aqui, `receptor_do_valor` identifica a posição de memória em que armazenar a entrada. Pode ser o nome de uma variável, uma referência, um ponteiro desreferenciado ou um membro de uma estrutura ou de uma classe. A forma como `cin` interpreta a entrada depende do tipo de dado de `receptor_do_valor`. A classe `istream`, definida no arquivo de cabeçalho `iostream`, sobrecarrega o operador de extração `>>` para reconhecer os seguintes tipos básicos:

- `signed char &`
- `unsigned char &`
- `char &`
- `short &`
- `unsigned short &`
- `int &`
- `unsigned int &`
- `long &`
- `unsigned long &`
- `long long &` (C++11)
- `unsigned long long &` (C++11)
- `float &`
- `double &`
- `long double &`

Esses são referidos como funções de entrada formatada (formatted input functions) porque convertem os dados de entrada para o formato indicado pelo destino.

Uma função operador típica tem um protótipo como o seguinte:

```cpp
istream & operator>>(int &);
```

Tanto o argumento quanto o valor de retorno são referências. Com um argumento de referência (ver Capítulo 8, "Aventuras em Funções"), uma instrução como a seguinte faz com que a função `operator>>()` trabalhe com a própria variável `tamanho_equipe` em vez de com uma cópia, como seria o caso com um argumento regular:

```cpp
cin >> tamanho_equipe;
```

Como o argumento é do tipo referência, `cin` é capaz de modificar diretamente o valor de uma variável usada como argumento. A instrução anterior, por exemplo, modifica diretamente o valor da variável `tamanho_equipe`. Chegaremos ao significado de um valor de retorno de referência em instantes. Primeiro, vamos examinar o aspecto de conversão de tipo do operador de extração. Para argumentos de cada tipo na lista de tipos anterior, o operador de extração converte a entrada de caracteres para o tipo indicado de valor. Por exemplo, suponha que `tamanho_equipe` seja do tipo `int`. Nesse caso, o compilador associa:

```cpp
cin >> tamanho_equipe;
```

ao seguinte protótipo:

```cpp
istream & operator>>(int &);
```

A função correspondente a esse protótipo então lê o fluxo de caracteres sendo enviado ao programa — digamos, os caracteres `2`, `3`, `1`, `8` e `4`. Para um sistema que usa um `int` de 2 bytes, a função converte esses caracteres para a representação binária de 2 bytes do inteiro `23184`. Se, por outro lado, `tamanho_equipe` fosse do tipo `double`, `cin` usaria `operator>>(double &)` para converter a mesma entrada para a representação de ponto flutuante de 8 bytes do valor `23184.0`.

Aliás, você pode usar os manipuladores `hex`, `oct` e `dec` com `cin` para especificar que a entrada inteira deve ser interpretada no formato hexadecimal, octal ou decimal. Por exemplo, a instrução a seguir faz com que uma entrada de `12` ou `0x12` seja lida como `12` hexadecimal, ou decimal `18`, e faz com que `f` ou `FF` seja lido como decimal `255`:

```cpp
cin >> hex;
```

A classe `istream` também sobrecarrega o operador de extração `>>` para tipos de ponteiro de caractere:

- `signed char *`
- `char *`
- `unsigned char *`

Para esse tipo de argumento, o operador de extração lê a próxima palavra da entrada e a coloca no endereço indicado, acrescentando um caractere nulo para transformá-la em string. Por exemplo, suponha que você tenha este código:

```cpp
cout << "Digite seu primeiro nome:\n";
char nome[20];
cin >> nome;
```

Se você responder à solicitação digitando `Liz`, o operador de extração colocará os caracteres `Liz\0` no array `nome`. (Como de costume, `\0` representa o caractere nulo de terminação.) O identificador `nome`, sendo o nome de um array de `char`, atua como o endereço do primeiro elemento do array, tornando `nome` do tipo `char *` (ponteiro para `char`).

O fato de que cada operador de extração retorna uma referência ao objeto invocador permite concatenar a entrada, assim como você pode concatenar a saída:

```cpp
char nome[20];
float taxa;
int grupo;
cin >> nome >> taxa >> grupo;
```

Aqui, por exemplo, o objeto `cin` retornado por `cin >> nome` torna-se o objeto que trata `taxa`.

### Como `cin >>` Enxerga a Entrada

As várias versões dos operadores de extração têm uma maneira comum de analisar o fluxo de entrada. Elas pulam o espaço em branco (espaços, novas linhas e tabulações) até encontrarem um caractere que não seja espaço em branco. Isso é verdade mesmo para os modos de caractere único (aqueles nos quais o argumento é do tipo `char`, `unsigned char` ou `signed char`), o que não é verdade para as funções de entrada de caractere do C. Nos modos de caractere único, o operador `>>` lê esse caractere e o atribui ao local indicado. Nos outros modos, o operador lê uma unidade do tipo indicado. Ou seja, ele lê tudo desde o caractere inicial que não seja espaço em branco até o primeiro caractere que não corresponda ao tipo de destino.

Por exemplo, considere o seguinte código:

```cpp
int altitude;
cin >> altitude;
```

Suponha que você digitou os seguintes caracteres:

```
-123Z
```

O operador lerá os caracteres `-`, `1`, `2` e `3` porque todos são partes válidas de um inteiro. Mas o caractere `Z` não é válido, portanto o último caractere aceito para entrada é o `3`. O `Z` permanece no fluxo de entrada, e a próxima instrução `cin` começará a leitura a partir desse ponto. Enquanto isso, o operador converte a sequência de caracteres `-123` para um valor inteiro e o atribui a `altitude`.

Pode acontecer que a entrada não corresponda ao que um programa espera. Por exemplo, suponha que você digitasse `Zcar` em vez de `-123Z`. Nesse caso, o operador de extração deixa o valor de `altitude` inalterado e retorna o valor 0. (Mais tecnicamente, uma instrução `if` ou `while` avalia um objeto `istream` como `false` se ele tiver um estado de erro definido; discutiremos isso com mais profundidade em breve.) O valor de retorno `false` permite que um programa verifique se a entrada corresponde aos requisitos, como mostra a Listagem 17.11.

**Listagem 17.11 — `verificar.cpp`**

```cpp
// verificar.cpp -- verificando entrada válida
#include <iostream>

int main()
{
    using namespace std;
    cout << "Digite números: ";
    int soma = 0;
    int entrada;
    while (cin >> entrada)
    {
        soma += entrada;
    }
    cout << "Último valor digitado = " << entrada << endl;
    cout << "Soma = " << soma << endl;
    return 0;
}
```

Eis a saída do programa da Listagem 17.11 quando alguma entrada inadequada (`-123Z`) invade o fluxo de entrada:

```
Digite números: 200
10 -50 -123Z 60
Último valor digitado = -123
Soma = 37
```

Como a entrada é armazenada em buffer, a segunda linha de valores de entrada do teclado não foi enviada ao programa até que você pressionou Enter no final da linha. Mas o loop parou de processar a entrada no caractere `Z` porque ele não correspondia a nenhum dos formatos de ponto flutuante. A falha da entrada em corresponder ao formato esperado, por sua vez, fez com que a expressão `cin >> entrada` avaliasse como `false`, encerrando o loop `while`.

## Estados de Fluxo

Vamos examinar mais de perto o que acontece com a entrada inadequada. Um objeto `cin` ou `cout` contém um membro de dado (herdado da classe `ios_base`) que descreve o estado do fluxo. Um estado de fluxo (stream state; definido como tipo `iostate`, que, por sua vez, é um tipo de máscara de bits, como descrito anteriormente) consiste em três elementos de `ios_base`: `eofbit`, `badbit` e `failbit`. Cada elemento é um único bit que pode ser 1 (definido) ou 0 (limpo). Quando uma operação `cin` chega ao fim de arquivo, ela define `eofbit`. Quando uma operação `cin` não consegue ler os caracteres esperados, como no exemplo anterior, ela define `failbit`. Falhas de E/S, como tentar ler um arquivo inacessível ou tentar gravar em um disco protegido contra gravação, também podem definir `failbit` como 1. O elemento `badbit` é definido quando alguma falha não diagnosticada pode ter corrompido o fluxo. (As implementações nem sempre concordam sobre quais eventos definem `failbit` e quais definem `badbit`.) Quando todos os três bits de estado são definidos como 0, tudo está bem. Os programas podem verificar o estado do fluxo e usar essas informações para decidir o que fazer a seguir. A Tabela 17.4 lista esses bits, juntamente com alguns métodos de `ios_base` que relatam ou alteram o estado do fluxo.

**Tabela 17.4 — Estados de Fluxo**

| Membro                 | Descrição                                                                                       |
|------------------------|-------------------------------------------------------------------------------------------------|
| `eofbit`               | É definido como 1 se o fim de arquivo for alcançado.                                            |
| `badbit`               | É definido como 1 se o fluxo pode estar corrompido; por exemplo, pode ter ocorrido um erro de leitura de arquivo. |
| `failbit`              | É definido como 1 se uma operação de entrada não leu os caracteres esperados ou uma operação de saída não gravou os caracteres esperados. |
| `goodbit`              | Apenas outra forma de dizer 0.                                                                  |
| `good()`               | Retorna `true` se o fluxo pode ser usado (todos os bits estão limpos).                          |
| `eof()`                | Retorna `true` se `eofbit` estiver definido.                                                    |
| `bad()`                | Retorna `true` se `badbit` estiver definido.                                                    |
| `fail()`               | Retorna `true` se `badbit` ou `failbit` estiver definido.                                       |
| `rdstate()`            | Retorna o estado do fluxo.                                                                      |
| `exceptions()`         | Retorna uma máscara de bits que identifica quais flags causam o lançamento de uma exceção.      |
| `exceptions(iostate e)`| Define quais estados farão `clear()` lançar uma exceção; por exemplo, se `e` for `eofbit`, então `clear()` lançará uma exceção se `eofbit` estiver definido. |
| `clear(iostate s)`     | Define o estado do fluxo como `s`; o padrão para `s` é 0 (`goodbit`); lança uma exceção `basic_ios::failure` se `rdstate() & exceptions()) != 0`. |
| `setstate(iostate s)`  | Chama `clear(rdstate() | s)`. Define os bits de estado do fluxo correspondentes aos bits definidos em `s`; outros bits de estado do fluxo ficam inalterados. |

### Definindo Estados

Dois dos métodos na Tabela 17.4, `clear()` e `setstate()`, são similares. Ambos redefinem o estado, mas o fazem de maneira diferente. O método `clear()` define o estado com seu argumento. Assim, a seguinte chamada usa o argumento padrão de 0, que limpa todos os três bits de estado (`eofbit`, `badbit` e `failbit`):

```cpp
clear();
```

Da mesma forma, a chamada a seguir faz o estado ser igual a `eofbit`; ou seja, `eofbit` é definido, e os outros dois bits de estado são limpos:

```cpp
clear(eofbit);
```

O método `setstate()`, no entanto, afeta apenas os bits que estão definidos em seu argumento. Assim, a chamada a seguir define `eofbit` sem afetar os outros bits:

```cpp
setstate(eofbit);
```

Portanto, se `failbit` já estava definido, ele permanece definido.

Por que você redefineria o estado do fluxo? Para um programador, a razão mais comum é usar `clear()` sem argumento para reabrir a entrada depois de encontrar entrada incompatível ou fim de arquivo; se fazer isso faz sentido depende do que o programa está tentando realizar.

O objetivo principal de `setstate()` é fornecer um meio para que funções de entrada e saída alterem o estado. Por exemplo, se `num` é um `int`, a chamada a seguir pode resultar em `operator>>(int &)` usando `setstate()` para definir `failbit` ou `eofbit`:

```cpp
cin >> num; // ler um int
```

### E/S e Exceções

Suponha que uma função de entrada defina `eofbit`. Isso causa o lançamento de uma exceção? Por padrão, a resposta é não. No entanto, você pode usar o método `exceptions()` para controlar como as exceções são tratadas.

Primeiramente, um pouco de contexto. O método `exceptions()` retorna um campo de bits com três bits correspondentes a `eofbit`, `failbit` e `badbit`. Alterar o estado do fluxo envolve `clear()` ou `setstate()`, que usa `clear()`. Após alterar o estado do fluxo, o método `clear()` compara o estado atual do fluxo com o valor retornado por `exceptions()`. Se um bit estiver definido no valor de retorno e o bit correspondente estiver definido no estado atual, `clear()` lança uma exceção `ios_base::failure`. Isso aconteceria, por exemplo, se ambos os valores tivessem `badbit` definido. Portanto, se `exceptions()` retornar `goodbit`, nenhuma exceção é lançada. A classe de exceção `ios_base::failure` deriva da classe `std::exception` e, portanto, tem um método `what()`.

A configuração padrão para `exceptions()` é `goodbit` — ou seja, nenhuma exceção é lançada. No entanto, a função sobrecarregada `exceptions(iostate)` oferece controle sobre o comportamento:

```cpp
cin.exceptions(badbit); // definir badbit causa lançamento de exceção
```

O operador OR bit a bit (`|`), conforme discutido no Apêndice E, permite especificar mais de um bit. Por exemplo, a instrução a seguir resulta no lançamento de uma exceção se `badbit` ou `eofbit` forem subsequentemente definidos:

```cpp
cin.exceptions(badbit | eofbit);
```

A Listagem 17.12 modifica a Listagem 17.11 para que o programa lance e capture uma exceção se `failbit` for definido.

**Listagem 17.12 — `cinexcecao.cpp`**

```cpp
// cinexcecao.cpp -- fazendo cin lançar uma exceção
#include <iostream>
#include <exception>

int main()
{
    using namespace std;
    // fazer failbit causar o lançamento de uma exceção
    cin.exceptions(ios_base::failbit);
    cout << "Digite números: ";
    int soma = 0;
    int entrada;
    try {
        while (cin >> entrada)
        {
            soma += entrada;
        }
    } catch(ios_base::failure & bf)
    {
        cout << bf.what() << endl;
        cout << "Oh! Que horror!\n";
    }
    cout << "Último valor digitado = " << entrada << endl;
    cout << "Soma = " << soma << endl;
    return 0;
}
```

Eis uma execução de exemplo do programa da Listagem 17.12 (a mensagem de `what()` depende da implementação):

```
Digite números: 20 30 40 pi 6
ios_base failure in clear
Oh! Que horror!
Último valor digitado = 40.00
Soma = 90.00
```

Então é assim que você pode usar exceções para entrada. Mas deveria? Depende do contexto. Para este exemplo, a resposta é não. Uma exceção deveria capturar uma ocorrência incomum e inesperada, mas este programa em particular usa uma incompatibilidade de tipo como a forma pretendida de sair do loop. No entanto, pode fazer sentido para este programa lançar uma exceção por `badbit`, porque essa circunstância seria inesperada. Ou, se o programa fosse projetado para ler números de um arquivo de dados até o fim de arquivo, poderia fazer sentido lançar uma exceção por `failbit`, porque isso representaria um problema com o arquivo de dados.

### Efeitos do Estado do Fluxo

Um teste `if` ou `while` como o seguinte é verdadeiro apenas se o estado do fluxo for bom (todos os bits limpos):

```cpp
while (cin >> entrada)
```

Se um teste falhar, você pode usar as funções-membro da Tabela 17.4 para discriminar entre as possíveis causas. Por exemplo, você poderia modificar a parte central da Listagem 17.11 para que ficasse assim:

```cpp
while (cin >> entrada)
{
    soma += entrada;
}
if (cin.eof())
    cout << "Loop encerrado porque EOF foi encontrado\n";
```

Definir um bit de estado do fluxo tem uma consequência muito importante: o fluxo fica fechado para entrada ou saída adicional até que o bit seja limpo. Por exemplo, o seguinte código não funcionará:

```cpp
while (cin >> entrada)
{
    soma += entrada;
}
cout << "Último valor digitado = " << entrada << endl;
cout << "Soma = " << soma << endl;
cout << "Agora digite um novo número: ";
cin >> entrada; // não funcionará
```

Se você quiser que um programa leia mais entrada após um bit de estado do fluxo ter sido definido, é preciso redefinir o estado do fluxo para bom. Isso pode ser feito chamando o método `clear()`:

```cpp
while (cin >> entrada)
{
    soma += entrada;
}
cout << "Último valor digitado = " << entrada << endl;
cout << "Soma = " << soma << endl;
cout << "Agora digite um novo número: ";
cin.clear();          // redefinir estado do fluxo
while (!isspace(cin.get()))
    continue;         // descartar entrada inválida
cin >> entrada;       // funcionará agora
```

Observe que não basta redefinir o estado do fluxo. A entrada incompatível que encerrou o loop de entrada ainda está na fila de entrada, e o programa tem que ignorá-la. Uma maneira é continuar lendo caracteres até atingir espaço em branco. A função `isspace()` (ver Capítulo 6, "Instruções de Ramificação e Operadores Lógicos") é uma função `cctype` que retorna `true` se seu argumento for um caractere de espaço em branco. Ou você pode descartar o resto da linha em vez de apenas a próxima palavra:

```cpp
while (cin.get() != '\n')
    continue; // descartar o restante da linha
```

Este exemplo assume que o loop foi encerrado por causa de entrada inadequada. Suponha, em vez disso, que o loop fosse encerrado por causa de fim de arquivo ou por causa de uma falha de hardware. Então o novo código que descarta a entrada inválida não faz sentido. Você pode corrigir as coisas usando o método `fail()` para testar se a suposição estava correta. Como por razões históricas `fail()` retorna `true` se `failbit` ou `eofbit` estiver definido, o código tem que excluir o último caso. O código a seguir mostra um exemplo de tal exclusão:

```cpp
while (cin >> entrada)
{
    soma += entrada;
}
cout << "Último valor digitado = " << entrada << endl;
cout << "Soma = " << soma << endl;
if (cin.fail() && !cin.eof()) // falhou por entrada incompatível
{
    cin.clear();                  // redefinir estado do fluxo
    while (!isspace(cin.get()))
        continue;                 // descartar entrada inválida
}
else // caso contrário, encerrar
{
    cout << "Não posso continuar!\n";
    exit(1);
}
cout << "Agora digite um novo número: ";
cin >> entrada; // funcionará agora
```

## Outros Métodos da Classe `istream`

Os Capítulos 3 a 5 discutem os métodos `get()` e `getline()`. Como você pode se lembrar, eles fornecem as seguintes capacidades adicionais de entrada:

- Os métodos `get(char &)` e `get(void)` fornecem entrada de caractere único que não pula o espaço em branco.
- As funções `get(char *, int, char)` e `getline(char *, int, char)` por padrão leem linhas inteiras em vez de palavras únicas.

Esses são denominados funções de entrada não formatada (unformatted input functions) porque simplesmente leem a entrada de caractere como está, sem pular o espaço em branco e sem realizar conversões de dados.

### Entrada de Caractere Único

Quando usados com um argumento `char` ou sem argumento algum, os métodos `get()` buscam o próximo caractere de entrada, mesmo que seja um espaço, tabulação ou caractere de nova linha.

**A função-membro `get(char &)`**

Vamos tentar `get(char &)` primeiro. Suponha que você tenha o seguinte loop em um programa:

```cpp
int ct = 0;
char ch;
cin.get(ch);
while (ch != '\n')
{
    cout << ch;
    ct++;
    cin.get(ch);
}
cout << ct << endl;
```

Suponha então que você digitou a seguinte entrada otimista:

```
I C++ clearly.<Enter>
```

Pressionar a tecla Enter envia esta linha de entrada ao programa. O fragmento de programa lê o caractere `I`, o exibe com `cout` e incrementa `ct` para 1. Em seguida, lê o caractere de espaço seguinte ao `I`, o exibe e incrementa `ct` para 2. Isso continua até que o programa processe a tecla Enter como um caractere de nova linha e encerre o loop. O ponto principal aqui é que, usando `get(ch)`, o código lê, exibe e conta os espaços assim como os caracteres de impressão.

Suponha, em vez disso, que o programa tentasse usar `>>`:

```cpp
int ct = 0;
char ch;
cin >> ch;
while (ch != '\n') // FALHA
{
    cout << ch;
    ct++;
    cin >> ch;
}
cout << ct << endl;
```

Primeiro, o código pularia os espaços, assim não os contando e comprimindo a saída correspondente para:

```
IC++clearly.
```

Pior ainda, o loop nunca terminaria! Como o operador de extração pula novas linhas, o código nunca atribuiria o caractere de nova linha a `ch`, portanto o teste do loop `while` nunca encerraria o loop.

A função-membro `get(char &)` retorna uma referência ao objeto `istream` usado para invocá-la. Isso significa que você pode concatenar outras extrações após `get(char &)`:

```cpp
char c1, c2, c3;
cin.get(c1).get(c2) >> c3;
```

Primeiro, `cin.get(c1)` atribui o primeiro caractere de entrada a `c1` e retorna o objeto invocador, que é `cin`. Isso reduz o código a `cin.get(c2) >> c3`, que atribui o segundo caractere de entrada a `c2`. A chamada de função retorna `cin`, reduzindo o código a `cin >> c3`. Isso, por sua vez, atribui o próximo caractere que não seja espaço em branco a `c3`. Observe que `c1` e `c2` poderiam acabar recebendo espaço em branco, mas `c3` não poderia.

Se `cin.get(char &)` encontrar o fim de arquivo, seja real ou simulado pelo teclado (Ctrl+Z para o prompt de comando do DOS e do Windows, Ctrl+D no início de uma linha para Unix), não atribui um valor ao seu argumento. Isso é bastante correto, porque se o programa chegou ao fim do arquivo, não há valor a ser atribuído. Além disso, o método chama `setstate(failbit)`, fazendo com que `cin` seja avaliado como falso:

```cpp
char ch;
while (cin.get(ch))
{
    // processar entrada
}
```

Enquanto houver entrada válida, o valor de retorno de `cin.get(ch)` é `cin`, que avalia como `true`, de modo que o loop continua. Ao atingir o fim de arquivo, o valor de retorno avalia como `false`, encerrando o loop.

**A função-membro `get(void)`**

O método `get(void)` também lê espaço em branco, mas usa seu valor de retorno para comunicar a entrada ao programa. Assim, você o usaria da seguinte forma:

```cpp
int ct = 0;
char ch;
ch = cin.get(); // usar valor de retorno
while (ch != '\n')
{
    cout << ch;
    ct++;
    ch = cin.get();
}
cout << ct << endl;
```

A função-membro `get(void)` retorna o tipo `int` (ou algum tipo inteiro maior, dependendo do conjunto de caracteres e da localidade). Isso torna o seguinte inválido:

```cpp
char c1, c2, c3;
cin.get().get() >> c3; // inválido
```

Aqui `cin.get()` retorna um valor do tipo `int`. Como esse valor de retorno não é um objeto de classe, você não pode aplicar o operador de associação a ele. Assim, você obtém um erro de sintaxe. No entanto, pode usar `get()` no final de uma sequência de extração:

```cpp
char c1;
cin.get(c1).get(); // válido
```

O fato de que `get(void)` retorna o tipo `int` significa que você não pode segui-lo com um operador de extração. Mas como `cin.get(c1)` retorna `cin`, torna-o um prefixo adequado para `get()`. Este código particular leria o primeiro caractere de entrada, atribuiria-o a `c1` e, em seguida, leria o segundo caractere de entrada e o descartaria.

Ao atingir o fim de arquivo, seja real ou simulado, `cin.get(void)` retorna o valor `EOF`, que é uma constante simbólica fornecida pelo arquivo de cabeçalho `iostream`. Esse recurso de design permite a seguinte construção para leitura de entrada:

```cpp
int ch;
while ((ch = cin.get()) != EOF)
{
    // processar entrada
}
```

Você deve usar o tipo `int` para `ch` em vez do tipo `char` aqui, porque o valor de `EOF` pode não ser expresso como tipo `char`.

O Capítulo 5, "Loops e Expressões Relacionais," descreve essas funções com um pouco mais de detalhes, e a Tabela 17.5 resume as características das funções de entrada de caractere único.

**Tabela 17.5 — `cin.get(ch)` versus `cin.get()`**

| Propriedade                         | `cin.get(ch)`                               | `ch = cin.get()`                              |
|-------------------------------------|---------------------------------------------|-----------------------------------------------|
| Método para transmitir a entrada    | Atribui ao argumento `ch`                   | Usa o valor de retorno da função para atribuir a `ch` |
| Valor de retorno para entrada de caractere | Referência a um objeto de classe `istream` | Código para o caractere como valor do tipo `int` |
| Valor de retorno no fim de arquivo  | Converte para `false`                       | `EOF`                                         |

**Qual Forma de Entrada de Caractere Único Usar?**

Dada a escolha entre `>>`, `get(char &)` e `get(void)`, qual você deve usar? Primeiro, você precisa decidir se quer que a entrada pule o espaço em branco. Se pular o espaço em branco for conveniente, você deve usar o operador de extração `>>`. Por exemplo, pular o espaço em branco é conveniente ao oferecer opções de menu:

```cpp
cout << "a. irritar cliente b. faturar cliente\n"
     << "c. acalmar cliente d. enganar cliente\n"
     << "q.\n";
cout << "Digite a, b, c, d ou q: ";
char ch;
cin >> ch;
while (ch != 'q')
{
    switch(ch)
    {
        ...
    }
    cout << "Digite a, b, c, d ou q: ";
    cin >> ch;
}
```

Para digitar, digamos, uma resposta `b`, você digita `b` e pressiona Enter, gerando a resposta de dois caracteres `b\n`. Se você usasse qualquer forma de `get()`, teria que adicionar código para processar esse caractere `\n` em cada ciclo do loop, mas o operador de extração o ignora convenientemente.

Se você quiser que um programa examine cada caractere, deve usar um dos métodos `get()`. Por exemplo, um programa de contagem de palavras poderia usar o espaço em branco para determinar quando uma palavra chega ao fim. Dos dois métodos `get()`, o método `get(char &)` tem a interface mais elegante. A principal vantagem do método `get(void)` é que ele se assemelha muito à função `getchar()` padrão do C, o que significa que você pode converter um programa C para um programa C++ incluindo `iostream` em vez de `stdio.h`, substituindo globalmente `getchar()` por `cin.get()` e substituindo globalmente o `putchar(ch)` do C por `cout.put(ch)`.

### Entrada de String: `getline()`, `get()` e `ignore()`

Vamos revisar as funções-membro de entrada de string introduzidas no Capítulo 4, "Tipos Compostos". A função-membro `getline()` e a versão de leitura de string de `get()` leem strings, e ambas têm as mesmas assinaturas de função (aqui simplificadas a partir da declaração de template mais geral):

```cpp
istream & get(char *, int, char);
istream & get(char *, int);
istream & getline(char *, int, char);
istream & getline(char *, int);
```

O primeiro argumento, lembre-se, é o endereço da localização em que colocar a string de entrada. O segundo argumento é um a mais do que o número máximo de caracteres a serem lidos. (O caractere adicional deixa espaço para o caractere nulo de terminação usado ao armazenar a entrada como uma string.) O terceiro argumento especifica um caractere para atuar como delimitador da entrada. As versões com apenas dois argumentos usam o caractere de nova linha como delimitador. Cada função lê até o número máximo de caracteres ou até encontrar o caractere delimitador, o que vier primeiro.

Por exemplo, o seguinte código lê a entrada de caracteres no array de caracteres `linha`:

```cpp
char linha[50];
cin.get(linha, 50);
```

A função `cin.get()` para de ler a entrada no array após encontrar 49 caracteres ou, por padrão, após encontrar um caractere de nova linha, o que vier primeiro. A principal diferença entre `get()` e `getline()` é que `get()` deixa o caractere de nova linha no fluxo de entrada, tornando-o o primeiro caractere visto pela próxima operação de entrada, enquanto `getline()` extrai e descarta o caractere de nova linha do fluxo de entrada.

O Capítulo 4 ilustrou o uso da forma de dois argumentos para essas duas funções-membro. Agora vamos ver as versões de três argumentos. O terceiro argumento é o caractere delimitador. Encontrar o caractere delimitador faz a entrada cessar, mesmo que o número máximo de caracteres ainda não tenha sido atingido. Portanto, por padrão, ambos os métodos param de ler a entrada se chegarem ao fim de uma linha antes de ler o número alocado de caracteres. Assim como no caso padrão, `get()` deixa o caractere delimitador na fila de entrada, e `getline()` não.

A Listagem 17.13 demonstra como `getline()` e `get()` funcionam. Ela também apresenta a função-membro `ignore()`. `ignore()` recebe dois argumentos: um número que especifica um número máximo de caracteres a serem lidos e um caractere que atua como caractere delimitador para a entrada. Por exemplo, a seguinte chamada de função lê e descarta os próximos 255 caracteres ou até o primeiro caractere de nova linha, o que vier primeiro:

```cpp
cin.ignore(255, '\n');
```

O protótipo fornece valores padrão de 1 e `EOF` para os dois argumentos, e o tipo de retorno da função é `istream &`:

```cpp
istream & ignore(int = 1, int = EOF);
```

(O valor padrão `EOF` faz `ignore()` ler até o número especificado de caracteres ou até o fim de arquivo, o que vier primeiro.)

A função retorna o objeto invocador. Isso permite concatenar chamadas de função, como no seguinte:

```cpp
cin.ignore(255, '\n').ignore(255, '\n');
```

Aqui, o primeiro método `ignore()` lê e descarta uma linha, e a segunda chamada lê e descarta a segunda linha. Juntas, as duas funções leem duas linhas.

**Listagem 17.13 — `obter.cpp`**

```cpp
// obter.cpp -- usando get() e getline()
#include <iostream>
const int Limite = 255;
int main()
{
    using std::cout;
    using std::cin;
    using std::endl;
    char entrada[Limite];

    cout << "Digite uma string para processamento com getline():\n";
    cin.getline(entrada, Limite, '#');
    cout << "Eis sua entrada:\n";
    cout << entrada << "\nFase 1 concluída\n";
    char ch;
    cin.get(ch);
    cout << "O próximo caractere de entrada é " << ch << endl;
    if (ch != '\n')
        cin.ignore(Limite, '\n'); // descartar restante da linha

    cout << "Digite uma string para processamento com get():\n";
    cin.get(entrada, Limite, '#');
    cout << "Eis sua entrada:\n";
    cout << entrada << "\nFase 2 concluída\n";
    cin.get(ch);
    cout << "O próximo caractere de entrada é " << ch << endl;

    return 0;
}
```

Eis uma execução de exemplo do programa da Listagem 17.13:

```
Digite uma string para processamento com getline():
Please pass
me a #3 melon!
Eis sua entrada:
Please pass
me a
Fase 1 concluída
O próximo caractere de entrada é 3
Digite uma string para processamento com get():
I still
want my #3 melon!
Eis sua entrada:
I still
want my
Fase 2 concluída
O próximo caractere de entrada é #
```

Observe que a função `getline()` descarta o caractere de terminação `#` na entrada, e a função `get()` não.

**Entrada de String Inesperada**

Algumas formas de entrada para `get(char *, int)` e `getline()` afetam o estado do fluxo. Assim como com as outras funções de entrada, encontrar o fim de arquivo define `eofbit`, e qualquer coisa que corrompa o fluxo, como falha de dispositivo, define `badbit`. Dois outros casos especiais são a ausência de entrada e a entrada que atinge ou excede o número máximo de caracteres especificado pela chamada de função.

Se qualquer um dos métodos não conseguir extrair nenhum caractere, o método coloca um caractere nulo na string de entrada e usa `setstate()` para definir `failbit`. Quando um método falharia em extrair qualquer caractere? Uma possibilidade é se um método de entrada encontrar imediatamente o fim de arquivo. Para `get(char *, int)`, outra possibilidade é se você digitar uma linha vazia:

```cpp
char temp[80];
while (cin.get(temp, 80)) // encerra em linha vazia
    ...
```

Curiosamente, uma linha vazia não faz `getline()` definir `failbit`. Isso porque `getline()` ainda extrai o caractere de nova linha, mesmo que não o armazene. Se você quiser que um loop com `getline()` encerre em uma linha vazia, pode escrevê-lo desta forma:

```cpp
char temp[80];
while (cin.getline(temp, 80) && temp[0] != '\0') // encerra em linha vazia
```

Agora, suponha que o número de caracteres na fila de entrada atinja ou exceda o máximo especificado pelo método de entrada. Considere primeiro `getline()`. O método `getline()` definirá `failbit` e encerrará a entrada se 29 caracteres tiverem sido armazenados sem encontrar a nova linha. Assim, uma linha de entrada de 30 ou mais caracteres encerrará a entrada.

Agora considere o método `get(char *, int)`. Ele testa o número de caracteres primeiro, o fim de arquivo segundo, e se o próximo caractere é uma nova linha por último. Ele não define o flag `failbit` se ler o número máximo de caracteres. No entanto, você pode saber se houve muitos caracteres de entrada que causaram a parada do método. Você pode usar `peek()` (ver a próxima seção) para examinar o próximo caractere de entrada. Se for uma nova linha, então `get()` deve ter lido a linha inteira. Se não for uma nova linha, então `get()` deve ter parado antes de atingir o fim. A Tabela 17.6 resume esses comportamentos.

**Tabela 17.6 — Comportamento de Entrada**

| Método              | Comportamento                                                                                 |
|---------------------|-----------------------------------------------------------------------------------------------|
| `getline(char*, int)` | Define `failbit` se nenhum caractere for lido (mas a nova linha conta como um caractere lido). Define `failbit` se o número máximo de caracteres for lido e ainda houver mais na linha. |
| `get(char*, int)`   | Define `failbit` se nenhum caractere for lido.                                                |

### Outros Métodos de `istream`

Outros métodos de `istream` além dos discutidos até agora incluem `read()`, `peek()`, `gcount()` e `putback()`. A função `read()` lê um número dado de bytes e os armazena na localização especificada. Por exemplo, as instruções a seguir leem 144 caracteres da entrada padrão e os colocam no array `bruto`:

```cpp
char bruto[144];
cin.read(bruto, 144);
```

Diferentemente de `getline()` e `get()`, `read()` não acrescenta um caractere nulo à entrada, portanto não converte a entrada para a forma de string. O método `read()` não se destina à entrada pelo teclado. Em vez disso, é usado com mais frequência em conjunto com a função `write()` de `ostream` para entrada e saída de arquivos. O tipo de retorno do método é `istream &`, portanto pode ser concatenado da seguinte forma:

```cpp
char bruto[144];
char pontuacao[20];
cin.read(bruto, 144).read(pontuacao, 20);
```

A função `peek()` retorna o próximo caractere da entrada sem extraí-lo do fluxo de entrada. Ou seja, ela permite que você espie o próximo caractere. Suponha que você queira ler a entrada até a primeira nova linha ou ponto final, o que vier primeiro. Você pode usar `peek()` para espiar o próximo caractere no fluxo de entrada a fim de julgar se deve continuar:

```cpp
char grande_entrada[80];
char ch;
int i = 0;
while ((ch = cin.peek()) != '.' && ch != '\n')
    cin.get(grande_entrada[i++]);
grande_entrada[i] = '\0';
```

A chamada a `cin.peek()` espia o próximo caractere de entrada e atribui seu valor a `ch`. Em seguida, a condição de teste do loop `while` verifica que `ch` não é um ponto final nem uma nova linha. Se esse for o caso, o loop lê o caractere no array e atualiza o índice do array. Quando o loop termina, o ponto final ou o caractere de nova linha permanece no fluxo de entrada, posicionado para ser o primeiro caractere lido pela próxima operação de entrada. Em seguida, o código acrescenta um caractere nulo ao array, tornando-o uma string.

O método `gcount()` retorna o número de caracteres lidos pelo último método de extração não formatada. Isso significa caracteres lidos por um método `get()`, `getline()`, `ignore()` ou `read()`, mas não pelo operador de extração (`>>`), que formata a entrada para ajustar tipos de dados específicos. Por exemplo, suponha que você acabou de usar `cin.get(meuarray, 80)` para ler uma linha no array `meuarray` e quer saber quantos caracteres foram lidos. Você poderia usar a função `strlen()` para contar os caracteres no array, mas seria mais rápido usar `cin.gcount()` para relatar quantos caracteres foram acabados de ler do fluxo de entrada.

A função `putback()` insere um caractere de volta na string de entrada. O caractere inserido torna-se então o primeiro caractere lido pela próxima instrução de entrada. O método `putback()` recebe um argumento `char`, que é o caractere a ser inserido, e retorna o tipo `istream &`, que permite que a chamada seja concatenada com outros métodos `istream`. Usar `peek()` é como usar `get()` para ler um caractere e depois usar `putback()` para devolver o caractere ao fluxo de entrada. No entanto, `putback()` oferece a opção de devolver um caractere diferente do que acabou de ser lido.

A Listagem 17.14 usa duas abordagens para ler e ecoar a entrada até, mas não incluindo, um caractere `#`. A primeira abordagem lê através do caractere `#` e depois usa `putback()` para inserir o caractere de volta na entrada. A segunda abordagem usa `peek()` para antecipar antes de ler a entrada.

**Listagem 17.14 — `espiar.cpp`**

```cpp
// espiar.cpp -- alguns métodos de istream
#include <iostream>

int main()
{
    using std::cout;
    using std::cin;
    using std::endl;
    // ler e ecoar entrada até o caractere #
    char ch;

    while(cin.get(ch)) // encerra no EOF
    {
        if (ch != '#')
            cout << ch;
        else
        {
            cin.putback(ch); // reinserir caractere
            break;
        }
    }
    if (!cin.eof())
    {
        cin.get(ch);
        cout << endl << ch << " é o próximo caractere de entrada.\n";
    }
    else
    {
        cout << "Fim de arquivo alcançado.\n";
        std::exit(0);
    }
    while(cin.peek() != '#') // antecipar
    {
        cin.get(ch);
        cout << ch;
    }
    if (!cin.eof())
    {
        cin.get(ch);
        cout << endl << ch << " é o próximo caractere de entrada.\n";
    }
    else
        cout << "Fim de arquivo alcançado.\n";
    return 0;
}
```

Eis uma execução de exemplo do programa da Listagem 17.14:

```
I used a #3 pencil when I should have used a #2.
I used a
# é o próximo caractere de entrada.
3 pencil when I should have used a
# é o próximo caractere de entrada.
```

**Notas do Programa**

Vamos examinar mais de perto parte do código da Listagem 17.14. A primeira abordagem usa um loop `while` para ler a entrada:

```cpp
while(cin.get(ch)) // encerra no EOF
{
    if (ch != '#')
        cout << ch;
    else
    {
        cin.putback(ch); // reinserir caractere
        break;
    }
}
```

A expressão `cin.get(ch)` retorna `false` ao atingir a condição de fim de arquivo, portanto simular o fim de arquivo pelo teclado encerra o loop. Se o caractere `#` aparecer primeiro, o programa devolve o caractere ao fluxo de entrada e usa uma instrução `break` para encerrar o loop.

A segunda abordagem é mais simples em aparência:

```cpp
while(cin.peek() != '#') // antecipar
{
    cin.get(ch);
    cout << ch;
}
```

O programa espia o próximo caractere. Se não for o caractere `#`, o programa lê o próximo caractere, o ecoa e espia o próximo caractere. Isso continua até que o caractere de terminação apareça.

A Listagem 17.15, como prometido, usa `peek()` para determinar se uma linha inteira foi lida. Se apenas parte de uma linha couber no array de entrada, o programa descarta o restante da linha.

**Listagem 17.15 — `truncar.cpp`**

```cpp
// truncar.cpp -- usando get() para truncar a linha de entrada, se necessário
#include <iostream>
const int COMP = 10;
inline void descartarLinha() { while (std::cin.get() != '\n') continue; }
int main()
{
    using std::cin;
    using std::cout;
    using std::endl;
    char nome[COMP];
    char titulo[COMP];
    cout << "Digite seu nome: ";
    cin.get(nome, COMP);
    if (cin.peek() != '\n')
        cout << "Desculpe, só temos espaço para "
             << nome << endl;
    descartarLinha();
    cout << "Caro(a) " << nome << ", digite seu título: \n";
    cin.get(titulo, COMP);
    if (cin.peek() != '\n')
        cout << "Fomos forçados a truncar seu título.\n";
    descartarLinha();
    cout << "  Nome: " << nome
         << "\nTítulo: " << titulo << endl;
    return 0;
}
```

Eis uma execução de exemplo do programa da Listagem 17.15:

```
Digite seu nome: Ella Fishsniffer
Desculpe, só temos espaço para Ella Fish
Caro(a) Ella Fish, digite seu título:
Executive Adjunct
Fomos forçados a truncar seu título.
  Nome: Ella Fish
Título: Executive
```

Observe que o seguinte código faz sentido quer a primeira instrução de entrada leia a linha inteira ou não:

```cpp
while (cin.get() != '\n') continue;
```

Se `get()` leu a linha inteira, ainda deixa a nova linha no lugar, e este código a lê e descarta. Se `get()` leu apenas parte da linha, este código lê e descarta o restante da linha.

## Entrada e Saída de Arquivos

A maioria dos programas de computador trabalha com arquivos. Processadores de texto criam arquivos de documentos. Programas de banco de dados criam e pesquisam arquivos de informações. Compiladores leem arquivos de código-fonte e geram arquivos executáveis. Um arquivo em si é um conjunto de bytes armazenados em algum dispositivo, talvez fita magnética, talvez disco óptico, disquete ou disco rígido. Tipicamente, o sistema operacional gerencia arquivos, controlando suas localizações, seus tamanhos, quando foram criados e assim por diante. A menos que você esteja programando no nível do sistema operacional, normalmente não precisa se preocupar com essas coisas. O que você precisa é de uma forma de conectar um programa a um arquivo, uma forma de fazer um programa ler o conteúdo de um arquivo e uma forma de fazer um programa criar e gravar em arquivos. O redirecionamento (conforme discutido anteriormente neste capítulo) pode fornecer algum suporte a arquivos, mas é mais limitado do que a E/S de arquivo explícita de dentro de um programa. Além disso, o redirecionamento vem do sistema operacional, não do C++, portanto não está disponível em todos os sistemas.

O pacote de classes de E/S do C++ trata a entrada e saída de arquivo da mesma forma que trata a entrada e saída padrão. Para escrever em um arquivo, você cria um objeto `ofstream` e usa os métodos `ostream`, como o operador de inserção `<<` ou `write()`. Para ler um arquivo, você cria um objeto `ifstream` e usa os métodos `istream`, como o operador de extração `>>` ou `get()`. Os arquivos exigem mais gerenciamento do que a entrada e saída padrão, no entanto. Por exemplo, você precisa associar um arquivo recém-aberto a um fluxo. Você pode abrir um arquivo no modo somente leitura, somente gravação ou leitura e gravação. Se você gravar em um arquivo, pode querer criar um novo arquivo, substituir um arquivo antigo ou acrescentar a um arquivo antigo. Ou pode querer avançar e retroceder em um arquivo. Para ajudar nessas tarefas, o C++ define várias novas classes no arquivo de cabeçalho `fstream` (antigamente `fstream.h`), incluindo a classe `ifstream` para entrada de arquivo e a classe `ofstream` para saída de arquivo. O C++ também define a classe `fstream` para E/S simultânea de arquivo. Essas classes são derivadas das classes no arquivo de cabeçalho `iostream`, portanto os objetos dessas novas classes podem usar os métodos que você já aprendeu.

### E/S de Arquivo Simples

Suponha que você queira que um programa escreva em um arquivo. Você deve fazer o seguinte:

1. Criar um objeto `ofstream` para gerenciar o fluxo de saída.
2. Associar esse objeto a um arquivo específico.
3. Usar o objeto da mesma forma que você usaria `cout`; a única diferença é que a saída vai para o arquivo em vez de para a tela.

Para isso, você começa incluindo o arquivo de cabeçalho `fstream`. Incluir este arquivo inclui automaticamente o arquivo `iostream` para a maioria das implementações, mas não todas, portanto pode não ser necessário incluir `iostream` explicitamente. Em seguida, você declara um objeto `ofstream`:

```cpp
ofstream fout; // criar um objeto ofstream chamado fout
```

O nome do objeto pode ser qualquer nome válido de C++, como `fout`, `arquivoSaida`, `portao` ou `didi`.

Em seguida, você deve associar esse objeto a um arquivo específico. Você pode fazer isso usando o método `open()`. Suponha, por exemplo, que você queira abrir o arquivo `jarra.txt` para saída. Você faria o seguinte:

```cpp
fout.open("jarra.txt"); // associar fout com jarra.txt
```

Você pode combinar esses dois passos (criar o objeto e associar um arquivo) em uma única instrução usando um construtor diferente:

```cpp
ofstream fout("jarra.txt"); // criar fout, associar com jarra.txt
```

Depois disso, você usa `fout` (ou qualquer nome que escolher) da mesma forma que `cout`. Por exemplo, se você quiser colocar as palavras `Dados Chatos` no arquivo, pode fazer o seguinte:

```cpp
fout << "Dados Chatos";
```

Como `ostream` é uma classe base para a classe `ofstream`, você pode usar todos os métodos de `ostream`, incluindo as várias definições do operador de inserção e os métodos e manipuladores de formatação. A classe `ofstream` usa saída em buffer, portanto o programa aloca espaço para um buffer de saída ao criar um objeto `ofstream` como `fout`. Se você criar dois objetos `ofstream`, o programa criará dois buffers, um para cada objeto. Um objeto `ofstream` como `fout` coleta saída byte a byte do programa; em seguida, quando o buffer está cheio, ele transfere os conteúdos do buffer em massa para o arquivo de destino. Como os drives de disco são projetados para transferir dados em pedaços maiores, não byte a byte, a abordagem com buffer acelera enormemente a taxa de transferência de dados de um programa para um arquivo.

Abrir um arquivo para saída dessa forma cria um novo arquivo se não houver nenhum arquivo com esse nome. Se um arquivo com esse nome existir antes de ser aberto para saída, o ato de abri-lo o trunca para que a saída comece com um arquivo limpo.

> **Atenção:** Abrir um arquivo para saída no modo padrão trunca automaticamente o arquivo para zero tamanho, descartando o conteúdo anterior.

Os requisitos para leitura de um arquivo são muito semelhantes aos requisitos para escrita em um arquivo:

1. Criar um objeto `ifstream` para gerenciar o fluxo de entrada.
2. Associar esse objeto a um arquivo específico.
3. Usar o objeto da mesma forma que você usaria `cin`.

Os passos para leitura de um arquivo são similares aos de escrita. Primeiro, você inclui o arquivo de cabeçalho `fstream`. Em seguida, você declara um objeto `ifstream` e o associa ao nome do arquivo. Você pode fazer isso em duas instruções ou em uma:

```cpp
// duas instruções
ifstream fin;          // criar objeto ifstream chamado fin
fin.open("potedegeleia.txt"); // abrir para leitura
// uma instrução
ifstream fis("potedegeleia.txt"); // criar fis e associar com o arquivo
```

Você pode então usar `fin` ou `fis` da mesma forma que usaria `cin`. Por exemplo, você pode usar o seguinte:

```cpp
char ch;
fin >> ch;          // ler um caractere do arquivo
char buf[80];
fin >> buf;         // ler uma palavra do arquivo
fin.getline(buf, 80); // ler uma linha do arquivo
string linha;
getline(fin, linha); // ler do arquivo para um objeto string
```

A entrada, como a saída, é armazenada em buffer, portanto criar um objeto `ifstream` como `fin` cria um buffer de entrada, que o objeto `fin` gerencia. Assim como na saída, o armazenamento em buffer move dados muito mais rápido do que a transferência byte a byte.

As conexões com um arquivo são fechadas automaticamente quando os objetos de fluxo de entrada e saída expiram — por exemplo, quando o programa termina. Além disso, você pode fechar explicitamente uma conexão com um arquivo usando o método `close()`:

```cpp
fout.close(); // fechar conexão de saída com o arquivo
fin.close();  // fechar conexão de entrada com o arquivo
```

Fechar tal conexão não elimina o fluxo; apenas o desconecta do arquivo. No entanto, o aparato de gerenciamento do fluxo permanece em vigor. Por exemplo, o objeto `fin` ainda existe, junto com o buffer de entrada que ele gerencia. Como você verá mais adiante, pode reconectar o fluxo ao mesmo arquivo ou a outro arquivo.

Vamos ver um exemplo breve. O programa da Listagem 17.16 solicita um nome de arquivo. Ele cria um arquivo com esse nome, grava algumas informações nele e fecha o arquivo. Fechar o arquivo descarrega o buffer, garantindo que o arquivo seja atualizado. Em seguida, o programa abre o mesmo arquivo para leitura e exibe seu conteúdo. Observe que o programa usa `fin` e `fout` da mesma maneira que usaria `cin` e `cout`. Além disso, o programa lê o nome do arquivo para um objeto `string` e depois usa o método `c_str()` para fornecer argumentos de string no estilo C para os construtores de `ofstream` e `ifstream`.

**Listagem 17.16 — `arqes.cpp`**

```cpp
// arqes.cpp -- salvando em arquivo
#include <iostream> // não necessário para muitos sistemas
#include <fstream>
#include <string>

int main()
{
    using namespace std;
    string nomarq;
    cout << "Digite um nome para o novo arquivo: ";
    cin >> nomarq;
    // criar objeto de fluxo de saída para o novo arquivo
    ofstream fout(nomarq.c_str());
    fout << "Só para seus olhos!\n"; // escrever no arquivo
    cout << "Digite seu número secreto: "; // escrever na tela
    float segredo;
    cin >> segredo;
    fout << "Seu número secreto é " << segredo << endl;
    fout.close(); // fechar arquivo
    // criar objeto de fluxo de entrada para o novo arquivo
    ifstream fin(nomarq.c_str());
    cout << "Eis o conteúdo de " << nomarq << ":\n";
    char ch;
    while (fin.get(ch)) // ler caractere do arquivo e
        cout << ch;     // exibi-lo na tela
    cout << "Concluído\n";
    fin.close();
    return 0;
}
```

Eis uma execução de exemplo do programa da Listagem 17.16:

```
Digite um nome para o novo arquivo: pythag
Digite seu número secreto: 3.14159
Eis o conteúdo de pythag:
Só para seus olhos!
Seu número secreto é 3.14159
Concluído
```

Se você verificar o diretório que contém o programa, deverá encontrar um arquivo chamado `pythag`, e qualquer editor de texto deverá mostrar o mesmo conteúdo que a saída do programa exibe.

### Verificação do Fluxo e `is_open()`

As classes de fluxo de arquivo do C++ herdam um membro de estado do fluxo da classe `ios_base`. Esse membro, como discutido anteriormente, armazena informações que refletem o status do fluxo: tudo está bem, o fim de arquivo foi atingido, a operação de E/S falhou e assim por diante. Se tudo estiver bem, o estado do fluxo é zero (sem notícia, boa notícia). Os outros estados são registrados definindo bits específicos como 1. As classes de fluxo de arquivo também herdam os métodos de `ios_base` que relatam sobre o estado do fluxo e que são resumidos na Tabela 17.4. Você pode verificar o estado do fluxo para descobrir se a operação de fluxo mais recente foi bem-sucedida ou falhou. Para fluxos de arquivo, isso inclui verificar o sucesso de uma tentativa de abrir um arquivo. Por exemplo, tentar abrir um arquivo inexistente para entrada define `failbit`. Portanto, você poderia verificar desta forma:

```cpp
fin.open(argv[arquivo]);
if (fin.fail()) // tentativa de abertura falhou
{
    ...
}
```

Ou, como um objeto `ifstream`, assim como um objeto `istream`, é convertido para o tipo `bool` onde um tipo `bool` é esperado, você poderia usar isto:

```cpp
fin.open(argv[arquivo]);
if (!fin) // tentativa de abertura falhou
{
    ...
}
```

No entanto, implementações mais recentes do C++ têm uma maneira melhor de verificar se um arquivo foi aberto — o método `is_open()`:

```cpp
if (!fin.is_open()) // tentativa de abertura falhou
{
    ...
}
```

A razão pela qual isso é melhor é que ele testa alguns problemas sutis que as outras formas perdem, como discutido no Aviso a seguir.

> **Atenção:** No passado, os testes usuais para a abertura bem-sucedida de um arquivo eram os seguintes:
>
> ```cpp
> if(fin.fail()) ... // falha ao abrir
> if(!fin.good()) ... // falha ao abrir
> if (!fin) ... // falha ao abrir
> ```
>
> O objeto `fin`, quando usado em uma condição de teste, é convertido para `false` se `fin.good()` for `false` e para `true` caso contrário, portanto as duas formas são equivalentes. No entanto, esses testes não detectam uma circunstância: a tentativa de abrir um arquivo usando um modo de arquivo inapropriado (ver a seção "Modos de Arquivo" no próximo arquivo). O método `is_open()` detecta essa forma de erro, além dos detectados pelo método `good()`. No entanto, implementações mais antigas do C++ não possuem `is_open()`.

### Abrindo Vários Arquivos

Você pode precisar que um programa abra mais de um arquivo. A estratégia para abrir vários arquivos depende de como eles serão usados. Se você precisar de dois arquivos abertos simultaneamente, deve criar um fluxo separado para cada arquivo. Por exemplo, um programa que intercala dois arquivos classificados em um terceiro arquivo criaria dois objetos `ifstream` para os dois arquivos de entrada e um objeto `ofstream` para o arquivo de saída. O número de arquivos que você pode abrir simultaneamente depende do sistema operacional.

No entanto, você pode planejar processar um grupo de arquivos sequencialmente. Por exemplo, você pode querer contar quantas vezes um nome aparece em um conjunto de 10 arquivos. Nesse caso, você pode abrir um único fluxo e associá-lo a cada arquivo por sua vez. Isso conserva recursos do computador de forma mais eficaz do que abrir um fluxo separado para cada arquivo. Para usar essa abordagem, você declara um objeto `ifstream` sem inicializá-lo e depois usa o método `open()` para associar o fluxo a um arquivo. Por exemplo, é assim que você poderia lidar com a leitura de dois arquivos em sucessão:

```cpp
ifstream fin;       // criar fluxo usando construtor padrão
fin.open("gordo.txt"); // associar fluxo com gordo.txt
...                 // fazer o que precisar
fin.close();        // encerrar associação com gordo.txt
fin.clear();        // redefinir fin (pode não ser necessário)
fin.open("rato.txt");  // associar fluxo com rato.txt
...
fin.close();
```

### Processamento de Linha de Comando

Programas de processamento de arquivo frequentemente usam argumentos de linha de comando para identificar arquivos. Argumentos de linha de comando (command-line arguments) são argumentos que aparecem na linha de comando quando você digita um comando. Por exemplo, para contar o número de palavras em alguns arquivos em um sistema Unix ou Linux, você digitaria esse comando no prompt de linha de comando:

```
wc relatorio1 relatorio2 relatorio3
```

Aqui `wc` é o nome do programa, e `relatorio1`, `relatorio2` e `relatorio3` são nomes de arquivo passados ao programa como argumentos de linha de comando.

O C++ tem um mecanismo para deixar um programa em execução em um ambiente de linha de comando acessar os argumentos de linha de comando. Você pode usar o seguinte cabeçalho alternativo para `main()`:

```cpp
int main(int argc, char *argv[])
```

O argumento `argc` representa o número de argumentos na linha de comando. A contagem inclui o próprio nome do comando. A variável `argv` é um ponteiro para um ponteiro para `char`. Isso soa um pouco abstrato, mas você pode tratar `argv` como se fosse um array de ponteiros para os argumentos de linha de comando, com `argv[0]` sendo um ponteiro para o primeiro caractere de uma string contendo o nome do comando, `argv[1]` sendo um ponteiro para o primeiro caractere de uma string contendo o primeiro argumento de linha de comando e assim por diante.

Por exemplo, suponha que você tenha a seguinte linha de comando:

```
wc relatorio1 relatorio2 relatorio3
```

Nesse caso, `argc` seria 4, `argv[0]` seria `wc`, `argv[1]` seria `relatorio1` e assim por diante. O seguinte loop imprimiria cada argumento de linha de comando em uma linha separada:

```cpp
for (int i = 1; i < argc; i++)
    cout << argv[i] << endl;
```

Começar com `i = 1` imprime apenas os argumentos de linha de comando; começar com `i = 0` também imprimiria o nome do comando.

A Listagem 17.17 combina a técnica de linha de comando com técnicas de fluxo de arquivo para contar caracteres em arquivos listados na linha de comando.

**Listagem 17.17 — `contar.cpp`**

```cpp
// contar.cpp -- contando caracteres em uma lista de arquivos
#include <iostream>
#include <fstream>
#include <cstdlib> // para exit()
int main(int argc, char * argv[])
{
    using namespace std;
    if (argc == 1) // sair se nenhum argumento
    {
        cerr << "Uso: " << argv[0] << " nome_arquivo[s]\n";
        exit(EXIT_FAILURE);
    }
    ifstream fin;   // abrir fluxo
    long contagem;
    long total = 0;
    char ch;
    for (int arquivo = 1; arquivo < argc; arquivo++)
    {
        fin.open(argv[arquivo]); // conectar fluxo a argv[arquivo]
        if (!fin.is_open())
        {
            cerr << "Não foi possível abrir " << argv[arquivo] << endl;
            fin.clear();
            continue;
        }
        contagem = 0;
        while (fin.get(ch))
            contagem++;
        cout << contagem << " caracteres em " << argv[arquivo] << endl;
        total += contagem;
        fin.clear();  // necessário para algumas implementações
        fin.close();  // desconectar arquivo
    }
    cout << total << " caracteres em todos os arquivos\n";
    return 0;
}
```

> **Nota:** Algumas implementações do C++ exigem o uso de `fin.clear()` ao final do programa, e outras não. Depende se associar um novo arquivo ao objeto `ifstream` redefine automaticamente o estado do fluxo. Não há mal nenhum em usar `fin.clear()` mesmo que não seja necessário.

Em um sistema Linux, por exemplo, você poderia compilar a Listagem 17.17 para um arquivo executável chamado `a.out`. Em seguida, execuções de exemplo poderiam ser assim:

```
$ a.out
Uso: a.out nome_arquivo[s]
$ a.out paris roma
3580 caracteres em paris
4886 caracteres em roma
8466 caracteres em todos os arquivos
$
```

Observe que o programa usa `cerr` para a mensagem de erro. Um detalhe menor é que a mensagem usa `argv[0]` em vez do nome do arquivo executável: assim, se você alterar o nome do arquivo executável, o programa usará automaticamente o novo nome.

---

[Anterior](capitulo-17-01-streams-entrada-saida.md) | [Índice](README.md) | [Próximo](capitulo-17-03-arquivo-avancado-resumo.md)
