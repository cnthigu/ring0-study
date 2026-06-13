# Capitulo 6 - Escrevendo Smart Pointers (Writing Smart Pointers)

> Traducao nao oficial do livro *C++ Memory Management* (Patrice Roy, 2025).
> Texto adaptado para portugues brasileiro com fins educacionais. Paginas originais: 111-137.
> Repositorio: [ring0-study](https://github.com/cnthigu/ring0-study)

> Navegacao: [Anterior](capitulo-05.md) | [Indice](README.md) | [Proximo](capitulo-07.md)

---

No Capitulo 5, examinamos os *smart pointers* padrao a nossa disposicao, com enfase nos mais importantes: `unique_ptr<T>` e `shared_ptr<T>`. Esses tipos sao ferramentas preciosas e importantes na caixa de ferramentas de todo programador C++ contemporaneo, e usá-los quando apropriado leva a programas menores, mais rapidos e mais simples do que seriam com a maioria das alternativas escritas manualmente.

Este livro visa discutir como gerenciar memoria em um programa C++. Por essa razao, neste capitulo, escreveremos versoes simples de `unique_ptr<T>` e de `shared_ptr<T>` para mostrar maneiras pelas quais se poderia escrever versoes ingênuas-mas-funcionais desses tipos se necessario. Recomendamos fortemente que voce use as versoes padrao na pratica, nao as deste livro (pelo menos em codigo de producao): as versoes padrao foram exaustivamente testadas, otimizadas e usadas por uma multidao de programadores com bons resultados. A razao pela qual escrevemos versoes "caseiras" aqui e simplesmente para desenvolver uma intuicao sobre como se poderia escrever esse tipo: ainda existem empresas usando compiladores pre-C++11, as vezes por razoes razoaveis, e pode haver razoes em alguns contextos para escrever um *smart pointer* inspirado nos padrao, porem ligeiramente diferente.

Em seguida, examinaremos alguns nichos nao cobertos pelos *smart pointers* padrao, seja porque sao considerados simples o suficiente para que os usuarios criem os seus proprios, ou especializados o suficiente para serem cobertos por bibliotecas de terceiros, ou porque ainda nao ha um caminho claro para padroniza-los.

Para resumir, neste capitulo faremos o seguinte:

- Dar uma breve olhada na semantica de *ownership*, tanto dos *smart pointers* padrao quanto de outros que poderıamos — e as vezes iremos — implementar nos mesmos;
- Implementar nossa propria versao ingênua-mas-utilizavel de `std::unique_ptr`, a fim de compreender algumas das tecnicas que isso pode implicar;
- Implementar nossa propria versao ingênua-mas-utilizavel de `std::shared_ptr`. Note que por "utilizavel" aqui queremos dizer utilizavel em contextos simples, pois uma implementacao completa de algo como `std::shared_ptr` e significativamente mais complexa do que um livro como este pode cobrir razoavelmente;
- Implementar um *smart pointer* nao padrao com semantica de posse exclusiva e duplicacao, mostrando diferentes tecnicas para atingir esse objetivo;
- Implementar dois "ponteiros inteligentes" nao-proprietarios distintos, que sao tipos muito leves mas ajudam a escrever codigo melhor e mais seguro.

Apos ler este capitulo, devemos ter uma melhor compreensao das tecnicas envolvidas ao escrever tipos que sintaticamente se comportam como ponteiros, mas que fornecem (ou simplesmente esclarecem) a semantica de *ownership*. As tecnicas usadas devem ser em grande parte reutilizaveis para outros tipos de problemas, relacionados ou nao ao gerenciamento de memoria.

Isso soa como um plano? Entao mao a obra!

## Requisitos tecnicos

O codigo deste capitulo pode ser encontrado em: [https://github.com/PacktPublishing/C-Plus-Plus-Memory-Management/tree/main/chapter6](https://github.com/PacktPublishing/C-Plus-Plus-Memory-Management/tree/main/chapter6)

## Semantica de ownership

*Smart pointers* tratam fundamentalmente de esclarecer a *ownership* sobre recursos acessados indiretamente. Se nos restringirmos as facilidades padrao, *smart* ou nem tanto, o que temos e o seguinte:

| Tipo | Nicho |
|---|---|
| `unique_ptr<T>` | **Semantica de ownership**: Posse exclusiva (unica). **Funcoes membro especiais notaveis**: Nao-copiavel. O destrutor e responsavel por destruir o *pointee*. |
| `shared_ptr<T>` | **Semantica de ownership**: Posse compartilhada. **Funcoes membro especiais notaveis**: Copiar, atribuir e destruir atualiza um contador de uso compartilhado. O destrutor do ultimo co-proprietario e responsavel por destruir tanto o *pointee* quanto o contador de uso. |
| `T*` | **Semantica de ownership**: Nenhuma *ownership* e definida no sistema de tipos (as regras de *ownership* precisam ser inscritas no codigo do usuario). **Funcoes membro especiais notaveis**: Nao aplicavel (e um tipo fundamental). |

E um "zoologico" pequeno, considerando tudo. Quais sao os outros tipos de semantica que poderıamos imaginar para preencher essa tabela? Bem, poderia haver os seguintes:

- Um tipo `ponteiro_observador<T>` que se comporta como `T*`, mas torna mais dificil reivindicar acidentalmente a *ownership* com operacoes como aplicar `delete` no ponteiro;
- Um tipo `ponteiro_nao_nulo<T>` que se comporta como `T*`, mas para o qual um ponteiro nulo nunca ocorre, simplificando o codigo cliente;
- Um tipo `ponteiro_remoto<T>` que se comporta como um proxy para *pointees* remotos;
- Um tipo `ponteiro_duplicante<T>` que implementa posse exclusiva do *pointee*, como `unique_ptr<T>`, mas e copiavel e duplica o *pointee* quando `ponteiro_duplicante<T>` e copiado, e assim por diante.

Nao implementaremos todos esses (o caso de `ponteiro_remoto<T>` em particular, por mais interessante que seja, esta fora do escopo deste livro), mas escreveremos alguns. O aspecto importante em cada caso e definir claramente qual e a semantica pretendida, garantir que ela ainda nao seja coberta por um tipo existente, e garantir que a implementemos adequadamente.

Comecemos com uma implementacao simples do que talvez seja o *smart pointer* padrao mais conhecido: `unique_ptr`.

## Escrevendo seu proprio (ingênuo) unique_ptr

Vamos primeiro tentar uma versao simples e caseira de `std::unique_ptr<T>`. Como mencionado no inicio do capitulo, nosso objetivo e desenvolver uma intuicao para o tipo de codigo necessario para escrever tal tipo, e nao encorajar voce a tentar substituir as facilidades padrao: elas existem, funcionam, sao testadas, use-as. E elas usam muitos truques legais que nao podemos explorar neste livro, pois queremos manter o tamanho do livro sob controle!

### Assinatura do tipo

Como mencionado no Capitulo 5, `unique_ptr<T>` nao existe realmente como tipo, pois o tipo e, de fato, `unique_ptr<T,D>`, onde `D` tem como padrao `default_deleter<T>`.

Cobriremos ambas as formas (escalar e array) de `unique_ptr`. A razao para essas duas especializacoes e que para `T[]`, queremos que `unique_ptr` exponha `operator[]`, mas nao queremos expor isso para um tipo escalar `T`.

Comecemos com os tipos de *deleter* basicos que ofereceremos. Note que os usuarios podem fornecer outros tipos de *deleter* se necessario, desde que usem a mesma assinatura para `operator()`:

```cpp
namespace livro_gerenciamento_memoria {
   // tipos basicos de deleter
   template <class T>
   struct wrapper_funcao_deleter {
      void (*pf)(T*); // ponteiro para a funcao de exclusao
      wrapper_funcao_deleter(void (*pf)(T*)) : pf{ pf } {}
      void operator()(T* p) const { pf(p); } // chama a funcao
   };

   template <class T>
   struct deleter_padrao {
      void operator()(T* p) const { delete p; } // delete simples
   };

   template <class T>
   struct deleter_padrao<T[]> {
      void operator()(T* p) const { delete[] p; } // delete para arrays
   };
}
```

O que temos ate agora sao tres tipos de *deleter* chamáveis da mesma forma, todos sendo tipos de classe. O estranho e `wrapper_funcao_deleter<T>`, que encapsula um estado copiavel (um ponteiro de funcao), mas se comporta como os outros dois: quando chamado em `T*`, aplica alguma funcao (fornecida pelo usuario) a aquele ponteiro.

O proximo passo sera escolher a forma de `unique_ptr<T,D>`. Esperamos que a maioria dos *deleters* seja sem estado (*stateless*) e usaremos a otimizacao de classe base vazia (EBO — *Empty Base Optimization*) a nosso favor derivando do nosso tipo *deleter*. A excecao sera quando o *deleter* e um ponteiro de funcao, pois nao podemos usar tal tipo como classe base; nesse caso, derivaremos de `wrapper_funcao_deleter<T>`. Para escolher entre essas duas opcoes, precisaremos detectar se `D` e um ponteiro de funcao ou nao, o que alcancaremos usando nosso proprio *trait* `e_candidato_a_deleter_funcao<T>`:

```cpp
#include <type_traits>

namespace livro_gerenciamento_memoria {
   // por padrao, nao e candidato a funcao-deleter
   template <class T>
   struct e_candidato_a_deleter_funcao : std::false_type {};

   // mas funcoes do tipo void(*)(T*) sao
   template <class T>
   struct e_candidato_a_deleter_funcao<void (*)(T*)>
      : std::true_type {};

   template <class T>
   constexpr auto e_candidato_a_deleter_funcao_v =
      e_candidato_a_deleter_funcao<T>::value;
}
```

Esse trecho provavelmente e autoexplicativo: a maioria dos tipos nao e candidata a funcao *deleter*, mas funcoes do tipo `void(*)(T*)` sao.

Em seguida, chegamos ao tipo geral `unique_ptr<T>`, usado para escalares. Usaremos nosso *trait* de deteccao de funcao *deleter* para escolher condicionalmente entre tipos `D` e `wrapper_funcao_deleter<T>` como classe base para nosso tipo, e faremos um cast para um ponteiro para essa base ao liberar o recurso em nosso destrutor:

```cpp
namespace livro_gerenciamento_memoria {
   // template geral unique_ptr (para escalares)
   template <class T, class D = deleter_padrao<T>>
   class unique_ptr : std::conditional_t<
      e_candidato_a_deleter_funcao_v<D>,
      wrapper_funcao_deleter<T>, D
   > {
      using tipo_deleter = std::conditional_t<
         e_candidato_a_deleter_funcao_v<D>,
         wrapper_funcao_deleter<T>,
         D
      >;
      T* ptr = nullptr; // o ponteiro gerenciado

   public:
      unique_ptr() = default; // comeca vazio (null)

      unique_ptr(T* p) : ptr{ p } {}

      unique_ptr(T* p, void (*pf)(T*))
         : tipo_deleter{ pf }, ptr{ p } {}

      ~unique_ptr() {
         // chama o deleter no ponteiro
         (*static_cast<tipo_deleter*>(this))(ptr);
      }
   };
}
```

A mesma abordagem, essencialmente, e adotada para a especializacao `T[]` do nosso tipo:

```cpp
namespace livro_gerenciamento_memoria {
   // especializacao de unique_ptr para arrays
   template <class T, class D>
   class unique_ptr<T[], D> : std::conditional_t<
      e_candidato_a_deleter_funcao_v<D>,
      wrapper_funcao_deleter<T>,
      D
   > {
      using tipo_deleter = std::conditional_t<
         e_candidato_a_deleter_funcao_v<D>,
         wrapper_funcao_deleter<T>,
         D
      >;
      T* ptr = nullptr;

   public:
      unique_ptr() = default;
      unique_ptr(T* p) : ptr{ p } {}
      unique_ptr(T* p, void (*pf)(T*))
         : tipo_deleter{ pf }, ptr{ p } {}

      ~unique_ptr() {
         (*static_cast<tipo_deleter*>(this))(ptr);
      }
   };
}
```

Note que um `unique_ptr` default se comportara conceitualmente como um ponteiro nulo, algo que nao deve surpreender a maioria. Agora que temos a ideia basica no lugar, vamos explorar a semantica especifica de `unique_ptr`.

### Funcoes membro especiais

O codigo para as funcoes membro especiais sera o mesmo para as formas escalar e array de `unique_ptr`. Ja vimos o destrutor e o construtor default na secao anterior, entao vejamos os outros quatro, em pares:

- Queremos que o tipo seja nao-copiavel, pois representa a posse exclusiva do *pointee* (se fosse copiavel, quem seria responsavel pelo *pointee*, o original ou a copia?);
- Queremos que as operacoes de movimento implementem a transferencia de *ownership*.

O codigo para o caso geral e sua especializacao para arrays sera o seguinte (note que o codigo usa `std::exchange()` e `std::swap()`, ambos encontrados no cabecalho `<utility>`):

```cpp
// ...
   unique_ptr(const unique_ptr&) = delete;             // nao copiavel
   unique_ptr& operator=(const unique_ptr&) = delete;  // nao copiavel

   void trocar(unique_ptr &outro) noexcept {
      using std::swap;
      swap(ptr, outro.ptr); // troca os ponteiros gerenciados
   }

   // construtor de movimento: transfere a posse
   unique_ptr(unique_ptr &&outro) noexcept
      : ptr{ std::exchange(outro.ptr, nullptr) } {
   }

   // atribuicao de movimento: libera atual, adquire o de 'outro'
   unique_ptr& operator=(unique_ptr &&outro) noexcept {
      unique_ptr{ std::move(outro) }.trocar(*this);
      return *this;
   }
// ...
```

A maior parte disso deve ser autoevidente neste ponto. Voce pode notar o uso de `std::exchange()`, que tem o efeito de copiar `outro.ptr` para `this->ptr` e depois copiar `nullptr` para `outro.ptr`, implementando a transferencia de *ownership* como esperado. Note que as operacoes de movimento para nosso tipo sao triviais e nunca lancam, ambas propriedades altamente desejáveis.

Ha algumas operacoes que serao implementadas tanto no caso geral quanto no caso de array: `operator bool` (verdadeiro apenas se o objeto nao modela um ponteiro nulo), `vazio()` (verdadeiro apenas se o objeto modela um ponteiro nulo), bem como `operator==()` e `operator!=()`. Estas sao essencialmente triviais de implementar. A outra funcao membro que queremos expor e `obter()` nas suas versoes `const` e nao-`const`, a fim de expor o ponteiro subjacente para o codigo cliente que precisa interagir com funcoes de nivel mais baixo como chamadas de sistema:

```cpp
// ...
   bool vazio()    const noexcept { return !ptr; }
   operator bool() const noexcept { return !vazio(); }

   bool operator==(const unique_ptr &outro) const noexcept {
      return ptr == outro.ptr;
   }
   // inferido de operator==() desde C++20
   bool operator!=(const unique_ptr &outro) const noexcept {
      return !(*this == outro);
   }

   T *obter() noexcept       { return ptr; }
   const T *obter() const noexcept { return ptr; }
// ...
```

Como mencionado nos comentarios no trecho de codigo anterior, nao e necessario implementar `operator!=()` explicitamente desde o C++20, desde que `operator==()` ofereça a assinatura esperada. O compilador sintetizara `operator!=()` a partir de `operator==()`, de forma bastante simples.

### Funcoes semelhantes a ponteiros

As funcoes semelhantes a ponteiros sao diferentes para o caso escalar e o caso de array. Para ponteiro-para-escalar, queremos implementar `operator*()` e `operator->()`:

```cpp
// ...
   T& operator*() noexcept       { return *ptr; }
   const T& operator*() const noexcept { return *ptr; }
   T* operator->() noexcept       { return ptr; }
   const T* operator->() const noexcept { return ptr; }
// ...
```

A funcao membro `operator->()` e uma criatura estranha: quando usada em um objeto, ela sera reinvocada no objeto retornado (e novamente naquele objeto retornado, e assim por diante) ate que algo retorne um *raw pointer*, momento em que o compilador saberá o que fazer. E um mecanismo muito poderoso.

Para ponteiro-para-array (a especializacao `unique_ptr<T[]>`), queremos implementar `operator[]`, que fara mais sentido do que `operator*()` ou `operator->()`:

```cpp
// ...
   T& operator[](std::size_t n) noexcept       { return ptr[n]; }
   const T& operator[](std::size_t n) const noexcept { return ptr[n]; }
// ...
```

Se voce tiver um compilador C++23, pode fazer o compilador sintetizar as formas que voce usa na pratica, dado um conjunto adequadamente escrito de funcoes membro template (*"deduced this"*):

```cpp
// para ambos os casos: escalar e array
template <class U>
   decltype(auto) obter(this U && self) noexcept {
      return self.ptr;
   }

// apenas para o caso nao-array
template <class U>
   decltype(auto) operator*(this U && self) noexcept {
      return *(self.ptr);
   }

template <class U>
   decltype(auto) operator->(this U && self) noexcept {
      return self.ptr;
   }

// apenas para o caso de array
template <class U>
   decltype(auto) operator[](this U && self, std::size_t n) noexcept {
      return self.ptr[n];
   }
```

Isso reduz o numero de funcoes membro que temos que escrever pela metade. Como isso funciona? O C++23 introduz o mecanismo *"deduced this"* que permite marcar explicitamente o primeiro argumento de uma funcao membro com a palavra-chave `this`. Fazer isso e combinar com uma referencia de encaminhamento (o tipo `U&&`) permite ao compilador deduzir o `const`-ness (ou sua ausencia) de `this`, expressando efetivamente as versoes `const` e nao-`const` em uma unica funcao.

E isso! Agora temos uma implementacao simples, porem funcional de `unique_ptr<T>` que funciona para a maioria dos casos de uso.

Um programa simples que usa nosso `unique_ptr<T>` caseiro com um *deleter* padrao seria o seguinte:

```cpp
// ... (nossa propria unique_ptr<T> vai aqui ...)

struct X {};

int main() {
   unique_ptr<X> p{ new X };
} // X::~X() chamado aqui
```

Outro que usa um *deleter* personalizado seria:

```cpp
// ... (nossa propria unique_ptr<T> vai aqui ...)

class X {
   ~X() {} // destrutor privado
public:
   static void destruir(X *p) { delete p; } // destruicao controlada
};

int main() {
   unique_ptr<X, &X::destruir> p{ new X };
} // X::destruir(p.obter()) chamado aqui
```

## Escrevendo seu proprio (ingênuo) shared_ptr

Um tipo `shared_ptr<T>` e uma criatura dificil de implementar e ainda mais dificil de otimizar. O convite para usar a versao padrao de *smart pointers* existentes e mais forte neste caso do que era para `unique_ptr<T>`: esse tipo e dificil de acertar, e a versao padrao se beneficia de anos de experiencia e testes. Use a versao ingênua desta secao apenas para experimentacao (ela funciona e faz o trabalho em casos simples, mas escrever uma implementacao de nivel industrial e um trabalho de alto calibre).

A principal dificuldade ao escrever um `shared_ptr` e que e um tipo com duas responsabilidades: ele co-possui tanto o *pointee* quanto o contador de uso, exigindo alguma medida de cuidado, especialmente em relacao a seguranca contra excecoes.

Para manter nossa proposta simples, evitaremos muitos detalhes do contrato padrao de `shared_ptr`, limitando-nos a gerenciar um `T` escalar. Vamos construir esse tipo passo a passo:

```cpp
#include <atomic>
#include <utility>

namespace livro_gerenciamento_memoria {
   // shared_ptr ingênuo
   template <class T>
   class shared_ptr {
      T* ptr = nullptr;
      std::atomic<long long> *contador = nullptr; // contador atomico compartilhado
      // ...
```

Como mencionado anteriormente, `shared_ptr<T>` e responsavel por `T*` e por um ponteiro para um contador compartilhado, ambos precisando ser gerenciados e compartilhados entre co-proprietarios. Note que nosso contador compartilhado e um ponteiro para um inteiro atomico, pois `shared_ptr<T>` e particularmente relevante em casos multithreaded onde nao se sabe qual thread sera a ultima a usar o objeto. Por essa razao, operacoes como incrementar e decrementar o contador exigem sincronizacao para evitar uma *data race*.

> **Evitando data races**
>
> Se um programa encontra uma situacao onde um determinado objeto e (a) acessado concorrentemente por pelo menos duas threads, (b) pelo menos um desses acessos e uma escrita, e (c) nao ha sincronizacao, entao aquele programa tem o que chamamos de *data race* e perdemos essencialmente a capacidade de raciocinar sobre ele a partir do codigo-fonte. Essa e uma situacao realmente ruim.
>
> No nosso caso, operacoes no contador compartilhado provavelmente serao feitas concorrentemente e, portanto, precisam ser sincronizadas. Isso explica nosso uso de inteiros atomicos como contadores.

Construir um objeto `shared_ptr<T>` pode ser complicado:

- Por padrao, definiremos `shared_ptr<T>` como vazio, conceitualmente equivalente a um ponteiro nulo;
- O construtor de `shared_ptr<T>` que recebe `T*` como argumento representa o ato de assumir a *ownership* do *pointee*. Por essa razao, se uma excecao for lancada ao alocar o contador, aquele *pointee* e destruido;
- O construtor de copia representara o compartilhamento da *ownership* do *pointee*, tendo o cuidado de considerar o caso em que o objeto-fonte modela um ponteiro nulo;
- O construtor de movimento modela a transferencia de *ownership*. Como e frequentemente o caso para operacoes de movimento, e muito rapido e mostra comportamento altamente previsivel.

Como pode ser visto no seguinte trecho de codigo, com um tipo que tem mais de uma responsabilidade, ate a construcao e um esforco delicado. No construtor que recebe `T*`, pode ser necessario alocar o contador compartilhado, o que pode lançar, uma situacao que precisamos gerenciar. No construtor de copia, precisamos levar em conta que o argumento pode modelar um `shared_ptr<T>` vazio, caso em que o contador compartilhado seria nulo:

```cpp
   // ...
   public:
      shared_ptr() = default; // comeca vazio

      shared_ptr(T* p) : ptr{ p } {
         if (p) try {
            // aloca o contador atomico, inicializado com 1 proprietario
            contador = new std::atomic<long long>{ 1LL };
         } catch (...) {
            delete p; // evita vazamento se alocacao do contador falhar
            throw;    // repropaga a excecao
         }
      }

      // copia: compartilha o pointee e incrementa o contador
      shared_ptr(const shared_ptr &outro)
         : ptr{ outro.ptr }, contador{ outro.contador } {
         if (contador) ++(*contador);
      }

      // movimento: transfere a posse, deixa 'outro' vazio
      shared_ptr(shared_ptr &&outro) noexcept
         : ptr    { std::exchange(outro.ptr,      nullptr) },
           contador{ std::exchange(outro.contador, nullptr) } {
      }

      bool vazio()    const noexcept { return !ptr; }
      operator bool() const noexcept { return !vazio(); }
// ...
```

O operador de atribuicao nao e surpreendente: a atribuicao de copia modela o ato de liberar o controle do recurso atualmente mantido e compartilhar o recurso do argumento, enquanto a atribuicao de movimento modela o ato de liberar o controle do recurso atualmente mantido e transferir o controle do recurso mantido pelo argumento para o objeto atribuido:

```cpp
      // ...
      void trocar(shared_ptr &outro) noexcept {
         using std::swap;
         swap(ptr,      outro.ptr);
         swap(contador, outro.contador);
      }

      // atribuicao de copia: libera o atual, compartilha o de 'outro'
      shared_ptr& operator=(const shared_ptr &outro) {
         shared_ptr{ outro }.trocar(*this);
         return *this;
      }

      // atribuicao de movimento: libera o atual, adquire o de 'outro'
      shared_ptr& operator=(shared_ptr &&outro) noexcept {
         shared_ptr{ std::move(outro) }.trocar(*this);
         return *this;
      }
      // ...
```

A destruicao e provavelmente o aspecto mais complicado desse tipo. Queremos garantir que o ultimo proprietario do *pointee* o destrua, para evitar objetos imortais. O ponto-chave e que `shared_ptr<T>` so deve destruir o objeto `T` apontado se era o ultimo usuario daquele objeto.

Ha pelo menos dois algoritmos ingênuos "autoevidents" que nao funcionam:

1. **Primeiro algoritmo incorreto:** *Se `contador` nao for nulo, entao se `*contador==1`, faca `delete ptr` e `delete contador`*. Esse algoritmo permite o caso em que duas threads entram no destrutor concorrentemente com `*contador==2`. Nesse caso, e possivel que nenhuma thread veja `*contador==1`, e os *pointees* nunca sao destruidos — objeto imortal!

2. **Segundo algoritmo incorreto:** *Se `contador` nao for nulo, decremente `*contador`. Se `*contador==0`, faca `delete ptr` e `delete contador`*. Esse algoritmo permite o caso em que duas threads entram no destrutor concorrentemente com `*contador==2`, e ambas concorrentemente decrementam `*contador`, levando a possibilidade de ambas verem `*contador==0`, resultando em uma dupla exclusao dos *pointees*!

Ambos os casos sao ruins, ainda que por razoes diferentes, entao precisamos fazer melhor. A parte dificil do processo e garantir que a thread em execucao possa ser informada de que foi ela quem fez `*contador` chegar a zero. A solucao geral para tal problema requer encerrar duas etapas (mudar o valor de uma variavel apenas se ela tinha um valor conhecido de antemao e ser informado se essa escrita aconteceu ou nao) em uma unica operacao, algo que deve ser suportado por pelo menos uma operacao de hardware em uma maquina multi-nucleo.

O C++ oferece abstracoes sobre essas operacoes de hardware essenciais por meio de atomicos. Uma dessas operacoes atomicas se chama `compare_exchange_weak()`, que recebe o valor esperado (o que se acredita estar na variavel) e o valor desejado (o que se busca escrever na variavel, mas apenas se ela contiver o esperado), e retorna `true` apenas se a escrita realmente aconteceu. Por conveniencia, `esperado` e passado por referencia e atualizado com o valor realmente mantido pelo objeto naquele momento, pois essa funcao geralmente e chamada em um loop ate que uma escrita bem-sucedida de `desejado` realmente ocorra.

> **Uma danca com fotos**
>
> Essa danca de `esperado` e `desejado` pode ser vista como tirar fotos. Uma thread quer decrementar `*contador`, mas `*contador` mantem um estado mutavel e e acessado concorrentemente, o que significa que seu valor pode mudar a qualquer momento. Assim, tiramos uma foto (`esperado`) em uma variavel local sob nosso controle. Baseamos o valor que queremos escrever (`desejado`) naquela foto local que sabemos que nao mudou. Entao, tentamos agir com base naquele conhecimento (potencialmente obsoleto) e verificamos se nossa suposicao (que `*contador` continha `esperado`) se manteve. Isso nos informa que fomos nos quem escreveu `desejado` em `*contador`.

Com esse conhecimento, uma possivel implementacao do destrutor seria a seguinte:

```cpp
      // ...
      ~shared_ptr() {
         if (contador) {
            auto esperado = contador->load();
            auto desejado = esperado - 1;

            // tenta decrementar atomicamente ate ter certeza que conseguiu
            while (contador->compare_exchange_weak(esperado, desejado))
               desejado = esperado - 1;

            if (desejado == 0) { // eu era o ultimo proprietario de *ptr
               delete ptr;
               delete contador;
            }
         }
      }
      // ...
```

Apos o loop, sabemos que escrevemos `desejado` quando `*contador` tinha `esperado`; portanto, se `desejado` era 0 (implicando que `esperado` era 1), sabemos que eramos o ultimo usuario daquele *pointee*. Sim, e sutil. E essa e apenas uma versao de brinquedo de `shared_ptr<T>`. Poderıamos otimiza-la de muitas formas, mas isso esta alem do escopo deste livro.

> **Uma solucao mais simples**
>
> A solucao mostrada aqui com `compare_exchange_weak()` e uma das muitas opcoes a nossa disposicao. Ela foi preferida para este livro por ser uma abordagem interessante para uma solucao geral ao problema de atualizacao concorrente. Neste caso especifico, poderíamos ter substituido o loop por algo como `if ((*contador)-- == 1)`, pois se decrement armos `*contador` atomicamente e o valor anteriormente mantido era 1, sabemos com certeza que `*contador` agora e 0.

As outras funcoes membro importantes da nossa implementacao de `shared_ptr<T>` envolvem comparacao (`operator==` e `operator!=`), as funcoes `obter()` que permitem obter o `T*` bruto subjacente para o codigo que precisa dele, e os operadores de indirecao `operator*()` e `operator->()`:

```cpp
      // ...
      bool operator==(const shared_ptr &outro) const noexcept {
         return ptr == outro.ptr;
      }
      bool operator!=(const shared_ptr &outro) const noexcept {
         return !(*this == outro);
      }

      T *obter() noexcept       { return ptr; }
      const T *obter() const noexcept { return ptr; }

      T& operator*() noexcept       { return *ptr; }
      const T& operator*() const noexcept { return *ptr; }

      T* operator->() noexcept       { return ptr; }
      const T* operator->() const noexcept { return ptr; }
   };
}
```

Um exemplo muito simples de codigo cliente para este *smart pointer* seria o seguinte:

```cpp
#include <thread>
#include <chrono>
#include <random>
#include <iostream>
using namespace std::literals;

struct X {
   int n;
   X(int n) : n{ n } {}
   ~X() { std::cout << "X::~X()\n"; }
};

int main() {
   using livro_gerenciamento_memoria::shared_ptr;
   std::mt19937 prng{ std::random_device{}() };
   std::uniform_int_distribution<int> dado{ 200, 300 };

   shared_ptr<X> p{ new X{ 3 } };
   using std::chrono::milliseconds;

   // duas threads compartilham a posse de *p
   std::thread th0{ [p, dt = dado(prng)] {
      std::this_thread::sleep_for(milliseconds{dt});
      std::cout << "fim de th0, p->n : " << p->n << '\n';
   } };

   std::thread th1{ [p, dt = dado(prng)] {
      std::this_thread::sleep_for(milliseconds{dt});
      std::cout << "fim de th1, p->n : " << p->n << '\n';
   } };

   th1.detach();
   th0.detach();

   std::this_thread::sleep_for(350ms); // aguarda ambas as threads
   std::cout << "fim de main()\n";
}
```

### Algumas palavras sobre make_shared()

Para entender por que recomendamos preferir a funcao de fabrica `make_shared<T>()` a uma chamada direta ao construtor de `shared_ptr<T>`, a ideia-chave e que com o construtor de `shared_ptr<T>`, o objeto `T` e alocado pelo codigo cliente e entregue a `shared_ptr<T>` em construcao, que assume a *ownership* daquele ponteiro e aloca um contador compartilhado separadamente. Acabamos com duas alocacoes (o objeto `T` e o contador), provavelmente em *cache lines* separadas.

Agora, se passarmos por `make_shared<T>()`, essa funcao de fabrica e responsavel por alocar tanto o objeto `T` quanto o contador, encaminhando perfeitamente os argumentos recebidos para o construtor de `T`. Como a mesma funcao realiza ambas as alocacoes, ela pode fundi-las em uma unica alocacao de um bloco de memoria que contenha tanto o objeto `T` quanto o contador, colocando ambos na mesma *cache line*. Isso pode levar a caracteristicas de desempenho aprimoradas se uma unica thread tende a ler ambos os ponteiros (o `T*` e o contador) em um curto intervalo de tempo.

Para alcancar essa otimizacao, precisaremos ser capazes de criar tal bloco (conceitualmente, uma `struct` contendo `T` e um inteiro atomico) e garantir que `shared_ptr<T>` possa conter qualquer representacao (dois ponteiros separados ou um ponteiro para um bloco com dois objetos) enquanto permanece utilizavel e eficiente. O uso controlado dos truques vistos nos Capitulos 2 e 3 sera util quando chegarmos la.

## Escrevendo um ponteiro duplicante baseado em politica

Vamos deixar os *smart pointers* padrao de lado por um momento. Suponha que buscamos escrever um tipo de *smart pointer* cuja semantica nao se enquadra nem no molde de posse exclusiva de `std::unique_ptr<T>` nem no molde de posse compartilhada de `std::shared_ptr<T>`. Para fins deste exemplo, suponha mais especificamente que queremos semantica de posse exclusiva mas, ao contrario de `std::unique_ptr<T>`, que e moval, porem nao-copiavel, queremos que a duplicacao do ponteiro leve a duplicacao do *pointee*. O que podemos fazer?

Bem, isso e C++, entao claro que podemos escrever o nosso proprio. Chamemos esse novo tipo de *smart pointer* de `ptr_duplicante<T>` (para "ponteiro duplicante" ou "ponteiro que duplica o *pointee*"). Como examinamos como se poderia implementar a posse exclusiva por meio de nosso `unique_ptr<T>` caseiro anteriormente neste capitulo, esta secao se concentrara principalmente na questao de duplicar o *pointee*.

O que queremos dizer com duplicacao? Ha dois casos esperados: copiar um objeto de um tipo nao-polimórfico e copiar um objeto de um tipo polimórfico, com polimórfico significando "com pelo menos uma funcao membro virtual" para fins deste exemplo.

Por que ha uma diferenca entre tipos polimórficos e nao-polimórficos? Considere o seguinte programa:

```cpp
struct X { int n; };

struct B {
   int n;
   B(int n) : n{ n } {}
   virtual ~B() = default;
};

struct D0 : B {
   D0(int n) : B{ n } { /* ... */ }
   // ...
};

struct D1 : B {
   D1(int n) : B{ n } { /* ... */ }
   // ...
};

// precondition: p != nullptr (para simplificar)
X* duplicar(X *p) {
   return new X{ *p }; // Ok: X nao e polimórfico
}

// precondition: p != nullptr (para simplificar)
B* duplicar(B *p) {
   return new B{ *p }; // Ma ideia: e se p aponta para D0?
}

#include <memory>

int main() {
   using std::unique_ptr;
   X x{ 3 };
   unique_ptr<X> px{ duplicar(&x) }; // ok

   D0 d0{ 4 };
   unique_ptr<B> pb{ duplicar(&d0) }; // problema! Cria apenas a parte B
}
```

Podemos supor que `duplicar(X*)` pode criar com seguranca um objeto do tipo `X`, pois `X` nao tem funcao membro virtual e, portanto, provavelmente nao foi concebido para servir como classe base publica. No entanto, ha grande probabilidade de que `duplicar(B*)` faca a coisa errada ao chamar o construtor de `B`, ja que o `B*` passado como argumento poderia ser `B` ou um ponteiro para um objeto de qualquer classe derivada de `B` (aqui, `D0*`). Portanto, chamar `new B{ *p };` constroi apenas a parte base, "fatiando" qualquer estado do objeto apontado e resultando em um programa provavelmente incorreto.

Como e bem conhecido nos circulos da programacao orientada a objetos, a maneira costumeira de duplicar um objeto de um tipo polimórfico e por meio da duplicacao subjetiva, conhecida como *clonagem*. Expresso informalmente, quando se mantem um ponteiro para um objeto com pelo menos uma funcao membro virtual, a unica entidade que pode realmente afirmar conhecer o tipo do *pointee* e... o proprio *pointee*.

O que `ptr_duplicante<T>` fara, entao, e escolher uma politica de duplicacao com base nas caracteristicas de `T`: por padrao, se `T` e polimórfico, duplicaremos por clonagem; caso contrario, duplicaremos por copia. Claro, deixaremos o codigo cliente especificar um mecanismo de duplicacao personalizado se necessario.

Exploraremos tres abordagens para essa selecao de uma politica de duplicacao padrao: uma abordagem intrusiva baseada em interfaces, uma abordagem nao-intrusiva baseada em *traits* e deteccao em tempo de compilacao de uma funcao membro de clonagem usando recursos C++17, e outra abordagem nao-intrusiva baseada em concepts do C++20.

### Deteccao por interfaces

Uma coisa que poderíamos fazer no codigo do usuario e impor que tipos clonáveis implementem uma interface especifica, como neste exemplo:

```cpp
// interface: qualquer tipo clonable sabe se clonar
struct clonable {
   virtual clonable* clonar() const = 0;
   virtual ~clonable() = default;
};
```

Tal solucao provavelmente nao e digna de padronizacao: e intrusiva, impoe alguma sobrecarga, e assim por diante. Pode ser uma solucao para sua propria base de codigo, claro. Aplicando essa ideia ao exemplo que tratou errado a duplicacao de um tipo polimórfico, anteriormente, chegamos ao seguinte:

```cpp
struct X { int n; };

struct B : clonable { // todo B e clonable
   int n;
   B(int n) : n{ n } {}
   virtual ~B() = default;
protected:
   B(const B&) = default; // copia protegida: duplicacao e subjetiva
};

struct D0 : B {
   D0(int n) : B{ n } {}
   D0* clonar() const override { return new D0{ *this }; } // cria copia real
};

struct D1 : B {
   D1(int n) : B{ n } {}
   D1* clonar() const override { return new D1{ *this }; } // cria copia real
};
```

Agora, suponha que queremos desenvolver um esqueleto de `ptr_duplicante<T>` que copia tipos que nao sao derivados de `clonable` e clona tipos que sao. Para isso, podemos usar o *trait* `std::conditional` e escolher entre dois tipos de objetos de funcao, um tipo `Copiador` que copia e um tipo `Clonador` que clona:

```cpp
struct Copiador {
   template <class T>
   T* operator()(const T *p) const {
      return new T{ *p }; // duplica por copia
   }
};

struct Clonador {
   template <class T>
   T* operator()(const T *p) const {
      return p->clonar(); // duplica por clonagem subjetiva
   }
};

#include <type_traits>

template <class T,
          class Dup = std::conditional_t<
             std::is_base_of_v<clonable, T>,
             Clonador, Copiador  // escolhe o mecanismo certo
          >>
class ptr_duplicante {
   T *ptr{};
public:
   ptr_duplicante(const ptr_duplicante &outro)
      : ptr{ outro.vazio() ? nullptr : Dup{}(outro.ptr) } {
   }
   // ...
};
```

### Deteccao por traits

Se nao quisermos impor uma classe base aos nossos tipos clonáveis, podemos usar *type traits* para detectar a presenca de uma funcao membro `clonar()` qualificada com `const` e supor que isso e uma indicacao razoavel de que a clonagem e uma opcao melhor do que a copia. Note que essa nao-intrusividade supe um acordo tacito sobre o significado de `clonar()`.

Podemos alcancar isso de varias maneiras, mas a mais limpa e geral provavelmente usa o `std::void_t` do Dr. Walter Brown, encontrado em `<type_traits>` desde o C++17:

```cpp
// tipos Clonador e Copiador (veja acima)

template <class, class = void>
struct tem_clonar : std::false_type {};  // por padrao: nao tem clonar()

template <class T>
struct tem_clonar<T, std::void_t<
   decltype(std::declval<const T*>()->clonar())  // testa se clonar() existe
>> : std::true_type {};  // especializacao: tem clonar()

template <class T>
constexpr bool tem_clonar_v = tem_clonar<T>::value;

template <class T, class Dup = std::conditional_t<
   tem_clonar_v<T>, Clonador, Copiador  // escolhe com base no trait
>>
class ptr_duplicante {
   T *ptr{};
public:
   // ...
   ptr_duplicante(const ptr_duplicante &outro)
      : ptr{ outro.vazio() ? nullptr : Dup{}(outro.ptr) } {
   }
   // ...
};
```

> **Uma palavra sobre `std::void_t`**
>
> O tipo `std::void_t` e uma obra brilhante. Usá-lo depende da ideia SFINAE (*Substitution Failure Is Not An Error*) para escolher entre uma implementacao basica e geral que diz "nao" e uma versao especializada que diz "sim" quando alguma expressao e bem formada. Em nosso caso, `tem_clonar<T>` e `false` para a maioria dos tipos, mas e `true` quando a expressao `p->clonar()` e valida para algum objeto `const T*`. Que possamos testar facilmente a validade de qualquer expressao mesmo antes de os concepts existirem e simplesmente lindo, e devemos muito ao Dr. Walter Brown por essa joia.

### Deteccao por concepts

Desde o C++20, truques como `std::void_t` sao menos uteis do que eram, ja que os *concepts* agora fazem parte do sistema de tipos da linguagem. Por meio de *concepts*, podemos definir um tipo `clonable`, `T`, como algo para o qual uma chamada a `clonar()` e bem-formada em `const T*` e produz algo convertivel para `T*`.

Com isso, temos o seguinte:

```cpp
// concept: define o que significa ser "clonable"
template <class T>
concept clonable = requires(const T *p) {
   { p->clonar() } -> std::convertible_to<T*>;
};

template <class T, class Dup = std::conditional_t<
   clonable<T>, Clonador, Copiador  // elegante e expressivo
>>
class ptr_duplicante {
   T *ptr{};
public:
   // ...
   ptr_duplicante(const ptr_duplicante &outro)
      : ptr{ outro.vazio() ? nullptr : Dup{}(outro.ptr) } {
   }
   // ...
};
```

*Concepts*, como *traits*, sao uma solucao nao-intrusiva para esse problema. Onde *traits* sao uma tecnica de programacao, no entanto, *concepts* estao incorporados no sistema de tipos e podemos (por exemplo) escrever codigo especializado para `clonable<T>` e codigo que nao e. Em nosso caso, o fato de que queremos deixar uma porta aberta para tipos que nao usam nem o construtor de copia nem uma funcao membro `clonar()` sugere que a configuracao atual, que deixa o codigo cliente fornecer outros mecanismos de duplicacao, e provavelmente preferivel.

> **C++26**
>
> O C++26 contera dois tipos padrao chamados `std::indirect` e `std::polymorphic` que cobrirao um nicho proximo ao descrito por este `ptr_duplicante`. Foi aprovado em 15 de fevereiro de 2025.

## Alguns ponteiros "pouco inteligentes", mas uteis

Entao, temos *smart pointers* padrao como `unique_ptr<T>` (posse exclusiva) e `shared_ptr<T>` (posse compartilhada), e podemos escrever os nossos proprios para situacoes mais exoticas. Ha outras semanticas comuns que poderíamos querer incorporar no sistema de tipos do nosso programa?

Bem, ha pelo menos duas "faceis" que alguem poderia pensar: implementar uma semantica "nunca nulo" e implementar uma semantica "apenas observando".

### Tipo ponteiro_nao_nulo

Voltemos a um exemplo anterior onde escrevemos o seguinte:

```cpp
// precondition: p != nullptr (para simplificar)
X* duplicar(X *p) {
   return new X{ *p }; // Ok
}
```

Note o comentario, que coloca o onus de nao fornecer um ponteiro nulo no codigo do usuario. Poderíamos ter abordado essa restricao de varias outras maneiras, incluindo:

- Fazer `assert(!p)`;
- Chamar `std::abort()` se `!p`;
- Chamar `std::terminate()` se `!p`;
- Lançar se `!p`, e assim por diante.

O importante e que se nos importamos com ponteiros sendo nao-nulos, e se injetamos testes `if(!p)` em nosso codigo de execucao, provavelmente estamos fazendo algo errado, pois isso poderia (ou deveria?) fazer parte do sistema de tipos: *essa funcao so aceita ponteiros nao-nulos*. O codigo fala mais alto do que os comentarios.

Essa ideia aparece em algumas bibliotecas comerciais (por exemplo, `gsl::non_null<T>` da *guideline support library* oferecida por alguns grandes fornecedores de compiladores) e e facil de implementar, desde que haja uma maneira clara de sinalizar erros. Para fins do exemplo, suporemos que essa maneira clara e lançar uma excecao:

```cpp
class ponteiro_invalido {};

template <class T>
class ponteiro_nao_nulo {
   T *ptr;
public:
   ponteiro_nao_nulo(T *p) : ptr{ p } {
      if (!p) throw ponteiro_invalido{}; // garante nao-nulo na construcao
   }

   T* obter() const { return ptr; }

   constexpr operator bool() const noexcept {
      return true; // sempre verdadeiro por definicao!
   }
   // ...
};
```

Usando esse tipo, qualquer funcao que aceite um argumento `ponteiro_nao_nulo<T>` sabe que o ponteiro `T*` ai contido sera nao-nulo, aliviando o codigo cliente do onus da validacao. Isso torna `ponteiro_nao_nulo<T>` um tipo excelente para a interface de funcoes que esperam um `T*` nao-nulo.

Em termos de uso, vejamos isso:

```cpp
struct X { int n; };
class invalido {};

// versao antiga: precisa validar o ponteiro
int extrair_valor(const X *p) {
   if (!p) throw invalido{};
   return p->n;
}

#include <iostream>
int main() try {
   X x{ 3 };
   std::cout << extrair_valor(&x) << '\n'
             << extrair_valor(nullptr) << '\n'; // lança invalido
} catch(invalido) {
   std::cerr << "oops\n";
}
```

Agora, compare com isso, supondo que `ponteiro_nao_nulo<T>` lança quando construido com um ponteiro nulo:

```cpp
struct X { int n; };

// versao nova: validacao esta no sistema de tipos!
int extrair_valor(const ponteiro_nao_nulo<X> &p) {
   return p->n; // sem necessidade de validar: o tipo garante nao-nulo
}

#include <iostream>
int main() try {
   X x{ 3 };
   std::cout << extrair_valor(&x) << '\n'
             << extrair_valor(nullptr) << '\n'; // lanca no construtor
} catch(...) {
   std::cerr << "oops\n";
}
```

As duas principais vantagens de `ponteiro_nao_nulo<T>` sobre `T*` neste caso sao que o sistema de tipos documenta melhor a intencao (com `T*`, um ponteiro nulo pode ser valido; com `ponteiro_nao_nulo<T>`, claramente nao) e que as funcoes chamadas podem prosseguir sem validar, pois a validacao esta incorporada no sistema de tipos. Usar um tipo mais rico do que `T*` melhora tanto o codigo do chamador quanto o do chamado.

### Tipo ponteiro_observador

Que tal ter um tipo de *smart pointer* muito simples chamado `ponteiro_observador<T>` que se preocupa apenas em expressar a ideia de que aquele ponteiro "inteligente" e, de fato, nao um ponteiro, no sentido de que operacoes que se aplicariam a um *raw pointer* sao restritas naquele tipo. O problema canonico e que aplicar `delete` em `T*` funcionaria, mas aplicar `delete` em `ponteiro_observador<T>` nao funcionaria, pois `ponteiro_observador<T>` e... nao um ponteiro. De fato, considere o seguinte:

```cpp
class X { /* ... */ };

void processar(X *p) {
   // usa *p
   // passamos um raw pointer para processar(), entao ela deve
   // OBSERVAR, nao POSSUIR
   delete p; // espera! Voce nao deveria fazer isso!
}
```

Voce pode dizer, como o comentario afirma, "Mas aquela funcao nao deveria fazer isso! Ela nao possui `*p`!" mas, bem, erros acontecem, assim como mal-entendidos. Neste caso, o impacto dos mal-entendidos e agravado pelo fato de que nada no tipo do argumento indica que aplicar `operator delete` em `p` e incorreto!

Agora, vamos mudar ligeiramente a assinatura:

```cpp
class X { /* ... */ };

void processar(ponteiro_observador<X> p) {
   // usa *p
   // delete p; // erro de compilacao: nao compila!
}
```

O comentario "usa `*p`" permanece o mesmo em ambas as versoes. O tipo `ponteiro_observador<T>` oferece versoes quase triviais de todos os operadores e funcoes membro razoaveis (`obter()`, `operator*()`, `operator->()`, `vazio()`, e assim por diante), portanto, o uso de `T*` e de `ponteiro_observador<T>` deve ser equivalente no codigo do usuario; a unica diferenca esta em usos equivocados como aplicar `delete` ou realizar aritmetica de ponteiros.

As vezes, apenas esclarecer a intencao em uma interface de funcao torna o codigo melhor.

## Resumo

No Capitulo 5, dedicamos algum tempo ao uso correto dos *smart pointers* padrao. No capitulo atual, "sujamos as maos", por assim dizer, e escrevemos versoes caseiras (e simplificadas) de `unique_ptr<T>` e `shared_ptr<T>`. Como mencionado mais de uma vez, isso pretende ser uma exploracao educacional, ja que o fornecedor de biblioteca do seu compilador certamente fornece implementacoes significativamente melhores (mais completas, mais eficientes, melhor testadas, etc.) em ambos os casos.

Neste capitulo, tambem exploramos a possibilidade de fornecer tipos de *smart pointer* caseiros, com um `ptr_duplicante<T>` baseado em politica e baseado em tres abordagens distintas para a selecao de um algoritmo de duplicacao. A intencao era mostrar que pode ser feito, como pode ser feito e como podemos fornecer padroes razoaveis e utilizaveis sem bloquear o codigo do usuario com requisitos mais exoticos.

No final deste capitulo, examinamos alguns ponteiros relativamente simples (mas uteis) que podem ser usados nas fronteiras de funcoes (tipicamente, como tipos de argumento) para tornar os requisitos semanticos implicitos por meio do sistema de tipos, em vez de forcam o codigo do usuario a impor esses requisitos explicitamente... e as vezes falhar em faze-lo.

Sem surpresas, o gerenciamento de memoria nao se limita a *smart pointers*. No proximo capitulo, exploraremos como os operadores `new`, `new[]`, `delete` e `delete[]` funcionam, como podemos implementa-los nos mesmos, e por que as vezes queremos faze-lo.
