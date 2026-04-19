# Capitulo 14 - Reusando Codigo em C++ (Reusing Code in C++)

> Traducao nao oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Texto adaptado para portugues brasileiro com fins educacionais. Paginas originais: 785-876.
> Repositorio: [ring0-study](https://github.com/cnthigu/ring0-study)

## Visao geral do capitulo

- Containment / composicao (`has-a`)
- Heranca privada e protegida
- Templates de classe
- Especializacao de templates
- Templates com parametros nao-tipo
- Classes aninhadas
- Heranca multipla revisitada

## Containment (composicao) - "has-a"

A forma mais comum de reuso: incluir objetos de uma classe como **membros** de outra:

```cpp
#include <string>
#include <vector>

class Endereco {
    std::string rua;
    std::string cidade;
    std::string cep;
public:
    Endereco(const std::string& r, const std::string& c, const std::string& z)
        : rua{r}, cidade{c}, cep{z} {}
    void mostrar() const {
        std::cout << rua << ", " << cidade << " (" << cep << ")\n";
    }
};

class Pessoa {
    std::string         nome;
    int                 idade;
    Endereco            endereco;          // composicao - "tem um"
    std::vector<int>    notas;             // composicao
public:
    Pessoa(const std::string& n, int i, const Endereco& e)
        : nome{n}, idade{i}, endereco{e} {}

    void apresentar() const {
        std::cout << nome << ", " << idade << " anos\n";
        endereco.mostrar();
    }
};
```

> **Composicao quase sempre vence sobre heranca**. Use heranca apenas quando ha relacao "is-a" verdadeira **e** voce quer polimorfismo.

## Heranca privada

Sintaticamente possivel, semanticamente significa "implementado em termos de":

```cpp
class Stack : private std::vector<int> {     // Stack USA vector internamente
    using vec = std::vector<int>;
public:
    void push(int v)         { vec::push_back(v); }
    void pop()               { vec::pop_back(); }
    int  top() const         { return vec::back(); }
    bool empty() const       { return vec::empty(); }
    size_t size() const      { return vec::size(); }
};
```

> **Em C++ moderno**, prefira **composicao** (membro `std::vector<int> dados`) em vez de heranca privada. Codigo mais simples e mais flexivel.

## Templates de classe

Templates permitem voce escrever **classes genericas** que funcionam com qualquer tipo:

```cpp
// Stack.h
template <typename T>
class Stack {
    std::vector<T> dados;
public:
    void push(const T& v) { dados.push_back(v); }
    void pop()            { dados.pop_back(); }
    T&   top()            { return dados.back(); }
    const T& top() const  { return dados.back(); }
    bool empty() const    { return dados.empty(); }
    size_t size() const   { return dados.size(); }
};
```

```cpp
int main()
{
    Stack<int>          ints;
    Stack<std::string>  strs;
    Stack<double>       doubles;

    ints.push(42);
    strs.push("ola");
    doubles.push(3.14);
}
```

> **Rode no Visual Studio ou compile com `g++ -std=c++17 stack.cpp -o stack`**

### Definicoes fora da classe

```cpp
template <typename T>
class Stack {
    std::vector<T> dados;
public:
    void push(const T& v);
    T& top();
};

template <typename T>
void Stack<T>::push(const T& v) {
    dados.push_back(v);
}

template <typename T>
T& Stack<T>::top() {
    return dados.back();
}
```

> **Importante**: implementacao de templates **deve estar visivel onde usada**, normalmente no proprio header `.h`. Nao da pra dividir em `.cpp` (na maioria dos casos).

## Templates com parametros nao-tipo

Voce pode passar **valores** (nao so tipos) como parametros de template:

```cpp
template <typename T, std::size_t N>
class ArrayFixo {
    T dados[N];
public:
    T& operator[](std::size_t i)       { return dados[i]; }
    const T& operator[](std::size_t i) const { return dados[i]; }
    constexpr std::size_t size() const { return N; }
};

ArrayFixo<int, 10>   arr1;
ArrayFixo<double, 5> arr2;
```

`std::array<T, N>` e exatamente isso.

## Especializacao de templates

### Especializacao completa

```cpp
template <typename T>
class Imprimir {
public:
    void mostrar(const T& v) { std::cout << v << "\n"; }
};

// Especializacao para bool
template <>
class Imprimir<bool> {
public:
    void mostrar(bool v) {
        std::cout << (v ? "verdadeiro" : "falso") << "\n";
    }
};
```

### Especializacao parcial (so para classes)

```cpp
template <typename T, typename U>
class Par { /* generica */ };

template <typename T>
class Par<T, T> { /* quando ambos sao iguais */ };

template <typename T>
class Par<T*, T*> { /* quando ambos sao ponteiros do mesmo tipo */ };
```

## Classes aninhadas (nested classes)

```cpp
class Lista {
    struct No {                          // tipo aninhado, so visivel dentro
        int  valor;
        No*  proximo;
    };

    No* cabeca = nullptr;

public:
    void adicionar(int v) {
        cabeca = new No{v, cabeca};
    }
};

// Lista::No no;        // ERRO se for private; OK se public
```

Util para tipos auxiliares que so fazem sentido no contexto da classe (ex: iteradores, nodes de listas/arvores).

## Templates com membros template

```cpp
template <typename T>
class Container {
public:
    template <typename U>
    void copiar_de(const Container<U>& outro) {
        // converte U para T
    }
};
```

A STL usa muito isso (iteradores, conversoes de containers).

## Heranca multipla - exemplo completo

```cpp
class Voador {
public:
    virtual void voar() = 0;
    virtual ~Voador() = default;
};

class Nadador {
public:
    virtual void nadar() = 0;
    virtual ~Nadador() = default;
};

class Pato : public Voador, public Nadador {
public:
    void voar()  override { std::cout << "Pato voando\n"; }
    void nadar() override { std::cout << "Pato nadando\n"; }
};
```

Isso e o padrao **interfaces multiplas** - heranca multipla **de classes abstratas** (so virtuais puras). Funciona como `interface` em Java/C#.

### O problema do diamante

```cpp
class Animal { public: int x; };
class Voador  : public Animal { };
class Nadador : public Animal { };
class Pato    : public Voador, public Nadador { };

Pato p;
p.x = 5;        // ERRO ambiguo - dois Animal!
p.Voador::x = 5;
```

Solucao: heranca virtual para Animal:

```cpp
class Voador  : public virtual Animal { };
class Nadador : public virtual Animal { };
class Pato    : public Voador, public Nadador { };

Pato p;
p.x = 5;        // OK - so existe um Animal agora
```

> Heranca multipla de classes concretas e quase sempre desnecessaria. Use **interfaces** ou **composicao**.

## Resumo

- **Composicao** (has-a) e quase sempre melhor que heranca
- **Heranca publica** = is-a com polimorfismo
- **Heranca privada** = "implementado em termos de" - prefira composicao
- **Templates de classe** sao a base da STL
- Implementacao de templates fica no `.h` (visivel onde usada)
- **Especializacoes** customizam o comportamento para tipos especificos
- **Classes aninhadas** organizam tipos auxiliares
- Heranca multipla e segura **de interfaces**; suspeita de classes concretas

## Proximo capitulo

[Capitulo 15 - Friends, Exceptions e Mais](capitulo-15.md): classes amigas, excecoes (`try`/`catch`/`throw`), RTTI em detalhe, casts.
