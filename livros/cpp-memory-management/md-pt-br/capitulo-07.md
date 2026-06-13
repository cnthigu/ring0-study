# Capitulo 7 - Sobrecarregando Operadores de Alocacao de Memoria (Overloading Memory Allocation Operators)

> Traducao nao oficial do livro *C++ Memory Management* (Patrice Roy, 2025).
> Texto adaptado para portugues brasileiro com fins educacionais. Paginas originais: 141-162.
> Repositorio: [ring0-study](https://github.com/cnthigu/ring0-study)

> Navegacao: [Anterior](capitulo-06.md) | [Indice](README.md) | [Proximo](capitulo-08.md)

---

Curtindo a jornada ate agora? Espero que sim! Chegamos ao ponto em que detemos todas as chaves e podemos comecar a fazer o que este livro anuncia: examinar com mais detalhe como o gerenciamento de memoria funciona em C++. Nao e um topico simples, nem e algo trivial, entao precisavamos ter certeza de que estariamos prontos... mas agora estamos, entao vamos la!

Os Capitulos 5 e 6 examinaram as ferramentas padrao que se pode usar para inscrever a responsabilidade sobre recursos alocados dinamicamente no sistema de tipos C++ por meio de *smart pointers*, tanto os fornecidos pelo padrao quanto os que poderíamos escrever para preencher outros nichos. Usar *smart pointers* no lugar de *raw pointers* como membros de dados e tipos de retorno de funcoes tende a simplificar (e esclarecer) uma proporcao significativa das tarefas de gerenciamento de memoria em programas C++.

As vezes, queremos trabalhar em um nivel mais baixo do que esse e tomar controle sobre o que acontece quando alguem escreve `new X`. As razoes para querer esse controle sao numerosas, e exploraremos algumas neste livro, mas neste capitulo nos concentraremos nos conceitos basicos das funcoes de gerenciamento de memoria e como assumir o controle desses mecanismos em C++.

Apos esses conceitos basicos serem cobertos, faremos o seguinte:

- Ver como nosso conhecimento dos mecanismos de alocacao de memoria do C++ nos permite escrever um detector de vazamentos simples (porem funcional) no Capitulo 8;
- Examinar como se pode gerenciar memoria atipica (persistente, compartilhada, etc.) em C++ no Capitulo 9;
- Escrever alocacao de memoria baseada em arena no Capitulo 10 para garantir tempo de alocacao e desalocacao deterministico, levando a implementacoes extremamente rapidas de `new` e `delete` quando o contexto permitir.

Os capitulos posteriores usarao o conhecimento adquirido neste capitulo e nos que se seguem para escrever containers eficientes e mecanismos de recuperacao diferida que se assemelham a um coletor de lixo. Depois disso, examinaremos como os containers podem usar essas facilidades, com e sem alocadores.

## Por que sobrecarregar funcoes de alocacao?

Antes de comecarmos a discutir como sobrecarregar mecanismos de alocacao de memoria, vamos dar um passo atras e examinar por que alguem quereria fazer isso. De fato, a maioria dos programadores (mesmo os experientes) nunca acaba fazendo nada do tipo, e poderíamos apostar que a maioria dos programadores nunca pensou ter uma razao para isso. No entanto, alocaremos (!) alguns capitulos a esse topico. Tem que haver uma razao...

A questao com a alocacao de memoria e que nao existe uma solucao perfeita para o problema em geral; ha muitas boas solucoes em media, e ha solucoes muito boas para versoes mais especializadas do problema. O que constitui uma boa solucao para um dado caso de uso na linguagem de programacao A pode ser inadequado para outro caso de uso ou na linguagem de programacao B.

Tome, por exemplo, linguagens onde e idiomatico alocar dinamicamente grandes quantidades de pequenos objetos, algo habitual em Java ou C#. Nessas linguagens, pode-se esperar que as estrategias de alocacao sejam otimizadas para esse padrao de uso. Em uma linguagem como C, onde se tenderia a alocar ao enfrentar objetos grandes demais para colocar na pilha ou ao usar estruturas de dados baseadas em nos, por exemplo, a melhor estrategia de alocacao dinamica poderia ser bastante diferente. No Capitulo 10, veremos um exemplo em que o processo de alocacao se beneficia do fato de que os objetos alocados sao todos do mesmo tamanho e alinhamento, outro caso de uso interessante.

O C++ enfatiza o controle e fornece ferramentas sofisticadas e versateis aos programadores. Ao enfrentar uma situacao em que conhecemos o contexto em que as alocacoes serao realizadas, podemos as vezes usar essas ferramentas para fazer melhor (ate muito melhor, como veremos no Capitulo 11!) do que uma implementacao padrao faria, e para numerosas metricas: melhor tempo de execucao, tempo de execucao mais deterministico, reducao da fragmentacao de memoria, e assim por diante.

## Breve visao geral das funcoes de alocacao da linguagem C

Antes de chegar aos mecanismos de alocacao de memoria do C++, vamos primeiro dar uma rapida olhada na familia de funcoes de alocacao de memoria do C por meio de seus representantes mais destacados: `malloc()` e `free()`. Ha, claro, muitas outras funcoes relacionadas a alocacao de memoria, como `calloc()`, `realloc()` e `aligned_alloc()`, sem contar os servicos especificos do sistema operacional que realizam tarefas semelhantes para casos de uso especializados, mas essas serao suficientes para nossa discussao.

Como este e um livro sobre gerenciamento de memoria com C++, usaremos a versao C++ dessas funcoes (de `<cstdlib>` em vez de `<stdlib.h>`), o que realmente nao muda nada no codigo que escreveremos, exceto pelo fato de que em C++, essas funcoes estao localizadas no namespace `std`.

As assinaturas dessas duas funcoes sao as seguintes:

```cpp
void* malloc(size_t n);  // aloca n bytes; retorna nullptr em falha
void  free(void *p);     // libera o bloco apontado por p
```

O papel de `malloc(n)` e encontrar um local onde haja pelo menos `n` bytes consecutivos disponiveis, potencialmente marcando aquele local como "ocupado" e retornando um ponteiro abstrato (um `void*`) para o inicio daquele bloco de memoria. Note que o ponteiro retornado deve ser alinhado para o pior caso natural possivel em uma determinada maquina, o que significa que deve atender aos requisitos de alinhamento de `std::max_align_t`. Na maioria das maquinas, esse tipo e um alias para `double`.

E legal chamar `malloc()` com `n==0`, mas os resultados de tal chamada sao definidos pela implementacao: uma chamada a `malloc(0)` poderia retornar `nullptr`, mas tambem poderia retornar um ponteiro nao-nulo. Note que o ponteiro retornado por `malloc(0)` nao deve ser desreferenciado, independentemente de ser nulo ou nao.

Se `malloc()` falhar ao alocar memoria, retornara `nullptr`, ja que a linguagem C nao suporta excecoes no sentido C++. No C contemporaneo (desde o C11), uma implementacao de `malloc()` deve ser *thread-safe* e deve sincronizar adequadamente com outras funcoes de alocacao C se elas forem chamadas concorrentemente, incluindo com `free()`.

O papel de `free(p)` e garantir que a memoria apontada por `p` fique disponivel para futuras requisicoes de alocacao, desde que `p` aponte para um bloco que foi alocado por meio de uma funcao de alocacao de memoria como `malloc()` e ainda nao foi liberado. E *undefined behavior* (UB) tentar `free()` um endereco que nao foi alocado por tal funcao de alocacao — nao faca isso! Alem disso, saiba que uma vez que a memoria tenha sido liberada, ela nao e mais considerada alocada, entao codigo como o seguinte leva a UB:

```cpp
#include <cstdlib>

int main() {
   using std::malloc, std::free;
   int *p = static_cast<int*>(malloc(sizeof(int)));
   free(p); // ok: veio de malloc()
   free(p); // NAOOOOO! UB: ja foi liberado (a menos que p seja nulo por sorte)
}
```

Como mencionado no exemplo anterior, `free(nullptr)` nao faz nada, e foi definido como nao fazendo nada por decadas. Se houver codigo em sua base de codigo que verifica `p!=nullptr` antes de chamar `free()` — por exemplo, `if(p) free(p)` — voce pode remover com seguranca esse teste.

As vezes (nao sempre) usaremos essas funcoes C para implementar nossas funcoes de alocacao C++ caseiras. Elas funcionam, sao bem compreendidas e sao abstracoes de baixo nivel que podemos usar a nosso favor ao construir abstracoes de nivel mais alto.

## Visao geral dos operadores de alocacao do C++

Em C++, ha muitos (infinitos!) sabores de operadores de alocacao de memoria, mas ha regras a seguir ao escrever os seus proprios. O capitulo atual trata principalmente dessas regras; os capitulos que se seguem explorarao formas de se beneficiar dessa liberdade que o C++ nos da:

- O C++ nos permite sobrecarregar as versoes globais dos operadores de alocacao de memoria. Se fizermos isso, ate coisas como `new int` usarao nossas versoes caseiras. E preciso ter cuidado aqui, pois pequenos erros podem ter um impacto significativo na execucao do codigo: se sua implementacao de `operator new()` e lenta, voce tornara lentas a maioria das alocacoes de memoria em seu programa! Usaremos essa abordagem ao escrever um detector de vazamentos simples-porem-funcional no Capitulo 8.
- O C++ nos permite sobrecarregar versoes de funcao membro dos operadores de alocacao de memoria. Se fizermos isso, as versoes globais (sobrecarregadas ou nao) se aplicam em geral, mas as versoes de funcao membro se aplicam para tipos especificos. Isso pode ser util quando temos conhecimento especifico sobre o padrao de uso de alguns tipos, mas nao de outros. Usaremos isso a nosso favor no Capitulo 10.
- O C++ nos permite sobrecarregar versoes com argumentos adicionais dos operadores de alocacao de memoria. No capitulo atual, examinaremos algumas versoes padrao desses operadores, como a versao `nothrow` e as versoes extremamente importantes relacionadas a *placement new*. Tambem podemos usar esse recurso para se beneficiar de memoria "exotica", como memoria compartilhada ou memoria persistente, como veremos no Capitulo 9.

Em cada caso, as funcoes de alocacao de memoria vem em grupos de quatro: `operator new()`, `operator new[]()`, `operator delete()` e `operator delete[]()`. Ha algumas excecoes a essa regra, como veremos, mas a regra geralmente vale. Se sobrecarregarmos pelo menos uma dessas funcoes, e importante que sobrecarreguemos todas as quatro para manter o comportamento do nosso programa consistente.

> **Uma palavra sobre a HALO (Heap Allocation Loop Optimization)**
>
> E importante saber que ha beneficios em *nao* sobrecarregar os operadores de alocacao de memoria. Um deles e que o fornecedor da sua biblioteca fornece versoes muito boas por padrao; outro e que se voce nao sobrecarregar os operadores de alocacao de memoria, o compilador pode assumir que o numero de alocacoes que voce faz nao e observavel. Isso significa que ele tem permissao para substituir `n` chamadas a `new` por uma unica chamada que aloca tudo de uma vez e depois gerencia os resultados como se voce tivesse feito muitas alocacoes. Isso pode levar a otimizacoes espetaculares na pratica, incluindo a remocao de chamadas a `new` e `delete` completamente do codigo gerado, mesmo quando aparecem no codigo-fonte! Em caso de duvida, certifique-se de que suas otimizacoes fornecem beneficios mensuraveis antes de confirma-las e usa-las em codigo de producao.

Note que para as sobrecargas de operadores de alocacao que veremos neste capitulo, voce vai querer incluir o cabecalho `<new>`, pois e la que `std::bad_alloc` e declarado, entre outras coisas, e esse e o tipo que as funcoes de alocacao tipicamente usam para reportar falha na alocacao.

### Operadores de alocacao globais

Suponha que queremos assumir o controle das versoes globais dos operadores de alocacao em C++. Para fins de expor como isso pode funcionar, simplesmente os usaremos para delegar a `malloc()` e `free()` por ora, e mostraremos um exemplo mais elaborado no Capitulo 8.

Se nos atermos as formas basicas desses operadores, vamos querer sobrecarregar quatro funcoes antes do C++11 ou seis desde entao. Como este livro pressupoe que estamos alem de uma decada apos o C++14, procederemos de acordo.

As assinaturas que queremos sobrecarregar sao as seguintes:

```cpp
void *operator new(std::size_t);        // aloca escalar
void *operator new[](std::size_t);      // aloca array

void operator delete(void *) noexcept;  // libera escalar
void operator delete[](void *) noexcept; // libera array

// desde C++14: versoes com o tamanho do bloco
void operator delete(void *, std::size_t) noexcept;
void operator delete[](void *, std::size_t) noexcept;
```

Isso e muito, concordo, mas assumir o controle das facilidades de gerenciamento de memoria e um trabalho especializado. Assim que voce escreve uma dessas funcoes, voce oficialmente substitui as fornecidas por sua biblioteca padrao para aquele programa, e aquela funcao se torna responsavel pelas requisicoes de alocacao (ou desalocacao) que chegam por aquele canal.

A razao pela qual e importante sobrecarregar o conjunto completo de funcoes se voce sobrecarregar pelo menos uma delas e que essas funcoes formam um todo coerente. Por exemplo, se voce mudar o comportamento de `new`, mas negligenciar mudar o comportamento de `delete`, e essencialmente impossivel prever quanto dano o programa incorrara.

#### Sobre os operadores new e new[]

As funcoes `operator new()` e `operator new[]()` recebem um unico objeto `std::size_t` como argumento e ambas retornam `void*`. O argumento e, em ambos os casos, o numero minimo de bytes contiguos a alocar. Como tal, suas assinaturas se assemelham as de `std::malloc()`. Isso frequentemente surpreende as pessoas: como a expressao `new X` pode criar um objeto `X` se `new` nao e um template e nao sabe o que criar?

Essa e a questao: **`new` nao cria objetos.** O que `new` faz e encontrar o local onde um objeto sera construido. E o construtor que transforma a memoria bruta encontrada por `new` em um objeto. Na pratica, voce poderia escrever algo como o seguinte:

```cpp
X *p = new X{ /* ... args ... */ };
```

O que voce escreveu e uma operacao de dois passos:

```cpp
// Passo 1: aloca espaco suficiente para um objeto X
void *bloco = operator new(sizeof(X));

// Passo 2: constroi um objeto X naquele local
X *p = /* aplica X::X(/* ... args ... */) em bloco */;
```

Isso significa que o construtor e como uma camada de tinta aplicada a um bloco de memoria, transformando aquela memoria em um objeto. Isso tambem significa que uma expressao como `new X` pode falhar no `operator new()`, se a requisicao de alocacao nao puder ser satisfeita, ou em `X::X()`, porque o construtor falhou de alguma forma. Somente se ambos os passos forem bem-sucedidos e que o codigo cliente se torna responsavel pelo objeto apontado.

> **Uma nota sobre como chamar esses operadores**
>
> Voce pode ter notado que as vezes escrevemos `new X` e as vezes escrevemos `operator new(sizeof(X))`. A primeira forma — a forma de operador — fara o processo de dois passos de alocacao seguido de construcao, enquanto a segunda forma — a forma de funcao — chama diretamente a funcao de alocacao sem invocar um construtor. Essa distincao tambem se aplica a `operator delete()`.

A situacao e similar com `operator new[]`: o numero de bytes passado como argumento para a funcao e o numero total de bytes para o array, portanto a propria funcao de alocacao nao sabe nem o tipo do objeto que sera criado, nem o numero de elementos, nem o tamanho individual dos objetos. Uma chamada a `new X[N]` na pratica chamara `operator new[](N*sizeof(X))` para encontrar um lugar para colocar o array que sera construido, e depois chamara `X::X()` em cada um dos `N` blocos de tamanho `sizeof(X)` naquele array.

Falha ao alocar um escalar por meio de `operator new` deve resultar em algo que corresponda a `std::bad_alloc` sendo lancado. Com `operator new[]()`, tambem se pode lançar `std::bad_array_new_length` (derivado de `std::bad_alloc`) se o tamanho solicitado for problematico.

#### Sobre os operadores delete e delete[]

Como a funcao `free()` da linguagem C, os operadores `delete()` e `delete[]()` ambos recebem um `void*` como argumento. Isso significa que eles nao podem destruir seu objeto... Quando sao chamados, o objeto ja foi destruido! De fato, voce poderia escrever o seguinte:

```cpp
delete p; // suponha que p e do tipo X*
```

Isso e, na pratica, uma operacao de dois passos equivalente ao seguinte:

```cpp
p->~X();          // Passo 1: destroi o objeto apontado
operator delete(p); // Passo 2: libera a memoria associada
```

Em C++, nem os destrutores nem `operator delete()` devem lançar excecoes. Se o fizerem, o programa e praticamente encerrado.

As versoes cientes do tamanho de `operator delete()` e `operator delete[]()` foram introduzidas com o C++14 e e costume implementa-las hoje, alem das versoes classicas. A ideia e que `operator new()` foi informado do tamanho dos blocos a alocar, mas `operator delete()` nao era, o que exigia acrobacias desnecessarias por parte de implementacoes que buscavam realizar tarefas relacionadas ao tamanho, como preencher o bloco de memoria com algum valor para tentar obscurecer o que estava armazenado naquele local.

Uma implementacao completa, porem ingênua dessas funcoes que essencialmente delega o trabalho as funcoes de alocacao C poderia ser a seguinte:

```cpp
#include <iostream>
#include <cstdlib>
#include <new>

// operador new: aloca n bytes via malloc
void *operator new(std::size_t n) {
    std::cout << "operator new(" << n << " bytes)\n";
    auto p = std::malloc(n);
    if (!p) throw std::bad_alloc{};
    return p;
}

// operador delete: libera via free
void operator delete(void *p) noexcept {
    std::cout << "operator delete(bloco)\n";
    std::free(p);
}

// operador delete ciente do tamanho (C++14)
void operator delete(void *p, std::size_t n) noexcept {
    std::cout << "operator delete(bloco, " << n << " bytes)\n";
    ::operator delete(p); // delega para a versao sem tamanho
}

// versoes para arrays
void *operator new[](std::size_t n) {
    std::cout << "operator new[](" << n << " bytes)\n";
    auto p = std::malloc(n);
    if (!p) throw std::bad_alloc{};
    return p;
}

void operator delete[](void *p) noexcept {
    std::cout << "operator delete[](bloco)\n";
    std::free(p);
}

void operator delete[](void *p, std::size_t n) noexcept {
    std::cout << "operator delete[](bloco, " << n << " bytes)\n";
    ::operator delete[](p);
}

int main() {
   auto p = new int{ 3 };   // chama operator new
   delete p;                 // chama operator delete
   p = new int[10];          // chama operator new[]
   delete[] p;               // chama operator delete[]
}
```

### Versoes nao-lancantes dos operadores de alocacao

Ha tambem versoes dos operadores de alocacao que nao lançam excecao em caso de falha na alocacao. As assinaturas para essas funcoes sao as seguintes:

```cpp
// versoes nothrow: retornam nullptr em falha (nao lancam)
void *operator new(std::size_t, const std::nothrow_t&);
void *operator new[](std::size_t, const std::nothrow_t&);
void operator delete(void *, const std::nothrow_t&) noexcept;
void operator delete[](void *, const std::nothrow_t&) noexcept;

// desde C++14: versoes nothrow cientes do tamanho
void operator delete(void *, std::size_t, const std::nothrow_t&) noexcept;
void operator delete[](void *, std::size_t, const std::nothrow_t&) noexcept;
```

O tipo `std::nothrow_t` e o que se chama de *tag type*: uma classe vazia cujas instancias (aqui, o objeto global `std::nothrow`) podem ser usadas para guiar o compilador ao gerar codigo. Note que essas assinaturas de funcao exigem que os argumentos `std::nothrow_t` sejam passados por referencia constante, nao por valor.

Um exemplo de uso dessas funcoes seria o seguinte:

```cpp
X *p = new (std::nothrow) X{ /* ... args ... */ };
if (p) {
   // ... usa *p
   // nota: este e o delete normal, nao a versao nothrow
   delete p; // ok mesmo se p fosse nulo
}
```

A razao pela qual e necessario escrever sobrecargas de `operator delete()` mesmo para as versoes *nothrow* e a seguranca contra excecoes. Lembre-se de que a alocacao de memoria por meio de `operator new()` e uma operacao de dois passos: encontrar o local para colocar o objeto, depois construir o objeto naquele local. Assim, mesmo que `operator new()` nao lançe, nao sabemos se o construtor que sera chamado lançara. Nosso codigo obtera o ponteiro apenas apos a alocacao E a construcao que se segue terem sido concluidas com sucesso; como tal, o codigo cliente nao pode gerenciar excecoes que ocorrem apos a alocacao ter sido bem-sucedida mas durante a construcao do objeto...

Por essa razao, cabe ao *runtime* do C++ realizar a desalocacao se uma excecao for lancada pelo construtor, e isso e valido para todas as versoes de `operator new()`, nao apenas as *nothrow*. O algoritmo (informalmente) e o seguinte:

```cpp
// Passo 1: tenta realizar a alocacao para algum objeto T
p = operator new(n /*, ... possiveis argumentos adicionais ... */)

// a linha abaixo e apenas para o new nothrow
if (!p) return p;

try {
   // Passo 2: constroi o objeto no endereco p
   aplica o construtor de T no endereco p; // pode lancar
} catch (...) { // construcao lancou excecao
   libera p;    // desfaz a alocacao automaticamente
   relanca a excecao;
}
return p; // p aponta para um objeto completamente construido
          // so apos este ponto o codigo cliente ve p
```

Aqui esta uma implementacao completa e ingênua onde as versoes lancantes delegam para as versoes nao-lancantes para reduzir repeticao:

```cpp
#include <iostream>
#include <cstdlib>
#include <new>

// versao nothrow: retorna nullptr em falha
void* operator new(std::size_t n, const std::nothrow_t&) noexcept {
   return std::malloc(n);
}

// versao lancante: usa a nothrow e lanca se necessario
void* operator new(std::size_t n) {
   auto p = operator new(n, std::nothrow);
   if (!p) throw std::bad_alloc{};
   return p;
}

// delete nothrow (necessario para exception-safety na construcao)
void operator delete(void* p, const std::nothrow_t&) noexcept {
   std::free(p);
}

void operator delete(void* p) noexcept {
   operator delete(p, std::nothrow);
}

void operator delete(void* p, std::size_t) noexcept {
   operator delete(p, std::nothrow);
}

// versoes para arrays (identicas em estrutura)
void* operator new[](std::size_t n, const std::nothrow_t&) noexcept {
   return std::malloc(n);
}

void* operator new[](std::size_t n) {
   auto p = operator new[](n, std::nothrow);
   if (!p) throw std::bad_alloc{};
   return p;
}

void operator delete[](void* p, const std::nothrow_t&) noexcept {
   std::free(p);
}

void operator delete[](void* p) noexcept {
   operator delete[](p, std::nothrow);
}

void operator delete[](void* p, std::size_t) noexcept {
   operator delete[](p, std::nothrow);
}

int main() {
   using std::nothrow;
   auto p = new (nothrow) int{ 3 }; // nao lanca; retorna nullptr em falha
   delete p;
   p = new (nothrow) int[10];
   delete[] p;
}
```

### O operador new mais importante: placement new

A versao mais importante de `operator new()` e seus companheiros e uma que voce nao pode substituir, mas mesmo que pudesse... bem, seria dificil alcancar algo mais eficiente:

```cpp
// nota: estas existem, voce pode usa-las mas nao pode substitui-las
void *operator new(std::size_t, void *p) { return p; }   // placement new
void *operator new[](std::size_t, void *p) { return p; } // placement new[]
void operator delete(void*, void*) noexcept { }           // nao faz nada
void operator delete[](void*, void*) noexcept { }         // nao faz nada
```

Chamamos essas funcoes de alocacao de posicionamento, mais conhecidas como *placement new* pela comunidade de programacao.

Qual e o proposito dessas funcoes? Voce pode se lembrar, no inicio de nossa discussao sobre as versoes globais dos operadores de alocacao, que declaramos: *"O que `new` faz e encontrar o local onde um objeto sera construido."* Isso nao significa necessariamente que `new` alocara memoria, e de fato, *placement new* nao aloca; ele simplesmente devolve o endereco que recebeu como argumento. Isso nos permite colocar um objeto onde quisermos na memoria... desde que tenhamos o direito de escrever a memoria naquele local.

O *placement new* serve a muitos propositos:

- Se tivermos direitos suficientes, pode nos deixar mapear um objeto em um pedaco de hardware mapeado em memoria, nos dando uma camada de abstracao extremamente fina sobre aquele dispositivo;
- Ele nos permite desacoplar alocacao de construcao, levando a melhorias significativas de velocidade ao escrever containers;
- Abre opcoes para implementar facilidades importantes como `optional<T>` (que pode ou nao armazenar um objeto `T`) e `variant<T0,T1,...,Tn>` (que armazena um objeto de um dos tipos `T0,T1,...,Tn`), ou mesmo tipos habilitados para *small object optimization* (SOO) como `std::string` e `std::function`.

Para mostrar como o *placement new* funciona sua magica, aqui esta um exemplo artificial (deliberadamente ruim) mas explicito:

```cpp
auto comprimento_string(const char *p) {
   using std::string;

   // Passo A: cria buffer local com tamanho e alinhamento corretos
   alignas(string) char buffer[sizeof(string)];

   // Passo B: "pinta" um objeto string nesse buffer via placement new
   // (o objeto pode alocar sua propria memoria interna, mas nao e nosso problema)
   string *s = new (static_cast<void*>(buffer)) string{ p };

   // Passo C: usa o objeto para calcular o comprimento
   const auto tamanho = s->size();

   // Passo D: destroi o objeto SEM liberar o buffer
   // (o buffer nao foi alocado dinamicamente, e apenas storage local)
   s->~string(); // chama destrutor explicitamente, sem liberar memoria

   return tamanho;
}
```

Quais sao os beneficios da versao complicada em comparacao com a simples `std::string{ p }.size()`? Nenhum, mas ela mostra as nuances de fazer esse tipo de manobra de gerenciamento de memoria de baixo nivel. A partir dos comentarios no exemplo:

- **Passo A** garante que o local onde o objeto sera construido tem o tamanho e o alinhamento corretos: e um buffer de bytes (tipo `char`), alinhado em memoria como um objeto `std::string` deve ser, e de tamanho suficiente para conter um objeto `std::string`;
- **Passo B** "pinta" um objeto `std::string` naquele buffer. E isso que um construtor faz: transforma memoria bruta em um objeto e inicializa o estado daquele objeto;
- **Passo C** usa o objeto recém-construido para nossa finalidade;
- **Passo D** destroi o objeto antes de sairmos da funcao, pois nao faze-lo levaria a um possivel vazamento de recursos. Note a sintaxe `s->~string()`, que chama o destrutor mas nao desaloca o storage para `*s`.

Usaremos esse recurso de maneiras muito mais razoaveis para obter vantagens significativas de velocidade ao escrever containers com gerenciamento explicito de memoria no Capitulo 12.

> **Uma nota sobre `make_shared<T>(args...)`**
>
> Mencionamos no Capitulo 6 que `make_shared<T>(args...)` geralmente leva a um layout de memoria melhor do que `shared_ptr<T>{ new T(args...) }`, pelo menos em relacao ao uso de cache. Agora podemos comecar a entender por que.
>
> Chamar `shared_ptr<T>::shared_ptr(T*)` torna o objeto responsavel por um *pointee* pre-existente. Uma vez que aquele objeto foi construido, o `shared_ptr<T>` tem que alocar um contador de referencia separadamente, terminando com duas alocacoes separadas, provavelmente em *cache lines* diferentes.
>
> Por outro lado, chamar `make_shared<T>(args...)` faz com que essa funcao de fabrica seja responsavel por criar um bloco de memoria cujo layout acomoda o objeto `T` e o contador de referencia, respeitando as restricoes de tamanho e alinhamento de ambos. Ha mais de uma maneira de fazer isso, incluindo recorrer a uma `union` ou a um buffer de bytes de tamanho e alinhamento apropriados, e depois realizar *placement new* para ambos os objetos nos locais apropriados dentro daquele buffer. No ultimo caso, terminamos com uma unica alocacao para um bloco contiguo de memoria capaz de hospedar ambos os objetos e duas chamadas a *placement new*.

### Versoes de funcao membro dos operadores de alocacao

As vezes, temos conhecimento especial das necessidades e requisitos de tipos especificos em relacao a alocacao dinamica de memoria. Um exemplo completo que entra em detalhes sobre um caso de uso real (mas simplificado) de tal conhecimento especifico de tipo e dado no Capitulo 10, onde discutimos a alocacao baseada em arena.

Por ora, nos limitaremos a cobrir a sintaxe e o efeito de uma sobrecarga de funcao membro dos operadores de alocacao. No exemplo a seguir, supomos que a classe `X` se beneficiaria de alguma forma de uma especializacao por classe desses mecanismos, e mostramos que o codigo cliente chamara essas especializacoes quando chamarmos `new X`, mas nao quando chamarmos `new int`:

```cpp
#include <iostream>
#include <new>

class X {
   // ...
public:
   X()  { std::cout << "X::X()\n";  }
   ~X() { std::cout << "X::~X()\n"; }

   // operadores de alocacao especificos para a classe X
   void *operator new(std::size_t);
   void *operator new[](std::size_t);
   void operator delete(void*);
   void operator delete[](void*);
};

// implementacoes: delegam para os operadores globais com logging
void* X::operator new(std::size_t n) {
   std::cout << "Magia de X::operator new()\n";
   return ::operator new(n); // :: = escopo global
}

void* X::operator new[](std::size_t n) {
   std::cout << "Magia de X::operator new[]()\n";
   return ::operator new[](n);
}

void X::operator delete(void *p) {
   std::cout << "Magia de X::operator delete()\n";
   return ::operator delete(p);
}

void X::operator delete[](void *p) {
   std::cout << "Magia de X::operator delete[]()\n";
   return ::operator delete[](p);
}

int main() {
   std::cout << "p = new int{3}\n";
   int *p = new int{ 3 }; // usa operator new global

   std::cout << "q = new X\n";
   X *q = new X;          // usa X::operator new

   std::cout << "delete p\n";
   delete p;              // usa operator delete global

   std::cout << "delete q\n";
   delete q;              // usa X::operator delete
}
```

Um detalhe importante a mencionar e que esses operadores sobrecarregados serao herdados por classes derivadas, o que significa que se a implementacao desses operadores de alguma forma depende de detalhes especificos daquela classe — por exemplo, seu tamanho ou alinhamento — considere marcar a classe que sobrecarrega esses operadores como `final`.

### Versoes cientes do alinhamento dos operadores de alocacao

Ao projetar o C++17, um problema fundamental com o processo de alocacao de memoria foi corrigido em relacao ao que chamamos de tipos *sobrealinhados*. A ideia e que ha tipos para os quais queremos restricoes de alinhamento mais estritas do que o alinhamento do alinhamento natural mais restrito em uma maquina, modelado pelo tipo `std::max_align_t`.

Ha muitas razoes para isso, mas um exemplo simples seria ao se comunicar com hardware especializado com requisitos que diferem dos do nosso computador. Suponha que o seguinte tipo `Float4` seja tal tipo. Seu tamanho e `4*sizeof(float)`, e exigimos que um `Float4` seja alinhado em um limite de 16 bytes:

```cpp
struct alignas(16) Float4 { float vals[4]; };
```

Neste exemplo, se removermos `alignas(16)` da declaracao do tipo, o alinhamento natural do tipo `Float4` seria `alignof(float)`, que provavelmente e 4 na maioria das plataformas.

O problema com tais tipos antes do C++17 e que as variaveis geradas pelo compilador respeitariam nossos requisitos de alinhamento, mas as localizadas no storage alocado dinamicamente, por padrao, terminariam com um alinhamento de `std::max_align_t`, o que seria incorreto.

Desde o C++17, podemos especificar requisitos de tipo sobrealinhado ao chamar `operator new()` ou `operator new[]()` passando um argumento adicional do tipo `std::align_val_t`, um tipo integral:

```cpp
#include <iostream>
#include <new>
#include <cstdlib>
#include <type_traits>

// versao ciente do alinhamento de operator new
void* operator new(std::size_t n, std::align_val_t al) {
   std::cout << "new(" << n << " bytes, alinhamento: "
             << static_cast<std::underlying_type_t<
                   std::align_val_t
                >>(al) << " bytes)\n";
   return std::aligned_alloc(static_cast<std::size_t>(al), n);
}
// (outros operadores omitidos por brevidade)

struct alignas(16) Float4 { float vals[4]; };

int main() {
   auto p = new Float4;                           // operator new(size_t)
   auto q = new(std::align_val_t{ 16 }) Float4;  // operator new(size_t, align_val_t)
   // vaza, claro, mas isso e secundario aqui
}
```

O bloco de memoria alocado para `p` neste exemplo sera alinhado em um limite de `std::max_align_t`, enquanto o bloco alocado para `q` sera alinhado em um limite de 16 bytes.

### Destroying delete

O C++20 traz um recurso novo e altamente especializado chamado *destroying delete* (exclusao destrutiva). O caso de uso aqui e uma sobrecarga de funcao membro que se beneficia do conhecimento especifico do tipo do objeto sendo destruido para realizar melhor o processo de destruicao. Quando essa funcao membro e definida para algum tipo `T`, ela e preferida em relacao a outras opcoes quando `delete` e invocado em um `T*`. Para usar *destroying delete* para algum tipo `X`, e preciso implementar a seguinte funcao membro:

```cpp
class X {
   // ...
public:
   void operator delete(X*, std::destroying_delete_t);
   // ...
};
```

Aqui, `std::destroying_delete_t` e um *tag type* como `std::nothrow_t`. Note que o primeiro argumento do *destroying delete* para a classe `X` e um `X*`, nao um `void*`, pois o *destroying delete* tem o duplo papel de destruir o objeto E desalocar a memoria.

Como isso funciona e por que e util? Vejamos um exemplo concreto com a seguinte classe `Encapsulador`:

```cpp
#include <new>
#include <iostream>

class Encapsulador {
public:
   enum class Tipo { A, B }; // qual implementacao escolher

private:
   struct Impl {
      virtual int f() const = 0;
   };

   struct ImplA final : Impl {
      int f() const override { return 3; }
      ~ImplA() { std::cout << "ImplA destruida\n"; }
   };

   struct ImplB final : Impl {
      int f() const override { return 4; }
      ~ImplB() { std::cout << "ImplB destruida\n"; }
   };

   Impl *impl;    // ponteiro para a implementacao escolhida
   Tipo tipo;     // qual implementacao foi escolhida

   static Impl *criar(Tipo t) {
      switch (t) {
         using enum Tipo;
         case A: return new ImplA;
         case B: return new ImplB;
      }
      throw 0;
   }

public:
   Encapsulador(Tipo t)
      : impl{ criar(t) }, tipo{ t } {
   }

   // destroying delete: sabe qual destrutor chamar sem virtual
   void operator delete(Encapsulador *p, std::destroying_delete_t) {
      if (p->tipo == Tipo::A) {
         delete static_cast<ImplA*>(p->impl); // destroi ImplA
      } else {
         delete static_cast<ImplB*>(p->impl); // destroi ImplB
      }
      p->~Encapsulador(); // destroi o Encapsulador
      ::operator delete(p); // libera a memoria
   }

   int f() const { return impl->f(); }
};

int main() {
   using namespace std;
   auto p = new Encapsulador{ Encapsulador::Tipo::A };
   cout << p->f() << endl;
   delete p; // usa destroying delete

   p = new Encapsulador{ Encapsulador::Tipo::B };
   cout << p->f() << endl;
   delete p; // usa destroying delete
}
```

O *destroying delete* e um recurso C++ recente por ocasiao da escrita deste texto, mas e uma ferramenta que pode nos dar mais controle sobre o processo de destruicao dos nossos objetos. A maioria dos seus tipos provavelmente nao precisa desse recurso, mas e bom saber que ele existe para os casos em que voce precisa daquele controle extra sobre a velocidade de execucao e o tamanho do programa. Como sempre, meça os resultados dos seus esforcos para garantir que eles tragam os beneficios desejados.

## Resumo

Ufa, que passeio! Agora que temos os conceitos basicos de sobrecarga de operadores de alocacao de memoria disponíveis, comecemos a usá-los a nosso favor. Nossa primeira aplicacao sera um detector de vazamentos (Capitulo 8) usando as formas globais desses operadores, seguido por exemplos simplificados de gerenciamento exotico de memoria (Capitulo 9) usando formas globais especializadas e personalizadas dos operadores, e gerenciamento de memoria baseado em arena (Capitulo 10) com versoes de membro dos operadores que realizarao otimizacoes muito satisfatorias.
