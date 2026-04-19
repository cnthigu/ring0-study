# Capitulo 18 - Visitando o Novo Padrao C++ (Visiting with the New C++ Standard)

> Traducao nao oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Texto adaptado para portugues brasileiro com fins educacionais. Paginas originais: 1153-1214.
> Repositorio: [ring0-study](https://github.com/cnthigu/ring0-study)

## Sobre este capitulo

O capitulo 18 do livro foi escrito em 2011 cobrindo as features do **C++11** (na epoca, "o novo padrao"). Hoje, em 2026, ja temos **C++14, C++17, C++20 e C++23** publicados.

Esta versao traduzida vai cobrir o que o livro original aborda (C++11) e tambem dar uma **visao geral resumida** do que veio depois. Cada item recebe a versao em que foi introduzido.

## Features do C++11 (cobertas no livro original)

### `auto`

Deducao automatica de tipo:

```cpp
auto x = 42;                       // int
auto y = 3.14;                     // double
auto s = std::string{"ola"};       // std::string
auto v = std::vector<int>{1, 2};   // std::vector<int>

for (auto it = v.begin(); it != v.end(); ++it) { }   // sem precisar repetir o tipo
```

### `decltype`

Pega o tipo de uma expressao em compile-time:

```cpp
int n = 5;
decltype(n) m = 10;        // m e int

template <typename A, typename B>
auto soma(A a, B b) -> decltype(a + b) {       // trailing return type
    return a + b;
}
```

### Range-based `for`

```cpp
std::vector<int> v = {1, 2, 3, 4, 5};

for (int x : v)        { /* copia */ }
for (auto& x : v)      { /* referencia - pode modificar */ }
for (const auto& x : v){ /* referencia const - so leitura, sem copia */ }
```

### Nullptr

Substitui o `NULL` (que era apenas `0`):

```cpp
int* p = nullptr;          // type-safe
if (p == nullptr) { }
```

### Smart pointers

`std::unique_ptr`, `std::shared_ptr`, `std::weak_ptr` em `<memory>`:

```cpp
auto p = std::make_unique<int>(42);
auto sp = std::make_shared<std::string>("ola");
```

### Lambdas

```cpp
auto soma = [](int a, int b) { return a + b; };
std::sort(v.begin(), v.end(), [](int a, int b){ return a > b; });
```

### `enum class`

Enums fortemente tipadas e com escopo:

```cpp
enum class Cor { Vermelho, Verde, Azul };
Cor c = Cor::Vermelho;
```

### `constexpr`

Avaliacao em compile-time:

```cpp
constexpr int quadrado(int x) { return x * x; }
constexpr int N = quadrado(5);     // calculado em compile-time
```

### Move semantics

```cpp
class Coisa {
public:
    Coisa(Coisa&& outra) noexcept;            // move constructor
    Coisa& operator=(Coisa&& outra) noexcept; // move assignment
};

std::vector<int> v1 = {1, 2, 3};
std::vector<int> v2 = std::move(v1);    // move (sem copiar)
```

### Variadic templates

```cpp
template <typename... Args>
void print(Args... args) {
    ((std::cout << args << " "), ...);   // C++17 fold expression
}
```

### `static_assert`

```cpp
static_assert(sizeof(int) >= 4, "int precisa ter pelo menos 4 bytes");
```

### `final` e `override`

```cpp
class Base {
public:
    virtual void f();
    virtual void g() final;            // nao pode sobrescrever
};

class Derived : public Base {
public:
    void f() override;                 // erro de compilacao se nao sobrescrever de fato
    // void g() override;              // ERRO: g e final
};
```

### `= default` e `= delete`

```cpp
class Coisa {
public:
    Coisa() = default;                // construtor padrao gerado
    Coisa(const Coisa&) = delete;     // proibe copia
};
```

### Initializer lists

```cpp
#include <initializer_list>

std::vector<int> v = {1, 2, 3, 4, 5};

class MeuContainer {
public:
    MeuContainer(std::initializer_list<int> init);
};
```

### Hash containers

```cpp
#include <unordered_map>
#include <unordered_set>

std::unordered_map<std::string, int> m;
std::unordered_set<int> s;
```

### Threading (`<thread>`, `<mutex>`, `<atomic>`)

```cpp
#include <thread>
#include <mutex>

std::mutex mtx;
std::thread t([](){ /* trabalho */ });
t.join();

std::lock_guard<std::mutex> lock{mtx};
```

### Regex (`<regex>`)

```cpp
#include <regex>

std::regex r{R"(\d+)"};
std::string s = "tenho 42 anos";
std::smatch match;
if (std::regex_search(s, match, r)) {
    std::cout << match[0] << "\n";   // "42"
}
```

### Random (`<random>`)

Substitui `rand()`:

```cpp
#include <random>

std::random_device rd;
std::mt19937 gen{rd()};
std::uniform_int_distribution<> dist{1, 100};
int x = dist(gen);
```

### Chrono (`<chrono>`)

Tempo type-safe:

```cpp
#include <chrono>
namespace ch = std::chrono;

auto inicio = ch::steady_clock::now();
// ... trabalho ...
auto fim = ch::steady_clock::now();
auto ms = ch::duration_cast<ch::milliseconds>(fim - inicio).count();

std::this_thread::sleep_for(ch::seconds(2));
```

### Raw string literals

```cpp
std::string regex = R"(\d{3}-\d{4})";
std::string path  = R"(C:\Users\cnt\arquivo.txt)";
std::string json  = R"({"nome": "cnt", "idade": 24})";
```

### `noexcept`

```cpp
void f() noexcept;            // promete nao lancar excecoes
void g() noexcept(true);
```

## C++14 (publicado em 2014)

Mais incremental:

- **`auto` em retornos de funcao** (sem trailing):
  ```cpp
  auto soma(int a, int b) { return a + b; }
  ```
- **Generic lambdas** (`auto` nos parametros):
  ```cpp
  auto print = [](auto x){ std::cout << x; };
  ```
- **`std::make_unique`** (estranhamente faltou no C++11)
- **`constexpr` mais flexivel** (loops, if, etc dentro)
- **Binary literals**: `0b1010`
- **Digit separators**: `1'000'000`

## C++17 (publicado em 2017)

Salto significativo:

- **`std::optional<T>`**: representa "valor ou nada"
  ```cpp
  std::optional<int> achar(int x);
  if (auto r = achar(5); r) { use(*r); }
  ```
- **`std::variant<A, B, C>`**: union type-safe
- **`std::any`**: container para qualquer tipo
- **`std::string_view`**: view nao-owning de string (sem copia!)
- **`std::filesystem`**: manipular arquivos/pastas
- **Structured bindings**: `auto [a, b] = par;`
- **`if`/`switch` com initializer**: `if (auto x = f(); x > 0) { }`
- **Class template argument deduction (CTAD)**:
  ```cpp
  std::vector v{1, 2, 3};        // deduz vector<int>
  std::pair p{1, "ola"};         // deduz pair<int, const char*>
  ```
- **Fold expressions** para variadic templates:
  ```cpp
  template <typename... T>
  auto soma(T... args) { return (args + ...); }
  ```
- **`if constexpr`**: branches em compile-time
- **`inline` variables**: variaveis em headers sem ODR violation
- **Nested namespaces**: `namespace a::b::c { }`
- **`[[nodiscard]]`**, `[[maybe_unused]]`, `[[fallthrough]]`
- **`std::byte`**: tipo de byte real (nao char)
- **Algoritmos paralelos**:
  ```cpp
  std::sort(std::execution::par, v.begin(), v.end());
  ```

## C++20 (publicado em 2020)

Considerado o segundo maior salto depois do C++11:

- **Concepts**: restricoes em templates
  ```cpp
  template <std::integral T>
  T dobro(T x) { return x * 2; }
  ```
- **Ranges**: pipelines lazy sobre containers
  ```cpp
  auto pares_quad = v | std::views::filter([](int x){ return x % 2 == 0; })
                      | std::views::transform([](int x){ return x * x; });
  ```
- **Coroutines**: `co_await`, `co_yield`, `co_return`
- **Modules**: substituem `#include` (compilation muito mais rapida)
  ```cpp
  // mod.cppm
  export module mymod;
  export int func();

  // main.cpp
  import mymod;
  ```
- **`std::format`**: formatacao tipo Python
- **3-way comparison** (`operator<=>`):
  ```cpp
  struct Ponto {
      int x, y;
      auto operator<=>(const Ponto&) const = default;
  };
  // gera ==, !=, <, <=, >, >= automaticamente
  ```
- **`std::span<T>`**: view nao-owning de array
- **Designated initializers**:
  ```cpp
  struct P { int x, y, z; };
  P p{.x = 1, .y = 2, .z = 3};
  ```
- **`consteval`**: forca avaliacao em compile-time
- **`constinit`**: garante init em compile-time (sem fiasco da static initialization order)
- **`std::jthread`**: thread com join automatico no destrutor
- **`std::stop_token`**: cancelamento cooperativo
- **`<numbers>`**: `std::numbers::pi`, `std::numbers::e`, etc.

## C++23 (publicado em 2023)

- **`std::expected<T, E>`**: alternativa a excecoes
  ```cpp
  std::expected<int, std::string> parsear(const std::string& s);
  if (auto r = parsear("42"); r) { use(*r); }
  else { std::cerr << r.error(); }
  ```
- **`std::print`/`std::println`**: substituto definitivo do `cout`
  ```cpp
  std::println("Ola, {}!", "mundo");
  ```
- **Deducing `this`**: `(this Self&& self, ...)` resolve duplicacao em const/non-const overloads
- **`std::stacktrace`**: capturar stack trace
- **`std::generator<T>`**: coroutines geradoras na biblioteca padrao
- **`std::flat_map`/`std::flat_set`**: containers ordenados sobre vector (cache-friendly)
- **`std::mdspan`**: view multi-dimensional sobre dados
- **Multiline raw string literals melhorados**
- **`std::ranges::to`**: converter ranges em containers
  ```cpp
  auto v = std::views::iota(1, 11)
         | std::views::filter([](int x){ return x % 2 == 0; })
         | std::ranges::to<std::vector>();
  ```

## C++26 (em desenvolvimento)

Algumas features que provavelmente entram (em 2026):

- **Reflection** (estatica) - inspecionar tipos em compile-time
- **Pattern matching** (`inspect`/`match`)
- **Contracts** - pre/pos-condicoes na linguagem
- **`std::execution`** - executores e modelo de async unificado
- **Linear algebra** na biblioteca padrao
- **Hazard pointers** e RCU para concorrencia lock-free

## Resumo: o que importa pra um dev junior em 2026

### O que voce **precisa** dominar

- `auto`, range-based `for`, lambdas
- Smart pointers (`unique_ptr` por padrao, `shared_ptr` so quando precisa)
- Move semantics e quando usar `std::move`
- STL: `vector`, `string`, `map`, `unordered_map`
- `<algorithm>`: `sort`, `find`, `transform`, `accumulate`, `for_each`
- RAII e Rule of Zero/Three/Five
- `enum class`, `constexpr`, `nullptr`, `override`/`final`

### O que **vai te diferenciar**

- `std::optional`, `std::variant`, `std::string_view`, `std::span`
- Structured bindings, `if`/`switch` com init
- `std::filesystem` para I/O em vez de funcoes C
- `std::format`/`std::print` em vez de `cout`/`printf`
- Concepts em templates
- `std::ranges` e views
- `std::jthread`, `std::atomic`, `std::mutex`

### Em codebases antigas voce ainda vai ver

- `printf`/`scanf`
- `new`/`delete` cru (sem smart pointers)
- C-strings (`char*`)
- Macros para tudo
- `using namespace std;` em headers
- `NULL` em vez de `nullptr`
- Casts em estilo C: `(int)x`

Saber **modernizar** codigo antigo e uma habilidade valiosa.

## Fim do livro

Esse foi o ultimo capitulo. Se voce chegou ate aqui:

- Voce ja sabe **mais C++ do que a maioria dos devs ditos "C++ devs"**
- Pratique escrevendo projetos reais (nao so exercicios)
- Leia codigo de projetos open source: WebKit, LLVM, Folly (Facebook), Chromium
- Leia [cppreference.com](https://en.cppreference.com/w/) - referencia oficial
- Veja palestras da CppCon no YouTube (Stroustrup, Sutter, Klemens, etc.)
- Aprenda **uma area de aplicacao**: jogos (Unreal), sistemas (Windows internals), embedded, finance (HFT), graficos (OpenGL/Vulkan), etc.

C++ e uma linguagem que voce **nunca** termina de aprender. E essa e parte da diversao.

---

## Voltar

[Indice](README.md) | [Capitulo 17](capitulo-17.md)
