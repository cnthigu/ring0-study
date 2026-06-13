# Capitulo 5 - Loops e Expressoes Relacionais (Loops and Relational Expressions)
## Parte 7: Loops Aninhados, Arrays Bidimensionais, Resumo e Revisao

> Traducao nao oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Navegacao: [Anterior](capitulo-05-06-entrada-texto-eof.md) | [Indice](README.md) | [Proximo](capitulo-06-01-instrucao-if.md)

---

## Loops Aninhados e Arrays Bidimensionais

Anteriormente neste capitulo, voce viu que o loop `for` e uma ferramenta natural para processar arrays. Agora vamos dar um passo adiante e ver como um loop `for` dentro de um loop `for` (loops aninhados) serve para lidar com arrays bidimensionais.

Primeiro, vamos examinar o que e um array bidimensional. Os arrays usados ate agora neste capitulo sao chamados de **arrays unidimensionais** (one-dimensional arrays) porque voce pode visualizar cada array como uma unica linha de dados. Voce pode visualizar um **array bidimensional** (two-dimensional array) como sendo mais parecido com uma tabela, tendo tanto linhas quanto colunas de dados. Voce pode usar um array bidimensional, por exemplo, para representar dados de vendas trimestrais de seis distritos separados, com uma linha de dados para cada distrito. Ou voce pode usar um array bidimensional para representar a posicao do RoboDork em um tabuleiro de jogo computadorizado.

O C++ nao fornece um tipo de array bidimensional especial. Em vez disso, voce cria um array para o qual cada elemento e ele proprio um array. Por exemplo, suponha que voce queira armazenar dados de temperatura maxima para cinco cidades ao longo de um periodo de 4 anos. Nesse caso, voce pode declarar um array da seguinte forma:

```cpp
int tempos_max[4][5];
```

Essa declaracao significa que `tempos_max` e um array com quatro elementos. Cada um desses elementos e ele proprio um array de cinco `int`s. Voce pode pensar em `tempos_max[0]` como sendo o nome do primeiro array de cinco `int`s, em `tempos_max[1]` como o nome do segundo array de cinco `int`s, e assim por diante. Voce acessa elementos individuais fornecendo dois indices (subscripts). `tempos_max[0][0]` e o primeiro elemento de `tempos_max[0]`, `tempos_max[0][1]` e o segundo elemento de `tempos_max[0]`, e assim por diante. Voce pode pensar no primeiro indice como a linha e no segundo como a coluna.

Suponha que voce queira imprimir todo o conteudo do array. Nesse caso, voce pode usar um loop `for` para mudar de linha e um segundo loop `for` aninhado para mudar de coluna:

```cpp
for (int linha = 0; linha < 4; linha++)
{
    for (int col = 0; col < 5; ++col)
        cout << tempos_max[linha][col] << "\t";
    cout << endl;
}
```

Para cada valor de `linha`, o loop `for` interno percorre todos os valores de `col`. Este exemplo imprime um caractere de tabulacao (`\t` na notacao de caractere de escape do C++) apos cada valor e um caractere de nova linha apos cada linha completa.

## Inicializando um Array Bidimensional

Quando voce cria um array bidimensional, tem a opcao de inicializar cada elemento. A tecnica e baseada na de inicializar um array unidimensional. Lembre-se de que voce faz isso fornecendo uma lista de valores separados por virgulas delimitada por chaves:

```cpp
// inicializando um array unidimensional
int btus[5] = { 23, 26, 24, 31, 28};
```

Para um array bidimensional, cada elemento e ele proprio um array, portanto voce pode inicializar cada elemento usando uma forma como a do exemplo de codigo anterior. Assim, a inicializacao consiste em uma serie de inicializacoes unidimensionais separadas por virgulas, todas delimitadas por um conjunto de chaves:

```cpp
int tempos_max[4][5] = // array 2-D
{
    {96, 100, 87, 101, 105}, // valores para tempos_max[0]
    {96,  98, 91, 107, 104}, // valores para tempos_max[1]
    {97, 101, 93, 108, 107}, // valores para tempos_max[2]
    {98, 103, 95, 109, 108}  // valores para tempos_max[3]
};
```

Voce pode visualizar `tempos_max` como quatro linhas de cinco numeros cada. O termo `{96, 100, 87, 101, 105}` inicializa a primeira linha, representada por `tempos_max[0]`. Como questao de estilo, colocar cada linha de dados em sua propria linha, se possivel, torna os dados mais faceis de ler.

## Usando um Array Bidimensional

A Listagem 5.20 incorpora um array bidimensional inicializado e um loop aninhado em um programa. Desta vez, o programa inverte a ordem dos loops, colocando o loop de coluna (indice de cidade) no exterior e o loop de linha (indice de ano) no interior. Alem disso, usa uma pratica comum do C++ de inicializar um array de ponteiros para um conjunto de constantes de string. Ou seja, `cidades` e declarado como um array de ponteiros-para-`char`. Isso torna cada elemento, como `cidades[0]`, um ponteiro-para-`char` que pode ser inicializado para o endereco de uma string. O programa inicializa `cidades[0]` para o endereco da string `"Gribble City"`, e assim por diante. Assim, esse array de ponteiros se comporta como um array de strings.

**Listagem 5.20 — nested.cpp**

```cpp
// nested.cpp -- loops aninhados e array 2-D
#include <iostream>
const int Cidades = 5;
const int Anos = 4;
int main()
{
    using namespace std;
    const char * cidades[Cidades] = // array de ponteiros
    {                               // para 5 strings
        "Gribble City",
        "Gribbletown",
        "New Gribble",
        "San Gribble",
        "Gribble Vista"
    };
    int tempos_max[Anos][Cidades] = // array 2-D
    {
        {96, 100, 87, 101, 105}, // valores para tempos_max[0]
        {96,  98, 91, 107, 104}, // valores para tempos_max[1]
        {97, 101, 93, 108, 107}, // valores para tempos_max[2]
        {98, 103, 95, 109, 108}  // valores para tempos_max[3]
    };
    cout << "Temperaturas maximas para 2008 - 2011\n\n";
    for (int cidade = 0; cidade < Cidades; ++cidade)
    {
        cout << cidades[cidade] << ":\t";
        for (int ano = 0; ano < Anos; ++ano)
            cout << tempos_max[ano][cidade] << "\t";
        cout << endl;
    }
    return 0;
}
```

Aqui esta a saida do programa na Listagem 5.20:

```
Temperaturas maximas para 2008 - 2011

Gribble City: 96 96 97  98
Gribbletown: 100 98 101 103
New Gribble: 87 91 93   95
San Gribble: 101 107 108 109
Gribble Vista: 105 104 107 108
```

Usar tabulacoes na saida espaca os dados de forma mais regular do que usar espacos. Porem, diferentes configuracoes de tabulacao podem fazer com que a saida varie em aparencia de um sistema para outro. O Capitulo 17 apresenta metodos mais precisos, mas mais complexos, para formatar a saida.

De forma mais trabalhosa, voce poderia usar um array de arrays de `char` em vez de um array de ponteiros para os dados de string. A declaracao seria assim:

```cpp
char cidades[Cidades][25] = // array de 5 arrays de 25 char
{
    "Gribble City",
    "Gribbletown",
    "New Gribble",
    "San Gribble",
    "Gribble Vista"
};
```

Essa abordagem limita cada uma das cinco strings a um maximo de 24 caracteres. O array de ponteiros armazena os enderecos dos cinco literais de string, mas o array de arrays de `char` copia cada um dos cinco literais de string para os cinco arrays correspondentes de 25 `char`. Assim, o array de ponteiros e muito mais economico em termos de espaco. Porem, se voce pretendia modificar qualquer uma das strings, o array bidimensional seria uma escolha melhor. Curiosamente, ambas as escolhas usam a mesma lista de inicializacao e o mesmo codigo do loop `for` para exibir as strings.

Voce tambem poderia usar um array de objetos da classe `string` em vez de um array de ponteiros para os dados de string. A declaracao seria assim:

```cpp
const string cidades[Cidades] = // array de 5 strings
{
    "Gribble City",
    "Gribbletown",
    "New Gribble",
    "San Gribble",
    "Gribble Vista"
};
```

Se voce pretendia que as strings fossem modificaveis, omitia o qualificador `const`. Esta forma usa a mesma lista de inicializacao e o mesmo codigo do loop `for` de exibicao que as outras duas formas. Se voce quiser strings modificaveis, o recurso de dimensionamento automatico da classe `string` torna essa abordagem mais conveniente de usar do que a abordagem do array bidimensional.

---

## Resumo

O C++ oferece tres variedades de loops: loops `for`, loops `while` e loops `do while`. Um loop percorre o mesmo conjunto de instrucoes repetidamente, desde que a condicao de teste do loop avalie como `true` ou diferente de zero, e o loop termina a execucao quando a condicao de teste avalia como `false` ou zero. Os loops `for` e `while` sao loops de condicao de entrada, o que significa que examinam a condicao de teste antes de executar as instrucoes no corpo do loop. O loop `do while` e um loop de condicao de saida, o que significa que examina a condicao de teste apos executar as instrucoes no corpo do loop.

A sintaxe de cada loop exige que o corpo do loop consista em uma unica instrucao. Porem, essa instrucao pode ser uma instrucao composta, ou bloco, formado pela inclusao de diversas instrucoes entre pares de chaves.

As expressoes relacionais, que comparam dois valores, sao frequentemente usadas como condicoes de teste de loop. As expressoes relacionais sao formadas usando um dos seis operadores relacionais: `<`, `<=`, `==`, `>=`, `>` ou `!=`. As expressoes relacionais avaliam para os valores `bool` `true` e `false`.

Muitos programas leem entrada de texto ou arquivos de texto caractere por caractere. A classe `istream` fornece diversas maneiras de fazer isso. Se `ch` e uma variavel do tipo `char`, a seguinte instrucao le o proximo caractere de entrada em `ch`:

```cpp
cin >> ch;
```

Porem, ela pula espacos, novas linhas e tabulacoes. A seguinte chamada de funcao membro le o proximo caractere de entrada, independentemente de seu valor, e o coloca em `ch`:

```cpp
cin.get(ch);
```

A chamada de funcao membro `cin.get()` retorna o proximo caractere de entrada, incluindo espacos, novas linhas e tabulacoes, portanto pode ser usada da seguinte forma:

```cpp
ch = cin.get();
```

A chamada de funcao membro `cin.get(char)` relata o encontro da condicao de EOF retornando um valor com a conversao `bool` de `false`, enquanto a chamada de funcao membro `cin.get()` relata o EOF retornando o valor `EOF`, que e definido no arquivo `iostream`.

Um loop aninhado e um loop dentro de um loop. Os loops aninhados fornecem uma maneira natural de processar arrays bidimensionais.

---

## Revisao do Capitulo

**1.** Qual e a diferenca entre um loop de condicao de entrada e um loop de condicao de saida? Qual tipo e cada um dos loops C++?

**2.** O que o seguinte fragmento de codigo imprimiria se fizesse parte de um programa valido?

```cpp
int i;
for (i = 0; i < 5; i++)
    cout << i;
    cout << endl;
```

**3.** O que o seguinte fragmento de codigo imprimiria se fizesse parte de um programa valido?

```cpp
int j;
for (j = 0; j < 11; j += 3)
    cout << j;
cout << endl << j << endl;
```

**4.** O que o seguinte fragmento de codigo imprimiria se fizesse parte de um programa valido?

```cpp
int j = 5;
while ( ++j < 9)
    cout << j++ << endl;
```

**5.** O que o seguinte fragmento de codigo imprimiria se fizesse parte de um programa valido?

```cpp
int k = 8;
do
    cout <<" k = " << k << endl;
while (k++ < 5);
```

**6.** Escreva um loop `for` que imprima os valores `1 2 4 8 16 32 64` aumentando o valor de uma variavel contadora por um fator de dois em cada ciclo.

**7.** Como voce faz com que o corpo de um loop inclua mais de uma instrucao?

**8.** A seguinte instrucao e valida? Se nao, por que nao? Se sim, o que ela faz?

```cpp
int x = (1,024);
```

E quanto ao seguinte?

```cpp
int y;
y = 1,024;
```

**9.** Como `cin >> ch` difere de `cin.get(ch)` e de `ch = cin.get()` em como visualiza a entrada?

---

## Exercicios de Programacao

**1.** Escreva um programa que solicite ao usuario que insira dois inteiros. O programa deve entao calcular e relatar a soma de todos os inteiros entre e incluindo os dois inteiros. Neste ponto, assuma que o inteiro menor e inserido primeiro. Por exemplo, se o usuario inserir `2` e `9`, o programa deve relatar que a soma de todos os inteiros de 2 a 9 e 44.

**2.** Refaca a Listagem 5.4 usando um objeto do tipo `array` em vez de um array embutido e o tipo `long double` em vez de `long long`. Encontre o valor de `100!`

**3.** Escreva um programa que peca ao usuario que insira numeros. Apos cada entrada, o programa deve relatar a soma acumulada das entradas ate o momento. O programa deve terminar quando o usuario inserir `0`.

**4.** Daphne investe $100 a 10% de juros simples. Ou seja, a cada ano, o investimento rende 10% do investimento original, ou $10 a cada ano:

```
juros = 0,10 × saldo original
```

Ao mesmo tempo, Cleo investe $100 a 5% de juros compostos. Ou seja, os juros sao 5% do saldo atual, incluindo adicoes anteriores de juros:

```
juros = 0,05 × saldo atual
```

Cleo ganha 5% de $100 no primeiro ano, dando-lhe $105. No proximo ano ela ganha 5% de $105, ou $5,25, e assim por diante. Escreva um programa que descubra quantos anos leva para o valor do investimento de Cleo superar o valor do investimento de Daphne e entao exiba o valor de ambos os investimentos nesse momento.

**5.** Voce vende o livro *C++ para Principiantes*. Escreva um programa que faca voce inserir um ano inteiro de vendas mensais (em termos de numero de livros, nao de dinheiro). O programa deve usar um loop para solicitar por mes, usando um array de `char *` (ou um array de objetos `string`, se preferir) inicializado com as strings de mes e armazenando os dados de entrada em um array de `int`. Em seguida, o programa deve encontrar a soma do conteudo do array e relatar o total de vendas do ano.

**6.** Faca o Exercicio de Programacao 5, mas use um array bidimensional para armazenar a entrada de 3 anos de vendas mensais. Relate o total de vendas de cada ano individual e o total combinado dos anos.

**7.** Elabore uma estrutura chamada `carro` que contenha as seguintes informacoes sobre um automovel: sua marca, como uma string em um array de caracteres ou em um objeto `string`, e o ano em que foi fabricado, como um inteiro. Escreva um programa que peca ao usuario quantos carros catalogo. O programa deve entao usar `new` para criar um array dinamico desse numero de estruturas `carro`. Em seguida, deve solicitar ao usuario que insira as informacoes de marca (que podem consistir em mais de uma palavra) e ano para cada estrutura. Observe que isso requer algum cuidado porque alterna a leitura de strings com dados numericos (veja o Capitulo 4). Por fim, deve exibir o conteudo de cada estrutura. Uma execucao de exemplo deve ser parecida com a seguinte:

```
Quantos carros voce deseja catalogar? 2
Carro #1:
Por favor, insira a marca: Hudson Hornet
Por favor, insira o ano de fabricacao: 1952
Carro #2:
Por favor, insira a marca: Kaiser
Por favor, insira o ano de fabricacao: 1951
Aqui esta sua colecao:
1952 Hudson Hornet
1951 Kaiser
```

---

> Navegacao: [Anterior](capitulo-05-06-entrada-texto-eof.md) | [Indice](README.md) | [Proximo](capitulo-06-01-instrucao-if.md)
