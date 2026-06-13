# Capítulo 8 — Templates de Funções, Resumo e Exercícios

> Tradução não oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Navegação: [Anterior](capitulo-08-02-argumentos-padrao-sobrecarga.md) | [Índice](README.md) | [Próximo](capitulo-09-01-compilacao-separada.md)

## Templates de Funções (Function Templates)

Os compiladores contemporâneos de C++ implementam uma das adições mais recentes ao C++: *templates de funções* (function templates). Um template de função é uma descrição genérica de função; ou seja, ele define uma função em termos de um tipo genérico para o qual um tipo específico, como `int` ou `double`, pode ser substituído. Ao passar um tipo como parâmetro para um template, você faz com que o compilador gere uma função para aquele tipo específico. Como os templates permitem programar em termos de um tipo genérico em vez de um tipo específico, o processo é às vezes chamado de *programação genérica* (generic programming). Como os tipos são representados por parâmetros, o recurso de template é às vezes referido como *tipos parametrizados* (parameterized types).

Anteriormente, a Listagem 8.4 definiu uma função que trocava dois valores `int`. Suponha que você queira trocar dois valores `double` em vez disso. Uma abordagem é duplicar o código original, mas substituir cada `int` por `double`. Se você precisa trocar dois valores `char`, pode usar a mesma técnica novamente. Ainda assim, é um desperdício do seu valioso tempo ter que fazer essas mudanças triviais, e sempre há a possibilidade de cometer um erro. O recurso de template de função do C++ automatiza o processo, economizando tempo e proporcionando maior confiabilidade.

Os templates de função permitem que você defina uma função em termos de algum tipo arbitrário. Por exemplo, você pode criar um template de troca como este:

```cpp
template <typename TipoQualquer>
void Trocar(TipoQualquer &a, TipoQualquer &b)
{
    TipoQualquer temp;
    temp = a;
    a = b;
    b = temp;
}
```

A primeira linha especifica que você está configurando um template e que está chamando o tipo arbitrário de `TipoQualquer`. As palavras-chave `template` e `typename` são obrigatórias, exceto que você pode usar a palavra-chave `class` em vez de `typename`. Além disso, você deve usar os colchetes angulares. O nome do tipo (`TipoQualquer`, neste exemplo) é sua escolha, desde que você siga as regras de nomenclatura usuais do C++; muitos programadores usam nomes simples como `T`. O restante do código descreve o algoritmo para trocar dois valores do tipo `TipoQualquer`. O template não cria nenhuma função. Em vez disso, ele fornece ao compilador instruções sobre como definir uma função. Se você quiser uma função que troque valores `int`, o compilador cria uma função seguindo o padrão do template, substituindo `int` por `TipoQualquer`.

Antes do Padrão C++98 adicionar a palavra-chave `typename` à linguagem, o C++ usava a palavra-chave `class` neste contexto particular. Ou seja, você pode escrever a definição do template desta forma:

```cpp
template <class TipoQualquer>
void Trocar(TipoQualquer &a, TipoQualquer &b)
{
    TipoQualquer temp;
    temp = a;
    a = b;
    b = temp;
}
```

A palavra-chave `typename` torna um pouco mais óbvio que o parâmetro `TipoQualquer` representa um tipo; no entanto, grandes bibliotecas de código já foram desenvolvidas usando a palavra-chave mais antiga `class`. O Padrão C++ trata as duas palavras-chave de forma idêntica quando usadas neste contexto. Este livro usa ambas as formas para que você fique familiarizado com elas quando as encontrar em outros lugares.

> **Dica:** Você deve usar templates se precisar de funções que aplicam o mesmo algoritmo a uma variedade de tipos. Se você não está preocupado com compatibilidade retroativa e pode lidar com o esforço de digitar uma palavra mais longa, pode usar a palavra-chave `typename` em vez de `class` ao declarar parâmetros de tipo.

Para informar ao compilador que você precisa de uma forma específica de função de troca, você simplesmente usa uma função chamada `Trocar()` em seu programa. O compilador verifica os tipos de argumento que você usa e, em seguida, gera a função correspondente. A Listagem 8.11 mostra como isso funciona. O programa segue o padrão usual para funções comuns, com um protótipo de função template perto do topo do arquivo e a definição da função template seguindo `main()`.

**Listagem 8.11** funtemp.cpp

```cpp
// funtemp.cpp -- usando um template de função
#include <iostream>
// protótipo do template de função
template <typename T> // ou class T
void Trocar(T &a, T &b);
int main()
{
    using namespace std;
    int i = 10;
    int j = 20;
    cout << "i, j = " << i << ", " << j << ".\n";
    cout << "Usando trocador int gerado pelo compilador:\n";
    Trocar(i, j); // gera void Trocar(int &, int &)
    cout << "Agora i, j = " << i << ", " << j << ".\n";
    double x = 24.5;
    double y = 81.7;
    cout << "x, y = " << x << ", " << y << ".\n";
    cout << "Usando trocador double gerado pelo compilador:\n";
    Trocar(x, y); // gera void Trocar(double &, double &)
    cout << "Agora x, y = " << x << ", " << y << ".\n";
    return 0;
}
// definição do template de função
template <typename T> // ou class T
void Trocar(T &a, T &b)
{
    T temp; // temp é uma variável do tipo T
    temp = a;
    a = b;
    b = temp;
}
```

A primeira função `Trocar()` na Listagem 8.11 tem dois argumentos `int`, então o compilador gera uma versão `int` da função. Ou seja, ele substitui cada uso de `T` por `int`, produzindo uma definição que se parece com esta:

```cpp
void Trocar(int &a, int &b)
{
    int temp;
    temp = a;
    a = b;
    b = temp;
}
```

Você não vê esse código, mas o compilador o gera e depois o usa no programa. A segunda função `Trocar()` tem dois argumentos `double`, então o compilador gera uma versão `double`.

Saída do programa:

```
i, j = 10, 20.
Usando trocador int gerado pelo compilador:
Agora i, j = 20, 10.
x, y = 24.5, 81.7.
Usando trocador double gerado pelo compilador:
Agora x, y = 81.7, 24.5.
```

Observe que templates de função não tornam programas executáveis mais curtos. Na Listagem 8.11, você ainda acaba com duas definições de função separadas, assim como teria se as definisse manualmente. E o código final não contém nenhum template; ele contém apenas as funções reais geradas para o programa. Os benefícios dos templates são que eles tornam a geração de múltiplas definições de função mais simples e confiável.

Mais tipicamente, templates são colocados em um arquivo de cabeçalho que é então incluído no arquivo que os usa. O Capítulo 9 discute arquivos de cabeçalho.

## Templates Sobrecarregados (Overloaded Templates)

Você usa templates quando precisa de funções que aplicam o mesmo algoritmo a uma variedade de tipos, como na Listagem 8.11. Pode ser, no entanto, que nem todos os tipos usem o mesmo algoritmo. Para lidar com essa possibilidade, você pode sobrecarregar definições de template, assim como você sobrecarrega definições de funções regulares. Como com a sobrecarga comum, templates sobrecarregados precisam de assinaturas de função distintas. Por exemplo, a Listagem 8.12 adiciona um novo template de troca — um para trocar elementos de dois arrays.

**Listagem 8.12** twotemps.cpp

```cpp
// twotemps.cpp -- usando funções template sobrecarregadas
#include <iostream>
template <typename T> // template original
void Trocar(T &a, T &b);

template <typename T> // novo template
void Trocar(T *a, T *b, int n);

void Exibir(int a[]);
const int Lim = 8;
int main()
{
    using namespace std;
    int i = 10, j = 20;
    cout << "i, j = " << i << ", " << j << ".\n";
    cout << "Usando trocador int gerado pelo compilador:\n";
    Trocar(i, j); // combina com template original
    cout << "Agora i, j = " << i << ", " << j << ".\n";
    int d1[Lim] = {0,7,0,4,1,7,7,6};
    int d2[Lim] = {0,7,2,0,1,9,6,9};
    cout << "Arrays originais:\n";
    Exibir(d1);
    Exibir(d2);
    Trocar(d1, d2, Lim); // combina com novo template
    cout << "Arrays trocados:\n";
    Exibir(d1);
    Exibir(d2);
    return 0;
}
template <typename T>
void Trocar(T &a, T &b)
{
    T temp;
    temp = a;
    a = b;
    b = temp;
}
template <typename T>
void Trocar(T a[], T b[], int n)
{
    T temp;
    for (int i = 0; i < n; i++)
    {
        temp = a[i];
        a[i] = b[i];
        b[i] = temp;
    }
}
void Exibir(int a[])
{
    using namespace std;
    cout << a[0] << a[1] << "/";
    cout << a[2] << a[3] << "/";
    for (int i = 4; i < Lim; i++)
        cout << a[i];
    cout << endl;
}
```

Saída do programa:

```
i, j = 10, 20.
Usando trocador int gerado pelo compilador:
Agora i, j = 20, 10.
Arrays originais:
07/04/1776
07/20/1969
Arrays trocados:
07/20/1969
07/04/1776
```

## Limitações de Templates (Template Limitations)

Suponha que você tenha uma função template:

```cpp
template <class T> // ou template <typename T>
void f(T a, T b)
{...}
```

Frequentemente o código faz suposições sobre quais operações são possíveis para o tipo. Por exemplo, a instrução a seguir assume que a atribuição está definida, o que não seria verdade se o tipo `T` for um tipo de array embutido:

```cpp
a = b;
```

Da mesma forma, o seguinte assume que `>` está definido, o que não é verdade se `T` for uma estrutura comum:

```cpp
if (a > b)
```

E o seguinte assume que o operador de multiplicação está definido para o tipo `T`, o que não é o caso se `T` for um array, um ponteiro ou uma estrutura:

```cpp
T c = a * b;
```

Em resumo, é fácil escrever uma função template que não pode lidar com certos tipos. Por outro lado, às vezes uma generalização faz sentido, mesmo que a sintaxe comum do C++ não a permita. Por exemplo, pode fazer sentido adicionar estruturas contendo coordenadas de posição, mesmo que o operador `+` não esteja definido para estruturas. Uma abordagem é que o C++ permite sobrecarregar o operador `+` para que ele possa ser usado com uma forma específica de estrutura ou classe. Um template que requer o uso do operador `+` pode então tratar uma estrutura que tenha um operador `+` sobrecarregado. Outra abordagem é fornecer definições de template especializadas para tipos específicos. Vamos examinar isso a seguir.

## Especializações Explícitas (Explicit Specializations)

Suponha que você defina uma estrutura como a seguinte:

```cpp
struct trabalho
{
    char nome[40];
    double salario;
    int andar;
};
```

Além disso, suponha que você queira ser capaz de trocar o conteúdo de dois objetos desse tipo. O template original usa o seguinte código para efetuar uma troca:

```cpp
temp = a;
a = b;
b = temp;
```

Como o C++ permite atribuir uma estrutura a outra, isso funciona corretamente, mesmo que o tipo `T` seja uma estrutura `trabalho`. Mas suponha que você queira apenas trocar os membros `salario` e `andar`, mantendo os membros `nome` inalterados. Isso requer código diferente, mas os argumentos para `Trocar()` seriam os mesmos que no primeiro caso (referências a duas estruturas `trabalho`), então você não pode usar sobrecarga de template para fornecer o código alternativo.

No entanto, você pode fornecer uma definição de função especializada, chamada de *especialização explícita* (explicit specialization), com o código necessário. Se o compilador encontrar uma definição especializada que corresponde exatamente a uma chamada de função, ele usa essa definição sem procurar templates.

### Especialização de Terceira Geração (ISO/ANSI C++ Standard)

O Padrão C++98 estabeleceu esta abordagem:

- Para um dado nome de função, você pode ter uma função não-template, uma função template e uma função template com especialização explícita, juntamente com versões sobrecarregadas de todos esses.
- O protótipo e a definição para uma especialização explícita devem ser precedidos por `template <>` e devem mencionar o tipo especializado pelo nome.
- Uma especialização substitui o template regular, e uma função não-template substitui ambos.

Veja como os protótipos para trocar estruturas do tipo `trabalho` ficariam para essas três formas:

```cpp
// protótipo de função não-template
void Trocar(trabalho &, trabalho &);
// protótipo de template
template <typename T>
void Trocar(T &, T &);
// especialização explícita para o tipo trabalho
template <> void Trocar<trabalho>(trabalho &, trabalho &);
```

O compilador escolhe a versão não-template sobre especializações explícitas e versões de template, e escolhe uma especialização explícita sobre uma versão gerada a partir de um template.

### Um Exemplo de Especialização Explícita (An Example of Explicit Specialization)

A Listagem 8.13 ilustra como a especialização explícita funciona.

**Listagem 8.13** twoswap.cpp

```cpp
// twoswap.cpp -- especialização substitui um template
#include <iostream>
template <typename T>
void Trocar(T &a, T &b);
struct trabalho
{
    char nome[40];
    double salario;
    int andar;
};
// especialização explícita
template <> void Trocar<trabalho>(trabalho &j1, trabalho &j2);
void Exibir(trabalho &j);
int main()
{
    using namespace std;
    cout.precision(2);
    cout.setf(ios::fixed, ios::floatfield);
    int i = 10, j = 20;
    cout << "i, j = " << i << ", " << j << ".\n";
    cout << "Usando trocador int gerado pelo compilador:\n";
    Trocar(i, j); // gera void Trocar(int &, int &)
    cout << "Agora i, j = " << i << ", " << j << ".\n";
    trabalho lia = {"Lia Souza", 73000.60, 7};
    trabalho paulo = {"Paulo Taffa", 78060.72, 9};
    cout << "Antes da troca de empregos:\n";
    Exibir(lia);
    Exibir(paulo);
    Trocar(lia, paulo); // usa void Trocar(trabalho &, trabalho &)
    cout << "Apos a troca de empregos:\n";
    Exibir(lia);
    Exibir(paulo);
    return 0;
}
template <typename T>
void Trocar(T &a, T &b) // versão geral
{
    T temp;
    temp = a;
    a = b;
    b = temp;
}
// troca apenas os campos salario e andar de uma estrutura trabalho
template <> void Trocar<trabalho>(trabalho &j1, trabalho &j2) // especialização
{
    double t1;
    int t2;
    t1 = j1.salario;
    j1.salario = j2.salario;
    j2.salario = t1;
    t2 = j1.andar;
    j1.andar = j2.andar;
    j2.andar = t2;
}
void Exibir(trabalho &j)
{
    using namespace std;
    cout << j.nome << ": R$" << j.salario
         << " no andar " << j.andar << endl;
}
```

Saída do programa:

```
i, j = 10, 20.
Usando trocador int gerado pelo compilador:
Agora i, j = 20, 10.
Antes da troca de empregos:
Lia Souza: R$73000.60 no andar 7
Paulo Taffa: R$78060.72 no andar 9
Apos a troca de empregos:
Lia Souza: R$78060.72 no andar 9
Paulo Taffa: R$73000.60 no andar 7
```

## Instanciações e Especializações (Instantiations and Specializations)

Para ampliar sua compreensão de templates, vamos investigar os termos *instanciação* (instantiation) e *especialização* (specialization). Tenha em mente que incluir um template de função no seu código não gera por si só uma definição de função. É apenas um plano para gerar uma definição de função. Quando o compilador usa o template para gerar uma definição de função para um tipo específico, o resultado é chamado de *instanciação* (instantiation) do template. Por exemplo, na Listagem 8.13, a chamada de função `Trocar(i, j)` faz com que o compilador gere uma instanciação de `Trocar()`, usando `int` como o tipo. O template *não* é uma definição de função, mas a instanciação específica usando `int` *é* uma definição de função. Esse tipo de instanciação é chamado de *instanciação implícita* (implicit instantiation) porque o compilador deduz a necessidade de criar a definição ao notar que o programa usa uma função `Trocar()` com parâmetros `int`.

Originalmente, usar instanciação implícita era a única forma de o compilador gerar definições de funções a partir de templates, mas agora o C++ permite *instanciação explícita* (explicit instantiation). Isso significa que você pode instruir o compilador a criar uma instanciação específica — por exemplo, `Trocar<int>()` — diretamente. A sintaxe é declarar a variedade específica desejada, usando a notação `<>` para indicar o tipo e prefixando a declaração com a palavra-chave `template`:

```cpp
template void Trocar<int>(int, int); // instanciação explícita
```

Um compilador que implementa esse recurso, ao ver esta declaração, usará o template `Trocar()` para gerar uma instanciação usando o tipo `int`. Contraste a instanciação explícita com a especialização explícita, que usa uma ou outra dessas declarações equivalentes:

```cpp
template <> void Trocar<int>(int &, int &); // especialização explícita
template <> void Trocar(int &, int &);      // especialização explícita
```

A diferença é que essas últimas duas declarações significam "Não use o template `Trocar()` para gerar uma definição de função. Em vez disso, use uma definição de função separada e especializada explicitamente definida para o tipo `int`." A declaração de especialização explícita tem `<>` após a palavra-chave `template`, enquanto a instanciação explícita omite o `<>`.

> **Cuidado:** É um erro tentar usar tanto uma instanciação explícita quanto uma especialização explícita para os mesmos tipo(s) no mesmo arquivo ou, mais geralmente, na mesma unidade de tradução.

Instanciações explícitas também podem ser criadas usando a função em um programa. Por exemplo, considere o seguinte:

```cpp
template <class T>
T Somar(T a, T b) // passagem por valor
{
    return a + b;
}
...
int m = 6;
double x = 10.2;
cout << Somar<double>(x, m) << endl; // instanciação explícita
```

O template não combinaria com a chamada de função `Somar(x, m)` porque o template espera que ambos os argumentos de função sejam do mesmo tipo. Mas usar `Somar<double>(x, m)` força a instanciação do tipo `double`, e o argumento `m` sofre conversão de tipo para o tipo `double` para corresponder ao segundo parâmetro da função `Somar<double>(double, double)`.

Instanciações implícitas, instanciações explícitas e especializações explícitas são coletivamente chamadas de *especializações* (specializations). O que todas têm em comum é que representam uma definição de função que usa tipos específicos em vez de ser uma descrição genérica.

O seguinte fragmento resume esses conceitos:

```cpp
template <class T>
void Trocar(T &, T &); // protótipo de template
template <> void Trocar<trabalho>(trabalho &, trabalho &); // especialização explícita para trabalho
int main(void)
{
    template void Trocar<char>(char &, char &); // instanciação explícita para char
    short a, b;
    ...
    Trocar(a, b); // instanciação implícita de template para short
    trabalho n, m;
    ...
    Trocar(n, m); // usa especialização explícita para trabalho
    char g, h;
    ...
    Trocar(g, h); // usa instanciação explícita de template para char
    ...
}
```

## Qual Versão da Função o Compilador Escolhe? (Which Function Version Does the Compiler Pick?)

Com sobrecarga de função, templates de função e sobrecarga de templates de função, o C++ precisa, e tem, uma estratégia bem definida para decidir qual definição de função usar para uma chamada de função, especialmente quando há múltiplos argumentos. O processo é chamado de *resolução de sobrecarga* (overload resolution). Detalhar a estratégia completa levaria um pequeno capítulo, então vamos dar apenas uma visão geral de como o processo funciona:

- **Fase 1** — Montar uma lista de funções candidatas. Estas são funções e funções template que têm o mesmo nome que as funções chamadas.
- **Fase 2** — A partir das funções candidatas, montar uma lista de funções viáveis. Estas são funções com o número correto de argumentos e para as quais existe uma sequência de conversão implícita, o que inclui o caso de uma correspondência exata para cada tipo de argumento real ao tipo do argumento formal correspondente.
- **Fase 3** — Determinar se há uma melhor função viável. Se houver, você usa essa função. Caso contrário, a chamada de função é um erro.

Considere um caso com apenas um argumento de função — por exemplo, a seguinte chamada:

```cpp
maio('B'); // argumento real é do tipo char
```

Primeiro, o compilador reúne os suspeitos, que são funções e templates de função com o nome `maio()`. Então encontra aqueles que podem ser chamados com um argumento:

```cpp
void maio(int);            // #1
float maio(float, float = 3); // #2
void maio(char);           // #3
char * maio(const char *); // #4
char maio(const char &);   // #5
template<class T> void maio(const T &); // #6
template<class T> void maio(T *);      // #7
```

Observe que apenas as assinaturas, e não os tipos de retorno, são considerados. Dois desses candidatos (`#4` e `#7`), no entanto, não são viáveis porque um tipo integral não pode ser convertido implicitamente para um tipo de ponteiro. O template restante é viável porque pode ser usado para gerar uma especialização, com `T` tomado como tipo `char`. Isso deixa cinco funções viáveis.

Em seguida, o compilador tem que determinar qual das funções viáveis é a melhor. Ele examina a conversão necessária para fazer o argumento da chamada de função corresponder ao argumento do candidato viável. Em geral, a classificação do melhor para o pior é esta:

1. Correspondência exata (exact match), com funções regulares superando templates
2. Conversão por promoção (por exemplo, as conversões automáticas de `char` e `short` para `int` e de `float` para `double`)
3. Conversão por conversão padrão (por exemplo, converter `int` para `char` ou `long` para `double`)
4. Conversões definidas pelo usuário, como as definidas em declarações de classe

Por exemplo, a Função `#1` é melhor que a Função `#2` porque a conversão char-para-int é uma promoção, enquanto char-para-float é uma conversão padrão. As Funções `#3`, `#5` e `#6` são melhores do que `#1` ou `#2` porque são correspondências exatas. Tanto `#3` quanto `#5` são melhores que `#6` porque `#6` é um template.

### Correspondências Exatas e Melhores Correspondências (Exact Matches and Best Matches)

O C++ permite algumas "conversões triviais" ao fazer uma correspondência exata. A Tabela 8.1 lista-as, com `Tipo` representando algum tipo arbitrário. Por exemplo, um argumento real `int` é uma correspondência exata para um parâmetro formal `int &`. Observe que `Tipo` pode ser algo como `char &`, então essas regras incluem a conversão de `char &` para `const char &`.

**Tabela 8.1** — Conversões Triviais Permitidas para Correspondência Exata

| De um Argumento Real | Para um Argumento Formal |
|---------------------|--------------------------|
| `Tipo`              | `Tipo &`                 |
| `Tipo &`            | `Tipo`                   |
| `Tipo[]`            | `*Tipo`                  |
| `Tipo(lista-arg)`   | `Tipo(*)(lista-arg)`     |
| `Tipo`              | `const Tipo`             |
| `Tipo`              | `volatile Tipo`          |
| `Tipo *`            | `const Tipo *`           |
| `Tipo *`            | `volatile Tipo *`        |

Suponha que você tenha o seguinte código de função:

```cpp
struct mancha {int a; char b[10];};
mancha tinta = {25, "pontos"};
...
reciclar(tinta);
```

Nesse caso, todos os seguintes protótipos seriam correspondências exatas:

```cpp
void reciclar(mancha);        // #1 mancha-para-mancha
void reciclar(const mancha);  // #2 mancha-para-(const mancha)
void reciclar(mancha &);      // #3 mancha-para-(mancha &)
void reciclar(const mancha &); // #4 mancha-para-(const mancha &)
```

Como seria de esperar, o resultado de ter vários protótipos correspondentes é que o compilador não pode concluir o processo de resolução de sobrecarga. Não há melhor função viável, e o compilador gera uma mensagem de erro, provavelmente usando palavras como "ambíguo".

No entanto, às vezes pode haver resolução de sobrecarga mesmo que duas funções sejam correspondências exatas. Primeiro, ponteiros e referências a dados não-`const` são preferencialmente combinados a parâmetros de ponteiro e referência não-`const`. Ou seja, se apenas as Funções `#3` e `#4` estivessem disponíveis no exemplo de `reciclar()`, `#3` seria escolhida porque `tinta` não foi declarada como `const`. No entanto, essa discriminação entre `const` e não-`const` se aplica apenas a dados referenciados por ponteiros e referências.

Outro caso em que uma correspondência exata é melhor do que outra é quando uma função é uma função não-template e a outra não é. Nesse caso, a não-template é considerada melhor do que um template, incluindo especializações explícitas.

Se você acabar com duas correspondências exatas que são ambas funções template, o template de função que é o mais especializado, se um deles for, é a melhor função. Isso significa, por exemplo, que uma especialização explícita é escolhida sobre uma gerada implicitamente a partir do padrão do template.

O termo *mais especializado* não implica necessariamente uma especialização explícita; de forma mais geral, indica que menos conversões ocorrem quando o compilador deduz qual tipo usar. Por exemplo, considere os dois templates seguintes:

```cpp
template <class Tipo> void reciclar(Tipo t);    // #1
template <class Tipo> void reciclar(Tipo * t);  // #2
```

Suponha que o programa que contém esses templates também contenha o seguinte código:

```cpp
struct mancha {int a; char b[10];};
mancha tinta = {25, "pontos"};
...
reciclar(&tinta); // endereço de uma estrutura
```

A chamada `reciclar(&tinta)` combina com o Template `#1`, com `Tipo` interpretado como `mancha *`. A chamada de função `reciclar(&tinta)` também combina com o Template `#2`, desta vez com `Tipo` sendo `tinta`. Dessas duas funções template, `reciclar<mancha *>(mancha *)` é considerada a mais especializada porque sofreu menos conversões ao ser gerada. No Template `#2`, `Tipo` já estava especializado como um ponteiro, portanto é "mais especializado".

As regras para encontrar o template mais especializado são chamadas de *regras de ordenamento parcial* (partial ordering rules) para templates de função.

### Um Exemplo de Regras de Ordenamento Parcial (A Partial Ordering Rules Example)

A Listagem 8.14 tem dois templates de definição para exibir o conteúdo de um array. O primeiro (Template A) assume que o array passado como argumento contém os dados a serem exibidos. O segundo (Template B) assume que os elementos do array são ponteiros para os dados a serem exibidos.

**Listagem 8.14** tempover.cpp

```cpp
// tempover.cpp -- sobrecarga de template
#include <iostream>
template <typename T> // template A
void ExibirArray(T arr[], int n);

template <typename T> // template B
void ExibirArray(T * arr[], int n);
struct dividas
{
    char nome[50];
    double valor;
};
int main()
{
    using namespace std;
    int coisas[6] = {13, 31, 103, 301, 310, 130};
    struct dividas sr_E[3] =
    {
        {"Ima Wolfe", 2400.0},
        {"Ura Foxe", 1300.0},
        {"Iby Stout", 1800.0}
    };
    double * pd[3];
    // define ponteiros para os membros valor das estruturas em sr_E
    for (int i = 0; i < 3; i++)
        pd[i] = &sr_E[i].valor;
    cout << "Listando a contagem de coisas do Sr. E:\n";
    // coisas é um array de int
    ExibirArray(coisas, 6); // usa template A
    cout << "Listando as dividas do Sr. E:\n";
    // pd é um array de ponteiros para double
    ExibirArray(pd, 3); // usa template B (mais especializado)
    return 0;
}
template <typename T>
void ExibirArray(T arr[], int n)
{
    using namespace std;
    cout << "template A\n";
    for (int i = 0; i < n; i++)
        cout << arr[i] << ' ';
    cout << endl;
}
template <typename T>
void ExibirArray(T * arr[], int n)
{
    using namespace std;
    cout << "template B\n";
    for (int i = 0; i < n; i++)
        cout << *arr[i] << ' ';
    cout << endl;
}
```

Saída do programa:

```
Listando a contagem de coisas do Sr. E:
template A
13 31 103 301 310 130
Listando as dividas do Sr. E:
template B
2400 1300 1800
```

O identificador `coisas` é o nome de um array de `int`, portanto combina com o Template A com `T` tomado como tipo `int`. Para `pd` (um array de `double *`), ambos os templates combinam — com o Template A, `T` seria `double *`; com o Template B, `T` seria `double`. O Template B é mais especializado porque já afirma explicitamente que o conteúdo do array é um ponteiro. Se você remover o Template B do programa, o compilador usa o Template A para listar o conteúdo de `pd`, exibindo os endereços em vez dos valores.

Em resumo, o processo de resolução de sobrecarga procura a função que é a melhor correspondência. Se apenas uma existe, essa função é escolhida. Se mais de uma é igualmente boa, mas apenas uma é uma função não-template, essa não-template é escolhida. Se mais de um candidato são template, mas um template é mais especializado que os demais, esse é escolhido. Se há dois ou mais não-templates igualmente bons ou dois ou mais templates igualmente bons, nenhum dos quais é mais especializado que os demais, a chamada de função é ambígua e um erro.

### Fazendo Suas Próprias Escolhas (Making Your Own Choices)

Em algumas circunstâncias, você pode levar o compilador a fazer a escolha que você quer escrevendo a chamada de função de forma adequada. A Listagem 8.15 elimina o protótipo de template e coloca a definição da função template no topo do arquivo. Como com funções regulares, uma definição de função template pode agir como seu próprio protótipo se aparecer antes que a função seja usada.

**Listagem 8.15** choices.cpp

```cpp
// choices.cpp -- escolhendo um template
#include <iostream>

template<class T> // ou template <typename T>
T menor(T a, T b) // #1
{
    return a < b ? a : b;
}
int menor(int a, int b) // #2
{
    a = a < 0 ? -a : a;
    b = b < 0 ? -b : b;
    return a < b ? a : b;
}
int main()
{
    using namespace std;
    int m = 20;
    int n = -30;
    double x = 15.5;
    double y = 25.9;
    cout << menor(m, n) << endl;       // usa #2
    cout << menor(x, y) << endl;       // usa #1 com double
    cout << menor<>(m, n) << endl;     // usa #1 com int
    cout << menor<int>(x, y) << endl;  // usa #1 com int
    return 0;
}
```

Saída do programa:

```
-30
15.5
20
15
```

A Listagem 8.15 fornece um template que retorna o menor de dois valores e uma função padrão que retorna o menor valor absoluto de dois valores. Considere a seguinte instrução:

```cpp
cout << menor(m, n) << endl; // usa #2
```

Os argumentos da chamada de função combinam tanto com a função template quanto com a função não-template, então a função não-template é escolhida e retorna o valor `20` (pois `|-30| = 30 > 20`).

A seguir, a chamada de função na instrução combina com o template, com o tipo `T` tomado como `double`:

```cpp
cout << menor(x, y) << endl; // usa #1 com double
```

Agora considere esta instrução:

```cpp
cout << menor<>(m, n) << endl; // usa #1 com int
```

A presença dos colchetes angulares em `menor<>(m, n)` indica que o compilador deve escolher uma função template em vez de uma função não-template, e o compilador, ao notar que os argumentos reais são do tipo `int`, instancia o template usando `int` para `T`.

Finalmente, considere esta instrução:

```cpp
cout << menor<int>(x, y) << endl; // usa #1 com int
```

Aqui temos um pedido de instanciação explícita usando `int` para `T`, e essa é a função que é usada. Os valores de `x` e `y` sofrem conversão de tipo para o tipo `int`, e a função retorna um valor `int`, razão pela qual o programa exibe `15` em vez de `15.5`.

## Evolução do Template de Função (Template Function Evolution)

Nos primeiros dias do C++, a maioria das pessoas não imaginava o quão poderosos e úteis os templates de funções e templates de classes se mostrariam. Mas programadores inteligentes e dedicados expandiram as ideias do que era possível. Seu feedback levou a algumas mudanças no Padrão C++11.

### Qual É Esse Tipo? (What's That Type?)

Um problema é que, quando você escreve uma função template, nem sempre é possível no C++98 saber qual tipo usar em uma declaração. Considere este exemplo parcial:

```cpp
template<class T1, class T2>
void ft(T1 x, T2 y)
{
    ...
    ?tipo? xpy = x + y;
    ...
}
```

Qual deve ser o tipo para `xpy`? Não sabemos de antemão como `ft()` pode ser usada. O tipo adequado pode ser `T1` ou `T2` ou algum outro tipo inteiramente. Por exemplo, `T1` pode ser `double` e `T2` pode ser `int`, caso em que o tipo da soma é `double`. Ou `T1` pode ser `short` e `T2` pode ser `int`, caso em que o tipo da soma é `int`. Portanto, no C++98, não há escolha óbvia para o tipo de `xpy`.

### A Palavra-chave `decltype` (C++11)

A solução do C++11 é uma nova palavra-chave: `decltype`. Ela pode ser usada desta forma:

```cpp
int x;
decltype(x) y; // torna y do mesmo tipo que x
```

O argumento para `decltype` pode ser uma expressão, então no exemplo de `ft()` poderíamos usar este código:

```cpp
decltype(x + y) xpy; // torna xpy do mesmo tipo que x + y
xpy = x + y;
```

Alternativamente, poderíamos combinar essas duas instruções em uma inicialização:

```cpp
decltype(x + y) xpy = x + y;
```

Então podemos corrigir o template `ft()` desta forma:

```cpp
template<class T1, class T2>
void ft(T1 x, T2 y)
{
    ...
    decltype(x + y) xpy = x + y;
    ...
}
```

O mecanismo `decltype` é um pouco mais complexo do que pode parecer a partir desses exemplos. O compilador precisa percorrer uma lista de verificação para decidir sobre o tipo. Suponha que temos o seguinte:

```cpp
decltype(expressao) var;
```

Aqui está uma versão um pouco simplificada da lista:

- **Estágio 1:** Se `expressao` é um identificador sem parênteses extras (ou seja, sem parênteses adicionais), então `var` é do mesmo tipo que o identificador, incluindo qualificadores como `const`:

```cpp
double x = 5.5;
double y = 7.9;
double & rx = x;
const double * pd;
decltype(x) w;    // w é do tipo double
decltype(rx) u = y; // u é do tipo double &
decltype(pd) v;   // v é do tipo const double *
```

- **Estágio 2:** Se `expressao` é uma chamada de função, então `var` tem o tipo do tipo de retorno da função:

```cpp
long certamente(int);
decltype(certamente(3)) m; // m é do tipo long
```

> **Nota:** A expressão de chamada não é avaliada. Neste caso, o compilador examina o protótipo para obter o tipo de retorno; não há necessidade de chamar a função de verdade.

- **Estágio 3:** Se `expressao` é um lvalue, então `var` é uma referência ao tipo da expressão. Para este estágio se aplicar, `expressao` não pode ser um identificador sem parênteses. Uma possibilidade óbvia é um identificador entre parênteses:

```cpp
double xx = 4.4;
decltype ((xx)) r2 = xx; // r2 é double &
decltype(xx) w = xx;     // w é double (correspondência do Estágio 1)
```

- **Estágio 4:** Se nenhum dos casos especiais anteriores se aplica, `var` é do mesmo tipo que `expressao`:

```cpp
int j = 3;
int & k = j;
int & n = j;
decltype(j + 6) i1; // i1 é do tipo int
decltype(100L) i2;  // i2 é do tipo long
decltype(k + n) i3; // i3 é do tipo int (k+n não é referência)
```

Se você precisar de mais de uma declaração, pode usar `typedef` com `decltype`:

```cpp
template<class T1, class T2>
void ft(T1 x, T2 y)
{
    ...
    typedef decltype(x + y) tipo_xy;
    tipo_xy xpy = x + y;
    tipo_xy arr[10];
    tipo_xy & rxy = arr[2]; // rxy é uma referência
    ...
}
```

### Sintaxe de Função Alternativa: Tipo de Retorno à Direita (C++11 Trailing Return Type)

O mecanismo `decltype` por si só deixa outro problema relacionado sem solução. Considere esta função template incompleta:

```cpp
template<class T1, class T2>
?tipo? gt(T1 x, T2 y)
{
    ...
    return x + y;
}
```

Novamente, não sabemos de antemão qual tipo resulta da adição de `x` e `y`. Pode parecer que poderíamos usar `decltype(x + y)` para o tipo de retorno. Infelizmente, naquele ponto do código, os parâmetros `x` e `y` ainda não foram declarados, então não estão em escopo. Para tornar isso possível, o C++11 permite uma nova sintaxe para declarar e definir funções. A prototipagem:

```cpp
double h(int x, float y);
```

pode ser escrita com esta sintaxe alternativa:

```cpp
auto h(int x, float y) -> double;
```

Isso move o tipo de retorno para depois das declarações de parâmetros. A combinação `-> double` é chamada de *tipo de retorno à direita* (trailing return type). Aqui, `auto`, em outro papel novo do C++11, é um espaço reservado para o tipo fornecido pelo tipo de retorno à direita. A mesma forma seria usada com a definição da função:

```cpp
auto h(int x, float y) -> double
{/* corpo da função */};
```

Combinar essa sintaxe com `decltype` leva à seguinte solução para especificar o tipo de retorno para `gt()`:

```cpp
template<class T1, class T2>
auto gt(T1 x, T2 y) -> decltype(x + y)
{
    ...
    return x + y;
}
```

Agora `decltype` vem depois das declarações de parâmetros, então `x` e `y` estão em escopo e podem ser usados.

## Resumo (Summary)

O C++ expandiu as capacidades das funções em C. Usando a palavra-chave `inline` com uma definição de função e colocando essa definição antes da primeira chamada à função, você sugere ao compilador C++ que torne a função inline. Ou seja, em vez de fazer o programa saltar para uma seção separada do código para executar a função, o compilador substitui a chamada de função pelo código correspondente inline. Um recurso inline deve ser usado apenas quando a função for curta.

Uma variável de referência é um tipo de ponteiro disfarçado que permite criar um alias (ou seja, um segundo nome) para uma variável. Variáveis de referência são usadas principalmente como argumentos para funções que processam estruturas e objetos de classe. Normalmente, um identificador declarado como referência para um tipo específico só pode se referir a dados daquele tipo. No entanto, quando uma classe é derivada de outra, como `ofstream` de `ostream`, uma referência ao tipo base também pode se referir ao tipo derivado.

Os protótipos do C++ permitem que você defina valores padrão para argumentos. Se uma chamada de função omitir o argumento correspondente, o programa usa o valor padrão. Se a chamada de função incluir um valor de argumento, o programa usa esse valor em vez do padrão. Argumentos padrão podem ser fornecidos apenas da direita para a esquerda na lista de argumentos. Assim, se você fornecer um valor padrão para um argumento específico, também deve fornecer valores padrão para todos os argumentos à direita daquele argumento.

A assinatura de uma função é sua lista de argumentos. Você pode definir duas funções com o mesmo nome, desde que tenham assinaturas diferentes. Isso é chamado de polimorfismo de função (function polymorphism), ou sobrecarga de função (function overloading). Tipicamente, você sobrecarrega funções para fornecer essencialmente o mesmo serviço para diferentes tipos de dados.

Templates de funções automatizam o processo de sobrecarga de funções. Você define uma função usando um tipo genérico e um algoritmo específico, e o compilador gera definições de função apropriadas para os tipos de argumento específicos que você usa em um programa.

## Revisão do Capítulo (Chapter Review)

1. Que tipos de funções são bons candidatos para o status inline?

2. Suponha que a função `musica()` tenha este protótipo:

   ```cpp
   void musica(const char * nome, int vezes);
   ```

   a. Como você modificaria o protótipo para que o valor padrão de `vezes` seja `1`?
   b. Que mudanças você faria na definição da função?
   c. Você pode fornecer um valor padrão de `"O Meu Pai"` para `nome`?

3. Escreva versões sobrecarregadas de `icitar()`, uma função que exibe seu argumento entre aspas duplas. Escreva três versões: uma para um argumento `int`, uma para um argumento `double` e uma para um argumento `string`.

4. O seguinte é um template de estrutura:

   ```cpp
   struct caixa
   {
       char fabricante[40];
       float altura;
       float largura;
       float comprimento;
       float volume;
   };
   ```

   a. Escreva uma função que tem uma referência para uma estrutura `caixa` como seu argumento formal e exibe o valor de cada membro.
   b. Escreva uma função que tem uma referência para uma estrutura `caixa` como seu argumento formal e define o membro `volume` como o produto das outras três dimensões.

5. Que mudanças precisariam ser feitas na Listagem 7.15 para que as funções `preencher()` e `mostrar()` usem parâmetros de referência?

6. A seguir estão alguns efeitos desejados. Indique se cada um pode ser realizado com argumentos padrão, sobrecarga de funções, ambos ou nenhum. Forneça os protótipos apropriados.

   a. `massa(densidade, volume)` retorna a massa de um objeto tendo uma densidade de `densidade` e um volume de `volume`, enquanto `massa(densidade)` retorna a massa tendo uma densidade de `densidade` e um volume de `1.0` metros cúbicos. Todas as quantidades são do tipo `double`.
   b. `repetir(10, "Estou bem")` exibe a string indicada 10 vezes, e `repetir("Mas você é meio tolo")` exibe a string indicada 5 vezes.
   c. `media(3,6)` retorna a média `int` de dois argumentos `int`, e `media(3.0, 6.0)` retorna a média `double` de dois valores `double`.
   d. `embaralhar("Fico feliz em te conhecer")` retorna o caractere `F` ou um ponteiro para a string `"Fico mela fe en t coerhnhe"`, dependendo se você atribui o valor de retorno a uma variável `char` ou a uma variável `char *`.

7. Escreva uma função template que retorna o maior de seus dois argumentos.

8. Dado o template da Questão de Revisão 7 e a estrutura `caixa` da Questão de Revisão 4, forneça uma especialização de template que recebe dois argumentos `caixa` e retorna aquela com o maior volume.

9. Que tipos são atribuídos a `v1`, `v2`, `v3`, `v4` e `v5` no seguinte código (supondo que o código seja parte de um programa completo)?

   ```cpp
   int g(int x);
   ...
   float m = 5.5f;
   float & rm = m;
   decltype(m) v1 = m;
   decltype(rm) v2 = m;
   decltype((m)) v3 = m;
   decltype(g(100)) v4;
   decltype(2.0 * m) v5;
   ```

## Exercícios de Programação (Programming Exercises)

1. Escreva uma função que normalmente recebe um argumento, o endereço de uma string, e imprime aquela string uma vez. No entanto, se um segundo argumento do tipo `int` for fornecido e for diferente de zero, a função deve imprimir a string um número de vezes igual ao número de vezes que aquela função foi chamada até aquele ponto. (Observe que o número de vezes que a string é impressa não é igual ao valor do segundo argumento; é igual ao número de vezes que a função foi chamada.) Use a função em um programa simples que demonstra como a função funciona.

2. A estrutura `BarraDeDoce` contém três membros. O primeiro membro guarda o nome da marca da barra de doce. O segundo membro guarda o peso (que pode ter uma parte fracionária) da barra, e o terceiro membro guarda o número de calorias (um valor inteiro) na barra. Escreva um programa que usa uma função que recebe como argumentos uma referência para `BarraDeDoce`, um ponteiro-para-char, um `double` e um `int` e usa os últimos três valores para definir os membros correspondentes da estrutura. Os últimos três argumentos devem ter valores padrão de `"Petisco Milenário"`, `2.85` e `350`. Além disso, o programa deve usar uma função que recebe uma referência para `BarraDeDoce` como argumento e exibe o conteúdo da estrutura. Use `const` onde apropriado.

3. Escreva uma função que recebe uma referência para um objeto `string` como seu parâmetro e que converte o conteúdo da `string` para maiúsculas. Use a função `toupper()` descrita na Tabela 6.4 do Capítulo 6. Escreva um programa que usa um loop que permite testar a função com diferentes entradas. Uma execução de exemplo pode parecer assim:

   ```
   Digite uma string (q para sair): vá embora
   VÁ EMBORA
   Próxima string (q para sair): meu Deus!
   MEU DEUS!
   Próxima string (q para sair): q
   Tchau.
   ```

4. O seguinte é um esboço de programa:

   ```cpp
   #include <iostream>
   using namespace std;
   #include <cstring> // para strlen(), strcpy()
   struct stringy {
       char * str; // aponta para uma string
       int ct;    // comprimento da string (não contando '\0')
   };
   // protótipos para definir(), mostrar() e mostrar() vão aqui
   int main()
   {
       stringy feijao;
       char teste[] = "A realidade não é o que costumava ser.";
       definir(feijao, teste); // primeiro argumento é uma referência,
                  // aloca espaço para guardar cópia de teste,
                  // define membro str de feijao para apontar para
                  // o novo bloco, copia teste para novo bloco,
                  // e define membro ct de feijao
       mostrar(feijao);    // imprime membro string uma vez
       mostrar(feijao, 2); // imprime membro string duas vezes
       teste[0] = 'D';
       teste[1] = 'u';
       mostrar(teste);     // imprime string de teste uma vez
       mostrar(teste, 3);  // imprime string de teste três vezes
       mostrar("Pronto!");
       return 0;
   }
   ```

   Complete este esboço fornecendo as funções descritas e os protótipos. Observe que deve haver duas funções `mostrar()`, cada uma usando argumentos padrão. Use argumentos `const` quando apropriado. Observe que `definir()` deve usar `new` para alocar espaço suficiente para guardar a string designada.

5. Escreva uma função template `max5()` que recebe como seu argumento um array de cinco itens do tipo `T` e retorna o maior item do array. (Como o tamanho é fixo, ele pode ser codificado diretamente no loop em vez de ser passado como argumento.) Teste-a em um programa que usa a função com um array de cinco valores `int` e um array de cinco valores `double`.

6. Escreva uma função template `maxn()` que recebe como seus argumentos um array de itens do tipo `T` e um inteiro representando o número de elementos no array, e que retorna o maior item do array. Teste-a em um programa que usa a função template com um array de seis valores `int` e um array de quatro valores `double`. O programa também deve incluir uma especialização que recebe um array de ponteiros-para-char como argumento e o número de ponteiros como segundo argumento, e que retorna o endereço da string mais longa. Se múltiplas strings empatarem na maior extensão, a função deve retornar o endereço da primeira empatada com a mais longa. Teste a especialização com um array de cinco ponteiros de string.

7. Modifique a Listagem 8.14 para que ela use duas funções template chamadas `SomarArray()` para retornar a soma do conteúdo do array em vez de exibir o conteúdo. O programa agora deve reportar o número total de coisas e a soma de todas as dívidas.

---

Navegação: [Anterior](capitulo-08-02-argumentos-padrao-sobrecarga.md) | [Índice](README.md) | [Próximo](capitulo-09-01-compilacao-separada.md)
