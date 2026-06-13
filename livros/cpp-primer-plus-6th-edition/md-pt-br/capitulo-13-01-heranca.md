# Capítulo 13 — Herança de Classes (Class Inheritance)

> Tradução não oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Navegação: [Anterior](capitulo-12-03-fila-resumo.md) | [Índice](README.md) | [Próximo](capitulo-13-02-virtual-binding.md)

---

Neste capítulo você aprenderá sobre o seguinte:

- Herança (inheritance) como uma relação *é-um* (is-a)
- Como derivar publicamente uma classe de outra
- Acesso protegido (protected access)
- Listas de inicializadores de membros de construtores (constructor member initializer lists)
- Upcasting e downcasting
- Funções membro virtuais (virtual member functions)
- Ligação antecipada (*static binding*, estática) e tardia (*dynamic binding*, dinâmica)
- Classes base abstratas (abstract base classes)
- Funções virtuais puras (pure virtual functions)
- Quando e como usar herança pública

Um dos objetivos principais da programação orientada a objetos é fornecer código reutilizável. Quando você desenvolve um novo projeto, especialmente se ele é grande, é conveniente reutilizar código já testado em vez de reinventá-lo. Empregar código antigo economiza tempo e, como esse código já foi usado e testado, pode ajudar a evitar a introdução de erros no programa. Além disso, quanto menos você precisa se preocupar com detalhes, melhor pode se concentrar na estratégia geral do programa.

Bibliotecas de funções C tradicionais oferecem reutilização por meio de funções predefinidas e pré-compiladas, como `strlen()` e `rand()`, que você pode usar em seus programas. Muitos fornecedores disponibilizam bibliotecas C especializadas com funções além das da biblioteca C padrão. Você pode, por exemplo, comprar bibliotecas de funções para gerenciamento de bancos de dados ou de controle de tela. No entanto, bibliotecas de funções têm uma limitação: a menos que o fornecedor forneça o código-fonte das funções (o que frequentemente não acontece), você não pode estendê-las ou modificá-las para atender às suas necessidades. Em vez disso, você deve adaptar seu programa ao funcionamento da biblioteca. Mesmo que o fornecedor forneça o código-fonte, você corre o risco de modificar acidentalmente o comportamento de parte de uma função ou de alterar as relações entre as funções da biblioteca ao incluir suas mudanças.

As classes C++ elevam a reutilização a um nível superior. Muitos fornecedores oferecem bibliotecas de classes, compostas de declarações e implementações de classes. Como uma classe combina representação de dados com métodos de classe, ela fornece um pacote mais integrado do que uma biblioteca de funções. Uma única classe, por exemplo, pode fornecer todos os recursos para gerenciar uma caixa de diálogo. Frequentemente, as bibliotecas de classes estão disponíveis em código-fonte, o que significa que você pode modificá-las para atender às suas necessidades. Mas o C++ tem um método melhor do que a modificação de código para estender e modificar classes. Esse método, chamado *herança de classes* (class inheritance), permite derivar novas classes de classes existentes, com a classe derivada herdando as propriedades, incluindo os métodos, da classe antiga, chamada *classe base* (base class). Assim como herdar uma fortuna costuma ser mais fácil do que ganhá-la do zero, derivar uma classe por herança geralmente é mais fácil do que projetar uma nova. Aqui estão algumas coisas que você pode fazer com herança:

- Você pode adicionar funcionalidades a uma classe existente. Por exemplo, dada uma classe de array básica, você poderia adicionar operações aritméticas.
- Você pode adicionar dados que uma classe representa. Por exemplo, dada uma classe de string básica, você poderia derivar uma classe que adiciona um membro de dados representando uma cor a ser usada ao exibir a string.
- Você pode modificar o comportamento de um método de classe. Por exemplo, dada uma classe `Passageiro` que representa os serviços fornecidos a um passageiro de avião, você pode derivar uma classe `PassageiroPrimeiraClasse` que fornece um nível superior de serviços.

Naturalmente, você poderia atingir os mesmos objetivos duplicando o código da classe original e modificando-o, mas o mecanismo de herança permite avançar fornecendo apenas as novas funcionalidades. Você nem precisa ter acesso ao código-fonte para derivar uma classe. Portanto, se você comprar uma biblioteca de classes que fornece apenas os arquivos de cabeçalho e o código compilado dos métodos de classe, ainda poderá derivar novas classes baseadas nas classes da biblioteca. De forma análoga, você pode distribuir suas próprias classes para outros, mantendo partes da sua implementação em sigilo, mas ainda dando a seus clientes a opção de adicionar funcionalidades às suas classes.

Herança é um conceito magnífico, e sua implementação básica é bastante simples. Mas gerenciar a herança para que funcione corretamente em todas as situações requer alguns ajustes. Este capítulo examina tanto os aspectos simples quanto os sutis da herança.

---

## Começando com uma Classe Base Simples

Quando uma classe herda de outra, a classe original é chamada de *classe base* (base class), e a classe que herda é chamada de *classe derivada* (derived class). Para ilustrar herança, vamos começar com uma classe base. O Clube Social de Webtown decidiu monitorar seus membros que jogam tênis de mesa. Como programador-chefe do clube, você projetou a classe simples `JogadorTenisMesa` (TableTennisPlayer) definida nas Listagens 13.1 e 13.2.

**Listagem 13.1 — tentenis0.h**

```cpp
// tentenis0.h -- uma classe base para tênis de mesa
#ifndef TENTENIS0_H_
#define TENTENIS0_H_
#include <string>
using std::string;
// classe base simples
class JogadorTenisMesa
{
private:
    string primeiroNome;
    string ultimoNome;
    bool temMesa;
public:
    JogadorTenisMesa (const string & pn = "nenhum",
               const string & un = "nenhum", bool tm = false);
    void Nome() const;
    bool TemMesa() const { return temMesa; };
    void RedefinirMesa(bool v) { temMesa = v; };
};
#endif
```

**Listagem 13.2 — tentenis0.cpp**

```cpp
// tentenis0.cpp -- métodos da classe base simples
#include "tentenis0.h"
#include <iostream>
JogadorTenisMesa::JogadorTenisMesa (const string & pn,
    const string & un, bool tm) : primeiroNome(pn),
        ultimoNome(un), temMesa(tm) {}
void JogadorTenisMesa::Nome() const
{
    std::cout << ultimoNome << ", " << primeiroNome;
}
```

A classe `JogadorTenisMesa` apenas rastreia os nomes dos jogadores e se eles têm mesas. Há alguns pontos a notar. Primeiro, a classe usa a classe `string` padrão para armazenar os nomes. Isso é mais conveniente, flexível e seguro do que usar um array de caracteres. É também bem mais profissional do que a classe `String` do Capítulo 12, "Classes e Alocação Dinâmica de Memória". Segundo, o construtor usa a sintaxe de lista de inicializadores de membros (member initializer list) introduzida no Capítulo 12. Você também poderia fazer assim:

```cpp
JogadorTenisMesa::JogadorTenisMesa (const string & pn,
                const string & un, bool tm)
{
    primeiroNome = pn;
    ultimoNome = un;
    temMesa = tm;
}
```

Porém essa abordagem tem o efeito de primeiro chamar o construtor padrão de `string` para `primeiroNome` e depois invocar o operador de atribuição de `string` para redefinir `primeiroNome` como `pn`. Mas a sintaxe de lista de inicializadores de membros economiza um passo ao usar apenas o construtor de cópia de `string` para inicializar `primeiroNome` como `pn`.

A Listagem 13.3 mostra essa modesta classe em ação.

**Listagem 13.3 — usartentenis0.cpp**

```cpp
// usartentenis0.cpp -- usando uma classe base
#include <iostream>
#include "tentenis0.h"

int main ( void )
{
    using std::cout;
    JogadorTenisMesa jogador1("Carlos", "Blizzard", true);
    JogadorTenisMesa jogador2("Tara", "Boomdea", false);
    jogador1.Nome();
    if (jogador1.TemMesa())
        cout << ": tem uma mesa.\n";
    else
        cout << ": não tem uma mesa.\n";
    jogador2.Nome();
    if (jogador2.TemMesa())
        cout << ": tem uma mesa";
    else
        cout << ": não tem uma mesa.\n";
    return 0;
}
```

E aqui está a saída do programa das Listagens 13.1, 13.2 e 13.3:

```
Blizzard, Carlos: tem uma mesa.
Boomdea, Tara: não tem uma mesa.
```

Observe que o programa usa construtores com argumentos de string no estilo C:

```cpp
JogadorTenisMesa jogador1("Carlos", "Blizzard", true);
JogadorTenisMesa jogador2("Tara", "Boomdea", false);
```

Mas os parâmetros formais para o construtor foram declarados como tipo `const string &`. Isso é uma incompatibilidade de tipos, mas a classe `string`, muito parecida com a classe `String` do Capítulo 12, tem um construtor com um parâmetro `const char *`, e esse construtor é usado automaticamente para criar um objeto `string` inicializado pela string no estilo C. Em resumo, você pode usar tanto um objeto `string` quanto uma string no estilo C como argumento para o construtor de `JogadorTenisMesa`. O primeiro invoca um construtor de `string` com um parâmetro `const string &`, e o segundo invoca um construtor de `string` com um parâmetro `const char *`.

---

## Derivando uma Classe

Alguns membros do Clube Social de Webtown participaram de torneios locais de tênis de mesa e exigem uma classe que inclua as pontuações que eles ganharam em suas partidas. Em vez de começar do zero, você pode derivar uma classe da classe `JogadorTenisMesa`. O primeiro passo é fazer com que a declaração da classe `JogadorAvaliado` (RatedPlayer) indique que ela deriva da classe `JogadorTenisMesa`:

```cpp
// JogadorAvaliado deriva da classe base JogadorTenisMesa
class JogadorAvaliado : public JogadorTenisMesa
{
...
};
```

Os dois-pontos indicam que a classe `JogadorAvaliado` é baseada na classe `JogadorTenisMesa`. Esse cabeçalho específico indica que `JogadorTenisMesa` é uma *classe base pública* (public base class); isso é chamado de *derivação pública* (public derivation). Um objeto de uma classe derivada incorpora um objeto da classe base. Com derivação pública, os membros públicos da classe base tornam-se membros públicos da classe derivada. As partes privadas de uma classe base tornam-se parte da classe derivada, mas podem ser acessadas apenas por meio dos métodos públicos e protegidos da classe base. (Chegaremos aos membros protegidos em breve.)

O que isso proporciona? Se você declarar um objeto `JogadorAvaliado`, ele terá as seguintes propriedades especiais:

- Um objeto do tipo derivado armazena dentro de si os membros de dados do tipo base. (A classe derivada herda a implementação da classe base.)
- Um objeto do tipo derivado pode usar os métodos do tipo base. (A classe derivada herda a interface da classe base.)

Assim, um objeto `JogadorAvaliado` pode armazenar o primeiro nome e o sobrenome de cada jogador e se o jogador tem uma mesa. Também um objeto `JogadorAvaliado` pode usar os métodos `Nome()`, `TemMesa()` e `RedefinirMesa()` da classe `JogadorTenisMesa`.

O que precisa ser adicionado a essas funcionalidades herdadas?

- Uma classe derivada precisa de seus próprios construtores.
- Uma classe derivada pode adicionar membros de dados adicionais e funções membro conforme necessário.

Neste caso específico, a classe precisa de mais um membro de dados para armazenar o valor da avaliação. Ela também deve ter um método para recuperar a avaliação e um método para redefini-la. Assim, a declaração da classe pode ser assim:

```cpp
// classe derivada simples
class JogadorAvaliado : public JogadorTenisMesa
{
private:
    unsigned int avaliacao; // adiciona um membro de dados
public:
    JogadorAvaliado (unsigned int av = 0, const string & pn = "nenhum",
            const string & un = "nenhum", bool tm = false);
    JogadorAvaliado(unsigned int av, const JogadorTenisMesa & jtp);
    unsigned int Avaliacao() const { return avaliacao; } // adiciona um método
    void RedefinirAvaliacao (unsigned int av) {avaliacao = av;} // adiciona um método
};
```

---

## Construtores: Considerações de Acesso

Uma classe derivada não tem acesso direto aos membros privados da classe base; ela deve trabalhar por meio dos métodos da classe base. Por exemplo, os construtores de `JogadorAvaliado` não podem definir diretamente os membros herdados (`primeiroNome`, `ultimoNome` e `temMesa`). Em vez disso, eles devem usar métodos públicos da classe base para acessar os membros privados da classe base. Em particular, os construtores da classe derivada devem usar os construtores da classe base.

Quando um programa constrói um objeto de classe derivada, ele primeiro constrói o objeto da classe base. Conceitualmente, isso significa que o objeto da classe base deve ser construído antes que o programa entre no corpo do construtor da classe derivada. O C++ usa a sintaxe de lista de inicializadores de membros para realizar isso. Aqui, por exemplo, está o código para o primeiro construtor de `JogadorAvaliado`:

```cpp
JogadorAvaliado::JogadorAvaliado(unsigned int av, const string & pn,
    const string & un, bool tm) : JogadorTenisMesa(pn, un, tm)
{
    avaliacao = av;
}
```

A parte seguinte é a lista de inicializadores de membros:

```cpp
: JogadorTenisMesa(pn, un, tm)
```

É código executável, e ele chama o construtor de `JogadorTenisMesa`. Suponha, por exemplo, que um programa tenha a seguinte declaração:

```cpp
JogadorAvaliado jAvaliado1(1140, "Mallory", "Duck", true);
```

O construtor de `JogadorAvaliado` atribui os argumentos reais `"Mallory"`, `"Duck"` e `true` aos parâmetros formais `pn`, `un` e `tm`. Ele então passa esses parâmetros como argumentos reais para o construtor de `JogadorTenisMesa`. Esse construtor, por sua vez, cria o objeto `JogadorTenisMesa` embutido e armazena os dados `"Mallory"`, `"Duck"` e `true` nele. Depois o programa entra no corpo do construtor de `JogadorAvaliado`, completa a construção do objeto `JogadorAvaliado` e atribui o valor do parâmetro `av` (isto é, 1140) ao membro `avaliacao`.

E se você omitir a lista de inicializadores de membros?

```cpp
JogadorAvaliado::JogadorAvaliado(unsigned int av, const string & pn,
    const string & un, bool tm) // e se não houver lista de inicialização?
{
    avaliacao = av;
}
```

O objeto da classe base deve ser criado primeiro, portanto, se você omitir uma chamada de construtor da classe base, o programa usa o construtor padrão da classe base. Portanto, o código anterior é equivalente ao seguinte:

```cpp
JogadorAvaliado::JogadorAvaliado(unsigned int av, const string & pn,
    const string & un, bool tm) // : JogadorTenisMesa()
{
    avaliacao = av;
}
```

A menos que você queira que o construtor padrão seja usado, você deve fornecer explicitamente a chamada correta ao construtor da classe base.

Agora vejamos o código para o segundo construtor:

```cpp
JogadorAvaliado::JogadorAvaliado(unsigned int av, const JogadorTenisMesa & jtp)
    : JogadorTenisMesa(jtp)
{
    avaliacao = av;
}
```

Novamente, as informações de `JogadorTenisMesa` são passadas para um construtor de `JogadorTenisMesa`:

```cpp
JogadorTenisMesa(jtp)
```

Como `jtp` é do tipo `const JogadorTenisMesa &`, essa chamada invoca o construtor de cópia da classe base. A classe base não definiu um construtor de cópia, mas lembre-se do Capítulo 12 que o compilador gera automaticamente um construtor de cópia se necessário e você não tiver definido um. Nesse caso, o construtor de cópia implícito, que faz a cópia membro a membro, está correto porque a classe não usa alocação dinâmica de memória diretamente. (Os membros `string` usam alocação dinâmica de memória, mas, lembre-se, a cópia membro a membro usará os construtores de cópia da classe `string` para copiar os membros `string`.)

Você pode, se quiser, usar também a sintaxe de lista de inicializadores de membros para os membros da classe derivada. Nesse caso, você usa o nome do membro em vez do nome da classe na lista. Assim, o segundo construtor também pode ser escrito da seguinte forma:

```cpp
// versão alternativa
JogadorAvaliado::JogadorAvaliado(unsigned int av, const JogadorTenisMesa & jtp)
    : JogadorTenisMesa(jtp), avaliacao(av)
{
}
```

Estes são os pontos-chave sobre os construtores de classes derivadas:

- O objeto da classe base é construído primeiro.
- O construtor da classe derivada deve passar as informações da classe base para um construtor da classe base por meio de uma lista de inicializadores de membros.
- O construtor da classe derivada deve inicializar os membros de dados que foram adicionados à classe derivada.

Este exemplo não fornece destrutores explícitos, portanto, os destrutores implícitos são usados. A destruição de um objeto ocorre na ordem oposta à usada para construí-lo. Ou seja, o corpo do destrutor da classe derivada é executado primeiro, e então o destrutor da classe base é chamado automaticamente.

> **Nota**
> Ao criar um objeto de uma classe derivada, o programa primeiro chama o construtor da classe base e depois chama o construtor da classe derivada. O construtor da classe base é responsável por inicializar os membros de dados herdados. O construtor da classe derivada é responsável por inicializar quaisquer membros de dados adicionados. Um construtor de classe derivada sempre chama um construtor de classe base. Você pode usar a sintaxe de lista de inicializadores para indicar qual construtor da classe base usar. Caso contrário, o construtor padrão da classe base é usado.
>
> Quando um objeto de uma classe derivada expira, o programa primeiro chama o destrutor da classe derivada e depois chama o destrutor da classe base.

> **Listas de Inicializadores de Membros**
> Um construtor para uma classe derivada pode usar o mecanismo de lista de inicializadores para passar valores ao construtor da classe base. Considere este exemplo:
>
> ```cpp
> derivada::derivada(tipo1 x, tipo2 y) : base(x, y) // lista de inicializadores
> {
>     ...
> }
> ```
>
> Aqui, `derivada` é a classe derivada, `base` é a classe base, e `x` e `y` são variáveis usadas pelo construtor da classe base. Se, digamos, o construtor da classe derivada receber os argumentos `10` e `12`, esse mecanismo então passa `10` e `12` para o construtor da classe base definido para receber argumentos desses tipos. Exceto no caso de classes base virtuais (veja o Capítulo 14, "Reutilizando Código em C++"), uma classe pode passar valores somente para sua classe base imediata. No entanto, essa classe pode usar o mesmo mecanismo para passar informações de volta para sua classe base imediata, e assim por diante. Se você não fornecer um construtor da classe base em uma lista de inicializadores de membros, o programa usa o construtor padrão da classe base. A lista de inicializadores de membros pode ser usada apenas em construtores.

---

## Usando uma Classe Derivada

Para usar uma classe derivada, um programa precisa ter acesso às declarações da classe base. A Listagem 13.4 coloca as duas declarações de classe no mesmo arquivo de cabeçalho. Você poderia dar a cada classe seu próprio arquivo de cabeçalho, mas como as duas classes são relacionadas, faz mais sentido do ponto de vista organizacional mantê-las juntas.

**Listagem 13.4 — tentenis1.h**

```cpp
// tentenis1.h -- uma classe base para tênis de mesa
#ifndef TENTENIS1_H_
#define TENTENIS1_H_
#include <string>
using std::string;
// classe base simples
class JogadorTenisMesa
{
private:
    string primeiroNome;
    string ultimoNome;
    bool temMesa;
public:
    JogadorTenisMesa (const string & pn = "nenhum",
               const string & un = "nenhum", bool tm = false);
    void Nome() const;
    bool TemMesa() const { return temMesa; };
    void RedefinirMesa(bool v) { temMesa = v; };
};
// classe derivada simples
class JogadorAvaliado : public JogadorTenisMesa
{
private:
    unsigned int avaliacao;
public:
    JogadorAvaliado (unsigned int av = 0, const string & pn = "nenhum",
            const string & un = "nenhum", bool tm = false);
    JogadorAvaliado(unsigned int av, const JogadorTenisMesa & jtp);
    unsigned int Avaliacao() const { return avaliacao; }
    void RedefinirAvaliacao (unsigned int av) {avaliacao = av;}
};
#endif
```

A Listagem 13.5 fornece as definições dos métodos para ambas as classes. Novamente, você poderia usar arquivos separados, mas é mais simples manter as definições juntas.

**Listagem 13.5 — tentenis1.cpp**

```cpp
// tentenis1.cpp -- métodos das classes base e derivada
#include "tentenis1.h"
#include <iostream>
JogadorTenisMesa::JogadorTenisMesa (const string & pn,
    const string & un, bool tm) : primeiroNome(pn),
        ultimoNome(un), temMesa(tm) {}
void JogadorTenisMesa::Nome() const
{
    std::cout << ultimoNome << ", " << primeiroNome;
}
// Métodos de JogadorAvaliado
JogadorAvaliado::JogadorAvaliado(unsigned int av, const string & pn,
    const string & un, bool tm) : JogadorTenisMesa(pn, un, tm)
{
    avaliacao = av;
}
JogadorAvaliado::JogadorAvaliado(unsigned int av, const JogadorTenisMesa & jtp)
    : JogadorTenisMesa(jtp), avaliacao(av)
{
}
```

A Listagem 13.6 cria objetos de ambas as classes `JogadorTenisMesa` e `JogadorAvaliado`. Observe que objetos de ambas as classes podem usar os métodos `Nome()` e `TemMesa()` da classe `JogadorTenisMesa`.

**Listagem 13.6 — usartentenis1.cpp**

```cpp
// usartentenis1.cpp -- usando classe base e derivada
#include <iostream>
#include "tentenis1.h"
int main ( void )
{
    using std::cout;
    using std::endl;
    JogadorTenisMesa jogador1("Tara", "Boomdea", false);
    JogadorAvaliado jAvaliado1(1140, "Mallory", "Duck", true);
    jAvaliado1.Nome(); // objeto derivado usa método da base
    if (jAvaliado1.TemMesa())
        cout << ": tem uma mesa.\n";
    else
        cout << ": não tem uma mesa.\n";
    jogador1.Nome(); // objeto base usa método da base
    if (jogador1.TemMesa())
        cout << ": tem uma mesa";
    else
        cout << ": não tem uma mesa.\n";
    cout << "Nome: ";
    jAvaliado1.Nome();
    cout << "; Avaliação: " << jAvaliado1.Avaliacao() << endl;
    // inicializa JogadorAvaliado usando objeto JogadorTenisMesa
    JogadorAvaliado jAvaliado2(1212, jogador1);
    cout << "Nome: ";
    jAvaliado2.Nome();
    cout << "; Avaliação: " << jAvaliado2.Avaliacao() << endl;
    return 0;
}
```

Aqui está a saída do programa das Listagens 13.4, 13.5 e 13.6:

```
Duck, Mallory: tem uma mesa.
Boomdea, Tara: não tem uma mesa.
Nome: Duck, Mallory; Avaliação: 1140
Nome: Boomdea, Tara; Avaliação: 1212
```

---

## Relacionamentos Especiais Entre Classes Derivadas e Base

Uma classe derivada tem alguns relacionamentos especiais com a classe base. Um deles, que você acabou de ver, é que um objeto de classe derivada pode usar os métodos da classe base, desde que os métodos não sejam privados:

```cpp
JogadorAvaliado jAvaliado1(1140, "Mallory", "Duck", true);
jAvaliado1.Nome(); // objeto derivado usa método da base
```

Outros dois relacionamentos importantes são que um ponteiro para a classe base pode apontar para um objeto da classe derivada sem uma conversão de tipo explícita (*type cast*) e que uma referência para a classe base pode se referir a um objeto da classe derivada sem uma conversão de tipo explícita:

```cpp
JogadorAvaliado jAvaliado1(1140, "Mallory", "Duck", true);
JogadorTenisMesa & rt = jAvaliado1;
JogadorTenisMesa * pt = &jAvaliado1;
rt.Nome();  // invoca Nome() com referência
pt->Nome(); // invoca Nome() com ponteiro
```

No entanto, um ponteiro ou referência para a classe base pode invocar apenas métodos da classe base, portanto você não poderia usar `rt` ou `pt` para invocar, digamos, o método `RedefinirAvaliacao()` da classe derivada.

Normalmente, o C++ exige que os tipos de referências e ponteiros correspondam aos tipos atribuídos, mas essa regra é relaxada para herança. No entanto, o relaxamento da regra é apenas em uma direção. Você não pode atribuir objetos e endereços da classe base a referências e ponteiros da classe derivada:

```cpp
JogadorTenisMesa jogador("Betsy", "Bloop", true);
JogadorAvaliado & rr = jogador; // NÃO PERMITIDO
JogadorAvaliado * pr = &jogador; // NÃO PERMITIDO
```

Ambos os conjuntos de regras fazem sentido. Por exemplo, considere as implicações de ter uma referência para a classe base se referir a um objeto derivado. Nesse caso, você pode usar a referência da classe base para invocar métodos da classe base para o objeto da classe derivada. Como a classe derivada herda os métodos e membros de dados da classe base, isso não causa problemas. Agora considere o que aconteceria se você pudesse atribuir um objeto da classe base a uma referência da classe derivada. A referência da classe derivada seria capaz de invocar métodos da classe derivada para o objeto base, e isso poderia causar problemas. Por exemplo, aplicar o método `JogadorAvaliado::Avaliacao()` a um objeto `JogadorTenisMesa` não faz sentido porque o objeto `JogadorTenisMesa` não tem um membro `avaliacao`.

O fato de que referências e ponteiros para a classe base podem se referir a objetos de classe derivada tem algumas consequências interessantes. Uma delas é que funções definidas com argumentos de referência ou ponteiro para a classe base podem ser usadas tanto com objetos da classe base quanto com objetos da classe derivada. Por exemplo, considere esta função:

```cpp
void Mostrar(const JogadorTenisMesa & rt)
{
    using std::cout;
    cout << "Nome: ";
    rt.Nome();
    cout << "\nMesa: ";
    if (rt.TemMesa())
        cout << "sim\n";
    else
        cout << "não\n";
}
```

O parâmetro formal `rt` é uma referência a uma classe base, portanto pode se referir a um objeto da classe base ou a um objeto da classe derivada. Assim, você pode usar `Mostrar()` com um argumento `JogadorTenisMesa` ou um argumento `JogadorAvaliado`:

```cpp
JogadorTenisMesa jogador1("Tara", "Boomdea", false);
JogadorAvaliado jAvaliado1(1140, "Mallory", "Duck", true);
Mostrar(jogador1);   // funciona com argumento JogadorTenisMesa
Mostrar(jAvaliado1); // funciona com argumento JogadorAvaliado
```

Um relacionamento similar valeria para uma função com um parâmetro formal ponteiro para a classe base; ela poderia ser usada com o endereço de um objeto da classe base ou o endereço de um objeto da classe derivada como argumento real:

```cpp
void MostrarP(const JogadorTenisMesa * pt); // função com parâmetro ponteiro
...
JogadorTenisMesa jogador1("Tara", "Boomdea", false);
JogadorAvaliado jAvaliado1(1140, "Mallory", "Duck", true);
MostrarP(&jogador1);   // funciona com argumento JogadorTenisMesa *
MostrarP(&jAvaliado1); // funciona com argumento JogadorAvaliado *
```

A compatibilidade de referências também permite inicializar um objeto da classe base a partir de um objeto da classe derivada, embora de forma um pouco indireta. Suponha que você tenha este código:

```cpp
JogadorAvaliado olaf1(1840, "Olaf", "Loaf", true);
JogadorTenisMesa olaf2(olaf1);
```

A correspondência exata para inicializar `olaf2` seria um construtor com este protótipo:

```cpp
JogadorTenisMesa(const JogadorAvaliado &); // não existe
```

As definições de classe não incluem esse construtor, mas existe o construtor de cópia implícito:

```cpp
// construtor de cópia implícito
JogadorTenisMesa(const JogadorTenisMesa &);
```

O parâmetro formal é uma referência ao tipo base, então ele pode se referir a um tipo derivado. Assim, a tentativa de inicializar `olaf2` a partir de `olaf1` usa esse construtor, que copia os membros `primeiroNome`, `ultimoNome` e `temMesa`. Em outras palavras, ele inicializa `olaf2` para o objeto `JogadorTenisMesa` embutido no objeto `JogadorAvaliado` `olaf1`.

Da mesma forma, você pode atribuir um objeto da classe derivada a um objeto da classe base:

```cpp
JogadorAvaliado olaf1(1840, "Olaf", "Loaf", true);
JogadorTenisMesa vencedor;
vencedor = olaf1; // atribui derivado ao base
```

Nesse caso, o programa usa o operador de atribuição sobrecarregado implícito:

```cpp
JogadorTenisMesa & operator=(const JogadorTenisMesa &) const;
```

Novamente, uma referência da classe base se refere a um objeto da classe derivada, e apenas a parte da classe base de `olaf1` é copiada para `vencedor`.

---

## Herança: Uma Relação *É-Um*

O relacionamento especial entre uma classe derivada e uma classe base baseia-se em um modelo subjacente para a herança C++. Na verdade, o C++ tem três variedades de herança: pública, protegida e privada. A herança pública é a forma mais comum e modela uma relação *é-um* (is-a). Isso é um atalho para dizer que um objeto de uma classe derivada também deve ser um objeto da classe base. Tudo o que você faz com um objeto da classe base, você deve ser capaz de fazer com um objeto da classe derivada. Suponha, por exemplo, que você tenha uma classe `Fruta`. Ela poderia armazenar, digamos, o peso e o conteúdo calórico de uma fruta. Como uma banana é um tipo particular de fruta, você poderia derivar uma classe `Banana` da classe `Fruta`. A nova classe herdaria todos os membros de dados da classe original, então um objeto `Banana` teria membros representando o peso e o conteúdo calórico de uma banana. A nova classe `Banana` também pode adicionar membros que se aplicam particularmente a bananas e não a frutas em geral, como o Índice de Casca do Instituto Banana. Como a classe derivada pode adicionar funcionalidades, provavelmente é mais preciso descrever o relacionamento como uma relação *é-um-tipo-de*, mas *é-um* é o termo usual.

Para esclarecer as relações *é-um*, vejamos alguns exemplos que não se encaixam nesse modelo. A herança pública não modela uma relação *tem-um* (has-a). Um almoço, por exemplo, pode conter uma fruta. Mas um almoço, em geral, não é uma fruta. Portanto, você não deve derivar uma classe `Almoco` da classe `Fruta` na tentativa de colocar frutas em um almoço. A maneira correta de tratar a questão de colocar fruta em um almoço é considerar o assunto como uma relação *tem-um*: um almoço tem uma fruta. Como você verá no Capítulo 14, isso é melhor modelado incluindo um objeto `Fruta` como membro de dados de uma classe `Almoco`.

A herança pública não modela uma relação *é-parecido-com* (is-like-a) — isto é, ela não faz símiles. Frequentemente é dito que advogados são como tubarões. Mas não é literalmente verdade que um advogado é um tubarão. Por exemplo, tubarões podem viver debaixo d'água. Portanto, você não deve derivar uma classe `Advogado` de uma classe `Tubarao`. A herança pode adicionar propriedades a uma classe base; ela não remove propriedades de uma classe base. Em alguns casos, características compartilhadas podem ser tratadas projetando uma classe que englobe essas características e usando então essa classe, em uma relação *é-um* ou *tem-um*, para definir as classes relacionadas.

A herança pública não modela uma relação *é-implementado-como* (is-implemented-as-a). Por exemplo, você poderia implementar uma pilha usando um array. No entanto, não seria adequado derivar uma classe `Pilha` de uma classe `Array`. Uma pilha não é um array. Por exemplo, indexação de array não é uma propriedade de pilha. Além disso, uma pilha poderia ser implementada de outra maneira, como por uma lista encadeada. Uma abordagem adequada seria ocultar a implementação do array dando à pilha um membro privado do tipo `Array`.

A herança pública não modela uma relação *usa-um* (uses-a). Por exemplo, um computador pode usar uma impressora a laser, mas não faz sentido derivar uma classe `Impressora` de uma classe `Computador`, ou vice-versa. Você poderia, porém, criar funções ou classes amigas para lidar com a comunicação entre objetos `Impressora` e objetos `Computador`.

Nada na linguagem C++ impede que você use herança pública para modelar relações *tem-um*, *é-implementado-como* ou *usa-um*. No entanto, fazer isso geralmente leva a problemas de programação. Portanto, vamos nos ater às relações *é-um*.

---

## Herança Pública Polimórfica

O exemplo de herança de `JogadorAvaliado` é um caso simples. Objetos da classe derivada usam os métodos da classe base sem alteração. Mas você pode encontrar situações em que deseja que um método se comporte de forma diferente para a classe derivada do que para a classe base. Ou seja, o comportamento de um método específico pode depender do objeto que o invoca. Esse comportamento mais sofisticado é chamado de *polimórfico* ("tendo muitas formas") porque você pode ter vários comportamentos para um método, dependendo do contexto. Existem dois mecanismos-chave para implementar herança pública polimórfica:

- Redefinir métodos da classe base em uma classe derivada
- Usar métodos virtuais

É hora de outro exemplo. Você alavancou sua experiência no Clube Social de Webtown para se tornar o programador-chefe do Banco Nacional Pontoon. A primeira coisa que o banco pede para você fazer é desenvolver duas classes. Uma classe representará seu plano básico de conta corrente, a *Conta Latão* (Brass Account), e a segunda classe representará a Conta Latão Plus (Brass Plus), que adiciona um recurso de proteção contra cheque especial. Ou seja, se um usuário escrever um cheque maior (mas não muito maior) do que seu saldo, o banco cobrirá o cheque, cobrando do usuário pelo excesso e adicionando uma sobretaxa. Você pode caracterizar as duas contas em termos de dados a serem armazenados e das operações a serem permitidas.

Primeiro, aqui estão as informações para o plano de conta corrente Latão:

- Nome do cliente
- Número de conta
- Saldo atual

E aqui estão as operações a serem representadas:

- Criar uma conta
- Depositar dinheiro na conta
- Sacar dinheiro da conta
- Exibir as informações da conta

Para o plano de conta corrente Latão Plus, o Banco Nacional Pontoon quer todas as funcionalidades da Conta Latão, além dos seguintes itens adicionais de informação:

- Um limite máximo para a proteção contra cheque especial
- Uma taxa de juros cobrada sobre os empréstimos de cheque especial
- O valor do cheque especial atualmente devido ao banco

Nenhuma operação adicional é necessária, mas duas operações precisam ser implementadas de forma diferente:

- A operação de sacar dinheiro deve incorporar proteção contra cheque especial para a Conta Latão Plus
- A operação de exibição deve mostrar as informações adicionais exigidas pela Conta Latão Plus

Suponha que você chame uma classe de `Latao` e a segunda de `LataoPlus`. Você deve derivar `LataoPlus` publicamente de `Latao`? Para responder a essa pergunta, primeiro responda a outra: a classe `LataoPlus` passa no teste *é-um*? Claro. Tudo que é verdadeiro para um objeto `Latao` será verdadeiro para um objeto `LataoPlus`. Ambos armazenam o nome do cliente, um número de conta e um saldo. Com ambos, você pode fazer depósitos, saques e exibir informações da conta. Note que a relação *é-um* não é, em geral, simétrica. Uma fruta, em geral, não é uma banana; da mesma forma, um objeto `Latao` não terá todas as capacidades de um objeto `LataoPlus`.

### Desenvolvendo as Classes Latao e LataoPlus

As informações sobre a classe `Latao` são bastante diretas, mas o banco não forneceu detalhes suficientes sobre como o sistema de cheque especial funciona. Em resposta ao seu pedido de mais informações, o amigável representante do Banco Nacional Pontoon lhe diz o seguinte:

- Uma Conta LataoPlus limita o quanto o banco lhe emprestará para cobrir cheques especiais. O valor padrão é R$500, mas alguns clientes podem começar com um limite diferente.
- O banco pode alterar o limite de cheque especial de um cliente.
- Uma Conta Latão Plus cobra juros sobre o empréstimo. A taxa padrão é 11,125%, mas alguns clientes podem começar com uma taxa diferente.
- O banco pode alterar a taxa de juros de um cliente.
- A conta rastreia o quanto o cliente deve ao banco (empréstimos de cheque especial mais juros). O usuário não pode pagar esse valor por meio de um depósito regular ou por uma transferência de outra conta. Em vez disso, ele deve pagar em dinheiro a um agente especial do banco, que, se necessário, procurará o cliente. Quando a dívida for paga, a conta pode redefinir o valor devido para 0.

A última funcionalidade é uma maneira incomum de um banco fazer negócios, mas tem o feliz efeito colateral de manter o problema de programação mais simples.

Essa lista sugere que a nova classe precisa de construtores que forneçam informações de conta e que incluam um limite de dívida com um valor padrão de R$500 e uma taxa de juros com um valor padrão de 10%. Também deve haver métodos para redefinir o limite de dívida, a taxa de juros e a dívida atual. Essas são as coisas a serem adicionadas à classe `Latao`, e elas serão declaradas na declaração da classe `LataoPlus`.

As informações sobre as duas classes sugerem declarações de classe como as da Listagem 13.7.

**Listagem 13.7 — latao.h**

```cpp
// latao.h -- classes de contas bancárias
#ifndef LATAO_H_
#define LATAO_H_
#include <string>
// Classe Conta Latão
class Latao
{
private:
    std::string nomeCompleto;
    long numConta;
    double saldo;
public:
    Latao(const std::string & s = "Ninguém", long nc = -1,
           double sal = 0.0);
    void Depositar(double val);
    virtual void Sacar(double val);
    double Saldo() const;
    virtual void VerConta() const;
    virtual ~Latao() {}
};
// Classe Conta Latão Plus
class LataoPlus : public Latao
{
private:
    double limiteEmprestimo;
    double taxa;
    double deveAoBanco;
public:
    LataoPlus(const std::string & s = "Ninguém", long nc = -1,
         double sal = 0.0, double le = 500,
         double t = 0.11125);
    LataoPlus(const Latao & la, double le = 500,
                     double t = 0.11125);
    virtual void VerConta() const;
    virtual void Sacar(double val);
    void RedefinirLimite(double m) { limiteEmprestimo = m; }
    void RedefinirTaxa(double t) { taxa = t; };
    void RedefinirDebito() { deveAoBanco = 0; }
};
#endif
```

Há vários pontos a observar na Listagem 13.7:

- A classe `LataoPlus` adiciona três novos membros de dados privados e três novas funções membro públicas à classe `Latao`.
- Tanto a classe `Latao` quanto a classe `LataoPlus` declaram os métodos `VerConta()` e `Sacar()`; esses são os métodos que se comportarão de forma diferente para um objeto `LataoPlus` do que para um objeto `Latao`.
- A classe `Latao` usa a nova palavra-chave `virtual` na declaração de `VerConta()` e `Sacar()`. Esses métodos agora são chamados de *métodos virtuais* (virtual methods).
- A classe `Latao` também declara um destrutor virtual, mesmo que ele não faça nada.

O primeiro ponto da lista não é novidade. A classe `JogadorAvaliado` fez algo semelhante quando adicionou um novo membro de dados e dois novos métodos à classe `JogadorTenisMesa`.

O segundo ponto da lista mostra como as declarações especificam que os métodos devem se comportar de forma diferente para a classe derivada. Os dois protótipos `VerConta()` indicam que haverá duas definições de método separadas. O nome qualificado para a versão da classe base é `Latao::VerConta()`, e o nome qualificado para a versão da classe derivada é `LataoPlus::VerConta()`. Um programa usará o tipo do objeto para determinar qual versão usar:

```cpp
Latao dom("Dominic Banqueiro", 11224, 4183.45);
LataoPlus dot("Dorothy Banqueira", 12118, 2592.00);
dom.VerConta(); // usa Latao::VerConta()
dot.VerConta(); // usa LataoPlus::VerConta()
```

Da mesma forma, haverá duas versões de `Sacar()`: uma usada por objetos `Latao` e outra usada por objetos `LataoPlus`. Métodos que se comportam da mesma forma para ambas as classes, como `Depositar()` e `Saldo()`, são declarados apenas na classe base.

O terceiro ponto (o uso de `virtual`) é mais envolvente do que os dois primeiros pontos. Ele determina qual método é usado se o método é invocado por uma referência ou ponteiro em vez de por um objeto. Se você não usar a palavra-chave `virtual`, o programa escolhe um método com base no tipo da referência ou no tipo do ponteiro. Se você usar a palavra-chave `virtual`, o programa escolhe um método com base no tipo do objeto ao qual a referência ou ponteiro se refere. Veja como um programa se comporta se `VerConta()` não é virtual:

```cpp
// comportamento com VerConta() não virtual
// método escolhido de acordo com o tipo da referência
Latao dom("Dominic Banqueiro", 11224, 4183.45);
LataoPlus dot("Dorothy Banqueira", 12118, 2592.00);
Latao & b1_ref = dom;
Latao & b2_ref = dot;
b1_ref.VerConta(); // usa Latao::VerConta()
b2_ref.VerConta(); // usa Latao::VerConta()
```

As variáveis de referência são do tipo `Latao`, então `Latao::VerConta()` é escolhido. Usar ponteiros para `Latao` em vez de referências resulta em comportamento similar.

Em contraste, veja o comportamento se `VerConta()` é virtual:

```cpp
// comportamento com VerConta() virtual
// método escolhido de acordo com o tipo do objeto
Latao dom("Dominic Banqueiro", 11224, 4183.45);
LataoPlus dot("Dorothy Banqueira", 12118, 2592.00);
Latao & b1_ref = dom;
Latao & b2_ref = dot;
b1_ref.VerConta(); // usa Latao::VerConta()
b2_ref.VerConta(); // usa LataoPlus::VerConta()
```

Nesse caso, ambas as referências são do tipo `Latao`, mas `b2_ref` se refere a um objeto `LataoPlus`, portanto `LataoPlus::VerConta()` é usado para ela. Usar ponteiros para `Latao` em vez de referências resulta em comportamento similar.

Acontece, como você verá em breve, que esse comportamento de funções virtuais é muito útil. Portanto, é prática comum declarar como virtual na classe base aqueles métodos que podem ser redefinidos em uma classe derivada. Quando um método é declarado virtual em uma classe base, ele é automaticamente virtual na classe derivada, mas é uma boa ideia documentar quais funções são virtuais usando também a palavra-chave `virtual` nas declarações da classe derivada.

O quarto ponto é que a classe base declara um destrutor virtual. Isso é para garantir que a sequência correta de destrutores seja chamada quando um objeto derivado é destruído. Discutiremos esse ponto em mais detalhes posteriormente neste capítulo.

> **Nota**
> Se você redefinir um método da classe base em uma classe derivada, a prática usual é declarar o método da classe base como virtual. Isso faz o programa escolher a versão do método com base no tipo do objeto em vez do tipo de uma referência ou ponteiro. Também é prática usual declarar um destrutor virtual para a classe base.

### Implementações das Classes

O próximo passo é preparar a implementação das classes. Parte disso já foi feita pelas definições de funções inline no arquivo de cabeçalho. A Listagem 13.8 fornece as definições de métodos restantes. Observe que a palavra-chave `virtual` é usada apenas nos protótipos de métodos na declaração de classe, não nas definições de métodos na Listagem 13.8.

**Listagem 13.8 — latao.cpp**

```cpp
// latao.cpp -- métodos das classes de contas bancárias
#include <iostream>
#include "latao.h"
using std::cout;
using std::endl;
using std::string;
// ferramentas de formatação
typedef std::ios_base::fmtflags formato;
typedef std::streamsize precis;
formato definirFormato();
void restaurar(formato f, precis p);
// Métodos de Latao
Latao::Latao(const string & s, long nc, double sal)
{
    nomeCompleto = s;
    numConta = nc;
    saldo = sal;
}
void Latao::Depositar(double val)
{
    if (val < 0)
        cout << "Depósito negativo não permitido; "
             << "depósito cancelado.\n";
    else
        saldo += val;
}
void Latao::Sacar(double val)
{
    // configura formato ###.##
    formato estadoInicial = definirFormato();
    precis prec = cout.precision(2);
    if (val < 0)
        cout << "O valor do saque deve ser positivo; "
             << "saque cancelado.\n";
    else if (val <= saldo)
        saldo -= val;
    else
        cout << "O valor do saque de $" << val
             << " excede seu saldo.\n"
             << "Saque cancelado.\n";
    restaurar(estadoInicial, prec);
}
double Latao::Saldo() const
{
    return saldo;
}
void Latao::VerConta() const
{
    // configura formato ###.##
    formato estadoInicial = definirFormato();
    precis prec = cout.precision(2);
    cout << "Cliente: " << nomeCompleto << endl;
    cout << "Número de Conta: " << numConta << endl;
    cout << "Saldo: $" << saldo << endl;
    restaurar(estadoInicial, prec); // restaura formato original
}
// Métodos de LataoPlus
LataoPlus::LataoPlus(const string & s, long nc, double sal,
        double le, double t) : Latao(s, nc, sal)
{
    limiteEmprestimo = le;
    deveAoBanco = 0.0;
    taxa = t;
}
LataoPlus::LataoPlus(const Latao & la, double le, double t)
        : Latao(la) // usa construtor de cópia implícito
{
    limiteEmprestimo = le;
    deveAoBanco = 0.0;
    taxa = t;
}
// redefine como VerConta() funciona
void LataoPlus::VerConta() const
{
    // configura formato ###.##
    formato estadoInicial = definirFormato();
    precis prec = cout.precision(2);
    Latao::VerConta(); // exibe a parte base
    cout << "Limite de empréstimo: $" << limiteEmprestimo << endl;
    cout << "Deve ao banco: $" << deveAoBanco << endl;
    cout.precision(3); // formato ###.###
    cout << "Taxa de empréstimo: " << 100 * taxa << "%\n";
    restaurar(estadoInicial, prec);
}
// redefine como Sacar() funciona
void LataoPlus::Sacar(double val)
{
    // configura formato ###.##
    formato estadoInicial = definirFormato();
    precis prec = cout.precision(2);
    double sal = Saldo();
    if (val <= sal)
        Latao::Sacar(val);
    else if ( val <= sal + limiteEmprestimo - deveAoBanco)
    {
        double adiantamento = val - sal;
        deveAoBanco += adiantamento * (1.0 + taxa);
        cout << "Adiantamento do banco: $" << adiantamento << endl;
        cout << "Encargo financeiro: $" << adiantamento * taxa << endl;
        Depositar(adiantamento);
        Latao::Sacar(val);
    }
    else
        cout << "Limite de crédito excedido. Transação cancelada.\n";
    restaurar(estadoInicial, prec);
}
formato definirFormato()
{
    // configura formato ###.##
    return cout.setf(std::ios_base::fixed,
           std::ios_base::floatfield);
}
void restaurar(formato f, precis p)
{
    cout.setf(f, std::ios_base::floatfield);
    cout.precision(p);
}
```

Antes de examinar os detalhes da Listagem 13.8, como o tratamento de formatação em alguns dos métodos, vamos examinar os aspectos que se relacionam diretamente com a herança. Lembre-se de que a classe derivada não tem acesso direto aos dados privados da classe base; a classe derivada deve usar os métodos públicos da classe base para acessar esses dados. Os meios de acesso dependem do método. Os construtores usam uma técnica, e as outras funções membro usam uma técnica diferente.

A técnica que os construtores da classe derivada usam para inicializar os dados privados da classe base é a sintaxe de lista de inicializadores de membros. Os construtores de `JogadorAvaliado` usam essa técnica, e os construtores de `LataoPlus` também:

```cpp
LataoPlus::LataoPlus(const string & s, long nc, double sal,
        double le, double t) : Latao(s, nc, sal)
{
    limiteEmprestimo = le;
    deveAoBanco = 0.0;
    taxa = t;
}
LataoPlus::LataoPlus(const Latao & la, double le, double t)
        : Latao(la) // usa construtor de cópia implícito
{
    limiteEmprestimo = le;
    deveAoBanco = 0.0;
    taxa = t;
}
```

Cada um desses construtores usa a sintaxe de lista de inicializadores de membros para passar as informações da classe base para um construtor da classe base e então usa o corpo do construtor para inicializar os novos itens de dados adicionados pela classe `LataoPlus`.

Os não-construtores não podem usar a sintaxe de lista de inicializadores de membros. Mas um método de classe derivada pode chamar um método público da classe base. Por exemplo, ignorando o aspecto de formatação, o núcleo da versão de `VerConta()` de `LataoPlus` é este:

```cpp
// redefine como VerConta() funciona
void LataoPlus::VerConta() const
{
...
    Latao::VerConta(); // exibe a parte base
    cout << "Limite de empréstimo: $" << limiteEmprestimo << endl;
    cout << "Deve ao banco: $" << deveAoBanco << endl;
    cout << "Taxa de empréstimo: " << 100 * taxa << "%\n";
...
}
```

Em outras palavras, `LataoPlus::VerConta()` exibe os membros de dados adicionados de `LataoPlus` e chama o método da classe base `Latao::VerConta()` para exibir os membros de dados da classe base. Usar o operador de escopo em um método de classe derivada para invocar um método da classe base é uma técnica padrão.

É vital que o código use o operador de escopo. Suponha que, em vez disso, você escrevesse o código desta forma:

```cpp
// redefine erroneamente como VerConta() funciona
void LataoPlus::VerConta() const
{
...
    VerConta(); // ops! chamada recursiva
...
}
```

Se o código não usar o operador de escopo, o compilador assume que `VerConta()` é `LataoPlus::VerConta()`, e isso cria uma função recursiva sem terminação — algo que não é bom.

Em seguida, considere o método `LataoPlus::Sacar()`. Se o cliente sacar um valor maior do que o saldo, o método deve providenciar um empréstimo. Ele pode usar `Latao::Sacar()` para acessar o membro saldo, mas `Latao::Sacar()` emite uma mensagem de erro se o valor do saque excede o saldo. Essa implementação evita a mensagem usando o método `Depositar()` para fazer o empréstimo e depois chamando `Latao::Sacar()` quando há fundos suficientes disponíveis.

Observe que o método usa a função `Saldo()` da classe base para determinar o saldo original. O código não precisa usar o operador de escopo para `Saldo()` porque esse método não foi redefinido na classe derivada.

Os métodos `VerConta()` e `Sacar()` usam os métodos de formatação `setf()` e `precision()` para definir o modo de saída para valores de ponto flutuante como ponto fixo, com duas casas à direita do ponto decimal. Quando esses modos são definidos, a saída permanece nesse modo, então a atitude correta desses métodos é redefinir o modo de formatação para seu estado anterior à chamada dos métodos. Para evitar alguma duplicação de código, o programa move algumas ações de formatação para funções auxiliares:

```cpp
// ferramentas de formatação
typedef std::ios_base::fmtflags formato;
typedef std::streamsize precis;
formato definirFormato();
void restaurar(formato f, precis p);
```

### Usando as Classes Latao e LataoPlus

A Listagem 13.9 mostra as definições de classe com um objeto `Latao` e um objeto `LataoPlus`.

**Listagem 13.9 — usarlatao1.cpp**

```cpp
// usarlatao1.cpp -- testando as classes de contas bancárias
// compilar com latao.cpp
#include <iostream>
#include "latao.h"
int main()
{
    using std::cout;
    using std::endl;
    Latao Leitoa("Porcelot Pigg", 381299, 4000.00);
    LataoPlus Porco("Horatio Hogg", 382288, 3000.00);
    Leitoa.VerConta();
    cout << endl;
    Porco.VerConta();
    cout << endl;
    cout << "Depositando $1000 na Conta Hogg:\n";
    Porco.Depositar(1000.00);
    cout << "Novo saldo: $" << Porco.Saldo() << endl;
    cout << "Sacando $4200 da Conta Pigg:\n";
    Leitoa.Sacar(4200.00);
    cout << "Saldo da conta Pigg: $" << Leitoa.Saldo() << endl;
    cout << "Sacando $4200 da Conta Hogg:\n";
    Porco.Sacar(4200.00);
    Porco.VerConta();
    return 0;
}
```

Na saída a seguir do programa da Listagem 13.9, observe que Hogg tem proteção de cheque especial e Pigg não:

```
Cliente: Porcelot Pigg
Número de Conta: 381299
Saldo: $4000.00
Cliente: Horatio Hogg
Número de Conta: 382288
Saldo: $3000.00
Limite de empréstimo: $500.00
Deve ao banco: $0.00
Taxa de empréstimo: 11.125%
Depositando $1000 na Conta Hogg:
Novo saldo: $4000
Sacando $4200 da Conta Pigg:
O valor do saque de $4200.00 excede seu saldo.
Saque cancelado.
Saldo da conta Pigg: $4000
Sacando $4200 da Conta Hogg:
Adiantamento do banco: $200.00
Encargo financeiro: $22.25
Cliente: Horatio Hogg
Número de Conta: 382288
Saldo: $0.00
Limite de empréstimo: $500.00
Deve ao banco: $222.25
Taxa de empréstimo: 11.125%
```

### Mostrando o Comportamento de Métodos Virtuais

Na Listagem 13.9, os métodos são invocados por objetos, não por ponteiros ou referências, portanto esse programa não usa o recurso de método virtual. Vejamos um exemplo no qual os métodos virtuais entram em jogo. Suponha que você queira gerenciar uma mistura de contas `Latao` e `LataoPlus`. Seria bom se você pudesse ter um único array contendo uma mistura de objetos `Latao` e `LataoPlus`, mas isso não é possível. Cada item em um array deve ser do mesmo tipo, e `Latao` e `LataoPlus` são dois tipos separados. No entanto, você pode criar um array de ponteiros-para-`Latao`. Nesse caso, cada elemento é do mesmo tipo, mas, por causa do modelo de herança pública, um ponteiro-para-`Latao` pode apontar tanto para um objeto `Latao` quanto para um objeto `LataoPlus`. Assim, em efeito, você tem uma forma de representar uma coleção de mais de um tipo de objeto com um único array. Isso é polimorfismo, e a Listagem 13.10 mostra um exemplo simples.

**Listagem 13.10 — usarlatao2.cpp**

```cpp
// usarlatao2.cpp -- exemplo polimórfico
// compilar com latao.cpp
#include <iostream>
#include <string>
#include "latao.h"
const int CLIENTES = 4;
int main()
{
    using std::cin;
    using std::cout;
    using std::endl;
    Latao * p_clientes[CLIENTES];
    std::string temp;
    long tempnum;
    double tempsal;
    char tipo;
    for (int i = 0; i < CLIENTES; i++)
    {
        cout << "Digite o nome do cliente: ";
        getline(cin, temp);
        cout << "Digite o número da conta do cliente: ";
        cin >> tempnum;
        cout << "Digite o saldo inicial: $";
        cin >> tempsal;
        cout << "Digite 1 para Conta Latão ou "
             << "2 para Conta Latão Plus: ";
        while (cin >> tipo && (tipo != '1' && tipo != '2'))
            cout << "Digite 1 ou 2: ";
        if (tipo == '1')
            p_clientes[i] = new Latao(temp, tempnum, tempsal);
        else
        {
            double tmax, ttaxa;
            cout << "Digite o limite de cheque especial: $";
            cin >> tmax;
            cout << "Digite a taxa de juros "
                 << "como fração decimal: ";
            cin >> ttaxa;
            p_clientes[i] = new LataoPlus(temp, tempnum, tempsal,
                                    tmax, ttaxa);
        }
        while (cin.get() != '\n')
            continue;
    }
    cout << endl;
    for (int i = 0; i < CLIENTES; i++)
    {
        p_clientes[i]->VerConta();
        cout << endl;
    }
    for (int i = 0; i < CLIENTES; i++)
    {
        delete p_clientes[i]; // libera memória
    }
    cout << "Concluído.\n";
    return 0;
}
```

O programa da Listagem 13.10 permite que a entrada do usuário determine o tipo de conta a ser adicionado e depois usa `new` para criar e inicializar um objeto do tipo correto. Você deve se lembrar de que `getline(cin, temp)` lê uma linha de entrada de `cin` e a armazena no objeto `string` `temp`.

Aqui está uma execução de amostra do programa da Listagem 13.10:

```
Digite o nome do cliente: Harry Fishsong
Digite o número da conta do cliente: 112233
Digite o saldo inicial: $1500
Digite 1 para Conta Latão ou 2 para Conta Latão Plus: 1
Digite o nome do cliente: Dinah Otternoe
Digite o número da conta do cliente: 121213
Digite o saldo inicial: $1800
Digite 1 para Conta Latão ou 2 para Conta Latão Plus: 2
Digite o limite de cheque especial: $350
Digite a taxa de juros como fração decimal: 0.12
Digite o nome do cliente: Brenda Birdherd
Digite o número da conta do cliente: 212118
Digite o saldo inicial: $5200
Digite 1 para Conta Latão ou 2 para Conta Latão Plus: 2
Digite o limite de cheque especial: $800
Digite a taxa de juros como fração decimal: 0.10
Digite o nome do cliente: Tim Turtletop
Digite o número da conta do cliente: 233255
Digite o saldo inicial: $688
Digite 1 para Conta Latão ou 2 para Conta Latão Plus: 1
Cliente: Harry Fishsong
Número de Conta: 112233
Saldo: $1500.00
Cliente: Dinah Otternoe
Número de Conta: 121213
Saldo: $1800.00
Limite de empréstimo: $350.00
Deve ao banco: $0.00
Taxa de empréstimo: 12.00%
Cliente: Brenda Birdherd
Número de Conta: 212118
Saldo: $5200.00
Limite de empréstimo: $800.00
Deve ao banco: $0.00
Taxa de empréstimo: 10.00%
Cliente: Tim Turtletop
Número de Conta: 233255
Saldo: $688.00
Concluído.
```

O aspecto polimórfico é fornecido pelo seguinte código:

```cpp
for (i = 0; i < CLIENTES; i++)
{
    p_clientes[i]->VerConta();
    cout << endl;
}
```

Se o membro do array aponta para um objeto `Latao`, `Latao::VerConta()` é invocado; se o membro do array aponta para um objeto `LataoPlus`, `LataoPlus::VerConta()` é invocado. Se `Latao::VerConta()` não tivesse sido declarado como virtual, `Latao::VerConta()` seria invocado em todos os casos.

---

## A Necessidade de Destrutores Virtuais

O código da Listagem 13.10 que usa `delete` para liberar os objetos alocados por `new` ilustra por que a classe base deve ter um destrutor virtual, mesmo que nenhum destrutor pareça ser necessário. Se os destrutores não são virtuais, então apenas o destrutor correspondente ao tipo do ponteiro é chamado. Na Listagem 13.10, isso significa que apenas o destrutor de `Latao` seria chamado, mesmo que o ponteiro apontasse para um objeto `LataoPlus`. Se os destrutores são virtuais, o destrutor correspondente ao tipo do objeto é chamado. Então se um ponteiro aponta para um objeto `LataoPlus`, o destrutor de `LataoPlus` é chamado. E quando um destrutor de `LataoPlus` termina, ele automaticamente chama o destrutor da classe base. Portanto, usar destrutores virtuais garante que a sequência correta de destrutores seja chamada. Na Listagem 13.10, esse comportamento correto não é essencial porque os destrutores não fazem nada. Mas se, digamos, `LataoPlus` tivesse um destrutor que faz alguma coisa, seria vital que `Latao` tivesse um destrutor virtual, mesmo que ele não fizesse nada.

---

[Anterior](capitulo-12-03-fila-resumo.md) | [Índice](README.md) | [Próximo](capitulo-13-02-virtual-binding.md)
