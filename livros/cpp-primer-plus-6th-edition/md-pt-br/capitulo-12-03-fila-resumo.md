# Capítulo 12 — Classes e Alocação Dinâmica de Memória (Parte 3: Simulação de Fila, Resumo e Exercícios)

> Tradução não oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Navegação: [Anterior](capitulo-12-02-retorno-ponteiros.md) | [Índice](README.md) | [Próximo](capitulo-13-01-heranca.md)

---

## Uma Simulação de Fila

Vamos aplicar sua compreensão aprimorada de classes a um problema de programação. O Banco de Heather quer abrir um caixa eletrônico (ATM — *Automatic Teller Machine*) no supermercado Food Heap. A gerência do Food Heap está preocupada com filas no ATM que interfiram no fluxo de tráfego no mercado e pode querer impor um limite no número de pessoas autorizadas a fazer fila no ATM. As pessoas do Banco de Heather querem estimativas de quanto tempo os clientes terão que esperar na fila. Sua tarefa é preparar um programa que simule a situação para que a gerência possa ver qual será o efeito do ATM.

Uma maneira bastante natural de representar o problema é usar uma fila de clientes. Uma **fila** (*queue*) é um tipo de dado abstrato (ADT) que armazena uma sequência ordenada de itens. Novos itens são adicionados ao fim da fila, e itens podem ser removidos da frente. Uma fila é um pouco como uma pilha, exceto que uma pilha tem adições e remoções na mesma extremidade. Isso torna uma pilha uma estrutura LIFO (*last in, first out* — último a entrar, primeiro a sair), enquanto a fila é uma estrutura FIFO (*first in, first out* — primeiro a entrar, primeiro a sair). Conceitualmente, uma fila é como uma fila em um caixa ou ATM, portanto é ideal para a tarefa. Então uma parte do projeto é definir uma classe `Fila`. (No Capítulo 16, você lerá sobre a classe `queue` da Biblioteca de Templates Padrão, mas aprenderá mais desenvolvendo a sua própria do que apenas lendo sobre tal classe.)

Os itens na fila serão clientes. Um representante do Banco de Heather informa que, em média, um terço dos clientes levará um minuto para ser processado, um terço levará dois minutos e um terço levará três minutos. Além disso, os clientes chegam em intervalos aleatórios, mas o número médio de clientes por hora é bastante constante. Outras duas partes do seu projeto serão projetar uma classe que representa clientes e montar um programa que simule as interações entre clientes e a fila.

### A Classe Fila

A primeira ordem do dia é projetar uma classe `Fila`. Primeiro, você precisa listar os atributos do tipo de fila de que você precisará:

- Uma fila armazena uma sequência ordenada de itens.
- Uma fila tem um limite no número de itens que pode armazenar.
- Você deve ser capaz de criar uma fila vazia.
- Você deve ser capaz de verificar se uma fila está vazia.
- Você deve ser capaz de verificar se uma fila está cheia.
- Você deve ser capaz de adicionar um item ao final de uma fila.
- Você deve ser capaz de remover um item da frente de uma fila.
- Você deve ser capaz de determinar o número de itens na fila.

Como de costume, ao projetar uma classe, você precisa desenvolver uma interface pública e uma implementação privada.

**A Interface da Classe Fila**

Os atributos de fila listados na seção anterior sugerem a seguinte interface pública para uma classe fila:

```cpp
class Fila
{
    enum {TAM_F = 10};
private:
    // representação privada a ser desenvolvida depois
public:
    Fila(int qs = TAM_F); // cria fila com limite qs
    ~Fila();
    bool esta_vazia() const;
    bool esta_cheia() const;
    int contagem_fila() const;
    bool enfileirar(const Item &item); // adiciona item ao fim
    bool desenfileirar(Item &item);    // remove item da frente
};
```

O construtor cria uma fila vazia. Por padrão, a fila pode armazenar até 10 itens, mas isso pode ser substituído com um argumento de inicialização explícito:

```cpp
Fila fila1;     // fila com limite de 10 itens
Fila fila2(20); // fila com limite de 20 itens
```

Ao usar a fila, você pode usar um `typedef` para definir `Item`. (No Capítulo 14, "Reutilizando Código em C++", você aprenderá como usar templates de classe em vez disso.)

**A Implementação da Classe Fila**

Depois de determinar a interface, você pode implementá-la. Primeiro, você precisa decidir como representar os dados da fila. Uma abordagem é usar `new` para alocar dinamicamente um array com o número necessário de elementos. Porém, arrays não são uma boa correspondência para operações de fila. Por exemplo, remover um item da frente do array deve ser seguido por deslocar cada elemento restante uma unidade mais perto da frente. Caso contrário, você precisaria fazer algo mais elaborado, como tratar o array como circular. Usar uma **lista encadeada** (*linked list*), porém, é uma correspondência razoável aos requisitos de uma fila. Uma lista encadeada consiste em uma sequência de nós. Cada **nó** (*node*) contém as informações a serem armazenadas na lista, mais um ponteiro para o próximo nó na lista. Para a fila neste exemplo, cada parte de dados é um valor do tipo `Item`, e você pode usar uma estrutura para representar um nó:

```cpp
struct No
{
    Item item;      // dados armazenados no nó
    struct No * prox; // ponteiro para o próximo nó
};
```

O exemplo mostrado é chamado de **lista simplesmente encadeada** (*singly linked list*) porque cada nó tem um único link, ou ponteiro, para outro nó. Se você tem o endereço do primeiro nó, pode seguir os ponteiros para cada nó subsequente na lista. Comumente, o ponteiro no último nó da lista é definido como `NULL` (ou equivalentemente, como `0`) para indicar que não há mais nós. Com C++11, você deve usar a nova palavra-chave `nullptr`. Para acompanhar uma lista encadeada, você deve saber o endereço do primeiro nó. Você pode usar um membro de dados da classe `Fila` para apontar para o início da lista. Em princípio, isso é tudo de que você precisa porque pode percorrer a cadeia de nós para encontrar qualquer outro nó. Porém, como uma fila sempre adiciona um novo item ao final da fila, é conveniente ter um membro de dados apontando para o último nó também. Além disso, você pode usar membros de dados para acompanhar o número máximo de itens permitidos na fila e o número atual de itens. Assim, a parte privada da declaração de classe pode ser parecida com isto:

```cpp
class Fila
{
private:
// definições de escopo de classe
    // No é uma definição de estrutura aninhada local a esta classe
    struct No { Item item; struct No * prox;};
    enum {TAM_F = 10};
// membros de classe privados
    No * frente;      // ponteiro para a frente da Fila
    No * fundo;       // ponteiro para o fundo da Fila
    int itens;        // número atual de itens na Fila
    const int tam_fila; // número máximo de itens na Fila
    ...
public:
//...
};
```

A declaração usa a capacidade do C++ de aninhar uma declaração de estrutura ou classe dentro de uma classe. Ao colocar a declaração `No` dentro da classe `Fila`, você lhe dá escopo de classe. Ou seja, `No` é um tipo que você pode usar para declarar membros de classe e como nome de tipo em métodos de classe, mas o tipo é restrito à classe. Dessa forma, você não precisa se preocupar com essa declaração de `No` conflitando com alguma declaração global ou com um `No` declarado dentro de alguma outra classe.

> **Estruturas e Classes Aninhadas**
> Uma estrutura, classe ou enumeração declarada dentro de uma declaração de classe é dita **aninhada** na classe. Ela tem escopo de classe. Tal declaração não cria um objeto de dados. Em vez disso, especifica um tipo que pode ser usado internamente dentro da classe. Se a declaração é feita na seção privada da classe, então o tipo declarado pode ser usado apenas dentro da classe. Se a declaração é feita na seção pública, então o tipo declarado também pode ser usado fora da classe, por meio do uso do operador de resolução de escopo. Por exemplo, se `No` fosse declarado na seção pública da classe `Fila`, você poderia declarar variáveis do tipo `Fila::No` fora da classe `Fila`.

**Os Métodos da Classe**

Um construtor de classe deve fornecer valores para os membros de classe. Como a fila neste exemplo começa em estado vazio, você deve definir os ponteiros de frente e fundo como `NULL` (ou `0` ou `nullptr`) e `itens` como `0`. Além disso, deve definir o tamanho máximo da fila `tam_fila` como o argumento de construtor `qs`. Aqui está uma implementação que não funciona:

```cpp
Fila::Fila(int qs)
{
    frente = fundo = NULL;
    itens = 0;
    tam_fila = qs; // não aceitável!
}
```

O problema é que `tam_fila` é uma constante, portanto pode ser *inicializada* para um valor, mas não pode *receber* um valor. Conceitualmente, chamar um construtor cria um objeto antes que o código dentro dos colchetes seja executado. Assim, chamar o construtor `Fila(int qs)` faz com que o programa primeiro aloque espaço para os quatro membros de variável. Então o fluxo do programa entra nos colchetes e usa atribuição ordinária para colocar valores no espaço alocado. Portanto, se você quiser inicializar um membro de dado `const`, terá que fazê-lo quando o objeto for criado, antes que a execução chegue ao corpo do construtor. C++ fornece uma sintaxe especial para fazer exatamente isso. É chamada de **lista de inicializadores de membro** (*member initializer list*). A lista de inicializadores de membro consiste em uma lista separada por vírgulas de inicializadores precedida por dois pontos. É colocada após o parêntese fechado da lista de argumentos e antes do colchete de abertura do corpo da função. Se um membro de dados é chamado `mdados` e deve ser inicializado para o valor `val`, o inicializador tem a forma `mdados(val)`. Usando essa notação, você pode escrever o construtor de `Fila` assim:

```cpp
Fila::Fila(int qs) : tam_fila(qs) // inicializa tam_fila para qs
{
    frente = fundo = NULL;
    itens = 0;
}
```

Em geral, o valor inicial pode envolver constantes e argumentos da lista de argumentos do construtor. A técnica não se limita a inicializar constantes; você também pode escrever o construtor de `Fila` assim:

```cpp
Fila::Fila(int qs) : tam_fila(qs), frente(NULL), fundo(NULL), itens(0)
{
}
```

Apenas os construtores podem usar essa sintaxe de lista de inicializadores. Como você viu, deve usar essa sintaxe para membros de classe `const`. Você também deve usá-la para membros de classe declarados como referências:

```cpp
class Agencia {...};
class Agente
{
private:
    Agencia & pertence; // deve usar lista de inicializadores para inicializar
    ...
};
Agente::Agente(Agencia & a) : pertence(a) {...}
```

Isso ocorre porque as referências, como os dados `const`, só podem ser inicializadas quando criadas. Para membros de dados simples, como `frente` e `itens`, não faz muita diferença se você usa uma lista de inicializadores de membro ou usa atribuição no corpo da função. Como você verá no Capítulo 14, porém, é mais eficiente usar a lista de inicializadores de membro para membros que são eles próprios objetos de classe.

> **A Sintaxe da Lista de Inicializadores de Membro**
> Se `Classy` é uma classe e se `mem1`, `mem2` e `mem3` são membros de dados da classe, um construtor de classe pode usar a seguinte sintaxe para inicializar os membros de dados:
> ```cpp
> Classy::Classy(int n, int m) : mem1(n), mem2(0), mem3(n*m + 2)
> {
>     //...
> }
> ```
> Isso inicializa `mem1` para `n`, `mem2` para `0` e `mem3` para `n*m + 2`. Conceitualmente, essas inicializações ocorrem quando o objeto é criado e antes que qualquer código dentro dos colchetes seja executado. Observe o seguinte:
> - Esta forma só pode ser usada com construtores.
> - Você deve (pelo menos, em C++ anterior ao C++11) usar esta forma para inicializar um membro de dado não estático `const`.
> - Você deve usar esta forma para inicializar um membro de dado de referência.
>
> Os membros de dados são inicializados na ordem em que aparecem na declaração de classe, não na ordem em que os inicializadores são listados.

> **Cuidado**
> Você não pode usar a sintaxe de lista de inicializadores de membro com métodos de classe que não sejam construtores.

A forma entre parênteses usada na lista de inicializadores de membro também pode ser usada em inicializações comuns. Ou seja, se você quiser, pode substituir código como

```cpp
int jogos = 162;
double conversa = 2.71828;
```

por

```cpp
int jogos(162);
double conversa(2.71828);
```

Isso permite que a inicialização de tipos embutidos se pareça com a inicialização de objetos de classe.

> **Inicialização In-Class do C++11**
> C++11 permite que você faça o que parecia ser a coisa intuitivamente óbvia a fazer:
> ```cpp
> class Classy
> {
>     int mem1 = 10; // inicialização in-class
>     const int mem2 = 20; // inicialização in-class
>     //...
> };
> ```
> Isso é equivalente a usar uma lista de inicializadores de membro nos construtores:
> ```cpp
> Classy::Classy() : mem1(10), mem2(20) {...}
> ```
> Os membros `mem1` e `mem2` são inicializados com 10 e 20, respectivamente, a menos que um construtor usando uma lista de inicialização de membro seja chamado. Então a lista real substitui essas inicializações padrão:
> ```cpp
> Classy::Classy(int n) : mem1(n) {...}
> ```
> Neste caso, o construtor usaria o valor de `n` para inicializar `mem1`, e `mem2` ainda seria definido como 20.

O código para `esta_vazia()`, `esta_cheia()` e `contagem_fila()` é simples. Se `itens` é `0`, a fila está vazia. Se `itens` é `tam_fila`, a fila está cheia. Retornar o valor de `itens` responde à pergunta sobre quantos itens estão na fila. Você verá o código mais adiante neste capítulo na Listagem 12.11.

Adicionar um item ao final da fila (enfileirar) é mais envolvido. Aqui está uma abordagem:

```cpp
bool Fila::enfileirar(const Item & item)
{
    if (esta_cheia())
        return false;
    No * add = new No; // cria nó
    // em caso de falha, new lança exceção std::bad_alloc
    add->item = item; // define ponteiros do nó
    add->prox = NULL; // ou nullptr;
    itens++;
    if (frente == NULL)  // se a fila está vazia,
        frente = add;    // coloca o item na frente
    else
        fundo->prox = add; // senão coloca no fundo
    fundo = add;         // faz fundo apontar para o novo nó
    return true;
}
```

Resumidamente, o método passa pelas seguintes fases:

1. Termina se a fila já estiver cheia. (Para esta implementação, o tamanho máximo é selecionado pelo usuário via construtor.)
2. Cria um novo nó. Se `new` não puder fazê-lo, ele lança uma exceção, um tópico abordado no Capítulo 15, "Amigos, Exceções e Mais". O resultado prático é que, a menos que um programa forneça programação adicional para tratar a exceção, ele termina.
3. Coloca os valores adequados no nó. Neste caso, o código copia um valor `Item` na parte de dados do nó e define o ponteiro `prox` do nó como `NULL` (ou `0` ou, em C++11, `nullptr`). Isso prepara o nó para ser o último item na fila.
4. Aumenta a contagem de itens (`itens`) em um.
5. Anexa o nó ao fundo da fila. Há duas partes neste processo. A primeira é ligar o nó aos outros nós da lista. Isso é feito fazendo com que o ponteiro `prox` do nó atual no fundo aponte para o novo nó do fundo. A segunda parte é definir o ponteiro membro `fundo` da `Fila` para apontar para o novo nó para que a fila possa acessar o último nó diretamente. Se a fila está vazia, você também deve definir o ponteiro `frente` para apontar para o novo nó. (Se houver apenas um nó, ele é tanto o nó da frente quanto o do fundo.)

Remover um item da frente da fila (desenfileirar) também tem várias etapas. Aqui está uma abordagem:

```cpp
bool Fila::desenfileirar(Item & item)
{
    if (frente == NULL)
        return false;
    item = frente->item;  // define item para o primeiro item da fila
    itens--;
    No * temp = frente;   // salva a localização do primeiro item
    frente = frente->prox; // redefine frente para o próximo item
    delete temp;           // deleta o antigo primeiro item
    if (itens == 0)
        fundo = NULL;
    return true;
}
```

Resumidamente, o método passa pelas seguintes fases:

1. Termina se a fila já estiver vazia.
2. Fornece o primeiro item da fila para a função chamadora. Isso é feito copiando a porção de dados do nó `frente` atual para a variável de referência passada ao método.
3. Diminui a contagem de itens (`itens`) em um.
4. Salva o local do nó da frente para deleção posterior.
5. Retira o nó da fila. Isso é feito definindo o ponteiro membro `frente` da `Fila` para apontar para o próximo nó, cujo endereço é fornecido por `frente->prox`.
6. Para conservar memória, deleta o antigo primeiro nó.
7. Se a lista agora estiver vazia, define `fundo` como `NULL`. (O ponteiro `frente` já seria `NULL` neste caso, após definir `frente->prox`.) Novamente, você pode usar `0` em vez de `NULL`, ou, com C++11, pode usar `nullptr`.

O passo 4 é necessário porque o passo 5 apaga a memória da fila de onde estava o antigo primeiro nó.

**Outros Métodos de Classe?**

Você precisa de mais algum método? O construtor de classe não usa `new`, então à primeira vista pode parecer que você não precisa se preocupar com os requisitos especiais de classes que usam `new` nos construtores. Claro, esse primeiro olhar é enganoso porque adicionar objetos a uma fila invoca `new` para criar novos nós. É verdade que o método `desenfileirar()` faz a limpeza deletando nós, mas não há garantia de que uma fila estará vazia quando expirar. Portanto, a classe requer um destrutor explícito — que deleta todos os nós restantes. Aqui está uma implementação que começa na frente da lista e deleta cada nó por vez:

```cpp
Fila::~Fila()
{
    No * temp;
    while (frente != NULL) // enquanto a fila ainda não está vazia
    {
        temp = frente;      // salva endereço do item da frente
        frente = frente->prox; // redefine ponteiro para o próximo item
        delete temp;        // deleta o antigo primeiro item
    }
}
```

Hmm. Você viu que classes que usam `new` geralmente requerem construtores de cópia explícitos e operadores de atribuição que fazem cópia profunda. Esse é o caso aqui? A primeira questão a responder é: "A cópia membro a membro padrão faz a coisa certa?" A resposta é não. A cópia membro a membro de um objeto `Fila` produziria um novo objeto que aponta para a frente e o fundo da mesma lista encadeada do original. Assim, adicionar um item ao objeto `Fila` copiado altera a lista encadeada compartilhada. Isso já é ruim o suficiente. Pior ainda é que apenas o ponteiro `fundo` da cópia é atualizado, corrompendo essencialmente a lista do ponto de vista do objeto original. Claramente, então, clonar ou copiar filas requer fornecer um construtor de cópia e um construtor de atribuição que fazem cópia profunda.

Claro, isso levanta a questão de por que você quereria copiar uma fila. Bem, talvez você queira salvar instantâneos de uma fila em diferentes estágios de uma simulação. Ou você gostaria de fornecer entrada idêntica para duas estratégias diferentes. Na verdade, pode ser útil ter operações que dividem uma fila, da forma como os supermercados às vezes fazem ao abrir um caixa adicional. Da mesma forma, você pode querer combinar duas filas em uma ou truncar uma fila.

Mas suponha que você não queira fazer nenhuma dessas coisas nesta simulação. Você não pode simplesmente ignorar essas preocupações e usar os métodos que você já tem? Claro que pode. Porém, em algum momento no futuro, você pode precisar usar uma fila novamente, mas com cópia. E você pode se esquecer de que não forneceu o código adequado para cópia. Nesse caso, seus programas compilarão e executarão, mas gerarão resultados confusos e travamentos. Então parece melhor fornecer um construtor de cópia e um operador de atribuição, mesmo que você não precise deles agora.

Felizmente, há uma maneira furtiva de evitar esse trabalho extra enquanto ainda protege contra travamentos futuros de programa. A ideia é definir os métodos necessários como métodos privados fictícios:

```cpp
class Fila
{
private:
    Fila(const Fila & q) : tam_fila(0) { } // definição preventiva
    Fila & operator=(const Fila & q) { return *this;}
//...
};
```

Isso tem dois efeitos. Primeiro, substitui as definições de método padrão que de outra forma seriam geradas automaticamente. Segundo, como esses métodos são privados, eles não podem ser usados pelo mundo em geral. Ou seja, se `pronto` e `tucano` são objetos `Fila`, o compilador não permitirá o seguinte:

```cpp
Fila copia(pronto); // não permitido
tucano = pronto;    // não permitido
```

Portanto, em vez de se deparar com maus funcionamentos misteriosos em tempo de execução no futuro, você receberá um erro de compilador mais fácil de rastrear, informando que esses métodos não são acessíveis. Além disso, esse truque é útil quando você define uma classe cujos objetos realmente não devem ser copiados.

C++11 oferece uma maneira alternativa de desabilitar um método usando a palavra-chave `delete`; o Capítulo 18 retorna a este tópico.

### A Classe Cliente

Neste ponto, precisamos projetar uma classe de cliente. Em geral, um cliente de ATM tem muitas propriedades, como nome, números de conta e saldos. Porém, as únicas propriedades necessárias para a simulação são quando um cliente entra na fila e o tempo necessário para a transação do cliente. Quando a simulação produz um novo cliente, o programa deve criar um novo objeto cliente, armazenando nele o horário de chegada do cliente e um valor gerado aleatoriamente para o tempo de transação. Quando o cliente chega à frente da fila, o programa deve registrar o horário e subtrair o horário em que entrou na fila para obter o tempo de espera do cliente. Veja como você pode definir e implementar a classe `Cliente`:

```cpp
class Cliente
{
private:
    long chegada;     // horário de chegada para o cliente
    int tempo_proc;   // tempo de processamento para o cliente
public:
    Cliente() { chegada = tempo_proc = 0; }
    void definir(long quando);
    long quando() const { return chegada; }
    int tempo_p() const { return tempo_proc; }
};
void Cliente::definir(long quando)
{
    tempo_proc = std::rand() % 3 + 1;
    chegada = quando;
}
```

O construtor padrão cria um cliente nulo. A função membro `definir()` define o horário de chegada para seu argumento e escolhe aleatoriamente um valor de 1 a 3 para o tempo de processamento.

A Listagem 12.10 reúne as declarações das classes `Fila` e `Cliente`, e a Listagem 12.11 fornece os métodos.

### Listagem 12.10 — queue.h

```cpp
// queue.h -- interface para uma fila
#ifndef QUEUE_H_
#define QUEUE_H_

// Esta fila conterá itens do tipo Cliente
class Cliente
{
private:
    long chegada;      // horário de chegada para o cliente
    int tempo_proc;    // tempo de processamento para o cliente
public:
    Cliente() { chegada = tempo_proc = 0; }
    void definir(long quando);
    long quando() const { return chegada; }
    int tempo_p() const { return tempo_proc; }
};
typedef Cliente Item;
class Fila
{
private:
// definições de escopo de classe
    // No é uma definição de estrutura aninhada local a esta classe
    struct No { Item item; struct No * prox;};
    enum {TAM_F = 10};
// membros de classe privados
    No * frente;       // ponteiro para a frente da Fila
    No * fundo;        // ponteiro para o fundo da Fila
    int itens;         // número atual de itens na Fila
    const int tam_fila; // número máximo de itens na Fila
    // definições preventivas para impedir cópia pública
    Fila(const Fila & q) : tam_fila(0) { }
    Fila & operator=(const Fila & q) { return *this;}
public:
    Fila(int qs = TAM_F); // cria fila com limite qs
    ~Fila();
    bool esta_vazia() const;
    bool esta_cheia() const;
    int contagem_fila() const;
    bool enfileirar(const Item &item); // adiciona item ao fim
    bool desenfileirar(Item &item);    // remove item da frente
};
#endif
```

### Listagem 12.11 — queue.cpp

```cpp
// queue.cpp -- métodos de Fila e Cliente
#include "queue.h"
#include <cstdlib> // (ou stdlib.h) para rand()
// métodos de Fila
Fila::Fila(int qs) : tam_fila(qs)
{
    frente = fundo = NULL; // ou nullptr
    itens = 0;
}
Fila::~Fila()
{
    No * temp;
    while (frente != NULL) // enquanto a fila ainda não está vazia
    {
        temp = frente;       // salva endereço do item da frente
        frente = frente->prox; // redefine ponteiro para o próximo item
        delete temp;         // deleta o antigo primeiro item
    }
}
bool Fila::esta_vazia() const
{
    return itens == 0;
}
bool Fila::esta_cheia() const
{
    return itens == tam_fila;
}
int Fila::contagem_fila() const
{
    return itens;
}
// Adiciona item à fila
bool Fila::enfileirar(const Item & item)
{
    if (esta_cheia())
        return false;
    No * add = new No; // cria nó
    // em caso de falha, new lança exceção std::bad_alloc
    add->item = item; // define ponteiros do nó
    add->prox = NULL; // ou nullptr;
    itens++;
    if (frente == NULL)  // se a fila está vazia,
        frente = add;    // coloca o item na frente
    else
        fundo->prox = add; // senão coloca no fundo
    fundo = add;       // faz fundo apontar para o novo nó
    return true;
}
// Coloca o item da frente na variável item e remove da fila
bool Fila::desenfileirar(Item & item)
{
    if (frente == NULL)
        return false;
    item = frente->item;   // define item para o primeiro item da fila
    itens--;
    No * temp = frente;    // salva localização do primeiro item
    frente = frente->prox; // redefine frente para o próximo item
    delete temp;           // deleta o antigo primeiro item
    if (itens == 0)
        fundo = NULL;
    return true;
}
// método de cliente
// quando é o horário no qual o cliente chega
// o horário de chegada é definido para quando e o
// tempo de processamento é definido para um valor aleatório no intervalo 1 - 3
void Cliente::definir(long quando)
{
    tempo_proc = std::rand() % 3 + 1;
    chegada = quando;
}
```

### A Simulação do ATM

Agora você tem as ferramentas necessárias para a simulação do ATM. O programa deve permitir que o usuário insira três quantidades: o tamanho máximo da fila, o número de horas que o programa simulará e o número médio de clientes por hora. O programa deve usar um loop no qual cada ciclo representa um minuto. Durante cada ciclo de minuto, o programa deve fazer o seguinte:

1. Determinar se um novo cliente chegou. Se sim, adicionar o cliente à fila se houver espaço; caso contrário, recusar o cliente.
2. Se ninguém estiver sendo processado, pegar a primeira pessoa da fila. Determinar por quanto tempo a pessoa esperou e definir um contador `tempo_espera` para o tempo de processamento que o novo cliente precisará.
3. Se um cliente estiver sendo processado, decrementar o contador `tempo_espera` em um minuto.
4. Rastrear várias quantidades, como o número de clientes atendidos, o número de clientes recusados, os tempos acumulados de espera na fila e o comprimento acumulado da fila.

Quando o ciclo de simulação terminar, o programa deve relatar vários resultados estatísticos.

Um assunto interessante é como o programa determina se um novo cliente chegou. Suponha que, em média, 10 clientes cheguem por hora. Isso equivale a um cliente a cada 6 minutos. O programa calcula e armazena esse valor na variável `min_por_cliente`. Porém, ter um cliente aparecendo exatamente a cada 6 minutos não é realista. O que você realmente quer (pelo menos na maioria das vezes) é um processo mais aleatório que em média resulta em um cliente a cada 6 minutos. O programa usa esta função para determinar se um cliente aparece durante um ciclo:

```cpp
bool novo_cliente(double x)
{
    return (std::rand() * x / RAND_MAX < 1);
}
```

Veja como funciona. O valor `RAND_MAX` é definido no arquivo `cstdlib` (anteriormente `stdlib.h`) e representa o maior valor que a função `rand()` pode retornar (`0` é o menor valor). Suponha que `x`, o tempo médio entre clientes, seja 6. Então o valor de `rand() * x / RAND_MAX` será em algum lugar entre `0` e `6`. Em particular, será menor que `1` um sexto das vezes, em média. Porém, é possível que esta função produza dois clientes com 1 minuto de intervalo em um momento e dois clientes com 20 minutos de intervalo em outro. Esse comportamento leva à imprevisibilidade que muitas vezes distingue processos reais da regularidade relojoeira de exatamente um cliente a cada 6 minutos. Este método específico falha se o tempo médio entre chegadas cair abaixo de 1 minuto, mas a simulação não se destina a lidar com esse cenário.

A Listagem 12.12 apresenta os detalhes da simulação. Executar a simulação por um longo período de tempo fornece informações sobre médias de longo prazo, e executá-la por períodos curtos fornece informações sobre variações de curto prazo.

### Listagem 12.12 — bank.cpp

```cpp
// bank.cpp -- usando a interface de Fila
// compilar com queue.cpp
#include <iostream>
#include <cstdlib> // para rand() e srand()
#include <ctime>   // para time()
#include "queue.h"
const int MIN_POR_HR = 60;
bool novo_cliente(double x); // há um novo cliente?
int main()
{
    using std::cin;
    using std::cout;
    using std::endl;
    using std::ios_base;
// configurando as coisas
    std::srand(std::time(0)); // inicialização aleatória de rand()
    cout << "Estudo de Caso: Caixa Eletrônico do Banco de Heather\n";
    cout << "Informe o tamanho máximo da fila: ";
    int qs;
    cin >> qs;
    Fila fila(qs); // fila comporta até qs pessoas
    cout << "Informe o número de horas da simulação: ";
    int horas; // horas de simulação
    cin >> horas;
    // a simulação rodará 1 ciclo por minuto
    long limite_ciclo = MIN_POR_HR * horas; // número de ciclos
    cout << "Informe o número médio de clientes por hora: ";
    double por_hora; // número médio de chegadas por hora
    cin >> por_hora;
    double min_por_cliente;   // tempo médio entre chegadas
    min_por_cliente = MIN_POR_HR / por_hora;
    Item temp;              // dados do novo cliente
    long recusados = 0;     // recusados pela fila cheia
    long clientes = 0;      // entraram na fila
    long atendidos = 0;     // atendidos durante a simulação
    long soma_fila = 0;     // comprimento acumulado da fila
    int tempo_espera = 0;   // tempo até o caixa estar livre
    long espera_fila = 0;   // tempo acumulado na fila

// executando a simulação
    for (int ciclo = 0; ciclo < limite_ciclo; ciclo++)
    {
        if (novo_cliente(min_por_cliente)) // há recém-chegado
        {
            if (fila.esta_cheia())
                recusados++;
            else
            {
                clientes++;
                temp.definir(ciclo); // ciclo = horário de chegada
                fila.enfileirar(temp); // adiciona recém-chegado à fila
            }
        }
        if (tempo_espera <= 0 && !fila.esta_vazia())
        {
            fila.desenfileirar(temp); // atende o próximo cliente
            tempo_espera = temp.tempo_p(); // por tempo_espera minutos
            espera_fila += ciclo - temp.quando();
            atendidos++;
        }
        if (tempo_espera > 0)
            tempo_espera--;
        soma_fila += fila.contagem_fila();
    }
// relatando resultados
    if (clientes > 0)
    {
        cout << "clientes aceitos: " << clientes << endl;
        cout << "  clientes atendidos: " << atendidos << endl;
        cout << "    recusados: " << recusados << endl;
        cout << "tamanho médio da fila: ";
        cout.precision(2);
        cout.setf(ios_base::fixed, ios_base::floatfield);
        cout << (double) soma_fila / limite_ciclo << endl;
        cout << "  tempo médio de espera: "
           << (double) espera_fila / atendidos << " minutos\n";
    }
    else
        cout << "Nenhum cliente!\n";
    cout << "Feito!\n";
    return 0;
}

// x = tempo médio, em minutos, entre clientes
// valor de retorno é true se um cliente aparecer neste minuto
bool novo_cliente(double x)
{
    return (std::rand() * x / RAND_MAX < 1);
}
```

> **Nota**
> Você pode ter um compilador que não implementou `bool`. Nesse caso, você pode usar `int` em vez de `bool`, `0` em vez de `false` e `1` em vez de `true`. Você também pode ter que usar `stdlib.h` e `time.h` em vez dos mais novos `cstdlib` e `ctime`. Pode ser necessário definir `RAND_MAX` você mesmo.

Aqui estão algumas execuções de amostra do programa construído a partir das Listagens 12.10, 12.11 e 12.12:

```
Estudo de Caso: Caixa Eletrônico do Banco de Heather
Informe o tamanho máximo da fila: 10
Informe o número de horas da simulação: 100
Informe o número médio de clientes por hora: 15
clientes aceitos: 1485
  clientes atendidos: 1485
    recusados: 0
tamanho médio da fila: 0.15
  tempo médio de espera: 0.63 minutos
Feito!
```

```
Estudo de Caso: Caixa Eletrônico do Banco de Heather
Informe o tamanho máximo da fila: 10
Informe o número de horas da simulação: 100
Informe o número médio de clientes por hora: 30
clientes aceitos: 2896
  clientes atendidos: 2888
    recusados: 101
tamanho médio da fila: 4.64
  tempo médio de espera: 9.63 minutos
Feito!
```

```
Estudo de Caso: Caixa Eletrônico do Banco de Heather
Informe o tamanho máximo da fila: 20
Informe o número de horas da simulação: 100
Informe o número médio de clientes por hora: 30
clientes aceitos: 2943
  clientes atendidos: 2943
    recusados: 93
tamanho médio da fila: 13.06
  tempo médio de espera: 26.63 minutos
Feito!
```

Note que passar de 15 clientes por hora para 30 clientes por hora não dobra o tempo médio de espera; ele aumenta por um fator de cerca de 15. Permitir uma fila mais longa só piora as coisas. Porém, a simulação não permite pelo fato de que muitos clientes, frustrados com uma longa espera, simplesmente deixariam a fila.

Aqui estão mais algumas execuções de amostra do programa na Listagem 12.12; elas ilustram as variações de curto prazo que você pode ver, mesmo que o número médio de clientes por hora seja mantido constante:

```
Estudo de Caso: Caixa Eletrônico do Banco de Heather
Informe o tamanho máximo da fila: 10
Informe o número de horas da simulação: 4
Informe o número médio de clientes por hora: 30
clientes aceitos: 114
  clientes atendidos: 110
    recusados: 0
tamanho médio da fila: 2.15
  tempo médio de espera: 4.52 minutos
Feito!
```

```
Estudo de Caso: Caixa Eletrônico do Banco de Heather
Informe o tamanho máximo da fila: 10
Informe o número de horas da simulação: 4
Informe o número médio de clientes por hora: 30
clientes aceitos: 121
  clientes atendidos: 116
    recusados: 5
tamanho médio da fila: 5.28
  tempo médio de espera: 10.72 minutos
Feito!
```

```
Estudo de Caso: Caixa Eletrônico do Banco de Heather
Informe o tamanho máximo da fila: 10
Informe o número de horas da simulação: 4
Informe o número médio de clientes por hora: 30
clientes aceitos: 112
  clientes atendidos: 109
    recusados: 0
tamanho médio da fila: 2.41
  tempo médio de espera: 5.16 minutos
Feito!
```

---

## Resumo

Este capítulo cobre muitos aspectos importantes da definição e uso de classes. Vários desses aspectos são sutis — até difíceis — conceitos. Se alguns deles parecem obscuros ou complexos de maneira incomum para você, não se sinta mal; eles afetam a maioria dos iniciantes em C++ dessa forma. Muitas vezes a maneira como você realmente aprecia conceitos como construtores de cópia é ficando em apuros ao ignorá-los. Portanto, parte do material deste capítulo pode parecer vago para você até que suas próprias experiências enriqueçam seu entendimento.

Você pode usar `new` em um construtor de classe para alocar memória para dados e depois atribuir o endereço da memória a um membro de classe. Isso permite que uma classe, por exemplo, lide com strings de vários tamanhos sem comprometer o design de classe com antecedência a um tamanho de array fixo. Usar `new` em construtores de classe também levanta problemas potenciais quando um objeto expira. Se um objeto tem ponteiros membros apontando para memória alocada por `new`, liberar a memória usada para armazenar o objeto não libera automaticamente a memória apontada pelos ponteiros membros do objeto. Portanto, se você usar `new` em um construtor de classe para alocar memória, deve usar `delete` no destrutor de classe para liberar aquela memória. Dessa forma, o perecimento de um objeto automaticamente aciona a deleção da memória apontada.

Objetos que têm membros apontando para memória alocada por `new` também têm problemas com a inicialização de um objeto a partir de outro ou com a atribuição de um objeto a outro. Por padrão, C++ usa inicialização e atribuição membro a membro, o que significa que o objeto inicializado ou o objeto para o qual se atribui termina com cópias exatas dos membros do objeto original. Se um membro original aponta para um bloco de dados, o membro copiado aponta para o mesmo bloco. Quando o programa eventualmente deleta os dois objetos, o destrutor de classe tenta deletar o mesmo bloco de memória duas vezes, o que é um erro. A solução é definir um construtor de cópia especial que redefine a inicialização e sobrecarregar o operador de atribuição. Em cada caso, a nova definição deve criar duplicatas de quaisquer dados apontados e fazer com que o novo objeto aponte para as cópias. Dessa forma, tanto o objeto antigo quanto o novo se referem a dados separados mas idênticos, sem sobreposição. O mesmo raciocínio se aplica à definição de um operador de atribuição. Em cada caso, o objetivo é fazer uma cópia profunda — ou seja, copiar os dados reais e não apenas ponteiros para os dados.

Quando um objeto tem armazenamento automático ou externo, o destrutor daquele objeto é chamado automaticamente quando o objeto deixa de existir. Se você alocar armazenamento para um objeto usando `new` e atribuir seu endereço a um ponteiro, o destrutor daquele objeto é chamado automaticamente quando você aplica `delete` ao ponteiro. Porém, se você alocar armazenamento para objetos de classe usando `placement new` em vez de `new` regular, também assume a responsabilidade de chamar o destrutor daquele objeto explicitamente, invocando o método destrutor com um ponteiro para o objeto.

C++ permite colocar definições de estrutura, classe e enumeração dentro de uma classe. Tais tipos aninhados têm escopo de classe, o que significa que são locais à classe e não conflitam com estruturas, classes e enumerações de mesmo nome definidas em outro lugar.

C++ fornece uma sintaxe especial para construtores de classe que pode ser usada para inicializar membros de dados. Essa sintaxe consiste em dois pontos seguidos de uma lista separada por vírgulas de inicializadores. Isso é colocado após o parêntese fechado dos argumentos do construtor e antes do colchete de abertura do corpo da função. Cada inicializador consiste no nome do membro sendo inicializado seguido de parênteses contendo o valor de inicialização. Conceitualmente, essas inicializações ocorrem quando o objeto é criado e antes de qualquer instrução no corpo da função ser executada. A sintaxe fica assim:

```cpp
fila(int qs) : tam_fila(qs), itens(0), frente(NULL), fundo(NULL) { }
```

Essa forma é obrigatória se o membro de dados for um membro `const` não estático ou uma referência, exceto que a inicialização in-class do C++11 pode ser usada para membros `const` não estáticos.

C++11 permite a inicialização in-class (ou seja, inicialização na definição da classe):

```cpp
class Fila
{
private:
...
    No * frente = NULL;
    enum {TAM_F = 10};
    No * fundo = NULL;
    int itens = 0;
    const int tam_fila = TAM_F;
...
};
```

Isso é equivalente a usar uma lista de inicialização de membro. Porém, qualquer construtor que use uma lista de inicialização de membro substituirá as inicializações in-class correspondentes.

Como você deve ter notado, as classes exigem muito mais cuidado e atenção a detalhes do que as estruturas simples no estilo C. Em troca, elas fazem muito mais por você.

---

## Revisão do Capítulo

**1.** Suponha que uma classe `String` tenha os seguintes membros privados:

```cpp
class String
{
private:
    char * str; // aponta para a string alocada por new
    int len;    // armazena o comprimento da string
    //...
};
```

**a.** O que há de errado com este construtor padrão?

```cpp
String::String() {}
```

**b.** O que há de errado com este construtor?

```cpp
String::String(const char * s)
{
    str = s;
    len = strlen(s);
}
```

**c.** O que há de errado com este construtor?

```cpp
String::String(const char * s)
{
    strcpy(str, s);
    len = strlen(s);
}
```

**2.** Nomeie três problemas que podem surgir se você define uma classe na qual um membro ponteiro é inicializado usando `new`. Indique como eles podem ser remedeados.

**3.** Quais métodos de classe o compilador gera automaticamente se você não os fornece explicitamente? Descreva como essas funções implicitamente geradas se comportam.

**4.** Identifique e corrija os erros na seguinte declaração de classe:

```cpp
class nifty
{
// dados
    char personality[];
    int talents;
// métodos
    nifty();
    nifty(char * s);
    ostream & operator<<(ostream & os, nifty & n);
}
nifty:nifty()
{
    personality = NULL;
    talents = 0;
}
nifty:nifty(char * s)
{
    personality = new char [strlen(s)];
    personality = s;
    talents = 0;
}
ostream & nifty:operator<<(ostream & os, nifty & n)
{
    os << n;
}
```

**5.** Considere a seguinte declaração de classe:

```cpp
class Golfista
{
private:
    char * nomecomp;  // aponta para string contendo o nome do golfista
    int jogos;        // armazena o número de jogos de golfe jogados
    int * pontos;     // aponta para o primeiro elemento de um array de pontos de golfe
public:
    Golfista();
    Golfista(const char * nome, int g = 0);
    // cria array dinâmico vazio de g elementos se g > 0
    Golfista(const Golfista & g);
    ~Golfista();
};
```

**a.** Quais métodos de classe seriam invocados por cada uma das seguintes instruções?

```cpp
Golfista nancy;                        // #1
Golfista lulu("Little Lulu");          // #2
Golfista roy("Roy Hobbs", 12);         // #3
Golfista * par = new Golfista;         // #4
Golfista next = lulu;                  // #5
Golfista hazzard = "Weed Thwacker";   // #6
*par = nancy;                          // #7
nancy = "Nancy Putter";                // #8
```

**b.** Claramente, a classe requer vários outros métodos para ser útil. Qual método adicional ela requer para proteger contra corrupção de dados?

---

## Exercícios de Programação

**1.** Considere a seguinte declaração de classe:

```cpp
class Vaca {
    char nome[20];
    char * hobby;
    double peso;
public:
    Vaca();
    Vaca(const char * nm, const char * ho, double wt);
    Vaca(const Vaca & c);
    ~Vaca();
    Vaca & operator=(const Vaca & c);
    void MostrarVaca() const; // exibe todos os dados da vaca
};
```

Forneça a implementação para esta classe e escreva um programa curto que use todas as funções membro.

**2.** Melhore a declaração da classe `String` (ou seja, atualize `string1.h` para `string2.h`) fazendo o seguinte:

**a.** Sobrecarregue o operador `+` para permitir que você junte duas strings em uma.

**b.** Forneça uma função membro `minusculas()` que converta todos os caracteres alfabéticos em uma string para minúsculas. (Não esqueça da família de funções de caracteres `cctype`.)

**c.** Forneça uma função membro `maiusculas()` que converta todos os caracteres alfabéticos em uma string para maiúsculas.

**d.** Forneça uma função membro que receba um argumento `char` e retorne o número de vezes que o caractere aparece na string.

Teste seu trabalho no seguinte programa:

```cpp
// pe12_2.cpp
#include <iostream>
using namespace std;
#include "string2.h"
int main()
{
    String s1(" e eu sou um estudante de C++.");
    String s2 = "Por favor, informe seu nome: ";
    String s3;
    cout << s2;        // operador << sobrecarregado
    cin >> s3;         // operador >> sobrecarregado
    s2 = "Meu nome é " + s3; // operadores = e + sobrecarregados
    cout << s2 << ".\n";
    s2 = s2 + s1;
    s2.maiusculas(); // converte string para maiúsculas
    cout << "A string\n" << s2 << "\ncontém " << s2.has('A')
       << " caracteres 'A' nela.\n";
    s1 = "vermelho"; // String(const char *),
                     // depois String & operator=(const String&)
    String rgb[3] = { String(s1), String("verde"), String("azul")};
    cout << "Informe o nome de uma cor primária para mistura de luz: ";
    String resp;
    bool sucesso = false;
    while (cin >> resp)
    {
        resp.minusculas(); // converte string para minúsculas
        for (int i = 0; i < 3; i++)
        {
            if (resp == rgb[i]) // operador == sobrecarregado
            {
                cout << "Correto!\n";
                sucesso = true;
                break;
            }
        }
        if (sucesso)
            break;
        else
            cout << "Tente novamente!\n";
    }
    cout << "Até logo\n";
    return 0;
}
```

Sua saída deve ser semelhante a esta execução de amostra:

```
Por favor, informe seu nome: Fretta Farbo
Meu nome é Fretta Farbo.
A string
MEU NOME É FRETTA FARBO E EU SOU UM ESTUDANTE DE C++.
contém 6 caracteres 'A' nela.
Informe o nome de uma cor primária para mistura de luz: amarelo
Tente novamente!
AZUL
Correto!
Até logo
```

**3.** Reescreva a classe `Stock`, conforme descrita nas Listagens 10.7 e 10.8 do Capítulo 10, de modo que ela use memória alocada dinamicamente diretamente em vez de usar objetos de classe `string` para armazenar os nomes das ações. Além disso, substitua a função membro `show()` por uma definição de `operator<<()` sobrecarregado. Teste a nova definição com o programa da Listagem 10.9.

**4.** Considere a seguinte variação da classe `Stack` definida na Listagem 10.10:

```cpp
// stack.h -- declaração de classe para o ADT pilha
typedef unsigned long Item;
class Stack
{
private:
    enum {MAX = 10}; // constante específica à classe
    Item * pitens;   // armazena itens da pilha
    int tamanho;     // número de elementos na pilha
    int topo;        // índice para o item do topo da pilha
public:
    Stack(int n = MAX); // cria pilha com n elementos
    Stack(const Stack & st);
    ~Stack();
    bool esta_vazia() const;
    bool esta_cheia() const;
    // empilhar() retorna false se a pilha já está cheia, true caso contrário
    bool empilhar(const Item & item); // adiciona item à pilha
    // desempilhar() retorna false se a pilha já está vazia, true caso contrário
    bool desempilhar(Item & item); // desempilha o topo para item
    Stack & operator=(const Stack & st);
};
```

Como os membros privados sugerem, esta classe usa um array alocado dinamicamente para armazenar os itens da pilha. Reescreva os métodos para se adequar a esta nova representação e escreva um programa que demonstre todos os métodos, incluindo o construtor de cópia e o operador de atribuição.

**5.** O Banco de Heather realizou um estudo mostrando que os clientes do ATM não esperarão mais de um minuto na fila. Usando a simulação da Listagem 12.10, encontre um valor para o número de clientes por hora que leve a um tempo médio de espera de um minuto. (Use pelo menos um período de teste de 100 horas.)

**6.** O Banco de Heather gostaria de saber o que aconteceria se adicionasse um segundo ATM. Modifique a simulação deste capítulo para que ela tenha duas filas. Assuma que um cliente entrará na primeira fila se ela tiver menos pessoas do que a segunda fila e que o cliente entrará na segunda fila caso contrário. Novamente, encontre um valor para o número de clientes por hora que leve a um tempo médio de espera de um minuto. (Nota: este é um problema não linear no sentido de que dobrar o número de ATMs não dobra o número de clientes que podem ser atendidos por hora com espera máxima de um minuto.)

---

[Anterior](capitulo-12-02-retorno-ponteiros.md) | [Índice](README.md) | [Próximo](capitulo-13-01-heranca.md)
