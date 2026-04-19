# Capitulo 15 - Friends, Exceptions e Mais (Friends, Exceptions, and More)

> Traducao nao oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Texto adaptado para portugues brasileiro com fins educacionais. Paginas originais: 877-950.
> Repositorio: [ring0-study](https://github.com/cnthigu/ring0-study)

## Visao geral do capitulo

- Classes amigas (`friend class`)
- Funcoes amigas membro de outra classe
- Classes aninhadas em mais detalhe
- Excecoes (`try`/`catch`/`throw`)
- Hierarquia padrao de excecoes
- RTTI: `dynamic_cast`, `typeid`
- Os quatro tipos de cast em C++

## Classes amigas

Uma classe pode ser `friend` de outra, dando acesso aos membros `private`/`protected`:

```cpp
class TV;          // forward declaration

class Controle {
public:
    void ligar(TV& aparelho);
    void mudar_canal(TV& aparelho, int canal);
};

class TV {
    friend class Controle;       // Controle tem acesso TOTAL a TV
private:
    bool ligada = false;
    int  canal  = 1;
};

void Controle::ligar(TV& aparelho) {
    aparelho.ligada = true;             // acesso direto a private!
}

void Controle::mudar_canal(TV& aparelho, int canal) {
    if (aparelho.ligada) aparelho.canal = canal;
}
```

> **Cuidado**: `friend` quebra encapsulamento. Use raramente, em casos onde duas classes genuinamente sao "uma so coisa" dividida em duas para organizacao.

### Funcao membro como friend

```cpp
class B;

class A {
public:
    void mexer_em_b(B& b);
};

class B {
    friend void A::mexer_em_b(B& b);     // so esse metodo de A tem acesso
    int valor;
};

void A::mexer_em_b(B& b) {
    b.valor = 42;
}
```

## Excecoes (Exceptions)

Excecoes sao o mecanismo padrao em C++ para reportar erros que **nao** podem ser tratados localmente.

### Sintaxe basica

```cpp
#include <iostream>
#include <stdexcept>

double dividir(double a, double b) {
    if (b == 0.0) {
        throw std::runtime_error{"divisao por zero"};
    }
    return a / b;
}

int main()
{
    try {
        double r = dividir(10.0, 0.0);
        std::cout << r << "\n";
    }
    catch (const std::runtime_error& e) {
        std::cerr << "erro: " << e.what() << "\n";
    }
}
```

> **Rode no Visual Studio ou compile com `g++ -std=c++17 excecao.cpp -o excecao`**

### Como funciona

1. `throw` joga um objeto de qualquer tipo
2. O programa vai desempilhando funcoes (**stack unwinding**) chamando destrutores ate achar um `catch` que combine
3. O `catch` correspondente recebe o objeto
4. Se nao encontrar nenhum `catch`, o programa chama `std::terminate` (encerra)

### Multiplos `catch`

```cpp
try {
    fazer_algo();
}
catch (const std::out_of_range& e)  { /* especifico */ }
catch (const std::runtime_error& e) { /* mais geral */ }
catch (const std::exception& e)     { /* ainda mais geral - capt. tudo da std */ }
catch (...)                          { /* captura literalmente qualquer coisa */ }
```

A ordem importa: ponha **mais especifico primeiro**, mais geral depois.

### Hierarquia padrao (`<stdexcept>`)

```
std::exception
├── std::logic_error
│   ├── std::domain_error
│   ├── std::invalid_argument
│   ├── std::length_error
│   └── std::out_of_range
└── std::runtime_error
    ├── std::overflow_error
    ├── std::underflow_error
    └── std::range_error
```

```cpp
class MeuErro : public std::runtime_error {
public:
    MeuErro(const std::string& msg) : std::runtime_error{msg} {}
};
```

### Re-lancando

```cpp
try { /* ... */ }
catch (...) {
    fazer_log();
    throw;          // re-lanca a excecao original
}
```

### Cuidado: NUNCA jogue excecao no destrutor

```cpp
class Conexao {
public:
    ~Conexao() {
        // throw std::runtime_error{"erro"};  // NAO FACA!
    }
};
```

Se houver excecao no destrutor durante stack unwinding, voce tem duas excecoes ao mesmo tempo - `std::terminate` e chamado.

### `noexcept`

Marca uma funcao como "nao lanca excecao":

```cpp
int dobrar(int x) noexcept {
    return 2 * x;
}
```

- Permite otimizacoes
- `std::vector` move elementos com `noexcept`, copia caso contrario
- Se uma funcao `noexcept` lancar mesmo assim -> `std::terminate`

## Quando usar excecoes (e quando nao)

**Use** para:
- Erros de programa que **nao podem** ser tratados onde aconteceram (deixe propagar)
- Erros de inicializacao (construtores nao tem outro jeito)
- Erros raros e excepcionais

**Nao use** para:
- Controle de fluxo normal (e MUITO mais lento que `if`)
- Codigo de baixa latencia (jogos, audio, drivers): muitos projetos compilam com `-fno-exceptions`
- Casos comuns: prefira `std::optional<T>`, `std::expected<T, E>` (C++23)

### Em C++23: `std::expected`

```cpp
#include <expected>

std::expected<double, std::string> dividir(double a, double b) {
    if (b == 0.0) return std::unexpected{"divisao por zero"};
    return a / b;
}

auto r = dividir(10, 0);
if (r) {
    std::cout << *r << "\n";
} else {
    std::cout << "erro: " << r.error() << "\n";
}
```

`std::expected<T, E>` e um tipo soma: ou tem `T` (sucesso) ou `E` (erro). Sem custo de excecao.

## RTTI (Run-Time Type Information)

### `dynamic_cast`

Converte com seguranca em uma hierarquia de heranca:

```cpp
class Base { public: virtual ~Base() = default; };
class Filho1 : public Base { public: void f1() {} };
class Filho2 : public Base { public: void f2() {} };

void processar(Base* b) {
    if (Filho1* p = dynamic_cast<Filho1*>(b)) {
        p->f1();
    } else if (Filho2* p = dynamic_cast<Filho2*>(b)) {
        p->f2();
    }
}
```

Para ponteiros: retorna `nullptr` em falha.
Para referencias: lanca `std::bad_cast` em falha.

> Funciona apenas com classes que tem **pelo menos uma funcao virtual** (polimorficas).

### `typeid`

```cpp
#include <typeinfo>

Base* b = new Filho1{};
std::cout << typeid(*b).name() << "\n";       // mangled name do Filho1
if (typeid(*b) == typeid(Filho1)) { /* ... */ }
```

`typeid(expr)` retorna um `std::type_info` que voce pode comparar.

> Em geral, **prefira virtuals** sobre `dynamic_cast`/`typeid`. Polimorfismo bem feito raramente precisa de RTTI.

## Os quatro casts de C++

### `static_cast`

Conversao "obvia" verificada em compilacao:

```cpp
int i = 42;
double d = static_cast<double>(i);            // int -> double
Base* b = static_cast<Base*>(filho_ptr);      // upcast
```

### `dynamic_cast`

Cast seguro em hierarquia (run-time check). Visto acima.

### `const_cast`

Remove `const` (use raramente, geralmente indica design ruim):

```cpp
void f(int* p);

const int x = 10;
f(const_cast<int*>(&x));     // VAI compilar, mas modificar x e UB!

// Caso valido: API antiga que nao tem const correto
const char* str = "ola";
foo_legacy(const_cast<char*>(str));   // se foo_legacy NAO modifica
```

### `reinterpret_cast`

Reinterpretacao baixo-nivel de bits. **Perigoso**:

```cpp
int    x = 0x41424344;
char*  c = reinterpret_cast<char*>(&x);   // ve x como bytes

// Casos validos: protocolos de rede, drivers, hashes
```

### Cast estilo C - evite!

```cpp
double d = (double)i;        // estilo C - faz qualquer coisa
double d = double(i);        // estilo "construtor"
double d = static_cast<double>(i);   // estilo C++ MODERNO - PREFIRA
```

`static_cast` e mais facil de buscar com grep, deixa intencao clara, e o compilador checa mais.

## Resumo

- `friend` da acesso a private; quebra encapsulamento
- Excecoes (`try`/`catch`/`throw`) para erros que **nao podem** ser tratados localmente
- Use `std::exception` ou suas subclasses; nao jogue tipos primitivos
- Marque funcoes que nao lancam com `noexcept`
- Em C++23, `std::expected<T, E>` e alternativa moderna a excecoes
- `dynamic_cast` para downcast seguro; **prefira virtuals** quando possivel
- Use `static_cast` em vez de cast estilo C
- `const_cast` e `reinterpret_cast` sao codigo de baixo nivel - use com cuidado

## Proximo capitulo

[Capitulo 16 - A classe string e a STL](capitulo-16.md): `std::string` em detalhe, containers (`vector`, `list`, `map`, `set`), iteradores, algoritmos.
