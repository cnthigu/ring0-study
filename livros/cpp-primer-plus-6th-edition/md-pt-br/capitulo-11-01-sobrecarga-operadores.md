# Capítulo 11 — Trabalhando com Classes (Working with Classes)

> Tradução não oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Navegação: [Anterior](capitulo-10-03-escopo-adt-resumo.md) | [Índice](README.md) | [Próximo](capitulo-11-02-funcoes-amigas-vetores.md)

---

Neste capítulo você aprenderá sobre:

- Sobrecarga de operadores (operator overloading)
- Funções amigas (friend functions)
- Sobrecarga do operador `<<` para saída
- Membros de estado (state members)
- Uso de `rand()` para gerar valores aleatórios
- Conversões automáticas e type casts para classes
- Funções de conversão de classe (class conversion functions)

As classes C++ são ricas em recursos, complexas e poderosas. No Capítulo 10, "Objetos e Classes," você iniciou a jornada em direção à programação orientada a objetos aprendendo a definir e usar uma classe simples. Viu como uma classe define um tipo de dado especificando o tipo de dados a ser usado para representar um objeto e, por meio de funções membro, as operações que podem ser realizadas com esses dados. Também aprendeu sobre duas funções membro especiais — o construtor e o destrutor — que gerenciam a criação e o descarte de objetos conforme uma especificação de classe. Este capítulo o leva alguns passos adiante na exploração das propriedades das classes, concentrando-se em técnicas de projeto de classes em vez de princípios gerais. Provavelmente você achará alguns dos recursos aqui abordados diretos e outros um pouco mais sutis. Para melhor entender esses novos recursos, experimente os exemplos: O que acontece se você usar um argumento regular em vez de um argumento por referência nessa função? O que acontece se você deixar algo de fora de um destrutor? Não tenha medo de cometer erros; geralmente você aprende mais desvendando um erro do que fazendo algo corretamente mas de forma mecânica. No final, você será recompensado com uma compreensão mais completa de como o C++ funciona e do que ele pode fazer por você.

Este capítulo começa com a sobrecarga de operadores, que permite usar operadores C++ padrão como `=` e `+` com objetos de classes. Em seguida examina as funções amigas, o mecanismo C++ para permitir que funções não membro acessem dados privados. Por fim, mostra como instruir o C++ a realizar conversões automáticas de tipo com classes. À medida que você avança neste capítulo e no Capítulo 12, "Classes e Alocação Dinâmica de Memória," obterá uma maior apreciação dos papéis que construtores e destrutores desempenham. Também verá alguns dos estágios pelos quais pode passar ao desenvolver e aprimorar um projeto de classe.

Uma dificuldade em aprender C++, pelo menos quando você chegou tão longe no assunto, é que há muita coisa a lembrar. E é razoável não esperar lembrar tudo até acumular experiência suficiente sobre a qual ancorar suas memórias. Aprender C++, nesse aspecto, é como aprender um processador de texto ou planilha repleto de recursos. Nenhum recurso individual é tão assustador, mas na prática a maioria das pessoas conhece bem apenas os recursos que usa regularmente. Provavelmente a melhor abordagem para absorver a riqueza de material neste capítulo é começar a incorporar apenas alguns desses novos recursos em sua própria programação C++. À medida que suas experiências aprimoram sua compreensão e apreciação desses recursos, você pode começar a adicionar outros recursos C++. Como Bjarne Stroustrup, o criador do C++, sugeriu em uma conferência C++ para programadores profissionais: "Entre suavemente na linguagem. Não se sinta obrigado a usar *todos* os recursos, e não tente usá-los todos no primeiro dia."

## Sobrecarga de Operadores (Operator Overloading)

Vamos olhar para uma técnica que dá às operações sobre objetos uma aparência mais agradável. A *sobrecarga de operadores* (operator overloading) é um exemplo de polimorfismo em C++. No Capítulo 8, "Aventuras em Funções," você viu como o C++ permite definir diversas funções com o mesmo nome, desde que tenham assinaturas diferentes (listas de argumentos). Isso é chamado de *sobrecarga de função* (function overloading) ou polimorfismo funcional. Seu propósito é deixar você usar o mesmo nome de função para a mesma operação básica, mesmo que você a aplique a tipos de dados diferentes. A sobrecarga de operadores estende o conceito de sobrecarga aos operadores, permitindo que você atribua múltiplos significados aos operadores C++. Na verdade, muitos operadores C++ (e C) já são sobrecarregados. Por exemplo, o operador `*`, quando aplicado a um endereço, produz o valor armazenado naquele endereço. Mas aplicar `*` a dois números produz o produto dos valores. O C++ usa o número e o tipo dos operandos para decidir qual ação tomar.

O C++ permite estender a sobrecarga de operadores a tipos definidos pelo usuário, permitindo, digamos, usar o símbolo `+` para somar dois objetos. Novamente, o compilador usa o número e o tipo dos operandos para determinar qual definição de adição usar. Operadores sobrecarregados muitas vezes podem tornar o código mais natural. Por exemplo, uma tarefa de computação comum é somar dois arrays. Normalmente isso resulta em um laço `for` como o seguinte:

```cpp
for (int i = 0; i < 20; i++)
    noite[i] = sam[i] + janet[i]; // somar elemento a elemento
```

Mas em C++ você pode definir uma classe que representa arrays e que sobrecarrega o operador `+` para que você possa fazer isso:

```cpp
noite = sam + janet; // somar dois objetos array
```

Essa notação de adição simples oculta a mecânica e enfatiza o que é essencial — e esse é outro objetivo da POO.

Para sobrecarregar um operador, você usa uma forma especial de função chamada *função operadora* (operator function). Uma função operadora tem a seguinte forma, onde *op* é o símbolo do operador sendo sobrecarregado:

```
operatorop(lista-de-argumentos)
```

Por exemplo, `operator+()` sobrecarrega o operador `+` e `operator*()` sobrecarrega o operador `*`. O *op* deve ser um operador C++ válido; você não pode inventar um novo símbolo. Por exemplo, não pode ter uma função `operator@()` porque C++ não tem o operador `@`. Mas a função `operator[]()` sobrecarregaria o operador `[]`, porque `[]` é o operador de indexação de array. Suponha, por exemplo, que você tem uma classe `Vendedor` para a qual define uma função membro `operator+()` para sobrecarregar o operador `+` de forma que ele soma as figuras de vendas de um objeto vendedor a outro. Então, se `regiao2`, `sid` e `sara` são todos objetos da classe `Vendedor`, você pode escrever esta equação:

```cpp
regiao2 = sid + sara;
```

O compilador, reconhecendo os operandos como pertencentes à classe `Vendedor`, substitui o operador pela chamada de função operadora correspondente:

```cpp
regiao2 = sid.operator+(sara);
```

A função então usa o objeto `sid` implicitamente (porque ela invocou o método) e o objeto `sara` explicitamente (porque é passado como argumento) para calcular a soma, que ela então retorna. Claro, a parte agradável é que você pode usar a notação com o operador `+` em vez da notação de função mais desajeitada.

O C++ impõe algumas restrições à sobrecarga de operadores, mas é mais fácil entendê-las depois de ver como funciona a sobrecarga. Então vamos desenvolver alguns exemplos para esclarecer o processo e depois discutir as limitações.

## O Tempo em Nossas Mãos: Desenvolvendo um Exemplo de Sobrecarga de Operadores

Se você trabalhou na conta Priggs por 2 horas 35 minutos pela manhã e 2 horas 40 minutos à tarde, por quanto tempo trabalhou no total na conta? Aqui está um exemplo em que o conceito de adição faz sentido, mas as unidades que você está somando (uma mistura de horas e minutos) não correspondem a um tipo interno. O Capítulo 7, "Funções: Módulos de Programação do C++," lida com um caso similar definindo uma estrutura `tempo_viagem` e uma função `soma()` para somar tais estruturas. Agora vamos generalizar isso para uma classe `Tempo`, usando um método para lidar com a adição. Vamos começar com um método comum, chamado `Soma()`, e então ver como convertê-lo em um operador sobrecarregado. O Listagem 11.1 mostra a declaração da classe.

**Listagem 11.1 — mytime0.h**

```cpp
// mytime0.h — classe Tempo antes da sobrecarga de operadores
#ifndef MYTIME0_H_
#define MYTIME0_H_
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
    Tempo Soma(const Tempo & t) const;
    void Exibir() const;
};
#endif
```

A classe `Tempo` fornece métodos para ajustar e redefinir tempos, para exibir valores de tempo e para somar dois tempos. A Listagem 11.2 mostra as definições dos métodos; observe como os métodos `AdicionarMin()` e `Soma()` usam divisão inteira e o operador módulo para ajustar os valores de `minutos` e `horas` quando o total de minutos excede 59. Também, como o único recurso de `iostream` usado é `cout` e é usado apenas uma vez, parece econômico usar `std::cout` em vez de trazer todo o namespace `std` para escopo.

**Listagem 11.2 — mytime0.cpp**

```cpp
// mytime0.cpp — implementando os métodos de Tempo
#include <iostream>
#include "mytime0.h"
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
Tempo Tempo::Soma(const Tempo & t) const
{
    Tempo soma;
    soma.minutos = minutos + t.minutos;
    soma.horas = horas + t.horas + soma.minutos / 60;
    soma.minutos %= 60;
    return soma;
}
void Tempo::Exibir() const
{
    std::cout << horas << " horas, " << minutos << " minutos";
}
```

Considere o código da função `Soma()`. Observe que o argumento é uma referência, mas o tipo de retorno não é uma referência. O motivo de tornar o argumento uma referência é eficiência — o código produziria os mesmos resultados se o objeto `Tempo` fosse passado por valor, mas geralmente é mais rápido e mais eficiente em memória apenas passar uma referência.

Entretanto, o valor de retorno não pode ser uma referência. O motivo é que a função cria um novo objeto `Tempo` (`soma`) que representa a soma dos outros dois objetos `Tempo`. Retornar o objeto, como faz este código, cria uma cópia do objeto que a função chamadora pode usar. Se o tipo de retorno fosse `Tempo &`, a referência seria para o objeto `soma`. Mas `soma` é uma variável local e é destruída quando a função termina, portanto a referência seria uma referência a um objeto inexistente. Usar o tipo de retorno `Tempo`, entretanto, significa que o programa constrói uma cópia de `soma` antes de destruí-lo, e a função chamadora recebe a cópia.

> **Cuidado**
> Não retorne uma referência a uma variável local ou outro objeto temporário. Quando a função termina e a variável local ou objeto temporário desaparece, a referência torna-se uma referência a dados inexistentes.

Por fim, a Listagem 11.3 testa a parte de soma de tempo da classe `Tempo`.

**Listagem 11.3 — usetime0.cpp**

```cpp
// usetime0.cpp — usando a primeira versão da classe Tempo
// compilar usetime0.cpp e mytime0.cpp juntos
#include <iostream>
#include "mytime0.h"
int main()
{
    using std::cout;
    using std::endl;
    Tempo planejamento;
    Tempo codificacao(2, 40);
    Tempo correcao(5, 55);
    Tempo total;
    cout << "tempo de planejamento = ";
    planejamento.Exibir();
    cout << endl;
    cout << "tempo de codificacao = ";
    codificacao.Exibir();
    cout << endl;
    cout << "tempo de correcao = ";
    correcao.Exibir();
    cout << endl;
    total = codificacao.Soma(correcao);
    cout << "codificacao.Soma(correcao) = ";
    total.Exibir();
    cout << endl;
    return 0;
}
```

Aqui está a saída do programa das Listagens 11.1, 11.2 e 11.3:

```
tempo de planejamento = 0 horas, 0 minutos
tempo de codificacao = 2 horas, 40 minutos
tempo de correcao = 5 horas, 55 minutos
codificacao.Soma(correcao) = 8 horas, 35 minutos
```

### Adicionando um Operador de Adição

É simples converter a classe `Tempo` para usar um operador de adição sobrecarregado. Você simplesmente muda o nome de `Soma()` para o nome de aparência mais estranha `operator+()`. É isso mesmo: você apenas acrescenta o símbolo do operador (`+`, neste caso) ao final de `operator` e usa o resultado como nome de método. As Listagens 11.4 e 11.5 refletem essa pequena mudança.

**Listagem 11.4 — mytime1.h**

```cpp
// mytime1.h — classe Tempo com operator+()
#ifndef MYTIME1_H_
#define MYTIME1_H_

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
    void Exibir() const;
};
#endif
```

**Listagem 11.5 — mytime1.cpp**

```cpp
// mytime1.cpp — implementando os métodos de Tempo
#include <iostream>
#include "mytime1.h"
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
void Tempo::Exibir() const
{
    std::cout << horas << " horas, " << minutos << " minutos";
}
```

Assim como `Soma()`, `operator+()` é invocado por um objeto `Tempo`, recebe um segundo objeto `Tempo` como argumento e retorna um objeto `Tempo`. Portanto, você pode invocar o método `operator+()` usando a mesma sintaxe que `Soma()` usa:

```cpp
total = codificacao.operator+(correcao); // notação de função
```

Mas nomear o método `operator+()` também permite usar notação de operador:

```cpp
total = codificacao + correcao; // notação de operador
```

Qualquer notação invoca o método `operator+()`. Observe que com a notação de operador, o objeto à esquerda do operador (`codificacao`, neste caso) é o objeto invocador, e o objeto à direita (`correcao`, neste caso) é o passado como argumento. A Listagem 11.6 ilustra esse ponto.

**Listagem 11.6 — usetime1.cpp**

```cpp
// usetime1.cpp — usando a segunda versão da classe Tempo
// compilar usetime1.cpp e mytime1.cpp juntos
#include <iostream>
#include "mytime1.h"
int main()
{
    using std::cout;
    using std::endl;
    Tempo planejamento;
    Tempo codificacao(2, 40);
    Tempo correcao(5, 55);
    Tempo total;
    cout << "tempo de planejamento = ";
    planejamento.Exibir();
    cout << endl;
    cout << "tempo de codificacao = ";
    codificacao.Exibir();
    cout << endl;
    cout << "tempo de correcao = ";
    correcao.Exibir();
    cout << endl;
    total = codificacao + correcao;
    // notação de operador
    cout << "codificacao + correcao = ";
    total.Exibir();
    cout << endl;
    Tempo mais_correcao(3, 28);
    cout << "tempo de mais correcao = ";
    mais_correcao.Exibir();
    cout << endl;
    total = mais_correcao.operator+(total);
    // notação de função
    cout << "mais_correcao.operator+(total) = ";
    total.Exibir();
    cout << endl;
    return 0;
}
```

Aqui está a saída do programa das Listagens 11.4, 11.5 e 11.6:

```
tempo de planejamento = 0 horas, 0 minutos
tempo de codificacao = 2 horas, 40 minutos
tempo de correcao = 5 horas, 55 minutos
codificacao + correcao = 8 horas, 35 minutos
tempo de mais correcao = 3 horas, 28 minutos
mais_correcao.operator+(total) = 12 horas, 3 minutos
```

Em resumo, o nome da função `operator+()` permite que ela seja invocada usando tanto a notação de função quanto a notação de operador. O compilador usa os tipos dos operandos para determinar o que fazer:

```cpp
int a, b, c;
Tempo A, B, C;
c = a + b; // usa adição de int
C = A + B; // usa adição definida para objetos Tempo
```

Você pode somar mais de dois objetos? Por exemplo, se `t1`, `t2`, `t3` e `t4` são todos objetos `Tempo`, você pode fazer o seguinte?

```cpp
t4 = t1 + t2 + t3; // válido?
```

A maneira de responder isso é considerar como a instrução é traduzida em chamadas de função. Como a adição é um operador da esquerda para a direita, a instrução é primeiro traduzida para:

```cpp
t4 = t1.operator+(t2 + t3); // válido?
```

Então o argumento da função é ele mesmo traduzido para uma chamada de função, resultando no seguinte:

```cpp
t4 = t1.operator+(t2.operator+(t3)); // válido? SIM
```

Isso é válido. A chamada de função `t2.operator+(t3)` retorna um objeto `Tempo` que representa a soma de `t2` e `t3`. Esse objeto torna-se então o objeto da chamada de função `t1.operator+()`, e essa chamada retorna a soma de `t1` e do objeto `Tempo` que representa a soma de `t2` e `t3`. Em resumo, o valor de retorno final é a soma de `t1`, `t2` e `t3`, exatamente como desejado.

## Restrições de Sobrecarga (Overloading Restrictions)

A maioria dos operadores C++ (ver Tabela 11.1) pode ser sobrecarregada da maneira descrita na seção anterior. Operadores sobrecarregados (com algumas exceções) não precisam necessariamente ser funções membro. No entanto, ao menos um dos operandos deve ser de um tipo definido pelo usuário. Vamos examinar mais de perto os limites que o C++ impõe sobre a sobrecarga de operadores:

- O operador sobrecarregado deve ter ao menos um operando que seja de um tipo definido pelo usuário. Isso impede que você sobrecarregue operadores para os tipos padrão. Assim, você não pode redefinir o operador menos (`-`) para que ele produza a soma de dois valores `double` em vez de sua diferença. Essa restrição preserva a sanidade do programa.

- Você não pode usar um operador de uma maneira que viole as regras de sintaxe do operador original. Por exemplo, não pode sobrecarregar o operador módulo (`%`) para que possa ser usado com um único operando: `% x;` é inválido assim como `% objeto;`. Da mesma forma, você não pode alterar a precedência de operadores. Portanto, se você sobrecarregar o operador de adição para somar duas classes, o novo operador terá a mesma precedência da adição comum.

- Você não pode criar novos símbolos de operadores. Por exemplo, não pode definir uma função `operator**()` para denotar exponenciação.

- Você não pode sobrecarregar os seguintes operadores:

| Operador | Descrição |
|----------|-----------|
| `sizeof` | O operador sizeof |
| `.` | O operador de acesso a membro |
| `.*` | O operador de ponteiro para membro |
| `::` | O operador de resolução de escopo |
| `?:` | O operador condicional |
| `typeid` | Um operador RTTI |
| `const_cast` | Um operador de type cast |
| `dynamic_cast` | Um operador de type cast |
| `reinterpret_cast` | Um operador de type cast |
| `static_cast` | Um operador de type cast |

Ainda assim restam todos os operadores da Tabela 11.1 disponíveis para sobrecarga.

- A maioria dos operadores da Tabela 11.1 pode ser sobrecarregada usando funções membro ou não membro. No entanto, você pode usar *apenas* funções membro para sobrecarregar os seguintes operadores:

| Operador | Descrição |
|----------|-----------|
| `=` | Operador de atribuição |
| `()` | Operador de chamada de função |
| `[]` | Operador de subscripting |
| `->` | Operador de acesso a membro de classe por ponteiro |

> **Nota**
> Este capítulo não cobre todos os operadores mencionados na lista de restrições ou na Tabela 11.1. Entretanto, o Apêndice E, "Outros Operadores," resume os operadores que não são cobertos no corpo principal deste texto.

**Tabela 11.1 — Operadores Que Podem Ser Sobrecarregados**

```
+      -      *      /      %      ^
&      |      ~      !      =      <
>      +=     -=     *=     /=     %=
^=     &=     |=     <<     >>     >>=
<<=    ==     !=     <=     >=     &&
||     ++     --     ,      ->*    ->
()     []     new    delete new[]  delete[]
```

Além dessas restrições formais, você deve usar moderação sensata ao sobrecarregar operadores. Por exemplo, você não deveria sobrecarregar o operador `*` para que ele troque os membros de dados de dois objetos `Tempo`. Nada na notação sugeriria o que o operador faria, por isso seria melhor definir um método de classe com um nome explicativo como `Trocar()`.

## Mais Operadores Sobrecarregados (More Overloaded Operators)

Algumas outras operações fazem sentido para a classe `Tempo`. Por exemplo, talvez você queira subtrair um tempo de outro ou multiplicar um tempo por um fator. Isso sugere sobrecarregar os operadores de subtração e multiplicação. A técnica é a mesma do operador de adição: você cria métodos `operator-()` e `operator*()`. Ou seja, você acrescenta os seguintes protótipos à declaração da classe:

```cpp
Tempo operator-(const Tempo & t) const;
Tempo operator*(double n) const;
```

A Listagem 11.7 mostra o novo arquivo de cabeçalho.

**Listagem 11.7 — mytime2.h**

```cpp
// mytime2.h — classe Tempo com operadores sobrecarregados +, -, *
#ifndef MYTIME2_H_
#define MYTIME2_H_
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
    void Exibir() const;
};
#endif
```

Em seguida você acrescenta definições para os novos métodos ao arquivo de implementação, como mostrado na Listagem 11.8.

**Listagem 11.8 — mytime2.cpp**

```cpp
// mytime2.cpp — implementando os métodos de Tempo
#include <iostream>
#include "mytime2.h"
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
void Tempo::Exibir() const
{
    std::cout << horas << " horas, " << minutos << " minutos";
}
```

Com essas mudanças feitas, você pode testar as novas definições com o código mostrado na Listagem 11.9.

**Listagem 11.9 — usetime2.cpp**

```cpp
// usetime2.cpp — usando a terceira versão da classe Tempo
// compilar usetime2.cpp e mytime2.cpp juntos
#include <iostream>
#include "mytime2.h"
int main()
{
    using std::cout;
    using std::endl;
    Tempo capina(4, 35);
    Tempo cera(2, 47);
    Tempo total;
    Tempo dif;
    Tempo ajustado;
    cout << "tempo de capina = ";
    capina.Exibir();
    cout << endl;
    cout << "tempo de cera = ";
    cera.Exibir();
    cout << endl;
    cout << "tempo total de trabalho = ";
    total = capina + cera; // usa operator+()
    total.Exibir();
    cout << endl;
    dif = capina - cera; // usa operator-()
    cout << "tempo de capina - tempo de cera = ";
    dif.Exibir();
    cout << endl;
    ajustado = total * 1.5; // usa operator*()
    cout << "tempo de trabalho ajustado = ";
    ajustado.Exibir();
    cout << endl;
    return 0;
}
```

Aqui está a saída do programa das Listagens 11.7, 11.8 e 11.9:

```
tempo de capina = 4 horas, 35 minutos
tempo de cera = 2 horas, 47 minutos
tempo total de trabalho = 7 horas, 22 minutos
tempo de capina - tempo de cera = 1 horas, 48 minutos
tempo de trabalho ajustado = 11 horas, 3 minutos
```

## Apresentando Funções Amigas (Introducing Friends)

Como você viu, o C++ controla o acesso às partes privadas de um objeto de classe. Geralmente, apenas os métodos públicos da classe servem como único acesso, mas às vezes essa restrição é muito rígida para se encaixar a determinados problemas de programação. Nesses casos, o C++ fornece outra forma de acesso: o *amigo* (friend). Amigos vêm em três variedades:

- Funções amigas (friend functions)
- Classes amigas (friend classes)
- Funções membro amigas (friend member functions)

Ao tornar uma função amiga de uma classe, você permite que ela tenha os mesmos privilégios de acesso que uma função membro da classe tem. Examinaremos funções amigas agora, deixando as outras duas variedades para o Capítulo 15, "Amigos, Exceções e Mais."

Antes de ver como criar amigos, vamos examinar por que eles podem ser necessários. Com frequência, sobrecarregar um operador binário (isto é, um operador com dois argumentos) para uma classe gera a necessidade de amigos. Multiplicar um objeto `Tempo` por um número real fornece exatamente essa situação, então vamos estudar esse caso.

No exemplo anterior da classe `Tempo`, o operador de multiplicação sobrecarregado é diferente dos outros dois operadores sobrecarregados por combinar dois tipos diferentes. Ou seja, os operadores de adição e subtração combinam dois valores `Tempo`, mas o operador de multiplicação combina um valor `Tempo` com um valor `double`. Isso restringe como o operador pode ser usado. Lembre-se: o operando à esquerda é o objeto invocador. Ou seja,

```cpp
A = B * 2.75;
```

é traduzido para a seguinte chamada de função membro:

```cpp
A = B.operator*(2.75);
```

Mas e quanto à seguinte instrução?

```cpp
A = 2.75 * B; // não pode corresponder a uma função membro
```

Conceitualmente, `2.75 * B` deveria ser o mesmo que `B * 2.75`, mas a primeira expressão não pode corresponder a uma função membro porque `2.75` não é um objeto do tipo `Tempo`. Lembre-se: o operando à esquerda é o objeto invocador, mas `2.75` não é um objeto. Portanto, o compilador não pode substituir a expressão por uma chamada de função membro.

Uma maneira de contornar essa dificuldade é dizer a todos (e lembrar a si mesmo) que você só pode escrever `B * 2.75` mas nunca `2.75 * B`. Essa é uma solução amigável ao servidor e hostil ao cliente, e não é isso que a POO diz.

Entretanto, existe outra possibilidade — usar uma função não membro. (Lembre-se: a maioria dos operadores pode ser sobrecarregada usando funções membro ou não membro.) Uma função não membro não é invocada por um objeto; em vez disso, todos os valores que ela usa, incluindo objetos, são argumentos explícitos. Assim, o compilador poderia corresponder a expressão

```cpp
A = 2.75 * B; // não pode corresponder a uma função membro
```

à seguinte chamada de função não membro:

```cpp
A = operator*(2.75, B);
```

A função teria este protótipo:

```cpp
Tempo operator*(double m, const Tempo & t);
```

Com a função operadora não membro sobrecarregada, o operando à esquerda de uma expressão de operador corresponde ao primeiro argumento da função operadora, e o operando à direita corresponde ao segundo argumento. Enquanto isso, a função membro original trata operandos na ordem oposta — ou seja, um valor `Tempo` multiplicado por um valor `double`.

Usar uma função não membro resolve o problema de obter os operandos na ordem desejada (primeiro `double` e então `Tempo`), mas gera um novo problema: funções não membro não podem acessar diretamente dados privados em uma classe. Bem, pelo menos funções não membro comuns não têm acesso. Mas existe uma categoria especial de funções não membro, chamadas *amigas* (friends), que podem acessar membros privados de uma classe.

---

*Continua em:* [Capítulo 11 — Funções Amigas e a Classe Vetor](capitulo-11-02-funcoes-amigas-vetores.md)

---

> Navegação: [Anterior](capitulo-10-03-escopo-adt-resumo.md) | [Índice](README.md) | [Próximo](capitulo-11-02-funcoes-amigas-vetores.md)
