# Capítulo 18 — Visitando o Novo Padrão C++ (parte 3)

> Tradução não oficial de *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Navegação: [Anterior](capitulo-18-02-move-semantics-novidades-classes.md) | [Índice](README.md) | [Próximo](capitulo-18-04-variadicos-mais-c11-resumo.md)

## Funções Lambda

Quando você vê o termo funções lambda (lambda functions; também conhecidas como expressões lambda, ou simplesmente lambdas), pode suspeitar que esta não é uma das adições do C++11 destinadas a ajudar o programador iniciante. Você terá suas suspeitas aparentemente confirmadas quando ver como as funções lambda realmente parecem — aqui está um exemplo:

```cpp
[&contagem](int x){contagem += (x % 13 == 0);}
```

Mas elas não são tão arcanas quanto podem parecer, e fornecem um serviço útil, particularmente com algoritmos STL que usam predicados de função.

### O Como de Ponteiros de Função, Funtores e Lambdas

Vamos examinar um exemplo usando três abordagens para passar informações a um algoritmo STL: ponteiros de função, funtores e lambdas. (Para conveniência, nos referiremos a essas três formas como objetos de função, para não precisar continuar repetindo "ponteiro de função ou funtor ou lambda.") Suponha que você deseje gerar uma lista de inteiros aleatórios e determinar quantos deles são divisíveis por 3 e quantos são divisíveis por 13.

Gerar a lista é bastante simples. Uma opção é usar um array `vector<int>` para conter os números e usar o algoritmo STL `generate()` para abastecer o array com números aleatórios:

```cpp
#include <vector>
#include <algorithm>
#include <cmath>
// ...
std::vector<int> numeros(1000);
std::generate(numeros.begin(), numeros.end(), std::rand);
```

A função `generate()` recebe um intervalo, especificado pelos dois primeiros argumentos, e define cada elemento com o valor retornado pelo terceiro argumento, que é um objeto de função que não recebe argumentos. Nesse caso, o objeto de função é um ponteiro para a função padrão `rand()`.

Com a ajuda do algoritmo `count_if()`, é fácil contar o número de elementos divisíveis por 3. Os dois primeiros argumentos devem especificar o intervalo, assim como para `generate()`. O terceiro argumento deve ser um objeto de função que retorne `true` ou `false`. A função `count_if()` então conta todos os elementos para os quais o objeto de função retorna `true`. Para encontrar elementos divisíveis por 3, você pode usar esta definição de função:

```cpp
bool div3(int x) {return x % 3 == 0;}
```

Da mesma forma, você pode usar a seguinte definição de função para encontrar elementos divisíveis por 13:

```cpp
bool div13(int x) {return x % 13 == 0;}
```

Com essas definições, você pode contar elementos da seguinte forma:

```cpp
int cont3 = std::count_if(numeros.begin(), numeros.end(), div3);
cout << "Contagem de números divisíveis por 3: " << cont3 << '\n';
int cont13 = std::count_if(numeros.begin(), numeros.end(), div13);
cout << "Contagem de números divisíveis por 13: " << cont13 << "\n\n";
```

Em seguida, vamos revisar como realizar a mesma tarefa usando um funtor. Um funtor, como você se lembrará do Capítulo 16, é um objeto de classe que pode ser usado como se fosse um nome de função, graças à classe definir `operator()()` como um método de classe. Uma vantagem do funtor em nosso exemplo é que você pode usar o mesmo funtor para ambas as tarefas de contagem. Aqui está uma definição possível:

```cpp
class f_mod
{
private:
    int dv;
public:
    f_mod(int d = 1) : dv(d) {}
    bool operator()(int x) {return x % dv == 0;}
};
```

Você pode usar o construtor para criar um objeto `f_mod` armazenando um valor inteiro específico:

```cpp
f_mod obj(3); // f_mod.dv definido como 3
```

Esse objeto pode usar o método `operator()` para retornar um valor `bool`:

```cpp
bool e_div_por_3 = obj(7); // mesmo que obj.operator()(7)
```

O próprio construtor pode ser usado como argumento para funções como `count_if()`:

```cpp
cont3 = std::count_if(numeros.begin(), numeros.end(), f_mod(3));
```

O argumento `f_mod(3)` cria um objeto armazenando o valor `3`, e `count_if()` usa o objeto criado para chamar o método `operator()()`, definindo o parâmetro `x` igual a um elemento de `numeros`. Para contar quantos números são divisíveis por `13` em vez de `3`, use `f_mod(13)` como terceiro argumento.

Por fim, vamos examinar a abordagem lambda. O nome vem do cálculo lambda, um sistema matemático para definir e aplicar funções. O sistema permite o uso de funções anônimas — ou seja, permite dispensar os nomes de funções. No contexto do C++11, você pode usar uma definição de função anônima (um lambda) como argumento para funções que esperam um ponteiro de função ou funtor. O lambda correspondente à função `div3()` é este:

```cpp
[](int x) {return x % 3 == 0;}
```

Ele parece muito com a definição de `div3()`:

```cpp
bool div3(int x) {return x % 3 == 0;}
```

As duas diferenças são que o nome da função é substituído por `[]` (quão anônimo é isso!) e que não há tipo de retorno declarado. Em vez disso, o tipo de retorno é o tipo que `decltype` deduziria do valor de retorno, o que seria `bool` nesse caso. Se o lambda não tiver uma instrução `return`, o tipo é deduzido como `void`. Em nosso exemplo, você usaria este lambda da seguinte forma:

```cpp
cont3 = std::count_if(numeros.begin(), numeros.end(),
     [](int x){return x % 3 == 0;});
```

Ou seja, você usa a expressão lambda inteira como usaria um ponteiro ou um construtor de funtor.

A dedução automática de tipo para lambdas funciona apenas se o corpo consistir em uma única instrução `return`. Caso contrário, você precisa usar a nova sintaxe de tipo de retorno à direita:

```cpp
[](double x)->double{int y = x; return x - y;} // tipo de retorno é double
```

A Listagem 18.4 ilustra os pontos discutidos.

**Listagem 18.4 — `lambda0.cpp`**

```cpp
// lambda0.cpp -- usando expressões lambda
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <ctime>
const long Tam1 = 39L;
const long Tam2 = 100 * Tam1;
const long Tam3 = 100 * Tam2;

bool div3(int x)  {return x % 3  == 0;}
bool div13(int x) {return x % 13 == 0;}
int main()
{
    using std::cout;
    std::vector<int> numeros(Tam1);
    std::srand(std::time(0));
    std::generate(numeros.begin(), numeros.end(), std::rand);

    // usando ponteiros de função
    cout << "Tamanho da amostra = " << Tam1 << '\n';
    int cont3 = std::count_if(numeros.begin(), numeros.end(), div3);
    cout << "Contagem de números divisíveis por 3: " << cont3 << '\n';
    int cont13 = std::count_if(numeros.begin(), numeros.end(), div13);
    cout << "Contagem de números divisíveis por 13: " << cont13 << "\n\n";

    // aumentar o número de números
    numeros.resize(Tam2);
    std::generate(numeros.begin(), numeros.end(), std::rand);
    cout << "Tamanho da amostra = " << Tam2 << '\n';

    // usando um funtor
    class f_mod
    {
    private:
        int dv;
    public:
        f_mod(int d = 1) : dv(d) {}
        bool operator()(int x) {return x % dv == 0;}
    };
    cont3 = std::count_if(numeros.begin(), numeros.end(), f_mod(3));
    cout << "Contagem de números divisíveis por 3: " << cont3 << '\n';
    cont13 = std::count_if(numeros.begin(), numeros.end(), f_mod(13));
    cout << "Contagem de números divisíveis por 13: " << cont13 << "\n\n";

    // aumentar o número de números novamente
    numeros.resize(Tam3);
    std::generate(numeros.begin(), numeros.end(), std::rand);
    cout << "Tamanho da amostra = " << Tam3 << '\n';

    // usando lambdas
    cont3 = std::count_if(numeros.begin(), numeros.end(),
         [](int x){return x % 3 == 0;});
    cout << "Contagem de números divisíveis por 3: " << cont3 << '\n';
    cont13 = std::count_if(numeros.begin(), numeros.end(),
         [](int x){return x % 13 == 0;});
    cout << "Contagem de números divisíveis por 13: " << cont13 << '\n';
    return 0;
}
```

Eis uma saída de exemplo:

```
Tamanho da amostra = 39
Contagem de números divisíveis por 3: 15
Contagem de números divisíveis por 13: 6

Tamanho da amostra = 3900
Contagem de números divisíveis por 3: 1305
Contagem de números divisíveis por 13: 302
Tamanho da amostra = 390000
Contagem de números divisíveis por 3: 130241
Contagem de números divisíveis por 13: 29860
```

### O Porquê dos Lambdas

Você pode estar se perguntando que necessidade, além da expressão artística nerd, o lambda serve. Vamos examinar essa questão em termos de quatro qualidades: proximidade, brevidade, eficiência e capacidade.

Muitos programadores sentem que é útil localizar as definições perto de onde são usadas. Dessa forma, você não precisa percorrer páginas de código-fonte para descobrir, digamos, o que o terceiro argumento de uma chamada `count_if()` realiza. Além disso, se você precisar modificar o código, todos os componentes estão à mão. E se você recortar e colar o código para uso em outro lugar, novamente todos os componentes estão à mão. Desse ponto de vista, os lambdas são ideais porque a definição está no ponto de uso. As funções são as piores porque funções não podem ser definidas dentro de outras funções, portanto a definição estará localizada possivelmente bem distante do ponto de uso. Os funtores podem ser bastante bons porque uma classe, incluindo uma classe funtor, pode ser definida dentro de uma função, portanto a definição pode ser localizada perto do ponto de uso.

Em termos de brevidade, o código do funtor é mais verboso do que o código equivalente de função ou lambda. Funções e lambdas são aproximadamente igualmente breves. Uma aparente exceção seria se você precisasse usar um lambda duas vezes:

```cpp
cont1 = std::count_if(n1.begin(), n1.end(),
     [](int x){return x % 3 == 0;});
cont2 = std::count_if(n2.begin(), n2.end(),
     [](int x){return x % 3 == 0;});
```

Mas você não precisa realmente escrever o lambda duas vezes. Essencialmente, você pode criar um nome para o lambda anônimo e depois usar o nome duas vezes:

```cpp
auto mod3 = [](int x){return x % 3 == 0;} // mod3 é um nome para o lambda
cont1 = std::count_if(n1.begin(), n1.end(), mod3);
cont2 = std::count_if(n2.begin(), n2.end(), mod3);
```

Você pode até usar esse lambda não mais anônimo como uma função comum:

```cpp
bool resultado = mod3(z); // resultado é true se z % 3 == 0
```

Ao contrário de uma função comum, porém, um lambda nomeado pode ser definido dentro de uma função. O tipo real de `mod3` será algum tipo dependente de implementação que o compilador usa para rastrear lambdas.

A eficiência relativa das três abordagens se resume ao que o compilador decide inline. Aqui, a abordagem de ponteiro de função é prejudicada pelo fato de que os compiladores tradicionalmente não fazem inline de uma função cujo endereço foi obtido, porque o conceito de endereço de função implica uma função não inline. Com funtores e lambdas, não há contradição aparente com inlining.

Por fim, lambdas têm algumas capacidades adicionais. Em particular, um lambda pode acessar pelo nome qualquer variável automática no escopo. As variáveis a serem usadas são capturadas (captured) por ter seus nomes listados entre colchetes. Se apenas o nome for usado, como em `[z]`, a variável é acessada por valor. Se o nome for precedido por `&`, como em `[&contagem]`, a variável é acessada por referência. Usar `[&]` fornece acesso a todas as variáveis automáticas por referência, e `[=]` fornece acesso a todas as variáveis automáticas por valor. Você também pode misturar. Por exemplo, `[ted, &ed]` forneceria acesso a `ted` por valor e a `ed` por referência, `[&, ted]` forneceria acesso a `ted` por valor e a todas as outras variáveis automáticas por referência, e `[=, &ed]` forneceria acesso por referência a `ed` e por valor às demais variáveis automáticas.

Na Listagem 18.4, você pode substituir:

```cpp
int cont13;
// ...
cont13 = std::count_if(numeros.begin(), numeros.end(),
      [](int x){return x % 13 == 0;});
```

por isto:

```cpp
int cont13 = 0;
std::for_each(numeros.begin(), numeros.end(),
   [&cont13](int x){cont13 += x % 13 == 0;});
```

O `[&cont13]` permite ao lambda usar `cont13` em seu código. Como `cont13` é capturado por referência, qualquer mudança em `cont13` no lambda é uma mudança no `cont13` original. A expressão `x % 13 == 0` é avaliada como `true` se `x` é divisível por 13, e `true` é convertido para `1` quando adicionado a `cont13`. Da mesma forma, `false` é convertido para `0`.

Você pode usar essa técnica para contar elementos divisíveis por 3 e elementos divisíveis por 13 usando uma única expressão lambda:

```cpp
int cont3 = 0;
int cont13 = 0;
std::for_each(numeros.begin(), numeros.end(),
   [&](int x){cont3 += x % 3 == 0; cont13 += x % 13 == 0;});
```

Desta vez, `[&]` torna todas as variáveis automáticas, incluindo `cont3` e `cont13`, disponíveis para a expressão lambda.

A Listagem 18.5 coloca essas técnicas em uso.

**Listagem 18.5 — `lambda1.cpp`**

```cpp
// lambda1.cpp -- usar variáveis capturadas
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <ctime>
const long Tam = 390000L;
int main()
{
    using std::cout;
    std::vector<int> numeros(Tam);
    std::srand(std::time(0));
    std::generate(numeros.begin(), numeros.end(), std::rand);
    cout << "Tamanho da amostra = " << Tam << '\n';
    // usando lambdas
    int cont3 = std::count_if(numeros.begin(), numeros.end(),
         [](int x){return x % 3 == 0;});
    cout << "Contagem de números divisíveis por 3: " << cont3 << '\n';
    int cont13 = 0;
    std::for_each(numeros.begin(), numeros.end(),
       [&cont13](int x){cont13 += x % 13 == 0;});
    cout << "Contagem de números divisíveis por 13: " << cont13 << '\n';
    // usando um único lambda
    cont3 = cont13 = 0;
    std::for_each(numeros.begin(), numeros.end(),
       [&](int x){cont3 += x % 3 == 0; cont13 += x % 13 == 0;});
    cout << "Contagem de números divisíveis por 3: " << cont3 << '\n';
    cout << "Contagem de números divisíveis por 13: " << cont13 << '\n';
    return 0;
}
```

Eis uma saída de exemplo:

```
Tamanho da amostra = 390000
Contagem de números divisíveis por 3: 130274
Contagem de números divisíveis por 13: 30009
Contagem de números divisíveis por 3: 130274
Contagem de números divisíveis por 13: 30009
```

É reconfortante que ambas as abordagens (dois lambdas separados e um único lambda) neste programa levem às mesmas respostas.

A principal motivação para adicionar lambdas ao C++ foi habilitar o uso de uma expressão semelhante a uma função como argumento para uma função que espera um ponteiro de função ou funtor como argumento. Portanto, o lambda típico seria uma expressão de teste ou uma expressão de comparação que poderia ser escrita como uma única instrução `return`. Isso mantém o lambda curto e fácil de entender e habilita a dedução automática do valor de retorno.

---

## Wrappers

O C++ fornece vários wrappers ou adaptadores. São objetos usados para fornecer uma interface mais uniforme ou mais apropriada para outros elementos de programação. Por exemplo, o Capítulo 16 descreveu `bind1st` e `bind2nd`, que adaptam funções com dois parâmetros para combinar com algoritmos STL que esperam funções com um parâmetro a serem fornecidas como argumento. O C++11 fornece wrappers adicionais. Eles incluem o template `bind`, que fornece uma alternativa mais flexível a `bind1st` e `bind2nd`, o template `mem_fn`, que permite que uma função-membro passe como uma função regular, o template `reference_wrapper`, que permite criar um objeto que age como referência mas que pode ser copiado, e o wrapper `function`, que fornece uma maneira de tratar diversas formas semelhantes a funções de forma uniforme.

Vamos examinar mais de perto um exemplo de wrapper, o wrapper `function`, e o problema que ele aborda.

### O Wrapper `function` e Ineficiências de Template

Considere a seguinte linha de código:

```cpp
resposta = ef(q);
```

O que é `ef`? Pode ser o nome de uma função. Pode ser um ponteiro para uma função. Pode ser um objeto de função. Pode ser um nome atribuído a uma expressão lambda. Todos esses são exemplos de tipos chamáveis (callable types). A abundância de tipos chamáveis pode levar a ineficiências de template. Para ver isso, vamos examinar um caso simples.

Primeiro, vamos definir alguns templates em um arquivo de cabeçalho, como mostrado na Listagem 18.6.

**Listagem 18.6 — `algundefs.h`**

```cpp
// algundefs.h
#include <iostream>
template <typename T, typename F>
T usar_f(T v, F f)
{
    static int contagem = 0;
    contagem++;
    std::cout << " contagem de usar_f = " << contagem
          << ", &contagem = " << &contagem << std::endl;
    return f(v);
}
class Fp
{
private:
    double z_;
public:
    Fp(double z = 1.0) : z_(z) {}
    double operator()(double p) { return z_ * p; }
};
class Fq
{
private:
    double z_;
public:
    Fq(double z = 1.0) : z_(z) {}
    double operator()(double q) { return z_ + q; }
};
```

O template `usar_f()` usa o parâmetro `f` para representar um tipo chamável:

```cpp
return f(v);
```

Em seguida, o programa na Listagem 18.7 chama a função template `usar_f()` seis vezes.

**Listagem 18.7 — `chamavel.cpp`**

```cpp
// chamavel.cpp -- tipos chamáveis e templates
#include "algundefs.h"
#include <iostream>
double dobro(double x)   {return 2.0 * x;}
double quadrado(double x) {return x * x;}

int main()
{
    using std::cout;
    using std::endl;
    double y = 1.21;
    cout << "Ponteiro de função dobro:\n";
    cout << " " << usar_f(y, dobro) << endl;
    cout << "Ponteiro de função quadrado:\n";
    cout << " " << usar_f(y, quadrado) << endl;
    cout << "Objeto de função Fp:\n";
    cout << " " << usar_f(y, Fp(5.0)) << endl;
    cout << "Objeto de função Fq:\n";
    cout << " " << usar_f(y, Fq(5.0)) << endl;
    cout << "Expressão lambda 1:\n";
    cout << " " << usar_f(y, [](double u) {return u*u;}) << endl;
    cout << "Expressão lambda 2:\n";
    cout << " " << usar_f(y, [](double u) {return u+u/2.0;}) << endl;
    return 0;
}
```

O parâmetro de template `T` é definido como tipo `double` para cada chamada. Quanto ao parâmetro de template `F`? Cada vez que o argumento real é algo que recebe um argumento do tipo `double` e retorna um valor do tipo `double`, poderia parecer que `F` seria o mesmo tipo para todas as seis chamadas a `usar_f()` e que o template seria instanciado apenas uma vez. Mas, como a saída de exemplo a seguir mostra, essa crença é ingênua:

```
Ponteiro de função dobro:
  contagem de usar_f = 1, &contagem = 0x402028
  2.42
Ponteiro de função quadrado:
  contagem de usar_f = 2, &contagem = 0x402028
  1.1
Objeto de função Fp:
  contagem de usar_f = 1, &contagem = 0x402020
  6.05
Objeto de função Fq:
  contagem de usar_f = 1, &contagem = 0x402024
  6.21
Expressão lambda 1:
  contagem de usar_f = 1, &contagem = 0x405020
  1.4641
Expressão lambda 2:
  contagem de usar_f = 1, &contagem = 0x40501c
  1.815
```

A função template `usar_f()` tem um membro estático `contagem`, e podemos usar seu endereço para ver quantas instanciações foram feitas. Há cinco endereços distintos, portanto deve ter havido cinco instanciações distintas do template `usar_f()`.

### Corrigindo o Problema

O wrapper `function` permite que você reescreva o programa para que use apenas uma instanciação de `usar_f()` em vez de cinco. Observe que os ponteiros de função, os objetos de função e as expressões lambda na Listagem 18.7 compartilham um comportamento comum — cada um recebe um argumento do tipo `double` e cada um retorna um valor do tipo `double`. Podemos dizer que cada um tem a mesma assinatura de chamada (call signature), que é descrita pelo tipo de retorno seguido de uma lista separada por vírgulas de tipos de parâmetros entre um par de parênteses. Assim, esses seis exemplos têm `double(double)` como assinatura de chamada.

O template `function`, declarado no arquivo de cabeçalho `functional`, especifica um objeto em termos de uma assinatura de chamada, e pode ser usado para encapsular um ponteiro de função, objeto de função ou expressão lambda com a mesma assinatura de chamada. Por exemplo, a declaração a seguir cria um objeto de função `fdci` que recebe um argumento `char` e `int` e retorna o tipo `double`:

```cpp
std::function<double(char, int)> fdci;
```

Você pode então atribuir a `fdci` qualquer ponteiro de função, objeto de função ou expressão lambda que receba argumentos do tipo `char` e `int` e retorne o tipo `double`.

Os vários argumentos chamáveis na Listagem 18.7 têm todos a mesma assinatura de chamada — `double(double)`. Para corrigir a Listagem 18.7 e reduzir o número de instanciações, podemos usar `function<double(double)>` para criar seis wrappers para as seis funções, funtores e lambdas. Em seguida, todas as seis chamadas a `usar_f()` podem ser feitas com o mesmo tipo (`function<double(double)>`) para `F`, resultando em apenas uma instanciação. A Listagem 18.8 mostra o resultado.

**Listagem 18.8 — `encapsulado.cpp`**

```cpp
// encapsulado.cpp -- usando um wrapper function como argumento
#include "algundefs.h"
#include <iostream>
#include <functional>
double dobro(double x)   {return 2.0 * x;}
double quadrado(double x) {return x * x;}

int main()
{
    using std::cout;
    using std::endl;
    using std::function;
    double y = 1.21;
    function<double(double)> ef1 = dobro;
    function<double(double)> ef2 = quadrado;
    function<double(double)> ef3 = Fq(10.0);
    function<double(double)> ef4 = Fp(10.0);
    function<double(double)> ef5 = [](double u) {return u*u;};
    function<double(double)> ef6 = [](double u) {return u+u/2.0;};
    cout << "Ponteiro de função dobro:\n";
    cout << " " << usar_f(y, ef1) << endl;
    cout << "Ponteiro de função quadrado:\n";
    cout << " " << usar_f(y, ef2) << endl;
    cout << "Objeto de função Fp:\n";
    cout << " " << usar_f(y, ef3) << endl;
    cout << "Objeto de função Fq:\n";
    cout << " " << usar_f(y, ef4) << endl;
    cout << "Expressão lambda 1:\n";
    cout << " " << usar_f(y, ef5) << endl;
    cout << "Expressão lambda 2:\n";
    cout << " " << usar_f(y, ef6) << endl;
    return 0;
}
```

Eis uma saída de exemplo:

```
Ponteiro de função dobro:
  contagem de usar_f = 1, &contagem = 0x404020
  2.42
Ponteiro de função quadrado:
  contagem de usar_f = 2, &contagem = 0x404020
  1.1
Objeto de função Fp:
  contagem de usar_f = 3, &contagem = 0x404020
  11.21
Objeto de função Fq:
  contagem de usar_f = 4, &contagem = 0x404020
  12.1
Expressão lambda 1:
  contagem de usar_f = 5, &contagem = 0x404020
  1.4641
Expressão lambda 2:
  contagem de usar_f = 6, &contagem = 0x404020
  1.815
```

Como você pode ver na saída, há apenas um endereço para `contagem`, e o valor de `contagem` mostra que `usar_f()` foi chamada seis vezes. Portanto, agora temos apenas uma instanciação invocada seis vezes, reduzindo o tamanho do código executável.

### Mais Opções

Vamos ver mais algumas coisas que você pode fazer usando `function`. Primeiro, não precisamos realmente declarar seis objetos `function<double(double)>` na Listagem 18.8. Em vez disso, podemos usar um objeto temporário `function<double(double)>` como argumento para a função `usar_f()`:

```cpp
typedef function<double(double)> fdd; // simplificar a declaração de tipo
cout << usar_f(y, fdd(dobro)) << endl;    // criar e inicializar objeto como dobro
cout << usar_f(y, fdd(quadrado)) << endl;
// ...
```

Segundo, a Listagem 18.8 adapta os segundos argumentos em `usar_f()` para corresponder ao parâmetro formal `f`. Outra abordagem é adaptar o tipo do parâmetro formal `f` para corresponder aos argumentos originais. Isso pode ser feito usando um objeto wrapper `function` como segundo parâmetro para a definição do template `usar_f()`. Podemos definir `usar_f()` desta forma:

```cpp
#include <functional>
template <typename T>
T usar_f(T v, std::function<T(T)> f) // assinatura de chamada de f é T(T)
{
    static int contagem = 0;
    contagem++;
    std::cout << " contagem de usar_f = " << contagem
          << ", &contagem = " << &contagem << std::endl;
    return f(v);
}
```

Em seguida, as chamadas de função podem se parecer com isto:

```cpp
cout << " " << usar_f<double>(y, dobro) << endl;
// ...
cout << " " << usar_f<double>(y, Fp(5.0)) << endl;
// ...
cout << " " << usar_f<double>(y, [](double u) {return u*u;}) << endl;
```

Os argumentos `dobro`, `Fp(5.0)`, etc., não são em si mesmos do tipo `function<double(double)>`, portanto as chamadas usam `<double>` após `usar_f` para indicar a especialização desejada. Assim, `T` é definido como `double` e `std::function<T(T)>` torna-se `std::function<double(double)>`.

---

[Anterior](capitulo-18-02-move-semantics-novidades-classes.md) | [Índice](README.md) | [Próximo](capitulo-18-04-variadicos-mais-c11-resumo.md)
