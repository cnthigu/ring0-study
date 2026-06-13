# Capitulo 5 - Usando Smart Pointers Padrao (Using Standard Smart Pointers)

> Traducao nao oficial do livro *C++ Memory Management* (Patrice Roy, 2025).
> Texto adaptado para portugues brasileiro com fins educacionais. Paginas originais: 83-109.
> Repositorio: [ring0-study](https://github.com/cnthigu/ring0-study)

> Navegacao: [Anterior](capitulo-04.md) | [Indice](README.md) | [Proximo](capitulo-06.md)

---

O C++ enfatiza a programacao com valores. Por padrao, seu codigo usa objetos, nao indireccoes (referencias e ponteiros) para objetos. O acesso indireto a objetos e, claro, permitido, e raro e o programa que nunca usa tal semantica, mas e uma escolha opt-in que requer sintaxe adicional. O Capitulo 4 explorou a associacao do gerenciamento de recursos ao *lifetime* de objetos por meio de destrutores e do idioma RAII, demonstrando um dos principais pontos fortes do C++: praticamente todos os recursos (inclusive a memoria) podem ser tratados implicitamente pela propria mecanica da linguagem.

O C++ permite o uso de *raw pointers* (ponteiros brutos) no codigo, mas nao os encoraja ativamente. Muito pelo contrario, de fato — *raw pointers* sao um mecanismo de baixo nivel, extremamente eficiente, mas facil de usar de forma errada, e a partir do qual nao e simples inferir a responsabilidade sobre o *pointee* diretamente no codigo-fonte. A partir do (hoje removido) recurso `auto_ptr<T>` de decadas atras, houve um esforco na comunidade C++ para definir abstracoes em torno de mecanismos de mais baixo nivel, como os *raw pointers*, por meio de tipos que fornecem semantica clara e bem definida e reduzem o risco de erros de programacao. Esse esforco teve exito significativo, em grande parte devido a expressividade da linguagem C++ e sua capacidade de criar abstracoes poderosas e eficientes sem perder velocidade nem usar mais memoria em tempo de execucao. Por essa razao, no C++ contemporaneo, os *raw pointers* geralmente sao encapsulados por abstracoes mais dificeis de usar de forma errada — exemplos incluem containers padrao e *smart pointers*, como os que exploraremos neste capitulo; *raw pointers* que nao sao encapsulados sao usados principalmente para significar "aqui esta um recurso que voce pode usar, mas do qual nao e dono."

Este capitulo vera como usar os tipos de *smart pointer* padrao do C++. Primeiro, examinaremos o que eles sao, e depois nos aprofundaremos nas formas de usar os principais tipos de *smart pointer* eficientemente. Por fim, veremos os momentos em que precisamos "sujar as maos" (por assim dizer) e usar *raw pointers*, idealmente (mas nao apenas) pela mediacao de *smart pointers*. Isso devera nos ensinar a escolher *smart pointers* padrao para um determinado caso de uso, como usa-los adequadamente, e como lidar com recursos que precisam ser liberados por mecanismos personalizados. Ao longo dessa jornada, manteremos em mente e explicaremos os custos das escolhas que fizermos.

Neste capitulo, faremos o seguinte:

- Dar uma rapida olhada na ideia geral dos *smart pointers* padrao para desenvolver uma nocao do motivo de sua existencia;
- Examinar mais de perto `std::unique_ptr`, incluindo como ele pode ser usado para tratar escalares, arrays e liberar recursos alocados de maneiras atipicas;
- Examinar `std::shared_ptr` e os casos de uso para esse tipo essencial, porem mais custoso, a fim de compreender quando alternativas devem ser preferidas;
- Dar uma rapida olhada em `std::weak_ptr`, um complemento de `std::shared_ptr` util quando ha necessidade de modelar posse temporaria compartilhada;
- Examinar os casos onde *raw pointers* devem ser usados, pois eles ainda tem seu lugar no ecossistema C++.

Prontos? Vamos la!

## Requisitos tecnicos

O codigo deste capitulo pode ser encontrado em: [https://github.com/PacktPublishing/C-Plus-Plus-Memory-Management/tree/main/chapter5](https://github.com/PacktPublishing/C-Plus-Plus-Memory-Management/tree/main/chapter5)

## Os smart pointers padrao

O C++ tem um "zoologico" relativamente pequeno de *smart pointers*. Antes de examinar o conjunto de opcoes fornecidas pelo padrao, vamos nos deter um momento para mostrar o problema que estamos tentando resolver. Considere o seguinte programa (deliberadamente incompleto). Voce ve algo de errado?

```cpp
class X {
   // ...
};

// retorna um ponteiro para X
X *criar_x();

// usa o objeto X apontado por p
void usar(X *p);

void processar() {
   X *p = criar_x();
   usar(p);
   delete p;
}
```

Esse e um codigo legal mas que voce nao quer ver em um programa contemporaneo. Ha muita coisa que pode dar errado, como os seguintes itens de uma lista nao exaustiva de problemas em potencial:

- Nao sabemos se `usar()` chamara `delete p`, levando a um segundo `delete` (em um objeto ja destruido!) em `processar()` depois;
- Nao sabemos se `usar()` pode lançar, caso em que a instrucao `delete p;` em `processar()` jamais sera alcancada;
- Nao sabemos se `processar()` deve ser considerada a "dona" de `p`, no sentido de que nao sabemos se ela e responsavel por chamar `operator delete()` em `p` (talvez seja responsabilidade de `usar()`, ou de outra funcao);
- Nao sabemos se o que `p` aponta foi alocado com `new`, `new[]` ou algo mais (`malloc()`, algum recurso de outra linguagem, algum utilitario personalizado em sua base de codigo, etc.);
- Nem sabemos se o que `p` aponta foi alocado dinamicamente de forma alguma: `p` poderia apontar para uma variavel global ou `static` declarada em `criar_x()`, por exemplo (uma ma ideia, mas algumas pessoas fazem isso — por exemplo, ao implementar o padrao de projeto singleton de forma nao idiomatica para C++).

Compare, por exemplo, duas possiveis implementacoes de `criar_x()` (ha muitas outras que poderıamos considerar, mas essas sao suficientes por ora):

```cpp
X *criar_x() { // possibilidade 1: aloca dinamicamente
   return new X;
}

X *criar_x() { // possibilidade 2: retorna endereco de estatico
   static X x;
   return &x;
}
```

No primeiro caso, pode fazer sentido chamar `delete` no ponteiro retornado, mas no segundo caso, isso seria desastroso. Nada na assinatura da funcao informa claramente ao codigo cliente se estamos diante de uma situacao ou de outra, ou mesmo de algo completamente diferente.

Como "bonus" de sorte, o que acontece se alguem chamar `criar_x()` sem usar o valor retornado? Se `criar_x()` esta implementada como `return new X;` ou algo semelhante, entao o codigo vazara — uma perspectiva desagradavel. Note que desde o C++17, voce pode mitigar esse problema especifico anotando o tipo de retorno de `criar_x()` com o atributo `[[nodiscard]]`, mas ainda e algo que voce deveria conhecer. Retornar *raw pointers* de uma funcao e algo que tentamos principalmente evitar, mesmo que as vezes precisemos faze-lo.

Ha outras possiveis armadilhas aqui, e todas tem um tema em comum — ao usar *raw pointers*, tradicionalmente nao podemos dizer a partir do codigo-fonte qual e a semantica. Mais especificamente, nao podemos dizer com certeza quem e responsavel tanto pelo ponteiro quanto pelo que ele aponta. O fato de que os *raw pointers* nao fornecem informacoes claras sobre *ownership* tem sido uma fonte recorrente de bugs em C++ ao longo dos anos.

Agora, para uma situacao diferente, considere o seguinte trecho de codigo:

```cpp
void f() {
   X *p = new X;
   std::thread th0{ [p] { /* usa *p */ } };
   std::thread th1{ [p] { /* usa *p */ } };
   th0.detach();
   th1.detach();
}
```

Neste caso, `f()` aloca um objeto `X` apontado por `p`, apos o que duas threads, `th0` e `th1`, copiam `p` (assim compartilhando o objeto `X` para o qual `p` aponta). Finalmente, `th0` e `th1` sao desanexadas (*detached*), o que significa que as threads rodarao ate a conclusao mesmo apos `f()` terminar. Se nao sabemos em que ordem `th0` e `th1` serao concluidas, nao podemos dizer claramente qual delas deve ser responsavel por chamar `operator delete()` em `p`. Esse e mais um problema de responsabilidade pouco clara sobre o *pointee*, mas de um tipo diferente do nosso primeiro exemplo, e por isso requer uma solucao diferente.

Para os casos em que existe um ultimo proprietario claramente identificado de um objeto apontado, independentemente de o *pointee* ser compartilhado ou nao entre ponteiros, voce provavelmente quer usar `std::unique_ptr`. No caso (mais de nicho, mas muito real e bastante sutil) em que o objeto apontado e compartilhado por pelo menos dois "co-proprietarios" e a ordem em que esses proprietarios serao destruidos e a priori desconhecida, `std::shared_ptr` e a ferramenta certa. As secoes a seguir entram em mais detalhes sobre os papeis e o significado desses tipos, esperancosamente ajudando voce a fazer uma escolha informada ao selecionar um tipo de *smart pointer* para um determinado caso de uso.

### Sobre a expressao de intencao por meio de assinaturas de funcoes

Embora ainda nao tenhamos examinado os *smart pointers* padrao em detalhe, pode ser apropriado oferecer algumas palavras sobre o que eles significam, em particular `std::unique_ptr` e `std::shared_ptr`. Esses dois tipos transmitem semantica de *ownership* — `std::unique_ptr` representa a posse exclusiva do *pointee*, e `std::shared_ptr` representa a co-posse (ou posse compartilhada) do *pointee`. E importante entender a diferenca entre possuir (em particular, co-possuir) um *pointee* e compartilhar um *pointee*. Considere o seguinte exemplo, que usa `std::unique_ptr` (embora ainda nao o tenhamos coberto, mas estamos chegando la) e *raw pointers* juntos para inscerever a semantica de *ownership* no sistema de tipos:

```cpp
#include <memory>
#include <iostream>

// exibir_pointee() COMPARTILHA um ponteiro com o chamador
// mas NAO assume a posse (ownership)
template <class T>
void exibir_pointee(T *p) {
   if (p) std::cout << *p << '\n';
}

template <class T>
std::unique_ptr<T> criar_um(const T &valor) {
   return std::make_unique<T>(valor);
}

int main() {
   auto p = criar_um(3); // p e um std::unique_ptr<int>
   exibir_pointee(p.get()); // chamador e chamado COMPARTILHAM
                            // o ponteiro durante esta chamada
}
```

Como mencionado ao apresentar este exemplo, usamos um objeto `std::unique_ptr` para modelar a *ownership* — `criar_um()` constroi `std::unique_ptr<T>` e transfere a posse para o chamador; esse chamador mantem a posse daquele objeto e compartilha o ponteiro subjacente com outros (aqui, `exibir_pointee()`), mas nao renuncia a posse do *pointee*. Usar sem possuir e modelado por um *raw pointer*. Isso nos mostra, de forma muito simplificada, que ha uma diferenca entre possuir e compartilhar um recurso — `p` em `main()` possui o recurso, mas o compartilha com o nao-proprietario `p` em `exibir_pointee()`. Esse e um codigo C++ seguro e idiomatico.

Sabendo que os tipos de *smart pointer* padrao modelam a *ownership*, sabemos que enquanto houver um unico e claro ultimo usuario de um recurso, `std::unique_ptr` tende a ser o tipo preferido; e muito mais leve do que `std::shared_ptr` (como veremos), e fornece a semantica de *ownership* adequada.

Ha, claro, casos de uso em que `std::unique_ptr` nao e uma boa escolha. Considere este trecho de codigo simplificado, nao seguro para threads e incompleto:

```cpp
class entidade {
   bool reservada{ false };
public:
   void reservar()  { reservada = true;  }
   void liberar()   { reservada = false; }
   bool esta_reservada() const { return reservada; }
   // ...
};

constexpr int N = /* ... */;
// 'pool' e onde os objetos entidade vivem.
// Nao alocamos dinamicamente, mas se o fizessemos,
// usariamos unique_ptr<entidade> pois este sera
// o unico ponto de uso final para esses objetos
std::array<entidade, N> pool;

class sem_entidades_livres {};

// esta funcao retorna um ponteiro NAO-PROPRIETARIO
// (Capitulo 6 cobrira opcoes mais ergonomicas que raw pointer)
entidade *emprestar_uma() {
   if (auto it = std::find_if(std::begin(pool), std::end(pool),
               [](auto &&e) { return !e.esta_reservada(); });
       it != std::end(pool)) {
      it->reservar();
      return &(*it); // ponteiro nao-proprietario
   }
   throw sem_entidades_livres{};
}
```

Note que `emprestar_uma()` compartilha um ponteiro com o codigo chamador, mas nao compartilha a *ownership* daquele ponteiro — o fornecedor dos objetos `entidade` permanece como o unico responsavel pelo *lifetime* desses objetos neste caso. Isso nao seria um caso nem para `std::unique_ptr` (o unico proprietario do recurso) nem para `std::shared_ptr` (o co-proprietario do recurso). Ha alternativas ao uso de *raw pointers* para expressar um ponteiro nao-proprietario, como veremos no Capitulo 6.

O ponto importante aqui e que as assinaturas de funcoes transmitem significado, e e importante usar os tipos que expressem nossa intencao. Para isso, precisamos entender essa intencao. Vamos ter isso em mente enquanto exploramos como usar os *smart pointers* padrao a nosso favor nas secoes a seguir.

## Tipo unique_ptr

Como o nome sugere, um objeto `unique_ptr<T>` representa a posse exclusiva (unica) de um objeto apontado. Esse e um caso comum — talvez ate o caso mais comum — de semantica de *ownership* ao lidar com memoria alocada dinamicamente.

Considere nosso primeiro exemplo (ainda deliberadamente incompleto) neste capitulo, onde a *ownership* do *pointee* nao era algo que podiamos determinar a partir do codigo-fonte, e vamos reescrevê-lo com objetos `unique_ptr` no lugar de *raw pointers*:

```cpp
#include <memory>

class X {
   // ...
};

// f() RETORNA a posse do objeto X criado
std::unique_ptr<X> criar_x();

// g() USA o X sem se tornar responsavel por ele
void usar(std::unique_ptr<X> &ref_p);

void processar() {
   // poderiamos escrever std::unique_ptr<X> em vez de auto
   auto p = criar_x();
   usar(p);
} // p libera implicitamente o objeto X aqui
```

Com este codigo, fica claro que o objeto retornado por `criar_x()` e responsavel pelo *lifetime* do objeto `X` que aponta, e tambem fica claro que `usar()` acessa o `X` encapsulado sem se tornar responsavel pelo objeto apontado. Some-se a isso o fato de que `p` e um objeto e, como tal, sera destruido se `usar()` lançar ou se `criar_x()` for chamada de forma que o codigo chamador se esqueca de usar o valor de retorno, e voce obtera um programa *exception-safe* — mais curto e simples que o original!

> **Murphy e Maquiavel**
>
> Voce pode estar pensando: "mas tenho certeza que poderia roubar o ponteiro gerenciado pelo `std::unique_ptr` dentro de `usar()`", e estaria certo. Nao e apenas possivel mas tambem facil, ja que `unique_ptr` oferece acesso direto ao ponteiro subjacente de mais de uma maneira. No entanto, o sistema de tipos foi projetado para nos proteger de acidentes e fazer com que codigo bem escrito razoavel funcione bem. Ele o protegera de Murphy (os acidentes que acontecem), nao de Maquiavel (o codigo deliberadamente hostil).
>
> Se voce escrever codigo deliberadamente quebrado, acabara com um programa deliberadamente quebrado. E praticamente o que voce esperaria.

Em termos de semantica, voce poderia contar uma historia apenas com assinaturas de funcoes usando objetos `std::unique_ptr`. Note que no exemplo a seguir as funcoes foram deixadas deliberadamente incompletas para deixar claro que nos preocupamos apenas com suas assinaturas:

```cpp
// cria dinamicamente um X (ou algo derivado de X)
// e o retorna sem risco de vazamento
std::unique_ptr<X> fabrica(args);

// passa-por-valor = passa-por-movimento na pratica,
// pois unique_ptr nao e copiavel
std::unique_ptr<X> emprestar(std::unique_ptr<X>);

// passa-por-referencia para permitir mutar o pointee
// Na pratica, X* seria melhor aqui
void possivelmente_mutar(std::unique_ptr<X> &);

// passa-por-referencia-para-const para consultar
// o pointee sem mutar. Prefira const X* aqui
void consultar(const std::unique_ptr<X> &);

// consumir() engole o objeto passado: entra, nunca sai.
// Poderia usar passagem-por-valor, mas a intencao fica
// mais clara com rvalue-reference
void consumir(std::unique_ptr<X> &&);
```

Como podemos ver, as assinaturas de funcoes nos falam. E melhor prestarmos atencao.

### Tratando objetos

O tipo `unique_ptr` e uma ferramenta notavel, com a qual voce deve se esforcar para se familiarizar se ainda nao o fez. Aqui estao alguns fatos interessantes sobre esse tipo e como ele pode ser usado para gerenciar ponteiros para objetos.

Um objeto `unique_ptr<T>` e nao-copiavel, pois seu construtor de copia e a funcao membro de atribuicao de copia sao marcados como deletados. E por isso que `usar()` no primeiro exemplo do tipo `unique_ptr` recebe seu argumento por referencia — `usar()` compartilha o *pointee* com o chamador, mas nao assume a sua posse. Tambem poderíamos ter expressado `usar()` recebendo `X*` como argumento, com a aceitacao contemporanea de que argumentos de funcao que sao *raw pointers* tem a intencao de modelar o uso de um ponteiro sem possuí-lo:

```cpp
#include <memory>

class X { /* ... */ };

// criacao retorna posse
std::unique_ptr<X> criar_x();

// uso nao implica em posse
void usar(X *p_sem_posse);

void processar() {
   auto p = criar_x();
   usar(p.get()); // .get() entrega o raw pointer sem transferir posse
} // p libera o objeto X aqui
```

`unique_ptr<T>` tambem e moval — um `unique_ptr<T>` "movido-de" se comporta como um ponteiro nulo, pois o movimento para esse tipo implementa semanticamente uma transferencia de *ownership*. Isso torna mais simples a implementacao de varios tipos que precisam gerenciar recursos indiretamente.

Considere, por exemplo, a seguinte classe `sistema_solar`, que supoe um tipo hipotetico `Planeta` bem como uma implementacao hipotetica para `criar_planeta()`:

```cpp
#include "planeta.h"
#include <memory>
#include <string>
#include <vector>

std::unique_ptr<Planeta> criar_planeta(std::string_view nome_arquivo);

class sistema_solar {
   // cada planeta e possuido exclusivamente pelo sistema_solar
   std::vector<std::unique_ptr<Planeta>> planetas {
      criar_planeta("mercurio.dat"),
      criar_planeta("venus.dat"),
      // etc.
   };
public:
   // sistema_solar nao e copiavel por padrao (unique_ptr nao copia)
   // sistema_solar e moval por padrao
   // nao precisamos de ~sistema_solar pois 'planetas' cuida disso
};
```

Se tivessemos decidido implementar `sistema_solar` com `vector<Planeta*>` ou como `Planeta*`, o gerenciamento de memoria do nosso tipo teria que ser realizado pelo proprio `sistema_solar`, adicionando complexidade. Como usamos `vector<unique_ptr<Planeta>>`, tudo e implicitamente correto por padrao. Claro, dependendo do que estamos fazendo, `vector<Planeta>` pode ser ainda melhor, mas suponhamos que precisamos de ponteiros para este exemplo.

Um `unique_ptr<T>` oferece a maioria das mesmas operacoes que `T*`, incluindo `operator*()` e `operator->()`, bem como a capacidade de compara-los com `==` ou `!=` para verificar se dois objetos `unique_ptr<T>` apontam para o mesmo objeto `T`. Os dois ultimos podem parecer estranhos, ja que o tipo representa a posse exclusiva do *pointee*, mas voce poderia usar referencias a `unique_ptr<T>`, caso em que essas funcoes fazem sentido:

```cpp
#include <memory>

template <class T>
bool apontam_para_o_mesmo(const std::unique_ptr<T> &p0,
                          const std::unique_ptr<T> &p1) {
   return p0 == p1; // compara os enderecos
}

template <class T>
bool tem_mesmo_valor(const std::unique_ptr<T> &p0,
                     const std::unique_ptr<T> &p1) {
   return p0 && p1 && *p0 == *p1; // compara os valores apontados
}

#include <cassert>

int main() {
   // dois ponteiros distintos para objetos com o mesmo valor
   std::unique_ptr<int> a{ new int { 3 } };
   std::unique_ptr<int> b{ new int { 3 } };
   assert(apontam_para_o_mesmo(a, a) && tem_mesmo_valor(a, a));
   assert(!apontam_para_o_mesmo(a, b) && tem_mesmo_valor(a, b));
}
```

Por boas razoes, voce nao pode fazer aritmetica de ponteiros em `unique_ptr<T>`. Se precisar de aritmetica de ponteiros (e as vezes precisaremos — por exemplo, ao escrevermos nossos proprios containers no Capitulo 13), e sempre possivel acessar o *raw pointer* de um `unique_pointer<T>` por meio de sua funcao membro `get()`. Isso e frequentemente util ao interagir com bibliotecas C, fazer chamadas de sistema ou chamar funcoes que usam um *raw pointer* sem assumir a sua posse.

> **Curiosidade:** `sizeof(unique_ptr<T>) == sizeof(T*)` com poucas excecoes que serao discutidas mais tarde neste capitulo. Isso significa que geralmente nao ha custo em termos de espaco de memoria ao usar um *smart pointer* no lugar de um *raw pointer*. Em outras palavras, por padrao, o unico estado encontrado em um objeto `unique_ptr<T>` e `T*`.

### Tratando arrays

Um aspecto interessante de `unique_ptr` e que ele oferece uma especializacao para tratar arrays. Considere o seguinte:

```cpp
void f(int n) {
   // p aponta para um int de valor 3
   std::unique_ptr<int> p{ new int{ 3 } };

   // q aponta para um array de n ints inicializados com zero
   std::unique_ptr<int[]> q{ new int[n]{} };

   // exemplo de uso
   std::cout << *p << '\n'; // exibe 3

   for (int i = 0; i != n; ++i) {
      // operator[] e suportado para unique_ptr<T[]>
      q[i] = i + 1;
   }
   // ...

} // destrutor de q chama delete[] no seu pointee
  // destrutor de p chama delete no seu pointee
```

Para que serve isso? Depende das suas necessidades. Por exemplo:

- Se voce precisa de um array de `T` de tamanho variavel que cresce conforme necessario, use `vector<T>`. E uma ferramenta maravilhosa e extremamente eficiente quando bem usada.
- Se voce quer um array de tamanho fixo pequeno o suficiente para caber na pilha de execucao, onde o numero de elementos `N` e conhecido em tempo de compilacao, use um array bruto de `T` ou um objeto do tipo `std::array<T,N>`.
- Se voce quer um array de tamanho fixo que ou nao e pequeno o suficiente para caber na pilha, ou cujo numero de elementos `n` e conhecido apenas em tempo de execucao, voce pode usar `vector<T>` (ainda uma otima escolha!), ou pode usar `unique_ptr<T[]>`. Nesse ultimo caso, voce precisara controlar o tamanho separadamente, ja que `unique_ptr` nao faz esse rastreamento. Alternativamente, voce pode encapsula-lo em sua propria abstracao, como `array_tamanho_fixo<T>`:

```cpp
#include <cstddef>
#include <memory>

template <class T>
class array_tamanho_fixo {
   std::size_t num_elementos{};
   std::unique_ptr<T[]> dados{};
public:
   array_tamanho_fixo() = default;

   auto tamanho() const { return num_elementos; }
   bool vazio()   const { return tamanho() == 0; }

   array_tamanho_fixo(std::size_t n)
      : num_elementos{ n }, dados{ new T[n]{} } {
   }

   T& operator[](int i)       { return dados[i]; }
   const T& operator[](int i) const { return dados[i]; }
   // etc.
};
```

Essa e uma implementacao ingênua que reune o conhecimento do numero de elementos com a posse implicita do recurso. Note que nao precisamos escrever as operacoes de copia, as de movimento nem o destrutor, pois todos eles fazem implicitamente algo razoavel. Alem disso, esse tipo sera relativamente eficiente se o tipo `T` for trivialmente construivel, mas nao sera tao eficiente quanto `vector<T>` para numerosos casos de uso. Por que? Acontece que `vector` faz um gerenciamento de memoria significativamente melhor do que o nosso... mas chegaremos la.

### Deleters personalizados

Voce pode estar pensando: "na minha base de codigo, nao usamos `delete` para desalocar objetos por [insira sua razao favorita aqui], entao nao posso usar `unique_ptr`". De fato, ha muitas situacoes em que aplicar `operator delete` em um ponteiro para destruir o objeto apontado nao e uma opcao:

- As vezes, `T::~T()` e privado ou protegido, tornando-o inacessivel a outras classes como `unique_ptr<T>`;
- As vezes, a semantica de finalizacao exige fazer algo diferente de chamar `delete` — por exemplo, chamar uma funcao membro `destroy()` ou `release()`;
- As vezes, a expectativa e chamar uma funcao livre que realizara trabalho auxiliar alem de liberar um recurso.

Independente das razoes para liberar um recurso de maneira nao convencional, `unique_ptr<T>` pode receber um *deleter* personalizado que realizara essas tarefas de desalocacao customizadas. Um *deleter* personalizado pode ser um functor ou uma funcao que sera aplicado ao `T*` armazenado dentro de `unique_ptr<T>` quando o destrutor do *smart pointer* for chamado. De fato, a assinatura real do template `unique_ptr` e a seguinte:

```cpp
template<class T, class D = std::default_delete<T>>
class unique_ptr {
   // ...
};
```

Aqui, `default_delete<T>` e essencialmente o seguinte:

```cpp
template<class T>
struct default_delete {
   constexpr default_delete() noexcept = default;
   // ...
   constexpr void operator()(T *p) const { delete p; } // simplesmente chama delete
};
```

A presenca de um tipo default para `D` e o que geralmente nos permite escrever codigo que ignora esse parametro. O parametro `D` na assinatura `unique_ptr<T,D>` e esperado ser sem estado (*stateless*), pois nao e armazenado dentro do objeto `unique_ptr` mas instanciado conforme necessario, e depois usado como uma funcao que recebe o ponteiro e faz o que e necessario para finalizar o *pointee*.

Assim, imagine a seguinte classe com destrutor privado, uma tecnica comum se voce busca evitar instanciacao por outros meios que nao alocacao dinamica:

```cpp
#include <memory>

class requer_alocacao_dinamica {
   ~requer_alocacao_dinamica() = default; // destrutor privado!
   // ...
   friend struct limpador; // limpador tem acesso privilegiado
};

struct limpador {
   template <class T>
   void operator()(T *p) const { delete p; } // quem pode chamar delete
};

int main() {
   using namespace std;

   // requer_alocacao_dinamica r0;  // ERRO: destrutor privado
   // auto p0 = unique_ptr<requer_alocacao_dinamica>{
   //    new requer_alocacao_dinamica
   // }; // ERRO: delete nao acessivel ao deleter padrao

   // OK: usa limpador::operator() para destruir o objeto
   auto p1 = unique_ptr<requer_alocacao_dinamica, limpador>{
      new requer_alocacao_dinamica
   };
}
```

Note que ao tornar o functor `limpador` seu `friend`, a classe `requer_alocacao_dinamica` permite que `limpador` acesse especificamente seus membros protegidos e privados, incluindo acesso ao seu destrutor privado.

Imagine agora que estamos usando um objeto por meio de uma interface que esconde do codigo cliente a informacao sobre se somos o unico proprietario do recurso apontado ou se compartilhamos aquele recurso com outros. Imagine tambem que o potencial compartilhamento e feito por meios intrusivos, como e feito em muitas plataformas, de modo que a maneira de sinalizar que estamos nos desconectando daquele recurso e chamar sua funcao membro `release()`, que por sua vez levara em conta que nos desconectamos ou liberara o recurso se formos seus ultimos usuarios. Para simplificar o codigo cliente, nossa base de codigo tem uma funcao livre `liberar()` que chama a funcao membro `release()` em tal ponteiro, se ele nao for nulo.

Ainda podemos usar `unique_ptr` para isso, mas observe a sintaxe, ligeiramente diferente, pois precisaremos passar o ponteiro de funcao como argumento para o construtor, ja que esse ponteiro sera armazenado internamente. Assim, essa especializacao de `unique_ptr` com um ponteiro de funcao como *deleter* leva a um leve aumento de tamanho:

```cpp
#include <memory>

struct liberavel {
   void release() {
      // simplificado para fins deste exemplo
      delete this;
   }
protected:
   ~liberavel() = default;
};

class recurso_importante : public liberavel {
   // ...
};

// funcao livre que chama release() se o ponteiro nao for nulo
void liberar(liberavel *p) {
   if (p) p->release();
}

int main() {
   using namespace std;

   // unique_ptr com funcao personalizada de liberacao
   auto p = unique_ptr<recurso_importante,
                       void(*)(liberavel*)>{
      new recurso_importante, liberar
   }; // usara liberar() para destruir o pointee
}
```

Se o custo extra do tamanho de um ponteiro de funcao (mais alinhamento) no tamanho de `unique_ptr` for inaceitavel (por exemplo, porque voce esta em uma plataforma com restricoes de recursos ou porque tem um container com muitos objetos `unique_ptr`, o que faz os custos aumentarem significativamente mais rapido), ha um truque interessante que voce pode usar, empurrando o uso do *deleter* em tempo de execucao para o maravilhoso mundo do sistema de tipos:

```cpp
#include <memory>

// ... (mesmas definicoes de liberavel, recurso_importante, liberar)

int main() {
   using namespace std;

   // versao com funcao: armazena ponteiro para funcao -> maior
   auto p = unique_ptr<recurso_importante,
                       void(*)(liberavel*)>{
      new recurso_importante, liberar
   };
   static_assert(sizeof(p) > sizeof(void*)); // confirmado: maior

   // versao com lambda: nao tem estado -> mesmo tamanho que void*
   auto q = unique_ptr<
      recurso_importante,
      decltype([](auto ptr) { liberar(ptr); })>{
      new recurso_importante
   };
   static_assert(sizeof(q) == sizeof(void*)); // confirmado: sem custo extra
}
```

Como voce pode ver, no caso de `p`, usamos um ponteiro de funcao como *deleter*, o que exige armazenar o endereco da funcao, enquanto com `q`, substituimos o ponteiro de funcao pelo tipo de um lambda hipotetico, que quando instanciado chamara aquela funcao passando o ponteiro como argumento. E simples e pode economizar espaco quando usado com criterio!

### make_unique

Desde o C++14, `unique_ptr<T>` veio acompanhado de uma funcao de fabrica que encaminha perfeitamente seus argumentos para um construtor de `T`, aloca e constroi o `T` bem como `unique_ptr<T>` para conte-lo, e retorna o objeto resultante. Essa funcao e `std::make_unique<T>(args...)`, e uma implementacao ingênua seria a seguinte:

```cpp
template <class T, class ... Args>
std::unique_ptr<T> make_unique(Args &&... args) {
   return std::unique_ptr<T>{
      new T(std::forward<Args>(args)...)
   };
}
```

Ha tambem variantes para criar um `T[]`, claro. Voce pode se perguntar qual e a utilidade de tal funcao, e de fato essa funcao nao foi entregue junto com `unique_ptr` inicialmente (`unique_ptr` e um tipo C++11), mas considere o seguinte exemplo (artificial):

```cpp
template <class T>
class par_com_alocacao {
   T *p0, *p1; // dois objetos alocados dinamicamente
public:
   par_com_alocacao(const T &val0, const T &val1)
      : p0{ new T(val0) }, p1{ new T(val1) } {
   }
   ~par_com_alocacao() {
      delete p1;
      delete p0;
   }
   // operacoes de copia e movimento deixadas para a imaginacao
};
```

Podemos supor deste exemplo que esta classe e usada quando, por alguma razao, o codigo cliente prefere alocar os objetos `T` dinamicamente. Sabendo que subobjetos em um objeto C++ sao construidos na ordem de declaracao, sabemos que `p0` sera construido antes de `p1`:

```cpp
// p0 declarado antes de p1
T *p0, *p1;

// operacoes na ordem:
// (A) new T(val0) ocorre antes da construcao de p0
// (B) construcao de p0 precede construcao de p1
// (C) new T(val1) ocorre antes da construcao de p1
par_com_alocacao(const T &val0, const T &val1)
   : p0{ new T(val0) }, p1{ new T(val1) } {
}
```

Mas suponha que a ordem das operacoes seja `new T(val0)`, construcao de `p0`, `new T(val1)`, construcao de `p1`. O que acontece se `new T(val1)` lançar uma excecao, seja porque `new` nao consegue alocar memoria suficiente ou porque o construtor de `T` falha? Voce pode ser tentado a pensar que o destrutor de `par_com_alocacao` vai limpar, mas nao sera o caso — para um destrutor ser chamado, o construtor correspondente deve ter sido concluido primeiro; caso contrario, nao ha objeto a destruir!

Ha formas de contornar isso. Uma delas pode ser usar `unique_ptr<T>` no lugar de `T*`, o que seria otimo, dado que e exatamente o que estamos discutindo! Vamos reescrever `par_com_alocacao` dessa forma:

```cpp
#include <memory>

template <class T>
class par_com_alocacao {
   std::unique_ptr<T> p0, p1; // agora gerenciados automaticamente
public:
   par_com_alocacao(const T &val0, const T &val1)
      : p0{ new T(val0) }, p1{ new T(val1) } {
   }
   // destrutor implicitamente correto
   // copia e movimento funcionam implicitamente (ou deixe para a imaginacao)
};
```

Com esta versao, se `new T(val1)` lançar, o objeto `par_com_alocacao` ainda nao sera destruido (nao foi construido). No entanto, `p0` ja foi construido nesse ponto, e portanto sera destruido. Nosso codigo ficou mais simples e seguro!

O que isso tem a ver com `make_unique<T>()`? Bem, ha uma armadilha oculta. Vejamos mais de perto a ordem das operacoes em nosso construtor:

```cpp
// p0 declarado antes de p1
std::unique_ptr<T> p0, p1;

// Identifiquemos as operacoes assim:
// A: new T(val0)
// B: construcao de p0
// C: new T(val1)
// D: construcao de p1
//
// Sabemos que:
// - A precede B
// - C precede D
// - B precede D (ordem de declaracao dos membros)
par_com_alocacao(const T &val0, const T &val1)
   : p0{ new T(val0) }, p1{ new T(val1) } {
}
```

Se voce olhar as regras nos comentarios, vera que poderíamos ter as operacoes na seguinte ordem: A→B→C→D, mas tambem poderıamos tê-las ordenadas como A→C→B→D ou C→A→B→D, caso em que as duas chamadas a `new T(...)` ocorreriam, seguidas pelos dois construtores de `unique_ptr<T>`. Se isso acontecer, uma excecao lançada pela segunda chamada a `new` ou pelo construtor de `T` associado ainda levaria a um vazamento de recurso.

Isso e uma pena. Mas esse e exatamente o ponto de `make_unique<T>()` — com uma funcao de fabrica, o codigo cliente nunca se encontra com "resultados flutuantes de chamadas a `new`"; ou ele tem um objeto `unique_ptr<T>` completo ou nao tem nada:

```cpp
#include <memory>

template <class T>
class par_com_alocacao {
   std::unique_ptr<T> p0, p1;
public:
   par_com_alocacao(const T &val0, const T &val1)
      : p0{ std::make_unique<T>(val0) }, // fabrica atomica
        p1{ std::make_unique<T>(val1) } { // fabrica atomica
   }
   // destrutor implicitamente correto
};

#include <string>
#include <random>
#include <iostream>

class arriscado {
   std::mt19937 prng{ std::random_device{}() };
   std::uniform_int_distribution<int> moeda{ 0, 1 };
public:
   arriscado() = default;
   arriscado(const arriscado &) {
      if (moeda(prng)) throw 3; // lança 50% das vezes
   }
   ~arriscado() {
      std::cout << "~arriscado()\n";
   }
};

int main() {
   // os objetos abaixo NAO vazam mesmo se uma excecao for lançada
   if (std::string s0, s1; std::cin >> s0 >> s1)
      try {
         par_com_alocacao a{ s0, s1 };
         par_com_alocacao b{ arriscado{}, arriscado{} };
      } catch(...) {
         std::cerr << "Algo foi lancado...\n";
      }
}
```

Como voce pode ver, `make_unique<T>()` e um recurso de seguranca, util principalmente para evitar expor recursos sem dono no codigo cliente. Como bonus, `make_unique<T>()` nos permite limitar as repeticoes no codigo-fonte. Confira:

```cpp
// escreve o tipo duas vezes
unique_ptr<algum_tipo> p0 { new algum_tipo{ args } };

// tambem escreve o tipo duas vezes
auto p1 = unique_ptr<algum_tipo> { new algum_tipo{ args } };

// escreve o tipo UMA unica vez
auto p2 = make_unique<algum_tipo>(args);
```

Como voce pode ver, `p0` e `p1` exigem que voce soletere o nome do tipo apontado duas vezes, enquanto `p2` so exige que voce o escreva uma vez. Isso e sempre uma vantagem.

## Tipos shared_ptr e weak_ptr

Na maioria dos casos, `unique_ptr<T>` sera o *smart pointer* de sua escolha. E pequeno, rapido e faz o que a maioria dos codigos precisa. Ha alguns casos de uso especializados, mas importantes, em que `unique_ptr<T>` nao e o que voce precisa, e esses tem em comum o seguinte:

- A semantica transmitida e a de posse compartilhada do recurso;
- O ultimo proprietario do recurso nao e conhecido a priori (o que acontece principalmente em codigo concorrente).

Note que se a execucao nao for concorrente, voce geralmente saberá quem e o ultimo proprietario do recurso — e o ultimo objeto que observara o recurso a ser destruido no programa. Este e um ponto importante — voce pode ter codigo concorrente que compartilha recursos e ainda usa `unique_ptr` para gerenciar o recurso. Usuarios nao-proprietarios do recurso, como *raw pointers*, podem acessa-lo sem assumir a posse (mais sobre isso mais adiante neste capitulo), e essa abordagem e suficiente.

Voce pode, claro, ter codigo nao-concorrente onde o ultimo proprietario de um recurso nao e conhecido a priori. Um exemplo pode envolver um protocolo em que o provedor do recurso ainda o mantem apos retorna-lo ao cliente, mas pode ser solicitado a libera-lo em um ponto posterior enquanto o codigo cliente o retém, tornando o cliente o ultimo proprietario a partir desse ponto, ou pode nunca ser solicitado a libera-lo, caso em que o provedor pode ser o ultimo proprietario. Tais situacoes sao altamente especificas, e mostram que pode haver razoes para usar semantica de posse compartilhada expressa por `std::shared_ptr` mesmo em codigo nao-concorrente.

Como o codigo concorrente permanece o emblema das situacoes onde o ultimo proprietario de um recurso compartilhado nao e conhecido a priori, usaremos isso como base para nossa investigacao. Lembre do exemplo do inicio deste capitulo:

```cpp
void f() {
   X *p = new X;
   std::thread th0{ [p] { /* usa *p */ } };
   std::thread th1{ [p] { /* usa *p */ } };
   th0.detach();
   th1.detach();
}
```

Aqui, `p` em `f()` nao possui o `X` que aponta, sendo um *raw pointer*, e tanto `th0` quanto `th1` copiam aquele *raw pointer*, portanto nenhum e responsavel pelo *pointee* (pelo menos com base nas regras impostas pelo sistema de tipos). Este exemplo pode ser alterado para ter semantica de *ownership* clara mudando `p` de `X*` para `shared_ptr<X>`:

```cpp
void f() {
   std::shared_ptr<X> p { new X };
   // ao capturar p por copia, cada thread recebe co-posse
   std::thread th0{ [p] { /* usa *p */ } };
   std::thread th1{ [p] { /* usa *p */ } };
   th0.detach();
   th1.detach();
}
```

Em `f()`, o objeto `p` e inicialmente o unico proprietario do `X` que aponta. Quando `p` e copiado, como e feito nos blocos de captura dos lambdas executados por `th0` e `th1`, a mecanica de `shared_ptr` garante que `p` e suas duas copias compartilhem tanto `X*` quanto um contador integral, usado para determinar quantos proprietarios compartilhados existem para o recurso.

As funcoes-chave de `shared_ptr` sao seu construtor de copia (compartilha o recurso e incrementa o contador), a atribuicao de copia (desconecta do recurso original, decrementando seu contador, e depois se conecta ao novo recurso, incrementando seu contador) e o destrutor (decrementa o contador e destroi o recurso se nao restar proprietario). Cada uma dessas funcoes e sutil de implementar; para ajudar a entender o que esta em jogo, forneceremos exemplos de implementacao simplificados no Capitulo 6. A semantica de movimento, sem surpresas, implementa semantica de transferencia de *ownership* para `shared_ptr`.

Note que `shared_ptr<T>` implementa semantica de posse compartilhada extrinseca (nao-intrusiva). O tipo `T` poderia ser um tipo fundamental e nao precisa implementar uma interface especifica para que esse tipo funcione. Isso difere da semantica intrusiva compartilhada mencionada anteriormente neste capitulo.

### Utilidade e custos

Ha custos intrinsecos ao modelo `shared_ptr<T>`. O mais obvio e que `sizeof(shared_ptr<T>) > sizeof(unique_ptr<T>)` para qualquer tipo `T`, ja que `shared_ptr<T>` precisa lidar tanto com um ponteiro para o recurso compartilhado quanto com um ponteiro para o contador compartilhado.

Outro custo e que copiar um `shared_ptr<T>` nao e uma operacao barata. Lembre-se de que `shared_ptr<T>` faz sentido principalmente em codigo concorrente, onde voce nao sabe a priori quem e o ultimo proprietario de um recurso. Por essa razao, os incrementos e decrementos do contador compartilhado exigem sincronizacao, o que significa que o contador e tipicamente um inteiro atomico, e mutar um objeto `atomic<int>` (por exemplo) custa mais do que mutar um `int`.

Outro custo nao-negligenciavel e o seguinte:

```cpp
shared_ptr<X> p{ new X }; // realiza DUAS alocacoes separadas
```

Uma instrucao como essa levara a duas alocacoes, nao uma — uma para o objeto `X` e outra (realizada internamente pelo `shared_ptr`) para o contador. Como essas duas alocacoes serao feitas separadamente, uma pelo codigo cliente e outra pelo proprio construtor, os dois objetos alocados podem se encontrar em *cache lines* distintas, potencialmente levando a uma perda de eficiencia ao acessar o objeto `shared_ptr`.

### make_shared()

Ha uma forma de aliviar este ultimo custo: fazer com que a mesma entidade realize ambas as alocacoes, em vez de deixar o codigo cliente fazer uma e o construtor fazer a outra. A ferramenta padrao para isso e a funcao de fabrica `std::make_shared<T>()`.

Compare as seguintes duas instrucoes:

```cpp
shared_ptr<X> p{ new X(args) };     // 2 alocacoes separadas
auto q = make_shared<X>(args);      // 1 alocacao unica (mais eficiente)
```

Ao construir `p`, `shared_ptr<X>` recebe um `X*` ja existente para gerenciar, portanto nao tem escolha a nao ser realizar uma segunda alocacao separada para o contador compartilhado. Por outro lado, a chamada expressa como `make_shared<X>(args)` especifica o tipo `X` a construir junto com os argumentos `args` a encaminhar diretamente para o construtor. Cabe a essa funcao criar `shared_ptr<X>`, `X` e o contador compartilhado, o que nos permite colocar tanto `X` quanto o contador no mesmo espaco contiguo (o *control block*), usando mecanismos como uma `union` ou o mecanismo de *placement new*, que sera explorado no Capitulo 7.

Claramente, dados os mesmos argumentos usados para construcao, os objetos `p` e `q` anteriores serao `shared_ptr<X>` equivalentes, mas em geral `q` tera melhor desempenho que `p`, pois seus dois componentes-chave estarao organizados de forma mais amigavel ao cache.

### E o weak_ptr?

Se `shared_ptr<T>` e um tipo com um nicho mais estreito (ainda que essencial) do que `unique_ptr<T>`, o `weak_ptr<T>` ocupa um nicho ainda mais estreito (mas ainda essencial). O papel de `weak_ptr<T>` e modelar a posse temporaria de `T`. O tipo `weak_ptr<T>` foi concebido para interagir com `shared_ptr<T>` de uma forma que permita testar a continuidade de existencia do *pointee* a partir do codigo cliente.

Um bom exemplo de uso de `weak_ptr`, inspirado no excelente site cppreference, e o seguinte:

```cpp
// inspirado em exemplo do cppreference
#include <iostream>
#include <memory>
#include <format>

void observar(std::weak_ptr<int> ref_fraca) {
   // lock() tenta obter um shared_ptr valido
   if (std::shared_ptr<int> ptr_forte = ref_fraca.lock())
      std::cout << std::format("valor = {}\n", *ptr_forte);
   else
      std::cout << "referencia expirou\n";
}

int main() {
   std::weak_ptr<int> ref_fraca;

   {
      auto ptr = std::make_shared<int>(3);
      ref_fraca = ptr; // weak_ptr criado a partir de shared_ptr

      // ref_fraca aponta para um shared_ptr<int> vivo aqui
      observar(ref_fraca); // exibe: valor = 3
   }
   // ptr foi destruido, ref_fraca agora esta expirada
   observar(ref_fraca); // exibe: referencia expirou
}
```

Como este exemplo mostra, voce pode fazer `weak_ptr<T>` a partir de `shared_ptr<T>`, mas `weak_ptr` nao possui o recurso ate voce chamar `lock()` nele, obtendo `shared_ptr<T>`, a partir do qual voce pode usar o recurso com seguranca apos ter verificado que ele nao modela um ponteiro vazio.

Outro caso de uso para `std::weak_ptr` e `std::shared_ptr` seria um cache de recursos tal que:

- Os dados em um objeto `Recurso` sao suficientemente grandes ou custosos para duplicar que e preferivel compartilha-los a copiá-los;
- Um objeto `Cache` compartilha os objetos que armazena, mas precisa invalida-los antes de substitui-los quando sua capacidade e atingida.

Nessa situacao, um objeto `Cache` poderia manter objetos `std::shared_ptr<Recurso>`, mas fornece ao codigo cliente `std::weak_ptr<Recurso>` sob demanda, de modo que os objetos `Recurso` possam ser descartados quando o `Cache` precisar, mas o codigo cliente precisa verificar se os objetos apontados ainda nao foram invalidados.

Um exemplo completo (simplificado) seria o seguinte (veja o repositorio GitHub do livro para o exemplo completo):

```cpp
template <auto Capacidade>
class Cache {
   using relogio = std::chrono::system_clock;

   // cache com capacidade Capacidade, mantem os Recursos mais recentemente usados
   std::vector<std::pair<
      decltype(relogio::now()),
      std::shared_ptr<Recurso>
   >> recursos;

   bool cheio() const { return recursos.size() == Capacidade; }

   // precondition: !recursos.empty()
   void remover_mais_antigo() {
      auto it = std::min_element(
         std::begin(recursos), std::end(recursos),
         [](auto &&a, auto &&b) { return a.first < b.first; }
      );
      assert(it != std::end(recursos));
      it->second.reset(); // renuncia a posse
      recursos.erase(it);
   }

public:
   void adicionar(Recurso *p) {
      const auto t = relogio::now();
      if (cheio()) remover_mais_antigo();
      recursos.emplace_back(t, std::shared_ptr<Recurso>{ p });
   }

   // retorna weak_ptr (nao-proprietario, verificavel)
   std::weak_ptr<Recurso> obter(Recurso::tipo_id id) {
      const auto t = relogio::now();
      auto it = std::find_if(
         std::begin(recursos), std::end(recursos),
         [id](auto &&par) { return par.second->id() == id; }
      );
      if (it == std::end(recursos))
         return {}; // nao encontrado
      it->first = t; // atualiza tempo de ultimo uso
      return it->second; // converte shared_ptr em weak_ptr
   }
};

int main() {
   Cache<5> cache;
   for (int i = 0; i != 5; ++i)
      cache.adicionar(new Recurso{ i + 1 });

   // obtemos uma referencia fraca para o recurso 3
   auto ref = cache.obter(3);

   if (auto ptr = ref.lock(); ptr)
      std::cout << "Usando recurso " << ptr->id() << '\n';

   // muitas coisas acontecem: recursos adicionados, usados, etc.
   for (int i = 6; i != 15; ++i)
      cache.adicionar(new Recurso{ i + 1 });

   if (auto ptr = ref.lock(); ptr)
      std::cout << "Usando recurso " << ptr->id() << '\n';
   else
      std::cout << "Recurso nao esta mais disponivel...\n";
}
```

Apos um numero suficiente de adicoes ao cache, o objeto apontado por `ref` em `main()` se torna invalidado e apagado do conjunto de recursos. Ainda assim, `main()` pode testar a validade do objeto apontado por `ref` por meio da construcao de `std::shared_ptr` a partir do `std::weak_ptr` que ele mantem.

Na pratica, `weak_ptr` e as vezes usado para quebrar ciclos quando objetos `shared_ptr<T>` se referem uns aos outros de alguma forma. Se voce tiver dois tipos cujos objetos se referem mutuamente (digamos, `X` e `Y`) e nao sabe qual sera destruido primeiro, considere fazer um deles o proprietario (`shared_ptr`) e o outro o nao-proprietario de forma verificavel (`weak_ptr`), o que garantira que eles nao se mantenham vivos para sempre. Por exemplo, isto sera concluido, mas os destrutores de `X` e `Y` nunca serao chamados:

```cpp
#include <memory>
#include <iostream>

struct Y;

struct X {
   std::shared_ptr<Y> ptr_y; // X e co-dono de Y: ciclo!
   ~X() { std::cout << "~X()\n"; }
};

struct Y {
   std::shared_ptr<X> ptr_x; // Y e co-dono de X: ciclo!
   ~Y() { std::cout << "~Y()\n"; }
};

void ciclo_eterno() {
   auto x = std::make_shared<X>();
   auto y = std::make_shared<Y>();
   x->ptr_y = y; // x aponta para y
   y->ptr_x = x; // y aponta para x -> ciclo de posse!
}

int main() {
   ciclo_eterno();
   std::cout << "Done\n";
   // X e Y NUNCA sao destruidos: vazamento de memoria!
}
```

Se voce mudar `X::ptr_y` ou `Y::ptr_x` para `weak_ptr`, vera os destrutores de `X` e `Y` sendo chamados:

```cpp
#include <memory>
#include <iostream>

struct Y;

struct X {
   std::weak_ptr<Y> ptr_y; // X apenas OBSERVA Y (sem co-posse)
   ~X() { std::cout << "~X()\n"; }
};

struct Y {
   std::shared_ptr<X> ptr_x; // Y e co-dono de X
   ~Y() { std::cout << "~Y()\n"; }
};

void sem_ciclo() {
   auto x = std::make_shared<X>();
   auto y = std::make_shared<Y>();
   x->ptr_y = y; // observacao fraca, sem co-posse
   y->ptr_x = x; // posse normal
}

int main() {
   sem_ciclo();
   std::cout << "Done\n";
   // X e Y sao destruidos corretamente
}
```

Claro, a maneira mais facil de nao chegar ao ponto de enfrentar um ciclo de objetos `shared_ptr<T>` e nao construir tal ciclo, mas ao lidar com bibliotecas externas e ferramentas de terceiros, isso as vezes e mais facil dizer do que fazer.

## Quando usar raw pointers

Vimos que tipos de *smart pointer* como `unique_ptr<T>` e `shared_ptr<T>` se destacam quando ha necessidade de descrever a *ownership* de um recurso do tipo `T` por meio do sistema de tipos. Isso significa que `T*` se tornou inutil?

Nao, claro que nao. O truque e usa-lo em situacoes controladas. A primeira e que para uma funcao, receber um `T*` como argumento deve significar que a funcao e uma observadora, nao uma proprietaria, daquele `T`. Se sua base de codigo usar *raw pointers* nesse sentido, voce provavelmente nao encontrara problemas.

Em segundo lugar, voce pode usar um *raw pointer* dentro de uma classe que implementa sua semantica de *ownership* preferida. E correto implementar um container que manipula objetos por meio de *raw pointers* (por exemplo, uma estrutura em arvore destinada a varias ordens de travessia), desde que aquele container implemente semantica clara de copia e movimento. O que voce nao quer fazer e expor ponteiros para os nos internos do seu container ao codigo externo. Preste atencao a interface do container.

De fato, considere esta lista simplesmente encadeada (trecho):

```cpp
template <class T>
class lista_encadeada_simples {
   struct no {
      T valor;
      no *proximo = nullptr; // raw pointer interno ao container
      no(const T &val) : valor{ val } {}
   };

   no *cabeca = nullptr; // raw pointer interno ao container
   // ...

public:
   // ...
   ~lista_encadeada_simples() {
      for (auto p = cabeca; p;) {
         auto prox = p->proximo;
         delete p; // destruicao manual dos nos
         p = prox;
      }
   }
};
```

Exploraremos esse exemplo com mais detalhes no Capitulo 13. O destrutor funciona bem e (supondo que o restante da classe seja razoavelmente bem escrito) a classe e utilizavel e util. Agora, suponha que decidamos usar `unique_ptr<no>` no lugar de `no*` como membro `cabeca` da `lista_encadeada_simples`, e como substituto para o membro `proximo` do `no`. Isso parece uma boa ideia, exceto quando voce considera as consequencias:

```cpp
template <class T>
class lista_encadeada_simples {
   struct no {
      T valor;
      std::unique_ptr<no> proximo; // boa ideia?
      no(const T &val) : valor{ val } {}
   };

   std::unique_ptr<no> cabeca; // boa ideia?
   // ...

public:
   // ...
   ~lista_encadeada_simples() = default; // parece simples...
};
```

Isso parece uma boa ideia na superficie, mas nao transmite a semantica correta — nao e verdade que um `no` possui e e responsavel pelo `no` seguinte. Nao queremos que a remocao de um `no` destrua o `no` que vem apos (e assim por diante, recursivamente), e se isso parece uma simplificacao no destrutor de `lista_encadeada_simples`, pense nas consequencias: essa estrategia leva a tantos destrutores chamados recursivamente quantos nos existirem na lista, o que e uma excelente maneira de causar um *stack overflow*!

Use um *smart pointer* quando o caso de uso corresponder a semantica que ele modela. Claro, quando a relacao modelada por seus ponteiros nao e nem posse exclusiva nem posse compartilhada, voce provavelmente nao quer tipos de *smart pointer* que fornecem essas semanticas, recorrendo em vez disso a *smart pointers* nao padrao e nao-proprietarios ou, simplesmente, a *raw pointers*.

Por fim, voce muitas vezes precisara de *raw pointers* para usar interfaces de mais baixo nivel — por exemplo, ao realizar chamadas de sistema. Isso nao desqualifica abstracoes de nivel mais alto, como `vector<T>` ou `unique_ptr<T>`, ao escrever codigo de nivel de sistema — voce pode acessar o array subjacente de `vector<T>` por meio de sua funcao membro `data()`, assim como pode acessar o *raw pointer* subjacente de `unique_ptr<T>` por meio de sua funcao membro `get()`. Enquanto fizer sentido, veja o codigo chamado como "emprestando" o ponteiro do codigo chamador durante a duracao da chamada.

E se voce nao tiver outra opcao, use *raw pointers*. Eles existem, afinal, e funcionam. Simplesmente lembre-se de usar abstracoes de nivel mais alto onde for possivel — isso tornara seu codigo mais simples, mais seguro e (mais frequentemente do que voce pensaria) mais rapido. Se voce nao consegue definir a semantica de nivel mais alto, talvez ainda seja cedo demais para escrever aquela parte do codigo, e voce obtera resultados melhores se dedicar mais tempo a pensar sobre essas semanticas.

## Resumo

Neste capitulo, vimos como usar os *smart pointers* padrao. Discutimos a semantica de *ownership* que eles implementam (posse exclusiva, co-posse compartilhada e co-posse temporaria), vimos exemplos de como podem ser usados e discutimos algumas maneiras de usa-los reconhecendo que outras opcoes mais adequadas existem.

No proximo capitulo, daremos um passo adiante e escreveremos nossas proprias versoes (utilizaveis, embora ingênuas) de `unique_ptr<T>` e `shared_ptr<T>`, a fim de obter uma compreensao intuitiva do que isso implica, e tambem escreveremos alguns *smart pointers* nao padrao, porem uteis. Isso nos ajudara a construir um conjunto de ferramentas de gerenciamento de recursos mais completo e interessante.
