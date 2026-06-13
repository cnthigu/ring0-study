# Capítulo 15 — Amigos, Exceções e Mais (Friends, Exceptions, and More) — Parte 2: Exceções Avançadas

> Tradução não oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Navegação: [Anterior](capitulo-15-01-amigos-excecoes.md) | [Índice](README.md) | [Próximo](capitulo-15-03-rtti-casts-resumo.md)

## Notas do Programa (erro5.cpp)

Vamos rastrear o curso da execução de exemplo mostrada na seção anterior. Primeiro, conforme o construtor de `demo` anuncia, um objeto é criado no bloco em `main()`. Em seguida, `medias()` é chamada, e outro objeto `demo` é criado. A função `medias()` passa os valores `6` e `12` para `mharm()` e `mgeom()`, e essas funções retornam valores para `medias()`, que calcula um resultado e o retorna. Antes de retornar o resultado, `medias()` invoca `d2.exibir()`. Após retornar o resultado, `medias()` termina, e o destrutor de `d2` é chamado automaticamente:

```
demo encontrado em medias() vive!
demo encontrado em medias() destruido
```

O próximo ciclo de entrada envia os valores `6` e `-6` para `medias()`, e então `medias()` cria um novo objeto `demo` e repassa os valores para `mharm()`. A função `mharm()` então lança uma exceção `erro_mharm`, que é capturada pelo bloco `catch` em `medias()`, como mostrado pela seguinte saída:

```
mharm(6, -6): argumentos invalidos: a = -b
Capturado em medias()
```

A instrução `throw` nesse bloco então termina `medias()` e envia a exceção para cima para `main()`. O fato de que `d2.exibir()` não é chamado demonstra que a execução de `medias()` foi encerrada. Mas note que o destrutor de `d2` é chamado:

```
demo encontrado em medias() destruido
```

Isso ilustra um aspecto extremamente importante das exceções: à medida que o programa desdobra a pilha para alcançar onde uma exceção é capturada, ele libera variáveis de classe de armazenamento automático na pilha. Se uma variável for um objeto de classe, então o destrutor desse objeto é chamado.

Enquanto isso, a exceção relançada chega a `main()`, onde o bloco `catch` apropriado a captura e trata:

```
mharm(6, -6): argumentos invalidos: a = -b
Tente novamente.
```

Agora o terceiro ciclo de entrada começa, com `6` e `-8` enviados para `medias()`. Mais uma vez, `medias()` cria um novo objeto `demo`. Ele passa `6` e `-8` para `mharm()`, que os processa sem problema. Então `medias()` passa `6` e `-8` para `mgeom()`, que lança uma exceção `erro_mgeom`. Como `medias()` não captura exceções `erro_mgeom`, a exceção é passada para cima para `main()`, e nenhum código adicional em `medias()` é executado. No entanto, mais uma vez, à medida que o programa desdobra a pilha, ele libera variáveis automáticas locais, então o destrutor de `d2` é chamado:

```
demo encontrado em medias() destruido
```

Finalmente, o handler de `erro_mgeom` em `main()` captura a exceção e encerra o loop:

```
argumentos de mgeom() devem ser >= 0
Valores usados: 6, -8
Desculpe, voce nao pode mais jogar.
```

Então o programa termina normalmente, exibindo algumas mensagens e chamando automaticamente o destrutor de `d1`. Se o bloco `catch` usasse, digamos, `exit(EXIT_FAILURE)` em vez de `break`, o programa terminaria imediatamente, então você não veria estas mensagens:

```
demo encontrado no bloco em main() vive!
Ate logo!
```

No entanto, você ainda veria esta mensagem:

```
demo encontrado no bloco em main() destruido
```

Mais uma vez, o mecanismo de exceção cuidaria de liberar variáveis automáticas na pilha.

## Mais Características de Exceções (More Exception Features)

Embora o mecanismo throw-catch seja semelhante aos argumentos de função e ao mecanismo de retorno de função, há algumas diferenças. Uma, que você já encontrou, é que uma instrução `return` em uma função `fun()` transfere a execução para a função que chamou `fun()`, mas um `throw` transfere a execução até a primeira função que possui uma combinação try-catch que captura a exceção. Por exemplo, na Listagem 15.12, quando `mharm()` lança uma exceção, o controle passa para `medias()`, mas quando `mgeom()` lança uma exceção, o controle passa para `main()`.

Outra diferença é que o compilador sempre cria uma cópia temporária ao lançar uma exceção, mesmo que o especificador de exceção e os blocos `catch` especifiquem uma referência. Por exemplo, considere este código:

```cpp
class problema {...};
...
void super() throw(problema)
{
    ...
    if (uh_oh)
    {
        problema oops;   // constrói objeto
        throw oops;      // lança-o
    ...
    }
    ...
}
try {
    super();
}
catch(problema & p)
{
    // instruções
}
```

Aqui, `p` se referiria a uma cópia de `oops` em vez de a `oops` em si. Isso é bom porque `oops` não existe mais após `super()` terminar. A propósito, é mais simples combinar a construção com o `throw`:

```cpp
throw problema();    // constrói e lança objeto problema padrão
```

A esse ponto você pode se perguntar por que o código usa uma referência se o `throw` gera uma cópia. Afinal, a razão usual para usar um valor de retorno de referência é a eficiência de não ter que fazer uma cópia. A resposta é que referências têm outra propriedade importante: uma referência de classe base pode também se referir a objetos de classes derivadas. Suponha que você tenha uma coleção de tipos de exceção relacionados por herança. Nesse caso, a especificação de exceção precisa listar apenas uma referência ao tipo base, e isso também corresponderia a quaisquer objetos derivados lançados.

Suponha que você tenha uma hierarquia de classes de exceções e queira tratar os diferentes tipos separadamente. Uma referência de classe base pode capturar todos os objetos de uma família, mas um objeto de classe derivada pode capturar apenas aquele objeto e objetos de classes derivadas daquela classe. Um objeto lançado é capturado pelo primeiro bloco `catch` que corresponde a ele. Isso sugere organizar os blocos `catch` em ordem inversa de derivação:

```cpp
class ruim_1 {...};
class ruim_2 : public ruim_1 {...};
class ruim_3 : public ruim_2 {...};
...
void super()
{
    ...
    if (uh_oh)
        throw ruim_1();
    if (ratos)
        throw ruim_2();
    if (droga)
        throw ruim_3();
}
...
try {
    super();
}
catch(ruim_3 & be)
{ // instruções }
catch(ruim_2 & be)
{ // instruções }
catch(ruim_1 & be)
{ // instruções }
```

Se o handler de `ruim_1 &` estivesse primeiro, ele capturaria as exceções `ruim_1`, `ruim_2` e `ruim_3`. Com a ordem inversa, uma exceção `ruim_3` seria capturada pelo handler de `ruim_3 &`.

> **Dica:** Se você tiver uma hierarquia de herança de classes de exceção, deverá organizar a ordem dos blocos `catch` de forma que a exceção da classe mais derivada (isto é, a classe mais distante na sequência da hierarquia de classes) seja capturada primeiro e a exceção de classe base seja capturada por último.

Organizar os blocos `catch` na sequência adequada permite que você seja seletivo sobre como cada tipo de exceção é tratado. Mas às vezes você pode não saber qual tipo de exceção esperar. Por exemplo, digamos que você escreva uma função que chama outra função, e você não sabe se essa outra função lança exceções. Você ainda pode capturar a exceção mesmo que não saiba o tipo. O truque para capturar qualquer exceção é usar uma elipse para o tipo de exceção:

```cpp
catch (...) { // instruções }    // captura exceção de qualquer tipo
```

Se você conhece algumas exceções que são lançadas, você pode colocar essa forma catch-all no final da sequência de blocos `catch`, um pouco como o caso `default` de um `switch`:

```cpp
try {
    super();
}
catch(ruim_3 & be)
{ // instruções }
catch(ruim_2 & be)
{ // instruções }
catch(ruim_1 & be)
{ // instruções }
catch(erro_mharm & h)
{ // instruções }
catch (...)    // captura o que sobrar
{ // instruções }
```

Você pode configurar um handler para capturar um objeto em vez de uma referência. Um objeto de classe base capturará um objeto de classe derivada, mas os aspectos derivados serão removidos. Assim, versões de classe base de métodos virtuais serão usadas.

## A Classe exception

A intenção principal das exceções de C++ é fornecer suporte de nível de linguagem para projetar programas tolerantes a falhas. Ou seja, as exceções tornam mais fácil incorporar tratamento de erros em um design de programa para que você não precise acrescentar alguma forma mais rígida de tratamento de erros como uma reflexão tardia. A flexibilidade e relativa conveniência das exceções devem encorajar os programadores a integrar o tratamento de falhas no processo de design do programa, quando apropriado. Em resumo, exceções são o tipo de recurso que, como classes, pode modificar sua abordagem de programação.

Os compiladores C++ mais novos estão incorporando exceções à linguagem. Por exemplo, o arquivo de cabeçalho `exception` (anteriormente `exception.h` ou `except.h`) define uma classe `exception` que C++ usa como classe base para outras classes de exceção usadas para suportar a linguagem. Seu código também pode lançar um objeto `exception` ou usar a classe `exception` como classe base. Uma função membro virtual é chamada `what()`, e retorna uma string, cuja natureza depende da implementação. No entanto, como esse método é virtual, você pode redefini-lo em uma classe derivada de `exception`:

```cpp
#include <exception>
class erro_mharm : public std::exception
{
public:
    const char * what() { return "argumentos invalidos para mharm()"; }
    ...
};
class erro_mgeom : public std::exception
{
public:
    const char * what() { return "argumentos invalidos para mgeom()"; }
    ...
};
```

Se você não quiser tratar essas exceções derivadas de forma diferente uma da outra, você pode capturá-las com o mesmo handler de classe base:

```cpp
try {
    ...
}
catch(std::exception & e)
{
    cout << e.what() << endl;
    ...
}
```

Ou você poderia capturar os diferentes tipos separadamente. A biblioteca C++ define muitos tipos de exceção baseados em `exception`.

## As Classes de Exceção de stdexcept

O arquivo de cabeçalho `stdexcept` define várias classes de exceção adicionais. Primeiro, o arquivo define as classes `logic_error` e `runtime_error`, ambas derivando publicamente de `exception`:

```cpp
class logic_error : public exception {
public:
    explicit logic_error(const string& what_arg);
    ...
};
class domain_error : public logic_error {
public:
    explicit domain_error(const string& what_arg);
    ...
};
```

Observe que os construtores recebem um objeto `string` como argumento; esse argumento fornece os dados de caracteres retornados como uma string de estilo C pelo método `what()`.

Essas duas novas classes servem, por sua vez, como bases para duas famílias de classes derivadas. A família `logic_error` descreve, como você poderia esperar, erros de lógica típicos. Em princípio, uma boa programação poderia evitar tais erros, mas na prática, tais erros podem aparecer. O nome de cada classe indica o tipo de erro que ela pretende reportar:

- `domain_error`
- `invalid_argument`
- `length_error`
- `out_of_bounds`

Cada classe tem um construtor como o de `logic_error` que permite fornecer a string a ser retornada pelo método `what()`.

Uma amplificação pode ser útil. Uma função matemática tem um domínio e uma imagem. O domínio consiste nos valores para os quais a função está definida, e a imagem consiste nos valores que a função retorna. Por exemplo, o domínio da função seno vai de infinito negativo a infinito positivo porque o seno é definido para todos os números reais. Mas a imagem da função seno vai de -1 a +1 porque esses são os valores extremos possíveis do seno de um ângulo. Por outro lado, o domínio da função inversa arcosseno é de -1 a +1, enquanto sua imagem é de -π a +π. Se você escrevesse uma função que passasse um argumento para a função `std::sin()`, você poderia fazer sua função lançar um objeto `domain_error` se o argumento estivesse fora do domínio de -1 a +1.

A exceção `invalid_argument` alerta que um valor inesperado foi passado para uma função. Por exemplo, se uma função espera receber uma string na qual cada caractere é '0' ou '1', ela poderia lançar a exceção `invalid_argument` se algum outro caractere aparecesse na string.

A exceção `length_error` é usada para indicar que não há espaço suficiente disponível para a ação desejada. Por exemplo, a classe `string` tem um método `append()` que lança uma exceção `length_error` se a string resultante fosse maior do que o comprimento máximo possível de string.

A exceção `out_of_bounds` é tipicamente usada para indicar erros de indexação. Por exemplo, você poderia definir uma classe semelhante a array para a qual `operator[]()` lança a exceção `out_of_bounds` se o índice usado for inválido para aquele array.

Em seguida, a família `runtime_error` descreve erros que podem aparecer durante o tempo de execução, mas que não podem ser facilmente previstos e prevenidos. O nome de cada classe indica o tipo de erro que ela pretende reportar:

- `range_error`
- `overflow_error`
- `underflow_error`

Cada classe tem um construtor como o de `runtime_error` que permite fornecer a string a ser retornada pelo método `what()`.

Um erro de underflow pode ocorrer em cálculos de ponto flutuante. Em geral, há uma menor magnitude não-zero que um tipo de ponto flutuante pode representar. Um cálculo que produziria um valor menor causaria um erro de underflow. Um erro de overflow pode ocorrer com tipos inteiros ou de ponto flutuante quando a magnitude do resultado de um cálculo excederia o maior valor representável para aquele tipo. Um resultado computacional pode estar fora do intervalo válido de uma função sem ser um underflow ou overflow, e você pode usar a exceção `range_error` para tais situações.

Em geral, uma exceção da família `logic_error` indica um problema suscetível a uma correção de programação, enquanto uma exceção da família `runtime_error` é apenas um pouco de dificuldade inevitável. Todas essas classes de erro têm as mesmas características gerais. A distinção principal é que os diferentes nomes de classe permitem que você trate cada tipo de exceção individualmente. Por outro lado, os relacionamentos de herança permitem que você os agrupe se preferir. Por exemplo, o código seguinte captura a exceção `out_of_bounds` individualmente, trata o restante da família `logic_error` como um grupo e trata objetos `exception`, a família `runtime_error` de objetos e quaisquer tipos de exceção restantes derivados de `exception` coletivamente:

```cpp
try {
    ...
}
catch(out_of_bounds & oe)    // captura erro out_of_bounds
{...}
catch(logic_error & oe)      // captura família restante de logic_error
{...}
catch(exception & oe)        // captura runtime_error, objetos exception
{...}
```

Se uma dessas classes de biblioteca não atende às suas necessidades, faz sentido derivar uma classe de exceção de `logic_error` ou `runtime_error` para que você possa encaixar suas exceções na mesma hierarquia geral.

## A Exceção bad_alloc e new

A maneira atual do C++ de lidar com problemas de alocação de memória com `new` é fazer `new` lançar uma exceção `bad_alloc`. O arquivo de cabeçalho `new` inclui uma declaração para a classe `bad_alloc`, que é derivada publicamente da classe `exception`. Nos tempos antigos, porém, `new` retornava um ponteiro nulo quando não conseguia alocar a quantidade solicitada de memória.

A Listagem 15.13 demonstra a abordagem atual. Se a exceção for capturada, o programa exibe a mensagem dependente da implementação retornada pelo método `what()` herdado e termina prematuramente.

**Listagem 15.13 — novaexcp.cpp**

```cpp
// novaexcp.cpp -- a exceção bad_alloc
#include <iostream>
#include <new>
#include <cstdlib>    // para exit(), EXIT_FAILURE
using namespace std;
struct Grande
{
    double dados[20000];
};
int main()
{
    Grande * pb;
    try {
        cout << "Tentando obter um grande bloco de memoria:\n";
        pb = new Grande[10000];    // 1.600.000.000 bytes
        cout << "Passou pela requisicao new:\n";
    }
    catch (bad_alloc & ba)
    {
        cout << "Excecao capturada!\n";
        cout << ba.what() << endl;
        exit(EXIT_FAILURE);
    }
    cout << "Memoria alocada com sucesso\n";
    pb[0].dados[0] = 4;
    cout << pb[0].dados[0] << endl;
    delete [] pb;
    return 0;
}
```

Aqui está a saída para um sistema:

```
Tentando obter um grande bloco de memoria:
Excecao capturada!
std::bad_alloc
```

Neste caso, o método `what()` retorna a string `"std::bad_alloc"`.

Se o programa executar sem problemas de alocação em seu sistema, você pode tentar aumentar a quantidade de memória solicitada.

### O Ponteiro Nulo e new

Muito código foi escrito quando `new` (o antigo `new`) retornava um ponteiro nulo em caso de falha. Alguns compiladores trataram a transição para o novo `new` deixando o usuário definir um flag ou switch para escolher qual comportamento queria. Atualmente, o padrão fornece uma forma alternativa de `new` que ainda retorna um ponteiro nulo. Seu uso se parece assim:

```cpp
int * pi = new (std::nothrow) int;
int * pa = new (std::nothrow) int[500];
```

Usando essa forma, você poderia reescrever o núcleo da Listagem 15.13 assim:

```cpp
Grande * pb;
pb = new (std::nothrow) Grande[10000];    // 1.600.000.000 bytes
if (pb == 0)
{
    cout << "Nao foi possivel alocar memoria. Ate logo.\n";
    exit(EXIT_FAILURE);
}
```

## Exceções, Classes e Herança

Exceções, classes e herança interagem de algumas formas. Primeiro, você pode derivar uma classe de exceção de outra, como faz a biblioteca C++ padrão. Segundo, você pode incorporar exceções em classes aninhando declarações de classe de exceção dentro de uma definição de classe. Terceiro, tais declarações aninhadas podem ser herdadas e podem servir elas próprias como classes base.

A Listagem 15.14 nos coloca no caminho de explorar algumas dessas possibilidades. Este arquivo de cabeçalho declara uma classe `Vendas` rudimentar que contém um valor de ano e um array de 12 figuras de vendas mensais. A classe `VendasRotuladas` deriva de `Vendas` e adiciona um membro para conter um rótulo para os dados.

**Listagem 15.14 — vendas.h**

```cpp
// vendas.h -- exceções e herança
#include <stdexcept>
#include <string>

class Vendas
{
public:
    enum {MESES = 12};    // poderia ser static const
    class indice_invalido : public std::logic_error
    {
    private:
        int bi;    // valor de índice inválido
    public:
        explicit indice_invalido(int ix,
            const std::string & s = "Erro de indice no objeto Vendas\n");
        int val_indice() const {return bi;}
        virtual ~indice_invalido() throw() {}
    };
    explicit Vendas(int ano = 0);
    Vendas(int ano, const double * gr, int n);
    virtual ~Vendas() { }
    int Ano() const { return ano; }
    virtual double operator[](int i) const;
    virtual double & operator[](int i);
private:
    double total[MESES];
    int ano;
};
class VendasRotuladas : public Vendas
{
public:
    class nindice_invalido : public Vendas::indice_invalido
    {
    private:
        std::string rot;
    public:
        nindice_invalido(const std::string & lb, int ix,
            const std::string & s = "Erro de indice no objeto VendasRotuladas\n");
        const std::string & val_rotulo() const {return rot;}
        virtual ~nindice_invalido() throw() {}
    };
    explicit VendasRotuladas(const std::string & lb = "nenhum", int ano = 0);
    VendasRotuladas(const std::string & lb, int ano, const double * gr, int n);
    virtual ~VendasRotuladas() { }
    const std::string & Rotulo() const {return rotulo;}
    virtual double operator[](int i) const;
    virtual double & operator[](int i);
private:
    std::string rotulo;
};
```

Vamos examinar alguns detalhes da Listagem 15.14. Primeiro, o símbolo constante `MESES` está na seção protegida de `Vendas`; isso torna o valor disponível para classes derivadas, como `VendasRotuladas`.

Em seguida, a classe `indice_invalido` está aninhada na seção pública de `Vendas`; isso torna a classe disponível como um tipo para blocos `catch` de clientes. Observe que o mundo exterior exige que o tipo seja identificado como `Vendas::indice_invalido`. Esta classe deriva da classe padrão `logic_error`. A classe `indice_invalido` tem a capacidade de armazenar e reportar um valor de índice fora dos limites para um índice de array.

A classe `nindice_invalido` está aninhada na seção pública de `VendasRotuladas`, tornando-a disponível para o código cliente como `VendasRotuladas::nindice_invalido`. Ela deriva de `indice_invalido`, adicionando a capacidade de armazenar e reportar o rótulo de um objeto `VendasRotuladas`. Como `indice_invalido` deriva de `logic_error`, `nindice_invalido` também deriva ultimamente de `logic_error`.

Ambas as classes têm métodos `operator[]()` sobrecarregados projetados para acessar os elementos individuais do array armazenados em um objeto e para lançar uma exceção se um índice estiver fora dos limites.

Ambas as classes `indice_invalido` e `nindice_invalido` usam a especificação de exceção `throw()`. A razão é que ambas herdam eventualmente da classe base `exception`, e seu destrutor virtual usa a especificação de exceção `throw()`. Esta é uma característica do C++98; sob C++11, o destrutor de `exception` não tem uma especificação de exceção.

A Listagem 15.15 mostra a implementação dos métodos que ainda não foram definidos inline na Listagem 15.14. Observe que classes aninhadas exigem o uso do operador de resolução de escopo mais de uma vez. Observe também que as funções `operator[]()` lançam exceções se o índice do array estiver fora dos limites.

**Listagem 15.15 — vendas.cpp**

```cpp
// vendas.cpp -- implementação de Vendas
#include "vendas.h"
using std::string;
Vendas::indice_invalido::indice_invalido(int ix, const string & s)
    : std::logic_error(s), bi(ix)
{
}
Vendas::Vendas(int a)
{
    ano = a;
    for (int i = 0; i < MESES; ++i)
        total[i] = 0;
}
Vendas::Vendas(int a, const double * gr, int n)
{
    ano = a;
    int lim = (n < MESES)? n : MESES;
    int i;
    for (i = 0; i < lim; ++i)
        total[i] = gr[i];
    // para i > n e i < MESES
    for ( ; i < MESES; ++i)
        total[i] = 0;
}
double Vendas::operator[](int i) const
{
    if(i < 0 || i >= MESES)
        throw indice_invalido(i);
    return total[i];
}
double & Vendas::operator[](int i)
{
    if(i < 0 || i >= MESES)
        throw indice_invalido(i);
    return total[i];
}
VendasRotuladas::nindice_invalido::nindice_invalido(const string & lb, int ix,
        const string & s) : Vendas::indice_invalido(ix, s)
{
    rot = lb;
}
VendasRotuladas::VendasRotuladas(const string & lb, int ano)
    : Vendas(ano)
{
    rotulo = lb;
}
VendasRotuladas::VendasRotuladas(const string & lb, int ano,
                const double * gr, int n)
                     : Vendas(ano, gr, n)
{
    rotulo = lb;
}
double VendasRotuladas::operator[](int i) const
{
    if(i < 0 || i >= MESES)
        throw nindice_invalido(Rotulo(), i);
    return Vendas::operator[](i);
}
double & VendasRotuladas::operator[](int i)
{
    if(i < 0 || i >= MESES)
        throw nindice_invalido(Rotulo(), i);
    return Vendas::operator[](i);
}
```

A Listagem 15.16 usa as classes em um programa que primeiro tenta ir além do fim do array no objeto `VendasRotuladas` `vendas2` e então além do fim do array no objeto `Vendas` `vendas1`. Essas tentativas são feitas em dois blocos `try` separados que testam cada tipo de exceção.

**Listagem 15.16 — usar_vendas.cpp**

```cpp
// usar_vendas.cpp -- exceções aninhadas
#include <iostream>
#include "vendas.h"

int main()
{
    using std::cout;
    using std::cin;
    using std::endl;

    double vals1[12] =
    {
        1220, 1100, 1122, 2212, 1232, 2334,
        2884, 2393, 3302, 2922, 3002, 3544
    };
    double vals2[12] =
    {
        12, 11, 22, 21, 32, 34,
        28, 29, 33, 29, 32, 35
    };
    Vendas vendas1(2011, vals1, 12);
    VendasRotuladas vendas2("Blogstar", 2012, vals2, 12);
    cout << "Primeiro bloco try:\n";
    try
    {
        int i;
        cout << "Ano = " << vendas1.Ano() << endl;
        for (i = 0; i < 12; ++i)
        {
            cout << vendas1[i] << ' ';
            if (i % 6 == 5)
                cout << endl;
        }
        cout << "Ano = " << vendas2.Ano() << endl;
        cout << "Rotulo = " << vendas2.Rotulo() << endl;
        for (i = 0; i <= 12; ++i)
        {
            cout << vendas2[i] << ' ';
            if (i % 6 == 5)
                cout << endl;
        }
        cout << "Fim do bloco try 1.\n";
    }
    catch(VendasRotuladas::nindice_invalido & bad)
    {
        cout << bad.what();
        cout << "Empresa: " << bad.val_rotulo() << endl;
        cout << "indice invalido: " << bad.val_indice() << endl;
    }
    catch(Vendas::indice_invalido & bad)
    {
        cout << bad.what();
        cout << "indice invalido: " << bad.val_indice() << endl;
    }
    cout << "\nProximo bloco try:\n";
    try
    {
        vendas2[2] = 37.5;
        vendas1[20] = 23345;
        cout << "Fim do bloco try 2.\n";
    }
    catch(VendasRotuladas::nindice_invalido & bad)
    {
        cout << bad.what();
        cout << "Empresa: " << bad.val_rotulo() << endl;
        cout << "indice invalido: " << bad.val_indice() << endl;
    }
    catch(Vendas::indice_invalido & bad)
    {
        cout << bad.what();
        cout << "indice invalido: " << bad.val_indice() << endl;
    }
    cout << "concluido\n";
    return 0;
}
```

Aqui está a saída do programa nas Listagens 15.14, 15.15 e 15.16:

```
Primeiro bloco try:
Ano = 2011
1220 1100 1122 2212 1232 2334
2884 2393 3302 2922 3002 3544
Ano = 2012
Rotulo = Blogstar
12 11 22 21 32 34
28 29 33 29 32 35
Erro de indice no objeto VendasRotuladas
Empresa: Blogstar
indice invalido: 12

Proximo bloco try:
Erro de indice no objeto Vendas
indice invalido: 20
concluido
```

## Quando Exceções Dão Errado

Após uma exceção ser lançada, ela tem duas oportunidades de causar problemas. Primeiro, se ela é lançada em uma função que tem uma especificação de exceção, ela deve corresponder a um dos tipos na lista de especificação. (Lembre-se de que em uma hierarquia de herança, um tipo de classe corresponde a objetos daquele tipo e de tipos descendentes dele.) Se a exceção não corresponder à especificação, a exceção não correspondida é classificada como uma exceção inesperada (unexpected exception), e, por padrão, faz o programa abortar. (Embora C++11 deprecie as especificações de exceção, elas ainda permanecem na linguagem e em algum código existente.) Se a exceção passar por este primeiro obstáculo (ou o evitar porque a função não tem especificação de exceção), ela então precisa ser capturada. Se não for, o que pode acontecer se não houver bloco `try` ou nenhum bloco `catch` correspondente, a exceção é classificada como uma exceção não capturada (uncaught exception), e por padrão faz o programa abortar. No entanto, você pode alterar a resposta de um programa a exceções inesperadas e não capturadas.

### Exceções Não Capturadas

Uma exceção não capturada não inicia um aborto imediato. Em vez disso, o programa primeiro chama uma função chamada `terminate()`. Por padrão, `terminate()` chama a função `abort()`. Você pode modificar o comportamento de `terminate()` registrando (registering) uma função que `terminate()` deve chamar em vez de `abort()`. Para fazer isso, você chama a função `set_terminate()`. Tanto `set_terminate()` quanto `terminate()` são declaradas no arquivo de cabeçalho `exception`:

```cpp
typedef void (*terminate_handler)();
terminate_handler set_terminate(terminate_handler f) throw();    // C++98
terminate_handler set_terminate(terminate_handler f) noexcept;  // C++11
void terminate();    // C++98
void terminate() noexcept;    // C++11
```

Aqui o `typedef` torna `terminate_handler` o nome do tipo para um ponteiro para uma função que não tem argumentos nem valor de retorno. A função `set_terminate()` recebe, como argumento, o nome de uma função (isto é, seu endereço) que não tem argumentos e tem o tipo de retorno `void`. Ela retorna o endereço da função previamente registrada. Se você chamar a função `set_terminate()` mais de uma vez, `terminate()` chama a função definida pela chamada mais recente a `set_terminate()`.

Por exemplo, suponha que você queira que uma exceção não capturada faça um programa imprimir uma mensagem a esse efeito e então chamar a função `exit()`, fornecendo um valor de status de saída de `5`. Primeiro, você inclui o arquivo de cabeçalho `exception`. Em seguida, você projeta uma função que faz as duas ações exigidas e tem o protótipo adequado:

```cpp
void meuSair()
{
    cout << "Encerrando devido a excecao nao capturada\n";
    exit(5);
}
```

Finalmente, no início do programa, você designa esta função como sua ação de encerramento escolhida:

```cpp
set_terminate(meuSair);
```

Agora, se uma exceção for lançada e não capturada, o programa chama `terminate()`, e `terminate()` chama `meuSair()`.

### Exceções Inesperadas

Em seguida, vejamos exceções inesperadas. Ao usar especificações de exceção para uma função, você fornece o meio para os usuários das funções saberem quais exceções capturar. Por exemplo, suponha que você tenha o seguinte protótipo:

```cpp
double Argh(double, double) throw(out_of_bounds);
```

Então você poderia usar a função assim:

```cpp
try {
    x = Argh(a, b);
}
catch(out_of_bounds & ex)
{
    ...
}
```

No entanto, em princípio, a especificação de exceção deve incluir exceções lançadas por funções chamadas pela função em questão. Por exemplo, se `Argh()` chamar uma função `Duh()` que pode lançar uma exceção de objeto `retort`, então `retort` deve aparecer na especificação de exceção de `Argh()` também. A menos que você escreva todas as funções você mesmo e seja cuidadoso, não há garantia de que isso será feito corretamente. Isso sugere que você deve olhar mais de perto o que acontece se uma função lança uma exceção que não está em sua especificação de exceção.

O comportamento é muito parecido com o das exceções não capturadas. Se houver uma exceção inesperada, o programa chama a função `unexpected()`. Essa função, por sua vez, chama `terminate()`, que, por padrão, chama `abort()`. Assim como há uma função `set_terminate()` que modifica o comportamento de `terminate()`, há uma função `set_unexpected()` que modifica o comportamento de `unexpected()`. Essas novas funções também são declaradas no arquivo de cabeçalho `exception`:

```cpp
typedef void (*unexpected_handler)();
unexpected_handler set_unexpected(unexpected_handler f) throw();    // C++98
unexpected_handler set_unexpected(unexpected_handler f) noexcept;  // C++11
void unexpected();    // C++98
void unexpected() noexcept;    // C++11
```

No entanto, o comportamento da função que você fornece para `set_unexpected()` é mais regulado do que o de uma função para `set_terminate()`. Em particular, a função `unexpected_handler` tem as seguintes opções:

- Pode encerrar o programa chamando `terminate()` (comportamento padrão), `abort()`, ou `exit()`.
- Pode lançar uma exceção.

O resultado de lançar uma exceção (a segunda opção aqui) depende da exceção lançada pela função `unexpected_handler` de substituição e da especificação de exceção original para a função que lançou o tipo inesperado:

- Se a exceção recém-lançada corresponder à especificação de exceção original, então o programa prossegue normalmente a partir daí; ou seja, ele procurará um bloco `catch` que corresponda à exceção recém-lançada. Basicamente, essa abordagem substitui uma exceção de um tipo inesperado por uma exceção de um tipo esperado.
- Se a exceção recém-lançada não corresponder à especificação de exceção original e se a especificação de exceção não incluir o tipo `std::bad_exception`, o programa chama `terminate()`. O tipo `bad_exception` deriva do tipo `exception` e é declarado no arquivo de cabeçalho `exception`.
- Se a exceção recém-lançada não corresponder à especificação de exceção original e se a especificação de exceção original incluir o tipo `std::bad_exception`, a exceção não correspondida é substituída por uma exceção do tipo `std::bad_exception`.

Em resumo, se você gostaria de capturar todas as exceções, esperadas ou não, você pode fazer algo como o seguinte. Primeiro, inclua o arquivo de cabeçalho `exception`. Em seguida, projete uma função de substituição que converte exceções inesperadas para o tipo `bad_exception` e que tem o protótipo adequado:

```cpp
void meuInesperado()
{
    throw std::bad_exception();    // ou apenas throw;
}
```

Usar apenas `throw` sem uma exceção causa o relançamento da exceção original. No entanto, a exceção será substituída por um objeto `bad_exception` se a especificação de exceção incluir esse tipo.

Em seguida, no início do programa, você designa esta função como sua ação de exceção inesperada escolhida:

```cpp
set_unexpected(meuInesperado);
```

Finalmente, você inclui o tipo `bad_exception` em especificações de exceção e sequências de blocos `catch`:

```cpp
double Argh(double, double) throw(out_of_bounds, bad_exception);
...
try {
    x = Argh(a, b);
}
catch(out_of_bounds & ex)
{
    ...
}
catch(bad_exception & ex)
{
    ...
}
```

## Precauções com Exceções

Da discussão precedente sobre o uso de exceções, você pode concluir (e concluir corretamente) que o tratamento de exceções deve ser projetado em um programa em vez de acrescentado. Fazer isso tem algumas desvantagens, no entanto. Por exemplo, usar exceções adiciona ao tamanho e subtrai da velocidade de um programa. As especificações de exceção não funcionam bem com templates porque funções de template podem lançar diferentes tipos de exceções, dependendo da especialização específica usada. Exceções e alocação dinâmica de memória nem sempre funcionam muito bem juntas.

Vejamos um pouco mais sobre alocação dinâmica de memória e exceções. Primeiro, considere a seguinte função:

```cpp
void teste1(int n)
{
    string msg("Estou preso em um loop sem fim");
    ...
    if (uh_oh)
        throw exception();
    ...
    return;
}
```

A classe `string` usa alocação dinâmica de memória. Normalmente, o destrutor de `string` para `msg` seria chamado quando a função alcançasse `return` e terminasse. Graças ao desdobramento da pilha, a instrução `throw`, mesmo que encerre a função prematuramente, ainda permite que o destrutor seja chamado. Então, neste caso, a memória é gerenciada corretamente.

Agora considere esta função:

```cpp
void teste2(int n)
{
    double * ar = new double[n];
    ...
    if (uh_oh)
        throw exception();
    ...
    delete [] ar;
    return;
}
```

Aqui há um problema. Desdobrar a pilha remove a variável `ar` da pilha. Mas o encerramento prematuro da função significa que a instrução `delete []` no final da função é ignorada. O ponteiro se foi, mas o bloco de memória para o qual apontava ainda está intacto e inacessível. Em resumo, há um vazamento de memória (memory leak).

O vazamento pode ser evitado. Por exemplo, você pode capturar a exceção na mesma função que a lança, colocar algum código de limpeza no bloco `catch` e relançar a exceção:

```cpp
void teste3(int n)
{
    double * ar = new double[n];
    ...
    try {
        if (uh_oh)
            throw exception();
    }
    catch(exception & ex)
    {
        delete [] ar;
        throw;
    }
    ...
    delete [] ar;
    return;
}
```

No entanto, isso claramente aumenta as oportunidades de descuidos e outros erros. Outra solução é usar um dos templates de ponteiro inteligente (smart pointer) discutidos no Capítulo 16, "A Classe string e a Biblioteca de Templates Padrão".

Em resumo, embora o tratamento de exceções seja extremamente importante para alguns projetos, ele tem custos em termos de esforço de programação, tamanho do programa e velocidade do programa. Por outro lado, o custo de nenhuma verificação de erros pode ser muito pior.

---

[Anterior](capitulo-15-01-amigos-excecoes.md) | [Índice](README.md) | [Próximo](capitulo-15-03-rtti-casts-resumo.md)
