# Capitulo 9 - Memory Models e Namespaces (Memory Models and Namespaces)

> Traducao nao oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Texto adaptado para portugues brasileiro com fins educacionais. Paginas originais: 447-504.
> Repositorio: [ring0-study](https://github.com/cnthigu/ring0-study)

## Visao geral do capitulo

- Compilacao separada (multi-arquivos)
- Storage classes (`automatic`, `static`, `extern`, `thread_local`)
- Linkage (interna vs externa)
- Escopo: bloco, funcao, arquivo, programa
- Especificadores de armazenamento dinamico (`new`/`delete`)
- Namespaces

## Compilacao separada

Programas grandes sao divididos em varios arquivos. O padrao em C++ e:

| Arquivo | Conteudo |
|---|---|
| `.h` (header) | Declaracoes (prototipos, classes, templates) |
| `.cpp` (source) | Definicoes (corpos das funcoes) |

Exemplo - calculadora simples:

**`calc.h`** - declaracoes:

```cpp
#ifndef CALC_H            // include guard
#define CALC_H

namespace calc {
    int  somar(int a, int b);
    int  multiplicar(int a, int b);
    void imprimir_versao();
}

#endif
```

**`calc.cpp`** - definicoes:

```cpp
#include "calc.h"
#include <iostream>

namespace calc {
    int somar(int a, int b) { return a + b; }
    int multiplicar(int a, int b) { return a * b; }

    void imprimir_versao() {
        std::cout << "calc v1.0\n";
    }
}
```

**`main.cpp`** - usa a biblioteca:

```cpp
#include "calc.h"
#include <iostream>

int main()
{
    std::cout << calc::somar(3, 4) << "\n";
    std::cout << calc::multiplicar(5, 6) << "\n";
    calc::imprimir_versao();
}
```

Compilar:

```bash
g++ -std=c++17 calc.cpp main.cpp -o app
```

### Include guards

```cpp
#ifndef MEU_HEADER_H
#define MEU_HEADER_H
// ...
#endif
```

Evita que o mesmo header seja incluido duas vezes (causaria erros de "redefinicao").

Alternativa moderna (compilador-especifica, mas suportada por todos):

```cpp
#pragma once
// conteudo do header
```

`#pragma once` e mais simples e nao precisa de macro unica. Use a menos que voce precise compatibilidade com compiladores muito antigos.

### Em C++ moderno (20): modules

C++20 introduziu **modules** que substituem o sistema de headers/include:

```cpp
// calc.cppm (ou .ixx no MSVC)
export module calc;

export namespace calc {
    int somar(int a, int b) { return a + b; }
}
```

```cpp
// main.cpp
import calc;

int main() { return calc::somar(1, 2); }
```

Modules sao **muito** mais rapidos para compilar (sem reparsear headers a cada vez), mas o suporte ainda esta amadurecendo nos compiladores em 2026.

## Storage classes (classes de armazenamento)

Determinam **onde** a variavel vive na memoria e **quanto tempo**.

### `automatic` (padrao para variaveis locais)

```cpp
void f() {
    int x = 10;       // automatic - vive na stack
    int y{20};        // mesma coisa
    // x e y morrem ao sair da funcao
}
```

A palavra-chave `auto` em C++98 indicava isso, mas em **C++11+ `auto` foi reaproveitada** para deducao de tipo. Hoje `auto` significa "deduza o tipo".

### `static` em variavel local

```cpp
int contador() {
    static int n = 0;     // inicializado UMA vez, persiste entre chamadas
    return ++n;
}

contador();  // 1
contador();  // 2
contador();  // 3
```

`static` local: vive durante toda a vida do programa, mas so e visivel dentro da funcao.

### `static` em variavel global / arquivo

```cpp
// arquivo.cpp
static int privado = 42;        // visivel SO neste arquivo (internal linkage)
int        publico = 100;       // visivel em outros arquivos (external linkage)
```

### `extern`

```cpp
// foo.cpp
int contador_global = 0;

// bar.cpp
extern int contador_global;     // declara que existe em outro lugar
```

`extern` diz ao compilador "essa variavel/funcao existe em algum outro arquivo". O linker cuida de juntar.

### `thread_local` (C++11)

```cpp
thread_local int meu_id = 0;   // cada thread tem sua propria copia
```

Essencial para programacao multithread.

### `const` em escopo de arquivo

```cpp
const int MAX = 100;        // internal linkage POR PADRAO em C++ (diferente de C)
extern const int VERSAO = 7; // external (precisa do extern + inicializar)
```

Por isso voce pode definir `const int X = 10;` em headers sem causar conflito de "definicao multipla".

## Inicializacao de variaveis

```cpp
int a;            // global: zero-initialized; local: indefinido (LIXO!)
int b = 0;        // copy initialization
int c(0);         // direct initialization
int d{0};         // brace initialization (C++11) - PREFERIDO
int e{};          // value initialization - zero
```

### Em C++ moderno

**Sempre inicialize variaveis** (preferencialmente com `{}`).

Para variaveis nao usadas, use `[[maybe_unused]]` (C++17):

```cpp
[[maybe_unused]] int debug_count = 0;
```

## Alocacao dinamica - revisao

```cpp
// Estilo do livro (C++11 manual)
int* p = new int(42);
delete p;

int* arr = new int[100];
delete[] arr;
```

```cpp
// Em C++ moderno (17/20) - sempre prefira
#include <memory>

auto p = std::make_unique<int>(42);
auto arr = std::make_unique<int[]>(100);
// liberacao automatica - sem chance de vazamento ou double-free
```

### Smart pointers - quando usar cada um

| Smart pointer | Uso |
|---|---|
| `std::unique_ptr<T>` | Dono unico. **PADRAO**. Use sempre que possivel. |
| `std::shared_ptr<T>` | Dono compartilhado (reference counted). Use quando varios objetos genuinamente compartilham. |
| `std::weak_ptr<T>` | Observador nao-dono de um `shared_ptr`. Quebra ciclos. |

```cpp
#include <memory>

// unique_ptr - barato, sem overhead
auto p1 = std::make_unique<MinhaClasse>(arg1, arg2);

// shared_ptr - tem refcount, mais caro
auto p2 = std::make_shared<MinhaClasse>(arg1, arg2);
auto p3 = p2;     // refcount = 2

// weak_ptr - observa sem manter vivo
std::weak_ptr<MinhaClasse> obs = p2;
if (auto p = obs.lock()) {
    // p ainda e valido
}
```

> **Regra de ouro**: em C++ moderno, **`new` e `delete` raramente aparecem em codigo de aplicacao**. Use `make_unique`/`make_shared` ou containers (`vector`, `string`, `unordered_map`) que ja gerenciam memoria.

### RAII - Resource Acquisition Is Initialization

A ideia central de C++ moderno: **objetos gerenciam recursos no construtor e os liberam no destrutor**. Sem precisar lembrar de liberar.

```cpp
{
    std::ifstream arquivo("dados.txt");   // construtor abre
    // usar arquivo...
}   // destrutor fecha automaticamente, mesmo se houver excecao!
```

`std::lock_guard`, `std::unique_lock`, `std::scoped_lock` (mutexes), `std::ofstream` (arquivos), smart pointers - tudo segue RAII.

## Escopo (scope)

```cpp
int global = 1;            // escopo de programa

void f() {
    int local = 2;         // escopo de funcao
    {
        int interno = 3;   // escopo de bloco
        // global, local e interno todos visiveis
    }
    // interno NAO e mais visivel
}
```

### Escopo no `for`

```cpp
for (int i = 0; i < 10; ++i) {
    // i so existe aqui dentro
}
// i nao existe mais
```

## Namespaces

Namespaces evitam conflitos de nomes em projetos grandes:

```cpp
namespace minha_lib {
    int valor = 42;
    void f() { /* ... */ }

    namespace detalhes {        // namespace aninhado
        int interno = 99;
    }
}

// Acesso
int x = minha_lib::valor;
minha_lib::f();
int y = minha_lib::detalhes::interno;
```

### `using` declaration vs `using` directive

```cpp
using minha_lib::f;             // declaration: traz so f
using namespace minha_lib;      // directive: traz TUDO

// Em C++17: namespace aninhado abreviado
namespace meu::projeto::utils {
    void g();
}
```

### Namespace anonimo

```cpp
namespace {
    int privado = 0;            // visivel SO neste arquivo
    void helper() { /* ... */ }
}
```

Equivale a `static` em escopo de arquivo, mas mais "C++ idiomatico".

### `inline namespace` (C++11)

```cpp
namespace lib {
    inline namespace v2 {
        void f();              // implementacao atual
    }

    namespace v1 {
        void f();              // versao antiga, ainda acessivel via lib::v1::f()
    }
}

lib::f();        // chama lib::v2::f() (inline)
lib::v1::f();    // versao antiga
```

Usado para versionamento de bibliotecas.

## Resumo

- Programas grandes sao divididos em `.h` (declaracoes) e `.cpp` (definicoes)
- Use `#pragma once` ou include guards
- Storage classes: `automatic` (default), `static`, `extern`, `thread_local`
- `const` em escopo de arquivo tem internal linkage por padrao em C++
- **C++ moderno**: use smart pointers (`unique_ptr`, `shared_ptr`) e containers da STL em vez de `new`/`delete`
- **RAII** e o conceito mais importante de C++: objetos cuidam dos seus recursos
- Use **namespaces** para organizar codigo e evitar conflitos
- C++20 traz **modules** (substitui includes), mas suporte ainda imaturo

## Proximo capitulo

[Capitulo 10 - Objetos e Classes](capitulo-10.md): comeca a parte OOP do livro - definir classes, metodos, construtores, encapsulamento.
