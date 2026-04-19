# Capitulo 4 - Tipos Compostos (Compound Types)

> Traducao nao oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Texto adaptado para portugues brasileiro com fins educacionais. Paginas originais: 115-194.
> Repositorio: [ring0-study](https://github.com/cnthigu/ring0-study)

## Visao geral do capitulo

- Arrays (em C++)
- Strings: estilo C (`char[]`) e a classe `std::string`
- Structs e seu uso
- Unions e enums
- Ponteiros (`pointer`), `new` e `delete` (free store)
- Aritmetica de ponteiros
- Combinacoes de tipos
- Alternativas modernas: `std::vector` e `std::array`

## Arrays

Um **array** e uma colecao de valores do mesmo tipo armazenados em posicoes contiguas de memoria.

```cpp
int meses[12];                       // 12 ints, indices 0..11
double precos[5] = {1.0, 2.5, 3.7, 4.2, 0.8};
int    zeros[10]   = {0};            // todos zerados
int    parcial[5]  = {1, 2, 3};      // {1, 2, 3, 0, 0}
int    auto_size[] = {10, 20, 30};   // tamanho deduzido = 3

double primeiro = precos[0];         // primeiro elemento
double ultimo   = precos[4];         // ultimo (indice 4, nao 5!)
```

> **Cuidado**: o compilador **nao** verifica limites em tempo de execucao. Acessar `precos[10]` (fora do array) e **undefined behavior** e pode corromper memoria.

### Inicializacao com chaves (C++11)

```cpp
int xs[5]{};               // todos zero
int ys[5]{1, 2, 3};        // {1, 2, 3, 0, 0}
int zs[]{1, 2, 3, 4, 5};   // tamanho deduzido = 5
```

### Em C++ moderno: `std::array` (C++11)

```cpp
#include <array>

std::array<int, 5> nums = {1, 2, 3, 4, 5};

nums.size();           // 5 (sem precisar saber de cor)
nums.at(2);            // 3 - com checagem de limites (lanca excecao)
nums[2];               // 3 - sem checagem (igual array C)
nums.front();          // 1
nums.back();           // 5

for (int n : nums) {   // range-based for (C++11)
    std::cout << n << " ";
}
```

`std::array` tem o mesmo desempenho de array C, mas conhece o proprio tamanho e tem metodos uteis.

## Strings

### Estilo C: array de `char` terminado em `'\0'`

```cpp
char nome1[20]   = "Carniato";       // null-terminator implicito
char nome2[]     = "carniato";       // tamanho automatico (9: 8 letras + '\0')
char letras[5]   = {'C', 'N', 'T', '\0'};

std::cout << nome1 << "\n";          // imprime ate o '\0'
std::cout << strlen(nome1) << "\n";  // 8 (NAO conta o '\0')
std::cout << sizeof(nome1) << "\n";  // 20 (tamanho do array)
```

### `std::string` (preferida em C++)

```cpp
#include <string>

std::string nome = "Carniato";
std::string sobrenome("Higor");
std::string completo = nome + " " + sobrenome;   // concatenacao com +!

nome.size();             // 8
nome.length();           // 8 (mesmo que size)
nome[0];                 // 'C'
nome.empty();            // false
nome += "!";             // append
nome.find("rni");        // 2 (indice onde encontrou)
nome.substr(0, 3);       // "Car"
```

### Por que preferir `std::string`?

```cpp
// estilo C - tem que se preocupar com tamanho do buffer
char a[20], b[20];
strcpy(a, "Hello");
strcat(a, " world");          // se nao couber, BUFFER OVERFLOW
strcpy(b, a);
if (strcmp(a, b) == 0) {}     // comparar
```

```cpp
// estilo C++ moderno - sem dor de cabeca
std::string a = "Hello";
a += " world";                // gerencia memoria sozinha
std::string b = a;
if (a == b) {}                // comparar com ==
```

`std::string` cuida de alocar/redimensionar memoria, tem metodos uteis e e segura. Use **sempre** que possivel.

### Lendo strings do teclado

```cpp
#include <iostream>
#include <string>

int main()
{
    std::string nome;
    std::cout << "Nome: ";
    std::cin >> nome;             // ate o primeiro espaco

    std::string linha;
    std::cin.ignore();            // descarta o '\n' deixado por >>
    std::cout << "Linha: ";
    std::getline(std::cin, linha); // linha inteira ate Enter
    return 0;
}
```

## Structs

`struct` agrupa varios campos (de tipos possivelmente diferentes) em um unico tipo:

```cpp
struct Inflavel {
    std::string nome;
    float       volume;
    double      preco;
};

int main()
{
    Inflavel goose;
    goose.nome   = "Glorious Gloria";
    goose.volume = 1.88;
    goose.preco  = 29.99;

    Inflavel pal{"Audacious Arthur", 3.12, 32.99};   // brace-init
    return 0;
}
```

### Em C++ moderno: structured bindings (C++17)

```cpp
struct Ponto { int x; int y; };

Ponto p{10, 20};
auto [x, y] = p;       // desestrutura: x == 10, y == 20

// tambem funciona com pair, tuple, e mapas
std::map<std::string, int> idades = {{"cnt", 24}, {"alice", 30}};
for (const auto& [nome, idade] : idades) {
    std::cout << nome << " tem " << idade << "\n";
}
```

## Unions

`union` armazena tipos diferentes **no mesmo lugar de memoria**, um de cada vez:

```cpp
union Numero {
    int    i;
    double d;
    char   c[8];
};

Numero n;
n.i = 42;          // agora ele e int
n.d = 3.14;        // agora ele e double - sobrescreveu o int!
```

Uso classico: economia de memoria em sistemas embarcados, ou interpretar bytes de formas diferentes.

### Em C++ moderno: `std::variant` (C++17)

```cpp
#include <variant>

std::variant<int, double, std::string> v;
v = 42;
v = 3.14;
v = std::string{"ola"};

if (auto* s = std::get_if<std::string>(&v)) {
    std::cout << "string: " << *s << "\n";
}

std::visit([](auto&& x){ std::cout << x << "\n"; }, v);
```

`std::variant` e **type-safe**: ele lembra qual tipo esta armazenado. Use no lugar de `union` no C++ moderno, exceto em codigo de baixo nivel onde precisa do layout exato.

## Enums

```cpp
enum Cor { vermelho, azul, verde, amarelo };  // valores: 0, 1, 2, 3

Cor c = azul;        // c == 1

enum Resp { sim = 1, talvez = 2, nao = 4 };   // valores explicitos
```

### Em C++ moderno: `enum class` (C++11)

```cpp
enum class Cor { vermelho, azul, verde };
enum class Status { ok, erro, pendente };

Cor    c = Cor::vermelho;          // PRECISA do prefixo Cor::
Status s = Status::ok;

if (c == Cor::azul) { /* ... */ }
// if (c == 0) { }                  // ERRO - sem conversao implicita
// if (c == Status::ok) { }         // ERRO - tipos diferentes
```

`enum class` e fortemente tipado e nao polui o escopo. **Sempre prefira** sobre `enum` antigo.

## Ponteiros (Pointers)

Um **ponteiro** guarda um endereco de memoria.

```cpp
int n = 42;
int* p = &n;            // p contem o endereco de n
                        // & = "endereco de"

std::cout << n  << "\n";   // 42 - valor de n
std::cout << p  << "\n";   // 0x... - endereco em hexa
std::cout << *p << "\n";   // 42 - valor APONTADO por p
                            // * = "valor em" (dereference)

*p = 100;               // muda n via ponteiro
std::cout << n;         // 100
```

### Sintaxe declarativa

```cpp
int*  p1;       // p1 e ponteiro para int
int  *p2;       // mesma coisa - estilo "C-classic"
int*  p3, p4;   // CUIDADO: p3 e ponteiro, p4 e int!
int*  p5; int* p6;  // melhor: uma declaracao por linha
```

### Ponteiro nulo

```cpp
int* p = nullptr;       // C++11 - PADRAO MODERNO
int* q = NULL;          // estilo C, evite em C++
int* r = 0;             // funciona, mas nao expressivo

if (p == nullptr) { }
if (!p)           { }   // tambem testa nullptr
```

> **Sempre use `nullptr`** em C++. `NULL` e `0` tem ambiguidades em sobrecarga de funcoes.

## `new` e `delete` (alocacao dinamica)

```cpp
int* p = new int;          // aloca um int no heap, p aponta pra ele
*p = 42;
delete p;                  // libera. SEMPRE faca isso!
p = nullptr;               // boa pratica: evita "dangling pointer"

int* arr = new int[100];   // aloca array de 100 ints
arr[0] = 1;
delete[] arr;              // ATENCAO: [] para arrays!
```

Regras de ouro do `new`/`delete`:

1. Cada `new` precisa de exatamente um `delete`
2. Cada `new[]` precisa de exatamente um `delete[]`
3. Misturar `delete` com `new[]` (ou vice-versa) e **undefined behavior**
4. Liberar duas vezes (`double free`) e **undefined behavior**
5. Usar depois de liberar (`use after free`) e **undefined behavior**

### Em C++ moderno: smart pointers (C++11+)

```cpp
#include <memory>

// unique_ptr - dono unico, libera automaticamente
auto p = std::make_unique<int>(42);
*p = 100;
// quando p sai de escopo, ele faz delete sozinho

// shared_ptr - varios donos, libera quando o ultimo morre
auto sp1 = std::make_shared<int>(42);
auto sp2 = sp1;     // ambos apontam pro mesmo, refcount = 2
// quando ambos morrem, libera

// arrays
auto arr = std::make_unique<int[]>(100);
arr[0] = 1;
```

> **Em C++ moderno (11 em diante), voce raramente deveria usar `new`/`delete` manuais.** Use `std::unique_ptr`, `std::shared_ptr`, `std::vector` ou `std::string` que cuidam da memoria sozinhos. Isso elimina **a grande maioria** dos bugs de memoria.

## Aritmetica de ponteiros

```cpp
int arr[5] = {10, 20, 30, 40, 50};
int* p = arr;          // p aponta pro primeiro elemento

std::cout << *p     << "\n";   // 10
std::cout << *(p+1) << "\n";   // 20
std::cout << *(p+2) << "\n";   // 30
std::cout << p[2]   << "\n";   // 30 (notacao equivalente!)

++p;                   // p avanca 1 elemento (sizeof(int) bytes)
std::cout << *p;       // 20
```

`arr[i]` e exatamente equivalente a `*(arr + i)`. O nome do array decai para um ponteiro pro primeiro elemento.

### Diferenca importante

```cpp
int arr[5];
int* p = arr;

sizeof(arr);   // 20 (5 * 4 bytes)
sizeof(p);     // 8 em x64 (tamanho do ponteiro!)
```

Quando voce passa um array para uma funcao, ela recebe so um ponteiro. **Perde** a informacao de tamanho.

## Strings dinamicas com `new`

```cpp
char* nome = new char[20];
strcpy(nome, "Carniato");
delete[] nome;
```

Mas em C++ moderno, sempre prefira:

```cpp
std::string nome = "Carniato";  // simples, seguro, eficiente
```

## Combinacoes: array de structs, ponteiro para struct

```cpp
struct Inflavel { std::string nome; double preco; };

Inflavel guests[2] = {
    {"Bambi", 19.99},
    {"Godzilla", 199.99}
};

std::cout << guests[0].nome << "\n";

Inflavel* pi = &guests[0];
std::cout << pi->nome << "\n";        // -> equivale a (*pi).nome
std::cout << (*pi).nome << "\n";      // mesma coisa
```

## Alternativas modernas a arrays

| Cenario | C-style | C++ moderno |
|---|---|---|
| Tamanho fixo conhecido em tempo de compilacao | `int arr[10]` | `std::array<int, 10>` |
| Tamanho dinamico | `int* arr = new int[n]` | `std::vector<int>` |
| Lista grande, insere/remove no meio | (precisa lista ligada) | `std::list<int>` |
| Visualizar parte de um array sem copiar | `int* p, size_t n` | `std::span<int>` (C++20) |
| String de bytes | `char[]` | `std::string` ou `std::vector<unsigned char>` |
| Visualizar string sem copiar | `const char*` | `std::string_view` (C++17) |

### `std::vector` (essencial)

```cpp
#include <vector>

std::vector<int> v;                       // vazio
std::vector<int> v2 = {1, 2, 3, 4, 5};    // 5 elementos
std::vector<int> v3(10, 0);               // 10 zeros
std::vector<int> v4(10);                  // 10 default-init (zero pra int)

v.push_back(42);          // adiciona no fim
v.pop_back();             // remove do fim
v.size();                 // numero de elementos
v.empty();
v[0]; v.at(0);            // acesso (at lanca excecao se fora)
v.front(); v.back();
v.clear();

for (int x : v) { /* ... */ }

// reservar memoria pra evitar realocacoes
std::vector<int> grande;
grande.reserve(10000);    // ja aloca 10000 slots
```

`vector` e dinamico (cresce sob demanda), tem desempenho proximo a array, e cuida de toda a memoria. Em C++ moderno, **e o container padrao** para colecoes.

### `std::span` (C++20)

```cpp
#include <span>

void imprimir(std::span<int> dados) {
    for (int x : dados) std::cout << x << " ";
}

int arr[5] = {1, 2, 3, 4, 5};
std::vector<int> v = {10, 20, 30};

imprimir(arr);          // funciona
imprimir(v);            // funciona
imprimir({arr, 3});     // primeiros 3 elementos
```

`std::span` e uma **vista** de uma sequencia contigua. Nao copia nada. Substitui o velho par `T*, size_t`.

## Resumo

- Arrays C-style funcionam, mas perdem tamanho ao serem passados como argumento
- Use `std::array` para tamanho fixo, `std::vector` para tamanho dinamico
- `std::string` substitui completamente strings C-style em codigo novo
- Ponteiros sao essenciais para entender C++, mas em codigo moderno prefira `std::unique_ptr`/`std::shared_ptr` em vez de `new`/`delete`
- Use `nullptr` (nao `NULL`)
- Use `enum class` (nao `enum`)
- Use `std::variant` (nao `union`) quando precisa "um de varios tipos"
- C++ moderno (11+) reduziu **drasticamente** a necessidade de gerenciar memoria manualmente

## Proximo capitulo

[Capitulo 5 - Loops e Expressoes Relacionais](capitulo-05.md): `for`, `while`, `do while`, range-based `for` (C++11), operadores relacionais, leitura em loops.
