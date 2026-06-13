# Capítulo 16 — initializer_list, Resumo e Exercícios

> Tradução não oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> [Anterior](capitulo-16-03-containers-algoritmos.md) | [Índice](README.md) | [Próximo](capitulo-17-01-streams-entrada-saida.md)

## O Template `initializer_list` (C++11)

O template `initializer_list` é outra adição do C++11 à biblioteca do C++. Você pode usar a sintaxe de lista inicializadora para inicializar um container STL com uma lista de valores:

```cpp
std::vector<double> pagamentos {45.99, 39.23, 19.95, 89.01};
```

Isso criaria um container para quatro elementos e inicializaria os elementos com os quatro valores na lista. O que torna isso possível é que as classes de container agora têm construtores que recebem um argumento `initializer_list<T>`. Um objeto `vector<double>`, por exemplo, tem um construtor que aceita um argumento `initializer_list<double>`, e a declaração anterior é equivalente a esta:

```cpp
std::vector<double> pagamentos({45.99, 39.23, 19.95, 89.01});
```

Aqui, a lista é escrita explicitamente como um argumento de construtor.

Normalmente, como parte da sintaxe de inicialização universal do C++11, você pode invocar um construtor de classe usando a notação `{}` em vez de `()`:

```cpp
shared_ptr<double> pd {new double}; // ok usar {} em vez de ()
```

Mas isso criaria problemas se também houvesse um construtor de `initializer_list`:

```cpp
std::vector<int> vi{10}; // ??
```

Qual construtor isso invoca?

```cpp
std::vector<int> vi(10);   // caso A: 10 elementos não-inicializados
std::vector<int> vi({10}); // caso B: 1 elemento definido como 10
```

A resposta é que se a classe tem um construtor que aceita um argumento `initializer_list`, usar a sintaxe `{}` invoca esse construtor específico. Portanto, neste exemplo, o caso B se aplica.

Os elementos de `initializer_list` devem ser todos de um tipo. No entanto, o compilador fará conversões para combinar com o tipo:

```cpp
std::vector<double> pagamentos {45.99, 39.23, 19, 89};
// mesmo que std::vector<double> pagamentos {45.99, 39.23, 19.0, 89.0};
```

Aqui, como o tipo do elemento do `vector` é `double`, a lista é do tipo `initializer_list<double>`, e `19` e `89` são convertidos para `double`.

As restrições usuais de estreitamento (narrowing) se aplicam:

```cpp
std::vector<int> valores = {10, 8, 5.5}; // estreitamento, erro de compilação
```

Aqui, o tipo de elemento é `int`, e a conversão implícita de `5.5` para `int` não é permitida.

Não faz sentido fornecer um construtor de `initializer_list` a não ser que a classe seja destinada a lidar com listas de tamanhos variáveis. Por exemplo, você não quer um construtor de `initializer_list` para uma classe que recebe um número fixo de valores. Por exemplo, a seguinte declaração não fornece um construtor de `initializer_list` para os três membros de dado:

```cpp
class Posicao
{
private:
    int x;
    int y;
    int z;
public:
    Posicao(int xx = 0, int yy = 0, int zz = 0)
          : x(xx), y(yy), z(zz) {}
    // sem construtor initializer_list
    ...
};
```

Isso permite que você use a sintaxe `{}` com o construtor `Posicao(int,int,int)`:

```cpp
Posicao A = {20, -3}; // usa Posicao(20,-3,0)
```

### Usando `initializer_list`

Você pode usar objetos `initializer_list` em seu código incluindo o arquivo de cabeçalho `initializer_list`. O template de classe tem membros `begin()` e `end()`, e você pode usá-los para acessar os elementos da lista. Ele também tem um membro `size()` que retorna o número de elementos. A Listagem 16.22 mostra um exemplo simples usando `initializer_list`. Ela requer um compilador que suporte essa funcionalidade do C++11.

**Listagem 16.22 — ilista.cpp**

```cpp
// ilista.cpp -- usar initializer_list (funcionalidade C++11)
#include <iostream>
#include <initializer_list>
double soma(std::initializer_list<double> il);
double media(const std::initializer_list<double> & ril);

int main()
{
    using std::cout;
    cout << "Lista 1: soma = " << soma({2,3,4})
       <<", media = " << media({2,3,4}) << '\n';
    std::initializer_list<double> dl = {1.1, 2.2, 3.3, 4.4, 5.5};
    cout << "Lista 2: soma = " << soma(dl)
       <<", media = " << media(dl) << '\n';
    dl = {16.0, 25.0, 36.0, 40.0, 64.0};
    cout << "Lista 3: soma = " << soma(dl)
       <<", media = " << media(dl) << '\n';
    return 0;
}
double soma(std::initializer_list<double> il)
{
    double tot = 0;
    for (auto p = il.begin(); p != il.end(); p++)
      tot += *p;
    return tot;
}
double media(const std::initializer_list<double> & ril)
{
    double tot = 0;
    int n = ril.size();
    double ave = 0.0;
    if (n > 0)
    {
       for (auto p = ril.begin(); p != ril.end(); p++)
         tot += *p;
       ave = tot / n;
    }
    return ave;
}
```

Aqui está uma execução de exemplo:

```
Lista 1: soma = 9, media = 3
Lista 2: soma = 16.5, media = 3.3
Lista 3: soma = 181, media = 36.2
```

### Notas do Programa

Você pode passar um objeto `initializer_list` por valor ou por referência, como mostrado por `soma()` e `media()`. O objeto em si é pequeno, tipicamente dois ponteiros (um para o início e outro para past-the-end) ou um ponteiro para o início e um inteiro representando o tamanho, portanto a escolha não é uma questão importante de desempenho. (A STL os passa por valor.)

O argumento de função pode ser um literal de lista, como `{2,3,4}`, ou pode ser uma variável de lista, como `dl`.

Os tipos de iterador para `initializer_list` são `const`, portanto você não pode alterar os valores em uma lista:

```cpp
*dl.begin() = 2011.6; // não permitido
```

Mas, como a Listagem 16.22 mostra, você pode anexar uma variável de lista a uma lista diferente:

```cpp
dl = {16.0, 25.0, 36.0, 40.0, 64.0}; // permitido
```

No entanto, o uso pretendido da classe `initializer_list` é passar uma lista de valores para um construtor ou alguma outra função.

---

## Resumo

O C++ inclui um conjunto poderoso de bibliotecas que fornecem soluções para muitos problemas comuns de programação e as ferramentas para simplificar muitos outros problemas. A classe `string` fornece um meio conveniente de manipular strings como objetos, bem como gerenciamento automático de memória e uma série de métodos e funções para trabalhar com strings. Por exemplo, esses métodos e funções permitem concatenar strings, inserir uma string em outra, reverter uma string, pesquisar uma string por caracteres ou substrings e realizar operações de entrada e saída.

Templates de ponteiro inteligente como `auto_ptr` e `shared_ptr` e `unique_ptr` do C++11 tornam mais fácil gerenciar a memória alocada por `new`. Se você usar um desses ponteiros inteligentes em vez de um ponteiro regular para armazenar o endereço retornado por `new`, não precisará lembrar de usar o operador `delete` mais tarde. Quando o objeto ponteiro inteligente expira, seu destrutor chama o operador `delete` automaticamente.

A STL é uma coleção de templates de classe de container, templates de classe de iterador, templates de objeto função e templates de função de algoritmo que apresentam um projeto unificado baseado em princípios de programação genérica. Os algoritmos usam templates para torná-los genéricos em termos do tipo de objeto armazenado e uma interface de iterador para torná-los genéricos em termos do tipo de container. Iteradores são generalizações de ponteiros.

A STL usa o termo *conceito* para denotar um conjunto de requisitos. Por exemplo, o conceito de iterador de avanço inclui os requisitos de que um objeto iterador de avanço pode ser desreferenciado para leitura e escrita e que pode ser incrementado. Implementações reais do conceito são ditas *modelar* o conceito. Por exemplo, o conceito de iterador de avanço poderia ser modelado por um ponteiro comum ou por um objeto projetado para navegar em uma lista encadeada. Conceitos baseados em outros conceitos são denominados *refinamentos*. Por exemplo, o iterador bidirecional é um refinamento do conceito de iterador de avanço.

Classes de container, como `vector` e `set`, são modelos de conceitos de container, como containers, sequências e containers associativos. A STL define vários templates de classe de container: `vector`, `deque`, `list`, `set`, `multiset`, `map`, `multimap` e `bitset`. Ela também define os templates de classe adaptadora `queue`, `priority_queue` e `stack`; essas classes adaptam uma classe de container subjacente para dar a ela a interface característica sugerida pelo nome do template de classe adaptadora. Assim, `stack`, embora baseado por padrão em `vector`, permite inserção e remoção apenas no topo da pilha. O C++11 adiciona `forward_list`, `unordered_set`, `unordered_multiset`, `unordered_map` e `unordered_multimap`.

Alguns algoritmos são expressos como métodos de classe de container, mas a maior parte é expressa como funções genéricas não-membro. Isso é possível graças ao uso de iteradores como interface entre containers e algoritmos. Uma vantagem dessa abordagem é que há apenas uma função `for_each()` ou `copy()`, e assim por diante, em vez de uma versão separada para cada container. Uma segunda vantagem é que os algoritmos da STL podem ser usados com containers não-STL, como arrays comuns, objetos `string`, objetos `array` e quaisquer classes que você projete de forma consistente com o idioma de iterador e container da STL.

Tanto containers quanto algoritmos são caracterizados pelo tipo de iterador que fornecem ou precisam. Você deve verificar se um container apresenta um conceito de iterador que suporta as necessidades do algoritmo. Por exemplo, o algoritmo `for_each()` usa um iterador de entrada, cujos requisitos mínimos são atendidos por todos os tipos de classe de container da STL. Mas `sort()` requer iteradores de acesso aleatório, que nem todas as classes de container suportam. Uma classe de container pode oferecer um método especializado como opção se não atender aos requisitos para um algoritmo específico. Por exemplo, a classe `list` tem um método `sort()` baseado em iteradores bidirecionais, portanto pode usar esse método em vez da função geral.

A STL também fornece objetos função, ou functores, que são classes para as quais o operador `()` é sobrecarregado — ou seja, para as quais o método `operator()()` é definido. Objetos dessas classes podem ser invocados usando notação de função, mas podem carregar informações adicionais. Functores adaptáveis, por exemplo, têm instruções `typedef` que identificam os tipos de argumento e o tipo do valor de retorno para o funtor. Essas informações podem ser usadas por outros componentes, como adaptadores de função.

Ao representar tipos comuns de container e fornecer uma variedade de operações comuns implementadas com algoritmos eficientes, tudo feito de forma genérica, a STL fornece uma excelente fonte de código reutilizável. Você pode ser capaz de resolver um problema de programação diretamente com as ferramentas da STL, ou pode ser capaz de usá-las como blocos de construção para construir a solução de que precisa.

Os templates de classe `complex` e `valarray` suportam operações numéricas para números complexos e arrays.

---

## Revisão do Capítulo

**1.** Considere a seguinte declaração de classe:

```cpp
class RQ1
{
private:
    char * st; // aponta para string estilo C
public:
    RQ1() { st = new char [1]; strcpy(st,""); }
    RQ1(const char * s)
    {st = new char [strlen(s) + 1]; strcpy(st, s); }
    RQ1(const RQ1 & rq)
    {st = new char [strlen(rq.st) + 1]; strcpy(st, rq.st); }
    ~RQ1() {delete [] st};
    RQ & operator=(const RQ & rq);
    // mais coisas
};
```

Converta isso para uma declaração que use um objeto `string` em vez disso. Quais métodos não precisam mais de definições explícitas?

**2.** Nomeie pelo menos duas vantagens que objetos `string` têm sobre strings estilo C em termos de facilidade de uso.

**3.** Escreva uma função que recebe uma referência a um objeto `string` como argumento e que converte o objeto `string` para letras maiúsculas.

**4.** Qual dos itens a seguir não são exemplos de uso correto (conceitual ou sintaticamente) de `auto_ptr`? (Suponha que os arquivos de cabeçalho necessários foram incluídos.)

```cpp
auto_ptr<int> pia(new int[20]);
auto_ptr<string> (new string);
int rigue = 7;
auto_ptr<int> pr(&rigue);
auto_ptr dbl (new double);
```

**5.** Se você pudesse fazer o equivalente mecânico de uma pilha que contém tacos de golfe em vez de números, por que seria (conceitualmente) uma bolsa de golfe ruim?

**6.** Por que um container `set` seria uma escolha ruim para armazenar um registro buraco a buraco de suas pontuações de golfe?

**7.** Como um ponteiro é um iterador, por que os designers da STL simplesmente não usaram ponteiros em vez de iteradores?

**8.** Por que os designers da STL simplesmente não definiram uma classe base de iterador, usaram herança para derivar classes para os outros tipos de iterador e expressaram os algoritmos em termos dessas classes de iterador?

**9.** Dê pelo menos três exemplos de vantagens de conveniência que um objeto `vector` tem sobre um array comum.

**10.** Se a Listagem 16.9 fosse implementada com `list` em vez de `vector`, que partes do programa se tornariam inválidas? A parte inválida poderia ser corrigida facilmente? Se sim, como?

**11.** Considere o funtor `GrandeDemais` da Listagem 16.15. O que o código a seguir faz, e que valores são atribuídos a `bo`?

```cpp
bool bo = GrandeDemais<int>(10)(15);
```

---

## Exercícios de Programação

**1.** Um *palíndromo* (palindrome) é uma string que é igual se lida ao contrário ou à frente. Por exemplo, `"tot"` e `"otto"` são palíndromos bastante curtos. Escreva um programa que permita ao usuário inserir uma string e que passe para uma função `bool` uma referência à string. A função deve retornar `true` se a string for um palíndromo e `false` caso contrário. Neste ponto, não se preocupe com complicações como capitalização, espaços e pontuação. Ou seja, essa versão simples deve rejeitar `"Otto"` e `"Madam, I'm Adam"`. Sinta-se à vontade para consultar a lista de métodos de `string` no Apêndice F para métodos que simplifiquem a tarefa.

**2.** Resolva o mesmo problema do Exercício de Programação 1, mas preocupe-se com complicações como capitalização, espaços e pontuação. Ou seja, `"Madam, I'm Adam"` deve ser testado como palíndromo. Por exemplo, a função de teste poderia reduzir a string para `"madamimadam"` e depois testar se o reverso é o mesmo. Não se esqueça da útil biblioteca `cctype`. Você pode achar uma ou duas funções STL úteis, embora não necessárias.

**3.** Refaça a Listagem 16.3 de modo que ela obtenha suas palavras de um arquivo. Uma abordagem é usar um objeto `vector<string>` em vez de um array de strings. Então você pode usar `push_back()` para copiar quantas palavras houver em seu arquivo de dados para o objeto `vector<string>` e usar o membro `size()` para determinar o comprimento da lista de palavras. Como o programa deve ler uma palavra por vez do arquivo, você deve usar o operador `>>` em vez de `getline()`. O arquivo em si deve conter palavras separadas por espaços, tabulações ou novas linhas.

**4.** Escreva uma função com interface de estilo antigo que tem este protótipo:

```cpp
int reduce(long ar[], int n);
```

Os argumentos reais devem ser o nome de um array e o número de elementos no array. A função deve ordenar um array, remover valores duplicados e retornar um valor igual ao número de elementos no array reduzido. Escreva a função usando funções STL. (Se você decidir usar a função geral `unique()`, observe que ela retorna o fim do intervalo resultante.) Teste a função em um programa curto.

**5.** Resolva o mesmo problema descrito no Exercício de Programação 4, exceto que deve ser uma função template:

```cpp
template <class T>
int reduce(T ar[], int n);
```

Teste a função em um programa curto, usando tanto uma instanciação de `long` quanto uma instanciação de `string`.

**6.** Refaça o exemplo mostrado na Listagem 12.12, usando o template de classe `queue` da STL em vez da classe `Queue` descrita no Capítulo 12.

**7.** Um jogo comum é o cartão de loteria. O cartão tem pontos numerados, dos quais um certo número é selecionado aleatoriamente. Escreva uma função `Loto()` que recebe dois argumentos. O primeiro deve ser o número de pontos em um cartão de loteria, e o segundo deve ser o número de pontos selecionados aleatoriamente. A função deve retornar um objeto `vector<int>` que contém, em ordem crescente, os números selecionados aleatoriamente. Por exemplo, você poderia usar a função da seguinte forma:

```cpp
vector<int> ganhadores;
ganhadores = Loto(51,6);
```

Isso atribuiria a `ganhadores` um vetor que contém seis números selecionados aleatoriamente do intervalo de 1 a 51. Observe que simplesmente usar `rand()` não funciona bem porque pode produzir valores duplicados. Sugestão: Faça a função criar um vetor que contém todos os valores possíveis, use `random_shuffle()` e depois use o início do vetor embaralhado para obter os valores. Escreva também um programa curto que permita testar a função.

**8.** Mat e Pat querem convidar seus amigos para uma festa. Eles pedem a você que escreva um programa que faça o seguinte:

- Permita que Mat insira uma lista com os nomes de seus amigos. Os nomes são armazenados em um container e então exibidos em ordem crescente.
- Permita que Pat insira uma lista com os nomes de seus amigos. Os nomes são armazenados em um segundo container e então exibidos em ordem crescente.
- Crie um terceiro container que mescle as duas listas, elimine duplicatas e exiba o conteúdo desse container.

**9.** Comparado a um array, uma lista encadeada apresenta adição e remoção de elementos mais fáceis, mas é mais lenta para ordenar. Isso levanta uma possibilidade: talvez seja mais rápido copiar uma lista para um array, ordenar o array e copiar o resultado ordenado de volta para a lista do que simplesmente usar o algoritmo de list para ordenação. (Mas também poderia usar mais memória.) Teste a hipótese de velocidade com a seguinte abordagem:

a. Crie um grande objeto `vector<int>` `vi0`, usando `rand()` para fornecer valores iniciais.

b. Crie um segundo objeto `vector<int>` `vi` e um objeto `list<int>` `li` do mesmo tamanho que o original e inicialize-os com os valores no vetor original.

c. Cronometre quanto tempo o programa leva para ordenar `vi` usando o algoritmo `sort()` da STL, e depois cronometre quanto tempo leva para ordenar `li` usando o método `sort()` da list.

d. Reinicialize `li` para o conteúdo não-ordenado de `vi0`. Cronometre a operação combinada de copiar `li` para `vi`, ordenar `vi` e copiar o resultado de volta para `li`.

Para cronometrar essas operações, você pode usar `clock()` da biblioteca `ctime`. Como na Listagem 5.14, você pode usar esta instrução para iniciar a primeira cronometragem:

```cpp
clock_t inicio = clock();
```

E depois use o seguinte ao final da operação para obter o tempo decorrido:

```cpp
clock_t fim = clock();
cout << (double)(fim - inicio)/CLOCKS_PER_SEC;
```

Esse não é de forma alguma um teste definitivo porque os resultados dependerão de uma variedade de fatores, incluindo memória disponível, se há multiprocessamento em andamento e o tamanho do array ou lista. Além disso, se você tiver a opção entre uma build de depuração (debug build) e uma build de lançamento (release build), use a build de lançamento para a medição. Com os computadores velozes de hoje, você provavelmente precisará usar o maior array possível para obter leituras significativas. Você pode tentar, por exemplo, 100.000 elementos, 1.000.000 de elementos e 10.000.000 de elementos.

**10.** Modifique a Listagem 16.9 (vet3.cpp) da seguinte forma:

a. Adicione um membro `preco` à estrutura `Resenha`.

b. Em vez de usar um `vector` de objetos `Resenha` para armazenar a entrada, use um `vector` de objetos `shared_ptr<Resenha>`. Lembre-se de que um `shared_ptr` deve ser inicializado com um ponteiro retornado por `new`.

c. Siga a etapa de entrada com um laço que permita ao usuário as seguintes opções para exibir livros: na ordem original, em ordem alfabética, em ordem crescente de avaliações, em ordem decrescente de avaliações, em ordem crescente de preço, em ordem decrescente de preço e sair.

Aqui está uma possível abordagem. Após obter a entrada inicial, crie outro `vector` de `shared_ptr`s inicializado para o array original. Defina uma função `operator<()` que compara estruturas apontadas e use-a para ordenar o segundo vetor de modo que os `shared_ptr`s estejam na ordem dos nomes dos livros armazenados nos objetos apontados. Repita o processo para obter vetores de `shared_ptr`s ordenados por avaliação e por preço. Observe que `rbegin()` e `rend()` poupam o trabalho de também criar vetores de ordem invertida.

---

[Anterior](capitulo-16-03-containers-algoritmos.md) | [Índice](README.md) | [Próximo](capitulo-17-01-streams-entrada-saida.md)
