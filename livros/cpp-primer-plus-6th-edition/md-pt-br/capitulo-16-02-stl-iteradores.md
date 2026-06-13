# Capítulo 16 — A Standard Template Library: Vector e Iteradores

> Tradução não oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> [Anterior](capitulo-16-01-string-ponteiros-inteligentes.md) | [Índice](README.md) | [Próximo](capitulo-16-03-containers-algoritmos.md)

## A Standard Template Library

A STL fornece uma coleção de templates representando containers, iteradores, objetos função e algoritmos. Um container é uma unidade, como um array, que pode conter vários valores. Os containers da STL são homogêneos; ou seja, eles contêm valores todos do mesmo tipo. Algoritmos são receitas para realizar tarefas específicas, como ordenar um array ou encontrar um valor específico em uma lista. Iteradores são objetos que permitem percorrer um container assim como ponteiros permitem percorrer um array; eles são generalizações de ponteiros. Objetos função são objetos que agem como funções; eles podem ser objetos de classe ou ponteiros de função (incluindo nomes de função, pois um nome de função age como um ponteiro). A STL permite construir uma variedade de containers, incluindo arrays, filas e listas, e permite realizar uma variedade de operações, incluindo busca, ordenação e aleatorização.

Alex Stepanov e Meng Lee desenvolveram a STL nos Laboratórios Hewlett-Packard, lançando a implementação em 1994. O comitê ISO/ANSI C++ votou para incorporá-la como parte do Padrão C++. A STL não é um exemplo de programação orientada a objetos. Em vez disso, ela representa um paradigma de programação diferente chamado *programação genérica* (generic programming). Isso torna a STL interessante tanto em termos do que ela faz quanto em termos de sua abordagem.

Há muita informação sobre a STL para apresentar em um único capítulo, portanto examinaremos alguns exemplos representativos e exploraremos o espírito da abordagem de programação genérica. Começaremos examinando alguns exemplos específicos. Então, quando você tiver uma apreciação prática de containers, iteradores e algoritmos, examinaremos a filosofia de projeto subjacente e depois faremos uma visão geral da STL inteira. O Apêndice G, "Os Métodos e Funções da STL", resume os vários métodos e funções da STL.

### O Template de Classe `vector`

O Capítulo 4 tocou brevemente na classe `vector`. Vamos examiná-la mais de perto agora. Na computação, o termo *vetor* corresponde a um array em vez de ao vetor matemático discutido no Capítulo 11. (Matematicamente, um vetor matemático de N dimensões pode ser representado por um conjunto de N componentes, portanto nesse aspecto um vetor matemático é como um array de N dimensões. Porém, um vetor matemático tem propriedades adicionais, como produtos internos e externos, que um vetor computacional não tem necessariamente.) Um vetor no estilo computacional contém um conjunto de valores iguais que podem ser acessados aleatoriamente. Ou seja, você pode usar, digamos, um índice para acessar diretamente o 10º elemento de um vetor sem ter que acessar os 9 elementos anteriores primeiro. Portanto, uma classe `vector` forneceria operações semelhantes às das classes `valarray` e `ArrayTP` introduzidas no Capítulo 14 e às da classe `array` introduzida no Capítulo 4. Ou seja, você poderia criar um objeto `vector`, atribuir um objeto `vector` a outro e usar o operador `[]` para acessar os elementos de `vector`. Para tornar a classe genérica, você a torna uma classe template. É o que a STL faz, definindo um template `vector` no arquivo de cabeçalho `vector`.

Para criar um objeto template `vector`, você usa a notação usual `<tipo>` para indicar o tipo a ser usado. Além disso, o template `vector` usa alocação de memória dinâmica, e você pode usar um argumento de inicialização para indicar quantos elementos de vetor deseja:

```cpp
#include <vector>
using namespace std;
vector<int> avaliacoes(5); // um vetor de 5 ints
int n;
cin >> n;
vector<double> notas(n); // um vetor de n doubles
```

Após criar um objeto `vector`, a sobrecarga de operadores para `[]` torna possível usar a notação usual de array para acessar os elementos individuais:

```cpp
avaliacoes[0] = 9;
for (int i = 0; i < n; i++)
    cout << notas[i] << endl;
```

> **Alocadores Novamente:** Assim como a classe `string`, os vários templates de container da STL recebem um argumento de template opcional que especifica qual objeto alocador usar para gerenciar a memória. Por exemplo, o template `vector` começa assim:
> ```cpp
> template <class T, class Allocator = allocator<T> >
>   class vector {...
> ```
> Se você omitir um valor para esse argumento de template, o template do container usa a classe `allocator<T>` por padrão. Essa classe usa `new` e `delete`.

A Listagem 16.7 usa essa classe em uma aplicação sem exigências. Esse programa específico cria dois objetos `vector`, um com especialização `int` e outro com especialização `string`; cada um tem cinco elementos.

**Listagem 16.7 — vet1.cpp**

```cpp
// vet1.cpp -- introduzindo o template vector
#include <iostream>
#include <string>
#include <vector>
const int NUM = 5;
int main()
{
    using std::vector;
    using std::string;
    using std::cin;
    using std::cout;
    using std::endl;
    vector<int> avaliacoes(NUM);
    vector<string> titulos(NUM);
    cout << "Você fará exatamente o que mandei. Você vai inserir\n"
         << NUM << " títulos de livros e suas avaliações (0-10).\n";
    int i;
    for (i = 0; i < NUM; i++)
    {
        cout << "Digite o título #" << i + 1 << ": ";
        getline(cin, titulos[i]);
        cout << "Digite sua avaliação (0-10): ";
        cin >> avaliacoes[i];
        cin.get();
    }
    cout << "Obrigado. Você inseriu o seguinte:\n"
         << "Avaliação\tLivro\n";
    for (i = 0; i < NUM; i++)
    {
        cout << avaliacoes[i] << "\t" << titulos[i] << endl;
    }
    return 0;
}
```

Aqui está uma execução de exemplo do programa da Listagem 16.7:

```
You will do exactly as told. You will enter
5 book titles and your ratings (0-10).
Enter title #1: The Cat Who Knew C++
Enter your rating (0-10): 6
Enter title #2: Felonious Felines
Enter your rating (0-10): 4
Enter title #3: Warlords of Wonk
Enter your rating (0-10): 3
Enter title #4: Don't Touch That Metaphor
Enter your rating (0-10): 5
Enter title #5: Panic Oriented Programming
Enter your rating (0-10): 8
Thank you. You entered the following:
Rating  Book
6       The Cat Who Knew C++
4       Felonious Felines
3       Warlords of Wonk
5       Don't Touch That Metaphor
8       Panic Oriented Programming
```

### O que Fazer com Vetores

Além de alocar armazenamento, o que mais o template `vector` pode fazer? Todas as classes de container da STL fornecem certos métodos básicos, incluindo `size()`, que retorna o número de elementos em um container; `swap()`, que troca o conteúdo de dois containers; `begin()`, que retorna um iterador que se refere ao primeiro elemento em um container; e `end()`, que retorna um iterador que representa a posição *past-the-end* (além do fim) para o container.

O que é um iterador? É uma *generalização de um ponteiro*. Na verdade, pode ser um ponteiro. Ou pode ser um objeto para o qual operações semelhantes a ponteiros, como desreferenciamento (por exemplo, `operator*()`) e incremento (por exemplo, `operator++()`), foram definidas. Como você verá adiante, generalizar ponteiros para iteradores permite que a STL forneça uma interface uniforme para uma variedade de classes de container, incluindo aquelas para as quais ponteiros simples não funcionariam. Cada classe de container define um iterador adequado. O nome do tipo para esse iterador é um `typedef` de escopo de classe chamado `iterator`. Por exemplo, para declarar um iterador para uma especialização `double` de `vector`, você usaria:

```cpp
vector<double>::iterator pd; // pd é um iterador
```

Suponha que `notas` seja um objeto `vector<double>`:

```cpp
vector<double> notas;
```

Então você pode usar o iterador `pd` em código como:

```cpp
pd = notas.begin(); // fazer pd apontar para o primeiro elemento
*pd = 22.3;         // desreferenciar pd e atribuir valor ao primeiro elemento
++pd;               // fazer pd apontar para o próximo elemento
```

Como você pode ver, um iterador se comporta como um ponteiro. A propósito, aqui está outro lugar onde a dedução automática de tipo do C++11 pode ser útil. Em vez de:

```cpp
vector<double>::iterator pd = notas.begin();
```

você pode usar:

```cpp
auto pd = notas.begin(); // dedução automática de tipo do C++11
```

Voltando ao exemplo, o que é *past-the-end*? É um iterador que se refere a um elemento uma posição além do último elemento em um container. A ideia é semelhante à ideia do caractere nulo ser um elemento além do último caractere real em uma string C-style, exceto que o caractere nulo é o valor no elemento, e past-the-end é um ponteiro (ou iterador) para o elemento. O método `end()` identifica a posição past-the-end. Se você definir um iterador para o primeiro elemento em um container e continuar incrementando-o, ele eventualmente alcançará past-the-end, e você terá percorrido todo o conteúdo do container. Assim, se `notas` e `pd` forem definidos como no exemplo anterior, você pode exibir o conteúdo com este código:

```cpp
for (pd = notas.begin(); pd != notas.end(); pd++)
    cout << *pd << endl;
```

Todos os containers têm os métodos recém-discutidos. O template de classe `vector` também tem alguns métodos que apenas alguns containers da STL têm. Um método conveniente, chamado `push_back()`, adiciona um elemento ao final de um vetor. Ao fazer isso, ele cuida do gerenciamento de memória para que o tamanho do `vector` aumente para acomodar os membros adicionados. Isso significa que você pode escrever código como:

```cpp
vector<double> notas;  // criar um vetor vazio
double temp;
while (cin >> temp && temp >= 0)
    notas.push_back(temp);
cout << "Você inseriu " << notas.size() << " notas.\n";
```

Cada ciclo do laço acrescenta um elemento a mais ao objeto `notas`. Você não precisa escolher o número de elementos quando escreve o programa ou quando o executa. Enquanto o programa tiver acesso a memória suficiente, ele expandirá o tamanho de `notas` conforme necessário.

O método `erase()` remove um determinado intervalo de um vetor. Ele recebe dois argumentos iteradores que definem o intervalo a ser removido. É importante que você entenda como a STL define intervalos usando dois iteradores. O primeiro iterador refere-se ao início do intervalo, e o segundo iterador está uma posição além do fim do intervalo. Por exemplo, o seguinte apaga o primeiro e o segundo elementos — ou seja, aqueles referenciados por `begin()` e `begin() + 1`:

```cpp
notas.erase(notas.begin(), notas.begin() + 2);
```

(Como `vector` fornece acesso aleatório, operações como `begin() + 2` são definidas para os iteradores da classe `vector`.) Se `it1` e `it2` são dois iteradores, a literatura da STL usa a notação `[p1, p2)` para indicar um intervalo que começa com `p1` e vai até, mas não incluindo, `p2`. Assim, o intervalo `[begin(), end())` abrange todo o conteúdo de uma coleção. Além disso, o intervalo `[p1, p1)` é vazio. (A notação `[)` não faz parte do C++, portanto não aparece no código; ela apenas aparece na documentação.)

> **Nota:** Um intervalo `[it1, it2)` é especificado por dois iteradores `it1` e `it2`, e vai de `it1` até, mas não incluindo, `it2`.

Um método `insert()` complementa `erase()`. Ele recebe três argumentos iteradores. O primeiro fornece a posição antes da qual os novos elementos serão inseridos. O segundo e terceiro parâmetros iteradores definem o intervalo a ser inserido. Esse intervalo normalmente faz parte de outro objeto container. Por exemplo, o seguinte código insere todos os elementos, exceto o primeiro, do vetor `novo_v` antes do primeiro elemento do vetor `antigo_v`:

```cpp
vector<int> antigo_v;
vector<int> novo_v;
...
antigo_v.insert(antigo_v.begin(), novo_v.begin() + 1, novo_v.end());
```

A Listagem 16.8 ilustra o uso de `size()`, `begin()`, `end()`, `push_back()`, `erase()` e `insert()`. Para simplificar o tratamento de dados, os componentes de avaliação e título da Listagem 16.7 são incorporados em uma única estrutura `Resenha`, e as funções `PreencherResenha()` e `MostrarResenha()` fornecem facilidades de entrada e saída para objetos `Resenha`.

**Listagem 16.8 — vet2.cpp**

```cpp
// vet2.cpp -- métodos e iteradores
#include <iostream>
#include <string>
#include <vector>

struct Resenha {
    std::string titulo;
    int avaliacao;
};
bool PreencherResenha(Resenha & rr);
void MostrarResenha(const Resenha & rr);
int main()
{
    using std::cout;
    using std::vector;
    vector<Resenha> livros;
    Resenha temp;
    while (PreencherResenha(temp))
        livros.push_back(temp);
    int num = livros.size();
    if (num > 0)
    {
        cout << "Obrigado. Você inseriu o seguinte:\n"
             << "Avaliação\tLivro\n";
        for (int i = 0; i < num; i++)
            MostrarResenha(livros[i]);
        cout << "Repetindo:\n"
             << "Avaliação\tLivro\n";
        vector<Resenha>::iterator pr;
        for (pr = livros.begin(); pr != livros.end(); pr++)
            MostrarResenha(*pr);
        vector<Resenha> listaAntiga(livros); // construtor de cópia usado
        if (num > 3)
        {
            // remover 2 itens
            livros.erase(livros.begin() + 1, livros.begin() + 3);
            cout << "Após apagamento:\n";
            for (pr = livros.begin(); pr != livros.end(); pr++)
                MostrarResenha(*pr);
            // inserir 1 item
            livros.insert(livros.begin(), listaAntiga.begin() + 1,
                    listaAntiga.begin() + 2);
            cout << "Após inserção:\n";
            for (pr = livros.begin(); pr != livros.end(); pr++)
                MostrarResenha(*pr);
        }
        livros.swap(listaAntiga);
        cout << "Trocando listaAntiga com livros:\n";
        for (pr = livros.begin(); pr != livros.end(); pr++)
            MostrarResenha(*pr);
    }
    else
        cout << "Nada inserido, nada ganho.\n";
    return 0;
}
bool PreencherResenha(Resenha & rr)
{
    std::cout << "Digite o título do livro (quit para sair): ";
    std::getline(std::cin, rr.titulo);
    if (rr.titulo == "quit")
        return false;
    std::cout << "Digite sua avaliação: ";
    std::cin >> rr.avaliacao;
    if (!std::cin)
        return false;
    // descartar o restante da linha de entrada
    while (std::cin.get() != '\n')
        continue;
    return true;
}
void MostrarResenha(const Resenha & rr)
{
    std::cout << rr.avaliacao << "\t" << rr.titulo << std::endl;
}
```

### Mais Coisas para Fazer com Vetores

Existem muitas coisas que os programadores comumente fazem com arrays, como pesquisá-los, ordená-los, aleatorizar a ordem e assim por diante. O template de classe `vector` tem métodos para essas operações comuns? Não! A STL adota uma visão mais ampla, definindo *funções não-membro* para essas operações. Assim, em vez de definir uma função-membro `find()` separada para cada classe de container, ela define uma única função `find()` não-membro que pode ser usada para todas as classes de container. Essa filosofia de projeto economiza muito trabalho duplicado.

Por outro lado, a STL às vezes define uma função-membro mesmo que também defina uma função não-membro para a mesma tarefa. A razão é que, para algumas ações, há um algoritmo específico da classe que é mais eficiente do que o algoritmo mais geral. Portanto, o `swap()` do `vector` será mais eficiente do que o `swap()` não-membro. Por outro lado, a versão não-membro permitirá trocar conteúdos entre dois tipos diferentes de containers.

Vamos examinar três funções representativas da STL: `for_each()`, `random_shuffle()` e `sort()`. A função `for_each()` pode ser usada com qualquer classe de container. Ela recebe três argumentos. Os dois primeiros são iteradores que definem um intervalo no container, e o último é um ponteiro para uma função (mais geralmente, um objeto função — você aprenderá sobre objetos função em breve). A função `for_each()` então aplica a função apontada a cada elemento do container no intervalo. A função apontada não deve alterar o valor dos elementos do container. Você pode usar a função `for_each()` em vez de um laço `for`. Por exemplo, você pode substituir o código:

```cpp
vector<Resenha>::iterator pr;
for (pr = livros.begin(); pr != livros.end(); pr++)
    MostrarResenha(*pr);
```

pelo seguinte:

```cpp
for_each(livros.begin(), livros.end(), MostrarResenha);
```

Isso evita que você suje suas mãos (e o código) com uso explícito de variáveis iteradoras.

A função `random_shuffle()` recebe dois iteradores que especificam um intervalo e reordena os elementos nesse intervalo de forma aleatória. Por exemplo, a seguinte instrução reordena aleatoriamente a ordem de todos os elementos no vetor `livros`:

```cpp
random_shuffle(livros.begin(), livros.end());
```

Diferentemente de `for_each`, que funciona com qualquer classe de container, esta função requer que a classe de container permita acesso aleatório, o que a classe `vector` permite.

A função `sort()` também requer que o container suporte acesso aleatório. Ela vem em duas versões. A primeira versão recebe dois iteradores que definem um intervalo, e ordena esse intervalo usando o operador `<` definido para o tipo elemento armazenado no container. Por exemplo, o seguinte ordena o conteúdo de `coisasLegais` em ordem crescente, usando o operador `<` embutido para comparar valores:

```cpp
vector<int> coisasLegais;
...
sort(coisasLegais.begin(), coisasLegais.end());
```

Se os elementos do container forem objetos definidos pelo usuário, deve haver uma função `operator<()` definida que funcione com esse tipo de objeto para poder usar `sort()`. Por exemplo, você poderia ordenar um vetor contendo objetos `Resenha` se fornecer uma função-membro ou uma função não-membro de `Resenha` para `operator<()`. Como `Resenha` é uma estrutura, seus membros são públicos, e uma função não-membro como esta serviria:

```cpp
bool operator<(const Resenha & r1, const Resenha & r2)
{
    if (r1.titulo < r2.titulo)
        return true;
    else if (r1.titulo == r2.titulo && r1.avaliacao < r2.avaliacao)
        return true;
    else
        return false;
}
```

Com uma função como essa instalada, você poderia então ordenar um vetor de objetos `Resenha` (como `livros`):

```cpp
sort(livros.begin(), livros.end());
```

Essa versão da função `operator<()` ordena em ordem lexicográfica dos membros `titulo`. Se dois objetos têm os mesmos membros `titulo`, eles são então ordenados pela ordem de `avaliacao`. Mas suponha que você queira ordenar em ordem decrescente ou por `avaliacao` em vez de `titulo`. Nesse caso, você pode usar a segunda forma de `sort()`. Ela recebe três argumentos. Os dois primeiros são iteradores que indicam o intervalo. O argumento final é um ponteiro para uma função (mais geralmente, um objeto função) a ser usado em vez de `operator<()` para fazer a comparação. O valor de retorno deve ser conversível para `bool`, com `false` significando que os dois argumentos estão na ordem errada. Aqui está um exemplo de tal função:

```cpp
bool piorQue(const Resenha & r1, const Resenha & r2)
{
    if (r1.avaliacao < r2.avaliacao)
        return true;
    else
        return false;
}
```

Com essa função instalada, você pode usar a seguinte instrução para ordenar o vetor `livros` de objetos `Resenha` em ordem crescente de valores de avaliação:

```cpp
sort(livros.begin(), livros.end(), piorQue);
```

A Listagem 16.9 ilustra o uso dessas funções da STL.

**Listagem 16.9 — vet3.cpp**

```cpp
// vet3.cpp -- usando funções da STL
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
struct Resenha {
    std::string titulo;
    int avaliacao;
};
bool operator<(const Resenha & r1, const Resenha & r2);
bool piorQue(const Resenha & r1, const Resenha & r2);
bool PreencherResenha(Resenha & rr);
void MostrarResenha(const Resenha & rr);
int main()
{
    using namespace std;
    vector<Resenha> livros;
    Resenha temp;
    while (PreencherResenha(temp))
        livros.push_back(temp);
    if (livros.size() > 0)
    {
        cout << "Obrigado. Você inseriu as seguintes "
             << livros.size() << " avaliações:\n"
             << "Avaliação\tLivro\n";
        for_each(livros.begin(), livros.end(), MostrarResenha);
        sort(livros.begin(), livros.end());
        cout << "Ordenado por título:\nAvaliação\tLivro\n";
        for_each(livros.begin(), livros.end(), MostrarResenha);
        sort(livros.begin(), livros.end(), piorQue);
        cout << "Ordenado por avaliação:\nAvaliação\tLivro\n";
        for_each(livros.begin(), livros.end(), MostrarResenha);
        random_shuffle(livros.begin(), livros.end());
        cout << "Após embaralhamento:\nAvaliação\tLivro\n";
        for_each(livros.begin(), livros.end(), MostrarResenha);
    }
    else
        cout << "Nenhuma entrada. ";
    cout << "Até logo.\n";
    return 0;
}
bool operator<(const Resenha & r1, const Resenha & r2)
{
    if (r1.titulo < r2.titulo)
        return true;
    else if (r1.titulo == r2.titulo && r1.avaliacao < r2.avaliacao)
        return true;
    else
        return false;
}
bool piorQue(const Resenha & r1, const Resenha & r2)
{
    if (r1.avaliacao < r2.avaliacao)
        return true;
    else
        return false;
}
bool PreencherResenha(Resenha & rr)
{
    std::cout << "Digite o título do livro (quit para sair): ";
    std::getline(std::cin, rr.titulo);
    if (rr.titulo == "quit")
        return false;
    std::cout << "Digite sua avaliação: ";
    std::cin >> rr.avaliacao;
    if (!std::cin)
        return false;
    while (std::cin.get() != '\n')
        continue;
    return true;
}
void MostrarResenha(const Resenha & rr)
{
    std::cout << rr.avaliacao << "\t" << rr.titulo << std::endl;
}
```

Aqui está uma execução de exemplo do programa da Listagem 16.9:

```
Enter book title (quit to quit): The Cat Who Can Teach You Weight Loss
Enter book rating: 8
Enter book title (quit to quit): The Dogs of Dharma
Enter book rating: 6
Enter book title (quit to quit): The Wimps of Wonk
Enter book rating: 3
Enter book title (quit to quit): Farewell and Delete
Enter book rating: 7
Enter book title (quit to quit): quit
Thank you. You entered the following 4 ratings:
Rating  Book
8       The Cat Who Can Teach You Weight Loss
6       The Dogs of Dharma
3       The Wimps of Wonk
7       Farewell and Delete
Sorted by title:
Rating  Book
7       Farewell and Delete
8       The Cat Who Can Teach You Weight Loss
6       The Dogs of Dharma
3       The Wimps of Wonk
Sorted by rating:
Rating  Book
3       The Wimps of Wonk
6       The Dogs of Dharma
7       Farewell and Delete
8       The Cat Who Can Teach You Weight Loss
After shuffling:
Rating  Book
7       Farewell and Delete
3       The Wimps of Wonk
6       The Dogs of Dharma
8       The Cat Who Can Teach You Weight Loss
Bye.
```

### O Laço `for` Baseado em Intervalo (C++11)

O laço `for` baseado em intervalo, mencionado no Capítulo 5, é projetado para funcionar com a STL. Para revisar, aqui está o primeiro exemplo do Capítulo 5:

```cpp
double precos[5] = {4.99, 10.99, 6.87, 7.99, 8.49};
for (double x : precos)
    cout << x << std::endl;
```

O conteúdo entre os parênteses do laço `for` declara uma variável do tipo armazenado em um container e então o nome do container. Em seguida, o corpo do laço usa a variável nomeada para acessar cada elemento do container por vez. Considere, por exemplo, esta instrução da Listagem 16.9:

```cpp
for_each(livros.begin(), livros.end(), MostrarResenha);
```

Ela pode ser substituída pelo seguinte laço `for` baseado em intervalo:

```cpp
for (auto x : livros) MostrarResenha(x);
```

O compilador usará o tipo de `livros`, que é `vector<Resenha>`, para deduzir que `x` é do tipo `Resenha`, e o laço passará cada objeto `Resenha` em `livros` para `MostrarResenha()` por vez.

Diferentemente de `for_each()`, o `for` baseado em intervalo pode alterar o conteúdo de um container. O truque é especificar um parâmetro de referência. Por exemplo, suponha que tenhamos esta função:

```cpp
void AumentarResenha(Resenha &r) { r.avaliacao++; }
```

Você poderia aplicar essa função a cada elemento de `livros` com o seguinte laço:

```cpp
for (auto & x : livros) AumentarResenha(x);
```

---

## Programação Genérica

Agora que você tem alguma experiência usando a STL, vamos examinar a filosofia subjacente. A STL é um exemplo de *programação genérica* (generic programming). A programação orientada a objetos concentra-se no aspecto de dados da programação, enquanto a programação genérica concentra-se nos algoritmos. As principais coisas que as duas abordagens têm em comum são a abstração e a criação de código reutilizável, mas as filosofias são bem diferentes.

Um objetivo da programação genérica é escrever código independente de tipos de dados. Os templates são as ferramentas do C++ para criar programas genéricos. Os templates, é claro, permitem definir uma função ou classe em termos de um tipo genérico. A STL vai mais longe ao fornecer uma representação genérica de algoritmos. Os templates tornam isso possível, mas não sem o elemento adicional de projeto cuidadoso e consciente. Para ver como essa mistura de templates e projeto funciona, vamos examinar por que os iteradores são necessários.

### Por que Iteradores?

Entender iteradores é talvez a chave para entender a STL. Assim como os templates tornam os algoritmos independentes do tipo de dados armazenados, os iteradores tornam os algoritmos independentes do tipo de container usado. Portanto, eles são um componente essencial da abordagem genérica da STL.

Para ver por que os iteradores são necessários, vamos examinar como você poderia implementar uma função `encontrar` para duas representações de dados diferentes e então ver como você poderia generalizar a abordagem. Primeiro, vamos considerar uma função que pesquisa um array comum de `double` em busca de um valor particular. Você poderia escrever a função assim:

```cpp
double * encontrar_arr(double * ar, int n, const double & val)
{
    for (int i = 0; i < n; i++)
        if (ar[i] == val)
            return &ar[i];
    return 0; // ou, em C++11, return nullptr;
}
```

Se a função encontrar o valor no array, ela retorna o endereço no array onde o valor é encontrado; caso contrário, retorna o ponteiro nulo. Ela usa notação de índice para percorrer o array. Você poderia usar um template para generalizar para arrays de qualquer tipo que tenha um operador `==`. No entanto, esse algoritmo ainda está vinculado a uma estrutura de dados específica — o array.

Portanto, vamos examinar a busca em outro tipo de estrutura de dados, a lista encadeada. A lista consiste em estruturas `No` encadeadas:

```cpp
struct No
{
    double item;
    No * p_proximo;
};
```

Suponha que você tenha um ponteiro que aponta para o primeiro nó da lista. O ponteiro `p_proximo` em cada nó aponta para o próximo nó, e o ponteiro `p_proximo` do último nó da lista é definido como 0. Você poderia escrever uma função `encontrar_ll()` desta forma:

```cpp
No* encontrar_ll(No * cabeca, const double & val)
{
    No * inicio;
    for (inicio = cabeca; inicio != 0; inicio = inicio->p_proximo)
        if (inicio->item == val)
            return inicio;
    return 0;
}
```

Novamente, você poderia usar um template para generalizar isso para listas de qualquer tipo de dados que suporte o operador `==`. No entanto, esse algoritmo ainda está vinculado a uma estrutura de dados específica — a lista encadeada.

Se você considerar os detalhes de implementação, as duas funções `encontrar` usam algoritmos diferentes: uma usa indexação de array para percorrer uma lista de itens, e a outra redefine `inicio` para `inicio->p_proximo`. Mas amplamente, os dois algoritmos são o mesmo: compare o valor com cada valor no container em sequência até encontrar uma correspondência.

O objetivo da programação genérica neste caso seria ter uma única função `encontrar` que funcionasse com arrays, listas encadeadas ou qualquer outro tipo de container. Ou seja, não apenas a função deveria ser independente do tipo de dado armazenado no container, ela deveria ser independente da estrutura de dados do container em si. Os templates fornecem uma representação genérica para o tipo de dado armazenado em um container. O que é necessário é uma representação genérica do processo de percorrer os valores em um container. O iterador é essa representação generalizada.

Que propriedades um iterador deve ter para implementar uma função `encontrar`? Aqui está uma lista breve:

- Você deve ser capaz de desreferenciar um iterador para acessar o valor ao qual ele se refere. Ou seja, se `p` é um iterador, `*p` deve ser definido.
- Você deve ser capaz de atribuir um iterador a outro. Ou seja, se `p` e `q` são iteradores, a expressão `p = q` deve ser definida.
- Você deve ser capaz de comparar um iterador com outro quanto à igualdade. Ou seja, se `p` e `q` são iteradores, as expressões `p == q` e `p != q` devem ser definidas.
- Você deve ser capaz de mover um iterador por todos os elementos de um container. Isso pode ser satisfeito definindo-se `++p` e `p++` para um iterador `p`.

Com tais operações, a função `encontrar` pode ser escrita como:

```cpp
typedef double * iterator;
iterator encontrar_arr(iterator begin, iterator end, const double & val)
{
    iterator ar;
    for (ar = begin; ar != end; ar++)
        if (*ar == val)
            return ar;
    return end; // indica que val não foi encontrado
}
```

E para listas encadeadas, você pode definir uma classe iterador que define os operadores `*` e `++`, tornando as duas funções essencialmente idênticas — que é exatamente o ponto da abordagem da STL.

A STL segue a abordagem recém-descrita. Primeiro, cada classe de container (`vector`, `list`, `deque`, e assim por diante) define um tipo iterador apropriado para a classe. Para uma classe, o iterador pode ser um ponteiro; para outra, pode ser um objeto. Qualquer que seja a implementação, o iterador fornecerá as operações necessárias, como `*` e `++`. Em seguida, cada classe de container terá um marcador past-the-end, que é o valor atribuído a um iterador quando ele foi incrementado uma posição além do último valor no container. Cada classe de container terá métodos `begin()` e `end()` que retornam iteradores para o primeiro elemento em um container e para a posição past-the-end. E cada classe de container terá a operação `++` levando um iterador do primeiro elemento ao past-the-end, visitando cada elemento do container pelo caminho.

Para usar uma classe de container, você não precisa saber como seus iteradores são implementados nem como past-the-end é implementado. É suficiente saber que ele tem iteradores, que `begin()` retorna um iterador para o primeiro elemento, e que `end()` retorna um iterador para past-the-end. Por exemplo, suponha que você queira imprimir os valores em um objeto `vector<double>`. Nesse caso, você pode usar:

```cpp
vector<double>::iterator pr;
for (pr = notas.begin(); pr != notas.end(); pr++)
    cout << *pr << endl;
```

Se você usasse o template de classe `list<double>` para armazenar notas, você poderia usar este código:

```cpp
list<double>::iterator pr;
for (pr = notas.begin(); pr != notas.end(); pr++)
    cout << *pr << endl;
```

A única mudança está no tipo declarado para `pr`. Assim, ao fazer cada classe definir iteradores adequados e projetar as classes de maneira uniforme, a STL permite que você escreva o mesmo código para containers com representações internas bastante diferentes.

Com a dedução automática de tipo do C++, você pode simplificar ainda mais e usar o seguinte código com vector ou list:

```cpp
for (auto pr = notas.begin(); pr != notas.end(); pr++)
    cout << *pr << endl;
```

Na verdade, como uma questão de estilo, é melhor evitar o uso direto de iteradores; em vez disso, se possível, você deve usar uma função da STL, como `for_each()`, que cuida dos detalhes para você. Como alternativa, use o laço `for` baseado em intervalo do C++11:

```cpp
for (auto x : notas) cout << x << endl;
```

Portanto, para resumir a abordagem da STL: você começa com um algoritmo para processar um container. Você o expressa nos termos mais gerais possíveis, tornando-o independente do tipo de dado e do tipo de container. Para fazer o algoritmo genérico funcionar com casos específicos, você define iteradores que atendem às necessidades do algoritmo e impõe requisitos ao projeto do container.

### Tipos de Iteradores

Algoritmos diferentes têm requisitos diferentes para iteradores. Por exemplo, um algoritmo `find` precisa que o operador `++` seja definido para que o iterador possa percorrer todo o container. Ele precisa de acesso de leitura aos dados, mas não de escrita. O algoritmo de ordenação usual, por outro lado, requer acesso aleatório para que possa trocar dois elementos não adjacentes.

A STL define cinco tipos de iteradores e descreve seus algoritmos em termos de quais tipos de iteradores eles precisam. Os cinco tipos são o *iterador de entrada* (input iterator), o *iterador de saída* (output iterator), o *iterador de avanço* (forward iterator), o *iterador bidirecional* (bidirectional iterator) e o *iterador de acesso aleatório* (random access iterator). Por exemplo, o protótipo de `find()` se parece com:

```cpp
template<class InputIterator, class T>
InputIterator find(InputIterator first, InputIterator last, const T& value);
```

Isso informa que esse algoritmo requer um iterador de entrada. Da mesma forma, o seguinte protótipo diz que o algoritmo de ordenação requer um iterador de acesso aleatório:

```cpp
template<class RandomAccessIterator>
void sort(RandomAccessIterator first, RandomAccessIterator last);
```

Todos os cinco tipos de iteradores podem ser desreferenciados (ou seja, o operador `*` é definido para eles) e podem ser comparados por igualdade (usando o operador `==`) e desigualdade (usando o operador `!=`).

**Iteradores de Entrada**

O termo *entrada* é usado do ponto de vista de um programa. Ou seja, informações indo do container para o programa são consideradas entrada, assim como informações do teclado para o programa são consideradas entrada. Portanto, um iterador de entrada é aquele que um programa pode usar para ler valores de um container. Em particular, desreferenciar um iterador de entrada deve permitir que um programa leia um valor de um container, mas não necessariamente altere esse valor. Portanto, algoritmos que requerem um iterador de entrada são algoritmos que não alteram os valores mantidos em um container.

Um iterador de entrada tem que permitir que você acesse todos os valores em um container. Ele o faz suportando o operador `++`, tanto na forma prefixada quanto na sufixada. Se você definir um operador de entrada para o primeiro elemento em um container e incrementá-lo até que alcance past-the-end, ele apontará para cada item do container uma vez no caminho. Observe que um iterador de entrada é um iterador de sentido único; ele pode incrementar, mas não pode retroceder.

**Iteradores de Saída**

No uso da STL, o termo *saída* indica que o iterador é usado para transferir informações de um programa para um container. (Assim, a saída para o programa é entrada para o container.) Um iterador de saída é semelhante a um iterador de entrada, exceto que a desreferência é garantida para permitir que um programa altere um valor do container, mas não o leia.

Em resumo, você pode usar um iterador de entrada para algoritmos de passagem única e somente leitura e um iterador de saída para algoritmos de passagem única e somente escrita.

**Iteradores de Avanço**

Como os iteradores de entrada e saída, os iteradores de avanço usam apenas os operadores `++` para navegar por um container. Portanto, um iterador de avanço só pode avançar pelo container um elemento de cada vez. Entretanto, diferentemente dos iteradores de entrada e saída, ele necessariamente percorre uma sequência de valores na mesma ordem toda vez que é usado. Além disso, depois de incrementar um iterador de avanço, você ainda pode desreferenciar o valor iterador anterior, se o tiver salvo, e obter o mesmo valor. Essas propriedades tornam possíveis os algoritmos de múltiplas passagens.

Um iterador de avanço pode permitir tanto leitura quanto modificação de dados, ou pode permitir apenas leitura:

```cpp
int * pirw;  // iterador de leitura-escrita
const int * pis; // iterador somente leitura
```

**Iteradores Bidirecionais**

Suponha que você tenha um algoritmo que precise ser capaz de percorrer um container em ambas as direções. Um iterador bidirecional tem todos os recursos de um iterador de avanço e adiciona suporte para os dois operadores de decremento (prefixado e sufixado).

**Iteradores de Acesso Aleatório**

Alguns algoritmos, como ordenação padrão e busca binária, requerem a capacidade de saltar diretamente para um elemento arbitrário de um container. Isso é chamado de *acesso aleatório* (random access), e requer um iterador de acesso aleatório. Esse tipo de iterador tem todos os recursos de um iterador bidirecional, mais ele adiciona operações (como adição de ponteiro) que suportam acesso aleatório e operadores relacionais para ordenar os elementos. A Tabela 16.3 lista as operações que um iterador de acesso aleatório tem além das de um iterador bidirecional.

**Tabela 16.3 — Operações do Iterador de Acesso Aleatório**

| Expressão | Descrição |
|---|---|
| `a + n` | Aponta para o n-ésimo elemento após aquele para o qual `a` aponta |
| `n + a` | Mesmo que `a + n` |
| `a - n` | Aponta para o n-ésimo elemento antes daquele para o qual `a` aponta |
| `r += n` | Equivalente a `r = r + n` |
| `r -= n` | Equivalente a `r = r - n` |
| `a[n]` | Equivalente a `*(a + n)` |
| `b - a` | O valor de `n` tal que `b == a + n` |
| `a < b` | Verdadeiro se `b - a > 0` |
| `a > b` | Verdadeiro se `b < a` |
| `a >= b` | Verdadeiro se `!(a < b)` |
| `a <= b` | Verdadeiro se `!(a > b)` |

### Hierarquia de Iteradores

Você provavelmente notou que os tipos de iteradores formam uma hierarquia. Um iterador de avanço tem todas as capacidades de um iterador de entrada e de um iterador de saída, mais suas próprias capacidades. Um iterador bidirecional tem todas as capacidades de um iterador de avanço, mais suas próprias capacidades. E um iterador de acesso aleatório tem todas as capacidades de um iterador bidirecional, mais suas próprias capacidades.

A Tabela 16.4 resume as principais capacidades dos iteradores.

**Tabela 16.4 — Capacidades dos Iteradores**

| Capacidade do Iterador | Entrada | Saída | Avanço | Bidirecional | Acesso Aleatório |
|---|---|---|---|---|---|
| Desref. leitura | Sim | Não | Sim | Sim | Sim |
| Desref. escrita | Não | Sim | Sim | Sim | Sim |
| Ordem fixa e repetível | Não | Não | Sim | Sim | Sim |
| `++i`, `i++` | Sim | Sim | Sim | Sim | Sim |
| `--i`, `i--` | Não | Não | Não | Sim | Sim |
| `i[n]` | Não | Não | Não | Não | Sim |
| `i + n` | Não | Não | Não | Não | Sim |
| `i - n` | Não | Não | Não | Não | Sim |
| `i += n` | Não | Não | Não | Não | Sim |
| `i -= n` | Não | Não | Não | Não | Sim |

Um algoritmo escrito em termos de um tipo específico de iterador pode usar esse tipo de iterador ou qualquer outro iterador que tenha as capacidades requeridas. Assim, um container com, digamos, um iterador de acesso aleatório pode usar um algoritmo escrito para um iterador de entrada.

Por que todos esses tipos diferentes de iteradores? A ideia é escrever um algoritmo usando o iterador com o menor número de requisitos possível, permitindo que seja usado com a maior variedade de containers. Assim, a função `find()`, ao usar um humilde iterador de entrada, pode ser usada com qualquer container que contenha valores legíveis. A função `sort()`, entretanto, ao requerer um iterador de acesso aleatório, só pode ser usada com containers que suportam esse tipo de iterador.

Observe que os vários tipos de iteradores não são tipos definidos; em vez disso, eles são caracterizações conceituais. Como mencionado anteriormente, cada classe de container define um nome `typedef` de escopo de classe chamado `iterator`. Assim, a classe `vector<int>` tem iteradores do tipo `vector<int>::iterator`. A documentação dessa classe diria que os iteradores de `vector` são iteradores de acesso aleatório. Isso, por sua vez, permite que você use algoritmos baseados em qualquer tipo de iterador, porque um iterador de acesso aleatório tem todas as capacidades de iterador. De forma análoga, uma classe `list<int>` tem iteradores do tipo `list<int>::iterator`. A STL implementa uma lista duplamente encadeada, portanto usa um iterador bidirecional. Assim, ela não pode usar algoritmos baseados em iteradores de acesso aleatório, mas pode usar algoritmos baseados em iteradores menos exigentes.

### Conceitos, Refinamentos e Modelos

A STL tem vários recursos, como tipos de iteradores, que não são expressos na linguagem C++. Ou seja, embora você possa projetar, digamos, uma classe que tenha as propriedades de um iterador de avanço, você não pode fazer o compilador restringir um algoritmo a usar apenas essa classe. A razão é que o iterador de avanço é um conjunto de requisitos, não um tipo. Os requisitos poderiam ser satisfeitos por uma classe de iterador que você projetou, mas também poderiam ser satisfeitos por um ponteiro comum. Um algoritmo da STL funciona com qualquer implementação de iterador que atenda aos seus requisitos. A literatura da STL usa a palavra *conceito* (concept) para descrever um conjunto de requisitos. Assim, há um conceito de iterador de entrada, um conceito de iterador de avanço, e assim por diante.

Conceitos podem ter um relacionamento semelhante à herança. Por exemplo, um iterador bidirecional herda as capacidades de um iterador de avanço. Porém, você não pode aplicar o mecanismo de herança do C++ a iteradores. Conceitualmente, no entanto, ele herda. Alguma literatura da STL usa o termo *refinamento* para indicar essa herança conceitual. Assim, um iterador bidirecional é um refinamento do conceito de iterador de avanço.

Uma implementação específica de um conceito é chamada de *modelo* (model). Assim, um ponteiro para `int` comum é um modelo do conceito de iterador de acesso aleatório. É também um modelo de um iterador de avanço, pois satisfaz todos os requisitos desse conceito.

### O Ponteiro como Iterador

Iteradores são generalizações de ponteiros, e um ponteiro satisfaz todos os requisitos de iterador. Iteradores formam a interface para os algoritmos da STL, e ponteiros são iteradores, portanto os algoritmos da STL podem usar ponteiros para operar em containers não-STL baseados em ponteiros. Por exemplo, você pode usar algoritmos da STL com arrays. Suponha que `Recebimentos` seja um array de valores `double`, e você queira ordená-los em ordem crescente:

```cpp
const int TAMANHO = 100;
double Recebimentos[TAMANHO];
```

A função `sort()` da STL, lembre-se, recebe como argumentos um iterador apontando para o primeiro elemento em um container e um iterador apontando para o past-the-end. Bem, `&Recebimentos[0]` (ou simplesmente `Recebimentos`) é o endereço do primeiro elemento, e `&Recebimentos[TAMANHO]` (ou simplesmente `Recebimentos + TAMANHO`) é o endereço do elemento seguinte ao último no array. Portanto, a seguinte chamada de função ordena o array:

```cpp
sort(Recebimentos, Recebimentos + TAMANHO);
```

### `copy()`, `ostream_iterator` e `istream_iterator`

A STL fornece alguns iteradores predefinidos. Para ver por quê, vamos estabelecer um pouco de contexto. Há um algoritmo chamado `copy()` para copiar dados de um container para outro. Esse algoritmo é expresso em termos de iteradores, portanto pode copiar de um tipo de container para outro ou até mesmo de ou para um array, porque você pode usar ponteiros em um array como iteradores. Por exemplo, o seguinte copia um array para um vetor:

```cpp
int lancamentos[10] = {6, 7, 2, 9, 4, 11, 8, 7, 10, 5};
vector<int> dados(10);
copy(lancamentos, lancamentos + 10, dados.begin()); // copiar array para vector
```

Os dois primeiros argumentos iteradores para `copy()` representam um intervalo a ser copiado, e o argumento iterador final representa o local para o qual o primeiro item é copiado. Os dois primeiros argumentos devem ser iteradores de entrada (ou melhores), e o argumento final deve ser um iterador de saída (ou melhor). A função `copy()` sobrescreve os dados existentes no container de destino, e o container deve ser grande o suficiente para conter os elementos copiados.

Agora suponha que você queira copiar informações para a tela. Você poderia usar `copy()` se houvesse um iterador representando o fluxo de saída. A STL fornece tal iterador com o template `ostream_iterator`. Usando a terminologia da STL, esse template é um modelo do conceito de iterador de saída. Ele também é um exemplo de um *adaptador* — uma classe ou função que converte alguma outra interface em uma interface usada pela STL. Você pode criar um iterador desse tipo incluindo o arquivo de cabeçalho `iterator` e fazendo uma declaração:

```cpp
#include <iterator>
...
ostream_iterator<int, char> iter_saida(cout, " ");
```

O iterador `iter_saida` agora se torna uma interface que permite usar `cout` para exibir informações. O primeiro argumento de template (`int`, neste caso) indica o tipo de dado sendo enviado ao fluxo de saída. O segundo argumento de template (`char`, neste caso) indica o tipo de caractere usado pelo fluxo de saída. O primeiro argumento do construtor (`cout`, neste caso) identifica o fluxo de saída sendo usado. O argumento de string de caractere final é um separador a ser exibido após cada item enviado ao fluxo de saída.

Você poderia usar o iterador desta forma:

```cpp
*iter_saida++ = 15; // funciona como cout << 15 << " ";
```

Para um ponteiro regular, isso significaria atribuir o valor 15 ao local apontado e depois incrementar o ponteiro. Para este `ostream_iterator`, entretanto, a instrução significa enviar 15 e depois uma string consistindo de um espaço ao fluxo de saída gerenciado por `cout`. Em seguida, ele deve se preparar para a próxima operação de saída. Você poderia usar o iterador com `copy()` da seguinte forma:

```cpp
copy(dados.begin(), dados.end(), iter_saida); // copiar vector para fluxo de saída
```

Isso significaria copiar todo o intervalo do container `dados` para o fluxo de saída — ou seja, exibir o conteúdo do container.

Ou você poderia pular a criação de um iterador nomeado e construir um iterador anônimo:

```cpp
copy(dados.begin(), dados.end(), ostream_iterator<int, char>(cout, " "));
```

De forma semelhante, o arquivo de cabeçalho `iterator` define um template `istream_iterator` para adaptar a entrada de `istream` à interface de iterador. É um modelo do conceito de iterador de entrada. Você poderia usar dois objetos `istream_iterator` para definir um intervalo de entrada para `copy()`:

```cpp
copy(istream_iterator<int, char>(cin),
     istream_iterator<int, char>(), dados.begin());
```

Como `ostream_iterator`, `istream_iterator` usa dois argumentos de template. O primeiro indica o tipo de dado a ser lido, e o segundo indica o tipo de caractere usado pelo fluxo de entrada. Usar um argumento de construtor `cin` significa usar o fluxo de entrada gerenciado por `cin`. Omitir o argumento de construtor indica falha de entrada, portanto o código anterior significa ler do fluxo de entrada até o fim de arquivo, incompatibilidade de tipo ou alguma outra falha de entrada.

### Outros Iteradores Úteis

O arquivo de cabeçalho `iterator` fornece alguns outros tipos de iteradores predefinidos especializados além de `ostream_iterator` e `istream_iterator`. Eles são `reverse_iterator`, `back_insert_iterator`, `front_insert_iterator` e `insert_iterator`.

**O `reverse_iterator`**

Incrementar um iterador reverso (reverse iterator) faz com que ele decremente. Por que não simplesmente decrementar um iterador regular? A principal razão é simplificar o uso de funções existentes. Suponha que você queira exibir o conteúdo do container `dados` em ordem inversa. O container `vector` tem uma função-membro chamada `rbegin()` que retorna um iterador reverso apontando para past-the-end, e uma `rend()` que retorna um iterador reverso apontando para o primeiro elemento. Como incrementar um iterador reverso faz com que ele decremente, você pode usar a seguinte instrução para exibir o conteúdo ao contrário:

```cpp
copy(dados.rbegin(), dados.rend(), iter_saida); // exibir em ordem inversa
```

Você nem precisa declarar um iterador reverso.

> **Nota:** Tanto `rbegin()` quanto `end()` retornam o mesmo valor (past-the-end), mas como um tipo diferente (`reverse_iterator` versus `iterator`). De forma semelhante, tanto `rend()` quanto `begin()` retornam o mesmo valor (um iterador para o primeiro elemento), mas como um tipo diferente.

Os ponteiros reversos têm que fazer uma compensação especial. Suponha que `rp` seja um ponteiro reverso inicializado para `dados.rbegin()`. Como `rbegin()` retorna past-the-end, você não deve tentar desreferenciar esse endereço. Da mesma forma, se `rend()` é realmente a localização do primeiro elemento, `copy()` para uma posição antes, porque o final do intervalo não está dentro de um intervalo. Os ponteiros reversos resolvem ambos os problemas decrementando primeiro e depois desreferenciando. Ou seja, `*rp` desreferencia o valor do iterador imediatamente anterior ao valor atual de `*rp`.

A Listagem 16.10 ilustra o uso de `copy()`, um iterador `ostream` e um iterador reverso.

**Listagem 16.10 — copiar.cpp**

```cpp
// copiar.cpp -- copy() e iteradores
#include <iostream>
#include <iterator>
#include <vector>
int main()
{
    using namespace std;
    int lancamentos[10] = {6, 7, 2, 9, 4, 11, 8, 7, 10, 5};
    vector<int> dados(10);
    // copiar do array para o vector
    copy(lancamentos, lancamentos + 10, dados.begin());
    cout << "Que os dados sejam lançados!\n";
    // criar um iterador ostream
    ostream_iterator<int, char> iter_saida(cout, " ");
    // copiar do vector para a saída
    copy(dados.begin(), dados.end(), iter_saida);
    cout << endl;
    cout << "Uso implícito do iterador reverso.\n";
    copy(dados.rbegin(), dados.rend(), iter_saida);
    cout << endl;
    cout << "Uso explícito do iterador reverso.\n";
    vector<int>::reverse_iterator ri;
    for (ri = dados.rbegin(); ri != dados.rend(); ++ri)
        cout << *ri << ' ';
    cout << endl;
    return 0;
}
```

Aqui está a saída do programa da Listagem 16.10:

```
Let the dice be cast!
6 7 2 9 4 11 8 7 10 5
Implicit use of reverse iterator.
5 10 7 8 11 4 9 2 7 6
Explicit use of reverse iterator.
5 10 7 8 11 4 9 2 7 6
```

**Os Iteradores de Inserção**

Os outros três iteradores (`back_insert_iterator`, `front_insert_iterator` e `insert_iterator`) também aumentam a generalidade dos algoritmos da STL. Muitas funções da STL são como `copy()` por enviar seus resultados para um local indicado por um iterador de saída. Recall that the following copies values to the location beginning at `dados.begin()`:

```cpp
copy(lancamentos, lancamentos + 10, dados.begin());
```

Esses valores sobrescrevem o conteúdo anterior em `dados`, e a função assume que `dados` tem espaço suficiente para conter os valores. Isso é, `copy()` não ajusta automaticamente o tamanho do destino para caber nas informações enviadas a ele. Suponha que você não saiba com antecedência quão grande `dados` deveria ser. Ou suponha que você queira adicionar elementos a `dados` em vez de sobrescrever os existentes.

Os três iteradores de inserção resolvem esses problemas convertendo o processo de cópia em um processo de inserção. A inserção adiciona novos elementos sem sobrescrever dados existentes, e usa alocação automática de memória para garantir que as novas informações caibam. Um `back_insert_iterator` insere itens no final do container, e um `front_insert_iterator` insere itens no início. Finalmente, o `insert_iterator` insere itens antes do local especificado como argumento para o construtor do `insert_iterator`. Todos os três desses iteradores são modelos do conceito de iterador de saída.

Há restrições. Um `back_insert_iterator` só pode ser usado com tipos de container que permitem inserção rápida no final. (Rápida refere-se a um algoritmo de tempo constante.) A classe `vector` se qualifica. Um `front_insert_iterator` só pode ser usado com tipos de container que permitem inserção em tempo constante no início. Aqui, a classe `vector` não se qualifica, mas a classe `queue` se qualifica. O `insert_iterator` não tem essas restrições. Portanto, você pode usá-lo para inserir material na frente de um vetor.

> **Dica:** Você pode usar um `insert_iterator` para converter um algoritmo que copia dados em um que insere dados.

Esses iteradores recebem o tipo de container como argumento de template e o identificador do container real como argumento de construtor. Para criar um `back_insert_iterator` para um container `vector<int>` chamado `dados`:

```cpp
back_insert_iterator<vector<int> > iter_tras(dados);
```

A razão pela qual você precisa declarar o tipo de container é que o iterador tem que fazer uso do método de container apropriado. O código para o construtor do `back_insert_iterator` vai assumir que um método `push_back()` existe para o tipo passado a ele. A função `copy()`, sendo uma função autônoma, não tem direitos de acesso para redimensionar um container. Mas a declaração recém-mostrada permite que `iter_tras` use o método `vector<int>::push_back()`, que tem direitos de acesso.

Uma declaração de `front_insert_iterator` tem a mesma forma. Uma declaração de `insert_iterator` tem um argumento adicional de construtor para identificar o local de inserção:

```cpp
insert_iterator<vector<int> > iter_inserir(dados, dados.begin());
```

A Listagem 16.11 ilustra o uso de dois desses iteradores. Além disso, ela usa `for_each()` em vez de um iterador `ostream` para saída.

**Listagem 16.11 — inserir.cpp**

```cpp
// inserir.cpp -- copy() e iteradores de inserção
#include <iostream>
#include <string>
#include <iterator>
#include <vector>
#include <algorithm>
void exibir(const std::string & s) { std::cout << s << " "; }
int main()
{
    using namespace std;
    string s1[4] = {"fine", "fish", "fashion", "fate"};
    string s2[2] = {"busy", "bats"};
    string s3[2] = {"silly", "singers"};
    vector<string> palavras(4);
    copy(s1, s1 + 4, palavras.begin());
    for_each(palavras.begin(), palavras.end(), exibir);
    cout << endl;
    // construir objeto back_insert_iterator anônimo
    copy(s2, s2 + 2, back_insert_iterator<vector<string> >(palavras));
    for_each(palavras.begin(), palavras.end(), exibir);
    cout << endl;
    // construir objeto insert_iterator anônimo
    copy(s3, s3 + 2, insert_iterator<vector<string> >(palavras,
                                palavras.begin()));
    for_each(palavras.begin(), palavras.end(), exibir);
    cout << endl;
    return 0;
}
```

Aqui está a saída do programa da Listagem 16.11:

```
fine fish fashion fate
fine fish fashion fate busy bats
silly singers fine fish fashion fate busy bats
```

O primeiro `copy()` copia as quatro strings de `s1` para `palavras`. Isso funciona em parte porque `palavras` é declarado para conter quatro strings. Em seguida, o `back_insert_iterator` insere as strings de `s2` logo antes do final do array `palavras`, expandindo o tamanho de `palavras` para seis elementos. Finalmente, o `insert_iterator` insere as duas strings de `s3` logo antes do primeiro elemento de `palavras`, expandindo o tamanho de `palavras` para oito elementos.

Se você estiver se sentindo sobrecarregado com todas as variedades de iteradores, tenha em mente que usá-los os tornará familiares. Tenha também em mente que esses iteradores predefinidos expandem a generalidade dos algoritmos da STL. Assim, não apenas `copy()` pode copiar informações de um container para outro, como pode copiar informações de um container para o fluxo de saída e do fluxo de entrada para um container. E você pode também usar `copy()` para inserir material em outro container. Portanto, você acaba com uma única função fazendo o trabalho de muitas.

---

[Anterior](capitulo-16-01-string-ponteiros-inteligentes.md) | [Índice](README.md) | [Próximo](capitulo-16-03-containers-algoritmos.md)
