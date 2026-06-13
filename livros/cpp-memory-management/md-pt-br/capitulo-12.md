# Capitulo 12 - Escrevendo Containers Genericos com Gerenciamento Explicito de Memoria

> Traducao nao oficial do livro *C++ Memory Management* (Patrice Roy, 2025).
> Texto adaptado para portugues brasileiro com fins educacionais. Paginas originais: 263-301.
> Repositorio: [ring0-study](https://github.com/cnthigu/ring0-study)

> Navegacao: [Anterior](capitulo-11.md) | [Indice](README.md) | [Proximo](capitulo-13.md)

---

Percorremos um longo caminho desde o inicio da nossa jornada pelos mecanismos e tecnicas de gerenciamento de memoria em C++. Do Capitulo 4 ao Capitulo 7, construimos uma caixa de ferramentas interessante, a partir da qual podemos construir e adaptar para resolver novos problemas que enfrentaremos no futuro. Esta caixa de ferramentas agora contem, entre outras coisas, o seguinte:

- Tecnicas pelas quais um objeto gerencia implicitamente seus recursos;
- Tipos que se comportam como ponteiros, mas codificam a responsabilidade sobre o *pointee* no sistema de tipos;
- Varias formas pelas quais podemos assumir o controle do comportamento dos mecanismos de alocacao de memoria de um programa.

Um aspecto importante do gerenciamento de memoria que ainda nao cobrimos e como os containers gerenciam memoria. Este e um topico bastante interessante, que abordaremos por tres angulos diferentes, em tres capitulos diferentes.

O primeiro angulo e como lidar com o gerenciamento de memoria de forma explicita, porem eficiente, em um container. E o que trata o capitulo atual. Em alguns dominios de aplicacao, e costume implementar (ou manter) os proprios containers em vez de usar os fornecidos pela biblioteca padrao. Pode haver varias razoes para isso: talvez sua empresa tenha necessidades altamente especializadas, ou talvez haja insatisfacao com o desempenho dos containers da biblioteca padrao.

O segundo angulo, um pouco mais curto, e como lidar com a memoria de forma implicita, porem eficiente, num container, e sera coberto no Capitulo 13. O terceiro angulo, mais complexo e sutil, e como lidar com a memoria por meio de um *allocator* num container, e formara o Capitulo 14.

Neste capitulo, escreveremos um `Vector<T>` similar a `std::vector<T>`. Usaremos isso como oportunidade para discutir seguranca de excecoes. Em seguida, notaremos que fomos muito ineficientes ate aquele ponto — `std::vector<T>` sera significativamente mais eficiente do que nosso `Vector<T>` para alguns tipos. Baseando-nos nessa realizacao, revisitaremos nosso design com melhor gerenciamento de memoria.

Tambem escreveremos um `ForwardList<T>` similar a `std::forward_list<T>`, pois ha questoes e consideracoes especificas de containers baseados em nos que um tipo semelhante a vetor nao nos permite discutir.

Apos ler este capitulo, voce sera capaz de:

- Escrever um container correto e seguro para excecoes com tecnicas ingenuas de gerenciamento de memoria;
- Entender os problemas associados a membros `const` ou de referencia;
- Usar algoritmos de gerenciamento de memoria de baixo nivel fornecidos pela biblioteca padrao.

## Escrevendo seu proprio vetor

Suponha que um dia voce acorde e diga: "Vou superar `std::vector` no seu proprio jogo" e comece a codificar confiante. Algumas palavras de sabedoria:

- Esta tarefa aparentemente simples e surpreendentemente dificil: `std::vector` e uma obra de arte;
- Voce pode ainda achar que consegue, entao tudo bem tentar, mas certifique-se de testar suas ideias com um tipo de elemento trivialmente construivel (por exemplo, `int`) e um que nao seja (por exemplo, `std::string`) e compare os resultados;
- A razao desta diferenca e que um container como `std::vector` e extremamente eficiente no gerenciamento de memoria.

### Escolhas de representacao para um container de elementos contiguos

Informalmente, um `std::vector` representa um array alocado dinamicamente que pode crescer conforme necessario. Chamaremos nossa versao artesanal de `Vector<T>` para torna-la visivelmente distinta de `std::vector<T>`.

A primeira ideia-chave para uma implementacao razoavelmente eficiente e distinguir tamanho de capacidade. Ha duas abordagens principais para a representacao interna:

**Abordagem 1 — tres ponteiros:**

```cpp
template <class T>
   class Vector {
      T *dados;           // inicio do armazenamento alocado
      T *fim_elementos;   // apos o ultimo elemento
      T *fim_armazenamento; // apos o ultimo byte alocado
      // ...
```

**Abordagem 2 — ponteiro + dois inteiros:**

```cpp
template <class T>
   class Vector {
      T *dados;              // inicio do armazenamento alocado
      std::size_t nelem;     // numero de elementos
      std::size_t cap;       // capacidade total
      // ...
```

Essas sao representacoes equivalentes, porem com trocas diferentes. Neste livro, usaremos a abordagem de "um ponteiro e dois inteiros".

### A implementacao de Vector\<T\>

Percorreremos nossa implementacao inicial (ingenua) passo a passo. O passo inicial define nossas abstracoes por meio de *type aliases* conformes com a biblioteca padrao e escolhe nossa representacao interna:

```cpp
#include <cstddef>
#include <algorithm>
#include <utility>
#include <initializer_list>
#include <iterator>
#include <type_traits>

template <class T>
   class Vector {
   public:
      using value_type      = T;
      using size_type       = std::size_t;
      using pointer         = T*;
      using const_pointer   = const T*;
      using reference       = T&;
      using const_reference = const T&;
   private:
      pointer   dados{};
      size_type nelem{},
                cap{};
      // ...
```

Algumas funcoes membro simples, porem fundamentais:

```cpp
   // ...
   public:
      size_type size()     const { return nelem; }
      size_type capacity() const { return cap; }
      bool empty()         const { return size() == 0; }
   private:
      bool cheio() const { return size() == capacity(); }
   // ...
```

> Observe o uso de `empty()` e `cheio()`. Preferimos reusar as funcoes membro mais fundamentais para implementar as "sinteticas". Isso torna o codigo menos sensivel a mudancas de implementacao, e compiladores C++ sao muito bons em *inlining*, especialmente para funcoes nao virtuais.

### Iteradores

Containers C++ geralmente expoe iteradores como parte de sua interface. Para `Vector<T>`, como os elementos sao contiguos, um ponteiro bruto ja e um iterador valido:

```cpp
      // ...
   public:
      using iterator       = pointer;
      using const_iterator = const_pointer;
      iterator begin()             { return dados; }
      const_iterator begin() const { return dados; }
      iterator end()               { return begin() + size(); }
      const_iterator end() const   { return begin() + size(); }
      const_iterator cend()   const { return end(); }
      const_iterator cbegin() const { return begin(); }
      // ...
```

Com um compilador C++23, podemos usar o recurso "deduced this" para coalescer as versoes `const` e nao-`const`:

```cpp
      // abordagem alternativa (requer C++23)
      template <class S>
         auto begin(this S && self) { return self.dados; }
      template <class S>
         auto end(this S && self) {
            return self.begin() + self.size();
         }
```

### Construtores e outras funcoes membro especiais

```cpp
      // ...
      Vector() = default;
      Vector(size_type n, const_reference init)
         : dados{ new value_type[n] },
           nelem{ n }, cap{ n } {
         try {
            std::fill(begin(), end(), init);
         } catch(...) {
            delete [] dados;
            throw;
         }
      }
      // ...
```

Preste atencao ao codigo de tratamento de excecoes: estamos escrevendo um container generico, entao usamos algum tipo `T` que nao conhecemos previamente. Como `std::fill()` atribui valores de `T`, se uma atribuicao lancar uma excecao, precisamos garantir que o array seja destruido e desalocado; caso contrario, vazaremos a memoria.

O construtor de copia segue um padrao similar:

```cpp
      // ...
      Vector(const Vector &outro)
         : dados{ new value_type[outro.size()] },
           nelem{ outro.size() }, cap{ outro.size() } {
         try {
            std::copy(outro.begin(), outro.end(), begin());
         } catch(...) {
            delete [] dados;
            throw;
         }
      }
      Vector(std::initializer_list<value_type> lista)
         : dados{ new value_type[lista.size()] },
           nelem{ lista.size() }, cap{ lista.size() } {
         try {
            std::copy(lista.begin(), lista.end(), begin());
         } catch(...) {
            delete [] dados;
            throw;
         }
      }
      Vector(Vector &&outro) noexcept
         : dados{ std::exchange(outro.dados, nullptr) },
           nelem{ std::exchange(outro.nelem, 0) },
           cap{ std::exchange(outro.cap, 0) } {
      }
      // ...
```

O destrutor precisa liberar o array alocado:

```cpp
      // ...
      ~Vector() {
         delete [] dados;
      }
      // ...
```

A atribuicao pode ser expressa pelo idioma de atribuicao segura:

```cpp
      // ...
      void trocar(Vector &outro) noexcept {
         using std::swap;
         swap(dados, outro.dados);
         swap(nelem, outro.nelem);
         swap(cap, outro.cap);
      }
      Vector& operator=(const Vector &outro) {
         Vector{ outro }.trocar(*this);
         return *this;
      }
      Vector& operator=(Vector &&outro) noexcept {
         Vector{ std::move(outro) }.trocar(*this);
         return *this;
      }
      // ...
```

### Crescimento e redimensionamento

```cpp
      // ...
   private:
      void crescer() {
         redimensionar(capacity() ? capacity() * 2 : 16);
      }
   public:
      void resize(size_type novo_cap) {
         if (novo_cap <= capacity()) return;
         auto p = new value_type[novo_cap];
         try {
            std::copy(begin(), end(), p);
         } catch(...) {
            delete [] p;
            throw;
         }
         delete [] dados;
         dados = p;
         cap = novo_cap;
      }
      // ...
```

### Insercao e remocao de elementos

```cpp
      // ...
      reference back() { return *(end() - 1); }
      const_reference back() const { return *(end() - 1); }

      void push_back(const_reference val) {
         if (cheio()) crescer();
         *(end()) = val;
         ++nelem;
      }

      template <class It>
      iterator insert(const_iterator pos, It first, It last) {
         iterator pos_ = const_cast<iterator>(pos);
         const std::size_t restante = capacity() - size();
         const std::size_t n = std::distance(first, last);
         if (restante < n) {
            auto indice = std::distance(begin(), pos_);
            resize(capacity() + n - restante);
            pos_ = std::next(begin(), indice);
         }
         std::copy_backward(pos_, end(), end() + n);
         std::copy(first, last, pos_);
         nelem += n;
         return pos_;
      }

      iterator erase(const_iterator pos) {
         iterator pos_ = const_cast<iterator>(pos);
         if (pos_ == end()) return pos_;
         std::copy(std::next(pos_), end(), pos_);
         *std::prev(end()) = {};
         --nelem;
         return pos_;
      }
      // ...
```

## Escrevendo seu proprio forward_list\<T\>

Escrever um container baseado em nos, como `std::list` ou `std::forward_list`, e um exercicio interessante. Uma lista encadeada simples e um exercicio de leveza: queremos que o tipo seja pequeno e faca bem o que faz. Em nossa implementacao, `ForwardList<T>` mantera nos, e cada no mantera um par composto por um valor (do tipo `T`) e um ponteiro para o proximo no na sequencia.

### Implementacao de ForwardList\<T\>

```cpp
#include <cstddef>
#include <algorithm>
#include <utility>
#include <iterator>
#include <initializer_list>
#include <concepts>

template <class T>
class ForwardList {
public:
   using value_type      = T;
   using size_type       = std::size_t;
   using pointer         = T*;
   using const_pointer   = const T*;
   using reference       = T&;
   using const_reference = const T&;
   // ...
```

O `No` de `ForwardList<T>` mantém um valor e um ponteiro para o proximo:

```cpp
   // ...
private:
   struct No {
      value_type valor;
      No *proximo = nullptr;
      No(const_reference v) : valor{ v } { }
      No(value_type &&v) : valor{ std::move(v) } { }
   };
   No *inicio {};
   size_type nelem {};
   // ...
```

As funcoes `size()` e `empty()` sao triviais:

```cpp
   // ...
public:
   size_type size()  const { return nelem; }
   bool empty() const { return !inicio; }
   // ...
```

### Iteradores da lista encadeada

Os iteradores de uma lista encadeada nao podem ser ponteiros brutos, pois os elementos nao sao contiguos. Nossa classe privada `ForwardList<T>::Iterador<U>` sera template no tipo `U`:

```cpp
   // ...
private:
   template <class U> class Iterador {
   public:
      using value_type        = typename ForwardList<T>::value_type;
      using pointer           = typename ForwardList<T>::pointer;
      using reference         = typename ForwardList<T>::reference;
      using difference_type   = std::ptrdiff_t;
      using iterator_category = std::forward_iterator_tag;
      friend class ForwardList<T>;
   private:
      No *atual {};
   public:
      Iterador() = default;
      Iterador(No *p) : atual{ p } { }
      Iterador& operator++() { atual = atual->proximo; return *this; }
      Iterador operator++(int) { auto temp = *this; operator++(); return temp; }
      bool operator==(const Iterador &outro) const { return atual == outro.atual; }
      bool operator!=(const Iterador &outro) const { return !(*this == outro); }
      U& operator*()        { return atual->valor; }
      const U& operator*() const { return atual->valor; }
      U* operator->()       { return &atual->valor; }
      const U* operator->() const { return &atual->valor; }
   };
public:
   using iterator       = Iterador<T>;
   using const_iterator = Iterador<const T>;
   // ...
```

As funcoes `begin()` e `end()`:

```cpp
   // ...
   iterator begin()             { return { inicio }; }
   const_iterator begin() const { return { inicio }; }
   const_iterator cbegin() const { return begin(); }
   iterator end()               { return {}; }
   const_iterator end() const   { return {}; }
   const_iterator cend() const  { return end(); }
   // ...
```

### Destrucao e limpeza

```cpp
   // ...
   void clear() noexcept {
      for(auto p = inicio; p; ) {
         auto q = p->proximo;
         delete p;
         p = q;
      }
      nelem = 0;
   }
   ~ForwardList() { clear(); }
   // ...
```

> **Por que nao usar destrutor recursivo em `No`?**
>
> Seria tentador escrever um destrutor em `No` que chame `delete proximo`, tornando `clear()` apenas `delete inicio`. Porem, (a) a responsabilidade de organizar nos deve ser da lista, nao dos nos; (b) chamadas recursivas de destrutores podem causar estouro de pilha em listas longas.

### Construtores da lista

```cpp
   // ...
   ForwardList() = default;
   template <std::input_iterator It>
      ForwardList(It b, It e) {
         if(b == e) return;
         try {
            inicio = new No{ *b };
            auto q = inicio;
            ++nelem;
            for(++b; b != e; ++b) {
               q->proximo = new No{ *b };
               q = q->proximo;
               ++nelem;
            }
         } catch (...) {
            clear();
            throw;
         }
      }
   ForwardList(const ForwardList &outro)
      : ForwardList(outro.begin(), outro.end()) { }
   ForwardList(std::initializer_list<T> lista)
      : ForwardList(lista.begin(), lista.end()) { }
   ForwardList(ForwardList &&outro) noexcept
      : inicio{ std::exchange(outro.inicio, nullptr) },
        nelem{ std::exchange(outro.nelem, 0) } { }
   // ...
```

Atribuicao via idioma de atribuicao segura:

```cpp
   // ...
   void trocar(ForwardList &outro) noexcept {
      using std::swap;
      swap(inicio, outro.inicio);
      swap(nelem, outro.nelem);
   }
   ForwardList& operator=(const ForwardList &outro) {
      ForwardList{ outro }.trocar(*this);
      return *this;
   }
   ForwardList& operator=(ForwardList &&outro) {
      ForwardList{ std::move(outro) }.trocar(*this);
      return *this;
   }
   // ...
```

### Operacoes sobre elementos

```cpp
   // ...
   reference front()             { return inicio->valor; }
   const_reference front() const { return inicio->valor; }

   bool operator==(const ForwardList &outro) const {
      return size() == outro.size() &&
             std::equal(begin(), end(), outro.begin());
   }
   bool operator!=(const ForwardList &outro) const {
      return !(*this == outro);
   }

   void push_front(const_reference val) {
      auto p = new No{ val };
      p->proximo = inicio;
      inicio = p;
      ++nelem;
   }
   void push_front(T&& val) {
      auto p = new No{ std::move(val) };
      p->proximo = inicio;
      inicio = p;
      ++nelem;
   }

   iterator insert_after(iterator pos, const_reference val) {
      auto p = new No{ val };
      p->proximo = pos.atual->proximo;
      pos.atual->proximo = p;
      ++nelem;
      return { p };
   }

   iterator erase_after(iterator pos) {
      if (pos == end() || std::next(pos) == end())
         return end();
      auto p = pos.atual->proximo->proximo;
      delete pos.atual->proximo;
      pos.atual->proximo = p;
      return { p };
   }
};
```

## Melhor gerenciamento de memoria

Agora o escritor humilde afirma que nosso bom mas simples `Vector<T>` nao e rival de `std::vector<T>`. Isso pode parecer uma afirmacao ousada — afinal, parecemos fazer o necessario e usamos algoritmos em vez de loops brutos. O que estamos fazendo de errado?

Se voce rodar *benchmarks* comparativos entre `Vector<int>` e `std::vector<int>`, provavelmente nao notara muita diferenca. Porem, mude para um teste comparativo entre `Vector<std::string>` e `std::vector<std::string>` e ficara entristecido, vendo que estamos "comendo poeira."

### O problema: construtores desnecessarios

A razao: `int` e um tipo trivialmente construivel, e `std::string` nao e. Isso e um indicio de que `std::vector` pode estar chamando menos construtores do que nos, sendo mais eficiente na forma como gerencia a memoria.

Veja o construtor que aceita um numero de elementos e um valor inicial:

```cpp
   // ...
   Vector(size_type n, const_reference init)
      : dados{ new value_type[n] }, nelem{ n }, cap{ n } {
      try {
         std::fill(begin(), end(), init);   // primeiro constroi, depois atribui!
      } catch(...) {
         delete [] dados;
         throw;
      }
   }
   // ...
```

A construcao do membro `dados` aloca um bloco de memoria grande o suficiente para conter `n` objetos do tipo `T` e chama o construtor padrao para cada um desses `n` elementos. Obviamente, se `T` for trivialmente construivel, esses construtores padrao nao sao um grande problema, mas se `T` nao for, isso e um desperdicio: `std::fill()` em seguida substitui cada um desses objetos por uma copia de `init`, mostrando que a construcao padrao inicial foi essencialmente uma perda de tempo.

### Usando facilidades de baixo nivel da biblioteca padrao

O cabecalho `<memory>` e um tesouro de facilidades uteis para quem lida com gerenciamento de memoria. Alem dos *smart pointers*, ele oferece algoritmos que operam sobre memoria bruta.

#### Impacto nos construtores

```cpp
   // ...
   Vector(size_type n, const_reference init)
      : dados{ static_cast<pointer>(
           std::malloc(n * sizeof(value_type))
        ) }, nelem{ n }, cap{ n } {
      try {
         // constroi objetos diretamente na memoria bruta
         std::uninitialized_fill(begin(), end(), init);
      } catch(...) {
         std::free(dados);
         throw;
      }
   }
   // ...
```

Isso e muito mais elegante! As duas melhorias sao:

1. Alocamos um bloco de memoria bruta de tamanho adequado em vez de um array de objetos `T`, evitando todos os construtores padrao desnecessarios.
2. Substituimos `std::fill()` (que usa `T::operator=` e pressupoe um objeto existente) por `std::uninitialized_fill()`, que pressupoe que esta iterando por memoria bruta e inicializa os objetos via *placement new*.

A beleza deste algoritmo (e de outros da mesma familia) e que ele e seguro para excecoes: se um dos construtores lancar uma excecao, os objetos que ele ja construiu serao destruidos (em ordem inversa de construcao) antes que a excecao saia da funcao.

O mesmo para o construtor de copia:

```cpp
   // ...
   Vector(const Vector &outro)
      : dados{ static_cast<pointer>(
           std::malloc(outro.size() * sizeof(value_type))
        ) },
        nelem{ outro.size() }, cap{ outro.size() } {
      try {
         // copia da sequencia fonte para a memoria bruta de destino
         std::uninitialized_copy(
            outro.begin(), outro.end(), begin()
         );
      } catch (...) {
         std::free(dados);
         throw;
      }
   }
   // ...
```

#### Impacto no destrutor

Com a nova abordagem, `dados` aponta para memoria bruta que pode conter objetos `T` seguidos de bytes brutos. Nao podemos simplesmente chamar `delete[]` pois `dados` nao foi alocado por `new[]`. Precisamos destruir os objetos manualmente, depois liberar a memoria:

```cpp
   // ...
   ~Vector() {
      std::destroy(begin(), end()); // chama ~T() em cada elemento
      std::free(dados);             // libera a memoria bruta
   }
   // ...
```

#### Impacto nas funcoes de insercao por elemento

Onde antes atibuiamos ao elemento no final do array, agora precisamos construir um objeto no local, pois nao ha objeto la:

```cpp
   // ...
   void push_back(const_reference val) {
      if (cheio()) crescer();
      std::construct_at(end(), val);   // equivalente ao placement new
      ++nelem;
   }
   void push_back(T&& val) {
      if (cheio()) crescer();
      std::construct_at(end(), std::move(val));
      ++nelem;
   }
   template <class ... Args>
      reference emplace_back(Args &&...args) {
         if (cheio()) crescer();
         std::construct_at(
            end(), std::forward<Args>(args)...
         );
         ++nelem;
         return back();
      }
```

#### Impacto nas funcoes de crescimento

A funcao `reserve()` agora adiciona armazenamento sem inicializar objetos. A funcao `resize()` adiciona objetos com valor padrao:

```cpp
   // ...
private:
   void crescer() {
      reserve(capacity() ? capacity() * 2 : 16);
   }
public:
   void reserve(size_type novo_cap) {
      if(novo_cap <= capacity()) return;
      auto p = static_cast<pointer>(
         std::malloc(novo_cap * sizeof(T))
      );
      // move se possivel sem lancamento; copia caso contrario
      if constexpr(std::is_nothrow_move_assignable_v<T>) {
         std::uninitialized_move(begin(), end(), p);
      } else try {
         std::uninitialized_copy(begin(), end(), p);
      } catch (...) {
         std::free(p);
         throw;
      }
      std::destroy(begin(), end()); // destroi os objetos movidos-de
      std::free(dados);
      dados = p;
      cap = novo_cap;
   }

   void resize(size_type novo_cap) {
      if(novo_cap <= capacity()) return;
      auto p = static_cast<pointer>(
         std::malloc(novo_cap * sizeof(T))
      );
      if constexpr(std::is_nothrow_move_assignable_v<T>) {
         std::uninitialized_move(begin(), end(), p);
      } else try {
         std::uninitialized_copy(begin(), end(), p);
      } catch (...) {
         std::free(p);
         throw;
      }
      // inicializa os novos slots com valor padrao
      std::uninitialized_fill(
         p + size(), p + novo_cap, value_type{}
      );
      std::destroy(begin(), end());
      std::free(dados);
      dados = p;
      nelem = novo_cap;
      cap = novo_cap;
   }
   // ...
```

> **`reserve()` vs `resize()`:** `resize()` potencialmente adiciona objetos ao container, podendo modificar `size()` e `capacity()`. `reserve()` nao adiciona nenhum objeto, limitando-se a potencialmente aumentar o espaco de armazenamento — ou seja, `reserve()` pode alterar `capacity()` mas nao altera `size()`.

#### Impacto nas funcoes de insercao e remocao

A funcao `insert()` agora precisa considerar que parte do armazenamento contem objetos e parte contem memoria bruta:

```cpp
   // ...
   template <class It>
   iterator insert(const_iterator pos, It first, It last) {
      iterator pos_ = const_cast<iterator>(pos);
      const auto restante = capacity() - size();
      const auto n = std::distance(first, last);
      // usamos cmp_less() pois restante e inteiro sem sinal e n e com sinal
      if (std::cmp_less(restante, n)) {
         auto indice = std::distance(begin(), pos_);
         reserve(capacity() + n - restante);
         pos_ = std::next(begin(), indice);
      }
      // objetos a deslocar para a memoria bruta no final
      const auto nb_desloc_bruta =
         std::min<std::ptrdiff_t>(n, end() - pos_);
      auto onde_desloc_bruta = end() + n - nb_desloc_bruta;
      if constexpr(std::is_nothrow_move_constructible_v<T>)
         std::uninitialized_move(
            end() - nb_desloc_bruta, end(), onde_desloc_bruta
         );
      else
         std::uninitialized_copy(
            end() - nb_desloc_bruta, end(), onde_desloc_bruta
         );
      // objetos de [first,last) a inserir na memoria bruta
      const auto nb_inserir_bruta =
         std::max<std::ptrdiff_t>(0, n - nb_desloc_bruta);
      std::uninitialized_copy(
         last - nb_inserir_bruta, last, end()
      );
      // objetos a deslocar dentro da area de objetos existentes
      const auto nb_desloc_atras =
         std::max<std::ptrdiff_t>(
            0, end() - pos_ - nb_desloc_bruta
         );
      if constexpr (std::is_nothrow_move_assignable_v<T>)
         std::move_backward(
            pos_, pos_ + nb_desloc_atras, end()
         );
      else
         std::copy_backward(
            pos_, pos_ + nb_desloc_atras, end()
         );
      // copia o restante de [first,last) para pos
      std::copy(first, first + n - nb_inserir_bruta, pos_);
      nelem += n;
      return pos_;
   }
```

E o `erase()` atualizado — em vez de atribuir um `T` padrao ao elemento removido, simplesmente o destroi:

```cpp
   iterator erase(const_iterator pos) {
      iterator pos_ = const_cast<iterator>(pos);
      if (pos_ == end()) return pos_;
      std::copy(std::next(pos_), end(), pos_);
      std::destroy_at(std::prev(end())); // encerra o tempo de vida do ultimo objeto
      --nelem;
      return pos_;
   }
```

## Membros const ou de referencia e std::launder()

Antes de concluir este capitulo, precisamos falar sobre os casos exoticos de containers que armazenam objetos de tipos `const` ou de tipos com membros `const` ou de referencia.

Considere este programa aparentemente inofensivo:

```cpp
// ...
int main() {
   Vector<const int> v;
   for(int n : { 2, 3, 5, 7, 11 })
      v.push_back(n);
}
```

Com nossa implementacao, isso se recusara a compilar, pois varias funcoes de baixo nivel (`std::free()`, `std::destroy_at()`, `std::construct_at()` etc.) aceitam ponteiros para tipos nao-`const`. Se quisermos suportar esse programa, precisaremos "remover" a constness em alguns lugares da nossa implementacao:

```cpp
   // em vez de: std::free(dados);
   using tipo_base = std::remove_const_t<value_type>*;
   std::free(const_cast<tipo_base>(dados));
```

Uma situacao similar surge com objetos compostos que tem membros de referencia. Voce nao pode fazer um container de referencias, mas pode fazer um container de objetos com membros de tipo referencia. O problema e que referencias nao podem ser revinculadas, entao "substituir" um objeto com membro de referencia requer destruir o original e construir um novo no mesmo lugar.

```cpp
#include <type_traits>
template <class T>
struct X {
   static_assert(std::is_trivially_destructible_v<T>);
   T &r;
public:
   X(T &r) : r{ r } { }
   T&       value()       { return r; }
   const T& value() const { return r; }
};
// ...
#include <iostream>
#include <new>
int main() {
   int n = 3;
   X<int> h{ n };
   h.value()++;
   std::cout << n << '\n';        // 4
   std::cout << h.value() << '\n'; // 4
   int m = -3;
   // h = X<int>{ m };  // erro: operador= deletado
   X<int> *p = new (static_cast<void*>(&h)) X<int>{ m };
   std::cout << p->value() << '\n'; // -3
   // UB (-3? 4? algo mais?)
   std::cout << h.value() << '\n';
   // correto: diz ao compilador "olhe os bits, nao seu modelo"
   std::cout << std::launder(&h)->value() << '\n'; // -3
}
```

Substituir um objeto `X<int>` via atribuicao e incorreto pois ter um membro de referencia exclui o `operator=`. Uma maneira de contornar esse problema e destruir o objeto original e construir um novo objeto no lugar. O compilador, porem, pode nao seguir nosso raciocinio porque o objeto `X<int>` original nunca foi explicitamente destruido — ele ainda pode ser considerado presente pelo compilador, mas os bits foram substituidos pelo novo objeto colocado naquele endereco.

Acessar `value()` atraves de `p` definitivamente dara `-3`. Acessar `value()` atraves de `h` e comportamento indefinido. Quando os bits nao necessariamente se alinham com o que o compilador pode entender, temos `std::launder()`. Use-o com cautela: e uma barreira de otimizacao que diz "apenas olhe os bits, compilador; esqueca o que sabe sobre o codigo-fonte quando olhar para este objeto apontado."

## Resumo

Neste capitulo, implementamos um container semelhante a vetor (ingenuo e depois mais eficiente), depois um container semelhante a lista encadeada simples, e entao revisamos o container semelhante a vetor para mostrar como um controle mais rigido sobre a memoria pode levar a containers mais eficientes.

Nossas implementacoes neste capitulo foram "manuais", no sentido de que fizemos o gerenciamento de memoria a mao. Isso envolveu escrever muito codigo, algo que reconsideraremos no Capitulo 13. No Capitulo 14, examinaremos como os *allocators* interagem com os containers, e usaremos essa oportunidade para revisitar nosso `ForwardList<T>`.
