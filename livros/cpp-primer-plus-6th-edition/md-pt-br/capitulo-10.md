# Capitulo 10 - Objetos e Classes (Objects and Classes)

> Traducao nao oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Texto adaptado para portugues brasileiro com fins educacionais. Paginas originais: 505-562.
> Repositorio: [ring0-study](https://github.com/cnthigu/ring0-study)

## Visao geral do capitulo

- Programacao orientada a objetos (OOP)
- Classes e objetos
- `public`, `private`
- Construtores e destrutores
- O ponteiro `this`
- Arrays de objetos
- Escopo de classe
- Membros `const`

## Programacao orientada a objetos

OOP foca em **dados** ao inves de **algoritmos**:

- **Abstracao**: representar conceitos do mundo real
- **Encapsulamento**: esconder detalhes internos
- **Reuso**: classes podem ser reutilizadas em varios programas
- **Heranca**: criar classes novas a partir de existentes (cap. 13)
- **Polimorfismo**: mesmo nome, comportamento diferente conforme o tipo (cap. 13)

## Definindo uma classe

Uma `class` e um **tipo definido pelo usuario**: combina dados (membros) e operacoes (metodos):

```cpp
// Stock.h
#ifndef STOCK_H
#define STOCK_H

#include <string>

class Stock {
private:
    std::string company;
    long        shares;
    double      share_val;
    double      total_val;

    void set_total() { total_val = shares * share_val; }

public:
    Stock();                                     // construtor padrao
    Stock(const std::string& co, long n = 0, double pr = 0.0);  // construtor
    ~Stock();                                    // destrutor

    void buy(long num, double price);
    void sell(long num, double price);
    void update(double price);
    void show() const;                           // const = nao modifica o objeto
};

#endif
```

```cpp
// Stock.cpp
#include "Stock.h"
#include <iostream>

Stock::Stock() : company{"sem nome"}, shares{0}, share_val{0.0}, total_val{0.0} {}

Stock::Stock(const std::string& co, long n, double pr)
    : company{co}, shares{n}, share_val{pr}
{
    set_total();
}

Stock::~Stock() {
    // limpeza, se precisar
}

void Stock::buy(long num, double price) {
    if (num < 0) return;
    shares    += num;
    share_val  = price;
    set_total();
}

void Stock::sell(long num, double price) {
    if (num < 0 || num > shares) return;
    shares    -= num;
    share_val  = price;
    set_total();
}

void Stock::update(double price) {
    share_val = price;
    set_total();
}

void Stock::show() const {
    std::cout << company << ": " << shares
              << " a " << share_val << " (total " << total_val << ")\n";
}
```

```cpp
// main.cpp
#include "Stock.h"

int main()
{
    Stock acme{"Acme Inc.", 100, 50.0};
    acme.show();
    acme.buy(50, 55.0);
    acme.update(60.0);
    acme.show();
    acme.sell(80, 58.0);
    acme.show();
}
```

> **Rode no Visual Studio ou compile com `g++ -std=c++17 Stock.cpp main.cpp -o app`**

## `public`, `private`, `protected`

| Modificador | Quem pode acessar |
|---|---|
| `public` | Qualquer um |
| `private` | So a propria classe (e `friend`s) |
| `protected` | A propria classe + classes derivadas (cap. 13) |

```cpp
class Conta {
private:
    double saldo;          // ninguem mexe direto - so via metodos

public:
    void depositar(double v) {
        if (v > 0) saldo += v;
    }
    double obter_saldo() const { return saldo; }
};

Conta c;
c.saldo = 1000000;        // ERRO - private
c.depositar(100);          // OK
```

> **Encapsulamento**: deixe os dados `private` e exponha apenas metodos `public` necessarios. Isso protege seus dados de uso indevido e te da liberdade pra mudar a implementacao depois.

### `class` vs `struct` em C++

A unica diferenca: `class` tem membros `private` por padrao; `struct` tem `public`.

```cpp
class A { int x; };           // x e private
struct B { int x; };          // x e public

class C { public: int x; };   // C e B sao equivalentes na pratica
```

**Convencao moderna**:

- Use `struct` para tipos simples agregadores (so dados, sem invariantes)
- Use `class` para tipos com invariantes/encapsulamento

## Construtores

Constroem o objeto. Tem o **mesmo nome da classe** e nenhum tipo de retorno:

```cpp
class Ponto {
public:
    int x, y;

    Ponto() : x{0}, y{0} {}                    // construtor padrao
    Ponto(int a, int b) : x{a}, y{b} {}        // construtor com argumentos
    Ponto(const Ponto& outro)                  // construtor de copia
        : x{outro.x}, y{outro.y} {}
};

Ponto p1;            // chama Ponto()
Ponto p2{3, 4};      // chama Ponto(int, int)
Ponto p3 = p2;       // chama Ponto(const Ponto&)
Ponto p4{p2};        // tambem construtor de copia
```

### Lista de inicializacao

```cpp
class A {
    int x;
    int y;
public:
    A(int a, int b) : x{a}, y{b} {}    // PREFIRA: lista de inicializacao
    // A(int a, int b) { x = a; y = b; }   // funciona, mas pior
};
```

A lista de inicializacao **constroi** os membros direto. A versao com `=` no corpo: cria membros default e depois atribui (duas operacoes em vez de uma). Para tipos primitivos da na mesma. Para classes, a lista e mais eficiente.

> **Membros `const` e referencias DEVEM ser inicializados na lista** (nao podem ser atribuidos depois):
>
> ```cpp
> class B {
>     const int N;
>     int& ref;
> public:
>     B(int n, int& r) : N{n}, ref{r} {}     // unica forma
> };
> ```

### Construtor padrao implicito

Se voce **nao define nenhum construtor**, o compilador cria um padrao (sem argumentos, faz nada). Mas se voce define **qualquer** construtor, o padrao **some**:

```cpp
class A { };
A a;                  // OK - usa o padrao implicito

class B {
public:
    B(int x) {}
};
B b1{5};              // OK
B b2;                 // ERRO - nao tem construtor padrao
```

Para forcar o padrao mesmo definindo outros (C++11):

```cpp
class B {
public:
    B() = default;        // forca o padrao
    B(int x) {}
    B(const B&) = delete; // PROIBE copia
};
```

### Em C++ moderno: brace-init de membros

```cpp
class Config {
    int    timeout = 30;       // valor padrao no proprio membro (C++11)
    bool   debug   = false;
    double version{1.0};

public:
    Config() = default;
    Config(int t) : timeout{t} {}    // sobrescreve so o timeout
};
```

Isso elimina muita repeticao em construtores.

## Destrutores

Chamado **automaticamente** quando o objeto e destruido (sai de escopo, ou voce da `delete`):

```cpp
class Recurso {
    int* dados;
public:
    Recurso(int n) : dados{new int[n]} {}
    ~Recurso() { delete[] dados; }   // libera no destrutor (RAII)
};

void f() {
    Recurso r{100};   // construtor: aloca
}                     // destrutor: libera AUTOMATICAMENTE
```

Em C++ moderno, voce raramente precisa escrever destrutores: smart pointers e containers cuidam dos seus proprios destrutores.

## O ponteiro `this`

Todo metodo nao-estatico recebe um ponteiro implicito chamado `this` para o objeto sendo manipulado:

```cpp
class A {
    int x;
public:
    void set_x(int x) {
        this->x = x;       // resolve a ambiguidade do nome
    }

    A& adicionar(int v) {
        this->x += v;
        return *this;       // permite chaining
    }
};

A a;
a.set_x(10).adicionar(5).adicionar(3);   // chaining
```

## Metodos `const`

Metodos marcados `const` **nao podem modificar** o objeto:

```cpp
class Conta {
    double saldo;
public:
    double obter_saldo() const { return saldo; }    // so le, OK
    // void zerar() const { saldo = 0; }            // ERRO - modifica
};

const Conta c;
c.obter_saldo();                       // OK - const pode chamar metodos const
// c.zerar();                          // ERRO - nao pode chamar nao-const
```

> **Regra**: marque `const` **todos** os metodos que nao modificam o objeto. Isso documenta a intencao e permite usar a classe com `const` e em contextos thread-safe.

## Arrays de objetos

```cpp
Stock minha_carteira[3] = {
    Stock{"Acme", 100, 50.0},
    Stock{"Beta", 200, 25.0},
    Stock{"Coca", 150, 60.0}
};

for (const auto& s : minha_carteira) {
    s.show();
}
```

Em C++ moderno, prefira `std::vector<Stock>` ou `std::array<Stock, 3>`.

## Membros `static`

Compartilhados por **todas as instancias** da classe:

```cpp
class Contador {
    static int total;       // declaracao
public:
    Contador()  { ++total; }
    ~Contador() { --total; }
    static int obter_total() { return total; }
};

int Contador::total = 0;    // definicao (em .cpp)

Contador a, b, c;
std::cout << Contador::obter_total();   // 3
```

### Em C++17: `inline` static

```cpp
class Contador {
public:
    inline static int total = 0;    // definicao no .h, sem precisar de .cpp
};
```

## Resumo

- Classes encapsulam dados (`private`) e operacoes (`public`)
- Use `class` quando ha invariantes; `struct` para agregadores simples
- Construtores constroem; **use sempre lista de inicializacao** (`: x{a}, y{b}`)
- Destrutor libera recursos automaticamente (base de RAII)
- Marque metodos como `const` quando eles nao modificam o objeto
- C++11+ permite valores padrao em membros (`int x = 0;`)
- C++11+ permite `= default` e `= delete` para controlar construtores especiais
- Em C++ moderno, escreva destrutores manuais raramente - deixe smart pointers e containers cuidarem

## Proximo capitulo

[Capitulo 11 - Trabalhando com Classes](capitulo-11.md): sobrecarga de operadores, friend, conversoes entre tipos.
