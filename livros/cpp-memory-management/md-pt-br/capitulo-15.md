# Capitulo 15 - Questoes Contemporaneas (Contemporary Issues)

> Traducao nao oficial do livro *C++ Memory Management* (Patrice Roy, 2025).
> Texto adaptado para portugues brasileiro com fins educacionais. Paginas originais: 369-382.
> Repositorio: [ring0-study](https://github.com/cnthigu/ring0-study)

> Navegacao: [Anterior](capitulo-14.md) | [Indice](README.md) | [Proximo](anexo.md)

---

Estamos chegando ao fim de nossa jornada, caro leitor. Ao longo deste livro, examinamos aspectos fundamentais do modelo de objetos de C++ e discutimos aspectos perigosos da programacao de baixo nivel. Vimos os fundamentos do gerenciamento de recursos em C++ por meio do idioma RAII, exploramos como ponteiros inteligentes sao usados e como escrever esse tipo de objeto. Tambem assumimos o controle das funcoes de alocacao de memoria a nossa disposicao (e fizemos isso de muitas maneiras!), e escrevemos containers que gerenciam memoria diretamente ou por meio de outros objetos ou tipos, incluindo alocadores.

Uma experiencia e tanto!

O que ainda temos a cobrir? Muito... mas ha um limite para o que podemos colocar em um unico livro. Portanto, para concluir nossa discussao sobre gerenciamento de memoria em C++, iremos conversar sobre alguns topicos interessantes no gerenciamento de memoria contemporanea em C++: coisas tao recentemente padronizadas (no momento da escrita) que a maioria das bibliotecas ainda nao as implementa, e coisas que o comite de normas esta ativamente trabalhando.

E importante olhar para o C++ como ele e hoje e como pode ser no futuro proximo porque a linguagem continua evoluindo, e bem rapidamente: uma nova versao do padrao C++ e publicada a cada tres anos desde 2011. A evolucao do C++ e muito lenta para alguns e rapida demais para outros, mas e implacavel (chamamos esse ritmo de publicacao de "modelo de trem" para destacar seu ritmo sustentado) e traz progresso e inovacao regulares a essa linguagem que amamos tanto.

No momento da escrita, no inicio de 2025, o C++23 e um padrao recentemente adotado, tendo sido oficializado em novembro de 2024 (sim, o processo ISO demora algum tempo), e o comite esta discutindo propostas destinadas ao C++26 e ao C++29.

Os topicos relacionados ao gerenciamento de memoria que discutiremos neste capitulo sao aspectos do padrao C++23 que ainda nao discutimos neste livro ou aspectos que, no momento da escrita, estao sendo discutidos para proximas versoes do padrao. Esteja ciente, caro leitor, de que o que voce vai ler agora pode se tornar realidade na forma como esta descrito aqui, mas tambem pode surgir em outra forma apos discussoes no comite de normas C++... ou pode, ao final, nunca se concretizar.

> **Nota sobre os exemplos de codigo deste capitulo**
>
> Se voce tentar compilar os exemplos deste capitulo, esteemed reader, podera se deparar com o fato de que alguns ainda nao compilam e outros podem nao compilar por um bom tempo, ou mesmo nunca. Essa situacao e normal para um capitulo como este: discutiremos uma combinacao de recursos que foram muito recentemente adicionados a linguagem C++ (recentemente o suficiente para que nao tenham sido implementados no momento de escrita deste livro) e recursos que estao em discussao pelo comite de normas. Trate os exemplos como ilustracoes e adapte-os conforme os recursos tomam forma mais definitiva.

Os topicos abordados neste capitulo sao:

- Iniciar explicitamente o tempo de vida de um ou varios objetos sem recorrer a seus construtores
- Relocacao trivial: o que significa e de que maneiras o comite de normas esta tentando aborda-la
- Funcoes de alocacao e desalocacao cientes do tipo: o que fariam e como aproveitá-las

---

## Iniciando o Tempo de Vida de Objetos sem Construtores

Considere o caso de um programa que consume dados serializados de um stream e busca criar objetos a partir desses dados. Aqui esta um exemplo:

```cpp
#include <fstream>
#include <cstdint>
#include <array>
#include <memory>
#include <string_view>

struct Ponto3D {
   float x{}, y{}, z{};
   Ponto3D() = default;
   constexpr Ponto3D(float x, float y, float z)
      : x{ x }, y{ y }, z{ z } {
   }
};

// ...
// le no maximo N bytes do arquivo nome_arquivo e
// escreve esses bytes em buf. Retorna a quantidade de
// bytes lidos (pos-condicao: valor_retorno <= N)
//
template <int N>
   int ler_do_stream(std::array<unsigned char, N> &buf,
                     std::string_view nome_arquivo) {
   // ...
}
```

Nesse exemplo, temos a classe `Ponto3D`. Um objeto desse tipo representa um conjunto de coordenadas x, y, z. Tambem temos uma funcao `ler_do_stream<N>()` que consome bytes de um arquivo, armazena no maximo N bytes no argumento `buf` (passado por referencia) e retorna a quantidade de bytes lidos (que pode ser zero, mas nunca sera maior que N).

Para este exemplo, suponhamos que o arquivo a ser lido contenha a forma binaria de objetos `Ponto3D` serializados, equivalentes a objetos do tipo `float` serializados em formato binario em grupos de tres. Agora considere o seguinte programa, que consome a representacao de bytes de no maximo quatro objetos do tipo `Ponto3D` de um arquivo chamado `some_file.dat`:

```cpp
// ...
#include <print>
#include <cassert>
using namespace std::literals;

int main() {
   static constexpr int NUM_PTS = 4;
   static constexpr int NUM_BYTES =
      NUM_PTS * sizeof(Ponto3D);

   alignas(Ponto3D)
      std::array<unsigned char, NUM_BYTES> buf{};

   if (int n = ler_do_stream<NUM_BYTES>(
          buf, "some_file.dat"sv
       ); n != 0) {

      // imprime os bytes: preenchido com 0 a esquerda, 2
      // caracteres de largura, formato hexadecimal
      for (int i = 0; i != n; ++i)
         std::print("{:0<2x} ", buf[i]);
      std::println();

      // se queremos tratar os bytes como objetos Ponto3D,
      // precisamos iniciar o tempo de vida desses objetos.
      // Se nao fizermos isso, estamos em territorio de UB
      // (pode funcionar ou nao, e mesmo que funcione,
      // nao podemos contar com isso)
      const Ponto3D* pontos =
         std::start_lifetime_as_array(buf.data(), n);

      assert(n % 3 == 0);
      for (std::size_t i = 0;
           i != n / sizeof(Ponto3D); ++i)
         std::print("{} {} {}\n",
                    pontos[i].x, pontos[i].y, pontos[i].z);
   }
}
```

Esse programa de exemplo le bytes de um arquivo para um objeto `std::array` grande o suficiente para conter os bytes de quatro objetos do tipo `Ponto3D`, tendo primeiro garantido que esse array estaria alinhado adequadamente caso fosse armazenar objetos desse tipo. Essa consideracao de alinhamento e essencial, ja que planejamos tratar os bytes como objetos daquele tipo depois que forem lidos.

O ponto desse exemplo e que, uma vez lidos os bytes, o programador esta confiante de que todos os bytes sao corretos para hipotetivos objetos `Ponto3D`, mas ainda nao pode usar esses objetos, pois seu tempo de vida ainda nao comecou.

Esse tipo de situacao tradicionalmente faz muitos programadores de C sorrirem e alguns programadores de C++ estremeceram: o modelo de objetos de C++ impos restricoes em programas que tornam *undefined behavior* (veja o Capitulo 2) usar objetos fora de seu tempo de vida, mesmo que todos os bytes estejam corretos e as restricoes de alinhamento tenham sido respeitadas, enquanto C e menos restritivo. Para usar o conteudo do buffer que acabamos de usar para ler daquele arquivo, nossas opcoes tradicionais sao:

- Percorrer o array de bytes, escrever subconjuntos de tamanho adequado desses bytes em objetos do tipo `float`, chamar os construtores de objetos `Ponto3D` e colocá-los em outro container.
- Fazer `reinterpret_cast` do array de bytes em um array de objetos `Ponto3D` e torcer para o melhor, levando a um codigo que pode ou nao funcionar e, sendo UB, nao seria portavel de qualquer forma (nem mesmo entre versoes de um dado compilador). Com nossos objetos `Ponto3D`, provavelmente dara os resultados desejados, mas substitua-os por, digamos, objetos `std::complex<float>` da biblioteca padrao (um tipo que provavelmente tem uma estrutura interna semelhante ao nosso tipo `Ponto3D`) e... bem, quem sabe o que pode acontecer?
- Fazer `std::memcpy()` do array de bytes sobre si mesmo, fazendo cast do valor de retorno para o tipo `Ponto3D*` e usando o ponteiro resultante como se fosse um array de objetos `Ponto3D`. Isso e na verdade valido (a funcao `std::memcpy()` faz parte de um conjunto seleto de funcoes autorizadas a iniciar o tempo de vida de objetos). Claro, existe o risco de criar uma copia real dos bytes (o que seria tempo de execucao desperdicado); algumas bibliotecas padrao reconhecem esse padrao e se comportam como se a chamada fosse uma no-op, mas um tipo especial de no-op que pode iniciar o tempo de vida de objetos.

Nenhuma dessas opcoes parece verdadeiramente satisfatoria, portanto, e necessaria uma solucao mais limpa que nao dependa de otimizacoes especificas do compilador. Para isso, o padrao C++23 introduz um conjunto de funcoes `constexpr` (acompanhadas de varias sobrecargas) chamadas `std::start_lifetime_as_array<T>(p, n)` e `std::start_lifetime_as<T>(p)`. Ambas sao formas portaveis de funcoes magicas no-op que informam ao compilador que os bytes estao corretos e que considere o tempo de vida dos objetos apontados como tendo comecado.

Claro, se por alguma razao os objetos apontados tiverem destrutores nao-triviais, voce deve garantir que seu codigo chame esses destrutores quando apropriado. Espere que essa situacao seja rara e incomum. Ja que consumimos bytes brutos de alguma fonte de dados e os convertemos em objetos, a probabilidade de que os objetos resultantes possuam recursos e bastante pequena. E claro, caro leitor, esses objetos podem adquirir recursos depois que seus tempos de vida comecaram. Programadores de C++ nao sao nada senao criativos!

Esse conjunto de funcoes `std::start_lifetime_...` e esperado ser uma bencao para programadores de rede em particular: esses individuos frequentemente recebem frames de dados com sequencias de bytes bem formadas que precisam transformar em objetos para processamento. Essas funcoes tambem devem ser uteis para programas que consomem bytes de arquivos para formar agregados. Muitos programadores pensam que simplesmente ler bytes em um array e fazer cast desse array para um tipo pretendido (ou array deste) e suficiente para ter acesso ao(s) objeto(s) hipotetico(s) e ficam surpresos quando seu codigo C++ comeca a se comportar de forma inesperada. C++ e uma linguagem de programacao de sistemas, e o conjunto dessas funcoes `std::start_lifetime_...` fecha uma lacuna onde poderia-se dizer que a linguagem estava sub-servindo.

Claro, essas funcoes formam um conjunto de ferramentas muito afiado devido aos riscos envolvidos: objetos com destruicao nao-trivial cujo tempo de vida comeca dessa forma sao especialmente suspeitos, e voce precisa confiar muito na fonte dos bytes nos quais o tempo de vida de um objeto e manualmente e explicitamente iniciado. Portanto, essas facilidades devem ser usadas com extremo cuidado.

> **Nota:** No momento da escrita, nenhum compilador importante ainda implementa essas funcoes, mesmo que tenham sido padronizadas e facam parte do C++23. Talvez ja estejam implementadas quando voce estiver lendo isso.

---

## Relocacao Trivial

Como voce sabe, caro leitor, C++ e conhecido na comunidade de programacao como uma daquelas linguagens que usamos quando precisamos extrair o maximo de nosso computador ou plataforma de hardware. Alguns dos credos da linguagem podem ser parafraseados como "voce nao devera pagar pelo que nao usa" e "nao devera existir espaco para uma linguagem de nivel inferior (exceto por algum trecho ocasional de codigo assembly)". E o segundo explica a importancia das funcoes `std::start_lifetime_...` da secao anterior.

E provavelmente por isso que, quando se torna evidente que poderemos fazer ainda melhor do que ja fazemos em termos de velocidade de execucao, isso se torna um assunto de interesse para a comunidade de programadores C++ em geral, e mais especificamente para os membros do comite de normas de C++.

Um caso onde podemos fazer melhor e quando encontramos tipos para os quais mover um objeto de origem para um objeto destino, seguido de destruir o objeto original, poderia na pratica ser substituido por uma chamada a `std::memcpy()`: copiar diretamente um array de bytes e mais rapido do que executar uma serie de movimentacoes e destrutores (e se nao for, provavelmente ha algum trabalho necessario na sua implementacao de `std::memcpy()`), mesmo que a combinacao de operadores de movimentacao e destrutores seja rapida.

Acontece que ha muitos tipos para os quais tal otimizacao poderia ser considerada, incluindo `std::string`, `std::any` e `std::optional<T>` (dependendo do tipo `T`), classes como `Ponto3D` da secao anterior, qualquer tipo que nao defina nenhuma das seis funcoes membro especiais vistas no Capitulo 1 (incluindo tipos fundamentais), etc.

Para entender o impacto, considere a seguinte funcao livre `redimensionar()`, que imita uma funcao membro `C::resize()` para algum container `C` que gerencia memoria contigua como nosso tipo `Vector<T>` nas varias encarnacoes vistas neste livro. Essa funcao redimensiona `arr` de `cap_antiga` (a capacidade antiga) para `cap_nova` (a nova capacidade), preenchendo o espaco no final com objetos `T` padrao:

```cpp
// Esta nao e uma boa interface de funcao, mas queremos
// manter o exemplo relativamente simples
//
template <class T>
  void redimensionar
    (T *&arr, std::size_t cap_antiga, std::size_t cap_nova) {
    //
    // poderiamos lidar com construtor padrao lancador
    // mas complicaria nosso codigo; essas complexidades,
    // por mais validas que sejam, estao alem do ponto
    //
    static_assert(
      std::is_nothrow_default_constructible_v<T>
    );
    //
    // as vezes nao ha nada a fazer
    //
    if(cap_nova <= cap_antiga) return arr;
    //
    // aloca um bloco de memoria bruta (sem criar objetos)
    //
    auto p = static_cast<T*>(
      std::malloc(cap_nova * sizeof(T))
    );
    if(!p) throw std::bad_alloc{};
    // ...
```

Nesse ponto, estamos prontos para copiar (ou mover) objetos:

```cpp
    // ...
    //
    // se a movimentacao nao lanca excecao, seja agressivo
    //
    if constexpr(std::is_nothrow_move_assignable_v<T>) {
      std::uninitialized_move(arr, arr + cap_antiga, p);
      std::destroy(arr, arr + cap_antiga);
    } else {
      //
      // como a movimentacao pode lancar excecao, seja
      // conservador e copie em vez disso
      //
      try {
        std::uninitialized_copy(arr, arr + cap_antiga, p);
        std::destroy(arr, arr + cap_antiga);
      } catch (...) {
        std::free(p);
        throw;
      }
    }
    //
    // preenche o espaco restante com objetos padrao
    // (lembre-se: afirmamos estaticamente que T::T() nao
    // lanca excecao)
    //
    std::uninitialized_default_construct(
      p + cap_antiga, p + cap_nova
    );
    //
    // substitui o bloco de memoria antigo (agora sem objetos)
    // pelo novo
    //
    std::free(arr);
    arr = p;
  }
```

Analisando as linhas destacadas dessa funcao, mesmo que a combinacao de `std::uninitialized_move()` seguida de `std::destroy()` forme um caminho rapido, poderemos ser ainda mais rapidos e substituir um numero linear de operadores de movimentacao seguidos de um numero linear de chamadas a destrutores por uma unica chamada a `std::memcpy()`.

Como conseguimos isso? Ha muitas propostas concorrentes de Arthur O'Dwyer, Mingxin Wang, Alisdair Meredith, Mungo Gill, entre outros. Cada proposta tem meritos, mas em comum, elas compartilham:

- Fornecer uma maneira de testar um tipo para "relocabilidade trivial" em tempo de compilacao, por exemplo, um trait `std::is_trivially_relocatable_v<T>`.
- Fornecer uma funcao que realmente realoca os objetos, por exemplo, `std::relocate()` ou `std::trivially_relocate()`, que recebe um ponteiro de origem e um ponteiro de destino como argumentos e realoca o objeto de origem para o local de destino, encerrando o tempo de vida do objeto original e iniciando o do novo.
- Fornecer uma maneira de marcar um tipo como trivialmente relocavel, por exemplo, por meio de uma palavra-chave ou atributo.
- Fornecer regras para deduzir a relocabilidade trivial de um tipo em tempo de compilacao.

Os detalhes podem variar dependendo da abordagem, mas supondo essas ferramentas, a mesma funcao `redimensionar()` poderia se beneficiar da relocacao trivial com um pequeno ajuste a implementacao apresentada anteriormente:

```cpp
template <class T>
   void redimensionar
     (T * &arr, std::size_t cap_antiga, std::size_t cap_nova) {
      static_assert(
         std::is_nothrow_default_constructible_v<T>
      );
      if(cap_nova <= cap_antiga) return arr;
      auto p = static_cast<T*>(
         std::malloc(cap_nova * sizeof(T))
      );
      if(!p) throw std::bad_alloc{};

      //
      // este e o nosso caso ideal
      //
      if constexpr (std::is_trivially_relocatable_v<T>) {
         // equivalente a memcpy() mais considera que o
         // tempo de vida dos objetos em [arr, arr + cap_antiga)
         // terminou e o dos objetos em
         // [p, p + cap_antiga) comecou
         //
         // nota: assume que o trait
         // std::is_trivially_relocatable<T>
         // implica std::is_trivially_destructible<T>
         std::relocate(arr, arr + cap_antiga, p);

      //
      // se a movimentacao nao lanca excecao, seja agressivo
      //
      } else if constexpr(
           std::is_nothrow_move_assignable_v<T>
      ){
         std::uninitialized_move(arr, arr + cap_antiga, p);
         std::destroy(arr, arr + cap_antiga);
      } else {
         // ... veja o exemplo de codigo anterior para o restante
      }
   }
```

Essa otimizacao aparentemente simples foi reportada como oferecendo beneficios consideraveis, com alguns tendo afirmado um speedup de ate 30% em casos comuns. Trata-se de trabalho experimental, e mais benchmarks sao esperados se as propostas convergirem (como esperamos que facam) em algo que sera integrado ao padrao C++.

Esses potenciais ganhos de desempenho fazem parte do que a linguagem C++ visa tornar possivel, portanto, podemos razoavelmente esperar que a relocabilidade trivial se torne realidade num futuro proximo. A questao e "como": como os compiladores devem detectar a propriedade de relocabilidade trivial? Como os programadores devem poder indicar essa propriedade em seus proprios tipos quando as regras de deducao de relocabilidade trivial padrao nao sao atendidas?

Em fevereiro de 2025, o comite de normas votou a relocacao trivial para o que se tornara o padrao C++26. Isso significa que podemos esperar que alguns programas compilados com versoes anteriores do padrao C++ e recompilados com C++26 possam simplesmente rodar mais rapido sem alterar uma unica linha de codigo-fonte.

---

## Funcoes de Alocacao e Desalocacao Cientes do Tipo

Nosso ultimo topico deste capitulo sobre novas abordagens ao gerenciamento de memoria e oportunidades de otimizacao relacionadas ao tempo de vida dos objetos sao as *funcoes de alocacao e desalocacao cientes do tipo* (type-aware allocation and deallocation functions). Esta e uma abordagem nova para funcoes de alocacao em casos onde o codigo do usuario pode querer usar informacoes sobre o tipo que esta sendo alocado (e eventualmente construido) para guiar o processo de alocacao.

Vimos uma faceta dessas funcionalidades no Capitulo 9 ao descrever o mecanismo de *destroying delete* possibilitado pelo C++20, onde uma versao funcao-membro de `T::operator delete()` recebe um `T*` em vez do `void*` abstrato como argumento, e por isso e responsavel tanto pela finalizacao do objeto quanto pela desalocacao de seu armazenamento subjacente. Vimos que ha casos onde isso revela oportunidades de otimizacao interessantes.

O que esta sendo discutido para o C++26 e uma nova familia de funcoes `operator new()` e `operator delete()` — tanto funcoes-membro quanto funcoes livres — que recebem um objeto `std::type_identity<T>` como primeiro argumento para algum tipo `T`, guiando o operador selecionado em direcao a algum comportamento especializado para o tipo `T`. Note que essas funcoes de alocacao cientes do tipo sao realmente funcoes de alocacao: elas nao realizam construcao, nem sua contraparte de desalocacao realiza finalizacao.

> **O que e o trait `std::type_identity<T>`?**
>
> A expressao `typename std::type_identity<T>::type` corresponde a `T`. OK, isso parece trivial. Entao, que papel esse trait desempenha na programacao moderna em C++? O trait `std::type_identity<T>`, introduzido com C++20, e uma ferramenta tipicamente usada para fornecer controle adicional sobre a deducao de tipo de argumento em funcoes genericas.
>
> Por exemplo, com a assinatura `template <class T> void f(T, T)`, voce poderia chamar `f(3, 3)` pois ambos os argumentos sao do mesmo tipo, mas nao `f(3, 3.0)` pois `int` e `double` sao tipos distintos. Dito isso, ao substituir qualquer tipo de argumento por `std::type_identity_t<T>`, voce poderia chamar `f(3, 3.0)`, e como `T` seria deduzido pelo outro argumento (o de tipo `T`), esse tipo seria usado para o outro argumento. Isso levaria ambos os argumentos a serem `int` ou `double`, dependendo de qual argumento e do tipo `T`.
>
> A ideia de usar `std::type_identity<T>` (nao `std::type_identity_t<T>`) em vez do `T` simples como tipo do primeiro argumento nas funcoes de alocacao cientes do tipo e deixar claro que estamos usando essa sobrecarga especifica de `operator new()` e que isso nao e acidente nem uma chamada a alguma outra forma especializada da funcao de alocacao.

Isso significa que voce pode fornecer funcoes de alocacao especializadas para uma classe especifica, `X`, por meio das seguintes assinaturas de funcao:

```cpp
#include <new>
#include <type_traits>

void* operator new(std::type_identity<X>, std::size_t n);
void operator delete(std::type_identity<X>, void* p);
```

Nesses casos, ao chamar `new X`, por exemplo, a forma especializada sera preferida em relacao a forma usual de `operator new()` e `operator delete()`, presumindo-se que seja mais apropriada, a menos que o programador tome medidas para evita-la.

Isso tambem significa que, dado um algoritmo de alocacao especializado que se aplica ao tipo `T` apenas se `alocacao_especial<T>` for satisfeito, voce pode fornecer funcoes de alocacao que usam esse algoritmo especializado para o tipo `T` por meio das seguintes assinaturas:

```cpp
#include <new>
#include <type_traits>

template <class T> requires alocacao_especial<T>
  void* operator new(std::type_identity<T>, std::size_t n);

template <class T> requires alocacao_especial<T>
  void operator delete(std::type_identity<T>, void* p);
```

Isso fornece novos caminhos para otimizacoes como as descritas no Capitulo 10. Considere este exemplo simples onde temos um algoritmo de alocacao especial para os tipos `X` e `Y`, mas esse algoritmo nao se aplica a outras classes como `Z`:

```cpp
#include <concepts>
#include <type_traits>

class X { /* ... */ };
class Y { /* ... */ };
class Z { /* ... */ };

template <class C>
   concept algoritmo_alocacao_especial =
      std::is_same_v<C, X> || std::is_same_v<C, Y>;

// operador new com algoritmo especial (para X e Y)
template <class T> requires algoritmo_alocacao_especial<T>
  void* operator new(std::type_identity<T>, std::size_t n) {
     // aplica o algoritmo de alocacao especial
  }

// operador delete com algoritmo especial (para X e Y)
template <class T> requires algoritmo_alocacao_especial<T>
  void operator delete(std::type_identity<T>, void* p) {
     // aplica o algoritmo de desalocacao especial
  }

#include <memory>
int main() {
   // usa o algoritmo de alocacao especial
   auto p = std::make_unique<X>();
   // usa o algoritmo de alocacao padrao
   auto q = std::make_unique<Z>();
} // usa o algoritmo de desalocacao padrao para q
  // usa o algoritmo de desalocacao especial para p
```

As funcoes de alocacao cientes do tipo tambem podem ser sobrecargas de funcoes-membro, levando a algoritmos que se aplicam a classe onde essas funcoes sao definidas, bem como a classes derivadas.

Considere o seguinte exemplo, inspirado em um exemplo mais complexo encontrado na proposta para esta funcionalidade:

```cpp
class D0; // declaracao antecipada de classe

struct B {
  // i) aplica-se a B e suas classes derivadas
  template <class T>
  void* operator new(std::type_identity<T>, std::size_t);

  // ii) aplica-se especificamente ao tipo D0
  void* operator new(std::type_identity<D0>, std::size_t);
};
// ...
```

Como expresso, `i)` aplica-se a `B` e suas classes derivadas, mas `ii)` aplica-se ao caso especifico da classe declarada antecipadamente `D0` e so sera usado se `D0` for de fato uma classe derivada de `B`.

Continuando este exemplo, adicionamos tres classes que derivam de `B`, com `D2` acrescentando `iii)`, que e uma sobrecarga de funcao-membro nao-ciente-do-tipo de `operator new()`:

```cpp
// ...
struct D0 : B { };
struct D1 : B { };
struct D2 : B {
  // iii) sobrecarga nao-ciente-do-tipo
  void *operator new(std::size_t);
};
// ...
```

Dadas essas sobrecargas, aqui estao alguns exemplos de expressoes que chamam as sobrecargas `i)`, `ii)` e `iii)`:

```cpp
// ...
void f() {
  new B;       // i) onde T e B
  new D0;      // ii)
  new D1;      // i) onde T e D1
  new D2;      // iii)
  ::new B;     // usa o operator new global apropriado
}
```

Como pode ver, caro leitor, as funcoes de alocacao cientes do tipo irao, se aceitas no padrao C++, fornecer novas maneiras de controlar qual algoritmo de alocacao de memoria sera usado (dependendo das circunstancias), mantendo o codigo do usuario no controle e possibilitando que ele defira ao operador `new()` global, caso seja a opcao preferida, como a ultima linha da funcao `f()` no exemplo anterior demonstra.

Ao contrario da funcionalidade *destroying delete* do C++20 — que realiza tanto a finalizacao do objeto quanto a desalocacao do armazenamento subjacente —, as versoes cientes do tipo de `operator new()` e `operator delete()` sao apenas funcoes de alocacao, e no momento da escrita nao ha planos para fornecer uma versao ciente do tipo do *destroying delete*.

---

## Resumo

Neste capitulo, tivemos um vislumbre do futuro com as funcoes `std::start_lifetime_...` que fazem parte do C++23 mas, no momento da escrita, ainda nao foram implementadas por nenhum compilador importante. Tambem examinamos partes provaveis (mas ainda nao oficiais) do futuro do C++ com o potencial suporte a relocabilidade trivial e a possibilidade de introduzir versoes cientes do tipo de `operator new()` e `operator delete()`.

A cada passo, C++ se torna uma linguagem mais rica e versatil com a qual podemos fazer mais e expressar nossas ideias de maneiras mais precisas. C++ e uma linguagem que oferece cada vez mais controle significativo sobre o comportamento de nossos programas. Por mais poderoso que seja o C++ hoje, e por mais poderoso que nos torne como programadores, este capitulo mostra que ainda podemos continuar melhorando.

Estamos no fim de nossa jornada, ao menos por ora. Espero que a viagem tenha sido suficientemente rica para ser agradavel e divertida para voce, estimado leitor, e que tenha aprendido uma ou duas coisas ao longo do caminho. Tambem espero que algumas das ideias discutidas aqui o ajudem em suas tarefas e enriquecam sua perspectiva sobre a programacao em C++.

Obrigado por me acompanhar. Espero que as jornadas adiante para voce sejam agradaveis, assim como espero que este livro melhore sua caixa de ferramentas e que voce continue explorando por conta propria. Boas viagens.

---

> Navegacao: [Anterior](capitulo-14.md) | [Indice](README.md) | [Proximo](anexo.md)
