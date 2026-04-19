# Capitulo 5 - Loops e Expressoes Relacionais (Loops and Relational Expressions)

> Traducao nao oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Texto adaptado para portugues brasileiro com fins educacionais. Paginas originais: 195-252.
> Repositorio: [ring0-study](https://github.com/cnthigu/ring0-study)

## Visao geral do capitulo

- O loop `for`
- O loop `while`
- O loop `do while`
- Range-based `for` (C++11)
- Loops e leitura de texto
- Loops aninhados e arrays bidimensionais
- Operadores relacionais (`==`, `!=`, `<`, `>`, `<=`, `>=`)

## O loop `for`

```cpp
for (inicializacao; condicao; atualizacao) {
    // corpo
}
```

```cpp
#include <iostream>

int main()
{
    for (int i = 0; i < 5; ++i) {
        std::cout << "i = " << i << "\n";
    }
    return 0;
}
```

> **Rode no Visual Studio ou compile com `g++ for.cpp -o for`**

Ordem de execucao:

1. **Inicializacao** roda uma vez (`int i = 0`)
2. **Condicao** e avaliada. Se `false`, sai do loop
3. **Corpo** executa
4. **Atualizacao** roda (`++i`)
5. Volta para o passo 2

### Operadores de incremento e decremento

```cpp
int x = 5;
++x;     // pre-incremento: incrementa, depois retorna (6)
x++;     // pos-incremento: retorna o valor, depois incrementa (5, mas x vira 7)

int a = ++x;   // x vira 8, a == 8
int b = x++;   // b == 8, depois x vira 9
```

> **Em C++, prefira `++i` (pre)** sobre `i++` (pos) em loops, especialmente com iteradores complexos. Para `int` puro nao tem diferenca de desempenho, mas e habito util.

### Operadores compostos

```cpp
x += 5;    // x = x + 5
x -= 5;    // x = x - 5
x *= 2;    // x = x * 2
x /= 3;    // x = x / 3
x %= 7;    // x = x % 7
```

## Operadores relacionais

| Operador | Significado |
|---|---|
| `==` | igual a |
| `!=` | diferente de |
| `<` | menor que |
| `>` | maior que |
| `<=` | menor ou igual |
| `>=` | maior ou igual |

```cpp
if (x == 5) { }
if (x != y) { }
if (x < 10 && y > 0) { }
```

> **Erro classico**: `if (x = 5)` (atribuicao!) em vez de `if (x == 5)` (comparacao). O compilador moderno geralmente avisa, mas e bom ter cuidado.

### Em C++ moderno: spaceship operator (C++20)

```cpp
#include <compare>

int a = 5, b = 7;
auto r = a <=> b;        // retorna std::strong_ordering
if (r < 0)  { /* a < b */ }
if (r == 0) { /* a == b */ }
if (r > 0)  { /* a > b */ }

// permite gerar todos os operadores relacionais de uma vez:
struct Ponto {
    int x, y;
    auto operator<=>(const Ponto&) const = default;  // gera ==, !=, <, >, <=, >=
};
```

## O loop `while`

```cpp
while (condicao) {
    // corpo
}
```

```cpp
int i = 0;
while (i < 5) {
    std::cout << i << " ";
    ++i;
}
```

`while` e equivalente a um `for (; condicao; )`. Use quando voce **nao sabe** quantas iteracoes vao ocorrer.

## O loop `do while`

```cpp
do {
    // corpo (sempre executa pelo menos uma vez)
} while (condicao);
```

```cpp
int idade;
do {
    std::cout << "Idade (1-120): ";
    std::cin >> idade;
} while (idade < 1 || idade > 120);
```

Use quando o corpo **precisa** rodar pelo menos uma vez. Bom para ler entrada com validacao.

## Range-based `for` (C++11)

A forma moderna e mais legivel de iterar sobre containers:

```cpp
#include <vector>
#include <string>

int main()
{
    std::vector<int> nums = {1, 2, 3, 4, 5};

    for (int n : nums) {
        std::cout << n << " ";
    }

    for (auto n : nums) {           // auto deduz int
        std::cout << n << " ";
    }

    for (auto& n : nums) {          // & = referencia, MODIFICA o original
        n *= 2;
    }

    for (const auto& n : nums) {    // const& = leitura sem copia (PADRAO)
        std::cout << n << " ";
    }

    std::string s = "Hello";
    for (char c : s) {
        std::cout << c << "\n";
    }
    return 0;
}
```

> **Rode no Visual Studio ou compile com `g++ -std=c++17 range.cpp -o range`**

> **Regra pratica**: para iterar so lendo, use `for (const auto& x : container)`. Para modificar, `for (auto& x : container)`. Para copiar (raro), `for (auto x : container)`.

### Em C++ moderno (20): `std::ranges`

```cpp
#include <ranges>
#include <vector>
#include <iostream>

int main()
{
    std::vector<int> nums = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    auto pares_dobrados = nums
        | std::views::filter([](int x){ return x % 2 == 0; })
        | std::views::transform([](int x){ return x * 2; });

    for (int n : pares_dobrados) {
        std::cout << n << " ";   // 4 8 12 16 20
    }
    return 0;
}
```

`std::ranges` (C++20) traz programacao funcional para C++: filter, transform, take, drop, reverse, etc., compostos por pipe `|`. Lazy evaluation (so calcula quando voce itera).

## Loops aninhados e arrays 2D

```cpp
int matriz[3][4] = {
    {1, 2, 3, 4},
    {5, 6, 7, 8},
    {9, 10, 11, 12}
};

for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 4; ++j) {
        std::cout << matriz[i][j] << " ";
    }
    std::cout << "\n";
}
```

Em C++ moderno:

```cpp
#include <array>

std::array<std::array<int, 4>, 3> mat = {{
    {1, 2, 3, 4},
    {5, 6, 7, 8},
    {9, 10, 11, 12}
}};

for (const auto& linha : mat) {
    for (int v : linha) std::cout << v << " ";
    std::cout << "\n";
}
```

Ou com `std::vector<std::vector<int>>` quando o tamanho varia.

## Lendo entrada em loop

### Lendo numeros ate condicao

```cpp
#include <iostream>

int main()
{
    int n;
    int soma = 0;
    int count = 0;
    std::cout << "Digite numeros (0 para parar): ";
    while (std::cin >> n && n != 0) {
        soma += n;
        ++count;
    }
    if (count > 0) {
        std::cout << "Media: " << double(soma) / count << "\n";
    }
    return 0;
}
```

`std::cin >> n` retorna o proprio `cin`, que num contexto bool e `true` se a leitura deu certo. Detecta EOF (`Ctrl+Z` Windows / `Ctrl+D` Linux) e dados invalidos.

### Lendo caractere a caractere

```cpp
char c;
while (std::cin.get(c)) {       // le qualquer caractere, ate espaco/newline
    std::cout.put(c);
}
```

### Lendo linha inteira

```cpp
std::string linha;
while (std::getline(std::cin, linha)) {
    std::cout << "Voce digitou: " << linha << "\n";
}
```

## `break`, `continue`, `goto`

```cpp
for (int i = 0; i < 100; ++i) {
    if (i == 50) break;        // sai do loop
    if (i % 2 == 0) continue;  // pula pra proxima iteracao
    std::cout << i << " ";
}
```

`goto` existe em C++ mas **e considerado mau estilo**. Existe um caso aceitavel: sair de loops profundamente aninhados (em C++ moderno, melhor extrair para uma funcao e usar `return`).

## Loops infinitos

```cpp
while (true)  { /* ... break para sair */ }
for (;;)      { /* ... break para sair */ }
do { /* ... */ } while (true);
```

## Resumo

- `for` quando voce sabe o numero de iteracoes
- `while` quando depende de uma condicao desconhecida
- `do while` quando precisa executar pelo menos uma vez
- **Range-based `for`** (C++11) e o jeito moderno de iterar containers
- Em C++20, **`std::ranges`** traz operacoes funcionais compostaveis
- Use `++i` em vez de `i++` por habito
- `break` sai, `continue` pula iteracao
- Evite `goto`

## Proximo capitulo

[Capitulo 6 - Branching e Operadores Logicos](capitulo-06.md): `if`, `else`, `switch`, operadores logicos `&&`, `||`, `!`, e `<cctype>` para classificacao de caracteres.
