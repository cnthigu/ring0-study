# Capitulo 4 - Usando Destrutores (Using Destructors)

> Traducao nao oficial do livro *C++ Memory Management* (Patrice Roy, 2025).
> Texto adaptado para portugues brasileiro com fins educacionais. Paginas originais: 59-81.
> Repositorio: [ring0-study](https://github.com/cnthigu/ring0-study)

> Navegacao: [Anterior](capitulo-03.md) | [Indice](README.md) | [Proximo](capitulo-05.md)

---

Nossa jornada para uma compreensao melhor e mais profunda do gerenciamento de memoria em C++ agora emerge para o mundo do codigo limpo e das praticas contemporaneas. Nos capitulos anteriores, exploramos conceitos fundamentais da representacao em memoria (o que sao objetos, referencias, ponteiros, etc.), quais armadilhas nos aguardam se nos afastarmos de praticas de programacao solidas de formas inadequadas, e como podemos mentir para o sistema de tipos de maneira controlada e disciplinada — tudo isso sera util no restante deste livro. Agora discutiremos aspectos fundamentais do gerenciamento de recursos em nossa linguagem; como a memoria e um tipo especial de recurso, as ideias e tecnicas deste capitulo nos ajudarao a escrever codigo limpo e robusto, incluindo codigo que realiza tarefas de gerenciamento de memoria.

C++ e uma linguagem de programacao que suporta (entre outros paradigmas) programacao orientada a objetos, mas com objetos reais. Isso pode parecer uma piada de algum tipo, mas e uma afirmacao verdadeira: muitas linguagens fornecem apenas acesso indireto a objetos (por meio de ponteiros ou referencias), o que significa que, nessas linguagens, a semantica de atribuicao geralmente e compartilhar o objeto referenciado (o *pointee*). Ha vantagens nisso, claro: por exemplo, copiar uma referencia tipicamente nao pode falhar, enquanto copiar um objeto pode falhar se o construtor de copia ou a atribuicao de copia (dependendo da situacao) lançar uma excecao.

Em C++, por padrao, programas usam objetos, copiam objetos, atribuem para objetos, e assim por diante, e o acesso indireto e opcional, exigindo sintaxe adicional tanto para ponteiros quanto para referencias. Isso exige que programadores C++ pensem sobre o *lifetime* dos objetos, o que significa copiar um objeto, o que significa mover de um objeto... Esses podem ser topicos profundos dependendo dos tipos envolvidos.

> **Nota:** Consulte o Capitulo 1 para mais informacoes sobre objetos e *lifetime* de objetos, incluindo o papel dos construtores e destrutores.

Mesmo que ter objetos reais no codigo-fonte exija ajustar sua mentalidade ao programar, isso tambem oferece uma vantagem significativa: objetos automaticos sao destruidos quando chegam ao final do escopo em que foram declarados (quando chegam a chave de fechamento daquele escopo) e, quando um objeto e destruido, uma funcao especial, o destrutor do tipo, e chamada. Esse momento especial nos permite executar codigo arbitrario em momentos definidos e e parte de um idioma C++ fundamental chamado RAII, um acronimo que explicaremos na secao O idioma RAII mais adiante neste capitulo. Isso levou alguns luminares a afirmar que a instrucao mais bela em C++ e `}`, a chave de fechamento.

Neste capitulo, veremos o que os destrutores fazem, o que eles nao devem fazer, quando devem ser escritos (e quando devemos nos ater ao que o compilador faz por padrao), bem como como nosso codigo pode usar destrutores efetivamente para gerenciar recursos em geral... e memoria mais especificamente. Em seguida, daremos uma olhada rapida em alguns tipos-chave da biblioteca padrao que usam destrutores a nosso favor.

Em mais detalhes, neste capitulo:

- Forneceremos uma visao geral de como os recursos podem ser gerenciados com seguranca em C++;
- Examinaremos de perto o idioma RAII, uma pratica idiomatica bem conhecida que usa o *lifetime* de um objeto para garantir que os recursos gerenciados por esse objeto sejam liberados adequadamente;
- Examinaremos algumas armadilhas associadas ao gerenciamento automatizado de recursos;
- Daremos uma rapida visao geral de algumas ferramentas de gerenciamento automatizado de recursos fornecidas pela biblioteca padrao.

## Requisitos tecnicos

O codigo deste capitulo pode ser encontrado em: [https://github.com/PacktPublishing/C-Plus-Plus-Memory-Management/tree/main/chapter4](https://github.com/PacktPublishing/C-Plus-Plus-Memory-Management/tree/main/chapter4)

## Sobre destrutores: uma recapitulacao rapida

Este capitulo tem como objetivo discutir o uso de destrutores para gerenciar recursos, em particular a memoria, mas como discutimos destrutores ha algum tempo (no Capitulo 1), permitiremos uma recapitulacao rapida da ideia basica por tras dessa funcionalidade poderosa:

- Quando um objeto atinge o final de seu *lifetime*, uma funcao membro especial chamada de destrutor e chamada. Para alguma classe `X`, essa funcao membro e chamada `X::~X()`. Essa funcao e uma ocasiao para o tipo `X` realizar algumas acoes "de ultima hora" antes de concluir seu *lifetime*. Como discutiremos neste capitulo, um uso idiomatico do destrutor e liberar recursos mantidos pelo objeto que esta sendo destruido;
- Em uma hierarquia de classes, quando um objeto atinge o final de seu *lifetime*, o que acontece e: (a) o destrutor daquele objeto e chamado, depois o mesmo acontece com (b) o destrutor de cada um dos seus membros de dados nao estaticos na ordem de declaracao, seguido por (c) o destrutor de cada um dos seus subobjetos de classe base (seus "pais", informalmente) na ordem de declaracao;
- Ao destruir explicitamente um objeto por meio da aplicacao de `operator delete` em um ponteiro, o processo resultante envolve a destruicao do *pointee* seguida pela desalocacao do bloco de memoria onde o objeto estava localizado. Nao surpreendentemente, ha ressalvas a isso, como veremos no Capitulo 7;
- Em algumas situacoes, notavelmente quando alguma classe `X` expoe pelo menos uma funcao membro virtual, isso envia a mensagem de que um `X*` pode na pratica apontar para um objeto de uma classe `Y` derivada direta ou indiretamente de `X`. Para garantir que o destrutor de `Y` seja realmente chamado, nao o destrutor de `X`, e costume tambem qualificar `X::~X()` como `virtual`. Nao fazer isso arrisca nao chamar o destrutor correto, levando a *resource leaks* (vazamentos de recursos).

Para um pequeno exemplo, considere o seguinte:

```cpp
#include <iostream>
struct Base {
    ~Base() { std::cout << "~Base()\n"; }
};
struct DerivedA : Base {
    ~DerivedA() { std::cout << "~DerivedA()\n"; }
};
struct VirtBase {
    virtual ~VirtBase() {
       std::cout << "~VirtBase()\n";
    }
};
struct DerivedB : VirtBase {
    ~DerivedB() {
       std::cout << "~DerivedB()\n";
    }
};
int main() {
   {
      Base base;
   }
   {
      DerivedA derivedA;
   }
   std::cout << "----\n";
   Base *pBase = new DerivedA;
   delete pBase; // bad
   VirtBase *pVirtBase = new DerivedB;
   delete pVirtBase; // Ok
}
```

Se voce executar esse codigo, vera um destrutor chamado para `base` e dois chamados para `derivedA`: o destrutor da classe derivada seguido pelo da classe base. Isso e o esperado, e essa parte do codigo e correta.

O caso problematico e `pBase`, um ponteiro do tipo `Base*` que aponta para um objeto de uma classe derivada de `Base`, ja que o destrutor de `Base` nao e virtual — o que indica que tentar deletar o objeto derivado por meio de um ponteiro para a classe base e provavelmente uma violacao da intencao: `delete pBase` so chama `Base::~Base()`, nunca chamando `DerivedA::~DerivedA()`. Com `pVirtBase`, esse problema e evitado, pois `VirtBase::~VirtBase()` e virtual.

Claro, em C++, temos opcoes porque sempre surgem casos de uso surpreendentes, e veremos um no Capitulo 7, onde deletaremos um ponteiro-para-derivado a partir de um ponteiro-para-base sem a mediacao de um destrutor virtual, por razoes boas (embora especializadas).

Observe que funcoes membro virtuais sao uteis, mas tem custos: uma implementacao tipica criara uma tabela de ponteiros para funcoes por tipo com pelo menos uma funcao membro virtual e armazenara um ponteiro para essa tabela em cada objeto desse tipo, o que torna os objetos um pouco maiores. Como tal, use destrutores virtuais quando esperar usar um ponteiro para um objeto derivado a partir de um ponteiro para uma de suas bases, especialmente quando esperar que o destrutor seja chamado por meio de um ponteiro para dita classe base.

Dito isso, vamos examinar como tudo isso se relaciona ao gerenciamento de recursos.

## Gerenciando recursos

Suponha que voce esteja escrevendo uma funcao que abre um arquivo, le dele e o fecha depois. Voce esta desenvolvendo em uma plataforma procedural (como a maioria das APIs de sistemas operacionais sao) que oferece um conjunto de funcoes para realizar essas tarefas. Observe que todas as funcoes de "sistema operacional" neste exemplo sao deliberadamente ficticias, mas se assemelham a suas contrapartes do mundo real. As funcoes de interesse para nos nessa API sao:

```cpp
// opens the file called "name", returns a pointer
// to a file descriptor for that file (nullptr on failure)
FILE *open_file(const char *name);
// returns the number of bytes read from the file into
// buf. Preconditions: file is non-null and valid, buf
// points to a buffer of at least capacity bytes, and
// capacity >= 0
int read_from(FILE *file, char *buf, int capacity);
// closes file. Precondition: file is non-null and valid,
void close_file(FILE *file);
```

Suponha que seu codigo precisa processar os dados lidos do arquivo, mas que esse processamento pode lançar uma excecao. A razao para essa excecao nao e importante aqui: podem ser dados corrompidos, falha na alocacao de memoria, chamada de alguma funcao auxiliar que lança, etc. O ponto-chave e que ha um risco de que a funcao lançe.

Se tentarmos escrever o codigo para essa funcao de forma ingênua, poderia ser algo assim:

```cpp
void f(const char *name) {
   FILE *file = open_file(name);
   if(!file) return false; // failure
   vector<char> v;
   char buf[N]; // N is a positive integral constant
   for(int n = read_from(file, buf, N); n != 0;
       n = read_from(file, buf, N))
      v.insert(end(v), buf + 0, buf + n);
   process(v); // our processing function
   close_file(file);
}
```

Esse codigo funciona e, na ausencia de excecoes, faz praticamente o que queremos. Agora, suponha que `process(v)` lançe uma excecao... O que acontece?

Nesse caso, a funcao `f()` termina sem cumprir suas pós-condicoes. A chamada a `process(v)` nunca conclui... e `close_file(file);` nunca e chamado. Temos um *leak* (vazamento). Nao necessariamente um *memory leak*, mas um vazamento mesmo assim, pois o arquivo nunca e fechado: uma excecao lancada por `process()` mas nao capturada no codigo chamador `f()` concluira `f()` e deixara a excecao fluir para o chamador de `f()` (e assim por diante, ate ser capturada ou ate o programa travar — o que acontecer primeiro).

Ha formas de contornar essa situacao. Uma delas e proceder "manualmente" e adicionar um bloco `try ... catch` em torno do codigo que pode lançar:

```cpp
void f(const char *name) {
   FILE *file = open_file(name);
   if(!file) return; // failure
   vector<char> v;
   char buf[N]; // N is a positive integral constant
   try {
      for(int n = read_from(file, buf, N); n != 0;
          n = read_from(file, buf, N))
         v.insert(end(v), buf + 0, buf + n);
      process(v); // our processing function
      close_file(file);
   } catch(...) { // catch anything
      close_file(file);
      throw; // re-throw what we caught
   }
}
```

Concordo que isso e um pouco "desajeitado", com duas ocorrencias de `close_file(file)`: uma no final do bloco `try` para fechar o arquivo em ocorrencias normais, e outra no final do bloco `catch` para evitar o vazamento dos recursos do arquivo.

A abordagem manual pode funcionar, mas e uma abordagem fragil para o problema: em C++, qualquer funcao que nao seja `noexcept` ou `noexcept(true)` poderia lançar; isso significa que, na pratica, quase qualquer expressao poderia lançar.

> **Capturando qualquer coisa**
>
> Em C++, nao ha uma unica classe base obrigatoria para todos os tipos de excecao, como se poderia ver em algumas outras linguagens. Com efeito, `throw 3;` e codigo C++ totalmente legal. Alem disso, o C++ tem mecanismos de programacao generica extremamente poderosos, o que torna o codigo generico prevalente em nossa linguagem. Consequentemente, frequentemente nos encontramos chamando funcoes que podem lançar, mas para as quais nao podemos realmente saber o que poderia ser lançado. Saiba que `catch(...)` capturara qualquer objeto C++ usado para representar uma excecao: voce nao saberá o que capturou, mas tera capturado.
>
> Nesses casos, tipicamente queremos interceptar excecoes, provavelmente para fazer alguma limpeza, e entao deixar essa excecao continuar seu caminho inalterada, para que o codigo cliente possa lidar com ela conforme necessario. A parte de limpeza e porque queremos que nossa funcao seja *exception-safe* (sem vazamentos, sem estado corrompido, etc.) bem como *exception-neutral* (nao esconder a natureza do problema daqueles que vao querer trata-lo). Para relançar qualquer objeto de excecao que voce tenha capturado, mesmo de um bloco `catch(...)`, simplesmente use `throw;`, que e chamado de "relançamento".

### Tratamento de excecoes... ou nao?

Isso leva a outra questao: em uma funcao como `f()`, onde apenas pretendemos consumir dados e processa-los para nossos propositos, deveriamos realmente buscar tratar excecoes? Pense nisso: os requisitos para lançar uma excecao sao significativamente diferentes dos requisitos para tratar uma excecao.

Com efeito, lançamos uma excecao de uma funcao para sinalizar que nossa funcao nao pode atingir suas pós-condicoes (ela nao pode realizar a tarefa para a qual foi concebida): talvez a memoria seja insuficiente, talvez o arquivo a ser lido nao exista, talvez realizar aquela divisao inteira que voce pediu levasse a uma divisao por zero, destruindo o universo (e nao queremos que isso aconteca), talvez uma das funcoes chamadas por nossa funcao nao consiga satisfazer suas proprias pós-condicoes de formas que nao previmos ou nao quisemos tratar... Ha muitas razoes para uma funcao falhar. Sao muitas as situacoes em que uma funcao pode se encontrar em uma posicao em que prosseguir causaria problemas graves e, em alguns casos (construtores e operadores sobrecarregados vem a mente), excecoes sao realmente a unica maneira sensata de sinalizar um problema ao codigo cliente.

Tratar uma excecao por si so e uma ocorrencia muito mais rara: para lançar uma excecao, e preciso reconhecer um problema, mas para tratar uma excecao e preciso uma compreensao do contexto. Com efeito, as acoes que se tomaria em reacao a uma excecao em um aplicativo de console interativo sao diferentes das realizadas em um aplicativo de audio quando as pessoas estao se movendo na pista de danca, ou das necessarias ao enfrentar um colapso de codigo nuclear.

A maioria das funcoes precisa ser *exception-safe* em alguma medida (ha nuances nisso) mais do que precisam tratar o problema. Em nosso exemplo, as dificuldades resultam do fechamento manual do arquivo no advento de uma excecao. A maneira mais facil de evitar esse gerenciamento manual de recursos e automatiza-lo, e o que acontece no final de uma funcao — seja ela concluida normalmente (alcancando a chave de fechamento da funcao, atingindo uma instrucao `return`, vendo uma excecao "passar") — e modelado de forma mais adequada por um destrutor. Essa pratica tornou-se tao arraigada nas praticas dos programadores C++ que e considerada idiomatica e recebeu um nome: o idioma RAII.

## O idioma RAII

Programadores C++ tendem a usar destrutores para automatizar a liberacao de recursos, e isso pode ser verdadeiramente dito como uma tecnica de programacao idiomatica em nossa linguagem, tanto que lhe demos um nome. Provavelmente nao o melhor dos nomes, mas um nome bem conhecido assim mesmo: RAII, que significa *Resource Acquisition Is Initialization* (Aquisicao de Recurso e Inicializacao — alguns tambem sugeriram *Responsibility Acquisition Is Initialization*, que tambem funciona e carrega um significado semelhante). A ideia geral e que os objetos tendem a adquirir recursos no momento da construcao (ou depois), mas (e mais importante!) que a liberacao dos recursos mantidos por um objeto e algo que geralmente deve ser feito no final do *lifetime* daquele objeto. Assim, o RAII tem mais a ver com destrutores do que com construtores, mas como eu disse, tendemos a ser ruins com nomes e acronimos.

Revisitando nosso exemplo de leitura e processamento de arquivo da secao Gerenciando recursos, no inicio deste capitulo, podemos construir um manipulador de recursos RAII para facilitar o fechamento do arquivo, independentemente de como a funcao conclui:

```cpp
class FileCloser { // perfectible, as we will see
   FILE * file;
public:
   FileCloser(FILE *file) : file{ file } {
   }
   ~FileCloser() {
      close_file(file);
   }
};
void f(const char *name) {
   FILE *file = open_file(name);
   if(!file) return; // failure
   FileCloser fc{ file }; // <-- fc manages file now
   vector<char> v;
   char buf[N]; // N is a positive integral constant
   for(int n = read_from(file, buf, N); n != 0;
       n = read_from(file, buf, N))
      v.insert(end(v), buf + 0, buf + n);
   process(v); // our processing function
} // implicit close_file(file)
```

Os detalhes e a granularidade do que `FileCloser` faz variarao de acordo com nossa percepcao de seu papel: essa classe apenas gerencia o fechamento do arquivo ou realmente representa o arquivo com todos os seus servicos? Optei pela primeira opcao neste caso, mas ambas as opcoes sao razoaveis: tudo depende da semantica que voce busca implementar. O ponto-chave e que, ao usar um objeto `FileCloser`, estamos aliviando o codigo cliente de uma responsabilidade, delegando em vez disso a responsabilidade de fechar um arquivo a um objeto que automatiza essa tarefa, simplificando nosso proprio codigo e reduzindo os riscos de deixa-lo inadvertidamente aberto.

Esse objeto `FileCloser` e muito especifico para nossa tarefa. Poderıamos generalize-lo de varias maneiras, por exemplo, por meio de um objeto generico que realiza um conjunto de acoes fornecidas pelo usuario quando destruido:

```cpp
template <class F> class scoped_finalizer { // simplified
   F f;
public:
   scoped_finalizer(F f) : f{ f } {
   }
   ~scoped_finalizer() {
      f();
   }
};
void f(const char *name) {
   FILE *file = open_file(name);
   if(!file) return; // failure
   auto sf = scoped_finalizer{ [&file] {
      close_file(file);
   } }; // <-- sf manages file now
   vector<char> v;
   char buf[N]; // N is a positive integral constant
   for(int n = read_from(file, buf, N); n != 0;
       n = read_from(file, buf, N))
      v.insert(end(v), buf + 0, buf + n);
   process(v); // our processing function
} // implicit close_file(file) through sf's destructor
```

O idioma RAII esta praticamente em todo lugar em C++; poder-se-ia dizer que e o idioma mais pervasivo da linguagem e uma de suas praticas de programacao mais reconheciveis e definidoras. Muitas linguagens oferecem recursos semelhantes hoje em dia: C# tem blocos `using`, Java tem blocos `try-with`, Go tem a palavra-chave `defer`, etc., mas em C++, a possibilidade de usar escopo para automatizar acoes, frequentemente relacionadas ao gerenciamento de recursos, flui diretamente do sistema de tipos e faz com que os objetos, nao o codigo do usuario, sejam os que gerenciam recursos de forma idiomatica.

### RAII e as funcoes membro especiais do C++

O Capitulo 1 descreveu as seis funcoes membro especiais (construtor default, destrutor, construtor de copia, atribuicao de copia, construtor de movimento e atribuicao de movimento). Quando se implementa essas funcoes em uma classe, geralmente significa que aquela classe e responsavel por algum recurso. Como mencionado no Capitulo 1, quando uma classe nao gerencia recursos explicitamente, muitas vezes podemos deixar essas funcoes para o compilador e o comportamento default resultante geralmente levara a codigo mais simples e eficiente.

Considere agora que o idioma RAII trata principalmente de gerenciamento de recursos, pois associamos o momento de destruicao de um objeto ao ato de liberar recursos previamente adquiridos. Varios objetos RAII (incluindo as classes `FileCloser` e `scoped_finalizer` nos exemplos anteriores) podem ser ditos responsaveis pelos recursos que lhes fornecemos, o que significa que copiar esses objetos poderia induzir bugs (quem seria responsavel pelos recursos, o original ou a copia?). Portanto, considere deletar as operacoes de copia para seus tipos RAII, a menos que voce tenha uma boa razao para implementa-las explicitamente:

```cpp
template <class F> class scoped_finalizer {
   F f;
public:
   scoped_finalizer(const scoped_finalizer&) = delete;
   scoped_finalizer& operator=
      (const scoped_finalizer&) = delete;
   scoped_finalizer(F f) : f{ f } {
   }
   ~scoped_finalizer() {
      f();
   }
};
```

Como a maioria dos idiomas, o RAII e uma pratica de programacao geralmente aceita, mas nao e uma panaceia, e o mesmo vale para o uso de destrutores em geral. Veremos alguns riscos envolvidos com destrutores e como podemos evitar esse tipo de problema.

## Algumas armadilhas

Destrutores sao maravilhosos. Eles nos permitem automatizar tarefas, simplificam o codigo e o tornam mais seguro em geral. Ainda assim, ha algumas ressalvas, alguns aspectos do uso de destrutores que exigem atencao especial.

### Destrutores nao devem lançar

O titulo desta secao diz de forma bastante simples: destrutores nao devem lançar. Eles podem lançar, mas e uma ma ideia faze-lo.

Isso pode parecer surpreendente a primeira vista. Afinal, construtores podem (e de fato lancam!) excecoes. Quando um construtor lança, significa que o construtor nao pode satisfazer suas pós-condicoes: o objeto em construcao nao foi construido (o construtor nao foi concluido!), portanto aquele objeto nao existe. E um modelo simples e funcional.

Se um destrutor lança... bem, provavelmente e o fim do seu programa. Com efeito, destrutores sao implicitamente `noexcept`, o que significa que lançar de um destrutor chamara `std::terminate()` e esse sera o fim do seu programa.

Bom, voce pode pensar: e se eu marcar explicitamente meu destrutor como `noexcept(false)`, substituindo assim o comportamento default? Bem, isso pode funcionar, mas cuidado: se um destrutor lança durante o *stack unwinding* (desenrolamento da pilha), como o que acontece quando uma excecao ja esta em curso, isso ainda chama `std::terminate()` e, como voce se comportou mal e quebrou as regras, o compilador pode otimizar parte do seu codigo. Por exemplo, no seguinte programa, e muito possivel que nem "A\n" nem "B\n" sejam impressos, mesmo que o destrutor de `Evil` nao tenha sido chamado naquele ponto:

```cpp
#include <iostream>
class Darn {};
void f() { throw 3; }
struct Evil {
   Evil() { std::cout << "Evil::Evil()\n"; }
   ~Evil() noexcept(false) {
      std::cout << "Evil::~Evil()\n";
      throw Darn {};
   }
};
void g() {
    std::cout << "A\n";
    Evil e;
    std::cout << "B\n";
    f();
    std::cout << "C\n";
}
int main() {
   try {
      g();
   } catch(int) {
      std::cerr << "catch(int)\n";
   } catch(Darn) {
      std::cerr << "darn...\n";
   }
}
```

Um resultado provavel deste codigo pode ser simplesmente que o programa nao exiba nada, e alguma informacao indicando que lançar `Darn` levou a chamar `std::terminate()` sera exibida. Por que parte do codigo (notavelmente as mensagens que tentamos exibir) e visivelmente removida pelo compilador? A resposta e que uma excecao lançada mas nunca capturada entra em comportamento definido pela implementacao, e o lançamento de `Darn` neste caso nao pode ser capturado (ele chama diretamente `std::terminate()`, pois acontece durante o *stack unwinding*), o que permite ao compilador otimizar nosso codigo significativamente.

Resumindo: nao lançe de um destrutor, a menos que voce realmente saiba o que esta fazendo, controle o contexto em que ele sera chamado e tenha discutido com outros para garantir que seja razoavel, mesmo que todas as evidencias apontem para o contrario. Mesmo assim, provavelmente e melhor procurar abordagens alternativas.

### Conheca a ordem de destruicao

O titulo desta secao pode parecer uma advertencia engraçada. Por que e importante saber sobre a ordem em que nossos objetos serao destruidos? Afinal, a regra basica e simples: a construcao e a destruicao de objetos sao simetricas, portanto os objetos sao destruidos na ordem inversa de construcao... certo?

Bom, esse e o caso para objetos locais e automaticos. Se voce escrever o seguinte:

```cpp
void f() {
   A a; // a's ctor
   B b; // b's ctor
   {
      C c; // c's ctor
   } // c's dtor
   D d; // d's ctor
} // d's dtor, b's dtor, a's dtor (in that order)
```

... entao a ordem de construcao e destruicao sera conforme indicado nos comentarios: objetos automaticos em escopo sao destruidos na ordem inversa de construcao, e escopos aninhados se comportam como esperado.

A situacao fica mais complexa se voce adicionar objetos nao automaticos ao contexto. O C++ permite ter objetos `static` declarados dentro de uma funcao: eles sao construidos quando a funcao e chamada pela primeira vez e permanecem vivos desde esse ponto ate o final da execucao do programa. O C++ permite ter variaveis globais (ha muitas nuances aqui com especificacoes de *linkage* como `static` ou `extern`). O C++ permite ter membros de dados `static` em uma classe: esses sao essencialmente variaveis globais tambem. Nao vou nem abordar variaveis `thread_local` aqui, pois estao fora do escopo deste livro, mas se voce as usar, saiba que elas podem ser inicializadas *lazily* (preguicosamente), o que adiciona complexidade ao quadro geral. Objetos globais sao destruidos na ordem inversa de construcao, mas essa ordem de construcao nem sempre e trivial de prever da nossa perspectiva humana.

Considere o seguinte exemplo, que usa objetos `Verbose` que nos informam de seu momento de construcao, bem como de seu momento de destruicao:

```cpp
#include <iostream>
#include <format>
struct Verbose {
   int n;
   Verbose(int n) : n{ n } {
      std::cout << std::format("Verbose({})\n", n);
   }
   ~Verbose(){
      std::cout << std::format("~Verbose({})\n", n);
   }
};
class X {
   static inline Verbose v0 { 0 };
   Verbose v1{ 1 };
};
Verbose v2{ 2 };
static void f() {
    static Verbose v3 { 3 };
    Verbose v4{ 4 };
}
static void g() { // note : never called
    static Verbose v5 { 5 };
}
int main() {
   Verbose v6{ 6 };
   {
      Verbose v7{ 7 };
      f();
      X x;
   }
   f();
   X x;
}
```

Tome um momento para deixar este exemplo assentar e tente descobrir o que sera exibido. Temos um objeto global, um membro de dados `static inline` em uma classe, dois objetos `static` locais a funcoes, bem como alguns objetos locais automaticos.

Entao, o que sera exibido se executarmos este programa? Se voce tentar, devera ver:

```
Verbose(0)
Verbose(2)
Verbose(6)
Verbose(7)
Verbose(3)
Verbose(4)
~Verbose(4)
Verbose(1)
~Verbose(1)
~Verbose(7)
Verbose(4)
~Verbose(4)
Verbose(1)
~Verbose(1)
~Verbose(6)
~Verbose(3)
~Verbose(2)
~Verbose(0)
```

O primeiro a ser construido (e o ultimo a ser destruido) e `v0`, o membro de dados `static inline`. Ele tambem acontece de ser nosso primeiro objeto global, seguido por `v2` (nosso segundo objeto global). Em seguida, entramos em `main()` e criamos `v6`, que sera destruido no final de `main()`.

Agora, se voce olhar para a saida daquele programa, vera que a simetria quebra neste ponto: apos a construcao de `v6`, construimos `v7` (em um escopo interno e mais estreito; `v7` sera destruido logo depois) e entao chamamos `f()` pela primeira vez, o que constroi `v3` — mas `v3` e um objeto global e por essa razao sera destruido depois de `v6` e `v7`.

O processo geral e mecanico e deterministico, mas entende-lo requer algum pensamento e analise. Se usarmos os destrutores de nossos objetos para liberar recursos, a falha em entender o que acontece e quando acontece pode fazer com que nosso codigo tente usar recursos que ja foram liberados.

Para um exemplo concreto envolvendo uma mistura de gerenciamento automatizado e manual de recursos, vamos ver algo que o padrao C++ nao conhece: bibliotecas vinculadas dinamicamente (arquivos `.dll`). Nao vou entrar em detalhes aqui, mas saiba que se voce estiver em uma maquina Linux (usando *shared objects*, arquivos `.so`) ou em um Mac (arquivos `.dylib`), a ideia geral e a mesma, mas os nomes das funcoes diferirao.

Nosso programa vai (a) carregar uma biblioteca vinculada dinamicamente, (b) obter o endereco de uma funcao, (c) chamar esta funcao e (d) descarregar a biblioteca. Suponha que a biblioteca se chame `Lib` e a funcao que queremos chamar se chame `factory`, que retorna um `X*` a partir do qual queremos chamar a funcao membro `f()`:

```cpp
#include "Lib.h"
#include <Windows.h> // LoadLibrary, GetProcAddress
int main() {
   using namespace std;
   HMODULE hMod = LoadLibrary(L"Lib.dll");
   // suppose the signature of factory is in Lib.h
   auto factory_ptr = reinterpret_cast<
      decltype(&factory)
   >(GetProcAddress(hMod, "factory"));
   X *p = factory_ptr();
   p->f();
   delete p;
   FreeLibrary(hMod);
}
```

Voce pode ter notado o gerenciamento manual de memoria ali: adquirimos um recurso (um `X*` apontando para algo que e pelo menos um `X`) chamando `factory()` por meio de `factory_ptr`, depois usamos (chamamos `f()`) e descartamos manualmente o *pointee*.

Nesse ponto, voce provavelmente esta dizendo que o gerenciamento manual de recursos nao e a melhor das ideias (aqui: o que acontece com o recurso se `p->f()` lançar?), entao voce consulta o padrao e descobre que um objeto do tipo `std::unique_ptr` assumira a responsabilidade pelo *pointee* e o destruira quando seu destrutor for alcancado. Belo, nao e? E de fato provavelmente e, mas considere o seguinte trecho, reescrito para usar `std::unique_ptr` e automatizar o processo de gerenciamento de recursos:

```cpp
#include "Lib.h"
#include <memory> // std::unique_ptr
#include <Windows.h> // LoadLibrary, GetProcAddress
int main() {
   using namespace std;
   HMODULE hMod = LoadLibrary(L"Lib.dll");
   // suppose the signature of factory is in Lib.h
   auto factory_ptr = reinterpret_cast<
      decltype(&factory)
   >(GetProcAddress(hMod, "factory"));
   std::unique_ptr<X> p { factory_ptr() };
   p->f();
   // delete p; // not needed anymore
   FreeLibrary(hMod);
} // p is destroyed here... but is this good?
```

A primeira vista, essa nova versao parece mais segura, ja que `p` agora e um objeto RAII responsavel pela destruicao do *pointee*. Sendo destruido na chave de fechamento de nossa funcao `main()`, sabemos que o destrutor do *pointee* sera chamado mesmo se `p->f()` lançar, portanto, nos consideramos mais *exception-safe* do que antes...

... exceto que esse codigo trava naquela chave de fechamento! Se voce investigar a origem da queda, provavelmente acabara percebendo que ela acontece no ponto em que o destrutor de `p` chama `operator delete` no `X*` que ele armazena internamente. Lendo mais, voce notara que a razao pela qual essa queda acontece e que a biblioteca da qual o objeto veio foi liberada (chamada de `FreeLibrary()`) antes do destrutor ser executado.

Isso significa que nao podemos usar uma ferramenta de gerenciamento automatizado de memoria aqui? Claro que nao, mas precisamos ser mais cuidadosos com a forma como usamos o *lifetime* do objeto a nosso favor. Neste exemplo, queremos garantir que `p` seja destruido antes que a chamada a `FreeLibrary()` aconteca; isso pode ser alcancado por meio da simples introducao de um escopo em nossa funcao:

```cpp
#include "Lib.h"
#include <memory> // std::unique_ptr
#include <Windows.h> // LoadLibrary, GetProcAddress
int main() {
   using namespace std;
   HMODULE hMod = LoadLibrary(L"Lib.dll");
   // suppose the signature of factory is in Lib.h
   auto factory_ptr = reinterpret_cast<
      decltype(&factory)
   >(GetProcAddress(hMod, "factory"));
   {
      std::unique_ptr<X> p { factory_ptr() };
      p->f();
   } // p is destroyed here
   FreeLibrary(hMod);
}
```

Neste exemplo especifico, encontramos uma solucao simples; em outros casos, poderiamos ter que mover algumas declaracoes para garantir que os escopos nos quais nossos objetos se encontram nao alterem a semantica pretendida de nossa funcao. Entender a ordem em que os objetos sao destruidos e essencial para usar adequadamente esse precioso recurso de gerenciamento de recursos que e o destrutor.

## Ferramentas padrao de automacao do gerenciamento de recursos

A biblioteca padrao oferece um numero significativo de classes que gerenciam memoria eficientemente. Basta considerar os containers padrao para ver exemplos marcantes desse tipo. Nesta secao, daremos uma rapida olhada em alguns exemplos de tipos uteis para o gerenciamento de recursos. Longe de fornecer uma lista exaustiva, tentaremos mostrar diferentes maneiras de se beneficiar do idioma RAII.

Como mencionado antes, ao expressar um tipo que fornece gerenciamento automatizado de recursos, os aspectos-chave do comportamento daquele tipo sao expressos por meio de suas seis funcoes membro especiais. Por essa razao, com cada um dos tipos a seguir, daremos uma breve olhada em qual e a semantica dessas funcoes.

### unique_ptr<T> e shared_ptr<T>

Esta curta secao tem como objetivo fornecer uma breve visao geral dos dois principais tipos de *smart pointers* padrao na biblioteca padrao C++: `std::unique_ptr<T>` e `std::shared_ptr<T>`. Ela pretende fornecer uma visao ampla do papel de cada tipo; um exame mais detalhado de como esses tipos podem ser usados aparece no Capitulo 5, e implementaremos versoes simplificadas de ambos os tipos (bem como de alguns outros tipos de *smart pointers*) no Capitulo 6.

Vimos um exemplo usando `std::unique_ptr<T>` anteriormente neste capitulo. Um objeto desse tipo implementa semantica de "posse exclusiva do recurso": um objeto do tipo `std::unique_ptr<T>` nao e copiavel e, quando fornecido com um `T*` para gerenciar, ele destroi o *pointee* no final de seu *lifetime*. Por padrao, esse tipo chamara `delete` no ponteiro que gerencia, mas pode ser feito para usar algum outro meio de descarte, se necessario.

Um `std::unique_ptr<T>` default representa um objeto vazio e se comporta principalmente como um ponteiro nulo. Como esse tipo expressa posse exclusiva de um recurso, e nao copiavel. Mover de um `std::unique_ptr<T>` transfere a *ownership* (posse) do recurso, deixando o objeto movido em um estado vazio conceitualmente analogo a um ponteiro nulo. O destrutor desse tipo destroi o recurso gerenciado pelo objeto, se houver algum.

O tipo `std::shared_ptr<T>` implementa semantica de "posse compartilhada do recurso". Com esse tipo, cada objeto `std::shared_ptr<T>` que co-possui um determinado ponteiro compartilha responsabilidades em relacao ao *lifetime* do *pointee*, e o ultimo co-proprietario do recurso e responsavel por libera-lo; como e o caso com a maioria dos *smart pointers*, essa responsabilidade recai sobre o destrutor do objeto. Esse tipo e surpreendentemente complicado de escrever, mesmo em uma implementacao um tanto ingênua como a que escreveremos no Capitulo 6, e e menos frequentemente util do que algumas pessoas pensam, pois o principal caso de uso (expressar posse no sistema de tipos para casos em que o ultimo proprietario do *pointee* e a priori desconhecido, algo mais frequentemente visto em codigo multithread) e mais especializado do que muitos acreditariam. Mas quando se precisa preencher esse nicho, e o tipo que e imensamente util.

Um `std::shared_ptr<T>` default tambem representa um objeto vazio e se comporta principalmente como um ponteiro nulo. Como esse tipo expressa posse compartilhada de um recurso, e copiavel, mas copiar um objeto significa compartilhar o *pointee*; a atribuicao de copia libera o recurso mantido pelo objeto no lado esquerdo da atribuicao e, em seguida, compartilha o recurso mantido pelo objeto no lado direito da atribuicao entre os dois objetos. Mover de um `std::unique_ptr<T>` transfere a posse do recurso, deixando o objeto movido em um estado vazio. O destrutor desse tipo libera a posse do recurso compartilhado, destruindo o recurso gerenciado pelo objeto se aquele objeto foi o ultimo proprietario.

> **O que "shared" em shared_ptr significa?**
>
> Pode haver confusao em relacao ao que a palavra "shared" no nome do tipo `std::shared_ptr` realmente significa. Por exemplo, deveriamos usar esse tipo sempre que quisermos compartilhar um ponteiro entre chamador e chamado? Deveriamos usa-lo sempre que o codigo cliente fizer uma copia de um ponteiro com a intencao de compartilhar o *pointee*, como ao passar um ponteiro por valor para uma funcao ou compartilhar recursos armazenados em um objeto gerenciador global?
>
> A resposta curta e que essa e a abordagem errada para os *smart pointers*. Compartilhar um recurso alocado dinamicamente nao significa co-possuir esse recurso: apenas o ultimo e o que `std::shared_ptr` modela, enquanto o primeiro pode ser feito com tipos muito mais leves. Examinaremos essa ideia em detalhes no Capitulo 5 de uma perspectiva de uso, depois a reexaminaremos no Capitulo 6 com nossos olhos de implementador, esperancosamente construindo uma compreensao mais abrangente dessas questoes profundas e sutis.

### lock_guard e scoped_lock

Possuir um recurso nao se limita a possuir memoria. Com efeito, considere o seguinte trecho de codigo e suponha que `string_mutator` seja uma classe usada para realizar transformacoes arbitrarias em caracteres de uma string, mas que se espera ser usada em um contexto multithread, no sentido de que e necessario sincronizar os acessos aquele objeto string:

```cpp
#include <thread>
#include <mutex>
#include <string>
#include <algorithm>
#include <string_view>
class string_mutator {
   std::string text;
   mutable std::mutex m;
public:
   // note: m in uncopiable so string_mutator
   // also is uncopiable
   string_mutator(std::string_view src)
      : text{ src.begin(), src.end() } {
   }
   template <class F> void operator()(F f) {
      m.lock();
      std::transform(text.begin(), text.end(),
                     text.begin(), f);
      m.unlock();
   }
   std::string grab_snapshot() const {
      m.lock();
      std::string s = text;
      m.unlock();
      return s;
   }
};
```

Neste exemplo, o operador de chamada de funcao de um objeto `string_mutator` aceita uma funcao arbitraria `f` aplicavel a um `char` e que retorna algo que pode ser convertido para um `char`, e depois aplica `f` a cada `char` na sequencia. Por exemplo, a seguinte chamada exibiria "I LOVE MY INSTRUCTOR":

```cpp
// ...
string_mutator sm{ "I love my instructor" };
sm([](char c) {
   return static_cast<char>(std::toupper(c));
});
std::cout << sm.grab_snaphot();
// ...
```

Agora, como `string_mutator::operator()(F)` aceita qualquer funcao com a assinatura adequada como argumento, poderia, entre outras coisas, aceitar uma funcao que poderia lançar uma excecao. Olhando para a implementacao daquele operador, voce notara que, com a implementacao atual (ingênua), isso bloquearia `m`, mas nunca o desbloquearia — uma situacao ruim de fato.

Ha linguagens que oferecem construcoes de linguagem especializadas para resolver esse problema. Em C++, nao ha necessidade de tal suporte especializado, pois codigo robusto flui diretamente do fato de que se poderia escrever um objeto que bloqueia um *mutex* no momento da construcao e o desbloqueia quando destruido... e e praticamente tudo que precisamos. Em C++, o tipo mais simples desse tipo e `std::lock_guard<M>`, onde uma implementacao simples poderia ser:

```cpp
template <class M>
   class lock_guard { // simplified version
      M &m;
   public:
      lock_guard(M &m) : m { m } { m.lock(); }
      ~lock_guard() { m.unlock(); }
      lock_guard(const lock_guard&) = delete;
      lock_guard& operator=(const lock_guard&) = delete;
   };
```

Os tipos mais simples sao frequentemente os melhores. Com efeito, aplicando esse tipo ao nosso exemplo `string_mutator`, acabamos com uma implementacao mais simples, porem muito mais robusta:

```cpp
#include <thread>
#include <mutex>
#include <string>
#include <algorithm>
#include <string_view>
class string_mutator {
   std::string text;
   mutable std::mutex m;
public:
   // note: m in uncopiable so string_mutator
   // also is uncopiable
   string_mutator(std::string_view src)
      : text{ src.begin(), src.end() } {
   }
   template <class F> void operator()(F f) {
      std::lock_guard lck{ m };
      std::transform(text.begin(), text.end(),
                     text.begin(), f);
   } // implicit m.unlock
   std::string grab_snapshot() const {
      std::lock_guard lck{ m };
      return text;
   } // implicit m.unlock
};
```

Claramente, usar destrutores para automatizar o desbloqueio de nosso *mutex* e vantajoso em casos como este: simplifica o codigo e ajuda a torna-lo *exception-safe*.

### Objetos stream

Em C++, objetos *stream* tambem sao proprietarios de recursos. Considere o seguinte exemplo de codigo onde copiamos cada byte do arquivo `in.txt` para a *stream* de saida padrao:

```cpp
#include <fstream>
#include <iostream>
int main() {
   std::ifstream in{ "in.txt" };
   for(char c; in.get(c); )
      std::cout << c;
}
```

Voce pode notar alguns detalhes interessantes neste codigo: nunca chamamos `close()`, nao ha bloco `try` onde estarıamos nos preparando para o gerenciamento de excecoes, nao ha chamada a `open()` para abrir o arquivo, nao ha verificacao explicita de algum estado de fim de arquivo... ainda assim, esse codigo funciona corretamente, faz o que supostamente deve fazer e nao vaza recursos.

Como um programa tao simples pode fazer tudo isso? Por meio da "magia dos destrutores", ou (mais precisamente) a magia de uma boa API. Pense bem:

- O papel do construtor e colocar o objeto em um estado inicial correto. Assim, o usamos para abrir o arquivo, pois seria tanto inutil quanto ineficiente construir o *stream* por default e depois abre-lo mais tarde.
- Erros ao ler de um *stream* nao sao excepcionais de forma alguma... Pense bem: com que frequencia enfrentamos erros ao ler de um *stream*? Em C++, ler de um *stream* (aqui: chamar `in.get(c)`) retorna uma referencia para o *stream* apos a leitura, e esse *stream* se comporta como um valor `false` booleano se o *stream* estiver em um estado de erro.
- Por fim, o destrutor de um objeto *stream* fecha qualquer representacao de *stream* pela qual e responsavel. Chamar `close()` em um *stream* em C++ e desnecessario na maioria das vezes; simplesmente usar o objeto *stream* em um escopo limitado geralmente e suficiente.

Destrutores (e construtores!), quando usados adequadamente, levam a codigo mais robusto e simples.

### vector<T> e outros containers

Nao escreveremos uma comparacao completa dos containers com arrays nativos ou outras construcoes de baixo nivel, como listas encadeadas com nos gerenciados manualmente ou arrays dinamicos mantidos explicitamente pelo codigo cliente. Examinaremos, no entanto, como se pode escrever containers como `std::vector` ou `std::list` em capitulos posteriores deste livro (Capitulos 12, 13 e 14), quando soubermos um pouco mais sobre tecnicas de gerenciamento de memoria.

Observe, ainda assim, que usar `std::vector<T>` (por exemplo) nao e apenas significativamente mais simples e seguro do que gerenciar um array alocado dinamicamente de `T`: na pratica, e muito provavelmente significativamente mais rapido, pelo menos se usado com conhecimento de causa. Como veremos, nao ha como os usuarios investirem o cuidado e a atencao que vao para o gerenciamento de memoria e a criacao, destruicao, copia ou movimento de objetos que vao para um container padrao ao escrever codigo do dia a dia. O destrutor desses tipos, aliado a forma como suas outras funcoes membro especiais sao implementadas, os torna quase tao faceis de usar quanto objetos `int` — um objetivo digno se e que algum dia existiu!

## Resumo

Neste capitulo, discutimos alguns problemas relacionados a seguranca, com foco nos que envolvem excecoes. Vimos que alguns tipos da biblioteca padrao oferecem semantica especializada em relacao ao gerenciamento de recursos, onde "recurso" inclui, mas nao se limita a, memoria. No Capitulo 5, passaremos algum tempo examinando como usar e se beneficiar dos *smart pointers* padrao; depois, no Capitulo 6, iremos mais fundo e olharemos para alguns dos desafios por tras de escrever suas proprias versoes desses *smart pointers*, bem como alguns outros tipos inspirados em *smart pointers* com outras semanticas. Em seguida, nos aprofundaremos em preocupacoes mais profundas relacionadas ao gerenciamento de memoria.
