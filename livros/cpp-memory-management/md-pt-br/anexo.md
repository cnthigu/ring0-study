# Anexo - Coisas que Voce Deveria Saber (Things You Should Know)

> Traducao nao oficial do livro *C++ Memory Management* (Patrice Roy, 2025).
> Texto adaptado para portugues brasileiro com fins educacionais. Paginas originais: 383-406.
> Repositorio: [ring0-study](https://github.com/cnthigu/ring0-study)

> Navegacao: [Anterior](capitulo-15.md) | [Indice](README.md)

---

Este livro pressupoe que os leitores possuam alguma base tecnica que alguns podem nao considerar "conhecimento comum". Nas secoes a seguir, voce pode encontrar as informacoes complementares que o ajudarao a aproveitar ao maximo este livro. Consulte quando necessario, e aproveite!

Sinta-se livre para passar rapidamente pelas secoes a seguir se achar que conhece bem o conteudo, e examine mais detalhadamente aquelas com as quais se sente menos confortavel. Voce pode ate pular este anexo inteiro e voltar se perceber durante a leitura do livro que esses topicos nao sao tao familiares quanto pensava.

O objetivo geral e aproveitar ao maximo este livro!

---

## `struct` e `class`

Em C++, as palavras `struct` e `class` significam essencialmente a mesma coisa, e o seguinte codigo e perfeitamente legal:

```cpp
struct Desenhavel {
   virtual void desenhar() = 0;
   virtual ~Desenhavel() = default;
};

class Pintura : public Desenhavel {
   void desenhar() override;
};
```

Alguns detalhes a observar:

- C++ nao tem uma palavra-chave `abstract` como algumas outras linguagens. Uma funcao membro abstrata em C++ e `virtual` e tem `= 0` em vez de uma definicao. A palavra-chave `virtual` significa "pode ser especializada por classes derivadas" (o `= 0` significa essencialmente "deve ser especializada..."). Frequentemente falamos de *sobrescrever* a funcao ao especializar uma funcao membro virtual. Funcoes que devem ser sobrescritas sao chamadas de *funcoes virtuais puras*.

> **Fornecendo uma implementacao padrao para uma funcao virtual pura**
>
> E possivel fornecer uma definicao para uma funcao membro abstrata: nao e tipico, mas e possivel. Isso pode ser util quando a classe base quer fornecer uma implementacao padrao de um servico mas requer que as classes derivadas pelo menos considerem fornecer a sua propria:
>
> ```cpp
> #include <iostream>
>
> struct X { virtual int f() const = 0; };
> int X::f() const { return 3; }
>
> struct D : X {
>    int f() const override {
>       return X::f() + 1;
>    }
> };
>
> void g(X &x) { std::cout << x.f() << '\n'; }
>
> int main() {
>    D d;
>    // X x; // ilegal: X tem uma funcao virtual pura
>    g(d);
> }
> ```

- Classes em C++ possuem destrutores que lidam com o que acontece quando um objeto atinge o fim de seu tempo de vida. Ao contrario de muitas outras linguagens populares, objetos automaticos e estaticos em C++ tem tempos de vida deterministicos, e usar destrutores de forma eficiente e idiomatico na linguagem. Em uma classe polimórfica (uma classe com pelo menos uma funcao membro virtual), e costume ter um destrutor virtual (aqui, `virtual ~Desenhavel()`) para indicar que, em uma situacao como a seguinte, destruir um objeto usado por meio de uma indirecao como `p` deve efetivamente destruir o objeto apontado (`Pintura`), nao o denotado pelo tipo estatico do ponteiro (`Desenhavel`):

```cpp
//
// o seguinte assume que Pintura e uma classe publica
// derivada de Desenhavel como sugerido anteriormente
//
Desenhavel *p = new Pintura;
// ...
delete p; // <-- aqui
```

- Uma classe pode derivar de uma `struct` assim como uma `struct` pode derivar de uma `class`, pois ambas sao estruturalmente equivalentes. As principais diferencas sao que, para uma `struct`, a heranca e `public` por padrao (mas isso pode ser alterado usando `protected` ou `private`), o mesmo valendo para os membros; ao passo que para uma `class`, a heranca e os membros sao `private` por padrao (mas tambem podem ser alterados).

Vale notar que e perfeitamente valido em C++ ter uma funcao membro com um qualificador de acesso na classe base (por exemplo, `Desenhavel::desenhar()`, que e `public`) e na classe derivada (por exemplo, `Pintura::desenhar()`, que e `private`). Algumas outras linguagens populares nao permitem isso.

---

## `std::size_t`

O tipo `std::size_t` e um alias para algum tipo integral sem sinal, mas o tipo real pode variar de compilador para compilador (pode ser `unsigned int`, `unsigned long`, `unsigned long long`, etc.).

O tipo `std::size_t` e frequentemente encontrado ao discutir tamanhos de containers e o espaco ocupado na memoria por um objeto como expresso pelo operador `sizeof`.

---

## O Operador `sizeof`

O operador `sizeof` retorna o tamanho em bytes de um objeto ou de um tipo. E avaliado em tempo de compilacao e sera utilizado extensivamente ao longo deste livro, pois precisaremos dessa informacao para alocar blocos de memoria de tamanho adequado:

```cpp
auto s0 = sizeof(int); // s0 e o numero de bytes em um int
                       // (parenteses obrigatorios)
int n;
auto s1 = sizeof n;    // s1 e o numero de bytes ocupados
                       // por n, identico a s0.
                       // Nota: para objetos, parenteses sao
                       // permitidos mas nao obrigatorios
```

O tamanho de um objeto e um dos componentes-chave do gerenciamento de memoria e influencia a velocidade de execucao dos programas. Por esse motivo, e um tema recorrente ao longo deste livro.

---

## Asserções

Asserções sao afirmacoes de fatos que os programadores acreditam que devem ser mantidas pelo codigo. Algumas sao dinamicas, baseadas em informacoes conhecidas em tempo de execucao, por exemplo: "O ponteiro a seguir nao deve ser nulo neste ponto." Outras sao estaticas, baseadas em informacoes conhecidas em tempo de compilacao, por exemplo: "Este programa foi escrito com a suposicao nao-portavel de que um `int` ocupa quatro bytes de armazenamento." Neste ultimo caso, temos um programa escrito com base em uma suposicao nao-portavel e precisamos viver com essa escolha, mas nao queremos que nosso codigo compile em plataformas onde essa suposicao nao se sustenta.

Para asserções dinamicas, e costume usar a macro `assert()` do cabecalho `<cassert>`. Essa macro recebe como argumento uma expressao booleana e interrompe a execucao do programa se ela for avaliada como `false`:

```cpp
void f(int *p) {
   assert(p); // afirmamos que p != nullptr e verdadeiro
   // usa *p
}
```

Note que muitos projetos desabilitam `assert()` no codigo de producao, o que pode ser feito definindo a macro `NDEBUG` antes da compilacao. Por isso, nunca coloque expressoes com efeitos colaterais em `assert()`, pois pode ser removida pelas opcoes do compilador:

```cpp
int *obter_buf(int);

void perigo(int n) {
   int *p; // nao inicializado
   assert(p = obter_buf(n)); // PERIGOSO!!!
   // usa *p, mas p pode nao estar inicializado se assert()
   // tiver sido desabilitado. Muito ruim!
}
```

Ao contrario de `assert()`, que e uma macro de biblioteca, `static_assert` e uma funcionalidade da linguagem que impede a compilacao se sua condicao nao for atendida. Com base no exemplo anterior onde uma empresa pode ter construido software com uma suposicao nao-portavel como `sizeof(int) == 4`, podemos garantir que o codigo nao compilara (e nao fara coisas ruins) em plataformas nao suportadas:

```cpp
static_assert(sizeof(int) == 4); // so compila se a
                                 // condicao for verdadeira
```

Corrigir bugs antes de lançar um produto de software e significativamente melhor para desenvolvedores e usuarios do que corrigi-los apos o lancamento. Consequentemente, `static_assert` pode ser visto como uma ferramenta poderosa para entregar produtos de maior qualidade. E um recurso que essencialmente nao tem desvantagens.

---

## Comportamento Indefinido

O *comportamento indefinido* (undefined behavior, frequentemente abreviado como UB) resulta de uma situacao em que o padrao nao prescreve um comportamento especifico. No padrao C++, UB e um comportamento para o qual nenhum requisito e imposto. Pode levar ao problema ser ignorado, assim como pode levar a um diagnostico ou a interrupcao do programa. A ideia-chave e que, se o seu programa tem comportamento indefinido, ele nao esta respeitando as regras da linguagem e esta quebrado; seu comportamento nao e garantido em sua plataforma, nao e portavel entre plataformas ou compiladores, e nao pode ser confiavel.

Um programa C++ corretamente escrito nao tem comportamento indefinido. Ao se deparar com uma funcao que contem comportamento indefinido, o compilador pode fazer praticamente qualquer coisa com o codigo dessa funcao, o que torna o raciocinio a partir do codigo-fonte essencialmente impossivel.

O comportamento indefinido e uma das principais "coisas a ter cuidado" listadas no Capitulo 2. Esforce-se para evita-lo: ele sempre volta para te assombrar se voce o deixar no codigo.

---

## Traits de Tipo (Type Traits)

Ao longo dos anos, os programadores de C++ desenvolveram varias tecnicas para raciocinar sobre as propriedades de seus tipos, principalmente em tempo de compilacao. Inferir coisas como "O tipo `T` e `const`?" ou "O tipo `T` e trivialmente copiavel?" pode ser muito util, especialmente no contexto de codigo generico. As construcoes resultantes dessas tecnicas sao chamadas de *traits de tipo* (type traits), e muitas daquelas que foram adotadas ao longo do tempo (bem como algumas que requerem suporte do compilador para serem implementadas) foram padronizadas e podem ser encontradas no cabecalho `<type_traits>`.

A forma como os traits de tipo padrao sao expressos se padronizou ao longo do tempo, passando de construcoes complexas como `std::numeric_limits<T>`, que fornecem muitos servicos diferentes para o tipo `T`, para servicos mais especificos como `std::is_const<T>` ("O tipo `T` e de fato `const`?") ou `std::remove_const<T>` ("Por favor, me de o tipo que e como `T` mas sem a qualificacao `const` se houver"), que retornam um unico tipo ou um unico valor. A pratica mostrou que traits de tipo pequenos e unitarios que retornam um tipo (chamado `type`) ou um valor conhecido em tempo de compilacao (chamado `value`) podem ser considerados "boas praticas", e a maioria dos traits contemporaneos (incluindo os padrao) sao escritos dessa forma.

Desde C++14, os traits que retornam tipos tem aliases que terminam com `_t` (por exemplo, em vez de escrever o incantamento bastante trabalhoso `typename std::remove_const<T>::type`, pode-se escrever `std::remove_const_t<T>`), e desde C++17, os traits que retornam valores tem aliases que terminam com `_v` (por exemplo, em vez de escrever `std::is_const<T>::value`, pode-se escrever `std::is_const_v<T>`).

> **E os conceitos (concepts)?**
>
> Os traits de tipo sao uma tecnica de programacao que faz parte do C++ ha decadas, mas desde C++20 temos *conceitos* (concepts), que sao algo parecido com traits (frequentemente sao expressos por meio de traits), mas mais poderosos no sentido de que fazem parte do sistema de tipos. Este livro nao usa conceitos com frequencia, mas voce (como programador) realmente deveria se familiarizar com eles. Eles sao extremamente poderosos e extremamente uteis para a programacao contemporanea em C++.

---

## Os Traits `std::true_type` e `std::false_type`

Ao expressar traits de tipo, a biblioteca padrao aplica a pratica comum de usar os nomes `type` para tipos e `value` para valores, como neste exemplo:

```cpp
// is_const<T> e remove_const<T> artesanais
// (use as versoes padrao de <type_traits>!)

template <class> struct is_const {
   static constexpr bool value = false; // caso geral
};
// especializacao para tipos const
template <class T> struct is_const<const T> {
   static constexpr bool value = true;
};

// caso geral
template <class T> struct remove_const {
   using type = T;
};
// especializacao para const T
template <class T> struct remove_const<const T> {
   using type = T;
};
```

Acontece que muitos traits de tipo tem valores booleanos. Para simplificar a tarefa de escrever tais traits e garantir que sua forma seja homogenea, voce encontrara os tipos `std::true_type` e `std::false_type` no cabecalho `<type_traits>`. Esses tipos podem ser vistos como as contrapartes do sistema de tipos para as constantes `true` e `false`.

Com esses tipos, podemos reescrever traits como `is_const` da seguinte forma:

```cpp
#include <type_traits>

// is_const<T> artesanal (prefira as versoes std::...)
template <class> struct is_const : std::false_type {
};
template <class T>
   struct is_const<const T> : std::true_type {
   };
```

Esses tipos sao ao mesmo tempo uma conveniencia e uma forma de expressar ideias com mais clareza.

---

## O Trait `std::conditional<B,T,F>`

As vezes e util escolher entre dois tipos com base em uma condicao conhecida em tempo de compilacao. Considere o seguinte exemplo onde buscamos implementar uma comparacao entre dois valores de algum tipo `T` que se comporta de forma diferente para tipos de ponto flutuante e para outros tipos como `int`:

```cpp
#include <cmath>

// usaremos tipos-tag (classes vazias para distinguir
// assinaturas de funcoes)
struct ponto_flutuante {};
struct exato {};

// as versoes com tres argumentos nao sao para chamada
// direta do codigo do usuario
template <class T>
   bool proximo_o_suficiente(T a, T b, exato) {
      return a == b; // funciona para int, short, bool, etc.
   }

template <class T>
   bool proximo_o_suficiente(T a, T b, ponto_flutuante) {
      // nota: isso poderia se beneficiar de mais rigor,
      // mas isso e ortogonal a nossa discussao
      return std::abs(a - b) < static_cast<T>(0.000001);
   }

// esta versao com dois argumentos e a destinada ao usuario
template <class T>
   bool proximo_o_suficiente(T a, T b) {
      // NOSSO OBJETIVO: chamar a versao "ponto_flutuante"
      // para float, double e long double; chamar a versao
      // "exato" caso contrario
   }
```

Repare que nao nomeamos os argumentos dos tipos `exato` e `ponto_flutuante` em nossas funcoes `proximo_o_suficiente()`. Isso e correto pois nao usamos esses objetos; a razao para esses argumentos e garantir que as duas funcoes tenham assinaturas distintas.

Ha um trait `std::is_floating_point<T>` no cabecalho `<type_traits>` com valor `true` para numeros de ponto flutuante e `false` caso contrario. Se nao tivessemos esse trait, poderemos escrever o nosso:

```cpp
// poderíamos escrever is_floating_point<T> assim
// (mas use std::is_floating_point<T>!)
template <class> struct is_floating_point
   : std::false_type {}; // caso geral

// especializacoes
template <> struct is_floating_point<float>
   : std::true_type {};
template <> struct is_floating_point<double>
   : std::true_type {};
template <> struct is_floating_point<long double>
   : std::true_type {};

// conveniencia para simplificar o codigo do usuario
template <class T>
   constexpr bool is_floating_point_v =
      is_floating_point<T>::value;
```

Podemos usar isso para tomar nossa decisao. No entanto, nao queremos tomar uma decisao em tempo de execucao, pois a natureza do tipo `T` e totalmente conhecida em tempo de compilacao, e ninguem quer pagar por uma instrucao de desvio ao comparar inteiros!

O trait `std::conditional<B,T,F>` pode ser usado para tomar tal decisao. Se escrevermos o nosso, poderia ser assim:

```cpp
// conditional<B,T,F> artesanal (prefira a versao std::)

// caso geral (tipo incompleto)
template <bool, class T, class F> struct conditional;

// especializacoes
template <class T, class F>
   struct conditional<true, T, F> {
      using type = T; // constante true, escolhe o tipo T
   };
template <class T, class F>
   struct conditional<false, T, F> {
      using type = F; // constante false, escolhe o tipo F
   };

// conveniencia para simplificar o codigo do usuario
template <bool B, class T, class F>
   using conditional_t = typename conditional<B, T, F>::type;
```

Com esse trait, podemos escolher, em tempo de compilacao, um de dois tipos com base em um valor booleano de tempo de compilacao, que e exatamente o que estávamos tentando fazer:

```cpp
// ...
// esta versao sera chamada pelo codigo do usuario
template <class T>
   bool proximo_o_suficiente(T a, T b) {
      return proximo_o_suficiente(
         a, b, conditional_t<
            is_floating_point_v<T>,
            ponto_flutuante,
            exato
         > {}
      );
   }
```

A forma de ler isso e que o terceiro argumento na chamada a `proximo_o_suficiente()` (encontrado dentro de nossa funcao voltada ao usuario com dois argumentos) sera um objeto do tipo `ponto_flutuante` ou `exato`, mas o tipo exato sera escolhido em tempo de compilacao com base no valor da constante de tempo de compilacao `is_floating_point_v<T>`. O resultado final e que instanciamos um objeto de uma dessas duas classes vazias, chamamos o algoritmo apropriado e deixamos o inlining de funcoes fazer o resto, otimizando todo o scaffolding.

---

## Algoritmos

A biblioteca padrao de C++ contem, entre outras pedras preciosas, um conjunto de algoritmos. Cada uma dessas funcoes realiza as tarefas que um loop muito bem escrito faria, mas com nomes especificos, garantias de complexidade e otimizacoes. Assim, em vez de escrever:

```cpp
int vals[]{ 2, 3, 5, 7, 11 };
int dest[5];
for(int i = 0; i != 5; ++i)
   dest[i] = vals[i];
```

E idiomatico em C++ escrever:

```cpp
int vals[]{ 2, 3, 5, 7, 11 };
int dest[5];
std::copy(begin(vals), end(vals), begin(dest));
```

O importante a saber e que sequencias de C++ sao da forma `[begin, end)`, o que significa que para todos os algoritmos, o iterador de inicio (aqui, `begin(vals)`) e incluido e o iterador de fim (aqui, `end(vals)`) e excluido, tornando `[begin, end)` um intervalo semiaberto. Todos os algoritmos em `<algorithm>` e em seu cabecalho primo `<numeric>` seguem essa simples convencao.

> **E a biblioteca ranges?**
>
> A biblioteca `<ranges>` e uma grande adicao a biblioteca padrao de C++ desde o C++20 e as vezes pode ser usada para produzir codigo ainda melhor do que a ja tremenda biblioteca `<algorithm>`. Este livro nao usa ranges com frequencia, mas isso nao significa que essa biblioteca nao seja maravilhosa, entao sinta-se livre para usa-la e investigar formas pelas quais ela pode tornar seu codigo melhor.

---

## Functores (Objetos Funcao) e Lambdas

E costume em C++ usar *functores*, tambem chamados de *objetos funcao* (function objects), para representar computacoes com estado. Pense, por exemplo, em um programa que imprime inteiros na saida padrao usando um algoritmo:

```cpp
#include <iostream>
#include <algorithm>
#include <iterator>
using namespace std;

void exibir(int n) { cout << n << ' '; }

int main() {
   int vals[]{ 2, 3, 5, 7, 11 };
   for_each(begin(vals), end(vals), exibir);
}
```

Esse pequeno programa funciona bem, mas se quisessemos imprimir em outro lugar que nao a saida padrao, nos encontrarıamos em uma situacao desconfortavel: o algoritmo `for_each()` espera uma funcao unaria (no sentido de "funcao que aceita um unico argumento" — aqui, o valor a imprimir), entao nao ha espaco sintatico para adicionar um argumento como o stream de saida a usar. Poderíamos "resolver" esse problema com uma variavel global, ou usando uma funcao diferente para cada stream de saida, mas isso ficaria aquem de um design razoavel.

Se substituirmos a funcao `exibir` por uma classe, que chamaremos de `Exibidor` para difer-encia-las visualmente, chegamos ao seguinte:

```cpp
#include <iostream>
#include <algorithm>
#include <iterator>
#include <fstream>
using namespace std;

class Exibidor {
   ostream &os;
public:
   Exibidor(ostream &os) : os{ os } {
   }
   void operator()(int n) const { os << n << ' '; }
};

int main() {
   int vals[]{ 2, 3, 5, 7, 11 };
   // exibe na saida padrao
   for_each(begin(vals), end(vals), Exibidor{ cout });

   ofstream out{"out.txt"};
   // escreve no arquivo out.txt
   for_each(begin(vals), end(vals), Exibidor{ out });
}
```

Isso leva a um codigo legivel e de boa leitura, com flexibilidade adicional. Note que, conceitualmente, expressoes lambda sao functores (voce pode inclusive usar lambdas como classes base!), entao o exemplo anterior pode ser reescrito equivalentemente como:

```cpp
#include <iostream>
#include <algorithm>
#include <iterator>
#include <fstream>
using namespace std;

int main() {
   int vals[]{ 2, 3, 5, 7, 11 };
   // exibe na saida padrao
   for_each(begin(vals), end(vals), [](int n) {
      cout << n << ' ';
   });

   ofstream out{"out.txt"};
   // escreve no arquivo out.txt
   for_each(begin(vals), end(vals), [&out](int n) {
      out << n << ' ';
   });
}
```

As lambdas sao, portanto, essencialmente functores que se limitam a um construtor e a uma funcao membro `operator()`, e essa combinacao representa de longe o caso mais comum para esses objetos. Voce pode, claro, ainda usar functores explicitos completos se quiser mais do que isso.

---

## Amigos (`friend`)

C++ oferece um qualificador de acesso que nao e comumente encontrado em outras linguagens e e frequentemente mal compreendido: o qualificador `friend`. Uma classe pode especificar outra classe ou uma funcao como um de seus amigos, dando ao dito amigo acesso total a todos os membros dessa classe, incluindo aqueles qualificados como `protected` ou `private`.

Alguns consideram que `friend` quebra o encapsulamento, e de fato pode fazer isso se usado imprudentemente, mas a intencao e fornecer acesso privilegiado a entidades especificas em vez de expô-las como membros `public` ou `protected` que nao foram projetados para esse fim, levando a uma violacao de encapsulamento ainda maior.

Considere, por exemplo, as seguintes classes, onde `coisa` e algo destinado a ser construido a partir do conteudo de um arquivo chamado `nome` por um `fabrica_de_coisa` capaz de validar o conteudo do arquivo antes de construir a `coisa`:

```cpp
class coisa {
   coisa(string_view); // nota: privado
   // ... varios membros interessantes
   // fabrica_de_coisa pode acessar membros privados de coisa
   friend class fabrica_de_coisa;
};

// no caso de leitura de arquivo invalido
class formato_invalido{};

class fabrica_de_coisa {
   // ... varias coisas interessantes aqui tambem
   string ler_arquivo(const string &nome) const {
      ifstream in{ nome };
      // consume o arquivo de uma vez, retornando
      // todo o conteudo em uma unica string
      return { istreambuf_iterator<char>{ in },
               istreambuf_iterator<char>{ } };
   }
   bool conteudo_valido(string_view) const;
public:
   coisa criar_coisa_de(const string &nome) const {
      auto conteudo = ler_arquivo(nome);
      if(!conteudo_valido(conteudo))
         throw formato_invalido{};
      // nota: chama o construtor privado de coisa
      return { conteudo };
   }
};
```

Nao queremos que o mundo inteiro possa chamar o construtor privado qualificado de `coisa` que recebe um `string_view` arbitrario como argumento, pois esse construtor nao foi projetado para lidar com strings de caracteres que nao foram validadas. Por essa razao, so deixamos `fabrica_de_coisa` usa-lo, assim fortalecendo o encapsulamento em vez de enfraquece-lo.

E costume colocar uma classe e seus amigos juntos ao distribuir o codigo, pois eles andam juntos: um amigo de uma classe e, em essencia, uma adicao externa a interface dessa classe. Por fim, note que existem restricoes para a relacao de amizade:

**A amizade nao e reflexiva**: se `A` declara `B` como seu amigo, nao se segue que `B` declara `A` como seu amigo:

```cpp
class A {
   int n = 3;
   friend class B;
public:
   void f(B);
};
class B {
   int m = 4;
public:
   void f(A);
};

void A::f(B b) {
   // int val = b.m; // nao: A nao e amigo de B
}
void B::f(A a) {
   int val = a.n; // Ok: B e amigo de A
}
```

**A amizade nao e transitiva**: se `A` declara `B` como seu amigo e `B` declara `C` como seu amigo, nao se segue que `A` declara `C` como seu amigo:

```cpp
class A {
   int n = 3;
   friend class B;
};
class B {
   friend class C;
public:
   void f(A a) {
      int val = a.n; // Ok: B e amigo de A
   }
};
class C {
public:
   void f(A a) {
      // int val = a.n; // nao: C nao e amigo de A
   }
};
```

**A amizade nao e herdada**: se `A` declara `B` como seu amigo, nao se segue que se `C` e uma classe filha de `B`, `A` declarou `C` como seu amigo:

```cpp
class A {
   int n = 3;
   friend class B;
};
class B {
public:
   void f(A a) {
      int val = a.n; // Ok: B e amigo de A
   }
};
class C : B {
public:
   void f(A a) {
      // int val = a.n; // nao: C nao e amigo de A
   }
};
```

Usado com discernimento, `friend` resolve problemas de encapsulamento que seriam difíceis de lidar de outra forma.

---

## O Operador `decltype`

O sistema de tipos de C++ e poderoso e matizado, oferecendo (entre outras coisas) um conjunto de facilidades de deducao de tipo. A ferramenta de deducao de tipo mais conhecida e provavelmente `auto`, usada para inferir o tipo de uma expressao a partir do tipo de seu inicializador:

```cpp
const int n = f();
auto m = n;          // m e do tipo int
auto & r = m;        // r e do tipo int&
const auto & cr0 = m; // cr0 e do tipo const int&
auto & cr1 = n;      // cr1 e do tipo const int&
```

Como pode notar no exemplo anterior, por padrao, `auto` faz copias (veja a declaracao da variavel `m`), mas voce pode qualificar `auto` com `&`, `&&`, `const`, etc., se necessario.

As vezes, voce quer deduzir o tipo de uma expressao com mais precisao, mantendo os varios qualificadores que a acompanham. Isso pode ser util ao inferir o tipo de uma expressao aritmetica, o tipo de uma lambda, o tipo de retorno de uma funcao generica complexa, etc. Para isso, existe o operador `decltype`:

```cpp
template <class T>
   T& passagem(T &arg) {
      return arg;
   }

int main() {
   int n = 3;
   auto m = passagem(n);              // m e um int
   ++m;
   cout << n << ' ' << m << '\n';    // 3 4
   decltype(passagem(n)) r = passagem(n); // r e um int&
   ++r;
   cout << n << ' ' << r << '\n';    // 4 4
}
```

O uso de `auto` tornou-se comum no codigo C++ desde o C++11. O operador `decltype`, tambem parte do C++ desde o C++11, e uma ferramenta mais afiada, ainda amplamente utilizada, mas para casos de uso mais especializados.

### Quando os tipos sao dificeis de escrever

No exemplo anterior com `decltype`, escrevemos `passagem(n)` duas vezes: uma no operador `decltype` e outra na chamada real da funcao. Isso nao e pratico em geral, pois duplica o esforco de manutencao. Desde C++14, pode-se usar `decltype(auto)` para expressar "o tipo totalmente qualificado da expressao inicializadora."

Assim, escreveriamos customariamente `decltype(auto) r = passagem(n);` para expressar que `r` deve ter o tipo totalmente qualificado da expressao `passagem(n)`.

---

## Encaminhamento Perfeito (Perfect Forwarding)

O advento dos templates variadicos em C++11 tornou necessario garantir que haja uma maneira de a semantica no ponto de chamada de uma funcao ser transmitida por toda a cadeia de chamadas. Isso pode parecer abstrato, mas e bastante real e tem implicacoes no efeito das chamadas de funcao.

Considere a seguinte classe:

```cpp
#include <string>

struct X {
   X(int, const std::string&); // A
   X(int, std::string&&);      // B
   // ... outros construtores e varios membros
};
```

Essa classe expoe pelo menos dois construtores: um que recebe `int` e `const string&` como argumento, e outro que recebe `int` e `string&&`. Para tornar o exemplo mais geral, tambem supomos a existencia de outros construtores de `X`. Se chamassemos esses dois construtores explicitamente:

```cpp
X x0{ 3, "hello" };            // chama A
string s = "hi!";
X x1{ 4, s };                  // tambem chama A
X x2{ 5, string{ "there" } };  // chama B
X x3{ 5, "there too"s };       // tambem chama B
```

O construtor de `x0` chama `A`, pois `"hello"` e `const char(&)[6]` (incluindo o `'\0'` final), nao um tipo `string`, mas o compilador pode sintetizar uma `string` temporaria para passar como `const string&`. O construtor de `x1` tambem chama `A`, pois `s` e uma `string` nomeada, que nao pode ser implicitamente passada por movimentacao. Os construtores de `x2` e `x3` chamam `B`, que recebe `string&&`, pois ambos recebem objetos `string` temporarios e anonimos que podem ser implicitamente passados por movimentacao.

Agora, suponhamos que queremos escrever uma fabrica de objetos `X` que repassa argumentos ao construtor de `X` apropriado apos algum trabalho preliminar. Digamos que apenas registramos que estamos construindo um objeto `X`. Escrevendo assim:

```cpp
template <class ... Args>
   X criarX(Args ... args) {
      clog << "Criando um objeto X\n";
      return X(args...); // <-- AQUI (incorreto: perde move)
   }
```

Nesse caso, todos os argumentos teriam nomes e seriam passados por valor, portanto o construtor que recebe `string&&` nunca seria escolhido.

Escrevendo com referencias de encaminhamento (forwarding references):

```cpp
template <class ... Args>
   X criarX(Args &&... args) {
      clog << "Criando um objeto X\n";
      return X(args...); // <-- AQUI (ainda incorreto)
   }
```

A assinatura de `criarX()` agora esta correta, mas os argumentos recebidos como referencias rvalue tem nome dentro de `criarX()` (fazem parte do pacote `args`!), portanto, ao chamar o construtor de `X`, nao ha movimentacao implicita.

O que precisamos fazer para completar nosso esforco e converter de volta cada argumento para o tipo que tinha no ponto de chamada. Esse tipo esta inscrito em `Args`, o tipo do nosso pacote, e a forma de realizar essa conversao e aplicar `std::forward<T>()` a cada argumento do pacote. Uma funcao `criarX()` correta, por fim, seria:

```cpp
template <class ... Args>
   X criarX(Args &&... args) {
      clog << "Criando um objeto X\n";
      return X(std::forward<Args>(args)...); // <-- AQUI
   }
```

Existem sintaxes mais simples, mas chegamos la!

---

## O Padrao de Projeto Singleton

Ha muitos padroes de projeto por ai. Padroes de projeto sao um topico por si mesmos, representando formas bem conhecidas de resolver problemas que podem ser representados de forma abstrata, nomeados, explicados a outros e reificados dentro das restricoes e idiomas da linguagem de programacao escolhida.

O padrao de projeto *singleton* descreve maneiras pelas quais podemos escrever uma classe que garante ser instanciada apenas uma vez em um programa.

Singleton nao e um padrao muito apreciado: dificulta testes, introduz dependencias no estado global, representa um unico ponto de falha no programa e um potencial gargalo em todo o programa, complica o multithreading (se o singleton for mutavel, seu estado requer sincronizacao), etc. Mas tem seus usos e e utilizado na pratica, e nós o usamos ocasionalmente neste livro.

Ha muitas maneiras de escrever uma classe que e instanciada apenas uma vez em um programa com a linguagem C++. Todas elas compartilham algumas caracteristicas-chave:

- As operacoes de copia do tipo precisam ser deletadas. Se um singleton puder ser copiado, havera mais de uma instancia desse tipo, levando a uma contradicao.
- Nao deve haver construtor publico. Se houvesse, o codigo cliente poderia chama-lo e criar mais de uma instancia.
- Nao deve haver membros `protected`. Objetos de classes derivadas tambem sao, conceitualmente, objetos da classe base, levando novamente a uma contradicao.
- Como nao ha construtor publico, deve haver um construtor privado (provavelmente um construtor padrao), e esse so sera acessivel a propria classe ou a seus amigos (se houver).

Olharemos para maneiras de implementar um singleton excessivamente simplista em C++. Para fins deste exemplo, o singleton fornecera inteiros sequenciais sob demanda:

```cpp
#include <atomic>

class ProvedorIdSequencial {
   // ...
   std::atomic<long long> atual; // estado (sincronizado)
   // construtor padrao (privado)
   ProvedorIdSequencial() : atual{ 0LL } {
   }
public:
   // servico oferecido pelo singleton (sincronizado)
   auto proximo() { return atual++; }
   // operacoes de copia deletadas
   ProvedorIdSequencial(const ProvedorIdSequencial&)
      = delete;
   ProvedorIdSequencial&
      operator=(const ProvedorIdSequencial&) = delete;
   // ...
};
```

### Instanciacao na inicializacao do programa

Uma maneira de instanciar um singleton e criá-lo antes que `main()` comece, tornando-o um membro de dados estatico de sua classe. Isso requer declarar o singleton na classe e defini-lo em um arquivo fonte separado para evitar problemas com a ODR.

> **ODR?**
>
> A *One Definition Rule* (Regra de Unica Definicao, ODR) e os problemas associados sao descritos no Capitulo 2, mas a ideia e que em C++, cada objeto pode ter muitas declaracoes, mas apenas uma definicao.

Uma possivel implementacao seria:

```cpp
#include <atomic>

class ProvedorIdSequencial {
   // declaracao (privada)
   static ProvedorIdSequencial singleton;
   std::atomic<long long> atual; // estado (sincronizado)
   // construtor padrao (privado)
   ProvedorIdSequencial() : atual{ 0LL } {
   }
public:
   // funcao membro estatica que fornece acesso ao objeto
   static auto & obter() { return singleton; }
   // servico oferecido pelo singleton (sincronizado)
   auto proximo() { return atual++; }
   // operacoes de copia deletadas
   ProvedorIdSequencial(const ProvedorIdSequencial&)
      = delete;
   ProvedorIdSequencial&
      operator=(const ProvedorIdSequencial&) = delete;
   // ...
};

// em algum arquivo fonte, p.ex. ProvedorIdSequencial.cpp
#include "ProvedorIdSequencial.h"
// definicao (chama o construtor padrao)
ProvedorIdSequencial ProvedorIdSequencial::singleton;
```

Isso funciona bem e e eficiente, desde que nao haja dependencia entre objetos globais separados. Por exemplo, se outro singleton no mesmo programa precisasse dos servicos de `ProvedorIdSequencial`, poderíamos ter problemas, pois C++ nao garante a ordem em que objetos globais de multiplos arquivos sao instanciados.

Um possivel codigo cliente para essa implementacao seria:

```cpp
auto & provedor = ProvedorIdSequencial::obter();
for(int i = 0; i != 5; ++i)
   cout << provedor.proximo() << ' ';
```

### Instanciacao na primeira chamada

Outra maneira de instanciar um singleton e criá-lo na primeira vez que seus servicos sao solicitados, tornando-o uma variavel estatica da funcao que fornece acesso ao singleton. Dessa forma, como variaveis locais estaticas sao criadas na primeira vez que a funcao e chamada e mantem seu estado apos isso, um singleton pode fornecer servicos a outros singletons, desde que isso nao crie ciclos.

Uma possivel implementacao seria:

```cpp
#include <atomic>

class ProvedorIdSequencial {
   std::atomic<long long> atual; // estado (sincronizado)
   // construtor padrao (privado)
   ProvedorIdSequencial() : atual{ 0LL } {
   }
public:
   // funcao membro estatica que fornece acesso ao objeto
   static auto & obter() {
      static ProvedorIdSequencial singleton; // definicao
      return singleton;
   }
   // servico oferecido pelo singleton (sincronizado)
   auto proximo() { return atual++; }
   // operacoes de copia deletadas
   ProvedorIdSequencial(const ProvedorIdSequencial&)
      = delete;
   ProvedorIdSequencial&
      operator=(const ProvedorIdSequencial&) = delete;
   // ...
};
```

O codigo cliente possivel para essa implementacao seria:

```cpp
auto & provedor = ProvedorIdSequencial::obter();
for(int i = 0; i != 5; ++i)
   cout << provedor.proximo() << ' ';
```

Note que essa versao tem um custo oculto: variaveis estaticas locais a funcoes sao chamadas de *magic statics* em C++, pois a linguagem garante que serao construidas apenas uma vez, mesmo que duas ou mais threads chamem a funcao simultaneamente. Essa propriedade implica que o acesso a essa variavel estatica envolve alguma sincronizacao e que essa sincronizacao e paga em cada chamada a essa funcao. O codigo cliente acima atenua esse custo chamando `ProvedorIdSequencial::obter()` uma vez e reutilizando a referencia obtida apos essa chamada; e a chamada a `obter()` que introduz o custo de sincronizacao.

---

## A Funcao `std::exchange()`

Ha (pelo menos) duas funcoes muito uteis e fundamentais escondidas no cabecalho `<utility>`. Uma e bem conhecida e existe ha muito tempo: `std::swap()`, usada para muitos propositos em toda a biblioteca padrao e no codigo do usuario.

A outra, mais recente, e `std::exchange()`. Enquanto `swap(a, b)` troca os valores dos objetos `a` e `b`, a expressao `a = exchange(b, c)` altera o valor de `b` pelo valor de `c`, retornando o valor antigo de `b` (para atribuir a `a`). Isso pode parecer estranho a primeira vista, mas e uma facilidade muito util.

Considere o construtor de movimentacao para o seguinte `array_tamanho_fixo` simplificado:

```cpp
template <class T>
   class array_tamanho_fixo {
      T *elems{};
      std::size_t nelems{};
   public:
      // ...
      array_tamanho_fixo(array_tamanho_fixo &&outro)
         : elems{ outro.elems }, nelems{ outro.nelems } {
         outro.elems = nullptr;
         outro.nelems = 0;
      }
      // ...
   };
```

Pode notar que esse construtor faz duas coisas: captura os membros de dados de `outro` e substitui os membros de `outro` por valores padrao. Esse e o caso classico para `std::exchange()`, entao esse construtor pode ser simplificado como:

```cpp
template <class T>
   class array_tamanho_fixo {
      T *elems{};
      std::size_t nelems{};
   public:
      // ...
      array_tamanho_fixo(array_tamanho_fixo &&outro)
         : elems{ std::exchange(outro.elems, nullptr) },
           nelems{ std::exchange(outro.nelems, 0) } {
      }
      // ...
   };
```

Com `std::exchange()`, essa operacao comum de dois passos pode ser reduzida a uma chamada de funcao, simplificando o codigo e tornando-o mais eficiente (nesse caso, convertendo atribuicoes em chamadas de construtores).

---

> Navegacao: [Anterior](capitulo-15.md) | [Indice](README.md)
