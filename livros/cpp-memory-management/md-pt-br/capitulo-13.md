# Capitulo 13 - Escrevendo Containers Genericos com Gerenciamento Implicito de Memoria

> Traducao nao oficial do livro *C++ Memory Management* (Patrice Roy, 2025).
> Texto adaptado para portugues brasileiro com fins educacionais. Paginas originais: 303-323.
> Repositorio: [ring0-study](https://github.com/cnthigu/ring0-study)

> Navegacao: [Anterior](capitulo-12.md) | [Indice](README.md) | [Proximo](capitulo-14.md)

---

No capitulo anterior, escrevemos uma implementacao funcional (se simples) do tipo `Vector<T>` similar a `std::vector<T>`, bem como uma implementacao funcional (se igualmente simples) do tipo `ForwardList<T>` similar a `std::forward_list<T>`. Nada mal!

No caso do `Vector<T>`, apos um esforco inicial que levou a uma implementacao funcional mas as vezes ineficiente, fizemos o esforco de separar alocacao de construcao, o que reduziu a quantidade de trabalho redundante em tempo de execucao, mas ao custo de uma implementacao mais sutil.

Nossa implementacao de `Vector<T>` do capitulo anterior e uma classe expressa com uma quantidade consideravel de codigo-fonte. Uma das razoes para essa situacao e o gerenciamento explicito de memoria que temos realizado. Neste capitulo, revisitaremos esse design tornando o gerenciamento de memoria implicito e discutiremos as consequencias dessa nova abordagem.

Neste capitulo, nossos objetivos serao:

- Adaptar um container escrito a mao, como `Vector<T>`, de tal forma a simplificar significativamente suas responsabilidades de gerenciamento de memoria;
- Entender as consequencias de nosso design na complexidade do codigo-fonte;
- Entender as consequencias de nosso design na seguranca de excecoes.

## Por que o gerenciamento explicito de memoria complica nossa implementacao

Vamos olhar por um momento para um dos construtores de `Vector<T>` como escrito no Capitulo 12. Para simplicidade, usaremos o construtor que aceita um numero de elementos e um valor inicial:

```cpp
   // versao ingenua com dados do tipo T*
   Vector(size_type n, const_reference init)
      : dados{ new value_type[n] }, nelem{ n }, cap{ n } {
      try {
         std::fill(begin(), end(), init);
      } catch (...) {
         delete [] dados;
         throw;
      }
   }
```

Este construtor aloca um array de objetos `T`, inicializa-os por meio de uma sequencia de atribuicoes e "trata" excecoes. O bloco `try` e seu bloco `catch` correspondente fazem parte de nossa implementacao, nao porque queremos tratar excecoes lancadas pelos construtores de objetos `T` (como poderiamos saber quais excecoes poderiam ser lancadas se nao sabemos o que `T` e?), mas porque precisamos desalocar explicitamente o array se quisermos evitar vazamentos.

A situacao fica ainda mais complicada se olharmos para a versao mais sofisticada que distingue alocacao de construcao:

```cpp
   // versao sofisticada com dados do tipo T* (alocacao separada da construcao)
   Vector(size_type n, const_reference init)
      : dados{ static_cast<pointer>(
           std::malloc(n * sizeof(value_type))
        ) }, nelem{ n }, cap{ n } {
      try {
         std::uninitialized_fill(begin(), end(), init);
      } catch (...) {
         std::free(dados);
         throw;
      }
   }
```

Fazemos esse trabalho porque decidimos que `Vector<T>` seria o dono dessa memoria. Mas e se fizessemos outra coisa responsavel por nossa memoria?

## Gerenciamento implicito de memoria com um smart pointer

Em C++, a maneira mais simples de mudar nossa implementacao de `Vector<T>` de uma que gerencia memoria manualmente para uma que o faz implicitamente e atraves de um *smart pointer*. A ideia aqui e, essencialmente, mudar o tipo do membro de dados `dados` de `T*` para `std::unique_ptr<T[]>`.

Examinaremos isso por dois angulos:

1. Como essa mudanca afeta a versao ingenua de `Vector<T>` (que nao distingue objetos de memoria bruta)?
2. Como essa mudanca afeta a versao sofisticada de `Vector<T>` (que evita a armadilha de desempenho de construir objetos desnecessarios)?

### Impacto na implementacao ingenua de Vector\<T\>

Se baseassemos nosso esforco de simplificacao na versao inicial e ingenua do Capitulo 12, onde `dados` simplesmente apontava para uma sequencia contiguous de objetos `T`, isso seria bastante simples. Mudariamos de:

```cpp
// implementacao ingenua, gerenciamento EXPLICITO de memoria
pointer dados{};
size_type nelem{}, cap{};
```

...para:

```cpp
// implementacao ingenua, gerenciamento IMPLICITO de memoria
std::unique_ptr<value_type[]> dados;
size_type nelem{}, cap{};
```

E adaptariamos as funcoes `begin()`:

```cpp
// gerenciamento EXPLICITO
iterator begin()             { return dados; }
const_iterator begin() const { return dados; }

// gerenciamento IMPLICITO
iterator begin()             { return dados.get(); }
const_iterator begin() const { return dados.get(); }
```

Fazer apenas isso simplificaria significativamente a implementacao do tipo `Vector<T>`, pois desalocar memoria se tornaria implicito. Por exemplo, poderiamos simplificar cada construtor removendo o tratamento de excecoes:

```cpp
// implementacao ingenua, gerenciamento EXPLICITO
Vector(size_type n, const_reference init)
   : dados{ new value_type[n] }, nelem{ n }, cap{ n } {
   try {
      std::fill(begin(), end(), init);
   } catch (...) {
      delete [] dados;
      throw;
   }
}

// implementacao ingenua, gerenciamento IMPLICITO
Vector(size_type n, const_reference init)
   : dados{ new value_type[n] }, nelem{ n }, cap{ n } {
   std::fill(begin(), end(), init);
   // sem try/catch! o unique_ptr cuida da limpeza
}
```

A razao desta simplificacao:
- Se `Vector<T>` e responsavel pela memoria alocada, entao deletar o array sera feito implicitamente quando o destrutor for chamado — mas para um destrutor ser chamado, um objeto precisa ter sido construido. Isso explica por que precisamos capturar qualquer excecao lancada, deletar manualmente o array e relancá-la.
- Por outro lado, se `dados` for um *smart pointer*, ele se torna responsavel pelo *pointee* assim que o *smart pointer* for construido, o que ocorre antes da chave de abertura do construtor de `Vector<T>`. Isso significa que, uma vez que `dados` tenha sido construido, ele sera destruido se uma excecao sair do construtor, liberando o objeto `Vector<T>` da tarefa de destruir o array.

> **Seguranca sem `try`/`catch`:** mesmo que voce trabalhe em uma empresa onde excecoes sao proibidas, a seguranca de excecoes que ganhamos ao usar um *smart pointer* permanece. Escrevemos codigo seguro para excecoes sem escrever as palavras `try` ou `catch`.

Outras simplificacoes incluem as operacoes de movimentacao e o destrutor:

```cpp
// gerenciamento EXPLICITO
Vector(Vector &&outro)
   : dados{ std::exchange(outro.dados, nullptr) },
     nelem{ std::exchange(outro.nelem, 0) },
     cap{ std::exchange(outro.cap, 0) } { }
Vector& operator=(Vector &&outro) {
   Vector{ outro }.trocar(*this);
   return *this;
}
~Vector() {
   delete [] dados;
}

// gerenciamento IMPLICITO
Vector(Vector&&) = default;
Vector& operator=(Vector&&) = default;
~Vector() = default;
```

> **Atencao:** ao tornar as operacoes de movimentacao `=default`, induzimos uma ligeira mudanca semantica. Nossas operacoes de movimentacao escritas a mao restauravam o objeto movido para o equivalente de um `Vector<T>` padrao. A versao `=default` deixa o objeto movido com `dados` nulo mas com tamanho e capacidade potencialmente nao-zero. Funciona na pratica, mas e uma mudanca que merece reconhecimento.

A funcao `resize()` tambem pode ser simplificada:

```cpp
// gerenciamento EXPLICITO
void resize(size_type novo_cap) {
   if(novo_cap <= capacity()) return;
   auto p = new T[novo_cap];
   if constexpr(std::is_nothrow_move_assignable_v<T>) {
      std::move(begin(), end(), p);
   } else try {
      std::copy(begin(), end(), p);
   } catch (...) {
      delete[] p;
      throw;
   }
   delete[] dados;
   dados = p;
   cap = novo_cap;
}

// gerenciamento IMPLICITO
void resize(size_type novo_cap) {
   if(novo_cap <= capacity()) return;
   auto p = std::make_unique<value_type[]>(novo_cap);
   if constexpr(std::is_nothrow_move_assignable_v<T>) {
      std::move(begin(), end(), p.get());
   } else {
      std::copy(begin(), end(), p.get());
   }
   dados.reset(p.release()); // troca de posse sem vazamento
   cap = novo_cap;
}
```

Todo o codigo de tratamento de excecoes desapareceu. O objeto `p` possui o novo array e o destruira quando a funcao concluir a execucao. Ao aplicar essa simplificacao em todo `Vector<T>`, o codigo-fonte gradualmente encolhe — em uma implementacao academica como a nossa, podemos economizar quase 25% das linhas de codigo-fonte.

### Impacto na implementacao sofisticada de Vector\<T\>

Aplicar a mesma tecnica ao `Vector<T>` mais sofisticado (que distingue objetos de memoria bruta) requer um pouco mais de trabalho, pois o comportamento padrao do destrutor de `std::unique_ptr<T[]>` sera aplicar `operator delete[]` ao ponteiro que ele possui.

Nossa implementacao sofisticada pode ser conceituada como sendo feita de duas "secoes" (potencialmente vazias): uma secao inicial feita de objetos `T` colocados manualmente em memoria bruta, seguida por outra secao de memoria bruta nao inicializada, sem objetos. Precisaremos usar um *deletador personalizado* que leve em conta os especificos da nossa implementacao:

```cpp
// ...
template <class T>
class Vector {
public:
   using value_type      = T;
   using size_type       = std::size_t;
   using pointer         = T*;
   using const_pointer   = const T*;
   using reference       = T&;
   using const_reference = const T&;
   // ...
private:
   struct deletador {
      Vector &origem;  // referencia ao Vector dono
      void operator()(value_type *p) {
         // destroi os objetos, depois libera a memoria bruta
         std::destroy(std::begin(origem), std::end(origem));
         std::free(static_cast<void*>(p));
      }
   };
   std::unique_ptr<value_type[], deletador> dados;
   size_type nelem{},
             cap{};
   // ...
```

O membro `dados` sabe que o tipo do deletador personalizado sera `deletador`, mas o objeto real que fara o papel de deletador precisara saber com qual objeto `Vector<T>` ele estara interagindo. Os construtores de `Vector<T>` serao responsaveis por fornecer essa informacao:

```cpp
   // ...
   using iterator       = pointer;
   using const_iterator = const_pointer;
   iterator begin()             { return dados.get(); }
   const_iterator begin() const { return dados.get(); }

   constexpr Vector()
      : dados{ nullptr, deletador{ *this } } { }

   Vector(size_type n, const_reference init)
      : dados{ static_cast<pointer>(
           std::malloc(n * sizeof(value_type))
        ), deletador{ *this }
      } {
      // constroi os objetos na memoria bruta recem-alocada
      // (nelem ainda e 0, entao o deletador nao tentara destruir nada)
      std::uninitialized_fill(begin(), begin() + n, init);
      nelem = cap = n;
   }

   Vector(Vector &&outro) noexcept
      : dados{ std::exchange(
           outro.dados.release(), nullptr), deletador{ *this }
        },
        nelem{ std::exchange(outro.nelem, 0) },
        cap{ std::exchange(outro.cap, 0) } { }
   // ...
```

> **Por que nao usar `=default` para o construtor de movimentacao aqui?** Nao o tornamos `=default` pois nao queremos transferir o estado interno do deletador personalizado, que esta associado a um objeto `Vector<T>` especifico.

O destrutor agora pode ser padronizado, e as funcoes `reserve()` e `resize()` simplificadas:

```cpp
   // ...
   ~Vector() = default;

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
   Vector& operator=(Vector &&outro) {
      Vector{ std::move(outro) }.trocar(*this);
      return *this;
   }

   reference       operator[](size_type n)       { return dados[n]; }
   const_reference operator[](size_type n) const { return dados[n]; }

   void reserve(size_type novo_cap) {
      if (novo_cap <= capacity()) return;
      std::unique_ptr<value_type[]> p{
         static_cast<pointer>(std::malloc(novo_cap * sizeof(T)))
      };
      if constexpr (std::is_nothrow_move_assignable_v<T>) {
         std::uninitialized_move(begin(), end(), p.get());
      } else {
         std::uninitialized_copy(begin(), end(), p.get());
      }
      std::destroy(begin(), end());
      dados.reset(p.release());
      cap = novo_cap;
   }

   void resize(size_type novo_cap) {
      if (novo_cap <= capacity()) return;
      std::unique_ptr<value_type[]> p =
         static_cast<pointer>(std::malloc(novo_cap * sizeof(T)));
      if constexpr (std::is_nothrow_move_assignable_v<T>) {
         std::uninitialized_move(begin(), end(), p.get());
      } else {
         std::uninitialized_copy(begin(), end(), p.get());
      }
      // inicializa novos slots com objetos padrao
      std::uninitialized_fill(
         p.get() + size(), p.get() + novo_cap, value_type{}
      );
      std::destroy(begin(), end());
      dados.reset(p.release());
      nelem = cap = novo_cap;
   }
   // ...
```

## Consequencias deste redesign

Quais sao as consequencias desta "reformulacao"? Vamos resumir:

- **Para o codigo usuario**, as consequencias sao essencialmente nulas: um objeto do tipo `Vector<T>` ocupa o mesmo espaco em memoria, e cada implementacao expoe a mesma interface publica.
- **Nao ha custos de velocidade**, pelas razoes explicadas no Capitulo 5: em codigo compilado com niveis de otimizacao nao basicos, passar por `std::unique_ptr<T>` levara, devido ao *inlining* de chamadas de funcao, a codigo tao eficiente quanto passar por um `T*`.
- **A implementacao e significativamente mais simples**: menos instrucoes, sem codigo explicito de tratamento de excecoes, mais funcoes membro que podem ser padronizadas.
- **A implementacao e segura para excecoes mesmo na ausencia de blocos `try` e `catch` explícitos.** Isso pode fazer diferenca em muitas situacoes, por exemplo, se voce esta em uma situacao onde excecoes nao sao permitidas, mas usa uma biblioteca onde excecoes sao uma possibilidade.

## Generalizando para ForwardList\<T\>?

Agora sabemos que podemos adaptar a implementacao de `Vector<T>`, transformando-a de um modelo de gerenciamento explicito de memoria para um implicito, e que isso tem muitas vantagens. E tentador fazer o mesmo com outros containers, mas antes de embarcar em tal aventura, pode ser sensato analisar o problema um pouco.

Implementamos um container baseado em nos com gerenciamento explicito de memoria chamado `ForwardList<T>` no Capitulo 12. Qual seria o impacto de tentar mudar a implementacao deste container para torna-la mais implicita?

### Tentativa 1 — tornando cada no responsavel pelo seu sucessor

Uma abordagem possivel seria mudar a definicao de `ForwardList<T>::No` tal que o membro `proximo` se torna um `std::unique_ptr<No>` em vez de `No*`:

```cpp
template <class T>
class ForwardList {
   // ...
   struct No {
      value_type valor;
      std::unique_ptr<No> proximo;  // <-- proprietario do proximo
      No(const_reference v) : valor{ v } { }
      No(value_type &&v) : valor{ std::move(v) } { }
   };
   No *inicio {};     // ainda ponteiro bruto (head nao e dono)
   size_type nelem {};
   // ...
```

Isso pode parecer uma melhoria a primeira vista, pois simplificaria o destrutor:

```cpp
   ~ForwardList() {
      delete inicio;  // efeito cascata: destroi recursivamente todos os nos!
   }
```

No entanto, essa aparente simplificacao esconde um fato ardiloso: ao chamar `delete inicio`, podemos estar provocando um estouro de pilha. De fato, substituimos um loop que aplicava `delete` em cada no sucessivamente por algo que e essencialmente uma chamada recursiva, significando que o impacto no uso de pilha mudou de algo fixo para algo proporcional ao numero de nos na lista.

Alem disso, as funcoes de insercao se tornam notavelmente mais complicadas. Veja `insert_after()`:

```cpp
   iterator insert_after(iterator pos, const_reference valor) {
      auto p = std::make_unique<No>(valor); // A: cria o novo no
      p->proximo.reset(pos.atual->proximo.get()); // B: p aponta para o sucessor de pos
      pos.atual->proximo.release(); // C: pos relinquisce seu sucessor
      pos.atual->proximo.reset(p.get()); // D: pos passa a apontar para p
      p.release(); // E: p relinquisce o ponteiro
      ++nelem;
      return { pos.atual->proximo.get() }; // F: retorna iterador para o novo no
   }
```

Isso e... complicado. A razao principal e que a maior parte do esforco nesta funcao e a transferencia de posse. Um `std::unique_ptr<T>` representa posse exclusiva sobre um `T*`, e em uma lista encadeada, cada insercao ou remocao requer mover ponteiros, transferindo posse entre nos. Simplificamos uma situacao ocasional (delecao dos nos) ao complicar a maioria das operacoes em nosso tipo. Triste!

### Tentativa 2 — tornando o ponteiro cabeca responsavel pelos outros nos

Talvez tornar apenas o no `inicio` um `std::unique_ptr<No>` com um deletador personalizado responsavel por deletar toda a lista seja benefico:

```cpp
template <class T>
class ForwardList {
   // ...
   struct No {
      value_type valor;
      No *proximo = nullptr;  // ponteiro bruto — nos nao sao donos
      No(const_reference v) : valor{ v } { }
      No(value_type &&v) : valor{ std::move(v) } { }
   };

   struct deletador {  // deleta todos os nos em loop, sem recursao
      void operator()(No *p) const {
         while (p) {
            No *q = p->proximo;
            delete p;
            p = q;
         }
      }
   };

   std::unique_ptr<No, deletador> inicio;  // <-- dono de todos os nos
   size_type nelem {};
   // ...
```

Agora temos um tipo `ForwardList<T>` que, quando um objeto desse tipo e destruido, garante implicitamente que os nos na lista sao destruidos — sem recursao! O destrutor agora pode ser padronizado.

Adaptamos as funcoes que interagem com `inicio`:

```cpp
   // ...
   void clear() noexcept {
      for (auto p = inicio.get(); p; ) {  // <-- usa .get()
         auto q = p->proximo;
         delete p;
         p = q;
      }
      nelem = 0;
   }
   ~ForwardList() = default;  // padronizado!

   iterator begin()             { return { inicio.get() }; }  // <--
   const_iterator begin() const { return { inicio.get() }; }  // <--

   template <std::forward_iterator It>
      ForwardList(It b, It e) {
         if(b == e) return;
         inicio.reset(new No{ *b });  // <-- usa .reset()
         auto q = inicio.get();       // <-- usa .get()
         ++nelem;
         for(++b; b != e; ++b) {
            q->proximo = new No{ *b };
            q = q->proximo;
            ++nelem;
         }
      }  // excecoes sao tratadas implicitamente

   void push_front(const_reference val) {
      auto p = new No{ val };
      p->proximo = inicio.get();  // <--
      inicio.release();           // <--
      inicio.reset(p);            // <--
      ++nelem;
   }
   // ...
```

Nenhuma funcao membro que nao interage com o membro `inicio` requer qualquer modificacao.

Vale a pena essa "implicidade"? Provavelmente depende da forma como voce aborda a escrita de codigo. Ganhamos algo de valor na seguranca implicita de excecoes. Mas a complexidade adicionada nas funcoes que interagem com `inicio` pode ou nao superar a simplificacao obtida.

## Resumo

Neste capitulo, reexaminamos os containers escritos no Capitulo 12, buscando usar ferramentas de gerenciamento implicito de memoria de tal forma a tornar nossas implementacoes mais simples e seguras. Alcancar uma melhoria em `Vector<T>` foi possivel; os resultados obtidos com nosso container baseado em nos `ForwardList<T>` foram... nao ausentes, mas indiscutivelmente menos conclusivos dependendo da sua perspectiva.

No proximo capitulo, introduziremos a ideia de *allocators*, objetos que informam os containers sobre como a memoria deve ser obtida ou liberada, e examinaremos como eles afetam as formas pelas quais escrevemos codigo.
