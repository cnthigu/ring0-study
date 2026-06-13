# Capitulo 11 - Reclamacao Diferida (Deferred Reclamation)

> Traducao nao oficial do livro *C++ Memory Management* (Patrice Roy, 2025).
> Texto adaptado para portugues brasileiro com fins educacionais. Paginas originais: 239-260.
> Repositorio: [ring0-study](https://github.com/cnthigu/ring0-study)

> Navegacao: [Anterior](capitulo-10.md) | [Indice](README.md) | [Proximo](capitulo-12.md)

---

No Capitulo 9, mostramos alguns exemplos de mecanismos de alocacao de memoria incomuns e como eles podem ser usados, incluindo como reagir a erros para dar aos nossos programas uma forma de "segunda chance" de continuar, bem como como usar memoria atipica ou exotica por meio da mediacao das facilidades da linguagem C++. Entao, no Capitulo 10, examinamos a alocacao baseada em arena e algumas variantes com foco em questoes de velocidade, determinismo e controle sobre o consumo de recursos.

O que faremos no capitulo atual e algo que nao e feito com frequencia em C++, mas que e pratica comum em programas escritos em muitas outras linguagens, particularmente as que possuem coletores de lixo integrados: escreveremos mecanismos que atrasam a destruicao de objetos alocados dinamicamente em momentos selecionados da execucao de um programa.

Nao escreveremos um coletor de lixo (*garbage collector*) adequado, pois isso envolveria um envolvimento mais profundo no funcionamento interno do compilador e impactaria o modelo de programacao que torna o C++ uma ferramenta tao maravilhosa. No entanto, criaremos mecanismos para *reclamacao diferida*, no sentido de que objetos selecionados serao deliberadamente destruidos e seu armazenamento subjacente liberado juntos em momentos escolhidos, mas sem necessariamente garantir uma ordem de destruicao.

Apos ler este capitulo, voce sera capaz de:

- Entender as contrapartidas associadas a reclamacao diferida, pois ha ganhos a serem obtidos, mas tambem ha custos envolvidos;
- Implementar um wrapper externo quase transparente para rastrear a memoria que precisa ser coletada;
- Implementar um wrapper externo quase transparente para ajudar a finalizar os objetos sujeitos a reclamacao diferida;
- Implementar um ponteiro contante semelhante ao contador de referencia de um objeto `std::shared_ptr` para identificar objetos que podem ser reclamados ao final de um escopo escolhido.

> **Finalizacao? Reclamacao?**
>
> Voce notara que, neste capitulo, frequentemente usaremos a palavra *finalizacao* em vez da palavra *destruicao*, pois queremos enfatizar o fato de que o codigo executado ao final do tempo de vida de um objeto (seu destrutor) e distinto do codigo que libera seu armazenamento subjacente.
>
> Como declarado anteriormente neste capitulo, nomearemos *reclamacao* o ato de liberar a memoria para um ou muitos objetos em momentos selecionados, por exemplo, no final de um escopo ou ao atingir o fim da execucao de um programa.

## O que queremos dizer com reclamacao diferida?

Por que alguem quereria recorrer a reclamacao diferida? Essa e uma pergunta valida!

A resposta curta e que ela resolve um problema real. De fato, ha programas onde faz sentido nao coletar objetos logo apos eles deixarem de ser referenciados pelo codigo cliente, ou onde e incerto se eles podem ser coletados ate que saibamos com certeza que o codigo que poderia usa-los foi concluido. Esses programas sao um tanto raros em C++ por causa da forma como raciocinamos sobre codigo em nossa linguagem, mas nao sao raros quando olhamos para o mundo da programacao em geral.

Por exemplo, considere uma funcao em que ha referencias circulares entre alguns dos objetos alocados localmente. Em outros casos, se sabemos que um grupo de objetos nao escapara de uma determinada funcao, podemos tambem usar o conhecimento de que, no final dessa funcao, todos eles podem ser reclamados como um grupo.

Se voce e familiar com linguagens com coleta de lixo, provavelmente sabe que na maioria delas, o coletor "reclama os bytes", liberando o armazenamento subjacente dos objetos reclamados, mas nao finaliza os objetos. Uma razao para isso e que e dificil (em alguns casos, impossivel) para um objeto em tal linguagem saber quais outros objetos ainda existem no programa, pois nao ha garantia de ordem de finalizacao.

O fato de que a reclamacao nao significa finalizacao em muitas linguagens simplifica a tarefa de coletar os objetos: pode-se conceitualmente chamar `std::free()` ou alguma funcao equivalente e liberar memoria sem se preocupar com os objetos nela. Em linguagens que garantem finalizacao antes da reclamacao, muitas vezes se encontra uma hierarquia de classes enraizada em uma unica classe base comum (frequentemente chamada `object` ou `Object`), o que possibilita chamar o equivalente de um destrutor virtual em cada objeto e finalizá-lo polimorficamente.

O C++ nao tem blocos `finally`, nem usa tecnicas intrusivas como interfaces especiais conhecidas pela linguagem que recebem tratamento especial ou uma classe base comum a todos os tipos. Em C++, os objetos sao geralmente feitos responsaveis pelo gerenciamento de seus recursos por meio do idioma RAII.

Nao ter uma classe base comum a todos os tipos significa que teremos que ou renunciar a finalizacao (o que pode funcionar se nos limitarmos a alocar objetos de tipos trivialmente destrutiveis) ou encontrar alguma outra maneira de lembrar os tipos dos objetos que alocamos e chamar o destrutor apropriado quando o momento chegar. Neste capitulo, mostraremos como implementar ambas as abordagens.

Mostraremos tres exemplos diferentes:

- Codigo que reclama objetos selecionados no final da execucao do programa, mas nao os finaliza, limitando a reclamacao diferida a objetos trivialmente destrutiveis;
- Codigo que reclama e finaliza objetos selecionados no final da execucao do programa;
- Codigo que reclama e finaliza objetos selecionados no final de escopos selecionados.

## Reclamacao (sem finalizacao) no final do programa

Nossa primeira implementacao fornecera reclamacao, mas nao finalizacao, no final da execucao do programa. Por essa razao, nao aceitara gerenciar objetos de algum tipo `T` se `T` nao for trivialmente destruivel.

O codigo cliente neste caso seria o seguinte:

```cpp
// ...
// nota: nao trivialmente destruivel
struct CoisaNomeada {
   const char *nome;
   CoisaNomeada(const char *n) : nome{ n } {
      std::print("{} ctor\n", nome);
   }
   ~CoisaNomeada() {
      std::print("{} dtor\n", nome);
   }
};

struct Identificador {
   int valor;
};

// nao compilaria (CoisaNomeada nao e trivialmente destruivel):
/*
void g() {
   [[maybe_unused]] auto p = gcnovo<CoisaNomeada>("oi");
   [[maybe_unused]] auto q = gcnovo<CoisaNomeada>("la");
}
*/

void g() {
   [[maybe_unused]] auto p = gcnovo<Identificador>(2);
   [[maybe_unused]] auto q = gcnovo<Identificador>(3);
}

auto h() {
   struct X {
      int m() const { return 123; }
   };
   return gcnovo<X>();
}

auto f() {
   g();
   return h();
}

int main() {
   std::print("Antes\n");
   std::print("{}\n", f()->m());
   std::print("Depois\n");
}
```

Com este codigo e o codigo de reclamacao diferida (ainda ausente), este programa imprimira:

```
Antes
123
Depois
~GC com 3 objetos para desalocar
```

Note que `f()` aloca e retorna um objeto do qual `main()` chama a funcao membro `m()` sem recorrer explicitamente a um *smart pointer*, mas este programa nao vaza memoria. Objetos alocados por meio da funcao `gcnovo<T>()` sao registrados no objeto `GC`, e o destrutor do objeto `GC` garantira que os blocos de memoria registrados sejam desalocados.

A classe `GC` (inspirada no apelido tipicamente associado a coletores de lixo, mesmo que nao seja exatamente o que estamos implementando) usa o padrao de projeto Singleton:

```cpp
#include <vector>
#include <memory>
#include <string>
#include <print>
#include <type_traits>

class GC {
   std::vector<void*> raizes;   // ponteiros brutos para os objetos
   GC() = default;

   static auto &obter() {
      static GC gc;
      return gc;
   }

   template <class T, class ... Args>
      T *adicionar_raiz(Args &&... args) {
         // apenas tipos trivialmente destrutiveis sao aceitos
         static_assert(
            std::is_trivially_destructible_v<T>
         );
         return static_cast<T*>(
            raizes.emplace_back(
               new T(std::forward<Args>(args)...)
            )
         );
      }

   template <class T, class ... Args>
      friend T* gcnovo(Args&&...);

public:
   ~GC() {
      std::print("~GC com {} objetos para desalocar",
                 std::size(raizes));
      // reclama sem finalizar
      for(auto p : raizes) std::free(p);
   }

   GC(const GC &) = delete;
   GC& operator=(const GC &) = delete;
};

template <class T, class ... Args>
   T *gcnovo(Args &&...args) {
      return GC::obter().adicionar_raiz<T>(
         std::forward<Args>(args)...
      );
   }
```

`GC::~GC()` chama `std::free()` mas nao invoca nenhum destrutor, pois esta implementacao reclama memoria mas nao finaliza objetos.

Este exemplo mostra uma forma de agrupar a reclamacao de memoria como um unico bloco a ser executado no final de um programa. Em codigo onde ha mais memoria disponivel do que o programa requer, isso pode levar a uma execucao mais simplificada do programa, embora ao custo de uma ligeira lentidao no encerramento do programa.

## Reclamacao e finalizacao no final do programa

Nossa segunda implementacao nao apenas liberara o armazenamento subjacente para os objetos alocados por meio de nosso sistema de reclamacao diferida, mas tambem os finalizara chamando seus destrutores. Para fazer isso, precisaremos lembrar o tipo de cada objeto que passa por nosso sistema.

Ao garantir a finalizacao de objetos reclamados, podemos nos livrar do requisito de trivialmente destruivel de nossa implementacao anterior. Ainda nao garantiremos a ordem em que os objetos sao finalizados, entao e importante que os objetos reclamados nao se refiram uns aos outros durante a finalizacao.

O codigo cliente neste caso:

```cpp
// nota: nao trivialmente destruivel
struct CoisaNomeada {
   const char *nome;
   CoisaNomeada(const char *n) : nome{ n } {
      std::print("{} ctor\n", nome);
   }
   ~CoisaNomeada() {
      std::print("{} dtor\n", nome);
   }
};

void g() {
   [[maybe_unused]] auto p = gcnovo<CoisaNomeada>("oi");
   [[maybe_unused]] auto q = gcnovo<CoisaNomeada>("la");
}

auto h() {
   struct X {
      int m() const { return 123; }
   };
   return gcnovo<X>();
}

auto f() {
   g();
   return h();
}

int main() {
   std::print("Antes\n");
   std::print("{}\n", f()->m());
   std::print("Depois\n");
}
```

A saida esperada:

```
Antes
oi ctor
la ctor
123
Depois
oi dtor
la dtor
```

Como se pode ver, os construtores ocorrem quando invocados no codigo-fonte, mas os destrutores sao chamados no encerramento do programa (apos o fim de `main()`).

A implementacao usa a tecnica de *polimorfismo externo*: a classe `GC` armazena ponteiros para objetos de um tipo base `GcRaiz`, mas na pratica guarda instancias de `GcNodo<T>` que contem o codigo especifico do tipo:

```cpp
#include <vector>
#include <memory>
#include <print>

class GC {
   // abstraction type-agnostica que troca em ponteiros brutos
   class GcRaiz {
      void *p;
   public:
      auto obter() const noexcept { return p; }
      GcRaiz(void *p) : p{ p } { }
      GcRaiz(const GcRaiz &) = delete;
      GcRaiz& operator=(const GcRaiz &) = delete;
      virtual void destruir(void *) const noexcept = 0;
      virtual ~GcRaiz() = default;
   };

   // subtipo que contem o codigo especifico do tipo T
   template <class T> class GcNodo : public GcRaiz {
      void destruir(void* q) const noexcept override {
         delete static_cast<T*>(q);   // finaliza e desaloca
      }
   public:
      template <class ... Args>
         GcNodo(Args &&... args) :
            GcRaiz(new T(std::forward<Args>(args)...)) {
         }
      ~GcNodo() {
         destruir(obter());
      }
   };

   std::vector<std::unique_ptr<GcRaiz>> raizes;
   GC() = default;

   static auto &obter() {
      static GC gc;
      return gc;
   }

   template <class T, class ... Args>
      T *adicionar_raiz(Args &&... args) {
         return static_cast<T*>(raizes.emplace_back(
            std::make_unique<GcNodo<T>>(
               std::forward<Args>(args)...)
         )->obter());
      }

   template <class T, class ... Args>
      friend T* gcnovo(Args&&...);

public:
   GC(const GC &) = delete;
   GC& operator=(const GC &) = delete;
};

template <class T, class ... Args>
   T *gcnovo(Args &&...args) {
      return GC::obter().adicionar_raiz<T>(
         std::forward<Args>(args)...
      );
   }
```

> **Sobre a importancia das interfaces**
>
> Voce pode notar que o codigo usuario essencialmente nao mudou entre a implementacao que nao finaliza objetos e esta. A beleza aqui e que nossa melhoria e completamente alcancada na implementacao, deixando a interface estavel e, como tal, as diferencas transparentes para o codigo cliente. Ser capaz de mudar a implementacao sem impactar as interfaces e um sinal de baixo acoplamento.

## Reclamacao e finalizacao no final do escopo

Nossa terceira e ultima implementacao neste capitulo garantira reclamacao e finalizacao no final do escopo, mas apenas sob demanda. Com isso, queremos dizer que se um usuario quiser reclama objetos inutilizados sujeitos a reclamacao diferida no final de um escopo, sera possivel fazê-lo. Objetos sujeitos a reclamacao diferida que ainda sao considerados em uso nao serao reclamados, e objetos que nao estao em uso nao serao reclamados se o codigo usuario nao solicitar.

Para rastrear referencias a objetos, usaremos objetos do tipo `ptr_contagem<T>`. Em vez de destruir o contador e o *pointee* como faria um `shared_ptr<T>`, `ptr_contagem<T>` excluira o contador mas "marcara" o *pointee*, tornando-o um candidato para reclamacao posterior.

O codigo cliente para este exemplo:

```cpp
// ...
struct CoisaNomeada {
   const char *nome;
   CoisaNomeada(const char *n) : nome{ n } {
      std::cout << nome << " ctor" << std::endl;
   }
   ~CoisaNomeada() {
      std::cout << nome << " dtor" << std::endl;
   }
};

auto g() {
   auto _ = coletar_escopo{};                    // solicita coleta no final do escopo
   [[maybe_unused]] auto p = gcnovo<CoisaNomeada>("oi");
   auto q = gcnovo<CoisaNomeada>("la");
   return q;                                     // "la" ainda esta em uso
} // coleta ocorre aqui: "oi" e coletado, "la" nao

auto h() {
   struct X {
      int m() const { return 123; }
   };
   return gcnovo<X>();
}

auto f() {
   auto _ = coletar_escopo{};
   auto p = g();
   std::cout << '\"' << p->nome << '\"' << std::endl;
} // coleta ocorre aqui: "la" e coletado

int main() {
   std::cout << "Antes" << std::endl;
   f();
   std::cout << h()->m() << std::endl;
   std::cout << "Depois" << std::endl;
} // coleta ocorre aqui (fim do programa)
```

Saida esperada:

```
Antes
oi ctor
la ctor
oi dtor
"la"
la dtor
123
Depois
```

O tipo `coletar_escopo` e um tipo RAII simples que invoca uma coleta no final do seu tempo de vida:

```cpp
struct coletar_escopo {
   coletar_escopo() = default;
   coletar_escopo(const coletar_escopo &) = delete;
   coletar_escopo(coletar_escopo &&) = delete;
   coletar_escopo& operator=(const coletar_escopo &) = delete;
   coletar_escopo& operator=(coletar_escopo &&) = delete;
   ~coletar_escopo() {
      GC::obter().coletar(); // solicita coleta de objetos marcados
   }
};
```

O tipo `ptr_contagem<T>` rastreia quantos clientes ainda referenciam um objeto, e quando o ultimo cliente se desconecta, marca o objeto como pronto para ser reclamado:

```cpp
#include <vector>
#include <memory>
#include <iostream>
#include <atomic>
#include <functional>
#include <utility>

template <class T>
   class ptr_contagem {
      using tipo_contador = std::atomic<int>;
      T *p;
      tipo_contador *contador;
      std::function<void()> marcar;  // funcao para marcar como coletavel

   public:
      template <class M>
         constexpr ptr_contagem(T *p, M marcar_fn) try :
            p{ p }, marcar{ marcar_fn } {
               contador = new tipo_contador{ 1 };
         } catch(...) {
            delete p;
            throw;
         }

      T& operator*() noexcept { return *p; }
      const T& operator*() const noexcept { return *p; }
      T* operator->() noexcept { return p; }
      const T* operator->() const noexcept { return p; }

      constexpr bool operator==(const ptr_contagem &outro) const {
         return p == outro.p;
      }

      // semantica de copia: incrementa o contador
      void trocar(ptr_contagem &outro) {
         using std::swap;
         swap(p, outro.p);
         swap(contador, outro.contador);
         swap(marcar, outro.marcar);
      }

      constexpr operator bool() const noexcept {
         return p != nullptr;
      }

      ptr_contagem(ptr_contagem &&outro) noexcept
         : p{ std::exchange(outro.p, nullptr) },
           contador{ std::exchange(outro.contador, nullptr) },
           marcar{ outro.marcar } {
      }

      ptr_contagem& operator=(ptr_contagem &&outro) noexcept {
         ptr_contagem{ std::move(outro) }.trocar(*this);
         return *this;
      }

      ptr_contagem(const ptr_contagem &outro)
         : p{ outro.p }, contador{ outro.contador },
           marcar{ outro.marcar } {
         if (contador) ++(*contador);
      }

      ptr_contagem& operator=(const ptr_contagem &outro) {
         ptr_contagem{ outro }.trocar(*this);
         return *this;
      }

      // quando o ultimo cliente se desconecta: marca para reclamacao
      ~ptr_contagem() {
         if (contador) {
            if ((*contador)-- == 1) {
               marcar();       // marca o objeto como coletavel
               delete contador;
            }
         }
      }
   };

namespace std {
   template <class T, class M>
      void swap(ptr_contagem<T> &a, ptr_contagem<T> &b) {
         a.trocar(b);
      }
}
```

A classe `GC` nesta versao final associa um indicador `bool` (coletar ou nao coletar) a cada ponteiro reclamavel e usa `coletar()` para liberar em grupo todos os objetos marcados:

```cpp
class GC {
   class GcRaiz { /* igual ao anterior */ };

   template <class T> class GcNodo : public GcRaiz { /* igual ao anterior */ };

   // pares: (ponteiro_unico_para_raiz, marcado_para_coleta)
   std::vector<
      std::pair<std::unique_ptr<GcRaiz>, bool>
   > raizes;

   GC() = default;

   static auto &obter() {
      static GC gc;
      return gc;
   }

   // marca o objeto apontado por p como coletavel
   void marcar_coletavel(void *p) {
      for (auto &[q, coletar] : raizes)
         if (static_cast<GcRaiz*>(p) == q.get())
            coletar = true;
   }

   // reclaim todos os objetos marcados como coletaveis
   void coletar() {
      for (auto p = std::begin(raizes);
           p != std::end(raizes); ) {
         if (auto &[ptr, coletavel] = *p; coletavel) {
            ptr = nullptr;                   // destrui via unique_ptr
            p = raizes.erase(p);
         } else {
            ++p;
         }
      }
   }

   template <class T, class ... Args>
      auto adicionar_raiz(Args &&... args) {
         auto q = static_cast<T*>(raizes.emplace_back(
            std::make_unique<GcNodo<T>>(
               std::forward<Args>(args)...
            ), false
         ).first->obter());

         // a funcao de marcacao e uma lambda que varre
         // as raizes para encontrar e marcar q
         return ptr_contagem{
            q, [&,q]() {
               for (auto &[p, coletar] : raizes)
                  if (static_cast<void*>(q) ==
                      p.get()->obter()) {
                     coletar = true;
                     return;
                  }
            }
         };
      }

   template <class T, class ... Args>
      friend ptr_contagem<T> gcnovo(Args&&...);
   friend struct coletar_escopo;

public:
   GC(const GC &) = delete;
   GC& operator=(const GC &) = delete;
};

template <class T, class ... Args>
   ptr_contagem<T> gcnovo(Args &&... args) {
      return GC::obter().adicionar_raiz<T>(
         std::forward<Args>(args)...
      );
   }
```

Como se pode ver, este ultimo exemplo se beneficiaria de varias otimizacoes, mas funciona e e simples o suficiente para ser entendido e melhorado.

Agora sabemos que e possivel reclama objetos em grupos em C++, como e feito em outras linguagens populares. Pode nao ser codigo idiomatico em C++, mas a reclamacao diferida pode ser alcancada com esforco razoavel, em uma base de adesao voluntaria.

## Resumo

Este capitulo nos levou ao territorio da reclamacao diferida, um territorio desconhecido para muitos programadores C++. Vimos formas pelas quais podemos reclama objetos em grupos em pontos especificos de um programa, discutimos restricoes sobre o que pode ser feito ao reclama tais objetos, e examinamos varias tecnicas para finalizar objetos antes de liberar seu armazenamento de memoria associado.

Agora estamos prontos para ver como o gerenciamento de memoria interage com os containers do C++, um topico importante que nos ocupara nos proximos tres capitulos.

Na verdade, poderíamos escrever containers que gerenciam memoria explicitamente, mas em geral, isso seria contraproducente (por exemplo, se vincularmos `std::vector<T>` a `new` e `delete`, como `std::vector<T>` poderia lidar com algum tipo `T` para o qual alocacao e desalocacao tem que ser feita por outros meios?).

Ha, claro, algumas maneiras de chegar la. Quer conhecer algumas delas? Vamos respirar fundo e mergulhar...
