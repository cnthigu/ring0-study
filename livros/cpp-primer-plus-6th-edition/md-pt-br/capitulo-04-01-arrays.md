# Capitulo 4 - Tipos Compostos (Compound Types)
## Parte 1: Arrays

> Traducao nao oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Navegacao: [Anterior](capitulo-03.md) | [Indice](README.md) | [Proximo](capitulo-04-02-strings.md)

---

Neste capitulo voce vai aprender sobre os seguintes topicos:

- Criacao e uso de arrays
- Criacao e uso de strings no estilo C
- Criacao e uso de strings da classe `string`
- Uso dos metodos `getline()` e `get()` para leitura de strings
- Combinacao de entrada de strings e numeros
- Criacao e uso de estruturas
- Criacao e uso de unioes
- Criacao e uso de enumeracoes
- Criacao e uso de ponteiros
- Gerenciamento de memoria dinamica com `new` e `delete`
- Criacao de arrays dinamicos
- Criacao de estruturas dinamicas
- Armazenamento automatico, estatico e dinamico
- As classes `vector` e `array` (introducao)

Digamos que voce desenvolveu um jogo de computador chamado User-Hostile, no qual os jogadores medem forcas com uma interface de computador críptica e abusiva. Agora voce precisa escrever um programa que controle as vendas mensais do seu jogo ao longo de um período de cinco anos. Ou talvez voce queira fazer um inventario da sua colecao de cards de herois hackers. Logo voce conclui que precisa de algo alem dos tipos basicos simples do C++ para atender a esses requisitos de dados, e o C++ oferece exatamente isso — tipos compostos (compound types). Sao tipos construidos a partir dos tipos inteiros e de ponto flutuante basicos. O tipo composto mais abrangente e a classe, aquela fortaleza da POO (programacao orientada a objetos) para a qual estamos caminhando. Mas o C++ tambem oferece varios tipos compostos mais simples, herdados do C. O array, por exemplo, pode armazenar varios valores do mesmo tipo. Um tipo especial de array pode armazenar uma string, que e uma serie de caracteres. Estruturas podem armazenar varios valores de tipos diferentes. E ha os ponteiros, que sao variaveis que indicam ao computador onde os dados estao armazenados. Voce vai examinar todas essas formas compostas (exceto classes) neste capitulo, ter um primeiro contato com `new` e `delete` e como usá-los para gerenciar dados, e dar uma primeira olhada na classe `string` do C++, que oferece uma forma alternativa de trabalhar com strings.

## Apresentando Arrays

Um **array** (vetor) e uma forma de dado que pode armazenar varios valores, todos do mesmo tipo. Por exemplo, um array pode armazenar 60 valores do tipo `int` que representam dados de vendas de um jogo ao longo de cinco anos, 12 valores `short` que representam o numero de dias em cada mes, ou 365 valores `float` que indicam seus gastos com alimentacao em cada dia do ano. Cada valor e armazenado em um elemento separado do array, e o computador armazena todos os elementos de um array de forma consecutiva na memoria.

Para criar um array, voce usa uma instrucao de declaracao. A declaracao de um array deve indicar tres coisas:

- O tipo do valor a ser armazenado em cada elemento
- O nome do array
- O numero de elementos no array

Voce faz isso em C++ modificando a declaracao de uma variavel simples e adicionando colchetes que contem o numero de elementos. Por exemplo, a declaracao a seguir cria um array chamado `meses` com 12 elementos, cada um dos quais pode armazenar um valor do tipo `short`:

```cpp
short meses[12]; // cria array com 12 shorts
```

Cada elemento e, em essencia, uma variavel que voce pode tratar como uma variavel simples.

Esta e a forma geral para declarar um array:

```cpp
tipoDado nomeArray[tamanhoArray];
```

A expressao `tamanhoArray`, que e o numero de elementos, deve ser uma constante inteira, como `10`, ou um valor `const`, ou uma expressao constante, como `8 * sizeof(int)`, para a qual todos os valores sao conhecidos no momento da compilacao. Em particular, `tamanhoArray` nao pode ser uma variavel cujo valor e definido enquanto o programa esta em execucao. Entretanto, mais adiante neste capitulo voce vai aprender como usar o operador `new` para contornar essa restricao.

> **O Array como Tipo Composto**
>
> Um array e chamado de *tipo composto* porque e construido a partir de algum outro tipo. (C usa o termo *tipo derivado*, mas como C++ usa esse termo para relacionamentos entre classes, foi preciso criar um novo termo.) Voce nao pode simplesmente declarar que algo e um array; ele sempre tem que ser um array de algum tipo especifico. Nao existe um tipo de array generico. Em vez disso, ha muitos tipos especificos de array, como array de `char` ou array de `long`. Por exemplo, considere esta declaracao:
>
> ```cpp
> float emprestimos[20];
> ```
>
> O tipo de `emprestimos` nao e "array"; na verdade, e "array de `float`". Isso enfatiza que o array `emprestimos` e construido a partir do tipo `float`.

Grande parte da utilidade do array vem do fato de que voce pode acessar seus elementos individualmente. A maneira de fazer isso e usar um subscrito, ou indice, para numerar os elementos. A numeracao de arrays em C++ comeca em zero. (Isso nao e negociavel; voce tem que comecar em zero. Usuarios de Pascal e BASIC terao que se adaptar.) O C++ usa uma notacao de colchetes com o indice para especificar um elemento do array. Por exemplo, `meses[0]` e o primeiro elemento do array `meses`, e `meses[11]` e o ultimo elemento. Note que o indice do ultimo elemento e um a menos que o tamanho do array (veja a Figura 4.1). Assim, uma declaracao de array permite criar muitas variaveis com uma unica declaracao, e voce pode usar um indice para identificar e acessar elementos individuais.

> **A Importancia de Usar Indices Validos**
>
> O compilador nao verifica se voce esta usando um subscrito valido. Por exemplo, o compilador nao vai reclamar se voce atribuir um valor ao elemento inexistente `meses[101]`. Mas essa atribuicao pode causar problemas quando o programa for executado, possivelmente corrompendo dados ou codigo, ou fazendo o programa abortar. Portanto, e sua responsabilidade garantir que o seu programa use apenas valores de subscrito validos.

O programa de analise de inhames (tipo de batata-doce) na Listagem 4.1 demonstra algumas propriedades de arrays, incluindo declaracao de um array, atribuicao de valores a elementos do array e inicializacao de um array.

**Listagem 4.1 — arrayone.cpp**

```cpp
// arrayone.cpp -- arrays pequenos de inteiros
#include <iostream>
int main()
{
    using namespace std;
    int inhames[3];             // cria array com tres elementos
    inhames[0] = 7;             // atribui valor ao primeiro elemento
    inhames[1] = 8;
    inhames[2] = 6;
    int custosInhame[3] = {20, 30, 5}; // cria e inicializa array
    // NOTA: se o compilador nao conseguir inicializar este array,
    // use static int custosInhame[3] em vez de int custosInhame[3]
    cout << "Total de inhames = ";
    cout << inhames[0] + inhames[1] + inhames[2] << endl;
    cout << "O pacote com " << inhames[1] << " inhames custa ";
    cout << custosInhame[1] << " centavos por inhame.\n";
    int total = inhames[0] * custosInhame[0] + inhames[1] * custosInhame[1];
    total = total + inhames[2] * custosInhame[2];
    cout << "A despesa total com inhames e " << total << " centavos.\n";
    cout << "\nTamanho do array inhames = " << sizeof inhames;
    cout << " bytes.\n";
    cout << "Tamanho de um elemento = " << sizeof inhames[0];
    cout << " bytes.\n";
    return 0;
}
```

Aqui esta a saida do programa na Listagem 4.1:

```
Total de inhames = 21
O pacote com 8 inhames custa 30 centavos por inhame.
A despesa total com inhames e 410 centavos.

Tamanho do array inhames = 12 bytes.
Tamanho de um elemento = 4 bytes.
```

**Notas do Programa**

Primeiro, o programa na Listagem 4.1 cria um array de tres elementos chamado `inhames`. Como `inhames` tem tres elementos, os elementos sao numerados de `0` a `2`, e `arrayone.cpp` usa os valores de indice `0` a `2` para atribuir valores aos tres elementos individuais. Cada elemento individual de `inhames` e um `int` com todos os direitos e privilegios do tipo `int`, entao `arrayone.cpp` pode — e faz isso — atribuir valores a elementos, somar elementos, multiplicar elementos e exibir elementos.

O programa usa a maneira longa para atribuir valores aos elementos de `inhames`. O C++ tambem permite inicializar elementos do array dentro da instrucao de declaracao. A Listagem 4.1 usa esse atalho para atribuir valores ao array `custosInhame`:

```cpp
int custosInhame[3] = {20, 30, 5};
```

Basta fornecer uma lista de valores separados por virgulas (a lista de inicializacao) entre chaves. Os espacos na lista sao opcionais. Se voce nao inicializar um array definido dentro de uma funcao, os valores dos elementos ficam indefinidos. Isso significa que o elemento assume qualquer valor que estava anteriormente naquela posicao da memoria.

Em seguida, o programa usa os valores do array em alguns calculos. Essa parte do programa parece confusa com todos os subscritos e colchetes. O laco `for`, que sera abordado no Capitulo 5, "Lacos e Expressoes Relacionais", oferece uma forma eficaz de lidar com arrays e elimina a necessidade de escrever cada indice explicitamente. Por enquanto, vamos nos limitar a arrays pequenos.

Como voce deve lembrar, o operador `sizeof` retorna o tamanho, em bytes, de um tipo ou objeto de dados. Note que se voce usar o operador `sizeof` com o nome de um array, obtem o numero de bytes no array inteiro. Mas se usar `sizeof` com um elemento do array, obtem o tamanho, em bytes, do elemento. Isso ilustra que `inhames` e um array, mas `inhames[1]` e simplesmente um `int`.

## Regras de Inicializacao para Arrays

O C++ tem varias regras sobre a inicializacao de arrays. Elas restringem quando voce pode fazer isso e determinam o que acontece quando o numero de elementos do array nao corresponde ao numero de valores na lista de inicializacao. Vamos examinar essas regras.

Voce pode usar a forma de inicializacao apenas ao *definir* o array. Nao e possivel usá-la depois, e tampouco e possivel atribuir um array inteiro a outro:

```cpp
int cartas[4] = {3, 6, 8, 10}; // correto
int mao[4];                     // correto
mao[4] = {5, 6, 7, 9};         // nao permitido
mao = cartas;                   // nao permitido
```

Entretanto, voce pode usar subscritos e atribuir valores aos elementos de um array individualmente.

Ao inicializar um array, voce pode fornecer menos valores do que elementos do array. Por exemplo, a instrucao a seguir inicializa apenas os dois primeiros elementos de `dicaHotel`:

```cpp
float dicaHotel[5] = {5.0, 2.5};
```

Se voce inicializar parcialmente um array, o compilador define os elementos restantes como zero. Assim, e facil inicializar todos os elementos de um array como zero — basta inicializar explicitamente o primeiro elemento como zero e deixar o compilador inicializar os elementos restantes como zero:

```cpp
long totais[500] = {0};
```

Note que se voce inicializar com `{1}` em vez de `{0}`, apenas o primeiro elemento e definido como `1`; os demais ainda sao definidos como `0`.

Se voce deixar os colchetes (`[]`) vazios ao inicializar um array, o compilador C++ conta os elementos para voce. Por exemplo, suponha que voce faca esta declaracao:

```cpp
short coisas[] = {1, 5, 3, 8};
```

O compilador torna `coisas` um array de quatro elementos.

> **Deixar o Compilador Fazer o Trabalho**
>
> Frequentemente, deixar o compilador contar o numero de elementos e uma pratica ruim porque a contagem dele pode ser diferente do que voce espera. Voce poderia, por exemplo, omitir acidentalmente um valor inicial da lista. Entretanto, essa abordagem pode ser segura para inicializar um array de caracteres com uma string, como voce vera em breve. E se a sua principal preocupacao e que o programa, e nao voce, saiba o tamanho do array, voce pode fazer algo assim:
>
> ```cpp
> short coisas[] = {1, 5, 3, 8};
> int num_elementos = sizeof coisas / sizeof (short);
> ```
>
> Se isso e util ou preguicoso depende das circunstancias.

## Inicializacao de Arrays em C++11

Como mencionado no Capitulo 3, "Lidando com Dados", o C++11 torna a forma de inicializacao com chaves (inicializacao por lista) uma forma universal para todos os tipos. Arrays ja usam inicializacao por lista, mas a versao do C++11 acrescenta alguns recursos adicionais.

Primeiro, voce pode omitir o sinal `=` ao inicializar um array:

```cpp
double ganhos[4] {1.2e4, 1.6e4, 1.1e4, 1.7e4}; // correto em C++11
```

Segundo, voce pode usar chaves vazias para definir todos os elementos como `0`:

```cpp
unsigned int contagens[10] = {}; // todos os elementos definidos como 0
float saldos[100] {};            // todos os elementos definidos como 0
```

Terceiro, como discutido no Capitulo 3, a inicializacao por lista protege contra estreitamento (narrowing):

```cpp
long plifs[] = {25, 92, 3.0};         // nao permitido
char slifs[4] {'h', 'i', 1122011, '\0'}; // nao permitido
char tlifs[4] {'h', 'i', 112, '\0'};     // permitido
```

A primeira inicializacao falha porque converter de um tipo de ponto flutuante para um tipo inteiro e estreitamento, mesmo que o valor de ponto flutuante tenha apenas zeros apos o ponto decimal. A segunda falha porque `1122011` esta fora do intervalo de um `char`, assumindo que temos um `char` de 8 bits. A terceira e bem-sucedida porque, embora `112` seja um valor `int`, ainda esta dentro do intervalo de um `char`.

A Biblioteca Padrao de Templates do C++ (STL, do ingles *Standard Template Library*) fornece uma alternativa aos arrays chamada de classe template `vector`, e o C++11 acrescenta uma classe template `array`. Essas alternativas sao mais sofisticadas e flexiveis do que o tipo de array nativo. Este capitulo as discutira brevemente mais adiante, e o Capitulo 16, "A Classe `string` e a Biblioteca Padrao de Templates", as discutira de forma mais completa.

---

> Navegacao: [Anterior](capitulo-03.md) | [Indice](README.md) | [Proximo](capitulo-04-02-strings.md)
