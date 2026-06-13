# Capitulo 4 - Tipos Compostos (Compound Types)
## Parte 7: Combinacoes de Tipos e Alternativas a Arrays

> Traducao nao oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Navegacao: [Anterior](capitulo-04-06-ponteiros.md) | [Indice](README.md) | [Proximo](capitulo-04-08-resumo-revisao.md)

---

## Combinacoes de Tipos

Este capitulo apresentou arrays, estruturas e ponteiros. Esses podem ser combinados de varias formas, entao vamos revisar algumas das possibilidades, comecando com uma estrutura:

```cpp
struct fim_ano_antartica
{
    int ano;
    /* alguns dados realmente interessantes, etc. */
};
```

Podemos criar variaveis desse tipo:

```cpp
fim_ano_antartica s01, s02, s03; // s01, s02, s03 sao estruturas
```

Podemos entao acessar membros usando o operador de membro:

```cpp
s01.ano = 1998;
```

Podemos criar um ponteiro para essa estrutura:

```cpp
fim_ano_antartica * pa = &s02;
```

Desde que o ponteiro tenha sido definido para um endereco valido, podemos usar o operador de membro indireto para acessar membros:

```cpp
pa->ano = 1999;
```

Podemos criar arrays de estruturas:

```cpp
fim_ano_antartica trio[3]; // array de 3 estruturas
```

Podemos entao usar o operador de membro para acessar membros de um elemento:

```cpp
trio[0].ano = 2003; // trio[0] e uma estrutura
```

Aqui, `trio` e um array, mas `trio[0]` e uma estrutura, e `trio[0].ano` e um membro dessa estrutura. Como um nome de array e um ponteiro, tambem podemos usar o operador de membro indireto:

```cpp
(trio+1)->ano = 2004; // mesmo que trio[1].ano = 2004;
```

Podemos criar um array de ponteiros:

```cpp
const fim_ano_antartica * arp[3] = {&s01, &s02, &s03};
```

Isso esta ficando um pouco complicado. Como podemos acessar dados com esse array? Bem, se `arp` e um array de ponteiros, entao `arp[1]` deve ser um ponteiro, e podemos usar o operador de membro indireto com ele para acessar um membro:

```cpp
std::cout << arp[1]->ano << std::endl;
```

Podemos criar um ponteiro para tal array:

```cpp
const fim_ano_antartica ** ppa = arp;
```

Aqui, `arp` e o nome de um array; portanto, e o endereco de seu primeiro elemento. Mas seu primeiro elemento e um ponteiro, entao `ppa` tem que ser um ponteiro para um ponteiro para `const fim_ano_antartica`, daı o `**`. Ha varias maneiras de errar essa declaracao. Por exemplo, voce poderia omitir o `const`, esquecer um `*` ou dois, transpor letras ou de outra forma desfigurar o tipo de estrutura. Aqui esta um caso em que a versao C++11 de `auto` e conveniente. O compilador conhece perfeitamente o tipo de `arp`, portanto pode deduzir o tipo correto para voce:

```cpp
auto ppb = arp; // deducao automatica de tipo do C++11
```

No passado, o compilador usava seu conhecimento do tipo correto para reclamar sobre erros que voce poderia ter cometido na declaracao; agora ele pode deixar seu conhecimento trabalhar para voce.

Como voce pode usar `ppa` para acessar dados? Como `ppa` e um ponteiro para um ponteiro para uma estrutura, `*ppa` e um ponteiro para uma estrutura, portanto voce pode usar o operador de membro indireto com ele:

```cpp
std::cout << (*ppa)->ano << std::endl;
std::cout << (*(ppb+1))->ano << std::endl;
```

Como `ppa` aponta para o primeiro membro de `arp`, `*ppa` e o primeiro membro, que e `&s01`. Portanto, `(*ppa)->ano` e o membro `ano` de `s01`. Na segunda instrucao, `ppb+1` aponta para o proximo elemento, `arp[1]`, que e `&s02`. Os parenteses sao necessarios para obter as associacoes corretas. Por exemplo, `*ppa->ano` tentaria aplicar o operador `*` a `ppa->ano`, o que falha porque o membro `ano` nao e um ponteiro.

Tudo isso e realmente verdade? A Listagem 4.23 incorpora todas as instrucoes anteriores em um programa curto.

**Listagem 4.23 — mixtypes.cpp**

```cpp
// mixtypes.cpp -- algumas combinacoes de tipos
#include <iostream>
struct fim_ano_antartica
{
    int ano;
    /* alguns dados realmente interessantes, etc. */
};
int main()
{
    fim_ano_antartica s01, s02, s03;
    s01.ano = 1998;
    fim_ano_antartica * pa = &s02;
    pa->ano = 1999;
    fim_ano_antartica trio[3]; // array de 3 estruturas
    trio[0].ano = 2003;
    std::cout << trio->ano << std::endl;
    const fim_ano_antartica * arp[3] = {&s01, &s02, &s03};
    std::cout << arp[1]->ano << std::endl;
    const fim_ano_antartica ** ppa = arp;
    auto ppb = arp; // deducao automatica de tipo do C++11
    // ou use const fim_ano_antartica ** ppb = arp;
    std::cout << (*ppa)->ano << std::endl;
    std::cout << (*(ppb+1))->ano << std::endl;
    return 0;
}
```

Aqui esta a saida:

```
2003
1999
1998
1999
```

O programa compila e funciona conforme prometido.

## Alternativas a Arrays

Anteriormente neste capitulo, foram mencionadas as classes template `vector` e `array` como alternativas ao tipo de array nativo. Vamos dar uma breve olhada agora em como elas sao usadas e em alguns dos beneficios de usá-las.

## A Classe Template vector

A classe template `vector` e similar a classe `string` no sentido de que e um array dinamico. Voce pode definir o tamanho de um objeto `vector` durante o tempo de execucao, e pode acrescentar novos dados ao final ou inserir novos dados no meio. Basicamente, e uma alternativa ao uso de `new` para criar um array dinamico. Na verdade, a classe `vector` usa `new` e `delete` para gerenciar memoria, mas o faz automaticamente.

No momento nao iremos nos aventurar muito profundamente no significado de uma classe template. Em vez disso, vamos examinar alguns assuntos praticos basicos. Primeiro, para usar um objeto `vector`, voce precisa incluir o arquivo de cabecalho `vector`. Segundo, o identificador `vector` faz parte do namespace `std`, portanto voce pode usar uma diretiva `using`, uma declaracao `using` ou `std::vector`. Terceiro, os templates usam uma sintaxe diferente para indicar o tipo de dado armazenado. Quarto, a classe `vector` usa uma sintaxe diferente para indicar o numero de elementos. Aqui estao alguns exemplos:

```cpp
#include <vector>
...
using namespace std;
vector<int> vi;     // cria um array de tamanho zero de int
int n;
cin >> n;
vector<double> vd(n); // cria um array de n doubles
```

Dizemos que `vi` e um objeto do tipo `vector<int>` e que `vd` e um objeto do tipo `vector<double>`. Como os objetos `vector` se redimensionam automaticamente quando voce insere ou adiciona valores a eles, esta tudo bem que `vi` comece com tamanho 0. Mas para que o redimensionamento funcione, voce usaria os varios metodos que fazem parte do pacote `vector`.

Em geral, a seguinte declaracao cria um objeto `vector` `vt` que pode conter `n_elem` elementos do tipo `tipoDado`:

```cpp
vector<tipoDado> vt(n_elem);
```

O parametro `n_elem` pode ser uma constante inteira ou uma variavel inteira.

## A Classe Template array (C++11)

A classe `vector` tem mais capacidades do que o tipo de array nativo, mas isso tem um custo de eficiencia ligeiramente menor. Se tudo que voce precisa e de um array de tamanho fixo, pode ser vantajoso usar o tipo nativo. No entanto, isso tem seus proprios custos de menor conveniencia e seguranca. O C++11 respondeu a essa situacao adicionando a classe template `array`, que faz parte do namespace `std`. Como o tipo nativo, um objeto `array` tem tamanho fixo e usa a pilha (ou armazenamento de memoria estatica) em vez da area de armazenamento livre, compartilhando assim a eficiencia dos arrays nativos. A isso ele acrescenta conveniencia e seguranca adicional. Para criar um objeto `array`, voce precisa incluir o arquivo de cabecalho `array`. A sintaxe e um pouco diferente da de um `vector`:

```cpp
#include <array>
...
using namespace std;
array<int, 5> ai;                        // cria objeto array de 5 ints
array<double, 4> ad = {1.2, 2.1, 3.43, 4.3};
```

De forma mais geral, a seguinte declaracao cria um objeto `array` `arr` com `n_elem` elementos do tipo `tipoDado`:

```cpp
array<tipoDado, n_elem> arr;
```

Ao contrario do caso para `vector`, `n_elem` nao pode ser uma variavel.

Com o C++11, voce pode usar a inicializacao por lista com objetos `vector` e `array`. No entanto, essa nao era uma opcao com objetos `vector` do C++98.

## Comparando Arrays, Objetos Vector e Objetos Array

Talvez a maneira mais simples de entender as semelancas e diferencas entre arrays, objetos `vector` e objetos `array` seja examinar um breve exemplo (Listagem 4.24) que usa as tres abordagens.

**Listagem 4.24 — choices.cpp**

```cpp
// choices.cpp -- variacoes de arrays
#include <iostream>
#include <vector>  // STL C++98
#include <array>   // C++11
int main()
{
    using namespace std;
    // C, C++ original
    double a1[4] = {1.2, 2.4, 3.6, 4.8};
    // C++98 STL
    vector<double> a2(4); // cria vector com 4 elementos
    // sem forma simples de inicializar no C++98
    a2[0] = 1.0/3.0;
    a2[1] = 1.0/5.0;
    a2[2] = 1.0/7.0;
    a2[3] = 1.0/9.0;
    // C++11 -- cria e inicializa objeto array
    array<double, 4> a3 = {3.14, 2.72, 1.62, 1.41};
    array<double, 4> a4;
    a4 = a3; // valido para objetos array do mesmo tamanho
    // usa notacao de array
    cout << "a1[2]: " << a1[2] << " em " << &a1[2] << endl;
    cout << "a2[2]: " << a2[2] << " em " << &a2[2] << endl;
    cout << "a3[2]: " << a3[2] << " em " << &a3[2] << endl;
    cout << "a4[2]: " << a4[2] << " em " << &a4[2] << endl;
    // acesso indevido
    a1[-2] = 20.2;
    cout << "a1[-2]: " << a1[-2] << " em " << &a1[-2] << endl;
    cout << "a3[2]: " << a3[2] << " em " << &a3[2] << endl;
    cout << "a4[2]: " << a4[2] << " em " << &a4[2] << endl;
    return 0;
}
```

Aqui esta uma saida de exemplo:

```
a1[2]: 3.6 em 0x28cce8
a2[2]: 0.142857 em 0xca0328
a3[2]: 1.62 em 0x28ccc8
a4[2]: 1.62 em 0x28cca8
a1[-2]: 20.2 em 0x28ccc8
a3[2]: 20.2 em 0x28ccc8
a4[2]: 1.62 em 0x28cca8
```

**Notas do Programa**

Primeiro, observe que, quer usemos um array nativo, um objeto `vector` ou um objeto `array`, podemos usar a notacao de array padrao para acessar membros individuais. Segundo, voce pode ver pelos enderecos que os objetos `array` usam a mesma regiao de memoria (a pilha, neste caso) que o array nativo, enquanto o objeto `vector` e armazenado em uma regiao diferente (a area de armazenamento livre, ou heap). Terceiro, note que voce pode atribuir um objeto `array` a outro objeto `array`. Para arrays nativos, voce tem que copiar os dados elemento por elemento.

Em seguida, e isso merece atencao especial, observe esta linha:

```cpp
a1[-2] = 20.2;
```

O que significa um indice de `-2`? Lembre-se de que isso e traduzido para o seguinte:

```cpp
*(a1-2) = 20.2;
```

Exprimindo isso em palavras, veja para onde `a1` aponta, recue dois elementos `double`, e coloque `20.2` la. Ou seja, armazene as informacoes em uma localizacao fora do array. O C++, como o C, nao verifica esse tipo de erros de fora do intervalo. Neste caso particular, essa localizacao acabou sendo no objeto `array` `a3`. Outro compilador colocou o `20.2` desviante em `a4`, e outros compiladores podem fazer outras escolhas ruins. Este e um exemplo do comportamento inseguro de arrays nativos.

Os objetos `vector` e `array` protegem contra esse comportamento? Eles podem, se voce deixar. Ou seja, voce ainda pode escrever codigo inseguro, como o seguinte:

```cpp
a2[-2] = .5;  // ainda permitido
a3[200] = 1.4;
```

No entanto, voce tem alternativas. Uma e usar a funcao membro `at()`. Assim como voce pode usar a funcao membro `getline()` com o objeto `cin`, voce pode usar a funcao membro `at()` com objetos do tipo `vector` ou `array`:

```cpp
a2.at(1) = 2.3; // atribui 2.3 a a2[1]
```

A diferenca entre usar a notacao de colchetes e a funcao membro `at()` e que se voce usar `at()`, um indice invalido sera detectado durante o tempo de execucao e o programa, por padrao, aborta. Essa verificacao adicional tem o custo de aumento do tempo de execucao, razao pela qual o C++ oferece a opcao de usar qualquer uma das notacoes. Alem disso, essas classes oferecem formas de usar objetos que reduzem as chances de erros de intervalo acidentais. Por exemplo, as classes tem funcoes membro `begin()` e `end()` que permitem delimitar o intervalo sem exceder acidentalmente os limites. Mas vamos reservar essa discussao para o Capitulo 16.

---

> Navegacao: [Anterior](capitulo-04-06-ponteiros.md) | [Indice](README.md) | [Proximo](capitulo-04-08-resumo-revisao.md)
