# Capitulo 4 - Tipos Compostos (Compound Types)
## Parte 5: Unioes e Enumeracoes

> Traducao nao oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Navegacao: [Anterior](capitulo-04-04-estruturas.md) | [Indice](README.md) | [Proximo](capitulo-04-06-ponteiros.md)

---

## Unioes

Uma **uniao** (union) e uma forma de dado que pode conter diferentes tipos de dados, mas apenas um tipo por vez. Ou seja, enquanto uma estrutura pode conter, digamos, um `int` e um `long` e um `double`, uma uniao pode conter um `int` ou um `long` ou um `double`. A sintaxe e semelhante a de uma estrutura, mas o significado e diferente. Por exemplo, considere a seguinte declaracao:

```cpp
union umParaTodos
{
    int val_int;
    long val_long;
    double val_double;
};
```

Voce pode usar uma variavel `umParaTodos` para conter um `int`, um `long` ou um `double`, desde que o faca em momentos diferentes:

```cpp
umParaTodos balde;
balde.val_int = 15;      // armazena um int
cout << balde.val_int;
balde.val_double = 1.38; // armazena um double, valor int e perdido
cout << balde.val_double;
```

Assim, `balde` pode servir como variavel `int` em uma ocasiao e como variavel `double` em outra. O nome do membro identifica em que capacidade a variavel esta atuando. Como uma uniao contem apenas um valor por vez, ela precisa ter espaco suficiente para conter seu maior membro. Portanto, o tamanho da uniao e o tamanho de seu maior membro.

Um uso para uma uniao e economizar espaco quando um item de dados pode usar dois ou mais formatos, mas nunca simultaneamente. Por exemplo, suponha que voce gerencie um estoque misto de produtos, alguns dos quais tem um ID inteiro, e outros tem um ID de string. Nesse caso, voce poderia usar o seguinte:

```cpp
struct produto
{
    char marca[20];
    int tipo;
    union id           // formato depende do tipo de produto
    {
        long id_num;   // produtos tipo 1
        char id_char[20]; // outros produtos
    } val_id;
};
...
produto premio;
...
if (premio.tipo == 1) // instrucao if-else (Capitulo 6)
    cin >> premio.val_id.id_num;  // usa nome do membro para indicar o modo
else
    cin >> premio.val_id.id_char;
```

Uma **uniao anonima** (anonymous union) nao tem nome; em essencia, seus membros se tornam variaveis que compartilham o mesmo endereco. Naturalmente, apenas um membro pode estar ativo de cada vez:

```cpp
struct produto
{
    char marca[20];
    int tipo;
    union             // uniao anonima
    {
        long id_num;     // produtos tipo 1
        char id_char[20]; // outros produtos
    };
};
...
produto premio;
...
if (premio.tipo == 1)
    cin >> premio.id_num;
else
    cin >> premio.id_char;
```

Como a uniao e anonima, `id_num` e `id_char` sao tratados como dois membros de `premio` que compartilham o mesmo endereco. A necessidade de um identificador intermediario `val_id` e eliminada. Cabe ao programador controlar qual opcao esta ativa.

As unioes frequentemente (mas nao exclusivamente) sao usadas para economizar espaco de memoria. Isso pode nao parecer tao necessario nos dias de gigabytes de RAM e terabytes de armazenamento, mas nem todos os programas C++ sao escritos para esses sistemas. O C++ tambem e usado para sistemas embarcados (embedded systems), como os processadores usados para controlar um forno de microondas, um player de MP3, ou um rover em Marte. Nessas aplicacoes, o espaco pode ser escasso. Alem disso, unioes frequentemente sao usadas quando se trabalha com estruturas de dados do sistema operacional ou de hardware.

## Enumeracoes

O recurso `enum` do C++ fornece uma alternativa a `const` para criar constantes simbolicas. Ele tambem permite definir novos tipos, mas de forma bastante restrita. A sintaxe para `enum` se assemelha a sintaxe de estrutura. Por exemplo, considere a instrucao a seguir:

```cpp
enum espectro {vermelho, laranja, amarelo, verde, azul, violeta, indigo, ultravioleta};
```

Esta instrucao faz duas coisas:

- Torna `espectro` o nome de um novo tipo; `espectro` e chamado de **enumeracao** (enumeration), assim como uma variavel `struct` e chamada de estrutura.
- Estabelece `vermelho`, `laranja`, `amarelo` e assim por diante como constantes simbolicas para os valores inteiros de 0 a 7. Essas constantes sao chamadas de **enumeradores** (enumerators).

Por padrao, os enumeradores recebem valores inteiros comecando com 0 para o primeiro enumerador, 1 para o segundo enumerador e assim por diante. Voce pode sobrescrever o padrao atribuindo valores inteiros explicitamente. Voce vera como fazer isso mais adiante neste capitulo.

Voce pode usar um nome de enumeracao para declarar uma variavel do tipo de enumeracao:

```cpp
espectro banda; // banda e uma variavel do tipo espectro
```

Uma variavel de enumeracao tem algumas propriedades especiais, que vamos examinar agora.

Os unicos valores validos que voce pode atribuir a uma variavel de enumeracao sem uma conversao de tipo (type cast) sao os valores de enumerador usados na definicao do tipo. Portanto, temos o seguinte:

```cpp
banda = azul;   // valido, azul e um enumerador
banda = 2000;   // invalido, 2000 nao e um enumerador
```

Assim, uma variavel `espectro` esta limitada a apenas oito valores possiveis. Alguns compiladores emitem um erro de compilacao se voce tentar atribuir um valor invalido, enquanto outros emitem um aviso. Para maxima portabilidade, voce deve tratar a atribuicao de um valor que nao e da enumeracao a uma variavel de enumeracao como um erro.

Apenas o operador de atribuicao e definido para enumeracoes. Em particular, operacoes aritmeticas nao sao definidas:

```cpp
banda = laranja;        // valido
++banda;                // invalido, ++ discutido no Capitulo 5
banda = laranja + vermelho; // invalido, mas um pouco complicado
...
```

No entanto, algumas implementacoes nao respeitam essa restricao. Isso pode tornar possivel violar os limites do tipo. Por exemplo, se `banda` tem o valor `ultravioleta`, ou 7, entao `++banda`, se valido, incrementa `banda` para 8, que nao e um valor valido para um tipo `espectro`. Novamente, para maxima portabilidade, voce deve adotar as restricoes mais rigorosas.

Os enumeradores sao do tipo inteiro e podem ser promovidos para o tipo `int`, mas os tipos `int` nao sao convertidos automaticamente para o tipo de enumeracao:

```cpp
int cor = azul;     // valido, tipo espectro promovido para int
banda = 3;          // invalido, int nao convertido para espectro
cor = 3 + vermelho; // valido, vermelho convertido para int
...
```

Note que neste exemplo, mesmo que `3` corresponda ao enumerador `verde`, atribuir `3` a `banda` e um erro de tipo. Mas atribuir `verde` a `banda` esta correto porque ambos sao do tipo `espectro`. Novamente, algumas implementacoes nao impoe essa restricao. Na expressao `3 + vermelho`, a adicao nao esta definida para enumeradores. Entretanto, `vermelho` e convertido para tipo `int`, e o resultado e do tipo `int`. Devido a conversao de enumeracao para `int` nessa situacao, voce pode usar enumeracoes em expressoes aritmeticas para combiná-las com inteiros ordinarios, mesmo que a aritmetica nao seja definida para as proprias enumeracoes.

O exemplo anterior:

```cpp
banda = laranja + vermelho; // invalido, mas um pouco complicado
```

falha por uma razao um tanto elaborada. E verdade que o operador `+` nao esta definido para enumeradores. Mas tambem e verdade que os enumeradores sao convertidos para inteiros quando usados em expressoes aritmeticas, portanto a expressao `laranja + vermelho` e convertida para `1 + 0`, que e uma expressao valida. Mas ela e do tipo `int` e, portanto, nao pode ser atribuida a variavel `espectro` do tipo `banda`.

Voce pode atribuir um valor `int` a uma enumeracao, desde que o valor seja valido e que voce use uma conversao de tipo explicita:

```cpp
banda = espectro(3); // converte 3 para o tipo espectro
```

E se voce tentar converter um valor inadequado? O resultado e indefinido, significando que a tentativa nao sera sinalizada como erro, mas voce nao pode confiar no valor do resultado:

```cpp
banda = espectro(40003); // indefinido
```

(Veja a secao "Intervalos de Valor para Enumeracoes", mais adiante neste capitulo, para uma discussao sobre quais valores sao e nao sao apropriados.)

Como voce pode ver, as regras que regem as enumeracoes sao bastante restritivas. Na pratica, as enumeracoes sao usadas com mais frequencia como uma forma de definir constantes simbolicas relacionadas do que como um meio de definir novos tipos. Por exemplo, voce pode usar uma enumeracao para definir constantes simbolicas para uma instrucao `switch`. (Veja o Capitulo 6, "Instrucoes de Ramificacao e Operadores Logicos", para um exemplo.) Se voce planeja usar apenas as constantes e nao criar variaveis do tipo de enumeracao, voce pode omitir um nome de tipo de enumeracao, como neste exemplo:

```cpp
enum {vermelho, laranja, amarelo, verde, azul, violeta, indigo, ultravioleta};
```

## Definindo Valores de Enumeradores

Voce pode definir valores de enumeradores explicitamente usando o operador de atribuicao:

```cpp
enum bits {um = 1, dois = 2, quatro = 4, oito = 8};
```

Os valores atribuidos devem ser inteiros. Voce tambem pode definir apenas alguns dos enumeradores explicitamente:

```cpp
enum passosGrandes {primeiro, segundo = 100, terceiro};
```

Nesse caso, `primeiro` e `0` por padrao. Enumeradores subsequentes nao inicializados sao maiores por um do que seus predecessores. Portanto, `terceiro` teria o valor `101`.

Por fim, voce pode criar mais de um enumerador com o mesmo valor:

```cpp
enum {zero, nulo = 0, um, numero_um = 1};
```

Aqui, tanto `zero` quanto `nulo` sao `0`, e tanto `um` quanto `numero_um` sao `1`. Em versoes anteriores do C++, voce so podia atribuir valores `int` (ou valores que promovem para `int`) aos enumeradores, mas essa restricao foi removida para que voce possa usar valores `long` ou ate mesmo `long long`.

## Intervalos de Valor para Enumeracoes

Originalmente, os unicos valores validos para uma enumeracao eram os nomeados na declaracao. No entanto, o C++ expandiu a lista de valores validos que podem ser atribuidos a uma variavel de enumeracao por meio do uso de uma conversao de tipo. Cada enumeracao tem um intervalo, e voce pode atribuir qualquer valor inteiro no intervalo, mesmo que nao seja um valor de enumerador, usando uma conversao de tipo para uma variavel de enumeracao. Por exemplo, suponha que `bits` e `minhaBandeira` sejam definidos desta forma:

```cpp
enum bits {um = 1, dois = 2, quatro = 4, oito = 8};
bits minhaBandeira;
```

Nesse caso, o seguinte e valido:

```cpp
minhaBandeira = bits(6); // valido, porque 6 esta no intervalo de bits
```

Aqui, `6` nao e um dos valores de enumeracao, mas esta no intervalo que as enumeracoes definem.

O intervalo e definido da seguinte forma. Primeiro, para encontrar o limite superior, voce pega o maior valor de enumerador. Em seguida, voce encontra a menor potencia de dois maior que esse maior valor e subtrai um; o resultado e o limite superior do intervalo. (Por exemplo, o maior valor de `passosGrandes`, conforme definido anteriormente, e `101`. A menor potencia de dois maior que isso e `128`, portanto o limite superior do intervalo e `127`.) Em seguida, para encontrar o limite inferior, voce encontra o menor valor de enumerador. Se for `0` ou maior, o limite inferior para o intervalo e `0`. Se o menor enumerador for negativo, voce usa a mesma abordagem para encontrar o limite superior, mas adiciona um sinal de menos. (Por exemplo, se o menor enumerador for `-6`, a proxima potencia de dois [vezes um sinal de menos] e `-8`, e o limite inferior e `-7`.)

A ideia e que o compilador pode escolher quanto espaco usar para armazenar uma enumeracao. Ele pode usar 1 byte ou menos para uma enumeracao com um intervalo pequeno e 4 bytes para uma enumeracao com valores do tipo `long`.

O C++11 estende as enumeracoes com uma forma chamada **enumeracao com escopo** (scoped enumeration). O Capitulo 10 discute brevemente essa forma na secao "Escopo de Classe".

---

> Navegacao: [Anterior](capitulo-04-04-estruturas.md) | [Indice](README.md) | [Proximo](capitulo-04-06-ponteiros.md)
