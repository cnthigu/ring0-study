# Capitulo 1 - Objetos, Ponteiros e Referencias (Objects, Pointers, and References)

> Traducao nao oficial do livro *C++ Memory Management* (Patrice Roy, 2025).
> Texto adaptado para portugues brasileiro com fins educacionais. Paginas originais: 1-32.
> Repositorio: [ring0-study](https://github.com/cnthigu/ring0-study)

> Navegacao: [Indice](README.md) | [Proximo](capitulo-02.md)

---

Antes de comecarmos a discutir gerenciamento de memoria em C++, vamos garantir que nos entendemos e concordamos com um vocabulario comum. Se voce e um programador C++ ha muito tempo, provavelmente tem suas proprias ideias sobre o que sao ponteiros (pointers), objetos (objects) e referencias (references). Essas ideias serao fruto de uma rica experiencia. Se voce vem a este livro de outra linguagem, talvez tambem tenha suas proprias ideias sobre o que esses termos significam em C++ e como eles se relacionam com memoria e gerenciamento de memoria.

Neste capitulo, vamos garantir que tenhamos um entendimento comum de algumas ideias basicas (mas profundas) para que possamos construir sobre esse entendimento compartilhado pelo restante de nossa aventura juntos. Especificamente, vamos explorar perguntas como as seguintes:

- Como a memoria e representada em C++? O que exatamente e essa coisa que chamamos de memoria, pelo menos no contexto da linguagem C++?
- O que sao objetos, ponteiros e referencias? O que queremos dizer com esses termos em C++? Quais sao as regras de *lifetime* (tempo de vida) dos objetos? Como eles se relacionam com a memoria?
- O que sao arrays em C++? Nessa linguagem, arrays sao uma construcao de baixo nivel, porem altamente eficiente, representada de uma forma que impacta diretamente o gerenciamento de memoria.

## Requisitos tecnicos

Este livro pressupoe que os leitores tenham algum conhecimento basico de C++ ou de linguagens sintaticamente similares, como C, Java, C# ou JavaScript. Por essa razao, nao explicaremos os fundamentos de declaracoes de variaveis, loops, instrucoes `if` ou funcoes.

No entanto, usaremos alguns aspectos da linguagem C++ neste capitulo que podem ser menos familiares a alguns leitores. Consulte o Anexo: Coisas que Voce Deve Saber antes de ler este livro.

Alguns dos exemplos usam C++20 ou C++23, portanto, certifique-se de que seu compilador suporta essa versao do padrao para aproveitar ao maximo os exemplos.

O codigo deste capitulo pode ser encontrado em: [https://github.com/PacktPublishing/C-Plus-Plus-Memory-Management/tree/main/chapter1](https://github.com/PacktPublishing/C-Plus-Plus-Memory-Management/tree/main/chapter1)

## Representacao da memoria em C++

Este e um livro sobre gerenciamento de memoria. Voce, leitor, esta tentando entender o que isso significa, e eu, como autor, estou tentando transmitir esse significado.

A forma como o padrao descreve a memoria pode ser vista em `[wg21.link/basic.memobj]`. Essencialmente, a memoria em C++ e expressa como uma ou mais sequencias de bytes contiguos. Isso abre a possibilidade de uma memoria expressa como um conjunto de blocos descontinuos de memoria contigua porque, historicamente, o C++ suportou memorias compostas por varios segmentos distintos. Todo byte em um programa C++ tem um endereco unico.

A memoria em um programa C++ e populada por varias entidades, como objetos, funcoes, referencias e assim por diante. Gerenciar a memoria de forma eficiente exige compreender o que essas entidades significam e como os programas podem fazer uso delas.

O significado da palavra *byte* e importante em C++. Conforme detalhado em `[wg21.link/intro.memory]`, bytes sao a unidade fundamental de armazenamento em C++. O numero de bits em um byte e definido pela implementacao em C++. O padrao afirma, no entanto, que um byte deve ser largo o suficiente para conter tanto a codificacao literal ordinaria de qualquer elemento do conjunto de caracteres literal basico quanto as unidades de codigo de oito bits da forma de codificacao UTF-8. Afirma tambem que um byte e composto por uma sequencia contigua de bits.

O que muitas vezes surpreende as pessoas e que, em C++, um byte nao e necessariamente um octeto: um byte consiste em pelo menos oito bits, mas pode ser composto por mais (algo util em alguns hardwares exoticos). Isso pode mudar no futuro, pois o comite de padronizacao pode um dia restringir essa definicao, mas esta e a situacao no momento da publicacao deste livro. A ideia central aqui e que um byte e a menor unidade de memoria enderecavel em um programa.

### Objetos, ponteiros e referencias

Tendemos a usar palavras como *object*, *pointer* e *reference* de forma informal, sem pensar muito no que significam. Em uma linguagem como C++, essas palavras possuem significados precisos que definem e delimitam o que podemos fazer na pratica.

Antes de colocarmos a mao na massa, vamos examinar o significado formal desses termos em C++.

#### Objetos

Se fossemos sondar programadores que trabalham com diferentes linguagens e perguntassemos como eles definiriam o termo *object* (objeto), provavelmente esperariamos respostas como "algo que agrupa variaveis e funcoes relacionadas" ou "uma instancia de uma classe," que correspondem as visoes tradicionais desse termo no campo da programacao orientada a objetos.

O C++ como linguagem tenta fornecer suporte homogeneo para tipos definidos pelo usuario, como `struct`s ou classes. Ele tambem oferece suporte para tipos fundamentais como `int` ou `float`. Portanto, provavelmente nao deve surpreender que, para o C++, a definicao de um objeto seja expressa em termos de suas propriedades, nao em termos do que a palavra significa, e que essa definicao inclua os tipos mais fundamentais. A definicao de objeto em C++ e descrita em `[wg21.link/intro.object]` e leva em conta os seguintes fatores:

- Como o objeto e criado explicitamente, como ao definir o objeto ou construi-lo por meio de uma das muitas variacoes do `operator new`. O objeto tambem pode ser criado implicitamente, como ao criar um objeto temporario como resultado de alguma expressao ou ao mudar o membro ativo de uma `union`.
- O fato de que um objeto esta em algum lugar (tem um endereco) e ocupa uma regiao de armazenamento de tamanho nao nulo, desde o inicio de sua construcao ate o fim de sua destruicao.
- Outras propriedades de um objeto, incluindo seu nome (se tiver um), seu tipo e sua *storage duration* (duracao de armazenamento): `automatic`, `static`, `thread_local`, e assim por diante.

O padrao C++ afirma explicitamente que funcoes nao sao objetos, mesmo que uma funcao tenha um endereco e ocupe armazenamento.

A partir disso, podemos inferir que ate mesmo um humilde `int` e um objeto, mas uma funcao nao e. Voce pode ver ja, caro leitor, que o livro que esta lendo tocara em topicos fundamentais, uma vez que *lifetime* e o armazenamento ocupado por objetos fazem parte das propriedades fundamentais dessas entidades que usamos em nossos programas todo dia. Coisas como *lifetime* e armazenamento sao claramente parte do que o gerenciamento de memoria significa. Voce pode se convencer desse fato com este programa simples:

```cpp
#include <type_traits>
int main() {
   static_assert(std::is_object_v<int>);
   static_assert(!std::is_object_v<decltype(main)>);
}
```

O que e um objeto? E algo que tem um *lifetime* e ocupa armazenamento. Controlar essas caracteristicas e parte das razoes pelas quais este livro existe.

#### Ponteiros

Ha inumeras (cerca de 2.000) mencoes a palavra "pointer" (ponteiro) no texto do padrao C++, mas se voce abrir uma copia eletronica desse documento e pesquisar por ela, descobrira que uma definicao formal e surpreendentemente dificil de encontrar. Isso pode ser surpreendente dado o fato de que as pessoas tendem a associar essa ideia ao C e (por extensao) ao C++.

Vamos tentar oferecer uma definicao util, embora informal, entao: um *pointer* e um endereco tipado. Ele associa um tipo ao que e encontrado em algum local na memoria. Por essa razao, em um codigo como o seguinte, le-se que `n` e um objeto `int` e que `p` aponta para um objeto `int` que coincidentemente e o endereco do objeto `n`:

```cpp
int n = 3; // n is an int object
char c;
// int *p = &c; // no, illegal
int *p = &n;
```

E importante entender aqui que `p` de fato aponta para um `int`, a menos que `p` esteja sem inicializacao, aponte para `nullptr`, ou programadores tenham manipulado o sistema de tipos e feito `p` apontar para outra coisa deliberadamente. Claro, o ponteiro `p` e um objeto, pois respeita todas as regras a esse respeito.

Grande parte da confusao (sintatica) sobre ponteiros provavelmente vem do significado contextual dos simbolos `*` e `&`. O segredo e lembrar que eles tem papeis diferentes quando aparecem na introducao de um nome e quando sao usados em um objeto existente:

```cpp
int m = 4, n = 3;
int *p; // p declares (and defines) a pointer to an int
        // (currently uninitialized), introducing a name
p = 0; // p is a null pointer (it does not necessarily
       // point to address zero; 0 as used here is
       // just a convention)
p = nullptr; // likewise, but clearer. Prefer nullptr to
             // literal 0 whenever possible to describe
             // a null pointer
p = &m; // p points to m (p contains the address of m)
assert(*p == 4); // p already exists; with *p we are
                 // accessing what p points to
p = &n; // p now points to n (p contains the address of n)
int *q = &n; // q declares (and defines) a pointer to an
             // int and &n represents the address of n, the
             // address of an int: q is a pointer to an int
assert(*q == 3); // n holds 3 at this stage, and q points
                 // to n, so what q points to has value 3
assert(*p == 3); // the same holds for p
assert(p == q); // p and q point to the same int object
*q = 4; // q already exists, so *q means "whatever q
        // points to"
assert(n == 4); // indeed, n now holds value 4 since we
                // modified it indirectly through q
auto qq = &q;   // qq is the address of q, and its type is
                // "pointer to a pointer to an int", thus
                // int **... But we will rarely – if ever –
                // need this
int &r = n; // declaration of r as a reference to integer n
            // (see below). Note that & is used in a
            // declaration in this case
```

Como voce pode ver, ao introduzir um objeto, `*` significa "ponteiro para." Em um objeto existente, significa "o que aquele ponteiro aponta" (o *pointee*). Da mesma forma, ao introduzir um nome, `&` significa "referencia para" (algo que discutiremos em breve). Em um objeto existente, significa "endereco de" e produz um ponteiro.

Ponteiros nos permitem fazer aritmetica, mas isso e (legitimamente) visto como uma operacao perigosa, pois pode nos levar a locais arbitrarios em um programa e, portanto, causar danos serios. A aritmetica em um ponteiro depende de seu tipo:

```cpp
int *f();
char *g();
int danger() {
   auto p = f(); // p points to whatever f() returned
   int *q = p + 3; // q points to where p points to plus
                   // three times the size of an int. No
                   // clue where this is, but it's a bad,
                   // bad idea...
   auto pc = g(); // pc points to whatever g() returned
   char * qc = pc + 3; // qc points to where pc points
                       // to plus three times the size
                       // of a char. Please don't make
                       // your pointers go to places you
                       // don't know about like this
}
```

Obviamente, acessar o conteudo de enderecos arbitrarios e pedir para ter problemas. Isso porque significaria invocar *undefined behavior* (comportamento indefinido, descrito no Capitulo 2), e se voce fizer isso, estara por conta propria. Por favor, nao faca essas coisas em codigo real, pois voce poderia prejudicar programas — ou pior, pessoas. O C++ e poderoso e flexivel, mas se voce programa em C++, espera-se que se comporte com responsabilidade e profissionalismo.

O C++ possui quatro tipos especiais para manipulacao de ponteiros:

- `void*` significa "endereco sem semantica especifica (relacionada ao tipo)." Um `void*` e um endereco sem tipo associado. Todos os ponteiros (se desconsiderarmos os qualificadores `const` e `volatile`) sao implicitamente conversiveis para `void*`; uma maneira informal de ler isso e "todos os ponteiros, independentemente do tipo, realmente sao enderecos." O inverso nao se aplica. Por exemplo, nao e verdade que todos os enderecos sejam implicitamente conversiveis para ponteiros `int`.
- `char*` significa "ponteiro para um byte." Devido as raizes da linguagem C no C++, um `char*` pode ser *alias* (alias) para qualquer endereco na memoria (o tipo `char`, independentemente de seu nome, que evoca "caractere," realmente significa "byte" em C e, por extensao, em C++). Ha um esforco em andamento no C++ para dar ao `char` o significado de "caractere", mas no momento da escrita deste livro, um `char*` pode ser alias de praticamente qualquer coisa em um programa. Isso prejudica algumas oportunidades de otimizacao do compilador (e dificil restringir ou raciocinar sobre algo que pode levar a literalmente qualquer coisa na memoria).
- `std::byte*` e o novo "ponteiro para um byte," pelo menos desde o C++17. A intencao (a longo prazo) de `byte*` e substituir `char*` nas funcoes que fazem manipulacao ou enderecamento byte a byte, mas como ha tanto codigo que usa `char*` para esse fim, isso levara tempo.

Para um exemplo de conversao de e para `void*`, considere o seguinte:

```cpp
int n = 3;
int *p = &n; // fine so far
void *pv = p; // Ok, a pointer is an address
// p = pv; // no, a void* does not necessarily point to
           // an int (Ok in C, not in C++)
p = static_cast<int *>(pv); // fine, you asked for it, but
                            // if you're wrong you're on
                            // your own
```

O exemplo a seguir, um pouco mais elaborado, usa `const char*` (mas poderia usar `const byte*` em vez disso). Ele mostra que e possivel comparar a representacao byte a byte de dois objetos, pelo menos em algumas circunstancias, para verificar se sao equivalentes:

```cpp
#include <iostream>
#include <type_traits>
using namespace std;
bool same_bytes(const char *p0, const char *p1,
                std::size_t n) {
    for(std::size_t i = 0; i != n; ++i)
        if(*(p0 + i) != *(p1 + i))
            return false;
    return true;
}
template <class T, class U>
   bool same_bytes(const T &a, const U &b) {
      using namespace std;
       static_assert(sizeof a == sizeof b);
       static_assert(has_unique_object_representations_v<
          T
       >);
       static_assert(has_unique_object_representations_v<
          U
       >);
       return same_bytes(reinterpret_cast<const char*>(&a),
                         reinterpret_cast<const char*>(&b),
                         sizeof a);
   }
struct X {
   int x {2}, y{3};
};
struct Y {
   int x {2}, y{3};
};
#include <cassert>
int main() {
    constexpr X x;
    constexpr Y y;
    assert(same_bytes(x, y));
}
```

A *trait* `has_unique_object_representations` e `true` para tipos unicamente definidos por seus valores, ou seja, tipos isentos de *padding bits* (bits de preenchimento). Isso e as vezes importante pois o C++ nao especifica o que acontece com os *padding bits* em um objeto, e realizar uma comparacao bit a bit de dois objetos pode produzir resultados surpreendentes. Observe que objetos de tipos de ponto flutuante nao sao considerados unicamente definidos por seus valores, pois ha muitos valores distintos que se qualificam como NaN, ou "not a number" (nao e um numero).

#### Referencias

A linguagem C++ suporta duas familias relacionadas de indiretos: ponteiros e referencias. Como seus primos, os ponteiros, as referencias sao frequentemente mencionadas pelo padrao C++ (mais de 1.800 vezes), mas e dificil encontrar uma definicao formal para elas.

Vamos tentar, mais uma vez, fornecer uma definicao informal, porem operacional: uma *reference* pode ser vista como um *alias* (apelido) para uma entidade existente. Deliberadamente nao usamos *object* aqui, pois podemos referenciar uma funcao e ja sabemos que uma funcao nao e um objeto.

Ponteiros sao objetos. Como tal, eles ocupam armazenamento. Referencias, por outro lado, nao sao objetos e nao usam armazenamento proprio, mesmo que uma implementacao possa simular sua existencia com ponteiros. Compare `std::is_object_v<int*>` com `std::is_object_v<int&>`: o primeiro e `true`, e o segundo e `false`.

O operador `sizeof`, aplicado a uma referencia, retornara o tamanho do que ela referencia. Consequentemente, obter o endereco de uma referencia retorna o endereco do que ela referencia.

Em C++, uma referencia esta sempre vinculada a um objeto e permanece vinculada a esse objeto ate o fim do *lifetime* da referencia. Um ponteiro, por outro lado, pode apontar para varios objetos distintos durante seu *lifetime*, como vimos antes:

```cpp
// int &nope; // would not compile (what would nope
              // refer to?)
int n = 3;
int &r = n; // r refers to n
++r; // n becomes 4
assert(&r == &n); // taking the address of r means taking
                  // the address of n
```

Outra diferenca entre ponteiros e referencias e que, ao contrario da situacao que prevalece com ponteiros, nao existe aritmetica de referencias. Isso torna as referencias um pouco mais seguras que os ponteiros. Ha espaco para ambos os tipos de indireto em um programa (e usaremos ambos neste livro!), mas para a programacao do dia a dia, uma boa regra geral e usar referencias sempre que possivel e usar ponteiros quando necessario.

Agora que examinamos a representacao da memoria e demos uma olhada em como o C++ define algumas ideias fundamentais, como byte, objeto, ponteiro ou referencia, podemos nos aprofundar um pouco mais em algumas propriedades definidoras importantes dos objetos.

## Compreendendo as propriedades fundamentais dos objetos

Vimos anteriormente que, em C++, um objeto tem um tipo e um endereco. Ele tambem ocupa uma regiao de armazenamento desde o inicio de sua construcao ate o fim de sua destruicao. Vamos agora examinar essas propriedades fundamentais em mais detalhes para entender como elas afetam as formas pelas quais escrevemos programas.

### Lifetime de objetos

Uma das forcas do C++, mas tambem uma razao para sua relativa complexidade, surge do controle que se tem sobre o *lifetime* dos objetos. Em C++, de modo geral, objetos automaticos (*automatic objects*) sao destruidos no final de seu escopo em uma ordem bem definida. Objetos estaticos (globais) sao destruidos no termino do programa em uma ordem razoavelmente bem definida (em um dado arquivo, a ordem de destruicao e clara, mas e mais complicado para objetos estaticos em arquivos diferentes). Objetos alocados dinamicamente sao destruidos "quando seu programa mandar" (ha muitas nuances nisso).

Vamos examinar alguns aspectos do *lifetime* de objetos com o seguinte programa (muito) simples:

```cpp
#include <string>
#include <iostream>
#include <format>
struct X {
   std::string s;
   X(std::string_view s) : s{ s } {
      std::cout << std::format("X::X({})\n", s);
   }
   ~X(){
      std::cout << std::format("~X::X() for {}\n", s);
   }
};
X glob { "glob" };
void g() {
   X xg{ "g()" };
}
int main() {
   X *p0 = new X{ "p0" };
   [[maybe_unused]] X *p1 = new X{ "p1" }; // will leak
   X xmain{ "main()" };
   g();
   delete p0;
   // oops, forgot delete p1
}
```

Quando executado, esse programa imprimira o seguinte:

```
X::X(glob)
X::X(p0)
X::X(p1)
X::X(main())
X::X(g())
~X::X() for g()
~X::X() for p0
~X::X() for main()
~X::X() for glob
```

O fato de o numero de construtores e destrutores nao coincidir e um sinal de que fizemos algo errado. Mais especificamente, neste exemplo, criamos manualmente um objeto (apontado por `p1`) com `operator new`, mas nunca o destruimos manualmente depois.

Uma fonte comum de confusao para programadores nao familiarizados com C++ e a distincao entre o ponteiro (*pointer*) e o *pointee* (o objeto para o qual o ponteiro aponta). Neste programa, `p0` e `p1` sao ambos destruidos quando se atinge o final de seu escopo (pela chave de fechamento da funcao `main()`), assim como `xmain`. No entanto, como `p0` e `p1` apontam para objetos alocados dinamicamente, os *pointees* precisam ser destruidos explicitamente — algo que fizemos para `p0` mas (deliberadamente, para fins de exemplo) negligenciamos para `p1`.

O que acontece entao com o *pointee* de `p1`? Bom, ele foi construido manualmente e nao foi destruido manualmente. Como tal, ele flutua na memoria onde ninguem mais pode acessa-lo. Isso e o que as pessoas frequentemente chamam de *memory leak* (vazamento de memoria): um pedaco de memoria que seu programa alocou, mas nunca desalocou.

Pior do que vazar o armazenamento para o objeto `X` apontado por `p1`, no entanto, e o fato de que o destrutor do *pointee* nunca sera chamado, o que pode causar todos os tipos de *resource leaks* (vazamentos de recursos): arquivos nao fechados, conexoes de banco de dados nao encerradas, *handles* de sistema nao liberados, e assim por diante. No Capitulo 4, Usando Destrutores, examinaremos como e possivel evitar tais situacoes e escrever codigo limpo e simples ao mesmo tempo.

### Tamanho, alinhamento e padding de objetos

Como cada objeto ocupa armazenamento, o espaco associado a um objeto e uma propriedade importante (embora de baixo nivel) dos tipos C++. Por exemplo, observe o seguinte codigo:

```cpp
class B; // forward declaration: there will be a class B
         // at some point in the future
void f(B*); // fine, we know what B is, even if we don't
            // know the details yet, and all object
            // addresses are of the same size
// class D : B {}; // oops! To know what a D is, we have
                   // to know how big a B is and what a
                   // B object contains since a D is a B
```

Nesse exemplo, tentar definir a classe `D` nao compilaria. Isso porque, para criar um objeto `D`, o compilador precisa reservar espaco suficiente para um objeto `D`, mas um objeto `D` tambem e um objeto `B`, e assim nao podemos saber o tamanho de um objeto `D` sem saber o tamanho de um objeto `B`.

O tamanho de um objeto ou, equivalentemente, de um tipo pode ser obtido por meio do operador `sizeof`. Esse operador retorna um valor inteiro sem sinal de tempo de compilacao (compile-time), nao nulo, correspondente ao numero de bytes necessarios para armazenar um objeto:

```cpp
char c;
// a char occupies precisely one byte of storage, per
// standard wording
static_assert(sizeof c == 1); // for objects parentheses
                              // are not required
static_assert(sizeof(c) == 1); // ... but you can use them
static_assert(sizeof(char) == 1); // for types, parentheses
                                  // are required
struct Tiny {};
// all C++ types occupy non-zero bytes of storage by
// definition, even if they are "empty" like type Tiny
static_assert(sizeof(Tiny) > 0);
```

No exemplo anterior, a classe `Tiny` e vazia porque nao tem nenhum membro de dados. Uma classe pode ter funcoes membro e ainda assim ser vazia. Classes vazias que expoe funcoes membro sao muito comuns em C++.

Um objeto C++ sempre ocupa pelo menos um byte de armazenamento, mesmo no caso de classes vazias como `Tiny`. Isso porque, se o tamanho de um objeto fosse zero, esse objeto poderia estar no mesmo local de memoria que seu vizinho imediato, o que seria dificil de raciocinar.

O C++ difere de muitas outras linguagens pelo fato de nao padronizar o tamanho de todos os tipos fundamentais. Por exemplo, `sizeof(int)` pode retornar valores diferentes dependendo do compilador e da plataforma. Ainda assim, ha regras sobre o tamanho dos objetos:

- O tamanho reportado pelo operador `sizeof` para objetos do tipo `signed char`, `unsigned char` e `char` e 1, e o mesmo vale para `sizeof(std::byte)`, ja que cada um desses tipos pode ser usado para representar um unico byte.
- As expressoes `sizeof(short)>=sizeof(char)` e `sizeof(int)>=sizeof(short)` serao validas em todas as plataformas, o que significa que pode haver casos em que `sizeof(char)` e `sizeof(int)` sejam ambos 1. Em termos de largura (ou seja, bits usados na representacao do valor) dos tipos fundamentais, o padrao C++ se limita a declarar a largura minima para cada tipo. A lista pode ser encontrada em `[wg21.link/tab:basic.fundamental.width]`.
- Como ja dissemos, a expressao `sizeof(T)>0` vale para qualquer tipo `T`. Em C++, nao existem objetos de tamanho zero, nem mesmo objetos de classes vazias.
- O tamanho ocupado por um objeto de qualquer `struct` ou classe nao pode ser menor que a soma do tamanho de seus membros de dados (mas ha ressalvas).

Essa ultima regra merece uma explicacao. Considere a seguinte situacao:

```cpp
class X {};
class Y {
   X x;
};
int main() {
   static_assert(sizeof(X) > 0);
   static_assert(sizeof(Y) == sizeof(X)); // <-- here
}
```

A linha marcada com `<-- here` pode ser intrigante. Por que `sizeof(Y)` seria igual a `sizeof(X)` se cada objeto `Y` contem um objeto `X`? Lembre-se de que `sizeof(X)` e maior que 0, mesmo que `X` seja uma classe vazia, porque todo objeto C++ deve ocupar pelo menos um byte de armazenamento. No entanto, no caso de `Y`, que nao e uma classe vazia, cada objeto `Y` ja ocupa armazenamento devido ao seu membro de dados `x`. Nao ha razao para adicionar artificialmente espaco de armazenamento a objetos desse tipo.

Agora, considere o seguinte:

```cpp
class X {
   char c;
};
class Y {
   X x;
};
int main() {
   static_assert(sizeof(X) == sizeof(char)); // <-- here
   static_assert(sizeof(Y) == sizeof(X)); // <-- here too
}
```

O mesmo raciocinio se aplica novamente: um objeto do tipo `X` ocupa a mesma quantidade de espaco de armazenamento que seu unico membro de dados (do tipo `char`), e um objeto do tipo `Y` ocupa a mesma quantidade de espaco de armazenamento que seu unico membro de dados (do tipo `X`).

Continuando essa exploracao, considere o seguinte:

```cpp
class X { };
class Y {
   X x;
   char c;
};
int main() {
   static_assert(sizeof(Y) >= sizeof(char) + sizeof(X));
}
```

Esta e a regra que mencionamos anteriormente, porem expressa formalmente para um tipo especifico. Nessa situacao, supondo que `sizeof(X)` seja igual a 1 — o que e altamente provavel — poder-se-ia esperar razoavelmente que `sizeof(Y)` fosse igual a soma de `sizeof(char)` e `sizeof(X)`.

Por fim, considere o seguinte:

```cpp
class X { };
class Y : X { // <--  private inheritance
   char c;
};
int main() {
   static_assert(sizeof(Y) == sizeof(char)); // <-- here
}
```

Passamos de ter um objeto da classe `X` como membro de dados de `Y` para `X` ser uma classe base de `Y`. Isso tem uma consequencia interessante: como a classe base `X` e vazia e como sabemos por definicao que objetos da classe derivada `Y` ocuparao pelo menos um byte de armazenamento, a classe base pode ser "achatada" na classe derivada para objetos `Y`. Esta e uma otimizacao util chamada *empty base optimization* (otimizacao de base vazia). Voce pode razoavelmente esperar que compiladores realizem essa otimizacao na pratica, pelo menos no caso de relacionamentos de heranca simples.

Observe que, como a presenca de um `X` em um `Y` e um detalhe de implementacao, nao algo que participa da interface da classe `Y`, usamos heranca privada neste exemplo. A *empty base optimization* se aplicaria tambem com heranca publica ou protegida, mas neste caso, a heranca privada preserva o fato de que a parte `X` de um `Y` e algo que apenas o `Y` conhece.

Desde o C++20, se voce achar que a composicao seria mais adequada do que a heranca para descrever a relacao entre duas classes como `X` e `Y`, pode marcar um membro de dados com `[[no_unique_address]]` para informar ao compilador que esse membro, se for um objeto de uma classe vazia, nao precisa ocupar armazenamento dentro do objeto que o envolve. Os compiladores nao sao obrigados a cumprir isso, pois os atributos podem ser ignorados, portanto, certifique-se de verificar que seu compilador de escolha implementa isso antes de escrever codigo que depende desse comportamento:

```cpp
class X { };
class Y {
   char c;
   [[no_unique_address]] X x;
};
int main() {
   static_assert(sizeof(X) > 0);
   static_assert(sizeof(Y) == sizeof(char)); // <-- here
}
```

Todos os exemplos ate agora foram muito simples, usando classes com zero, um ou dois membros de dados muito pequenos. O codigo raramente e tao simples. Considere o seguinte programa:

```cpp
class X {
   char c; // sizeof(char) == 1 by definition
   short s;
   int n;
};
int main() {
   static_assert(sizeof(short) == 2); // we suppose this...
   static_assert(sizeof(int) == 4);   // ... and this
   static_assert(
      sizeof(X) >= sizeof(char)+sizeof(short)+sizeof(int)
   );
}
```

Supondo que as duas primeiras `static_assert`s sejam validas — o que e provavel, mas nao garantido — sabemos que `sizeof(X)` sera pelo menos 7 (a soma dos tamanhos de seus membros de dados). Na pratica, no entanto, voce provavelmente vera que `sizeof(X)` e igual a 8. Agora, isso pode parecer surpreendente a primeira vista, mas e uma consequencia logica de algo chamado *alignment* (alinhamento).

O *alignment* de um objeto (ou de seu tipo) nos diz onde esse objeto pode ser colocado na memoria. O tipo `char` tem um alinhamento de 1 e, como tal, pode-se colocar um objeto `char` literalmente em qualquer lugar (contanto que se possa acessar essa memoria). Para um alinhamento de 2 (que e provavel para o tipo `short`), os objetos so podem ser colocados em enderecos que sao multiplos de 2. De forma mais geral, se um tipo tem um alinhamento de `n`, entao os objetos desse tipo devem ser colocados em um endereco que e multiplo de `n`. Observe que o alinhamento deve ser uma potencia de 2 estritamente positiva; nao respeitar essa regra implica *undefined behavior*. Claro, seu compilador nao o colocara nessa posicao, mas voce pode se meter em tal apuros se nao tiver cuidado, dado alguns dos truques que usaremos neste livro. Com grande controle vem grande responsabilidade.

A linguagem C++ oferece dois operadores relacionados ao alinhamento:

- O operador `alignof`, que retorna o alinhamento natural de um tipo `T` ou de um objeto desse tipo.
- O operador `alignas`, que permite aos programadores impor o alinhamento de um objeto. Isso e muitas vezes util ao manipular memoria (como faremos) ou ao interfacear com hardware exotico (o termo "exotico" aqui pode ser tomado em um sentido muito amplo). Claro, `alignas` so pode razoavelmente aumentar o alinhamento natural de um tipo, nao reduzi-lo.

Para algum tipo fundamental `T`, pode-se esperar que a afirmacao de que `sizeof(T)` e igual a `alignof(T)` seja valida, mas essa afirmacao nao se generaliza para tipos compostos. Por exemplo, considere o seguinte:

```cpp
class X {
   char c;
   short s;
   int n;
};
int main() {
   static_assert(sizeof(short) == alignof(short));
   static_assert(sizeof(int) == alignof(int));
   static_assert(sizeof(X) == 8); // highly probable
   static_assert(alignof(X) == alignof(int)); // likewise
}
```

De modo geral, para um tipo composto, o alinhamento correspondera ao pior alinhamento de seus membros de dados. Aqui, "pior" significa "maior." Para a classe `X`, o membro de dados com pior alinhamento e `n` do tipo `int` e, como tal, objetos `X` serao alinhados em limites de `alignof(int)` bytes.

Voce pode se perguntar agora por que podemos esperar que a afirmacao de que `sizeof(X)` seja igual a 8 seja valida, se `sizeof(short)==2` e `sizeof(int)==4`. Vamos olhar para o layout provavel dos objetos do tipo `X`:

> **[Figura 1.1 - Layout compacto de um objeto do tipo X na memoria]**
>
> O layout mostra: 1 byte para `c` (char), 1 byte de padding (?), 2 bytes para `s` (short), 4 bytes para `n` (int). Total: 8 bytes.

Cada caixa nessa figura e um byte na memoria. Como podemos ver, ha um `?` entre `c` e o primeiro byte de `s`. Isso vem do alinhamento. Se `alignof(short)==2` e `alignof(int)==4`, entao o unico layout correto para um objeto `X` coloca seu membro `n` em um limite de 4. Isso significa que havera um byte de *padding* (um byte que nao participa da representacao de valor de `X`) entre `c` e `s` para alinhar `s` em um limite de dois bytes e alinhar `n` em um limite de quatro bytes.

O que pode parecer mais surpreendente e que a ordem em que os membros de dados estao dispostos em uma classe impacta o tamanho dos objetos dessa classe. Por exemplo, considere o seguinte:

```cpp
class X {
   short s;
   int n;
   char c;
};
int main() {
   static_assert(sizeof(short) == alignof(short));
   static_assert(sizeof(int) == alignof(int));
   static_assert(alignof(X) == alignof(int));
   static_assert(sizeof(X) == 12); // highly probable
}
```

> **[Figura 1.2 - Layout menos compacto para um objeto do tipo X na memoria]**
>
> O layout mostra: 2 bytes para `s` (short), 2 bytes de padding (??), 4 bytes para `n` (int), 1 byte para `c` (char), 3 bytes de padding (???) ao final. Total: 12 bytes.

Os dois `?` entre `s` e `n` provavelmente ja estao claros agora, mas os tres `?` no final podem parecer surpreendentes. Afinal, por que adicionar *padding* no final de um objeto?

A resposta e por causa dos arrays. Como discutiremos em breve, os elementos de um array sao contiguos na memoria e, por essa razao, e importante que cada elemento de um array esteja corretamente alinhado. Nesse caso, os bytes de *padding* ao final de um objeto da classe `X` garantem que, se um elemento em um array de objetos `X` estiver corretamente alinhado, o proximo elemento tambem estara.

Agora que voce sabe sobre alinhamento, considere que simplesmente mudar a ordem dos elementos de uma versao da classe `X` para outra resultou em um aumento de 50% no consumo de memoria para cada objeto desse tipo. Isso prejudica o consumo de espaco de memoria do seu programa e sua velocidade ao mesmo tempo. Compiladores C++ nao podem reordenar os membros de dados por voce, pois seu codigo ve os enderecos dos objetos. Alterar a posicao relativa dos membros de dados poderia quebrar o codigo dos usuarios, portanto, cabe aos programadores tomar cuidado com os layouts escolhidos. Observe que manter os objetos pequenos nao e o unico fator que pode influenciar a escolha do layout em um objeto, especialmente em codigo multithread (onde as vezes manter dois objetos a uma certa distancia um do outro pode levar a um melhor uso de cache), portanto, deve-se lembrar que o layout e importante, mas nao algo a ser tratado ingenuamente.

### Copia e movimento

Neste ponto, precisamos dizer algumas palavras sobre *copy* (copia) e *movement* (movimento), duas consideracoes fundamentais em uma linguagem como C++, onde existem objetos reais.

A linguagem C++ considera seis funcoes membro como especiais. Essas funcoes serao geradas automaticamente para os seus tipos, a menos que voce tome medidas para evita-lo. Sao as seguintes:

- O **construtor default** (*default constructor*): E provavelmente o menos especial de todos os seis, pois so e gerado implicitamente se voce nao escrever nenhum construtor proprio.
- O **destrutor** (*destructor*): E chamado no final do *lifetime* de um objeto.
- O **construtor de copia** (*copy constructor*): E chamado ao construir um objeto com um unico objeto do mesmo tipo como argumento.
- A **atribuicao de copia** (*copy assignment*): E chamada ao substituir o conteudo de um objeto existente por uma copia do conteudo de outro objeto.
- O **construtor de movimento** (*move constructor*): E chamado ao construir um objeto a partir de uma referencia a um objeto do qual se pode mover. Exemplos de objetos dos quais se pode mover incluem objetos que nao poderiam mais ser referenciados posteriormente, como o resultado (anonimo) da avaliacao de uma expressao ou o retorno de uma funcao. O programa tambem pode explicitamente tornar um objeto movivel com `std::move()`.
- A **atribuicao de movimento** (*move assignment*): Comporta-se como a atribuicao de copia, mas e aplicada quando o argumento passado ao operador de atribuicao e algo do qual se pode mover.

Quando um tipo nao gerencia explicitamente nenhum recurso por conta propria, geralmente pode-se nao escrever nenhuma dessas funcoes especiais, pois as geradas pelo compilador serao exatamente o que se quer. Por exemplo, considere o seguinte:

```cpp
struct Point2D {
   float x{}, y{};
};
```

Aqui, o tipo `Point2D` representa uma coordenada 2D que nao tem invariantes (todos os valores sao validos para seus membros de dados `x` e `y`). Como usamos inicializadores padrao para `x` e `y` que definem esses membros como 0, um objeto `Point2D` default representara a coordenada (0,0) e as seis funcoes membro especiais se comportarao como esperado. O construtor de copia chamara os construtores de copia dos membros de dados, a atribuicao de copia chamara seus operadores de atribuicao de copia, o destrutor sera trivial e as operacoes de movimento se comportarao como operacoes de copia, uma vez que os membros de dados sao de tipos fundamentais.

Caso decidamos adicionar um construtor parametrico para permitir explicitamente que o codigo do usuario inicialize os membros de dados `x` e `y` com outros valores alem de nossos defaults escolhidos, podemos faze-lo. No entanto, isso nos custara o construtor default implicito:

```cpp
struct Point2D {
   float x{}, y{};
   Point2D(float x, float y) : x{ x }, y{ y } {
   }
};
void oops() {
   Point2D pt; // does not compile, pt has no default ctor
}
```

Podemos, claro, corrigir isso. Uma maneira e escrever os detalhes de um construtor default explicitamente:

```cpp
struct Point2D {
   float x, y; // no need for default initializations
   Point2D(float x, float y) : x{ x }, y{ y } {
   }
   Point2D() : x{ }, y{ } { // <-- here
   }
};
void oops() {
   Point2D pt; // Ok
}
```

Outra abordagem e delegar o trabalho do construtor default para o construtor parametrico:

```cpp
struct Point2D {
   float x, y; // no need for default initializations
   Point2D(float x, float y) : x{ x }, y{ y } {
   }
   Point2D() : Point2D{ 0, 0 } { // <-- here
   }
};
void oops() {
   Point2D pt; // Ok
}
```

Outra abordagem ainda melhor e informar ao compilador que queremos manter o comportamento default, mesmo que tenhamos feito algo (escrever outro construtor) que de outra forma o impediria:

```cpp
struct Point2D {
   float x{}, y{};
   Point2D(float x, float y) : x{ x }, y{ y } {
   }
   Point2D() = default; // <-- here
};
void oops() {
   Point2D pt; // Ok
}
```

Esta ultima opcao geralmente levara ao melhor codigo gerado, pois os compiladores sao realmente bons em obter resultados maximos de esforco minimo quando entendem a intencao do programador. Nesse caso, `= default` torna a intencao muito explicita: por favor, faca o que voce teria feito normalmente se meu codigo nao tivesse interferido.

> **Nota sobre esses construtores**
>
> Adicionamos construtores parametricos a `Point2D` para fins de exemplo, mas nao e necessario neste caso, pois `Point2D` e um *aggregate* (tipo agregado). Esses tipos tem suporte especial de inicializacao, mas isso esta fora do ponto de nossa ilustracao. Tipos agregados sao tipos que cumprem varias restricoes (sem construtores declarados pelo usuario ou herdados, sem membros de dados nao estaticos privados, sem classes base virtuais, etc.) e que geralmente nao tem invariantes a manter, mas que podem ser inicializados de forma muito eficiente pelo compilador.

Quando uma classe gerencia recursos explicitamente, as funcoes especiais geradas por default raramente fazem o que queremos. Como poderia o compilador conhecer nossas intencoes em tal situacao? Suponha que criemos uma classe simples tipo-string propria, comecando com o seguinte trecho (incompleto):

```cpp
#include <cstring> // std::strlen()
#include <algorithm> // std::copy()
class naive_string { // too simple to be useful
   char *p {}; // pointer to the elements (nullptr)
   std::size_t nelems {}; // number of elements (zero)
public:
   std::size_t size() const {
      return nelems;
   }
   bool empty() const {
      return size() == 0;
   }
   naive_string() = default; // empty string
   naive_string(const char *s)
      : nelems{ std::strlen(s) } {
      p = new char[size() + 1]; // leaving room for a
                                // (convenient) trailing 0
      std::copy(s, s + size(), p);
      p[size()] = '\0';
   }
   // index-wise access to characters, const and non-const
   // versions: the const version is useful for const
   // naive_string objects, whereas the non-const version
   // lets user code modify elements
   // precondition: n < size()
   char operator[](std::size_t n) const { return p[n]; }
   char& operator[](std::size_t n) { return p[n]; }
   // ... additional code (below) goes here
};
```

Simples como e, essa classe claramente faz alocacao explicita de recursos ao alocar um bloco de `size()+1` bytes para manter uma copia da sequencia de caracteres iniciada em `p`. Por essa razao, as funcoes membro especiais fornecidas pelo compilador nao farao a coisa certa para nossa classe. Por exemplo, o construtor de copia gerado por default copiara o ponteiro `p`, mas isso significa que teremos dois ponteiros (o `p` original e o `p` na copia) compartilhando um *pointee* comum, o que provavelmente nao e o que queremos. O destrutor gerado por default destruira o ponteiro, mas tambem queremos desalocar o *pointee* e evitar um *memory leak*, e assim por diante.

#### Destruicao

Como nossa `naive_string` faz gerenciamento de recursos com o array alocado dinamicamente apontado por `p`, o destrutor dessa classe sera simples, pois seu papel se limitara a desalocar o bloco de memoria apontado por `p`:

```cpp
// ...
   ~naive_string() {
      delete [] p;
   }
// ...
```

Observe que nao e necessario verificar se `p` nao e nulo (`delete nullptr;` nao faz nada em C++ e e inerentemente inofensivo). Observe tambem que estamos usando `delete[]`, nao `delete`, pois alocamos o bloco de memoria com `new[]`, nao `new`. As nuances entre essas operacoes serao explicadas no Capitulo 7.

#### Operacoes de copia

O construtor de copia e a funcao chamada ao construir um objeto da classe `naive_string` com um argumento que e outro objeto dessa classe. Por exemplo, considere o seguinte:

```cpp
// ...
void f(naive_string); // pass-by-value
void copy_construction_examples() {
   naive_string s0{ "What a fine day" };
   naive_string s1 = s0; // constructs s1 so this is
                         // copy construction
   naive_string s2(s0); // ...this too
   naive_string s3{ s0 }; // ...and so is this
   f(s0); // likewise because of pass-by-value
   s1 = s0; // this is not a copy construction as s1
            // already exists: this is a copy assignment
}
```

Para nossa classe `naive_string`, um construtor de copia correto poderia ser escrito da seguinte forma:

```cpp
// ...
   naive_string(const naive_string &other)
      : p{ new char[other.size() + 1] },
        nelems{ other.size() } {
      std::copy(other.p, other.p + other.size(), p);
      p[size()] = '\0';
   }
// ...
```

As atribuicoes de copia poderiam ser escritas de varias maneiras, mas muitas delas sao complicadas ou simplesmente perigosas. Por exemplo, considere o seguinte exemplo — mas nao escreva seu operador de atribuicao assim!:

```cpp
// ...
   // bad copy assignment operator
   naive_string& operator=(const naive_string &other) {
      // first, release the memory held by *this
      delete [] p;
      // then, allocate a new chunk of memory
      p = new char[other.size() + 1]; // <-- note this line
      // copy the contents themselves
      std::copy(other.p, other.p + other.size(), p);
      // adjust the size and add the trailing zero
      nelems = other.size();
      p[size()] = '\0';
      return *this;
   }
// ...
```

Agora, isso pode parecer razoavel (se um pouco longo), mas se olharmos para a linha onde a alocacao de memoria e realizada, deve-se perguntar: o que acontecera se isso falhar? De fato, poderia. Por exemplo, pode falhar se o processo estiver com pouca memoria disponivel e `other.size()` for demais para os recursos restantes. Em C++, por default, a alocacao com `operator new` lanca uma excecao em caso de falha. Isso completaria a execucao de nossa funcao de atribuicao de copia, deixando `*this` em um estado incorreto (e perigoso!), em que `p` nao e nulo e `nelems` nao e zero, mas `p` aponta para o que a maioria chamaria de lixo: memoria que nao possuimos e cujo conteudo levaria a *undefined behavior* se usada.

Poder-se-ia tentar corrigir isso escrevendo ainda mais codigo:

```cpp
// ...
   // another bad copy assignment operator
   naive_string& operator=(const naive_string &other) {
      // first, allocate a new chunk of memory
      char *q = new char[other.size() + 1];
      // then release the memory held by *this and make
      // p point to the new chunk
      delete [] p; // <-- pay attention to this line
      p = q;
      // copy the contents themselves
      std::copy(other.p, other.p + other.size(), p);
      // adjust the size and add the trailing zero
      nelems = other.size();
      p[size()] = '\0';
      return *this;
   }
// ...
```

Isso parece mais seguro na superficie, pois nao tentamos limpar o estado existente de `*this` ate termos certeza de que a alocacao funcionou. Pode ate passar na maioria dos testes — ate que alguem construa o seguinte teste:

```cpp
void test_self_assignment() {
   naive_string s0 { "This is not going to end well..." };
   s0 = s0; // oops!
}
```

Com esse caso de uso, nossa atribuicao de copia se comportara muito mal. Apos alocar um bloco de memoria devidamente dimensionado apontado por `q`, ela deletara o que `p` aponta. Infelizmente, isso tambem e o que `other.p` aponta, destruindo os dados-fonte reais que pretendemos copiar. O que se segue a esse passo le da memoria que nao possuimos mais, e o programa deixa de fazer sentido.

Ainda podemos tentar corrigir isso:

```cpp
// ...
   // this works, but it's getting complicated and
   // is a sign we're doing something wrong
   naive_string& operator=(const naive_string &other) {
      // prevent self-assignment
      if(this == &other) return *this;
      // then, do that sequence of steps
      char *q = new char[other.size() + 1];
      delete [] p; // <-- pay attention to this line
      p = q;
      std::copy(other.p, other.p + other.size(), p);
      nelems = other.size();
      p[size()] = '\0';
      return *this;
   }
// ...
```

Essa correcao e uma *pessimization* (pessimizacao), pois toda chamada de atribuicao de copia pagara pelo ramo `if` que, na pratica, quase nunca sera usado.

> **Sobre a palavra pessimizacao**
>
> A palavra *pessimization* e geralmente usada como o oposto de *optimization*, referindo-se a uma manobra ou tecnica de programacao que torna o comportamento do programa menos eficiente do que deveria. O caso anterior e um exemplo bem conhecido de tal manobra: todos pagarao pelo ramo potencial introduzido pela instrucao `if`, mesmo que quase ninguem se beneficie dele — coisas que nao deveriam acontecer.
>
> Quando diante de uma oportunidade de "pessimizacao", muitas vezes vale a pena dar um passo atras e reconsiderar. Talvez estejamos adotando o angulo errado ao abordar o problema.

Felizmente, ha um idioma bem conhecido em C++ chamado *safe assignment idiom* (idioma de atribuicao segura), coloquialmente conhecido como *copy-and-swap* (copiar-e-trocar). O segredo e reconhecer que a atribuicao e composta de duas partes: uma parte destrutiva que limpa o estado existente pertencente ao objeto de destino (o lado esquerdo da atribuicao) e uma parte construtiva que copia o estado de um objeto de origem (lado direito da atribuicao) para o objeto de destino. A parte destrutiva e geralmente equivalente ao codigo encontrado no destrutor do tipo, e a parte construtiva e geralmente equivalente ao codigo encontrado no construtor de copia do tipo.

O nome informal *copy-and-swap* para essa tecnica vem do fato de que geralmente e implementado por meio de uma combinacao do construtor de copia do tipo, seu destrutor e uma funcao membro `swap()` que troca variaveis membro uma por uma:

```cpp
// ...
   void swap(naive_string &other) noexcept {
      using std::swap; // make the standard swap function
                       // available
      swap(p, other.p); // swap data members
      swap(nelems, other.nelems);
   }

   // idiomatic copy assignment
   naive_string& operator=(const naive_string &other) {
      naive_string { other }.swap(*this); // <-- here
      return *this; // yes, that's it!
   }
// ...
```

Esse idioma e extremamente util de conhecer e usar, pois e seguro a excecoes, simples e funciona para quase todos os tipos. A linha que faz todo o trabalho realiza tres etapas:

1. Primeiro, ela constroi uma copia anonima de `other` usando o construtor de copia desse tipo. Agora, isso pode falhar se uma excecao for lancada, mas se isso acontecer, `*this` nao foi modificado e, portanto, permanece sem corrupcao.
2. Segundo, ela troca o conteudo desse temporario anonimo (que contem o que queremos colocar em `*this`) com o conteudo do objeto de destino (colocando o estado agora desnecessario naquele objeto temporario anonimo).
3. Finalmente, o objeto temporario anonimo e destruido no final da expressao (sendo anonimo), deixando `*this` com uma copia do estado de `other`.

Esse idioma ate funciona com seguranca para auto-atribuicao. Ele incorre em uma copia desnecessaria, mas troca um ramo `if` que toda chamada pagaria — mas quase nenhuma se beneficiaria — por uma copia que raramente sera inutil.

Voce pode notar `noexcept` antes da chave de abertura da funcao membro `swap()`. Voltaremos a isso mais tarde, mas por agora, o ponto importante e que podemos afirmar que essa funcao (que troca objetos de tipos fundamentais) nunca lancara uma excecao. Essa informacao nos ajudara a alcanzar algumas otimizacoes valiosas mais adiante neste livro.

#### Operacoes de movimento

Nossa `naive_string`, complementada por seu destrutor, construtor de copia e funcoes membro de atribuicao de copia, agora gerencia recursos adequadamente. Ela poderia, no entanto, ser tornada mais rapida e, as vezes, ate mais segura.

Considere o seguinte operador de concatenacao de strings nao membro que alguem poderia querer adicionar para complementar nossa classe:

```cpp
// returns the concatenation of s0 and s1
naive_string operator+(naive_string s0, naive_string s1);
```

Uma operacao como essa poderia ser usada em codigo de usuario como o seguinte:

```cpp
naive_string make_message(naive_string name) {
   naive_string s0{ "Hello "},
                s1{ "!" };
   return s0 + name + s1; // <-- note this line
}
```

A expressao que segue a instrucao `return` primeiro realiza uma chamada a `operator+()` e cria um objeto `naive_string` sem nome a partir da concatenacao de `s0` e `name`. Em seguida, esse objeto sem nome e passado como primeiro argumento para outra chamada a `operator+()` que produz outro objeto sem nome criado a partir da concatenacao do primeiro objeto sem nome e `s1`. Com nossa implementacao atual, cada objeto sem nome incorre em uma alocacao, uma copia dos dados mantidos em seu buffer, uma destruicao e mais. E mais custoso do que parece a primeira vista e ainda piorado pelo fato de que cada alocacao pode lancar uma excecao.

Ainda assim, funciona.

Desde o C++11, podemos tornar esse codigo significativamente mais eficiente por meio de *move semantics* (semantica de movimento). Alem das tradicionais funcoes da *rule of three* (regra dos tres) que acabamos de discutir, pode-se aumentar uma classe como `naive_string` com um construtor de movimento e um operador de atribuicao de movimento. Esses serao acionados implicitamente quando o compilador operar em objetos que ele sabe que nao serao mais usados.

Quando se pensa bem, a razao para as operacoes de copia e manter o objeto de origem intacto, caso precisemos dele mais tarde. Objetos temporarios sem nome nao precisam ser preservados de modificacoes posteriores, pois nao podem ser referenciados depois. Por essa razao, podemos ser mais agressivos com eles e essencialmente mover seu conteudo em vez de copia-lo. A regra que o padrao nos pede para seguir e deixar o objeto movido em um *valid-yet-indeterminate state* (estado valido, porem indeterminado). Essencialmente, o objeto movido deve estar em um estado tal que possa ser destruido ou atribuido com seguranca, e seus invariantes ainda devem ser respeitados. Na pratica, isso geralmente equivale a deixar o objeto movido em algo equivalente ao seu estado default.

Para o nosso tipo `naive_string`, um construtor de movimento poderia ter a seguinte aparencia:

```cpp
// ...
   naive_string(naive_string &&other) noexcept
      : p{ std::move(other.p) },
        nelems{ std::move(other.nelems) } {
      other.p = nullptr;
      other.nelems = 0;
   }
// ...
```

As chamadas a `std::move()` neste caso especifico poderiam ser evitadas (mover objetos de tipos fundamentais e equivalente a copia-los), mas provavelmente e mais higienico garantir que a intencao de mover esses objetos esteja inscrita explicitamente no codigo-fonte. Veremos `std::move()` resumidamente mais adiante nesta secao, mas a coisa importante a lembrar e que `std::move()` nao move nada. Ele marca um objeto como movivel aos olhos do compilador. Em outras palavras, e um *cast*.

As coisas importantes a observar com nosso construtor de movimento sao as seguintes:

- O argumento e do tipo `naive_string&&`. Isso significa que e uma referencia a um *rvalue*, sendo que *rvalue* significa informalmente "algo que se poderia encontrar no lado direito de um operador de atribuicao."
- Como `swap()`, e marcado como `noexcept` para expressar o fato de que nenhuma excecao sera lancada durante sua execucao.
- Efetivamente, esta transferindo estado do objeto de origem, `other`, para o objeto em construcao, `*this`. Apos a conclusao dessa transferencia, deixamos `other` em um estado valido (equivalente ao que teriamos com um objeto `naive_string` default), respeitando a recomendacao do padrao.

Pode-se escrever essa funcao de uma maneira um pouco mais concisa com uma pequena, mas bastante util funcao encontrada no cabecalho `<utility>`, chamada `std::exchange()`. De fato, considere a seguinte expressao:

```cpp
a = std::exchange(b, c);
```

Essa expressao significa "atribua a `a` o valor de `b`, mas substitua o valor de `b` pelo valor de `c`." Esta e uma sequencia de operacoes muito comum em codigo real. Com essa funcao, nosso construtor de movimento fica assim:

```cpp
// ...
   naive_string(naive_string &&other) noexcept
      : p{ std::exchange(other.p, nullptr) },
        nelems{ std::exchange(other.nelems, 0) } {
   }
// ...
```

Essa forma e C++ idiomatico e pode levar a algumas otimizacoes interessantes em algumas circunstancias.

E quanto a atribuicao de movimento? Bem, podemos usar a atribuicao de copia idiomatica que discutimos anteriormente e expressa-la como segue:

```cpp
   // idiomatic copy assignment
   naive_string& operator=(naive_string &&other) noexcept {
      naive_string { std::move(other) }.swap(*this);
      return *this;
   }
```

Seguindo o caminho definido por nosso operador de atribuicao de copia, expressamos o operador de atribuicao de movimento como uma combinacao de `swap()`, um destrutor e um construtor de movimento. A logica geral por tras de ambos os idiomas e a mesma.

## Arrays

Usamos arrays em nossos exemplos anteriores, mas nao fornecemos realmente uma definicao formal para essa construcao util, embora de baixo nivel. Observe que nesta secao, o termo "array" se refere a arrays nativos (built-in arrays), nao a outras construcoes muito uteis, porem de mais alto nivel, como `std::vector<T>` ou `std::array<T,N>`.

Simplificando, em C++, um array e uma sequencia contigua de elementos do mesmo tipo. Assim, no trecho a seguir, o objeto `a0` ocupa `10*sizeof(int)` bytes na memoria, enquanto o objeto `a1` ocupa `20*sizeof(std::string)` bytes:

```cpp
int a0[10];
std::string a1[20];
```

O numero de bytes entre elementos nos indices `i` e `i+1` em um array de algum tipo `T` e precisamente igual a `sizeof(T)`.

Considere a seguinte expressao, que seria usada em C++, como em C, para algum array `arr`:

```cpp
arr[i]
```

Ela avalia para o mesmo endereco que o seguinte:

```cpp
*(arr + i)
```

Como a aritmetica de ponteiros e tipada, a parte `+ i` nessa expressao significa "mais `i` elementos" ou "mais `i` vezes o tamanho de um elemento em bytes."

Os tamanhos dos arrays sao positivos, mas nao podem ser 0, a menos que o array seja alocado dinamicamente:

```cpp
int a0[5]; // Ok
static_assert(sizeof a0 == 5 * sizeof(int));
enum { N = sizeof a0 / sizeof a0[0] }; // N == 5
// int a1[0]; // not allowed: the array would be at the
              // same address as the next object in memory!
int *p0 = new int[5]; // Ok, but you have to manage the
                      // pointee now
int *p1 = new int[0]; // Ok, dynamically allocated; you
                      // still have to manage the pointee
// ...
delete [] p1; // good
delete [] p0; // good; be responsible
```

Cada chamada a `operator new[]` deve retornar um endereco diferente, mesmo que o tamanho do array seja 0. Cada chamada tecnicamente retorna o endereco de um objeto diferente.

## Resumo

Neste capitulo, demos uma olhada em ideias fundamentais da linguagem C++: o que e um objeto? O que sao ponteiros e referencias? O que queremos dizer quando falamos sobre o tamanho e o alinhamento de um objeto ou de um tipo? Por que nao existem objetos de tamanho zero em C++? Quais sao as funcoes membro especiais de uma classe e quando precisamos escreve-las explicitamente? Esta lista nao exaustiva de topicos nos forneceu um vocabulario comum a partir do qual construir o que voce, caro leitor, encontrara nos capitulos vindouros.

Com isso, estamos prontos para colocar a mao na massa. Nos demos um conjunto de ferramentas e ideias de baixo nivel a partir das quais construir abstracoes de mais alto nivel, mas precisamos nos dar uma certa medida de disciplina.

O proximo capitulo discutira algumas coisas que precisamos evitar. Estas incluem *undefined behavior*, *implementation-defined behavior* (em menor medida), codigo *ill-formed no-diagnostic-required*, *buffer overflows* e outros comportamentos nao recomendados.

Em seguida, apresentaremos um capitulo descrevendo os *casts* do C++ e como eles podem nos ajudar a expressar ideias claras, mesmo quando sentimos a necessidade de contornar algumas das regras estabelecidas pelo sistema de tipos da linguagem.

Depois disso, comecaremos a construir abstracoes belas e poderosas que nos ajudarao com nosso objetivo declarado de gerenciar recursos de forma segura e eficiente em geral, e gerenciar memoria em particular.
