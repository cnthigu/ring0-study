# Capitulo 3 - Lidando com Dados (Dealing with Data)

> Traducao nao oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Texto adaptado para portugues brasileiro com fins educacionais. Paginas originais: 65-114.
> Repositorio: [ring0-study](https://github.com/cnthigu/ring0-study)

## Visao geral do capitulo

- Regras para nomes de variaveis em C++
- Tipos inteiros: `short`, `int`, `long`, `long long`, `char`, `bool` (signed e unsigned)
- O header `<climits>` (limites do sistema)
- Literais numericos (decimal, octal, hexadecimal, binario)
- O qualificador `const`
- Tipos de ponto flutuante: `float`, `double`, `long double`
- O header `<cfloat>`
- Operadores aritmeticos
- Conversoes automaticas e `static_cast`

## Variaveis simples

Para guardar informacao, o programa precisa saber:

1. **Onde** esta na memoria (endereco)
2. **Qual valor** esta armazenado
3. **Que tipo** de dado e

```cpp
int braincount;     // declara - reserva memoria para um inteiro
braincount = 5;     // armazena 5 nessa memoria
```

### Regras para nomes de variaveis

- Caracteres permitidos: letras, digitos e `_` (underscore)
- O primeiro caractere **nao** pode ser digito
- C++ e case sensitive: `Time`, `time` e `TIME` sao diferentes
- Nao pode usar palavras-chave da linguagem (`int`, `double`, `return`, etc.)
- Nomes que comecam com `_` ou `__` sao **reservados** para o compilador. Nao use.
- Sem limite de tamanho

```cpp
int poodle;          // valido
int Poodle;          // valido, distinto de poodle
int POODLE;          // valido, ainda mais distinto
int my_stars3;       // valido
int _Mystars3;       // valido tecnicamente, mas RESERVADO. Evite
int 4ever;           // INVALIDO - comeca com digito
int double;          // INVALIDO - palavra-chave
```

## Tipos inteiros

C++ oferece varios tipos inteiros para diferentes faixas de valores:

| Tipo | Tamanho minimo (C++) | Faixa tipica em x64 |
|---|---|---|
| `bool` | 1 byte | `true` ou `false` |
| `char` / `signed char` / `unsigned char` | 1 byte | -128 a 127 / 0 a 255 |
| `short` / `unsigned short` | 2 bytes | -32 768 a 32 767 / 0 a 65 535 |
| `int` / `unsigned int` | 2 bytes (na pratica 4) | -2.1B a 2.1B / 0 a 4.2B |
| `long` / `unsigned long` | 4 bytes | -2.1B a 2.1B (Windows) / 8 bytes (Linux x64) |
| `long long` / `unsigned long long` (C++11) | 8 bytes | aprox. +-9.2 quintilhoes |

> **Importante**: o padrao C++ so garante **tamanhos minimos**. Para tamanhos exatos use `<cstdint>`: `int8_t`, `int16_t`, `int32_t`, `int64_t`, `uint32_t`, etc.

### Em C++ moderno (11+)

```cpp
#include <cstdint>

int32_t   id        = 12345;        // exatamente 32 bits, com sinal
uint64_t  timestamp = 1700000000;   // exatamente 64 bits, sem sinal
int8_t    flag      = -1;           // exatamente 8 bits

std::size_t tamanho = 100;          // tipo "natural" para tamanhos/indices
```

Use **sempre** `<cstdint>` quando o tamanho importa (protocolos de rede, formatos de arquivo, drivers, anti-cheat, etc.).

## Limites do sistema (`<climits>`)

```cpp
#include <iostream>
#include <climits>

int main()
{
    std::cout << "int min:           " << INT_MIN  << "\n";
    std::cout << "int max:           " << INT_MAX  << "\n";
    std::cout << "long max:          " << LONG_MAX << "\n";
    std::cout << "long long max:     " << LLONG_MAX << "\n";
    std::cout << "unsigned int max:  " << UINT_MAX << "\n";
    std::cout << "tamanho de int:    " << sizeof(int) << " bytes\n";
    return 0;
}
```

> **Rode no Visual Studio ou compile com `g++ limites.cpp -o limites`**

`sizeof(tipo)` retorna o numero de bytes que o tipo ocupa.

### Em C++ moderno: `<limits>`

```cpp
#include <limits>
int max_int = std::numeric_limits<int>::max();
int min_int = std::numeric_limits<int>::min();
double eps  = std::numeric_limits<double>::epsilon();   // menor diferenca representavel
```

## Inicializacao

```cpp
int n_chairs = 5;     // estilo C
int n_chairs(5);      // estilo "construtor" (C++)
int n_chairs{5};      // brace-init (C++11) - PREFIRA ESTE
int n_chairs{};       // zero-init: n_chairs == 0
```

A vantagem de `{}` e que o compilador **rejeita conversoes que perdem dados**:

```cpp
int  a = 3.14;     // OK silenciosamente, a == 3 (perdeu o .14)
int  b{3.14};      // ERRO de compilacao - narrowing conversion
char c{300};       // ERRO de compilacao - 300 nao cabe em char
```

## Literais numericos

```cpp
int dec  = 42;        // decimal
int oct  = 042;       // OCTAL (comeca com 0) -> 34 em decimal
int hex  = 0x2A;      // HEXADECIMAL -> 42
int hex2 = 0X2a;      // tambem hex
int bin  = 0b101010;  // BINARIO (C++14) -> 42

// separadores de digitos (C++14)
int milhao   = 1'000'000;
long bilhao  = 1'000'000'000L;

// sufixos de tipo
int    a = 42;
long   b = 42L;
long long c = 42LL;
unsigned int  d = 42U;
unsigned long e = 42UL;
```

## Mostrando em bases diferentes

```cpp
#include <iostream>

int main()
{
    using namespace std;
    int n = 255;
    cout << dec << n << "\n";    // 255
    cout << hex << n << "\n";    // ff
    cout << oct << n << "\n";    // 377
    cout << dec;                  // volta pra decimal
    return 0;
}
```

## `char` - inteiro pequeno tambem usado para caracteres

```cpp
char c = 'A';                // 'A' tem valor 65 na tabela ASCII
std::cout << c   << "\n";    // imprime: A
std::cout << int(c) << "\n"; // imprime: 65

char tab = '\t';             // tabulacao
char nl  = '\n';             // nova linha
char nul = '\0';             // caractere nulo (terminador de string C)
```

`char` pode ser `signed` ou `unsigned` por padrao; depende do compilador. Para guardar **bytes** prefira `unsigned char` ou `std::byte` (C++17).

## `bool`

```cpp
bool ativo = true;
bool ok    = false;
bool zero  = 0;     // false
bool um    = 1;     // true
bool x     = 42;    // true (qualquer nao-zero converte para true)

if (ativo) { /* ... */ }
```

`true` e `false` sao palavras-chave (nao constantes inteiras como em C antigo).

## O qualificador `const`

```cpp
const int MESES_NO_ANO = 12;
const double PI = 3.14159265358979;

MESES_NO_ANO = 13;   // ERRO - nao pode modificar
```

`const` substitui `#define` para constantes:

```cpp
#define MESES 12        // estilo C, evite em C++
const int MESES = 12;   // estilo C++, com tipo!
```

A versao `const` tem **tipo conhecido** pelo compilador, respeita escopos e nao tem efeitos colaterais de macros.

### Em C++ moderno: `constexpr` e `consteval`

```cpp
constexpr int MAX = 100;                // calculado em tempo de compilacao
constexpr double area_circulo(double r) { return 3.14159 * r * r; }
constexpr double a = area_circulo(2.5); // computado durante a compilacao!

// C++20: consteval forca avaliacao em tempo de compilacao
consteval int dobrar(int x) { return 2 * x; }
constexpr int valor = dobrar(21);       // OK
// int x = 5; int y = dobrar(x);        // ERRO - x nao e constante
```

## Tipos de ponto flutuante

| Tipo | Tamanho tipico | Precisao |
|---|---|---|
| `float` | 4 bytes | 6-7 digitos decimais |
| `double` | 8 bytes | 15-16 digitos decimais |
| `long double` | 8/12/16 bytes | depende do compilador |

```cpp
float  a = 3.14F;        // sufixo F -> float
double b = 3.14;         // sem sufixo -> double (PADRAO)
long double c = 3.14L;   // sufixo L -> long double

double x = 1.5e3;        // 1500.0 (notacao cientifica)
double y = 2.5E-4;       // 0.00025
```

> **Regra pratica**: use `double` por padrao. So use `float` se voce tem **muitos** valores e o consumo de memoria importa (ex: arrays gigantes em jogos/simulacoes).

### Limites e cuidados com float

```cpp
#include <iostream>
#include <iomanip>

int main()
{
    using namespace std;
    float  tax = 47.123456789F;
    double bay = 47.123456789;
    cout << fixed << setprecision(15);
    cout << "float : " << tax << "\n";   // perde precisao depois do 7o digito
    cout << "double: " << bay << "\n";
    return 0;
}
```

> **Rode no Visual Studio ou compile com `g++ float.cpp -o float`**

> **Cuidado**: nunca compare floats com `==`. `0.1 + 0.2 != 0.3` em float! Use:
>
> ```cpp
> #include <cmath>
> if (std::abs(a - b) < 1e-9) { /* iguais o suficiente */ }
> ```

## Operadores aritmeticos

```cpp
int a = 10, b = 3;

a + b   // soma:           13
a - b   // subtracao:       7
a * b   // multiplicacao:  30
a / b   // divisao INT:     3  (descarta o .333)
a % b   // resto/modulo:    1

double x = 10, y = 3;
x / y   // divisao FLOAT:   3.3333...
```

> **Atencao**: `/` em inteiros **trunca**. Para divisao real, pelo menos um operando deve ser float:
>
> ```cpp
> int a = 7, b = 2;
> double r1 = a / b;             // r1 == 3.0 (calculou int, depois converteu)
> double r2 = double(a) / b;     // r2 == 3.5 (calculou float)
> double r3 = a / 2.0;           // r3 == 3.5
> ```

### Precedencia

Igual a matematica: `*`, `/`, `%` antes de `+`, `-`. Use parenteses quando duvidar:

```cpp
int x = 2 + 3 * 4;    // 14, nao 20
int y = (2 + 3) * 4;  // 20
```

## Conversoes de tipo

### Conversao implicita (automatica)

```cpp
int    n = 5;
double d = n;          // OK: int promove pra double, sem perda

double pi = 3.14;
int    i  = pi;        // OK MAS perde dados, vira 3 (truncamento)
```

C++ promove automaticamente tipos menores para maiores em expressoes mistas.

### Conversao explicita: `static_cast`

```cpp
int total = 47, count = 3;
double media1 = total / count;                  // 15 (calculou em int!)
double media2 = static_cast<double>(total) / count;  // 15.6666...
```

`static_cast` e o **cast moderno** preferido em C++. Existem outros (`const_cast`, `dynamic_cast`, `reinterpret_cast`) com usos especificos.

> **Evite o cast estilo C** (`(double)total`): o `static_cast` e mais visivel, mais facil de procurar com grep e o compilador checa mais.

### `auto` deduz o tipo (C++11)

```cpp
auto n  = 42;       // int
auto x  = 3.14;     // double
auto s  = "ola";    // const char*
auto b  = true;     // bool

auto z  = 1L * 1L;  // long
auto u  = 1u;       // unsigned int
```

`auto` e essencial para tipos longos:

```cpp
std::map<std::string, std::vector<int>>::iterator it = mapa.begin(); // verboso
auto it = mapa.begin();                                              // bem melhor
```

## Resumo

- C++ tem varios tipos inteiros (`bool`, `char`, `short`, `int`, `long`, `long long`) e tres ponto flutuante (`float`, `double`, `long double`)
- Use `<cstdint>` quando o tamanho exato importa (`int32_t`, `uint64_t`)
- Prefira inicializacao com chaves: `int x{42};` (detecta narrowing)
- Use `const`/`constexpr` em vez de `#define`
- Cuidado com divisao inteira (`7 / 2 == 3`)
- Use `static_cast` para conversoes explicitas
- `auto` deduz o tipo, util para tipos longos

## Proximo capitulo

[Capitulo 4 - Tipos Compostos (Compound Types)](capitulo-04.md): arrays, strings, structs, unions, enums e ponteiros.
