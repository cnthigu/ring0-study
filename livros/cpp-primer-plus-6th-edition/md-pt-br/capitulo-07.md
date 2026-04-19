# Capitulo 7 - Funcoes: Modulos de Programacao em C++ (Functions: C++ Programming Modules)

> Traducao nao oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Texto adaptado para portugues brasileiro com fins educacionais. Paginas originais: 305-378.
> Repositorio: [ring0-study](https://github.com/cnthigu/ring0-study)

## Visao geral do capitulo

- Revisao de funcoes e seus argumentos
- Passagem por valor (`pass-by-value`)
- Funcoes e arrays
- Funcoes e arrays bidimensionais
- Funcoes e strings C-style
- Funcoes e structs
- Funcoes e objetos `std::string`
- Funcoes e `std::array`
- Recursao
- Ponteiros para funcoes (`function pointers`)

## Revisao: anatomia de uma funcao

Toda funcao precisa de:

1. **Prototipo** (declaracao): tipo de retorno + nome + tipos dos parametros
2. **Definicao**: cabecalho + corpo
3. **Chamada**: invocar a funcao

```cpp
#include <iostream>

double cubic(double x);             // prototipo

int main()
{
    double side = 2.0;
    std::cout << cubic(side) << "\n";   // chamada
    return 0;
}

double cubic(double x)              // definicao
{
    return x * x * x;
}
```

## Passagem por valor (pass-by-value)

Em C++ por padrao, **argumentos sao copiados** para a funcao:

```cpp
void aumenta(int x) {
    x = x + 1;          // modifica a COPIA
}

int main()
{
    int n = 5;
    aumenta(n);
    std::cout << n;     // ainda 5! a funcao mexeu numa copia
}
```

Para a funcao **modificar** o original, voce passa por **referencia** ou **ponteiro**.

## Passagem por referencia (`&`) - C++

```cpp
void aumenta(int& x) {       // & = referencia
    x = x + 1;
}

int main()
{
    int n = 5;
    aumenta(n);
    std::cout << n;          // agora 6
}
```

Uma referencia e um **alias** para a variavel original. Sem copia, sem ponteiro explicito.

### Por valor vs por referencia vs `const&`

```cpp
void f1(int x);              // copia (rapido para tipos pequenos)
void f2(int& x);             // referencia mutavel (modifica original)
void f3(const int& x);       // referencia leitura-apenas (sem copia, sem mutacao)

void g1(std::string s);          // COPIA a string (caro!)
void g2(std::string& s);         // muda a string original
void g3(const std::string& s);   // le sem copiar (PADRAO RECOMENDADO)
```

> **Regra pratica em C++ moderno**:
>
> - Tipos pequenos (`int`, `double`, ponteiros): **passe por valor**
> - Tipos grandes que voce so quer ler: **passe por `const T&`**
> - Quando voce **vai modificar**: passe por `T&`
> - Quando voce **vai consumir** (mover) o argumento: passe por valor + `std::move`

## Passagem por ponteiro (estilo C)

```cpp
void aumenta(int* x) {
    *x = *x + 1;
}

int main()
{
    int n = 5;
    aumenta(&n);                 // passa o endereco
    std::cout << n;              // 6
}
```

Funciona, mas em C++ moderno **prefira referencias** quando o argumento sempre existe (nunca e nullptr). Use ponteiro quando ele pode ser opcional (`nullptr`).

### Em C++ moderno: `std::optional` (C++17)

Em vez de ponteiro para "talvez existe":

```cpp
#include <optional>

std::optional<int> dividir(int a, int b) {
    if (b == 0) return std::nullopt;
    return a / b;
}

if (auto r = dividir(10, 3); r.has_value()) {
    std::cout << *r;     // 3
}
```

## Funcoes e arrays

Quando voce passa um array, **ele decai para ponteiro**:

```cpp
int soma(int arr[], int tamanho) {     // equivale a: int* arr
    int total = 0;
    for (int i = 0; i < tamanho; ++i) total += arr[i];
    return total;
}

int main()
{
    int xs[5] = {1, 2, 3, 4, 5};
    std::cout << soma(xs, 5);   // PRECISA passar o tamanho separado!
}
```

`sizeof(arr)` dentro da funcao retorna o tamanho do **ponteiro** (8 em x64), nao do array original. Por isso passar o tamanho separado.

### Em C++ moderno: `std::span` (C++20) ou `std::vector`

```cpp
#include <span>
#include <vector>
#include <numeric>

int soma(std::span<const int> dados) {
    int total = 0;
    for (int x : dados) total += x;
    return total;
}

int main()
{
    int                arr[5] = {1, 2, 3, 4, 5};
    std::vector<int>   v      = {10, 20, 30};
    std::array<int,4>  a      = {1, 1, 1, 1};

    soma(arr);   // OK
    soma(v);     // OK
    soma(a);     // OK
}
```

`std::span` aceita qualquer sequencia contigua e ja vem com tamanho. Ou use `std::accumulate` direto:

```cpp
int total = std::accumulate(v.begin(), v.end(), 0);
```

## Funcoes e arrays 2D

Estilo C (rigido - precisa do tamanho da segunda dimensao):

```cpp
int soma_matriz(int mat[][4], int linhas) {     // [4] obrigatorio!
    int total = 0;
    for (int i = 0; i < linhas; ++i)
        for (int j = 0; j < 4; ++j)
            total += mat[i][j];
    return total;
}
```

Estilo moderno:

```cpp
int soma_matriz(const std::vector<std::vector<int>>& mat) {
    int total = 0;
    for (const auto& linha : mat)
        for (int v : linha) total += v;
    return total;
}
```

## Funcoes e strings C-style

```cpp
int contar_letras(const char* s) {
    int n = 0;
    while (*s) {        // ate encontrar '\0'
        ++n; ++s;
    }
    return n;
}
```

Em C++ moderno, **prefira sempre** `std::string_view` (C++17):

```cpp
#include <string_view>

int contar_letras(std::string_view s) {
    return s.size();        // O(1)
}

contar_letras("ola");                       // OK
contar_letras(std::string{"ola"});          // OK
contar_letras(std::string_view{"ola", 3});  // OK
```

`std::string_view` e uma vista sem copia: `const char*` + tamanho. Substitui as APIs C antigas.

## Funcoes e structs

```cpp
struct Ponto { double x, y; };

double distancia(const Ponto& a, const Ponto& b) {
    double dx = a.x - b.x;
    double dy = a.y - b.y;
    return std::sqrt(dx*dx + dy*dy);
}

Ponto translacao(const Ponto& p, double dx, double dy) {
    return {p.x + dx, p.y + dy};
}
```

Passe structs por `const T&` para evitar copia desnecessaria.

## Funcoes e `std::string`

```cpp
#include <string>

std::string maiusculas(std::string s) {     // copia, modifica e retorna
    for (auto& c : s) c = std::toupper(static_cast<unsigned char>(c));
    return s;
}

bool comeca_com(std::string_view texto, std::string_view prefixo) {
    return texto.starts_with(prefixo);      // C++20
}
```

## Recursao

```cpp
int fatorial(int n) {
    if (n <= 1) return 1;       // caso base
    return n * fatorial(n - 1); // chamada recursiva
}

int main()
{
    std::cout << fatorial(5);   // 120
}
```

> **Cuidado**: cada chamada recursiva consome stack. Em recursao profunda (>10 mil) voce pode estourar a stack. Para isso, **converta em iteracao** ou use **tail recursion** (alguns compiladores otimizam).

### Fibonacci - exemplo classico

```cpp
// LENTO: O(2^n) - calcula a mesma coisa varias vezes
int fib(int n) {
    if (n < 2) return n;
    return fib(n-1) + fib(n-2);
}

// RAPIDO: O(n) com iteracao
int fib_iter(int n) {
    if (n < 2) return n;
    int a = 0, b = 1;
    for (int i = 2; i <= n; ++i) {
        int t = a + b;
        a = b;
        b = t;
    }
    return b;
}
```

## Ponteiros para funcoes (function pointers)

Funcoes tem endereco igual variaveis. Voce pode guardar em ponteiros e passar como argumento:

```cpp
int dobrar(int x) { return 2 * x; }
int triplicar(int x) { return 3 * x; }

int main()
{
    int (*op)(int);     // declara ponteiro pra funcao (int -> int)

    op = dobrar;
    std::cout << op(5) << "\n";       // 10

    op = triplicar;
    std::cout << op(5) << "\n";       // 15
}
```

Sintaxe horrivel. Em C++ moderno, **use `std::function` ou lambdas**.

### Em C++ moderno: lambdas (C++11)

```cpp
auto dobrar    = [](int x) { return 2 * x; };
auto triplicar = [](int x) { return 3 * x; };

std::cout << dobrar(5);      // 10
std::cout << triplicar(5);   // 15
```

### `std::function` - guarda qualquer "callable"

```cpp
#include <functional>

std::function<int(int)> op = dobrar;
op = triplicar;
op = [](int x) { return x * x; };
op = std::function<int(int)>([](int x){ return -x; });

std::cout << op(7);
```

`std::function` aceita funcoes, lambdas, functors (objetos com `operator()`) e ate metodos via `std::bind`.

### Funcao que recebe outra funcao

```cpp
#include <vector>

void aplicar(const std::vector<int>& v, std::function<void(int)> f) {
    for (int x : v) f(x);
}

int main()
{
    std::vector<int> nums = {1, 2, 3, 4, 5};
    aplicar(nums, [](int x){ std::cout << x*x << " "; });   // 1 4 9 16 25
}
```

### Lambdas - sintaxe completa

```cpp
auto soma = [](int a, int b) -> int {
    return a + b;
};

int multiplicador = 3;
auto multiplicar = [multiplicador](int x) {     // captura por valor
    return x * multiplicador;
};

int contador = 0;
auto incrementar = [&contador]() {              // captura por referencia
    ++contador;
};

// Captura tudo por valor: [=]
// Captura tudo por referencia: [&]
auto exemplo = [=](int x) { return x + multiplicador; };
```

## Resumo

- C++ tem **passagem por valor** (padrao), **por referencia** (`&`) e **por ponteiro** (`*`)
- Para tipos grandes que voce so le, passe `const T&`
- Arrays decaem em ponteiros - **passe o tamanho** ou use `std::span` (C++20)
- Use `std::string_view` (C++17) no lugar de `const char*`
- `std::optional` (C++17) substitui ponteiros nullable em retornos
- Recursao funciona, mas cuidado com stack overflow
- Para callbacks, **prefira lambdas e `std::function`** sobre ponteiros para funcao
- Lambdas (`[](){}`) sao um dos recursos mais usados de C++ moderno

## Proximo capitulo

[Capitulo 8 - Aventuras com Funcoes](capitulo-08.md): `inline`, **referencias** em detalhe, **argumentos default**, **sobrecarga (overloading)** e **templates**.
