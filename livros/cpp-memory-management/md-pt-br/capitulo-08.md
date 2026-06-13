# Capitulo 8 - Escrevendo um Detector de Vazamentos Ingenuo (Writing a Naïve Leak Detector)

> Traducao nao oficial do livro *C++ Memory Management* (Patrice Roy, 2025).
> Texto adaptado para portugues brasileiro com fins educacionais. Paginas originais: 163-185.
> Repositorio: [ring0-study](https://github.com/cnthigu/ring0-study)

> Navegacao: [Anterior](capitulo-07.md) | [Indice](README.md) | [Proximo](capitulo-09.md)

---

No Capitulo 7, examinamos diversas formas de sobrecarregar os operadores de alocacao de memoria — `new`, `new[]`, `delete` e `delete[]` — para compreender a sintaxe envolvida na escrita desses operadores, bem como como podem ser usados no codigo cliente. Discutimos como esses operadores interagem com excecoes (mesmo no caso das versoes *nothrow*) e vimos por que eles devem, na maioria dos casos, ser escritos em grupos de quatro ou multiplos disso. Por exemplo, codigo que chama a versao *nothrow* de `operator new()` para obter algum ponteiro e depois chama `delete p` rapidamente entrara em apuros se sobrecarregarmos a versao *nothrow* mas nao a versao "normal", pois ambas podem nao ser mais compatíveis entre si.

O que nao discutimos de fato e como nosso codigo poderia se beneficiar de assumir o controle desses operadores. Ha de fato varios usos para isso: rastrear como ou onde a memoria e alocada, medir a fragmentacao de memoria em um processo, implementar uma estrategia especializada para controlar as caracteristicas de desempenho do processo de alocacao ou desalocacao, e assim por diante. Como este livro tem tamanho finito, nao podemos esperar cobrir exemplos de todas as opcoes possiveis, entao escolheremos uma, esperando que esse exemplo seja suficientemente inspirador para voce explorar outros caminhos por conta propria.

O exemplo que exploraremos neste capitulo e um detector de vazamentos de memoria simples porem funcional. Com mais detalhes, faremos o seguinte:

1. Primeiro detalharemos o plano, dando uma visao geral de como nosso detector de vazamentos funcionara e quais truques usaremos para atingir nossos objetivos;
2. Entao implementaremos a primeira versao de nossa ferramenta, que parecera funcionar, pelo menos na superficie. Percorreremos uma chamada a `operator new()` e ao `operator delete()` correspondente para entender o que acontece na memoria ao longo desse processo;
3. Nesse ponto, usaremos o conhecimento adquirido em capitulos anteriores para identificar as falhas em nossa primeira solucao, bem como maneiras de corrigi-las;
4. Finalmente, revisitaremos nossa implementacao inicial e terminaremos com algo simples porem utilizavel em codigo real.

Como este sera um capitulo bastante concreto, voce pode esperar desenvolver (ou refinar) algumas habilidades uteis ao longo do caminho:

- A primeira e **planejar antes de codificar**. Estaremos escrevendo codigo de baixo nivel neste capitulo, o que torna particularmente importante ter uma direcao clara em mente. Afinal, ao codificar "proximo a maquina" e manipular memoria bruta, a rede de seguranca fornecida pelo compilador — o sistema de tipos — tende a ser um pouco mais fina e e mais facil cometer erros (custosos) se nao formos cuidadosos.
- A segunda e **usar com seguranca recursos mutaveis compartilhados**. Nosso detector de vazamentos usara as versoes globais dos operadores de alocacao de memoria a fim de cobrir requisicoes de alocacao para todos os tipos, pelo menos a menos que os usuarios decidam usar versoes especializadas desses operadores, e como tal, precisaremos gerenciar o estado que sera global para nosso programa. Alem disso, sabemos que o codigo do usuario pode ser *multithreaded*, entao nossa contabilidade da memoria alocada exigira alguma forma de sincronizacao para evitar *data races*.
- A terceira sera **reconhecer o impacto do alinhamento ao contornar o sistema de tipos**. Como iremos lidar com memoria bruta para as necessidades a priori desconhecidas do codigo cliente, aprenderemos a fazer escolhas que funcionem para todos os casos de uso de alocacao de memoria "naturais" (no sentido de "nao sobrealinhados").
- Por fim, examinaremos como depurar nosso codigo com base no conteudo da memoria bruta. Como nosso objetivo e manter este livro agnóstico em relacao a ferramentas, aplicaremos uma abordagem esquematica a este problema, mas na pratica, voce devera adaptar o que fazemos neste capitulo as metaforas do seu utilitario de depuracao favorito.

Vamos la!

## O plano

Planejamos escrever um detector de vazamentos de memoria, uma tarefa que pode parecer estranha e abstrata a principio. Como comecar? Bem, uma forma de esclarecer o que precisamos fazer e escrever um pequeno programa de teste, mostrando de uma vez como esperamos que nossa ferramenta seja usada e destacando os aspectos-chave da ferramenta na perspectiva do codigo usuario:

```cpp
#include <iostream>
// incompleto por enquanto

int main() {
   auto anterior = // quantidade atual de memoria alocada
   { // INICIO
      int *p = new int{ 3 };
      int *q = new int[10]{ }; // inicializado com zero
      delete p;
     // ops! Esquecemos de fazer delete[] q
   } // FIM
   auto posterior = // quantidade atual de memoria alocada
   // com este codigo, supondo sizeof(int)==4, esperamos
   // ver "Vazou 40 bytes" impresso
   if (posterior != anterior)
      std::cout << "Vazou " << (posterior - anterior) << " bytes\n";
}
```

Como se ve, este programa "deliberadamente vazante" realiza duas alocacoes mas apenas uma desalocacao, "esquecendo" (convenientemente para nossos propositos) de desalocar um array de dez objetos `int`. Supondo `sizeof(int)==4`, nosso detector de vazamentos deve permitir que o programa reporte um vazamento de 40 bytes.

Este programa nao nos diz como obteremos (de forma portatil) a quantidade de memoria dinamicamente alocada em um determinado momento — escreveremos esse servico neste capitulo — mas mostra as alocacoes e desalocacoes localizadas entre um par de chaves (veja `INICIO` e `FIM` nos comentarios). Em C++, como se sabe, chaves correspondentes delimitam um escopo, e o escopo garante a destruicao de variaveis automaticas definidas nele. A ideia aqui e que queremos detectar vazamentos mesmo na presenca de objetos RAII (veja o Capitulo 4), pois eles tambem podem ter bugs, entao queremos garantir que sejam destruidos antes de tentarmos emitir um diagnostico.

Como mencionado na introducao deste capitulo, implementaremos nosso detector de vazamentos por meio da sobrecarga das formas globais dos operadores de alocacao de memoria. Como ja deve ter percebido, esses operadores precisarao compartilhar algum estado: no minimo, precisarao de conhecimento compartilhado sobre a quantidade de memoria alocada em um determinado momento, ja que os operadores `new` e `new[]` incrementarao esse valor e os operadores `delete` e `delete[]` o decrementarao.

Note que para nosso detector de vazamentos, as formas com array e sem array desses operadores serao identicas, mas isso nem sempre e o caso: pode-se imaginar estrategias diferentes para alocar escalares e arrays, assim como pode-se querer rastrear o que essas duas formas fazem separadamente em um programa. Para simplificar, neste capitulo muitas vezes mencionaremos apenas `new` para descrever tanto `new` quanto `new[]`, e usaremos a mesma abordagem para `delete`.

Como essas sao funcoes livres, nao funcoes membro de algum objeto, precisaremos recorrer a uma variavel global para esse estado. Variaveis globais sao frequentemente malvistas, na maioria das vezes por boas razoes, mas existem para casos como este.

> **Variaveis globais, nossa!**
>
> As razoes para nao gostar de variaveis globais sao abundantes: tornam o raciocinio local dificil (quem sabe onde e quando estao sendo acessadas?), tendem a ser gargalos para acesso ao cache e tornam programas mais lentos, tendem a exigir sincronizacao em programas contemporaneos (potencialmente *multithread*), e assim por diante. Estamos recorrendo a esse mecanismo aqui porque precisamos: o C++ nos fornece uma ampla variedade de ferramentas porque e uma linguagem usada para resolver uma ampla variedade de problemas, entao nao ha vergonha em usar essas ferramentas quando sao as ferramentas certas para a tarefa em questao. Apenas certifique-se de fazer escolhas informadas que voce possa justificar!

Para reduzir (ainda que levemente) a sensacao de repulsa aparente que variaveis globais provocam em muitos de nos, encapsularemos esse estado em um objeto, mas, claro, esse objeto tambem sera global.

Aplicaremos o padrao de projeto *singleton* (tambem execrado por programadores em geral) para esse fim, sendo um singleton uma classe da qual existe apenas uma instancia em um programa. O beneficio dessa abordagem e que controlaremos as formas pelas quais o estado global sera acessado, o que esperamos tambem ajudara a esclarecer o que estamos fazendo. Nomearemos nossa classe singleton `Contador` pois suas responsabilidades serao ajudar os operadores de alocacao de memoria a manter o controle do numero de bytes alocados e desalocados durante a execucao do programa.

> **Singletons, nossa!**
>
> No que diz respeito a padroes de projeto, o singleton e provavelmente um dos menos amados, por razoes semelhantes as que estao por tras da aversao a variaveis globais: dificil de testar ou simular, requer sincronizacao, tende a se tornar um gargalo de desempenho, e assim por diante. O verdadeiro culpado aqui, para ser honesto, e o estado mutavel compartilhado, agravado pelo fato de que esse estado e globalmente acessivel em todo o programa. Como ja deve ter adivinhado, como o estado mutavel compartilhado e exatamente o que precisamos para acompanhar a quantidade de memoria alocada em um determinado momento, bem... e isso que usaremos!

## Uma primeira implementacao (que quase funciona)

Agora temos um plano, entao estamos prontos para comecar a implementar a versao inicial do nosso detector de vazamentos. Esta implementacao sera um pouco ingenua mas nos ajudara a entender a ideia geral; examinaremos os aspectos mais delicados da implementacao uma vez que a infraestrutura basica esteja em vigor. **Nao use esta primeira versao em codigo de producao**, pois ela sera (levemente, porem perigosamente) incorreta. Forneceremos uma versao correta mais adiante neste capitulo.

Antes de continuar, tente identificar as "arestas brutas" de nossa implementacao por conta propria. Havera pistas deixadas aqui e ali para voce e, se leu os capitulos que precedem este, pode ja ter uma ideia do que deve procurar.

### A classe singleton Contador

Nossa classe `Contador` sera uma reificacao do padrao de projeto singleton cujo papel sera permitir que as sobrecargas globais dos operadores de alocacao de memoria mantenham o controle do numero de bytes de memoria dinamicamente alocada em um programa. Como mencionado anteriormente, um singleton e uma ideia: uma classe para a qual existe apenas uma instancia em um programa. Essa ideia pode ser reificada em varias linguagens (pelo menos aquelas que suportam alguma variante do paradigma orientado a objetos) de maneiras que respeitem as particularidades de cada linguagem.

Uma particularidade-chave do C++ e a presenca de objetos reais, nao apenas referencias a objetos, no codigo usuario. Isso significa que um singleton C++ normalmente tera as seguintes caracteristicas:

- Um construtor padrao privado, pois se esse construtor fosse publico, poderia ser chamado mais de uma vez, o que tornaria a classe um nao-singleton;
- Operacoes de copia deletadas, pois permitir copias do nosso objeto o tornaria um nao-singleton;
- Uma maneira de garantir que o singleton possa ser criado e, claro, acessado. Esse mecanismo deve ser tal que nao possa ser abusado para criar mais de um objeto. Como nosso construtor padrao sera privado, esse mecanismo sera uma funcao membro estatica (esta sera nossa escolha) ou uma funcao amiga;
- Finalmente, o estado usado para a representacao do objeto e os servicos oferecidos pelo singleton, se houver.

Um objeto de nossa classe `Contador` expora tres servicos: um para deixar os operadores `new` e `new[]` informarem o objeto `Contador` que memoria foi consumida, um para informar que memoria foi devolvida, e um para deixar o codigo cliente saber quanta memoria esta sendo usada em um determinado momento.

Uma visao incompleta da classe `Contador` com base no que discutimos ate agora seria a seguinte:

```cpp
#ifndef DETECTOR_VAZAMENTO_H
#define DETECTOR_VAZAMENTO_H
#include <cstddef>
#include <new>

class Contador {
   Contador(); // nota: privado
   // ...
public:
   // operacoes de copia deletadas
   Contador(const Contador&) = delete;
   Contador& operator=(const Contador&) = delete;

   // para acessar o objeto singleton
   static Contador& obter();

   // servicos oferecidos pelo objeto
   void consumir(std::size_t n);      // n bytes foram alocados
   void devolver(std::size_t n);      // n bytes foram desalocados
   std::size_t quanto_uso() const;    // bytes atualmente alocados
};

// operadores de alocacao (funcoes livres)
void *operator new(std::size_t);
void *operator new[](std::size_t);
void operator delete(void*) noexcept;
void operator delete[](void*) noexcept;

#endif
```

Com isso, ja podemos completar o esqueleto do nosso programa de teste apresentado anteriormente neste capitulo:

```cpp
#include "detector_vazamento.h"
#include <iostream>

int main() {
   auto anterior = Contador::obter().quanto_uso();
   { // INICIO
      int *p = new int{ 3 };
      int *q = new int[10]{ }; // inicializado com zero
      delete p;
     // ops! Esquecemos de fazer delete[] q
   } // FIM
   auto posterior = Contador::obter().quanto_uso();
   // supondo sizeof(int)==4, esperamos ver "Vazou 40 bytes"
   if (posterior != anterior)
      std::cout << "Vazou " << (posterior - anterior) << " bytes\n";
}
```

Agora precisamos examinar a implementacao da classe `Contador`. A primeira coisa que precisamos decidir e como e onde o objeto real sera criado. Acontece que existem surpreendentemente muitas maneiras de fazer isso, mas em nosso caso (onde nao nos preocupamos com velocidade de execucao), a maneira mais simples de instanciar corretamente o objeto e o que se chama de **Singleton de Meyers**, em homenagem ao agora aposentado, mas sempre respeitado, Scott Meyers, que sugeriu essa tecnica como parte do item 47 de seu bem conhecido livro *Effective C++*.

> **A tecnica do Singleton de Meyers**
>
> A tecnica do Singleton de Meyers visa evitar algo coloquialmente chamado de *static initialization order fiasco* (fiasco da ordem de inicializacao estatica), um nome informal dado ao fato de que em um programa C++ composto de multiplas unidades de traducao, nao se pode saber pelo codigo-fonte em que ordem os objetos globais serao construidos.
>
> O truque e declarar o objeto singleton como uma variavel local estatica na funcao membro estatica que fornece acesso ao objeto (aqui, a funcao `obter()`): fazer isso garante que o objeto sera criado apenas uma vez, na primeira vez que a funcao for chamada, e mantera seu estado durante toda a execucao do programa. Ha um custo pequeno, porem mensuravel, em fazer isso, pois ha uma forma de sincronizacao implicita de baixo nivel em torno da construcao do objeto para evitar que ele seja criado mais de uma vez mesmo em um programa *multithread*.
>
> Essa tecnica garante que todos esses singletons sejam criados na ordem correta (significando que, se o construtor do singleton A precisar de um servico do singleton B, isso levara ao singleton B sendo construido "exatamente a tempo") mesmo que sejam tecnicamente variaveis "globais", desde que nao haja ciclo nas chamadas que os criam.

Em termos de estado, como `consumir()` e `devolver()` ambas aceitam um argumento do tipo `std::size_t`, seria tentador representar a quantidade atual de memoria tambem como `std::size_t`, mas e recomendavel usar outra coisa. De fato, `std::size_t` e um alias para um tipo integral sem sinal, o que significaria que essa representacao tornaria dificil detectar um caso onde houve mais bytes desalocados do que alocados, uma situacao desagradavel que certamente gostariamos de tratar. Por essa razao, usaremos um inteiro com sinal (grande).

Pode-se pensar: "certo, podemos usar uma representacao `long long` entao!" Lembre-se, porem, que os mecanismos de alocacao e desalocacao de memoria precisam ser *thread-safe*, entao precisamos garantir que todos os acessos a essa representacao integral sejam sincronizados. Ha muitas maneiras de fazer isso, mas a mais simples e provavelmente usar um tipo atomico, em nosso caso, `std::atomic<long long>`. Note que objetos atomicos sao nao-copiáveis, entao nosso singleton seria implicitamente nao-copiavel, mas nao ha mal em declarar esse fato explicitamente como fizemos ao deletar as operacoes de copia.

Uma implementacao completa da classe `Contador` seria a seguinte:

```cpp
#ifndef DETECTOR_VAZAMENTO_H
#define DETECTOR_VAZAMENTO_H
#include <cstddef>
#include <atomic>
#include <new>

class Contador {
   std::atomic<long long> atual;      // bytes atualmente em uso
   Contador() : atual{ 0LL } { }      // privado

public:
   // operacoes de copia deletadas
   Contador(const Contador&) = delete;
   Contador& operator=(const Contador&) = delete;

   // acessa o objeto singleton (Singleton de Meyers)
   static auto& obter() {
      static Contador singleton;     // criado apenas uma vez
      return singleton;
   }

   void consumir(std::size_t n) { atual += n; }   // n bytes alocados
   void devolver(std::size_t n) { atual -= n; }   // n bytes liberados
   std::size_t quanto_uso() const { return atual.load(); } // estado atual
};

// operadores de alocacao (funcoes livres)
void *operator new(std::size_t);
void *operator new[](std::size_t);
void operator delete(void*) noexcept;
void operator delete[](void*) noexcept;

#endif
```

Os servicos sao provavelmente triviais de entender em sua maior parte. Como `atual` e um objeto atomico, operacoes como `+=` ou `-=` modificam `atual` de maneira sincronizada, evitando *data races*. Dois aspectos sutis de `quanto_uso()` merecem uma breve discussao:

- O primeiro e que estamos retornando `atual.load()`, nao `atual`, pois nos importamos com o valor representado pelo objeto atomico, nao o objeto atomico em si (que e um mecanismo de sincronizacao, nao um valor integral, e e nao-copiavel como mencionado anteriormente). E como tirar uma foto daquele valor em um momento especifico.
- O segundo, uma consequencia do primeiro, e que no momento em que o codigo cliente obtem o valor retornado por essa funcao, o valor real pode ter mudado, entao essa funcao e inerentemente "sujeita a corrida de dados" se usada em uma situacao *multithread*. Nao e um problema para nosso codigo de teste, claro, mas e algo a ter em mente.

### Implementando os operadores new e new[]

Se voce se lembra de nosso plano, o que faremos em nossos operadores de alocacao de memoria e pegar o numero de bytes, `n`, solicitado pelo codigo cliente, e entao alocar um pouco mais porque vamos esconder `n` logo antes do inicio do bloco de `n` bytes que acabaremos devolvendo ao nosso cliente. Minimamente, precisaremos alocar `n + sizeof n` bytes para isso.

Uma implementacao completa porem ingenua (e levemente incorreta, como anunciado) seria a seguinte:

```cpp
#include <cstdlib>

// aloca n bytes + espaco para esconder n
void *operator new(std::size_t n) {
   void *p = std::malloc(n + sizeof n);      // a revisar
   if (!p) throw std::bad_alloc{};
   auto q = static_cast<std::size_t*>(p);
   *q = n;                                   // esconde n no inicio do bloco
   Contador::obter().consumir(n);            // informa n bytes alocados
   return q + 1;                             // a revisar
}

void *operator new[](std::size_t n) {
   // identico ao operator new acima
}
```

Lembre-se de que mesmo que `operator new()` e `operator new[]()` sejam identicos neste exemplo, nao ha obrigacao de os tornar iguais em todas as situacoes.

### Implementando os operadores delete e delete[]

Nossos operadores de desalocacao colaborarao com a mentira elaborada pelos operadores de alocacao: sabemos que os operadores `new` e `new[]` retornam ponteiros para um bloco de `n` bytes, mas esse bloco nao e o que foi realmente alocado — e "apenas" o local onde um objeto viveu por um tempo. Por essa razao, e importante que os operadores `delete` e `delete[]` façam o ajuste de endereco necessario antes de executar a desalocacao real.

As regras para uma implementacao correta de `operator delete` sao as seguintes:

- Aplicar `operator delete()` ou `operator delete[]()` em um ponteiro nulo e um *no-op*;
- Funcoes de desalocacao nao devem lançar excecoes;
- O codigo de desalocacao deve ser coerente com a funcao de alocacao associada.

> **Nem todos os ponteiros nulos sao iguais**
>
> Embora seja verdade que dado algum objeto `T*` chamado `p`, escrever `delete p` ou `delete [] p` sera um *no-op* se `p==nullptr`, escrever `delete nullptr` falhara ao compilar pois `nullptr` e um objeto do tipo `std::nullptr_t`, nao um ponteiro.

Dada a implementacao de nossos operadores de alocacao, um operador de desalocacao inicialmente adequado poderia ser o seguinte:

```cpp
void operator delete(void *p) noexcept {
   if (!p) return;                                  // no-op se nulo
   auto q = static_cast<std::size_t*>(p) - 1;      // retrocede para onde n foi escondido
   Contador::obter().devolver(*q);                  // informa bytes liberados
   std::free(q);                                    // libera o bloco real
}

void operator delete[](void *p) noexcept {
   // identico ao operator delete acima
}
```

Isso completa a mentira e conclui o detector de vazamentos, pelo menos para esta primeira implementacao (imperfeita). Se voce rodar o programa de teste com nossa implementacao em um compilador onde `sizeof(int)==4`, pode esperar que ele exiba que sua execucao vazou 40 bytes, como esperado.

### Visualizando tudo

Ao trabalhar com programacao de baixo nivel como esta — assumir o controle das funcoes de alocacao de memoria do seu programa, manipular blocos de memoria bruta, esconder informacoes e brincar com enderecos — pode ser dificil visualizar o que se esta fazendo.

Vamos percorrer uma chamada a `operator new()` passo a passo. A primeira coisa que fazemos e perguntar ao `Contador` a quantidade de memoria dinamicamente alocada no inicio da funcao `main()`:

```cpp
int main() {
   auto anterior = Contador::obter().quanto_uso(); // provavelmente 0
   { // INICIO
      int *p = new int{ 3 };
      int *q = new int[10]{ };
      delete p;
     // ops! Esquecemos de fazer delete[] q
   } // FIM
   auto posterior = Contador::obter().quanto_uso();
   if (posterior != anterior)
      std::cout << "Vazou " << (posterior - anterior) << " bytes\n";
}
```

Pode-se esperar que `anterior==0` nesse ponto, mas ha situacoes, como um objeto global chamando `new` em seu construtor, que poderiam levar `anterior` a ter outros valores. Isso e aceitavel, pois o que monitoramos com essa abordagem e se ha um vazamento entre as chaves marcadas com `INICIO` e `FIM`, o que deve valer independentemente de a quantidade de bytes alocados fora dessas chaves ser zero ou nao.

O proximo passo e chamar `operator new()` e solicitar um bloco de memoria grande o suficiente para armazenar um objeto `int`. Isso nos leva a nossa implementacao de `operator new()` onde `n==sizeof(int)`. Supondo `sizeof(int)==4` e `sizeof(std::size_t)==8`, nossa chamada a `std::malloc()` solicitara um bloco de pelo menos 12 bytes.

Dentro do `operator new()`, realizamos a primeira "mentira": alocamos mais do que foi solicitado. Em seguida, escondemos o valor `n` no inicio do bloco alocado. Apos informar o `Contador` que 4 bytes foram alocados (nao 12, lembre-se), retornamos ao chamador o inicio do bloco de 4 bytes que foi realmente solicitado — um ponteiro `sizeof(std::size_t)` bytes alem do inicio real do bloco alocado.

Na perspectiva do codigo cliente, `p` aponta para um bloco de `sizeof(int)` bytes onde um objeto `int` sera construido. O `int` e um tipo de tempo de vida implicito, entao seu construtor essencialmente preenche o bloco com o valor `3`.

Quando entramos em `operator delete()`, o argumento `p` aponta para o inicio do bloco de 4 bytes dado ao codigo cliente — nao para o inicio do bloco que realmente alocamos. Nosso primeiro trabalho dentro de `operator delete()` e retroceder `sizeof(std::size_t)` bytes para recuperar o valor de `n` que escondemos la:

```cpp
void operator delete(void *p) noexcept {
   if (!p) return;
   auto q = static_cast<std::size_t*>(p) - 1;  // retrocede para o n escondido
   Contador::obter().devolver(*q);              // recupera n e informa liberacao
   std::free(q);                               // libera o bloco real a partir de q
}
```

Nesse ponto, `q` e o local onde o valor de `n` foi armazenado, bem como o inicio do bloco de memoria que foi alocado. Informamos o `Contador` do fato de que `n` bytes foram desalocados e chamamos `std::free()` para realizar a desalocacao real.

## Identificando (e corrigindo) os problemas

Nossa implementacao inicial tem de fato um problema real, alem de algo que funciona mas poderia ser mais limpo e merece discussao.

O problema real e que expressamos nossas "mentiras" de maneiras perigosas e que nao estamos dando a devida consideracao aos requisitos de alinhamento. De fato, veja nossa implementacao inicial de `operator new()`:

```cpp
void *operator new(std::size_t n) {
   void *p = std::malloc(n + sizeof n);     // a revisar
   if (!p) throw std::bad_alloc{};
   auto q = static_cast<std::size_t*>(p);
   *q = n;                                  // a revisar
   Contador::obter().consumir(n);
   return q + 1;                            // a revisar
}
```

Sabemos que a memoria retornada por `std::malloc()` deve ser alinhada apropriadamente para o alinhamento natural mais restrito de nossa maquina: como essa funcao nao sabe que objeto sera construido apos a alocacao ser concluida, ela deve garantir que o bloco de memoria alocado sera adequadamente alinhado em todos os casos "naturais". Os compiladores C++ oferecem `std::max_align_t` como um alias para o tipo com o alinhamento natural mais restrito em uma maquina, que geralmente, mas nao necessariamente, e o tipo `double` na pratica.

Agora, alocamos um pouco mais do que foi solicitado: precisamente `sizeof(std::size_t)` bytes a mais. Ate certo ponto, isso e aceitavel: podemos armazenar `std::size_t` com seguranca no inicio do bloco retornado por `std::malloc()`, pois esse bloco esta bem alinhado mesmo no pior caso.

Entao "pulamos sobre" `std::size_t` e retornamos um endereco que e `sizeof(std::size_t)` bytes alem do que alocamos. Isso pode ser aceitavel se ainda produzir um endereco corretamente alinhado mesmo no pior caso, mas isso so e verdade se `std::size_t` e `std::max_align_t` tiverem o mesmo tamanho — algo que nao e garantido (na pratica, seus tamanhos sao frequentemente diferentes).

O que acontece se esses tipos tiverem tamanhos diferentes e, consequentemente, o endereco retornado por `operator new()` nao corresponder aos requisitos de alinhamento de `std::max_align_t`? Bem, depende:

- **Pode funcionar por sorte** se o endereco retornado estiver alinhado corretamente para o tipo que queremos construir. Por exemplo, supondo que `alignof(int)==4` e `alignof(std::max_align_t)==8`, chamar `new int` funcionara mesmo se `operator new` retornar um endereco multiplo de quatro mas nao de oito. No entanto, e provavel que chamar `new double` leve apenas a dor. Esse tipo de "sorte" pode ser uma maldição, escondendo um bug latente e prejudicial por um tempo e causando surpresas desagradaveis mais tarde.
- **Pode-se obter codigo lento e perigoso**, pois alguns hardwares suportam acesso a objetos desalinhados. Nao se deve fazer isso, pois para a maquina conseguir isso, ela precisa realizar acrobacias e transformar uma operacao aparentemente simples como carregar um `double` em um registrador em uma sequencia de operacoes (carregar os bytes "baixos", carregar os bytes "altos" e criar um `double` a partir dessas duas partes por meio de manipulacoes de bits). Isso leva a um codigo significativamente mais lento para executar, obviamente, mas tambem perigoso em programas *multithread* pois uma thread pode ler um objeto parcialmente formado (*torn read*) ou escrever em um objeto parcialmente formado (*torn write*).
- **Seu codigo pode simplesmente travar**, como sera o caso em muitas plataformas embarcadas (incluindo varios consoles de jogos). E sem duvida o resultado mais razoavel em tal situacao.

Para corrigir esse problema, precisamos garantir que o endereco retornado por nosso `operator new()` sobrecarregado esteja corretamente alinhado para `std::max_align_t`, e que `operator delete()` seja ajustado adequadamente. Uma maneira de fazer isso e garantir que o tamanho do "esconderijo" para `n` seja tal que pular sobre esse bloco de memoria adicional ainda resulte em um endereco corretamente alinhado para um objeto `std::max_align_t`:

```cpp
void *operator new(std::size_t n) {
   // aloca n bytes + espaco para esconder n,
   // levando em conta o pior alinhamento natural
   void *p = std::malloc(sizeof(std::max_align_t) + n);
   if (!p) throw std::bad_alloc{};
   // esconde n no inicio do bloco alocado
   *static_cast<std::size_t*>(p) = n;   // a revisar ainda
   Contador::obter().consumir(n);
   // retorna inicio do bloco solicitado, corretamente alinhado
   return static_cast<std::max_align_t*>(p) + 1;
}
```

Como se pode ver, esta implementacao aloca espaco para `std::max_align_t` alem dos `n` bytes solicitados, e entao "pula sobre" esse armazenamento adicional para produzir um endereco que ainda esta corretamente alinhado para o pior caso. Isso pode significar desperdicar mais espaco do que na implementacao inicial (incorreta) se `sizeof(std::size_t)` for menor que `sizeof(std::max_align_t)`, mas pelo menos sabemos que o codigo cliente podera construir seu objeto la.

O `operator delete()` correspondente fara a mesma ginastica de ponteiros, mas ao contrario, voltando `sizeof(std::max_align_t)` bytes na memoria:

```cpp
void operator delete(void *p) noexcept {
   if (!p) return;
   // encontra o inicio do bloco que foi alocado
   p = static_cast<std::max_align_t*>(p) - 1;
   // informa o Contador da desalocacao
   Contador::obter().devolver(
      *static_cast<std::size_t*>(p)
   );
   // libera a memoria
   std::free(p);
}
```

Note que esta implementacao atribui `std::max_align_t*` a `void*` (ponteiro `p`), algo que e perfeitamente legal e nao requer um *cast*.

A outra questao que devemos discutir nao e tecnicamente um problema nesta implementacao, mas e um problema em geral. Veja este trecho de `operator new()`:

```cpp
void *operator new(std::size_t n) {
   void *p = std::malloc(n + sizeof(std::max_align_t));
   if (!p) throw std::bad_alloc{};
   *static_cast<std::size_t*>(p) = n;   // atribuicao a "objeto inexistente"?
   Contador::obter().consumir(n);
   return static_cast<std::max_align_t*>(p) + 1;
}
```

Voce nota algo estranho? A linha destacada realiza uma atribuicao onde `p` aponta, mas a atribuicao so faz sentido em um objeto existente. Existe um objeto no local `*p` naquele momento?

A resposta e... estranha. Para criar um objeto, e preciso chamar seu construtor, mas nunca chamamos o construtor de `std::size_t` no local `p` neste codigo. Isso pode fazer voce se perguntar por que nosso codigo parece funcionar. Acontece que o seguinte e verdade:

- **Alguns tipos em C++ sao ditos tipos de tempo de vida implicito (*implicit lifetime types*).** Esses tipos incluem escalares (ponteiros, ponteiros para membros, tipos aritmeticos, enumeracoes, `std::nullptr_t` incluindo seus equivalentes *cv-qualified*) e classes de tempo de vida implicito (agregados sem destrutor fornecido pelo usuario, pelo menos um construtor trivial elegivel e um destrutor trivial nao deletado). Voce notara que `std::size_t`, sendo um alias para um tipo integral sem sinal, se enquadra na categoria de tipos de tempo de vida implicito. Se voce tiver um compilador C++23, pode testar programaticamente se algum tipo `T` se qualifica como tipo de tempo de vida implicito atraves da trait `std::is_implicit_lifetime<T>`.
- **Algumas funcoes da biblioteca padrao iniciam implicitamente o tempo de vida de objetos de tipos de tempo de vida implicito.** Isso inclui algumas funcoes C, como `std::memcpy()`, `std::memmove()` e `std::malloc()`, mas tambem `std::bit_cast`, algumas funcoes em alocadores (veja o Capitulo 14) e duas funcoes do C++23 chamadas respectivamente `std::start_lifetime_as()` e `std::start_lifetime_as_array()`.

O que faz essa operacao de atribuicao funcionar neste caso especifico e que estamos escrevendo em um objeto de um tipo de tempo de vida implicito em um bloco de memoria que esta corretamente alinhado e foi alocado com uma dessas funcoes especiais que tem a propriedade de iniciar implicitamente o tempo de vida de objetos.

Uma abordagem melhor e, em geral, mais segura para o ato de esconder o valor de `n` em um armazenamento nao inicializado e usar *placement new*, como visto no Capitulo 7. A seguinte implementacao de `operator new()` e, portanto, preferivel em geral, pois evita uma atribuicao (muitas vezes equivocada) a um nao-objeto:

```cpp
void *operator new(std::size_t n) {
   void *p = std::malloc(n + sizeof(std::max_align_t));
   if (!p) throw std::bad_alloc{};
   // usa placement new para esconder n corretamente
   new (p) std::size_t{ n };
   Contador::obter().consumir(n);
   return static_cast<std::max_align_t*>(p) + 1;
}
```

Note que, como `std::size_t` tem um destrutor trivial, nao ha necessidade de chamar seu destrutor em `operator delete()`; simplesmente liberar o armazenamento subjacente e suficiente para encerrar seu tempo de vida. Agora temos um detector de vazamentos correto e funcional!

## Revisitando nossa implementacao (e licoes aprendidas)

Acabamos de sobrecarregar os operadores de alocacao de memoria, mentimos descaradamente pelas protecoes do sistema de tipos, realizamos operacoes potencialmente perigosas que arriscavam levar a objetos desalinhados e vimos como evitar essa armadilha. Foi uma aventura interessante, mas o leitor astuto que voce e esta provavelmente se perguntando sobre o custo desse truque, particularmente em termos de quanta memoria ele consome.

Com nossa abordagem de "alocar mais do que solicitado e esconder `n` no inicio", cada alocacao consume `sizeof(std::max_align_t)` bytes a mais do que o necessario para o codigo cliente. Se nosso codigo alocar objetos grandes, esse custo pode ser menor, mas se alocarmos objetos menores, essa sobrecarga pode ser irrazoavel e dominar o consumo de memoria de todo o nosso programa.

Lembre-se do Capitulo 7 que o C++14 tornou possivel fornecer uma sobrecarga de `operator delete()` que aceita o tamanho do objeto recém-destruido como argumento. Isso torna o ato de esconder `n` durante `operator new()` redundante, pois fizemos isso precisamente para recuperar `n` em `operator delete()` — algo que nao precisamos mais fazer.

Como nao precisamos esconder `n`, podemos simplificar nossa implementacao e reduzir significativamente nosso consumo de memoria:

```cpp
void *operator new(std::size_t n) {
   void *p = std::malloc(n);             // nao precisa alocar mais!
   if (!p) throw std::bad_alloc{};
   Contador::obter().consumir(n);
   return p;
}

void *operator new[](std::size_t n) {
   // identico ao operator new acima
}

// versao ciente do tamanho (C++14): recebe n diretamente
void operator delete(void *p, std::size_t n) noexcept {
   if (!p) return;
   Contador::obter().devolver(n);
   std::free(p);
}

void operator delete[](void *p, std::size_t n) noexcept {
   // identico ao operator delete acima
}
```

Este detector de vazamentos ainda funciona e representa uma melhoria estrita em comparacao com a versao mais ingenua que o precedeu.

## Resumo

Foi divertido, nao foi? Voce pode pegar essa ferramenta muito simples e torna-la mais interessante: por exemplo, pode usa-la para verificar *overflow* e *underflow* dos blocos de memoria alocados injetando valores-sentinela antes e depois de cada bloco, ou pode usa-la para fazer uma especie de mapa de como sua memoria esta sendo usada.

Isso conclui nossa primeira incursao em aplicacoes que se beneficiam de assumir o controle das facilidades de alocacao de memoria a nossa disposicao. Nosso proximo passo, e proximo capitulo, nos levara a examinar como um programa C++ pode interagir com memoria atipica ou lidar com situacoes de alocacao atipicas.

E claro, nenhuma linguagem de programacao (mesmo uma tao versatil e abrangente quanto o C++) pode afirmar cobrir todos os tipos possiveis de memoria que um sistema operacional poderia fornecer servicos, nem deveria ser esse o papel da linguagem. Ainda assim, como veremos, o C++ nos fornece o tipo de "cola sintatica" necessaria para construir pontes entre necessidades atipicas e o restante do programa.
