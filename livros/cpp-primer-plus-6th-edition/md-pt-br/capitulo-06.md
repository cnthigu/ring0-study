# Capitulo 6 - Branching e Operadores Logicos (Branching Statements and Logical Operators)

> Traducao nao oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Texto adaptado para portugues brasileiro com fins educacionais. Paginas originais: 253-304.
> Repositorio: [ring0-study](https://github.com/cnthigu/ring0-study)

## Visao geral do capitulo

- O `if` e o `if else`
- Operadores logicos `&&`, `||`, `!`
- A biblioteca `<cctype>` (classificacao de caracteres)
- O operador ternario `? :`
- O `switch`
- `break` e `continue`
- Loops de leitura de numeros
- I/O simples de arquivos

## O `if`

```cpp
if (condicao) {
    // executa se condicao for true
}

if (idade >= 18) {
    std::cout << "maior de idade\n";
}
```

### `if else`

```cpp
if (idade >= 18) {
    std::cout << "maior\n";
} else {
    std::cout << "menor\n";
}
```

### `else if`

```cpp
if (nota >= 9.0) {
    std::cout << "A\n";
} else if (nota >= 7.0) {
    std::cout << "B\n";
} else if (nota >= 5.0) {
    std::cout << "C\n";
} else {
    std::cout << "Reprovado\n";
}
```

### Em C++ moderno: `if` com inicializacao (C++17)

```cpp
if (auto it = mapa.find("chave"); it != mapa.end()) {
    std::cout << it->second << "\n";
}   // it sai de escopo aqui

if (int n = calcular(); n > 0 && n < 100) {
    usar(n);
}
```

Mantem variaveis curtas no escopo certo, sem poluir.

## Operadores logicos

| Operador | Nome | Descricao |
|---|---|---|
| `&&` | AND | true se ambos forem true |
| `\|\|` | OR | true se pelo menos um for true |
| `!` | NOT | inverte |

```cpp
if (idade >= 18 && tem_carteira) { /* dirigir */ }
if (cor == "azul" || cor == "verde") { /* ... */ }
if (!arquivo.is_open()) { /* erro */ }
```

### Curto-circuito

C++ avalia da esquerda pra direita e **para** assim que sabe a resposta:

```cpp
if (p != nullptr && p->valor > 0) { /* ... */ }
//   ^ se p for null, NAO avalia p->valor (que crasharia)

if (cache_hit || calcular_caro()) { /* ... */ }
//   ^ se cache_hit for true, NAO chama calcular_caro
```

Esse comportamento e fundamental e voce vai usar muito.

## `<cctype>` - classificacao de caracteres

```cpp
#include <cctype>
#include <iostream>

int main()
{
    char c = 'A';
    if (std::isalpha(c)) std::cout << "letra\n";
    if (std::isdigit('5')) std::cout << "digito\n";
    if (std::isupper('A')) std::cout << "maiuscula\n";
    if (std::islower('a')) std::cout << "minuscula\n";
    if (std::isspace(' ')) std::cout << "whitespace\n";
    if (std::isalnum('5')) std::cout << "alfanumerico\n";
    if (std::ispunct(',')) std::cout << "pontuacao\n";

    char up  = std::toupper('a');   // 'A'
    char low = std::tolower('Z');   // 'z'
    return 0;
}
```

> **Rode no Visual Studio ou compile com `g++ ctype.cpp -o ctype`**

> **Cuidado**: `std::isalpha(c)` espera `c` cast para `unsigned char` se voce passa `char` com possiveis valores negativos. Use:
> ```cpp
> std::isalpha(static_cast<unsigned char>(c));
> ```

## Operador ternario `? :`

```cpp
int idade = 17;
std::string status = (idade >= 18) ? "maior" : "menor";

int max = (a > b) ? a : b;

std::cout << ((n % 2 == 0) ? "par" : "impar") << "\n";
```

Equivalente a:

```cpp
if (idade >= 18) status = "maior";
else status = "menor";
```

Use `?:` para expressoes curtas. Para logica complexa, prefira `if`.

## `switch`

```cpp
char opcao;
std::cin >> opcao;

switch (opcao) {
case 'a':
case 'A':
    std::cout << "Atualizar\n";
    break;
case 'd':
case 'D':
    std::cout << "Deletar\n";
    break;
case 'q':
case 'Q':
    std::cout << "Sair\n";
    break;
default:
    std::cout << "Opcao invalida\n";
}
```

> **Cuidado**: sem `break`, ele "cai" pro proximo case (fall-through). Geralmente isso e bug. Em C++17 voce pode marcar fall-through intencional:
>
> ```cpp
> switch (x) {
> case 1:
>     fazAlgo();
>     [[fallthrough]];   // C++17 - "eu quero cair, nao esquecei"
> case 2:
>     fazOutraCoisa();
>     break;
> }
> ```

`switch` so funciona com tipos integrais (`int`, `char`, `enum`). **Nao funciona com `std::string`** (use `if/else if` ou um `std::map<std::string, std::function<void()>>`).

### Em C++ moderno: switch com inicializador (C++17)

```cpp
switch (auto status = obter_status(); status) {
case Status::ok:    /* ... */ break;
case Status::erro:  /* ... */ break;
case Status::pend:  /* ... */ break;
}
```

## `break` e `continue` em loops

```cpp
for (int i = 0; i < 100; ++i) {
    if (i == 50) break;          // SAI do loop
    if (i % 2 == 0) continue;    // pula para a proxima iteracao
    std::cout << i << " ";
}
```

`break` sai apenas do loop/switch **mais interno**.

### Saindo de loops aninhados

C++ nao tem `break N` (como em algumas linguagens). Opcoes:

```cpp
// 1. Flag
bool achou = false;
for (int i = 0; i < N && !achou; ++i) {
    for (int j = 0; j < M; ++j) {
        if (matriz[i][j] == alvo) {
            achou = true;
            break;
        }
    }
}

// 2. Extrair para funcao e usar return (PREFERIDO)
auto encontrar = [&]() -> std::optional<std::pair<int,int>> {
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < M; ++j)
            if (matriz[i][j] == alvo) return std::pair{i, j};
    return std::nullopt;
};
```

## Loops de leitura de numeros

```cpp
#include <iostream>
#include <vector>

int main()
{
    std::vector<double> dados;
    double x;
    std::cout << "Digite numeros (Ctrl+Z + Enter para encerrar):\n";
    while (std::cin >> x) {
        dados.push_back(x);
    }

    double soma = 0;
    for (double v : dados) soma += v;

    if (!dados.empty()) {
        std::cout << "Total: " << dados.size() << ", media: "
                  << soma / dados.size() << "\n";
    }
    return 0;
}
```

`std::cin >> x` falha (retorna `false`) em EOF ou quando o usuario digita algo nao-numerico. Para recuperar de erro:

```cpp
if (std::cin.fail()) {
    std::cin.clear();                   // limpa flags de erro
    std::cin.ignore(10000, '\n');       // descarta resto da linha
}
```

## I/O de arquivos simples

```cpp
#include <fstream>
#include <iostream>
#include <string>

int main()
{
    // ESCREVENDO num arquivo
    std::ofstream out("dados.txt");
    if (!out) {
        std::cerr << "Erro ao abrir para escrita\n";
        return 1;
    }
    out << "linha 1\n";
    out << "linha 2\n";
    out.close();    // opcional - destrutor fecha sozinho

    // LENDO de um arquivo
    std::ifstream in("dados.txt");
    if (!in) {
        std::cerr << "Erro ao abrir para leitura\n";
        return 1;
    }
    std::string linha;
    while (std::getline(in, linha)) {
        std::cout << linha << "\n";
    }
    return 0;
}
```

> **Rode no Visual Studio ou compile com `g++ arq.cpp -o arq`**

`std::ofstream` (output) e `std::ifstream` (input) herdam de `std::ostream`/`std::istream`, entao todos os operadores `<<` e `>>` funcionam igual a `cout`/`cin`.

### Em C++ moderno: `std::filesystem` (C++17)

```cpp
#include <filesystem>
namespace fs = std::filesystem;

if (fs::exists("dados.txt")) {
    std::cout << "Tamanho: " << fs::file_size("dados.txt") << " bytes\n";
}

for (const auto& entry : fs::directory_iterator(".")) {
    std::cout << entry.path() << "\n";
}

fs::create_directory("nova_pasta");
fs::remove("dados.txt");
```

`std::filesystem` substitui APIs especificas de SO (POSIX, WinAPI) com codigo portavel.

## Resumo

- `if/else if/else` para decisoes; `switch` para casos discretos
- Operadores logicos `&&`, `||`, `!` com curto-circuito
- `<cctype>` para classificar/converter caracteres
- Operador ternario `? :` para expressoes curtas
- `break` sai do loop, `continue` pula
- Em C++17, `if` e `switch` aceitam **inicializador** (`if (auto x = f(); x > 0)`)
- I/O de arquivos com `<fstream>`; em C++17, use `<filesystem>` para manipular caminhos

## Proximo capitulo

[Capitulo 7 - Funcoes](capitulo-07.md): definicao, prototipo, passagem por valor/referencia/ponteiro, funcoes com arrays, structs, e ponteiros para funcoes.
