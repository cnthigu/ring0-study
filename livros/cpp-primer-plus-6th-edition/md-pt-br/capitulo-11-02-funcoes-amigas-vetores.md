# Capítulo 11 — Funções Amigas e a Classe Vetor

> Tradução não oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Navegação: [Anterior](capitulo-11-01-sobrecarga-operadores.md) | [Índice](README.md) | [Próximo](capitulo-11-03-conversoes-resumo.md)

---

## Criando Funções Amigas (Creating Friends)

O primeiro passo para criar uma função amiga é colocar um protótipo na declaração da classe e prefixar a declaração com a palavra-chave `friend`:

```cpp
friend Tempo operator*(double m, const Tempo & t); // vai na declaração da classe
```

Esse protótipo tem duas implicações:

- Embora a função `operator*()` seja declarada na declaração da classe, ela não é uma função membro. Portanto, ela não é invocada usando o operador de acesso a membros.
- Embora a função `operator*()` não seja uma função membro, ela tem os mesmos direitos de acesso que uma função membro.

O segundo passo é escrever a definição da função. Como ela não é uma função membro, você não usa o qualificador `Tempo::`. Também não usa a palavra-chave `friend` na definição. A definição deve se parecer com esta:

```cpp
Tempo operator*(double m, const Tempo & t) // friend não é usado na definição
{
    Tempo resultado;
    long total_minutos = t.horas * m * 60 + t.minutos * m;
    resultado.horas = total_minutos / 60;
    resultado.minutos = total_minutos % 60;
    return resultado;
}
```

Com essa declaração e definição, a instrução

```cpp
A = 2.75 * B;
```

é traduzida para o seguinte e invoca a função amiga não membro recém-definida:

```cpp
A = operator*(2.75, B);
```

Em resumo, uma *função amiga* (friend function) de uma classe é uma função não membro que tem os mesmos direitos de acesso que uma função membro.

> **Funções Amigas Traem a POO?**
> À primeira vista, pode parecer que as amigas violam o princípio de ocultação de dados da POO porque o mecanismo de amizade permite que funções não membro acessem dados privados. No entanto, essa é uma visão excessivamente estreita. Em vez disso, você deve pensar em funções amigas como parte de uma interface estendida para uma classe. Por exemplo, do ponto de vista conceitual, multiplicar um `double` por um valor `Tempo` é praticamente o mesmo que multiplicar um valor `Tempo` por um `double`. O fato de que a primeira requer uma função amiga enquanto a segunda pode ser feita com uma função membro é resultado da sintaxe do C++, não de uma diferença conceitual profunda. Usando tanto uma função amiga quanto um método de classe, você pode expressar qualquer operação com a mesma interface de usuário. Além disso, lembre-se de que apenas uma declaração de classe pode decidir quais funções são amigas, portanto a declaração de classe ainda controla quais funções acessam dados privados. Em resumo, métodos de classe e amigas são simplesmente dois mecanismos diferentes para expressar uma interface de classe.

Na realidade, você pode escrever essa função amiga particular como uma não amiga alterando a definição para que ela troque qual valor vem primeiro na multiplicação:

```cpp
Tempo operator*(double m, const Tempo & t)
{
    return t * m; // usa t.operator*(m)
}
```

A versão original acessava `t.minutos` e `t.horas` explicitamente, então tinha que ser uma amiga. Esta versão usa apenas o objeto `Tempo` `t` como um todo, deixando uma função membro lidar com os valores privados, portanto esta versão não precisa ser uma amiga. Ainda assim, há razões para tornar esta versão uma amiga também. Mais importante, ela vincula a função como parte da interface oficial da classe. Em segundo lugar, se você mais tarde precisar que a função acesse dados privados diretamente, só precisará mudar a definição da função e não o protótipo da classe.

> **Dica**
> Se você quiser sobrecarregar um operador para uma classe e quiser usar o operador com um termo não de classe como primeiro operando, você pode usar uma função amiga para inverter a ordem dos operandos.

## Um Tipo Comum de Amigo: Sobrecarregando o Operador `<<`

Um recurso muito útil das classes é que você pode sobrecarregar o operador `<<` para usá-lo com `cout` e exibir o conteúdo de um objeto. De certa forma, essa sobrecarga é um pouco mais complicada que os exemplos anteriores, portanto a desenvolveremos em duas etapas em vez de uma.

Suponha que `viagem` seja um objeto `Tempo`. Para exibir valores `Tempo`, temos usado `Exibir()`. Mas não seria bom se você pudesse fazer o seguinte?

```cpp
cout << viagem; // faz cout reconhecer a classe Tempo?
```

Você pode fazer isso porque `<<` é um dos operadores C++ que podem ser sobrecarregados. Na verdade, ele já é muito sobrecarregado. Em sua forma mais básica, o operador `<<` é um dos operadores de manipulação de bits de C e C++; ele desloca bits para a esquerda em um valor. Mas a classe `ostream` sobrecarrega o operador, convertendo-o em uma ferramenta de saída. Lembre-se de que `cout` é um objeto `ostream` e que ele é inteligente o suficiente para reconhecer todos os tipos básicos C++. Isso é porque a declaração da classe `ostream` inclui uma definição sobrecarregada de `operator<<()` para cada um dos tipos básicos. Então uma maneira de ensinar `cout` a reconhecer um objeto `Tempo` seria acrescentar uma nova definição de função operadora à declaração da classe `ostream`. Mas é uma ideia perigosa alterar o arquivo `iostream` e bagunçar uma interface padrão. Em vez disso, use a declaração da classe `Tempo` para ensinar a classe `Tempo` a usar `cout`.

### A Primeira Versão de Sobrecarregar `<<`

Para ensinar a classe `Tempo` a usar `cout`, você pode usar uma função amiga. Por quê? Porque uma instrução como a seguinte usa dois objetos, com o objeto da classe `ostream` (`cout`) primeiro:

```cpp
cout << viagem;
```

Se você usar uma função membro de `Tempo` para sobrecarregar `<<`, o objeto `Tempo` viria primeiro, como ocorreu quando você sobrecarregou o operador `*` com uma função membro. Isso significaria que você teria que usar o operador `<<` desta forma:

```cpp
viagem << cout; // se operator<<() fosse uma função membro de Tempo
```

Isso seria confuso. Mas usando uma função amiga, você pode sobrecarregar o operador desta forma:

```cpp
void operator<<(ostream & os, const Tempo & t)
{
    os << t.horas << " horas, " << t.minutos << " minutos";
}
```

Isso permite usar

```cpp
cout << viagem;
```

para imprimir dados no seguinte formato:

```
4 horas, 23 minutos
```

> **Amiga ou Não Amiga?**
> A nova declaração da classe `Tempo` torna a função `operator<<()` uma função amiga da classe `Tempo`. Mas essa função, embora não seja contrária à classe `ostream`, não é uma amiga dessa classe. A função `operator<<()` recebe um argumento `ostream` e um argumento `Tempo`, então pode parecer que essa função tem que ser amiga das duas classes. Se você olhar o código da função, no entanto, notará que a função acessa membros individuais do objeto `Tempo` mas usa o objeto `ostream` apenas como um todo. Como `operator<<()` acessa membros privados do objeto `Tempo` diretamente, ela deve ser amiga da classe `Tempo`. Mas como ela *não* acessa diretamente membros privados do objeto `ostream`, a função não precisa ser amiga da classe `ostream`. Isso é bom porque significa que você não precisa mexer com a definição de `ostream`.

### A Segunda Versão de Sobrecarregar `<<`

A implementação apresentada tem um problema. Instruções como esta funcionam bem:

```cpp
cout << viagem;
```

Mas a implementação não permite combinar o operador `<<` redefinido com os que `cout` normalmente usa:

```cpp
cout << "Tempo de viagem: " << viagem << " (Terça-feira)\n"; // não funciona
```

Para entender por que isso não funciona e o que precisa ser feito para fazê-lo funcionar, você primeiro precisa saber um pouco mais sobre como `cout` opera. Considere as seguintes instruções:

```cpp
int x = 5;
int y = 8;
cout << x << y;
```

O C++ lê a instrução de saída da esquerda para a direita, o que significa que ela é equivalente ao seguinte:

```cpp
(cout << x) << y;
```

O operador `<<`, conforme definido em `iostream`, recebe um objeto `ostream` à sua esquerda. Claramente, a expressão `cout << x` satisfaz esse requisito porque `cout` é um objeto `ostream`. Mas a instrução de saída também requer que a expressão inteira `(cout << x)` seja um tipo de objeto `ostream` porque essa expressão está à esquerda de `<< y`. Portanto, a classe `ostream` implementa a função `operator<<()` de forma que ela retorna uma referência a um objeto `ostream`. Em particular, ela retorna uma referência ao objeto invocador — `cout`, neste caso. Assim, a expressão `(cout << x)` é ela mesma o objeto `ostream` `cout`, e pode ser usada à esquerda do operador `<<`.

Você pode adotar a mesma abordagem com a função amiga. Basta revisar a função `operator<<()` para que ela retorne uma referência a um objeto `ostream`:

```cpp
ostream & operator<<(ostream & os, const Tempo & t)
{
    os << t.horas << " horas, " << t.minutos << " minutos";
    return os;
}
```

Note que o tipo de retorno é `ostream &`. Lembre-se de que isso significa que a função retorna uma referência a um objeto `ostream`. Como um programa passa uma referência de objeto para a função para começar, o efeito líquido é que o valor de retorno da função é simplesmente o objeto passado a ela. Ou seja, a instrução

```cpp
cout << viagem;
```

torna-se a seguinte chamada de função:

```cpp
operator<<(cout, viagem);
```

E essa chamada retorna o objeto `cout`. Então agora a instrução a seguir funciona:

```cpp
cout << "Tempo de viagem: " << viagem << " (Terça-feira)\n"; // funciona
```

Vamos desmembrar isso em etapas separadas para ver como funciona. Primeiro, o seguinte invoca a definição particular de `ostream` de `<<` que exibe uma string e retorna o objeto `cout`:

```cpp
cout << "Tempo de viagem: "
```

Então a expressão `cout << "Tempo de viagem: "` exibe a string e é então substituída por seu valor de retorno, `cout`. Isso reduz a instrução original à seguinte:

```cpp
cout << viagem << " (Terça-feira)\n";
```

Em seguida, o programa usa a declaração de `<<` de `Tempo` para exibir os valores de `viagem` e retornar o objeto `cout` novamente. Isso reduz a instrução para:

```cpp
cout << " (Terça-feira)\n";
```

O programa agora termina usando a definição `ostream` de `<<` para strings para exibir a string final.

Como observação, esta versão de `operator<<()` também pode ser usada para saída em arquivo:

```cpp
#include <fstream>
...
ofstream fout;
fout.open("salvartempo.txt");
Tempo viagem(12, 40);
fout << viagem;
```

A última instrução torna-se:

```cpp
operator<<(fout, viagem);
```

E como o Capítulo 8 aponta, as propriedades de herança de classe permitem que uma referência `ostream` refira-se a objetos `ostream` e `ofstream`.

> **Dica**
> Em geral, para sobrecarregar o operador `<<` para exibir um objeto da classe `c_nome`, você usa uma função amiga com uma definição nesta forma:
> ```cpp
> ostream & operator<<(ostream & os, const c_nome & obj)
> {
>     os << ... ; // exibir conteúdo do objeto
>     return os;
> }
> ```

A Listagem 11.10 mostra a definição da classe conforme modificada para incluir as duas funções amigas `operator*()` e `operator<<()`. Ela implementa a primeira delas como uma função inline porque o código é muito curto. (Quando a definição também é o protótipo, como neste caso, você usa o prefixo `friend`.)

> **Cuidado**
> Você usa a palavra-chave `friend` apenas no protótipo encontrado na declaração da classe. Você não a usa na definição da função, a menos que a definição também seja o protótipo.

**Listagem 11.10 — mytime3.h**

```cpp
// mytime3.h — classe Tempo com funções amigas
#ifndef MYTIME3_H_
#define MYTIME3_H_
#include <iostream>
class Tempo
{
private:
    int horas;
    int minutos;
public:
    Tempo();
    Tempo(int h, int m = 0);
    void AdicionarMin(int m);
    void AdicionarHr(int h);
    void Redefinir(int h = 0, int m = 0);
    Tempo operator+(const Tempo & t) const;
    Tempo operator-(const Tempo & t) const;
    Tempo operator*(double n) const;
    friend Tempo operator*(double m, const Tempo & t)
        { return t * m; } // definição inline
    friend std::ostream & operator<<(std::ostream & os, const Tempo & t);
};
#endif
```

A Listagem 11.11 mostra o conjunto revisado de definições. Observe novamente que os métodos usam o qualificador `Tempo::`, enquanto as funções amigas não o usam. Observe também que como `mytime3.h` inclui `iostream` e fornece a declaração using `std::ostream`, incluir `mytime3.h` em `mytime3.cpp` fornece suporte para usar `ostream` no arquivo de implementação.

**Listagem 11.11 — mytime3.cpp**

```cpp
// mytime3.cpp — implementando os métodos de Tempo
#include "mytime3.h"
Tempo::Tempo()
{
    horas = minutos = 0;
}
Tempo::Tempo(int h, int m)
{
    horas = h;
    minutos = m;
}
void Tempo::AdicionarMin(int m)
{
    minutos += m;
    horas += minutos / 60;
    minutos %= 60;
}
void Tempo::AdicionarHr(int h)
{
    horas += h;
}
void Tempo::Redefinir(int h, int m)
{
    horas = h;
    minutos = m;
}
Tempo Tempo::operator+(const Tempo & t) const
{
    Tempo soma;
    soma.minutos = minutos + t.minutos;
    soma.horas = horas + t.horas + soma.minutos / 60;
    soma.minutos %= 60;
    return soma;
}
Tempo Tempo::operator-(const Tempo & t) const
{
    Tempo dif;
    int tot1, tot2;
    tot1 = t.minutos + 60 * t.horas;
    tot2 = minutos + 60 * horas;
    dif.minutos = (tot2 - tot1) % 60;
    dif.horas = (tot2 - tot1) / 60;
    return dif;
}
Tempo Tempo::operator*(double mult) const
{
    Tempo resultado;
    long total_minutos = horas * mult * 60 + minutos * mult;
    resultado.horas = total_minutos / 60;
    resultado.minutos = total_minutos % 60;
    return resultado;
}
std::ostream & operator<<(std::ostream & os, const Tempo & t)
{
    os << t.horas << " horas, " << t.minutos << " minutos";
    return os;
}
```

A Listagem 11.12 mostra um programa de exemplo. Tecnicamente, `usetime3.cpp` não precisa incluir `iostream` porque `mytime3.h` já inclui esse arquivo. Porém, como usuário da classe `Tempo`, você não necessariamente sabe quais arquivos são incluídos no código da classe, então você assumiria a responsabilidade de declarar os arquivos de cabeçalho que você sabe que sua parte do código precisa.

**Listagem 11.12 — usetime3.cpp**

```cpp
// usetime3.cpp — usando a quarta versão da classe Tempo
// compilar usetime3.cpp e mytime3.cpp juntos
#include <iostream>
#include "mytime3.h"

int main()
{
    using std::cout;
    using std::endl;
    Tempo aida(3, 35);
    Tempo tosca(2, 48);
    Tempo temp;
    cout << "Aida e Tosca:\n";
    cout << aida << "; " << tosca << endl;
    temp = aida + tosca; // operator+()
    cout << "Aida + Tosca: " << temp << endl;
    temp = aida * 1.17; // operator*() membro
    cout << "Aida * 1.17: " << temp << endl;
    cout << "10.0 * Tosca: " << 10.0 * tosca << endl;
    return 0;
}
```

Aqui está a saída do programa das Listagens 11.10, 11.11 e 11.12:

```
Aida e Tosca:
3 horas, 35 minutos; 2 horas, 48 minutos
Aida + Tosca: 6 horas, 23 minutos
Aida * 1.17: 4 horas, 11 minutos
10.0 * Tosca: 28 horas, 0 minutos
```

## Operadores Sobrecarregados: Funções Membro Versus Funções Não Membro

Para muitos operadores, você tem a escolha entre usar funções membro ou funções não membro para implementar a sobrecarga de operadores. Tipicamente, a versão não membro é uma função amiga para que possa acessar diretamente os dados privados de uma classe. Por exemplo, considere o operador de adição para a classe `Tempo`. Ele tem este protótipo na declaração da classe `Tempo`:

```cpp
Tempo operator+(const Tempo & t) const; // versão membro
```

Em vez disso, a classe poderia usar o seguinte protótipo:

```cpp
// versão não membro
friend Tempo operator+(const Tempo & t1, const Tempo & t2);
```

O operador de adição requer dois operandos. Para a versão de função membro, um é passado implicitamente via o ponteiro `this` e o segundo é passado explicitamente como argumento de função. Para a versão amiga, ambos são passados como argumentos.

> **Nota**
> Uma versão não membro de uma função operadora sobrecarregada requer tantos parâmetros formais quantos operandos o operador tiver. Uma versão membro do mesmo operador requer um parâmetro a menos porque um operando é passado implicitamente como objeto invocador.

Qualquer um desses dois protótipos corresponde à expressão `T2 + T3`, onde `T2` e `T3` são objetos do tipo `Tempo`. Ou seja, o compilador pode converter a instrução

```cpp
T1 = T2 + T3;
```

para qualquer uma das seguintes:

```cpp
T1 = T2.operator+(T3); // função membro
T1 = operator+(T2, T3); // função não membro
```

Tenha em mente que você deve escolher uma forma ou outra ao definir um dado operador, mas não ambas. Como ambas as formas correspondem à mesma expressão, definir ambas é um erro de ambiguidade, levando a um erro de compilação.

Qual forma, então, é melhor usar? Para alguns operadores, como mencionado anteriormente, a função membro é a única escolha válida. Caso contrário, muitas vezes não faz muita diferença. Às vezes, dependendo do projeto da classe, a versão não membro pode ter uma vantagem, particularmente se você definiu conversões de tipo para a classe. A seção "Conversões e Amigas," perto do final deste capítulo, discute essa situação com mais detalhes.

## Mais Sobrecarga: Uma Classe Vetor (More Overloading: A Vector Class)

Vamos examinar outro projeto de classe que usa sobrecarga de operadores e amigas — uma classe que representa vetores. Essa classe também ilustra outros aspectos do projeto de classes, como incorporar duas maneiras diferentes de descrever a mesma coisa em um objeto. Um *vetor* (vector), no sentido usado em engenharia e física, é uma quantidade que tem tanto magnitude (tamanho) quanto direção. Por exemplo, se você empurra algo, o efeito depende de com que força você empurra (a magnitude) e em qual direção você empurra. Para descrever completamente o movimento de seu carro, você deve dar tanto a velocidade (a magnitude) quanto a direção. (Imunologistas e cientistas da computação podem usar o termo *vetor* de forma diferente; ignore-os, pelo menos até o Capítulo 16, "A Classe `string` e a Biblioteca de Template Padrão," que examina uma versão da ciência da computação, a classe template `vector`.) Vetores são uma escolha natural para a sobrecarga de operadores. Primeiro, você não pode representar um vetor com um único número, então faz sentido criar uma classe para representar vetores. Segundo, vetores têm análogos às operações aritméticas comuns, como adição e subtração. Esse paralelismo sugere sobrecarregar os operadores correspondentes para que você possa usá-los com vetores.

Para manter as coisas simples, nesta seção implementaremos um vetor bidimensional, como um deslocamento na tela, em vez de um vetor tridimensional. Você precisa de apenas dois números para descrever um vetor bidimensional, mas tem a escolha do conjunto de dois números:

- Você pode descrever um vetor por sua magnitude (comprimento) e direção (um ângulo).
- Você pode representar um vetor por seus componentes x e y.

Os componentes são um vetor horizontal (o componente x) e um vetor vertical (o componente y), que se somam para dar o vetor final. Por exemplo, você pode descrever um movimento como mover um ponto 30 unidades para a direita e 40 unidades para cima. Esse movimento coloca o ponto no mesmo lugar que mover 50 unidades em um ângulo de 53,1° a partir da horizontal. Portanto, um vetor com magnitude de 50 e ângulo de 53,1° é equivalente a um vetor com componente horizontal de 30 e componente vertical de 40.

Às vezes uma forma é mais conveniente, às vezes a outra, então você incorporará ambas as representações na descrição da classe. Também projetará a classe de forma que, se você alterar uma representação de um vetor, o objeto atualize automaticamente a outra representação. A capacidade de incorporar tal inteligência em um objeto é outra virtude da classe C++. A Listagem 11.13 apresenta uma declaração de classe. Para refrescar sua memória sobre namespaces, a listagem coloca a declaração da classe dentro do namespace `VETOR`. Também o programa usa `enum` para criar duas constantes (`CART` e `POL`) para identificar as duas representações.

**Listagem 11.13 — vect.h**

```cpp
// vect.h — classe Vetor com <<, modo de estado
#ifndef VETOR_H_
#define VETOR_H_
#include <iostream>
namespace VETOR
{
    class Vetor
    {
    public:
        enum Modo {CART, POL};
        // CART para cartesiano, POL para polar
    private:
        double x;    // valor horizontal
        double y;    // valor vertical
        double mag;  // comprimento do vetor
        double ang;  // direção do vetor em graus
        Modo modo;   // CART ou POL
        // métodos privados para definir valores
        void def_mag();
        void def_ang();
        void def_x();
        void def_y();
    public:
        Vetor();
        Vetor(double n1, double n2, Modo forma = CART);
        void redefinir(double n1, double n2, Modo forma = CART);
        ~Vetor();
        double valx() const {return x;} // retorna valor x
        double valy() const {return y;} // retorna valor y
        double valmag() const {return mag;} // retorna magnitude
        double valang() const {return ang;} // retorna ângulo
        void modo_polar();    // define modo para POL
        void modo_cart();     // define modo para CART
        // sobrecarga de operadores
        Vetor operator+(const Vetor & b) const;
        Vetor operator-(const Vetor & b) const;
        Vetor operator-() const;
        Vetor operator*(double n) const;
        // amigas
        friend Vetor operator*(double n, const Vetor & a);
        friend std::ostream &
            operator<<(std::ostream & os, const Vetor & v);
    };
} // fim do namespace VETOR
#endif
```

Observe que as quatro funções em Listagem 11.13 que reportam valores de componentes são definidas na declaração de classe. Isso as torna automaticamente funções inline. O fato de essas funções serem tão curtas as torna excelentes candidatas para inlining. Nenhuma delas deve alterar dados do objeto, então são declaradas usando o modificador `const`.

A Listagem 11.14 mostra todos os métodos e funções amigas declaradas na Listagem 11.13. A listagem usa a natureza aberta de namespaces para adicionar as definições de métodos ao namespace `VETOR`. Note como as funções construtoras e a função `redefinir()` definem tanto as representações retangular quanto polar do vetor. Assim, qualquer conjunto de valores está disponível imediatamente sem cálculo adicional. Também, como mencionado nos Capítulos 4 e 7, as funções matemáticas internas do C++ usam ângulos em radianos, portanto as funções incorporam conversão de e para graus nos métodos. A implementação da classe `Vetor` oculta coisas como converter de coordenadas polares para retangulares ou converter radianos para graus do usuário. Tudo o que o usuário precisa saber é que a classe usa ângulos em graus e que torna um vetor disponível em duas representações equivalentes.

**Listagem 11.14 — vect.cpp**

```cpp
// vect.cpp — métodos para a classe Vetor
#include <cmath>
#include "vect.h" // inclui <iostream>
using std::sqrt;
using std::sin;
using std::cos;
using std::atan;
using std::atan2;
using std::cout;
namespace VETOR
{
    // calcula graus em um radiano
    const double Rad_para_grau = 45.0 / atan(1.0);
    // deve ser aproximadamente 57.2957795130823
    // métodos privados
    // calcula magnitude a partir de x e y
    void Vetor::def_mag()
    {
        mag = sqrt(x * x + y * y);
    }
    void Vetor::def_ang()
    {
        if (x == 0.0 && y == 0.0)
            ang = 0.0;
        else
            ang = atan2(y, x);
    }
    // define x a partir de coordenada polar
    void Vetor::def_x()
    {
        x = mag * cos(ang);
    }
    // define y a partir de coordenada polar
    void Vetor::def_y()
    {
        y = mag * sin(ang);
    }
    // métodos públicos
    Vetor::Vetor() // construtor padrão
    {
        x = y = mag = ang = 0.0;
        modo = CART;
    }
    // constrói vetor a partir de coordenadas cartesianas se forma for CART
    // (o padrão), ou a partir de coordenadas polares se forma for POL
    Vetor::Vetor(double n1, double n2, Modo forma)
    {
        modo = forma;
        if (forma == CART)
        {
            x = n1;
            y = n2;
            def_mag();
            def_ang();
        }
        else if (forma == POL)
        {
            mag = n1;
            ang = n2 / Rad_para_grau;
            def_x();
            def_y();
        }
        else
        {
            cout << "Terceiro argumento incorreto para Vetor() -- ";
            cout << "vetor definido como 0\n";
            x = y = mag = ang = 0.0;
            modo = CART;
        }
    }
    // redefine vetor a partir de coordenadas cartesianas se forma for
    // CART (o padrão), ou a partir de coordenadas polares se forma for POL
    void Vetor::redefinir(double n1, double n2, Modo forma)
    {
        modo = forma;
        if (forma == CART)
        {
            x = n1;
            y = n2;
            def_mag();
            def_ang();
        }
        else if (forma == POL)
        {
            mag = n1;
            ang = n2 / Rad_para_grau;
            def_x();
            def_y();
        }
        else
        {
            cout << "Terceiro argumento incorreto para Vetor() -- ";
            cout << "vetor definido como 0\n";
            x = y = mag = ang = 0.0;
            modo = CART;
        }
    }
    Vetor::~Vetor() // destrutor
    {
    }
    void Vetor::modo_polar() // define para modo polar
    {
        modo = POL;
    }
    void Vetor::modo_cart() // define para modo cartesiano
    {
        modo = CART;
    }
    // sobrecarga de operadores
    // soma dois Vetores
    Vetor Vetor::operator+(const Vetor & b) const
    {
        return Vetor(x + b.x, y + b.y);
    }
    // subtrai Vetor b de a
    Vetor Vetor::operator-(const Vetor & b) const
    {
        return Vetor(x - b.x, y - b.y);
    }
    // inverte o sinal do Vetor
    Vetor Vetor::operator-() const
    {
        return Vetor(-x, -y);
    }
    // multiplica vetor por n
    Vetor Vetor::operator*(double n) const
    {
        return Vetor(n * x, n * y);
    }
    // métodos amigos
    // multiplica n por Vetor a
    Vetor operator*(double n, const Vetor & a)
    {
        return a * n;
    }
    // exibe coordenadas cartesianas se modo for CART,
    // caso contrário exibe coordenadas polares se modo for POL
    std::ostream & operator<<(std::ostream & os, const Vetor & v)
    {
        if (v.modo == Vetor::CART)
            os << "(x,y) = (" << v.x << ", " << v.y << ")";
        else if (v.modo == Vetor::POL)
        {
            os << "(m,a) = (" << v.mag << ", "
               << v.ang * Rad_para_grau << ")";
        }
        else
            os << "Modo do objeto Vetor e invalido";
        return os;
    }
} // fim do namespace VETOR
```

Você poderia projetar a classe de forma diferente. Por exemplo, o objeto poderia armazenar apenas as coordenadas retangulares e não as coordenadas polares. Nesse caso, o cálculo das coordenadas polares poderia ser movido para os métodos `valmag()` e `valang()`. Para aplicações em que as conversões são raramente usadas, esse poderia ser um projeto mais eficiente. Além disso, o método `redefinir()` não é realmente necessário. Suponha que `empurrao` é um objeto `Vetor` e que você tem o seguinte código:

```cpp
empurrao.redefinir(100, 300);
```

Você pode obter o mesmo resultado usando um construtor:

```cpp
empurrao = Vetor(100, 300); // cria e atribui um objeto temporário
```

Entretanto, o método `redefinir()` altera o conteúdo de `empurrao` diretamente, enquanto usar o construtor acrescenta as etapas extras de criar um objeto temporário e atribuí-lo a `empurrao`.

Essas decisões de projeto seguem a tradição POO de ter a interface da classe concentrar-se nos essenciais (o modelo abstrato) enquanto oculta os detalhes. Assim, quando você usa a classe `Vetor`, pode pensar nas características gerais de um vetor, como o fato de que eles podem representar deslocamentos e que você pode somar dois vetores. Se você expressa um vetor em notação de componentes ou em notação de magnitude/direção torna-se secundário, porque você pode definir os valores de um vetor e exibi-los em qualquer formato que seja mais conveniente no momento.

## Usando um Membro de Estado (Using a State Member)

A classe `Vetor` armazena tanto as coordenadas retangulares quanto as polares para um vetor. Ela usa um membro chamado `modo` para controlar qual forma o construtor, o método `redefinir()` e a função `operator<<()` sobrecarregada usam, com as enumerações `CART` representando o modo cartesiano (o padrão) e `POL` o modo polar. Tal membro é chamado de *membro de estado* (state member) porque descreve o estado em que um objeto se encontra. Para ver o que isso significa, examine o código do construtor:

```cpp
Vetor::Vetor(double n1, double n2, Modo forma)
{
    modo = forma;
    if (forma == CART)
    {
        x = n1;
        y = n2;
        def_mag();
        def_ang();
    }
    else if (forma == POL)
    {
        mag = n1;
        ang = n2 / Rad_para_grau;
        def_x();
        def_y();
    }
    else
    {
        cout << "Terceiro argumento incorreto para Vetor() -- ";
        cout << "vetor definido como 0\n";
        x = y = mag = ang = 0.0;
        modo = CART;
    }
}
```

Se o terceiro argumento for `CART` ou se for omitido (caso em que o protótipo atribui um valor padrão de `CART`), as entradas são interpretadas como coordenadas cartesianas, enquanto um valor de `POL` faz com que sejam interpretadas como coordenadas polares:

```cpp
Vetor tolice(3.0, 4.0); // define x = 3, y = 4
Vetor besteira(20.0, 30.0, VETOR::Vetor::POL); // define mag = 20, ang = 30
```

O identificador `POL` tem escopo de classe, portanto definições de classe podem usar apenas o nome não qualificado. Mas o nome totalmente qualificado é `VETOR::Vetor::POL` porque `POL` é definido na classe `Vetor`, e `Vetor` é definido no namespace `VETOR`. Observe que o construtor usa os métodos privados `def_mag()` e `def_ang()` para definir os valores de magnitude e ângulo se você fornecer valores x e y, e usa os métodos privados `def_x()` e `def_y()` para definir valores de x e y se você fornecer magnitude e ângulo.

Em seguida, a função `operator<<()` usa o `modo` para determinar como os valores são exibidos:

```cpp
std::ostream & operator<<(std::ostream & os, const Vetor & v)
{
    if (v.modo == Vetor::CART)
        os << "(x,y) = (" << v.x << ", " << v.y << ")";
    else if (v.modo == Vetor::POL)
    {
        os << "(m,a) = (" << v.mag << ", "
           << v.ang * Rad_para_grau << ")";
    }
    else
        os << "Modo do objeto Vetor e invalido";
    return os;
}
```

Como `operator<<()` é uma função amiga e não faz parte do escopo da classe, ela precisa usar `Vetor::CART` em vez de apenas `CART`. Mas ela está no namespace `VETOR`, então não precisa usar o nome totalmente qualificado `VETOR::Vetor::CART`.

> **Representações Múltiplas e Classes**
> Quantidades que têm representações diferentes, mas equivalentes, são comuns. Por exemplo, você pode medir o consumo de gasolina em milhas por galão, como nos Estados Unidos, ou em litros por 100 quilômetros, como na Europa. As classes se prestam bem a englobar diferentes aspectos e representações de uma entidade em um único objeto. Primeiro, você pode armazenar múltiplas representações em um objeto. Segundo, pode escrever as funções da classe de forma que atribuir valores para uma representação atribua automaticamente valores para a(s) outra(s) representação(ões). Manipulando conversões internamente, uma classe pode ajudá-lo a pensar em uma quantidade em termos de sua natureza essencial em vez de em termos de sua representação.

## Sobrecarregando Operadores Aritméticos para a Classe Vetor

Adicionar dois vetores é muito simples quando você usa coordenadas x, y. Você simplesmente soma os dois componentes x para obter o componente x da resposta e soma os dois componentes y para obter o componente y da resposta. O código simples e mais confiável é deixar um construtor fazer o trabalho:

```cpp
Vetor Vetor::operator+(const Vetor & b) const
{
    return Vetor(x + b.x, y + b.y); // retorna o Vetor construído
}
```

Aqui, o código passa ao construtor de `Vetor` os dois novos valores para os componentes x e y. O construtor então cria um novo objeto sem nome usando esses valores, e a função retorna uma cópia desse objeto. Dessa forma, você garante que o novo objeto `Vetor` é criado de acordo com as regras padrão que você estabelece no construtor.

> **Dica**
> Se um método precisa calcular um novo objeto de classe, você deve verificar se pode usar um construtor de classe para fazer o trabalho. Isso não apenas poupa trabalho, mas garante que o novo objeto é construído da maneira correta.

### Multiplicação

Em termos visuais, multiplicar um vetor por um número torna o vetor mais longo ou mais curto por esse fator. Portanto, multiplicar um vetor por 3 produz um vetor com três vezes o comprimento, mas ainda apontando na mesma direção. Em termos polares, você multiplica a magnitude e deixa o ângulo em paz. Em termos retangulares, você multiplica um vetor por um número multiplicando seus componentes x e y separadamente pelo número. E é isso que o operador de multiplicação sobrecarregado faz:

```cpp
Vetor Vetor::operator*(double n) const
{
    return Vetor(n * x, n * y);
}
```

Assim como com a adição sobrecarregada, o código deixa um construtor criar o objeto `Vetor` correto a partir dos novos componentes x e y. Isso lida com multiplicar um valor `Vetor` por um valor `double`. Assim como no exemplo de `Tempo`, você pode usar uma função amiga inline para lidar com `double` vezes `Vetor`:

```cpp
Vetor operator*(double n, const Vetor & a) // função amiga
{
    return a * n; // converte double vezes Vetor para Vetor vezes double
}
```

### Mais Refinamento: Sobrecarregando um Operador Sobrecarregado

No C++ comum, o operador `-` já tem dois significados. Primeiro, quando usado com dois operandos, é o operador de subtração — chamado de operador *binário* porque tem exatamente dois operandos. Segundo, quando usado com um operando, como em `-x`, é um operador de sinal negativo — chamado de operador *unário* porque tem exatamente um operando. Ambas as operações — subtração e inversão de sinal — fazem sentido para vetores também, portanto a classe `Vetor` tem ambas.

Para subtrair o Vetor B do Vetor A, você simplesmente subtrai os componentes, então a definição para sobrecarregar a subtração é bastante similar à da adição:

```cpp
Vetor operator-(const Vetor & b) const; // protótipo
Vetor Vetor::operator-(const Vetor & b) const // definição
{
    return Vetor(x - b.x, y - b.y); // retorna o Vetor construído
}
```

Aqui, é importante obter a ordem correta. Considere a instrução:

```cpp
dif = v1 - v2;
```

Ela é convertida para uma chamada de função membro:

```cpp
dif = v1.operator-(v2);
```

Isso significa que o vetor passado como argumento explícito é subtraído do vetor argumento implícito, portanto você deve usar `x - b.x` e não `b.x - x`.

Em seguida, considere o operador unário de negação, que recebe apenas um operando. Aplicar esse operador a um número regular, como em `-x`, muda o sinal do valor. Portanto, aplicar esse operador a um vetor inverte o sinal de cada componente. Mais precisamente, a função deve retornar um novo vetor que é o inverso do original. Aqui estão o protótipo e a definição para sobrecarregar a negação:

```cpp
Vetor operator-() const;
Vetor Vetor::operator-() const
{
    return Vetor(-x, -y);
}
```

Observe que agora existem duas definições separadas para `operator-()`. Isso é válido porque as duas definições têm assinaturas diferentes. Você pode definir versões tanto binárias quanto unárias do operador `-` porque o C++ fornece versões binárias e unárias desse operador para começar.

> **Nota**
> Como a sobrecarga de operadores é implementada com funções, você pode sobrecarregar o mesmo operador muitas vezes, contanto que cada função operadora tenha uma assinatura distinta e que cada função operadora tenha o mesmo número de operandos que o operador C++ interno correspondente.

### Um Comentário sobre a Implementação

A implementação descrita nas seções anteriores armazena tanto as coordenadas retangulares quanto as polares para um vetor no objeto `Vetor`. No entanto, a interface pública não depende desse fato. A separação de interface da implementação é um dos objetivos da POO. Ela permite que você afine uma implementação sem alterar o código nos programas que usam a classe.

Ambas as implementações têm vantagens e desvantagens. Armazenar os dados significa que o objeto ocupa mais memória e que o código precisa ter cuidado de atualizar ambas as representações retangular e polar cada vez que um objeto `Vetor` é alterado. Mas a consulta de dados é mais rápida. Se um aplicativo frequentemente precisar acessar ambas as representações de um vetor, a implementação usada neste exemplo seria preferível; se os dados polares fossem necessários apenas raramente, a outra implementação seria melhor.

## A Classe Vetor em um Passeio Aleatório (Taking the Vector Class on a Random Walk)

A Listagem 11.15 fornece um programa curto que usa a classe `Vetor` revisada. Ele simula o famoso problema do Passeio do Bêbado, ou como é mais comumente chamado hoje, o problema do Passeio Aleatório. A ideia é colocar alguém em um poste de luz. A pessoa começa a caminhar, mas a direção de cada passo varia aleatoriamente em relação à direção do passo anterior. Uma maneira de formular o problema é: quantos passos leva o caminhante aleatório para percorrer, digamos, 50 pés de distância do poste? Em termos de vetores, isso equivale a somar uma porção de vetores orientados aleatoriamente até que a soma exceda 50 pés.

A Listagem 11.15 permite que você selecione a distância alvo a percorrer e o comprimento do passo do caminhante. Ela mantém um total corrente que representa a posição após cada passo (representado como um vetor) e relata o número de passos necessários para atingir a distância alvo, junto com a localização do caminhante (em ambos os formatos). Como você verá, o progresso do caminhante é bastante ineficiente.

**Listagem 11.15 — randwalk.cpp**

```cpp
// randwalk.cpp — usando a classe Vetor
// compilar com o arquivo vect.cpp
#include <iostream>
#include <cstdlib> // protótipos de rand() e srand()
#include <ctime>   // protótipo de time()
#include "vect.h"
int main()
{
    using namespace std;
    using VETOR::Vetor;
    srand(time(0)); // semente para gerador de números aleatórios
    double direcao;
    Vetor passo;
    Vetor resultado(0.0, 0.0);
    unsigned long passos = 0;
    double alvo;
    double dpasso;
    cout << "Entre com a distância alvo (q para sair): ";
    while (cin >> alvo)
    {
        cout << "Entre com o comprimento do passo: ";
        if (!(cin >> dpasso))
            break;
        while (resultado.valmag() < alvo)
        {
            direcao = rand() % 360;
            passo.redefinir(dpasso, direcao, Vetor::POL);
            resultado = resultado + passo;
            passos++;
        }
        cout << "Apos " << passos << " passos, o sujeito "
             "tem a seguinte localizacao:\n";
        cout << resultado << endl;
        resultado.modo_polar();
        cout << " ou\n" << resultado << endl;
        cout << "Distância media de afastamento por passo = "
             << resultado.valmag() / passos << endl;
        passos = 0;
        resultado.redefinir(0.0, 0.0);
        cout << "Entre com a distância alvo (q para sair): ";
    }
    cout << "Tchau!\n";
    cin.clear();
    while (cin.get() != '\n')
        continue;
    return 0;
}
```

Como o programa tem uma declaração `using` trazendo `Vetor` para escopo, o programa pode usar `Vetor::POL` em vez de `VETOR::Vetor::POL`.

Aqui está uma execução de amostra do programa das Listagens 11.13, 11.14 e 11.15:

```
Entre com a distância alvo (q para sair): 50
Entre com o comprimento do passo: 2
Apos 253 passos, o sujeito tem a seguinte localizacao:
(x,y) = (46.1512, 20.4902)
 ou
(m,a) = (50.495, 23.9402)
Distância media de afastamento por passo = 0.199587
Entre com a distância alvo (q para sair): 50
Entre com o comprimento do passo: 2
Apos 951 passos, o sujeito tem a seguinte localizacao:
(x,y) = (-21.9577, 45.3019)
 ou
(m,a) = (50.3429, 115.8593)
Distância media de afastamento por passo = 0.0529362
Entre com a distância alvo (q para sair): q
Tchau!
```

A natureza aleatória do processo produz variações consideráveis de tentativa para tentativa. Em média, no entanto, reduzir pela metade o tamanho do passo quadruplica o número de passos necessários para cobrir uma dada distância. A teoria das probabilidades sugere que, em média, o número de passos (N) de comprimento s necessários para alcançar uma distância líquida de D é dado pela seguinte equação: N = (D/s)².

## Notas do Programa (Program Notes)

Primeiro, vamos notar como foi fácil usar o namespace `VETOR` na Listagem 11.15. A seguinte declaração `using` coloca o nome da classe `Vetor` em escopo:

```cpp
using VETOR::Vetor;
```

Como todos os métodos da classe `Vetor` têm escopo de classe, importar o nome da classe também torna os métodos de `Vetor` disponíveis, sem a necessidade de quaisquer declarações `using` adicionais.

Em seguida, vamos falar sobre números aleatórios. A biblioteca padrão ANSI C, que também vem com C++, inclui uma função `rand()` que retorna um inteiro aleatório no intervalo de 0 a algum valor dependente da implementação. O programa de passeio aleatório usa o operador módulo para obter um valor de ângulo no intervalo de 0 a 359. A função `rand()` funciona aplicando um algoritmo a um valor de semente inicial para obter um valor aleatório. Esse valor é usado como semente para a próxima chamada de função, e assim por diante. Os números são realmente *pseudoaleatórios* porque 10 chamadas consecutivas normalmente produzem o mesmo conjunto de 10 números aleatórios. Entretanto, a função `srand()` permite substituir o valor de semente padrão e iniciar uma sequência diferente de números aleatórios. Este programa usa o valor de retorno de `time(0)` para definir a semente. A função `time(0)` retorna o tempo de calendário atual, frequentemente implementado como o número de segundos desde alguma data específica. Assim, a seguinte instrução define uma semente diferente a cada vez que você executa o programa, tornando a saída aleatória aparentemente ainda mais aleatória:

```cpp
srand(time(0));
```

O arquivo de cabeçalho `cstdlib` (anteriormente `stdlib.h`) contém os protótipos para `srand()` e `rand()`, enquanto `ctime` (anteriormente `time.h`) contém o protótipo de `time()`. (O C++11 fornece suporte mais extenso a números aleatórios com funções suportadas pelo arquivo de cabeçalho `random`.)

O programa usa o vetor `resultado` para acompanhar o progresso do caminhante. Em cada ciclo do laço interno, o programa define o vetor `passo` para uma nova direção e o soma ao vetor `resultado` atual. Quando a magnitude de `resultado` excede a distância alvo, o laço termina.

Definindo o modo do vetor, o programa exibe a posição final em termos cartesianos e em termos polares.

Aliás, a seguinte instrução tem o efeito de colocar `resultado` no modo `CART`, independentemente dos modos iniciais de `resultado` e `passo`:

```cpp
resultado = resultado + passo;
```

Eis o motivo. Primeiro, a função do operador de adição cria e retorna um novo vetor que contém a soma dos dois argumentos. A função cria esse vetor usando o construtor padrão, que cria vetores no modo `CART`. Portanto, o vetor sendo atribuído a `resultado` está no modo `CART`. Por padrão, a atribuição atribui cada variável membro individualmente, portanto `CART` é atribuído a `resultado.modo`. Se você preferir algum outro comportamento, como `resultado` retendo seu modo original, pode substituir a atribuição padrão definindo um operador de atribuição para a classe. O Capítulo 12 mostra exemplos disso.

---

*Continua em:* [Capítulo 11 — Conversões Automáticas e Resumo](capitulo-11-03-conversoes-resumo.md)

---

> Navegação: [Anterior](capitulo-11-01-sobrecarga-operadores.md) | [Índice](README.md) | [Próximo](capitulo-11-03-conversoes-resumo.md)
