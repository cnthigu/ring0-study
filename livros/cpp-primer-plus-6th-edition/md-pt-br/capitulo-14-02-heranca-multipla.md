# Capítulo 14 — Herança Múltipla (Multiple Inheritance) e Classes Base Virtuais

> Tradução não oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Navegação: [Anterior](capitulo-14-01-composicao-heranca-privada.md) | [Índice](README.md) | [Próximo](capitulo-14-03-templates-classe.md)

## Herança Múltipla

A herança múltipla (MI — Multiple Inheritance) descreve uma classe que tem mais de uma classe base imediata. Assim como na herança simples, a herança múltipla pública deve expressar um relacionamento *is-a*. Por exemplo, se você tem uma classe `Garcom` e uma classe `Cantor`, poderia derivar uma classe `GarcomCantor` das duas:

```cpp
class GarcomCantor : public Garcom, public Cantor {...};
```

Observe que você deve qualificar cada classe base com a palavra-chave `public`. Isso porque o compilador assume derivação privada a menos que seja instruído de outra forma:

```cpp
class GarcomCantor : public Garcom, Cantor {...};  // Cantor é base privada
```

Como discutido anteriormente neste capítulo, a herança múltipla privada e protegida pode expressar um relacionamento *has-a*; a implementação da classe `Aluno` em `alunoi.h` é um exemplo. Vamos nos concentrar na herança pública agora.

A herança múltipla pode introduzir novos problemas para os programadores. Os dois principais problemas são herdar diferentes métodos com o mesmo nome de duas classes base diferentes, e herdar múltiplas instâncias de uma classe via duas ou mais classes base imediatas relacionadas. Resolver esses problemas envolve introduzir algumas novas regras e variações de sintaxe. Assim, usar herança múltipla pode ser mais difícil e propenso a problemas do que usar herança simples. Por essa razão, muitos na comunidade C++ se opõem fortemente à herança múltipla; alguns querem removê-la da linguagem. Outros adoram a herança múltipla e argumentam que ela é muito útil, até necessária, para determinados projetos. Outros ainda sugerem usar herança múltipla com cautela e moderação.

Vamos explorar um exemplo particular e ver quais são os problemas e as soluções. Você precisa de várias classes para criar uma situação de herança múltipla. Para este exemplo, definiremos uma classe base abstrata `Trabalhador` e derivaremos uma classe `Garcom` e uma classe `Cantor` dela. Então podemos usar herança múltipla para derivar uma classe `GarcomCantor` das classes `Garcom` e `Cantor` (veja a Figura 14.3). Esse é um caso em que uma classe base (`Trabalhador`) é herdada por duas derivações separadas — a circunstância que causa as maiores dificuldades com herança múltipla. Começamos com declarações para as classes `Trabalhador`, `Garcom` e `Cantor`, conforme mostrado na Listagem 14.7.

```
           Trabalhador

    Cantor          Garcom

          GarcomCantor

Figura 14.3 Herança múltipla com um ancestral comum.
```

**Listagem 14.7 `trabalhador0.h`**

```cpp
// trabalhador0.h -- classes de trabalhadores
#ifndef TRABALHADOR0_H_
#define TRABALHADOR0_H_
#include <string>

class Trabalhador  // classe base abstrata
{
private:
    std::string nomeCompleto;
    long id;
public:
    Trabalhador() : nomeCompleto("ninguem"), id(0L) {}
    Trabalhador(const std::string & s, long n)
         : nomeCompleto(s), id(n) {}
    virtual ~Trabalhador() = 0;  // destrutor virtual puro
    virtual void Definir();
    virtual void Mostrar() const;
};
class Garcom : public Trabalhador
{
private:
    int elegancia;
public:
    Garcom() : Trabalhador(), elegancia(0) {}
    Garcom(const std::string & s, long n, int p = 0)
         : Trabalhador(s, n), elegancia(p) {}
    Garcom(const Trabalhador & wk, int p = 0)
         : Trabalhador(wk), elegancia(p) {}
    void Definir();
    void Mostrar() const;
};
class Cantor : public Trabalhador
{
protected:
    enum {outro, alto, contralto, soprano,
              baixo, baritono, tenor};
    enum {Ttipos = 7};
private:
    static char *pv[Ttipos];  // equivalentes string dos tipos de voz
    int voz;
public:
    Cantor() : Trabalhador(), voz(outro) {}
    Cantor(const std::string & s, long n, int v = outro)
         : Trabalhador(s, n), voz(v) {}
    Cantor(const Trabalhador & wk, int v = outro)
         : Trabalhador(wk), voz(v) {}
    void Definir();
    void Mostrar() const;
};
#endif
```

As declarações de classe na Listagem 14.7 incluem algumas constantes internas que representam tipos de voz. Uma enumeração torna `alto`, `contralto` e assim por diante constantes simbólicas para tipos de voz, e o array estático `pv` armazena ponteiros para os equivalentes em string estilo-C. O arquivo de implementação, mostrado na Listagem 14.8, inicializa esse array e fornece as definições dos métodos.

**Listagem 14.8 `trabalhador0.cpp`**

```cpp
// trabalhador0.cpp -- métodos das classes de trabalhadores
#include "trabalhador0.h"
#include <iostream>
using std::cout;
using std::cin;
using std::endl;
// métodos de Trabalhador
// o destrutor virtual puro deve ser implementado, mesmo que seja puro
Trabalhador::~Trabalhador() {}
void Trabalhador::Definir()
{
    cout << "Digite o nome do trabalhador: ";
    getline(cin, nomeCompleto);
    cout << "Digite o ID do trabalhador: ";
    cin >> id;
    while (cin.get() != '\n')
        continue;
}
void Trabalhador::Mostrar() const
{
    cout << "Nome: " << nomeCompleto << "\n";
    cout << "ID do funcionario: " << id << "\n";
}
// métodos de Garcom
void Garcom::Definir()
{
    Trabalhador::Definir();
    cout << "Digite a avaliacao de elegancia do garcom: ";
    cin >> elegancia;
    while (cin.get() != '\n')
        continue;
}
void Garcom::Mostrar() const
{
    cout << "Categoria: garcom\n";
    Trabalhador::Mostrar();
    cout << "Avaliacao de elegancia: " << elegancia << "\n";
}
// métodos de Cantor
char * Cantor::pv[] = {"outro", "alto", "contralto",
       "soprano", "baixo", "baritono", "tenor"};

void Cantor::Definir()
{
    Trabalhador::Definir();
    cout << "Digite o numero para o alcance vocal do cantor:\n";
    int i;
    for (i = 0; i < Ttipos; i++)
    {
        cout << i << ": " << pv[i] << " ";
        if (i % 4 == 3)
            cout << endl;
    }
    if (i % 4 != 0)
        cout << endl;
    while (cin >> voz && (voz < 0 || voz >= Ttipos))
        cout << "Por favor, insira um valor >= 0 e < " << Ttipos << endl;
    while (cin.get() != '\n')
        continue;
}
void Cantor::Mostrar() const
{
    cout << "Categoria: cantor\n";
    Trabalhador::Mostrar();
    cout << "Alcance vocal: " << pv[voz] << endl;
}
```

A Listagem 14.9 fornece um breve teste das classes, usando um array polimórfico de ponteiros.

**Listagem 14.9 `testartrabalhador.cpp`**

```cpp
// testartrabalhador.cpp -- testar hierarquia da classe trabalhador
#include <iostream>
#include "trabalhador0.h"
const int LIM = 4;
int main()
{
    Garcom bob("Bob Apple", 314L, 5);
    Cantor bev("Beverly Hills", 522L, 3);
    Garcom w_temp;
    Cantor s_temp;
    Trabalhador * pw[LIM] = {&bob, &bev, &w_temp, &s_temp};
    int i;
    for (i = 2; i < LIM; i++)
        pw[i]->Definir();
    for (i = 0; i < LIM; i++)
    {
        pw[i]->Mostrar();
        std::cout << std::endl;
    }
    return 0;
}
```

Aqui está a saída do programa das Listagens 14.7, 14.8 e 14.9:

```
Digite o nome do trabalhador: Waldo Dropmaster
Digite o ID do trabalhador: 442
Digite a avaliacao de elegancia do garcom: 3
Digite o nome do trabalhador: Sylvie Sirenne
Digite o ID do trabalhador: 555
Digite o numero para o alcance vocal do cantor:
0: outro 1: alto 2: contralto 3: soprano
4: baixo 5: baritono 6: tenor
Categoria: garcom
Nome: Bob Apple
ID do funcionario: 314
Avaliacao de elegancia: 5
Categoria: cantor
Nome: Beverly Hills
ID do funcionario: 522
Alcance vocal: soprano
Categoria: garcom
Nome: Waldo Dropmaster
ID do funcionario: 442
Avaliacao de elegancia: 3
Categoria: cantor
Nome: Sylvie Sirenne
ID do funcionario: 555
Alcance vocal: baritono
```

Esse projeto parece funcionar bem para as classes `Garcom` e `Cantor`, mas encontra problemas se você tentar combinar as duas via herança múltipla com uma classe `GarcomCantor`. Especificamente, você encontrará ambiguidade nos métodos `Mostrar()` herdados de `Garcom` e de `Cantor`.

### Problemas de Herança Múltipla

Suponha que você começasse com esta definição simples de `GarcomCantor`:

```cpp
class GarcomCantor : public Garcom, public Cantor {...};
```

Com essa definição, quando você tentasse invocar `Mostrar()` para um objeto `GarcomCantor`, o compilador ficaria confuso:

```cpp
GarcomCantor newhire("Elise Hawks", 2005, 6, soprano);
newhire.Mostrar();  // ambíguo
```

Tanto `Garcom` quanto `Cantor` herdam um componente `Trabalhador`, portanto a classe `GarcomCantor` acaba com dois componentes `Trabalhador`. Isso seria semelhante a derivar uma classe de dois `Trabalhadores` diferentes (o que não é o caso aqui, mas tem efeito semelhante). Você se depara com a questão de qual método de `Trabalhador` usar.

Para resolver a ambiguidade do método, você pode usar o operador de resolução de escopo:

```cpp
newhire.Garcom::Mostrar();   // usa a versão de Garcom
newhire.Cantor::Mostrar();   // usa a versão de Cantor
```

No entanto, você ainda precisa de um único método `Mostrar()` para o objeto `GarcomCantor`. Nenhuma das abordagens acima é completamente satisfatória.

O problema do subobjeto duplicado de `Trabalhador` é mais sério. Para isso, a solução é usar classes base *virtuais*.

## Classes Base Virtuais

*Classes base virtuais* (virtual base classes) permitem que uma classe herde de uma classe que, em múltiplos caminhos de herança, seja representada por um único objeto base. Você torna `Trabalhador` virtual declarando-o com a palavra-chave `virtual` ao derivar `Garcom` e `Cantor`:

```cpp
class Garcom : virtual public Trabalhador {...};
class Cantor : public virtual Trabalhador {...};
```

Em seguida, você definirá `GarcomCantor` como antes:

```cpp
class GarcomCantor : public Garcom, public Cantor {...};
```

Agora um objeto `GarcomCantor` conterá um único subobjeto `Trabalhador` compartilhado por todos os objetos `Garcom` e `Cantor`. Como resultado, os métodos de `Trabalhador` não serão ambíguos nesse aspecto — há apenas um `Trabalhador` para chamar.

Observe que a ordem das palavras-chave `virtual` e `public` é irrelevante — ambas as formas `virtual public` e `public virtual` são aceitas.

### Novos Problemas de Construtor com Classes Base Virtuais

O uso de classes base virtuais requer alteração das regras para inicializações de construtores. Para herança não virtual, a única classe que pode chamar construtores de classes base *indiretamente* por meio da cadeia de derivação é a classe que faz a derivação imediata. Mas com classes base virtuais, o compilador exige que o construtor da classe mais derivada (a que cria o objeto mais derivado) chame diretamente os construtores da classe base virtual.

Para a versão não virtual da herança, se você tiver a seguinte hierarquia de classes:

```
class A
class B : public A
class C : public B
```

então o construtor de `C` pode chamar apenas o construtor de `B`, e o construtor de `B` pode chamar apenas o construtor de `A`. Se, porém, a hierarquia tiver bases virtuais, o construtor da classe mais derivada deve chamar os construtores de todas as suas classes base virtuais.

Aqui o construtor de `GarcomCantor` deve chamar o construtor de `Trabalhador` diretamente:

```cpp
GarcomCantor(const std::string & s, long n, int p = 0, int v = outro)
    : Trabalhador(s, n), Garcom(s, n, p), Cantor(s, n, v) {}
```

Se `GarcomCantor` não fizer isso, o compilador usará o construtor padrão de `Trabalhador` para o subobjeto `Trabalhador`. Para objetos não-`GarcomCantor` da hierarquia, qualquer chamada explícita a um construtor de base virtual não-direta é simplesmente ignorada.

Isso pode parecer um encargo extra imposto ao programador, mas é necessário. Com a herança de classes base virtuais, a responsabilidade de inicializar o subobjeto compartilhado cabe à classe que cria o objeto completo — ou seja, a classe mais derivada.

### Qual Método? A Abordagem Modular

Além do problema dos dois subobjetos `Trabalhador`, a herança múltipla cria dificuldades com a estratégia incremental (chamar a versão da classe base no método da classe derivada). Com classes base virtuais, você precisa de uma abordagem modular em vez de incremental.

A *abordagem modular* define métodos `Dados()` protegidos que exibem apenas os dados da classe em questão, sem repassar ao ancestral. Cada classe `Mostrar()` então chama o método `Trabalhador::Dados()` para exibir os dados compartilhados, seguido de seus próprios `Dados()`:

```cpp
// abordagem modular
void Garcom::Mostrar() const
{
    cout << "Categoria: garcom\n";
    Trabalhador::Dados();  // exibe dados de Trabalhador
    Dados();               // exibe dados de Garcom
}
```

Para `GarcomCantor`, `Mostrar()` fica:

```cpp
void GarcomCantor::Mostrar() const
{
    cout << "Categoria: garcom-cantor\n";
    Trabalhador::Dados();  // exibe dados de Trabalhador (apenas uma vez)
    Dados();               // exibe dados de GarcomCantor
}
```

E o método protegido `Dados()` de `GarcomCantor` exibe apenas os dados das classes filhas:

```cpp
void GarcomCantor::Dados() const
{
    Cantor::Dados();
    Garcom::Dados();
}
```

### Exemplo Completo com Classes Base Virtuais

As Listagens 14.10, 14.11 e 14.12 mostram o exemplo completo com `GarcomCantor`, usando classes base virtuais e a abordagem modular.

**Listagem 14.10 `trabalhadormi.h`**

```cpp
// trabalhadormi.h -- classes de trabalhadores com herança múltipla
#ifndef TRABALHADORMI_H_
#define TRABALHADORMI_H_
#include <string>

class Trabalhador  // classe base abstrata
{
private:
    std::string nomeCompleto;
    long id;
protected:
    virtual void Dados() const;
    virtual void Obter();
public:
    Trabalhador() : nomeCompleto("ninguem"), id(0L) {}
    Trabalhador(const std::string & s, long n)
         : nomeCompleto(s), id(n) {}
    virtual ~Trabalhador() = 0;  // função virtual pura
    virtual void Definir() = 0;
    virtual void Mostrar() const = 0;
};
class Garcom : virtual public Trabalhador
{
private:
    int elegancia;
protected:
    void Dados() const;
    void Obter();
public:
    Garcom() : Trabalhador(), elegancia(0) {}
    Garcom(const std::string & s, long n, int p = 0)
         : Trabalhador(s, n), elegancia(p) {}
    Garcom(const Trabalhador & wk, int p = 0)
         : Trabalhador(wk), elegancia(p) {}
    void Definir();
    void Mostrar() const;
};
class Cantor : virtual public Trabalhador
{
protected:
    enum {outro, alto, contralto, soprano,
              baixo, baritono, tenor};
    enum {Ttipos = 7};
    void Dados() const;
    void Obter();
private:
    static char *pv[Ttipos];  // equivalentes string dos tipos de voz
    int voz;
public:
    Cantor() : Trabalhador(), voz(outro) {}
    Cantor(const std::string & s, long n, int v = outro)
         : Trabalhador(s, n), voz(v) {}
    Cantor(const Trabalhador & wk, int v = outro)
         : Trabalhador(wk), voz(v) {}
    void Definir();
    void Mostrar() const;
};
// herança múltipla
class GarcomCantor : public Cantor, public Garcom
{
protected:
    void Dados() const;
    void Obter();
public:
    GarcomCantor() {}
    GarcomCantor(const std::string & s, long n, int p = 0,
                  int v = outro)
         : Trabalhador(s, n), Garcom(s, n, p), Cantor(s, n, v) {}
    GarcomCantor(const Trabalhador & wk, int p = 0, int v = outro)
         : Trabalhador(wk), Garcom(wk, p), Cantor(wk, v) {}
    GarcomCantor(const Garcom & wt, int v = outro)
         : Trabalhador(wt), Garcom(wt), Cantor(wt, v) {}
    GarcomCantor(const Cantor & wt, int p = 0)
         : Trabalhador(wt), Garcom(wt, p), Cantor(wt) {}
    void Definir();
    void Mostrar() const;
};
#endif
```

**Listagem 14.11 `trabalhadormi.cpp`**

```cpp
// trabalhadormi.cpp -- métodos das classes de trabalhadores com herança múltipla
#include "trabalhadormi.h"
#include <iostream>
using std::cout;
using std::cin;
using std::endl;
// métodos de Trabalhador
Trabalhador::~Trabalhador() {}
// métodos protegidos
void Trabalhador::Dados() const
{
    cout << "Nome: " << nomeCompleto << endl;
    cout << "ID do funcionario: " << id << endl;
}
void Trabalhador::Obter()
{
    getline(cin, nomeCompleto);
    cout << "Digite o ID do trabalhador: ";
    cin >> id;
    while (cin.get() != '\n')
        continue;
}
// métodos de Garcom
void Garcom::Definir()
{
    cout << "Digite o nome do garcom: ";
    Trabalhador::Obter();
    Obter();
}
void Garcom::Mostrar() const
{
    cout << "Categoria: garcom\n";
    Trabalhador::Dados();
    Dados();
}
// métodos protegidos
void Garcom::Dados() const
{
    cout << "Avaliacao de elegancia: " << elegancia << endl;
}
void Garcom::Obter()
{
    cout << "Digite a avaliacao de elegancia do garcom: ";
    cin >> elegancia;
    while (cin.get() != '\n')
        continue;
}
// métodos de Cantor
char * Cantor::pv[Cantor::Ttipos] = {"outro", "alto", "contralto",
       "soprano", "baixo", "baritono", "tenor"};

void Cantor::Definir()
{
    cout << "Digite o nome do cantor: ";
    Trabalhador::Obter();
    Obter();
}
void Cantor::Mostrar() const
{
    cout << "Categoria: cantor\n";
    Trabalhador::Dados();
    Dados();
}
// métodos protegidos
void Cantor::Dados() const
{
    cout << "Alcance vocal: " << pv[voz] << endl;
}
void Cantor::Obter()
{
    cout << "Digite o numero para o alcance vocal do cantor:\n";
    int i;
    for (i = 0; i < Ttipos; i++)
    {
        cout << i << ": " << pv[i] << " ";
        if (i % 4 == 3)
            cout << endl;
    }
    if (i % 4 != 0)
        cout << '\n';
    cin >> voz;
    while (cin.get() != '\n')
        continue;
}
// métodos de GarcomCantor
void GarcomCantor::Dados() const
{
    Cantor::Dados();
    Garcom::Dados();
}
void GarcomCantor::Obter()
{
    Garcom::Obter();
    Cantor::Obter();
}
void GarcomCantor::Definir()
{
    cout << "Digite o nome do garcom-cantor: ";
    Trabalhador::Obter();
    Obter();
}
void GarcomCantor::Mostrar() const
{
    cout << "Categoria: garcom-cantor\n";
    Trabalhador::Dados();
    Dados();
}
```

A curiosidade exige que você teste essas classes, e a Listagem 14.12 fornece o código para isso. Observe que o programa faz uso de polimorfismo, atribuindo os endereços de vários tipos de classes a ponteiros para a classe base. O programa também usa a função de biblioteca estilo-C `strchr()` no seguinte teste:

```cpp
while (strchr("gctq", escolha) == NULL)
```

Essa função retorna o endereço da primeira ocorrência do valor do caractere `escolha` na string `"gctq"`; a função retorna o ponteiro `NULL` se o caractere não for encontrado. Esse teste é mais simples de escrever do que uma instrução `if` que compara `escolha` com cada letra individualmente.

Certifique-se de compilar a Listagem 14.12 junto com `trabalhadormi.cpp`.

**Listagem 14.12 `trabalhadormi_teste.cpp`**

```cpp
// trabalhadormi_teste.cpp -- herança múltipla
// compilar com trabalhadormi.cpp
#include <iostream>
#include <cstring>
#include "trabalhadormi.h"
const int TAMANHO = 5;
int main()
{
    using std::cin;
    using std::cout;
    using std::endl;
    using std::strchr;
    Trabalhador * lolas[TAMANHO];
    int ct;
    for (ct = 0; ct < TAMANHO; ct++)
    {
        char escolha;
        cout << "Digite a categoria do funcionario:\n"
             << "g: garcom c: cantor "
             << "t: garcom-cantor q: sair\n";
        cin >> escolha;
        while (strchr("gctq", escolha) == NULL)
        {
            cout << "Por favor, insira g, c, t ou q: ";
            cin >> escolha;
        }
        if (escolha == 'q')
            break;
        switch(escolha)
        {
            case 'g': lolas[ct] = new Garcom;
                   break;
            case 'c': lolas[ct] = new Cantor;
                   break;
            case 't': lolas[ct] = new GarcomCantor;
                   break;
        }
        cin.get();
        lolas[ct]->Definir();
    }
    cout << "\nAqui esta sua equipe:\n";
    int i;
    for (i = 0; i < ct; i++)
    {
        cout << endl;
        lolas[i]->Mostrar();
    }
    for (i = 0; i < ct; i++)
        delete lolas[i];
    cout << "Ate logo.\n";
    return 0;
}
```

Aqui está um exemplo de execução do programa das Listagens 14.10, 14.11 e 14.12:

```
Digite a categoria do funcionario:
g: garcom c: cantor t: garcom-cantor q: sair
g
Digite o nome do garcom: Wally Slipshod
Digite o ID do trabalhador: 1040
Digite a avaliacao de elegancia do garcom: 4
Digite a categoria do funcionario:
g: garcom c: cantor t: garcom-cantor q: sair
c
Digite o nome do cantor: Sinclair Parma
Digite o ID do trabalhador: 1044
Digite o numero para o alcance vocal do cantor:
0: outro 1: alto 2: contralto 3: soprano
4: baixo 5: baritono 6: tenor
Digite a categoria do funcionario:
g: garcom c: cantor t: garcom-cantor q: sair
t
Digite o nome do garcom-cantor: Natasha Gargalova
Digite o ID do trabalhador: 1021
Digite a avaliacao de elegancia do garcom: 6
Digite o numero para o alcance vocal do cantor:
0: outro 1: alto 2: contralto 3: soprano
4: baixo 5: baritono 6: tenor
Digite a categoria do funcionario:
g: garcom c: cantor t: garcom-cantor q: sair
q

Aqui esta sua equipe:
Categoria: garcom
Nome: Wally Slipshod
ID do funcionario: 1040
Avaliacao de elegancia: 4
Categoria: cantor
Nome: Sinclair Parma
ID do funcionario: 1044
Alcance vocal: baritono
Categoria: garcom-cantor
Nome: Natasha Gargalova
ID do funcionario: 1021
Alcance vocal: soprano
Avaliacao de elegancia: 6
Ate logo.
```

Vamos examinar mais alguns aspectos relacionados à herança múltipla.

## Bases Virtuais e Não Virtuais Misturadas

Vamos considerar novamente o caso de uma classe derivada que herda uma classe base por mais de uma rota. Se a classe base for virtual, a classe derivada contém um subobjeto da classe base. Se a classe base não for virtual, a classe derivada contém múltiplos subobjetos. O que acontece se houver uma mistura? Suponha, por exemplo, que a classe `B` seja uma classe base virtual para as classes `C` e `D`, e uma classe base não virtual para as classes `X` e `Y`. Além disso, suponha que a classe `M` seja derivada de `C`, `D`, `X` e `Y`. Neste caso, a classe `M` contém um subobjeto da classe `B` para todos os ancestrais herdados virtualmente (ou seja, as classes `C` e `D`), e um subobjeto separado da classe `B` para cada ancestral não virtual (ou seja, as classes `X` e `Y`). Portanto, ao todo, `M` conteria três subobjetos da classe `B`. Quando uma classe herda uma determinada classe base por vários caminhos virtuais e vários caminhos não virtuais, a classe tem um subobjeto da classe base para representar todos os caminhos virtuais e um subobjeto separado da classe base para representar cada caminho não virtual.

## Classes Base Virtuais e Dominância

O uso de classes base virtuais altera a forma como o C++ resolve ambiguidades. Com classes base não virtuais, as regras são simples. Se uma classe herda dois ou mais membros (dados ou métodos) com o mesmo nome de classes diferentes, usar esse nome sem qualificá-lo com um nome de classe é ambíguo. Se classes base virtuais estiverem envolvidas, no entanto, tal uso pode ou não ser ambíguo. Nesse caso, se um nome *domina* todos os outros, ele pode ser usado sem ambiguidade e sem qualificador.

Portanto, como um nome de membro domina outro? Um nome numa classe derivada *domina* o mesmo nome em qualquer classe ancestral, seja ela direta ou indireta. Por exemplo, considere as seguintes definições:

```cpp
class B
{
public:
    short q();
    ...
};
class C : virtual public B
{
public:
    long q();
    int omg()
    ...
};
class D : public C
{
    ...
};
class E : virtual public B
{
private:
    int omg();
    ...
};
class F : public D, public E
{
    ...
};
```

Aqui a definição de `q()` na classe `C` domina a definição na classe `B` porque `C` é derivada de `B`. Portanto, métodos em `F` podem usar `q()` para denotar `C::q()`. Por outro lado, nenhuma definição de `omg()` domina a outra porque nem `C` nem `E` é uma classe base da outra. Portanto, uma tentativa de `F` usar um `omg()` não qualificado seria ambígua.

As regras de ambiguidade virtual não prestam atenção às regras de acesso. Ou seja, mesmo que `E::omg()` seja privado e, portanto, não seja diretamente acessível à classe `F`, usar `omg()` é ambíguo. Da mesma forma, mesmo que `C::q()` fosse privado, ele dominaria `D::q()`. Nesse caso, você poderia chamar `B::q()` na classe `F`, mas um `q()` não qualificado para isso se referiria ao `C::q()` inacessível.

## Sinopse da Herança Múltipla

Primeiro, vamos revisar a herança múltipla sem classes base virtuais. Essa forma de herança múltipla não impõe novas regras. No entanto, se uma classe herda dois membros com o mesmo nome mas de classes diferentes, você precisa usar qualificadores de classe na classe derivada para distinguir entre os dois membros. Ou seja, métodos na classe `Valentao`, derivada de `Pistoleiro` e `JogadorDePoquer`, usariam `Pistoleiro::sacar()` e `JogadorDePoquer::sacar()` para distinguir entre os métodos `sacar()` herdados das duas classes. Caso contrário, o compilador reclamaria de uso ambíguo.

Se uma classe herda de uma classe base não virtual por mais de uma rota, então a classe herda um objeto da classe base para cada instância não virtual da classe base. Em alguns casos, isso pode ser o que você quer, mas com mais frequência, múltiplas instâncias de uma classe base são um problema.

Em seguida, vamos analisar a herança múltipla com classes base virtuais. Uma classe se torna uma classe base virtual quando uma classe derivada usa a palavra-chave `virtual` ao indicar a derivação:

```cpp
class marketing : public virtual reality { ... };
```

A principal mudança, e a razão para as classes base virtuais, é que uma classe que herda de uma ou mais instâncias de uma classe base virtual herda apenas um objeto da classe base. Implementar esse recurso implica outros requisitos:

- Uma classe derivada com uma classe base virtual indireta deve ter seus construtores invocando os construtores da classe base indireta diretamente, o que é ilegal para classes base indiretas não virtuais.
- A ambiguidade de nomes é resolvida por meio da regra de dominância.

Como você pode ver, a herança múltipla pode introduzir complexidades de programação. No entanto, a maioria dessas complexidades surge quando uma classe derivada herda da mesma classe base por mais de uma rota. Se você evitar essa situação, a única coisa com que precisa se preocupar é qualificar nomes herdados quando necessário.

---

[Anterior](capitulo-14-01-composicao-heranca-privada.md) | [Índice](README.md) | [Próximo](capitulo-14-03-templates-classe.md)
