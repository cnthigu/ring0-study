# Capítulo 10 — Construtores, Destrutores e o Ponteiro this

> Tradução não oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Navegação: [Anterior](capitulo-10-01-objetos-classes.md) | [Índice](README.md) | [Próximo](capitulo-10-03-escopo-adt-resumo.md)

## Construtores e Destrutores de Classe (Class Constructors and Destructors)

Neste ponto, você precisa fazer mais com a classe `Acao`. Existem certas funções padrão, chamadas *construtores* (constructors) e *destrutores* (destructors), que você normalmente deve fornecer para uma classe. Vamos falar sobre por que eles são necessários e como escrevê-los.

Um dos objetivos do C++ é tornar o uso de objetos de classe semelhante ao uso de tipos padrão. No entanto, o código fornecido até agora neste capítulo não permite que você inicialize um objeto `Acao` da mesma forma que pode inicializar um `int` comum ou uma `struct`. Ou seja, a sintaxe de inicialização usual não funciona para o tipo `Acao`:

```cpp
int ano = 2001;                     // inicialização válida
struct coisa
{
    char * pn;
    int m;
};
coisa balbucio = {"engeco", -23};   // inicialização válida
Acao quente = {"Autos da Sukie", 200, 50.25}; // NÃO! erro de compilação
```

A razão pela qual você não pode inicializar um objeto `Acao` dessa forma é que as partes de dados têm status de acesso privado, o que significa que um programa não pode acessar os membros de dados diretamente. Como você viu, a única forma de um programa acessar os membros de dados é através de uma função membro. Portanto, você precisa criar uma função membro apropriada se quiser inicializar um objeto. (Você poderia inicializar um objeto de classe como acabou de mostrar se tornasse os membros de dados públicos em vez de privados, mas tornar os dados públicos vai contra uma das principais justificativas para usar classes: ocultação de dados.)

Em geral, é melhor que todos os objetos sejam inicializados quando são criados. Por exemplo, considere o seguinte código:

```cpp
Acao presente;
presente.comprar(10, 24.75);
```

Com a implementação atual da classe `Acao`, o objeto `presente` não tem valor para o membro `empresa`. O design da classe pressupõe que o usuário chama `adquirir()` antes de chamar qualquer outra função membro, mas não há como impor essa suposição. Uma maneira de contornar essa dificuldade é ter objetos inicializados automaticamente quando são criados. Para realizar isso, o C++ fornece funções membros especiais, chamadas *construtores de classe* (class constructors), especialmente para construir novos objetos e atribuir valores aos seus membros de dados. Mais precisamente, o C++ fornece um nome para essas funções membros e uma sintaxe para usá-los, e você fornece a definição do método. O nome é o mesmo que o nome da classe. Por exemplo, um possível construtor para a classe `Acao` é uma função membro chamada `Acao()`. O protótipo de construtor e o cabeçalho têm uma propriedade interessante: embora o construtor não tenha valor de retorno, ele não é declarado como tipo `void`. Na verdade, um construtor não tem tipo declarado.

### Declarando e Definindo Construtores (Declaring and Defining Constructors)

Agora você precisa construir um construtor `Acao`. Como um objeto `Acao` tem três valores a serem fornecidos do mundo externo, você deve dar ao construtor três argumentos. (O quarto valor, o membro `val_total`, é calculado de `num_acoes` e `val_acao`, então você não precisa fornecê-lo ao construtor.) Possivelmente, você pode querer fornecer apenas o valor do membro `empresa` e definir os outros valores como zero; você pode fazer isso usando argumentos padrão (veja o Capítulo 8). Assim, o protótipo seria assim:

```cpp
// protótipo de construtor com alguns argumentos padrão
Acao(const string & emp, long n = 0, double pr = 0.0);
```

O primeiro argumento é uma referência para a string que é usada para inicializar o membro `empresa`. Os argumentos `n` e `pr` fornecem valores para os membros `num_acoes` e `val_acao`. Observe que não há tipo de retorno. O protótipo vai na seção pública da declaração de classe.

A seguir, aqui está uma possível definição para o construtor:

```cpp
// definição de construtor
Acao::Acao(const string & emp, long n, double pr)
{
    empresa = emp;
    if (n < 0)
    {
        std::cerr << "O numero de acoes nao pode ser negativo; "
                  << empresa << " acoes definidas como 0.\n";
        num_acoes = 0;
    }
    else
        num_acoes = n;
    val_acao = pr;
    def_total();
}
```

Este é o mesmo código que a função `adquirir()` usou anteriormente neste capítulo. A diferença é que, neste caso, um programa invoca automaticamente o construtor quando declara um objeto.

> **Nomes de Membros e Nomes de Parâmetros**
>
> Muitas vezes, aqueles que são novos em construtores tentam usar os nomes de membros de classe como nomes de parâmetros no construtor, como neste exemplo:
>
> ```cpp
> // NÃO!
> Acao::Acao(const string & empresa, long num_acoes, double val_acao)
> {
>     ...
> }
> ```
>
> Isso está errado. Os argumentos do construtor não representam os membros da classe; eles representam valores que são atribuídos aos membros da classe. Portanto, eles devem ter nomes distintos, ou você acabará com código confuso como este:
>
> ```cpp
> num_acoes = num_acoes;
> ```
>
> Uma prática de codificação comum para ajudar a evitar essa confusão é usar um prefixo `m_` para identificar nomes de membros de dados:
>
> ```cpp
> class Acao
> {
> private:
>     string m_empresa;
>     long m_num_acoes;
>     ...
> ```
>
> Outra prática comum é usar um sufixo de sublinhado para nomes de membros:
>
> ```cpp
> class Acao
> {
> private:
>     string empresa_;
>     long num_acoes_;
>     ...
> ```

### Usando Construtores (Using Constructors)

O C++ fornece duas maneiras de inicializar um objeto usando um construtor. A primeira é chamar o construtor explicitamente:

```cpp
Acao produto = Acao("Repolho Mundial", 250, 1.25);
```

Isso define o membro `empresa` do objeto `produto` para a string "Repolho Mundial", o membro `num_acoes` para 250, e assim por diante.

A segunda maneira é chamar o construtor implicitamente:

```cpp
Acao vestuario("Pelo Mason", 50, 2.5);
```

Esta forma mais compacta é equivalente à seguinte chamada explícita:

```cpp
Acao vestuario = Acao("Pelo Mason", 50, 2.5);
```

O C++ usa um construtor de classe sempre que você cria um objeto dessa classe, mesmo quando você usa `new` para alocação dinâmica de memória. Aqui está como usar o construtor com `new`:

```cpp
Acao *pacao = new Acao("Jogos de Eletroconvulsao", 18, 19.0);
```

Esta instrução cria um objeto `Acao`, inicializa-o com os valores fornecidos pelos argumentos e atribui o endereço do objeto ao ponteiro `pacao`. Neste caso, o objeto não tem nome, mas você pode usar o ponteiro para gerenciá-lo.

Construtores são usados de forma diferente dos outros métodos de classe. Normalmente, você usa um objeto para invocar um método:

```cpp
acao1.exibir();  // o objeto acao1 invoca o método exibir()
```

No entanto, você não pode usar um objeto para invocar um construtor porque até que o construtor termine seu trabalho de criar o objeto, não há nenhum objeto. Em vez de ser invocado por um objeto, o construtor é usado para criar o objeto.

### Construtores Padrão (Default Constructors)

Um *construtor padrão* (default constructor) é um construtor que é usado para criar um objeto quando você não fornece valores de inicialização explícitos. Ou seja, é um construtor usado para declarações como esta:

```cpp
Acao gato_fofo;  // usa o construtor padrão
```

Se você falhar em fornecer algum construtor, o C++ automaticamente fornece um construtor padrão. É uma versão implícita de um construtor padrão, e não faz nada. Para a classe `Acao`, o construtor padrão ficaria assim:

```cpp
Acao::Acao() { }
```

O resultado líquido é que o objeto `gato_fofo` é criado com seus membros não inicializados, assim como o seguinte cria `x` sem fornecer um valor para `x`:

```cpp
int x;
```

Um fato curioso sobre construtores padrão é que o compilador fornece um apenas se você não define nenhum construtor. Depois que você define qualquer construtor para uma classe, a responsabilidade de fornecer um construtor padrão para essa classe passa do compilador para você. Se você fornecer um construtor não-padrão, como `Acao(const string & emp, long n, double pr)`, e não fornecer sua própria versão de um construtor padrão, então uma declaração como esta se torna um erro:

```cpp
Acao acao1;  // não é possível com o construtor atual
```

A razão para esse comportamento é que você pode querer impossibilitar a criação de objetos não inicializados. Se, no entanto, você deseja criar objetos sem inicialização explícita, deve definir seu próprio construtor padrão. Este é um construtor que não tem argumentos. Você pode definir um construtor padrão de duas maneiras. Uma é fornecer valores padrão para todos os argumentos do construtor existente:

```cpp
Acao(const string & emp = "Erro", int n = 0, double pr = 0.0);
```

A segunda é usar sobrecarga de função para definir um segundo construtor, um que não tem argumentos:

```cpp
Acao();
```

Você pode ter apenas um construtor padrão, então certifique-se de não fazer ambos. Na verdade, você normalmente deve inicializar objetos para garantir que todos os membros comecem com valores conhecidos e razoáveis. Assim, um construtor padrão fornecido pelo usuário tipicamente fornece inicialização implícita para todos os valores de membros. Por exemplo, assim é como você pode definir um para a classe `Acao`:

```cpp
Acao::Acao()  // construtor padrão
{
    empresa = "sem nome";
    num_acoes = 0;
    val_acao = 0.0;
    val_total = 0.0;
}
```

> **Dica:** Ao projetar uma classe, você normalmente deve fornecer um construtor padrão que inicializa implicitamente todos os membros da classe.

Depois de usar qualquer método (sem argumentos ou valores padrão para todos os argumentos) para criar o construtor padrão, você pode declarar variáveis de objeto sem inicializá-las explicitamente:

```cpp
Acao primeiro;                    // chama o construtor padrão implicitamente
Acao primeiro = Acao();           // chama-o explicitamente
Acao *palivio = new Acao;         // chama-o implicitamente
```

No entanto, você não deve ser enganado pela forma implícita do construtor não-padrão:

```cpp
Acao primeiro("Conglomerado de Concreto");  // chama o construtor
Acao segundo();                              // declara uma função
Acao terceiro;                               // chama o construtor padrão
```

A primeira declaração aqui chama o construtor não-padrão — ou seja, aquele que aceita argumentos. A segunda declaração afirma que `segundo()` é uma função que retorna um objeto `Acao`. Quando você chama implicitamente o construtor padrão, você não usa parênteses.

### Destrutores (Destructors)

Quando você usa um construtor para criar um objeto, o programa assume a responsabilidade de rastrear esse objeto até que ele expire. Nesse momento, o programa chama automaticamente uma função membro especial com o título formidável de *destrutor* (destructor). O destrutor deve limpar qualquer resíduo, então na verdade serve a um propósito útil. Por exemplo, se o seu construtor usa `new` para alocar memória, o destrutor deve usar `delete` para liberar essa memória. O construtor `Acao` não faz nada sofisticado como usar `new`, então o destrutor da classe `Acao` realmente não tem nenhuma tarefa a realizar. Nesse caso, você pode simplesmente deixar o compilador gerar um destrutor implícito que não faz nada, que é exatamente o que a primeira versão da classe `Acao` faz.

Como um construtor, um destrutor tem um nome especial: é formado pelo nome da classe precedido por um til (`~`). Assim, o destrutor para a classe `Acao` é chamado `~Acao()`. Também como um construtor, um destrutor pode não ter valor de retorno e não tem tipo declarado. Ao contrário de um construtor, um destrutor não deve ter argumentos. Assim, o protótipo para um destrutor `Acao` deve ser este:

```cpp
~Acao();
```

Como um destrutor `Acao` não tem deveres vitais, você pode codificá-lo como uma função que não faz nada:

```cpp
Acao::~Acao()
{
}
```

No entanto, para que você possa ver quando o destrutor é chamado, vamos codificá-lo desta forma:

```cpp
Acao::~Acao()  // destrutor da classe
{
    cout << "Tchau, " << empresa << "!\n";
}
```

Quando um destrutor deve ser chamado? O compilador lida com essa decisão; normalmente seu código não deve chamar um destrutor explicitamente. Se você criar um objeto de classe de armazenamento estático, seu destrutor será chamado automaticamente quando o programa terminar. Se você criar um objeto de classe de armazenamento automático, como os exemplos têm feito, seu destrutor é chamado automaticamente quando o programa sai do bloco de código no qual o objeto é definido. Se o objeto for criado usando `new`, ele reside na memória heap, ou armazenamento livre, e seu destrutor é chamado automaticamente quando você usa `delete` para liberar a memória. Por fim, um programa pode criar objetos temporários para realizar certas operações; nesse caso, o programa chama automaticamente o destrutor para o objeto quando terminar de usá-lo.

## Melhorando a Classe Acao (Improving the Stock Class)

Neste ponto precisamos incorporar os construtores e o destrutor na classe e nas definições de métodos. Dado o significado de adicionar construtores, avançaremos o nome de `acao00.h` para `acao10.h`. Os métodos da classe vão para um arquivo chamado `acao10.cpp`. Por fim, colocamos o programa que usa esses recursos em um terceiro arquivo, `usaacao1.cpp`.

**Listagem 10.4** acao10.h

```cpp
// acao10.h -- declaração da classe Acao com construtores, destrutor adicionados
#ifndef ACAO10_H_
#define ACAO10_H_
#include <string>

class Acao
{
private:
    std::string empresa;
    long num_acoes;
    double val_acao;
    double val_total;
    void def_total() { val_total = num_acoes * val_acao; }
public:
    // dois construtores
    Acao();  // construtor padrão
    Acao(const std::string & emp, long n = 0, double pr = 0.0);
    ~Acao(); // destrutor verboso
    void comprar(long num, double preco);
    void vender(long num, double preco);
    void atualizar(double preco);
    void exibir();
};
#endif
```

**Listagem 10.5** acao10.cpp

```cpp
// acao10.cpp -- classe Acao com construtores, destrutor adicionados
#include <iostream>
#include "acao10.h"
// construtores (versões verbosas)
Acao::Acao()  // construtor padrão
{
    std::cout << "Construtor padrao chamado\n";
    empresa = "sem nome";
    num_acoes = 0;
    val_acao = 0.0;
    val_total = 0.0;
}
Acao::Acao(const std::string & emp, long n, double pr)
{
    std::cout << "Construtor usando " << emp << " chamado\n";
    empresa = emp;
    if (n < 0)
    {
        std::cout << "O numero de acoes nao pode ser negativo; "
                  << empresa << " acoes definidas como 0.\n";
        num_acoes = 0;
    }
    else
        num_acoes = n;
    val_acao = pr;
    def_total();
}
// destrutor da classe
Acao::~Acao()  // destrutor da classe verboso
{
    std::cout << "Tchau, " << empresa << "!\n";
}
// outros métodos
void Acao::comprar(long num, double preco)
{
    if (num < 0)
    {
        std::cout << "O numero de acoes compradas nao pode ser negativo. "
                  << "Transacao cancelada.\n";
    }
    else
    {
        num_acoes += num;
        val_acao = preco;
        def_total();
    }
}
void Acao::vender(long num, double preco)
{
    using std::cout;
    if (num < 0)
    {
        cout << "O numero de acoes vendidas nao pode ser negativo. "
             << "Transacao cancelada.\n";
    }
    else if (num > num_acoes)
    {
        cout << "Voce nao pode vender mais do que tem! "
             << "Transacao cancelada.\n";
    }
    else
    {
        num_acoes -= num;
        val_acao = preco;
        def_total();
    }
}
void Acao::atualizar(double preco)
{
    val_acao = preco;
    def_total();
}
void Acao::exibir()
{
    using std::cout;
    using std::ios_base;
    // define formato para #.###
    ios_base::fmtflags orig =
        cout.setf(ios_base::fixed, ios_base::floatfield);
    std::streamsize prec = cout.precision(3);
    cout << "Empresa: " << empresa
         << " Acoes: " << num_acoes << '\n';
    cout << " Preco por Acao: R$" << val_acao;
    // define formato para #.##
    cout.precision(2);
    cout << " Valor Total: R$" << val_total << '\n';
    // restaura o formato original
    cout.setf(orig, ios_base::floatfield);
    cout.precision(prec);
}
```

**Listagem 10.6** usaacao1.cpp

```cpp
// usaacao1.cpp -- usando a classe Acao
// compile com acao10.cpp
#include <iostream>
#include "acao10.h"
int main()
{
    {
        using std::cout;
        cout << "Usando construtores para criar novos objetos\n";
        Acao acao1("NanoSmart", 12, 20.0);   // sintaxe 1
        acao1.exibir();
        Acao acao2 = Acao("Objetos Boffo", 2, 2.0);  // sintaxe 2
        acao2.exibir();
        cout << "Atribuindo acao1 a acao2:\n";
        acao2 = acao1;
        cout << "Listando acao1 e acao2:\n";
        acao1.exibir();
        acao2.exibir();
        cout << "Usando um construtor para redefinir um objeto\n";
        acao1 = Acao("Alimentos Elegantes", 10, 50.0);  // objeto temporário
        cout << "acao1 revisada:\n";
        acao1.exibir();
        cout << "Concluido\n";
    }
    return 0;
}
```

Compilar o programa representado pelas Listagens 10.4, 10.5 e 10.6 produz um programa executável. Aqui está a saída de um compilador:

```
Usando construtores para criar novos objetos
Construtor usando NanoSmart chamado
Empresa: NanoSmart Acoes: 12
 Preco por Acao: R$20.000 Valor Total: R$240.00
Construtor usando Objetos Boffo chamado
Empresa: Objetos Boffo Acoes: 2
 Preco por Acao: R$2.000 Valor Total: R$4.00
Atribuindo acao1 a acao2:
Listando acao1 e acao2:
Empresa: NanoSmart Acoes: 12
 Preco por Acao: R$20.000 Valor Total: R$240.00
Empresa: NanoSmart Acoes: 12
 Preco por Acao: R$20.000 Valor Total: R$240.00
Usando um construtor para redefinir um objeto
Construtor usando Alimentos Elegantes chamado
Tchau, Alimentos Elegantes!
acao1 revisada:
Empresa: Alimentos Elegantes Acoes: 10
 Preco por Acao: R$50.000 Valor Total: R$500.00
Concluido
Tchau, NanoSmart!
Tchau, Alimentos Elegantes!
```

Alguns compiladores podem produzir um programa com a seguinte saída inicial, que tem uma linha adicional:

```
Usando construtores para criar novos objetos
Construtor usando NanoSmart chamado
Empresa: NanoSmart Acoes: 12
 Preco por Acao: R$20.000 Valor Total: R$240.00
Construtor usando Objetos Boffo chamado
Tchau, Objetos Boffo!        << linha adicional
Empresa: Objetos Boffo Acoes: 2
 Preco por Acao: R$2.000 Valor Total: R$4.00
...
```

> **Nota:** Você pode ter notado que a Listagem 10.6 tem uma chave extra no início e perto do final de `main()`. Variáveis automáticas como `acao1` e `acao2` expiram quando o programa sai do bloco que contém suas definições. Sem as chaves extras, esse bloco seria o corpo de `main()`, então os destrutores não seriam chamados até depois que `main()` concluísse a execução. Em um ambiente de janelas, isso significaria que a janela fecharia antes das últimas duas chamadas do destrutor, impedindo que você visse as duas últimas mensagens. Mas com as chaves, as duas últimas chamadas do destrutor ocorrem antes que a instrução `return` seja alcançada, então as mensagens são exibidas.

### Notas do Programa (Program Notes)

Na Listagem 10.6, a instrução

```cpp
Acao acao1("NanoSmart", 12, 20.0);
```

cria um objeto `Acao` chamado `acao1` e inicializa seus membros de dados com os valores indicados.

A seguinte instrução usa outra sintaxe para criar e inicializar um objeto chamado `acao2`:

```cpp
Acao acao2 = Acao("Objetos Boffo", 2, 2.0);
```

O Padrão C++ dá ao compilador algumas maneiras de executar esta segunda sintaxe. Uma é fazê-la se comportar exatamente como a primeira sintaxe. A segunda é permitir que a chamada ao construtor crie um objeto temporário que é copiado para `acao2`. Então o objeto temporário é descartado. Se o compilador usar esta opção, o destrutor é chamado para o objeto temporário.

A seguinte instrução ilustra que você pode atribuir um objeto a outro do mesmo tipo:

```cpp
acao2 = acao1;  // atribuição de objeto
```

Como com atribuição de estrutura, a atribuição de objeto de classe, por padrão, copia os membros de um objeto para o outro. Nesse caso, o conteúdo original de `acao2` é sobrescrito.

> **Nota:** Quando você atribui um objeto a outro da mesma classe, por padrão o C++ copia o conteúdo de cada membro de dados do objeto de origem para o membro de dados correspondente do objeto de destino.

Você pode usar o construtor para mais do que inicializar um novo objeto. Por exemplo, o programa tem esta instrução em `main()`:

```cpp
acao1 = Acao("Alimentos Elegantes", 10, 50.0);
```

O objeto `acao1` já existe. Portanto, em vez de inicializar `acao1`, esta instrução atribui novos valores ao objeto. Ela faz isso fazendo o construtor criar um novo objeto temporário e depois copiar o conteúdo do novo objeto para `acao1`. Então o programa descarta o objeto temporário, invocando o destrutor ao fazê-lo.

> **Dica:** Se você pode definir valores de objeto tanto através de inicialização quanto por atribuição, escolha a inicialização. Ela é geralmente mais eficiente.

### Inicialização de Lista C++11 (C++11 List Initialization)

Com C++11, você pode usar a sintaxe de inicialização de lista com classes, fornecendo que o conteúdo das chaves corresponda à lista de argumentos de um construtor:

```cpp
Acao dica_quente = {"Derivatives Plus Plus", 100, 45.0};
Acao atleta {"Sport Age Storage, Inc"};
Acao temp {};
```

As listas entre chaves nas duas primeiras declarações correspondem ao seguinte construtor:

```cpp
Acao::Acao(const std::string & emp, long n = 0, double pr = 0.0);
```

Portanto, esse construtor será usado para criar os dois objetos. Para `atleta`, os valores padrão de `0` e `0.0` serão usados para o segundo e terceiro argumentos. A terceira declaração corresponde ao construtor padrão, então `temp` é construído usando-o.

## Funções Membros const (const Member Functions)

Considere o seguinte fragmento de código:

```cpp
const Acao terra = Acao("Propriedades Kludgehorn");
terra.exibir();
```

Com o C++ atual, o compilador deve rejeitar a segunda linha. Por quê? Porque o código para `exibir()` não garante que não modificará o objeto que o invoca, que, por ser `const`, não deve ser alterado. Resolvemos esse tipo de problema antes declarando o argumento de uma função como uma referência `const` ou um ponteiro para `const`. Mas há um problema de sintaxe: O método `exibir()` não tem nenhum argumento para `const` qualificar. Em vez disso, o objeto que ele usa é fornecido implicitamente pela invocação do método. O que é necessário é uma nova sintaxe, que diz que uma função promete não modificar o objeto que a invoca. A solução C++ é colocar a palavra-chave `const` após os parênteses da função. Ou seja, a declaração `exibir()` deve ser assim:

```cpp
void exibir() const;  // promete não alterar o objeto que invoca
```

Da mesma forma, o início da definição da função deve ser assim:

```cpp
void Acao::exibir() const  // promete não alterar o objeto que invoca
```

Funções de classe declaradas e definidas dessa forma são chamadas de *funções membros const* (const member functions). Assim como você deve usar referências `const` e ponteiros como argumentos formais de função sempre que apropriado, você deve tornar os métodos de classe `const` sempre que eles não modificarem o objeto que os invoca. Seguiremos esta regra a partir daqui.

## Construtores e Destrutores em Revisão (Constructors and Destructors in Review)

Agora que passamos por alguns exemplos de construtores e destrutores, aqui está um resumo desses métodos.

Um construtor é uma função membro de classe especial que é chamada sempre que um objeto dessa classe é criado. Um construtor de classe tem o mesmo nome que sua classe, mas através do milagre da sobrecarga de função, você pode ter mais de um construtor com o mesmo nome, desde que cada um tenha sua própria assinatura, ou lista de argumentos. Além disso, um construtor não tem tipo declarado. Normalmente um construtor é usado para inicializar membros de um objeto de classe. Por exemplo, suponha que a classe `Bozo` tenha o seguinte protótipo para um construtor de classe:

```cpp
Bozo(const char * fname, const char * lname);  // protótipo de construtor
```

Nesse caso, você pode usá-lo para inicializar novos objetos como segue:

```cpp
Bozo bozeta = bozo("Bozeta", "Grandão");    // forma primária
Bozo fufu("Fufu", "O'Tosco");               // forma curta
Bozo *pc = new Bozo("Popo", "Le Peu");      // objeto dinâmico
```

Se as regras C++11 estiverem em vigor, você pode usar inicialização de lista em vez disso:

```cpp
Bozo bozeta = {"Bozeta", "Grandão"};       // C++11
Bozo fufu{"Fufu", "O'Tosco"};              // C++11
Bozo *pc = new Bozo{"Popo", "Le Peu"};     // C++11
```

Se um construtor tem apenas um argumento, esse construtor é invocado se você inicializar um objeto para um valor que tem o mesmo tipo que o argumento do construtor. Por exemplo, suponha que você tenha este protótipo de construtor:

```cpp
Bozo(int idade);
```

Então você pode usar qualquer uma das seguintes formas para inicializar um objeto:

```cpp
Bozo gotinha = bozo(44);    // forma primária
Bozo roon(66);              // forma secundária
Bozo gorducho = 32;         // forma especial para construtores de um argumento
```

Um construtor padrão não tem argumentos, e é usado se você criar um objeto sem inicializá-lo explicitamente. Se você não fornecer nenhum construtor, o compilador define um construtor padrão para você. Caso contrário, você tem que fornecer seu próprio construtor padrão. Ele pode não ter argumentos ou então deve ter valores padrão para todos os argumentos:

```cpp
Bozo();                                      // protótipo de construtor padrão
Bistro(const char * s = "Chez Zero");        // padrão para a classe Bistro
```

O programa usa o construtor padrão para objetos não inicializados:

```cpp
Bozo bubi;       // usa o padrão
Bozo *pb = new Bozo;  // usa o padrão
```

Assim como um programa invoca um construtor quando um objeto é criado, ele invoca um destrutor quando um objeto é destruído. Você pode ter apenas um destrutor por classe. Ele não tem tipo de retorno (nem mesmo `void`), não tem argumentos e seu nome é o nome da classe precedido por um til. Por exemplo, o destrutor da classe `Bozo` tem o seguinte protótipo:

```cpp
~Bozo();  // destrutor da classe
```

Destrutores de classe que usam `delete` tornam-se necessários quando construtores de classe usam `new`.

## Conhecendo Seus Objetos: O Ponteiro this (Knowing Your Objects: The this Pointer)

Você ainda pode fazer mais com a classe `Acao`. Até agora, cada função membro de classe lidou com apenas um objeto: o objeto que a invoca. Às vezes, entretanto, um método pode precisar lidar com dois objetos, e fazer isso pode envolver um curioso ponteiro C++ chamado `this`. Vamos examinar como a necessidade de `this` pode se desdobrar.

Embora a classe `Acao` exiba dados, ela é deficiente em poder analítico. Por exemplo, ao olhar a saída de `exibir()`, você pode dizer qual de suas posições tem o maior valor, mas o programa não pode dizer porque não pode acessar `val_total` diretamente. A maneira mais direta de deixar um programa saber sobre dados armazenados é fornecer métodos que retornem valores. Tipicamente, você usa código inline para isso, como no seguinte exemplo:

```cpp
class Acao
{
private:
    // ...
    double val_total;
    // ...
public:
    double total() const { return val_total; }
    // ...
};
```

Esta definição, em efeito, torna `val_total` memória somente leitura no que diz respeito ao acesso direto do programa. Ou seja, você pode usar o método `total()` para obter o valor, mas a classe não fornece um método para especificamente redefinir o valor de `val_total`.

Ao adicionar essa função à declaração da classe, você pode deixar um programa investigar uma série de ações para encontrar aquela com o maior valor. No entanto, você pode adotar uma abordagem diferente, uma que ajuda você a aprender sobre o ponteiro `this`. A abordagem é definir uma função membro que examina dois objetos `Acao` e retorna uma referência para o maior dos dois. Tentar implementar esta abordagem levanta algumas questões interessantes.

Primeiro, como você fornece à função membro dois objetos para comparar? Suponha, por exemplo, que você decida nomear o método `maior_val()`. Então a chamada de função `acao1.maior_val()` acessa os dados do objeto `acao1`, enquanto a mensagem `acao2.maior_val()` acessa os dados do objeto `acao2`. Se você quiser que o método compare dois objetos, você tem que passar o segundo objeto como argumento. Para eficiência, você pode passar o argumento por referência. Ou seja, você pode ter o método `maior_val()` usar um argumento de tipo `const Acao &`.

Segundo, como você comunica a resposta do método de volta ao programa que o chama? A maneira mais direta é ter o método retornar uma referência ao objeto que tem o maior valor total. Assim, o método de comparação deve ter o seguinte protótipo:

```cpp
const Acao & maior_val(const Acao & s) const;
```

Esta função acessa um objeto implicitamente e um objeto explicitamente, e retorna uma referência a um desses dois objetos. O `const` nos parênteses afirma que a função não modificará o objeto acessado explicitamente, e o `const` que segue os parênteses afirma que a função não modificará o objeto acessado implicitamente. Como a função retorna uma referência a um dos dois objetos `const`, o tipo de retorno também deve ser uma referência `const`.

Suponha então que você queira comparar os objetos `Acao` `acao1` e `acao2` e atribuir aquele com o maior valor total ao objeto `topo`. Você pode usar qualquer uma das seguintes instruções para fazer isso:

```cpp
topo = acao1.maior_val(acao2);
topo = acao2.maior_val(acao1);
```

A primeira forma acessa `acao1` implicitamente e `acao2` explicitamente, enquanto a segunda acessa `acao1` explicitamente e `acao2` implicitamente. De qualquer forma, o método compara os dois objetos e retorna uma referência àquele com o maior valor total.

Agora há ainda a implementação de `maior_val()` a ser resolvida. Ela levanta um problema menor. Aqui está uma implementação parcial que destaca o problema:

```cpp
const Acao & Acao::maior_val(const Acao & s) const
{
    if (s.val_total > val_total)
        return s;      // objeto argumento
    else
        return ?????;  // objeto que invoca
}
```

Aqui `s.val_total` é o valor total para o objeto passado como argumento, e `val_total` é o valor total para o objeto ao qual a mensagem é enviada. Se `s.val_total` for maior que `val_total`, a função retorna uma referência a `s`. Caso contrário, ela retorna uma referência ao objeto usado para evocar o método. Aqui está o problema: Que nome você dá a esse objeto? Se você fizer a chamada `acao1.maior_val(acao2)`, então `s` é uma referência para `acao2` (ou seja, um alias para `acao2`), mas não há alias para `acao1`.

A solução C++ para esse problema é usar um ponteiro especial chamado `this`. O ponteiro `this` aponta para o objeto usado para invocar uma função membro. (Basicamente, `this` é passado como um argumento oculto para o método.) Assim, a chamada de função `acao1.maior_val(acao2)` define `this` como o endereço do objeto `acao1` e torna esse ponteiro disponível para o método `maior_val()`. Da mesma forma, a chamada de função `acao2.maior_val(acao1)` define `this` como o endereço do objeto `acao2`. Em geral, todos os métodos de classe têm um ponteiro `this` definido como o endereço do objeto que invoca o método. De fato, `val_total` em `maior_val()` é apenas notação abreviada para `this->val_total`.

> **Nota:** Cada função membro, incluindo construtores e destrutores, tem um ponteiro `this`. A propriedade especial do ponteiro `this` é que ele aponta para o objeto que invoca. Se um método precisar se referir ao objeto que o invoca como um todo, ele pode usar a expressão `*this`. Usar o qualificador `const` após os parênteses do argumento da função qualifica `this` como sendo um ponteiro para `const`; nesse caso, você não pode usar `this` para alterar o valor do objeto.

O que você quer retornar, entretanto, não é `this` porque `this` é o endereço do objeto. Você quer retornar o próprio objeto, e isso é simbolizado por `*this`. (Lembre-se de que aplicar o operador de desreferência `*` a um ponteiro produz o valor para o qual o ponteiro aponta.) Agora você pode completar a definição do método usando `*this` como alias para o objeto que invoca:

```cpp
const Acao & Acao::maior_val(const Acao & s) const
{
    if (s.val_total > val_total)
        return s;       // objeto argumento
    else
        return *this;   // objeto que invoca
}
```

O fato de que o tipo de retorno é uma referência significa que o objeto retornado é o próprio objeto que invoca, em vez de uma cópia passada pelo mecanismo de retorno. A Listagem 10.7 mostra o novo arquivo de cabeçalho.

**Listagem 10.7** acao20.h

```cpp
// acao20.h -- versão ampliada
#ifndef ACAO20_H_
#define ACAO20_H_
#include <string>
class Acao
{
private:
    std::string empresa;
    int num_acoes;
    double val_acao;
    double val_total;
    void def_total() { val_total = num_acoes * val_acao; }
public:
    Acao();   // construtor padrão
    Acao(const std::string & emp, long n = 0, double pr = 0.0);
    ~Acao();  // destrutor silencioso
    void comprar(long num, double preco);
    void vender(long num, double preco);
    void atualizar(double preco);
    void exibir() const;
    const Acao & maior_val(const Acao & s) const;
};
#endif
```

A Listagem 10.8 apresenta o arquivo revisado de métodos da classe. Ela inclui o novo método `maior_val()`. Além disso, agora que você viu como os construtores e o destrutor funcionam, a Listagem 10.8 os substitui por versões silenciosas.

**Listagem 10.8** acao20.cpp

```cpp
// acao20.cpp -- versão ampliada
#include <iostream>
#include "acao20.h"
// construtores
Acao::Acao()  // construtor padrão
{
    empresa = "sem nome";
    num_acoes = 0;
    val_acao = 0.0;
    val_total = 0.0;
}
Acao::Acao(const std::string & emp, long n, double pr)
{
    empresa = emp;
    if (n < 0)
    {
        std::cout << "O numero de acoes nao pode ser negativo; "
                  << empresa << " acoes definidas como 0.\n";
        num_acoes = 0;
    }
    else
        num_acoes = n;
    val_acao = pr;
    def_total();
}
// destrutor da classe
Acao::~Acao()  // destrutor silencioso
{
}
// outros métodos
void Acao::comprar(long num, double preco)
{
    if (num < 0)
    {
        std::cout << "O numero de acoes compradas nao pode ser negativo. "
                  << "Transacao cancelada.\n";
    }
    else
    {
        num_acoes += num;
        val_acao = preco;
        def_total();
    }
}
void Acao::vender(long num, double preco)
{
    using std::cout;
    if (num < 0)
    {
        cout << "O numero de acoes vendidas nao pode ser negativo. "
             << "Transacao cancelada.\n";
    }
    else if (num > num_acoes)
    {
        cout << "Voce nao pode vender mais do que tem! "
             << "Transacao cancelada.\n";
    }
    else
    {
        num_acoes -= num;
        val_acao = preco;
        def_total();
    }
}
void Acao::atualizar(double preco)
{
    val_acao = preco;
    def_total();
}
void Acao::exibir() const
{
    using std::cout;
    using std::ios_base;
    // define formato para #.###
    ios_base::fmtflags orig =
        cout.setf(ios_base::fixed, ios_base::floatfield);
    std::streamsize prec = cout.precision(3);
    cout << "Empresa: " << empresa
         << " Acoes: " << num_acoes << '\n';
    cout << " Preco por Acao: R$" << val_acao;
    // define formato para #.##
    cout.precision(2);
    cout << " Valor Total: R$" << val_total << '\n';
    // restaura o formato original
    cout.setf(orig, ios_base::floatfield);
    cout.precision(prec);
}
const Acao & Acao::maior_val(const Acao & s) const
{
    if (s.val_total > val_total)
        return s;
    else
        return *this;
}
```

## Um Array de Objetos (An Array of Objects)

Muitas vezes, como com os exemplos de `Acao`, você quer criar vários objetos da mesma classe. Você pode criar variáveis de objeto separadas, como os exemplos fizeram até agora neste capítulo, mas pode ser mais sensato criar um array de objetos. Você declara um array de objetos da mesma maneira que declara um array de qualquer um dos tipos padrão:

```cpp
Acao minhas_acoes[4];  // cria um array de 4 objetos Acao
```

Lembre-se de que um programa sempre chama o construtor de classe padrão quando cria objetos de classe que não são explicitamente inicializados. Esta declaração requer que a classe não defina explicitamente nenhum construtor, caso em que o construtor padrão implícito que não faz nada é usado, ou, como neste caso, que um construtor padrão explícito seja definido. Cada elemento — `minhas_acoes[0]`, `minhas_acoes[1]`, e assim por diante — é um objeto `Acao` e assim pode ser usado com os métodos de `Acao`:

```cpp
minhas_acoes[0].atualizar();  // aplica atualizar() ao 1º elemento
minhas_acoes[3].exibir();     // aplica exibir() ao 4º elemento
const Acao * topos = minhas_acoes[2].maior_val(minhas_acoes[1]);
// compara 3º e 2º elementos e define topos
// para apontar para aquele com valor total mais alto
```

Você pode usar um construtor para inicializar os elementos do array. Nesse caso, você tem que chamar o construtor para cada elemento individual:

```cpp
const int NUM = 4;
Acao acoes[NUM] = {
    Acao("NanoSmart", 12.5, 20),
    Acao("Objetos Boffo", 200, 2.0),
    Acao("Obeliscos Monoliticos", 130, 3.25),
    Acao("Fleep Enterprises", 60, 6.5)
};
```

Aqui o código usa a forma padrão para inicializar um array: uma lista de valores separados por vírgula entre chaves. Neste caso, uma chamada ao método construtor representa cada valor. Se a classe tem mais de um construtor, você pode usar diferentes construtores para elementos diferentes:

```cpp
const int NUM = 10;
Acao acoes[NUM] = {
    Acao("NanoSmart", 12.5, 20),
    Acao(),
    Acao("Obeliscos Monoliticos", 130, 3.25),
};
```

Isso inicializa `acoes[0]` e `acoes[2]` usando o construtor `Acao(const string & emp, long n, double pr)` bem como `acoes[1]` usando o construtor `Acao()`. Como esta declaração inicializa apenas parcialmente o array, os sete membros restantes são inicializados usando o construtor padrão.

A Listagem 10.9 aplica esses princípios a um programa curto que inicializa quatro elementos do array, exibe seu conteúdo e testa os elementos para encontrar aquele com o valor total mais alto. Como `maior_val()` examina apenas dois objetos por vez, o programa usa um loop `for` para examinar todo o array. Além disso, usa um ponteiro para `Acao` para acompanhar qual elemento tem o valor mais alto.

**Listagem 10.9** usaacao2.cpp

```cpp
// usaacao2.cpp -- usando a classe Acao
// compile com acao20.cpp
#include <iostream>
#include "acao20.h"
const int NUM = 4;
int main()
{
    // cria um array de objetos inicializados
    Acao acoes[NUM] = {
        Acao("NanoSmart", 12, 20.0),
        Acao("Objetos Boffo", 200, 2.0),
        Acao("Obeliscos Monoliticos", 130, 3.25),
        Acao("Fleep Enterprises", 60, 6.5)
    };
    std::cout << "Posicoes em acoes:\n";
    int i;
    for (i = 0; i < NUM; i++)
        acoes[i].exibir();
    // define ponteiro para o primeiro elemento
    const Acao * topo = &acoes[0];
    for (i = 1; i < NUM; i++)
        topo = &topo->maior_val(acoes[i]);
    // agora topo aponta para a posição mais valiosa
    std::cout << "\nPosicao mais valiosa:\n";
    topo->exibir();
    return 0;
}
```

Aqui está a saída do programa na Listagem 10.9:

```
Posicoes em acoes:
Empresa: NanoSmart Acoes: 12
 Preco por Acao: R$20.000 Valor Total: R$240.00
Empresa: Objetos Boffo Acoes: 200
 Preco por Acao: R$2.000 Valor Total: R$400.00
Empresa: Obeliscos Monoliticos Acoes: 130
 Preco por Acao: R$3.250 Valor Total: R$422.50
Empresa: Fleep Enterprises Acoes: 60
 Preco por Acao: R$6.500 Valor Total: R$390.00

Posicao mais valiosa:
Empresa: Obeliscos Monoliticos Acoes: 130
 Preco por Acao: R$3.250 Valor Total: R$422.50
```

Uma coisa a observar sobre a Listagem 10.9 é que a maior parte do trabalho vai para projetar a classe. Quando isso é feito, escrever o programa em si é bastante simples.

Incidentalmente, conhecer sobre o ponteiro `this` torna mais fácil ver como o C++ funciona por baixo. Por exemplo, a implementação Unix original usava um front-end C++ `cfront` que convertia programas C++ em programas C. Para lidar com definições de métodos, tudo que tinha que fazer era converter uma definição de método C++ como

```cpp
void Acao::exibir() const
{
    cout << "Empresa: " << empresa << " Acoes: " << num_acoes << '\n'
         << " Preco por Acao: R$" << val_acao
         << " Valor Total: R$" << val_total << '\n';
}
```

para a seguinte definição no estilo C:

```cpp
void exibir(const Acao * this)
{
    cout << "Empresa: " << this->empresa << " Acoes: " << this->num_acoes << '\n'
         << " Preco por Acao: R$" << this->val_acao
         << " Valor Total: R$" << this->val_total << '\n';
}
```

Ou seja, convertia um qualificador `Acao::` em um argumento de função que é um ponteiro para `Acao` e depois usava o ponteiro para acessar membros de classe. Da mesma forma, o front-end convertia chamadas de função como

```cpp
topo.exibir();
```

para isto:

```cpp
exibir(&topo);
```

Dessa forma, o ponteiro `this` recebe o endereço do objeto que invoca.

---

Navegação: [Anterior](capitulo-10-01-objetos-classes.md) | [Índice](README.md) | [Próximo](capitulo-10-03-escopo-adt-resumo.md)
