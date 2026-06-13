# Capítulo 16 — Containers STL, Algoritmos e Outras Bibliotecas

> Tradução não oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> [Anterior](capitulo-16-02-stl-iteradores.md) | [Índice](README.md) | [Próximo](capitulo-16-04-lista-init-resumo.md)

## Tipos de Containers

A STL tem tanto conceitos de containers quanto tipos de containers. Os conceitos são categorias gerais com nomes como container, container de sequência e container associativo. Os tipos de containers são templates que você pode usar para criar objetos de container específicos. Os 11 tipos de containers originais são `deque`, `list`, `queue`, `priority_queue`, `stack`, `vector`, `map`, `multimap`, `set`, `multiset` e `bitset`. (Este capítulo não discute `bitset`, que é um container para lidar com dados no nível de bit.) O C++11 acrescenta `forward_list`, `unordered_map`, `unordered_multimap`, `unordered_set` e `unordered_multiset`, e move `bitset` da categoria de container para sua própria categoria separada. Como os conceitos categorizam os tipos, vamos começar com eles.

### Conceitos de Container

Nenhum tipo corresponde ao conceito de container básico, mas o conceito descreve elementos comuns a todas as classes de container. É uma espécie de classe base abstrata conceitual — conceitual porque as classes de container não usam realmente o mecanismo de herança. Ou, dito de outra forma, o conceito de container estabelece um conjunto de requisitos que todas as classes de container da STL devem satisfazer.

Um *container* (container) é um objeto que armazena outros objetos, todos de um único tipo. Os objetos armazenados podem ser objetos no sentido de OOP (programação orientada a objetos), ou podem ser valores de tipos embutidos. Os dados armazenados em um container são *de propriedade* do container. Isso significa que quando um container expira, os dados armazenados nele também expiram. (Entretanto, se os dados são ponteiros, os dados apontados não expiram necessariamente.)

Você não pode armazenar qualquer tipo de objeto em um container. Em particular, o tipo tem que ser *construível por cópia* (copy constructable) e *atribuível* (assignable). Tipos básicos satisfazem esses requisitos, assim como tipos de classe — a não ser que a definição da classe torne um ou ambos o construtor de cópia e o operador de atribuição privados ou protegidos. (O C++11 refina os conceitos, adicionando termos como `CopyInsertable` e `MoveInsertable`, mas faremos uma visão geral mais simplificada, ainda que menos precisa.)

O container básico não garante que seus elementos sejam armazenados em qualquer ordem particular ou que a ordem não mude, mas refinamentos do conceito podem adicionar tais garantias. Todos os containers fornecem certas funcionalidades e operações. A Tabela 16.5 resume várias dessas funcionalidades comuns. Na tabela, `X` representa um tipo de container (como `vector`), `T` representa o tipo de objeto armazenado no container, `a` e `b` representam valores do tipo `X`, `r` é um valor do tipo `X&`, e `u` representa um identificador do tipo `X`.

**Tabela 16.5 — Algumas Propriedades Básicas do Container**

| Expressão | Tipo de Retorno | Descrição | Complexidade |
|---|---|---|---|
| `X::iterator` | Iterador apontando para `T` | Qualquer categoria de iterador satisfazendo os requisitos de iterador de avanço | Tempo de compilação |
| `X::value_type` | `T` | O tipo para `T` | Tempo de compilação |
| `X u;` | | Cria um container vazio chamado `u` | Constante |
| `X();` | | Cria um container anônimo vazio | Constante |
| `X u(a);` | | Construtor de cópia, pós-condição: `u == a` | Linear |
| `X u = a;` | | Mesmo efeito que `X u(a);` | Linear |
| `r = a;` | `X&` | Atribuição por cópia, pós-condição: `r == a` | Linear |
| `(&a)->~X();` | `void` | Aplica destrutor a cada elemento do container | Linear |
| `a.begin()` | `iterator` | Retorna um iterador referindo-se ao primeiro elemento do container | Constante |
| `a.end()` | `iterator` | Retorna um iterador que é um valor past-the-end | Constante |
| `a.size()` | tipo integral sem sinal | Retorna um número de elementos igual a `a.end() - a.begin()` | Constante |
| `a.swap(b)` | `void` | Troca o conteúdo de `a` e `b` | Constante |
| `a == b` | conversível para `bool` | Retorna `true` se `a` e `b` têm o mesmo tamanho e cada elemento em `a` é equivalente ao elemento correspondente em `b` | Linear |
| `a != b` | conversível para `bool` | Retorna `!(a == b)` | Linear |

A coluna de Complexidade na Tabela 16.5 descreve o tempo necessário para realizar uma operação. Esta tabela lista três possibilidades, que, do mais rápido ao mais lento, são as seguintes:

- Tempo de compilação
- Tempo constante
- Tempo linear

Se a complexidade for de tempo de compilação, a ação é realizada durante a compilação e não usa tempo de execução. Uma complexidade constante significa que a operação ocorre em tempo de execução, mas não depende do número de elementos em um objeto. Uma complexidade linear significa que o tempo é proporcional ao número de elementos. Assim, se `a` e `b` são containers, `a == b` tem complexidade linear porque a operação `==` pode ter que ser aplicada a cada elemento do container. Na verdade, esse é o pior cenário. Se dois containers têm tamanhos diferentes, nenhuma comparação individual precisa ser feita.

> **Complexidade de Tempo Constante e Linear**
>
> Imagine uma caixa longa e estreita cheia de pacotes grandes dispostos em uma linha, e suponha que a caixa esteja aberta apenas em uma extremidade. Suponha que sua tarefa seja descarregar o pacote na extremidade aberta. Esta é uma tarefa de tempo constante. Se há 10 ou 1.000 pacotes atrás do que está no final, isso não faz diferença.
>
> Agora suponha que sua tarefa é pegar o pacote na extremidade fechada da caixa. Esta é uma tarefa de tempo linear. Se há 10 pacotes no total, você tem que descarregar 10 pacotes para chegar ao que está na extremidade fechada. Se há 100 pacotes, você tem que descarregar 100. Supondo que você é um trabalhador incansável que pode mover apenas 1 pacote por vez, essa tarefa levará 10 vezes mais tempo do que a primeira.
>
> A ideia de complexidade de tempo descreve o efeito do tamanho do container no tempo de execução, mas ignora outros fatores. Requisitos de complexidade são características da STL. Embora os detalhes de uma implementação possam estar ocultos, as especificações de desempenho devem ser públicas para que você saiba o custo computacional de realizar uma operação específica.

### Adições do C++11 aos Requisitos de Container

A Tabela 16.6 mostra algumas adições que o C++11 fez aos requisitos gerais de container. A tabela usa a notação `rv` para denotar um rvalue não constante do tipo `X`.

**Tabela 16.6 — Alguns Requisitos de Container Básico Adicionados (C++11)**

| Expressão | Tipo de Retorno | Descrição | Complexidade |
|---|---|---|---|
| `X u(rv);` | | Construtor de movimento, pós-condição: `u` tem o valor que `rv` tinha antes da construção | Linear |
| `X u = rv;` | | Mesmo efeito que `X u(rv);` | |
| `a = rv;` | `X&` | Atribuição de movimento, pós-condição: `a` tem o valor que `rv` tinha antes da atribuição | Linear |
| `a.cbegin()` | `const_iterator` | Retorna um iterador `const` referindo-se ao primeiro elemento do container | Constante |
| `a.cend()` | `const_iterator` | Retorna um iterador `const` que é um valor past-the-end | Constante |

A diferença entre construção por cópia e atribuição por cópia por um lado, e construção por movimento e atribuição por movimento por outro, é que uma operação de cópia deixa o original inalterado, enquanto uma operação de movimento pode alterar o original, talvez transferindo a propriedade sem fazer nenhuma cópia. Quando o objeto de origem é temporário, operações de movimento podem fornecer código mais eficiente do que a cópia regular. O Capítulo 18 discute a semântica de movimento mais detalhadamente.

---

## Sequências

Você pode refinar o conceito de container básico adicionando requisitos. A *sequência* (sequence) é um refinamento importante porque vários dos tipos de container da STL — `deque`, `forward_list` (C++11), `list`, `queue`, `priority_queue`, `stack` e `vector` — são sequências. (Lembre-se de que uma fila — `queue` — permite que elementos sejam adicionados na extremidade traseira e removidos da frente. Uma fila de dois lados, representada por `deque`, permite adição e remoção em ambas as extremidades.) O requisito de que o iterador seja pelo menos um iterador de avanço garante que os elementos sejam organizados em uma ordem definida que não muda de um ciclo de iteração para o próximo. A classe `array` também é classificada como container de sequência, embora não satisfaça todos os requisitos.

A sequência também requer que seus elementos sejam organizados em ordem linear estrita. Ou seja, há um primeiro elemento, há um último elemento, e cada elemento exceto o primeiro e o último tem exatamente um elemento imediatamente à sua frente e um elemento imediatamente após. Um array e uma lista encadeada são exemplos de sequências, enquanto uma estrutura de ramificação (na qual cada nó aponta para dois nós filhos) não é.

Como os elementos em uma sequência têm uma ordem definida, operações como inserir valores em um local específico e apagar um intervalo específico tornam-se possíveis. A Tabela 16.7 lista essas e outras operações exigidas de uma sequência.

**Tabela 16.7 — Requisitos de Sequência**

| Expressão | Tipo de Retorno | Descrição |
|---|---|---|
| `X a(n,t);` | | Declara uma sequência `a` de `n` cópias do valor `t` |
| `X(n, t)` | | Cria uma sequência anônima de `n` cópias do valor `t` |
| `X a(i, j)` | | Declara uma sequência `a` inicializada com o conteúdo do intervalo `[i,j)` |
| `X(i, j)` | | Cria uma sequência anônima inicializada com o conteúdo do intervalo `[i,j)` |
| `a.insert(p,t)` | `iterator` | Insere uma cópia de `t` antes de `p` |
| `a.insert(p,n,t)` | `void` | Insere `n` cópias de `t` antes de `p` |
| `a.insert(p,i,j)` | `void` | Insere cópias dos elementos no intervalo `[i,j)` antes de `p` |
| `a.erase(p)` | `iterator` | Apaga o elemento apontado por `p` |
| `a.erase(p,q)` | `iterator` | Apaga os elementos no intervalo `[p,q)` |
| `a.clear()` | `void` | Equivalente a `erase(begin(), end())` |

Como as classes de template `deque`, `list`, `queue`, `priority_queue`, `stack` e `vector` são todas modelos do conceito de sequência, todas suportam os operadores da Tabela 16.7. Além disso, há operações disponíveis para alguns desses seis modelos. Quando permitido, elas têm complexidade de tempo constante. A Tabela 16.8 lista essas operações adicionais.

**Tabela 16.8 — Requisitos de Sequência Opcionais**

| Expressão | Tipo de Retorno | Significado | Container |
|---|---|---|---|
| `a.front()` | `T&` | `*a.begin()` | `vector`, `list`, `deque` |
| `a.back()` | `T&` | `*--a.end()` | `vector`, `list`, `deque` |
| `a.push_front(t)` | `void` | `a.insert(a.begin(), t)` | `list`, `deque` |
| `a.push_back(t)` | `void` | `a.insert(a.end(), t)` | `vector`, `list`, `deque` |
| `a.pop_front(t)` | `void` | `a.erase(a.begin())` | `list`, `deque` |
| `a.pop_back(t)` | `void` | `a.erase(--a.end())` | `vector`, `list`, `deque` |
| `a[n]` | `T&` | `*(a.begin() + n)` | `vector`, `deque` |
| `a.at(n)` | `T&` | `*(a.begin() + n)` | `vector`, `deque` |

A Tabela 16.8 merece um ou dois comentários. Primeiro, observe que `a[n]` e `a.at(n)` retornam uma referência ao n-ésimo elemento (numerado a partir de 0) em um container. A diferença entre os dois é que `a.at(n)` faz verificação de limites e lança uma exceção `out_of_range` se `n` estiver fora do intervalo válido para o container. Em seguida, você pode se perguntar por que, digamos, `push_front()` é definido para `list` e `deque`, mas não para `vector`. Suponha que você queira inserir um novo valor na frente de um vetor de 100 elementos. Para criar espaço, você tem que mover o elemento 99 para a posição 100, e depois tem que mover o elemento 98 para a posição 99, e assim por diante. Essa é uma operação de complexidade de tempo linear porque mover 100 elementos leva 100 vezes mais tempo do que mover um único elemento. Mas as operações na Tabela 16.8 devem ser implementadas apenas se puderem ser realizadas com complexidade de tempo constante. O projeto para listas e filas de dois lados, entretanto, permite que um elemento seja adicionado à frente sem mover os outros elementos para novos locais, portanto eles podem implementar `push_front()` com complexidade de tempo constante.

Vamos examinar mais de perto os seis tipos de container de sequência.

### `vector`

Você já viu vários exemplos usando o template `vector`, que é declarado no arquivo de cabeçalho `vector`. Em resumo, `vector` é uma representação de classe de um array. A classe fornece gerenciamento automático de memória que permite que o tamanho de um objeto `vector` varie dinamicamente, crescendo e encolhendo à medida que os elementos são adicionados ou removidos. Ele fornece acesso aleatório a elementos. Elementos podem ser adicionados ou removidos do final em tempo constante, mas inserção e remoção do início e do meio são operações de tempo linear.

Além de ser uma sequência, um container `vector` também é um modelo do conceito de *container reversível* (reversible container). Isso adiciona mais dois métodos de classe: `rbegin()` retorna um iterador para o primeiro elemento da sequência invertida, e `rend()` retorna um iterador past-the-end para a sequência invertida.

O template de classe `vector` é o mais simples dos tipos de sequência e é considerado o tipo que deve ser usado por padrão, a menos que os requisitos do programa sejam melhor satisfeitos pelas virtudes particulares dos outros tipos.

### `deque`

O template de classe `deque` (declarado no arquivo de cabeçalho `deque`) representa uma fila de dois lados (double-ended queue), um tipo frequentemente chamado de *deque* (pronunciado "deck"), abreviado. Conforme implementado na STL, é muito parecido com um container `vector`, suportando acesso aleatório. A principal diferença é que inserir e remover itens do início de um objeto `deque` são operações de tempo constante em vez de serem operações de tempo linear como são para `vector`. Portanto, se a maioria das operações ocorrer no início e no final de uma sequência, você deve considerar usar uma estrutura de dados `deque`.

O objetivo de inserção e remoção em tempo constante em ambas as extremidades de um `deque` torna o projeto de um objeto `deque` mais complexo do que o de um objeto `vector`. Assim, embora ambos ofereçam acesso aleatório a elementos e inserção e remoção em tempo linear do meio de uma sequência, o container `vector` deve permitir execução mais rápida dessas operações.

### `list`

O template de classe `list` (declarado no arquivo de cabeçalho `list`) representa uma lista duplamente encadeada (doubly linked list). Cada elemento, exceto o primeiro e o último, é ligado ao item antes e ao item seguinte, implicando que uma lista pode ser percorrida em ambas as direções. A diferença crucial entre `list` e `vector` é que `list` fornece inserção e remoção de elementos em tempo constante em qualquer local da lista. (Lembre-se de que o template `vector` fornece inserção e remoção em tempo linear, exceto no final, onde fornece inserção e remoção em tempo constante.) Assim, `vector` enfatiza acesso rápido via acesso aleatório, enquanto `list` enfatiza inserção e remoção rápida de elementos.

Como `vector`, `list` é um container reversível. Diferentemente de `vector`, `list` não suporta notação de array e acesso aleatório. Diferentemente de um iterador de `vector`, um iterador de `list` permanece apontando para o mesmo elemento mesmo após itens serem inseridos ou removidos de um container.

O template de classe `list` tem alguns métodos orientados a lista além dos que vêm com sequências e containers reversíveis. A Tabela 16.9 lista muitos deles. O parâmetro de template `Alloc` é um que normalmente não precisa se preocupar porque tem um valor padrão.

**Tabela 16.9 — Alguns Métodos de `list`**

| Função | Descrição |
|---|---|
| `void merge(list<T, Alloc>& x)` | Funde a lista `x` com a lista invocante. Ambas as listas devem estar ordenadas. A lista ordenada resultante fica na lista invocante, e `x` fica vazia. Complexidade de tempo linear. |
| `void remove(const T & val)` | Remove todas as instâncias de `val` da lista. Complexidade de tempo linear. |
| `void sort()` | Ordena a lista usando o operador `<`; a complexidade é `NlogN` para `N` elementos. |
| `void splice(iterator pos, list<T, Alloc> x)` | Insere o conteúdo da lista `x` antes da posição `pos`, e `x` fica vazia. Complexidade de tempo constante. |
| `void unique()` | Recolhe cada grupo consecutivo de valores iguais para um único elemento. Complexidade de tempo linear. |

A Listagem 16.12 ilustra esses métodos, juntamente com o método `insert()`, que vem com todas as classes STL que modelam sequências.

**Listagem 16.12 — lista.cpp**

```cpp
// lista.cpp -- usando uma list
#include <iostream>
#include <list>
#include <iterator>
#include <algorithm>
void exibirInt(int n) {std::cout << n << " ";}
int main()
{
    using namespace std;
    list<int> um(5, 2); // list de 5 doses de 2
    int elementos[5] = {1,2,4,8, 6};
    list<int> dois;
    dois.insert(dois.begin(), elementos, elementos + 5);
    int mais[6] = {6, 4, 2, 4, 6, 5};
    list<int> tres(dois);
    tres.insert(tres.end(), mais, mais + 6);
    cout << "Lista um: ";
    for_each(um.begin(), um.end(), exibirInt);
    cout << endl << "Lista dois: ";
    for_each(dois.begin(), dois.end(), exibirInt);
    cout << endl << "Lista três: ";
    for_each(tres.begin(), tres.end(), exibirInt);
    tres.remove(2);
    cout << endl << "Lista três sem 2s: ";
    for_each(tres.begin(), tres.end(), exibirInt);
    tres.splice(tres.begin(), um);
    cout << endl << "Lista três após splice: ";
    for_each(tres.begin(), tres.end(), exibirInt);
    cout << endl << "Lista um: ";
    for_each(um.begin(), um.end(), exibirInt);
    tres.unique();
    cout << endl << "Lista três após unique: ";
    for_each(tres.begin(), tres.end(), exibirInt);
    tres.sort();
    tres.unique();
    cout << endl << "Lista três após sort & unique: ";
    for_each(tres.begin(), tres.end(), exibirInt);
    dois.sort();
    tres.merge(dois);
    cout << endl << "Dois ordenado fundido em três: ";
    for_each(tres.begin(), tres.end(), exibirInt);
    cout << endl;
    return 0;
}
```

Aqui está a saída do programa da Listagem 16.12:

```
Lista um: 2 2 2 2 2
Lista dois: 1 2 4 8 6
Lista três: 1 2 4 8 6 6 4 2 4 6 5
Lista três sem 2s: 1 4 8 6 6 4 4 6 5
Lista três após splice: 2 2 2 2 2 1 4 8 6 6 4 4 6 5
Lista um:
Lista três após unique: 2 1 4 8 6 4 6 5
Lista três após sort & unique: 1 2 4 5 6 8
Dois ordenado fundido em três: 1 1 2 2 4 4 5 6 6 8 8
```

**Notas do Programa**

O programa da Listagem 16.12 usa o algoritmo `for_each()` e uma função `exibirInt()` para exibir as listas. Com C++11, você poderia usar o laço `for` baseado em intervalo em vez disso:

```cpp
for (auto x : tres) cout << x << " ";
```

A principal diferença entre `insert()` e `splice()` é que `insert()` insere uma cópia do intervalo original no destino, enquanto `splice()` move o intervalo original para o destino. Assim, após o conteúdo de `um` ser emendado em `tres`, `um` fica vazio. O método `splice()` deixa os iteradores válidos. Ou seja, se você definiu um iterador específico para apontar para um elemento em `um`, esse iterador ainda aponta para o mesmo elemento após `splice()` o realocar em `tres`.

Observe que `unique()` só reduz valores iguais adjacentes a um único valor. Após o programa executar `tres.unique()`, `tres` ainda contém dois quatros e dois seis que não eram adjacentes. Mas aplicar `sort()` e depois `unique()` limita cada valor a uma única aparição.

Há uma função `sort()` não-membro (Listagem 16.9), mas ela requer iteradores de acesso aleatório. Como a contrapartida de inserção rápida é abrir mão do acesso aleatório, você não pode usar a função `sort()` não-membro com uma list. Portanto, a classe inclui uma versão membro que funciona dentro das restrições da classe.

**A Caixa de Ferramentas de `list`**

Os métodos de `list` formam uma caixa de ferramentas útil. Suponha, por exemplo, que você tenha duas listas de correspondência para organizar. Você poderia ordenar cada lista, fundi-las e usar `unique()` para remover entradas múltiplas.

Os métodos `sort()`, `merge()` e `unique()` também têm cada um uma versão que aceita um argumento adicional para especificar uma função alternativa a ser usada para comparar elementos. Da mesma forma, o método `remove()` tem uma versão com um argumento adicional que especifica uma função usada para determinar se um elemento é removido. Esses argumentos são exemplos de funções predicado, um tópico ao qual retornaremos mais tarde.

### `forward_list` (C++11)

O C++11 adiciona `forward_list` como classe de container. Essa classe implementa uma lista simplesmente encadeada (singly linked list). Nesse tipo de lista, cada item está ligado apenas ao próximo item, mas não ao item precedente. Portanto, a classe requer apenas um iterador de avanço, não um bidirecional. Assim, diferentemente de `vector` e `list`, `forward_list` não é um container reversível. Comparado a `list`, `forward_list` é mais simples, mais compacto, mas com menos funcionalidades.

### `queue`

O template de classe `queue` (declarado no arquivo de cabeçalho `queue`) é uma classe adaptadora. Assim como o template `ostream_iterator` é um adaptador que permite a um fluxo de saída usar a interface de iterador, o template `queue` permite que uma classe subjacente (`deque`, por padrão) exiba a interface típica de uma fila.

O template `queue` é mais restritivo do que `deque`. Ele não só não permite acesso aleatório a elementos de uma fila, como a classe `queue` não permite sequer iterar por uma fila. Em vez disso, limita-o às operações básicas que definem uma fila. Você pode adicionar um elemento ao final de uma fila, remover um elemento da frente de uma fila, ver os valores dos elementos do início e fim, verificar o número de elementos e testar se uma fila está vazia. A Tabela 16.10 lista essas operações.

Observe que `pop()` é um método de remoção de dados, não de recuperação de dados. Se você quiser usar um valor de uma fila, primeiro use `front()` para recuperar o valor e depois use `pop()` para removê-lo da fila.

**Tabela 16.10 — Operações de `queue`**

| Método | Descrição |
|---|---|
| `bool empty() const` | Retorna `true` se a fila estiver vazia e `false` caso contrário. |
| `size_type size() const` | Retorna o número de elementos na fila. |
| `T& front()` | Retorna uma referência ao elemento na frente da fila. |
| `T& back()` | Retorna uma referência ao elemento no fundo da fila. |
| `void push(const T& x)` | Insere `x` no fundo da fila. |
| `void pop()` | Remove o elemento na frente da fila. |

### `priority_queue`

O template de classe `priority_queue` (declarado no arquivo de cabeçalho `queue`) é outra classe adaptadora. Ele suporta as mesmas operações que `queue`. A principal diferença entre os dois é que com `priority_queue`, o maior item é movido para a frente da fila. Uma diferença interna é que a classe subjacente padrão é `vector`. Você pode alterar a comparação usada para determinar o que chega ao topo da fila fornecendo um argumento de construtor opcional:

```cpp
priority_queue<int> pq1; // versão padrão
priority_queue<int> pq2(greater<int>); // usa greater<int> para ordenar
```

A função `greater<>()` é um objeto função predefinido, discutido adiante neste capítulo.

### `stack`

Como `queue`, `stack` (declarado no arquivo de cabeçalho `stack`) é uma classe adaptadora. Ela dá a uma classe subjacente (`vector`, por padrão) a interface típica de pilha.

O template `stack` é mais restritivo do que `vector`. Ele não só não permite acesso aleatório a elementos de uma pilha, como a classe `stack` não permite sequer iterar por uma pilha. Em vez disso, limita-o às operações básicas que definem uma pilha. Você pode empilhar um valor no topo de uma pilha, desempilhar um elemento do topo de uma pilha, ver o valor no topo de uma pilha, verificar o número de elementos e testar se a pilha está vazia. A Tabela 16.11 lista essas operações.

**Tabela 16.11 — Operações de `stack`**

| Método | Descrição |
|---|---|
| `bool empty() const` | Retorna `true` se a pilha estiver vazia e `false` caso contrário. |
| `size_type size() const` | Retorna o número de elementos na pilha. |
| `T& top()` | Retorna uma referência ao elemento no topo da pilha. |
| `void push(const T& x)` | Insere `x` no topo da pilha. |
| `void pop()` | Remove o elemento do topo da pilha. |

Assim como com `queue`, se você quiser usar um valor de uma pilha, primeiro use `top()` para recuperar o valor e depois use `pop()` para removê-lo da pilha.

### `array` (C++11)

O template de classe `array`, introduzido no Capítulo 4 e definido no arquivo de cabeçalho `array`, não é um container STL porque tem tamanho fixo. Assim, operações que redimensionariam um container, como `push_back()` e `insert()`, não são definidas para `array`. Mas as funções-membro que fazem sentido, como `operator[]()` e `at()`, são fornecidas. E você pode usar muitos algoritmos STL padrão, como `copy()` e `for_each()`, com objetos `array`.

---

## Containers Associativos

Um *container associativo* (associative container) é outro refinamento do conceito de container. Um container associativo associa um valor com uma chave e usa a chave para encontrar o valor. Por exemplo, os valores poderiam ser estruturas representando informações de empregados, como nome, endereço, número de escritório, telefones residencial e profissional, plano de saúde, e assim por diante, e a chave poderia ser um número único de empregado. Para obter as informações do empregado, um programa usaria a chave para localizar a estrutura do empregado. Lembre-se de que para um container `X`, em geral, a expressão `X::value_type` indica o tipo de valor armazenado no container. Para um container associativo, a expressão `X::key_type` indica o tipo usado para a chave.

O ponto forte de um container associativo é que ele fornece acesso rápido a seus elementos. Como uma sequência, um container associativo permite que você insira novos elementos; entretanto, você não pode especificar um local específico para os elementos inseridos. A razão é que um container associativo geralmente tem um algoritmo específico para determinar onde colocar os dados de modo que possa recuperar informações rapidamente.

Containers associativos são tipicamente implementados usando alguma forma de árvore. Uma *árvore* (tree) é uma estrutura de dados na qual um nó raiz é ligado a um ou dois outros nós, cada um dos quais é ligado a um ou dois nós, formando assim uma estrutura de ramificação. O aspecto de nó torna relativamente simples adicionar ou remover um novo item de dados, assim como ocorre com uma lista encadeada. Mas comparado a uma lista, uma árvore oferece tempos de pesquisa muito mais rápidos.

A STL fornece quatro containers associativos: `set`, `multiset`, `map` e `multimap`. Os dois primeiros tipos são definidos no arquivo de cabeçalho `set`, e os dois últimos tipos são definidos no arquivo de cabeçalho `map`.

O mais simples do grupo é `set`; o tipo de valor é o mesmo que o tipo de chave, e as chaves são únicas, significando que não há mais de uma instância de uma chave em um conjunto. De fato, para `set`, o valor é a chave. O tipo `multiset` é como o tipo `set`, exceto que pode ter mais de um valor com a mesma chave. Por exemplo, se o tipo de chave e valor for `int`, um objeto `multiset` poderia conter, digamos, 1, 2, 2, 2, 3, 5, 7 e 7.

Para o tipo `map`, o tipo de valor é diferente do tipo de chave, e as chaves são únicas, com apenas um valor por chave. O tipo `multimap` é semelhante a `map`, exceto que uma chave pode ser associada a múltiplos valores.

### Um Exemplo de `set`

O `set` da STL modela vários conceitos. É um conjunto associativo, é reversível, está ordenado, e as chaves são únicas, portanto não pode conter mais de um valor qualquer. Como `vector` e `list`, `set` usa um parâmetro de template para fornecer o tipo armazenado:

```cpp
set<string> A; // um conjunto de objetos string
```

Um segundo argumento de template opcional pode ser usado para indicar uma função de comparação ou objeto a ser usado para ordenar a chave. Por padrão, o template `less<>` (discutido adiante) é usado.

Considere o seguinte código:

```cpp
const int N = 6;
string s1[N] = {"buffoon", "thinkers", "for", "heavy", "can", "for"};
set<string> A(s1, s1 + N); // inicializar conjunto A usando um intervalo do array
ostream_iterator<string, char> saida(cout, " ");
copy(A.begin(), A.end(), saida);
```

Como outros containers, `set` tem um construtor que recebe um intervalo de iteradores como argumentos. Isso fornece uma maneira simples de inicializar um conjunto com o conteúdo de um array. Lembre-se de que o último elemento de um intervalo está uma posição past-the-end, e `s1 + N` aponta para uma posição além do final do array `s1`. A saída desse fragmento de código ilustra que as chaves são únicas (a string `"for"` aparece duas vezes no array, mas uma vez no conjunto) e que o conjunto está ordenado:

```
buffoon can for heavy thinkers
```

A matemática define algumas operações padrão para conjuntos. Por exemplo, a *união* (union) de dois conjuntos é um conjunto que combina o conteúdo dos dois conjuntos. Se um valor específico é comum a ambos os conjuntos, ele aparece apenas uma vez na união por causa da característica de chave única. A *interseção* (intersection) de dois conjuntos é um conjunto que consiste nos elementos que são comuns a ambos os conjuntos. A *diferença* (difference) entre dois conjuntos é o primeiro conjunto menos os elementos comuns a ambos os conjuntos.

A STL fornece algoritmos que suportam essas operações. São funções gerais em vez de métodos, portanto não estão restritas a objetos `set`. Entretanto, todos os objetos `set` automaticamente satisfazem a pré-condição para usar esses algoritmos — ou seja, que o container esteja ordenado. A função `set_union()` recebe cinco iteradores como argumentos. Os dois primeiros definem um intervalo em um conjunto, os dois seguintes definem um intervalo em um segundo conjunto, e o iterador final identifica um local para o qual copiar o conjunto resultante. Por exemplo, para exibir a união dos conjuntos `A` e `B`, você pode usar:

```cpp
set_union(A.begin(), A.end(), B.begin(), B.end(),
    ostream_iterator<string, char>(cout, " "));
```

Suponha que você queira colocar o resultado em um conjunto `C` em vez de exibi-lo. Nesse caso, você usaria o `insert_iterator` discutido anteriormente para converter o processo de cópia em inserção e para escrever em `C`:

```cpp
set_union(A.begin(), A.end(), B.begin(), B.end(),
    insert_iterator<set<string> >(C, C.begin()));
```

As funções `set_intersection()` e `set_difference()` encontram a interseção de conjunto e a diferença de conjunto de dois conjuntos, e têm a mesma interface que `set_union()`.

Dois métodos úteis de `set` são `lower_bound()` e `upper_bound()`. O método `lower_bound()` recebe um valor do tipo de chave como argumento e retorna um iterador que aponta para o primeiro membro do conjunto que não é menor que o argumento de chave. Da mesma forma, o método `upper_bound()` recebe uma chave como argumento e retorna um iterador que aponta para o primeiro membro do conjunto que é maior que o argumento de chave. Por exemplo, se você tivesse um conjunto de strings, poderia usar esses métodos para identificar um intervalo abrangendo todas as strings de `"b"` até `"f"` no conjunto.

Como a ordenação determina onde as adições ao conjunto vão, a classe tem métodos de inserção que apenas especificam o material a ser inserido, sem especificar uma posição. Se `A` e `B` são conjuntos de strings, por exemplo, você pode usar:

```cpp
string s("tennis");
A.insert(s);              // inserir um valor
B.insert(A.begin(), A.end()); // inserir um intervalo
```

A Listagem 16.13 ilustra esses usos de conjuntos.

**Listagem 16.13 — opset.cpp**

```cpp
// opset.cpp -- algumas operações de set
#include <iostream>
#include <string>
#include <set>
#include <algorithm>
#include <iterator>
int main()
{
    using namespace std;
    const int N = 6;
    string s1[N] = {"buffoon", "thinkers", "for", "heavy", "can", "for"};
    string s2[N] = {"metal", "any", "food", "elegant", "deliver","for"};
    set<string> A(s1, s1 + N);
    set<string> B(s2, s2 + N);
    ostream_iterator<string, char> saida(cout, " ");
    cout << "Conjunto A: ";
    copy(A.begin(), A.end(), saida);
    cout << endl;
    cout << "Conjunto B: ";
    copy(B.begin(), B.end(), saida);
    cout << endl;
    cout << "União de A e B:\n";
    set_union(A.begin(), A.end(), B.begin(), B.end(), saida);
    cout << endl;
    cout << "Interseção de A e B:\n";
    set_intersection(A.begin(), A.end(), B.begin(), B.end(), saida);
    cout << endl;
    cout << "Diferença de A e B:\n";
    set_difference(A.begin(), A.end(), B.begin(), B.end(), saida);
    cout << endl;
    set<string> C;
    cout << "Conjunto C:\n";
    set_union(A.begin(), A.end(), B.begin(), B.end(),
        insert_iterator<set<string> >(C, C.begin()));
    copy(C.begin(), C.end(), saida);
    cout << endl;
    string s3("grungy");
    C.insert(s3);
    cout << "Conjunto C após inserção:\n";
    copy(C.begin(), C.end(), saida);
    cout << endl;
    cout << "Exibindo um intervalo:\n";
    copy(C.lower_bound("ghost"), C.upper_bound("spook"), saida);
    cout << endl;
    return 0;
}
```

Aqui está a saída do programa da Listagem 16.13:

```
Conjunto A: buffoon can for heavy thinkers
Conjunto B: any deliver elegant food for metal
União de A e B:
any buffoon can deliver elegant food for heavy metal thinkers
Interseção de A e B:
for
Diferença de A e B:
buffoon can heavy thinkers
Conjunto C:
any buffoon can deliver elegant food for heavy metal thinkers
Conjunto C após inserção:
any buffoon can deliver elegant food for grungy heavy metal thinkers
Exibindo um intervalo:
grungy heavy metal
```

### Um Exemplo de `multimap`

Como `set`, `multimap` é um container associativo reversível e ordenado. Entretanto, com `multimap`, o tipo de chave é diferente do tipo de valor, e um objeto `multimap` pode ter mais de um valor associado a uma chave particular.

A declaração básica de `multimap` especifica o tipo de chave e o tipo de valor armazenado como argumentos de template. Por exemplo, a seguinte declaração cria um objeto `multimap` que usa `int` como tipo de chave e `string` como tipo de valor armazenado:

```cpp
multimap<int,string> codigos;
```

Um terceiro argumento de template opcional pode ser usado para indicar uma função de comparação ou objeto a ser usado para ordenar a chave. Por padrão, o template `less<>` é usado com o tipo de chave como parâmetro.

Para manter as informações juntas, o tipo de valor real combina o tipo de chave e o tipo de dado em um único par. Para fazer isso, a STL usa um template de classe `pair<class T, class U>` para armazenar dois tipos de valores em um único objeto. Se `tipochave` for o tipo de chave e `tipodado` for o tipo de dado armazenado, o tipo de valor é `pair<const tipochave, tipodado>`. Por exemplo, o tipo de valor para o objeto `codigos` declarado anteriormente é `pair<const int, string>`.

Suponha que você queira armazenar nomes de cidades, usando o código de área como chave. Isso se encaixa na declaração de `codigos`, que usa um `int` para chave e uma `string` como tipo de dado. Uma abordagem é criar um `pair` e depois inseri-lo no objeto `multimap`:

```cpp
pair<const int, string> item(213, "Los Angeles");
codigos.insert(item);
```

Ou você pode criar um objeto `pair` anônimo e inseri-lo em uma única instrução:

```cpp
codigos.insert(pair<const int, string> (213, "Los Angeles"));
```

Como os itens são ordenados por chave, não há necessidade de identificar um local de inserção.

Dado um objeto `pair`, você pode acessar os dois componentes usando os membros `first` e `second`:

```cpp
pair<const int, string> item(213, "Los Angeles");
cout << item.first << ' ' << item.second << endl;
```

E para obter informações sobre um objeto `multimap`? A função-membro `count()` recebe uma chave como argumento e retorna o número de itens que têm essa chave. Os métodos `lower_bound()` e `upper_bound()` recebem uma chave e funcionam como fazem para `set`. Além disso, a função-membro `equal_range()` recebe uma chave como argumento e retorna iteradores representando o intervalo que corresponde a essa chave. Para retornar dois valores, o método os empacota em um objeto `pair`, desta vez com ambos os argumentos de template sendo o tipo iterador. Por exemplo, o seguinte imprimiria uma lista de cidades no objeto `codigos` com código de área 718:

```cpp
pair<multimap<TipoChave, string>::iterator,
   multimap<TipoChave, string>::iterator> intervalo
           = codigos.equal_range(718);
cout << "Cidades com código de área 718:\n";
multimap<TipoChave, string>::iterator it;
for (it = intervalo.first; it != intervalo.second; ++it)
  cout << (*it).second << endl;
```

Declarações como essas ajudaram a motivar a funcionalidade de dedução automática de tipo do C++11, que permite simplificar o código da seguinte forma:

```cpp
auto intervalo = codigos.equal_range(718);
cout << "Cidades com código de área 718:\n";
for (auto it = intervalo.first; it != intervalo.second; ++it)
  cout << (*it).second << endl;
```

A Listagem 16.14 demonstra a maioria dessas técnicas. Ela também usa `typedef` para simplificar a escrita de parte do código.

**Listagem 16.14 — multmapa.cpp**

```cpp
// multmapa.cpp -- usar um multimap
#include <iostream>
#include <string>
#include <map>
#include <algorithm>
typedef int TipoChave;
typedef std::pair<const TipoChave, std::string> Par;
typedef std::multimap<TipoChave, std::string> MapaCodigos;
int main()
{
    using namespace std;
    MapaCodigos codigos;
    codigos.insert(Par(415, "San Francisco"));
    codigos.insert(Par(510, "Oakland"));
    codigos.insert(Par(718, "Brooklyn"));
    codigos.insert(Par(718, "Staten Island"));
    codigos.insert(Par(415, "San Rafael"));
    codigos.insert(Par(510, "Berkeley"));
    cout << "Número de cidades com código de área 415: "
       << codigos.count(415) << endl;
    cout << "Número de cidades com código de área 718: "
       << codigos.count(718) << endl;
    cout << "Número de cidades com código de área 510: "
       << codigos.count(510) << endl;
    cout << "Código de Área Cidade\n";
    MapaCodigos::iterator it;
    for (it = codigos.begin(); it != codigos.end(); ++it)
      cout << " " << (*it).first << " "
         << (*it).second << endl;
    pair<MapaCodigos::iterator, MapaCodigos::iterator> intervalo
       = codigos.equal_range(718);
    cout << "Cidades com código de área 718:\n";
    for (it = intervalo.first; it != intervalo.second; ++it)
      cout << (*it).second << endl;
    return 0;
}
```

Aqui está a saída do programa da Listagem 16.14:

```
Número de cidades com código de área 415: 2
Número de cidades com código de área 718: 2
Número de cidades com código de área 510: 2
Código de Área Cidade
   415 San Francisco
   415 San Rafael
   510 Oakland
   510 Berkeley
   718 Brooklyn
   718 Staten Island
Cidades com código de área 718:
Brooklyn
Staten Island
```

---

## Containers Associativos Não-Ordenados (C++11)

Um *container associativo não-ordenado* (unordered associative container) é ainda outro refinamento do conceito de container. Como um container associativo, um container associativo não-ordenado associa um valor com uma chave e usa a chave para encontrar o valor. A diferença subjacente é que containers associativos são baseados em estruturas de árvore, enquanto containers associativos não-ordenados são baseados em outra forma de estrutura de dados chamada *tabela hash* (hash table). A intenção é fornecer containers para os quais adicionar e excluir elementos é relativamente rápido e para os quais existem algoritmos de busca eficientes. Os quatro containers associativos não-ordenados são chamados `unordered_set`, `unordered_multiset`, `unordered_map` e `unordered_multimap`. O Apêndice G examina um pouco mais essas adições.

---

## Objetos Função (Functores)

Muitos algoritmos STL usam *objetos função* (function objects), também conhecidos como *functores* (functors). Um funtor é qualquer objeto que pode ser usado com `()` à maneira de uma função. Isso inclui nomes de funções normais, ponteiros para funções e objetos de classe para os quais o operador `()` é sobrecarregado — ou seja, classes para as quais a função de aparência peculiar `operator()()` está definida. Por exemplo, você poderia definir uma classe assim:

```cpp
class Linear
{
private:
    double inclinacao;
    double y0;
public:
    Linear(double sl_ = 1, double y_ = 0)
       : inclinacao(sl_), y0(y_) {}
    double operator()(double x) {return y0 + inclinacao * x; }
};
```

O operador `()` sobrecarregado então permite que você use objetos `Linear` como funções:

```cpp
Linear f1;
Linear f2(2.5, 10.0);
double y1 = f1(12.5); // lado direito é f1.operator()(12.5)
double y2 = f2(0.4);
```

Aqui, `y1` é calculado usando a expressão `0 + 1 * 12.5`, e `y2` é calculado usando a expressão `10.0 + 2.5 * 0.4`.

Lembra-se da função `for_each`? Ela aplicava uma função especificada a cada membro de um intervalo. Em geral, o terceiro argumento poderia ser um funtor, não apenas uma função regular. Na verdade, isso levanta uma questão: como você declara o terceiro argumento? Você não pode declará-lo como um ponteiro para função porque um ponteiro para função especifica o tipo do argumento. Como um container pode conter praticamente qualquer tipo, você não sabe antecipadamente qual tipo de argumento específico deve ser usado. A STL resolve esse problema usando templates. O protótipo de `for_each` se parece com:

```cpp
template<class InputIterator, class Function>
Function for_each(InputIterator first, InputIterator last, Function f);
```

Isso torna o identificador `MostrarResenha` do tipo `void (*)(const Resenha &)`, então esse é o tipo atribuído ao argumento de template `Function`. Com uma chamada de função diferente, o argumento `Function` poderia representar um tipo de classe que tem um operador `()` sobrecarregado. Em última instância, o código de `for_each()` terá uma expressão usando `f()`. No exemplo de `MostrarResenha()`, `f` é um ponteiro para uma função, e `f()` invoca a função. Se o argumento final de `for_each()` for um objeto, então `f()` se torna o objeto que invoca seu operador `()` sobrecarregado.

### Conceitos de Funtor

Assim como a STL define conceitos para containers e iteradores, ela define conceitos de funtor:

- Um *gerador* (generator) é um funtor que pode ser chamado sem argumentos.
- Uma *função unária* (unary function) é um funtor que pode ser chamado com um argumento.
- Uma *função binária* (binary function) é um funtor que pode ser chamado com dois argumentos.

Por exemplo, o funtor fornecido a `for_each()` deve ser uma função unária porque é aplicado a um elemento de container por vez.

Claro, esses conceitos vêm com refinamentos:

- Uma função unária que retorna um valor `bool` é um *predicado* (predicate).
- Uma função binária que retorna um valor `bool` é um *predicado binário* (binary predicate).

Várias funções da STL requerem argumentos predicado ou predicado binário. Por exemplo, a Listagem 16.9 usa uma versão de `sort()` que recebe um predicado binário como seu terceiro argumento:

```cpp
bool piorQue(const Resenha & r1, const Resenha & r2);
...
sort(livros.begin(), livros.end(), piorQue);
```

O template `list` tem um membro `remove_if()` que recebe um predicado como argumento. Ele aplica o predicado a cada membro no intervalo indicado, removendo os elementos para os quais o predicado retorna `true`. Por exemplo, o seguinte código removeria todos os elementos maiores que 100 da lista `pontuacoes`:

```cpp
bool grandeDemais(int n){ return n > 100; }
list<int> pontuacoes;
...
pontuacoes.remove_if(grandeDemais);
```

Por sinal, esse último exemplo mostra onde um funtor de classe pode ser útil. Suponha que você queira remover todo valor maior que 200 de uma segunda lista. Seria bom se você pudesse passar o valor de corte para `grandeDemais()` como um segundo argumento para poder usar a função com valores diferentes, mas um predicado só pode ter um argumento. Se, no entanto, você projetar uma classe `GrandeDemais`, pode usar membros de classe em vez de argumentos de função para transmitir informações adicionais:

```cpp
template<class T>
class GrandeDemais
{
private:
    T corte;
public:
    GrandeDemais(const T & t) : corte(t) {}
    bool operator()(const T & v) { return v > corte; }
};
```

Aqui um valor (`v`) é passado como argumento de função, e o segundo argumento (`corte`) é definido pelo construtor de classe. Dado essa definição, você pode inicializar diferentes objetos `GrandeDemais` para valores de corte diferentes a serem usados em chamadas para `remove_if()`. A Listagem 16.15 ilustra a técnica.

**Listagem 16.15 — funtor.cpp**

```cpp
// funtor.cpp -- usando um funtor
#include <iostream>
#include <list>
#include <iterator>
#include <algorithm>
template<class T> // classe funtor define operator()()
class GrandeDemais
{
private:
    T corte;
public:
    GrandeDemais(const T & t) : corte(t) {}
    bool operator()(const T & v) { return v > corte; }
};
void exibirInt(int n) {std::cout << n << " ";}
int main()
{
    using std::list;
    using std::cout;
    using std::endl;
    GrandeDemais<int> f100(100); // limite = 100
    int valores[10] = {50, 100, 90, 180, 60, 210, 415, 88, 188, 201};
    list<int> tadatada(valores, valores + 10); // construtor de intervalo
    list<int> etcetera(valores, valores + 10);
    // C++11 pode usar o seguinte em vez disso
    // list<int> tadatada = {50, 100, 90, 180, 60, 210, 415, 88, 188, 201};
    // list<int> etcetera {50, 100, 90, 180, 60, 210, 415, 88, 188, 201};
    cout << "Listas originais:\n";
    for_each(tadatada.begin(), tadatada.end(), exibirInt);
    cout << endl;
    for_each(etcetera.begin(), etcetera.end(), exibirInt);
    cout << endl;
    tadatada.remove_if(f100); // usar um objeto função nomeado
    etcetera.remove_if(GrandeDemais<int>(200)); // construir um objeto função
    cout <<"Listas podadas:\n";
    for_each(tadatada.begin(), tadatada.end(), exibirInt);
    cout << endl;
    for_each(etcetera.begin(), etcetera.end(), exibirInt);
    cout << endl;
    return 0;
}
```

Um funtor (`f100`) é um objeto declarado, e o segundo (`GrandeDemais<int>(200)`) é um objeto anônimo criado por uma chamada de construtor. Aqui está a saída do programa da Listagem 16.15:

```
Listas originais:
50 100 90 180 60 210 415 88 188 201
50 100 90 180 60 210 415 88 188 201
Listas podadas:
50 100 90 60 88
50 100 90 180 60 88 188
```

Suponha que você já tenha uma função template com dois argumentos:

```cpp
template <class T>
bool grandeDemais(const T & val, const T & lim)
{
    return val > lim;
}
```

Você pode usar uma classe para convertê-la em um objeto função de um argumento:

```cpp
template<class T>
class GrandeDemais2
{
private:
    T corte;
public:
    GrandeDemais2(const T & t) : corte(t) {}
    bool operator()(const T & v) { return grandeDemais<T>(v, corte); }
};
```

Ou seja, você pode usar o seguinte:

```cpp
GrandeDemais2<int> tB100(100);
int x;
cin >> x;
if (tB100(x)) // mesmo que if (grandeDemais(x,100))
    ...
```

Portanto, a chamada `tB100(x)` é a mesma que `grandeDemais(x,100)`, mas a função de dois argumentos é convertida em um objeto função de um argumento, com o segundo argumento sendo usado para construir o objeto função. Em resumo, o funtor de classe `GrandeDemais2` é um adaptador de função que adapta uma função a uma interface diferente.

Como observado na listagem, a funcionalidade de lista inicializadora do C++11 simplifica a inicialização. Você pode substituir:

```cpp
int valores[10] = {50, 100, 90, 180, 60, 210, 415, 88, 188, 201};
list<int> tadatada(valores, valores + 10);
list<int> etcetera(valores, valores + 10);
```

por:

```cpp
list<int> tadatada = {50, 100, 90, 180, 60, 210, 415, 88, 188, 201};
list<int> etcetera {50, 100, 90, 180, 60, 210, 415, 88, 188, 201};
```

### Functores Predefinidos

A STL define vários functores elementares. Eles realizam ações como adicionar dois valores e comparar dois valores quanto à igualdade. São fornecidos para ajudar a suportar funções STL que recebem funções como argumentos. Por exemplo, considere a função `transform()`. Ela tem duas versões. A primeira versão recebe quatro argumentos. Os dois primeiros são iteradores que especificam um intervalo em um container. O terceiro é um iterador que especifica onde copiar o resultado. O final é um funtor que é aplicado a cada elemento no intervalo para produzir cada novo elemento no resultado. Por exemplo, considere o seguinte:

```cpp
const int LIM = 5;
double arr1[LIM] = {36, 39, 42, 45, 48};
vector<double> gr8(arr1, arr1 + LIM);
ostream_iterator<double, char> saida(cout, " ");
transform(gr8.begin(), gr8.end(), saida, sqrt);
```

Esse código calcula a raiz quadrada de cada elemento e envia os valores resultantes para o fluxo de saída. O iterador de destino pode estar no intervalo original. A segunda versão usa uma função que recebe dois argumentos, aplicando a função a um elemento de cada um de dois intervalos. Ela recebe um argumento adicional, que vem em terceiro lugar na ordem, identificando o início do segundo intervalo. Por exemplo, se `m8` fosse um segundo objeto `vector<double>` e `media(double, double)` retornasse a média de dois valores, o seguinte produziria a saída da média de cada par de valores de `gr8` e `m8`:

```cpp
transform(gr8.begin(), gr8.end(), m8.begin(), saida, media);
```

Agora suponha que você queira adicionar os dois arrays. Você não pode usar `+` como argumento porque, para o tipo `double`, `+` é um operador embutido, não uma função. A STL já tem o template. O arquivo de cabeçalho `functional` (antes `function.h`) define vários objetos função de template de classe, incluindo um chamado `plus<>()`.

Usar a classe `plus<>` para adição comum é possível, embora um pouco desajeitado:

```cpp
#include <functional>
...
plus<double> adicionar; // criar um objeto plus<double>
double y = adicionar(2.2, 3.4); // usando plus<double>::operator()()
```

Mas isso torna fácil fornecer um objeto função como argumento:

```cpp
transform(gr8.begin(), gr8.end(), m8.begin(), saida, plus<double>());
```

Aqui, em vez de criar um objeto nomeado, o código usa o construtor `plus<double>` para construir um funtor para fazer a adição.

A STL fornece equivalentes de funtor para todos os operadores aritméticos, relacionais e lógicos embutidos. A Tabela 16.12 mostra os nomes para esses equivalentes de funtor.

**Tabela 16.12 — Operadores e Equivalentes de Funtor**

| Operador | Equivalente de Funtor |
|---|---|
| `+` | `plus` |
| `-` | `minus` |
| `*` | `multiplies` |
| `/` | `divides` |
| `%` | `modulus` |
| `-` (unário) | `negate` |
| `==` | `equal_to` |
| `!=` | `not_equal_to` |
| `>` | `greater` |
| `<` | `less` |
| `>=` | `greater_equal` |
| `<=` | `less_equal` |
| `&&` | `logical_and` |
| `\|\|` | `logical_or` |
| `!` | `logical_not` |

> **Cuidado:** Implementações C++ mais antigas usam o nome de funtor `times` em vez de `multiplies`.

### Functores Adaptáveis e Adaptadores de Função

Os functores predefinidos da Tabela 16.12 são todos *adaptáveis* (adaptable). A STL tem cinco conceitos relacionados: geradores adaptáveis, funções unárias adaptáveis, funções binárias adaptáveis, predicados adaptáveis e predicados binários adaptáveis.

O que torna um funtor adaptável é que ele carrega membros `typedef` identificando seus tipos de argumento e tipo de retorno. Os membros são chamados `result_type`, `first_argument_type` e `second_argument_type`, e representam o que seus nomes sugerem. Por exemplo, o tipo de retorno de um objeto `plus<int>` é identificado como `plus<int>::result_type`, e isso seria um `typedef` para `int`.

A significância de um funtor ser adaptável é que ele pode então ser usado por objetos adaptadores de função, que assumem a existência desses membros `typedef`. De fato, a STL fornece classes adaptadoras de função que usam essas facilidades. Por exemplo, suponha que você queira multiplicar cada elemento do vetor `gr8` por 2.5. Isso requer usar a versão de `transform()` com um argumento de função unária. O funtor `multiplies()` pode fazer a multiplicação, mas é uma função binária. Portanto, você precisa de um adaptador de função que converta um funtor com dois argumentos em um com um argumento. A STL automatizou o processo com as classes `binder1st` e `binder2nd`, que convertem funções binárias adaptáveis em funções unárias adaptáveis.

Vamos olhar para `binder1st`. Suponha que você tenha um objeto função binária adaptável `f2()`. Você pode criar um objeto `binder1st` que vincula um valor particular, chamado `val`, a ser usado como o primeiro argumento para `f2()`:

```cpp
binder1st(f2, val) f1;
```

Então, invocar `f1(x)` com seu único argumento retorna o mesmo valor que invocar `f2()` com `val` como primeiro argumento e o argumento de `f1()` como segundo argumento. Ou seja, `f1(x)` é equivalente a `f2(val, x)`, exceto que é uma função unária em vez de binária.

A STL fornece a função `bind1st()` para simplificar o uso da classe `binder1st`. Você dá a ela o nome da função e o valor usado para construir um objeto `binder1st`, e ela retorna um objeto desse tipo. Por exemplo, você pode converter a função binária `multiplies()` em uma função unária que multiplica seu argumento por 2.5. Basta fazer isto:

```cpp
bind1st(multiplies<double>(), 2.5)
```

Assim, a solução para multiplicar cada elemento em `gr8` por 2.5 e exibir os resultados é esta:

```cpp
transform(gr8.begin(), gr8.end(), saida,
     bind1st(multiplies<double>(), 2.5));
```

A classe `binder2nd` é similar, exceto que atribui a constante ao segundo argumento em vez do primeiro. Ela tem uma função auxiliar chamada `bind2nd` que funciona analogamente a `bind1st`.

A Listagem 16.16 incorpora alguns dos exemplos recentes em um programa curto.

**Listagem 16.16 — adapfuntor.cpp**

```cpp
// adapfuntor.cpp -- usando adaptadores de função
#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <functional>
void Exibir(double);
const int LIM = 6;
int main()
{
    using namespace std;
    double arr1[LIM] = {28, 29, 30, 35, 38, 59};
    double arr2[LIM] = {63, 65, 69, 75, 80, 99};
    vector<double> gr8(arr1, arr1 + LIM);
    vector<double> m8(arr2, arr2 + LIM);
    cout.setf(ios_base::fixed);
    cout.precision(1);
    cout << "gr8:\t";
    for_each(gr8.begin(), gr8.end(), Exibir);
    cout << endl;
    cout << "m8: \t";
    for_each(m8.begin(), m8.end(), Exibir);
    cout << endl;
    vector<double> soma(LIM);
    transform(gr8.begin(), gr8.end(), m8.begin(), soma.begin(),
          plus<double>());
    cout << "soma:\t";
    for_each(soma.begin(), soma.end(), Exibir);
    cout << endl;
    vector<double> prod(LIM);
    transform(gr8.begin(), gr8.end(), prod.begin(),
          bind1st(multiplies<double>(), 2.5));
    cout << "prod:\t";
    for_each(prod.begin(), prod.end(), Exibir);
    cout << endl;
    return 0;
}
void Exibir(double v)
{
    std::cout.width(6);
    std::cout << v << ' ';
}
```

Aqui está a saída do programa da Listagem 16.16:

```
gr8:      28.0   29.0   30.0   35.0   38.0   59.0
m8:       63.0   65.0   69.0   75.0   80.0   99.0
soma:     91.0   94.0   99.0  110.0  118.0  158.0
prod:     70.0   72.5   75.0   87.5   95.0  147.5
```

O C++11 fornece uma alternativa a ponteiros de função e functores chamada *expressão lambda* (lambda expression), outro tópico discutido no Capítulo 18.

---

## Algoritmos

A STL contém muitas funções não-membro para trabalhar com containers. Você já viu algumas delas: `sort()`, `copy()`, `find()`, `for_each()`, `random_shuffle()`, `set_union()`, `set_intersection()`, `set_difference()` e `transform()`. Você provavelmente notou que elas apresentam o mesmo projeto geral, usando iteradores para identificar intervalos de dados a serem processados e para identificar para onde os resultados devem ir.

Há dois componentes genéricos principais para os projetos de funções de algoritmo. Primeiro, eles usam templates para fornecer tipos genéricos. Segundo, eles usam iteradores para fornecer uma representação genérica de acesso a dados em um container. Assim, a função `copy()` pode trabalhar com um container que mantém valores `double` em um array, com um container que mantém valores `string` em uma lista encadeada, ou com um container que armazena objetos definidos pelo usuário em uma estrutura de árvore. Como ponteiros são um caso especial de iteradores, funções STL como `copy()` podem ser usadas com arrays comuns.

### Grupos de Algoritmos

A STL divide a biblioteca de algoritmos em quatro grupos:

- Operações de sequência não-modificadoras (nonmodifying sequence operations)
- Operações de sequência mutantes (mutating sequence operations)
- Operações de ordenação e relacionadas (sorting and related operations)
- Operações numéricas generalizadas (generalized numeric operations)

Os três primeiros grupos são descritos no arquivo de cabeçalho `algorithm`, e o quarto grupo, sendo especificamente orientado para dados numéricos, obtém seu próprio arquivo de cabeçalho, chamado `numeric`.

Operações de sequência não-modificadoras operam em cada elemento em um intervalo. Essas operações deixam um container inalterado. Por exemplo, `find()` e `for_each()` pertencem a essa categoria.

Operações de sequência mutantes também operam em cada elemento em um intervalo. Como o nome sugere, entretanto, podem mutar, ou alterar, o conteúdo de um container. A mudança pode ser em valores ou na ordem em que os valores são armazenados. As funções `transform()`, `random_shuffle()` e `copy()` se enquadram nessa categoria.

Operações de ordenação e relacionadas incluem várias funções de ordenação (incluindo `sort()`) e uma variedade de outras funções, incluindo as operações de conjunto.

As operações numéricas incluem funções para somar o conteúdo de um intervalo, calcular o produto interno de dois containers, calcular somas parciais e calcular diferenças adjacentes. Tipicamente, essas são operações características de arrays, portanto `vector` é o container mais provável de ser usado com elas. O Apêndice G fornece um resumo completo dessas funções.

### Propriedades Gerais dos Algoritmos

Como você viu repetidamente neste capítulo, as funções STL trabalham com iteradores e intervalos de iteradores. O protótipo da função indica as suposições feitas sobre os iteradores. Por exemplo, a função `copy()` tem este protótipo:

```cpp
template<class InputIterator, class OutputIterator>
OutputIterator copy(InputIterator first, InputIterator last,
             OutputIterator result);
```

Como os identificadores `InputIterator` e `OutputIterator` são parâmetros de template, eles poderiam facilmente ter sido `T` e `U`. Entretanto, a documentação da STL usa os nomes de parâmetros de template para indicar o conceito que o parâmetro modela. Portanto, essa declaração diz que os parâmetros de intervalo devem ser iteradores de entrada ou melhores, e que o iterador indicando para onde o resultado vai deve ser um parâmetro de saída ou melhor.

Uma maneira de classificar algoritmos é com base em onde o resultado do algoritmo é colocado. Alguns algoritmos fazem seu trabalho no local, e outros criam cópias. Por exemplo, quando a função `sort()` termina, o resultado ocupa o mesmo local que os dados originais ocupavam. Portanto, `sort()` é um algoritmo *no-lugar* (in-place). A função `copy()`, entretanto, envia o resultado de seu trabalho para outro local, portanto é um algoritmo de *cópia* (copying). A função `transform()` pode fazer ambos.

Alguns algoritmos vêm em duas versões: uma versão no-lugar e uma versão de cópia. A convenção da STL é acrescentar `_copy` ao nome da versão de cópia. A última versão recebe um parâmetro adicional de iterador de saída para especificar o local para o qual copiar o resultado. Por exemplo, há uma função `replace()` que tem este protótipo:

```cpp
template<class ForwardIterator, class T>
void replace(ForwardIterator first, ForwardIterator last,
         const T& old_value, const T& new_value);
```

Ela substitui cada instância de `old_value` por `new_value`. Isso ocorre no lugar. A versão de cópia tem este protótipo:

```cpp
template<class InputIterator, class OutputIterator, class T>
OutputIterator replace_copy(InputIterator first, InputIterator last,
         OutputIterator result,
         const T& old_value, const T& new_value);
```

Desta vez os dados resultantes são copiados para um novo local, dado por `result`. Observe que `replace_copy()` tem um tipo de retorno `OutputIterator`. A convenção para algoritmos de cópia é que eles retornam um iterador apontando para o local uma posição além do último valor copiado.

Outra variação comum é que algumas funções têm uma versão que realiza uma ação condicionalmente, dependendo do resultado de aplicar uma função a um elemento de container. Essas versões tipicamente acrescentam `_if` ao nome da função. Por exemplo, `replace_if()` substitui um valor antigo por um novo se aplicar uma função ao valor antigo retornar `true`. Aqui está o protótipo:

```cpp
template<class ForwardIterator, class Predicate, class T>
void replace_if(ForwardIterator first, ForwardIterator last,
         Predicate pred, const T& new_value);
```

Há também uma versão chamada `replace_copy_if()`. Da mesma forma que `InputIterator`, `Predicate` é um nome de parâmetro de template e poderia facilmente ser chamado `T` ou `U`. No entanto, a STL escolhe usar `Predicate` para lembrar o usuário de que o argumento real deve ser um modelo do conceito `Predicate`.

---

## A STL e a Classe `string`

A classe `string`, embora não seja parte da STL, é projetada com a STL em mente. Por exemplo, ela tem membros `begin()`, `end()`, `rbegin()` e `rend()`. Assim, pode usar a interface STL. A Listagem 16.17 usa a STL para mostrar todas as permutações que você pode formar a partir das letras em uma palavra. Uma *permutação* (permutation) é um rearranjo da ordem dos elementos em um container. O algoritmo `next_permutation()` transforma o conteúdo de um intervalo para a próxima permutação; no caso de uma string, as permutações são organizadas em ordem alfabética ascendente. O algoritmo retorna `true` se tiver sucesso e `false` se o intervalo já estiver na sequência final. Para obter todas as permutações de um intervalo, você deve começar com os elementos na ordem mais cedo possível, e o programa usa o algoritmo `sort()` da STL para esse fim.

**Listagem 16.17 — stringstl.cpp**

```cpp
// stringstl.cpp -- aplicando a STL a uma string
#include <iostream>
#include <string>
#include <algorithm>
int main()
{
    using namespace std;
    string letras;
    cout << "Digite o agrupamento de letras (quit para sair): ";
    while (cin >> letras && letras != "quit")
    {
       cout << "Permutações de " << letras << endl;
       sort(letras.begin(), letras.end());
       cout << letras << endl;
       while (next_permutation(letras.begin(), letras.end()))
         cout << letras << endl;
       cout << "Digite a próxima sequência (quit para sair): ";
    }
    cout << "Concluído.\n";
    return 0;
}
```

Aqui está uma execução de exemplo do programa da Listagem 16.17:

```
Enter the letter grouping (quit to quit): awl
Permutações de awl
alw
awl
law
lwa
wal
wla
Enter next sequence (quit to quit): all
Permutações de all
all
lal
lla
Enter next sequence (quit to quit): quit
Concluído.
```

Observe que o algoritmo `next_permutation()` fornece automaticamente apenas permutações únicas, razão pela qual a saída mostra mais permutações para a palavra `awl` do que para a palavra `all`, que tem letras duplicadas.

---

## Funções Versus Métodos de Container

Às vezes você tem uma escolha entre usar um método STL e uma função STL. Geralmente, o método é a melhor escolha. Primeiro, ele deve ser melhor otimizado para um container específico. Segundo, sendo uma função-membro, pode usar as facilidades de gerenciamento de memória do template de classe e redimensionar um container quando necessário.

Suponha, por exemplo, que você tenha uma lista de números e queira remover todas as instâncias de um certo valor, digamos 4, da lista. Se `la` é um objeto `list<int>`, você pode usar o método `remove()` de list:

```cpp
la.remove(4); // remover todos os 4s da lista
```

Após essa chamada de método, todos os elementos com o valor 4 são removidos da lista, e a lista é redimensionada automaticamente.

Há também um algoritmo STL chamado `remove()` (veja o Apêndice G). Em vez de ser invocado por um objeto, ele recebe argumentos de intervalo. Portanto, se `lb` for um objeto `list<int>`, uma chamada para a função poderia ser assim:

```cpp
remove(lb.begin(), lb.end(), 4);
```

Entretanto, como esse `remove()` não é membro, ele não pode ajustar o tamanho da lista. Em vez disso, ele garante que todos os itens não-removidos estejam no início da lista, e retorna um iterador para o novo valor past-the-end. Você pode então usar esse iterador para corrigir o tamanho da lista. Por exemplo, você pode usar o método `erase()` da list para remover um intervalo que descreve a parte da lista que não é mais necessária. A Listagem 16.18 mostra como esse processo funciona.

**Listagem 16.18 — listaremv.cpp**

```cpp
// listaremv.cpp -- aplicando remove() a uma lista
#include <iostream>
#include <list>
#include <algorithm>
void Exibir(int);
const int LIM = 10;
int main()
{
    using namespace std;
    int ar[LIM] = {4, 5, 4, 2, 2, 3, 4, 8, 1, 4};
    list<int> la(ar, ar + LIM);
    list<int> lb(la);
    cout << "Conteúdo original da lista:\n\t";
    for_each(la.begin(), la.end(), Exibir);
    cout << endl;
    la.remove(4);
    cout << "Após usar o método remove():\n";
    cout << "la:\t";
    for_each(la.begin(), la.end(), Exibir);
    cout << endl;
    list<int>::iterator ultimo;
    ultimo = remove(lb.begin(), lb.end(), 4);
    cout << "Após usar a função remove():\n";
    cout << "lb:\t";
    for_each(lb.begin(), lb.end(), Exibir);
    cout << endl;
    lb.erase(ultimo, lb.end());
    cout << "Após usar o método erase():\n";
    cout << "lb:\t";
    for_each(lb.begin(), lb.end(), Exibir);
    cout << endl;
    return 0;
}
void Exibir(int v)
{
    std::cout << v << ' ';
}
```

Aqui está a saída do programa da Listagem 16.18:

```
Conteúdo original da lista:
    4 5 4 2 2 3 4 8 1 4
Após usar o método remove():
la: 5 2 2 3 8 1
Após usar a função remove():
lb: 5 2 2 3 8 1 4 8 1 4
Após usar o método erase():
lb: 5 2 2 3 8 1
```

Como você pode ver, o método `remove()` reduz a lista `la` de 10 elementos para 6. Entretanto, a lista `lb` ainda contém 10 elementos após a função `remove()` ser aplicada a ela. Os 4 últimos elementos são descartáveis porque cada um é ou o valor 4 ou uma duplicata de um valor movido mais para a frente da lista.

Embora os métodos geralmente sejam mais adequados, as funções não-membro são mais gerais. Como você viu, você pode usá-las em arrays e objetos `string` assim como em containers STL, e pode usá-las com tipos mistos de containers — por exemplo, para salvar dados de um container `vector` para uma `list` ou um `set`.

---

## Usando a STL

A STL é uma biblioteca cujas partes são projetadas para trabalhar juntas. Os componentes STL são ferramentas, mas também são blocos de construção para criar outras ferramentas. Vamos ilustrar isso com um exemplo. Suponha que você queira escrever um programa que deixe o usuário inserir palavras. No final, você gostaria de um registro das palavras conforme foram inseridas, uma lista alfabética das palavras usadas (diferença de capitalização ignorada) e um registro de quantas vezes cada palavra foi inserida. Para manter as coisas simples, vamos supor que a entrada não contém números ou pontuação.

Inserir e salvar a lista de palavras é bastante simples. Seguindo o exemplo das Listagens 16.8 e 16.9, você pode criar um objeto `vector<string>` e usar `push_back()` para adicionar palavras de entrada ao vetor:

```cpp
vector<string> palavras;
string entrada;
while (cin >> entrada && entrada != "quit")
   palavras.push_back(entrada);
```

E quanto a obter a lista de palavras alfabética? Você pode usar `sort()` seguido de `unique()`, mas essa abordagem sobrescreve os dados originais porque `sort()` é um algoritmo no-lugar. Há uma maneira mais fácil que evita esse problema. Você pode criar um objeto `set<string>` e copiar (usando um iterador de inserção) as palavras do vetor para o conjunto. Um conjunto automaticamente ordena seu conteúdo, o que significa que você não precisa chamar `sort()`, e um conjunto permite apenas uma cópia de uma chave, de modo que isso substitui a chamada de `unique()`. Espere! A especificação pedia para ignorar diferenças de capitalização. Uma maneira de lidar com isso é usar `transform()` em vez de `copy()` para copiar dados do vetor para o conjunto. Para a função de transformação, você pode usar uma que converte uma string para minúsculas:

```cpp
set<string> conjuntoPalavras;
transform(palavras.begin(), palavras.end(),
   insert_iterator<set<string> > (conjuntoPalavras, conjuntoPalavras.begin()),
   ParaMinuscula);
```

A função `ParaMinuscula()` é fácil de escrever. Você usa `transform()` para aplicar a função `tolower()` a cada elemento na string, usando a string como fonte e destino. Passar e retornar a string como referência significa que o algoritmo trabalha na string original sem ter que fazer cópias. Aqui está o código para `ParaMinuscula()`:

```cpp
string & ParaMinuscula(string & st)
{
    transform(st.begin(), st.end(), st.begin(), tolower);
    return st;
}
```

Um possível problema é que a função `tolower()` é definida como `int tolower(int)`, e alguns compiladores querem que a função combine com o tipo de elemento, que é `char`. Uma solução é substituir `tolower` por `paraminuscula` e fornecer a seguinte definição:

```cpp
char paraminuscula(char ch) { return tolower(ch); }
```

Para obter o número de vezes que cada palavra aparece na entrada, você pode usar a função `count()`. Ela recebe um intervalo e um valor como argumentos e retorna o número de vezes que o valor aparece no intervalo. Você pode usar o objeto `vector` para fornecer o intervalo e o objeto `set` para fornecer a lista de palavras a contar. Ou seja, para cada palavra no conjunto, você pode contar quantas vezes ela aparece no vetor. Para manter a contagem resultante associada à palavra correta, você pode armazenar a palavra e a contagem como um objeto `pair<const string, int>` em um objeto `map`. A palavra será a chave (apenas uma cópia) e a contagem será o valor. Isso pode ser feito em um único laço:

```cpp
map<string, int> mapaPalavras;
set<string>::iterator si;
for (si = conjuntoPalavras.begin(); si != conjuntoPalavras.end(); si++)
   mapaPalavras.insert(pair<string, int>(*si, count(palavras.begin(),
       palavras.end(), *si)));
```

A classe `map` tem uma funcionalidade interessante: você pode usar notação de array com chaves que servem como índices para acessar os valores armazenados. Por exemplo, `mapaPalavras["the"]` representaria o valor associado à chave `"the"`, que neste caso é o número de ocorrências da string `"the"`. Como o container `conjuntoPalavras` mantém todas as chaves usadas por `mapaPalavras`, você pode usar o seguinte código como uma forma alternativa e mais atraente de armazenar resultados:

```cpp
for (si = conjuntoPalavras.begin(); si != conjuntoPalavras.end(); si++)
   mapaPalavras[*si] = count(palavras.begin(), palavras.end(), *si);
```

Como `si` aponta para uma string no container `conjuntoPalavras`, `*si` é uma string e pode servir como chave para `mapaPalavras`. Da mesma forma, você pode usar a notação de array para relatar resultados:

```cpp
for (si = conjuntoPalavras.begin(); si != conjuntoPalavras.end(); si++)
   cout << *si << ": " << mapaPalavras[*si] << endl;
```

A Listagem 16.19 junta essas ideias e inclui código para exibir o conteúdo dos três containers (um `vector` com a entrada, um `set` com uma lista de palavras e um `map` com uma contagem de palavras).

**Listagem 16.19 — usaralgo.cpp**

```cpp
// usaralgo.cpp -- usando vários elementos da STL
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <iterator>
#include <algorithm>
#include <cctype>
using namespace std;
char paraminuscula(char ch) { return tolower(ch); }
string & ParaMinuscula(string & st);
void exibir(const string & s);
int main()
{
    vector<string> palavras;
    cout << "Digite palavras (digite quit para sair):\n";
    string entrada;
    while (cin >> entrada && entrada != "quit")
       palavras.push_back(entrada);
    cout << "Você digitou as seguintes palavras:\n";
    for_each(palavras.begin(), palavras.end(), exibir);
    cout << endl;
    // colocar palavras no conjunto, convertendo para minúsculas
    set<string> conjuntoPalavras;
    transform(palavras.begin(), palavras.end(),
      insert_iterator<set<string> > (conjuntoPalavras, conjuntoPalavras.begin()),
      ParaMinuscula);
    cout << "\nLista alfabética de palavras:\n";
    for_each(conjuntoPalavras.begin(), conjuntoPalavras.end(), exibir);
    cout << endl;
    // colocar palavra e frequência no map
    map<string, int> mapaPalavras;
    set<string>::iterator si;
    for (si = conjuntoPalavras.begin(); si != conjuntoPalavras.end(); si++)
       mapaPalavras[*si] = count(palavras.begin(), palavras.end(), *si);
    // exibir conteúdo do map
    cout << "\nFrequência de palavras:\n";
    for (si = conjuntoPalavras.begin(); si != conjuntoPalavras.end(); si++)
       cout << *si << ": " << mapaPalavras[*si] << endl;
    return 0;
}
string & ParaMinuscula(string & st)
{
    transform(st.begin(), st.end(), st.begin(), paraminuscula);
    return st;
}
void exibir(const string & s)
{
    cout << s << " ";
}
```

Aqui está uma execução de exemplo do programa da Listagem 16.19:

```
Enter words (enter quit to quit):
The dog saw the cat and thought the cat fat
The cat thought the cat perfect
quit
Você digitou as seguintes palavras:
The dog saw the cat and thought the cat fat The cat thought the cat perfect
Lista alfabética de palavras:
and cat dog fat perfect saw the thought
Frequência de palavras:
and: 1
cat: 4
dog: 1
fat: 1
perfect: 1
saw: 1
the: 5
thought: 2
```

A moral aqui é que sua atitude ao usar a STL deve ser a de evitar escrever o máximo de código possível. O projeto genérico e flexível da STL deve poupar muito trabalho. Além disso, os designers da STL são pessoas de algoritmos que são muito preocupadas com eficiência. Portanto, os algoritmos são bem escolhidos e eficientes.

---

## Outras Bibliotecas

O C++ fornece algumas outras bibliotecas de classe mais especializadas do que os exemplos cobertos até agora neste capítulo. Por exemplo, o arquivo de cabeçalho `complex` fornece um template de classe `complex` para números complexos, com especializações para `float`, `long` e `long double`. A classe fornece operações padrão de números complexos, juntamente com funções padrão que podem ser usadas com números complexos. O arquivo de cabeçalho `random` do C++11 estende a funcionalidade de números aleatórios.

O Capítulo 14 introduziu outro exemplo, o template de classe `valarray`, suportado pelo arquivo de cabeçalho `valarray`. Esse template de classe é projetado para representar arrays numéricos e fornece suporte para uma variedade de operações de array numérico, como adicionar o conteúdo de um array a outro, aplicar funções matemáticas a cada elemento de um array e aplicar operações de álgebra linear a arrays.

### `vector`, `valarray` e `array`

Talvez você esteja se perguntando por que o C++ tem três templates de array: `vector`, `valarray` e `array`. Essas classes foram desenvolvidas por grupos diferentes para finalidades diferentes. O template de classe `vector` é parte de um sistema de classes de container e algoritmos. A classe `vector` suporta atividades orientadas a container, como ordenação, inserção, rearranjo, busca, transferência de dados para outros containers e outras manipulações. O template de classe `valarray`, por outro lado, é orientado para computação numérica e não é parte da STL. Ele não tem métodos `push_back()` e `insert()`, por exemplo, mas fornece uma interface simples e intuitiva para muitas operações matemáticas. Finalmente, `array` é projetado como substituto para o tipo de array embutido, combinando a compacidade e eficiência desse tipo com uma interface melhor e mais segura. Sendo de tamanho fixo, `array` não suporta `push_back()` e `insert()`, mas oferece vários outros métodos STL. Esses incluem `begin()`, `end()`, `rbegin()` e `rend()`, tornando fácil aplicar algoritmos STL a objetos `array`.

Suponha, por exemplo, que você tenha estas declarações:

```cpp
vector<double> ved1(10), ved2(10), ved3(10);
array<double, 10> vod1, vod2, vod3;
valarray<double> vad1(10), vad2(10), vad3(10);
```

Além disso, suponha que `ved1`, `ved2`, `vod1`, `vod2`, `vad1` e `vad2` todos adquiram valores adequados. Suponha que você queira atribuir a soma dos primeiros elementos de dois arrays ao primeiro elemento de um terceiro array, e assim por diante. Com a classe `vector`, você faria isto:

```cpp
transform(ved1.begin(), ved1.end(), ved2.begin(), ved3.begin(),
      plus<double>());
```

Você pode fazer o mesmo com a classe `array`:

```cpp
transform(vod1.begin(), vod1.end(), vod2.begin(), vod3.begin(),
      plus<double>());
```

Entretanto, a classe `valarray` sobrecarrega todos os operadores aritméticos para trabalhar com objetos `valarray`, então você usaria isto:

```cpp
vad3 = vad1 + vad2; // + sobrecarregado
```

Da mesma forma, o seguinte resultaria em cada elemento de `vad3` sendo o produto dos elementos correspondentes em `vad1` e `vad2`:

```cpp
vad3 = vad1 * vad2; // * sobrecarregado
```

Suponha que você queira substituir cada valor em um array por esse valor multiplicado por 2,5. A abordagem STL é esta:

```cpp
transform(ved3.begin(), ved3.end(), ved3.begin(),
      bind1st(multiplies<double>(), 2.5));
```

A classe `valarray` sobrecarrega a multiplicação de um objeto `valarray` por um único valor, e também sobrecarrega os vários operadores de atribuição computados, então você poderia usar qualquer um dos seguintes:

```cpp
vad3 = 2.5 * vad3; // * sobrecarregado
vad3 *= 2.5;       // *= sobrecarregado
```

Suponha que você queira calcular o logaritmo natural de cada elemento de um array e armazenar o resultado no elemento correspondente de um segundo array. A abordagem STL é esta:

```cpp
transform(ved1.begin(), ved1.end(), ved3.begin(), log);
```

A classe `valarray` sobrecarrega as funções matemáticas usuais para receber um argumento `valarray` e retornar um objeto `valarray`, então você pode usar isto:

```cpp
vad3 = log(vad1); // log() sobrecarregado
```

Ou você poderia usar o método `apply()`, que também funciona para funções não sobrecarregadas:

```cpp
vad3 = vad1.apply(log);
```

O método `apply()` não altera o objeto invocante; em vez disso, ele retorna um novo objeto que contém os valores resultantes.

A simplicidade da interface `valarray` é ainda mais aparente quando você faz um cálculo em várias etapas:

```cpp
vad3 = 10.0 * ((vad1 + vad2) / 2.0 + vad1 * cos(vad2));
```

A classe `valarray` também fornece um método `sum()` que soma o conteúdo de um objeto `valarray`, um método `size()` que retorna o número de elementos, um método `max()` que retorna o maior valor em um objeto e um método `min()` que retorna o menor valor.

Como você pode ver, `valarray` tem uma clara vantagem notacional sobre `vector` para operações matemáticas, mas também é muito menos versátil. A classe `valarray` tem um método `resize()`, mas não há redimensionamento automático do tipo que você obtém quando usa o método `push_back()` do `vector`. Não há métodos para inserir valores, pesquisar, ordenar e similares. Em suma, a classe `valarray` é mais limitada do que a classe `vector`, mas seu foco mais restrito permite uma interface muito mais simples.

A interface mais simples que `valarray` fornece se traduz em melhor desempenho? Na maioria dos casos, não. A notação simples é tipicamente implementada com o mesmo tipo de laços que você usaria com arrays comuns. Entretanto, alguns designs de hardware permitem operações vetoriais nas quais os valores em um array são carregados simultaneamente em um array de registradores e processados simultaneamente. Em princípio, operações de `valarray` poderiam ser implementadas para aproveitar tais designs.

Você pode usar a STL com objetos `valarray`? Suponha que você tenha um objeto `valarray<double>` com 10 elementos:

```cpp
valarray<double> vad(10);
```

Após o array ter sido preenchido com números, você pode, digamos, usar a função `sort` da STL nele? A classe `valarray` não tem métodos `begin()` e `end()`, portanto você não pode usá-los como argumentos de intervalo:

```cpp
sort(vad.begin(), vad.end()); // NÃO, sem begin(), end()
```

O C++11 resolve a situação fornecendo funções template `begin()` e `end()` que recebem um objeto `valarray` como argumento. Portanto você usaria `begin(vad)` em vez de `vad.begin()`. Essas funções retornam valores compatíveis com os requisitos de intervalo STL:

```cpp
sort(begin(vad), end(vad)); // Solução C++11!
```

A Listagem 16.20 ilustra alguns dos pontos fortes relativos das classes `vector` e `valarray`. Ela usa `push_back()` e a funcionalidade de tamanho automático de `vector` para coletar dados. Depois de ordenar os números, o programa os copia do objeto `vector` para um objeto `valarray` de mesmo tamanho e faz algumas operações matemáticas.

**Listagem 16.20 — valvetor.cpp**

```cpp
// valvetor.cpp -- comparando vector e valarray
#include <iostream>
#include <valarray>
#include <vector>
#include <algorithm>
int main()
{
    using namespace std;
    vector<double> dados;
    double temp;
    cout << "Digite números (<=0 para sair):\n";
    while (cin >> temp && temp > 0)
       dados.push_back(temp);
    sort(dados.begin(), dados.end());
    int tamanho = dados.size();
    valarray<double> numeros(tamanho);
    int i;
    for (i = 0; i < tamanho; i++)
       numeros[i] = dados[i];
    valarray<double> raizes(tamanho);
    raizes = sqrt(numeros);
    valarray<double> resultados(tamanho);
    resultados = numeros + 2.0 * raizes;
    cout.setf(ios_base::fixed);
    cout.precision(4);
    for (i = 0; i < tamanho; i++)
    {
       cout.width(8);
       cout << numeros[i] << ": ";
       cout.width(8);
       cout << resultados[i] << endl;
    }
    cout << "concluído\n";
    return 0;
}
```

Aqui está uma execução de exemplo do programa da Listagem 16.20:

```
Enter numbers (<=0 to quit):
3.3 1.8 5.2 10 14.4 21.6 26.9 0
  1.8000:   4.4833
  3.3000:   6.9332
  5.2000:   9.7607
 10.0000:  16.3246
 14.4000:  21.9895
 21.6000:  30.8952
 26.9000:  37.2730
concluído
```

A classe `valarray` tem muitas funcionalidades além das discutidas até agora. Por exemplo, se `numeros` é um objeto `valarray<double>`, a seguinte instrução cria um array de valores `bool`, com `vbool[i]` definido para o valor de `numeros[i] > 9` — ou seja, para `true` ou `false`:

```cpp
valarray<bool> vbool = numeros > 9;
```

Há versões estendidas de indexação. Vamos examinar uma — a classe `slice`. Um objeto `slice` pode ser usado como um índice de array, caso em que representa, em geral, não apenas um valor, mas um subconjunto de valores. Um objeto `slice` é inicializado com três valores inteiros: o início, o número e o passo. O *início* (start) indica o índice do primeiro elemento a ser selecionado, o *número* (number) indica o número de elementos a serem selecionados, e o *passo* (stride) representa o espaçamento entre elementos. Por exemplo, o objeto construído por `slice(1,4,3)` significa selecionar os quatro elementos cujos índices são 1, 4, 7 e 10. Ou seja, comece com o elemento de início, adicione o passo para obter o próximo elemento e assim por diante até que quatro elementos sejam selecionados. Se, digamos, `varint` é um objeto `vararray<int>`, então a seguinte instrução definiria os elementos 1, 4, 7 e 10 como o valor 10:

```cpp
varint[slice(1,4,3)] = 10; // definir elementos selecionados como 10
```

Essa facilidade especial de indexação permite que você use um objeto `valarray` unidimensional para representar dados bidimensionais. Por exemplo, suponha que você queira representar um array com 4 linhas e 3 colunas. Você pode armazenar as informações em um objeto `valarray` de 12 elementos. Então um objeto `slice(0,3,1)` usado como índice representaria os elementos 0, 1 e 2 — ou seja, a primeira linha. Da mesma forma, um índice `slice(0,4,3)` representaria os elementos 0, 3, 6 e 9 — ou seja, a primeira coluna. A Listagem 16.21 ilustra alguns recursos de `slice`.

**Listagem 16.21 — vfatia.cpp**

```cpp
// vfatia.cpp -- usando fatias de valarray
#include <iostream>
#include <valarray>
#include <cstdlib>
const int TAMANHO = 12;
typedef std::valarray<int> vint; // simplificar declarações
void exibir(const vint & v, int cols);
int main()
{
    using std::slice;   // de <valarray>
    using std::cout;
    vint valint(TAMANHO);  // pensar como 4 linhas de 3
    int i;
    for (i = 0; i < TAMANHO; ++i)
       valint[i] = std::rand() % 10;
    cout << "Array original:\n";
    exibir(valint, 3);    // exibir em 3 colunas
    vint vcol(valint[slice(1,4,3)]); // extrair 2ª coluna
    cout << "Segunda coluna:\n";
    exibir(vcol, 1);      // exibir em 1 coluna
    vint vrow(valint[slice(3,3,1)]); // extrair 2ª linha
    cout << "Segunda linha:\n";
    exibir(vrow, 3);
    valint[slice(2,4,3)] = 10; // atribuir à 3ª coluna
    cout << "Definir última coluna como 10:\n";
    exibir(valint, 3);
    cout << "Definir primeira coluna como soma das próximas duas:\n";
    // + não definido para fatias, então converter para valarray<int>
    valint[slice(0,4,3)] = vint(valint[slice(1,4,3)])
                     + vint(valint[slice(2,4,3)]);
    exibir(valint, 3);
    return 0;
}
void exibir(const vint & v, int cols)
{
    using std::cout;
    using std::endl;
    int lim = v.size();
    for (int i = 0; i < lim; ++i)
    {
       cout.width(3);
       cout << v[i];
       if (i % cols == cols - 1)
         cout << endl;
       else
         cout << ' ';
    }
    if (lim % cols != 0)
       cout << endl;
}
```

O operador `+` é definido para objetos `valarray`, como `valint`, e é definido para um único elemento `int`, como `valint[1]`. Mas como o código na Listagem 16.21 observa, o operador `+` não é definido para unidades `valarray` indexadas por fatia, como `valint[slice(1,4,3)]`. Portanto, o programa constrói objetos completos a partir das fatias para permitir a adição:

```cpp
vint(valint[slice(1,4,3)]) // chama um construtor baseado em fatia
```

A classe `valarray` fornece construtores exatamente para esse fim.

Aqui está uma execução de exemplo do programa da Listagem 16.21:

```
Array original:
  0  3  3
  2  9  0
  8  2  6
  6  9  1
Segunda coluna:
  3
  9
  2
  9
Segunda linha:
  2  9  0
Definir última coluna como 10:
  0  3 10
  2  9 10
  8  2 10
  6  9 10
Definir primeira coluna como soma das próximas duas:
 13  3 10
 19  9 10
 12  2 10
 19  9 10
```

Como os valores são definidos usando `rand()`, diferentes implementações de `rand()` resultarão em valores diferentes.

Há mais, incluindo a classe `gslice` para representar fatias multidimensionais, mas isso deve ser suficiente para dar-lhe uma noção do que `valarray` é.

---

[Anterior](capitulo-16-02-stl-iteradores.md) | [Índice](README.md) | [Próximo](capitulo-16-04-lista-init-resumo.md)
