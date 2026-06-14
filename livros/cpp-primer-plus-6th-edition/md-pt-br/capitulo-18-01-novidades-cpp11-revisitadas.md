# Capítulo 18 — Visitando o Novo Padrão C++ (parte 1)

> Tradução não oficial de *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Navegação: [Anterior](capitulo-17-03-arquivo-avancado-resumo.md) | [Índice](README.md) | [Próximo](capitulo-18-02-move-semantics-novidades-classes.md)

Neste capítulo você revisará as funcionalidades do C++11 cobertas anteriormente e depois aprenderá sobre o seguinte:

- Semântica de move e referências rvalue
- Expressões lambda
- O template wrapper `function`
- Templates variádicos

Este capítulo concentra-se nas novas mudanças do C++11 à linguagem C++. O livro já cobriu diversas funcionalidades do C++11, e começaremos revisando-as. Em seguida, examinaremos algumas funcionalidades adicionais com mais detalhes. Depois identificaremos algumas adições do C++11 que estão além do escopo deste livro. (Dado que o rascunho do C++11 é mais de 80% mais longo que o C++98, não cobriremos tudo.) Por fim, examinaremos brevemente a biblioteca BOOST.

## Funcionalidades do C++11 Revisitadas

Até agora você pode ter perdido o fio das muitas mudanças do C++11 que já encontramos. Esta seção as revisa brevemente.

### Novos Tipos

O C++11 adiciona os tipos `long long` e `unsigned long long` para suportar inteiros de 64 bits (ou mais amplos) e os tipos `char16_t` e `char32_t` para suportar representações de caracteres de 16 e 32 bits, respectivamente. Também adiciona a string "bruta" (raw string). O Capítulo 3, "Lidando com Dados," discute essas adições.

### Inicialização Uniforme

O C++11 estende a aplicabilidade da lista delimitada por chaves (list-initialization) para que possa ser usada com todos os tipos embutidos e com tipos definidos pelo usuário (ou seja, objetos de classe). A lista pode ser usada com ou sem o sinal `=`:

```cpp
int x = {5};
double y {2.75};
short quar[5] {4,5,2,76,1};
```

Além disso, a sintaxe de inicialização por lista pode ser usada em expressões `new`:

```cpp
int * ar = new int [4] {2,4,6,7}; // C++11
```

Com objetos de classe, uma lista entre chaves pode ser usada em vez de uma lista entre parênteses para invocar um construtor:

```cpp
class Tronco
{
private:
    int raizes;
    double peso;
public:
    Tronco(int r, double p) : raizes(r), peso(p) {}
};
Tronco s1(3, 15.6);  // estilo antigo
Tronco s2{5, 43.4};  // C++11
Tronco s3 = {4, 32.1}; // C++11
```

No entanto, se uma classe tiver um construtor cujo argumento é um template `std::initializer_list`, então somente esse construtor pode usar a forma de inicialização por lista. Vários aspectos da inicialização por lista foram discutidos nos Capítulos 3, 4, 9, 10 e 16.

### Estreitamento (Narrowing)

A sintaxe de lista de inicialização fornece proteção contra estreitamento — ou seja, contra atribuir um valor numérico a um tipo numérico incapaz de conter o valor. A inicialização comum permite que você faça coisas que podem ou não fazer sentido:

```cpp
char c1 = 1.57e27;  // double-para-char, comportamento indefinido
char c2 = 459585821; // int-para-char, comportamento indefinido
```

No entanto, se você usar a sintaxe de lista de inicialização, o compilador não permite conversões de tipo que tentam armazenar valores em um tipo "mais estreito" do que o valor:

```cpp
char c1 {1.57e27};       // double-para-char, erro em tempo de compilação
char c2 = {459585821};   // int-para-char, fora do intervalo, erro em tempo de compilação
```

No entanto, conversões para tipos mais amplos são permitidas. Além disso, uma conversão para um tipo mais estreito é permitida se o valor estiver dentro do intervalo permitido pelo tipo:

```cpp
char c1 {66};     // int-para-char, dentro do intervalo, permitido
double c2 = {66}; // int-para-double, permitido
```

### `std::initializer_list`

O C++11 fornece uma classe template `initializer_list` (discutida no Capítulo 16, "A Classe `string` e a Biblioteca de Templates Padrão") que pode ser usada como argumento de construtor. Se uma classe tiver tal construtor, a sintaxe de chaves pode ser usada apenas com esse construtor. Os itens da lista devem ser do mesmo tipo ou conversíveis para o mesmo tipo. Os containers da STL fornecem construtores com argumentos `initializer_list`:

```cpp
vector<int> a1(10);  // vetor não inicializado com 10 elementos
vector<int> a2{10};  // lista de inicialização, a2 tem 1 elemento definido como 10
vector<int> a3{4,6,1}; // 3 elementos definidos como 4,6,1
```

O arquivo de cabeçalho `initializer_list` fornece suporte para essa classe template. A classe tem os métodos `begin()` e `end()` especificando o intervalo da lista. Você pode usar um argumento `initializer_list` para funções regulares, bem como para construtores:

```cpp
#include <initializer_list>
double soma(std::initializer_list<double> il);
int main()
{
    double total = soma({2.5, 3.1, 4}); // 4 convertido para 4.0
    // ...
}
double soma(std::initializer_list<double> il)
{
    double tot = 0;
    for (auto p = il.begin(); p != il.end(); p++)
        tot += *p;
    return tot;
}
```

### Declarações

O C++11 implementa várias funcionalidades que simplificam declarações, especialmente para situações que surgem quando templates são usados.

**`auto`**

O C++11 retira da palavra-chave `auto` seu significado anterior como especificador de classe de armazenamento (Capítulo 9, "Modelos de Memória e Namespaces") e a coloca em uso (Capítulo 3) para implementar a dedução automática de tipo, desde que um inicializador explícito seja fornecido. O compilador define o tipo da variável com o tipo do valor de inicialização:

```cpp
auto maton = 112;    // maton é do tipo int
auto pt = &maton;    // pt é do tipo int *
double fm(double, int);
auto pf = fm;        // pf é do tipo double (*)(double,int)
```

A palavra-chave `auto` também pode simplificar declarações de template. Por exemplo, se `il` é um objeto do tipo `std::initializer_list<double>`, você pode substituir:

```cpp
for (std::initializer_list<double>::iterator p = il.begin(); p != il.end(); p++)
```

por isto:

```cpp
for (auto p = il.begin(); p != il.end(); p++)
```

**`decltype`**

A palavra-chave `decltype` cria uma variável do tipo indicado por uma expressão. A instrução a seguir significa "faça `y` do mesmo tipo que `x`," onde `x` é uma expressão:

```cpp
decltype(x) y;
```

Eis alguns exemplos:

```cpp
double x;
int n;
decltype(x*n) q; // q do mesmo tipo que x*n, ou seja, double
decltype(&x) pd; // pd do mesmo tipo que &x, ou seja, double *
```

Isso é particularmente útil em definições de template, quando o tipo pode não ser determinado até que uma instanciação específica seja feita:

```cpp
template<typename T, typename U>
void ef(T t, U u)
{
    decltype(T*U) tu;
    // ...
}
```

Aqui, `tu` é do tipo resultante da operação `T*U`, supondo que a operação esteja definida. Por exemplo, se `T` é `char` e `U` é `short`, `tu` seria do tipo `int` por causa das promoções inteiras automáticas que ocorrem na aritmética de inteiros.

O funcionamento de `decltype` é mais complicado do que o de `auto`, e os tipos resultantes podem ser referências e podem ser qualificados com `const`, dependendo das expressões usadas. Aqui estão mais alguns exemplos:

```cpp
int j = 3;
int &k = j;
const int &n = j;
decltype(n) i1;   // i1 é do tipo const int &
decltype(j) i2;   // i2 é do tipo int
decltype((j)) i3; // i3 é do tipo int &
decltype(k + 1) i4; // i4 é do tipo int
```

### Tipo de Retorno à Direita (Trailing Return Type)

O C++11 introduz uma nova sintaxe para declarar funções, na qual o tipo de retorno vem após o nome da função e a lista de parâmetros em vez de antes deles:

```cpp
double f1(double, int); // sintaxe tradicional
auto f2(double, int) -> double; // nova sintaxe, tipo de retorno é double
```

A nova sintaxe pode parecer um passo para trás em legibilidade para as declarações de função usuais, mas permite usar `decltype` para especificar tipos de retorno de funções template:

```cpp
template<typename T, typename U>
auto eff(T t, U u) -> decltype(T*U)
{
    // ...
}
```

O problema que está sendo abordado aqui é que `T` e `U` não estão no escopo antes de o compilador ler a lista de parâmetros de `eff`, portanto qualquer uso de `decltype` deve vir após a lista de parâmetros. A nova sintaxe torna isso possível.

### Aliases de Template: `using =`

É útil poder criar aliases para identificadores de tipo longos ou complexos. O C++ já tinha `typedef` para esse propósito:

```cpp
typedef std::vector<std::string>::iterator tipoIterador;
```

O C++11 fornece uma segunda sintaxe (discutida no Capítulo 14, "Reutilizando Código em C++") para criar aliases:

```cpp
using tipoIterador = std::vector<std::string>::iterator;
```

A diferença é que a nova sintaxe também pode ser usada para especializações parciais de template, enquanto `typedef` não pode:

```cpp
template<typename T>
    using arr12 = std::array<T, 12>; // template para múltiplos aliases
```

Essa instrução especializa o template `array<T, int>` definindo o parâmetro `int` como `12`. Por exemplo, considere estas declarações:

```cpp
std::array<double, 12> a1;
std::array<std::string, 12> a2;
```

Elas podem ser substituídas pelo seguinte:

```cpp
arr12<double> a1;
arr12<std::string> a2;
```

### `nullptr`

O ponteiro nulo é um ponteiro garantidamente não apontando para dados válidos. Tradicionalmente, o C++ representa esse ponteiro no código fonte com `0`, embora a representação interna possa ser diferente. Isso levanta alguns problemas porque torna `0` tanto uma constante de ponteiro quanto uma constante inteira. Como discutido no Capítulo 12, "Classes e Alocação Dinâmica de Memória," o C++11 introduz a palavra-chave `nullptr` para representar o ponteiro nulo; é do tipo ponteiro e não convertível para o tipo inteiro. Para compatibilidade com versões anteriores, o C++ ainda permite o uso de `0`, e a expressão `nullptr == 0` é avaliada como `true`, mas usar `nullptr` em vez de `0` fornece melhor segurança de tipos.

### Ponteiros Inteligentes (Smart Pointers)

Um programa que usa `new` para alocar memória do heap (ou free store) deve usar `delete` para liberar essa memória quando não for mais necessária. Anteriormente, o C++ introduziu o ponteiro inteligente `auto_ptr` para ajudar a automatizar o processo. A experiência subsequente de programação, especialmente com a STL, indicou que algo mais sofisticado era necessário. Guiado pela experiência de programadores e pelas soluções fornecidas pela biblioteca BOOST, o C++11 deprecia `auto_ptr` e introduz três novos tipos de ponteiros inteligentes: `unique_ptr`, `shared_ptr` e `weak_ptr`. O Capítulo 16 discute os dois primeiros.

Todos os novos ponteiros inteligentes foram projetados para trabalhar com containers da STL e com semântica de move.

### Mudanças na Especificação de Exceção

O C++ fornece uma sintaxe para especificar quais exceções, se houver, uma função pode lançar (consulte o Capítulo 15, "Amigos, Exceções e Mais"):

```cpp
void f501(int) throw(bad_dog);  // pode lançar exceção do tipo bad_dog
void f733(long long) throw();   // não lança uma exceção
```

Assim como `auto_ptr`, a experiência coletiva da comunidade de programadores C++ é que, na prática, as especificações de exceção não funcionaram tão bem quanto pretendido. Portanto, o padrão C++11 deprecia as especificações de exceção. No entanto, o comitê de padrões considerou que há algum valor em documentar que uma função não lança uma exceção, e adicionou a palavra-chave `noexcept` para esse propósito:

```cpp
void f875(short, short) noexcept; // não lança uma exceção
```

### Enumerações com Escopo (Scoped Enumerations)

As enumerações tradicionais do C++ fornecem uma maneira de criar constantes nomeadas. No entanto, elas vêm com um nível bastante baixo de verificação de tipos. Além disso, o escopo para nomes de enumeração é o escopo que envolve a definição da enumeração, o que significa que duas enumerações definidas no mesmo escopo não devem ter membros de enumeração com o mesmo nome. Por fim, enumerações podem não ser completamente portáveis porque diferentes implementações podem escolher usar diferentes tipos subjacentes. O C++11 introduz uma variante de enumerações que aborda esses problemas. A nova forma é indicada pelo uso de `class` ou `struct` na definição:

```cpp
enum Old1 {sim, nao, talvez};            // forma tradicional
enum class Novo1 {nunca, as_vezes, frequentemente, sempre}; // nova forma
enum struct Novo2 {nunca, alavanca, cortar}; // nova forma
```

As novas formas evitam conflitos de nomes ao exigir escopo explícito. Assim, você usaria `Novo1::nunca` e `Novo2::nunca` para identificar essas enumerações particulares. O Capítulo 10, "Objetos e Classes," fornece mais detalhes.

### Mudanças de Classe

O C++11 faz várias mudanças para simplificar e expandir o design de classes. Isso inclui formas de permitir que construtores chamem uns aos outros e sejam herdados, melhores maneiras de controlar o acesso a métodos, e construtores de move e operadores de atribuição de move, todos os quais serão cobertos neste capítulo. Enquanto isso, vamos revisar as mudanças que foram discutidas anteriormente.

**Operadores de Conversão `explicit`**

Um dos aspectos empolgantes dos primeiros dias do C++ era a facilidade com que conversões automáticas para classes podiam ser estabelecidas. Uma das realizações que cresceu à medida que a experiência de programação se acumulava era que as conversões automáticas de tipo podiam levar a problemas na forma de conversões inesperadas. O C++ então abordou um aspecto do problema introduzindo a palavra-chave `explicit` para suprimir conversões automáticas invocadas por construtores de um argumento:

```cpp
class Plebeu
{
    Plebeu(int);    // conversão automática int-para-Plebeu
    explicit Plebeu(double); // requer uso explícito
    // ...
};
// ...
Plebeu a, b;
a = 5;        // conversão implícita, chama Plebeu(5)
b = 0.5;      // não permitido
b = Plebeu(0.5); // conversão explícita
```

O C++11 estende o uso de `explicit` (discutido no Capítulo 11, "Trabalhando com Classes") para que funções de conversão possam ser tratadas de forma semelhante:

```cpp
class Plebeu
{
    // ...
    // funções de conversão
    operator int() const;
    explicit operator double() const;
    // ...
};
// ...
Plebeu a, b;
int n = a;    // conversão automática Plebeu-para-int
double x = b; // não permitido
x = double(b); // conversão explícita, permitida
```

**Inicialização de Membro em Classe (Member In-Class Initialization)**

Muitos usuários iniciantes do C++ se perguntaram por que não podem inicializar membros simplesmente fornecendo valores na definição da classe. Agora eles (e você) podem. A sintaxe parece com esta:

```cpp
class Sessao
{
    int mem1 = 10;      // inicialização em classe
    double mem2 {1966.54}; // inicialização em classe
    short mem3;
public:
    Sessao() {}          // #1
    Sessao(short s) : mem3(s) {} // #2
    Sessao(int n, double d, short s) : mem1(n), mem2(d), mem3(s) {} // #3
    // ...
};
```

Você pode usar o sinal de igual ou as formas de chaves de inicialização, mas não a versão entre parênteses. O resultado é o mesmo que se você fornecesse os dois primeiros construtores com entradas da lista de inicialização de membros para `mem1` e `mem2`:

```cpp
Sessao() : mem1(10), mem2(1966.54) {}
Sessao(short s) : mem1(10), mem2(1966.54), mem3(s) {}
```

Observe como usar a inicialização em classe evita ter que duplicar código nos construtores, reduzindo assim o trabalho e o número de oportunidades de erro.

Esses valores padrão são substituídos por um construtor que fornece valores na lista de inicialização de membros, portanto o terceiro construtor substitui a inicialização de membro em classe.

### Mudanças em Templates e STL

O C++11 faz várias mudanças estendendo a usabilidade de templates em geral e da Standard Template Library em particular. Algumas estão na própria biblioteca; outras se relacionam à facilidade de uso. Neste capítulo já mencionamos aliases de template e os ponteiros inteligentes amigáveis à STL.

**Loop `for` Baseado em Intervalo**

O loop `for` baseado em intervalo (discutido no Capítulo 5, "Loops e Expressões Relacionais," e no Capítulo 16) simplifica a escrita de loops para arrays embutidos e para classes, como `std::string` e os containers da STL, que têm os métodos `begin()` e `end()` identificando um intervalo. O loop aplica a ação indicada a cada elemento no array ou container:

```cpp
double precos[5] = {4.99, 10.99, 6.87, 7.99, 8.49};
for (double x : precos)
    std::cout << x << std::endl;
```

Aqui, `x` assume o valor de cada elemento em `precos` por vez. Uma maneira mais fácil e segura de fazer isso é usar `auto` para declarar `x`; o compilador deduzirá o tipo a partir das informações na declaração de `precos`:

```cpp
double precos[5] = {4.99, 10.99, 6.87, 7.99, 8.49};
for (auto x : precos)
    std::cout << x << std::endl;
```

Se sua intenção for que o loop modifique elementos do array ou container, use um tipo de referência:

```cpp
std::vector<int> vi(6);
for (auto & x: vi) // usar referência se o loop alterar os conteúdos
    x = std::rand();
```

**Novos Containers da STL**

O C++11 adiciona `forward_list`, `unordered_map`, `unordered_multimap`, `unordered_set` e `unordered_multiset` à sua coleção de containers da STL (veja o Capítulo 16). O container `forward_list` é uma lista simplesmente encadeada que pode ser percorrida em apenas uma direção; é mais simples e mais econômica em espaço do que o container de lista duplamente encadeada. Os outros quatro containers suportam a implementação de tabelas hash.

O C++11 também adiciona o template `array` (discutido no Capítulo 4, "Tipos Compostos," e no Capítulo 16), para o qual se especifica um tipo de elemento e um número fixo de elementos:

```cpp
std::array<int, 360> ar; // array de 360 ints
```

Essa classe template não satisfaz todos os requisitos usuais de template. Por exemplo, como o tamanho é fixo, você não pode usar nenhum método, como `push_back()`, que muda o tamanho de um container. Mas `array` tem os métodos `begin()` e `end()`, que permitem usar muitos dos algoritmos da STL baseados em intervalo com objetos `array`.

**Novos Métodos da STL**

O C++11 adiciona `cbegin()` e `cend()` à lista de métodos da STL. Como `begin()` e `end()`, os novos métodos retornam iteradores para o primeiro elemento e para um após o último elemento de um container, especificando assim um intervalo abrangendo todos os elementos. Além disso, os novos métodos tratam os elementos como se fossem `const`. Da mesma forma, `crbegin()` e `crend()` são versões `const` de `rbegin()` e `rend()`.

Mais significativamente, os containers da STL agora têm construtores de move e operadores de atribuição de move além dos construtores de cópia e operadores de atribuição regulares. Este capítulo descreve a semântica de move mais adiante.

**Atualização do `valarray`**

O template `valarray` foi desenvolvido independentemente da STL, e seu design original impedia o uso de algoritmos da STL baseados em intervalo com objetos `valarray`. O C++11 adiciona duas funções, `begin()` e `end()`, que cada uma recebe um argumento `valarray`. Elas retornam iteradores para o primeiro e para um após o último elemento de um objeto `valarray`, permitindo usar algoritmos da STL baseados em intervalo (veja o Capítulo 16).

**Saída do `export`**

O C++98 introduziu a palavra-chave `export` na esperança de criar uma maneira de separar definições de template em arquivos de interface contendo os protótipos e declarações de template e arquivos de implementação contendo as definições de funções e métodos de template. Isso se mostrou impraticável, e o C++11 encerra esse papel do `export`. No entanto, o padrão retém `export` como palavra-chave para possível uso futuro.

**Colchetes Angulares**

Para evitar confusão com o operador `>>`, o C++ exigia um espaço entre os colchetes em declarações de template aninhadas:

```cpp
std::vector<std::list<int> > vl; // >> não ok
```

O C++11 remove esse requisito:

```cpp
std::vector<std::list<int>> vl; // >> ok no C++11
```

### A Referência Rvalue

A referência tradicional do C++, agora chamada de referência lvalue (lvalue reference), vincula um identificador a um lvalue. Um lvalue é uma expressão, como um nome de variável ou um ponteiro desreferenciado, que representa dados para os quais o programa pode obter um endereço. Originalmente, um lvalue era aquele que poderia aparecer no lado esquerdo de uma instrução de atribuição, mas o advento do modificador `const` permitiu construções que não podem ser atribuídas, mas que ainda são endereçáveis:

```cpp
int n;
int * pt = new int;
const int b = 101;    // não pode atribuir a b, mas &b é válido
int & rn = n;         // n identifica o dado no endereço &n
int & rt = *pt;       // *pt identifica o dado no endereço pt
const int & rb = b;   // b identifica o dado const no endereço &b
```

O C++11 adiciona a referência rvalue (discutida no Capítulo 8), indicada pelo uso de `&&`, que pode se vincular a rvalues — ou seja, valores que podem aparecer no lado direito de uma expressão de atribuição, mas para os quais não se pode aplicar o operador de endereço. Exemplos incluem constantes literais (exceto strings no estilo C, que são avaliadas como endereços), expressões como `x+y` e valores de retorno de função, desde que a função não retorne uma referência:

```cpp
int x = 10;
int y = 23;
int && r1 = 13;
int && r2 = x + y;
double && r3 = std::sqrt(2.0);
```

Observe que `r2` realmente se vincula ao valor para o qual `x + y` avalia naquele momento. Ou seja, `r2` se vincula ao valor `33`, e `r2` não é afetado por mudanças subsequentes em `x` ou `y`.

Curiosamente, vincular um rvalue a uma referência rvalue resulta em o valor sendo armazenado em um local cujo endereço pode ser obtido. Ou seja, embora você não possa aplicar o operador `&` a `13`, você pode aplicá-lo a `r1`. Essa vinculação dos dados a endereços específicos é o que torna possível acessar os dados por meio das referências rvalue.

A Listagem 18.1 fornece um breve exemplo ilustrando alguns desses pontos sobre referências rvalue.

**Listagem 18.1 — `ref_rvalor.cpp`**

```cpp
// ref_rvalor.cpp -- usos simples de referências rvalue
#include <iostream>
inline double f(double tf) { return 5.0 * (tf - 32.0) / 9.0; };
int main()
{
    using namespace std;
    double tc = 21.5;
    double && rd1 = 7.07;
    double && rd2 = 1.8 * tc + 32.0;
    double && rd3 = f(rd2);
    cout << " tc valor e endereço: " << tc  << ", " << &tc  << endl;
    cout << "rd1 valor e endereço: " << rd1 << ", " << &rd1 << endl;
    cout << "rd2 valor e endereço: " << rd2 << ", " << &rd2 << endl;
    cout << "rd3 valor e endereço: " << rd3 << ", " << &rd3 << endl;
    cin.get();
    return 0;
}
```

Eis uma saída de exemplo:

```
 tc valor e endereço: 21.5, 002FF744
rd1 valor e endereço: 7.07, 002FF728
rd2 valor e endereço: 70.7, 002FF70C
rd3 valor e endereço: 21.5, 002FF6F0
```

Uma das principais razões para introduzir a referência rvalue é implementar semântica de move, o próximo tópico neste capítulo.

---

[Anterior](capitulo-17-03-arquivo-avancado-resumo.md) | [Índice](README.md) | [Próximo](capitulo-18-02-move-semantics-novidades-classes.md)
