# Capítulo 14 — Templates de Classe (Class Templates)

> Tradução não oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Navegação: [Anterior](capitulo-14-02-heranca-multipla.md) | [Índice](README.md) | [Próximo](capitulo-14-04-resumo.md)

## Templates de Classe

Herança (pública, privada ou protegida) e composição (containment) nem sempre são a solução quando você quer reutilizar código. Considere, por exemplo, a classe `Pilha` (veja o Capítulo 10) e a classe `Fila` (veja o Capítulo 12). São exemplos de *classes container* (container classes), que são classes projetadas para armazenar outros objetos ou tipos de dados. A classe `Pilha` do Capítulo 10, por exemplo, armazena valores `unsigned long`. Você poderia facilmente definir uma classe pilha para armazenar valores `double` ou objetos `string`. O código seria idêntico, exceto pelo tipo do objeto armazenado. No entanto, em vez de escrever novas declarações de classe, seria melhor poder definir uma pilha de forma genérica (ou seja, independente de tipo) e depois fornecer um tipo específico como parâmetro para a classe. Dessa forma, você poderia usar o mesmo código genérico para produzir pilhas de diferentes tipos de valores. No Capítulo 10, o exemplo da `Pilha` usa `typedef` como uma primeira tentativa de lidar com esse desejo. No entanto, essa abordagem tem algumas desvantagens. Primeiro, você precisa editar o arquivo de cabeçalho cada vez que mudar o tipo. Segundo, você pode usar a técnica para gerar apenas um tipo de pilha por programa. Ou seja, você não pode fazer um `typedef` representar dois tipos diferentes simultaneamente, portanto não pode usar o método para definir uma pilha de `int` e uma pilha de `string` no mesmo programa.

Os *templates de classe* (class templates) do C++ fornecem uma maneira melhor de gerar declarações de classe genéricas. Templates fornecem *tipos parametrizados* — ou seja, eles são capazes de passar um nome de tipo como argumento para uma receita de construção de classe ou função. Ao alimentar o nome de tipo `int` num template `Fila`, por exemplo, você pode fazer o compilador construir uma classe `Fila` para enfileirar `int`s.

A biblioteca C++ fornece várias classes template. Anteriormente neste capítulo, você trabalhou com a classe template `valarray`, e o Capítulo 4 introduziu as classes template `vector` e `array`. A Biblioteca de Templates Padrão (STL — Standard Template Library) do C++, que o Capítulo 16 discute em parte, fornece implementações de template poderosas e flexíveis de várias classes container. Este capítulo explora projetos de natureza mais elementar.

### Definindo um Template de Classe

Vamos usar a classe `Pilha` do Capítulo 10 como modelo a partir do qual construir um template. Aqui está a declaração original da classe:

```cpp
typedef unsigned long Item;

class Pilha
{
private:
    enum {MAX = 10};   // constante específica da classe
    Item itens[MAX];   // armazena os itens da pilha
    int topo;          // índice para o item no topo da pilha
public:
    Pilha();
    bool estaVazia() const;
    bool estaCheia() const;
    // empilhar() retorna false se a pilha já está cheia, true caso contrário
    bool empilhar(const Item & item);  // adiciona item à pilha
    // desempilhar() retorna false se a pilha já está vazia, true caso contrário
    bool desempilhar(Item & item);     // desempilha o topo para item
};
```

A abordagem de template substituirá a definição de `Pilha` por uma definição de template e os métodos membros de `Pilha` por métodos membros de template. Assim como nos templates de função, você prefixa um template de classe com código que tem a seguinte forma:

```cpp
template <class Tipo>
```

A palavra-chave `template` informa ao compilador que você está prestes a definir um template. A parte entre colchetes angulares é análoga a uma lista de argumentos de uma função. Você pode pensar na palavra-chave `class` como servindo de nome de tipo para uma variável que aceita um tipo como valor, e em `Tipo` como representando um nome para essa variável.

Usar `class` aqui não significa que `Tipo` deva ser uma classe; significa apenas que `Tipo` serve como especificador de tipo genérico para o qual um tipo real será substituído quando o template for usado. Implementações mais recentes do C++ permitem usar a palavra-chave menos confusa `typename` em vez de `class` neste contexto:

```cpp
template <typename Tipo>  // escolha mais recente
```

Você pode usar o nome de tipo genérico que preferir na posição `Tipo`; as regras de nomeação são as mesmas que para qualquer outro identificador. Escolhas populares incluem `T` e `Tipo`. Quando um template é invocado, `Tipo` será substituído por um valor de tipo específico, como `int` ou `string`. Dentro da definição do template, você pode usar o nome de tipo genérico para identificar o tipo a ser armazenado na pilha. Para o caso da `Pilha`, isso significaria usar `Tipo` onde a declaração original usava o identificador `typedef` `Item`. Por exemplo:

```cpp
Item itens[MAX];   // armazena os itens da pilha
```

torna-se o seguinte:

```cpp
Tipo itens[MAX];   // armazena os itens da pilha
```

Da mesma forma, você pode substituir os métodos da classe original por métodos membros de template. Cada cabeçalho de função será prefixado com o mesmo anúncio de template:

```cpp
template <class Tipo>
```

Novamente, você deve substituir o identificador `typedef` `Item` pelo nome de tipo genérico `Tipo`. Mais uma mudança: você precisa alterar o qualificador de classe de `Pilha::` para `Pilha<Tipo>::`. Por exemplo:

```cpp
bool Pilha::empilhar(const Item & item)
{
    ...
}
```

torna-se o seguinte:

```cpp
template <class Tipo>   // ou template <typename Tipo>
bool Pilha<Tipo>::empilhar(const Tipo & item)
{
    ...
}
```

Se você definir um método dentro da declaração da classe (uma definição inline), pode omitir o prefácio de template e o qualificador de classe.

A Listagem 14.13 mostra os templates combinados de classe e de função membro. É importante perceber que esses templates não são definições de classe e de função membro. Em vez disso, são instruções ao compilador C++ sobre como gerar definições de classe e de função membro. Uma determinada instanciação de um template, como uma classe pilha para lidar com objetos `string`, é chamada de *instanciação* ou *especialização*. Colocar as funções membro de template num arquivo de implementação separado não funcionará. Como os templates não são funções, eles não podem ser compilados separadamente. Os templates devem ser usados em conjunto com pedidos de instanciações específicas de templates. A maneira mais simples de fazer isso funcionar é colocar todas as informações do template num arquivo de cabeçalho e incluir esse arquivo de cabeçalho no arquivo que usará os templates.

**Listagem 14.13 `pilhaTp.h`**

```cpp
// pilhaTp.h -- um template de pilha
#ifndef PILHATP_H_
#define PILHATP_H_
template <class Tipo>
class Pilha
{
private:
    enum {MAX = 10};   // constante específica da classe
    Tipo itens[MAX];   // armazena os itens da pilha
    int topo;          // índice para o item no topo da pilha
public:
    Pilha();
    bool estaVazia();
    bool estaCheia();
    bool empilhar(const Tipo & item);   // adiciona item à pilha
    bool desempilhar(Tipo & item);      // desempilha o topo para item
};
template <class Tipo>
Pilha<Tipo>::Pilha()
{
    topo = 0;
}
template <class Tipo>
bool Pilha<Tipo>::estaVazia()
{
    return topo == 0;
}
template <class Tipo>
bool Pilha<Tipo>::estaCheia()
{
    return topo == MAX;
}
template <class Tipo>
bool Pilha<Tipo>::empilhar(const Tipo & item)
{
    if (topo < MAX)
    {
        itens[topo++] = item;
        return true;
    }
    else
        return false;
}
template <class Tipo>
bool Pilha<Tipo>::desempilhar(Tipo & item)
{
    if (topo > 0)
    {
        item = itens[--topo];
        return true;
    }
    else
        return false;
}
#endif
```

### Usando uma Classe Template

Simplesmente incluir um template num programa não gera uma classe template. Você tem que solicitar uma instanciação. Para isso, declare um objeto do tipo de classe template, substituindo o nome de tipo genérico pelo tipo particular que você quer. Por exemplo, aqui está como você criaria duas pilhas, uma para empilhar `int`s e outra para empilhar objetos `string`:

```cpp
Pilha<int> graos;           // cria uma pilha de int
Pilha<string> coroneis;     // cria uma pilha de objetos string
```

Ao ver essas duas declarações, o compilador seguirá o template `Pilha<Tipo>` para gerar duas declarações de classe separadas e dois conjuntos separados de métodos de classe. A declaração de classe `Pilha<int>` substituirá `Tipo` por `int` em todo o código, e a declaração de classe `Pilha<string>` substituirá `Tipo` por `string`.

Identificadores de tipo genérico, como `Tipo` no exemplo, são chamados de *parâmetros de tipo*, o que significa que eles agem um pouco como variáveis, mas em vez de atribuir um valor numérico a um parâmetro de tipo, você atribui um tipo a ele.

Observe que você precisa fornecer o tipo desejado explicitamente. Isso é diferente dos templates de função comuns, para os quais o compilador pode usar os tipos dos argumentos de uma função para descobrir que tipo de função gerar:

```cpp
template <class T>
void simples(T t) { cout << t << '\n'; }
...
simples(2);       // gera void simples(int)
simples("dois");  // gera void simples(const char *)
```

A Listagem 14.14 modifica o programa original de teste de pilha (Listagem 10.12) para usar IDs de ordens de compra em `string` em vez de valores `unsigned long`.

**Listagem 14.14 `testarPilhaTp.cpp`**

```cpp
// testarPilhaTp.cpp -- testando a classe template de pilha
#include <iostream>
#include <string>
#include <cctype>
#include "pilhaTp.h"
using std::cin;
using std::cout;
int main()
{
    Pilha<std::string> pilha;  // cria uma pilha vazia
    char ch;
    std::string op;
    cout << "Digite A para adicionar uma ordem de compra,\n"
         << "P para processar uma OP, ou Q para sair.\n";
    while (cin >> ch && std::toupper(ch) != 'Q')
    {
        while (cin.get() != '\n')
            continue;
        if (!std::isalpha(ch))
        {
            cout << '\a';
            continue;
        }
        switch(ch)
        {
            case 'A':
            case 'a': cout << "Digite um numero de OP para adicionar: ";
                      cin >> op;
                      if (pilha.estaCheia())
                          cout << "pilha ja cheia\n";
                      else
                          pilha.empilhar(op);
                      break;
            case 'P':
            case 'p': if (pilha.estaVazia())
                          cout << "pilha ja vazia\n";
                      else {
                          pilha.desempilhar(op);
                          cout << "OP #" << op << " desempilhada\n";
                          break;
                      }
        }
        cout << "Digite A para adicionar uma ordem de compra,\n"
             << "P para processar uma OP, ou Q para sair.\n";
    }
    cout << "Ate logo\n";
    return 0;
}
```

Aqui está um exemplo de execução do programa da Listagem 14.14:

```
Digite A para adicionar uma ordem de compra,
P para processar uma OP, ou Q para sair.
A
Digite um numero de OP para adicionar: red911porsche
A
Digite um numero de OP para adicionar: blueR8audi
A
Digite um numero de OP para adicionar: silver747boeing
P
OP #silver747boeing desempilhada
P
OP #blueR8audi desempilhada
P
OP #red911porsche desempilhada
P
pilha ja vazia
Q
Ate logo
```

### Um Olhar Mais Atento à Classe Template

Você pode usar um tipo interno ou um objeto de classe como tipo para a classe template `Pilha<Tipo>`. Que tal um ponteiro? Por exemplo, você pode usar um ponteiro para `char` em vez de um objeto `string`? A resposta é que você *pode* criar uma pilha de ponteiros, mas ela não funcionará muito bem sem grandes modificações no programa. O compilador pode criar a classe, mas é sua tarefa ver que ela seja usada de forma sensata. Vejamos por que tal pilha de ponteiros não funciona bem com a Listagem 14.14.

**Usando uma Pilha de Ponteiros Incorretamente**

Vamos examinar rapidamente três tentativas simples, mas falhas, de adaptar a Listagem 14.14 para usar uma pilha de ponteiros. Todas as três começam com esta invocação perfeitamente válida do template `Pilha<Tipo>`:

```cpp
Pilha<char *> pilha;  // cria uma pilha para ponteiros-para-char
```

A **Versão 1** substitui `string op;` por `char * op;`. A ideia é usar um ponteiro `char` em vez de um objeto `string` para receber a entrada do teclado. Essa abordagem falha imediatamente porque criar apenas um ponteiro não cria espaço para armazenar as strings de entrada.

A **Versão 2** substitui `string op;` por `char op[40];`. Isso aloca espaço para uma string de entrada. Além disso, `op` é do tipo `char *`, portanto pode ser colocado na pilha. Mas um array é fundamentalmente incompatível com as suposições feitas para o método `desempilhar()`, pois você não pode atribuir a um nome de array.

A **Versão 3** substitui `string op;` por `char * op = new char[40];`. Isso aloca espaço para uma string de entrada, e `op` é uma variável compatível com o código para `desempilhar()`. No entanto, há um problema fundamental: existe apenas uma variável `op`, e ela sempre aponta para o mesmo local de memória. Assim, cada operação de empilhar coloca exatamente o mesmo endereço na pilha, e quando você desempilha a pilha, sempre obtém o mesmo endereço de volta.

**Usando uma Pilha de Ponteiros Corretamente**

Uma maneira de usar uma pilha de ponteiros é ter o programa chamador fornecendo um array de ponteiros, com cada ponteiro apontando para uma string diferente. Colocar esses ponteiros numa pilha faz sentido porque cada ponteiro se referirá a uma string diferente.

Por exemplo, suponha que você tenha que simular a seguinte situação. Alguém entregou um carrinho de pastas para Plodson. Se a caixa de entrada de Plodson estiver vazia, ele remove a pasta do topo do carrinho e a coloca em sua caixa de entrada. Se a caixa de entrada estiver cheia, Plodson remove o arquivo do topo da caixa, processa-o e coloca-o em sua caixa de saída. Se a caixa de entrada não estiver nem vazia nem cheia, Plodson pode processar o arquivo do topo da caixa de entrada, ou pode pegar o próximo arquivo do carrinho e colocá-lo na caixa de entrada — ele lança uma moeda para decidir. Você gostaria de investigar os efeitos de seu método sobre a ordem original dos arquivos.

A Listagem 14.15 redefine o template `Pilha<Tipo>` ligeiramente para que o construtor de `Pilha` aceite um argumento de tamanho opcional. Isso envolve usar um array dinâmico internamente, portanto a classe agora precisa de um destrutor, um construtor de cópia e um operador de atribuição. A definição também encurta o código tornando vários dos métodos inline.

**Listagem 14.15 `pilhaTp1.h`**

```cpp
// pilhaTp1.h -- template de Pilha modificado
#ifndef PILHATP1_H_
#define PILHATP1_H_

template <class Tipo>
class Pilha
{
private:
    enum {TAMANHO = 10};   // tamanho padrão
    int tamPilha;
    Tipo * itens;          // armazena os itens da pilha
    int topo;              // índice para o item no topo da pilha
public:
    explicit Pilha(int ss = TAMANHO);
    Pilha(const Pilha & st);
    ~Pilha() { delete [] itens; }
    bool estaVazia() { return topo == 0; }
    bool estaCheia() { return topo == tamPilha; }
    bool empilhar(const Tipo & item);   // adiciona item à pilha
    bool desempilhar(Tipo & item);      // desempilha o topo para item
    Pilha & operator=(const Pilha & st);
};
template <class Tipo>
Pilha<Tipo>::Pilha(int ss) : tamPilha(ss), topo(0)
{
    itens = new Tipo [tamPilha];
}
template <class Tipo>
Pilha<Tipo>::Pilha(const Pilha & st)
{
    tamPilha = st.tamPilha;
    topo = st.topo;
    itens = new Tipo [tamPilha];
    for (int i = 0; i < topo; i++)
        itens[i] = st.itens[i];
}
template <class Tipo>
bool Pilha<Tipo>::empilhar(const Tipo & item)
{
    if (topo < tamPilha)
    {
        itens[topo++] = item;
        return true;
    }
    else
        return false;
}
template <class Tipo>
bool Pilha<Tipo>::desempilhar(Tipo & item)
{
    if (topo > 0)
    {
        item = itens[--topo];
        return true;
    }
    else
        return false;
}
template <class Tipo>
Pilha<Tipo> & Pilha<Tipo>::operator=(const Pilha<Tipo> & st)
{
    if (this == &st)
        return *this;
    delete [] itens;
    tamPilha = st.tamPilha;
    topo = st.topo;
    itens = new Tipo [tamPilha];
    for (int i = 0; i < topo; i++)
        itens[i] = st.itens[i];
    return *this;
}
#endif
```

Observe que o protótipo declara o tipo de retorno para a função operador de atribuição como uma referência a `Pilha`, e a definição real da função template identifica o tipo como `Pilha<Tipo>`. O primeiro é uma abreviação do segundo, mas pode ser usado apenas no escopo da classe.

O programa da Listagem 14.16 usa o novo template de pilha para implementar a simulação de Plodson. Ele usa `rand()`, `srand()` e `time()` da mesma forma que simulações anteriores usaram essas funções para gerar números aleatórios. Neste caso, gerar aleatoriamente 0 ou 1 simula o lançamento de moeda.

**Listagem 14.16 `testarPilhaPtr.cpp`**

```cpp
// testarPilhaPtr.cpp -- testando pilha de ponteiros
#include <iostream>
#include <cstdlib>   // para rand(), srand()
#include <ctime>     // para time()
#include "pilhaTp1.h"
const int Num = 10;
int main()
{
    std::srand(std::time(0));  // aleatoriza rand()
    std::cout << "Por favor, insira o tamanho da pilha: ";
    int tamPilha;
    std::cin >> tamPilha;
    // cria uma pilha vazia com slots tamPilha
    Pilha<const char *> pilha(tamPilha);
    // caixa de entrada
    const char * entrada[Num] = {
         " 1: Hank Gilgamesh", " 2: Kiki Ishtar",
         " 3: Betty Rocker",   " 4: Ian Flagranti",
         " 5: Wolfgang Kibble", " 6: Portia Koop",
         " 7: Joy Almondo",    " 8: Xaverie Paprika",
         " 9: Juan Moore",     "10: Misha Mache"
         };
    // caixa de saída
    const char * saida[Num];
    int processados = 0;
    int proximoEntrar = 0;
    while (processados < Num)
    {
        if (pilha.estaVazia())
            pilha.empilhar(entrada[proximoEntrar++]);
        else if (pilha.estaCheia())
            pilha.desempilhar(saida[processados++]);
        else if (std::rand() % 2 && proximoEntrar < Num)  // chance 50-50
            pilha.empilhar(entrada[proximoEntrar++]);
        else
            pilha.desempilhar(saida[processados++]);
    }
    for (int i = 0; i < Num; i++)
        std::cout << saida[i] << std::endl;
    std::cout << "Ate logo\n";
    return 0;
}
```

Dois exemplos de execução do programa da Listagem 14.16 seguem (note que, graças ao recurso de aleatorização, a ordenação final dos arquivos pode variar bastante de uma tentativa para outra, mesmo quando o tamanho da pilha é mantido inalterado):

```
Por favor, insira o tamanho da pilha: 5
 2: Kiki Ishtar
 1: Hank Gilgamesh
 3: Betty Rocker
 5: Wolfgang Kibble
 4: Ian Flagranti
 7: Joy Almondo
 9: Juan Moore
 8: Xaverie Paprika
 6: Portia Koop
10: Misha Mache
Ate logo
```

As strings na Listagem 14.16 nunca se movem. Empilhar uma string na pilha realmente cria um novo ponteiro para uma string existente — ou seja, cria um ponteiro cujo valor é o endereço de uma string existente. E desempilhar uma string da pilha copia esse valor de endereço para o array `saida`. O destrutor da classe não tem efeito sobre as strings; o destrutor elimina o array para armazenar ponteiros, não as strings para as quais os elementos do array apontam.

## Um Exemplo de Template de Array e Argumentos Não-Tipo

Templates são frequentemente usados para classes container porque a ideia de parâmetros de tipo se encaixa bem na necessidade de aplicar um plano de armazenamento comum a uma variedade de tipos. Vejamos outro exemplo e exploremos mais algumas facetas do design e uso de templates. Em particular, vejamos argumentos não-tipo, ou de *expressão*, e o uso de um array para lidar com uma família de herança.

Vamos começar com um template de array simples que permite especificar um tamanho de array. Uma técnica, que a última versão do template `Pilha` usa, é usar um array dinâmico dentro da classe e um argumento de construtor para fornecer o número de elementos. Outra abordagem é usar um argumento de template para fornecer o tamanho de um array regular. É isso que o novo template `array` do C++11 faz. A Listagem 14.17 mostra uma versão mais modesta de como isso pode ser feito.

**Listagem 14.17 `arrayTp.h`**

```cpp
// arrayTp.h -- Template de Array
#ifndef ARRAYTP_H_
#define ARRAYTP_H_

#include <iostream>
#include <cstdlib>
template <class T, int n>
class ArrayTP
{
private:
    T ar[n];
public:
    ArrayTP() {};
    explicit ArrayTP(const T & v);
    virtual T & operator[](int i);
    virtual T operator[](int i) const;
};
template <class T, int n>
ArrayTP<T,n>::ArrayTP(const T & v)
{
    for (int i = 0; i < n; i++)
        ar[i] = v;
}
template <class T, int n>
T & ArrayTP<T,n>::operator[](int i)
{
    if (i < 0 || i >= n)
    {
        std::cerr << "Erro nos limites do array: " << i
                  << " esta fora do intervalo\n";
        std::exit(EXIT_FAILURE);
    }
    return ar[i];
}
template <class T, int n>
T ArrayTP<T,n>::operator[](int i) const
{
    if (i < 0 || i >= n)
    {
        std::cerr << "Erro nos limites do array: " << i
                  << " esta fora do intervalo\n";
        std::exit(EXIT_FAILURE);
    }
    return ar[i];
}
#endif
```

Observe o cabeçalho do template na Listagem 14.17:

```cpp
template <class T, int n>
```

A palavra-chave `class` (ou equivalentemente, neste contexto, `typename`) identifica `T` como um parâmetro de tipo, ou argumento de tipo. `int` identifica `n` como sendo do tipo `int`. Esse segundo tipo de parâmetro — um que especifica um tipo particular em vez de agir como nome genérico para um tipo — é chamado de *argumento não-tipo*, ou de *expressão*. Suponha que você tenha a seguinte declaração:

```cpp
ArrayTP<double, 12> pesosDeOvos;
```

Isso faz o compilador definir uma classe chamada `ArrayTP<double,12>` e criar um objeto `pesosDeOvos` daquela classe. Ao definir a classe, o compilador substitui `T` por `double` e `n` por `12`.

Argumentos de expressão têm algumas restrições. Um argumento de expressão pode ser um tipo inteiro, tipo enumeração, referência ou ponteiro. Assim, `double m` é descartado, mas `double & rm` e `double * pm` são permitidos. Além disso, o código do template não pode alterar o valor do argumento nem obter seu endereço. Quando você instancia um template, o valor usado para o argumento de expressão deve ser uma expressão constante.

Essa abordagem para dimensionar um array tem uma vantagem sobre a abordagem de construtor. A abordagem de construtor usa memória heap gerenciada por `new` e `delete`, enquanto a abordagem de argumento de expressão usa a pilha de memória mantida para variáveis automáticas. Isso fornece tempo de execução mais rápido, particularmente se você tiver muitos arrays pequenos.

A principal desvantagem da abordagem de argumento de expressão é que cada tamanho de array gera seu próprio template. Ou seja, as seguintes declarações geram duas declarações de classe separadas:

```cpp
ArrayTP<double, 12> pesosDeOvos;
ArrayTP<double, 13> donuts;
```

Mas as seguintes declarações geram apenas uma declaração de classe, e as informações de tamanho são passadas para o construtor daquela classe:

```cpp
Pilha<int> ovos(12);
Pilha<int> donuts(13);
```

Outra diferença é que a abordagem de construtor é mais versátil porque o tamanho do array é armazenado como um membro da classe em vez de ser codificado na definição.

## Versatilidade dos Templates

Você pode aplicar as mesmas técnicas a classes template que aplica a classes regulares. Classes template podem servir como classes base, e podem ser classes componentes. Elas próprias podem ser argumentos de tipo para outros templates. Por exemplo, você pode implementar um template de pilha usando um template de array. Ou você pode ter um template de array que é usado para construir um array cujos elementos são pilhas baseadas num template de pilha. Ou seja, você pode ter código como:

```cpp
template <typename T>  // ou <class T>
class Array
{
private:
    T entrada;
    ...
};
template <typename Tipo>
class ArrayCrescente : public Array<Tipo> {...};   // herança
template <typename Tp>
class Pilha
{
    Array<Tp> ar;   // usa um Array<> como componente
    ...
};
...
Array<Pilha<int>> asi;  // um array de pilhas de int
```

Na última instrução, o C++98 exigia separar os dois símbolos `>` por pelo menos um caractere de espaço em branco para evitar confusão com o operador `>>`. O C++11 remove esse requisito.

### Usando um Template Recursivamente

Outro exemplo de versatilidade dos templates é que você pode usá-los recursivamente. Por exemplo, dada a definição anterior de um template de array, você pode usá-lo da seguinte forma:

```cpp
ArrayTP< ArrayTP<int,5>, 10> doisD;
```

Isso torna `doisD` um array de 10 elementos, cada um dos quais é um array de cinco `int`s. O array simples equivalente teria esta declaração:

```cpp
int doisD[10][5];
```

Observe que a sintaxe dos templates apresenta as dimensões na ordem inversa à do equivalente array bidimensional comum. O programa da Listagem 14.18 experimenta essa ideia. Ele também usa o template `ArrayTP` para criar arrays unidimensionais para armazenar a soma e a média de cada um dos 10 conjuntos de cinco números.

**Listagem 14.18 `doisD.cpp`**

```cpp
// doisD.cpp -- criando um array 2-D
#include <iostream>
#include "arrayTp.h"
int main(void)
{
    using std::cout;
    using std::endl;
    ArrayTP<int, 10> somas;
    ArrayTP<double, 10> medias;
    ArrayTP< ArrayTP<int,5>, 10> doisD;
    int i, j;
    for (i = 0; i < 10; i++)
    {
        somas[i] = 0;
        for (j = 0; j < 5; j++)
        {
            doisD[i][j] = (i + 1) * (j + 1);
            somas[i] += doisD[i][j];
        }
        medias[i] = (double) somas[i] / 10;
    }
    for (i = 0; i < 10; i++)
    {
        for (j = 0; j < 5; j++)
        {
            cout.width(2);
            cout << doisD[i][j] << ' ';
        }
        cout << ": soma = ";
        cout.width(3);
        cout << somas[i] << ", media = " << medias[i] << endl;
    }
    cout << "Concluido.\n";
    return 0;
}
```

A saída do programa tem uma linha para cada um dos 10 elementos de `doisD`, cada um dos quais é um array de cinco elementos:

```
 1  2  3  4  5 : soma =  15, media = 1.5
 2  4  6  8 10 : soma =  30, media = 3
 3  6  9 12 15 : soma =  45, media = 4.5
 4  8 12 16 20 : soma =  60, media = 6
 5 10 15 20 25 : soma =  75, media = 7.5
 6 12 18 24 30 : soma =  90, media = 9
 7 14 21 28 35 : soma = 105, media = 10.5
 8 16 24 32 40 : soma = 120, media = 12
 9 18 27 36 45 : soma = 135, media = 13.5
10 20 30 40 50 : soma = 150, media = 15
Concluido.
```

### Usando Mais de Um Parâmetro de Tipo

Você pode ter um template com mais de um parâmetro de tipo. Por exemplo, suponha que queira uma classe que armazene dois tipos de valores. Você pode criar e usar uma classe template `Par` para armazenar dois valores distintos. (Incidentalmente, a STL fornece um template similar chamado `pair`.) O breve programa da Listagem 14.19 mostra um exemplo. Nele, os métodos `primeiro() const` e `segundo() const` relatam os valores armazenados, e os métodos `primeiro()` e `segundo()`, por virtue de retornarem referências para os membros de dados de `Par`, permitem redefinir os valores armazenados por meio de atribuição.

**Listagem 14.19 `pares.cpp`**

```cpp
// pares.cpp -- definindo e usando um template Par
#include <iostream>
#include <string>
template <class T1, class T2>
class Par
{
private:
    T1 a;
    T2 b;
public:
    T1 & primeiro();
    T2 & segundo();
    T1 primeiro() const { return a; }
    T2 segundo() const { return b; }
    Par(const T1 & aval, const T2 & bval) : a(aval), b(bval) {}
    Par() {}
};
template<class T1, class T2>
T1 & Par<T1,T2>::primeiro()
{
    return a;
}
template<class T1, class T2>
T2 & Par<T1,T2>::segundo()
{
    return b;
}
int main()
{
    using std::cout;
    using std::endl;
    using std::string;
    Par<string, int> avaliacoes[4] =
    {
        Par<string, int>("The Purpled Duck", 5),
        Par<string, int>("Jaquie's Frisco Al Fresco", 4),
        Par<string, int>("Cafe Souffle", 5),
        Par<string, int>("Bertie's Eats", 3)
    };
    int restaurantes = sizeof(avaliacoes) / sizeof(Par<string, int>);
    cout << "Avaliacao:\t Restaurante\n";
    for (int i = 0; i < restaurantes; i++)
        cout << avaliacoes[i].segundo() << ":\t "
             << avaliacoes[i].primeiro() << endl;
    cout << "Ops! Avaliacao revisada:\n";
    avaliacoes[3].primeiro() = "Bertie's Fab Eats";
    avaliacoes[3].segundo() = 6;
    cout << avaliacoes[3].segundo() << ":\t "
         << avaliacoes[3].primeiro() << endl;
    return 0;
}
```

Uma coisa a observar na Listagem 14.19 é que em `main()`, você tem que usar `Par<string,int>` para invocar os construtores e como argumento de `sizeof`. Isso porque `Par<string,int>` e não `Par` é o nome da classe.

Aqui está a saída do programa da Listagem 14.19:

```
Avaliacao:   Restaurante
5:   The Purpled Duck
4:   Jaquie's Frisco Al Fresco
5:   Cafe Souffle
3:   Bertie's Eats
Ops! Avaliacao revisada:
6:   Bertie's Fab Eats
```

### Parâmetros de Tipo com Valor Padrão

Outro novo recurso de templates de classe é que você pode fornecer valores padrão para parâmetros de tipo:

```cpp
template <class T1, class T2 = int> class Topo {...};
```

Isso faz o compilador usar `int` para o tipo `T2` se um valor para `T2` for omitido:

```cpp
Topo<double, double> m1;   // T1 é double, T2 é double
Topo<double> m2;           // T1 é double, T2 é int
```

A STL (discutida no Capítulo 16) frequentemente usa esse recurso, com o tipo padrão sendo uma classe.

Embora você possa fornecer valores padrão para parâmetros de tipo de templates de classe, você não pode fazer isso para parâmetros de templates de função. No entanto, você pode fornecer valores padrão para parâmetros não-tipo tanto para templates de classe quanto de função.

## Especializações de Templates

Classes template são como templates de função em que você pode ter instanciações implícitas, instanciações explícitas e especializações explícitas, coletivamente conhecidas como *especializações* (specializations). Ou seja, um template descreve uma classe em termos de um tipo geral, enquanto uma especialização é uma declaração de classe gerada usando um tipo específico.

### Instanciações Implícitas

Os exemplos de template que você viu até agora neste capítulo usam instanciações implícitas. Ou seja, eles declaram um ou mais objetos indicando o tipo desejado, e o compilador gera uma definição de classe especializada usando a receita fornecida pelo template geral:

```cpp
ArrayTP<int, 100> material;   // instanciação implícita
```

O compilador não gera uma instanciação implícita da classe até que precise de um objeto:

```cpp
ArrayTP<double, 30> * pt;     // um ponteiro, nenhum objeto necessário ainda
pt = new ArrayTP<double, 30>; // agora um objeto é necessário
```

### Instanciações Explícitas

O compilador gera uma *instanciação explícita* de uma declaração de classe quando você declara uma classe usando a palavra-chave `template` e indicando o tipo ou tipos desejados. A declaração deve estar no mesmo namespace que a definição do template. Por exemplo, a seguinte declaração declara `ArrayTP<string, 100>` como uma classe:

```cpp
template class ArrayTP<string, 100>;  // gera a classe ArrayTP<string, 100>
```

Nesse caso, o compilador gera a definição da classe, incluindo definições de método, mesmo que nenhum objeto da classe tenha sido ainda criado ou mencionado.

### Especializações Explícitas

Uma *especialização explícita* é uma definição para um tipo particular ou tipos que deve ser usada em vez do template geral. Às vezes você pode precisar ou querer modificar um template para se comportar de forma diferente quando instanciado para um tipo específico; nesse caso, você pode criar uma especialização explícita. Suponha, por exemplo, que você tenha definido um template para uma classe que representa um array ordenado no qual itens são ordenados à medida que são adicionados ao array:

```cpp
template <typename T>
class ArrayOrdenado
{
    ...  // detalhes omitidos
};
```

Suponha também que o template usa o operador `>` para comparar valores. Isso funciona bem para números. Funcionará se `T` representar um tipo de classe, também, desde que você tenha definido um método `T::operator>()`. Mas não funcionará se `T` for uma string representada pelo tipo `const char *`. Na verdade, o template funcionará, mas as strings terminarão sendo ordenadas por endereço em vez de alfabeticamente. O que é necessário é uma definição de classe que use `strcmp()` em vez de `>`. Nesse caso, você pode fornecer uma especialização explícita de template. Ela tem a seguinte forma:

```cpp
template <> class NomeClasse<nome-tipo-especializado> { ... };
```

Para fornecer uma especialização do template `ArrayOrdenado` para o tipo `const char *`, você usaria código como o seguinte:

```cpp
template <> class ArrayOrdenado<const char *>
{
    ...  // detalhes omitidos, usa strcmp() em vez de >
};
```

Agora, pedidos de um template `ArrayOrdenado` de `const char *` usarão essa definição especializada em vez da definição de template mais geral:

```cpp
ArrayOrdenado<int> pontuacoes;          // usa definição geral
ArrayOrdenado<const char *> datas;     // usa definição especializada
```

### Especializações Parciais

O C++ permite *especializações parciais*, que restringem parcialmente a generalidade de um template. Por exemplo, uma especialização parcial pode fornecer um tipo específico para um dos parâmetros de tipo:

```cpp
// template geral
template <class T1, class T2> class Par {...};
// especialização com T2 definido como int
template <class T1> class Par<T1, int> {...};
```

Os `<>` seguindo a palavra-chave `template` declaram os parâmetros de tipo que ainda não foram especializados. Portanto, a segunda declaração especializa `T2` para `int` mas mantém `T1` aberto. Especificar todos os tipos leva a um par de colchetes vazio e a uma especialização explícita completa:

```cpp
// especialização com T1 e T2 definidos como int
template <> class Par<int, int> {...};
```

O compilador usa o template mais especializado se houver uma escolha:

```cpp
Par<double, double> p1;   // usa template geral de Par
Par<double, int> p2;      // usa especialização parcial Par<T1, int>
Par<int, int> p3;         // usa especialização explícita Par<int, int>
```

Ou você pode especializar parcialmente um template existente fornecendo uma versão especial para ponteiros:

```cpp
template<class T>    // versão geral
class Feeb { ... };
template<class T*>   // especialização parcial de ponteiro
class Feeb { ... };  // código modificado
```

Se você fornecer um tipo não ponteiro, o compilador usará a versão geral; se fornecer um ponteiro, o compilador usará a especialização de ponteiro:

```cpp
Feeb<char> fb1;     // usa template geral de Feeb, T é char
Feeb<char *> fb2;   // usa especialização Feeb T*, T é char
```

O recurso de especialização parcial permite fazer uma variedade de restrições:

```cpp
// template geral
template <class T1, class T2, class T3> class Trio{...};
// especialização com T3 definido como T2
template <class T1, class T2> class Trio<T1, T2, T2> {...};
// especialização com T3 e T2 definidos como T1*
template <class T1> class Trio<T1, T1*, T1*> {...};
```

Dadas essas declarações, o compilador faria as seguintes escolhas:

```cpp
Trio<int, short, char *> t1;         // usa template geral
Trio<int, short> t2;                 // usa Trio<T1, T2, T2>
Trio<char, char *, char *> t3;       // usa Trio<T1, T1*, T1*>
```

## Templates Membros

Um template pode ser um membro de uma estrutura, classe ou classe template. A STL requer esse recurso para implementar completamente seu design. A Listagem 14.20 fornece um breve exemplo de uma classe template com uma classe template aninhada e uma função template como membros.

**Listagem 14.20 `membrosTemp.cpp`**

```cpp
// membrosTemp.cpp -- membros template
#include <iostream>
using std::cout;
using std::endl;
template <typename T>
class beta
{
private:
    template <typename V>   // classe membro template aninhada
    class suporte
    {
    private:
        V val;
    public:
        suporte(V v = 0) : val(v) {}
        void mostrar() const { cout << val << endl; }
        V Valor() const { return val; }
    };
    suporte<T> q;    // objeto template
    suporte<int> n;  // objeto template
public:
    beta(T t, int i) : q(t), n(i) {}
    template<typename U>   // método template
    U blab(U u, T t) { return (n.Valor() + q.Valor()) * u / t; }
    void Mostrar() const { q.mostrar(); n.mostrar(); }
};
int main()
{
    beta<double> cara(3.5, 3);
    cout << "T foi definido como double\n";
    cara.Mostrar();
    cout << "V foi definido como T, que e double, depois V foi definido como int\n";
    cout << cara.blab(10, 2.3) << endl;
    cout << "U foi definido como int\n";
    cout << cara.blab(10.0, 2.3) << endl;
    cout << "U foi definido como double\n";
    cout << "Concluido\n";
    return 0;
}
```

O template `suporte` é declarado na seção privada na Listagem 14.20, portanto é acessível apenas dentro do escopo da classe `beta`. A classe `beta` usa o template `suporte` para declarar dois membros de dados: `n`, um objeto `suporte` baseado no tipo `int`, e `q`, um objeto `suporte` baseado no tipo `T` (o parâmetro de template de `beta`). Em `main()`, a seguinte declaração faz `T` representar `double`, tornando `q` do tipo `suporte<double>`:

```cpp
beta<double> cara(3.5, 3);
```

O método `blab()` tem um tipo (`U`) determinado implicitamente pelo valor do argumento quando o método é chamado, e um tipo (`T`) determinado pelo tipo de instanciação do objeto.

Aqui está a saída do programa:

```
T foi definido como double
3.5
3
V foi definido como T, que e double, depois V foi definido como int
28
U foi definido como double
28.2609
Concluido
```

## Templates como Parâmetros

Você viu que um template pode ter parâmetros de tipo, como `typename T`, e parâmetros não-tipo, como `int n`. Um template também pode ter um parâmetro que é ele próprio um template. Tais parâmetros são mais um recurso de template usado para implementar a STL.

A Listagem 14.21 mostra um exemplo que começa com estas linhas:

```cpp
template <template <typename T> class Coisa>
class Caranguejo
```

O parâmetro de template é `template <typename T> class Coisa`. Aqui `template <typename T> class` é o tipo, e `Coisa` é o parâmetro. O que isso implica? Suponha que você tenha esta declaração:

```cpp
Caranguejo<Rei> pernas;
```

Para isso ser aceito, o argumento de template `Rei` deve ser uma classe template cuja declaração corresponda à do parâmetro de template `Coisa`:

```cpp
template <typename T>
class Rei {...};
```

A declaração `Caranguejo` na Listagem 14.21 declara dois objetos:

```cpp
Coisa<int> s1;
Coisa<double> s2;
```

**Listagem 14.21 `parametroTemp.cpp`**

```cpp
// parametroTemp.cpp -- templates como parâmetros
#include <iostream>
#include "pilhaTp.h"
template <template <typename T> class Coisa>
class Caranguejo
{
private:
    Coisa<int> s1;
    Coisa<double> s2;
public:
    Caranguejo() {};
    // supõe que a classe Coisa tem métodos empilhar() e desempilhar()
    bool empilhar(int a, double x) { return s1.empilhar(a) && s2.empilhar(x); }
    bool desempilhar(int & a, double & x) { return s1.desempilhar(a) && s2.desempilhar(x); }
};
int main()
{
    using std::cout;
    using std::cin;
    using std::endl;
    Caranguejo<Pilha> nebula;
    // Pilha deve corresponder a template <typename T> class Coisa
    int ni;
    double nb;
    cout << "Digite pares int double, como 4 3.5 (0 0 para terminar):\n";
    while (cin >> ni >> nb && ni > 0 && nb > 0)
    {
        if (!nebula.empilhar(ni, nb))
            break;
    }
    while (nebula.desempilhar(ni, nb))
        cout << ni << ", " << nb << endl;
    cout << "Concluido.\n";
    return 0;
}
```

Aqui está um exemplo de execução do programa da Listagem 14.21:

```
Digite pares int double, como 4 3.5 (0 0 para terminar):
50 22.48
25 33.87
60 19.12
0 0
60, 19.12
25, 33.87
50, 22.48
Concluido.
```

Você pode misturar parâmetros de template com parâmetros regulares. Por exemplo, a declaração da classe `Caranguejo` poderia começar assim:

```cpp
template <template <typename T> class Coisa, typename U, typename V>
class Caranguejo
{
private:
    Coisa<U> s1;
    Coisa<V> s2;
    ...
```

Agora os tipos a serem armazenados nos membros `s1` e `s2` são tipos genéricos em vez de tipos codificados fixos. Isso exigiria que a declaração de `nebula` no programa fosse alterada para:

```cpp
Caranguejo<Pilha, int, double> nebula;  // T=Pilha, U=int, V=double
```

## Classes Template e Funções Amigas

Declarações de classes template também podem ter funções amigas. Você pode classificar as funções amigas de templates em três categorias:

- Funções amigas não-template
- Funções amigas de template *vinculadas* (bound), significando que o tipo da função amiga é determinado pelo tipo da classe quando a classe é instanciada
- Funções amigas de template *não vinculadas* (unbound), significando que todas as especializações da função amiga são amigas de cada especialização da classe

### Funções Amigas Não-Template para Classes Template

Vamos declarar uma função comum dentro de uma classe template como função amiga:

```cpp
template <class T>
class TemAmigo
{
public:
    friend void contagens();  // amiga de todas as instanciações de TemAmigo
    ...
};
```

Essa declaração torna a função `contagens()` amiga de todas as instanciações possíveis do template. Por exemplo, ela seria amiga da classe `TemAmigo<int>` e da classe `TemAmigo<string>`.

Suponha que você queira fornecer um argumento de classe template para uma função amiga. Você pode usar assim:

```cpp
template <class T>
class TemAmigo
{
    friend void relatorio(TemAmigo<T> &);  // amiga template vinculada
    ...
};
```

Para entender o que isso faz, imagine a especialização produzida se você declarar um objeto de um tipo particular:

```cpp
TemAmigo<int> hf;
```

O compilador substituiria o parâmetro de template `T` por `int`, dando à declaração de amigo esta forma:

```cpp
class TemAmigo<int>
{
    friend void relatorio(TemAmigo<int> &);  // amiga template vinculada
    ...
};
```

Ou seja, `relatorio()` com um parâmetro `TemAmigo<int>` torna-se amigo da classe `TemAmigo<int>`. Da mesma forma, `relatorio()` com um parâmetro `TemAmigo<double>` seria uma versão sobrecarregada de `relatorio()` que é amiga da classe `TemAmigo<double>`.

Observe que `relatorio()` não é em si uma função template; ela apenas tem um parâmetro que é um template. Isso significa que você tem que definir especializações explícitas para as amigas que planeja usar.

A Listagem 14.22 ilustra esses pontos. O template `TemAmigo` tem um membro de dado estático `cont`. Note que isso significa que cada especialização particular da classe tem seu próprio membro estático. A função `contagens()`, que é amiga de todas as especializações de `TemAmigo`, relata o valor de `cont` para duas especializações particulares: `TemAmigo<int>` e `TemAmigo<double>`. O programa também fornece duas funções `relatorios()`, cada uma das quais é amiga de uma especialização particular de `TemAmigo`.

**Listagem 14.22 `amigo2Tmp.cpp`**

```cpp
// amigo2Tmp.cpp -- classe template com funções amigas não-template
#include <iostream>
using std::cout;
using std::endl;
template <typename T>
class TemAmigo
{
private:
    T item;
    static int cont;
public:
    TemAmigo(const T & i) : item(i) { cont++; }
    ~TemAmigo() { cont--; }
    friend void contagens();
    friend void relatorios(TemAmigo<T> &);  // parâmetro de template
};
// cada especialização tem seu próprio membro de dado estático
template <typename T>
int TemAmigo<T>::cont = 0;
// função amiga não-template de todas as classes TemAmigo<T>
void contagens()
{
    cout << "contagem int: " << TemAmigo<int>::cont << "; ";
    cout << "contagem double: " << TemAmigo<double>::cont << endl;
}
// função amiga não-template da classe TemAmigo<int>
void relatorios(TemAmigo<int> & hf)
{
    cout << "TemAmigo<int>: " << hf.item << endl;
}
// função amiga não-template da classe TemAmigo<double>
void relatorios(TemAmigo<double> & hf)
{
    cout << "TemAmigo<double>: " << hf.item << endl;
}
int main()
{
    cout << "Nenhum objeto declarado: ";
    contagens();
    TemAmigo<int> hfi1(10);
    cout << "Apos hfi1 declarado: ";
    contagens();
    TemAmigo<int> hfi2(20);
    cout << "Apos hfi2 declarado: ";
    contagens();
    TemAmigo<double> hfdb(10.5);
    cout << "Apos hfdb declarado: ";
    contagens();
    relatorios(hfi1);
    relatorios(hfi2);
    relatorios(hfdb);
    return 0;
}
```

Aqui está a saída do programa da Listagem 14.22:

```
Nenhum objeto declarado: contagem int: 0; contagem double: 0
Apos hfi1 declarado: contagem int: 1; contagem double: 0
Apos hfi2 declarado: contagem int: 2; contagem double: 0
Apos hfdb declarado: contagem int: 2; contagem double: 1
TemAmigo<int>: 10
TemAmigo<int>: 20
TemAmigo<double>: 10.5
```

### Funções Amigas de Template Vinculadas para Classes Template

Você pode modificar o exemplo anterior tornando as funções amigas em si próprias templates. Em particular, você pode configurar funções amigas de template vinculadas, de modo que cada especialização de uma classe obtenha uma especialização correspondente para uma função amiga. A técnica é um pouco mais complexa e envolve três etapas.

Para a primeira etapa, você declara cada função template antes da definição da classe:

```cpp
template <typename T> void contagens();
template <typename T> void relatorio(T &);
```

Em seguida, você declara os templates novamente como funções amigas dentro da função. Essas declarações declaram especializações baseadas no tipo do parâmetro de template da classe:

```cpp
template <typename TT>
class TemAmigoT
{
    ...
    friend void contagens<TT>();
    friend void relatorio<>(TemAmigoT<TT> &);
};
```

Os `<>` nas declarações identificam essas como especializações de template. No caso de `relatorio()`, os `<>` podem ficar vazios porque o seguinte argumento de tipo de template pode ser deduzido a partir do argumento da função `TemAmigoT<TT>`. Você poderia, no entanto, usar `relatorio<TemAmigoT<TT>>(TemAmigoT<TT> &)`.

O terceiro requisito é que o programa deve fornecer definições de template para os amigos. A Listagem 14.23 ilustra esses três aspectos.

**Listagem 14.23 `tmp2tmp.cpp`**

```cpp
// tmp2tmp.cpp -- funções amigas template para uma classe template
#include <iostream>
using std::cout;
using std::endl;
// protótipos template
template <typename T> void contagens();
template <typename T> void relatorio(T &);
// classe template
template <typename TT>
class TemAmigoT
{
private:
    TT item;
    static int cont;
public:
    TemAmigoT(const TT & i) : item(i) { cont++; }
    ~TemAmigoT() { cont--; }
    friend void contagens<TT>();
    friend void relatorio<>(TemAmigoT<TT> &);
};
template <typename T>
int TemAmigoT<T>::cont = 0;
// definições de funções amigas template
template <typename T>
void contagens()
{
    cout << "tamanho do template: " << sizeof(TemAmigoT<T>) << "; ";
    cout << "contagens() do template: " << TemAmigoT<T>::cont << endl;
}
template <typename T>
void relatorio(T & hf)
{
    cout << hf.item << endl;
}
int main()
{
    contagens<int>();
    TemAmigoT<int> hfi1(10);
    TemAmigoT<int> hfi2(20);
    TemAmigoT<double> hfdb(10.5);
    relatorio(hfi1);   // gera relatorio(TemAmigoT<int> &)
    relatorio(hfi2);   // gera relatorio(TemAmigoT<int> &)
    relatorio(hfdb);   // gera relatorio(TemAmigoT<double> &)
    cout << "saida de contagens<int>():\n";
    contagens<int>();
    cout << "saida de contagens<double>():\n";
    contagens<double>();
    return 0;
}
```

Aqui está a saída do programa da Listagem 14.23:

```
tamanho do template: 4; contagens() do template: 0
10
20
10.5
saida de contagens<int>():
tamanho do template: 4; contagens() do template: 2
saida de contagens<double>():
tamanho do template: 8; contagens() do template: 1
```

Como você pode ver, `contagens<double>` relata um tamanho de template diferente de `contagens<int>`, demonstrando que cada tipo `T` agora tem sua própria função amiga `contagens()`.

### Funções Amigas de Template Não Vinculadas para Classes Template

As funções amigas de template vinculadas da seção anterior são especializações de um template declarado fora de uma classe. Declarando um template dentro de uma classe, você pode criar funções amigas não vinculadas para as quais cada especialização de função é amiga de cada especialização de classe. Para amigos não vinculados, os parâmetros de tipo do template de amigo são diferentes dos parâmetros de tipo da classe template:

```cpp
template <typename T>
class MuitosAmigos
{
    ...
    template <typename C, typename D> friend void mostrar2(C &, D &);
};
```

A Listagem 14.24 mostra um exemplo usando um amigo não vinculado.

**Listagem 14.24 `muitosAmigos.cpp`**

```cpp
// muitosAmigos.cpp -- amigo template não vinculado para uma classe template
#include <iostream>
using std::cout;
using std::endl;
template <typename T>
class MuitosAmigos
{
private:
    T item;
public:
    MuitosAmigos(const T & i) : item(i) {}
    template <typename C, typename D> friend void mostrar2(C &, D &);
};
template <typename C, typename D> void mostrar2(C & c, D & d)
{
    cout << c.item << ", " << d.item << endl;
}
int main()
{
    MuitosAmigos<int> hfi1(10);
    MuitosAmigos<int> hfi2(20);
    MuitosAmigos<double> hfdb(10.5);
    cout << "hfi1, hfi2: ";
    mostrar2(hfi1, hfi2);
    cout << "hfdb, hfi2: ";
    mostrar2(hfdb, hfi2);
    return 0;
}
```

Aqui está a saída do programa da Listagem 14.24:

```
hfi1, hfi2: 10, 20
hfdb, hfi2: 10.5, 20
```

## Aliases de Template (C++11)

Pode ser conveniente, especialmente no design de templates, criar aliases para tipos. Você pode usar `typedef` para criar aliases para especializações de templates:

```cpp
// define três aliases typedef
typedef std::array<double, 12> arrD;
typedef std::array<int, 12> arrI;
typedef std::array<std::string, 12> arrSt;
arrD galoes;   // galoes é do tipo std::array<double, 12>
arrI dias;     // dias é do tipo std::array<int, 12>
arrSt meses;   // meses é do tipo std::array<std::string, 12>
```

Mas se você se encontrar escrevendo código similar a esses `typedef`s repetidamente, pode se perguntar se existe um recurso de linguagem que simplifique a tarefa. Nesse caso, o C++11 fornece um recurso que faltava anteriormente — uma maneira de usar um template para fornecer uma família de aliases. Aqui está o aspecto dessa abordagem:

```cpp
template<typename T>
    using tipoArr = std::array<T, 12>;  // template para criar múltiplos aliases
```

Isso torna `tipoArr` um alias de template que pode ser usado como um tipo:

```cpp
tipoArr<double> galoes;          // galoes é do tipo std::array<double, 12>
tipoArr<int> dias;               // dias é do tipo std::array<int, 12>
tipoArr<std::string> meses;      // meses é do tipo std::array<std::string, 12>
```

Em resumo, `tipoArr<T>` significa tipo `std::array<T, 12>`.

O C++11 estende a sintaxe `using =` também para não-templates. Nesse caso, torna-se equivalente a um `typedef` comum:

```cpp
typedef const char * pc1;          // sintaxe typedef
using pc2 = const char *;          // sintaxe using =
typedef const int *(*pa1)[10];     // sintaxe typedef
using pa2 = const int *(*)[10];    // sintaxe using =
```

À medida que você se acostuma com ela, pode achar a nova forma mais legível porque separa o nome do tipo das informações do tipo de forma mais clara.

Outra adição do C++11 a templates é o *template variádico* (variadic template), que permite definir um template de classe ou função que pode receber um número variável de inicializadores. O Capítulo 18, "Visitando o Novo Padrão C++", examina esse tópico.

---

[Anterior](capitulo-14-02-heranca-multipla.md) | [Índice](README.md) | [Próximo](capitulo-14-04-resumo.md)
