# Capitulo 10 - Gerenciamento de Memoria Baseado em Arena e Outras Otimizacoes (Arena-Based Memory Management and Other Optimizations)

> Traducao nao oficial do livro *C++ Memory Management* (Patrice Roy, 2025).
> Texto adaptado para portugues brasileiro com fins educacionais. Paginas originais: 211-238.
> Repositorio: [ring0-study](https://github.com/cnthigu/ring0-study)

> Navegacao: [Anterior](capitulo-09.md) | [Indice](README.md) | [Proximo](capitulo-11.md)

---

Nossa caixa de ferramentas de gerenciamento de memoria esta crescendo a cada capitulo. Agora sabemos como sobrecarregar os operadores de alocacao de memoria (Capitulo 7) e como colocar essa habilidade para trabalhar de formas que resolvem uma variedade de problemas concretos (Capitulos 8 e 9 dao alguns exemplos ilustrativos do mundo real).

Uma razao importante pela qual alguem gostaria de assumir o controle dos mecanismos de alocacao de memoria e o desempenho. Seria presunçoso (e simplesmente errado!) afirmar que e trivial superar a implementacao dessas funcoes fornecida pelo seu fornecedor de biblioteca, pois essas implementacoes sao boas, frequentemente muito boas, para o caso medio. O elemento-chave da frase anterior, e claro, e "para o caso medio." Quando o caso de uso de alguem tem especificidades conhecidas de antemaо, as vezes e possivel se beneficiar dessa informacao e esculpir uma implementacao que supera, talvez por uma margem consideravel, qualquer coisa que pudesse ter sido projetada para um excelente desempenho medio.

Este capitulo trata de usar o conhecimento do problema de gerenciamento de memoria que queremos resolver e construir uma solucao que se destaque para nos. Isso pode significar uma solucao que e mais rapida em media, suficientemente rapida mesmo no pior caso, que mostra tempos de execucao deterministicos, que reduz a fragmentacao de memoria, e assim por diante.

Apos este capitulo, nossa caixa de ferramentas sera ampliada para nos permitir fazer o seguinte:

- Escrever algoritmos de estrategia de alocacao baseada em arena otimizados para enfrentar restricoes conhecidas a priori;
- Escrever estrategias de alocacao por tamanho de bloco de memoria;
- Entender os beneficios bem como os riscos associados a tais tecnicas.

> **Sobre os (as vezes diminuentes) retornos da otimizacao**
>
> Como discutiremos tecnicas de otimizacao neste capitulo, algumas palavras de aviso sao necessarias: otimizacao e uma coisa complicada, um alvo movel, e o que torna o codigo melhor um dia pode piorá-lo em outro dia. Da mesma forma, o que pode parecer uma boa ideia em teoria pode levar a lentidoes na pratica quando implementado e testado.
>
> Antes de tentar otimizar partes do seu programa, geralmente e prudente medir, de preferencia com uma ferramenta de *profiling*, e identificar as partes que podem se beneficiar dos seus esforcoes. Entao, mantenha uma versao simples (mas correta) do seu codigo por perto e use-a como linha de base. Sempre que voce tentar uma otimizacao, compare os resultados com o codigo de linha de base e execute esses testes regularmente, especialmente ao mudar hardware, biblioteca, compilador ou versao dos mesmos. As vezes, algo como uma atualizacao do compilador pode induzir uma nova otimizacao que "ve atraves" do codigo de linha de base simples e o torna mais rapido do que sua alternativa cuidadosamente elaborada. Seja humilde, seja razoavel, meça cedo e meça com frequencia.

## Gerenciamento de memoria baseado em arena

A ideia por tras do gerenciamento de memoria baseado em arena e alocar um pedaco de memoria em um momento conhecido do programa e gerenciá-lo como um "pequeno heap personalizado" baseado em uma estrategia que se beneficia do conhecimento da situacao ou do dominio do problema.

Ha muitas variantes nesse tema geral, incluindo as seguintes:

- Em um jogo, alocar e gerenciar a memoria por cena ou por fase, desalocando-a como um unico bloco no final dessa cena ou fase. Isso pode ajudar a reduzir a fragmentacao de memoria no programa;
- Quando as condicoes em que alocacoes e desalocacoes sao conhecidas por seguir um determinado padrao ou ter requisitos de memoria limitados, especializar as funcoes de alocacao para se beneficiar dessas informacoes;
- Expressar uma forma de propriedade para um grupo de objetos similares de forma a destrui-los todos em um ponto posterior do programa em vez de fazer isso um objeto de cada vez.

### Exemplo especifico – implementacao baseada em tamanho

Suponha que estamos trabalhando em um videogame onde a acao converge para um finale grandioso onde Orcs e Elfos se encontram em uma batalha grandiosa. Ningem se lembra por que esses dois grupos se odeiam, mas ha uma suspeita de que um dia, um dos Elfos disse a um dos Orcs "Voce sabe, voce nao cheira tao mal hoje!" e este Orc ficou tao insultado que iniciou uma rivalidade que continua ate hoje. E um rumor, de qualquer forma.

Acontece que, neste jogo, algumas coisas sao conhecidas sobre o comportamento do codigo que usa Orcs, especificamente:

- Nunca haverа mais do que um certo numero de objetos `Orc` alocados dinamicamente no geral, entao temos um limite superior para o espaco necessario para armazenar essas criaturas;
- Os Orcs que morrem nao voltarao a vida naquele jogo, pois nao ha xamas para ressuscitá-los. Expresso de outra forma, nao ha necessidade de implementar uma estrategia que reutilize o armazenamento de um objeto `Orc` uma vez que ele seja destruido.

Essas duas propriedades abrem opcoes algoritmicas para nos:

- Se tivermos memoria suficiente disponivel, poderíamos alocar antecipadamente um unico bloco de memoria grande o suficiente para colocar todos os objetos `Orc` no jogo, pois sabemos qual e o cenario de pior caso;
- Como sabemos que nao precisaremos reutilizar a memoria associada a objetos `Orc` individuais, podemos implementar uma estrategia simples (e muito rapida) para alocacao que faz quase nenhuma contabilidade e, como veremos, nos permite alcancar alocacao em tempo constante e deterministico para esse tipo.

Para os propositos deste exemplo, a classe `Orc` sera representada por tres membros de dados, `nome` (um `char[4]`, pois essas criaturas tem um vocabulario limitado), `forca` (do tipo `int`) e `cheiro` (do tipo `double`, pois essas criaturas tem... uma reputacao):

```cpp
class Orc {
  char nome[4]{ 'U', 'R', 'G' };
  int forca = 100;
  double cheiro = 1000.0;
public:
  static constexpr int NB_MAX = 1'000'000;
  // ...
};
```

Supondo `sizeof(int)==4` e `sizeof(double)==8` e que, sendo tipos fundamentais, seus requisitos de alinhamento correspondem aos seus respectivos tamanhos, podemos assumir que `sizeof(Orc)==16` neste caso. Se almejamos alocar espaco suficiente para todos os objetos `Orc` de uma vez, garantir que `sizeof(Orc)` permaneca razoavel para os recursos a nossa disposicao e importante. Por exemplo, definindo o numero maximo de objetos `Orc` em um programa como `Orc::NB_MAX` e a quantidade maxima de memoria que podemos alocar de uma vez para objetos `Orc` como alguma constante hipotetica chamada `LIMITE`, poderíamos deixar um `static_assert` como este em nosso codigo-fonte como uma forma de verificacao de restricoes respeitadas:

```cpp
static_assert(Orc::NB_MAX * sizeof(Orc) <= LIMITE);
```

### Declarando a classe Orc

Aqui esta o arquivo de cabecalho `Orc.h`:

```cpp
#ifndef ORC_H
#define ORC_H
// #define VERSAO_CASEIRA
#include <cstddef>
#include <new>

class Orc {
  char nome[4]{ 'U', 'R', 'G' };
  int forca = 100;
  double cheiro = 1000.0;
public:
  static constexpr int NB_MAX = 1'000'000;
#ifdef VERSAO_CASEIRA
   void * operator new(std::size_t);
   void * operator new[](std::size_t);
   void operator delete(void *) noexcept;
   void operator delete[](void *) noexcept;
#endif
};
#endif
```

A macro `VERSAO_CASEIRA` pode ser descomentada para usar nossa versao das funcoes de alocacao. Como pode ser esperado, como estamos aplicando uma estrategia especial para a classe `Orc` e seus padroes de uso esperados, estamos usando sobrecargas de funcao membro para os operadores de alocacao. (Nao queremos tratar objetos `int` ou — imagine! — Elfos da mesma forma que trataremos os Orcs, certo? Pensei que nao.)

### Definindo a classe Orc e implementando uma arena

A essencia do codigo relacionado ao gerenciamento de memoria estara em `Orc.cpp`. Nomearemos nossa classe de arena `Tribo` e ela sera um singleton.

Os aspectos importantes de nossa implementacao sao os seguintes:

- O construtor padrao (e unico) da classe `Tribo` aloca um unico bloco de `Orc::NB_MAX*sizeof(Orc)` bytes. E importante notar imediatamente que nao ha objetos `Orc` nesse pedaco: esse bloco de memoria e apenas do tamanho e forma certos para colocar todos os objetos `Orc` que precisaremos. Uma ideia-chave para alocacao baseada em arena e que, pelo menos para esta implementacao, a arena gerencia memoria bruta, nao objetos;
- Validamos imediatamente que a alocacao foi bem-sucedida. Usamos um `assert()` neste caso, mas lancar `std::bad_alloc` ou chamar `std::abort()` tambem seriam opcoes razoaveis. Um objeto `Tribo` mantém dois ponteiros, `p` e `atual`, ambos apontando inicialmente para o inicio do bloco. Usaremos `p` como marcador do inicio do bloco, e `atual` como ponteiro para o proximo bloco a retornar; assim, `p` permanecera estavel durante a execucao do programa e `atual` avancara `sizeof(Orc)` bytes a cada alocacao;
- O destrutor libera todo o bloco de memoria gerenciado pelo objeto `Tribo`. Este e um procedimento muito eficiente: e mais rapido do que liberar separadamente blocos menores e leva a muito pouca fragmentacao de memoria;
- Esta primeira implementacao usa a tecnica do Singleton de Meyers vista no Capitulo 8, mas usaremos uma abordagem diferente mais adiante neste capitulo para comparar os impactos de desempenho de duas estrategias de implementacao para o mesmo padrao de projeto.

O codigo segue:

```cpp
#include "Orc.h"
#ifdef VERSAO_CASEIRA
#include <cassert>
#include <cstdlib>
#include <mutex>

class Tribo {
  std::mutex m;
  char *p, *atual;

  // aloca o bloco de memoria para todos os Orcs
  Tribo() : p{ static_cast<char*>(
      std::malloc(Orc::NB_MAX * sizeof(Orc))
  ) } {
      assert(p);
      atual = p;
  }

  Tribo(const Tribo&) = delete;
  Tribo& operator=(const Tribo&) = delete;

public:
  ~Tribo() {
      std::free(p);
  }

  static auto &obter() {
      static Tribo singleton;
      return singleton;
  }

  // alocacao em tempo constante: apenas avanca o ponteiro
  void * alocar() {
      std::lock_guard _ { m };
      auto q = atual;
      atual += sizeof(Orc);
      return q;
  }

  // desalocacao e no-op: nao reutilizamos blocos
  void desalocar(void *) noexcept {
  }
};
// ...
```

Chegamos entao aos operadores de alocacao sobrecarregados. Para manter esta implementacao simples, suporemos que nao havera arrays de objetos `Orc` para alocar:

```cpp
// ...
void * Orc::operator new(std::size_t) {
  return Tribo::obter().alocar();
}

void * Orc::operator new[](std::size_t) {
  assert(false); // nao suportado
}

void Orc::operator delete(void *p) noexcept {
  Tribo::obter().desalocar(p);
}

void Orc::operator delete[](void *) noexcept {
  assert(false); // nao suportado
}
#endif // VERSAO_CASEIRA
```

### Testando nossa implementacao

O programa de teste usa uma funcao `medir()` e a funcao `main()`. A funcao `medir()` tomara uma funcao nao-void `f()`, um pacote variadic de argumentos `args`, e chamara `f(args...)` fazendo a leitura de um relogio antes e depois da chamada, retornando um par contendo o resultado da execucao e o tempo decorrido:

```cpp
#include <chrono>
#include <utility>

template <class F, class ... Args>
  auto medir(F f, Args &&... args) {
      using namespace std;
      using namespace std::chrono;
      auto antes = high_resolution_clock::now();
      auto res = f(std::forward<Args>(args)...);
      auto depois = high_resolution_clock::now();
      return pair{ res, depois - antes };
  }
```

> **Por que exigir funcoes nao-void?**
>
> Retornamos o resultado de chamar `f(args...)` mesmo que em alguns casos o valor de retorno possa ser um pouco artificial. A ideia aqui e garantir que o compilador pense que o resultado de `f(args...)` e util e nao o otimize para fora. Os compiladores sao criaturas espertas e podem remover codigo que parece inutil sob o que e coloquialmente conhecido como a "regra as-if" (simplificando: se nao ha efeito visivel em chamar uma funcao, apenas livre-se dela!).

O programa de teste:

```cpp
// ...
#include "Orc.h"
#include <print>
#include <vector>

int main() {
  using namespace std;
  using namespace std::chrono;
#ifdef VERSAO_CASEIRA
  print("VERSAO CASEIRA\n");
#else
  print("VERSAO BIBLIOTECA PADRAO\n");
#endif

  vector<Orc*> orcs;
  orcs.reserve(Orc::NB_MAX);   // evita realocacoes durante a medicao

  // mede a construcao de NB_MAX orcs
  auto [r0, dt0] = medir([&orcs] {
      for(int i = 0; i != Orc::NB_MAX; ++i)
        orcs.push_back(new Orc);
      return size(orcs);
  });

  // ... CARNAGE (CENSURADO) ...

  // mede a destruicao de NB_MAX orcs
  auto [r1, dt1] = medir([&orcs] {
      for(auto p : orcs)
        delete p;
      return size(orcs);
  });

  print("Construcao: {} orcs em {}\n",
        size(orcs), duration_cast<microseconds>(dt0));
  print("Destruicao: {} orcs em {}\n",
        size(orcs), duration_cast<microseconds>(dt1));
}
```

### Olhando para os numeros

Usando um compilador gcc 15 online com nivel de otimizacao `-O2` e rodando este codigo duas vezes (uma vez com a versao da biblioteca padrao e uma vez com a versao caseira usando o Singleton de Meyers), obtemos os seguintes numeros para chamadas aos operadores `new` e `delete` em `Orc::NB_MAX` (aqui, 10⁶) objetos:

| N=10⁶ | Biblioteca padrao | Singleton de Meyers |
|---|---|---|
| `operator new()` | 23433μs | 17906μs |
| `operator delete()` | 7943μs | 638μs |

Nosso `operator new()` caseiro tomou apenas 76,4% do tempo consumido pela versao da biblioteca padrao, e nosso `operator delete()` tomou apenas 8,03% do tempo necessario para nossa linha de base. Realizamos alocacao em tempo constante e essencialmente "nenhum tempo" para desalocacao.

No entanto, nossa raciocinio agudo pode nos surpreender com o fato de que a desalocacao nao e realmente mais rapida do que medimos. E uma funcao vazia que estamos chamando, afinal, entao o que esta consumindo esse tempo de CPU? A resposta e... nosso singleton, ou mais precisamente, o acesso a variavel local estatica usada para a implementacao de Meyers.

O C++ implementa "estaticas magicas" (*magic statics*) onde a chamada ao construtor do objeto local estatico e protegida por mecanismos de sincronizacao que garantem que o objeto seja construido apenas uma vez. Como podemos ver, essa sincronizacao, por mais eficiente que seja, nao e gratuita.

Em nosso caso, se podemos garantir que nenhum outro objeto global precisara chamar `Tribo::obter()` antes que `main()` seja chamado, podemos substituir a abordagem de Meyers por uma abordagem mais classica onde o singleton e simplesmente um membro de dados estatico da classe `Tribo`:

```cpp
// implementacao de singleton "global" (o resto do
// codigo permanece inalterado)
class Tribo {
  std::mutex m;
  char *p, *atual;

  Tribo() : p{ static_cast<char*>(
      std::malloc(Orc::NB_MAX * sizeof(Orc))
  ) } {
      assert(p);
      atual = p;
  }

  Tribo(const Tribo&) = delete;
  Tribo& operator=(const Tribo&) = delete;
  static Tribo singleton;   // membro estatico

public:
  ~Tribo() { std::free(p); }

  static auto &obter() {
      return singleton;   // sem verificacao de construcao!
  }

  void * alocar() {
      std::lock_guard _ { m };
      auto q = atual;
      atual += sizeof(Orc);
      return q;
  }

  void desalocar(void *) noexcept {}
};

// em algum arquivo .cpp, dentro de #ifdef VERSAO_CASEIRA
Tribo Tribo::singleton;
```

| N=10⁶ | Biblioteca padrao | Singleton de Meyers | Singleton global |
|---|---|---|---|
| `operator new()` | 23433μs | 17906μs | 17573μs |
| `operator delete()` | 7943μs | 638μs | 0μs |

As chamadas a `operator new()` sao ligeiramente mais rapidas (74,99% do tempo da versao da biblioteca padrao), mas as chamadas a `operator delete()` se tornaram *no-ops* efetivos. Е dificil fazer melhor do que isso!

### Generalizando para SizeBasedArena\<T,N\>

A classe `Tribo` parece especifica para a classe `Orc`, mas na pratica ela realmente e especifica para objetos do tamanho de `Orc`, pois nunca chama nenhuma funcao da classe `Orc`, nunca constroi um objeto `Orc`, nem destroi um. Isso significa que poderíamos transformar essa classe em uma classe generica e reutilizá-la para outros tipos que se espera serem usados sob restricoes similares:

```cpp
#ifndef ARENA_BASEADA_EM_TAMANHO_H
#define ARENA_BASEADA_EM_TAMANHO_H
#include <cassert>
#include <cstdlib>
#include <mutex>

template <class T, std::size_t N>
class ArenaBaseadaEmTamanho {
  std::mutex m;
  char *p, *atual;

  ArenaBaseadaEmTamanho() : p{ static_cast<char*>(
      std::malloc(N * sizeof(T))
  ) } {
      assert(p);
      atual = p;
  }

  ArenaBaseadaEmTamanho(const ArenaBaseadaEmTamanho&) = delete;
  ArenaBaseadaEmTamanho&
      operator=(const ArenaBaseadaEmTamanho&) = delete;

public:
  ~ArenaBaseadaEmTamanho() { std::free(p); }

  static auto &obter() {
      static ArenaBaseadaEmTamanho singleton;
      return singleton;
  }

  // aloca um objeto
  void * alocar_um() {
      std::lock_guard _ { m };
      auto q = atual;
      atual += sizeof(T);
      return q;
  }

  // aloca um array de n objetos
  void * alocar_n(std::size_t n) {
      std::lock_guard _ { m };
      auto q = atual;
      atual += n * sizeof(T);
      return q;
  }

  void desalocar_um(void *) noexcept {}
  void desalocar_n(void *) noexcept {}
};
#endif
```

> **Por que usar T e N como parametros de template?**
>
> Por que o tipo `T` em vez de um inteiro inicializado com `sizeof(T)` se nao usamos `T` na arena? Bem, se a classe `Elfo` (por exemplo) tambem usasse uma arena baseada em tamanho, e se tivessemos o azar de que `sizeof(Orc)==sizeof(Elfo)`, entao basear em tamanhos em vez de nos proprios tipos poderia, se os valores para seus respectivos parametros `N` forem os mesmos, fazer com que `Orc` e `Elfo` usassem a mesma arena... e nao queremos isso (nem eles!).

A implementacao em `Orc.cpp` seria entao:

```cpp
#include "Orc.h"
#ifdef VERSAO_CASEIRA
#include "arena_tamanho.h"

using Tribo = ArenaBaseadaEmTamanho<Orc, Orc::NB_MAX>;

void * Orc::operator new(std::size_t) {
  return Tribo::obter().alocar_um();
}

void * Orc::operator new[](std::size_t n) {
  return Tribo::obter().alocar_n(n / sizeof(Orc));
}

void Orc::operator delete(void *p) noexcept {
  Tribo::obter().desalocar_um(p);
}

void Orc::operator delete[](void *p) noexcept {
  Tribo::obter().desalocar_n(p);
}
#endif
```

### Quando os parametros mudam

Nossa implementacao de arena baseada em tamanho e muito especifica: pressupoe a possibilidade de alocacoes sequenciais e a capacidade de dispensar a questao (geralmente importante) de reutilizar memoria apos ela ter sido liberada.

Uma ressalva importante para qualquer implementacao baseada em tamanho e, obviamente, que estamos contando com um tamanho especifico. Saiba, portanto, que com essa restricao, nossa implementacao atual e ligeiramente perigosa. De fato, considere a seguinte evolucao de nosso programa, onde imaginamos subclasses de `Orc` mais resistentes:

```cpp
class OrcMaldoso : public Orc {
  float bonusAtaque; // ops!
  // ...
};
```

Pode nao ser aparente a principio, mas acabamos de potencialmente quebrar algo importante com esta nova classe, pois os operadores de alocacao de funcao membro sao herdados por classes derivadas. Isso significa que a classe `Tribo` implementaria uma estrategia destinada a blocos de `sizeof(Orc)` bytes mas seria usada (acidentalmente) tambem para objetos de tamanho `OrcMaldoso`.

Podemos nos proteger dessa situacao desastrosa de duas maneiras. Para a classe `Orc`, poderíamos proibir classes derivadas marcando a classe como `final`:

```cpp
class Orc final {
  // ...
};
```

Da perspectiva da propria `ArenaBaseadaEmTamanho<T,N>`, tambem podemos decidir restringir nossa implementacao a tipos `final`:

```cpp
#include <type_traits>

template <class T, std::size_t N>
class ArenaBaseadaEmTamanho {
  static_assert(std::is_final_v<T>);
  // ...
};
```

As arenas baseadas em tamanho estao longe de ser o unico caso de uso para arenas de memoria. Poderíamos imaginar muitas variacoes tanto no tema baseado em tamanho quanto na estrategia de alocacao. Por exemplo, suponha que introduzamos xamas em nosso jogo e a necessidade de reutilizar memoria se torne uma realidade. Nesse caso, precisariamos considerar uma estrategia para reutilizar memoria — como manter uma lista ordenada de pares `inicio,fim` para delimitar os blocos livres, ou manter uma pilha de blocos liberados recentemente para facilitar a reutilizacao rapida.

## Pools de pedacos (Chunked pools)

Nosso exemplo de arena baseada em tamanho foi otimizado para um unico tamanho de bloco e padroes de uso especificos, mas ha muitas outras razoes para querer aplicar uma estrategia de alocacao especializada. Nesta secao, exploraremos a ideia de um "*chunked pool*" — um pool de memoria bruta pre-alocada de tamanhos de blocos selecionados.

A ideia neste exemplo e que o codigo usuario planeja alocar objetos de tamanhos similares (mas nao necessariamente identicos) e de varios tipos e assume um limite superior no numero maximo de objetos. Escreveremos um tipo `AlocadorPorPedacos<N,Tam...>` onde `N` sera o numero de objetos de cada "categoria de tamanho" e cada valor integral em `Tam...` sera uma categoria de tamanho distinta.

Por exemplo, um objeto `AlocadorPorPedacos<10,20,40,80,160>` pre-alocaria memoria bruta suficiente para conter 10 objetos de tamanho 20 bytes, 40 bytes, 80 bytes e 160 bytes cada — um total de pelo menos 3.000 bytes.

Para entender o que vamos fazer, aqui estao alguns indicadores:

- Na sequencia variadic de valores integrais `Tam...`, exigiremos que os valores sejam ordenados em ordem crescente, pois isso tornara a busca posterior mais rapida. Como esses valores sao conhecidos em tempo de compilacao, sendo parte dos parametros de template do nosso tipo, isso nao tem custos de execucao;
- Pacotes variadics podem estar vazios em C++, mas em nosso caso, um conjunto vazio de categorias de tamanho nao faria sentido, entao garantiremos que isso nao aconteca (em tempo de compilacao, claro);
- Os valores em `Tam...` precisam ser pelo menos `sizeof(std::max_align_t)` e, na pratica, precisaremos tornar as categorias de tamanho efetivas potencias de dois para garantir que tipos arbitrarios possam ser alocados.

### Funcoes auxiliares

```cpp
// ...
template <class T, std::same_as<T> ... Ts>
  constexpr std::array<T, sizeof...(Ts)+1>
      criar_array(T n, Ts ... ns) {
        return { n, ns... };
      }

// verifica se n e potencia de dois
constexpr bool eh_potencia_de_dois(std::integral auto n) {
  return n && ((n & (n - 1)) == 0);
}

class valor_integral_muito_grande {};

// retorna a proxima potencia de dois >= n
constexpr auto proxima_potencia_de_dois(std::integral auto n) {
  constexpr auto limite_superior =
      std::numeric_limits<decltype(n)>::max();
  for(; n != limite_superior && !eh_potencia_de_dois(n); ++n)
       ;
  if(!eh_potencia_de_dois(n)) throw valor_integral_muito_grande{};
  return n;
}

template <class T>
  constexpr bool esta_ordenado(const T &c) {
      return std::is_sorted(std::begin(c), std::end(c));
  }
// ...
```

### A classe AlocadorPorPedacos

```cpp
#include <algorithm>
#include <vector>
#include <utility>
#include <memory>
#include <cassert>
#include <concepts>
#include <limits>
#include <array>
#include <iterator>
#include <mutex>

template <int N, auto ... Tam>
  class AlocadorPorPedacos {
      // restricoes de uso
      static_assert(esta_ordenado(criar_array(Tam...)));
      static_assert(sizeof...(Tam) > 0);
      static_assert(
        ((Tam >= sizeof(std::max_align_t)) && ...)
      );
      static_assert(N > 0);

      // converte cada categoria para a proxima potencia de dois
      static constexpr unsigned long long tamanhos[] {
        proxima_potencia_de_dois(Tam)...
      };

      using ptr_bruto = void*;
      ptr_bruto blocos[sizeof...(Tam)];    // ponteiros para blocos pre-alocados
      int atual[sizeof...(Tam)] {};        // proximo indice em cada bloco

      // verifica se o ponteiro p esta dentro do bloco i
      bool dentro_do_bloco(void *p, int i) {
        void* inicio = blocos[i];
        void* fim = static_cast<char*>(inicio) + N * tamanhos[i];
        return p == inicio ||
                (std::less{}(inicio, p) && std::less{}(p, fim));
      }

      std::mutex m;

  public:
      AlocadorPorPedacos(const AlocadorPorPedacos&) = delete;
      AlocadorPorPedacos&
        operator=(const AlocadorPorPedacos&) = delete;

      // construtor: aloca todos os blocos upfront
      AlocadorPorPedacos() {
        int i = 0;
        for(auto tam : tamanhos)
            blocos[i++] = std::malloc(N * tam);
        assert(std::none_of(
            std::begin(blocos), std::end(blocos),
            [](auto p) { return !p; }
        ));
      }

      // destrutor: libera todos os blocos
      ~AlocadorPorPedacos() {
        for(auto p : blocos)
            std::free(p);
      }

      // aloca n bytes do menor bloco disponivel
      auto alocar(std::size_t n) {
        using std::size;
        for(std::size_t i = 0; i != size(tamanhos); ++i) {
            if(n < tamanhos[i]) {
              std::lock_guard _ { m };
              if(atual[i] < N) {
                  void *p = static_cast<char*>(blocos[i]) +
                            atual[i] * tamanhos[i];
                  ++atual[i];
                  return p;
              }
            }
        }
        // nenhum bloco cabe ou nao ha blocos restantes: usa o global
        return ::operator new(n);
      }

      // libera o ponteiro p
      void liberar(void *p) {
        using std::size;
        for(std::size_t i = 0; i != size(tamanhos); ++i) {
            if(dentro_do_bloco(p, i)) {
              // se quiser reutilizar memoria, e aqui em blocos[i]
              return;
            }
        }
        // p nao esta em nossos blocos: libera normalmente
        ::operator delete(p);
      }
  };
```

> **Por que potencias de dois?**
>
> Na pratica, blocos que nao sao potencias de dois levarao a objetos desalinhados apos a primeira alocacao se os alocarmos contiguamente, e gerenciar preenchimento para evitar isso e possivel mas complicaria nossa implementacao significativamente. Para tornar as alocacoes mais rapidas, computamos a proxima potencia de dois para cada elemento de `Tam...` em tempo de compilacao e os armazenamos no array `tamanhos`. Isso significa que poderiam haver duas categorias de tamanho que acabam tendo o mesmo tamanho (por exemplo, 40 e 60 ambos levariam a blocos de 64 bytes), mas e um problema menor considerando que esta e uma facilidade especializada projetada para usuarios experientes.

As sobrecargas de operadores de alocacao para usar com nosso alocador:

```cpp
template <int N, auto ... Tam>
  void *operator new(std::size_t n, AlocadorPorPedacos<
      N, Tam...
  > &pedacos) {
      return pedacos.alocar(n);
  }

template <int N, auto ... Tam>
  void operator delete(void *p, AlocadorPorPedacos<
      N, Tam...
  > &pedacos) {
      return pedacos.liberar(p);
  }
// new[] e delete[] ficam como exercicio ;)
```

### Testando AlocadorPorPedacos

```cpp
template <int N> struct falso { char _[N] {}; };

// versao com biblioteca padrao
template <int N> auto testar_falso() {
  return std::pair<void *, std::function<void(void*)>> {
      new falso<N>{},
      [](void *p) { delete static_cast<falso<N>*>(p); }
  };
}

// versao com nosso alocador
template <int N, class T> auto testar_falso(T &alocador) {
  return std::pair<void *, std::function<void(void*)>> {
      new (alocador) falso<N>{},
    [&alocador](void *p) { ::operator delete(p, alocador); }
  };
}
```

Os resultados ao rodar com gcc 15 `-O2`: a versao da biblioteca padrao reportou 13.360μs, enquanto a versao de "pedacos" reportou 12.032μs — efetivamente 90,05% do tempo da versao padrao. Esse tipo de aceleracao pode ser valioso desde que lembremos que a alocacao inicial feita no construtor do nosso objeto `pedacos` nao foi medida: a ideia e mostrar que podemos economizar tempo quando e importante e escolher pagar por isso quando nao estamos com pressa.

> **Consideracoes sobre o resultado**
>
> E importante lembrar que esta implementacao nao reutiliza memoria, mas a versao padrao sim, o que significa que nosso ganho de velocidade pode ser contrabalancado por uma perda de funcionalidade. Alem disso, bloquear ou nao o `std::mutex` teve um impacto significativo no ganho de velocidade — o que sugere que, dependendo da plataforma, pode haver um mecanismo de sincronizacao melhor disponivel. Esta implementacao provavelmente e muito ingenua para trazer beneficios se a funcao `liberar()` tambem precisar bloquear o `std::mutex`.

## Resumo

Neste capitulo, examinamos a alocacao baseada em arena com um exemplo concreto (uma arena baseada em tamanho com um padrao de uso particular) e vimos que podemos obter resultados significativos com ela, e entao vimos outro caso de uso com blocos de memoria pre-alocados dos quais escolhemos pedacos onde colocamos objetos, novamente vendo alguns beneficios. Essas tecnicas mostraram novas maneiras de controlar o gerenciamento de memoria, mas de nenhuma forma pretendem representar uma discussao exaustiva sobre o assunto.

O proximo passo em nossa jornada sera expandir as tecnicas vistas neste capitulo e escrever algo que nao e realmente um coletor de lixo (*garbage collector*), mas que e de algumas formas mais fraco e de outras formas melhor: zonas de memoria de reclamacao diferida. Este sera nosso ultimo passo antes de comecarmos a discutir gerenciamento de memoria em containers.
