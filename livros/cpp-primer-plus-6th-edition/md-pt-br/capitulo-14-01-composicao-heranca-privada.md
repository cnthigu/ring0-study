# Capítulo 14 — Reutilizando Código em C++ (Reusing Code in C++): Composição e Herança Privada

> Tradução não oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Navegação: [Anterior](capitulo-13-03-revisao-resumo.md) | [Índice](README.md) | [Próximo](capitulo-14-02-heranca-multipla.md)

Neste capítulo você aprenderá sobre o seguinte:

- Relacionamentos has-a (tem-um)
- Classes com objetos membros (composição, containment)
- A classe template `valarray`
- Herança privada e protegida
- Herança múltipla (multiple inheritance)
- Classes base virtuais (virtual base classes)
- Criação de templates de classe (class templates)
- Uso de templates de classe
- Especializações de templates (template specializations)

Um dos principais objetivos do C++ é facilitar a reutilização de código. A herança pública é um mecanismo para atingir esse objetivo, mas não é o único. Este capítulo investiga outras alternativas. Uma técnica é usar membros de classe que são, eles próprios, objetos de outra classe. Isso é chamado de *composição* (containment), *composição* ou *camadas* (layering). Outra opção é usar herança privada ou protegida. Composição, herança privada e herança protegida são tipicamente usadas para implementar relacionamentos *has-a* — ou seja, relacionamentos em que a nova classe *possui* um objeto de outra classe. Por exemplo, uma classe `CinemaEmCasa` poderia conter um objeto `TocadorBluRay`. A herança múltipla permite criar classes que herdam de duas ou mais classes base, combinando suas funcionalidades.

O Capítulo 10, "Objetos e Classes", introduziu templates de função. Neste capítulo veremos os *templates de classe*, que fornecem outra maneira de reutilizar código. Um template de classe permite definir uma classe em termos genéricos. Em seguida, pode-se usar o template para criar classes específicas definidas para tipos concretos. Por exemplo, você poderia definir um template de pilha (stack) genérico e depois usá-lo para criar uma classe que representa uma pilha de valores `int` e outra que representa uma pilha de valores `double`. É possível até gerar uma classe que representa uma pilha de pilhas.

## Classes com Objetos Membros

Vamos começar com classes que incluem objetos de classe como membros. Algumas classes, como a classe `string` ou as classes template padrão do C++ discutidas no Capítulo 16, "A Classe `string` e a Biblioteca de Templates Padrão", oferecem maneiras convenientes de representar componentes de uma classe mais abrangente. Vejamos um exemplo concreto agora.

O que é um aluno? Alguém matriculado numa escola? Alguém envolvido em investigação criteriosa? Um refugiado das duras exigências do mundo real? Alguém com um nome identificador e um conjunto de notas de provas? Claramente, a última definição é uma caracterização totalmente inadequada de uma pessoa, mas é muito bem adequada para uma representação computacional simples. Vamos, portanto, desenvolver uma classe `Aluno` baseada nessa definição.

Simplificar um aluno a um nome e um conjunto de notas de provas sugere usar uma classe com dois membros: um para representar o nome e outro para representar as notas. Para o nome, você poderia usar um array de `char`, mas isso impõe uma limitação de tamanho. Ou poderia usar um ponteiro `char` e alocação dinâmica de memória. No entanto, como o Capítulo 12, "Classes e Alocação Dinâmica de Memória", ilustra, isso exige muito código de suporte. Melhor ainda, você poderia usar um objeto de uma classe para a qual alguém já fez todo o trabalho. Por exemplo, poderia usar um objeto da classe `String` (veja o Capítulo 12) ou da classe `string` padrão do C++. A escolha mais simples é a classe `string`, pois a biblioteca C++ já fornece todo o código de implementação, além de uma implementação superior.

Representar as notas de provas apresenta escolhas semelhantes. Você poderia usar um array de tamanho fixo, com limitação de tamanho. Poderia usar alocação dinâmica de memória e fornecer um grande volume de código de suporte. Poderia usar seu próprio design de classe, com alocação dinâmica de memória, para representar um array. Ou poderia procurar uma classe na biblioteca padrão do C++ que seja capaz de representar os dados.

Desenvolver sua própria classe não está descartado. Uma versão simples não seria tão difícil, pois um array de `double` compartilha muitas semelhanças com um array de `char`, de modo que você poderia basear o design de uma classe array-de-double no design da classe `String`. Na verdade, edições anteriores deste livro fazem exatamente isso.

Mas, claro, é ainda mais fácil se a biblioteca já fornece uma classe adequada, e ela fornece: a classe `valarray`.

### A Classe `valarray`: Uma Visão Rápida

A classe `valarray` é suportada pelo arquivo de cabeçalho `valarray`. Como o nome sugere, a classe é voltada para lidar com valores numéricos (ou com classes de propriedades semelhantes), portanto ela suporta operações como somar o conteúdo e encontrar os valores maior e menor num array. Para que possa lidar com diferentes tipos de dados, `valarray` é definida como uma classe template. Mais adiante neste capítulo veremos como definir classes template, mas por enquanto tudo que você precisa saber é como usar uma.

O aspecto de template significa que você deve fornecer um tipo específico ao declarar um objeto. Para isso, siga o identificador `valarray` com colchetes angulares contendo o tipo desejado:

```cpp
valarray<int> q_values;    // um array de int
valarray<double> weights;  // um array de double
```

Você já viu esta sintaxe antes no Capítulo 4, "Tipos Compostos", com as classes `vector` e `array`, e é bem simples.

O aspecto de classe significa que, para usar objetos `valarray`, você precisa saber algo sobre os construtores e outros métodos da classe. Aqui estão alguns exemplos usando alguns dos construtores:

```cpp
double media[5] = {3.1, 3.5, 3.8, 2.9, 3.3};
valarray<double> v1;                 // array de double, tamanho 0
valarray<int> v2(8);                 // array de 8 elementos int
valarray<int> v3(10, 8);             // array de 8 elementos int, cada um com valor 10
valarray<double> v4(media, 4);       // array de 4 elementos inicializados com os
                                     // primeiros 4 elementos de media
```

Como você pode ver, é possível criar um array vazio de tamanho zero, um array vazio de tamanho dado, um array com todos os elementos inicializados com o mesmo valor, e um array inicializado com valores de um array comum. Com o C++11, também é possível usar uma lista de inicializadores:

```cpp
valarray<int> v5 = {20, 32, 17, 9}; // C++11
```

A seguir, alguns dos métodos:

- O método `operator[]()` fornece acesso a elementos individuais.
- O método `size()` retorna o número de elementos.
- O método `sum()` retorna a soma dos elementos.
- O método `max()` retorna o maior elemento.
- O método `min()` retorna o menor elemento.

Existem muitos outros métodos, alguns apresentados no Capítulo 16, mas você já viu mais do que suficiente para avançar com este exemplo.

### O Design da Classe `Aluno`

Neste ponto, o plano de design para a classe `Aluno` é usar um objeto `string` para representar o nome e um objeto `valarray<double>` para representar as notas de provas. Como isso deve ser feito? Você pode ser tentado a derivar publicamente uma classe `Aluno` dessas duas classes. Isso seria um exemplo de herança pública múltipla, que o C++ permite, mas seria inadequado aqui. O motivo é que o relacionamento de um aluno com essas classes não se encaixa no modelo *is-a*. Um aluno não é um nome. Um aluno não é um array de notas de provas. O que temos aqui é um relacionamento *has-a*. Um aluno *tem* um nome, e um aluno *tem* um array de notas de provas. A técnica usual do C++ para modelar relacionamentos *has-a* é usar composição (containment) — isto é, criar uma classe composta de, ou contendo, membros que são objetos de outra classe. Por exemplo, você pode começar uma declaração da classe `Aluno` assim:

```cpp
class Aluno
{
private:
    string nome;                   // usa objeto string para o nome
    valarray<double> notas;        // usa objeto valarray<double> para as notas
    ...
};
```

Como de costume, a classe torna os membros de dados privados. Isso implica que as funções membro da classe `Aluno` podem usar as interfaces públicas das classes `string` e `valarray<double>` para acessar e modificar os objetos `nome` e `notas`, mas o mundo externo não pode fazê-lo. O único acesso que o mundo externo terá a `nome` e `notas` é através da interface pública definida para a classe `Aluno` (veja a Figura 14.1). Uma forma comum de descrever isso é dizer que a classe `Aluno` adquire a implementação de seus objetos membros mas não herda a interface. Por exemplo, um objeto `Aluno` usa a implementação `string` em vez de uma implementação `char *` ou `char nome[26]` para armazenar o nome. Mas um objeto `Aluno` não tem, por natureza, a capacidade de usar a função `string::operator+=()` para concatenação.

> **Interfaces e Implementações**
>
> Com a herança pública, uma classe herda uma interface e, talvez, uma implementação. (Funções virtuais puras numa classe base podem fornecer uma interface sem implementação.) Adquirir a interface faz parte do relacionamento *is-a*. Com composição, por outro lado, uma classe adquire a implementação sem a interface. Não herdar a interface faz parte do relacionamento *has-a*.

O fato de um objeto de classe não adquirir automaticamente a interface de um objeto contido é positivo para um relacionamento *has-a*. Por exemplo, `string` sobrecarrega o operador `+` para permitir concatenar dois strings, mas, conceitualmente, não faz sentido concatenar dois objetos `Aluno`. Essa é uma das razões para não usar herança pública neste caso. Por outro lado, partes da interface da classe contida podem fazer sentido para a nova classe. Por exemplo, você pode querer usar o método `operator<()` da interface `string` para ordenar objetos `Aluno` por nome. Você pode fazer isso definindo uma função membro `Aluno::operator<()` que internamente usa a função `string::operator<()`. Vamos prosseguir para alguns detalhes.

### O Exemplo da Classe `Aluno`

Neste ponto, você precisa fornecer a declaração da classe `Aluno`. Ela deve, claro, incluir construtores e pelo menos algumas funções para fornecer uma interface para a classe `Aluno`. A Listagem 14.1 faz isso, definindo todos os construtores inline. Ela também fornece algumas funções amigas para entrada e saída.

**Listagem 14.1 `alunoc.h`**

```cpp
// alunoc.h -- definindo uma classe Aluno usando composição (containment)
#ifndef ALUNOC_H_
#define ALUNOC_H_
#include <iostream>
#include <string>
#include <valarray>
class Aluno
{
private:
    typedef std::valarray<double> ArrayBD;
    std::string nome;              // objeto contido
    ArrayBD notas;                 // objeto contido
    // método privado para saída das notas
    std::ostream & saida_arr(std::ostream & os) const;
public:
    Aluno() : nome("Aluno Nulo"), notas() {}
    explicit Aluno(const std::string & s)
        : nome(s), notas() {}
    explicit Aluno(int n) : nome("Nulo"), notas(n) {}
    Aluno(const std::string & s, int n)
        : nome(s), notas(n) {}
    Aluno(const std::string & s, const ArrayBD & a)
        : nome(s), notas(a) {}
    Aluno(const char * str, const double * pd, int n)
        : nome(str), notas(pd, n) {}
    ~Aluno() {}
    double Media() const;
    const std::string & Nome() const;
    double & operator[](int i);
    double operator[](int i) const;
// funções amigas
    // entrada
    friend std::istream & operator>>(std::istream & is,
                            Aluno & alu);           // 1 palavra
    friend std::istream & getline(std::istream & is,
                          Aluno & alu);             // 1 linha
    // saída
    friend std::ostream & operator<<(std::ostream & os,
                            const Aluno & alu);
};
#endif
```

Para simplificar a notação, a classe `Aluno` contém este `typedef`:

```cpp
typedef std::valarray<double> ArrayBD;
```

Isso permite que o restante do código use a notação mais conveniente `ArrayBD` em vez de `std::valarray<double>`. Assim, métodos e funções amigas podem se referir ao tipo `ArrayBD`. Colocar esse `typedef` na parte privada da definição da classe significa que ele pode ser usado internamente na implementação de `Aluno`, mas não por usuários externos da classe `Aluno`.

Observe o uso da palavra-chave `explicit`:

```cpp
explicit Aluno(const std::string & s)
    : nome(s), notas() {}
explicit Aluno(int n) : nome("Nulo"), notas(n) {}
```

Lembre-se de que um construtor que pode ser chamado com um argumento serve como função de conversão implícita do tipo do argumento para o tipo da classe. Muitas vezes isso não é uma boa ideia. No segundo construtor, por exemplo, o primeiro argumento representa o número de elementos num array, e não um valor para o array; portanto, ter o construtor servindo como função de conversão de `int` para `Aluno` não faz sentido. Usar `explicit` desativa conversões implícitas. Se essa palavra-chave fosse omitida, código como o seguinte seria possível:

```cpp
Aluno doh("Homer", 10); // armazena "Homer", cria array de 10 elementos
doh = 5;                // redefine nome para "Nulo", redefine para array vazio de 5 elementos
```

Aqui, o programador descuidado digitou `doh` em vez de `doh[0]`. Se o construtor omitisse `explicit`, o `5` seria convertido para um objeto `Aluno` temporário usando a chamada de construtor `Aluno(5)`, com o valor `"Nulo"` sendo usado para definir o membro `nome`. Então a atribuição substituiria o `doh` original pelo objeto temporário. Com `explicit`, o compilador detectará o operador de atribuição como erro.

> **C++ e Restrições**
>
> O C++ está repleto de recursos que permitem aos programadores restringir construções a certos limites — `explicit` para remover a conversão implícita de construtores de argumento único, `const` para restringir o uso de métodos na modificação de dados, e muito mais. A motivação subjacente é simplesmente esta: erros em tempo de compilação são melhores que erros em tempo de execução.

#### Inicializando Objetos Contidos

Observe que todos os construtores usam a sintaxe familiar de lista de inicializadores de membros para inicializar os objetos membros `nome` e `notas`. Em alguns casos anteriores neste livro, os construtores usam isso para inicializar membros de tipos internos:

```cpp
Fila::Fila(int qs) : tamFila(qs) {...}  // inicializa tamFila com qs
```

Esse código usa o nome do membro de dados (`tamFila`) numa lista de inicializadores de membros. Construtores de exemplos anteriores também usam uma lista de inicializadores de membros para inicializar a parte da classe base de um objeto derivado:

```cpp
comAD::comAD(const comAD & hs) : baseAD(hs) {...}
```

Para *objetos herdados*, construtores usam o nome da classe na lista de inicializadores de membros para invocar um construtor específico da classe base. Para *objetos membros*, construtores usam o nome do membro. Por exemplo, observe o último construtor na Listagem 14.1:

```cpp
Aluno(const char * str, const double * pd, int n)
    : nome(str), notas(pd, n) {}
```

Como ele inicializa objetos membros, não objetos herdados, esse construtor usa os nomes dos membros, não os nomes das classes, na lista de inicialização. Cada item nessa lista invoca o construtor correspondente. Ou seja, `nome(str)` invoca o construtor `string(const char *)`, e `notas(pd, n)` invoca o construtor `ArrayBD(const double *, int)`, que, por causa do `typedef`, é realmente o construtor `valarray<double>(const double *, int)`.

O que acontece se você não usar a sintaxe de lista de inicializadores? Como nos componentes herdados, o C++ requer que todos os objetos membros sejam construídos antes que o restante de um objeto seja construído. Portanto, se você omitir a lista de inicialização, o C++ usará os construtores padrão definidos para as classes dos objetos membros.

> **Ordem de Inicialização**
>
> Quando você tem uma lista de inicializadores de membros que inicializa mais de um item, os itens são inicializados na ordem em que foram declarados, não na ordem em que aparecem na lista de inicializadores. Por exemplo, suponha que você escreva um construtor de `Aluno` desta forma:
>
> ```cpp
> Aluno(const char * str, const double * pd, int n)
>     : notas(pd, n), nome(str) {}
> ```
>
> O membro `nome` ainda seria inicializado primeiro porque é declarado primeiro na definição da classe. A ordem exata de inicialização não é importante para este exemplo, mas seria importante se o código usasse o valor de um membro como parte da expressão de inicialização de um segundo membro.

#### Usando uma Interface para um Objeto Contido

A interface de um objeto contido não é pública, mas pode ser usada dentro dos métodos da classe. Por exemplo, aqui está como você pode definir uma função que retorna a média das notas de um aluno:

```cpp
double Aluno::Media() const
{
    if (notas.size() > 0)
        return notas.sum() / notas.size();
    else
        return 0;
}
```

Isso define um método que pode ser invocado por um objeto `Aluno`. Internamente, ele usa os métodos `size()` e `sum()` de `valarray`. É porque `notas` é um objeto `valarray`, portanto ele pode invocar as funções membro da classe `valarray`. Resumindo, o objeto `Aluno` invoca um método de `Aluno`, e o método de `Aluno` usa o objeto `valarray` contido para invocar métodos de `valarray`.

Da mesma forma, você pode definir uma função amiga que usa a versão `string` do operador `<<`:

```cpp
// usa versão string de operator<<()
ostream & operator<<(ostream & os, const Aluno & alu)
{
    os << "Notas para " << alu.nome << ":\n";
    ...
}
```

Como `alu.nome` é um objeto `string`, ele invoca a função `operator<<(ostream &, const string &)`, que faz parte do pacote da classe `string`. Note que a função `operator<<(ostream & os, const Aluno & alu)` precisa ser amiga da classe `Aluno` para que possa acessar o membro `nome`. (Alternativamente, a função poderia usar o método público `Nome()` em vez do membro de dado privado `nome`.)

Da mesma forma, a função poderia usar a implementação `valarray` de `<<` para saída; infelizmente, não existe tal implementação. Portanto, a classe define um método auxiliar privado para lidar com essa tarefa:

```cpp
// método privado
ostream & Aluno::saida_arr(ostream & os) const
{
    int i;
    int lim = notas.size();
    if (lim > 0)
    {
        for (i = 0; i < lim; i++)
        {
            os << notas[i] << " ";
            if (i % 5 == 4)
                os << endl;
        }
        if (i % 5 != 0)
            os << endl;
    }
    else
        os << " array vazio ";
    return os;
}
```

Usar um auxiliar como esse agrupa os detalhes complicados num único lugar e torna a codificação da função amiga mais organizada:

```cpp
// usa versão string de operator<<()
ostream & operator<<(ostream & os, const Aluno & alu)
{
    os << "Notas para " << alu.nome << ":\n";
    alu.saida_arr(os);  // usa método privado para as notas
    return os;
}
```

A função auxiliar também pode servir como bloco de construção para outras funções de saída de nível de usuário, caso você opte por fornecê-las.

A Listagem 14.2 mostra o arquivo de métodos da classe `Aluno`. Ela inclui métodos que permitem usar o operador `[]` para acessar notas individuais num objeto `Aluno`.

**Listagem 14.2 `alunoc.cpp`**

```cpp
// alunoc.cpp -- classe Aluno usando composição (containment)
#include "alunoc.h"
using std::ostream;
using std::endl;
using std::istream;
using std::string;
// métodos públicos
double Aluno::Media() const
{
    if (notas.size() > 0)
        return notas.sum() / notas.size();
    else
        return 0;
}
const string & Aluno::Nome() const
{
    return nome;
}
double & Aluno::operator[](int i)
{
    return notas[i];   // usa valarray<double>::operator[]()
}
double Aluno::operator[](int i) const
{
    return notas[i];
}
// método privado
ostream & Aluno::saida_arr(ostream & os) const
{
    int i;
    int lim = notas.size();
    if (lim > 0)
    {
        for (i = 0; i < lim; i++)
        {
            os << notas[i] << " ";
            if (i % 5 == 4)
                os << endl;
        }
        if (i % 5 != 0)
            os << endl;
    }
    else
        os << " array vazio ";
    return os;
}
// funções amigas
// usa versão string de operator>>()
istream & operator>>(istream & is, Aluno & alu)
{
    is >> alu.nome;
    return is;
}
// usa getline amigo de string (ostream &, const string &)
istream & getline(istream & is, Aluno & alu)
{
    getline(is, alu.nome);
    return is;
}
// usa versão string de operator<<()
ostream & operator<<(ostream & os, const Aluno & alu)
{
    os << "Notas para " << alu.nome << ":\n";
    alu.saida_arr(os);  // usa método privado para as notas
    return os;
}
```

Além do método auxiliar privado, a Listagem 14.2 não requer muito código novo. O uso de composição permite aproveitar o código que você ou outra pessoa já escreveu.

#### Usando a Nova Classe `Aluno`

Vamos montar um pequeno programa para testar a nova classe `Aluno`. Para manter as coisas simples, ele deve usar um array de apenas três objetos `Aluno`, cada um armazenando cinco notas de provas. E deve usar um ciclo de entrada sem sofisticação que não verifica a entrada e não permite interromper o processo de entrada antes do final. A Listagem 14.3 apresenta o programa de teste. Certifique-se de compilá-lo junto com `alunoc.cpp`.

**Listagem 14.3 `usar_alunoc.cpp`**

```cpp
// usar_alunoc.cpp -- usando uma classe composta
// compilar com alunoc.cpp
#include <iostream>
#include "alunoc.h"
using std::cin;
using std::cout;
using std::endl;
void definir(Aluno & sa, int n);

const int alunos = 3;
const int provas = 5;
int main()
{
    Aluno ada[alunos] =
        {Aluno(provas), Aluno(provas), Aluno(provas)};
    int i;
    for (i = 0; i < alunos; ++i)
        definir(ada[i], provas);
    cout << "\nLista de Alunos:\n";
    for (i = 0; i < alunos; ++i)
        cout << ada[i].Nome() << endl;
    cout << "\nResultados:";
    for (i = 0; i < alunos; ++i)
    {
        cout << endl << ada[i];
        cout << "media: " << ada[i].Media() << endl;
    }
    cout << "Concluido.\n";
    return 0;
}
void definir(Aluno & sa, int n)
{
    cout << "Por favor, insira o nome do aluno: ";
    getline(cin, sa);
    cout << "Por favor, insira " << n << " notas de provas:\n";
    for (int i = 0; i < n; i++)
        cin >> sa[i];
    while (cin.get() != '\n')
        continue;
}
```

Aqui está um exemplo de execução do programa das Listagens 14.1, 14.2 e 14.3:

```
Por favor, insira o nome do aluno: Gil Bayts
Por favor, insira 5 notas de provas:
92 94 96 93 95
Por favor, insira o nome do aluno: Pat Roone
Por favor, insira 5 notas de provas:
83 89 72 78 95
Por favor, insira o nome do aluno: Fleur O'Day
Por favor, insira 5 notas de provas:
92 89 96 74 64
Lista de Alunos:
Gil Bayts
Pat Roone
Fleur O'Day
Resultados:
Notas para Gil Bayts:
92 94 96 93 95
media: 94
Notas para Pat Roone:
83 89 72 78 95
media: 83.4
Notas para Fleur O'Day:
92 89 96 74 64
media: 83
Concluido.
```

## Herança Privada

O C++ tem um segundo meio de implementar o relacionamento *has-a*: a *herança privada* (private inheritance). Com a herança privada, membros públicos e protegidos da classe base tornam-se membros privados da classe derivada. Isso significa que os métodos da classe base não fazem parte da interface pública do objeto derivado. No entanto, eles podem ser usados dentro das funções membro da classe derivada.

Vamos analisar mais de perto o tema da interface. Com a herança pública, os métodos públicos da classe base tornam-se métodos públicos da classe derivada. Resumindo, a classe derivada herda a interface da classe base. Isso faz parte do relacionamento *is-a*. Com a herança privada, os métodos públicos da classe base tornam-se métodos privados da classe derivada. Resumindo, a classe derivada não herda a interface da classe base. Como você viu com os objetos contidos, essa ausência de herança faz parte do relacionamento *has-a*.

Com a herança privada, uma classe herda a implementação. Por exemplo, se você basear uma classe `Aluno` numa classe `string`, a classe `Aluno` terá um componente herdado de `string` que pode ser usado para armazenar uma string. Além disso, os métodos de `Aluno` podem usar os métodos de `string` internamente para acessar o componente string.

A composição adiciona um objeto a uma classe como um objeto membro nomeado, enquanto a herança privada adiciona um objeto a uma classe como um objeto herdado sem nome. Este livro usa o termo *subobjeto* para denotar um objeto adicionado por herança ou por composição.

A herança privada, portanto, fornece os mesmos recursos que a composição: adquirir a implementação, não adquirir a interface. Portanto, ela também pode ser usada para implementar um relacionamento *has-a*. Na verdade, você pode produzir uma classe `Aluno` que usa herança privada e tem a mesma interface pública que a versão de composição. Assim, as diferenças entre as duas abordagens afetam a implementação, não a interface. Vejamos como você pode usar a herança privada para redesenhar a classe `Aluno`.

### Uma Nova Versão do Exemplo da Classe `Aluno`

Para obter herança privada, use a palavra-chave `private` em vez de `public` ao definir a classe. (Na verdade, `private` é o padrão, então omitir um qualificador de acesso também leva à herança privada.) A classe `Aluno` deve herdar de duas classes, então a declaração deve listar as duas:

```cpp
class Aluno : private std::string, private std::valarray<double>
{
public:
    ...
};
```

Ter mais de uma classe base é chamado de *herança múltipla* (MI). Em geral, a herança múltipla, especialmente a pública, pode levar a problemas que precisam ser resolvidos com regras e sintaxes adicionais. Falaremos sobre esses assuntos mais adiante neste capítulo. Mas neste caso particular, a herança múltipla não causa problemas.

Observe que a nova classe não precisa de membros de dados privados. Isso porque as duas classes base herdadas já fornecem todos os membros de dados necessários. A versão de composição deste exemplo fornece dois objetos explicitamente nomeados como membros. A herança privada, no entanto, fornece dois subobjetos sem nome como membros herdados. Essa é a primeira das principais diferenças entre as duas abordagens.

#### Inicializando Componentes da Classe Base

Ter componentes herdados implicitamente em vez de objetos membros afeta a codificação deste exemplo, pois você não pode mais usar `nome` e `notas` para descrever os objetos. Em vez disso, você deve voltar às técnicas usadas para herança pública. Por exemplo, considere os construtores. A composição usa este construtor:

```cpp
Aluno(const char * str, const double * pd, int n)
    : nome(str), notas(pd, n) {}  // usa nomes de objetos para composição
```

A nova versão deve usar a sintaxe de lista de inicializadores de membros para classes herdadas, que usa o *nome da classe* em vez do nome do membro para identificar um construtor:

```cpp
Aluno(const char * str, const double * pd, int n)
    : std::string(str), ArrayBD(pd, n) {}  // usa nomes de classes para herança
```

Aqui, como no exemplo anterior, `ArrayBD` é um `typedef` para `std::valarray<double>`. Certifique-se de notar que a lista de inicializadores de membros usa termos como `std::string(str)` em vez de `nome(str)`. Essa é a segunda principal diferença entre as duas abordagens.

A Listagem 14.4 mostra a nova declaração da classe. As únicas mudanças são a omissão de nomes de objetos explícitos e o uso de nomes de classes em vez de nomes de membros nos construtores inline.

**Listagem 14.4 `alunoi.h`**

```cpp
// alunoi.h -- definindo uma classe Aluno usando herança privada
#ifndef ALUNOI_H_
#define ALUNOI_H_
#include <iostream>
#include <valarray>
#include <string>
class Aluno : private std::string, private std::valarray<double>
{
private:
    typedef std::valarray<double> ArrayBD;
    // método privado para saída das notas
    std::ostream & saida_arr(std::ostream & os) const;
public:
    Aluno() : std::string("Aluno Nulo"), ArrayBD() {}
    explicit Aluno(const std::string & s)
        : std::string(s), ArrayBD() {}
    explicit Aluno(int n) : std::string("Nulo"), ArrayBD(n) {}
    Aluno(const std::string & s, int n)
        : std::string(s), ArrayBD(n) {}
    Aluno(const std::string & s, const ArrayBD & a)
        : std::string(s), ArrayBD(a) {}
    Aluno(const char * str, const double * pd, int n)
        : std::string(str), ArrayBD(pd, n) {}
    ~Aluno() {}
    double Media() const;
    double & operator[](int i);
    double operator[](int i) const;
    const std::string & Nome() const;
// funções amigas
    // entrada
    friend std::istream & operator>>(std::istream & is,
                            Aluno & alu);           // 1 palavra
    friend std::istream & getline(std::istream & is,
                          Aluno & alu);             // 1 linha
    // saída
    friend std::ostream & operator<<(std::ostream & os,
                            const Aluno & alu);
};
#endif
```

#### Acessando Métodos da Classe Base

A herança privada limita o uso dos métodos da classe base aos métodos da classe derivada. Às vezes, porém, você pode querer tornar uma funcionalidade da classe base publicamente disponível. Por exemplo, a declaração da classe `Aluno` sugere a capacidade de usar uma função `Media()`. Assim como com a composição, a técnica para fazer isso é usar os métodos `size()` e `sum()` de `valarray` dentro de uma função pública `Aluno::Media()`. A composição invocava os métodos com um objeto:

```cpp
double Aluno::Media() const
{
    if (notas.size() > 0)
        return notas.sum() / notas.size();
    else
        return 0;
}
```

Aqui, porém, a herança permite usar o nome da classe e o operador de resolução de escopo para invocar métodos da classe base:

```cpp
double Aluno::Media() const
{
    if (ArrayBD::size() > 0)
        return ArrayBD::sum() / ArrayBD::size();
    else
        return 0;
}
```

Em resumo, a abordagem de composição usa nomes de objetos para invocar um método, enquanto a herança privada usa o nome da classe e o operador de resolução de escopo.

#### Acessando o Objeto da Classe Base

O operador de resolução de escopo permite acessar um método da classe base, mas e se você precisar do objeto da classe base em si? Por exemplo, a versão de composição da classe `Aluno` implementa o método `Nome()` fazendo o método retornar o objeto string membro `nome`. Mas com a herança privada, o objeto `string` não tem nome. Como, então, o código de `Aluno` pode acessar o objeto `string` interno?

A resposta é usar uma conversão de tipo (type cast). Como `Aluno` é derivado de `string`, é possível converter um objeto `Aluno` para um objeto `string`; o resultado é o objeto `string` herdado. Lembre-se de que o ponteiro `this` aponta para o objeto invocador, então `*this` é o objeto invocador — neste caso, um objeto do tipo `Aluno`. Para evitar invocar construtores ao criar novos objetos, você usa a conversão de tipo para criar uma referência:

```cpp
const string & Aluno::Nome() const
{
    return (const string &) *this;
}
```

Esse código retorna uma referência ao objeto `string` herdado que reside no objeto `Aluno` invocador.

#### Acessando Funções Amigas da Classe Base

A técnica de qualificar explicitamente um nome de função com seu nome de classe não funciona para funções amigas, pois uma função amiga não pertence a uma classe. No entanto, você pode usar uma conversão de tipo explícita para a classe base para invocar as funções corretas. Essa é basicamente a mesma técnica usada para acessar um objeto da classe base num método de classe, mas com funções amigas você tem um nome para o objeto `Aluno`, então o código usa o nome em vez de `*this`. Por exemplo, considere a seguinte definição de função amiga:

```cpp
ostream & operator<<(ostream & os, const Aluno & alu)
{
    os << "Notas para " << (const string &) alu << ":\n";
    ...
}
```

Se `plato` é um objeto `Aluno`, então a instrução a seguir invoca essa função, com `alu` sendo uma referência a `plato` e `os` sendo uma referência a `cout`:

```cpp
cout << plato;
```

A conversão de tipo converte explicitamente `alu` para uma referência a um objeto do tipo `string`; esse tipo, por sua vez, invoca a função `operator<<(ostream &, const string &)`.

A referência `alu` não é convertida automaticamente para uma referência de `string`. O motivo fundamental é que, com herança privada, uma referência ou ponteiro para a classe base não pode receber uma referência ou ponteiro para a classe derivada sem uma conversão de tipo explícita.

No entanto, mesmo que o exemplo usasse herança pública, seria necessário usar conversões de tipo explícitas. Um motivo é que, sem a conversão de tipo, código como o seguinte corresponderia ao protótipo da função amiga, levando a uma chamada recursiva:

```cpp
os << alu;
```

Um segundo motivo é que, como a classe usa herança múltipla, o compilador não consegue determinar para qual classe base converter caso ambas as classes base forneçam uma função `operator<<()`.

A Listagem 14.5 mostra todos os métodos da classe `Aluno`, exceto os definidos inline na declaração da classe.

**Listagem 14.5 `alunoi.cpp`**

```cpp
// alunoi.cpp -- classe Aluno usando herança privada
#include "alunoi.h"
using std::ostream;
using std::endl;
using std::istream;
using std::string;
// métodos públicos
double Aluno::Media() const
{
    if (ArrayBD::size() > 0)
        return ArrayBD::sum() / ArrayBD::size();
    else
        return 0;
}
const string & Aluno::Nome() const
{
    return (const string &) *this;
}
double & Aluno::operator[](int i)
{
    return ArrayBD::operator[](i);  // usa ArrayBD::operator[]()
}
double Aluno::operator[](int i) const
{
    return ArrayBD::operator[](i);
}
// método privado
ostream & Aluno::saida_arr(ostream & os) const
{
    int i;
    int lim = ArrayBD::size();
    if (lim > 0)
    {
        for (i = 0; i < lim; i++)
        {
            os << ArrayBD::operator[](i) << " ";
            if (i % 5 == 4)
                os << endl;
        }
        if (i % 5 != 0)
            os << endl;
    }
    else
        os << " array vazio ";
    return os;
}
// funções amigas
// usa versão string de operator>>()
istream & operator>>(istream & is, Aluno & alu)
{
    is >> (string &)alu;
    return is;
}
// usa getline amigo de string (ostream &, const string &)
istream & getline(istream & is, Aluno & alu)
{
    getline(is, (string &)alu);
    return is;
}
// usa versão string de operator<<()
ostream & operator<<(ostream & os, const Aluno & alu)
{
    os << "Notas para " << (const string &) alu << ":\n";
    alu.saida_arr(os);  // usa método privado para as notas
    return os;
}
```

Mais uma vez, como o exemplo reutiliza o código de `string` e `valarray`, relativamente pouco código novo é necessário, além do método auxiliar privado.

#### Usando a Classe `Aluno` Revisada

Mais uma vez, é hora de testar uma nova classe. Observe que as duas versões da classe `Aluno` têm exatamente a mesma interface pública, então você pode testar as duas versões com exatamente o mesmo programa. A única diferença é que você precisa incluir `alunoi.h` em vez de `alunoc.h`, e precisa vincular o programa com `alunoi.cpp` em vez de com `alunoc.cpp`. A Listagem 14.6 mostra o programa. Certifique-se de compilá-lo junto com `alunoi.cpp`.

**Listagem 14.6 `usar_alunoi.cpp`**

```cpp
// usar_alunoi.cpp -- usando uma classe com herança privada
// compilar com alunoi.cpp
#include <iostream>
#include "alunoi.h"
using std::cin;
using std::cout;
using std::endl;
void definir(Aluno & sa, int n);

const int alunos = 3;
const int provas = 5;
int main()
{
    Aluno ada[alunos] =
        {Aluno(provas), Aluno(provas), Aluno(provas)};
    int i;
    for (i = 0; i < alunos; i++)
        definir(ada[i], provas);
    cout << "\nLista de Alunos:\n";
    for (i = 0; i < alunos; ++i)
        cout << ada[i].Nome() << endl;
    cout << "\nResultados:";
    for (i = 0; i < alunos; i++)
    {
        cout << endl << ada[i];
        cout << "media: " << ada[i].Media() << endl;
    }
    cout << "Concluido.\n";
    return 0;
}
void definir(Aluno & sa, int n)
{
    cout << "Por favor, insira o nome do aluno: ";
    getline(cin, sa);
    cout << "Por favor, insira " << n << " notas de provas:\n";
    for (int i = 0; i < n; i++)
        cin >> sa[i];
    while (cin.get() != '\n')
        continue;
}
```

Aqui está um exemplo de execução do programa da Listagem 14.6:

```
Por favor, insira o nome do aluno: Gil Bayts
Por favor, insira 5 notas de provas:
92 94 96 93 95
Por favor, insira o nome do aluno: Pat Roone
Por favor, insira 5 notas de provas:
83 89 72 78 95
Por favor, insira o nome do aluno: Fleur O'Day
Por favor, insira 5 notas de provas:
92 89 96 74 64
Lista de Alunos:
Gil Bayts
Pat Roone
Fleur O'Day
Resultados:
Notas para Gil Bayts:
92 94 96 93 95
media: 94
Notas para Pat Roone:
83 89 72 78 95
media: 83.4
Notas para Fleur O'Day:
92 89 96 74 64
media: 83
Concluido.
```

A mesma entrada de antes leva à mesma saída que a versão de composição produz.

## Composição ou Herança Privada?

Dado que você pode modelar um relacionamento *has-a* tanto com composição quanto com herança privada, qual deve usar? A maioria dos programadores C++ prefere composição. Primeiro, é mais fácil de acompanhar. Quando você olha a declaração da classe, vê explicitamente objetos nomeados que representam as classes contidas, e seu código pode se referir a esses objetos pelo nome. Usar herança faz o relacionamento parecer mais abstrato. Segundo, herança pode levantar problemas, particularmente se uma classe herda de mais de uma classe base. Você pode ter que lidar com questões como classes base separadas tendo métodos com o mesmo nome ou classes base separadas compartilhando um ancestral comum. No geral, você tem menos chance de ter problemas usando composição. Além disso, composição permite incluir mais de um subobjeto da mesma classe. Se uma classe precisa de três objetos `string`, você pode declarar três membros `string` separados usando a abordagem de composição. Mas a herança limita você a um único objeto. (É difícil distinguir objetos quando todos são sem nome.)

No entanto, a herança privada oferece recursos além dos fornecidos pela composição. Suponha, por exemplo, que uma classe tem membros protegidos, que poderiam ser membros de dados ou funções membro. Tais membros estão disponíveis para classes derivadas mas não para o mundo em geral. Se você incluir tal classe em outra classe usando composição, a nova classe faz parte do mundo em geral, não é uma classe derivada. Portanto, ela não pode acessar membros protegidos. Mas usar herança torna a nova classe uma classe derivada, portanto ela pode acessar membros protegidos.

Outra situação que pede o uso de herança privada é se você quer redefinir funções virtuais. Novamente, esse é um privilégio concedido a uma classe derivada, mas não a uma classe contida. Com herança privada, as funções redefinidas seriam utilizáveis apenas dentro da classe, não publicamente.

> **Dica**
>
> Em geral, use composição para modelar um relacionamento *has-a*. Use herança privada se a nova classe precisar acessar membros protegidos da classe original ou se precisar redefinir funções virtuais.

## Herança Protegida

A *herança protegida* (protected inheritance) é uma variação da herança privada. Ela usa a palavra-chave `protected` ao listar uma classe base:

```cpp
class Aluno : protected std::string,
              protected std::valarray<double>
{...};
```

Com herança protegida, membros públicos e protegidos de uma classe base tornam-se membros protegidos da classe derivada. Assim como com a herança privada, a interface da classe base está disponível para a classe derivada mas não para o mundo externo. A principal diferença entre herança privada e herança protegida ocorre quando você deriva outra classe da classe derivada. Com herança privada, essa terceira geração de classes não obtém o uso interno da interface da classe base. Isso porque os métodos públicos da classe base tornam-se privados na classe derivada, e membros e métodos privados não podem ser acessados diretamente pelo próximo nível de derivação. Com herança protegida, os métodos públicos da classe base tornam-se protegidos na segunda geração e, portanto, estão disponíveis internamente para o próximo nível de derivação.

A Tabela 14.1 resume a herança pública, privada e protegida. O termo *upcasting implícito* significa que você pode ter um ponteiro ou referência de classe base referindo-se a um objeto de classe derivada sem usar uma conversão de tipo explícita.

**Tabela 14.1 Variedades de Herança**

| Propriedade | Herança Pública | Herança Protegida | Herança Privada |
|---|---|---|---|
| Membros públicos tornam-se | Membros públicos da classe derivada | Membros protegidos da classe derivada | Membros privados da classe derivada |
| Membros protegidos tornam-se | Membros protegidos da classe derivada | Membros protegidos da classe derivada | Membros privados da classe derivada |
| Membros privados tornam-se | Acessíveis apenas através da interface da classe base | Acessíveis apenas através da interface da classe base | Acessíveis apenas através da interface da classe base |
| Upcasting implícito | Sim | Sim (mas apenas dentro da classe derivada) | Não |

## Redefinindo Acesso com `using`

Membros públicos de uma classe base tornam-se protegidos ou privados quando você usa derivação protegida ou privada. Suponha que você queira tornar um método particular da classe base disponível publicamente na classe derivada. Uma opção é definir um método da classe derivada que usa o método da classe base. Por exemplo, suponha que você queira que a classe `Aluno` seja capaz de usar o método `sum()` de `valarray`. Você pode declarar um método `soma()` na declaração da classe e depois defini-lo assim:

```cpp
double Aluno::soma() const  // método público de Aluno
{
    return std::valarray<double>::sum();  // usa método herdado privadamente
}
```

Então um objeto `Aluno` pode invocar `Aluno::soma()`, que, por sua vez, aplica o método `valarray<double>::sum()` ao objeto `valarray` embutido.

Existe uma alternativa para encapsular uma chamada de função em outra: usar uma *declaração `using`* (como as usadas com namespaces) para anunciar que um determinado membro da classe base pode ser usado pela classe derivada, mesmo que a derivação seja privada. Por exemplo, suponha que você queira ser capaz de usar os métodos `min()` e `max()` de `valarray` com a classe `Aluno`. Neste caso, em `alunoi.h`, você pode adicionar declarações `using` à seção pública:

```cpp
class Aluno : private std::string, private std::valarray<double>
{
...
public:
    using std::valarray<double>::min;
    using std::valarray<double>::max;
    ...
};
```

A declaração `using` torna os métodos `valarray<double>::min()` e `valarray<double>::max()` disponíveis como se fossem métodos públicos de `Aluno`:

```cpp
cout << "nota mais alta: " << ada[i].max() << endl;
```

Observe que a declaração `using` usa apenas o nome do membro — sem parênteses, sem assinatura de função, sem tipo de retorno. Por exemplo, para tornar o método `operator[]()` de `valarray` disponível para a classe `Aluno`, você colocaria a seguinte declaração `using` na seção pública da declaração da classe `Aluno`:

```cpp
using std::valarray<double>::operator[];
```

Isso tornaria ambas as versões (`const` e não-`const`) disponíveis. Você poderia então remover os protótipos e definições existentes de `Aluno::operator[]()`. A abordagem da declaração `using` funciona apenas para herança, não para composição.

Há uma forma mais antiga de redeclarar métodos da classe base numa classe derivada privadamente: colocar o nome do método na seção pública da classe derivada:

```cpp
class Aluno : private std::string, private std::valarray<double>
{
public:
    std::valarray<double>::operator[];  // redeclara como público, usa apenas o nome
    ...
};
```

Isso parece uma declaração `using` sem a palavra-chave `using`. Essa abordagem está *descontinuada* (deprecated), ou seja, a intenção é eliminá-la gradualmente. Portanto, se seu compilador suporta a declaração `using`, use-a para tornar um método de uma classe base privada disponível para a classe derivada.

---

[Anterior](capitulo-13-03-revisao-resumo.md) | [Índice](README.md) | [Próximo](capitulo-14-02-heranca-multipla.md)
