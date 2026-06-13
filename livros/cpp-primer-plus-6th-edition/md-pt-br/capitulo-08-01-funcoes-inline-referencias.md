# Capítulo 8 — Aventuras em Funções (Adventures in Functions)

> Tradução não oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Navegação: [Anterior](capitulo-07-04-recursao-ponteiros-resumo.md) | [Índice](README.md) | [Próximo](capitulo-08-02-argumentos-padrao-sobrecarga.md)

Neste capítulo você aprenderá sobre os seguintes tópicos:

- Funções inline (inline functions)
- Variáveis de referência (reference variables)
- Como passar argumentos de função por referência
- Argumentos padrão (default arguments)
- Sobrecarga de funções (function overloading)
- Templates de funções (function templates)
- Especializações de templates de funções (function template specializations)

Com o Capítulo 7, "Funções: Os Módulos de Programação do C++", sob o cinto, você já sabe muito sobre funções em C++, mas há muito mais a descobrir. O C++ oferece muitos recursos novos de funções que o distinguem de sua herança em C. Os novos recursos incluem funções inline, passagem de variáveis por referência, valores padrão de argumentos, sobrecarga de funções (polimorfismo) e templates de funções. Este capítulo, mais do que qualquer outro que você leu até agora, explora recursos encontrados no C++ mas não no C, marcando assim sua primeira grande investida no mundo do plus-plussedness.

## Funções Inline do C++ (C++ Inline Functions)

Funções inline (inline functions) são um aprimoramento do C++ projetado para acelerar programas. A distinção principal entre funções normais e funções inline não está em como você as codifica, mas em como o compilador C++ as incorpora ao programa. Para entender a diferença entre funções inline e funções normais, você precisa examinar o interior de um programa com mais profundidade do que fizemos até agora. Vamos fazer isso agora.

O produto final do processo de compilação é um programa executável, que consiste em um conjunto de instruções em linguagem de máquina. Quando você inicia um programa, o sistema operacional carrega essas instruções na memória do computador de forma que cada instrução tenha um endereço de memória particular. O computador então percorre essas instruções passo a passo. Às vezes, como quando você tem um loop ou uma instrução de desvio, a execução do programa pula sobre instruções, saltando para frente ou para trás em direção a um endereço específico. Chamadas de função normais também envolvem fazer o programa saltar para outro endereço (o endereço da função) e depois saltar de volta quando a função termina. Vamos examinar uma implementação típica desse processo com um pouco mais de detalhe. Quando um programa atinge a instrução de chamada de função, o programa armazena o endereço de memória da instrução imediatamente seguinte à chamada de função, copia os argumentos da função para a pilha (stack, um bloco de memória reservado para essa finalidade), salta para o local de memória que marca o início da função, executa o código da função (possivelmente colocando um valor de retorno em um registrador) e então salta de volta para a instrução cujo endereço salvou. Saltar de um lado para outro e controlar para onde saltar implica uma sobrecarga (overhead) no tempo decorrido ao se usar funções.

As funções inline do C++ fornecem uma alternativa. Em uma função inline, o código compilado fica "em linha" (in line) com o restante do código no programa. Ou seja, o compilador substitui a chamada de função pelo código correspondente da função inline. Com código inline, o programa não precisa saltar para outro local para executar o código e depois saltar de volta. Funções inline, portanto, executam um pouco mais rápido do que funções regulares, mas vêm com uma penalidade de memória. Se um programa chama uma função inline em dez locais separados, o programa acaba com dez cópias da função inseridas no código.

Você deve ser seletivo ao usar funções inline. Se o tempo necessário para executar o código da função é longo em comparação com o tempo necessário para tratar o mecanismo de chamada da função, então o tempo economizado é uma porção relativamente pequena do processo inteiro. Se o tempo de execução do código é curto, então uma chamada inline pode economizar uma grande porção do tempo usado pela chamada não-inline. Por outro lado, você agora está economizando uma grande porção de um processo relativamente rápido, então a economia de tempo absoluta pode não ser tão grande, a menos que a função seja chamada frequentemente.

Para usar esse recurso, você deve tomar pelo menos uma das seguintes ações:

- Prefixar a declaração da função com a palavra-chave `inline`.
- Prefixar a definição da função com a palavra-chave `inline`.

Uma prática comum é omitir o protótipo e colocar toda a definição (o cabeçalho da função e todo o código da função) onde o protótipo normalmente estaria.

O compilador não é obrigado a atender ao seu pedido de tornar uma função inline. Ele pode decidir que a função é grande demais ou perceber que ela se chama recursivamente (recursão não é permitida ou possível para funções inline), ou o recurso pode não estar habilitado ou implementado no seu compilador particular.

A Listagem 8.1 ilustra a técnica inline com uma função inline `quadrado()` que eleva seu argumento ao quadrado. Observe que toda a definição está em uma linha. Isso não é obrigatório, mas se a definição não couber em uma ou duas linhas (assumindo que você não tenha identificadores muito longos), a função é provavelmente uma má candidata para ser inline.

**Listagem 8.1** inline.cpp

```cpp
// inline.cpp -- usando uma função inline
#include <iostream>
// uma definição de função inline
inline double quadrado(double x) { return x * x; }
int main()
{
    using namespace std;
    double a, b;
    double c = 13.0;
    a = quadrado(5.0);
    b = quadrado(4.5 + 7.5); // pode passar expressões
    cout << "a = " << a << ", b = " << b << "\n";
    cout << "c = " << c;
    cout << ", c ao quadrado = " << quadrado(c++) << "\n";
    cout << "Agora c = " << c << "\n";
    return 0;
}
```

Saída do programa da Listagem 8.1:

```
a = 25, b = 144
c = 13, c ao quadrado = 169
Agora c = 14
```

Esta saída ilustra que funções inline passam argumentos por valor exatamente como funções regulares fazem. Se o argumento é uma expressão, como `4.5 + 7.5`, a função recebe o valor da expressão — `12`, neste caso. Isso torna o recurso inline do C++ muito superior às definições de macros do C. Veja o quadro "Inline Versus Macros".

Embora o programa não forneça um protótipo separado, os recursos de prototipagem do C++ ainda estão em ação. Isso ocorre porque a definição inteira, que aparece antes do primeiro uso da função, serve como protótipo. Isso significa que você pode usar `quadrado()` com um argumento `int` ou `long`, e o programa automaticamente converte o valor para o tipo `double` antes de passá-lo à função.

> **Inline Versus Macros**
>
> O recurso `inline` é uma adição ao C++. O C usa a instrução de pré-processador `#define` para fornecer macros, que são implementações rudimentares de código inline. Por exemplo, aqui está uma macro para elevar um número ao quadrado:
>
> ```c
> #define QUADRADO(X) X*X
> ```
>
> Isso funciona não passando argumentos, mas por substituição de texto, com o `X` agindo como um rótulo simbólico para o "argumento":
>
> ```c
> a = QUADRADO(5.0);        // substituído por: a = 5.0*5.0;
> b = QUADRADO(4.5 + 7.5); // substituído por: b = 4.5 + 7.5 * 4.5 + 7.5;
> d = QUADRADO(c++);        // substituído por: d = c++*c++;
> ```
>
> Apenas o primeiro exemplo funciona corretamente. Você pode melhorar as coisas com uma aplicação generosa de parênteses:
>
> ```c
> #define QUADRADO(X) ((X)*(X))
> ```
>
> Ainda assim, o problema permanece: macros não passam por valor. Mesmo com essa nova definição, `QUADRADO(c++)` incrementa `c` duas vezes, mas a função inline `quadrado()` na Listagem 8.1 avalia `c`, passa esse valor para ser elevado ao quadrado e depois incrementa `c` uma vez.
>
> A intenção aqui não é mostrar como escrever macros em C. Em vez disso, é sugerir que, se você tem usado macros em C para fornecer serviços semelhantes a funções, deve considerar convertê-las em funções inline do C++.

## Variáveis de Referência (Reference Variables)

O C++ adiciona um novo tipo composto à linguagem — a variável de referência (reference variable). Uma referência é um nome que age como um apelido (alias), ou um nome alternativo, para uma variável previamente definida. Por exemplo, se você faz de `twain` uma referência para a variável `clemens`, você pode usar `twain` e `clemens` de forma intercambiável para representar aquela variável. Qual é a utilidade de tal apelido? A principal utilidade de uma variável de referência é como argumento formal para uma função. Se você usar uma referência como argumento, a função trabalha com os dados originais em vez de uma cópia. Referências fornecem uma alternativa conveniente a ponteiros para processar grandes estruturas com uma função, e são essenciais para projetar classes. Antes de ver como usar referências com funções, no entanto, vamos examinar o básico de como definir e usar uma referência. Tenha em mente que o propósito da discussão a seguir é ilustrar como as referências funcionam, não como elas são tipicamente usadas.

### Criando uma Variável de Referência

Você deve lembrar que C e C++ usam o símbolo `&` para indicar o endereço de uma variável. O C++ atribui um significado adicional ao símbolo `&` e o usa na declaração de referências. Por exemplo, para tornar `roedores` um nome alternativo para a variável `ratos`, você faria o seguinte:

```cpp
int ratos;
int & roedores = ratos; // torna roedores um alias para ratos
```

Neste contexto, `&` não é o operador de endereço. Em vez disso, serve como parte do identificador de tipo. Assim como `char *` em uma declaração significa ponteiro-para-char, `int &` significa referência-para-int. A declaração de referência permite que você use `ratos` e `roedores` de forma intercambiável; ambos se referem ao mesmo valor e ao mesmo local de memória. A Listagem 8.2 ilustra a verdade desta afirmação.

**Listagem 8.2** firstref.cpp

```cpp
// firstref.cpp -- definindo e usando uma referência
#include <iostream>
int main()
{
    using namespace std;
    int ratos = 101;
    int & roedores = ratos; // roedores é uma referência

    cout << "ratos = " << ratos;
    cout << ", roedores = " << roedores << endl;
    roedores++;
    cout << "ratos = " << ratos;
    cout << ", roedores = " << roedores << endl;
    // algumas implementações requerem cast do endereço para unsigned
    cout << "endereco de ratos = " << &ratos;
    cout << ", endereco de roedores = " << &roedores << endl;
    return 0;
}
```

Observe que o operador `&` na seguinte instrução *não* é o operador de endereço, mas declara que `roedores` é do tipo `int &` (ou seja, é uma referência para uma variável `int`):

```cpp
int & roedores = ratos;
```

Mas o operador `&` na próxima instrução *é* o operador de endereço, com `&roedores` representando o endereço da variável à qual `roedores` se refere:

```cpp
cout << ", endereco de roedores = " << &roedores << endl;
```

Saída do programa da Listagem 8.2:

```
ratos = 101, roedores = 101
ratos = 102, roedores = 102
endereco de ratos = 0x0065fd48, endereco de roedores = 0x0065fd48
```

Como você pode ver, tanto `ratos` quanto `roedores` têm o mesmo valor e o mesmo endereço. (Os valores de endereço e o formato de exibição variam de sistema para sistema.) Incrementar `roedores` em um afeta ambas as variáveis. Mais precisamente, a operação `roedores++` incrementa uma única variável para a qual existem dois nomes. (Novamente, tenha em mente que, embora este exemplo mostre como uma referência funciona, ele não representa o uso típico de uma referência, que é como parâmetro de função, particularmente para argumentos de estrutura e objeto.)

As referências tendem a ser um pouco confusas no início para veteranos de C que chegam ao C++, porque são tentadoramente parecidas com ponteiros, mas de alguma forma diferentes. Por exemplo, você pode criar tanto uma referência quanto um ponteiro para se referir a `ratos`:

```cpp
int ratos = 101;
int & roedores = ratos; // roedores é uma referência
int * pratos = &ratos;  // pratos é um ponteiro
```

Então você poderia usar as expressões `roedores` e `*pratos` de forma intercambiável com `ratos`, e usar as expressões `&roedores` e `pratos` de forma intercambiável com `&ratos`. Desse ponto de vista, uma referência parece muito com um ponteiro em notação disfarçada, na qual o operador de desreferenciamento `*` está implicitamente entendido. E, de fato, é mais ou menos isso que uma referência é. Mas há diferenças além das de notação. Por um lado, é necessário inicializar a referência quando você a declara; você não pode declarar a referência e depois atribuir um valor a ela da forma como pode fazer com um ponteiro:

```cpp
int rato;
int & roedor;      // Não, não pode fazer isso.
roedor = rato;
```

> **Nota:** Você deve inicializar uma variável de referência quando a declara.

Uma referência é como um ponteiro `const`; você deve inicializá-la quando a cria, e quando uma referência jura lealdade a uma variável particular, ela mantém essa promessa. Ou seja,

```cpp
int & roedores = ratos;
```

é, em essência, uma notação disfarçada para algo como isto:

```cpp
int * const pr = &ratos;
```

Aqui, a referência `roedores` desempenha o mesmo papel que a expressão `*pr`.

A Listagem 8.3 mostra o que acontece se você tentar fazer uma referência mudar de lealdade de uma variável `ratos` para uma variável `coelhos`.

**Listagem 8.3** secref.cpp

```cpp
// secref.cpp -- definindo e usando uma referência
#include <iostream>
int main()
{
    using namespace std;
    int ratos = 101;
    int & roedores = ratos; // roedores é uma referência
    cout << "ratos = " << ratos;
    cout << ", roedores = " << roedores << endl;
    cout << "endereco de ratos = " << &ratos;
    cout << ", endereco de roedores = " << &roedores << endl;

    int coelhos = 50;
    roedores = coelhos; // podemos mudar a referência?
    cout << "coelhos = " << coelhos;
    cout << ", ratos = " << ratos;
    cout << ", roedores = " << roedores << endl;
    cout << "endereco de coelhos = " << &coelhos;
    cout << ", endereco de roedores = " << &roedores << endl;
    return 0;
}
```

Saída do programa da Listagem 8.3:

```
ratos = 101, roedores = 101
endereco de ratos = 0x0065fd44, endereco de roedores = 0x0065fd44
coelhos = 50, ratos = 50, roedores = 50
endereco de coelhos = 0x0065fd48, endereco de roedores = 0x0065fd44
```

Inicialmente, `roedores` se refere a `ratos`, mas então o programa aparentemente tenta fazer `roedores` ser uma referência para `coelhos`:

```cpp
roedores = coelhos;
```

Por um momento, parece que essa tentativa teve sucesso porque o valor de `roedores` muda de `101` para `50`. Mas uma inspeção mais cuidadosa revela que `ratos` também mudou para `50` e que `ratos` e `roedores` ainda compartilham o mesmo endereço, que difere do endereço de `coelhos`. Como `roedores` é um alias para `ratos`, a instrução de atribuição realmente significa o mesmo que o seguinte:

```cpp
ratos = coelhos;
```

Ou seja, significa "Atribuir o valor da variável `coelhos` à variável `ratos`." Em resumo, você pode definir uma referência por uma declaração de inicialização, não por atribuição.

Suponha que você tentasse o seguinte:

```cpp
int ratos = 101;
int * pt = &ratos;
int & roedores = *pt;
int coelhos = 50;
pt = &coelhos;
```

Inicializar `roedores` com `*pt` faz `roedores` se referir a `ratos`. Alterar subsequentemente `pt` para apontar para `coelhos` não altera o fato de que `roedores` se refere a `ratos`.

## Referências como Parâmetros de Função (References as Function Parameters)

Na maioria das vezes, referências são usadas como parâmetros de função, fazendo com que um nome de variável em uma função seja um alias para uma variável no programa chamador. Esse método de passagem de argumentos é chamado de *passagem por referência* (passing by reference). Passagem por referência permite que uma função chamada acesse variáveis na função chamadora. A adição desse recurso pelo C++ é uma ruptura com o C, que só passa por valor. Passar por valor, como você se lembra, resulta em a função chamada trabalhar com cópias de valores do programa chamador. Claro, o C permite contornar a limitação de passagem por valor usando ponteiros.

Vamos comparar o uso de referências e ponteiros em um problema comum de computador: trocar os valores de duas variáveis. Uma função de troca deve ser capaz de alterar valores de variáveis no programa chamador. Isso significa que a abordagem usual de passar variáveis por valor não funcionará, porque a função acabaria trocando o conteúdo de cópias das variáveis originais em vez das variáveis em si. Se você passar referências, no entanto, a função pode trabalhar com os dados originais. Alternativamente, você pode passar ponteiros para acessar os dados originais. A Listagem 8.4 mostra todos os três métodos, incluindo o que não funciona, para que você possa compará-los.

**Listagem 8.4** swaps.cpp

```cpp
// swaps.cpp -- trocando com referências e com ponteiros
#include <iostream>
void trocar_r(int & a, int & b); // a, b são aliases para ints
void trocar_p(int * p, int * q); // p, q são endereços de ints
void trocar_v(int a, int b);     // a, b são novas variáveis
int main()
{
    using namespace std;
    int carteira1 = 300;
    int carteira2 = 350;

    cout << "carteira1 = $" << carteira1;
    cout << " carteira2 = $" << carteira2 << endl;
    cout << "Usando referencias para trocar conteudos:\n";
    trocar_r(carteira1, carteira2); // passa variáveis
    cout << "carteira1 = $" << carteira1;
    cout << " carteira2 = $" << carteira2 << endl;
    cout << "Usando ponteiros para trocar conteudos novamente:\n";
    trocar_p(&carteira1, &carteira2); // passa endereços das variáveis
    cout << "carteira1 = $" << carteira1;
    cout << " carteira2 = $" << carteira2 << endl;

    cout << "Tentando usar passagem por valor:\n";
    trocar_v(carteira1, carteira2); // passa valores das variáveis
    cout << "carteira1 = $" << carteira1;
    cout << " carteira2 = $" << carteira2 << endl;
    return 0;
}
void trocar_r(int & a, int & b) // usa referências
{
    int temp;
    temp = a; // usa a, b para valores das variáveis
    a = b;
    b = temp;
}
void trocar_p(int * p, int * q) // usa ponteiros
{
    int temp;
    temp = *p; // usa *p, *q para valores das variáveis
    *p = *q;
    *q = temp;
}
void trocar_v(int a, int b) // tenta usar valores
{
    int temp;
    temp = a; // usa a, b para valores das variáveis
    a = b;
    b = temp;
}
```

Saída do programa da Listagem 8.4:

```
carteira1 = $300 carteira2 = $350
Usando referencias para trocar conteudos:
carteira1 = $350 carteira2 = $300
Usando ponteiros para trocar conteudos novamente:
carteira1 = $300 carteira2 = $350
Tentando usar passagem por valor:
carteira1 = $300 carteira2 = $350
```

Como era de esperar, os métodos de referência e ponteiro trocam com sucesso o conteúdo das duas carteiras, enquanto o método de passagem por valor falha.

**Notas do Programa**

Primeiro, observe como cada função na Listagem 8.4 é chamada:

```cpp
trocar_r(carteira1, carteira2); // passa variáveis
trocar_p(&carteira1, &carteira2); // passa endereços das variáveis
trocar_v(carteira1, carteira2); // passa valores das variáveis
```

Passagem por referência (`trocar_r(carteira1, carteira2)`) e passagem por valor (`trocar_v(carteira1, carteira2)`) parecem idênticas. A única forma de saber que `trocar_r()` passa por referência é olhando o protótipo ou a definição da função. No entanto, a presença do operador de endereço (`&`) torna óbvio quando uma função passa por endereço (`trocar_p(&carteira1, &carteira2)`).

Em seguida, compare o código para as funções `trocar_r()` (passagem por referência) e `trocar_v()` (passagem por valor). A única diferença externa entre as duas é como os parâmetros de função são declarados:

```cpp
void trocar_r(int & a, int & b)
void trocar_v(int a, int b)
```

A diferença interna, é claro, é que em `trocar_r()` as variáveis `a` e `b` servem como aliases para `carteira1` e `carteira2`, portanto trocar `a` e `b` troca `carteira1` e `carteira2`. Mas em `trocar_v()`, as variáveis `a` e `b` são novas variáveis que copiam os valores de `carteira1` e `carteira2`, portanto trocar `a` e `b` não tem efeito sobre `carteira1` e `carteira2`.

## Propriedades e Peculiaridades de Referências (Reference Properties and Oddities)

Usar argumentos de referência tem várias nuances que você precisa conhecer. Primeiro, considere a Listagem 8.5. Ela usa duas funções para calcular o cubo de um argumento. Uma recebe um argumento do tipo `double`, e a outra recebe uma referência para `double`. O código real para calcular o cubo é propositadamente um pouco estranho para ilustrar um ponto.

**Listagem 8.5** cubes.cpp

```cpp
// cubes.cpp -- argumentos regulares e por referência
#include <iostream>
double cubo(double a);
double refcubo(double &ra);
int main ()
{
    using namespace std;
    double x = 3.0;
    cout << cubo(x);
    cout << " = cubo de " << x << endl;
    cout << refcubo(x);
    cout << " = cubo de " << x << endl;
    return 0;
}
double cubo(double a)
{
    a *= a * a;
    return a;
}
double refcubo(double &ra)
{
    ra *= ra * ra;
    return ra;
}
```

Saída do programa da Listagem 8.5:

```
27 = cubo de 3
27 = cubo de 27
```

Observe que a função `refcubo()` modifica o valor de `x` em `main()` e `cubo()` não, o que lembra por que a passagem por valor é a norma. A variável `a` é local para `cubo()`. Ela é inicializada com o valor de `x`, mas mudar `a` não tem efeito sobre `x`. Mas como `refcubo()` usa um argumento de referência, as mudanças que ela faz em `ra` são na verdade feitas em `x`. Se sua intenção é que uma função use as informações passadas a ela sem modificá-las, e se você está usando uma referência, deve usar uma referência constante. Aqui, por exemplo, você deveria usar `const` no protótipo e no cabeçalho da função:

```cpp
double refcubo(const double &ra);
```

Se você fizer isso, o compilador gerará uma mensagem de erro quando encontrar código que altera o valor de `ra`.

A propósito, se você precisar escrever uma função como a deste exemplo (ou seja, usando um tipo numérico básico), deve usar passagem por valor em vez da passagem por referência mais exótica. Argumentos de referência tornam-se úteis com unidades de dados maiores, como estruturas e classes, como você verá em breve.

Funções que passam por valor, como a função `cubo()` na Listagem 8.5, podem usar muitos tipos de argumentos reais. Por exemplo, todas as seguintes chamadas são válidas:

```cpp
double z = cubo(x + 2.0); // avalia x + 2.0, passa o valor
z = cubo(8.0);             // passa o valor 8.0
int k = 10;
z = cubo(k);               // converte valor de k para double, passa o valor
double yo[3] = { 2.2, 3.3, 4.4};
z = cubo(yo[2]);           // passa o valor 4.4
```

Suponha que você tente argumentos semelhantes para uma função com um parâmetro de referência. Pareceria que passar uma referência deveria ser mais restritivo. Afinal, se `ra` é o nome alternativo de uma variável, então o argumento real deveria ser aquela variável. Algo como o seguinte não parece fazer sentido porque a expressão `x + 3.0` não é uma variável:

```cpp
double z = refcubo(x + 3.0); // não deve compilar
```

Por exemplo, você não pode atribuir um valor a tal expressão:

```cpp
x + 3.0 = 5.0; // sem sentido
```

O que acontece se você tentar uma chamada de função como `refcubo(x + 3.0)`? No C++ contemporâneo, isso é um erro, e a maioria dos compiladores dirá isso. Alguns mais antigos darão um aviso como o seguinte:

```
Warning: Temporary used for parameter 'ra' in call to refcubo(double &)
```

A razão para essa resposta mais branda é que o C++, em seus primeiros anos, permitia passar expressões para uma variável de referência. Em alguns casos, ainda permite. O que acontece é que, como `x + 3.0` não é uma variável do tipo `double`, o programa cria uma variável temporária sem nome, inicializando-a com o valor da expressão `x + 3.0`. Então `ra` se torna uma referência para aquela variável temporária.

### Variáveis Temporárias, Argumentos de Referência e const

O C++ pode gerar uma variável temporária se o argumento real não corresponder a um argumento de referência. Atualmente, o C++ só permite isso se o argumento for uma referência `const`, mas nem sempre foi assim. Vamos examinar os casos em que o C++ gera variáveis temporárias e ver por que a restrição a uma referência `const` faz sentido.

Primeiro, quando uma variável temporária é criada? Desde que o parâmetro de referência seja `const`, o compilador gera uma variável temporária em dois tipos de situações:

- Quando o argumento real é do tipo correto, mas não é um lvalue
- Quando o argumento real é do tipo errado, mas é de um tipo que pode ser convertido para o tipo correto

O que é um *lvalue*? Um argumento que é um lvalue é um objeto de dados que pode ser referenciado por endereço. Por exemplo, uma variável, um elemento de array, um membro de estrutura, uma referência e um ponteiro desreferenciado são lvalues. Não-lvalues incluem constantes literais (exceto strings entre aspas, que são representadas por seus endereços) e expressões com múltiplos termos. O termo *lvalue* em C originalmente significava entidades que podiam aparecer no lado esquerdo de uma instrução de atribuição, mas isso foi antes de a palavra-chave `const` ser introduzida. Agora, tanto uma variável regular quanto uma variável `const` seriam consideradas lvalues porque ambas podem ser acessadas por endereço. Mas a variável regular pode ser ainda caracterizada como um *lvalue modificável* (modifiable lvalue) e a variável `const` como um *lvalue não modificável* (non-modifiable lvalue).

Agora, voltando ao nosso exemplo, suponha que você redefina `refcubo()` de forma que tenha um argumento de referência constante:

```cpp
double refcubo(const double &ra)
{
    return ra * ra * ra;
}
```

Em seguida, considere o seguinte código:

```cpp
double lado = 3.0;
double * pd = &lado;
double & rd = lado;
long aresta = 5L;
double lentes[4] = { 2.0, 5.0, 10.0, 12.0};
double c1 = refcubo(lado);        // ra é lado
double c2 = refcubo(lentes[2]);   // ra é lentes[2]
double c3 = refcubo(rd);          // ra é rd que é lado
double c4 = refcubo(*pd);         // ra é *pd que é lado
double c5 = refcubo(aresta);      // ra é variável temporária
double c6 = refcubo(7.0);         // ra é variável temporária
double c7 = refcubo(lado + 10.0); // ra é variável temporária
```

Os argumentos `lado`, `lentes[2]`, `rd` e `*pd` são objetos de dados do tipo `double` com nomes, portanto é possível gerar uma referência para eles e nenhuma variável temporária é necessária. (Lembre-se de que um elemento de array se comporta como uma variável do mesmo tipo que o elemento.) Mas embora `aresta` seja uma variável, é do tipo errado. Uma referência para `double` não pode se referir a um `long`. Os argumentos `7.0` e `lado + 10.0`, por outro lado, são do tipo correto, mas não são objetos de dados com nome. Em cada um desses casos, o compilador gera uma variável temporária anônima e faz `ra` se referir a ela. Essas variáveis temporárias duram pela duração da chamada de função, e então o compilador é livre para descartá-las.

Então por que esse comportamento está correto para referências constantes, mas não de outra forma? Lembre-se da função `trocar_r()` da Listagem 8.4:

```cpp
void trocar_r(int & a, int & b) // usa referências
{
    int temp;
    temp = a; // usa a, b para valores das variáveis
    a = b;
    b = temp;
}
```

O que aconteceria se você fizesse o seguinte sob as regras mais permissivas do C++ inicial?

```cpp
long a = 3, b = 5;
trocar_r(a, b);
```

Aqui há uma incompatibilidade de tipo, então o compilador criaria duas variáveis `int` temporárias, as inicializaria com `3` e `5`, e então trocaria o conteúdo das variáveis temporárias, deixando `a` e `b` inalterados.

Em resumo, se a intenção de uma função com argumentos de referência é modificar variáveis passadas como argumentos, situações que criam variáveis temporárias frustram esse propósito. A solução é proibir a criação de variáveis temporárias nessas situações, e é isso que o Padrão C++ agora faz. (No entanto, alguns compiladores ainda emitem avisos em vez de mensagens de erro por padrão, portanto, se você vir um aviso sobre variáveis temporárias, não o ignore.)

Agora pense na função `refcubo()`. Sua intenção é meramente usar os valores passados, não modificá-los, então variáveis temporárias não causam dano e tornam a função mais geral nos tipos de argumentos que pode tratar. Portanto, se a declaração afirma que uma referência é `const`, o C++ gera variáveis temporárias quando necessário. Em essência, uma função C++ com um argumento formal de referência `const` e um argumento real não correspondente imita o comportamento tradicional de passagem por valor, garantindo que os dados originais não sejam alterados e usando uma variável temporária para guardar o valor.

> **Nota:** Se um argumento de chamada de função não é um lvalue ou não corresponde ao tipo do parâmetro de referência `const` correspondente, o C++ cria uma variável anônima do tipo correto, atribui o valor do argumento da chamada de função à variável anônima e faz o parâmetro se referir a essa variável.

**Use `const` Quando Puder**

Há três razões fortes para declarar argumentos de referência como referências a dados constantes:

- Usar `const` protege você de erros de programação que inadvertidamente alteram dados.
- Usar `const` permite que uma função processe tanto argumentos reais `const` quanto não-`const`, enquanto uma função que omite `const` no protótipo só pode aceitar dados não-`const`.
- Usar uma referência `const` permite que a função gere e use uma variável temporária quando apropriado.

Você deve declarar argumentos de referência formais como `const` sempre que for apropriado.

O C++11 introduz um segundo tipo de referência, chamado *referência rvalue* (rvalue reference), que pode se referir a um rvalue. É declarado usando `&&`:

```cpp
double && rref = std::sqrt(36.00); // não permitido para double &
double j = 15.0;
double && jref = 2.0 * j + 18.5;  // não permitido para double &
std::cout << rref << '\n';  // exibe 6.0
std::cout << jref << '\n';  // exibe 48.5
```

A referência rvalue foi introduzida principalmente para ajudar os projetistas de bibliotecas a fornecer implementações mais eficientes de certas operações. O Capítulo 18, "Visitando o Novo Padrão C++", discute como referências rvalue são usadas para implementar uma abordagem chamada *semântica de movimento* (move semantics). O tipo de referência original (declarado usando um único `&`) agora é chamado de *referência lvalue* (lvalue reference).

## Usando Referências com Estruturas (Using References with a Structure)

Referências funcionam maravilhosamente com estruturas e classes, os tipos definidos pelo usuário do C++. Na verdade, as referências foram introduzidas principalmente para uso com esses tipos, não para uso com os tipos básicos embutidos.

O método para usar uma referência para uma estrutura como parâmetro de função é o mesmo que para usar uma referência para uma variável básica: você simplesmente usa o operador de referência `&` ao declarar um parâmetro de estrutura. Por exemplo, suponha que temos a seguinte definição de estrutura:

```cpp
struct arremessos_livres
{
    std::string nome;
    int convertidos;
    int tentativas;
    float percentual;
};
```

Então uma função usando uma referência para esse tipo pode ser prototipada da seguinte forma:

```cpp
void def_pct(arremessos_livres & ft); // usa referência para estrutura
```

Se a intenção é que a função não altere a estrutura, use `const`:

```cpp
void exibir(const arremessos_livres & ft); // não permite mudanças na estrutura
```

O programa na Listagem 8.6 faz exatamente essas coisas. Ele também adiciona um recurso interessante ao ter uma função retornar uma referência para a estrutura. Isso funciona de forma um pouco diferente de retornar uma estrutura. Há algumas precauções a notar, às quais chegaremos em breve.

**Listagem 8.6** strc_ref.cpp

```cpp
// strc_ref.cpp -- usando referências para estruturas
#include <iostream>
#include <string>
struct arremessos_livres
{
    std::string nome;
    int convertidos;
    int tentativas;
    float percentual;
};
void exibir(const arremessos_livres & ft);
void def_pct(arremessos_livres & ft);
arremessos_livres & acumular(arremessos_livres & alvo,
                             const arremessos_livres & fonte);
int main()
{
    // inicializações parciais – membros restantes definidos como 0
    arremessos_livres um = {"Ifelsa Branch", 13, 14};
    arremessos_livres dois = {"Andor Knott", 10, 16};
    arremessos_livres tres = {"Minnie Max", 7, 9};
    arremessos_livres quatro = {"Whily Looper", 5, 9};
    arremessos_livres cinco = {"Long Long", 6, 14};
    arremessos_livres equipe = {"Throwgoods", 0, 0};
    // sem inicialização
    arremessos_livres dup;
    def_pct(um);
    exibir(um);
    acumular(equipe, um);
    exibir(equipe);
    // usa valor de retorno como argumento
    exibir(acumular(equipe, dois));
    acumular(acumular(equipe, tres), quatro);
    exibir(equipe);
    // usa valor de retorno em atribuição
    dup = acumular(equipe, cinco);
    std::cout << "Exibindo equipe:\n";
    exibir(equipe);
    std::cout << "Exibindo dup apos atribuicao:\n";
    exibir(dup);
    def_pct(quatro);
    // atribuição indevida
    acumular(dup, cinco) = quatro;
    std::cout << "Exibindo dup apos atribuicao indevida:\n";
    exibir(dup);
    return 0;
}
void exibir(const arremessos_livres & ft)
{
    using std::cout;
    cout << "Nome: " << ft.nome << '\n';
    cout << " Convertidos: " << ft.convertidos << '\t';
    cout << "Tentativas: " << ft.tentativas << '\t';
    cout << "Percentual: " << ft.percentual << '\n';
}
void def_pct(arremessos_livres & ft)
{
    if (ft.tentativas != 0)
        ft.percentual = 100.0f * float(ft.convertidos) / float(ft.tentativas);
    else
        ft.percentual = 0;
}
arremessos_livres & acumular(arremessos_livres & alvo,
                             const arremessos_livres & fonte)
{
    alvo.tentativas += fonte.tentativas;
    alvo.convertidos += fonte.convertidos;
    def_pct(alvo);
    return alvo;
}
```

Saída do programa:

```
Nome: Ifelsa Branch
 Convertidos: 13  Tentativas: 14  Percentual: 92.8571
Nome: Throwgoods
 Convertidos: 13  Tentativas: 14  Percentual: 92.8571
Nome: Throwgoods
 Convertidos: 23  Tentativas: 30  Percentual: 76.6667
Nome: Throwgoods
 Convertidos: 35  Tentativas: 48  Percentual: 72.9167
Exibindo equipe:
Nome: Throwgoods
 Convertidos: 41  Tentativas: 62  Percentual: 66.129
Exibindo dup apos atribuicao:
Nome: Throwgoods
 Convertidos: 41  Tentativas: 62  Percentual: 66.129
Exibindo dup apos atribuicao indevida:
Nome: Whily Looper
 Convertidos: 5  Tentativas: 9  Percentual: 55.5556
```

**Notas do Programa**

O programa começa inicializando vários objetos de estrutura. Lembre-se de que, se houver menos inicializadores do que membros, os membros restantes (apenas os membros `percentual`, neste caso) são definidos como `0`. A primeira chamada de função é esta:

```cpp
def_pct(um);
```

Como o parâmetro formal `ft` em `def_pct()` é uma referência, `ft` se refere a `um`, e o código em `def_pct()` define o membro `um.percentual`. Passar por valor não funcionaria neste caso porque isso resultaria em definir o membro `percentual` de uma cópia temporária de `um`.

A próxima chamada de função é:

```cpp
exibir(um);
```

Como `exibir()` exibe o conteúdo da estrutura sem alterá-lo, a função usa um parâmetro de referência `const`. Neste caso, poder-se-ia ter passado a estrutura por valor, mas usar uma referência é mais econômico em tempo e memória do que fazer uma cópia da estrutura original.

A próxima chamada é:

```cpp
acumular(equipe, um);
```

A função `acumular()` recebe dois argumentos de estrutura. Ela adiciona dados dos membros `tentativas` e `convertidos` da segunda estrutura aos membros correspondentes da primeira estrutura. Somente a primeira estrutura é modificada, portanto o primeiro parâmetro é uma referência, enquanto o segundo parâmetro é uma referência `const`.

Quanto ao valor de retorno? A chamada de função que acabamos de discutir não o usou; até onde aquele uso ia, a função poderia ter sido do tipo `void`. Mas olhe para esta chamada de função:

```cpp
exibir(acumular(equipe, dois));
```

Vamos seguir o objeto de estrutura `equipe`. Primeiro, `equipe` é passado para `acumular()` como seu primeiro argumento. Isso significa que o objeto `alvo` em `acumular()` é realmente `equipe`. A função `acumular()` modifica `equipe`, depois a retorna como referência. O tipo de retorno é uma referência, então o valor de retorno é o objeto `equipe` original passado para `acumular()`.

O que acontece a seguir? O valor de retorno de `acumular()` é o primeiro argumento para `exibir()`, o que significa que `equipe` é o primeiro argumento para `exibir()`. Como o parâmetro de `exibir()` é uma referência, o objeto `ft` em `exibir()` é realmente `equipe`. Portanto, o conteúdo de `equipe` é exibido. O efeito líquido de

```cpp
exibir(acumular(equipe, dois));
```

é o mesmo que:

```cpp
acumular(equipe, dois);
exibir(equipe);
```

A mesma lógica se aplica a esta instrução:

```cpp
acumular(acumular(equipe, tres), quatro);
```

Isso tem o mesmo efeito que:

```cpp
acumular(equipe, tres);
acumular(equipe, quatro);
```

Em seguida, o programa usa uma instrução de atribuição:

```cpp
dup = acumular(equipe, cinco);
```

Como seria de esperar, isso copia os valores em `equipe` para `dup`.

Finalmente, o programa usa `acumular()` de uma maneira para a qual ela não foi planejada:

```cpp
acumular(dup, cinco) = quatro;
```

Essa instrução — ou seja, atribuir um valor a uma chamada de função — funciona porque o valor de retorno é uma referência. Como o valor de retorno é uma referência para `dup`, este código tem o mesmo efeito que:

```cpp
acumular(dup, cinco); // adiciona dados de cinco a dup
dup = quatro;         // sobrescreve o conteúdo de dup com o conteúdo de quatro
```

A segunda instrução desfaz o trabalho realizado pela primeira, portanto a instrução de atribuição original não foi um bom uso de `acumular()`.

**Por Que Retornar uma Referência?**

Uma função que retorna uma referência é na verdade um alias para a variável referenciada.

O ponto mais importante a lembrar ao retornar uma referência é evitar retornar uma referência para um local de memória que deixa de existir quando a função termina. O que você quer evitar é código ao longo destas linhas:

```cpp
const arremessos_livres & clone2(arremessos_livres & ft)
{
    arremessos_livres novato; // primeiro passo para um grande erro
    novato = ft;              // copia info
    return novato;            // retorna referência para cópia
}
```

Isso tem o infeliz efeito de retornar uma referência para uma variável temporária (`novato`) que deixa de existir assim que a função termina. Similarmente, você deve evitar retornar ponteiros para tais variáveis temporárias.

A maneira mais simples de evitar esse problema é retornar uma referência que foi passada como argumento para a função. Uma referência de parâmetro se referirá a dados usados pela função chamadora; portanto, a referência retornada se referirá a esses mesmos dados. É exatamente isso que `acumular()` faz na Listagem 8.6.

Um segundo método é usar `new` para criar novo armazenamento. Você já viu exemplos em que `new` cria espaço para uma string e a função retorna um ponteiro para esse espaço. Veja como você poderia fazer algo semelhante com uma referência:

```cpp
const arremessos_livres & clonar(arremessos_livres & ft)
{
    arremessos_livres * pt;
    *pt = ft;   // copia info
    return *pt; // retorna referência para cópia
}
```

Há um problema com essa abordagem: você deve usar `delete` para liberar memória alocada por `new` quando a memória não for mais necessária. Uma chamada a `clonar()` oculta a chamada a `new`, tornando mais fácil esquecer de usar `delete` posteriormente. O template `auto_ptr` ou, melhor ainda, o `unique_ptr` do C++11, discutido no Capítulo 16, pode ajudar a automatizar o processo de exclusão.

**Por Que Usar `const` com um Retorno de Referência?**

Às vezes, omitir `const` faz sentido. O operador sobrecarregado `<<` discutido no Capítulo 11, "Trabalhando com Classes", é um exemplo. Mas em geral, tornar o tipo de retorno uma referência `const` protege você da tentação de ofuscação.

## Usando Referências com um Objeto de Classe (Using References with a Class Object)

A prática usual em C++ para passar objetos de classe para uma função é usar referências. Por exemplo, você usaria parâmetros de referência para funções que recebem objetos das classes `string`, `ostream`, `istream`, `ofstream` e `ifstream` como argumentos.

Vamos examinar um exemplo que usa a classe `string` e ilustra diferentes escolhas de design, algumas delas ruins. A ideia geral é criar uma função que adiciona uma determinada string a cada extremidade de outra string. A Listagem 8.7 fornece três funções que pretendem fazer isso. No entanto, um dos designs é tão falho que pode fazer o programa travar ou até mesmo não compilar.

**Listagem 8.7** strquote.cpp

```cpp
// strquote.cpp -- designs diferentes
#include <iostream>
#include <string>
using namespace std;
string versao1(const string & s1, const string & s2);
const string & versao2(string & s1, const string & s2); // tem efeito colateral
const string & versao3(string & s1, const string & s2); // design ruim
int main()
{
    string entrada;
    string copia;
    string resultado;
    cout << "Digite uma string: ";
    getline(cin, entrada);
    copia = entrada;
    cout << "Sua string como digitada: " << entrada << endl;
    resultado = versao1(entrada, "***");
    cout << "Sua string aprimorada: " << resultado << endl;
    cout << "Sua string original: " << entrada << endl;

    resultado = versao2(entrada, "###");
    cout << "Sua string aprimorada: " << resultado << endl;
    cout << "Sua string original: " << entrada << endl;
    cout << "Redefinindo string original.\n";
    entrada = copia;
    resultado = versao3(entrada, "@@@");
    cout << "Sua string aprimorada: " << resultado << endl;
    cout << "Sua string original: " << entrada << endl;
    return 0;
}
string versao1(const string & s1, const string & s2)
{
    string temp;
    temp = s2 + s1 + s2;
    return temp;
}
const string & versao2(string & s1, const string & s2) // tem efeito colateral
{
    s1 = s2 + s1 + s2;
    // seguro retornar referência passada para a função
    return s1;
}
const string & versao3(string & s1, const string & s2) // design ruim
{
    string temp;
    temp = s2 + s1 + s2;
    // inseguro retornar referência para variável local
    return temp;
}
```

Saída de exemplo:

```
Digite uma string: Não é minha culpa.
Sua string como digitada: Não é minha culpa.
Sua string aprimorada: ***Não é minha culpa.***
Sua string original: Não é minha culpa.
Sua string aprimorada: ###Não é minha culpa.###
Sua string original: ###Não é minha culpa.###
Redefinindo string original.
[o programa trava aqui]
```

**Notas do Programa**

A versão 1 da função na Listagem 8.7 é a mais direta das três:

```cpp
string versao1(const string & s1, const string & s2)
{
    string temp;
    temp = s2 + s1 + s2;
    return temp;
}
```

Ela recebe dois argumentos `string` e usa a adição da classe `string` para criar uma nova string com as propriedades desejadas. Observe que os dois argumentos de função são referências `const`. A função produziria o mesmo resultado final se apenas passasse objetos `string`. Nesse caso, `s1` e `s2` seriam objetos `string` totalmente novos. Portanto, usar referências é mais eficiente porque a função não precisa criar novos objetos e copiar dados dos objetos antigos para os novos. O uso do qualificador `const` indica que esta função usará, mas não modificará, as strings originais.

O objeto `temp` é um novo objeto, local para a função `versao1()`, e deixa de existir quando a função termina. Portanto, retornar `temp` como referência não funcionaria, então o tipo da função é `string`. Isso significa que o conteúdo de `temp` será copiado para um local de retorno temporário e, em seguida, em `main()`, o conteúdo do local de retorno é copiado para a string chamada `resultado`.

> **Passando um Argumento de String de Estilo C para um Parâmetro de Referência de Objeto `string`**
>
> Você pode ter notado um fato interessante sobre a função `versao1()`: ambos os parâmetros formais (`s1` e `s2`) são do tipo `const string &`, mas os argumentos reais (`entrada` e `"***"`) são do tipo `string` e `const char *`, respectivamente. Como `entrada` é do tipo `string`, não há problema em fazer `s1` se referir a ela. Mas como o programa aceita passar um argumento ponteiro-para-char para uma referência de `string`?
>
> Duas coisas estão acontecendo aqui. Uma é que a classe `string` define uma conversão de `char *` para `string`, o que torna possível inicializar um objeto `string` para uma string de estilo C. A segunda é uma propriedade dos parâmetros de referência `const` discutida anteriormente neste capítulo. Suponha que o tipo do argumento real não corresponda ao tipo do parâmetro de referência, mas possa ser convertido para o tipo de referência. Então o programa cria uma variável temporária do tipo correto, a inicializa com o valor convertido e passa uma referência para a variável temporária. Portanto, um parâmetro `const string &` pode tratar um argumento `char *` ou `const char *`.

A função `versao2()` não cria uma string temporária. Em vez disso, ela diretamente altera a string original:

```cpp
const string & versao2(string & s1, const string & s2)
{
    s1 = s2 + s1 + s2;
    // seguro retornar referência passada para a função
    return s1;
}
```

Como `s1` é uma referência para `entrada` em `main()`, é seguro retornar `s1` como referência. Mas porque `s1` é uma referência para `entrada`, chamar essa função tem o efeito colateral de alterar `entrada` também, como mostra a saída.

A terceira versão na Listagem 8.7 é um lembrete do que não fazer:

```cpp
const string & versao3(string & s1, const string & s2) // design ruim
{
    string temp;
    temp = s2 + s1 + s2;
    // inseguro retornar referência para variável local
    return temp;
}
```

Ela tem a falha fatal de retornar uma referência para uma variável declarada localmente dentro de `versao3()`. Esta função compila (com um aviso), mas o programa trava ao tentar executar a função.

## Objetos, Herança e Referências (Objects, Inheritance, and References)

As classes `ostream` e `ofstream` trazem à tona uma propriedade interessante das referências. Como você deve lembrar do Capítulo 6, objetos do tipo `ofstream` podem usar métodos de `ostream`, permitindo que entrada/saída de arquivo use as mesmas formas que entrada/saída no console. O recurso da linguagem que torna possível passar características de uma classe para outra é chamado de *herança* (inheritance), e o Capítulo 13, "Herança de Classe", discute esse recurso em detalhes. Em resumo, `ostream` é denominado uma *classe base* (base class) (porque a classe `ofstream` é baseada nela) e `ofstream` é denominada uma *classe derivada* (derived class) (porque é derivada de `ostream`). Uma classe derivada herda os métodos da classe base, o que significa que um objeto `ofstream` pode usar recursos da classe base como os métodos de formatação `precision()` e `setf()`.

Outro aspecto da herança é que uma referência de classe base pode se referir a um objeto de classe derivada sem exigir um type cast. A consequência prática disso é que você pode definir uma função com um parâmetro de referência de classe base, e essa função pode ser usada tanto com objetos da classe base quanto com objetos derivados. Por exemplo, uma função com um parâmetro do tipo `ostream &` pode aceitar tanto um objeto `ostream`, como `cout`, quanto um objeto `ofstream`, igualmente.

A Listagem 8.8 demonstra este ponto usando a mesma função para gravar dados em um arquivo e para exibir os dados na tela; apenas o argumento da chamada de função é alterado. Este programa solicita a distância focal de um objetivo de telescópio (seu espelho ou lente principal) e de algumas oculares. Então calcula e exibe a ampliação que cada ocular produziria naquele telescópio.

**Listagem 8.8** filefunc.cpp

```cpp
// filefunc.cpp -- função com parâmetro ostream &
#include <iostream>
#include <fstream>
#include <cstdlib>
using namespace std;
void arquivo_it(ostream & os, double fo, const double oculares[], int n);
const int LIMITE = 5;
int main()
{
    ofstream fsaida;
    const char * nm = "ep-data.txt";
    fsaida.open(nm);
    if (!fsaida.is_open())
    {
        cout << "Nao e possivel abrir " << nm << ". Saindo.\n";
        exit(EXIT_FAILURE);
    }
    double objetivo;
    cout << "Digite a distancia focal do seu "
            "objetivo de telescopio em mm: ";
    cin >> objetivo;
    double oculares[LIMITE];
    cout << "Digite as distancias focais, em mm, de " << LIMITE
         << " oculares:\n";
    for (int i = 0; i < LIMITE; i++)
    {
        cout << "Ocular #" << i + 1 << ": ";
        cin >> oculares[i];
    }
    arquivo_it(fsaida, objetivo, oculares, LIMITE);
    arquivo_it(cout, objetivo, oculares, LIMITE);
    cout << "Concluido\n";
    return 0;
}
void arquivo_it(ostream & os, double fo, const double oculares[], int n)
{
    ios_base::fmtflags inicial;
    inicial = os.setf(ios_base::fixed); // salva estado de formatação inicial
    os.precision(0);
    os << "Distancia focal do objetivo: " << fo << " mm\n";
    os.setf(ios::showpoint);
    os.precision(1);
    os.width(12);
    os << "d.f. ocular";
    os.width(15);
    os << "ampliacao" << endl;
    for (int i = 0; i < n; i++)
    {
        os.width(12);
        os << oculares[i];
        os.width(15);
        os << int(fo / oculares[i] + 0.5) << endl;
    }
    os.setf(inicial); // restaura estado de formatação inicial
}
```

Saída de exemplo:

```
Distancia focal do objetivo: 1800 mm
d.f. ocular    ampliacao
        30.0           60
        19.0           95
        14.0          129
         8.8          205
         7.5          240
```

**Notas do Programa**

O ponto principal da Listagem 8.8 é que o parâmetro `os`, que é do tipo `ostream &`, pode se referir tanto a um objeto `ostream` como `cout` quanto a um objeto `ofstream` como `fsaida`. O programa também ilustra como os métodos de formatação de `ostream` podem ser usados para ambos os tipos.

O método `setf()` permite definir vários estados de formatação. Por exemplo, a chamada de método `setf(ios_base::fixed)` coloca um objeto no modo de usar notação de ponto decimal fixo. A chamada `setf(ios_base::showpoint)` coloca um objeto no modo de exibir um ponto decimal à direita, mesmo que os seguintes dígitos sejam zeros. O método `precision()` indica o número de casas a serem mostradas à direita do ponto decimal (desde que o objeto esteja no modo `fixed`). Todos esses configurações permanecem em vigor a menos que sejam redefinidas por outra chamada de método. A chamada `width()` define a largura de campo a ser usada para a próxima ação de saída. Essa configuração vale para exibir um único valor e depois reverte para o padrão.

A função `arquivo_it()` usa um par interessante de chamadas de método:

```cpp
ios_base::fmtflags inicial;
inicial = os.setf(ios_base::fixed); // salva estado de formatação inicial
...
os.setf(inicial); // restaura estado de formatação inicial
```

O método `setf()` retorna uma cópia de todas as configurações de formatação em vigor antes da chamada ser feita. `ios_base::fmtflags` é um nome sofisticado para o tipo necessário para armazenar essa informação. Portanto, a atribuição a `inicial` armazena as configurações que estavam em vigor antes da função `arquivo_it()` ser chamada. A variável `inicial` pode então ser usada como argumento para `setf()` para redefinir todas as configurações de formatação para esse valor original. Assim, a função restaura o objeto ao estado que tinha antes de ser passado para `arquivo_it()`.

Cada objeto armazena suas próprias configurações de formatação. Portanto, quando o programa passa `cout` para `arquivo_it()`, as configurações de `cout` são alteradas e depois restauradas. Quando o programa passa `fsaida` para `arquivo_it()`, as configurações de `fsaida` são alteradas e depois restauradas.

## Quando Usar Argumentos de Referência (When to Use Reference Arguments)

Há duas razões principais para usar argumentos de referência:

- Para permitir que você altere um objeto de dados na função chamadora
- Para acelerar o programa passando uma referência em vez de um objeto de dados inteiro

A segunda razão é mais importante para objetos de dados maiores, como estruturas e objetos de classe. Essas duas razões são as mesmas que você pode ter para usar um argumento de ponteiro. Isso faz sentido porque argumentos de referência são na verdade apenas uma interface diferente para código baseado em ponteiros. Então quando você deve usar uma referência? Usar um ponteiro? Passar por valor? A seguir estão algumas diretrizes.

**Uma função usa dados passados sem modificá-los:**

- Se o objeto de dados é pequeno, como um tipo de dados embutido ou uma pequena estrutura, passe-o por valor.
- Se o objeto de dados é um array, use um ponteiro porque é sua única opção. Torne o ponteiro um ponteiro para `const`.
- Se o objeto de dados é uma estrutura de boa dimensão, use um ponteiro `const` ou uma referência `const` para aumentar a eficiência do programa.
- Se o objeto de dados é um objeto de classe, use uma referência `const`. A semântica do design de classe frequentemente requer usar uma referência, que é a principal razão pela qual o C++ adicionou esse recurso.

**Uma função modifica dados na função chamadora:**

- Se o objeto de dados é um tipo de dados embutido, use um ponteiro.
- Se o objeto de dados é um array, use sua única opção: um ponteiro.
- Se o objeto de dados é uma estrutura, use uma referência ou um ponteiro.
- Se o objeto de dados é um objeto de classe, use uma referência.

É claro que essas são apenas diretrizes, e pode haver razões para fazer escolhas diferentes. Por exemplo, `cin` usa referências para tipos básicos para que você possa usar `cin >> n` em vez de `cin >> &n`.

---

Navegação: [Anterior](capitulo-07-04-recursao-ponteiros-resumo.md) | [Índice](README.md) | [Próximo](capitulo-08-02-argumentos-padrao-sobrecarga.md)
