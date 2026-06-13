# Capítulo 10 — Escopo de Classe, TAD, Resumo e Exercícios

> Tradução não oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Navegação: [Anterior](capitulo-10-02-construtores-destrutores-this.md) | [Índice](README.md) | [Próximo](capitulo-11-01-sobrecarga-operadores.md)

## Escopo de Classe (Class Scope)

O Capítulo 9 discutiu escopo global (ou de arquivo) e escopo local (ou de bloco). Lembre-se de que você pode usar uma variável com escopo global em qualquer lugar do arquivo que contém sua definição, enquanto uma variável com escopo local é local ao bloco que contém sua definição. Nomes de função também podem ter escopo global, mas nunca têm escopo local. As classes C++ introduzem um novo tipo de escopo: *escopo de classe* (class scope).

O escopo de classe se aplica a nomes definidos em uma classe, como os nomes de membros de dados de classe e funções membros de classe. Itens que têm escopo de classe são conhecidos dentro da classe, mas não fora dela. Assim, você pode usar os mesmos nomes de membros de classe em diferentes classes sem conflito. Por exemplo, o membro `num_acoes` da classe `Acao` é distinto do membro `num_acoes` de uma classe `PasseioDeJatinho`. Além disso, o escopo de classe significa que você não pode acessar diretamente membros de uma classe do mundo externo. Isso é verdade mesmo para membros de função públicas. Ou seja, para invocar uma função membro pública, você tem que usar um objeto:

```cpp
Acao dorminhoca("Minério Exclusivo", 100, 0.25);  // cria objeto
dorminhoca.exibir();  // usa objeto para invocar uma função membro
exibir();             // inválido -- não pode chamar o método diretamente
```

Da mesma forma, você tem que usar o operador de resolução de escopo ao definir funções membros:

```cpp
void Acao::atualizar(double preco)
{
    // ...
}
```

Em resumo, dentro de uma declaração de classe ou uma definição de função membro, você pode usar um nome de membro não qualificado (o nome não qualificado), como quando `vender()` chama a função membro `def_total()`. Um nome de construtor é reconhecido quando é chamado porque seu nome é o mesmo que o nome da classe. Caso contrário, você deve usar o operador de associação direta (`.`), o operador de associação indireta (`->`), ou o operador de resolução de escopo (`::`), dependendo do contexto, quando você usa um nome de membro de classe. O seguinte fragmento de código ilustra como identificadores com escopo de classe podem ser acessados:

```cpp
class Ik
{
private:
    int estado;  // estado tem escopo de classe
public:
    Ik(int f = 9) { estado = f; }  // estado está no escopo
    void ExibirIk() const;         // ExibirIk tem escopo de classe
};
void Ik::ExibirIk() const  // Ik:: coloca ExibirIk no escopo de Ik
{
    cout << estado << endl;  // estado está no escopo dentro de métodos da classe
}
// ...
int main()
{
    Ik * pik = new Ik;
    Ik ee = Ik(8);  // construtor no escopo porque tem o nome da classe
    ee.ExibirIk();  // objeto de classe traz ExibirIk para o escopo
    pik->ExibirIk();  // ponteiro-para-Ik traz ExibirIk para o escopo
    // ...
}
```

### Constantes de Escopo de Classe (Class Scope Constants)

Às vezes, seria bom ter constantes simbólicas com escopo de classe. Por exemplo, uma declaração de classe pode usar o literal `30` para especificar um tamanho de array. Como a constante é a mesma para todos os objetos, seria bom criar uma única constante compartilhada por todos os objetos. Você pode pensar que o seguinte seria uma solução:

```cpp
class Padaria
{
private:
    const int Meses = 12;  // declara uma constante? FALHA
    double custos[Meses];
    // ...
```

Mas isso não vai funcionar porque declarar uma classe descreve como um objeto parece, mas não cria um objeto. Portanto, até você criar um objeto, não há lugar para armazenar um valor. (Na verdade, C++11 fornece inicialização de membros, mas não de uma forma que faria a declaração de array anterior funcionar; o Capítulo 12 retorna a este tópico.) Há, entretanto, algumas maneiras de alcançar essencialmente o mesmo efeito desejado.

Primeiro, você pode declarar uma enumeração dentro de uma classe. Uma enumeração fornecida em uma declaração de classe tem escopo de classe, então você pode usar enumerações para fornecer nomes simbólicos com escopo de classe para constantes inteiras. Ou seja, você pode começar a declaração de `Padaria` desta forma:

```cpp
class Padaria
{
private:
    enum { Meses = 12 };
    double custos[Meses];
    // ...
```

Observe que declarar uma enumeração dessa forma não cria um membro de dados de classe. Ou seja, cada objeto individual não carrega uma enumeração nele. Em vez disso, `Meses` é apenas um nome simbólico que o compilador substitui por `12` quando o encontra em código no escopo da classe.

Como a classe `Padaria` usa a enumeração meramente para criar uma constante simbólica, sem intenção de criar variáveis do tipo de enumeração, você não precisa fornecer uma etiqueta de enumeração. Incidentalmente, para muitas implementações, a classe `ios_base` faz algo semelhante em sua seção pública; essa é a fonte de identificadores como `ios_base::fixed`. Aqui `fixed` é tipicamente um enumerador definido na classe `ios_base`.

O C++ tem uma segunda forma de definir uma constante dentro de uma classe — usando a palavra-chave `static`:

```cpp
class Padaria
{
private:
    static const int Meses = 12;
    double custos[Meses];
    // ...
```

Isso cria uma única constante chamada `Meses` que é armazenada com outras variáveis estáticas em vez de em um objeto. Assim, há apenas uma constante `Meses` compartilhada por todos os objetos `Padaria`. O Capítulo 12 examina mais membros de classe estáticos. Em C++98, você pode usar esta técnica apenas para declarar constantes estáticas com valores inteiros e de enumeração. Assim, C++98 não permite armazenar uma constante `double` desta forma. C++11 remove essa restrição.

### Enumerações com Escopo (Scoped Enumerations — C++11)

Enumerações tradicionais têm alguns problemas. Um é que enumeradores de duas definições de `enum` diferentes podem conflitar. Suponha que você estivesse trabalhando em um projeto envolvendo ovos e camisetas. Você pode tentar algo assim:

```cpp
enum ovo { Pequeno, Medio, Grande, Jumbo };
enum camiseta { Pequeno, Medio, Grande, ExtraGrande };
```

Isso não vai funcionar porque o `Pequeno` de `ovo` e o `Pequeno` de `camiseta` estariam ambos no mesmo escopo, e os nomes conflitariam. C++11 fornece uma nova forma de enumeração que evita esse problema ao ter *escopo de classe* para seus enumeradores. As declarações para essa forma são assim:

```cpp
enum class ovo { Pequeno, Medio, Grande, Jumbo };
enum class camiseta { Pequeno, Medio, Grande, ExtraGrande };
```

Alternativamente, você pode usar a palavra-chave `struct` em vez de `class`. Em ambos os casos, agora você precisa usar o nome do `enum` para qualificar o enumerador:

```cpp
ovo escolha = ovo::Grande;        // o enumerador Grande do enum ovo
camiseta Floyd = camiseta::Grande;  // o enumerador Grande do enum camiseta
```

Agora que os enumeradores têm escopo de classe, enumeradores de diferentes definições de `enum` não têm mais conflitos de nomes em potencial, e seu projeto de ovos-e-camisetas pode prosseguir.

C++11 também fortalece a segurança de tipo para enumerações com escopo. Enumerações regulares são convertidas para tipos inteiros automaticamente em algumas situações, como atribuição a uma variável `int` ou sendo usadas em uma expressão de comparação, mas enumerações com escopo não têm conversões implícitas para tipos inteiros:

```cpp
enum ovo_velho { Pequeno, Medio, Grande, Jumbo };  // sem escopo
enum class camiseta { Pequeno, Medio, Grande, ExtraGrande };  // com escopo
ovo_velho um = Medio;                    // sem escopo
camiseta rolf = camiseta::Grande;        // com escopo
int rei = um;                            // conversão de tipo implícita para sem escopo
int anel = rolf;                         // não permitido, sem conversão implícita
if (rei < Jumbo)                         // permitido
    std::cout << "Jumbo convertido para int antes da comparacao.\n";
if (rei < camiseta::Medio)              // não permitido
    std::cout << "Nao permitido: < nao definido para enum com escopo.\n";
```

Mas você pode fazer uma conversão de tipo explícita se sentir necessidade:

```cpp
int Frodo = int(camiseta::Pequeno);  // Frodo definido como 0
```

Enumerações são representadas por algum tipo inteiro subjacente, e no C++98 essa escolha dependia da implementação. Assim, uma estrutura contendo uma enumeração poderia ser de tamanhos diferentes em sistemas diferentes. C++11 remove essa dependência para enumerações com escopo. Por padrão, o tipo subjacente para enumerações com escopo C++11 é `int`. Além disso, há uma sintaxe para indicar uma escolha diferente:

```cpp
// tipo subjacente para pizza é short
enum class : short pizza { Pequeno, Medio, Grande, ExtraGrande };
```

O `: short` especifica que o tipo subjacente seja `short`. O tipo subjacente deve ser um tipo inteiro. No C++11, você também pode usar essa sintaxe para indicar o tipo subjacente para uma enumeração sem escopo, mas se você não escolher o tipo, a escolha que o compilador faz é dependente da implementação.

---

## Tipos de Dados Abstratos (Abstract Data Types)

A classe `Acao` é bastante específica. Muitas vezes, entretanto, programadores definem classes para representar conceitos mais gerais. Por exemplo, usar classes é uma boa maneira de implementar o que cientistas da computação descrevem como *tipos de dados abstratos* (ADTs, abstract data types). Como o nome sugere, um ADT descreve um tipo de dado de forma geral, sem trazer detalhes de linguagem ou implementação. Considere, por exemplo, a pilha (stack). Usando a pilha, você pode armazenar dados de modo que os dados sempre sejam adicionados ou deletados do topo da pilha. Por exemplo, programas C++ usam uma pilha para gerenciar variáveis automáticas. À medida que novas variáveis automáticas são geradas, elas são adicionadas ao topo da pilha. Quando expiram, são removidas da pilha.

Vamos examinar as propriedades de uma pilha de forma geral e abstrata. Primeiro, uma pilha mantém vários itens. (Essa propriedade a torna um contêiner, uma abstração ainda mais geral.) Em seguida, uma pilha é caracterizada pelas operações que você pode realizar nela:

- Você pode criar uma pilha vazia.
- Você pode adicionar um item ao topo de uma pilha (ou seja, você pode *empilhar* um item).
- Você pode remover um item do topo (ou seja, você pode *desempilhar* um item).
- Você pode verificar se a pilha está cheia.
- Você pode verificar se a pilha está vazia.

Você pode fazer corresponder esta descrição a uma declaração de classe em que as funções membros públicas fornecem uma interface que representa as operações de pilha. A seção de dados privados cuida do armazenamento dos dados da pilha. O conceito de classe é uma boa correspondência com a abordagem ADT.

A seção privada tem que se comprometer com como manter os dados. Por exemplo, você pode usar um array comum, um array alocado dinamicamente, ou alguma estrutura de dados mais avançada, como uma lista encadeada. No entanto, a interface pública deve ocultar a representação exata. Em vez disso, deve ser expressa em termos gerais, como criar uma pilha, empilhar um item, e assim por diante. A Listagem 10.10 mostra uma abordagem.

**Listagem 10.10** pilha.h

```cpp
// pilha.h -- definição de classe para o ADT pilha
#ifndef PILHA_H_
#define PILHA_H_

typedef unsigned long Item;
class Pilha
{
private:
    enum { MAX = 10 };    // constante específica da classe
    Item itens[MAX];      // mantém os itens da pilha
    int topo;             // índice do item no topo da pilha
public:
    Pilha();
    bool esta_vazia() const;
    bool esta_cheia() const;
    // empilhar() retorna false se a pilha já está cheia, true caso contrário
    bool empilhar(const Item & item);  // adiciona item à pilha
    // desempilhar() retorna false se a pilha já está vazia, true caso contrário
    bool desempilhar(Item & item);     // desempilha o topo para item
};
#endif
```

Na Listagem 10.10, a seção privada mostra que a pilha é implementada usando um array, mas a seção pública não revela esse fato. Assim, você pode substituir o array por, digamos, um array dinâmico sem alterar a interface da classe. Isso significa que mudar a implementação da pilha não requer que você recodifique programas que usam a pilha. Você apenas recompila o código da pilha e o vincula com o código do programa existente.

A interface é redundante no sentido de que `desempilhar()` e `empilhar()` retornam informações sobre o status da pilha (cheia ou vazia) em vez de ser do tipo `void`. Isso fornece ao programador algumas opções sobre como lidar com exceder o limite da pilha ou esvaziar a pilha. Ele ou ela pode usar `esta_vazia()` e `esta_cheia()` para verificar antes de tentar modificar a pilha, ou usar o valor de retorno de `empilhar()` e `desempilhar()` para determinar se a operação foi bem-sucedida.

Em vez de definir a pilha em termos de algum tipo específico, a classe a descreve em termos de um tipo geral `Item`. Neste caso, o arquivo de cabeçalho usa `typedef` para tornar `Item` o mesmo que `unsigned long`. Se você quiser, digamos, uma pilha de `double` ou de um tipo de estrutura, você pode alterar o `typedef` e deixar a declaração de classe e as definições de método sem alteração. *Templates* de classe (veja o Capítulo 14, "Reutilizando Código em C++") fornecem um método mais poderoso para isolar do design da classe o tipo de dados armazenados.

Em seguida, você precisa implementar os métodos da classe. A Listagem 10.11 mostra uma possibilidade.

**Listagem 10.11** pilha.cpp

```cpp
// pilha.cpp -- funções membros da Pilha
#include "pilha.h"
Pilha::Pilha()  // cria uma pilha vazia
{
    topo = 0;
}
bool Pilha::esta_vazia() const
{
    return topo == 0;
}
bool Pilha::esta_cheia() const
{
    return topo == MAX;
}
bool Pilha::empilhar(const Item & item)
{
    if (topo < MAX)
    {
        itens[topo++] = item;
        return true;
    }
    else
        return false;
}
bool Pilha::desempilhar(Item & item)
{
    if (topo > 0)
    {
        item = itens[--topo];
        return true;
    }
    else
        return false;
}
```

O construtor padrão garante que todas as pilhas sejam criadas vazias. O código para `desempilhar()` e `empilhar()` garante que o topo da pilha seja gerenciado corretamente. Garantias como essas são uma das coisas que tornam a OOP confiável. Suponha que, em vez disso, você criasse um array separado para representar a pilha e uma variável independente para representar o índice do topo. Nesse caso, é sua responsabilidade acertar o código cada vez que você criar uma nova pilha. Sem a proteção que os dados privados oferecem, sempre há a possibilidade de cometer algum erro de programa que altera dados involuntariamente.

Vamos testar esta pilha. A Listagem 10.12 modela a vida de um funcionário que processa ordens de compra do topo de sua caixa de entrada, usando a abordagem LIFO (último a entrar, primeiro a sair) de uma pilha.

**Listagem 10.12** empilhador.cpp

```cpp
// empilhador.cpp -- testando a classe Pilha
#include <iostream>
#include <cctype>  // ou ctype.h
#include "pilha.h"
int main()
{
    using namespace std;
    Pilha p;          // cria uma pilha vazia
    char ch;
    unsigned long po;
    cout << "Por favor, insira A para adicionar uma ordem de compra,\n"
         << "P para processar uma OC, ou Q para sair.\n";
    while (cin >> ch && toupper(ch) != 'Q')
    {
        while (cin.get() != '\n')
            continue;
        if (!isalpha(ch))
        {
            cout << '\a';
            continue;
        }
        switch(ch)
        {
            case 'A':
            case 'a': cout << "Insira um numero de OC para adicionar: ";
                      cin >> po;
                      if (p.esta_cheia())
                          cout << "pilha ja cheia\n";
                      else
                          p.empilhar(po);
                      break;
            case 'P':
            case 'p': if (p.esta_vazia())
                          cout << "pilha ja vazia\n";
                      else {
                          p.desempilhar(po);
                          cout << "OC #" << po << " desempilhada\n";
                      }
                      break;
        }
        cout << "Por favor, insira A para adicionar uma ordem de compra,\n"
             << "P para processar uma OC, ou Q para sair.\n";
    }
    cout << "Ate logo\n";
    return 0;
}
```

Aqui está uma execução de amostra:

```
Por favor, insira A para adicionar uma ordem de compra,
P para processar uma OC, ou Q para sair.
A
Insira um numero de OC para adicionar: 17885
Por favor, insira A para adicionar uma ordem de compra,
P para processar uma OC, ou Q para sair.
P
OC #17885 desempilhada
Por favor, insira A para adicionar uma ordem de compra,
P para processar uma OC, ou Q para sair.
A
Insira um numero de OC para adicionar: 17965
Por favor, insira A para adicionar uma ordem de compra,
P para processar uma OC, ou Q para sair.
A
Insira um numero de OC para adicionar: 18002
Por favor, insira A para adicionar uma ordem de compra,
P para processar uma OC, ou Q para sair.
P
OC #18002 desempilhada
Por favor, insira A para adicionar uma ordem de compra,
P para processar uma OC, ou Q para sair.
P
OC #17965 desempilhada
Por favor, insira A para adicionar uma ordem de compra,
P para processar uma OC, ou Q para sair.
P
pilha ja vazia
Por favor, insira A para adicionar uma ordem de compra,
P para processar uma OC, ou Q para sair.
Q
Ate logo
```

---

## Resumo (Summary)

A OOP enfatiza como um programa representa dados. O primeiro passo para resolver um problema de programação usando a abordagem OOP é descrever os dados em termos de sua interface com o programa, especificando como os dados são usados. Em seguida, você precisa projetar uma classe que implemente a interface. Tipicamente, membros de dados privados armazenam as informações, enquanto funções membros públicas, também chamadas de métodos, fornecem o único acesso aos dados. A classe combina dados e métodos em uma unidade, e o aspecto privado realiza a ocultação de dados.

Normalmente, você separa uma declaração de classe em duas partes, geralmente mantidas em arquivos separados. A declaração de classe propriamente dita vai em um arquivo de cabeçalho, com os métodos representados por protótipos de função. O código-fonte que define as funções membros vai em um arquivo de métodos. Essa abordagem separa a descrição da interface dos detalhes da implementação. Em princípio, você precisa saber apenas a interface pública da classe para usar a classe. É claro que você pode olhar para a implementação (a menos que tenha sido fornecida a você apenas em forma compilada), mas o seu programa não deve depender de detalhes da implementação, como saber que um valor específico é armazenado como `int`. Enquanto um programa e uma classe se comunicam apenas através de métodos que definem a interface, você está livre para melhorar qualquer parte separadamente sem se preocupar com interações imprevistas.

Uma classe é um tipo definido pelo usuário, e um objeto é uma instância de uma classe. Isso significa que um objeto é uma variável desse tipo ou o equivalente de uma variável, como memória alocada por `new` de acordo com a especificação da classe. O C++ tenta tornar os tipos definidos pelo usuário o mais semelhantes possível aos tipos padrão, então você pode declarar objetos, ponteiros para objetos e arrays de objetos. Você pode passar objetos como argumentos, devolvê-los como valores de retorno de função e atribuir um objeto a outro do mesmo tipo. Se você fornecer um método construtor, você pode inicializar objetos quando eles são criados. Se você fornecer um método destrutor, o programa executa esse método quando o objeto expira.

Cada objeto contém suas próprias cópias da parte de dados de uma declaração de classe, mas elas compartilham os métodos da classe. Se `mr_objeto` é o nome de um objeto particular e `tente_me()` é uma função membro, você invoca a função membro usando o operador de associação ponto: `mr_objeto.tente_me()`. A terminologia OOP descreve essa chamada de função como enviar uma mensagem `tente_me` para o objeto `mr_objeto`. Qualquer referência a membros de dados de classe no método `tente_me()` então se aplica aos membros de dados do objeto `mr_objeto`.

Se você quiser que uma função membro atue em mais de um objeto, você pode passar objetos adicionais para o método como argumentos. Se um método precisar se referir explicitamente ao objeto que o evocou, ele pode usar o ponteiro `this`. O ponteiro `this` é definido como o endereço do objeto que evoca, então `*this` é um alias para o objeto em si.

Classes são bem adequadas para descrever ADTs. A interface de função membro pública fornece os serviços descritos por um ADT, e a seção privada da classe e o código para os métodos da classe fornecem uma implementação oculta dos clientes da classe.

---

## Revisão do Capítulo (Chapter Review)

**1.** O que é uma classe?

**2.** Como uma classe realiza abstração, encapsulamento e ocultação de dados?

**3.** Qual é a relação entre um objeto e uma classe?

**4.** De que forma, além de serem funções, os membros de função de classe são diferentes dos membros de dados de classe?

**5.** Defina uma classe para representar uma conta bancária. Os membros de dados devem incluir o nome do depositante, o número da conta (use uma string) e o saldo. As funções membros devem permitir o seguinte:

   - Criar um objeto e inicializá-lo.
   - Exibir o nome do depositante, o número da conta e o saldo.
   - Depositar uma quantia de dinheiro dada por um argumento.
   - Sacar uma quantia de dinheiro dada por um argumento.

   Mostre apenas a declaração da classe, não as implementações dos métodos. (O Exercício de Programação 1 fornece uma oportunidade de escrever a implementação.)

**6.** Quando os construtores de classe são chamados? Quando os destrutores de classe são chamados?

**7.** Forneça código para um construtor para a classe de conta bancária da Pergunta de Revisão 5.

**8.** O que é um construtor padrão? Qual é a vantagem de ter um?

**9.** Modifique a definição da classe `Acao` (a versão em `acao20.h`) para que ela tenha funções membros que retornem os valores dos membros de dados individuais. Nota: Um membro que retorna o nome da empresa não deve fornecer uma arma para alterar o array. Ou seja, não pode simplesmente retornar uma referência a `string`. Poderia retornar uma referência `const`.

**10.** O que são `this` e `*this`?

---

## Exercícios de Programação (Programming Exercises)

**1.** Forneça definições de métodos para a classe descrita na Pergunta de Revisão 5 e escreva um programa curto que ilustre todos os recursos.

**2.** Aqui está uma definição de classe bastante simples:

```cpp
class Pessoa {
private:
    static const LIMITE = 25;
    string sobrenome;    // sobrenome da Pessoa
    char nome[LIMITE];   // nome da Pessoa
public:
    Pessoa() { sobrenome = ""; nome[0] = '\0'; }  // #1
    Pessoa(const string & ln, const char * fn = "Ei voce");  // #2
    // os seguintes métodos exibem sobrenome e nome
    void Exibir() const;         // formato nome sobrenome
    void ExibicaoFormal() const; // formato sobrenome, nome
};
```

(Ela usa um objeto `string` e um array de caracteres para que você possa comparar como as duas formas são usadas.) Escreva um programa que complete a implementação fornecendo código para os métodos indefinidos. O programa no qual você usa a classe também deve usar as três possíveis chamadas ao construtor (sem argumentos, um argumento e dois argumentos) e os dois métodos de exibição. Aqui está um exemplo que usa os construtores e métodos:

```cpp
Pessoa um;                              // usa o construtor padrão
Pessoa dois("Artesanato");              // usa #2 com um argumento padrão
Pessoa tres("Tosco", "Sam");            // usa #2, sem padrões
um.Exibir();
cout << endl;
um.ExibicaoFormal();
// etc. para dois e tres
```

**3.** Faça o Exercício de Programação 1 do Capítulo 9, mas substitua o código mostrado lá com uma declaração de classe `golfe` apropriada. Substitua `definir_golfe(golfe &, const char*, int)` com um construtor com o argumento apropriado para fornecer valores iniciais. Mantenha a versão interativa de `definir_golfe()`, mas implemente-a usando o construtor. (Por exemplo, para o código de `definir_golfe()`, obtenha os dados, passe os dados ao construtor para criar um objeto temporário e atribua o objeto temporário ao objeto que invoca, que é `*this`.)

**4.** Faça o Exercício de Programação 4 do Capítulo 9, mas converta a estrutura `Sales` e suas funções associadas para uma classe e seus métodos. Substitua a função `setSales(Sales &, double [], int)` por um construtor. Implemente o método interativo `setSales(Sales &)` usando o construtor. Mantenha a classe dentro do namespace `SALES`.

**5.** Considere a seguinte declaração de estrutura:

```cpp
struct cliente {
    char nome_completo[35];
    double pagamento;
};
```

Escreva um programa que adiciona e remove estruturas de clientes de uma pilha, representada por uma declaração de classe `Pilha`. Cada vez que um cliente é removido, seu pagamento deve ser adicionado a um total em execução, e o total em execução deve ser relatado. Nota: Você deve poder usar a classe `Pilha` sem alterações; apenas altere a declaração `typedef` para que `Item` seja do tipo `cliente` em vez de `unsigned long`.

**6.** Aqui está uma declaração de classe:

```cpp
class Movimento
{
private:
    double x;
    double y;
public:
    Movimento(double a = 0, double b = 0);  // define x, y como a, b
    void exibir_movimento() const;          // exibe os valores atuais de x, y
    Movimento somar(const Movimento & m) const;
    // esta função adiciona x de m ao x do objeto que invoca para obter novo x,
    // adiciona y de m ao y do objeto que invoca para obter novo y,
    // cria um novo objeto Movimento inicializado com os novos valores x, y e o retorna
    void redefinir(double a = 0, double b = 0);  // redefine x, y como a, b
};
```

Crie definições de funções membros e um programa que exercite a classe.

**7.** Um *plorg* de Betelgeuse tem estas propriedades:

**Dados**
- Um plorg tem um nome com no máximo 19 letras.
- Um plorg tem um índice de contentamento (IC), que é um inteiro.

**Operações**
- Um novo plorg começa com um nome e um IC de 50.
- O IC de um plorg pode mudar.
- Um plorg pode relatar seu nome e IC.
- O plorg padrão tem o nome "Plorga".

Escreva uma declaração de classe `Plorg` (incluindo membros de dados e protótipos de funções membros) que representa um plorg. Escreva as definições de funções para as funções membros. Escreva um programa curto que demonstre todos os recursos da classe `Plorg`.

**8.** Você pode descrever uma lista simples assim:

- A lista simples pode conter zero ou mais itens de algum tipo específico.
- Você pode criar uma lista vazia.
- Você pode adicionar itens à lista.
- Você pode determinar se a lista está vazia.
- Você pode determinar se a lista está cheia.
- Você pode visitar cada item na lista e realizar alguma ação nele.

Como você pode ver, esta lista realmente é simples; ela não permite inserção ou exclusão, por exemplo.

Projete uma classe `Lista` para representar este tipo abstrato. Você deve fornecer um arquivo de cabeçalho `lista.h` com a declaração de classe e um arquivo `lista.cpp` com as implementações dos métodos de classe. Você também deve criar um programa curto que utilize seu design. A principal razão para manter a especificação da lista simples é simplificar este exercício de programação. Você pode implementar a lista como um array ou, se estiver familiarizado com o tipo de dados, como uma lista encadeada. Mas a interface pública não deve depender de sua escolha. Ou seja, a interface pública não deve ter índices de array, ponteiros para nós, e assim por diante. Ela deve ser expressa nos conceitos gerais de criar uma lista, adicionar um item à lista, e assim por diante. A maneira usual de lidar com a visita a cada item e a realização de uma ação é usar uma função que aceita um ponteiro de função como argumento:

```cpp
void visitar(void (*pf)(Item &));
```

Aqui `pf` aponta para uma função (não uma função membro) que aceita um argumento de referência para `Item`, onde `Item` é o tipo dos itens na lista. A função `visitar()` aplica esta função a cada item na lista. Você pode usar a classe `Pilha` como guia geral.

---

Navegação: [Anterior](capitulo-10-02-construtores-destrutores-this.md) | [Índice](README.md) | [Próximo](capitulo-11-01-sobrecarga-operadores.md)
