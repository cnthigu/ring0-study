# Capitulo 9 - Mecanismos de Alocacao Atipicos (Atypical Allocation Mechanisms)

> Traducao nao oficial do livro *C++ Memory Management* (Patrice Roy, 2025).
> Texto adaptado para portugues brasileiro com fins educacionais. Paginas originais: 186-210.
> Repositorio: [ring0-study](https://github.com/cnthigu/ring0-study)

> Navegacao: [Anterior](capitulo-08.md) | [Indice](README.md) | [Proximo](capitulo-10.md)

---

Estamos progredindo em nossa exploracao do gerenciamento de memoria com C++. No Capitulo 7, exploramos as diversas maneiras sintaticas pelas quais se pode sobrecarregar `operator new()` e `operator delete()` (bem como seus equivalentes para arrays), e no Capitulo 8 escrevemos um exemplo real e concreto (um detector de vazamentos de memoria) que se apoia na capacidade de escrever tais sobrecargas. E um bom começo, mostrando concretamente que esse conhecimento tem usos praticos, mas voce pode (com razao) se perguntar o que mais podemos fazer ao controlar as facilidades de gerenciamento de memoria.

Este capitulo sera ligeiramente diferente dos outros. O que faremos aqui e apresentar um conjunto nao exaustivo de maneiras pelas quais se pode se beneficiar de assumir o controle das funcoes de alocacao de memoria do C++. Mais precisamente, mostraremos o seguinte:

- Como o *placement new* pode nos deixar conduzir hardware mapeado em memoria de forma eficiente;
- Como se pode simplificar o uso do tratamento de erros com a versao *nothrow* de `operator new()`;
- Como se pode instalar e usar `std::new_handler` para facilitar a reacao a situacoes de falta de memoria;
- Como se pode lidar com memorias "exoticas" como memoria compartilhada ou memoria persistente por meio da mediacao do C++ padrao.

Ao final deste capitulo, teremos uma visao mais ampla de quais oportunidades as facilidades basicas de alocacao de memoria do C++ nos fornecem.

## Placement new e hardware mapeado em memoria

Ha muitos usos para o *placement new* (um recurso importante discutido no Capitulo 7, como voce deve se lembrar), mas um uso que e particularmente interessante e que ele nos permite mapear objetos de software para hardware mapeado em memoria, permitindo efetivamente que conduzamos o hardware como se fosse software.

Um exemplo funcional desse recurso seria complicado de escrever, pois nos encontrariamos em "territorio de codigo nao portatil", usando recursos especificos do sistema operacional para obter o endereco de um determinado dispositivo e discutindo formas de obter privilégios de leitura e escrita em locais de memoria normalmente acessados por drivers de software. Por essa razao, criaremos um exemplo artificial, porem ilustrativo, e pediremos ao estimado leitor que imagine que as partes ausentes deste exemplo existem.

Primeiro, suponha que estamos desenvolvendo um driver para uma nova placa de video, tao maravilhosa que seu codinome e `placa_video_super`. Para fins de ilustracao, modelaremos isso por meio da seguinte classe:

```cpp
#include <cstdint>

class placa_video_super {
  // ...
public:
  // registradores do hardware
  volatile std::uint32_t r0{}, r1{}, r2{}, r3{};
  static_assert(sizeof(float) == 4); // verificacao de sanidade
  volatile float f0{}, f1{}, f2{}, f3{};
  // etc.

  // inicializa o estado da placa de video
  placa_video_super() = default;
  placa_video_super(const placa_video_super&) = delete;
  placa_video_super&
    operator=(const placa_video_super&) = delete;
  ~placa_video_super() = default;
  // varios servicos (omitidos por brevidade)
};
```

Os aspectos importantes desta classe para nosso proposito sao os seguintes:

- E um tipo nao-copiavel, pois destina-se a mapear para uma zona especifica de memoria. Copiar um objeto desse tipo seria no minimo contraproducente.
- Foi projetado de forma que seu estado possa conceitualmente ser sobreposto ao equivalente de hardware. Por exemplo, dada a declaracao de classe anterior, a partir do inicio do layout de memoria do hardware, esperamos quatro registradores integrais de 32 bits seguidos de quatro registradores de ponto flutuante de 32 bits. Usamos `<cstdint>` para obter os aliases para tipos integrais de largura fixa em nosso compilador.
- Como deve ser o caso sob tais circunstancias, expressamos nossas expectativas por meio de `static_assert` sempre que possivel. Alem disso, como o estado dos registradores de hardware pode mudar por medio de outras acoes alem das do nosso programa, qualificamos os equivalentes de registradores como `volatile` para que os acessos a essas variaveis membro sejam equivalentes a operacoes de E/S para os propositos da maquina abstrata do C++.

> **Por que usamos variaveis volatile neste exemplo?**
>
> Se voce nao esta acostumado a variaveis `volatile`, pode estar se perguntando por que usamos essa qualificacao nos membros de dados de nossa classe que representa hardware mapeado em memoria. A razao pela qual isso e importante e que queremos evitar que nosso compilador otimize codigo com base na suposicao (errada, neste caso) de que se nosso codigo nao toca essas variaveis, entao elas nao mudam de estado, ou que se nossas escritas nessas variaveis nao sao seguidas por leituras em nosso codigo, entao pode-se assumir que nao tem efeito. Por meio de variaveis qualificadas como `volatile`, estamos efetivamente dizendo ao compilador: "Ha coisas acontecendo nesses objetos que voce nao conhece, entao por favor nao suponha demais."

Para nosso programa acessar o hardware mapeado em memoria, normalmente comunicariamos com o sistema operacional com servicos que aceitam como argumento as informacoes necessarias para identificar o dispositivo cujo endereco buscamos. Em nosso caso, simplesmente faremos parecer que podemos acessar uma zona de memoria do tamanho e alinhamento corretos para a qual temos privilegios de leitura e escrita. O endereco de memoria e exposto como memoria bruta (do tipo `void*`), que e o que podemos realisticamente esperar de uma funcao do sistema operacional em circunstancias semelhantes:

```cpp
// em algum lugar na memoria onde temos privilegios de
// leitura / escrita ha um hardware mapeado em memoria
// que corresponde ao dispositivo real
alignas(placa_video_super) char
  dispositivo_mapeado[sizeof(placa_video_super)];

void* obter_endereco_super_placa() {
  return dispositivo_mapeado;
}
```

Chegamos entao a como se pode usar o *placement new* para mapear um objeto para um local de hardware mapeado em memoria. Note que precisamos incluir o cabecalho `<new>` pois e la que o *placement new* e definido. Os passos para atingir nosso objetivo sao os seguintes:

1. Primeiro, obter o endereco onde queremos mapear nosso objeto `placa_video_super`;
2. Entao, por meio do *placement new* naquele endereco, construir um objeto `placa_video_super` de forma que os membros de dados daquele objeto correspondam ao endereco dos registradores que representam;
3. Durante o tempo de vida daquele objeto, usa-lo por meio do ponteiro correspondente (a variavel `a_placa` no trecho de codigo a seguir);
4. Quando terminarmos, o que **nao** queremos fazer e aplicar `operator delete()` em `a_placa`, pois nunca alocamos a memoria associada. Queremos, porem, finalizar o objeto por meio de `~placa_video_super()`, para garantir que o codigo de limpeza ou reinicializacao daquele objeto seja executado.

Assim, terminamos com o seguinte:

```cpp
// ...
#include <new>

int main() {
  // mapeia nosso objeto para o hardware
  void* p = obter_endereco_super_placa();
  auto a_placa =
      new(p) placa_video_super{ /* args */ };

  // por meio do ponteiro a_placa, usa o hardware
  // mapeado em memoria real
  // ...

  a_placa->~placa_video_super(); // destroi sem liberar memoria
}
```

Se a chamada explicita ao destrutor for um problema — por exemplo, em codigo onde excecoes podem ser lancadas ao longo do caminho — podemos usar um objeto `std::unique_ptr` com um deleter customizado (veja o Capitulo 5) para finalizar o objeto `placa_video_super`:

```cpp
// ...
#include <new>
#include <memory>

int main() {
  void* p = obter_endereco_super_placa();

  // deleter customizado: destroi mas nao libera memoria
  std::unique_ptr<
      placa_video_super,
      decltype([](placa_video_super *p) {
        p->~placa_video_super(); // nao chamar delete p!
      })
  > a_placa {
      new(p) placa_video_super{ /* args */ }
  };

  // usa o hardware mapeado em memoria por meio de a_placa
  // ...
  // chamada implicita a a_placa->~placa_video_super()
}
```

Nesse caso, o objeto `std::unique_ptr` finaliza o *pointee* (o objeto `placa_video_super`) mas nao libera seu armazenamento de memoria, levando a codigo mais robusto na presenca de excecoes durante o tempo de vida da variavel `a_placa`.

## Simplificando o uso de nothrow new

Como mencionado no Capitulo 7, o comportamento padrao de `operator new()` quando incapaz de realizar uma requisicao de alocacao e lançar uma excecao. Isso pode resultar de situacoes como falta de memoria ou de outra forma ser incapaz de atender a requisicao de alocacao, caso em que geralmente se lança `std::bad_alloc`; de um comprimento de array incorreto (por exemplo, um comprimento negativo ou excedendo limites definidos pela implementacao), geralmente levando ao lancamento de `std::bad_array_new_length`; ou de falha ao concluir a construcao subsequente do objeto apos a conclusao de `operator new()`, caso em que a excecao lancada sera o que foi lancado pelo construtor que falhou.

Excecoes sao a maneira "normal" para uma funcao C++ sinalizar falha em atender as pos-condicoes da funcao. Em alguns casos, como em um construtor ou em um operador sobrecarregado, e a unica maneira real e praticavel de fazer isso: um construtor nao tem valor de retorno, e a assinatura de funcoes que sobrecarregam operadores geralmente nao deixa espaco para argumentos adicionais ou valores de retorno para relatorio de erros.

E claro que alguns dominios tipicamente nao usam excecoes: um numero significativo de jogos de video sao compilados sem suporte a excecoes, por exemplo, e o mesmo vale para muitos programas escritos para sistemas embarcados. As razoes invocadas vao desde as tecnicas (medo de *overhead* considerado indesejavel em termos de consumo de espaco de memoria, velocidade de execucao, ou ambos) ate as mais filosoficas, mas independentemente das razoes, o fato e que codigo C++ compilado sem suporte a excecoes existe e a versao *nothrow* de `operator new()` e uma realidade.

Isso significa, e claro, que mesmo codigo aparentemente simples como o seguinte pode levar a *undefined behavior* (UB):

```cpp
#include <new>
#include <iostream>

struct X {
  int valor;
  X(int n) : valor{ n } { }
};

int main() {
  auto p = new (std::nothrow) X{ 3 };
  std::cout << p->valor; // PERIGO: p pode ser nulo!
  delete p;
}
```

A razao para esse potencial UB e que se a versao *nothrow* de `operator new()` falhar (improvavel, mas nao impossivel, especialmente em situacoes com restricoes de memoria), entao `p` sera nulo, e acessar o membro `valor` por meio de `p` sera... uma ideia muito ruim.

Claro, a solucao e simples: basta testar o ponteiro antes de usa-lo! Uma maneira de simplificar esse padrao repetitivo e encapsula-lo em uma macro variadic:

```cpp
#include <new>
#include <cstdlib>

// TENTAR_NEW: aloca ou encerra o programa
#define TENTAR_NEW(T,...) [&] { \
  auto p = new (std::nothrow) T(__VA_ARGS__); \
  if(!p) std::abort(); \
  return p; \
}()

struct morre_quando_alocada {
  void* operator new(std::size_t, std::nothrow_t) {
      return {}; // simula falha de alocacao
  }
};

int main() {
  auto p0 = TENTAR_NEW(int);         // int* apontando para int{ 0 }
  auto p1 = TENTAR_NEW(int, 3);      // int* apontando para int{ 3 }
  auto q  = TENTAR_NEW(morre_quando_alocada); // chama abort()
}
```

Outro metodo e usar um template de funcao variadic, que pode levar a uma melhor experiencia de depuracao na pratica:

```cpp
#include <new>
#include <cstdlib>
#include <utility>

template <class T, class ... Args>
  auto tentar_new(Args &&... args) {
      auto p =
        new (std::nothrow) T(std::forward<Args>(args)...);
      if (!p) std::abort(); // encerra em caso de falha
      return p;
  }

struct morre_quando_alocada {
  void* operator new(std::size_t, std::nothrow_t) {
      return {};
  }
};

int main() {
  auto p0 = tentar_new<int>();       // int* para int{ 0 }
  auto p1 = tentar_new<int>(3);      // int* para int{ 3 }
  auto q  = tentar_new<morre_quando_alocada>(); // chama abort()
}
```

A sintaxe de chamada para a versao de template de funcao parece um *cast*, e os argumentos passados a `tentar_new()` sao perfeitamente encaminhados (*perfect forwarding*) para o construtor de `T` para garantir que o construtor esperado seja chamado. Como foi o caso com a macro, poderíamos ter escolhido retornar um objeto `std::unique_ptr<T>` em vez de um objeto `T*` com esta funcao.

> **Maneiras de concluir a execucao de um programa**
>
> Um programa C++ pode concluir de varias formas: atingir o fim da funcao `main()` e a mais obvia, mas outros exemplos existem. Por exemplo, `std::exit()` e usado para terminacao normal do programa acompanhada de etapas de limpeza; `std::quick_exit()` e usado para terminacao do programa sem etapas de limpeza. `std::atexit()` e `std::at_quick_exit()` permitem registrar funcoes a serem chamadas antes da saida, e `std::abort()` e usado para sinalizar terminacao anormal do programa sem etapas de limpeza. `std::terminate()` e usado quando alguma situacao indesejada em uma lista documentada de situacoes ocorre (como uma excecao sendo lancada do construtor de uma variavel estatica ou do corpo de uma funcao `noexcept`). Em nosso caso, o unico mecanismo que realmente se encaixou foi `std::abort()`.

## Situacoes de falta de memoria e new_handler

Ate agora neste livro, declaramos que `operator new()` e `operator new[]()` tipicamente lançam `std::bad_alloc` quando falham ao alocar memoria. E verdade em grande medida, mas ha uma sutileza que evitamos ate agora e a qual daremos atenção agora.

Imagine uma situacao em que o codigo usuario especializou as funcoes de alocacao de memoria para buscar blocos de memoria de uma estrutura de dados pre-alocada com caracteristicas de desempenho interessantes. Suponha que essa estrutura de dados inicialmente aloca espaco para um pequeno numero de blocos e depois passa a alocar mais espaco uma vez que o codigo usuario esgote os blocos da alocacao inicial. Expresso de outra forma: nessa situacao, temos uma configuracao inicial e rapida (vamos chama-la de estado "otimista") e uma configuracao secundaria (vamos chama-la de estado de "segunda chance") que permite ao codigo usuario continuar alocando uma vez que os recursos do estado "otimista" tenham sido consumidos.

Para que um cenario como esse seja transparente, com uma mudanca transparente de estrategia de alocacao alcancavel sem a intervencao explicita do codigo usuario, lancar explicitamente `std::bad_alloc` seria insuficiente. O lançamento completaria a execucao de `operator new()` e o codigo cliente poderia capturar a excecao e tomar alguma acao, claro, mas neste (razoavel) cenario, gostaríamos que a falha ao alocar levasse a alguma acao sendo tomada e que `operator new()` tentasse novamente com o estado atualizado das coisas, se houver.

Em C++, cenarios como esse sao tratados por meio de um `std::new_handler`, que e um alias para um ponteiro de funcao do tipo `void(*)()`. O que se precisa saber e o seguinte:

- Ha um `std::new_handler` global em um programa e, por padrao, seu valor e `nullptr`;
- Pode-se definir o `std::new_handler` ativo por meio da funcao `std::set_new_handler()`, e pode-se obter o `std::new_handler` ativo por meio da funcao `std::get_new_handler()`. Note que, por conveniencia, `std::set_new_handler()` retorna o `std::new_handler` que esta sendo substituido;
- Quando uma funcao de alocacao como `operator new()` falha, o que ela deve fazer e primeiro obter o `std::new_handler` ativo. Se esse ponteiro for nulo, entao a funcao de alocacao deve lançar `std::bad_alloc` como temos feito ate agora; caso contrario, ela deve chamar esse `std::new_handler` e tentar novamente sob as novas condicoes que essa chamada instalou.

Para mostrar como se beneficiar de um `std::new_handler`, implementaremos uma versao artificial do cenario de duas etapas mencionado. Esta implementacao de brinquedo usara a versao de funcao membro dos operadores de alocacao para algum tipo `X` e se comportara como se inicialmente tivessemos memoria suficiente para `limite` objetos daquele tipo (normalmente, realmente gerenciariamos essa memoria, e voce pode ver um exemplo de tal gerenciamento no Capitulo 10). Instalaremos um `std::new_handler` que, quando chamado, muda `limite` para um numero maior e entao redefine o *handler* ativo para `nullptr` de modo que falhas subsequentes ao alocar objetos `X` levem ao lancamento de `std::bad_alloc`:

```cpp
#include <new>
#include <vector>
#include <iostream>

struct X {
  // exemplo de brinquedo, nao thread-safe
  static inline int limite = 5;

  void* operator new(std::size_t n) {
      std::cout << "X::operator new() chamado com "
                << limite << " blocos restantes\n";
      while (limite <= 0) {
        if (auto hdl = std::get_new_handler(); hdl)
            hdl(); // chama o handler e tenta novamente
        else
            throw std::bad_alloc{};
      }
      --limite;
      return ::operator new(n);
  }

  void operator delete(void* p) {
      std::cout << "X::operator delete()\n";
      ::operator delete(p);
  }
  // idem para versoes de array
};

int main() {
  // instala um handler: expande o limite e se remove
  std::set_new_handler([]() noexcept {
      std::cout << "falha de alocacao, "
                   "buscando mais memoria\n";
      X::limite = 10;
      std::set_new_handler(nullptr); // reset
  });

  std::vector<X*> v;
  v.reserve(100);
  try {
      for (int i = 0; i != 10; ++i)
         v.emplace_back(new X);
  } catch(...) {
      // nunca sera alcancado com este programa
      std::cerr << "sem memoria\n";
  }
  for (auto p : v) delete p;
}
```

Note a forma como `X::operator new()` trata a falha: se notar que nao conseguira atender suas pos-condicoes, obtem o `std::new_handler` ativo e, se for nao-nulo, o chama antes de tentar novamente. Isso significa que o `std::new_handler`, quando chamado, deve ou mudar a situacao de forma que uma tentativa de alocacao subsequente possa ter sucesso, ou mudar o `std::new_handler` para `nullptr` de modo que a falha leve ao lancamento de uma excecao. Deixar de respeitar essas regras poderia levar a um loop infinito.

> **Uma lambda como new_handler?**
>
> Voce pode ter notado que descrevemos o tipo `std::new_handler` como sendo um alias para um ponteiro de funcao do tipo `void(*)()`, mas em nosso exemplo de brinquedo, instalamos uma lambda. Por que isso funciona? Acontece que uma lambda sem estado — uma expressao lambda com um bloco de captura vazio — e implicitamente conversivel para um ponteiro de funcao com a mesma assinatura de chamada. E algo util de saber em muitas circunstancias, como ao escrever codigo C++ que faz interface com codigo C ou APIs de sistema operacional.

## C++ padrao e memoria exotica

Nosso ultimo exemplo neste capitulo um pouco estranho com exemplos de uso incomum de gerenciamento de memoria diz respeito as maneiras pelas quais podemos escrever programas C++ padrao que lidam com memoria "exotica". Por "exotica", queremos dizer memoria que requer acoes explicitas para ser "tocada" (alocar, ler, escrever, desalocar, etc.) e que difere de um bloco de memoria "normal" sob o controle do nosso programa. Exemplos de tal memoria incluem memoria persistente (nao volatil) ou memoria compartilhada.

Como temos que escolher um exemplo, escreveremos um exemplo usando um bloco de memoria compartilhada (ficticio).

> **Uma pequena mentira inocente...**
>
> E importante entender que estamos descrevendo um mecanismo para memoria que normalmente seria compartilhada entre processos, mas a comunicacao entre processos e o dominio do sistema operacional. O C++ padrao apenas descreve as regras para compartilhar dados entre *threads* em um processo; por essa razao, contaremos uma pequena mentira inocente e escreveremos um sistema *multithread*, nao de multiplos processos, usando essa memoria para compartilhar dados. Nosso foco e nas facilidades de gerenciamento de memoria, nao na comunicacao entre processos, entao isso nao deve ser um problema.

### Uma API de memoria compartilhada ficticia

Escreveremos uma API que e ficticia, porem inspirada no que se encontra na maioria dos sistemas operacionais, exceto que reportaremos erros por excecoes para simplificar o codigo usuario. A maioria dos sistemas operacionais reporta erros por codigos de erro expressos a partir de valores de retorno, mas isso leva a um codigo usuario mais elaborado.

Como a maioria dos sistemas operacionais, abstrairemos o recurso real por meio de uma forma de *handle*, ou chave; criar um segmento de "memoria compartilhada" de algum tamanho producira uma chave (um identificador integral), apos o que, acessar essa memoria exigira aquela chave, assim como destrui-la. Como se pode esperar com uma facilidade destinada a ser usada para compartilhar dados entre processos, destruir a memoria nao finalizara os objetos nela contidos, entao o codigo usuario precisara garantir que os objetos na memoria compartilhada sejam destruidos antes de liberar o segmento de memoria compartilhada.

As assinaturas e tipos para nossa API serao as seguintes:

```cpp
#include <cstddef>
#include <new>
#include <utility>

class chave_mem_compartilhada_invalida {};
enum id_mem_compartilhada : std::size_t;  // tipo distinto

id_mem_compartilhada criar_mem_compartilhada(std::size_t tamanho);
std::pair<void*, std::size_t>
  obter_mem_compartilhada(id_mem_compartilhada);
void destruir_mem_compartilhada(id_mem_compartilhada);
```

> **Por que usar enum como tipo de chave?**
>
> Voce pode notar que estamos usando um tipo `enum` para `id_mem_compartilhada`. A razao e que tipos `enum` sao tipos distintos em C++, nao apenas aliases como se obteria de `using` ou `typedef`. Ter tipos distintos pode ser util ao sobrecarregar funcoes com base nos tipos de seus argumentos. E um truque util de conhecer.

A implementacao ficticia de nossa API de memoria compartilhada modela um segmento de memoria compartilhada como um `bloco_mem_compartilhada` — um par contendo um array de bytes e um tamanho em bytes. Mantemos um `std::vector` desse tipo, usando os indices naquele array como `id_mem_compartilhada`:

```cpp
#include <vector>
#include <memory>
#include <utility>

struct bloco_mem_compartilhada {
  std::unique_ptr<char[]> mem;
  std::size_t tamanho;
};

std::vector<bloco_mem_compartilhada> mems_compartilhadas;

std::pair<void*, std::size_t>
  obter_mem_compartilhada(id_mem_compartilhada id) {
  if (id < std::size(mems_compartilhadas))
      return { mems_compartilhadas[id].mem.get(),
               mems_compartilhadas[id].tamanho };
  return { nullptr, 0 };
}

id_mem_compartilhada criar_mem_compartilhada(std::size_t tamanho) {
  auto p = std::make_unique<char[]>(tamanho);
  mems_compartilhadas.emplace_back(std::move(p), tamanho);
  return id_mem_compartilhada(std::size(mems_compartilhadas) - 1);
}

bool chave_mem_compartilhada_valida(id_mem_compartilhada id) {
  return id < std::size(mems_compartilhadas) &&
         mems_compartilhadas[id].mem;
}

void destruir_mem_compartilhada(id_mem_compartilhada id) {
  if (!chave_mem_compartilhada_valida(id))
      throw chave_mem_compartilhada_invalida{};
  mems_compartilhadas[id].mem.reset();
}
```

### Um exemplo de codigo usuario "artesanal"

Podemos, claro, escrever codigo usuario que usa nossa API ficticia sem recorrer a facilidades especializadas de gerenciamento de memoria do C++, simplesmente confiando no uso do *placement new* como visto no Capitulo 7. Em ambas as implementacoes ("artesanal" e padrao), usaremos um objeto de dados composto de um valor `int` e um sinalizador `pronto` do tipo `bool`:

```cpp
struct dados {
  bool pronto;
  int valor;
};
```

Nosso programa de exemplo "artesanal" fara o seguinte:

1. Criar um segmento de memoria compartilhada de algum tamanho;
2. Construir um objeto `dados` no inicio desse segmento via *placement new*;
3. Iniciar uma *thread* escritora que escreve em `valor` e depois sinaliza `pronto`;
4. Iniciar uma *thread* leitora que aguarda `pronto` e entao le `valor`.

```cpp
#include <thread>
#include <atomic>
#include <iostream>

int main() {
  constexpr std::size_t N = 1'000'000;
  auto chave = criar_mem_compartilhada(N);

  auto [p, tam] = obter_mem_compartilhada(chave);
  if (!p) return -1;

  // inicia o tempo de vida de um objeto dados nao-pronto
  auto p_dados = new (p) dados{ false };

  std::atomic<bool> comecar{ false };
  std::atomic<bool> concluido{ false };

  // thread escritora
  std::jthread escritora{ [chave, &comecar] {
      comecar.wait(false);
      auto [p, tam] = obter_mem_compartilhada(chave);
      if (p) {
        auto p_dados = static_cast<dados*>(p);
        p_dados->valor = 3;
        std::atomic_thread_fence(std::memory_order_release);
        p_dados->pronto = true;
      }
  } };

  // thread leitora
  std::jthread leitora{ [chave, &concluido] {
      auto [p, tam] = obter_mem_compartilhada(chave);
      if (p) {
        auto p_dados = static_cast<dados*>(p);
        while (!p_dados->pronto)
            ; // espera ocupada, nao ideal
        std::cout << "valor lido: " << p_dados->valor << '\n';
      }
      concluido = true;
      concluido.notify_all();
  } };

  if (char c; !std::cin.get(c)) exit(-1);
  comecar = true;
  comecar.notify_all();

  concluido.wait(false);
  p_dados->~dados();
  destruir_mem_compartilhada(chave);
}
```

Fizemos isso funcionar: temos uma infraestrutura para gerenciar segmentos de memoria compartilhada, podemos usar esses blocos de memoria para compartilhar dados, e podemos escrever codigo que le e escreve nesses dados. No entanto, nao gerenciamos realmente esse bloco: criamos ele e usamos um pequeno pedaco de tamanho `sizeof(dados)` no inicio. E se quisessemos criar multiplos objetos nessa zona? E se quisessemos escrever codigo que tanto cria quanto destroi objetos, introduzindo a necessidade de gerenciar quais partes daquele bloco estao em uso? Isso significaria fazer tudo no codigo usuario, um esforco um tanto trabalhoso.

### Um equivalente com aparencia de codigo padrao

Qual mecanismo o C++ oferece se quisermos usar memoria "exotica" de forma mais idiomatica? Bem, uma maneira de fazer isso e a seguinte:

- Escrever uma classe gerenciadora para a memoria "exotica", encapsulando a interface nao-portatil para o sistema operacional e expondo servicos mais proximos do que o codigo usuario C++ esperaria;
- Escrever sobrecargas dos operadores de alocacao de memoria que tomam uma referencia a tal objeto gerenciador como argumento adicional;
- Fazer essas sobrecargas de operadores de alocacao de memoria fazerem a ponte entre codigo portatil e nao-portatil por meio de delegacao ao objeto gerenciador.

Nossa classe `gerenciador_mem_compartilhada` usara a API ficticia descrita anteriormente:

```cpp
#include <algorithm>
#include <iterator>
#include <new>

class gerenciador_mem_compartilhada {
  id_mem_compartilhada chave;
  std::vector<bool> ocupado;  // indica bytes em uso
  void *mem;

  // encontra o primeiro byte livre a partir de 'inicio'
  auto primeiro_livre(std::size_t inicio = 0) {
      using namespace std;
      auto p = find(begin(ocupado) + inicio, end(ocupado), false);
      return distance(begin(ocupado), p);
  }

  // verifica se ha 'n' bytes livres a partir de 'inicio'
  bool livres_suficientes(std::size_t inicio, int n) {
      using namespace std;
      return inicio + n < size(ocupado) &&
             count(begin(ocupado) + inicio,
                   begin(ocupado) + inicio + n, false) == n;
  }

  void marcar_ocupado(std::size_t de, std::size_t ate) {
      fill(ocupado.begin() + de, ocupado.begin() + ate, true);
  }

  void marcar_livre(std::size_t de, std::size_t ate) {
      fill(ocupado.begin() + de, ocupado.begin() + ate, false);
  }

public:
  // constructor: cria o segmento de memoria compartilhada (RAII)
  gerenciador_mem_compartilhada(std::size_t tamanho)
      : chave{ criar_mem_compartilhada(tamanho) }, ocupado(tamanho) {
      auto [p, tam] = obter_mem_compartilhada(chave);
      if (!p) throw chave_mem_compartilhada_invalida{};
      mem = p;
  }

  gerenciador_mem_compartilhada(const gerenciador_mem_compartilhada&) = delete;
  gerenciador_mem_compartilhada&
      operator=(const gerenciador_mem_compartilhada&) = delete;

  // aloca 'n' bytes dentro do segmento compartilhado
  void* alocar(std::size_t n) {
      using namespace std;
      std::size_t i = primeiro_livre();
      while (!livres_suficientes(i, n) && i != size(ocupado))
        i = primeiro_livre(i + 1);
      if (i == size(ocupado)) throw std::bad_alloc{};
      marcar_ocupado(i, i + n);
      return static_cast<char*>(mem) + i;
  }

  // libera 'n' bytes no endereco 'p' dentro do segmento
  void liberar(void *p, std::size_t n) {
      using namespace std;
      auto i = distance(
         static_cast<char*>(mem), static_cast<char*>(p)
      );
      marcar_livre(i, i + n);
  }

  // destructor: libera o segmento de memoria compartilhada (RAII)
  ~gerenciador_mem_compartilhada() {
      destruir_mem_compartilhada(chave);
  }
};

// sobrecargas dos operadores de alocacao para memoria compartilhada
void* operator new(std::size_t n, gerenciador_mem_compartilhada& mgr) {
  return mgr.alocar(n);
}

void* operator new[](std::size_t n, gerenciador_mem_compartilhada& mgr) {
  return mgr.alocar(n);
}

void operator delete(void *p, std::size_t n,
                    gerenciador_mem_compartilhada& mgr) {
  mgr.liberar(p, n);
}

void operator delete[](void *p, std::size_t n,
                      gerenciador_mem_compartilhada& mgr) {
  mgr.liberar(p, n);
}
```

Equipados com nosso gerenciador e essas sobrecargas, podemos escrever nosso programa de teste que realiza a mesma tarefa que o "artesanal" da secao anterior. Nesse caso, porem, ha algumas diferencas:

- Nao precisamos gerenciar a criacao e destruicao do segmento de memoria compartilhada. Essas tarefas sao tratadas pelo objeto `gerenciador_mem_compartilhada` como parte de sua implementacao do idioma RAII;
- Nao precisamos gerenciar o bloco de memoria compartilhada de forma alguma, pois essa tarefa e atribuida ao objeto `gerenciador_mem_compartilhada`;
- Podemos chamar `new` e `delete` livremente, pois o gerenciamento de memoria se torna transparente para o codigo cliente.

```cpp
int main() {
  constexpr std::size_t N = 1'000'000;
  // AQUI: RAII cuida da criacao e destruicao do segmento
  gerenciador_mem_compartilhada mgr{ N };

  // inicia o tempo de vida de um objeto dados nao-pronto
  auto p_dados = new (mgr) dados{ false };

  std::atomic<bool> comecar{ false };
  std::atomic<bool> concluido{ false };

  // thread escritora captura p_dados diretamente
  std::jthread escritora{ [p_dados, &comecar] {
      comecar.wait(false);
      p_dados->valor = 3;
      std::atomic_thread_fence(std::memory_order_release);
      p_dados->pronto = true;
  } };

  std::jthread leitora{ [p_dados, &concluido] {
      while (!p_dados->pronto)
        ; // espera ocupada
      std::cout << "valor lido: " << p_dados->valor << '\n';
      concluido = true;
      concluido.notify_all();
  } };

  if (char c; !std::cin.get(c)) exit(-1);
  comecar = true;
  comecar.notify_all();

  concluido.wait(false);
  p_dados->~dados();                              // destroi o objeto
  operator delete(p_dados, sizeof(dados), mgr);  // libera a memoria exotica
}
```

Ainda nao e um exemplo trivial, mas o aspecto do gerenciamento de memoria e claramente mais simples do que na versao "artesanal", e a compartimentalizacao de tarefas facilita a otimizacao da maneira como a memoria e gerenciada.

## Resumo

Este capitulo explorou diversas maneiras pelas quais se pode usar as facilidades de gerenciamento de memoria do C++ de formas incomuns: mapear objetos em hardware mapeado em memoria, integrar formas basicas de tratamento de erros com a versao *nothrow* de `operator new()`, reagir a situacoes de falta de memoria com um `std::new_handler`, e acessar memoria atipica com servicos nao-portateis por meio de uma especializacao do operador de alocacao "normal" e um objeto gerenciador. Isso nos da uma visao mais ampla das facilidades de gerenciamento de memoria em C++ e como se pode usa-las a nosso favor.

Uma coisa que mencionamos mas ainda nao discutimos e a otimizacao: como fazer a alocacao de memoria rapida, extremamente rapida ate, e deterministica em termos de velocidade de execucao quando algumas condicoes sao atendidas. E isso que faremos no Capitulo 10 ao explicar como escrever codigo de alocacao baseada em arena.

Oh, e como bonus, mataremos Orcs.
