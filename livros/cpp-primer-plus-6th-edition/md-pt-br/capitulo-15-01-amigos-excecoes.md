# Capítulo 15 — Amigos, Exceções e Mais (Friends, Exceptions, and More) — Parte 1: Amigos, Classes Aninhadas e Exceções Básicas

> Tradução não oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Navegação: [Anterior](capitulo-14-04-resumo.md) | [Índice](README.md) | [Próximo](capitulo-15-02-excecoes-avancadas.md)

Neste capítulo você aprenderá sobre o seguinte:

- Classes amigas (friend classes)
- Métodos membros amigos (friend member functions)
- Classes aninhadas (nested classes)
- Lançamento de exceções, blocos `try` e blocos `catch`
- Classes de exceção (exception classes)
- Identificação de tipo em tempo de execução (Runtime Type Identification — RTTI)
- `dynamic_cast` e `typeid`
- `static_cast`, `const_cast` e `reinterpret_cast`

Este capítulo amarra algumas pontas soltas e se aventura em algumas das adições mais recentes à linguagem C++. As pontas soltas incluem classes amigas, funções membro amigas e classes aninhadas, que são classes declaradas dentro de outras classes. As adições mais recentes discutidas aqui são exceções, identificação de tipo em tempo de execução (RTTI) e controle aprimorado de conversão de tipos. O tratamento de exceções em C++ fornece um mecanismo para lidar com ocorrências incomuns que de outra forma levariam o programa a parar. O RTTI é um mecanismo para identificar tipos de objetos. Os novos operadores de conversão de tipo melhoram a segurança das conversões. Essas três últimas facilidades são relativamente novas no C++, e compiladores mais antigos não as suportam.

## Amigos (Friends)

Vários exemplos neste livro até agora usam funções amigas como parte da interface estendida de uma classe. Tais funções não são os únicos tipos de amigos que uma classe pode ter. Uma classe também pode ser amiga. Nesse caso, qualquer método da classe amiga pode acessar os membros privados e protegidos da classe original. Você também pode ser mais restritivo e designar apenas funções membro particulares de uma classe como amigas de outra classe. Uma classe define quais funções, funções membro ou classes são suas amigas; a amizade não pode ser imposta de fora. Portanto, embora os amigos concedam acesso externo à porção privada de uma classe, eles não violam realmente o espírito da programação orientada a objetos. Em vez disso, fornecem mais flexibilidade à interface pública.

### Classes Amigas

Quando você pode querer tornar uma classe amiga de outra? Vejamos um exemplo. Suponha que você deva programar uma simulação simples de uma televisão e um controle remoto. Você decide definir uma classe `Televisao` representando uma televisão e uma classe `Controle` representando um controle remoto. Claramente, deve haver algum tipo de relação entre essas classes, mas que tipo? Um controle remoto não é uma televisão e vice-versa, então o relacionamento is-a da herança pública não se aplica. Nem um é componente do outro, então o relacionamento has-a da composição ou da herança privada ou protegida também não se aplica. O que é verdade é que um controle remoto pode modificar o estado de uma televisão, e isso sugere tornar a classe `Controle` amiga da classe `Televisao`.

Vamos definir a classe `Televisao`. Você pode representar uma televisão com um conjunto de membros de estado — isto é, variáveis que descrevem vários aspectos da televisão. Aqui estão alguns dos possíveis estados:

- Ligado/desligado
- Configuração de canal
- Configuração de volume
- Modo de sintonia a cabo ou antena
- Sintonizador de TV ou entrada A/V

O modo de sintonia reflete o fato de que, nos Estados Unidos, o espaçamento entre canais 14 e acima é diferente para recepção a cabo em comparação com recepção UHF de transmissão. A seleção de entrada escolhe entre TV, que pode ser cabo ou TV de transmissão, e DVD. Alguns aparelhos podem oferecer mais opções, como múltiplas entradas DVD/Blu-ray, mas esta lista é suficiente para os propósitos deste exemplo.

Além disso, uma televisão tem alguns parâmetros que não são variáveis de estado. Por exemplo, televisões variam no número de canais que podem receber, e você pode incluir um membro para rastrear esse valor.

Em seguida, você deve fornecer à classe métodos para alterar as configurações. Muitos aparelhos de televisão hoje em dia escondem seus controles atrás de painéis, mas ainda é possível na maioria das televisões mudar canais, etc., sem um controle remoto. Porém, muitas vezes você pode ir para cima ou para baixo um canal de cada vez, mas não pode selecionar um canal aleatoriamente. Da mesma forma, geralmente há um botão para aumentar o volume e um para diminuir.

Um controle remoto deve duplicar os controles embutidos na televisão. Muitos de seus métodos podem ser implementados usando os métodos de `Televisao`. Além disso, um controle remoto normalmente fornece seleção aleatória de canal. Ou seja, você pode ir diretamente do canal 2 ao canal 20 sem passar por todos os canais intermediários. Além disso, muitos controles remotos podem trabalhar em dois ou mais modos, por exemplo, como controlador de televisão e como controlador de DVD.

Essas considerações sugerem uma definição como a mostrada na Listagem 15.1. A definição inclui várias constantes definidas como enumerações. A seguinte instrução torna `Controle` uma classe amiga:

```cpp
friend class Controle;
```

Uma declaração de amizade pode aparecer em uma seção pública, privada ou protegida; a localização não faz diferença. Como a classe `Controle` menciona a classe `Televisao`, o compilador precisa conhecer a classe `Televisao` antes de poder processar a classe `Controle`. A maneira mais simples de conseguir isso é definir a classe `Televisao` primeiro. Como alternativa, você pode usar uma declaração prévia (forward declaration); discutiremos essa opção em breve.

**Listagem 15.1 — televisao.h**

```cpp
// televisao.h -- classes Televisao e Controle
#ifndef TELEVISAO_H_
#define TELEVISAO_H_
class Televisao
{
public:
    friend class Controle;    // Controle pode acessar partes privadas de Televisao
    enum {Desligada, Ligada};
    enum {ValMin, ValMax = 20};
    enum {Antena, Cabo};
    enum {TV, DVD};
    Televisao(int e = Desligada, int mc = 125) : estado(e), volume(5),
        maxCanal(mc), canal(2), modo(Cabo), entrada(TV) {}
    void ligarDesligar() {estado = (estado == Ligada)? Desligada : Ligada;}
    bool estaLigada() const {return estado == Ligada;}
    bool aumentarVolume();
    bool diminuirVolume();
    void subirCanal();
    void descerCanal();
    void definirModo() {modo = (modo == Antena)? Cabo : Antena;}
    void definirEntrada() {entrada = (entrada == TV)? DVD : TV;}
    void configuracoes() const;     // exibe todas as configurações
private:
    int estado;      // ligada ou desligada
    int volume;      // assumido como digitalizado
    int maxCanal;    // número máximo de canais
    int canal;       // configuração do canal atual
    int modo;        // transmissão ou cabo
    int entrada;     // TV ou DVD
};

class Controle
{
private:
    int modo;        // controla TV ou DVD
public:
    Controle(int m = Televisao::TV) : modo(m) {}
    bool aumentarVolume(Televisao & t) { return t.aumentarVolume();}
    bool diminuirVolume(Televisao & t) { return t.diminuirVolume();}
    void ligarDesligar(Televisao & t) { t.ligarDesligar(); }
    void subirCanal(Televisao & t) {t.subirCanal();}
    void descerCanal(Televisao & t) {t.descerCanal();}
    void definirCanal(Televisao & t, int c) {t.canal = c;}
    void definirModo(Televisao & t) {t.definirModo();}
    void definirEntrada(Televisao & t) {t.definirEntrada();}
};
#endif
```

A maioria dos métodos de classe na Listagem 15.1 são definidos como inline. Observe que cada método de `Controle` diferente do construtor recebe uma referência a um objeto `Televisao` como argumento. Isso reflete que um controle remoto deve ser apontado para uma TV específica. A Listagem 15.2 mostra as definições restantes. As funções de configuração de volume alteram o membro `volume` em uma unidade, a menos que o som tenha atingido sua configuração mínima ou máxima. As funções de seleção de canal usam wraparound (retorno ao início), com a configuração de canal mais baixa, considerada como 1, imediatamente após a configuração de canal mais alta, `maxCanal`.

Muitos dos métodos usam o operador condicional para alternar um estado entre duas configurações:

```cpp
void ligarDesligar() {estado = (estado == Ligada)? Desligada : Ligada;}
```

Desde que os dois valores de estado sejam `true` e `false`, ou equivalentemente, `0` e `1`, isso pode ser feito de forma mais compacta usando o operador XOR bit a bit combinado com atribuição (`^=`):

```cpp
void ligarDesligar() {estado ^= 1;}
```

Na verdade, você poderia armazenar até oito configurações de estado bivalentes em uma única variável `unsigned char` e alterná-las individualmente, mas essa é outra história, tornada possível pelos operadores bit a bit discutidos no Apêndice E.

**Listagem 15.2 — televisao.cpp**

```cpp
// televisao.cpp -- métodos para a classe Televisao (métodos de Controle são inline)
#include <iostream>
#include "televisao.h"
bool Televisao::aumentarVolume()
{
    if (volume < ValMax)
    {
        volume++;
        return true;
    }
    else
        return false;
}
bool Televisao::diminuirVolume()
{
    if (volume > ValMin)
    {
        volume--;
        return true;
    }
    else
        return false;
}
void Televisao::subirCanal()
{
    if (canal < maxCanal)
        canal++;
    else
        canal = 1;
}
void Televisao::descerCanal()
{
    if (canal > 1)
        canal--;
    else
        canal = maxCanal;
}
void Televisao::configuracoes() const
{
    using std::cout;
    using std::endl;
    cout << "TV esta " << (estado == Desligada? "Desligada" : "Ligada") << endl;
    if (estado == Ligada)
    {
        cout << "Configuracao de volume = " << volume << endl;
        cout << "Configuracao de canal = " << canal << endl;
        cout << "Modo = "
             << (modo == Antena? "antena" : "cabo") << endl;
        cout << "Entrada = "
             << (entrada == TV? "TV" : "DVD") << endl;
    }
}
```

A Listagem 15.3 é um programa curto que testa algumas das características do programa até agora. O mesmo controle remoto é usado para controlar duas televisões separadas.

**Listagem 15.3 — usar_televisao.cpp**

```cpp
// usar_televisao.cpp -- usando as classes Televisao e Controle
#include <iostream>
#include "televisao.h"
int main()
{
    using std::cout;
    Televisao s42;
    cout << "Configuracoes iniciais para TV de 42\":\n";
    s42.configuracoes();
    s42.ligarDesligar();
    s42.subirCanal();
    cout << "\nConfiguracoes ajustadas para TV de 42\":\n";
    s42.configuracoes();
    Controle cinza;
    cinza.definirCanal(s42, 10);
    cinza.aumentarVolume(s42);
    cinza.aumentarVolume(s42);
    cout << "\nConfiguracoes da TV de 42\" apos uso do controle:\n";
    s42.configuracoes();

    Televisao s58(Televisao::Ligada);
    s58.definirModo();
    cinza.definirCanal(s58, 28);
    cout << "\nConfiguracoes da TV de 58\":\n";
    s58.configuracoes();
    return 0;
}
```

Aqui está a saída do programa nas Listagens 15.1, 15.2 e 15.3:

```
Configuracoes iniciais para TV de 42":
TV esta Desligada

Configuracoes ajustadas para TV de 42":
TV esta Ligada
Configuracao de volume = 5
Configuracao de canal = 3
Modo = cabo
Entrada = TV
Configuracoes da TV de 42" apos uso do controle:
TV esta Ligada
Configuracao de volume = 7
Configuracao de canal = 10
Modo = cabo
Entrada = TV
Configuracoes da TV de 58":
TV esta Ligada
Configuracao de volume = 5
Configuracao de canal = 28
Modo = antena
Entrada = TV
```

O ponto principal deste exercício é que a amizade de classe é um idioma natural para expressar certos relacionamentos. Sem alguma forma de amizade, você teria que tornar as partes privadas da classe `Televisao` públicas ou então construir alguma classe maior e estranha que englobasse tanto uma televisão quanto um controle remoto. E essa solução não refletiria o fato de que um único controle remoto pode ser usado com várias televisões.

### Funções Membro Amigas

Olhando para o código do último exemplo, você pode notar que a maioria dos métodos de `Controle` é implementada usando a interface pública da classe `Televisao`. Isso significa que esses métodos não precisam realmente de status de amigo. De fato, o único método de `Controle` que acessa diretamente um membro privado de `Televisao` é `Controle::definirCanal()`, então esse é o único método que precisa ser amigo. Você tem a opção de tornar apenas membros de classe selecionados amigos de outra classe em vez de tornar toda a classe amiga, mas isso é um pouco mais complicado. Você precisa ter cuidado com a ordem em que organiza as várias declarações e definições.

A maneira de tornar `Controle::definirCanal()` amigo da classe `Televisao` é declará-lo como amigo na declaração da classe `Televisao`:

```cpp
class Televisao
{
    friend void Controle::definirCanal(Televisao & t, int c);
    ...
};
```

No entanto, para o compilador processar essa instrução, ele precisa já ter visto a definição de `Controle`. Caso contrário, ele não saberá que `Controle` é uma classe e que `definirCanal()` é um método dessa classe. Isso sugere colocar a definição de `Controle` acima da definição de `Televisao`. Mas o fato de que os métodos de `Controle` mencionam objetos `Televisao` significa que a definição de `Televisao` deve aparecer acima da definição de `Controle`. Parte da solução para a dependência circular é usar uma declaração prévia (forward declaration). Para fazer isso, você insere a seguinte instrução acima da definição de `Controle`:

```cpp
class Televisao;    // declaração prévia
```

Isso fornece o seguinte arranjo:

```cpp
class Televisao;    // declaração prévia
class Controle { ... };
class Televisao { ... };
```

Outra dificuldade permanece. Na Listagem 15.1, a declaração de `Controle` contém código inline como o seguinte:

```cpp
void ligarDesligar(Televisao & t) { t.ligarDesligar(); }
```

Como isso chama um método de `Televisao`, o compilador precisa já ter visto a declaração da classe `Televisao` neste ponto para saber quais métodos `Televisao` tem. Mas, como você viu, essa declaração necessariamente segue a declaração de `Controle`. A solução para esse problema é restringir `Controle` apenas a declarações de métodos e colocar as definições reais após a classe `Televisao`. Isso leva ao seguinte ordenamento:

```cpp
class Televisao;    // declaração prévia
class Controle { ... };    // métodos que usam Televisao apenas como protótipos
class Televisao { ... };
// colocar definições de métodos de Controle aqui
```

Os protótipos de `Controle` ficam assim:

```cpp
void ligarDesligar(Televisao & t);
```

Tudo que o compilador precisa saber ao inspecionar esse protótipo é que `Televisao` é uma classe, e a declaração prévia fornece essa informação. No momento em que o compilador chega às definições reais dos métodos, ele já leu a declaração da classe `Televisao` e tem as informações adicionais necessárias para compilar esses métodos. Usando a palavra-chave `inline` nas definições dos métodos, você ainda pode tornar os métodos inline. A Listagem 15.4 mostra o arquivo de cabeçalho revisado.

**Listagem 15.4 — tvmf.h**

```cpp
// tvmf.h -- classes Televisao e Controle usando um membro amigo
#ifndef TVMF_H_
#define TVMF_H_
class Televisao;    // declaração prévia

class Controle
{
public:
    enum Estado{Desligada, Ligada};
    enum {ValMin, ValMax = 20};
    enum {Antena, Cabo};
    enum {TV, DVD};
private:
    int modo;
public:
    Controle(int m = TV) : modo(m) {}
    bool aumentarVolume(Televisao & t);     // apenas protótipo
    bool diminuirVolume(Televisao & t);
    void ligarDesligar(Televisao & t);
    void subirCanal(Televisao & t);
    void descerCanal(Televisao & t);
    void definirModo(Televisao & t);
    void definirEntrada(Televisao & t);
    void definirCanal(Televisao & t, int c);
};
class Televisao
{
public:
    friend void Controle::definirCanal(Televisao & t, int c);
    enum Estado{Desligada, Ligada};
    enum {ValMin, ValMax = 20};
    enum {Antena, Cabo};
    enum {TV, DVD};
    Televisao(int e = Desligada, int mc = 125) : estado(e), volume(5),
        maxCanal(mc), canal(2), modo(Cabo), entrada(TV) {}
    void ligarDesligar() {estado = (estado == Ligada)? Desligada : Ligada;}
    bool estaLigada() const {return estado == Ligada;}
    bool aumentarVolume();
    bool diminuirVolume();
    void subirCanal();
    void descerCanal();
    void definirModo() {modo = (modo == Antena)? Cabo : Antena;}
    void definirEntrada() {entrada = (entrada == TV)? DVD : TV;}
    void configuracoes() const;
private:
    int estado;
    int volume;
    int maxCanal;
    int canal;
    int modo;
    int entrada;
};
// métodos de Controle como funções inline
inline bool Controle::aumentarVolume(Televisao & t) { return t.aumentarVolume();}
inline bool Controle::diminuirVolume(Televisao & t) { return t.diminuirVolume();}
inline void Controle::ligarDesligar(Televisao & t) { t.ligarDesligar(); }
inline void Controle::subirCanal(Televisao & t) {t.subirCanal();}
inline void Controle::descerCanal(Televisao & t) {t.descerCanal();}
inline void Controle::definirModo(Televisao & t) {t.definirModo();}
inline void Controle::definirEntrada(Televisao & t) {t.definirEntrada();}
inline void Controle::definirCanal(Televisao & t, int c) {t.canal = c;}
#endif
```

Se você incluir `tvmf.h` em vez de `televisao.h` em `televisao.cpp` e `usar_televisao.cpp`, o programa resultante se comporta da mesma forma que o original. A diferença é que apenas um método de `Controle` — em vez de todos os métodos de `Controle` — é amigo da classe `Televisao`.

Lembre-se de que funções inline têm ligação interna, o que significa que a definição da função deve estar no arquivo que usa a função. Aqui, as definições inline estão no arquivo de cabeçalho, então incluir o arquivo de cabeçalho no arquivo que usa as definições coloca a definição no lugar certo. Você poderia colocar as definições no arquivo de implementação, desde que remova a palavra-chave `inline`, dando assim às funções ligação externa.

A propósito, tornar toda a classe `Controle` amiga não precisa de uma declaração prévia, porque a própria instrução de amizade identifica `Controle` como uma classe:

```cpp
friend class Controle;
```

### Outros Relacionamentos de Amizade

Outras combinações de amigos e classes além das discutidas até agora neste capítulo são possíveis. Vejamos rapidamente algumas delas agora.

Suponha que o avanço da tecnologia traga controles remotos interativos. Por exemplo, um controle remoto interativo pode permitir que você insira uma resposta a alguma pergunta feita em um programa de televisão, e a televisão pode ativar um buzzer em seu controle remoto se sua resposta estiver errada. Ignorando a possibilidade de a televisão usar tais facilidades para programar os espectadores, vamos apenas olhar para os aspectos de programação C++. A nova configuração se beneficiaria da amizade mútua, com alguns métodos de `Controle` sendo capazes de afetar um objeto `Televisao`, como antes, e com alguns métodos de `Televisao` sendo capazes de afetar um objeto `Controle`. Isso pode ser realizado tornando as classes amigas uma da outra. Ou seja, `Televisao` será amiga de `Controle` além de `Controle` ser amiga de `Televisao`. Um ponto a ter em mente é que um método de `Televisao` que usa um objeto `Controle` pode ser prototipado antes da declaração de `Controle`, mas deve ser definido depois da declaração para que o compilador tenha informações suficientes para compilar o método. A configuração ficaria assim:

```cpp
class Televisao
{
friend class Controle;
public:
    void buzinar(Controle & r);
    ...
};
class Controle
{
friend class Televisao;
public:
    bool aumentarVolume(Televisao & t) { t.aumentarVolume(); }
    ...
};
inline void Televisao::buzinar(Controle & r)
{
    ...
}
```

Como a declaração de `Controle` segue a declaração de `Televisao`, `Controle::aumentarVolume()` pode ser definido na declaração de classe. No entanto, o método `Televisao::buzinar()` tem que ser definido fora da declaração de `Televisao` para que a definição possa seguir a declaração de `Controle`. Se você não quiser que `buzinar()` seja inline, você precisa defini-lo em um arquivo de definições de método separado.

#### Amigos Compartilhados

Outro uso para amigos é quando uma função precisa acessar dados privados em duas classes separadas. Logicamente, tal função deveria ser uma função membro de cada classe, mas isso é impossível. Ela poderia ser um membro de uma classe e amiga da outra, mas às vezes é mais razoável tornar a função amiga de ambas. Suponha, por exemplo, que você tenha uma classe `Sensor` que representa algum tipo de dispositivo de medição programável e uma classe `Analisador` que representa algum tipo de dispositivo de análise programável. Cada um tem um relógio interno, e você gostaria de poder sincronizar os dois relógios. Você poderia usar algo semelhante ao seguinte:

```cpp
class Analisador;    // declaração prévia
class Sensor
{
    friend void sincronizar(Analisador & a, const Sensor & s);    // sinc a para s
    friend void sincronizar(Sensor & s, const Analisador & a);    // sinc s para a
    ...
};
class Analisador
{
    friend void sincronizar(Analisador & a, const Sensor & s);    // sinc a para s
    friend void sincronizar(Sensor & s, const Analisador & a);    // sinc s para a
    ...
};
// definir as funções amigas
inline void sincronizar(Analisador & a, const Sensor & s)
{
    ...
}
inline void sincronizar(Sensor & s, const Analisador & a)
{
    ...
}
```

A declaração prévia permite ao compilador saber que `Analisador` é um tipo quando chega às declarações de amigo na declaração da classe `Sensor`.

## Classes Aninhadas (Nested Classes)

Em C++, você pode colocar uma declaração de classe dentro de outra classe. A classe declarada dentro de outra é chamada de classe aninhada (nested class), e ela ajuda a evitar a desordem de nomes ao atribuir ao novo tipo um escopo de classe. Funções membro da classe que contém a declaração podem criar e usar objetos da classe aninhada. O mundo exterior só pode usar a classe aninhada se a declaração estiver na seção pública e se você usar o operador de resolução de escopo. (Versões mais antigas de C++ não permitem classes aninhadas ou implementam o conceito de forma incompleta.)

Aninhar classes não é o mesmo que composição (containment). Lembre-se de que composição significa ter um objeto de classe como membro de outra classe. Aninhar uma classe, por outro lado, não cria um membro de classe. Em vez disso, define um tipo que é conhecido apenas localmente para a classe que contém a declaração da classe aninhada.

As razões usuais para aninhar uma classe são auxiliar na implementação de outra classe e evitar conflitos de nomes. O exemplo da classe `Fila` na Listagem 12.10 no Capítulo 12, "Classes e Alocação Dinâmica de Memória", fornece um caso disfarçado de classes aninhadas ao aninhar uma definição de estrutura:

```cpp
class Fila
{
private:
    // definições no escopo de classe
    // No é uma definição de estrutura aninhada local a esta classe
    struct No {Item item; struct No * prox;};
    ...
};
```

Como uma estrutura é uma classe cujos membros são públicos por padrão, `No` é realmente uma classe aninhada. No entanto, essa definição não aproveita as capacidades de classe. Em particular, ela não possui um construtor explícito. Vamos remediar isso agora.

Primeiro, você precisa encontrar onde objetos `No` são criados no exemplo de `Fila`. Examinar a declaração da classe (veja Listagem 12.10) e as definições dos métodos (veja Listagem 12.11) revela que o único lugar onde objetos `No` são criados é no método `enfileirar()`:

```cpp
bool Fila::enfileirar(const Item & item)
{
    if (estaCheia())
        return false;
    No * adicionar = new No;   // cria nó
    // em caso de falha, new lança exceção std::bad_alloc
    adicionar->item = item;    // define ponteiros do nó
    adicionar->prox = NULL;
    ...
}
```

Este código atribui explicitamente valores aos membros de `No` após criar um `No`. Esse é o tipo de trabalho que é mais adequadamente feito por um construtor.

Sabendo agora onde e como um construtor deve ser usado, você pode fornecer uma definição de construtor adequada:

```cpp
class Fila
{
    // definições no escopo de classe
    // No é uma definição de classe aninhada local a esta classe
    class No
    {
    public:
        Item item;
        No * prox;
        No(const Item & i) : item(i), prox(0) { }
    };
    ...
};
```

Este construtor inicializa o membro `item` do nó com `i` e define o ponteiro `prox` como `0`, que é uma maneira de escrever o ponteiro nulo em C++. (Usar `NULL` exigiria incluir um arquivo de cabeçalho que define `NULL`. Usuários de um compilador compatível com C++11 podem usar `nullptr`.) Como todos os nós criados pela classe `Fila` têm `prox` inicialmente definido como ponteiro nulo, este é o único construtor que a classe precisa.

Em seguida, você precisa reescrever `enfileirar()` usando o construtor:

```cpp
bool Fila::enfileirar(const Item & item)
{
    if (estaCheia())
        return false;
    No * adicionar = new No(item);    // cria e inicializa nó
    // em caso de falha, new lança exceção std::bad_alloc
    ...
}
```

Isso torna o código de `enfileirar()` um pouco mais curto e um pouco mais seguro porque automatiza a inicialização em vez de exigir que o programador se lembre corretamente do que deve ser feito.

Este exemplo define o construtor na declaração da classe. Suponha que você quisesse defini-lo em um arquivo de métodos. Nesse caso, a definição deve refletir que a classe `No` é definida dentro da classe `Fila`. Isso é feito usando o operador de resolução de escopo duas vezes:

```cpp
Fila::No::No(const Item & i) : item(i), prox(0) { }
```

### Classes Aninhadas e Acesso

Dois tipos de acesso pertencem às classes aninhadas. Primeiro, onde uma classe aninhada é declarada controla o escopo da classe aninhada; isto é, estabelece quais partes de um programa podem criar objetos dessa classe. Segundo, como com qualquer classe, as seções pública, protegida e privada de uma classe aninhada fornecem controle de acesso aos membros da classe.

**Escopo.** Se uma classe aninhada é declarada em uma seção privada de uma segunda classe, ela é conhecida apenas por essa segunda classe. Isso se aplica, por exemplo, à classe `No` aninhada na declaração de `Fila` no exemplo precedente. Portanto, membros de `Fila` podem usar objetos `No` e ponteiros para objetos `No`, mas outras partes de um programa nem sabem que a classe `No` existe. Se você derivasse uma classe de `Fila`, `No` seria invisível para essa classe também, porque uma classe derivada não pode acessar diretamente as partes privadas de uma classe base.

Se a classe aninhada é declarada em uma seção protegida de uma segunda classe, ela é visível para essa classe, mas invisível para o mundo exterior. No entanto, nesse caso, uma classe derivada saberia sobre a classe aninhada e poderia criar diretamente objetos desse tipo.

Se uma classe aninhada é declarada em uma seção pública de uma segunda classe, ela está disponível para a segunda classe, para classes derivadas da segunda classe e, como é pública, para o mundo exterior. No entanto, como a classe aninhada tem escopo de classe, ela deve ser usada com um qualificador de classe no mundo exterior. Por exemplo, suponha que você tenha esta declaração:

```cpp
class Time
{
public:
    class Tecnico { ... };
    ...
};
```

Agora suponha que você tenha um técnico desempregado, um que não pertence a nenhum time. Para criar um objeto `Tecnico` fora da classe `Time`, você pode usar isto:

```cpp
Time::Tecnico contratado;    // cria um objeto Tecnico fora da classe Time
```

Essas mesmas considerações de escopo se aplicam a estruturas e enumerações aninhadas também. De fato, muitos programadores usam enumerações públicas para fornecer constantes de classe que podem ser usadas por programadores clientes.

**Controle de Acesso.** Declarar a classe `No` na declaração da classe `Fila` não concede à classe `Fila` quaisquer privilégios especiais de acesso à classe `No`, nem concede à classe `No` quaisquer privilégios especiais de acesso à classe `Fila`. Assim, um objeto de classe `Fila` pode acessar apenas os membros públicos de um objeto `No` explicitamente. Por essa razão, o exemplo de `Fila` torna todos os membros da classe `No` públicos. Isso viola a prática usual de tornar os membros de dados privados, mas a classe `No` é uma característica de implementação interna da classe `Fila` e não é visível para o mundo exterior. Isso se deve ao fato de que a classe `No` é declarada na seção privada da classe `Fila`. Assim, embora os métodos de `Fila` possam acessar os membros de `No` diretamente, um cliente usando a classe `Fila` não pode fazê-lo.

Em resumo, a localização de uma declaração de classe determina o escopo ou visibilidade de uma classe. Dado que uma classe particular está no escopo, as regras habituais de controle de acesso (`public`, `protected`, `private`, `friend`) determinam o acesso que um programa tem aos membros da classe aninhada.

### Aninhamento em um Template

Você viu que templates são uma boa escolha para implementar classes contêiner como a classe `Fila`. Você pode estar se perguntando se ter uma classe aninhada representa algum problema ao converter a definição da classe `Fila` em um template. A resposta é não. A Listagem 15.5 mostra como essa conversão pode ser feita. Como é comum para templates de classe, o arquivo de cabeçalho inclui o template de classe, junto com os templates de funções de métodos.

**Listagem 15.5 — filaTp.h**

```cpp
// filaTp.h -- template de fila com uma classe aninhada
#ifndef FILATP_H_
#define FILATP_H_
template <class Item>
class FilaTP
{
private:
    enum {TAM_F = 10};
    // No é uma definição de classe aninhada
    class No
    {
    public:
        Item item;
        No * prox;
        No(const Item & i) : item(i), prox(0){ }
    };
    No * inicio;      // ponteiro para o início da Fila
    No * fim;         // ponteiro para o fim da Fila
    int itens;        // número atual de itens na Fila
    const int tamanhoMax;    // número máximo de itens na Fila
    FilaTP(const FilaTP & f) : tamanhoMax(0) {}
    FilaTP & operator=(const FilaTP & f) { return *this; }
public:
    FilaTP(int t = TAM_F);
    ~FilaTP();
    bool estaVazia() const
    {
        return itens == 0;
    }
    bool estaCheia() const
    {
        return itens == tamanhoMax;
    }
    int contagem() const
    {
        return itens;
    }
    bool enfileirar(const Item & item);    // adiciona item ao fim
    bool desenfileirar(Item & item);       // remove item do início
};

// métodos de FilaTP
template <class Item>
FilaTP<Item>::FilaTP(int t) : tamanhoMax(t)
{
    inicio = fim = 0;
    itens = 0;
}
template <class Item>
FilaTP<Item>::~FilaTP()
{
    No * temp;
    while (inicio != 0)              // enquanto a fila não estiver vazia
    {
        temp = inicio;               // salva o endereço do item do início
        inicio = inicio->prox;       // redefine o ponteiro para o próximo item
        delete temp;                 // exclui o antigo início
    }
}
// Adiciona item à fila
template <class Item>
bool FilaTP<Item>::enfileirar(const Item & item)
{
    if (estaCheia())
        return false;
    No * adicionar = new No(item);    // cria nó
    // em caso de falha, new lança exceção std::bad_alloc
    itens++;
    if (inicio == 0)          // se a fila estiver vazia,
        inicio = adicionar;   // coloca o item no início
    else
        fim->prox = adicionar;    // senão coloca no fim
    fim = adicionar;          // faz fim apontar para o novo nó
    return true;
}
// Coloca o item do início na variável item e remove da fila
template <class Item>
bool FilaTP<Item>::desenfileirar(Item & item)
{
    if (inicio == 0)
        return false;
    item = inicio->item;      // define item para o primeiro item da fila
    itens--;
    No * temp = inicio;       // salva a localização do primeiro item
    inicio = inicio->prox;    // redefine inicio para o próximo item
    delete temp;              // exclui o antigo primeiro item
    if (itens == 0)
        fim = 0;
    return true;
}
#endif
```

Uma coisa interessante sobre o template na Listagem 15.5 é que `No` é definido em termos do tipo genérico `Item`. Assim, uma declaração como a seguinte leva a um `No` definido para conter valores do tipo `double`:

```cpp
FilaTP<double> fd;
```

E a seguinte declaração leva a um `No` definido para conter valores do tipo `char`:

```cpp
FilaTP<char> fc;
```

Essas duas classes `No` são definidas em duas classes `FilaTP` separadas, portanto não há conflito de nomes entre as duas. Ou seja, um nó é do tipo `FilaTP<double>::No`, e o outro é do tipo `FilaTP<char>::No`.

A Listagem 15.6 oferece um programa curto para testar a nova classe.

**Listagem 15.6 — aninhado.cpp**

```cpp
// aninhado.cpp -- usando uma fila que tem uma classe aninhada
#include <iostream>
#include <string>
#include "filaTp.h"
int main()
{
    using std::string;
    using std::cin;
    using std::cout;
    FilaTP<string> cs(5);
    string temp;

    while(!cs.estaCheia())
    {
        cout << "Por favor, informe seu nome. Voce sera "
             "atendido na ordem de chegada.\n"
             "nome: ";
        getline(cin, temp);
        cs.enfileirar(temp);
    }
    cout << "A fila esta cheia. Processamento comecando!\n";
    while (!cs.estaVazia())
    {
        cs.desenfileirar(temp);
        cout << "Agora processando " << temp << "...\n";
    }
    return 0;
}
```

Aqui está uma execução de exemplo do programa nas Listagens 15.5 e 15.6:

```
Por favor, informe seu nome. Voce sera atendido na ordem de chegada.
nome: Kinsey Millhone
Por favor, informe seu nome. Voce sera atendido na ordem de chegada.
nome: Adam Dalgliesh
Por favor, informe seu nome. Voce sera atendido na ordem de chegada.
nome: Andrew Dalziel
Por favor, informe seu nome. Voce sera atendido na ordem de chegada.
nome: Kay Scarpetta
Por favor, informe seu nome. Voce sera atendido na ordem de chegada.
nome: Richard Jury
A fila esta cheia. Processamento comecando!
Agora processando Kinsey Millhone...
Agora processando Adam Dalgliesh...
Agora processando Andrew Dalziel...
Agora processando Kay Scarpetta...
Agora processando Richard Jury...
```

## Exceções (Exceptions)

Programas às vezes encontram problemas em tempo de execução que os impedem de continuar normalmente. Por exemplo, um programa pode tentar abrir um arquivo indisponível, ou pode solicitar mais memória do que está disponível, ou pode encontrar valores que não pode tolerar. Geralmente, programadores tentam antecipar tais calamidades. As exceções de C++ fornecem uma ferramenta poderosa e flexível para lidar com essas situações. As exceções são uma adição relativamente recente ao C++, portanto alguns compiladores mais antigos não as implementaram. Além disso, alguns compiladores desativam esse recurso por padrão, então você pode precisar usar as opções do compilador para ativá-lo.

Antes de examinar exceções, vejamos algumas das opções mais rudimentares disponíveis aos programadores. Como caso de teste, vejamos uma função que calcula a média harmônica de dois números. A média harmônica de dois números é definida como o inverso da média dos inversos. Isso pode ser reduzido à seguinte expressão:

```
2.0 × x × y / (x + y)
```

Observe que se y for o negativo de x, essa fórmula resulta em divisão por zero, uma operação bastante indesejável. Muitos compiladores mais novos tratam a divisão por zero gerando um valor especial de ponto flutuante que representa infinito; `cout` exibe esse valor como `Inf`, `inf`, `INF` ou algo semelhante. Outros compiladores podem produzir programas que travam quando ocorre divisão por zero. É melhor escrever código que se comporte da mesma maneira controlada em todos os sistemas.

### Chamando abort()

Uma maneira de lidar com isso é fazer a função chamar a função `abort()` se um argumento for o negativo do outro. A função `abort()` tem seu protótipo no arquivo de cabeçalho `cstdlib` (ou `stdlib.h`). Uma implementação típica, se chamada, envia uma mensagem como "terminação anormal do programa" para o fluxo de erro padrão (o mesmo usado por `cerr`) e encerra o programa. Ela também retorna um valor dependente da implementação que indica falha ao sistema operacional ou, se o programa foi iniciado por outro programa, ao processo pai. Se `abort()` libera buffers de arquivo (isto é, áreas de memória usadas para armazenar material para transferências de e para arquivos) depende da implementação. Se preferir, você pode usar `exit()`, que libera buffers de arquivo, mas sem exibir uma mensagem. A Listagem 15.7 mostra um programa curto que usa `abort()`.

**Listagem 15.7 — erro1.cpp**

```cpp
// erro1.cpp -- usando a função abort()
#include <iostream>
#include <cstdlib>
double mharm(double a, double b);
int main()
{
    double x, y, z;
    std::cout << "Digite dois numeros: ";
    while (std::cin >> x >> y)
    {
        z = mharm(x, y);
        std::cout << "Media harmonica de " << x << " e " << y
                  << " eh " << z << std::endl;
        std::cout << "Digite o proximo par de numeros <q para sair>: ";
    }
    std::cout << "Ate logo!\n";
    return 0;
}
double mharm(double a, double b)
{
    if (a == -b)
    {
        std::cout << "argumentos inviáveis para mharm()\n";
        std::abort();
    }
    return 2.0 * a * b / (a + b);
}
```

Aqui está uma execução de exemplo do programa na Listagem 15.7:

```
Digite dois numeros: 3 6
Media harmonica de 3 e 6 eh 4
Digite o proximo par de numeros <q para sair>: 10 -10
argumentos inviáveis para mharm()
terminação anormal do programa
```

Observe que chamar a função `abort()` de `mharm()` encerra o programa diretamente, sem retornar primeiro para `main()`. Em geral, diferentes compiladores emitem mensagens de abortamento diferentes. Aqui está outra mensagem desse tipo:

```
This application has requested the Runtime to terminate it
in an unusual way. Please contact the application's support
team for more information.
```

O programa poderia evitar abortar verificando os valores de `x` e `y` antes de chamar a função `mharm()`. No entanto, não é seguro depender de um programador para saber (ou se importar) o suficiente para realizar tal verificação.

### Retornando um Código de Erro

Uma abordagem mais flexível do que abortar é usar o valor de retorno de uma função para indicar um problema. Por exemplo, o membro `get(void)` da classe `ostream` ordinariamente retorna o código ASCII para o próximo caractere de entrada, mas retorna o valor especial `EOF` se encontrar o fim do arquivo. Essa abordagem não funciona para `mharm()`. Qualquer valor numérico poderia ser um valor de retorno válido, portanto não há valor especial disponível para indicar um problema. Nesse tipo de situação, você pode usar um argumento de ponteiro ou um argumento de referência para obter um valor de volta para o programa chamador e usar o valor de retorno da função para indicar sucesso ou falha. A família de operadores sobrecarregados `>>` de `istream` usa uma variante dessa técnica. Informando ao programa chamador sobre o sucesso ou falha, você dá ao programa a opção de tomar ações diferentes de abortar. A Listagem 15.8 mostra um exemplo dessa abordagem. Ela redefine `mharm()` como uma função `bool` cujo valor de retorno indica sucesso ou falha. Ela adiciona um terceiro argumento para obter a resposta.

**Listagem 15.8 — erro2.cpp**

```cpp
// erro2.cpp -- retornando um código de erro
#include <iostream>
#include <cfloat>    // (ou float.h) para DBL_MAX
bool mharm(double a, double b, double * ans);

int main()
{
    double x, y, z;
    std::cout << "Digite dois numeros: ";
    while (std::cin >> x >> y)
    {
        if (mharm(x, y, &z))
            std::cout << "Media harmonica de " << x << " e " << y
                      << " eh " << z << std::endl;
        else
            std::cout << "Um valor nao deve ser o negativo "
                      << "do outro - tente novamente.\n";
        std::cout << "Digite o proximo par de numeros <q para sair>: ";
    }
    std::cout << "Ate logo!\n";
    return 0;
}
bool mharm(double a, double b, double * ans)
{
    if (a == -b)
    {
        *ans = DBL_MAX;
        return false;
    }
    else
    {
        *ans = 2.0 * a * b / (a + b);
        return true;
    }
}
```

Aqui está uma execução de exemplo do programa na Listagem 15.8:

```
Digite dois numeros: 3 6
Media harmonica de 3 e 6 eh 4
Digite o proximo par de numeros <q para sair>: 10 -10
Um valor nao deve ser o negativo do outro - tente novamente.
Digite o proximo par de numeros <q para sair>: 1 19
Media harmonica de 1 e 19 eh 1.9
Digite o proximo par de numeros <q para sair>: q
Ate logo!
```

**Notas do Programa.** Na Listagem 15.8, o design do programa permite ao usuário continuar, ignorando os efeitos de entradas ruins. Claro, o design depende do usuário verificar o valor de retorno da função, o que os programadores nem sempre fazem. Você poderia usar um ponteiro ou uma referência para o terceiro argumento. Muitos programadores preferem usar ponteiros para argumentos dos tipos internos, porque isso torna óbvio qual argumento está sendo usado para a resposta.

### O Mecanismo de Exceção

Agora vejamos como você pode lidar com problemas usando o mecanismo de exceção. Uma exceção (exception) de C++ é uma resposta a uma circunstância excepcional que surge enquanto um programa está em execução, como uma tentativa de divisão por zero. As exceções fornecem uma maneira de transferir o controle de uma parte de um programa para outra. O tratamento de uma exceção tem três componentes:

- Lançar uma exceção (throwing)
- Capturar uma exceção com um handler (catching)
- Usar um bloco `try`

Um programa lança (throws) uma exceção quando um problema aparece. Por exemplo, você pode modificar `mharm()` na Listagem 15.7 para lançar uma exceção em vez de chamar a função `abort()`. Uma instrução `throw`, em essência, é um salto; ou seja, ela diz ao programa para saltar para instruções em outro local. A palavra-chave `throw` indica o lançamento de uma exceção. É seguida por um valor, como uma string de caracteres ou um objeto, que indica a natureza da exceção.

Um programa captura (catches) uma exceção com um tratador de exceção (exception handler) no lugar do programa onde você quer tratar o problema. A palavra-chave `catch` indica a captura de uma exceção. Um handler começa com a palavra-chave `catch`, seguida por uma declaração de tipo (entre parênteses) que indica o tipo de exceção ao qual ele responde. Isso, por sua vez, é seguido por um bloco de código entre chaves que indica as ações a tomar. A palavra-chave `catch`, junto com o tipo de exceção, serve como um rótulo que identifica o ponto no programa para o qual a execução deve saltar quando uma exceção é lançada. Um tratador de exceção também é chamado de bloco `catch`.

Um bloco `try` identifica um bloco de código para o qual exceções específicas serão ativadas. É seguido por um ou mais blocos `catch`. O próprio bloco `try` é indicado pela palavra-chave `try`, seguida por um bloco de código entre chaves indicando o código para o qual as exceções serão observadas.

**Listagem 15.9 — erro3.cpp**

```cpp
// erro3.cpp -- usando uma exceção
#include <iostream>
double mharm(double a, double b);

int main()
{
    double x, y, z;
    std::cout << "Digite dois numeros: ";
    while (std::cin >> x >> y)
    {
        try {          // início do bloco try
            z = mharm(x, y);
        }              // fim do bloco try
        catch (const char * s)    // início do handler de exceção
        {
            std::cout << s << std::endl;
            std::cout << "Digite um novo par de numeros: ";
            continue;
        }              // fim do handler
        std::cout << "Media harmonica de " << x << " e " << y
                  << " eh " << z << std::endl;
        std::cout << "Digite o proximo par de numeros <q para sair>: ";
    }
    std::cout << "Ate logo!\n";
    return 0;
}
double mharm(double a, double b)
{
    if (a == -b)
        throw "argumentos invalidos para mharm(): a = -b nao permitido";
    return 2.0 * a * b / (a + b);
}
```

Aqui está uma execução de exemplo do programa na Listagem 15.9:

```
Digite dois numeros: 3 6
Media harmonica de 3 e 6 eh 4
Digite o proximo par de numeros <q para sair>: 10 -10
argumentos invalidos para mharm(): a = -b nao permitido
Digite um novo par de numeros: 1 19
Media harmonica de 1 e 19 eh 1.9
Digite o proximo par de numeros <q para sair>: q
Ate logo!
```

**Notas do Programa.** O bloco `try` na Listagem 15.9 parece assim:

```cpp
try {         // início do bloco try
    z = mharm(x, y);
}             // fim do bloco try
```

Se alguma instrução nesse bloco levar ao lançamento de uma exceção, os blocos `catch` após esse bloco tratarão a exceção. Se o programa chamar `mharm()` em outro lugar fora deste (e de qualquer outro) bloco `try`, ele não terá oportunidade de tratar uma exceção.

Lançar uma exceção parece assim:

```cpp
if (a == -b)
    throw "argumentos invalidos para mharm(): a = -b nao permitido";
```

Executar o `throw` é um pouco como executar uma instrução `return` em que ele termina a execução da função. No entanto, em vez de retornar o controle ao programa chamador, um `throw` faz o programa recuar através da sequência de chamadas de função atuais até encontrar a função que contém o bloco `try`. Na Listagem 15.9, essa função é a mesma que a função chamadora. Em breve você verá um exemplo envolvendo recuar mais de uma função. Enquanto isso, neste caso, o `throw` passa o controle do programa de volta para `main()`. Lá, o programa procura um handler de exceção (seguindo o bloco `try`) que corresponda ao tipo de exceção lançada.

O handler, ou bloco `catch`, parece assim:

```cpp
catch (const char * s)    // início do handler de exceção
{
    std::cout << s << std::endl;
    std::cout << "Digite um novo par de numeros: ";
    continue;
}                         // fim do handler
```

O bloco `catch` se parece um pouco com uma definição de função, mas não é. A palavra-chave `catch` identifica isso como um handler, e `const char * s` significa que este handler corresponde a uma exceção lançada que é uma string. Esta declaração de `s` age muito como uma definição de argumento de função, pois uma exceção lançada correspondente é atribuída a `s`. Além disso, se uma exceção corresponder a este handler, o programa executa o código dentro das chaves.

Se um programa completa a execução de instruções em um bloco `try` sem que nenhuma exceção seja lançada, ele pula o bloco `catch` ou blocos após o bloco `try` e vai para a primeira instrução após os handlers.

Você pode estar se perguntando o que acontece se uma função lança uma exceção e não há bloco `try` ou não há handler correspondente. Por padrão, o programa acaba chamando a função `abort()`, mas você pode modificar esse comportamento. Voltaremos a este tópico mais adiante neste capítulo.

### Usando Objetos como Exceções

Tipicamente, funções que lançam exceções lançam objetos. Uma vantagem importante disso é que você pode usar diferentes tipos de exceção para distinguir entre diferentes funções e situações que produzem exceções. Além disso, um objeto pode carregar informações com ele, e você pode usar essas informações para ajudar a identificar as condições que causaram o lançamento da exceção. Aqui, por exemplo, está um possível design para uma exceção a ser lançada pela função `mharm()`:

```cpp
class erro_mharm
{
private:
    double v1;
    double v2;
public:
    erro_mharm(double a = 0, double b = 0) : v1(a), v2(b){}
    void msg();
};
inline void erro_mharm::msg()
{
    std::cout << "mharm(" << v1 << ", " << v2 << "): "
              << "argumentos invalidos: a = -b\n";
}
```

Um objeto `erro_mharm` pode ser inicializado para os valores passados a `mharm()`, e o método `msg()` pode ser usado para reportar o problema, incluindo os valores. A função `mharm()` pode usar código como este:

```cpp
if (a == -b)
    throw erro_mharm(a, b);
```

Isso chama o construtor de `erro_mharm()`, inicializando o objeto para conter os valores dos argumentos.

As Listagens 15.10 e 15.11 adicionam uma segunda classe de exceção, `erro_mgeom`, e uma segunda função, chamada `mgeom()`, que lança uma exceção `erro_mgeom`. A função `mgeom()` calcula a média geométrica de dois números, que é a raiz quadrada de seu produto. Esta função é definida se ambos os argumentos são não-negativos, então ela lança uma exceção se detectar argumentos negativos. A Listagem 15.10 é um arquivo de cabeçalho que contém as definições das classes de exceção, e a Listagem 15.11 é um programa de exemplo que usa esse arquivo de cabeçalho. Observe que o bloco `try` é seguido por dois blocos `catch` consecutivos:

```cpp
try {         // início do bloco try
    ...
}// fim do bloco try
catch (erro_mharm & bg)    // início do bloco catch
{
    ...
}
catch (erro_mgeom & hg)
{
    ...
}             // fim do bloco catch
```

Se, digamos, `mharm()` lançar uma exceção `erro_mharm`, o primeiro bloco `catch` a captura. Se `mgeom()` lançar uma exceção `erro_mgeom`, a exceção passa pelo primeiro bloco `catch` e é capturada pelo segundo.

**Listagem 15.10 — exc_media.h**

```cpp
// exc_media.h -- classes de exceção para mharm(), mgeom()
#include <iostream>
class erro_mharm
{
private:
    double v1;
    double v2;
public:
    erro_mharm(double a = 0, double b = 0) : v1(a), v2(b){}
    void msg();
};
inline void erro_mharm::msg()
{
    std::cout << "mharm(" << v1 << ", " << v2 << "): "
              << "argumentos invalidos: a = -b\n";
}

class erro_mgeom
{
public:
    double v1;
    double v2;
    erro_mgeom(double a = 0, double b = 0) : v1(a), v2(b){}
    const char * msg();
};
inline const char * erro_mgeom::msg()
{
    return "argumentos de mgeom() devem ser >= 0\n";
}
```

**Listagem 15.11 — erro4.cpp**

```cpp
// erro4.cpp -- usando classes de exceção
#include <iostream>
#include <cmath>    // ou math.h, usuários unix podem precisar da flag -lm
#include "exc_media.h"
// protótipos de função
double mharm(double a, double b);
double mgeom(double a, double b);
int main()
{
    using std::cout;
    using std::cin;
    using std::endl;
    double x, y, z;
    cout << "Digite dois numeros: ";
    while (cin >> x >> y)
    {
        try {         // início do bloco try
            z = mharm(x, y);
            cout << "Media harmonica de " << x << " e " << y
                 << " eh " << z << endl;
            cout << "Media geometrica de " << x << " e " << y
                 << " eh " << mgeom(x, y) << endl;
            cout << "Digite o proximo par de numeros <q para sair>: ";
        }// fim do bloco try
        catch (erro_mharm & bg)    // início do bloco catch
        {
            bg.msg();
            cout << "Tente novamente.\n";
            continue;
        }
        catch (erro_mgeom & hg)
        {
            cout << hg.msg();
            cout << "Valores usados: " << hg.v1 << ", "
                 << hg.v2 << endl;
            cout << "Desculpe, voce nao pode mais jogar.\n";
            break;
        }    // fim do bloco catch
    }
    cout << "Ate logo!\n";
    return 0;
}
double mharm(double a, double b)
{
    if (a == -b)
        throw erro_mharm(a, b);
    return 2.0 * a * b / (a + b);
}
double mgeom(double a, double b)
{
    if (a < 0 || b < 0)
        throw erro_mgeom(a, b);
    return std::sqrt(a * b);
}
```

Aqui está uma execução de exemplo do programa, uma que é encerrada por entrada ruim para a função `mgeom()`:

```
Digite dois numeros: 4 12
Media harmonica de 4 e 12 eh 6
Media geometrica de 4 e 12 eh 6.9282
Digite o proximo par de numeros <q para sair>: 5 -5
mharm(5, -5): argumentos invalidos: a = -b
Tente novamente.
5 -2
Media harmonica de 5 e -2 eh -6.66667
argumentos de mgeom() devem ser >= 0
Valores usados: 5, -2
Desculpe, voce nao pode mais jogar.
Ate logo!
```

Um ponto a notar é que o handler de `erro_mharm` usa uma instrução `continue`, enquanto o handler de `erro_mgeom` usa uma instrução `break`. Assim, entrada ruim para `mharm()` leva o programa a pular o restante do loop e iniciar o próximo ciclo de loop. Mas entrada ruim para `mgeom()` termina o loop. Isso ilustra como um programa pode determinar qual exceção é lançada (pelo tipo de exceção) e adequar a resposta à exceção.

Um segundo ponto a notar é que o design de `erro_mgeom` ilustra técnicas diferentes das que `erro_mharm` usa. Em particular, `erro_mgeom` usa dados públicos e um método que retorna uma string de estilo C.

### Especificações de Exceção e C++11

Às vezes uma ideia que parece promissora em princípio não funciona tão bem na prática. Esse é o caso das especificações de exceção (exception specifications), um recurso adicionado pelo C++98 e então reprovado (deprecated) pelo C++11. Isso significa que o recurso ainda está no padrão, mas pode não estar no futuro, então você é encorajado a não usá-lo.

No entanto, antes de ignorar as especificações de exceção, você pelo menos deveria saber como elas parecem. Elas parecem assim:

```cpp
double prejudicio(double a) throw(coisa_ruim);    // pode lançar exceção coisa_ruim
double marma(double) throw();                      // não lança uma exceção
```

A parte `throw()`, com ou sem uma lista de tipos, é a especificação de exceção, e ela apareceria tanto no protótipo quanto na definição da função.

Uma razão para as especificações de exceção era alertar o usuário sobre a possível necessidade de um bloco `try`. No entanto, isso pode ser realizado tão facilmente com um comentário. Uma segunda razão era permitir que o compilador adicionasse código para fazer verificações em tempo de execução para ver se a especificação de exceção foi violada. O consenso que se desenvolveu na comunidade de programação, particularmente entre os que trabalharam mais diligentemente na escrita de código seguro para exceções, foi que esse recurso é melhor ignorado. E agora você também pode ignorar as especificações de exceção com as bençãos do C++11.

No entanto, C++11 permite uma especificação especial — a nova palavra-chave `noexcept` pode ser usada para indicar uma função que não lança uma exceção:

```cpp
double marma() noexcept;    // marma() não lança uma exceção
```

Há algum debate sobre a necessidade e utilidade dessa especificação, com alguns sentindo que é melhor evitar usá-la (pelo menos na maioria dos casos). Mas outros sentiram fortemente o suficiente sobre a necessidade de introduzir uma nova palavra-chave. Pensa-se que saber que uma função não deve lançar uma exceção pode ajudar o compilador a otimizar o código. Esse uso deve ser pensado como uma promessa feita pelo programador da função.

Há também um operador `noexcept()` (veja Apêndice E) que reporta se seu operando poderia ou não lançar uma exceção.

### Desdoblando a Pilha (Unwinding the Stack)

Suponha que um bloco `try` não contenha uma chamada direta a uma função que lança uma exceção, mas que ele chame uma função que chama uma função que lança uma exceção. A execução ainda salta da função na qual a exceção é lançada para a função que contém o bloco `try` e os handlers. Fazer isso envolve desdobrar a pilha (unwinding the stack).

Primeiro, vejamos como C++ normalmente lida com chamadas e retornos de funções. C++ tipicamente lida com chamadas de função colocando informações em uma pilha. Em particular, um programa coloca o endereço de uma instrução de função chamadora (um endereço de retorno) na pilha. Quando a função chamada completa, o programa usa esse endereço para determinar onde continuar com a execução do programa. Além disso, a chamada de função coloca quaisquer argumentos de função na pilha, onde são tratados como variáveis automáticas. Se a função chamada criar quaisquer novas variáveis automáticas, elas também são adicionadas à pilha. Se uma função chamada chamar outra função, suas informações são adicionadas à pilha, e assim por diante. Quando uma função termina, a execução do programa passa para o endereço armazenado quando a função foi chamada, e o topo da pilha é liberado. Assim, uma função normalmente retorna para a função que a chamou, com cada função liberando suas variáveis automáticas ao terminar. Se uma variável automática é um objeto de classe, então o destrutor da classe, se houver, é chamado.

Agora suponha que uma função termine via uma exceção lançada em vez de via uma chamada de retorno. De novo, o programa libera memória da pilha. Mas em vez de parar no primeiro endereço de retorno na pilha, o programa continua liberando a pilha até que chegue a um endereço de retorno que resida em um bloco `try`. O controle então passa para os handlers de exceção no final do bloco, em vez de para a primeira instrução após a chamada de função. Esse processo é chamado de desdobramento da pilha (unwinding the stack). Uma característica muito importante do mecanismo `throw` é que, assim como com retornos de função, os destrutores de classe são chamados para quaisquer objetos de classe automáticos na pilha. No entanto, um retorno de função apenas processa objetos colocados na pilha por aquela função, enquanto a instrução `throw` processa objetos colocados na pilha por toda a sequência de chamadas de função entre o bloco `try` e o `throw`. Sem o recurso de desdobramento da pilha, um `throw` deixaria destruidores sem ser chamados para objetos de classe automáticos colocados na pilha por chamadas de função intermediárias.

A Listagem 15.12 fornece um exemplo de desdobramento da pilha. Nela, `main()` chama `medias()`, que por sua vez chama `mharm()` e `mgeom()`. A função `medias()`, por falta de algo melhor a fazer, calcula a média das médias aritmética, harmônica e geométrica. Tanto `main()` quanto `medias()` criam objetos do tipo `demo` (uma classe tagarela que anuncia quando seu construtor e destrutor são usados) para que você possa ver o que acontece com esses objetos quando exceções são lançadas. O bloco `try` em `main()` captura tanto exceções `erro_mharm` quanto `erro_mgeom`, e o bloco `try` em `medias()` captura apenas a exceção `erro_mharm`. Este bloco `catch` tem o seguinte código:

```cpp
catch (erro_mharm & bg)    // início do bloco catch
{
    bg.msg();
    std::cout << "Capturado em medias()\n";
    throw;     // relança a exceção
}
```

Após o código responder exibindo mensagens, ele relança (rethrows) a exceção, o que significa, nesse caso, enviar a exceção para cima para `main()`. (Em geral, uma exceção relançada sobe para a próxima combinação try-catch que captura aquele tipo específico de exceção. Se nenhum handler for encontrado, o programa, por padrão, aborta.) A Listagem 15.12 usa o mesmo arquivo de cabeçalho (`exc_media.h` da Listagem 15.10) que a Listagem 15.11.

**Listagem 15.12 — erro5.cpp**

```cpp
// erro5.cpp -- desdobrando a pilha
#include <iostream>
#include <cmath>    // ou math.h, usuários unix podem precisar da flag -lm
#include <string>
#include "exc_media.h"
class demo
{
private:
    std::string palavra;
public:
    demo(const std::string & str)
    {
        palavra = str;
        std::cout << "demo " << palavra << " criado\n";
    }
    ~demo()
    {
        std::cout << "demo " << palavra << " destruido\n";
    }
    void exibir() const
    {
        std::cout << "demo " << palavra << " vive!\n";
    }
};
// protótipos de função
double mharm(double a, double b);
double mgeom(double a, double b);
double medias(double a, double b);
int main()
{
    using std::cout;
    using std::cin;
    using std::endl;
    double x, y, z;
    {
        demo d1("encontrado no bloco em main()");
        cout << "Digite dois numeros: ";
        while (cin >> x >> y)
        {
            try {         // início do bloco try
                z = medias(x, y);
                cout << "A media das medias de " << x << " e " << y
                     << " eh " << z << endl;
                cout << "Digite o proximo par: ";
            }             // fim do bloco try
            catch (erro_mharm & bg)    // início do bloco catch
            {
                bg.msg();
                cout << "Tente novamente.\n";
                continue;
            }
            catch (erro_mgeom & hg)
            {
                cout << hg.msg();
                cout << "Valores usados: " << hg.v1 << ", "
                     << hg.v2 << endl;
                cout << "Desculpe, voce nao pode mais jogar.\n";
                break;
            }             // fim do bloco catch
        }
        d1.exibir();
    }
    cout << "Ate logo!\n";
    cin.get();
    cin.get();
    return 0;
}
double mharm(double a, double b)
{
    if (a == -b)
        throw erro_mharm(a, b);
    return 2.0 * a * b / (a + b);
}
double mgeom(double a, double b)
{
    if (a < 0 || b < 0)
        throw erro_mgeom(a, b);
    return std::sqrt(a * b);
}
double medias(double a, double b)
{
    double am, hm, gm;
    demo d2("encontrado em medias()");
    am = (a + b) / 2.0;    // média aritmética
    try
    {
        hm = mharm(a, b);
        gm = mgeom(a, b);
    }
    catch (erro_mharm & bg)    // início do bloco catch
    {
        bg.msg();
        std::cout << "Capturado em medias()\n";
        throw;     // relança a exceção
    }
    d2.exibir();
    return (am + hm + gm) / 3.0;
}
```

Aqui está uma execução de exemplo do programa:

```
demo encontrado no bloco em main() criado
Digite dois numeros: 6 12
demo encontrado em medias() criado
demo encontrado em medias() vive!
demo encontrado em medias() destruido
A media das medias de 6 e 12 eh 8.49509
6 -6
demo encontrado em medias() criado
mharm(6, -6): argumentos invalidos: a = -b
Capturado em medias()
demo encontrado em medias() destruido
mharm(6, -6): argumentos invalidos: a = -b
Tente novamente.
6 -8
demo encontrado em medias() criado
demo encontrado em medias() destruido
argumentos de mgeom() devem ser >= 0
Valores usados: 6, -8
Desculpe, voce nao pode mais jogar.
demo encontrado no bloco em main() vive!
demo encontrado no bloco em main() destruido
Ate logo!
```

---

[Anterior](capitulo-14-04-resumo.md) | [Índice](README.md) | [Próximo](capitulo-15-02-excecoes-avancadas.md)
