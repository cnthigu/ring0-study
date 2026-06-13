# Capítulo 11 — Conversões Automáticas, Resumo e Exercícios

> Tradução não oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Navegação: [Anterior](capitulo-11-02-funcoes-amigas-vetores.md) | [Índice](README.md) | [Próximo](capitulo-12-01-alocacao-dinamica.md)

---

## Conversões Automáticas e Type Casts para Classes (Automatic Conversions and Type Casts for Classes)

O próximo tópico do menu de classes é a conversão de tipos. Vamos examinar como o C++ lida com conversões de e para tipos definidos pelo usuário. Para preparar o terreno, vamos primeiro rever como o C++ lida com conversões para seus tipos internos. Quando você faz uma instrução que atribui um valor de um tipo padrão a uma variável de outro tipo padrão, o C++ converte automaticamente o valor para o mesmo tipo que a variável receptora, desde que os dois tipos sejam compatíveis. Por exemplo, as instruções a seguir geram conversões de tipo numérico:

```cpp
long contador = 8;   // valor int 8 convertido para tipo long
double tempo = 11;   // valor int 11 convertido para tipo double
int lado = 3.33;     // valor double 3.33 convertido para tipo int 3
```

Essas atribuições funcionam porque o C++ reconhece que os diferentes tipos numéricos todos representam a mesma coisa básica — um número — e porque o C++ incorpora regras internas para fazer as conversões. Lembre-se, porém, do Capítulo 3, "Lidando com Dados," que você pode perder alguma precisão nessas conversões. Por exemplo, atribuir `3.33` à variável `int` `lado` resulta em `lado` recebendo o valor `3`, perdendo a parte `0.33`.

A linguagem C++ não converte automaticamente tipos que não são compatíveis. Por exemplo, a instrução a seguir falha porque o lado esquerdo é um tipo ponteiro, enquanto o lado direito é um número:

```cpp
int * p = 10; // conflito de tipos
```

E mesmo que um computador possa representar um endereço internamente com um inteiro, inteiros e ponteiros são conceitualmente bastante diferentes. No entanto, quando as conversões automáticas falham, você pode usar um type cast:

```cpp
int * p = (int *) 10; // ok, p e (int *) 10 são ambos ponteiros
```

Isso define um ponteiro para o endereço `10` fazendo um type cast de `10` para o tipo ponteiro-para-int (ou seja, tipo `int *`). Se essa atribuição faz sentido é outro assunto.

Você pode definir uma classe suficientemente relacionada a um tipo básico ou a outra classe de forma que faça sentido converter de uma forma para outra. Nesse caso, você pode dizer ao C++ como fazer tais conversões automaticamente ou, talvez, via um type cast. Para ver como isso funciona, você pode reformular o programa de conversão de libras para pedras do Capítulo 3 na forma de classe. Primeiro, você precisa projetar um tipo apropriado. Fundamentalmente, você está representando uma coisa (um peso) de duas maneiras (libras e pedras). Uma classe fornece uma excelente forma de incorporar duas representações de um conceito em uma única entidade. Portanto, faz sentido colocar ambas as representações do peso na mesma classe e então fornecer métodos de classe para expressar o peso em diferentes formas. A Listagem 11.16 fornece o cabeçalho da classe.

**Listagem 11.16 — stonewt.h**

```cpp
// stonewt.h — definição para a classe PesoPedra
#ifndef STONEWT_H_
#define STONEWT_H_

class PesoPedra
{
private:
    enum {Lib_por_pedra = 14}; // libras por pedra
    int pedras;        // pedras inteiras
    double lib_resto;  // libras fracionárias
    double libras;     // peso total em libras
public:
    PesoPedra(double lbs);         // construtor para double libras
    PesoPedra(int peds, double lbs); // construtor para pedras, libras
    PesoPedra();                   // construtor padrão
    ~PesoPedra();
    void exibir_libras() const; // exibe peso em formato de libras
    void exibir_pedras() const; // exibe peso em formato de pedras
};
#endif
```

Como mencionado no Capítulo 10, `enum` fornece uma forma conveniente de definir constantes específicas de classe, desde que sejam inteiros. Ou você poderia usar a seguinte alternativa:

```cpp
static const int Lib_por_pedra = 14;
```

Observe que a classe `PesoPedra` tem três construtores. Eles permitem inicializar um objeto `PesoPedra` com um número de ponto flutuante de libras ou com uma combinação de pedras e libras. Ou você pode criar um objeto `PesoPedra` sem inicializá-lo:

```cpp
PesoPedra flor(132.5);          // peso = 132.5 libras
PesoPedra manteiga(10, 2);      // peso = 10 pedras, 2 libras
PesoPedra bolhas;               // peso = valor padrão
```

A classe também fornece duas funções de exibição. Uma exibe o peso em libras e a outra exibe o peso em pedras e libras. A Listagem 11.17 mostra a implementação dos métodos da classe. Observe que cada construtor atribui valores a todos os três membros privados. Assim, criar um objeto `PesoPedra` define automaticamente ambas as representações do peso.

**Listagem 11.17 — stonewt.cpp**

```cpp
// stonewt.cpp — métodos de PesoPedra
#include <iostream>
using std::cout;
#include "stonewt.h"

// constrói objeto PesoPedra a partir de valor double
PesoPedra::PesoPedra(double lbs)
{
    pedras = int(lbs) / Lib_por_pedra; // divisão inteira
    lib_resto = int(lbs) % Lib_por_pedra + lbs - int(lbs);
    libras = lbs;
}
// constrói objeto PesoPedra a partir de valores de pedras e double
PesoPedra::PesoPedra(int peds, double lbs)
{
    pedras = peds;
    lib_resto = lbs;
    libras = peds * Lib_por_pedra + lbs;
}
PesoPedra::PesoPedra() // construtor padrão, peso = 0
{
    pedras = libras = lib_resto = 0;
}
PesoPedra::~PesoPedra() // destrutor
{
}
// exibe peso em pedras
void PesoPedra::exibir_pedras() const
{
    cout << pedras << " pedras, " << lib_resto << " libras\n";
}
// exibe peso em libras
void PesoPedra::exibir_libras() const
{
    cout << libras << " libras\n";
}
```

Como um objeto `PesoPedra` representa um único peso, faz sentido fornecer maneiras de converter um valor inteiro ou de ponto flutuante em um objeto `PesoPedra`. E você já fez isso! Em C++, qualquer construtor que recebe um único argumento age como um modelo para converter um valor desse tipo de argumento para o tipo da classe. Portanto, o seguinte construtor serve como instrução para converter um valor do tipo `double` em um valor do tipo `PesoPedra`:

```cpp
PesoPedra(double lbs); // modelo para conversão de double para PesoPedra
```

Ou seja, você pode escrever código como o seguinte:

```cpp
PesoPedra meuGato;  // cria um objeto PesoPedra
meuGato = 19.6;     // usa PesoPedra(double) para converter 19.6 para PesoPedra
```

O programa usa o construtor `PesoPedra(double)` para construir um objeto `PesoPedra` temporário, usando `19.6` como valor de inicialização. Em seguida, a atribuição membro a membro copia o conteúdo do objeto temporário para `meuGato`. Esse processo é chamado de *conversão implícita* (implicit conversion) porque acontece automaticamente, sem a necessidade de um type cast explícito.

Apenas um construtor que pode ser usado com apenas um argumento funciona como função de conversão. O seguinte construtor tem dois argumentos, portanto não pode ser usado para converter tipos:

```cpp
PesoPedra(int peds, double lbs); // não é uma função de conversão
```

Entretanto, ele agiria como um guia para conversão de `int` se fornecesse um valor padrão para o segundo parâmetro:

```cpp
PesoPedra(int peds, double lbs = 0); // conversão de int para PesoPedra
```

Ter um construtor que funcione como uma função de conversão automática de tipo parece um recurso agradável. À medida que os programadores adquiriram mais experiência trabalhando com C++, no entanto, descobriram que o aspecto automático nem sempre é desejável porque pode levar a conversões inesperadas. Então o C++ adicionou uma nova palavra-chave, `explicit`, para desativar o aspecto automático. Ou seja, você pode declarar o construtor assim:

```cpp
explicit PesoPedra(double lbs); // sem conversões implícitas permitidas
```

Isso desativa conversões implícitas como o exemplo anterior, mas ainda permite conversões explícitas — isto é, conversões usando type casts explícitos:

```cpp
PesoPedra meuGato;           // cria um objeto PesoPedra
meuGato = 19.6;              // não válido se PesoPedra(double) for declarado como explicit
meuGato = PesoPedra(19.6);   // ok, uma conversão explícita
meuGato = (PesoPedra) 19.6;  // ok, forma antiga para type cast explícito
```

> **Nota**
> Um construtor C++ que contém um argumento define uma conversão de tipo do tipo do argumento para o tipo da classe. Se o construtor for qualificado com a palavra-chave `explicit`, o construtor é usado apenas para conversões explícitas; caso contrário, também é usado para conversões implícitas.

Quando o compilador usa a função `PesoPedra(double)`? Se a palavra-chave `explicit` for usada na declaração, `PesoPedra(double)` é usado apenas para um type cast explícito; caso contrário, também é usado para as seguintes *conversões implícitas*:

- Quando você inicializa um objeto `PesoPedra` com um valor do tipo `double`
- Quando você atribui um valor do tipo `double` a um objeto `PesoPedra`
- Quando você passa um valor do tipo `double` para uma função que espera um argumento `PesoPedra`
- Quando uma função declarada para retornar um valor `PesoPedra` tenta retornar um valor `double`
- Quando qualquer uma das situações anteriores usa um tipo interno que pode ser convertido sem ambiguidade para o tipo `double`

Vamos examinar o último ponto com mais detalhes. O processo de correspondência de argumentos fornecido pelo protótipo de função permite que o construtor `PesoPedra(double)` atue como conversão para outros tipos numéricos. Ou seja, ambas as instruções a seguir funcionam convertendo primeiro `int` para `double` e então usando o construtor `PesoPedra(double)`:

```cpp
PesoPedra Jumbo(7000);  // usa PesoPedra(double), convertendo int para double
Jumbo = 7300;           // usa PesoPedra(double), convertendo int para double
```

Entretanto, esse processo de conversão em dois passos funciona apenas se houver uma escolha sem ambiguidade. Ou seja, se a classe também definisse um construtor `PesoPedra(long)`, o compilador rejeitaria essas instruções, provavelmente apontando que um `int` pode ser convertido para `long` ou para `double`, portanto a chamada é ambígua.

A Listagem 11.18 usa os construtores da classe para inicializar alguns objetos `PesoPedra` e para lidar com conversões de tipo.

**Listagem 11.18 — stone.cpp**

```cpp
// stone.cpp — conversões definidas pelo usuário
// compilar com stonewt.cpp
#include <iostream>
using std::cout;
#include "stonewt.h"
void exibir_peso(const PesoPedra & st, int n);
int main()
{
    PesoPedra incognito = 275;    // usa construtor para inicializar
    PesoPedra wolfe(285.7);       // mesmo que PesoPedra wolfe = 285.7;
    PesoPedra taft(21, 8);
    cout << "A celebridade pesava ";
    incognito.exibir_pedras();
    cout << "O detetive pesava ";
    wolfe.exibir_pedras();
    cout << "O presidente pesava ";
    taft.exibir_libras();
    incognito = 276.8;  // usa construtor para conversão
    taft = 325;         // mesmo que taft = PesoPedra(325);
    cout << "Apos o jantar, a celebridade pesava ";
    incognito.exibir_pedras();
    cout << "Apos o jantar, o presidente pesava ";
    taft.exibir_libras();
    exibir_peso(taft, 2);
    cout << "O lutador pesava ainda mais.\n";
    exibir_peso(422, 2);
    cout << "Nenhuma pedra ficou sem virar\n";
    return 0;
}

void exibir_peso(const PesoPedra & st, int n)
{
    for (int i = 0; i < n; i++)
    {
        cout << "Uau! ";
        st.exibir_pedras();
    }
}
```

Aqui está a saída do programa da Listagem 11.18:

```
A celebridade pesava 19 pedras, 9 libras
O detetive pesava 20 pedras, 5.7 libras
O presidente pesava 302 libras
Apos o jantar, a celebridade pesava 19 pedras, 10.8 libras
Apos o jantar, o presidente pesava 325 libras
Uau! 23 pedras, 3 libras
Uau! 23 pedras, 3 libras
O lutador pesava ainda mais.
Uau! 30 pedras, 2 libras
Uau! 30 pedras, 2 libras
Nenhuma pedra ficou sem virar
```

### Notas do Programa

Note que quando um construtor tem um único argumento, você pode usar a seguinte forma ao inicializar um objeto de classe:

```cpp
// uma sintaxe para inicializar um objeto de classe quando
// usando um construtor com um argumento
PesoPedra incognito = 275;
```

Isso é equivalente às outras duas formas mostradas anteriormente:

```cpp
// formas de sintaxe padrão para inicializar objetos de classe
PesoPedra incognito(275);
PesoPedra incognito = PesoPedra(275);
```

Entretanto, as duas últimas formas também podem ser usadas com construtores que têm múltiplos argumentos.

Em seguida, note as seguintes duas atribuições da Listagem 11.18:

```cpp
incognito = 276.8;
taft = 325;
```

A primeira dessas atribuições usa o construtor com um argumento do tipo `double` para converter `276.8` em um valor do tipo `PesoPedra`. Isso define o membro `libras` de `incognito` como `276.8`. Como ela usa o construtor, essa atribuição também define os membros `pedras` e `lib_resto` da classe. Da mesma forma, a segunda atribuição converte um valor do tipo `int` para `double` e então usa `PesoPedra(double)` para definir todos os três valores membros no processo.

Por fim, note a seguinte chamada de função:

```cpp
exibir_peso(422, 2); // converte 422 para double, depois para PesoPedra
```

O protótipo de `exibir_peso()` indica que seu primeiro argumento deve ser o objeto `PesoPedra`. Confrontado com um argumento `int`, o compilador procura um construtor `PesoPedra(int)` para converter o `int` para o tipo `PesoPedra` desejado. Não encontrando esse construtor, o compilador procura um construtor com algum outro tipo interno para o qual um `int` possa ser convertido. O construtor `PesoPedra(double)` se encaixa na conta. Portanto, o compilador converte `int` para `double` e então usa `PesoPedra(double)` para converter o resultado em um objeto `PesoPedra`.

## Funções de Conversão (Conversion Functions)

A Listagem 11.18 converte um número em um objeto `PesoPedra`. Você pode fazer o inverso? Ou seja, você pode converter um objeto `PesoPedra` para um valor `double`, como no seguinte?

```cpp
PesoPedra wolfe(285.7);
double total = wolfe; // ?? possível ??
```

A resposta é que você pode fazer isso — mas não usando construtores. Construtores só fornecem conversão de outro tipo *para* o tipo da classe. Para fazer o inverso, você tem que usar uma forma especial de função operadora C++ chamada *função de conversão* (conversion function).

Funções de conversão são type casts definidos pelo usuário, e você pode usá-las da forma que usaria um type cast. Por exemplo, se você define uma função de conversão de `PesoPedra` para `double`, você pode usar as seguintes conversões:

```cpp
PesoPedra wolfe(285.7);
double total = double(wolfe);   // sintaxe #1
double pensador = (double) wolfe; // sintaxe #2
```

Ou você pode deixar o compilador descobrir o que fazer:

```cpp
PesoPedra wells(20, 3);
double estrela = wells; // uso implícito da função de conversão
```

O compilador, observando que o lado direito é do tipo `PesoPedra` e o lado esquerdo é do tipo `double`, procura ver se você definiu uma função de conversão que corresponde a essa descrição. (Se não encontrar tal definição, o compilador gera uma mensagem de erro indicando que não pode atribuir um `PesoPedra` a um `double`.)

Então como você cria uma função de conversão? Para converter para o tipo `nomeTipo`, você usa uma função de conversão nesta forma:

```cpp
operator nomeTipo();
```

Observe os seguintes pontos:

- A função de conversão deve ser um método de classe.
- A função de conversão não deve especificar um tipo de retorno.
- A função de conversão não deve ter argumentos.

Por exemplo, uma função para converter para o tipo `double` teria este protótipo:

```cpp
operator double();
```

A parte `nomeTipo` (neste caso `nomeTipo` é `double`) diz ao compilador para qual tipo converter, portanto nenhum tipo de retorno é necessário. O fato de a função ser um método de classe significa que ela deve ser invocada por um objeto de classe específico, e isso diz à função qual valor converter. Portanto, a função não precisa de argumentos.

Para adicionar funções que convertem objetos `PesoPedra` para o tipo `int` e para o tipo `double`, você precisa acrescentar os seguintes protótipos à declaração da classe:

```cpp
operator int();
operator double();
```

A Listagem 11.19 mostra a declaração de classe modificada.

**Listagem 11.19 — stonewt1.h**

```cpp
// stonewt1.h — definição revisada para a classe PesoPedra
#ifndef STONEWT1_H_
#define STONEWT1_H_
class PesoPedra
{
private:
    enum {Lib_por_pedra = 14}; // libras por pedra
    int pedras;        // pedras inteiras
    double lib_resto;  // libras fracionárias
    double libras;     // peso total em libras
public:
    PesoPedra(double lbs);
    PesoPedra(int peds, double lbs);
    PesoPedra();
    ~PesoPedra();
    void exibir_libras() const;
    void exibir_pedras() const;
    // funções de conversão
    operator int() const;
    operator double() const;
};
#endif
```

A Listagem 11.20 mostra a Listagem 11.17 modificada para incluir as definições dessas duas funções de conversão. Observe que cada função retorna o valor desejado, mesmo que não haja tipo de retorno declarado. Observe também que a definição de conversão para `int` arredonda para o inteiro mais próximo em vez de truncar.

**Listagem 11.20 — stonewt1.cpp**

```cpp
// stonewt1.cpp — métodos da classe PesoPedra + funções de conversão
#include <iostream>
using std::cout;
#include "stonewt1.h"
// constrói objeto PesoPedra a partir de valor double
PesoPedra::PesoPedra(double lbs)
{
    pedras = int(lbs) / Lib_por_pedra; // divisão inteira
    lib_resto = int(lbs) % Lib_por_pedra + lbs - int(lbs);
    libras = lbs;
}
// constrói objeto PesoPedra a partir de pedras e double
PesoPedra::PesoPedra(int peds, double lbs)
{
    pedras = peds;
    lib_resto = lbs;
    libras = peds * Lib_por_pedra + lbs;
}
PesoPedra::PesoPedra() // construtor padrão, peso = 0
{
    pedras = libras = lib_resto = 0;
}
PesoPedra::~PesoPedra() // destrutor
{
}
// exibe peso em pedras
void PesoPedra::exibir_pedras() const
{
    cout << pedras << " pedras, " << lib_resto << " libras\n";
}
// exibe peso em libras
void PesoPedra::exibir_libras() const
{
    cout << libras << " libras\n";
}
// funções de conversão
PesoPedra::operator int() const
{
    return int(libras + 0.5);
}
PesoPedra::operator double() const
{
    return libras;
}
```

A Listagem 11.21 testa as novas funções de conversão. A instrução de atribuição no programa usa uma conversão implícita, enquanto a instrução final `cout` usa um type cast explícito.

**Listagem 11.21 — stone1.cpp**

```cpp
// stone1.cpp — funções de conversão definidas pelo usuário
// compilar com stonewt1.cpp
#include <iostream>
#include "stonewt1.h"

int main()
{
    using std::cout;
    PesoPedra poppins(9, 2.8); // 9 pedras, 2.8 libras
    double p_lib = poppins;    // conversão implícita
    cout << "Converter para double => ";
    cout << "Poppins: " << p_lib << " libras.\n";
    cout << "Converter para int => ";
    cout << "Poppins: " << int(poppins) << " libras.\n";
    return 0;
}
```

Aqui está a saída do programa das Listagens 11.19, 11.20 e 11.21, que mostra o resultado de converter o objeto do tipo `PesoPedra` para `double` e para `int`:

```
Converter para double => Poppins: 128.8 libras.
Converter para int => Poppins: 129 libras.
```

### Aplicando Conversões de Tipo Automaticamente

A Listagem 11.21 usa `int(poppins)` com `cout`. Suponha que, em vez disso, ela omitisse o type cast explícito:

```cpp
cout << "Poppins: " << poppins << " libras.\n";
```

O programa usaria uma conversão implícita, como na seguinte instrução?

```cpp
double p_lib = poppins;
```

A resposta é não. No exemplo de `p_lib`, o contexto indica que `poppins` deve ser convertido para `double`. Mas no exemplo de `cout`, nada indica se a conversão deve ser para `int` ou para `double`. Diante dessa falta de informação, o compilador reclamaria que você estava usando uma conversão ambígua. Nada na instrução indica qual tipo usar.

Curiosamente, se a classe definisse apenas a função de conversão para `double`, o compilador aceitaria a instrução. Isso é porque com apenas uma conversão possível, não há ambiguidade.

Você pode ter uma situação similar com atribuição. Com as declarações de classe atuais, o compilador rejeita a seguinte instrução como sendo ambígua:

```cpp
long foi = poppins; // ambíguo
```

Em C++, você pode atribuir tanto valores `int` quanto `double` a uma variável `long`, portanto o compilador legitimamente pode usar qualquer função de conversão. O compilador não quer a responsabilidade de escolher. Mas se você eliminar uma das duas funções de conversão, o compilador aceita a instrução. Por exemplo, suponha que você omita a definição de `double`. Então o compilador usará a conversão para `int` para converter `poppins` em um valor `int`. Então ele converte o valor `int` para `long` ao atribuí-lo a `foi`.

Quando a classe define duas ou mais conversões, você ainda pode usar um type cast explícito para indicar qual função de conversão usar. Você pode usar qualquer uma dessas notações de type cast:

```cpp
long foi = (double) poppins;  // usa conversão double
long foi = int(poppins);      // usa conversão int
```

A primeira dessas instruções converte o peso de `poppins` para um valor `double` e, em seguida, a atribuição converte o valor `double` para `long`. Da mesma forma, a segunda instrução converte `poppins` primeiro para `int` e então para `long`.

Assim como os construtores de conversão, as funções de conversão podem ser uma faca de dois gumes. O problema em fornecer funções que fazem conversões automáticas e implícitas é que elas podem fazer conversões quando você não as espera. Suponha, por exemplo, que você por acaso escreva o seguinte código quando está com sono:

```cpp
int ar[20];
...
PesoPedra temp(14, 4);
...
int Temp = 1;
...
cout << ar[temp] << "!\n"; // usou temp em vez de Temp
```

Normalmente, você esperaria que o compilador detectasse um erro como usar um objeto em vez de um inteiro como índice de array. Mas a classe `PesoPedra` define um `operator int()`, portanto o objeto `PesoPedra` `temp` é convertido para o `int` `200` e é usado como índice de array. A moral é que muitas vezes é melhor usar conversões explícitas e excluir a possibilidade de conversões implícitas. Em C++98, a palavra-chave `explicit` não funcionava com funções de conversão, mas o C++11 remove essa limitação. Portanto, com o C++11, você pode declarar um operador de conversão como `explicit`:

```cpp
class PesoPedra
{
...
// funções de conversão
    explicit operator int() const;
    explicit operator double() const;
};
```

Com essas declarações no lugar, você usaria um type cast para invocar os operadores.

Outra abordagem é substituir uma função de conversão por uma função não de conversão que faz a mesma tarefa — mas apenas se chamada explicitamente. Ou seja, você pode substituir

```cpp
PesoPedra::operator int() { return int(libras + 0.5); }
```

por

```cpp
int PesoPedra::pedras_para_int() { return int(libras + 0.5); }
```

Isso proíbe o seguinte:

```cpp
int plib = poppins;
```

Mas se você realmente precisar de uma conversão, permite o seguinte:

```cpp
int plib = poppins.pedras_para_int();
```

> **Cuidado**
> Você deve usar funções de conversão implícita com cuidado. Muitas vezes, uma função que só pode ser invocada explicitamente é a melhor escolha.

Em resumo, então, o C++ fornece as seguintes conversões de tipo para classes:

- Um *construtor* de classe que tem apenas um argumento serve como instrução para converter um valor do tipo do argumento para o tipo da classe. Por exemplo, o construtor `PesoPedra` com um argumento do tipo `int` é invocado automaticamente quando você atribui um valor `int` a um objeto `PesoPedra`. Entretanto, usando `explicit` na declaração do construtor elimina as conversões implícitas e permite apenas conversões explícitas.

- Uma função membro de classe especial chamada *função de conversão* (conversion function) serve como instrução para converter um objeto de classe para algum outro tipo. A função de conversão é um membro de classe, não tem tipo de retorno declarado, não tem argumentos, e é chamada `operator nomeTipo()`, onde `nomeTipo` é o tipo para o qual o objeto deve ser convertido. Essa função de conversão é invocada automaticamente quando você atribui um objeto de classe a uma variável daquele tipo ou usa o operador de type cast para aquele tipo.

## Conversões e Amigas (Conversions and Friends)

Vamos trazer a adição para a classe `PesoPedra`. Como mencionado na discussão da classe `Tempo`, você pode usar uma função membro ou uma função amiga para sobrecarregar a adição. (Para simplificar os assuntos, suponha que nenhuma função de conversão do formulário `operator double()` esteja definida.) Você pode implementar a adição com a seguinte função membro:

```cpp
PesoPedra PesoPedra::operator+(const PesoPedra & st) const
{
    double pds = libras + st.libras;
    PesoPedra soma(pds);
    return soma;
}
```

Ou você pode implementar a adição como uma função amiga desta forma:

```cpp
PesoPedra operator+(const PesoPedra & st1, const PesoPedra & st2)
{
    double pds = st1.libras + st2.libras;
    PesoPedra soma(pds);
    return soma;
}
```

Lembre-se: você pode fornecer a definição do método ou a definição da função amiga, mas não ambas. Qualquer forma permite fazer o seguinte:

```cpp
PesoPedra jennySt(9, 12);
PesoPedra bennySt(12, 8);
PesoPedra total;
total = jennySt + bennySt;
```

Dado também o construtor `PesoPedra(double)`, cada forma permite fazer o seguinte:

```cpp
PesoPedra jennySt(9, 12);
double kennyD = 176.0;
PesoPedra total;
total = jennySt + kennyD;
```

Mas apenas a função amiga permite fazer isto:

```cpp
PesoPedra jennySt(9, 12);
double pennyD = 146.0;
PesoPedra total;
total = pennyD + jennySt;
```

Para ver por quê, você pode traduzir cada adição para as chamadas de função correspondentes. Primeiro, `total = jennySt + bennySt;` torna-se `total = jennySt.operator+(bennySt);` (versão membro) ou `total = operator+(jennySt, bennySt);` (versão amiga). Em qualquer caso, os tipos dos argumentos reais correspondem aos argumentos formais.

Em seguida, `total = jennySt + kennyD;` torna-se `total = jennySt.operator+(kennyD);` (versão membro) ou `total = operator+(jennySt, kennyD);` (versão amiga). Neste caso, um argumento (`kennyD`) é do tipo `double`, o que invoca o construtor `PesoPedra(double)` para converter o argumento em um objeto `PesoPedra`.

Porém, a versão da função membro não seria capaz de adicionar `jennyPe` a `pennyD`. Converter a sintaxe de adição para uma chamada de função ficaria assim:

```cpp
total = pennyD.operator+(jennySt); // não tem sentido
```

Mas isso não faz sentido porque apenas um objeto de classe pode invocar uma função membro. O C++ não tenta converter `pennyD` para um objeto `PesoPedra`. A conversão ocorre para argumentos de função membro, não para invocadores de função membro.

A lição aqui é que definir a adição como uma amiga torna mais fácil para um programa acomodar conversões automáticas de tipo. O motivo é que ambos os operandos tornam-se argumentos de função, então o protótipo de função entra em vigor para ambos os operandos.

### Opções na Implementação da Adição

Dado que você quer adicionar quantidades `double` a quantidades `PesoPedra`, você tem algumas opções. A primeira, como você acabou de ver, é definir a seguinte função amiga e fazer o construtor `PesoPedra(double)` lidar com as conversões de argumentos do tipo `double` para argumentos do tipo `PesoPedra`:

```cpp
operator+(const PesoPedra &, const PesoPedra &)
```

A segunda opção é sobrecarregar ainda mais o operador de adição com funções que usam explicitamente um argumento do tipo `double`:

```cpp
PesoPedra operator+(double x);                   // função membro
friend PesoPedra operator+(double x, PesoPedra & s); // função amiga
```

Dessa forma, a seguinte instrução corresponde exatamente à função membro `operator+(double x)`:

```cpp
total = jennySt + kennyD; // PesoPedra + double
```

E a seguinte instrução corresponde exatamente à função amiga `operator+(double x, PesoPedra & s)`:

```cpp
total = pennyD + jennySt; // double + PesoPedra
```

Cada escolha tem vantagens. A primeira (dependendo de conversões implícitas) resulta em um programa mais curto porque você define menos funções. Isso também implica menos trabalho para você e menos chances de cometer erros. A desvantagem é a sobrecarga extra em tempo e memória necessária para invocar o construtor de conversão sempre que uma conversão é necessária. A segunda opção (funções adicionais que correspondem explicitamente aos tipos) faz o contrário: resulta em um programa mais longo e mais trabalho de sua parte, mas funciona um pouco mais rápido.

Se seu programa faz uso intensivo de adicionar valores `double` a objetos `PesoPedra`, pode valer a pena sobrecarregar a adição para lidar com tais casos diretamente. Se o programa usa essa adição apenas ocasionalmente, é mais simples confiar em conversões automáticas ou, se você quiser ser mais cuidadoso, em conversões explícitas.

---

## Resumo (Summary)

Este capítulo cobre muitos aspectos importantes da definição e uso de classes. Parte do material deste capítulo pode parecer vago até que suas próprias experiências enriqueçam sua compreensão.

Normalmente, a única maneira de você acessar membros privados de uma classe é usando um método de classe. O C++ alivia essa restrição com funções amigas. Para tornar uma função amiga de uma classe, você declara a função na declaração de classe e prefixa a declaração com a palavra-chave `friend`.

O C++ estende a sobrecarga aos operadores deixando você definir funções operadoras especiais que descrevem como operadores específicos se relacionam com uma determinada classe. Uma função operadora pode ser uma função membro de classe ou uma função amiga. (Alguns operadores só podem ser funções membro de classe.) O C++ permite que você invoque uma função operadora tanto chamando a função quanto usando o operador sobrecarregado com sua sintaxe usual. Uma função operadora para o `operadorop` tem esta forma:

```
operatorop(lista-de-argumentos)
```

A `lista-de-argumentos` representa os operandos do operador. Se a função operadora for uma função membro de classe, o primeiro operando é o objeto invocador e não faz parte de `lista-de-argumentos`. Por exemplo, este capítulo sobrecarregou a adição definindo uma função membro `operator+()` para a classe `Vetor`. Se `cima`, `direita` e `resultado` são três vetores, você pode usar qualquer uma das seguintes instruções para invocar a adição de vetores:

```cpp
resultado = cima.operator+(direita);
resultado = cima + direita;
```

Para a segunda versão, o fato de que os operandos `cima` e `direita` são do tipo `Vetor` diz ao C++ para usar a definição de adição de `Vetor`.

Quando uma função operadora é uma função membro, o primeiro operando é o objeto que invoca a função. Nos exemplos anteriores, por exemplo, o objeto `cima` é o objeto invocador. Se você quiser definir uma função operadora de forma que o primeiro operando não seja um objeto de classe, você deve usar uma função amiga. Então você pode passar os operandos para a definição da função em qualquer ordem que quiser.

Uma das tarefas mais comuns para a sobrecarga de operadores é definir o operador `<<` para que possa ser usado em conjunto com o objeto `cout` para exibir o conteúdo de um objeto. Para permitir que um objeto `ostream` seja o primeiro operando, você define a função operadora como amiga. Para permitir que o operador redefinido seja concatenado consigo mesmo, você torna o tipo de retorno `ostream &`. Aqui está uma forma geral que satisfaz esses requisitos:

```cpp
ostream & operator<<(ostream & os, const c_nome & obj)
{
    os << ... ; // exibir conteúdo do objeto
    return os;
}
```

Se, entretanto, a classe tiver métodos que retornam valores para os membros de dados que você quer exibir, você pode usar esses métodos em vez de acesso direto em `operator<<()`. Nesse caso, a função não precisa (e não deve) ser uma amiga.

O C++ permite estabelecer conversões de e para tipos de classe. Primeiro, qualquer construtor de classe que recebe um único argumento age como função de conversão, convertendo valores do tipo do argumento para o tipo da classe. O C++ invoca o construtor automaticamente se você atribui um valor do tipo do argumento a um objeto. Se, no entanto, você precede a declaração do construtor com a palavra-chave `explicit`, o construtor pode ser usado apenas para conversões explícitas:

```cpp
bean = String("pinto"); // converte tipo char* para tipo String explicitamente
```

Para converter de uma classe para outro tipo, você deve definir uma função de conversão e fornecer instruções sobre como fazer a conversão. Uma função de conversão deve ser uma função membro. Se ela deve converter para `nomeTipo`, deve ter o seguinte protótipo:

```cpp
operator nomeTipo();
```

Observe que não deve ter tipo de retorno declarado, não deve ter argumentos, e deve (apesar de não ter tipo de retorno declarado) retornar o valor convertido. A experiência mostrou que muitas vezes é melhor não depender de tais funções de conversão implícita.

Como você deve ter notado, as classes requerem muito mais cuidado e atenção aos detalhes do que simples estruturas no estilo C. Em troca, elas fazem muito mais por você.

---

## Revisão do Capítulo (Chapter Review)

1. Use uma função membro para sobrecarregar o operador de multiplicação para a classe `PesoPedra`; faça o operador multiplicar os membros de dados por um valor do tipo `double`. Observe que isso exigirá transbordo para a representação pedras–libras. Ou seja, o dobro de 10 pedras 8 libras é 21 pedras 2 libras.

2. Quais são as diferenças entre uma função amiga e uma função membro?

3. Uma função não membro precisa ser amiga para acessar os membros de uma classe?

4. Use uma função amiga para sobrecarregar o operador de multiplicação para a classe `PesoPedra`; faça o operador multiplicar o valor `double` pelo valor `PesoPedra`.

5. Quais operadores não podem ser sobrecarregados?

6. Que restrição se aplica à sobrecarga dos seguintes operadores: `=`, `()`, `[]` e `->`?

7. Defina uma função de conversão para a classe `Vetor` que converte um objeto `Vetor` em um valor do tipo `double` que representa a magnitude do vetor.

---

## Exercícios de Programação (Programming Exercises)

1. Modifique a Listagem 11.15 para que ela escreva as localizações sucessivas do caminhante aleatório em um arquivo. Rotule cada posição com o número do passo. Também faça o programa escrever as condições iniciais (distância alvo e tamanho do passo) e os resultados resumidos no arquivo. O conteúdo do arquivo poderia ser assim:

   ```
   Distância Alvo: 100, Tamanho do Passo: 20
   0: (x,y) = (0, 0)
   1: (x,y) = (-11.4715, 16.383)
   2: (x,y) = (-8.68807, -3.42232)
   ...
   26: (x,y) = (42.2919, -78.2594)
   27: (x,y) = (58.6749, -89.7309)
   Apos 27 passos, o sujeito tem a seguinte localizacao:
   (x,y) = (58.6749, -89.7309)
    ou
   (m,a) = (107.212, -56.8194)
   Distância media de afastamento por passo = 3.97081
   ```

2. Modifique os arquivos de cabeçalho e implementação da classe `Vetor` (Listagens 11.13 e 11.14) para que magnitude e ângulo não sejam mais armazenados como componentes de dados. Em vez disso, eles devem ser calculados sob demanda quando os métodos `valmag()` e `valang()` são chamados. Você deve deixar a interface pública inalterada (os mesmos métodos públicos com os mesmos argumentos) mas alterar a seção privada, incluindo alguns dos métodos privados e as implementações dos métodos. Teste a versão modificada com a Listagem 11.15, que deve ser deixada sem alteração porque a interface pública da classe `Vetor` não mudou.

3. Modifique a Listagem 11.15 para que, em vez de relatar os resultados de uma única tentativa para uma combinação específica de alvo/passo, ela reporte o maior, menor e número médio de passos para N tentativas, onde N é um número inteiro inserido pelo usuário.

4. Reescreva o exemplo final da classe `Tempo` (Listagens 11.10, 11.11 e 11.12) de forma que todos os operadores sobrecarregados sejam implementados usando funções amigas.

5. Reescreva a classe `PesoPedra` (Listagens 11.16 e 11.17) para que ela tenha um membro de estado que governe se o objeto é interpretado na forma de pedras, na forma de libras inteiras ou na forma de libras de ponto flutuante. Sobrecarregue o operador `<<` para substituir os métodos `exibir_pedras()` e `exibir_libras()`. Sobrecarregue os operadores de adição, subtração e multiplicação para que se possa somar, subtrair e multiplicar valores `PesoPedra`. Teste sua classe com um programa curto que usa todos os métodos e amigas da classe.

6. Reescreva a classe `PesoPedra` (Listagens 11.16 e 11.17) para que ela sobrecarregue os seis operadores relacionais. Os operadores devem comparar os membros `libras` e retornar um valor do tipo `bool`. Escreva um programa que declare um array de seis objetos `PesoPedra` e inicialize os três primeiros objetos na declaração do array. Em seguida, deve usar um laço para ler valores usados para definir os três elementos restantes do array. Em seguida, deve relatar o menor elemento, o maior elemento e quantos elementos são maiores ou iguais a 11 pedras.

7. Um número complexo tem duas partes: uma parte real e uma parte imaginária. Uma maneira de escrever um número imaginário é esta: `(3.0, 4.0)`. Aqui `3.0` é a parte real e `4.0` é a parte imaginária. Suponha `a = (A, Bi)` e `c = (C, Di)`. Aqui estão algumas operações complexas:

   - Adição: `a + c = (A + C, (B + D)i)`
   - Subtração: `a - c = (A - C, (B - D)i)`
   - Multiplicação: `a × c = (A × C - B × D, (A × D + B × C)i)`
   - Multiplicação: `(x um número real): x × c = (x × C, x × Di)`
   - Conjugação: `~a = (A, -Bi)`

   Defina uma classe `complexo` para que o seguinte programa possa usá-la com resultados corretos:

   ```cpp
   #include <iostream>
   using namespace std;
   #include "complex0.h" // para evitar confusão com complex.h
   int main()
   {
       complexo a(3.0, 4.0); // inicializa para (3,4i)
       complexo c;
       cout << "Entre com um numero complexo (q para sair):\n";
       while (cin >> c)
       {
           cout << "c e " << c << '\n';
           cout << "conjugado complexo e " << ~c << '\n';
           cout << "a e " << a << '\n';
           cout << "a + c e " << a + c << '\n';
           cout << "a - c e " << a - c << '\n';
           cout << "a * c e " << a * c << '\n';
           cout << "2 * c e " << 2 * c << '\n';
           cout << "Entre com um numero complexo (q para sair):\n";
       }
       cout << "Pronto!\n";
       return 0;
   }
   ```

   Observe que você tem que sobrecarregar os operadores `<<` e `>>`. Use `const` sempre que justificado. Aqui está uma execução de amostra do programa:

   ```
   Entre com um numero complexo (q para sair):
   real: 10
   imaginario: 12
   c e (10,12i)
   conjugado complexo e (10,-12i)
   a e (3,4i)
   a + c e (13,16i)
   a - c e (-7,-8i)
   a * c e (-18,76i)
   2 * c e (20,24i)
   Entre com um numero complexo (q para sair):
   real: q
   Pronto!
   ```

---

> Navegação: [Anterior](capitulo-11-02-funcoes-amigas-vetores.md) | [Índice](README.md) | [Próximo](capitulo-12-01-alocacao-dinamica.md)
