# Capitulo 14 - Escrevendo Containers Genericos com Suporte a Allocators

> Traducao nao oficial do livro *C++ Memory Management* (Patrice Roy, 2025).
> Texto adaptado para portugues brasileiro com fins educacionais. Paginas originais: 325-368.
> Repositorio: [ring0-study](https://github.com/cnthigu/ring0-study)

> Navegacao: [Anterior](capitulo-13.md) | [Indice](README.md) | [Proximo](capitulo-15.md)

---

Percorremos um longo caminho desde o inicio deste livro. Os capitulos recentes examinaram como se pode escrever containers eficientes em termos de memoria, descrevendo como faze-lo quando o gerenciamento de memoria e feito explicitamente (Capitulo 12) e quando e feito implicitamente, por meio de *smart pointers* (Capitulo 13).

No entanto, nenhuma das abordagens cobertas ate agora corresponde ao que os containers da biblioteca padrao fazem. De fato, os containers da biblioteca padrao sao *allocator-aware* e delegam tarefas de gerenciamento de memoria de baixo nivel a objetos especializados que podem ser fornecidos pelo codigo cliente.

*Allocators* vieram oficialmente para a linguagem C++, junto com os containers da biblioteca padrao, no C++98, mas evoluiram ao longo do tempo. Escrever um *allocator* ficou significativamente mais simples com o C++11, e o C++17 introduziu uma abordagem totalmente nova para alocacao de memoria com os *allocators* PMR (*Polymorphic Memory Resource*) e seus containers.

Neste capitulo, voce vai:

- Entender e usar *allocators* tradicionais;
- Escrever um *allocator* tradicional para um dominio de aplicacao especializado;
- Aprender a gerenciar o tempo de vida do *allocator* quando um container e movido ou copiado;
- Clonar o tipo de um *allocator*;
- Entender e usar *allocators* PMR e seus containers.

## Por que allocators?

*Allocators* tendem a assustar as pessoas, incluindo alguns especialistas, mas voce nao precisa ter medo pois ja possui conhecimento significativo sobre gerenciamento de memoria. A primeira questao que precisamos abordar, antes mesmo de expressar o que e um *allocator*, e "Por que os *allocators* existem?".

Para fazer uma analogia, pense nos iteradores: por que sao uteis e como tornam sua vida como programador melhor. Eles desacoplam a iteracao sobre elementos de uma sequencia de como esses elementos sao organizados naquela sequencia, de modo que se pode escrever codigo que computa a soma dos valores em `std::list<int>` ou `std::vector<short>` sem ter que saber os detalhes de organizacao em memoria.

Da mesma forma, *allocators* desacoplam a organizacao de dados da forma como o armazenamento subjacente e obtido ou liberado. Para um container, um *allocator* representa uma camada muito fina de abstracao sobre o hardware — ele essencialmente *e* o hardware para o container.

## Allocators tradicionais

Como mencionado, *allocators* sao parte do C++ ha decadas, mas existiram em algumas formas e configuracoes diferentes. Nesta secao, adotaremos uma abordagem cronologica, comecando pelos tipos de *allocators* mais antigos e progredindo para os mais simples.

Um ponto-chave a ter em mente e que um tipo de container como `std::vector<T>` nao existe de verdade. O que existe e o tipo `std::vector<T,A>` onde, por padrao, `A` e `std::allocator<T>`, que aloca por meio de `::operator new()` e desaloca por meio de `::operator delete()`.

### Antes do C++11: small_allocator\<T\>

Vamos examinar `small_allocator<T>`, que implementaremos de forma a assemelhar-se a `std::allocator<T>` para destacar o que significava escrever um *allocator* na era do C++11:

```cpp
template <class T>
struct small_allocator {
   using value_type      = T;
   using pointer         = T*;
   using const_pointer   = const T*;
   using reference       = T&;
   using const_reference = const T&;
   using size_type       = std::size_t;
   using difference_type = std::ptrdiff_t;
   // ...
```

**`max_size()`** — tamanho maximo alocavel (geralmente apenas estimativo):

```cpp
   constexpr size_type max_size() const {
      return std::numeric_limits<size_type>::max(); // melhor estimativa
   }
```

**`address()`** — usa `std::addressof()` para evitar `operator&()` sobrecarregado:

```cpp
   constexpr pointer address(reference r) const {
      return std::addressof(r);
   }
   constexpr const_pointer address(const_reference r) const {
      return std::addressof(r);
   }
```

**`allocate()` e `deallocate()`** — a essencia do por que os *allocators* existem:

```cpp
   pointer allocate(size_type n) {
      // aloca n objetos (retorna T*, mas e memoria bruta sem objetos)
      auto p = static_cast<pointer>(malloc(n * sizeof(value_type)));
      if (!p) throw std::bad_alloc{};
      return p;
   }
   void deallocate(pointer p, size_type) {
      free(p);
   }
```

> **Bytes ou objetos?** Ao contrario de `operator new()`, que recebe um numero de bytes como argumento, `allocate()` e `deallocate()` recebem como argumento um numero de *objetos*. Isso ocorre porque *allocators* tradicionais sao conscientes do tipo (sao *allocators* de algum tipo `T`), enquanto `operator new()` e (em sua maior parte) agnosto de tipo.

**`construct()` e `destroy()`** — convertem memoria bruta em objetos e vice-versa:

```cpp
   void construct(pointer p, const_reference r) {
      new (static_cast<void*>(p)) value_type(r); // placement new
   }
   void destroy(const_pointer p) {
      if(p) p->~value_type();
   }

   template <class U>
   struct rebind {
      using other = small_allocator<U>; // "clone desta estrategia para U"
   };
};

// dois allocators sao iguais se memoria alocada por um pode ser
// desalocada pelo outro (para stateless, sempre verdade)
template <class T, class U>
constexpr bool operator==(const small_allocator<T>&,
                          const small_allocator<U>&) { return true; }
template <class T, class U>
constexpr bool operator!=(const small_allocator<T>&,
                          const small_allocator<U>&) { return false; }
```

> **O que e `rebind`?** E uma forma do *allocator* dizer: "Se voce quiser o mesmo tipo que eu, mas aplicado a algum tipo `U` em vez de `T`, aqui esta o que seria esse tipo." Isso e essencial para containers baseados em nos, como veremos adiante.

## Algoritmos de suporte conscientes de allocator

Como usamos *allocators* para fazer a ponte entre armazenamento bruto e objetos, nao podemos usar os algoritmos de memoria bruta vistos nos Capitulos 12 e 13. Escreveremos versoes simplificadas conscientes de *allocator*:

```cpp
// preenche memoria bruta [bd, ed) com copias de init
template <class A, class IIt, class T>
void preencher_nao_inicializado_com_alocador(
   A &alloc, IIt bd, IIt ed, T init
) {
   auto p = bd;
   try {
      for (; p != ed; ++p)
         alloc.construct(p, init);
   } catch (...) {
      for (auto q = bd; q != p; ++q)
         alloc.destroy(q);
      throw;
   }
}

// copia sequencia de objetos [bs, es) para memoria bruta a partir de bd
template <class A, class IIt, class OIt>
void copiar_nao_inicializado_com_alocador(
   A &alloc, IIt bs, IIt es, OIt bd
) {
   auto p = bd;
   try {
      for (auto q = bs; q != es; ++q) {
         alloc.construct(p, *q);
         ++p;
      }
   } catch (...) {
      for (auto q = bd; q != p; ++q)
         alloc.destroy(q);
      throw;
   }
}

// move sequencia de objetos [bs, es) para memoria bruta a partir de bd
template <class A, class IIt, class OIt>
void mover_nao_inicializado_com_alocador(
   A &alloc, IIt bs, IIt es, OIt bd
) {
   auto p = bd;
   try {
      for (auto q = bs; q != es; ++q) {
         alloc.construct(p, std::move(*q));
         ++p;
      }
   } catch (...) {
      for (auto q = bd; q != p; ++q)
         alloc.destroy(q);
      throw;
   }
}

// destroi sequencia de objetos [b, e) usando o allocator
template <class A, class It>
void destruir_com_alocador(A &alloc, It b, It e) {
   for (; b != e; ++b)
      alloc.destroy(b);
}

// comparacao segura entre inteiros com e sem sinal (necessaria antes do C++20)
template<class T, class U>
constexpr bool menor_que(T a, U b) noexcept {
   if constexpr (std::is_signed_v<T> == std::is_signed_v<U>)
      return a < b;
   else if constexpr (std::is_signed_v<T>)
      return a < 0 || std::make_unsigned_t<T>(a) < b;
   else
      return b >= 0 && a < std::make_unsigned_t<U>(b);
}
```

## Uma classe Vector\<T,A\> consciente de allocator

Agora estamos prontos para ver como introduzir consciencia de *allocator* em um container de memoria contigua. Note que usaremos a abordagem de gerenciamento explicito de memoria do Capitulo 12 como base:

```cpp
template <class T, class A = std::allocator<T>>
class Vector : A {  // heranca privada (empty base optimization)
public:
   using value_type      = typename A::value_type;
   using size_type       = typename A::size_type;
   using pointer         = typename A::pointer;
   using const_pointer   = typename A::const_pointer;
   using reference       = typename A::reference;
   using const_reference = typename A::const_reference;
private:
   // expoe seletivamente membros da classe base privada como proprios
   using A::allocate;
   using A::deallocate;
   using A::construct;
   using A::destroy;

   pointer   dados{};
   size_type nelem{}, cap{};
   // ...
```

Note as tecnicas:
- Usamos heranca privada tornando `A` uma classe base de `Vector<T,A>`, habilitando a *empty base optimization* para *allocators* sem estado.
- Deduzimos os *type aliases* do container a partir dos do seu *allocator*.
- Expusemos seletivamente membros da classe base privada como proprios, tornando o codigo menos verboso.

**Construtor com allocator (util para allocators com estado):**

```cpp
   Vector(A &alloc) : A{ alloc } { }
```

**Construtor com n elementos e valor inicial:**

```cpp
   Vector(size_type n, const_reference init)
      : A{}, dados{ allocate(n) }, nelem{ n }, cap{ n } {
      try {
         preencher_nao_inicializado_com_alocador(
            *static_cast<A*>(this), begin(), end(), init
         );
      } catch (...) {
         deallocate(dados, capacity());
         throw;
      }
   }
```

**Destrutor:**

```cpp
   ~Vector() {
      destruir_com_alocador(*static_cast<A*>(this), begin(), end());
      deallocate(dados, capacity());
   }
```

**`push_back()` e `emplace_back()`:**

```cpp
   void push_back(const_reference val) {
      if (cheio()) crescer();
      construct(end(), val);
      ++nelem;
   }
   void push_back(T &&val) {
      if (cheio()) crescer();
      construct(end(), std::move(val));
      ++nelem;
   }
   template <class ... Args>
   reference emplace_back(Args &&...args) {
      if (cheio()) crescer();
      construct(end(), std::forward<Args>(args)...);
      ++nelem;
      return back();
   }
```

**`reserve()` e `resize()`:**

```cpp
   void reserve(size_type novo_cap) {
      if (novo_cap <= capacity()) return;
      auto p = allocate(novo_cap);
      if constexpr (std::is_nothrow_move_assignable_v<T>) {
         mover_nao_inicializado_com_alocador(
            *static_cast<A*>(this), begin(), end(), p
         );
      } else {
         try {
            copiar_nao_inicializado_com_alocador(
               *static_cast<A*>(this), begin(), end(), p
            );
         } catch (...) {
            deallocate(p, novo_cap);
            throw;
         }
      }
      deallocate(dados, capacity());
      dados = p;
      cap = novo_cap;
   }

   void resize(size_type novo_cap) {
      if (novo_cap <= capacity()) return;
      auto p = allocate(novo_cap);
      if constexpr (std::is_nothrow_move_assignable_v<T>) {
         mover_nao_inicializado_com_alocador(
            *static_cast<A*>(this), begin(), end(), p
         );
      } else {
         copiar_nao_inicializado_com_alocador(
            *static_cast<A*>(this), begin(), end(), p
         );
      }
      try {
         preencher_nao_inicializado_com_alocador(
            *static_cast<A*>(this),
            p + size(), p + novo_cap, value_type{}
         );
         destruir_com_alocador(*static_cast<A*>(this), begin(), end());
         deallocate(dados, capacity());
         dados = p;
         nelem = cap = novo_cap;
      } catch(...) {
         destruir_com_alocador(*static_cast<A*>(this), p, p + size());
         deallocate(p, novo_cap);
         throw;
      }
   }
```

**`erase()`:**

```cpp
   iterator erase(const_iterator pos) {
      iterator pos_ = const_cast<iterator>(pos);
      if (pos_ == end()) return pos_;
      std::copy(std::next(pos_), end(), pos_);
      destroy(std::prev(end()));   // usa o allocator para destruir o ultimo
      --nelem;
      return pos_;
   }
};
```

## Uma classe ForwardList\<T,A\> consciente de allocator

Acontece algo interessante quando se escreve containers baseados em nos conscientes de *allocator*:

```cpp
template <class T, class A = std::allocator<T>>
class ForwardList {
   struct No {
      value_type valor;
      No *proximo = nullptr;
      // ...
   };
   // ...
```

O tipo `A` e o tipo *errado*! Um container baseado em nos como `ForwardList<T,A>` nunca aloca objetos do tipo `T` — ele aloca nos que (provavelmente) contêm objetos `T` e outras coisas, como um ponteiro para o proximo `No`. Se tivessemos um *allocator* consciente do tamanho de `T`, e nao de `No`, acabariamos com uma arena que gerencia objetos do tamanho errado.

E aqui que `rebind` entra. Precisamos de um tipo que seja "como `A`, mas aplicado ao tipo `No` em vez de `T`":

```cpp
   using TipoAlloc = typename A::rebind<No>::other;
   TipoAlloc alloc;
```

Com `alloc` disponivel, adaptamos os membros:

```cpp
   void clear() noexcept {
      for(auto p = inicio; p; ) {
         auto q = p->proximo;
         alloc.destroy(p);
         alloc.deallocate(p, 1);
         p = q;
      }
      nelem = 0;
   }

   template <class It>
      ForwardList(It b, It e) {
         if(b == e) return;
         try {
            inicio = alloc.allocate(1);
            alloc.construct(inicio, *b);
            auto q = inicio;
            ++nelem;
            for(++b; b != e; ++b) {
               auto ptr = alloc.allocate(1);
               alloc.construct(ptr, *b);
               q->proximo = ptr;
               q = q->proximo;
               ++nelem;
            }
         } catch (...) {
            clear();
            throw;
         }
      }

   void push_front(const_reference val) {
      auto p = alloc.allocate(1);
      alloc.construct(p, val);
      p->proximo = inicio;
      inicio = p;
      ++nelem;
   }

   iterator insert_after(iterator pos, const_reference valor) {
      auto p = alloc.allocate(1);
      alloc.construct(p, valor);
      p->proximo = pos.atual->proximo;
      pos.atual->proximo = p;
      ++nelem;
      return { p };
   }

   iterator erase_after(iterator pos) {
      if (pos == end() || std::next(pos) == end())
         return end();
      auto p = pos.atual->proximo->proximo;
      alloc.destroy(pos.atual->proximo);
      alloc.deallocate(pos.atual->proximo, 1);
      --nelem;
      pos.atual->proximo = p;
      return { p->proximo };
   }
};
```

## Exemplo de uso — um allocator de buffer sequencial

Um exemplo classico de uso de *allocator* e um que, em vez de alocar memoria do *free store*, gerencia um pedaco de memoria pre-alocado (tipicamente na pilha de execucao):

```cpp
#include <cstdint>
template <class T>
struct alocador_buf_sequencial {
   using value_type = T;
   using pointer    = T*;
   using size_type  = std::size_t;
   // outros aliases e max_size() como de costume

private:
   char    *buf;   // inicio do buffer
   pointer  cur;   // posicao atual
   size_type cap;  // capacidade total

public:
   alocador_buf_sequencial(char *buf, size_type cap) noexcept
      : buf{ buf }, cap{ cap } {
      cur = reinterpret_cast<pointer>(buf);
   }

   pointer allocate(size_type n) {
      auto pedido = reinterpret_cast<std::intptr_t>(cur + n),
           limite = reinterpret_cast<std::intptr_t>(buf + cap);
      if(pedido >= limite)
         throw std::bad_alloc{};
      auto q = cur;
      cur += n;
      return q;
   }

   void deallocate(pointer, size_type) {
      // nao reutiliza memoria — no-op intencional
   }

   // rebind, address(), construct(), destroy() como de costume

   template <class U>
   struct rebind { using other = alocador_buf_sequencial<U>; };
};

template <class T, class U>
constexpr bool operator==(
   const alocador_buf_sequencial<T> &a,
   const alocador_buf_sequencial<U> &b) { return a.cur == b.cur; }
template <class T, class U>
constexpr bool operator!=(
   const alocador_buf_sequencial<T> &a,
   const alocador_buf_sequencial<U> &b) { return !(a == b); }
```

Demonstracao de uso com benchmark:

```cpp
#include <chrono>
#include <vector>
#include <iostream>

struct Dado { int n; };

int main() {
   using namespace std::chrono;
   enum { N = 500'000 };

   // versao padrao
   {
      std::vector<Dado> v;
      v.reserve(N);
      auto inicio = high_resolution_clock::now();
      for(int i = 0; i != N; ++i)
         v.push_back({ i + 1 });
      auto fim = high_resolution_clock::now();
      std::cout << "vector<Dado> padrao: "
                << duration_cast<microseconds>(fim - inicio).count()
                << " us\n";
   }

   // versao com buffer na pilha
   {
      alignas(Dado) char buf[N * sizeof(Dado)];
      alocador_buf_sequencial<Dado> alloc{ buf, sizeof buf };
      std::vector<Dado, alocador_buf_sequencial<Dado>> v(alloc);
      v.reserve(N);
      auto inicio = high_resolution_clock::now();
      for(int i = 0; i != N; ++i)
         v.push_back({ i + 1 });
      auto fim = high_resolution_clock::now();
      std::cout << "vector<Dado, alocador_buf_sequencial>: "
                << duration_cast<microseconds>(fim - inicio).count()
                << " us\n";
   }
}
```

## Allocators tradicionais com padroes contemporaneos

Os *allocators* da secao anterior nao compilam como escritos em um compilador C++20, pois a API evoluiu. Podemos ainda escreve-los, mas de forma mais simples.

### C++11 e std::allocator_traits\<A\>

O C++11 introduziu `std::allocator_traits<A>`. A ideia e que, dado um `typename A::value_type`, pode-se gerar uma implementacao padrao razoavel e eficiente para a maioria dos servicos do *allocator* — incluindo *type aliases* como `pointer` ou `size_type` — desde que se fornecem implementacoes para `allocate()` e `deallocate()`.

`small_allocator<T>` pode agora ser expresso assim:

```cpp
template <class T>
struct small_allocator {
   using value_type = T;
   T* allocate(std::size_t n) {
      auto p = static_cast<T*>(malloc(n * sizeof(value_type)));
      if (!p) throw std::bad_alloc{};
      return p;
   }
   void deallocate(T *p, std::size_t) { free(p); }
};
// insira os operadores de igualdade aqui
```

E `alocador_buf_sequencial<T>` fica assim:

```cpp
template <class T>
struct alocador_buf_sequencial {
   using value_type = T;
   using pointer    = T*;
   using size_type  = std::size_t;
   char    *buf;
   pointer  cur;
   size_type cap;

   alocador_buf_sequencial(char *buf, size_type cap) noexcept
      : buf{ buf }, cap{ cap } {
      cur = reinterpret_cast<pointer>(buf);
   }
   pointer allocate(size_type n) {
      auto pedido = reinterpret_cast<std::intptr_t>(cur + n),
           limite = reinterpret_cast<std::intptr_t>(buf + cap);
      if (pedido > limite) throw std::bad_alloc{};
      auto q = cur;
      cur += n;
      return q;
   }
   void deallocate(pointer, size_type) { }
};
// insira os operadores de igualdade aqui
```

Com *traits*, um container como `Vector<T,A>` usa `std::allocator_traits<A>` em vez de `A` diretamente:

```cpp
template <class T, class A = std::allocator<T>>
class Vector : A {
public:
   using value_type  = typename std::allocator_traits<A>::value_type;
   using size_type   = typename std::allocator_traits<A>::size_type;
   using pointer     = typename std::allocator_traits<A>::pointer;
   using const_pointer = typename std::allocator_traits<A>::const_pointer;
   using reference       = value_type&;
   using const_reference = const value_type&;
   // ...

   Vector(size_type n, const_reference init)
      : A{},
        dados{ std::allocator_traits<A>::allocate(
           static_cast<A&>(*this), n)
        },
        nelem{ n }, cap{ n } {
      try {
         // ... usa std::allocator_traits<A>::construct(...)
      } catch (...) {
         std::allocator_traits<A>::deallocate(
            static_cast<A&>(*this), dados, capacity()
         );
         throw;
      }
   }
   // ...
```

Para `ForwardList<T,A>` com *traits*, o *rebind* fica:

```cpp
   using TipoAlloc = typename std::allocator_traits<A>::
      template rebind_alloc<No>;
   TipoAlloc alloc;
```

E os membros usam `std::allocator_traits<TipoAlloc>`:

```cpp
   void clear() noexcept {
      for(auto p = inicio; p; ) {
         auto q = p->proximo;
         std::allocator_traits<TipoAlloc>::destroy(alloc, p);
         std::allocator_traits<TipoAlloc>::deallocate(alloc, p, 1);
         p = q;
      }
      nelem = 0;
   }
```

### Gerenciamento do tempo de vida do allocator tradicional

`std::allocator_traits<A>` introduziu tres *type aliases* que informam aos containers o que deve ser feito com o *allocator* em momentos-chave:

- **POCCA** (`propagate_on_container_copy_assignment`): se o *allocator* deve ser copiado na atribuicao de copia do container.
- **POCMA** (`propagate_on_container_move_assignment`): se o *allocator* deve ser movido na atribuicao de movimentacao do container.
- **POCS** (`propagate_on_container_swap`): se o *allocator* deve ser trocado no `swap()` do container.

Todos equivalem a `std::false_type` por padrao, pois *allocators* nao estao, por padrao, destinados a ser copiados ou movidos.

Tambem foi introduzido `select_on_container_copy_construction()`, uma funcao membro estatica que deve ser usada no construtor de copia de um container para obter o *allocator* adequado.

## Irritantes dos allocators tradicionais

A abordagem tradicional e otima em tempo de execucao, mas tem custos:

- A implementacao de um container pode se tornar mais complexa devido ao layering em tempo de compilacao;
- Dois containers identicos em quase tudo, mas que diferem no *allocator*, serao na pratica tipos diferentes, o que pode tornar algumas operacoes mais complicadas (como a comparacao entre `Vector<T,A0>` e `Vector<T,A1>`);
- O *allocator* e parte do tipo do container, mas muitas operacoes sao relacionadas a `value_type` e nao tem nada a ver com *allocators*.

## Allocators de recursos de memoria polimorfica (PMR)

Com o C++17, a linguagem C++ adicionou os chamados *allocators* PMR. Um container PMR armazena informacoes de *allocator* como um valor em tempo de execucao, nao como parte de seu tipo em tempo de compilacao. Nesse modelo, um container PMR armazena um ponteiro para um *allocator* PMR, reduzindo o numero de tipos necessarios, mas adicionando chamadas a funcoes virtuais sempre que usa servicos de alocacao de memoria.

Trocas em relacao ao modelo tradicional:

- Containers PMR geralmente sao maiores que seus equivalentes nao-PMR;
- Cada chamada de alocacao ou desalocacao incorre em um custo de indirecao polirmofica;
- Todos os containers PMR simplesmente armazenam um ponteiro para `std::pmr::memory_resource`, portanto o tipo do container nao e influenciado pelo tipo do seu *allocator*.

### Exemplo simples com buffer monotone

```cpp
#include <print>
#include <vector>
#include <memory_resource>

int main() {
   enum { N = 10'000 };
   alignas(int) char buf[N * sizeof(int)]{};
   // recurso que gerencia um buffer contiguo de forma sequencial
   std::pmr::monotonic_buffer_resource res{
      std::begin(buf), std::size(buf)
   };
   std::pmr::vector<int> v{ &res };
   v.reserve(N);
   for (int i = 0; i != N; ++i)
      v.emplace_back(i + 1);
   // itera pelo container e pelo buffer raw — resultados identicos
   for (auto n : v) std::print("{} ", n);
   std::print("\n");
   for (char *p = buf; p != buf + std::size(buf); p += sizeof(int))
      std::print("{} ", *reinterpret_cast<int*>(p));
}
```

Este programa nao aloca nem um unico byte do *free store* para armazenar os objetos `int`.

### Allocators aninhados — propagacao automatica

Os *allocators* PMR propagam estrategias de alocacao por padrao:

```cpp
#include <print>
#include <vector>
#include <string>
#include <memory_resource>

int main() {
   auto criar_str = [](const char *p, int n) -> std::pmr::string {
      auto s = std::string{ p } + std::to_string(n);
      return { std::begin(s), std::end(s) };
   };
   enum { N = 2'000 };
   alignas(std::pmr::string) char buf[N]{};
   std::pmr::monotonic_buffer_resource res{
      std::begin(buf), std::size(buf)
   };
   // tanto o vector quanto as strings usam o mesmo recurso de memoria!
   std::pmr::vector<std::pmr::string> v{ &res };
   for (int i = 0; i != 10; ++i)
      v.emplace_back(criar_str("Eu amo C++ ", i));
   for (const auto &s : v) std::print("{} ", s);
}
```

A propagacao da estrategia de alocacao do container externo para os containers internos e implicita.

### Rastreando alocacoes com tracing_resource

Como terceiro e ultimo exemplo de uso de PMR, implementamos um recurso de rastreamento para entender as escolhas de implementacao de um container:

```cpp
#include <print>
#include <vector>
#include <memory_resource>

class recurso_rastreador : public std::pmr::memory_resource {
   void* do_allocate(
      std::size_t bytes, std::size_t alignment
   ) override {
      std::print("do_allocate de {} bytes\n", bytes);
      return upstream->allocate(bytes, alignment);
   }
   void do_deallocate(
      void *p, std::size_t bytes, std::size_t alignment
   ) override {
      std::print("do_deallocate de {} bytes\n", bytes);
      return upstream->deallocate(p, bytes, alignment);
   }
   bool do_is_equal(
      const std::pmr::memory_resource &outro
   ) const noexcept override {
      return upstream->is_equal(outro);
   }
   std::pmr::memory_resource *upstream;
public:
   recurso_rastreador(std::pmr::memory_resource *up) noexcept
      : upstream{ up } { }
};

int main() {
   enum { N = 100 };
   recurso_rastreador rastreador{
      std::pmr::new_delete_resource()
   };
   std::pmr::vector<int> v{ &rastreador };
   for (int i = 0; i != N; ++i)
      v.emplace_back(i + 1);
   for (auto s : v) std::print("{} ", s);
}
```

Ao executar este programa, voce desenvolvera uma intuicao para a estrategia de crescimento da implementacao `std::pmr::vector` da sua biblioteca padrao (tipicamente fator 1,5 ou 2).

### Recursos PMR da biblioteca padrao

O cabecalho `<memory_resource>` oferece:

- **`std::pmr::new_delete_resource()`** — aloca/desaloca via `::operator new`/`::operator delete`;
- **`std::pmr::monotonic_buffer_resource`** — alocacao sequencial em buffer existente;
- **`std::pmr::synchronized_pool_resource`** / **`std::pmr::unsynchronized_pool_resource`** — pools de blocos de varios tamanhos (use `synchronized` para codigo multithread);
- **`std::pmr::null_memory_resource()`** — sempre lanca `std::bad_alloc` (util como recurso *upstream* para garantir que nao ocorram alocacoes de fallback);
- **`std::pmr::get_default_resource()`** / **`std::pmr::set_default_resource()`** — obtem/substitui o recurso de memoria padrao do programa.

## Resumo

Este foi um capitulo denso! Apos nos aventurarmos em implementacoes de alocacao explicita e implicita nos Capitulos 12 e 13, este capitulo explorou os *allocators* e como essas facilidades nos permitem personalizar o comportamento de alocacao de memoria em containers para corresponder as nossas necessidades.

Vimos como um *allocator* tradicional, encapsulado no tipo do seu container, pode ser implementado e usado — tanto para um container de memoria contigua quanto para um container baseado em nos. Vimos tambem como a tarefa de escrever (e usar) tais *allocators* evoluiu ao longo dos anos para se tornar os *allocators* baseados em *traits* contemporaneos.

Em seguida, vimos o modelo de *allocator* PMR mais recente, que representa uma abordagem diferente para alocacao de memoria, e discutimos suas vantagens e desvantagens.

No proximo capitulo, olharemos para alguns problemas contemporaneos de alocacao de memoria em C++ e comecamos a pensar no que nos aguarda no futuro proximo.
