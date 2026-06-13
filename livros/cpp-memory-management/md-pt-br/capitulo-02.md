# Capitulo 2 - Cuidados Necessarios (Things to Be Careful With)

> Traducao nao oficial do livro *C++ Memory Management* (Patrice Roy, 2025).
> Texto adaptado para portugues brasileiro com fins educacionais. Paginas originais: 33-46.
> Repositorio: [ring0-study](https://github.com/cnthigu/ring0-study)

> Navegacao: [Anterior](capitulo-01.md) | [Indice](README.md) | [Proximo](capitulo-03.md)

---

Entao voce decidiu ler um livro sobre gerenciamento de memoria em C++ e esta tao pronto para estudar as abordagens e tecnicas de alto nivel quanto disposto a "colocar a mao na massa" para obter controle fino sobre o processo de gerenciamento de memoria. Excelente plano!

Como voce sabe que vai escrever tanto codigo de muito alto nivel quanto de muito baixo nivel, ha algumas coisas que precisamos garantir que voce conheca para que nao se meta em apuros nem escreva codigo que parece funcionar mas nao funciona — pelo menos, nao de forma portatil.

Neste capitulo, vamos apontar alguns aspectos da programacao C++ que entram em jogo ao longo deste livro, mas com os quais voce deve ter cuidado. Isso pode parecer um (muito) pequeno compendio de praticas ruins ou um incentivo para se meter em problemas, mas considere o que se segue como formas de usar funcionalidades um tanto perigosas ou complicadas de maneira adequada. Voce esta usando C++, tem liberdade significativa de expressao e tem acesso a funcionalidades que sao uteis se as conhece e compreende bem.

Queremos codigo limpo e eficiente, e queremos programadores responsaveis. Vamos tentar chegar la juntos.

Neste capitulo, aprenderemos sobre os seguintes topicos:

- Cobriremos algumas das maneiras pelas quais se pode se meter em apuros com codigo C++. De fato, ha coisas que um compilador nao consegue diagnosticar confiavelmente, assim como ha coisas para as quais o padrao C++ nao diz o que acontecera, e escrever codigo que faz tais coisas e uma receita para o desastre — ou pelo menos para comportamentos surpreendentes ou nao portateis.
- Em particular, exploraremos como se pode ter problemas com ponteiros. Como este livro discute gerenciamento de memoria, usaremos ponteiros e aritmetica de ponteiros com bastante frequencia, e ser capaz de distinguir usos adequados de usos inadequados dessa ferramenta de baixo nivel (mas as vezes preciosa) sera valioso.
- Por fim, discutiremos que tipos de conversoes de tipo podemos fazer sem recorrer a *type casts* (o assunto principal do Capitulo 3), e por que isso raramente e uma boa ideia, ao contrario do que se pensa popularmente.

Nosso objetivo geral sera aprender coisas que nao devemos fazer (mesmo que, ocasionalmente, realizemos algumas manobras semelhantes a elas) e evita-las depois, esperancosamente entendendo por que o fazemos. Com isso dito, teremos muitos capitulos para ver coisas que devemos fazer e como faze-las bem!

## Diferentes tipos de mal

Antes de nos aprofundarmos em algumas praticas reais que exigem cuidado, e interessante examinar as principais categorias de riscos que podemos enfrentar se nosso codigo nao respeitar as regras da linguagem. Com cada categoria vem uma forma de inconveniente que devemos nos esforcar para evitar.

### Ill-formed, no diagnostic required

Algumas construcoes em C++ sao ditas *Ill-Formed, No Diagnostic Required* (IFNDR — mal-formadas, sem exigencia de diagnostico). Com efeito, voce encontrara bastantes ocorrencias no padrao de expressoes como "se [...], o programa e mal-formado, sem exigencia de diagnostico." Quando algo e IFNDR, significa que seu programa esta quebrado. Coisas ruins podem acontecer, mas o compilador nao e obrigado a avisalo sobre elas (de fato, as vezes o compilador nao tem informacoes suficientes para diagnosticar a situacao problematica).

Violacoes da *One Definition Rule* (ODR), a qual voltaremos na secao A ODR mais adiante neste capitulo, se enquadram no IFNDR. No entanto, ha outros casos como esse, como ter um objeto global com requisitos de alinhamento diferentes (por meio de `alignas`) em diferentes *translation units* (unidades de traducao — essencialmente, arquivos-fonte diferentes), ou ter um construtor que delega a si mesmo direta ou indiretamente. Eis um exemplo:

```cpp
class X {
public:
    // #0 delegates to #1 which delegates to #0 which...
    X(float x) : X{ static_cast<int>(x) } { // #0
    }
    X(int n) : X{ n + 0.5f } { // #1
    }
};
int main() {}
```

Observe que seu compilador pode emitir um diagnostico; simplesmente nao e obrigado a faze-lo. Nao e que os compiladores sejam preguicosos — eles podem ate ser incapazes de fornecer um diagnostico em alguns casos! Portanto, tenha cuidado para nao escrever codigo que leve a situacoes IFNDR.

### Undefined behavior

Mencionamos o *Undefined Behavior* (UB — comportamento indefinido) no Capitulo 1. O UB e frequentemente visto como uma fonte de dores de cabeca e sofrimento para programadores C++, mas ele se refere a qualquer comportamento para o qual o padrao C++ nao imponha nenhum requisito. Na pratica, isso significa que, se voce escrever codigo que contenha UB, nao tem ideia do que vai acontecer em tempo de execucao (pelo menos se seu objetivo e codigo razoavelmente portatil). Exemplos canonicos de UB incluem desreferenciar um ponteiro nulo ou um ponteiro nao inicializado: faca isso e voce tera serios problemas.

Para os compiladores, o UB nao deveria acontecer (codigo que respeita as regras da linguagem nao contem UB, afinal). Por essa razao, os compiladores "otimizam em torno de" codigo que contem UB, com efeitos as vezes surpreendentes: eles podem comecar a remover testes e ramos, eliminar loops, e assim por diante.

Os efeitos do UB tendem a ser locais. Por exemplo, no seguinte codigo, ha um teste que garante que `p` nao e nulo antes de usar `*p` em um caso, mas ha pelo menos um acesso a `*p` sem verificacao. Este codigo esta quebrado (o acesso nao verificado a `*p` e UB), portanto, o compilador tem permissao para reescre-lo de forma que todos os testes para verificar se `p` nao e nulo sejam efetivamente removidos. Afinal, o dano ja teria sido feito se `p` fosse `nullptr`, entao o compilador tem o direito de assumir que o programador passou um ponteiro nao nulo para a funcao!

```cpp
int g(int);
int f(int *p) {
    if(p != nullptr)
        return g(*p); // Ok, we know p is not null
    return *p; // oops, if p == nullptr this is UB
}
```

O corpo inteiro de `f()` poderia legitimamente ser reescrito pelo seu compilador como `return g(*p)` nesse caso, com a instrucao `return *p` sendo transformada em codigo inatingivel (*unreachable code*).

O potencial para UB se esconde em varios lugares da linguagem, incluindo overflow de inteiros com sinal, acesso a arrays fora dos limites, *data races*, e assim por diante. Ha esforcos em andamento para reduzir o numero de casos potenciais de UB (ha inclusive um grupo de estudo, o SG12, dedicado a esse esforco), mas o UB provavelmente permanecera parte da linguagem por um futuro previsivel, e precisamos estar cientes disso.

### Comportamento definido pela implementacao

Algumas partes do padrao se enquadram na categoria de *implementation-defined behavior* (comportamento definido pela implementacao), ou seja, comportamento com o qual voce pode contar em uma plataforma especifica. Esse e um comportamento que a plataforma de sua escolha deve documentar, mas que nao tem garantia de portabilidade para outras plataformas.

O comportamento definido pela implementacao ocorre em muitas situacoes e inclui coisas como limites definidos pela implementacao: o numero maximo de parenteses aninhados; o numero maximo de *labels* de `case` em uma instrucao `switch`; o tamanho real de um objeto; o numero maximo de chamadas recursivas em uma funcao `constexpr`; o numero de bits em um byte; e assim por diante. Outros casos bem conhecidos de comportamento definido pela implementacao incluem o numero de bytes em um objeto `int` ou se o tipo `char` e um tipo inteiro com sinal ou sem sinal.

O comportamento definido pela implementacao nao e realmente uma fonte de mal, mas pode ser problematico se alguem busca codigo portatil, mas depende de suposicoes nao portateis. As vezes e util explicitar as suposicoes no codigo por meio de `static_assert` quando a suposicao pode ser validada em tempo de compilacao, ou algum mecanismo similar em tempo de execucao, para perceber — antes que seja tarde demais — que essas suposicoes sao invalidas para uma determinada plataforma-alvo. Por exemplo:

```cpp
int main() {
    // our code supposes int is four bytes wide, a non-
    // portable assumption
    static_assert(sizeof(int)==4);
    // only compiles if condition is true...
}
```

A menos que voce esteja convencido de que seu codigo nunca precisara ser portado para outra plataforma, esforce-se para depender o menos possivel de comportamento definido pela implementacao e, se o fizer, certifique-se de validar (por meio de `static_assert` se possivel, em tempo de execucao se nao houver outra opcao) e documentar essa situacao. Isso pode ajuda-lo a evitar algumas surpresas desagradaveis no futuro.

### Comportamento nao especificado (nao documentado)

Enquanto o comportamento definido pela implementacao e nao portatil, mas documentado para uma determinada plataforma, o *unspecified behavior* (comportamento nao especificado) e um comportamento que, mesmo para um programa bem-formado com dados corretos, se comporta de uma forma que depende da implementacao, mas nao precisa ser documentado.

Alguns casos de comportamento nao especificado incluem o estado de um objeto apos ser movido (*moved-from object*, que se diz ser valido, mas nao especificado — portanto, mais um estado nao especificado do que um comportamento nao especificado), a ordem de avaliacao de subexpressoes em uma chamada de funcao, ou seja, se `f(g(),h())` avaliara `g()` ou `h()` primeiro, os valores em um bloco de memoria recentemente alocado, e assim por diante. Este ultimo exemplo e interessante para nosso estudo; uma *debug build* (compilacao de depuracao) pode preencher blocos de memoria recentemente alocados com um padrao de bits reconhecivel para ajudar no processo de depuracao, e uma compilacao otimizada com o mesmo conjunto de ferramentas poderia deixar os bits iniciais de um bloco de memoria recentemente alocado "nao inicializados", com os bits que ele continha no momento em que a alocacao foi realizada, para obter melhorias de velocidade.

### A ODR

A *One Definition Rule* (ODR — regra de uma unica definicao), simplificando, diz que deve haver apenas uma definicao de cada "coisa" (funcao, objeto em um escopo, enumeracao, template, etc.) em uma *translation unit*, embora possa haver multiplas declaracoes dessa coisa. Considere o seguinte exemplo:

```cpp
int f(int); // declaration
int f(int n); // Ok, declaration again
int f(int m) { return m; } // Ok, definition
// int f(int) { return 3; } // not Ok (ODR violation)
```

Em C++, evitar violacoes da ODR e importante, pois essas "maldades" podem escapar do escrutinio do compilador e cair no reino das situacoes IFNDR. Por exemplo, devido a compilacao separada dos arquivos-fonte, um arquivo de cabecalho contendo a definicao de uma funcao nao-inline levara a que essa definicao seja replicada em cada arquivo-fonte que incluir esse mesmo cabecalho. Entao, cada compilacao pode ser bem-sucedida, e o fato de haver multiplas definicoes dessa funcao no mesmo *build* pode ser detectado mais tarde (em tempo de *link*) ou simplesmente nao ser detectado e causar caos.

### Comportamento erroneo

Esforcos em andamento relacionados a seguranca em C++ levaram a discussoes sobre um novo tipo de "mal" tentativamente chamado de *erroneous behavior* (comportamento erroneo). Essa nova categoria tem o objetivo de cobrir situacoes que poderiam ter sido UB no passado, mas para as quais podemos emitir diagnosticos e fornecer comportamento bem-definido. O comportamento ainda seria incorreto, mas o comportamento erroneo forneceria, de certa forma, limites para as consequencias. Observe que o trabalho sobre comportamento erroneo esta em andamento no momento da escrita deste livro, e essa nova funcionalidade de linguagem pode ter como alvo o C++26.

Um caso de uso previsto para o comportamento erroneo e a leitura de uma variavel nao inicializada, onde a implementacao poderia (por razoes de seguranca) fornecer um valor fixo para os bits lidos, e o erro conceitual que surge da leitura dessa variavel seria algo que as implementacoes sao encorajadas a diagnosticar. Outro caso de uso seria esquecer de retornar um valor de um operador de atribuicao nao-void.

Agora que vimos as grandes "familias" de inconveniencias que podem atingir nossos programas se nao nos comportarmos, vamos nos aprofundar em algumas das principais facilidades que podem nos causar problemas e ver o que devemos evitar.

## Ponteiros

O Capitulo 1 examinou os ponteiros em C++ no sentido do que eles representam e o que significam. Descreveu o que e a aritmetica de ponteiros e o que ela nos permite fazer. Agora examinaremos usos praticos da aritmetica de ponteiros, com usos tanto apropriados quanto inapropriados dessa ferramenta de baixo nivel (mas as vezes preciosa).

### Usos da aritmetica de ponteiros dentro de um array

A aritmetica de ponteiros e uma ferramenta boa e util, mas e afiada e tende a ser mal utilizada. Com arrays nativos, os dois loops a seguir, rotulados A e B, se comportam exatamente da mesma forma:

```cpp
void f(int);
int main() {
    int vals[]{ 2,3,5,7,11 };
    enum { N = sizeof vals / sizeof vals[0] };
    for(int i = 0; i != N; ++i) // A
      f(vals[i]);
    for(int *p = vals; p != vals + N; ++p) // B
      f(*p);
}
```

Voce pode se perguntar sobre a parte `vals + N` no loop B, mas e codigo C++ valido (e idiomatico). Voce pode observar o ponteiro logo apos o final do array, mesmo que nao seja permitido observar para o que ele aponta; o padrao garante que esse endereco especifico *one-past-the-end* (um alem do fim) seja acessivel ao seu programa. No entanto, nenhuma garantia semelhante e fornecida para o endereco seguinte, portanto, tenha cuidado!

Enquanto voce respeitar as regras, pode usar ponteiros para saltar para frente e para tras dentro de um array. Se voce exagerar e usar um ponteiro para ir alem de um *past-the-end*, acabara em territorio de UB; ou seja, pode estar tentando acessar um endereco que nao esta no espaco de enderecamento do seu processo:

```cpp
int arr[10]{ }; // all elements initialized to zero
int *p = &arr[3];
p += 4; assert(p == &arr[7]);
--p;    assert(p == &arr[6]);
p += 4; // still Ok as long as you don't try to access *p
++p; // UB, not guaranteed to be valid
```

### Interconvertibilidade de ponteiros

O padrao C++ define o que significa um objeto ser *pointer-interconvertible* (interconvertivel por ponteiro) com outro. A interconvertibilidade de ponteiros significa que se pode usar um ponteiro para um como ponteiro para o outro, normalmente por meio de `reinterpret_cast` (expandiremos isso no Capitulo 3), pois eles tem o mesmo endereco. Em termos gerais, os seguintes pontos sao verdadeiros:

- Um objeto e interconvertivel por ponteiro consigo mesmo.
- Uma `union` e interconvertivel por ponteiro com seus membros de dados, bem como com o primeiro membro de dados deles se forem tipos compostos.
- Com algumas restricoes, `x` e `y` sao interconvertiveis por ponteiro um com o outro se um e um objeto e o outro e do mesmo tipo que o primeiro membro de dados nao estatico desse objeto.

Alguns exemplos estao incluidos aqui:

```cpp
struct X { int n; };
struct Y : X {};
union U { X x; short s; };
int main() {
    X x;
    Y y;
    U u;
    // x is pointer-interconvertible with x
    // u is pointer-interconvertible with u.x
    // u is pointer-interconvertible with u.s
    // y is pointer-interconvertible with y.x
}
```

Se voce tentar aplicar `reinterpret_cast` de uma forma que nao respeite as regras de interconvertibilidade de ponteiros, seu codigo e tecnicamente incorreto e nao tem garantia de funcionar na pratica. Nao faca isso.

Ocasionalmente, usaremos a propriedade de interconvertibilidade de ponteiros em nossos exemplos de codigo, inclusive na proxima secao.

### Usos da aritmetica de ponteiros dentro de um objeto

A aritmetica de ponteiros dentro de um objeto tambem e permitida em C++, embora se deva ter cuidado com a forma como isso e tratado (usando os *casts* apropriados, que exploraremos no Capitulo 3, e garantindo que se realize a aritmetica de ponteiros adequadamente).

Por exemplo, o seguinte codigo e correto, embora nao seja algo que se deva buscar fazer (nao faz sentido e faz as coisas de formas desnecessariamente complicadas, mas e legal e nao causa danos):

```cpp
struct A {
    int a;
    short s;
};
short * f(A &a) {
    // pointer interconvertibility in action!
    int *p = reinterpret_cast<int*>(&a);
    p++;
    return reinterpret_cast<short*>(p); // Ok, within the
                                       // same object
}
int main() {
    A a;
    short *p = f(a);
    *p = 3; // fine, technically
}
```

Nao abusaremos desse aspecto da linguagem C++ neste livro, mas precisamos estar cientes dele para escrever codigo correto de baixo nivel.

> **Sobre a diferenca entre ponteiro e endereco**
>
> Para fortalecer a seguranca de hardware e software, houve trabalho em arquiteturas de hardware que podem fornecer uma forma de "marcacao de ponteiros" (*pointer tagging*), o que permite o rastreamento de hardware da proveniencia de ponteiros, entre outras coisas. Dois exemplos bem conhecidos sao a arquitetura CHERI e as *Memory Tagging Extensions* (MTEs).
>
> Para se beneficiar de tal hardware, a linguagem precisa distinguir entre a ideia de baixo nivel de enderecos e a ideia de alto nivel de ponteiros, pois o ultimo poderia levar em conta o fato de que um ponteiro e mais do que apenas um local de memoria. Se seu codigo absolutamente precisa comparar ponteiros nao relacionados para ordenacao, uma coisa que voce pode fazer e converter os ponteiros para `std::intptr_t` ou para `std::uintptr_t` e comparar os resultados (numericos) em vez de comparar os ponteiros reais. Observe que o suporte do compilador para esses dois tipos e opcional, embora todos os principais fornecedores de compiladores o ofereçam.

### O ponteiro nulo

A ideia de um ponteiro nulo como um valor reconhecivel para ponteiros que nao levam a lugar valido algum pode ser rastreada ate C.A.R. Hoare. Na linguagem C, por meio da macro `NULL`, foi representado primeiro como um `char*` de valor 0, depois como um `void*` de valor 0, e depois em C++ como simplesmente o valor 0, uma vez que coisas como `int *p = NULL;` com um `NULL` tipado eram C legal, mas nao C++ legal. Isso ocorre porque o sistema de tipos e mais restrito em C++. Observe que um ponteiro com valor 0 nao significa "apontar para o endereco zero", pois esse endereco em si e perfeitamente valido e e usado como tal em muitas plataformas.

Em C++, a forma preferida de expressar um ponteiro nulo e `nullptr`, um objeto do tipo `std::nullptr_t` que converte para ponteiros de qualquer tipo e se comporta como esperado. Isso resolve alguns problemas antigos com o literal `0` em C++, como o seguinte:

```cpp
int f(int); //#0
int f(char*); // #1
int main() {
    int n = 3;
    char c;
    f(n); // calls #0
    f(&c); // calls #1
    f(0); // ambiguous before C++11, calls #0 since
    f(nullptr); // only since C++11; unambiguously calls #1
}
```

Observe que `nullptr` nao e um ponteiro; e um objeto que converte implicitamente para um ponteiro. Por essa razao, a *trait* `std::is_pointer_v<nullptr>` e `false`, e o C++ oferece uma *trait* distinta chamada `std::is_null_pointer<T>` para testar estaticamente se `T` e um `std::nullptr_t` ou nao (levando em conta `const` e `volatile`).

Desreferenciar um ponteiro nulo e UB, assim como desreferenciar um ponteiro nao inicializado. O ponto de usar `nullptr` em seu codigo e tornar esse estado reconhecivel: `nullptr` e um valor distinguivel, enquanto um ponteiro nao inicializado poderia ser qualquer coisa.

Em C++ (ao contrario de C), a aritmetica em um ponteiro nulo e bem-definida... contanto que voce adicione zero ao ponteiro nulo. Ou, para dizer de outra forma: se voce adicionar zero a um ponteiro nulo, o codigo permanece bem-definido, mas se voce adicionar qualquer outra coisa, esta por conta propria. Isso significa que o seguinte e correto — no caso de um `Array` vazio, `begin()` retorna `nullptr` e `size()` retorna zero, portanto `end()` efetivamente computa `nullptr+0`, o que respeita as regras:

```cpp
template <class T> class Array {
    T *elems = nullptr; // pointer to the beginning
    std::size_t nelems = 0; // number of elements
public:
    Array() = default; // =empty array
    // ...
    auto size() const noexcept { return nelems; }
    // note: could return nullptr
    auto begin() noexcept { return elems; }
    auto end() noexcept { return begin() + size(); }
};
```

Voltaremos a este exemplo de `Array` com mais detalhes nos Capitulos 12, 13 e 14; ele nos ajudara a discutir varios aspectos importantes de tecnicas eficientes de gerenciamento de memoria. Por ora, vamos olhar para outra fonte de manobras de programacao arriscadas.

## Type punning

Outra area onde um programador C++ pode ter problemas e o *type punning*. Por *type punning* entendemos tecnicas que subvertem de alguma forma o sistema de tipos da linguagem. A ferramenta consagrada para realizar conversoes de tipo sao os *casts*, pois sao explicitos no texto do codigo-fonte e (exceto pelos *casts* estilo C) expressam a intencao da conversao — mas esse topico merece seu proprio capitulo (o Capitulo 3, se estiver se perguntando).

Nesta secao, examinaremos outras formas de realizar *type punning*, incluindo tanto as recomendaveis quanto as que voce deve buscar evitar.

### Type punning por meio de membros de uma union

Uma `union` e um tipo para o qual os membros estao todos no mesmo endereco. O tamanho de uma `union` e o tamanho de seu maior membro, e o alinhamento de uma `union` e o alinhamento mais estrito de seus membros. Considere o seguinte exemplo:

```cpp
struct X {
    char c[5]; short s;
} x;
// one byte of padding between x.c and x.s
static_assert(sizeof x.s == 2 && sizeof x == 8);
static_assert(alignof(x) == alignof(short));
union U {
    int n; X x;
} u;
static_assert(sizeof u == sizeof u.x);
static_assert(alignof(u) == alignof(u.n));
int main() {}
```

E tentador pensar que se pode usar uma `union` para converter implicitamente, por exemplo, um numero de ponto flutuante de quatro bytes em um numero inteiro de quatro bytes, e na linguagem C (nao em C++), isso e de fato possivel.

Embora haja uma crença generalizada de que essa pratica e legal em C++, a realidade e que nao e (com uma ressalva especial, que exploraremos em breve). De fato, em C++, o ultimo membro de uma `union` em que se escreveu e chamado de membro ativo da `union`, e esse membro e o unico do qual seu codigo tem permissao para ler. Assim, o seguinte codigo e ilegal, pois ler de um membro nao ativo de uma `union` e UB, e UB nao e permitido em uma funcao `constexpr`:

```cpp
union U {
    float f;
    int n;
};
constexpr int f() {
    U u{ 1.5f };
    return u.n; // UB (u.f is the active member)
}
int main() {
    // constexpr auto r0 = f(); // would not compile
    auto r1 = f(); // compiles, as not a constexpr
                  // context, but still UB
}
```

Como voce pode saber, uma funcao `constexpr` como `f()` no exemplo acima nao pode conter codigo que seja UB se chamada em um contexto `constexpr`. Isso as vezes a torna uma ferramenta interessante para provar um ponto.

Existe uma ressalva em relacao as conversoes entre membros de `union`, e essa ressalva esta associada a *common initial sequence* (sequencia inicial comum).

**Sequencia inicial comum**

Conforme explicado em `wg21.link/class.mem.general#23`, a *common initial sequence* (sequencia inicial comum) de duas `struct`s e composta pelos membros iniciais dessas duas `struct`s que possuem tipos *layout-compatible* (compativeis em layout) correspondentes. Por exemplo, a sequencia inicial comum de `A` e `B` e composta por seus dois primeiros membros (`int` e `const int` sao layout-compativeis e `float` e `volatile float` sao layout-compativeis):

```cpp
struct A { int n; float f; char c; };
struct B{ const int b0; volatile float x; };
```

Com uma `union`, e possivel ler de um membro nao ativo se o valor lido faz parte tanto da sequencia inicial comum desse membro quanto do membro ativo. Eis um exemplo:

```cpp
struct A { int n0; char c0; };
struct B { int n1; char c1; float x; };
union U {
    A a;
    B b;
};
int f() {
    U u{ { 1, '2' } }; // initializes u.a
    return u.b.n1; // not UB
}
int main() {
    return f(); // Ok
}
```

Observe que tal *type punning* deve ser mantido ao minimo, pois pode dificultar o raciocinio sobre o codigo-fonte, mas pode ser bastante util. Por exemplo, pode ser usado para implementar algumas representacoes subjacentes interessantes para classes que podem ter duas representacoes distintas (classes como `optional` ou `string`), tornando mais facil alternar de uma para a outra. Algumas otimizacoes uteis podem ser construidas sobre isso.

### Os tipos intptr_t e uintptr_t

Como mencionado anteriormente neste capitulo, nao se pode comparar diretamente ponteiros para locais arbitrarios na memoria de maneira bem-definida em C++. Pode-se, no entanto, comparar os valores inteiros associados a ponteiros de maneira bem-definida, como aqui:

```cpp
#include <iostream>
#include <cstdint>
int main() {
    using namespace std;
    int m, n;
    // simply comparing &m with &n is not allowed
    if(reinterpret_cast<intptr_t>(&m) <
      reinterpret_cast<intptr_t>(&n))
      cout << "m precedes n in address order\n";
    else
      cout << "n precedes m in address order\n";
}
```

Os tipos `std::intptr_t` e `std::uintptr_t` sao *aliases* para tipos inteiros grandes o suficiente para conter um endereco. Use o tipo com sinal, `intptr_t`, para operacoes que podem levar a valores negativos (por exemplo, subtracao).

### A funcao std::memcpy()

Por razoes historicas (e de compatibilidade com C), `std::memcpy()` e especial, pois pode iniciar o *lifetime* de um objeto se usada adequadamente. Um uso incorreto de `std::memcpy()` para *type punning* seria o seguinte:

```cpp
// suppose this holds for this example
static_assert(sizeof(int) == sizeof(float));
#include <cassert>
#include <cstdlib>
#include <cstring>
int main() {
    float f = 1.5f;
    void *p = malloc(sizeof f);
    assert(p);
    int *q = std::memcpy(p, &f, sizeof f);
    int value = *q; // UB
    //
}
```

A razao pela qual isso e ilegal e que a chamada a `std::memcpy()` copia um objeto `float` para o armazenamento apontado por `p`, efetivamente iniciando o *lifetime* de um objeto `float` nesse armazenamento. Como `q` e um `int*`, desreferencia-lo e UB.

Por outro lado, o seguinte e legal e mostra como `std::memcpy()` pode ser usada para *type punning*:

```cpp
// suppose this holds for this example
static_assert(sizeof(int) == sizeof(float));
#include <cassert>
#include <cstring>
int main() {
    float f = 1.5f;
    int value;
    std::memcpy(&value, &f, sizeof f); // Ok
    // ...
}
```

De fato, neste segundo exemplo, usar `std::memcpy()` para copiar os bits de `f` para `value` inicia o *lifetime* de `value`. Esse objeto pode ser usado como qualquer outro `int` a partir desse ponto.

### Os casos especiais de char*, unsigned char* e std::byte*

Os tipos `char*`, `unsigned char*` (nao `signed char*`) e `std::byte*` tem status especial em C++, pois podem literalmente apontar para qualquer lugar e fazer *alias* de qualquer coisa (`wg21.link/basic.lval#11`). Por essa razao, se voce precisar acessar os bytes subjacentes da representacao de valor de um objeto, esses tipos sao uma ferramenta importante em sua caixa de ferramentas.

Mais adiante neste livro, recorreremos ocasionalmente a esses tipos para realizar manipulacao de bytes de baixo nivel. Observe que tais manobras sao inerentemente frageis e nao portateis, ja que detalhes como a ordem dos bytes em um inteiro podem variar de plataforma para plataforma. Use essas facilidades de baixo nivel com cuidado.

### A funcao std::start_lifetime_as<T>()

Um ultimo conjunto de facilidades para este capitulo e `std::start_lifetime_as<T>()` e `std::start_lifetime_as_array<T>()`. Essas funcoes foram discutidas por anos, mas ganharam sua forma definitiva com o C++23. Seu papel e receber como argumento algo como um buffer de bytes de memoria bruta e retornar um ponteiro para algum `T` (apontando para esse buffer) cujo *lifetime* foi iniciado, de modo que o *pointee* possa ser usado como tal a partir desse ponto:

```cpp
static_assert(sizeof(short) == 2);
#include <memory>
int main() {
    char buf[]{ 0x00, 0x01, 0x02, 0x03 };
    short* p = std::start_lifetime_as<short>(buf);
    // use *p as a short
}
```

Este e, novamente, um recurso de baixo nivel a ser usado com cuidado. A intencao aqui e ser capaz de implementar coisas como codigo de E/S de arquivo de baixo nivel e codigo de rede (por exemplo, receber um pacote UDP e tratar sua representacao de valor como se fosse um objeto existente) em C++ puro sem cair em uma armadilha de UB. Discutiremos essas funcoes com mais detalhes no Capitulo 15.

## Resumo

Este capitulo explorou algumas facilidades de baixo nivel e as vezes desagradaveis que usaremos ocasionalmente, a fim de colocar os devidos "sinais de aviso" e nos lembrar de que temos de ser responsaveis e escrever codigo sensato e correto, mesmo que a linguagem de nossa escolha ofereça liberdade significativa.

Ao escrever facilidades avancadas de gerenciamento de memoria nos capitulos posteriores deste livro, essas facilidades perigosas as vezes nos serao uteis. Inspirados pelo conteudo deste capitulo sobre coisas com as quais devemos ter cuidado, recorreremos a essas facilidades com parcimonia, cuidado e de formas que tornem dificil usa-las incorretamente.

Em nosso proximo capitulo, examinaremos os principais *casts* do C++ colocados a nossa disposicao; a intencao e nos tornarmos conscientes do que cada *cast* faz, bem como quando (e para que fim) deve ser usado, de modo que possamos, a partir dai, construir as poderosas abstracoes de gerenciamento de memoria que queremos usar.
