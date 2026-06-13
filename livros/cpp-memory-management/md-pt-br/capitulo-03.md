# Capitulo 3 - Casts e Qualificacoes cv (Casts and cv-qualifications)

> Traducao nao oficial do livro *C++ Memory Management* (Patrice Roy, 2025).
> Texto adaptado para portugues brasileiro com fins educacionais. Paginas originais: 47-58.
> Repositorio: [ring0-study](https://github.com/cnthigu/ring0-study)

> Navegacao: [Anterior](capitulo-02.md) | [Indice](README.md) | [Proximo](capitulo-04.md)

---

Estamos progredindo. No Capitulo 1, vimos o que sao memoria, objetos e ponteiros, pois sabemos que precisamos entender essas ideias basicas se quisermos assumir o controle dos mecanismos de gerenciamento de memoria. Em seguida, no Capitulo 2, vimos algumas construcoes de baixo nivel que podem nos causar problemas se mal utilizadas — essenciais para entender em certas situacoes para assumir o controle de como nossos programas gerenciam memoria. E uma forma um tanto seca de comecar, mas isso tambem significa que as partes divertidas do nosso trabalho ainda estao por vir. Espero que isso seja encorajador!

No final do Capitulo 2, examinamos abordagens para *type punning*, uma forma de subverter o sistema de tipos, incluindo algumas que se acredita funcionarem, mas que na verdade nao funcionam. O C++ oferece varias maneiras controladas e explicitas de interagir com o sistema de tipos, informando ao compilador que ele deve ver o tipo de uma expressao como algo diferente do que ele poderia inferir do codigo-fonte. Essas ferramentas, os *type casts* (ou simplesmente *casts*), sao o assunto deste capitulo.

Primeiro examinaremos o que sao *casts* no sentido geral, distinguindo as varias razoes fundamentais para realizar *casts* e mostrando por que os *casts* estilo C sao, na maioria das vezes, inadequados (exceto para alguns casos especificos) em um programa C++. Em seguida, daremos uma olhada rapida em um aspecto de seguranca do sistema C++, as qualificacoes `cv`, e discutiremos o papel dos qualificadores `cv` na higiene e qualidade geral do codigo C++. Depois disso, examinaremos os seis *casts* C++ a nossa disposicao. Finalmente, voltaremos aos *casts* C para mostrar as situacoes limitadas em que ainda podem ser adequados.

Neste capitulo, aprenderemos o seguinte:

- O que sao *casts* e o que significam em um programa
- O que sao qualificacoes `cv` e como elas interagem com *casts*
- Quais sao os *casts* C++, incluindo o *cast* C, e quando devem ser usados

## Requisitos tecnicos

Voce pode encontrar os arquivos de codigo deste capitulo no repositorio GitHub do livro em: [https://github.com/PacktPublishing/C-Plus-Plus-Memory-Management/tree/main/chapter3](https://github.com/PacktPublishing/C-Plus-Plus-Memory-Management/tree/main/chapter3)

## O que e um cast?

Voce usara um *cast* para ajustar a visao do compilador sobre o tipo de uma expressao. O problema e que o compilador ve nosso codigo-fonte e entende o que escrevemos e o que o codigo de outras pessoas expressa. Na maioria das vezes (esperancosamente), esse codigo fara sentido, e o compilador traduzira suas fontes em binarios corretos sem reclamar.

As vezes, claro, havera discrepancias (esperancosamente temporarias) entre a intencao do programador e o codigo, conforme expresso nas fontes vistas pelo compilador. Na maioria das vezes, o compilador estara certo, e o programador reescreverá o codigo-fonte, pelo menos em parte, para melhor expressar a intencao, inspirado pelas mensagens de erro ou aviso que revelaram (a seu modo poetico) um problema. As vezes, claro, o codigo-fonte corresponde a intencao do programador, mas ainda ha discordancias com o compilador e ajustes necessarios para chegar a um acordo com ele. Por exemplo, suponha que um programador queira alocar um buffer grande o suficiente para armazenar muitos inteiros (*lots* sendo um valor que e grande demais para usar razoavelmente a pilha ou um valor nao conhecido em tempo de compilacao); uma maneira (de baixo nivel e propensa a erros, mas legal assim mesmo) de conseguir isso seria chamar a funcao `std::malloc()`:

```cpp
// ...
int *p = std::malloc(lots * sizeof(int)); // <-- HERE
if(p) {
    // use p as an array of int objects
    std::free(p);
}
// ...
```

Este trecho de codigo, como voce pode saber, nao e C++ valido — `std::malloc()` retorna `void*` (um ponteiro para um bloco de memoria bruta de pelo menos o tamanho solicitado, ou `nullptr` se a alocacao falhou), e `void*` nao e implicitamente convertivel para `int*` em C++ (o inverso e, claro, verdadeiro — `int*` e de fato implicitamente convertivel para `void*`).

Observe que poderíamos ter substituido `std::malloc(lots*sizeof(int))` por `new int[lots]` neste caso (simplificado demais), mas as coisas nem sempre sao tao simples, e as vezes precisamos mentir para o sistema de tipos, mesmo que por um momento. E e ai que os *casts* entram.

Entao, o que sao *casts*? *Casts* sao uma maneira controlada de guiar o sistema de tipos do compilador na compreensao da intencao do programador. Os *casts* tambem fornecem informacoes no codigo-fonte sobre as razoes por tras dessas mentiras temporarias; eles documentam o que o programador pretendia fazer exatamente no momento em que uma mentira era necessaria. Os *casts* C++ sao muito claros na intencao que transmitem e muito precisos em seus efeitos; o *cast* estilo C (tambem visto em outras linguagens) e muito mais vago em questoes de intencao, como veremos mais adiante neste capitulo, e pode realizar transformacoes inadequadas em uma linguagem com um sistema de tipos tao rico quanto o C++.

## Seguranca no sistema de tipos — qualificacoes cv

O C++ fornece dois qualificadores relacionados a seguranca em seu sistema de tipos. Eles se chamam `const` e `volatile`, e estao relacionados de varias maneiras.

O qualificador `const` significa que o objeto assim qualificado e considerado imutavel no escopo atual, como o seguinte:

```cpp
const int N = 3; // global constant
class X {
    int n; // note: not const
public:
    X(int n) : n{ n } {
    }
    int g() { // note: not const
      return n += N; // thus, n's state can be mutated
    }
    int f() const { // const applies to this, and
                    // transitively to its members
      // return g(); // illegal as g() is not const
      return n + 1;
    }
};
int f(const int &n) { // f() will not mutate argument n
    return X{ n }.f() + 1; // X::X(int) takes its argument
                          // by value so n remains intact
}
int main() {
    int a = 4;
    a = f(a); // a is not const in main()
}
```

Marcar um objeto como `const` significa que, no contexto em que e marcado como tal, ele nao pode ser mutado. No caso de membros de classe, a garantia `const` e mantida transitivamente por meio de funcoes membro `const`, no sentido de que uma funcao membro `const` nao pode modificar os membros de `*this`, nem pode chamar uma funcao membro nao-`const` do mesmo objeto. No exemplo anterior, `X::f` e `const` e, como tal, nao poderia chamar `X::g`, que nao oferece essa garantia; permitir que `X::f` chame `X::g` quebraria efetivamente a garantia `const`, pois `X::g` pode mutar `*this`, mas `X::f` nao pode.

O qualificador `const` e bem conhecido e bem documentado em C++. Ser "*const-correct*" (correto em relacao a `const`) e geralmente visto como boa higiene de codigo e e algo que voce deve buscar fazer na pratica; usar `const` onde quer que faca sentido e um dos pontos fortes mais importantes da linguagem C++, e muitas linguagens que afirmam ser "seguras em relacao a tipos" carecem desse recurso essencial, sem o qual a corretude e muito mais dificil de alcanzar.

A palavra-chave `volatile` e a contraparte de `const`; por isso, o termo *cv-qualifier* (qualificador cv) se refere a ambos os termos. Lamentavelmente mal definido no padrao, `volatile` tem alguns significados.

Quando aplicado a um tipo fundamental (por exemplo, `volatile int`), significa que o objeto assim qualificado pode ser acessado por meios desconhecidos pelo compilador e nao necessariamente visiveis no codigo-fonte. Como tal, esse termo e mais util ao escrever *device drivers* (drivers de dispositivo), onde alguma acao externa ao proprio programa (como a pressao fisica de uma tecla) poderia mudar a memoria associada ao objeto, ou quando algum componente de hardware ou software externo ao codigo-fonte poderia observar mudancas no estado desse objeto.

Informalmente, se o codigo-fonte disser "Por favor, leia o valor daquele objeto `volatile`", o codigo que sera gerado deve ler esse valor mesmo que o programa nao parecal modifica-lo de forma alguma; da mesma forma, se o codigo-fonte disser "Por favor, escreva naquele objeto `volatile`", entao uma escrita naquele local de memoria deve ocorrer, mesmo que nada no programa parecal ler daquele local de memoria posteriormente. Assim, `volatile` pode ser visto como algo que impede otimizacoes que o compilador de outra forma seria autorizado a realizar.

Na maquina abstrata do C++, acessar um objeto qualificado como `volatile` e o equivalente moral de uma operacao de E/S — ele pode mudar o estado do programa. Em um objeto de algum tipo de classe, `volatile` pode ser aplicado a uma funcao membro assim como `const` pode. De fato, uma funcao membro nao estatica pode ser qualificada como `const`, `volatile`, `const volatile`, ou nenhum desses (entre outras coisas).

O significado de aplicar o qualificador `const` em uma funcao membro foi descrito anteriormente com a funcao membro `X::f` — `*this` e `const`; seus membros de dados nao mutaveis e nao estaticos sao `const` nessa funcao, e as unicas funcoes membro nao estaticas que podem ser chamadas por meio de `*this` sao aquelas qualificadas como `const`. Uma funcao membro nao estatica qualificada como `volatile` e, da mesma forma, bastante similar — `*this` e `volatile` durante a execucao dessa funcao, e o mesmo vale para todos os seus membros, o que impacta quais operacoes voce pode realizar com esses objetos. Por exemplo, obter o endereco de `volatile int` retorna `volatile int*`, que nao e implicitamente convertivel para `int*`, pois a conversao descartaria algumas garantias de seguranca. Esta e uma das razoes pelas quais temos *casts*.

## Os casts C++

Tradicionalmente, o C++ suportou quatro maneiras de realizar essas conversoes de tipo explicitas que chamamos de *casts* — `static_cast`, `dynamic_cast`, `const_cast` e `reinterpret_cast`. O C++11 adicionou um quinto, `duration_cast`, que esta tangencialmente relacionado a este livro, mas aparecera ocasionalmente em exemplos, particularmente quando medimos o tempo de execucao de uma funcao. Por fim, o C++20 introduziu um sexto caso, `bit_cast`, que e de interesse para nosso trabalho neste livro.

As secoes a seguir oferecem uma breve visao geral de cada *cast* C++, juntamente com alguns exemplos de como e quando eles podem ser uteis.

### Seu melhor amigo (na maioria das vezes) — static_cast

A melhor e mais eficiente ferramenta em nosso conjunto de ferramentas de *type-casting* e `static_cast`. E essencialmente seguro, nao tem custo praticamente nenhum na maioria dos casos e pode ser usado em um contexto `constexpr`, o que o torna amenavel a manobras em tempo de compilacao.

Voce pode usar `static_cast` em situacoes que envolvam riscos potenciais, como converter um `int` para um `float` ou vice-versa. Neste ultimo caso, ele reconhece explicitamente a perda da parte decimal. Voce tambem pode usar `static_cast` para converter um ponteiro ou uma referencia de uma classe derivada para uma de suas bases diretas ou indiretas (desde que nao haja ambiguidade), o que e totalmente seguro e poderia ser feito implicitamente, bem como de uma base para uma de suas classes derivadas. Converter de uma classe base para uma classe derivada usando `static_cast` e altamente eficiente, mas extremamente arriscado se o *cast* for incorreto, pois nao realiza verificacoes em tempo de execucao.

Aqui estao alguns exemplos:

```cpp
struct B { virtual ~B() = default; /* ... */ };
struct D0 : B { /* ... */ };
struct D1 : B { /* ... */ };
class X {
public:
    X(int, double);
};
void f(D0&);
void f(D1*);
int main() {
    const float x = 3.14159f;
    int n = static_cast<int>(x); // Ok, no warning
    X x0{ 3, 3.5 }; // Ok
    // compiles, probably warns (narrowing conversion)
    X x1(3.5,0);
    // does not compile, narrowing not allowed with braces
    // X x2{ 3.5, 0 };
    X x3{ static_cast<int>(x), 3 }; // Ok
    D0 d0;
    // illegal, no base-derived relationship with D0 and D1
    // D1* d1 = static_cast<D1*>(&d0);
    // Ok, static_cast could be omitted
    B *b = static_cast<B*>(&d0);
    // f(*b); // illegal
    f(*static_cast<D0*>(b)); // Ok
    f(static_cast<D1*>(b)); // compiles but very dangerous!
}
```

Preste especial atencao ao ultimo uso de `static_cast` no exemplo anterior — converter de uma classe base para uma de suas classes derivadas e adequadamente feito com `static_cast`. No entanto, voce deve garantir que a conversao leve a um objeto do tipo escolhido, pois nenhuma verificacao em tempo de execucao e feita da validade dessa conversao; como o nome implica, apenas verificacoes em tempo de compilacao sao feitas com esse *cast*. Se voce nao tem certeza do que esta fazendo com um *downcast*, esta nao e a ferramenta certa para voce.

`static_cast` nao apenas muda a perspectiva do compilador sobre o tipo de uma expressao; ele tambem pode ajustar o endereco de memoria que esta sendo acessado para levar em conta os tipos envolvidos na conversao. Por exemplo, quando uma classe `D` tem pelo menos duas classes base nao vazias, `B0` e `B1`, essas duas partes da classe derivada nao estao no mesmo endereco dentro de um objeto `D` (se estivessem, se sobreporiam!), portanto, `static_cast` de `D*` para uma de suas bases pode retornar um endereco diferente do proprio `D*`. Voltaremos a isso ao discutir `reinterpret_cast`, para o qual o comportamento e diferente (e mais perigoso).

### Um sinal de que algo esta errado — dynamic_cast

Havera casos em que voce tem um ponteiro ou uma referencia para um objeto de algum tipo de classe, e esse tipo acontece de ser diferente (mas relacionado) ao tipo necessario. Isso acontece frequentemente — por exemplo, em *game engines* onde a maioria das classes deriva de alguma base `Component` e as funcoes tendem a receber argumentos `Component*`, mas precisam acessar membros de um objeto da classe derivada que esperam.

O principal problema aqui e, tipicamente, que a interface da funcao esta errada — ela aceita argumentos de tipos insuficientemente precisos. Ainda assim, todos nos temos software para entregar, e as vezes precisamos fazer as coisas funcionarem, mesmo que tenhamos feito algumas escolhas ao longo do caminho que provavelmente vamos querer revisitar mais tarde.

A forma segura de realizar tais *casts* e `dynamic_cast`. Esse *cast* permite converter um ponteiro ou uma referencia de um tipo para outro tipo relacionado de uma forma que permite testar se a conversao funcionou ou nao; com ponteiros, uma conversao incorreta retorna `nullptr`, enquanto com referencias, uma conversao incorreta lanca `std::bad_cast`. A relacao de tipos com `dynamic_cast` nao se limita a relacionamentos base-derivado e inclui a conversao de uma base para outra base em um design de heranca multipla. No entanto, observe que, na maioria dos casos, `dynamic_cast` requer que a expressao convertida para outro tipo seja de tipo polimorfico, no sentido de que deve ter pelo menos uma funcao membro virtual.

Aqui estao alguns exemplos:

```cpp
struct B0 {
    virtual int f() const = 0;
    virtual ~B0() = default;
};
struct B1 {
    virtual int g() const = 0;
    virtual ~B1() = default;
};
class D0 : public B0 {
    public: int f() const override { return 3; }
};
class D1 : public B1 {
    public: int g() const override { return 4; }
};
class D : public D0, public D1 {};
int f(D *p) {
    return p? p->f() + p->g() : -1; // Ok
}
// g has the wrong interface: it accepts a D0& but
// tries to use it as a D1&, which makes sense if
// the referred object is publicly D0 and D1 (for
// example, class D
int g(D0 &d0) {
    D1 &d1 = dynamic_cast<D1&>(d0); // throws if wrong
    return d1.g();
}
#include <iostream>
int main() {
    D d;
    f(&d); // Ok
    g(d); // Ok, a D is a D0
    D0 d0;
    // calls f(nullptr) as &d0 does not point to a D
    std::cout << f(dynamic_cast<D*>(&d0)) << '\n'; // -1
    try {
      g(d0); // compiles but will throw bad_cast
    } catch(std::bad_cast&) {
      std::cerr << "Nice try\n";
    }
}
```

Observe que, embora este exemplo exiba uma mensagem quando `std::bad_cast` e lancado, isso nao e de forma alguma o que poderíamos chamar de tratamento de excecoes; nao resolvemos o "problema", e a execucao do codigo continua em um estado potencialmente corrompido, o que poderia piorar as coisas em codigo mais serio. Em um exemplo de brinquedo como este, simplesmente deixar o codigo falhar e parar de executar tambem teria sido uma escolha razoavel.

Na pratica, o uso de `dynamic_cast` deve ser raro, pois tende a ser um sinal de que escolhemos nossas interfaces de funcoes de forma aperfeicoavel. Observe que `dynamic_cast` requer que os binarios sejam compilados com informacoes de tipo em tempo de execucao (RTTI — *Runtime Type Information*), levando a binarios maiores. Compreensivelmente, devido a esses custos, alguns dominios de aplicacao tendem a evitar esse *cast*, e nos tambem o faremos.

### Brincando de truques com seguranca — const_cast

Nem `static_cast`, nem `dynamic_cast` (nem `reinterpret_cast`, por sinal) podem mudar os qualificadores `cv` de uma expressao; para fazer isso, voce precisa de `const_cast`. Com `const_cast`, voce pode adicionar ou remover os qualificadores `const` ou `volatile` de uma expressao. Como voce pode ter adivinhado, isso so faz sentido em um ponteiro ou em uma referencia.

Por que voce faria algo como remover o `const` de uma expressao? Surpreendentemente, ha muitas situacoes em que isso e util, mas uma comum e permitir o uso de um tipo *const-correct* em um contexto em que o `const` nao foi usado adequadamente — por exemplo, codigo legado que nao usou `const`, como o seguinte:

```cpp
#include <vector>
struct ResourceHandle { /* ... */ };
// this function observes a resource without modifying it,
// but the type system is not aware of that fact (the
// argument is not const)
void observe_resource(ResourceHandle*);
class ResourceManager {
    std::vector<ResourceHandle *> resources;
    // ...
public:
    // note: const member function
    void observe_resources() const {
      // we want to observe each resource, for example
      // to collect data
      for(const ResourceHandle * h : resources) {
       // does not compile, h is const
       // observe_resource(h);
      // temporarily dismiss constness
          observe_resource(const_cast<ResourceHandle*>(h));
      }
    }
    // ...
};
```

`const_cast` e uma ferramenta para brincar com a seguranca do sistema de tipos; deve ser usado em situacoes especificas e controladas e nao para fazer coisas irrazoaveis como mudar o valor de uma constante matematica como pi. Se voce tentar algo assim, incorrera em UB — e com razao.

### "Acredite em mim, compilador" — reinterpret_cast

As vezes, voce simplesmente precisa fazer o compilador acreditar em voce. Por exemplo, sabendo que `sizeof(int)==4` em sua plataforma, voce pode querer tratar `int` como `char[4]` para interoperar com uma API existente que espera esse tipo. Observe que voce deve garantir que essa propriedade se mantenha (talvez por meio de `static_assert`), em vez de confiar na crenca de que essa propriedade se mantém em todas as plataformas (nao se mantem).

Isso e o que `reinterpret_cast` lhe oferece — a capacidade de converter um ponteiro de algum tipo para um ponteiro de um tipo nao relacionado. Isso pode ser usado em situacoes em que voce busca se beneficiar da interconvertibilidade de ponteiros, como vimos no Capitulo 2, assim como pode ser usado para mentir para o sistema de tipos de varias maneiras bastante perigosas e nao portateis.

Considere a conversao mencionada de um inteiro para um array de quatro bytes — se o objetivo e facilitar o enderecamento de bytes individuais, voce deve estar ciente de que a *endianness* (ordem dos bytes) dos inteiros depende da plataforma, assim como o codigo escrito provavelmente sera nao portatil, a menos que algumas medidas cuidadosas sejam tomadas.

Alem disso, observe que `reinterpret_cast` apenas muda o tipo associado a uma expressao — por exemplo, ele nao realiza os pequenos ajustes de endereco que `static_cast` faria ao converter de uma classe derivada para uma classe base em situacoes de heranca multipla.

O exemplo a seguir mostra a diferenca entre esses dois *casts*:

```cpp
struct B0 { int n = 3; };
struct B1 { float f = 3.5f; };
// B0 is the first base subobject of D
class D : public B0, public B1 { };
int main() {
    D d;
    // b0 and &d point to the same address
    // b1 and &d do not point to the same address
    B0 *b0 = static_cast<B0*>(&d);
    B1 *b1 = static_cast<B1*>(&d);
    int n0 = b0->n; // Ok
    float f0 = b1->f; // Ok
    // r0 and &d point to the same address
    // r1 and &d also point to the same address... oops!
    B0 *r0 = reinterpret_cast<B0*>(&d); // fragile
    B1 *r1 = reinterpret_cast<B1*>(&d); // bad idea
    int nr0 = r0->n; // Ok but fragile
    float fr0 = r1->f; // UB
}
```

Use `reinterpret_cast` com parcimonia. Usos relativamente seguros incluem converter um ponteiro para uma representacao integral dado um tipo integral suficientemente largo (e vice-versa), converter entre ponteiros nulos de tipos diferentes e converter entre tipos de ponteiro para funcao — embora nesse caso, os resultados de chamar a funcao por meio do ponteiro resultante sejam indefinidos. A lista completa de conversoes que podem ser realizadas com esse *cast* pode ser encontrada em `wg21.link/expr.reinterpret.cast` se voce quiser saber mais.

### Eu sei que os bits estao certos — bit_cast

O C++20 introduziu `bit_cast`, um novo *cast* que pode ser usado para copiar bits de um objeto para outro da mesma largura, iniciando o *lifetime* do objeto de destino (e dos objetos nele encerrados, se houver) ao longo do caminho, desde que tanto os tipos de origem quanto de destino sejam trivialmente copiáveis (*trivially copyable*). Essa funcao de biblioteca um tanto magica pode ser encontrada no cabecalho `<bit>` e e `constexpr`.

Eis um exemplo:

```cpp
#include <bit>
struct A { int a; double b; };
struct B { unsigned int c; double d; };
int main() {
    constexpr A a{ 3, 3.5 }; // ok
    constexpr B b = std::bit_cast<B>(a); // Ok
    static_assert(a.a == b.c && a.b == b.d); // Ok
    static_assert((void*)&a != (void*)&b); // Ok
}
```

Como pode ser visto neste exemplo, tanto `A` quanto `B` sao construidos em tempo de compilacao e sao bit a bit identicos um ao outro, mas seus enderecos sao diferentes, pois sao objetos completamente diferentes. Seus membros de dados sao parcialmente de tipos diferentes, mas sao dos mesmos tamanhos, na mesma ordem, e todos sao trivialmente copiáveis.

Alem disso, observe o uso de um *cast* estilo C na ultima linha deste exemplo. Como discutiremos em breve, este e um dos poucos usos razoaveis dos *casts* estilo C (poderíamos ter usado `static_cast` aqui tambem e seria igualmente eficiente).

### Algo um tanto nao relacionado, mas que vale mencionar — duration_cast

Nao nos deteremos muito em `duration_cast`, pois esta apenas tangencialmente relacionado ao nosso topico de interesse, mas como fara parte do nosso conjunto de ferramentas para *micro-benchmarking* neste livro, pelo menos merece uma mencao.

A funcao de biblioteca `duration_cast` pode ser encontrada no cabecalho `<chrono>` e faz parte do *namespace* `std::chrono`. E `constexpr` e pode ser usada para converter entre expressoes de diferentes unidades de medida.

Por exemplo, suponha que queremos medir o tempo necessario para executar alguma funcao `f()`, usando o `system_clock` fornecido por nosso fornecedor de biblioteca. Podemos ler esse relogio usando sua funcao membro estatica `now()` antes e depois de chamar `f()`, o que nos fornece dois objetos `time_point` para aquele relogio (dois momentos no tempo), e entao calcular a diferenca entre eles para obter uma duracao para aquele relogio. Nao sabemos qual unidade de medida foi usada para representar essa duracao, mas se quisermos usa-la expressa em, digamos, microsegundos, usamos `duration_cast` para realizar essa conversao:

```cpp
#include <chrono>
#include <iostream>
int f() { /* ... */ }
int main() {
    using std::cout;
    using namespace std::chrono;
    auto pre = system_clock::now();
    int res = f();
    auto post = system_clock::now();
    cout << "Computed " << res << " in "
        << duration_cast<microseconds>(post - pre);
}
```

Sistematizaremos nossas praticas de *benchmarking* mais adiante neste livro, mostrando uma maneira mais formal de medir o tempo de execucao de funcoes ou blocos de codigo, mas `duration_cast` sera nossa ferramenta de escolha para garantir que o formato em que apresentamos os resultados seja adequado para nossas necessidades.

### O detestado — o cast C

Voce pode ser tentado a usar *casts* estilo C quando conversoes de tipo sao necessarias, pois a sintaxe C aparece em outras linguagens e tende a ser concisa de expressar — `(T)expr` trata a expressao `expr` como sendo do tipo `T`. Essa concisao e na verdade uma desvantagem, nao uma vantagem, como veremos. Limite os *casts* estilo C ao minimo em codigo C++:

- Os *casts* estilo C sao mais dificeis de encontrar ao realizar uma pesquisa automatizada no texto do codigo-fonte, pois se parecem com argumentos em uma chamada de funcao. Como os *casts* sao formas pelas quais mentimos para o sistema de tipos, revisar periodicamente a decisao de usa-los e util, portanto, ser capaz de encontra-los e valioso. Em comparacao, os *casts* C++ sao palavras-chave, o que os torna mais faceis de encontrar.
- Um *cast* estilo C nao transmite informacoes sobre por que uma conversao ocorreu. Ao escrever `(T)expr`, nao estamos dizendo se queremos mudar os qualificadores `cv`, navegar em uma hierarquia de classes, simplesmente mudar o tipo de um ponteiro, etc. Em particular, ao converter entre ponteiros para tipos diferentes, um *cast* estilo C geralmente se comportara como `reinterpret_cast`, o que, como vimos, pode levar a resultados desastrosos em algumas circunstancias.

As vezes, voce vera *casts* estilo C em codigo C++, principalmente para situacoes em que a intencao e absolutamente clara. Vimos um exemplo no final de nossa secao sobre `bit_cast`. Outro exemplo seria silenciar avisos do compilador — por exemplo, ao chamar uma funcao que e `[[nodiscard]]`, mas ainda assim realmente querer descartar os resultados por algum motivo.

Em outro exemplo, considere a seguinte funcao generica:

```cpp
template <class ItA, class ItB>
    bool all_equal(ItA bA, ItA eA, ItB bB, ItB eB) {
      for(; bA != eA && bB != eB; ++bA, (void) ++bB)
          if (*bA != bB)
            return false;
      return true;
    }
```

Esta funcao itera por duas sequencias delimitadas, respectivamente, por `[bA,eA)` e `[bB,eB)` (garantindo parar assim que a sequencia mais curta for processada), compara os elementos na "mesma posicao" nessas duas sequencias e retorna `true` apenas se todas as comparacoes entre elementos dessas duas sequencias forem iguais.

Observe que o *cast* para `void` usa um *cast* estilo C entre os incrementos de `bA` e `bB` neste codigo, que converte o resultado de `++bB` para `void`. Isso pode parecer estranho, mas este e um codigo que pode ser usado em muitas situacoes por praticamente qualquer pessoa, incluindo usuarios hostis (ou distraidos). Suponha que alguem com uma mente maliciosa decidisse sobrecarregar o operador de virgula (sim, voce pode fazer isso) entre os tipos de `operator++(ItA)` e `operator++(ItB)`. Essa pessoa poderia entao essencialmente sequestrar nossa funcao para executar codigo inesperado. Ao converter um dos argumentos para `void`, garantimos que isso nao seja possivel.

## Resumo

Isso conclui nossa rapida visao geral dos *casts* e qualificacoes `cv` em C++. Agora que vimos algumas maneiras de enganar o sistema de tipos e nos meter em apuros, bem como sabemos por que devemos fazer essas coisas com cuidado (se e que devemos), podemos comecar a construir coisas belas com C++ e trabalhar em direcao a abstracoes seguras e eficientes em nosso empenho de escrever programas corretos que controlam como gerenciamos a memoria.

No proximo capitulo, comecaremos usando um dos recursos definidores da linguagem, o destrutor, para automatizar a maneira como nosso codigo lida com recursos, com um olhar especial sobre a forma como a memoria e gerenciada.
