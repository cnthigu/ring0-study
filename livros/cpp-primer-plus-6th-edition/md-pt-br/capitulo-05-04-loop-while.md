# Capitulo 5 - Loops e Expressoes Relacionais (Loops and Relational Expressions)
## Parte 4: O Loop while e Aliases de Tipo

> Traducao nao oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Navegacao: [Anterior](capitulo-05-03-expressoes-relacionais.md) | [Indice](README.md) | [Proximo](capitulo-05-05-loop-dowhile-for-faixa.md)

---

## O Loop while

O loop `while` e um loop `for` sem as partes de inicializacao e atualizacao; ele tem apenas uma condicao de teste e um corpo:

```
while (condicao-de-teste)
    corpo
```

Primeiro, um programa avalia a expressao `condicao-de-teste` entre parenteses. Se a expressao avaliar como `true`, o programa executa as instrucoes no corpo. Assim como com um loop `for`, o corpo consiste em uma unica instrucao ou em um bloco definido por chaves emparelhadas. Apos terminar com o corpo, o programa retorna a condicao de teste e a reavalia. Se a condicao for diferente de zero, o programa executa o corpo novamente. Esse ciclo de teste e execucao continua ate que a condicao de teste avalie como `false` (consulte a Figura 5.3). Claramente, se voce quiser que o loop termine eventualmente, algo dentro do corpo do loop deve fazer algo para afetar a expressao `condicao-de-teste`. Por exemplo, o loop pode incrementar uma variavel usada na condicao de teste ou ler um novo valor da entrada do teclado. Como o loop `for`, o loop `while` e um loop de condicao de entrada (entry-condition loop). Assim, se `condicao-de-teste` avaliar como `false` no inicio, o programa nunca executa o corpo do loop.

A Listagem 5.13 coloca um loop `while` para trabalhar. O loop percorre cada caractere em uma string e exibe o caractere e seu codigo ASCII. O loop para quando alcanca o caractere nulo. Essa tecnica de percorrer uma string caractere por caractere ate alcancar o caractere nulo e um metodo C++ padrao para processar strings de estilo C. Como uma string contem seu proprio marcador de terminacao, os programas frequentemente nao precisam de informacoes explicitas sobre o comprimento de uma string.

**Listagem 5.13 — while.cpp**

```cpp
// while.cpp -- apresentando o loop while
#include <iostream>
const int TamArr = 20;
int main()
{
    using namespace std;
    char nome[TamArr];
    cout << "Seu primeiro nome, por favor: ";
    cin >> nome;
    cout << "Aqui esta seu nome, verticalizado e ASCIIficado:\n";
    int i = 0;       // comeca no inicio da string
    while (nome[i] != '\0') // processa ate o final da string
    {
        cout << nome[i] << ": " << int(nome[i]) << endl;
        i++;          // nao esqueca desta etapa
    }
    return 0;
}
```

Aqui esta uma execucao de exemplo do programa na Listagem 5.13:

```
Seu primeiro nome, por favor: Muffy
Aqui esta seu nome, verticalizado e ASCIIficado:
M: 77
u: 117
f: 102
f: 102
y: 121
```

(Nao, verticalizado e ASCIIficado nao sao palavras reais. Mas elas adicionam um tom tecnoide encantador a saida.)

**Notas do Programa**

A condicao do `while` na Listagem 5.13 parece assim:

```cpp
while (nome[i] != '\0')
```

Ela testa se um determinado caractere no array e o caractere nulo. Para que este teste seja eventualmente bem-sucedido, o corpo do loop precisa alterar o valor de `i`. Ele o faz incrementando `i` ao final do corpo do loop. Omitir essa etapa mantem o loop preso no mesmo elemento do array, imprimindo o caractere e seu codigo ate que voce consiga encerrar o programa. Obter esse loop infinito e um dos problemas mais comuns com loops. Muitas vezes voce pode causa-lo quando se esquece de atualizar algum valor dentro do corpo do loop.

Voce pode reescrever a linha do `while` desta forma:

```cpp
while (nome[i])
```

Com essa mudanca, o programa funciona exatamente como antes. Isso e porque quando `nome[i]` e um caractere comum, seu valor e o codigo do caractere, que e diferente de zero, ou `true`. Mas quando `nome[i]` e o caractere nulo, seu valor de codigo de caractere e 0, ou `false`. Essa notacao e mais concisa (e mais comumente usada), mas menos clara do que a usada na Listagem 5.13. Compiladores menos inteligentes podem produzir codigo mais rapido para a segunda versao, mas compiladores inteligentes produzem o mesmo codigo para ambas.

Para imprimir o codigo ASCII de um caractere, o programa usa uma conversao de tipo para converter `nome[i]` para um tipo inteiro. Entao `cout` imprime o valor como um inteiro em vez de interpreta-lo como um codigo de caractere.

Ao contrario de uma string de estilo C, um objeto da classe `string` nao usa um caractere nulo para identificar o final de uma string, portanto voce nao pode converter a Listagem 5.13 para uma versao com a classe `string` simplesmente substituindo o array de `char` por um objeto `string`. O Capitulo 16 discute tecnicas que voce pode usar com um objeto `string` para identificar o ultimo caractere.

---

## for Versus while

Em C++, os loops `for` e `while` sao essencialmente equivalentes. Por exemplo, o loop `for`

```cpp
for (expressao-de-init; expressao-de-teste; expressao-de-atualizacao)
{
    instrucao(es)
}
```

poderia ser reescrito desta forma:

```cpp
expressao-de-init;
while (expressao-de-teste)
{
    instrucao(es)
    expressao-de-atualizacao;
}
```

Da mesma forma, o loop `while`

```cpp
while (expressao-de-teste)
    corpo
```

poderia ser reescrito desta forma:

```cpp
for ( ;expressao-de-teste;)
    corpo
```

Esse loop `for` requer tres expressoes (ou, mais tecnicamente, uma instrucao seguida de duas expressoes), mas elas podem ser expressoes vazias (ou instrucoes). Apenas os dois ponto e virgulas sao obrigatorios. Incidentalmente, uma expressao de teste ausente em um loop `for` e interpretada como `true`, portanto este loop e executado eternamente:

```cpp
for ( ; ; )
    corpo
```

Como os loops `for` e `while` sao quase equivalentes, qual voce usa e em grande parte uma questao de estilo. Ha tres diferencas. Uma, como mencionado, e que uma condicao de teste omitida em um loop `for` e interpretada como `true`. A segunda e que voce pode usar a instrucao inicializadora em um loop `for` para declarar uma variavel que e local para o loop; voce nao pode fazer isso com um loop `while`. Por fim, ha uma pequena diferenca se o corpo incluir uma instrucao `continue`, que e discutida no Capitulo 6. Tipicamente, os programadores usam loops `for` para loops de contagem porque o formato do loop `for` permite que voce coloque todas as informacoes relevantes — valor inicial, valor de terminacao e metodo de atualizacao do contador — em um unico lugar. Os programadores usam mais frequentemente loops `while` quando nao sabem de antemao precisamente quantas vezes um loop sera executado.

> **Dica:** Tenha em mente as seguintes diretrizes ao projetar um loop:
> - Identifique a condicao que encerra a execucao do loop.
> - Inicialize essa condicao antes do primeiro teste.
> - Atualize a condicao em cada ciclo do loop antes que a condicao seja testada novamente.
>
> Uma coisa boa sobre os loops `for` e que sua estrutura fornece um lugar para implementar essas tres diretrizes, ajudando assim a lembrar de faze-lo. Mas essas diretrizes se aplicam a um loop `while` tambem.

> **Pontuacao Incorreta**
>
> Tanto os loops `for` quanto os loops `while` tem corpos que consistem em uma unica instrucao seguindo as expressoes entre parenteses. Como voce ja viu, essa unica instrucao pode ser um bloco, que pode conter diversas instrucoes. Tenha em mente que sao as chaves, nao a indentacao, que definem um bloco. Considere o seguinte loop, por exemplo:
> ```cpp
> i = 0;
> while (nome[i] != '\0')
>     cout << nome[i] << endl;
>     i++;
> cout << "Pronto\n";
> ```
> A indentacao diz que o autor do programa pretendia que a instrucao `i++;` fosse parte do corpo do loop. A ausencia de chaves, porem, diz ao compilador que o corpo consiste somente na primeira instrucao `cout`. Assim, o loop continua imprimindo o primeiro caractere do array indefinidamente. O programa nunca alcanca a instrucao `i++;` porque ela esta fora do loop.
>
> O exemplo seguinte mostra outra armadilha potencial:
> ```cpp
> i = 0;
> while (nome[i] != '\0'); // ponto e virgula problematico
> {
>     cout << nome[i] << endl;
>     i++;
> }
> cout << "Pronto\n";
> ```
> Desta vez o codigo acerta as chaves, mas tambem insere um ponto e virgula extra. Lembre-se, um ponto e virgula termina uma instrucao, portanto este ponto e virgula termina o loop `while`. Em outras palavras, o corpo do loop e uma instrucao nula — ou seja, nada seguido de um ponto e virgula. Todo o material entre chaves agora vem *apos* o loop e nunca e alcancado. Em vez disso, o loop cicla, sem fazer nada, eternamente. Cuidado com o ponto e virgula extraviado.

---

## Espere um Momento — Construindo um Loop de Atraso de Tempo

As vezes e util incorporar um atraso de tempo em um programa. Por exemplo, voce pode ter encontrado programas que piscam uma mensagem na tela e depois passam para outra coisa antes que voce possa le-la. Voce acaba com medo de ter perdido informacoes irretrieveis de vital importancia. Seria muito mais agradavel se o programa fizesse uma pausa de 5 segundos antes de continuar. O loop `while` e util para produzir esse efeito. Uma tecnica dos primeiros dias dos computadores pessoais era fazer o computador contar por um tempo para desperdicar tempo:

```cpp
long espera = 0;
while (espera < 10000)
    espera++; // contando silenciosamente
```

O problema com essa abordagem e que voce tem que mudar o limite de contagem quando muda a velocidade do processador do computador. Muitos jogos escritos para o IBM PC original, por exemplo, tornaram-se incontrolavelmente rapidos quando executados em seus sucessores mais rapidos. E nos dias de hoje um compilador pode ate mesmo deduzir que pode simplesmente definir `espera` como 10000 e pular o loop. Uma abordagem melhor e deixar o relogio do sistema fazer a contagem de tempo para voce.

As bibliotecas ANSI C e C++ tem uma funcao para ajudar a fazer isso. A funcao e chamada `clock()`, e ela retorna o tempo do sistema decorrido desde o inicio da execucao de um programa. Ha algumas complicacoes, porem. Primeiro, `clock()` nao retorna necessariamente o tempo em segundos. Segundo, o tipo de retorno da funcao pode ser `long` em alguns sistemas, `unsigned long` em outros e talvez algum outro tipo em outros.

Mas o arquivo de cabecalho `ctime` (chamado `time.h` em implementacoes menos atuais) fornece solucoes para esses problemas. Primeiro, ele define uma constante simbolica, `CLOCKS_PER_SEC`, que e igual ao numero de unidades de tempo do sistema por segundo. Portanto, dividir o tempo do sistema por esse valor retorna os segundos. Ou voce pode multiplicar os segundos por `CLOCKS_PER_SEC` para obter o tempo nas unidades do sistema. Segundo, `ctime` estabelece `clock_t` como um alias para o tipo de retorno de `clock()`. (Veja a caixa lateral "Aliases de Tipo", mais adiante neste capitulo.) Isso significa que voce pode declarar uma variavel como tipo `clock_t`, e o compilador a convertera para `long` ou `unsigned int` ou qualquer que seja o tipo adequado para o seu sistema.

A Listagem 5.14 mostra como usar `clock()` e o cabecalho `ctime` para criar um loop de atraso de tempo.

**Listagem 5.14 — waiting.cpp**

```cpp
// waiting.cpp -- usando clock() em um loop de atraso de tempo
#include <iostream>
#include <ctime> // descreve a funcao clock(), tipo clock_t
int main()
{
    using namespace std;
    cout << "Digite o tempo de atraso, em segundos: ";
    float segundos;
    cin >> segundos;
    clock_t atraso = segundos * CLOCKS_PER_SEC; // converte para tiques de relogio
    cout << "iniciando\a\n";
    clock_t inicio = clock();
    while (clock() - inicio < atraso) // aguarda ate o tempo decorrer
        ;                    // observe o ponto e virgula
    cout << "concluido \a\n";
    return 0;
}
```

Ao calcular o tempo de atraso em unidades do sistema em vez de em segundos, o programa na Listagem 5.14 evita ter que converter o tempo do sistema para segundos em cada ciclo do loop.

---

> **Aliases de Tipo**
>
> O C++ tem duas formas de estabelecer um novo nome como alias para um tipo. Uma e usar o pre-processador:
>
> ```cpp
> #define BYTE char // pre-processador substitui BYTE por char
> ```
>
> O pre-processador entao substitui todas as ocorrencias de `BYTE` por `char` quando voce compila um programa, tornando assim `BYTE` um alias para `char`.
>
> O segundo metodo e usar a palavra-chave `typedef` do C++ (e do C) para criar um alias. Por exemplo, para tornar `byte` um alias para `char`, voce usa isto:
>
> ```cpp
> typedef char byte; // torna byte um alias para char
> ```
>
> Aqui esta a forma geral:
>
> ```cpp
> typedef nomeDoTipo nomeDoAlias;
> ```
>
> Em outras palavras, se voce quiser que `nomeDoAlias` seja um alias para um tipo especifico, declare `nomeDoAlias` como se fosse uma variavel desse tipo e, em seguida, prefixe a declaracao com a palavra-chave `typedef`. Por exemplo, para tornar `ponteiro_byte` um alias para ponteiro-para-`char`, voce poderia declarar `ponteiro_byte` como um ponteiro-para-`char` e depois adicionar `typedef` na frente:
>
> ```cpp
> typedef char * ponteiro_byte; // tipo ponteiro para char
> ```
>
> Voce poderia tentar algo semelhante com `#define`, mas isso nao funcionaria se voce declarasse uma lista de variaveis. Por exemplo, considere o seguinte:
>
> ```cpp
> #define PONTEIRO_FLOAT float *
> PONTEIRO_FLOAT pa, pb;
> ```
>
> A substituicao pelo pre-processador converte a declaracao para isto:
>
> ```cpp
> float * pa, pb; // pa e um ponteiro para float, pb e apenas um float
> ```
>
> A abordagem com `typedef` nao tem esse problema. Sua capacidade de lidar com aliases de tipo mais complexos torna o uso de `typedef` uma escolha melhor do que `#define` — e as vezes e a unica escolha.
>
> Observe que `typedef` nao cria um novo tipo. Ele apenas cria um novo nome para um tipo antigo. Se voce tornar `numero` um alias para `int`, `cout` trata um valor do tipo `numero` como o `int` que ele realmente e.

---

> Navegacao: [Anterior](capitulo-05-03-expressoes-relacionais.md) | [Indice](README.md) | [Proximo](capitulo-05-05-loop-dowhile-for-faixa.md)
