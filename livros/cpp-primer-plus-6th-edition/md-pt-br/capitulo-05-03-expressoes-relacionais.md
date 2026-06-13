# Capitulo 5 - Loops e Expressoes Relacionais (Loops and Relational Expressions)
## Parte 3: Expressoes Relacionais e Comparacao de Strings

> Traducao nao oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Navegacao: [Anterior](capitulo-05-02-operadores.md) | [Indice](README.md) | [Proximo](capitulo-05-04-loop-while.md)

---

## Expressoes Relacionais

Os computadores sao mais do que infatigaveis processadores numericos. Eles tem a capacidade de comparar valores, e essa capacidade e a base da tomada de decisao dos computadores. Em C++, os **operadores relacionais** (relational operators) incorporam essa capacidade. O C++ fornece seis operadores relacionais para comparar numeros. Como os caracteres sao representados por seus codigos ASCII, voce pode usar esses operadores com caracteres tambem. Eles nao funcionam com strings de estilo C, mas funcionam com objetos da classe `string`. Cada expressao relacional resulta no valor `bool` `true` se a comparacao for verdadeira e no valor `bool` `false` se a comparacao for falsa, portanto esses operadores sao bem adequados para uso em uma expressao de teste de loop. (As implementacoes mais antigas avaliam expressoes relacionais verdadeiras como 1 e falsas como 0.) A Tabela 5.2 resume esses operadores.

**Tabela 5.2 — Operadores Relacionais**

| Operador | Significado                  |
|----------|------------------------------|
| `<`      | E menor que                  |
| `<=`     | E menor que ou igual a       |
| `==`     | E igual a                    |
| `>`      | E maior que                  |
| `>=`     | E maior que ou igual a       |
| `!=`     | Nao e igual a                |

Os seis operadores relacionais esgotam as comparacoes que o C++ permite que voce faca para numeros. Se voce quiser comparar dois valores para ver qual e mais bonito ou mais sortudo, tera que procurar em outro lugar.

Aqui estao alguns exemplos de testes:

```cpp
for (x = 20; x > 5; x--)   // continua enquanto x e maior que 5
for (x = 1; y != x; ++x)   // continua enquanto y nao e igual a x
for (cin >> x; x == 0; cin >> x) // continua enquanto x e 0
```

Os operadores relacionais tem precedencia menor do que os operadores aritmeticos. Isso significa que esta expressao:

```cpp
x + 3 > y - 2    // Expressao 1
```

corresponde a esta:

```cpp
(x + 3) > (y - 2) // Expressao 2
```

e nao a seguinte:

```cpp
x + (3 > y) - 2  // Expressao 3
```

Como a expressao `(3 > y)` e 1 ou 0 apos o valor `bool` ser promovido para `int`, as Expressoes 2 e 3 sao ambas validas. Mas a maioria de nos gostaria que a Expressao 1 significasse a Expressao 2, e e isso que o C++ faz.

## Atribuicao, Comparacao e um Erro que Voce Provavelmente Comete

Nao confunda o operador de teste e-igual-a (`==`) com o operador de atribuicao (`=`). Esta expressao faz a pergunta musical "O valor de `musicos` e igual a 4?":

```cpp
musicos == 4 // comparacao
```

A expressao tem o valor `true` ou `false`. Esta expressao atribui o valor 4 a `musicos`:

```cpp
musicos = 4 // atribuicao
```

A expressao inteira, neste caso, tem o valor 4 porque esse e o valor do lado esquerdo.

O design flexivel do loop `for` cria uma oportunidade interessante para erro. Se voce acidentalmente omitir um sinal de igual (`=`) do operador `==` e usar uma expressao de atribuicao em vez de uma expressao relacional para a parte de teste de um loop `for`, voce ainda produz codigo valido. Isso e porque voce pode usar qualquer expressao C++ valida para uma condicao de teste de loop `for`. Lembre-se de que valores diferentes de zero testam como `true`, e zero testa como `false`. Uma expressao que atribui 4 a `musicos` tem o valor 4 e e tratada como `true`. Se voce vem de uma linguagem, como Pascal ou BASIC, que usa `=` para testar igualdade, pode ser particularmente propenso a este deslize.

A Listagem 5.10 mostra uma situacao em que voce pode cometer esse tipo de erro. O programa tenta examinar um array de pontuacoes de questionario e para quando encontra a primeira pontuacao que nao e 20. Ele mostra um loop que usa corretamente a comparacao e depois um que usa erroneamente a atribuicao na condicao de teste. O programa tambem tem outro erro de design crasso que voce vera como corrigir mais tarde.

**Listagem 5.10 — equal.cpp**

```cpp
// equal.cpp -- igualdade versus atribuicao
#include <iostream>
int main()
{
    using namespace std;
    int notas[10] =
        { 20, 20, 20, 20, 20, 19, 20, 18, 20, 20};
    cout << "Fazendo certo:\n";
    int i;
    for (i = 0; notas[i] == 20; i++)
        cout << "nota " << i << " e 20\n";
    // Aviso: voce pode preferir ler sobre este programa
    // a realmente executa-lo.
    cout << "Fazendo perigosamente errado:\n";
    for (i = 0; notas[i] = 20; i++)
        cout << "nota " << i << " e 20\n";
    return 0;
}
```

Como o programa na Listagem 5.10 tem um problema serio, voce pode preferir ler sobre ele a realmente executa-lo. Aqui esta um exemplo de saida do programa:

```
Fazendo certo:
nota 0 e 20
nota 1 e 20
nota 2 e 20
nota 3 e 20
nota 4 e 20
Fazendo perigosamente errado:
nota 0 e 20
nota 1 e 20
nota 2 e 20
...
```

O primeiro loop para corretamente apos exibir as primeiras cinco pontuacoes. Mas o segundo comeca exibindo o array inteiro. Pior do que isso, ele diz que todo valor e 20. E pior ainda, ele nao para no final do array! E o pior de tudo e que o programa pode (embora nao necessariamente) congelar outros aplicativos em execucao no momento e exigir uma reinicializacao do computador.

Onde as coisas vao erradas, obviamente, e com a seguinte expressao de teste:

```cpp
notas[i] = 20
```

Primeiro, simplesmente porque atribui um valor diferente de zero ao elemento do array, a expressao e sempre diferente de zero, portanto sempre verdadeira. Segundo, porque a expressao atribui valores aos elementos do array, ela realmente muda os dados. Terceiro, porque a expressao de teste permanece verdadeira, o programa continua mudando dados alem do final do array. Ele continua colocando cada vez mais 20s na memoria! Isso nao e bom.

A dificuldade com esse tipo de erro e que o codigo e sintaticamente correto, portanto o compilador nao o marcara como um erro. (No entanto, anos e anos de programadores C e C++ cometendo esse erro levaram muitos compiladores a emitir um aviso, perguntando se e isso que voce realmente queria fazer.)

> **Cuidado:** Nao use `=` para comparar igualdade; use `==`.

Como o C, o C++ concede a voce mais liberdade do que a maioria das linguagens de programacao. Isso tem o custo de exigir maior responsabilidade da sua parte. Nada, exceto seu proprio bom planejamento, impede que um programa vá alem dos limites de um array C++ padrao. Porem, com as classes C++, voce pode projetar um tipo de array protegido que impede esse tipo de absurdo. O Capitulo 13, "Heranca de Classes", fornece um exemplo. Por enquanto, voce deve incorporar a protecao em seus programas quando precisar. Por exemplo, o loop na Listagem 5.10 deve incluir um teste que o impeca de ultrapassar o ultimo membro. Isso e verdade mesmo para o loop "correto". Se todas as pontuacoes fossem 20s, o loop "correto" tambem ultrapassaria os limites do array. Em suma, o loop precisa testar os valores do array e o indice do array. O Capitulo 6 mostra como usar operadores logicos para combinar dois testes desse tipo em uma unica condicao.

## Comparando Strings de Estilo C

Suponha que voce queira verificar se uma string em um array de caracteres e a palavra `mate`. Se `palavra` e o nome do array, o seguinte teste pode nao fazer o que voce pensa que deveria fazer:

```cpp
palavra == "mate"
```

Lembre-se de que o nome de um array e um sinonimo para seu endereco. Da mesma forma, uma constante de string entre aspas e um sinonimo para seu endereco. Portanto, a expressao relacional anterior nao testa se as strings sao as mesmas; ela verifica se elas estao armazenadas no mesmo endereco. A resposta para isso e nao, mesmo que as duas strings tenham os mesmos caracteres.

Como o C++ trata strings de estilo C como enderecos, voce nao obtem satisfacao se tentar usar os operadores relacionais para comparar strings. Em vez disso, voce pode recorrer a biblioteca de strings de estilo C e usar a funcao `strcmp()` para comparar strings. Essa funcao recebe dois enderecos de string como argumentos. Isso significa que os argumentos podem ser ponteiros, constantes de string ou nomes de arrays de caracteres. Se as duas strings forem identicas, a funcao retorna o valor 0. Se a primeira string precede a segunda em ordem alfabetica, `strcmp()` retorna um valor negativo, e se a primeira string segue a segunda em ordem alfabetica, `strcmp()` retorna um valor positivo. Na verdade, "na sequencia de ordenacao do sistema" e mais preciso do que "em ordem alfabetica". Isso significa que os caracteres sao comparados de acordo com o codigo do sistema para os caracteres. Por exemplo, no codigo ASCII, as letras maiusculas tem codigos menores do que as letras minusculas, portanto as maiusculas precedem as minusculas na sequencia de ordenacao. Portanto, a string `"Zoo"` precede a string `"aviao"`. O fato de que as comparacoes sao baseadas em valores de codigo tambem significa que as letras maiusculas e minusculas diferem, portanto a string `"MESA"` e diferente da string `"mesa"`.

Em algumas linguagens, como BASIC e Pascal padrao, strings armazenadas em arrays de diferentes tamanhos sao necessariamente desiguais entre si. Mas as strings de estilo C sao definidas pelo caractere nulo de terminacao, nao pelo tamanho do array contendo. Isso significa que duas strings podem ser identicas mesmo que estejam contidas em arrays de diferentes tamanhos:

```cpp
char grande[80] = "Daffy"; // 5 letras mais \0
char pequeno[6] = "Daffy"; // 5 letras mais \0
```

Alias, embora voce nao possa usar operadores relacionais para comparar strings, voce pode usa-los para comparar caracteres porque os caracteres sao na verdade tipos inteiros. Portanto, o seguinte e codigo valido, pelo menos para os conjuntos de caracteres ASCII e Unicode, para exibir os caracteres do alfabeto:

```cpp
for (ch = 'a'; ch <= 'z'; ch++)
    cout << ch;
```

O programa na Listagem 5.11 usa `strcmp()` na condicao de teste de um loop `for`. O programa exibe uma palavra, altera sua primeira letra, exibe a palavra novamente e continua ate que `strcmp()` determine que `palavra` e o mesmo que a string `"mate"`. Observe que a listagem inclui o arquivo `cstring` porque ele fornece um prototipo de funcao para `strcmp()`.

**Listagem 5.11 — compstr1.cpp**

```cpp
// compstr1.cpp -- comparando strings usando arrays
#include <iostream>
#include <cstring> // prototipo para strcmp()
int main()
{
    using namespace std;
    char palavra[5] = "?ate";
    for (char ch = 'a'; strcmp(palavra, "mate"); ch++)
    {
        cout << palavra << endl;
        palavra[0] = ch;
    }
    cout << "Apos o termino do loop, palavra e " << palavra << endl;
    return 0;
}
```

Aqui esta a saida do programa na Listagem 5.11:

```
?ate
aate
bate
cate
date
eate
fate
gate
hate
iate
jate
kate
late
Apos o termino do loop, palavra e mate
```

**Notas do Programa**

O programa na Listagem 5.11 tem alguns pontos interessantes. Um, claro, e o teste. Voce quer que o loop continue enquanto `palavra` nao for `mate`. Ou seja, voce quer que o teste continue enquanto `strcmp()` disser que as duas strings nao sao as mesmas. O teste mais obvio para isso e este:

```cpp
strcmp(palavra, "mate") != 0 // as strings nao sao as mesmas
```

Esta instrucao tem o valor 1 (verdadeiro) se as strings sao desiguais e o valor 0 (falso) se sao iguais. Mas e quanto a `strcmp(palavra, "mate")` sozinho? Ele tem um valor diferente de zero (verdadeiro) se as strings sao desiguais e o valor 0 (falso) se as strings sao iguais. Em essencia, a funcao retorna `true` se as strings sao diferentes e `false` se sao as mesmas. Voce pode usar apenas a funcao em vez de toda a expressao relacional. Isso produz o mesmo comportamento e envolve menos digitacao. Alem disso, e a maneira como os programadores C e C++ usaram tradicionalmente `strcmp()`.

> **Testando Igualdade ou Ordem**
> Voce pode usar `strcmp()` para testar strings de estilo C quanto a igualdade ou ordem. A seguinte expressao e verdadeira se `str1` e `str2` sao identicas:
> ```cpp
> strcmp(str1, str2) == 0
> ```
> As expressoes
> ```cpp
> strcmp(str1, str2) != 0
> ```
> e
> ```cpp
> strcmp(str1, str2)
> ```
> sao verdadeiras se `str1` e `str2` nao sao identicas. A seguinte expressao e verdadeira se `str1` precede `str2`:
> ```cpp
> strcmp(str1, str2) < 0
> ```
> E a seguinte expressao e verdadeira se `str1` segue `str2`:
> ```cpp
> strcmp(str1, str2) > 0
> ```
> Assim, a funcao `strcmp()` pode desempenhar o papel dos operadores `==`, `!=`, `<` e `>`, dependendo de como voce configura uma condicao de teste.

Em seguida, `compstr1.cpp` usa o operador de incremento para percorrer a variavel `ch` pelo alfabeto:

```cpp
ch++
```

Voce pode usar os operadores de incremento e decremento com variaveis de caractere porque o tipo `char` e realmente um tipo inteiro, portanto a operacao realmente muda o codigo inteiro armazenado na variavel. Alem disso, observe que usar um indice de array torna simples alterar caracteres individuais em uma string:

```cpp
palavra[0] = ch;
```

## Comparando Strings da Classe string

A vida e um pouco mais simples se voce usar strings da classe `string` em vez de strings de estilo C porque o design da classe permite que voce use operadores relacionais para fazer as comparacoes. Isso e possivel porque alguem pode definir funcoes de classe que "sobrecarregam", ou redefinem, os operadores. O Capitulo 12, "Classes e Alocacao Dinamica de Memoria", discute como incorporar esse recurso nos designs de classe, mas do ponto de vista pratico, tudo que voce precisa saber agora e que voce pode usar os operadores relacionais com objetos da classe `string`. A Listagem 5.12 revisa a Listagem 5.11 para usar um objeto `string` em vez de um array de `char`.

**Listagem 5.12 — compstr2.cpp**

```cpp
// compstr2.cpp -- comparando strings usando a classe string
#include <iostream>
#include <string> // classe string
int main()
{
    using namespace std;
    string palavra = "?ate";
    for (char ch = 'a'; palavra != "mate"; ch++)
    {
        cout << palavra << endl;
        palavra[0] = ch;
    }
    cout << "Apos o termino do loop, palavra e " << palavra << endl;
    return 0;
}
```

A saida do programa na Listagem 5.12 e a mesma que a do programa na Listagem 5.11.

**Notas do Programa**

Na Listagem 5.12, a seguinte condicao de teste usa um operador relacional com um objeto `string` a esquerda e uma string de estilo C a direita:

```cpp
palavra != "mate"
```

A forma como a classe `string` sobrecarrega o operador `!=` permite que voce o use contanto que pelo menos um dos operandos seja um objeto `string`; o operando restante pode ser um objeto `string` ou uma string de estilo C.

O design da classe `string` permite que voce use um objeto `string` como uma entidade singular, como na expressao de teste relacional, ou como um objeto agregado para o qual voce pode usar notacao de array para extrair caracteres individuais.

Como voce pode ver, voce pode obter os mesmos resultados com strings de estilo C e com objetos `string`, mas a programacao com objetos `string` e mais simples e intuitiva.

Por fim, ao contrario da maioria dos loops `for` que voce viu ate este ponto, os dois ultimos loops nao sao loops de contagem. Ou seja, eles nao executam um bloco de instrucoes um numero especificado de vezes. Em vez disso, cada um desses loops observa uma circunstancia particular (`palavra` sendo `"mate"`) para sinalizar que e hora de parar. Mais tipicamente, os programas C++ usam loops `while` para esse segundo tipo de teste, portanto vamos examinar essa forma a seguir.

---

> Navegacao: [Anterior](capitulo-05-02-operadores.md) | [Indice](README.md) | [Proximo](capitulo-05-04-loop-while.md)
