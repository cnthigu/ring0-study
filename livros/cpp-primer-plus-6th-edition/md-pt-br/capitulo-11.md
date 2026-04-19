# Capitulo 11 - Trabalhando com Classes (Working with Classes)

> Traducao nao oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Texto adaptado para portugues brasileiro com fins educacionais. Paginas originais: 563-626.
> Repositorio: [ring0-study](https://github.com/cnthigu/ring0-study)

## Visao geral do capitulo

- Sobrecarga de operadores (`operator overloading`)
- Funcoes `friend`
- Conversoes de tipo entre classes
- Classes para vetores 2D (exemplo classico)
- Operadores de stream (`<<` e `>>`) sobrecarregados

## Sobrecarga de operadores

C++ permite que voce defina o que `+`, `-`, `*`, `==`, etc. significam para suas classes. A sintaxe e definir uma funcao com nome especial `operatorX`:

```cpp
class Vetor {
public:
    double x, y;

    Vetor(double a = 0, double b = 0) : x{a}, y{b} {}

    Vetor operator+(const Vetor& outro) const {
        return Vetor{x + outro.x, y + outro.y};
    }

    Vetor operator-(const Vetor& outro) const {
        return Vetor{x - outro.x, y - outro.y};
    }

    Vetor operator*(double escalar) const {
        return Vetor{x * escalar, y * escalar};
    }

    bool operator==(const Vetor& outro) const {
        return x == outro.x && y == outro.y;
    }

    Vetor& operator+=(const Vetor& outro) {
        x += outro.x;
        y += outro.y;
        return *this;
    }
};
```

```cpp
Vetor a{1, 2};
Vetor b{3, 4};
Vetor c = a + b;          // Vetor{4, 6}
Vetor d = a * 3.0;        // Vetor{3, 6}
a += b;                    // a vira Vetor{4, 6}
if (c == Vetor{4, 6}) { } // true
```

> **Rode no Visual Studio ou compile com `g++ -std=c++17 vetor.cpp -o vetor`**

### Operadores que voce pode sobrecarregar

A maioria: `+`, `-`, `*`, `/`, `%`, `==`, `!=`, `<`, `>`, `<=`, `>=`, `+=`, `-=`, `*=`, `/=`, `<<`, `>>`, `[]`, `()`, `->`, `++`, `--`, `&`, `|`, `^`, `~`, `!`, `&&`, `||`, `,`, `new`, `delete`.

**Nao pode sobrecarregar**: `.`, `.*`, `::`, `?:`, `sizeof`, `typeid`.

### Operadores como membro vs funcao livre

```cpp
class Vetor {
public:
    Vetor operator+(const Vetor& outro) const;   // membro
};

// OU como funcao livre
Vetor operator+(const Vetor& a, const Vetor& b);
```

A funcao livre tem uma vantagem: **simetria**. Se voce so define como membro, o compilador aceita `vetor + 3.0` mas **nao** `3.0 + vetor` (o lado esquerdo nao e do tipo `Vetor`).

```cpp
// Como funcao livre, ambos funcionam:
Vetor operator*(double k, const Vetor& v) { return v * k; }
Vetor operator*(const Vetor& v, double k) { return Vetor{v.x*k, v.y*k}; }
```

### Em C++ moderno (20): spaceship operator

```cpp
class Versao {
public:
    int maior, menor, patch;

    auto operator<=>(const Versao&) const = default;
    bool operator==(const Versao&)  const = default;
};

Versao v1{1, 2, 3};
Versao v2{1, 2, 4};
v1 < v2;       // true (gerado automatico)
v1 == v2;      // false
v1 != v2;      // true
v1 >= v2;      // false
```

Uma linha gera **todos** os 6 operadores relacionais.

## Funcoes `friend`

Uma funcao `friend` da classe pode acessar membros `private` dela:

```cpp
class Conta {
    double saldo;
public:
    Conta(double s) : saldo{s} {}
    friend std::ostream& operator<<(std::ostream& os, const Conta& c);
};

std::ostream& operator<<(std::ostream& os, const Conta& c) {
    os << "Saldo: R$ " << c.saldo;        // acessa private direto!
    return os;
}

int main()
{
    Conta minha{1500.50};
    std::cout << minha << "\n";   // chama operator<<
}
```

> **Atencao**: `friend` quebra encapsulamento. Use **com moderacao**. Casos comuns:
>
> - `operator<<` e `operator>>` (precisam ser funcoes livres com acesso interno)
> - Classes muito relacionadas que cooperam intimamente

### Sobrecarga de `<<` e `>>`

Padrao quase obrigatorio para classes que voce quer imprimir:

```cpp
class Ponto {
    double x, y;
public:
    Ponto(double a, double b) : x{a}, y{b} {}

    friend std::ostream& operator<<(std::ostream& os, const Ponto& p) {
        return os << "(" << p.x << ", " << p.y << ")";
    }

    friend std::istream& operator>>(std::istream& is, Ponto& p) {
        return is >> p.x >> p.y;
    }
};

Ponto p{3, 4};
std::cout << p << "\n";    // (3, 4)
std::cin  >> p;
```

A funcao retorna o stream para permitir chaining: `cout << p1 << p2 << "\n";`.

## Conversoes de tipo

### Construtor de conversao

Construtor de **um argumento** define uma conversao implicita:

```cpp
class Stonewt {
    double libras;
public:
    Stonewt(double lbs) : libras{lbs} {}      // Stonewt(double) - converte
};

Stonewt peso = 150.0;       // implicit conversion: 150.0 -> Stonewt
void f(Stonewt s);
f(125.0);                    // tambem converte!
```

Isso pode ser surpreendente. Em C++ moderno, marque com `explicit` para **desligar** a conversao implicita:

```cpp
class Stonewt {
    double libras;
public:
    explicit Stonewt(double lbs) : libras{lbs} {}
};

Stonewt peso = 150.0;       // ERRO agora
Stonewt peso{150.0};        // OK - construcao explicita
f(Stonewt{125.0});           // OK
```

> **Regra moderna**: marque construtores de **um argumento** com `explicit` por padrao. Tire so quando a conversao implicita faz sentido.

### Operadores de conversao

```cpp
class Stonewt {
    double libras;
public:
    Stonewt(double lbs) : libras{lbs} {}

    operator double() const { return libras; }       // Stonewt -> double
    explicit operator int() const { return int(libras); }  // C++11 explicit
};

Stonewt peso{150.0};
double d = peso;             // OK - conversao implicita
int    i = static_cast<int>(peso);   // precisa cast por causa do explicit
```

## Exemplo completo: classe Time

```cpp
// Time.h
class Time {
    int horas;
    int minutos;
public:
    Time(int h = 0, int m = 0) : horas{h}, minutos{m} { normalizar(); }

    void adicionar_min(int m);
    void adicionar_hr(int h);
    void resetar(int h = 0, int m = 0);

    Time operator+(const Time& t) const;
    Time operator-(const Time& t) const;
    Time operator*(double k)      const;

    friend std::ostream& operator<<(std::ostream& os, const Time& t);

private:
    void normalizar() {
        horas    += minutos / 60;
        minutos   = minutos % 60;
    }
};
```

```cpp
// Time.cpp
#include "Time.h"
#include <iostream>

void Time::adicionar_min(int m) { minutos += m; normalizar(); }
void Time::adicionar_hr(int h)  { horas += h; }
void Time::resetar(int h, int m) { horas = h; minutos = m; normalizar(); }

Time Time::operator+(const Time& t) const {
    return Time{horas + t.horas, minutos + t.minutos};
}

Time Time::operator-(const Time& t) const {
    int total_min = (horas * 60 + minutos) - (t.horas * 60 + t.minutos);
    return Time{0, total_min};
}

Time Time::operator*(double k) const {
    int total_min = static_cast<int>((horas * 60 + minutos) * k);
    return Time{0, total_min};
}

std::ostream& operator<<(std::ostream& os, const Time& t) {
    return os << t.horas << "h" << t.minutos << "m";
}
```

```cpp
// main.cpp
#include "Time.h"
#include <iostream>

int main()
{
    Time codar{4, 35};
    Time pausa{0, 25};

    std::cout << "trabalho: " << codar << "\n";
    std::cout << "pausa:    " << pausa << "\n";
    std::cout << "total:    " << (codar + pausa) << "\n";
    std::cout << "diff:     " << (codar - pausa) << "\n";
    std::cout << "metade:   " << codar * 0.5 << "\n";
}
```

> **Rode no Visual Studio ou compile com `g++ -std=c++17 Time.cpp main.cpp -o time`**

## Resumo

- C++ permite sobrecarregar quase todos os operadores
- Operadores comuns como membros: `+=`, `[]`, `()`, `->`, `++` em proprio objeto
- Operadores binarios simetricos (como `+`): preferivel funcao livre
- `friend` da acesso a membros private; use com moderacao (`<<`, `>>` sao classicos)
- Marque construtores de um argumento com **`explicit`** por padrao
- C++20: `operator<=>` gera todos os relacionais com uma linha
- Sobrecarregue `operator<<` para qualquer classe que voce queira imprimir

## Proximo capitulo

[Capitulo 12 - Classes e Alocacao Dinamica de Memoria](capitulo-12.md): membros que alocam memoria, construtor de copia, operador de atribuicao, regra dos tres/cinco, move semantics.
