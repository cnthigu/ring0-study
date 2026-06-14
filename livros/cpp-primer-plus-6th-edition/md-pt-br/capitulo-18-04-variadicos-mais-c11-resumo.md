# Capítulo 18 — Visitando o Novo Padrão C++ (parte 4)

> Tradução não oficial de *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Navegação: [Anterior](capitulo-18-03-lambdas-wrappers-variadicos.md) | [Índice](README.md)

## Templates Variádicos (Variadic Templates)

Templates variádicos fornecem um meio para criar funções template e classes template que aceitam um número variável de argumentos. Vamos examinar as funções template variádicas aqui. Por exemplo, suponha que você queira uma função que aceite qualquer número de parâmetros de qualquer tipo, desde que o tipo possa ser exibido com `cout`, e exiba os argumentos como uma lista separada por vírgulas. Por exemplo, considere este código:

```cpp
int n = 14;
double x = 2.71828;
std::string mr = "Objetos Mr. String!";
exibir_lista(n, x);
exibir_lista(x*x, '!', 7, mr);
```

O objetivo é definir `exibir_lista()` de tal forma que esse código compile e leve a esta saída:

```
14, 2.71828
7.38905, !, 7, Objetos Mr. String!
```

Há alguns pontos-chave para entender a fim de criar templates variádicos:

- Pacotes de parâmetros de template (template parameter packs)
- Pacotes de parâmetros de função (function parameter packs)
- Desempacotamento de um pacote (unpacking a pack)
- Recursão

### Pacotes de Parâmetros de Template e de Função

Como ponto de partida para ver como funcionam os pacotes de parâmetros, consideremos uma função template simples, que exibe uma lista consistindo de apenas um item:

```cpp
template<typename T>
void exibir_lista0(T valor)
{
    std::cout << valor << ", ";
}
```

Esta definição tem duas listas de parâmetros. A lista de parâmetros de template é apenas `T`. A lista de parâmetros de função é apenas `valor`. Uma chamada de função como a seguinte define `T` na lista de parâmetros de template como `double` e `valor` na lista de parâmetros de função como `2.15`:

```cpp
exibir_lista0(2.15);
```

O C++11 fornece um meta-operador de reticências (ellipsis) que permite declarar um identificador para um pacote de parâmetros de template, essencialmente uma lista de tipos. Da mesma forma, permite declarar um identificador para um pacote de parâmetros de função, essencialmente uma lista de valores. A sintaxe parece com esta:

```cpp
template<typename... Args> // Args é um pacote de parâmetros de template
void exibir_lista1(Args... args) // args é um pacote de parâmetros de função
{
    // ...
}
```

`Args` é um pacote de parâmetros de template, e `args` é um pacote de parâmetros de função. A diferença entre `Args` e `T` é que `T` corresponde a um único tipo, enquanto `Args` corresponde a qualquer número de tipos, incluindo nenhum. Considere a seguinte chamada de função:

```cpp
exibir_lista1('S', 80, "doce", 4.5);
```

Nesse caso, o pacote de parâmetros `Args` contém os tipos correspondentes aos parâmetros na chamada de função: `char`, `int`, `const char *` e `double`.

### Desempacotando os Pacotes

Mas como a função pode acessar os conteúdos desses pacotes? Não há recurso de indexação. Ou seja, você não pode usar algo como `Args[2]` para acessar o terceiro tipo em um pacote. Em vez disso, você pode desempacotar o pacote colocando as reticências à direita do nome do pacote de parâmetros de função. Por exemplo, considere o seguinte código falho:

```cpp
template<typename... Args>
void exibir_lista1(Args... args)
{
    exibir_lista1(args...); // passa args desempacotados para exibir_lista1()
}
```

O que isso significa e por que é falho? Suponha que tenhamos esta chamada de função:

```cpp
exibir_lista1(5, 'L', 0.5);
```

A chamada empacota os valores `5`, `'L'` e `0.5` em `args`. Dentro da função, a chamada `exibir_lista1(args...)` se expande para:

```cpp
exibir_lista1(5, 'L', 0.5);
```

Ou seja, a entidade única `args` é substituída pelos três valores armazenados no pacote. Assim, a notação `args...` se expande para uma lista de argumentos de função discretos. Infelizmente, a nova chamada é a mesma que a chamada de função original, portanto ela se chamará novamente com os mesmos argumentos, iniciando uma recursão infinita e inútil. (Essa seria a falha.)

### Usando Recursão em Funções Template Variádicas

Embora a recursão condene as aspirações de `exibir_lista1()` de ser uma função útil, a recursão usada adequadamente fornece uma solução para acessar itens de pacote. A ideia central é desempacotar o pacote de parâmetros de função, processar o primeiro item na lista, depois passar o restante da lista para uma chamada recursiva e assim por diante, até que a lista esteja vazia. Como de costume com recursão, é importante garantir que haja uma chamada que encerre a recursão. Parte do truque envolve mudar o cabeçalho do template para este:

```cpp
template<typename T, typename... Args>
void exibir_lista3(T valor, Args... args)
```

Com essa definição, o primeiro argumento para `exibir_lista3()` é captado como tipo `T` e atribuído a `valor`. Os argumentos restantes são captados por `Args` e `args`. Isso permite que a função faça algo com `valor`, como exibi-lo. Em seguida, os argumentos restantes, na forma `args...`, podem ser passados para uma chamada recursiva de `exibir_lista3()`. Cada chamada recursiva imprime um valor e passa uma lista encurtada até a lista se esgotar.

A Listagem 18.9 apresenta uma implementação que, embora não seja perfeita, ilustra a técnica.

**Listagem 18.9 — `variadico1.cpp`**

```cpp
// variadico1.cpp -- usando recursão para desempacotar um pacote de parâmetros
#include <iostream>
#include <string>

// definição para 0 parâmetros -- chamada terminadora
void exibir_lista3() {}
// definição para 1 ou mais parâmetros
template<typename T, typename... Args>
void exibir_lista3(T valor, Args... args)
{
    std::cout << valor << ", ";
    exibir_lista3(args...);
}
int main()
{
    int n = 14;
    double x = 2.71828;
    std::string mr = "Objetos Mr. String!";
    exibir_lista3(n, x);
    exibir_lista3(x*x, '!', 7, mr);
    return 0;
}
```

Considere esta chamada de função:

```cpp
exibir_lista3(x*x, '!', 7, mr);
```

O primeiro argumento corresponde `T` a `double` e `valor` a `x*x`. Os três tipos restantes (`char`, `int` e `std::string`) são colocados no pacote `Args`, e os três valores restantes (`'!'`, `7` e `mr`) são colocados no pacote `args`.

Em seguida, a função `exibir_lista3()` usa `cout` para exibir `valor` (aproximadamente `7.38905`) e a string `", "`. Isso cuida de exibir o primeiro item da lista.

Em seguida vem esta chamada:

```cpp
exibir_lista3(args...);
```

Isso, dada a expansão de `args...`, é o mesmo que o seguinte:

```cpp
exibir_lista3('!', 7, mr);
```

Como prometido, a lista é encurtada de um item. Dessa vez `T` e `valor` tornam-se `char` e `'!'`, e os dois tipos e valores restantes são empacotados em `Args` e `args`, respectivamente. A próxima chamada recursiva processa esses pacotes reduzidos. Por fim, quando `args` está vazio, a versão de `exibir_lista3()` sem argumentos é chamada, e o processo termina.

Eis a saída para as duas chamadas de função:

```
14, 2.71828, 7.38905, !, 7, Objetos Mr. String!,
```

### Melhorias

Podemos melhorar `exibir_lista3()` com algumas mudanças. Como está, a função exibe uma vírgula após cada item da lista, mas seria melhor omitir a vírgula após o último item. Isso pode ser feito adicionando um template para apenas um item e fazendo-o se comportar ligeiramente diferente do template geral:

```cpp
// definição para 1 parâmetro
template<typename T>
void exibir_lista3(T valor)
{
    std::cout << valor << '\n';
}
```

Assim, quando o pacote `args` é reduzido a um item, essa versão é chamada, e ela imprime uma nova linha em vez de uma vírgula. Como ela não tem uma chamada recursiva a `exibir_lista3()`, também encerra a recursão.

A segunda área para melhoria é que a versão atual passa tudo por valor. Isso está bem para os tipos simples que usamos, mas é ineficiente para classes de grande tamanho que poderiam ser printáveis por `cout`. Seria melhor usar referências `const`. Com templates variádicos, você pode impor um padrão no desempacotamento. Em vez de usar:

```cpp
exibir_lista3(Args... args);
```

você pode usar isto:

```cpp
exibir_lista3(const Args&... args);
```

Isso fará com que cada parâmetro de função tenha o padrão `const &` aplicado.

A Listagem 18.10 incorpora essas duas mudanças.

**Listagem 18.10 — `variadico2.cpp`**

```cpp
// variadico2.cpp
#include <iostream>
#include <string>

// definição para 0 parâmetros
void exibir_lista() {}

// definição para 1 parâmetro
template<typename T>
void exibir_lista(const T& valor)
{
    std::cout << valor << '\n';
}

// definição para 2 ou mais parâmetros
template<typename T, typename... Args>
void exibir_lista(const T& valor, const Args&... args)
{
    std::cout << valor << ", ";
    exibir_lista(args...);
}
int main()
{
    int n = 14;
    double x = 2.71828;
    std::string mr = "Objetos Mr. String!";
    exibir_lista(n, x);
    exibir_lista(x*x, '!', 7, mr);
    return 0;
}
```

Eis a saída:

```
14, 2.71828
7.38905, !, 7, Objetos Mr. String!
```

---

## Mais Funcionalidades do C++11

O C++11 adiciona muitas mais funcionalidades do que este livro pode cobrir, mesmo ignorando o fato de que muitas delas não estavam amplamente implementadas quando este livro foi escrito. Ainda assim, vale a pena dar uma olhada rápida na natureza de algumas dessas funcionalidades.

### Programação Concorrente

Nos dias de hoje, é mais fácil melhorar o desempenho do computador adicionando processadores do que aumentando a velocidade do processador. Portanto, computadores com processadores dual-core ou quad-core, ou mesmo com múltiplos processadores multicore, são a norma. Isso permite que os computadores executem múltiplas threads de execução simultaneamente.

Algumas atividades podem se beneficiar de múltiplas threads, enquanto outras não. Considere pesquisar algo em uma lista simplesmente encadeada. Um programa tem que começar no início e seguir os links, em ordem, até o final da lista; não há muito que uma segunda thread possa fazer para ajudar. Agora considere um array não ordenado. Usando o recurso de acesso aleatório de arrays, você poderia iniciar uma thread do início do array e outra do meio, reduzindo assim o tempo de pesquisa à metade.

Múltiplas threads levantam muitos problemas. O que acontece se uma thread travar ou se duas threads tentarem acessar os mesmos dados simultaneamente? O C++11 aborda a concorrência definindo um modelo de memória que suporta execução em thread, adicionando a palavra-chave `thread_local` e fornecendo suporte de biblioteca. A palavra-chave `thread_local` é usada para declarar variáveis com duração de armazenamento estático relativa a uma determinada thread; ou seja, elas expiram quando a thread em que são definidas expira.

O suporte de biblioteca consiste na biblioteca de operações atômicas, que especifica o arquivo de cabeçalho `atomic`, e na biblioteca de suporte a threads, que especifica os arquivos de cabeçalho `thread`, `mutex`, `condition_variable` e `future`.

### Adições de Biblioteca

O C++11 adiciona várias bibliotecas especializadas. Uma biblioteca de números aleatórios extensível, suportada pelo arquivo de cabeçalho `random`, fornece facilidades de números aleatórios muito mais extensas e sofisticadas do que `rand()`. Por exemplo, ela oferece uma escolha de geradores de números aleatórios e uma escolha de distribuições, incluindo uniforme (como `rand()`), binomial, normal e várias outras.

O arquivo de cabeçalho `chrono` suporta maneiras de lidar com durações de tempo.

O arquivo de cabeçalho `tuple` suporta o template `tuple`. Um objeto `tuple` é uma generalização de um objeto `pair`. Enquanto um objeto `pair` pode conter dois valores cujos tipos não precisam ser os mesmos, um `tuple` pode conter um número arbitrário de itens de tipos diferentes.

A biblioteca de aritmética racional em tempo de compilação, suportada pelo arquivo de cabeçalho `ratio`, permite a representação exata de qualquer número racional cujo numerador e denominador possam ser representados pelo tipo inteiro mais amplo.

Uma das adições mais interessantes é uma biblioteca de expressões regulares, suportada pelo arquivo de cabeçalho `regex`. Uma expressão regular especifica um padrão que pode ser usado para corresponder conteúdos em uma string de texto.

### Programação de Baixo Nível

O "baixo nível" na programação de baixo nível refere-se ao nível de abstração, não à qualidade da programação. Baixo nível significa mais próximo dos bits e bytes do hardware do computador e da linguagem de máquina. A programação de baixo nível é importante para programação embarcada e para aumentar a eficiência de algumas operações. O C++11 oferece algumas ajudas para aqueles que fazem programação de baixo nível.

Uma mudança é relaxar as restrições sobre o que se qualifica como "Dados Velhos Simples" ou POD (Plain Old Data). No C++98, um POD é um tipo escalar (um tipo de valor único, como `int` ou `double`) ou uma estrutura de estilo antigo sem construtores, classes base, dados privados, funções virtuais e assim por diante. A ideia era que um POD é algo para o qual é seguro fazer uma cópia byte a byte. Isso ainda é a ideia, mas o C++11 reconhece que se pode remover algumas das restrições do C++98 e ainda ter um POD viável.

Outra mudança é tornar as uniões mais flexíveis, permitindo que tenham membros com construtores e destruidores, mas com algumas restrições em outras propriedades, por exemplo, não permitindo funções virtuais.

O C++11 aborda questões de alinhamento de memória. O operador `alignof()` fornece informações sobre os requisitos de alinhamento para um tipo ou objeto. O especificador `alignas` fornece algum controle sobre o alinhamento usado.

O mecanismo `constexpr` expande a capacidade do compilador de avaliar durante o tempo de compilação expressões que avaliam para um valor constante.

### Miscelânea

O C++11 segue o exemplo do C99 ao permitir tipos inteiros estendidos dependentes de implementação. Por exemplo, tais tipos poderiam ser usados em um sistema com inteiros de 128 bits.

O C++11 fornece um mecanismo, o operador literal (literal operator), para criar literais definidos pelo usuário. Usando esse mecanismo, por exemplo, é possível definir literais binários, como `1001001b`, que o operador literal correspondente converterá para um valor inteiro.

O C++ tem uma ferramenta de depuração chamada `assert`. É uma macro que verifica durante o tempo de execução se uma afirmação é verdadeira e que exibe uma mensagem e chama `abort()` se a afirmação for falsa. O C++11 adiciona a palavra-chave `static_assert`, que pode ser usada para testar afirmações durante o tempo de compilação.

O C++11 fornece mais suporte para metaprogramação, que é criar programas que criam ou modificam outros programas ou até a si mesmos.

---

## Mudança de Linguagem

Como uma linguagem de computador cresce e evolui? Depois que o uso do C++ se tornou suficientemente difundido, a necessidade de um padrão internacional ficou clara, e o controle da linguagem essencialmente passou para um comitê de padrões — primeiro o comitê ANSI, depois os comitês conjuntos ISO/ANSI, e atualmente para ISO/IEC JTC1/SC22/WG21 (o Comitê de Padrões C++).

O comitê considera relatórios de defeitos e propostas de mudanças e extensões à linguagem, e tenta chegar a um consenso.

Mas com o C++ há uma segunda via para mudança: ação direta pela criativa comunidade de programadores C++. Os programadores não podem mudar independentemente a linguagem, mas podem criar bibliotecas úteis. Bibliotecas bem projetadas podem estender a utilidade e a versatilidade da linguagem, aumentar a confiabilidade e tornar a programação mais fácil e agradável.

### O Projeto Boost

Mais recentemente, a biblioteca Boost tornou-se uma parte importante da programação C++ e teve uma influência significativa no C++11. O projeto Boost começou em 1998 quando Beman Dawes, o então presidente do grupo de trabalho de biblioteca C++, reuniu alguns outros membros do grupo e desenvolveu um plano para gerar novas bibliotecas fora das restrições do comitê de padrões. A ideia básica era fornecer um site que atua como um fórum aberto para que as pessoas postem bibliotecas C++ gratuitas. O projeto fornece diretrizes para licenciamento e práticas de programação, e requer revisão por pares de bibliotecas propostas. O resultado é um grupo de bibliotecas muito elogiadas e muito usadas.

O Boost tem mais de 100 bibliotecas no momento desta escrita. A maioria das bibliotecas pode ser usada incluindo os arquivos de cabeçalho apropriados.

### O TR1

O Technical Report 1, ou TR1, foi um projeto de um subconjunto do comitê de padrões C++. O TR1 era uma compilação de extensões de biblioteca que eram compatíveis com o padrão C++98, mas que não eram exigidas pelo padrão. Eram candidatos para a próxima iteração do padrão. A biblioteca TR1 permitiu à comunidade C++ testar a adequação dos componentes de biblioteca. Assim, quando o comitê de padrões incorporou a maior parte do TR1 no C++11, estava lidando com bibliotecas conhecidas e comprovadas.

As bibliotecas Boost contribuíram muito para o TR1. Exemplos incluem a classe template `tuple`, a classe template `array`, os templates `bind` e `function`, ponteiros inteligentes (com algumas mudanças de nome e implementação), `static_assert`, a biblioteca `regex` e a biblioteca `random`.

### Usando Boost

Embora você possa agora acessar muitas bibliotecas desenvolvidas pelo Boost como parte do padrão C++11, há muitas bibliotecas Boost adicionais para explorar. Por exemplo, `lexical_cast` da biblioteca Conversion fornece conversões simples entre tipos numéricos e de string. A sintaxe é modelada a partir de `dynamic_cast`, na qual você fornece o tipo de destino como parâmetro de template. A Listagem 18.11 mostra um exemplo simples.

**Listagem 18.11 — `lexcast.cpp`**

```cpp
// lexcast.cpp -- conversão simples de float para string
#include <iostream>
#include <string>
#include "boost/lexical_cast.hpp"
int main()
{
    using namespace std;
    cout << "Digite seu peso: ";
    float peso;
    cin >> peso;
    string ganho = "Um aumento de 10% eleva ";
    string pt = boost::lexical_cast<string>(peso);
    ganho = ganho + pt + " para ";
    peso = 1.1 * peso;
    ganho = ganho + boost::lexical_cast<string>(peso) + ".";
    cout << ganho << endl;
    return 0;
}
```

Eis algumas execuções de exemplo:

```
Digite seu peso: 150
Um aumento de 10% eleva 150 para 165.
Digite seu peso: 156
Um aumento de 10% eleva 156 para 171.600006.
```

A segunda execução de exemplo mostra uma das limitações de `lexical_cast`; ela não fornece controle fino sobre a formatação de ponto flutuante. Para obter isso, você precisaria usar as facilidades de formatação interna mais elaboradas discutidas no Capítulo 17.

Você também pode usar `lexical_cast` para converter strings para valores numéricos.

Obviamente, há muito, muito mais no Boost. Por exemplo, a biblioteca Any permite que você armazene (e recupere) uma coleção heterogênea de valores e objetos de tipos diferentes em um container STL. A biblioteca Math do Boost estende a lista de funções matemáticas além da da biblioteca matemática padrão. A biblioteca Filesystem do Boost facilita a escrita de código portátil entre plataformas usando diferentes sistemas de arquivo.

### E Agora?

Se você trabalhou ao longo deste livro, deve ter um bom domínio das regras do C++. No entanto, isso é apenas o começo do aprendizado desta linguagem. A segunda etapa é aprender a usar a linguagem efetivamente, e essa é a jornada mais longa. A melhor situação é estar em um ambiente de trabalho ou aprendizado que o coloque em contato com bom código e bons programadores C++.

Um dos propósitos da POO é facilitar o desenvolvimento e aprimorar a confiabilidade de grandes projetos. Uma das atividades essenciais da abordagem POO é inventar as classes que representam a situação (chamada de domínio do problema — problem domain) que você está modelando. Como os problemas reais costumam ser complexos, encontrar um conjunto adequado de classes pode ser desafiador. Criar um sistema complexo do zero geralmente não funciona; em vez disso, é melhor adotar uma abordagem iterativa e evolutiva.

Duas técnicas comuns são a análise de casos de uso (use-case analysis) e os cartões CRC (Class-Responsibility-Collaboration). Na análise de casos de uso, a equipe de desenvolvimento lista as formas comuns, ou cenários, em que espera que o sistema final seja usado, identificando elementos, ações e responsabilidades que sugerem possíveis classes, dados e métodos. Os cartões CRC fornecem uma maneira simples de analisar e discutir designs de classe.

---

## Resumo

O C++11 adiciona tipos inteiros de 64 bits (`long long`, `unsigned long long`) e tipos de caractere de 16 e 32 bits (`char16_t`, `char32_t`). Também expande a sintaxe de inicialização por lista para todos os tipos, adicionando proteção contra estreitamento (narrowing). A palavra-chave `auto` passa a ser usada para dedução automática de tipo (com inicializador explícito), e `decltype` permite criar variáveis com o tipo de uma expressão.

A nova forma de tipo de retorno à direita (`-> tipo`) habilita a especificação de tipos de retorno de templates que dependem dos parâmetros de template. A sintaxe `using =` permite criar aliases de template (incluindo aliases parcialmente especializados) além dos `typedef` existentes. A palavra-chave `nullptr` representa o ponteiro nulo com tipo seguro.

O C++11 deprecia `auto_ptr` e introduz `unique_ptr`, `shared_ptr` e `weak_ptr` como ponteiros inteligentes. Deprecia as especificações de exceção (`throw(...)`) e adiciona `noexcept` para documentar que uma função não lança exceções. Introduz enumerações com escopo (`enum class`/`enum struct`) para evitar poluição de nomes e melhorar a verificação de tipos.

Para classes, o C++11 estende `explicit` aos operadores de conversão, permite a inicialização de membros diretamente na definição da classe, adiciona construtores de move e operadores de atribuição por move, e fornece as palavras-chave `default` e `delete` para controlar funções-membro especiais. Construtores podem delegar a outros construtores da mesma classe, e classes derivadas podem herdar construtores com `using`. Os especificadores `override` e `final` ajudam a gerenciar o uso correto de funções virtuais.

O C++11 introduz expressões lambda, que fornecem funções anônimas com uma lista de captura para acessar variáveis do escopo envolvente. O template `function` (cabeçalho `functional`) permite encapsular qualquer tipo chamável (ponteiro de função, funtor, lambda) com uma assinatura de chamada específica, evitando instanciações desnecessárias de templates.

Templates variádicos (`template<typename... Args>`) permitem criar funções e classes que aceitam um número arbitrário de argumentos de tipos diferentes; a recursão com desempacotamento de pacote (`args...`) é a técnica padrão de acesso.

Outras adições notáveis incluem o loop `for` baseado em intervalo, novos containers STL (`forward_list`, `array`, containers hash), métodos `cbegin()`/`cend()`, semântica de move em containers STL, eliminação do requisito de espaço antes de `>>` em templates aninhados, e a referência rvalue `&&`.

Para concorrência, o C++11 define um modelo de memória thread-safe, adiciona `thread_local` e fornece bibliotecas de suporte a threads (`thread`, `mutex`, `condition_variable`, `future`) e operações atômicas (`atomic`). Novas bibliotecas incluem `random` (números aleatórios), `chrono` (tempo), `tuple`, `ratio` e `regex`.

O projeto Boost (www.boost.org) e o TR1 foram grandes influências no C++11, contribuindo com bibliotecas como `function`, `bind`, `tuple`, `array`, ponteiros inteligentes, `regex` e `random`.

---

## Revisão do Capítulo

**1.** O que é uma referência rvalue, e como ela difere de uma referência lvalue?

**2.** O que se entende por semântica de move? Por que ela é benéfica?

**3.** Quais são as seis funções-membro especiais que o compilador pode fornecer automaticamente para uma classe?

**4.** Que efeito tem declarar um construtor de move sobre as funções-membro especiais geradas automaticamente?

**5.** Para que serve a palavra-chave `delete` quando aplicada a uma função-membro?

**6.** O que são construtores delegadores?

**7.** Qual é a diferença entre `override` e `final` quando aplicados a métodos virtuais?

**8.** O que torna uma expressão lambda diferente de uma função com nome? O que é uma lista de captura?

**9.** Qual é o propósito do wrapper `function` e como ele aborda as ineficiências de template?

**10.** O que é um template variádico? Como a recursão é tipicamente usada para acessar os elementos de um pacote de parâmetros de função?

---

## Exercícios de Programação

**1.** Modifique a classe `Inutil` da Listagem 18.2 para incluir tanto um operador de atribuição por cópia quanto um operador de atribuição por move verbosos, e test ambos. Faça isso sem olhar a Listagem 18.3.

**2.** Usando expressões lambda e a Listagem 18.5 como ponto de partida, escreva um programa que:
   - Gere um vector com 1.000.000 inteiros aleatórios.
   - Conta quantos são maiores que a média.
   - Usa `for_each` com um lambda que captura a média por valor e um contador por referência.
   - Exibe o resultado.

**3.** Considerando a seguinte função template simples:

   ```cpp
   template<typename T>
   T maxVal(T a, T b) { return a > b ? a : b; }
   ```

   Escreva uma versão variádica `maxVal(T a, T b, Args... args)` que retorne o maior valor de qualquer número de argumentos. Use recursão e a versão de dois argumentos como caso base.

**4.** Escreva uma classe `Recurso` que gerencie um array de `double` alocado dinamicamente. Ela deve ter:
   - Um construtor que aloca o array e o preenche com zeros.
   - Um construtor de cópia que faz uma cópia profunda.
   - Um construtor de move que "rouba" o array do objeto fonte.
   - Um operador de atribuição por cópia e um por move correspondentes.
   - Um destruidor que libera o array.
   - Um método `Exibir()` que mostra os elementos.

   Demonstre que `std::move()` com um lvalue aciona o construtor de move.

**5.** Use o template `function` para criar um programa que armazene e aplique diferentes transformações matemáticas (funções, funtores e lambdas) a um vetor de `double`. Todas as transformações devem ter a assinatura `double(double)`. Use um único template `usar_f<double, double>` para aplicar cada transformação e verificar que apenas uma instanciação é gerada.

---

[Anterior](capitulo-18-03-lambdas-wrappers-variadicos.md) | [Índice](README.md)
