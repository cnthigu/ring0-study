# Capitulo 16 - A Classe string e a STL (The string Class and the Standard Template Library)

> Traducao nao oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Texto adaptado para portugues brasileiro com fins educacionais. Paginas originais: 951-1060.
> Repositorio: [ring0-study](https://github.com/cnthigu/ring0-study)

## Visao geral do capitulo

- A classe `std::string` em detalhe
- Containers da STL: `vector`, `deque`, `list`, `forward_list`, `array`
- Containers associativos: `set`, `map`, `unordered_set`, `unordered_map`
- Iteradores
- Algoritmos da STL
- Functores e lambdas
- Outros: `auto_ptr` (deprecated) -> `unique_ptr`/`shared_ptr`

## A classe `std::string`

### Construcao

```cpp
#include <string>

std::string s1;                          // vazia
std::string s2 = "ola";                  // de C-string
std::string s3{"ola"};
std::string s4(10, 'X');                 // "XXXXXXXXXX"
std::string s5{s2};                       // copia
std::string s6{s2, 1, 2};                // substring de s2 (offset, len) -> "la"

// C++11+ - de iteradores
std::string s7{s2.begin(), s2.begin() + 3};
```

### Operacoes basicas

```cpp
std::string s = "Carniato";

s.size();                  // 8
s.length();                // 8 (mesmo)
s.empty();                 // false
s[0];                      // 'C' - sem checagem
s.at(0);                   // 'C' - lanca std::out_of_range se fora
s.front();                 // 'C'
s.back();                  // 'o'

s += "!";                  // append
s.append("123");
s.push_back('X');
s.pop_back();

s.find("rni");             // 2 (indice) ou std::string::npos se nao achar
s.rfind("a");              // ultima ocorrencia
s.substr(2, 3);            // "rni"
s.replace(0, 3, "Tudo");   // troca primeiros 3 chars

s.clear();
```

### Comparacao

```cpp
std::string a = "abc";
std::string b = "abd";

a == b;                    // false
a <  b;                    // true (comparacao lexicografica)
a.compare(b);              // < 0
```

### Conversoes

```cpp
int    n = std::stoi("42");
double d = std::stod("3.14");
long   l = std::stol("999999");

std::string s1 = std::to_string(42);       // "42"
std::string s2 = std::to_string(3.14);     // "3.140000"
```

### Em C++ moderno

```cpp
// C++17 - string_view (sem copia)
#include <string_view>
void f(std::string_view sv) { /* ... */ }

f("literal");              // sem copiar
f(std::string{"texto"});   // sem copiar

// C++20 - starts_with, ends_with, contains (C++23)
std::string s = "arquivo.txt";
s.starts_with("arq");      // true
s.ends_with(".txt");       // true
s.contains("vo.");         // C++23

// C++20 - std::format
#include <format>
std::string msg = std::format("nome={}, idade={}", "cnt", 24);
```

## Containers da STL

### Visao geral

| Container | Estrutura | Uso |
|---|---|---|
| `std::array<T, N>` | Array fixo | Tamanho conhecido em compile-time |
| `std::vector<T>` | Array dinamico | **PADRAO** - use por defeito |
| `std::deque<T>` | Double-ended queue | Push/pop em ambas as pontas O(1) |
| `std::list<T>` | Lista duplamente ligada | Insercao/remocao no meio em O(1) |
| `std::forward_list<T>` | Lista simplesmente ligada | Quando memoria e critica |
| `std::set<T>` | Arvore (red-black) | Conjunto ordenado, busca O(log n) |
| `std::map<K, V>` | Arvore | Dicionario ordenado |
| `std::unordered_set<T>` | Hash | Conjunto, busca O(1) media |
| `std::unordered_map<K, V>` | Hash | Dicionario, acesso O(1) media |
| `std::stack<T>` | Adapter | LIFO (sobre `vector` ou `deque`) |
| `std::queue<T>` | Adapter | FIFO |
| `std::priority_queue<T>` | Adapter | Heap |

### `std::vector` em detalhe

```cpp
#include <vector>

std::vector<int> v;
std::vector<int> v2 = {1, 2, 3, 4, 5};
std::vector<int> v3(10, 0);              // 10 zeros
std::vector<int> v4(10);                 // 10 default-init

v.push_back(42);
v.emplace_back(42);                       // C++11 - constroi no lugar
v.pop_back();
v.size();
v.capacity();                             // memoria reservada
v.reserve(1000);                          // pre-aloca (evita realocacao)
v.shrink_to_fit();                        // libera memoria nao usada
v.clear();
v.resize(20);

v.insert(v.begin() + 2, 99);             // insere no meio
v.erase(v.begin() + 2);                  // remove no meio
v.erase(v.begin(), v.begin() + 3);       // remove faixa

for (int x : v) { }                       // range-for (preferido)
for (auto it = v.begin(); it != v.end(); ++it) { }  // iterator
```

### `std::map` e `std::unordered_map`

```cpp
#include <map>
#include <unordered_map>

std::map<std::string, int>           idades = {{"cnt", 24}, {"alice", 30}};
std::unordered_map<std::string, int> hash_idades;

idades["bob"] = 25;                       // insere
idades.insert({"carol", 28});

if (idades.count("cnt"))   { /* existe */ }
if (idades.contains("cnt")) { /* C++20 */ }

if (auto it = idades.find("cnt"); it != idades.end()) {
    std::cout << it->second;
}

idades.erase("alice");

for (const auto& [nome, idade] : idades) {        // C++17 structured bindings
    std::cout << nome << " " << idade << "\n";
}
```

`map` e ordenado (arvore). `unordered_map` e hash, acesso O(1) medio mas nao ordenado.

### `std::set` e `std::unordered_set`

```cpp
#include <set>
std::set<int> s = {3, 1, 4, 1, 5, 9, 2, 6};
// guardado em ordem: {1, 2, 3, 4, 5, 6, 9}

s.insert(7);
s.erase(3);
if (s.contains(5)) { /* C++20 */ }
```

## Iteradores

Os "ponteiros generalizados" da STL:

```cpp
std::vector<int> v = {10, 20, 30};

auto it = v.begin();
*it;                       // 10
++it;
*it;                       // 20
it = v.end() - 1;          // ultimo
*it;                       // 30
```

### Categorias

- **Input iterator**: ler 1 vez para frente
- **Output iterator**: escrever 1 vez para frente
- **Forward iterator**: ler/escrever multiplas vezes para frente
- **Bidirectional iterator**: forward + para tras (`--`)
- **Random access iterator**: bidirectional + aritmetica (`+`, `-`, `[]`)
- **Contiguous iterator** (C++20): random access + memoria contigua

`vector` e `array` tem random access. `list` tem bidirectional. `forward_list` tem forward.

## Algoritmos

A `<algorithm>` traz dezenas de operacoes que funcionam em **qualquer container** via iteradores:

```cpp
#include <algorithm>
#include <numeric>
#include <vector>

std::vector<int> v = {5, 3, 8, 1, 9, 2, 7};

std::sort(v.begin(), v.end());                     // ordena
std::sort(v.begin(), v.end(), std::greater<>{});   // decrescente

auto it = std::find(v.begin(), v.end(), 8);
auto it2 = std::find_if(v.begin(), v.end(),
    [](int x){ return x > 5; });

int n_pares = std::count_if(v.begin(), v.end(),
    [](int x){ return x % 2 == 0; });

bool todos_positivos = std::all_of(v.begin(), v.end(),
    [](int x){ return x > 0; });
bool algum_grande    = std::any_of(v.begin(), v.end(),
    [](int x){ return x > 100; });

int soma = std::accumulate(v.begin(), v.end(), 0);

std::vector<int> dobrado(v.size());
std::transform(v.begin(), v.end(), dobrado.begin(),
    [](int x){ return x * 2; });

std::for_each(v.begin(), v.end(),
    [](int x){ std::cout << x << " "; });
```

### Erase-remove idiom

Para remover elementos por valor (pre-C++20):

```cpp
v.erase(std::remove(v.begin(), v.end(), 5), v.end());
v.erase(std::remove_if(v.begin(), v.end(),
    [](int x){ return x > 10; }), v.end());
```

### Em C++20: `std::erase` simplificou

```cpp
std::erase(v, 5);
std::erase_if(v, [](int x){ return x > 10; });
```

### Em C++20: `<ranges>`

```cpp
#include <ranges>
namespace rng = std::ranges;
namespace vw  = std::views;

std::vector<int> v = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

// nao precisa de begin()/end()
rng::sort(v);
auto it = rng::find(v, 5);

// pipelines lazy
auto resultado = v
    | vw::filter([](int x){ return x % 2 == 0; })
    | vw::transform([](int x){ return x * x; })
    | vw::take(3);

for (int x : resultado) std::cout << x << " ";   // 4 16 36
```

## Functores e lambdas

Um **functor** e um objeto com `operator()` sobrecarregado - se comporta como funcao:

```cpp
struct Multiplica {
    int fator;
    Multiplica(int f) : fator{f} {}
    int operator()(int x) const { return x * fator; }
};

Multiplica triplo{3};
std::cout << triplo(5);        // 15

std::transform(v.begin(), v.end(), v.begin(), Multiplica{2});
```

Em C++ moderno, **lambdas** substituem functores na maioria dos casos:

```cpp
int fator = 3;
auto triplo = [fator](int x) { return x * fator; };
std::cout << triplo(5);

std::transform(v.begin(), v.end(), v.begin(),
    [](int x) { return x * 2; });
```

### Callables genericas

```cpp
#include <functional>

std::function<int(int)> op = [](int x){ return x * 2; };
op = [](int x){ return x + 100; };
```

`std::function` aceita qualquer callable: lambda, functor, function pointer.

## Resumo

- `std::string` substitui completamente C-strings em codigo novo
- `std::vector` e o container padrao - **use por defeito**
- `std::map`/`std::unordered_map` para dicionarios (ordenado vs hash)
- Iteradores generalizam ponteiros e fazem todos os algoritmos funcionarem em qualquer container
- `<algorithm>` tem dezenas de operacoes - **explore-as** antes de escrever loops manuais
- Lambdas (C++11) tornaram a STL muito mais agradavel
- C++17 trouxe `std::optional`, `std::variant`, `std::string_view`, structured bindings, `std::filesystem`
- C++20 trouxe `<ranges>` que vai mudar como voce escreve C++
- C++23 trouxe `std::expected`, `std::print`, e mais

## Proximo capitulo

[Capitulo 17 - Input, Output e Arquivos](capitulo-17.md): streams em detalhe, formatacao, manipuladores, arquivos binarios, `seek`/`tell`.
