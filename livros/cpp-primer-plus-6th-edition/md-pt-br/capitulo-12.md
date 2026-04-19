# Capitulo 12 - Classes e Alocacao Dinamica de Memoria (Classes and Dynamic Memory Allocation)

> Traducao nao oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Texto adaptado para portugues brasileiro com fins educacionais. Paginas originais: 627-706.
> Repositorio: [ring0-study](https://github.com/cnthigu/ring0-study)

## Visao geral do capitulo

- Membros que alocam memoria dinamica
- Construtor de copia
- Operador de atribuicao
- A "regra dos tres" (e dos cinco em C++11+)
- Move semantics (`&&`, `std::move`)
- Membros estaticos
- Sobrecarga de `new`/`delete`

## O problema: classes que alocam memoria

```cpp
class StringSimples {
    char* dados;
    size_t tam;

public:
    StringSimples(const char* s) {
        tam   = std::strlen(s);
        dados = new char[tam + 1];
        std::strcpy(dados, s);
    }

    ~StringSimples() {
        delete[] dados;          // OK, libera
    }
};
```

Bug latente:

```cpp
StringSimples a{"ola"};
StringSimples b = a;          // construtor de copia IMPLICITO

// Ambos a e b apontam pro MESMO buffer.
// Quando saem de escopo, os dois fazem delete[] no mesmo buffer:
// DOUBLE FREE -> crash ou undefined behavior!
```

## Construtor de copia padrao - shallow copy

Quando voce nao define, o compilador gera um construtor de copia que **copia membro a membro** (shallow copy). Para ponteiros, isso copia o **endereco**, nao o conteudo.

Para classes que possuem recursos (memoria, arquivos, sockets), voce precisa controlar:

## A Regra dos Tres (C++ classico)

Se voce precisa de **um** desses, geralmente precisa dos **tres**:

1. **Destrutor** (libera o recurso)
2. **Construtor de copia** (copia profunda)
3. **Operador de atribuicao** (limpa o antigo, copia profunda)

```cpp
class StringSimples {
    char*  dados;
    size_t tam;

public:
    StringSimples(const char* s = "") {
        tam   = std::strlen(s);
        dados = new char[tam + 1];
        std::strcpy(dados, s);
    }

    ~StringSimples() { delete[] dados; }

    // Construtor de copia
    StringSimples(const StringSimples& outro) {
        tam   = outro.tam;
        dados = new char[tam + 1];
        std::strcpy(dados, outro.dados);
    }

    // Operador de atribuicao
    StringSimples& operator=(const StringSimples& outro) {
        if (this == &outro) return *this;     // self-assignment
        delete[] dados;
        tam   = outro.tam;
        dados = new char[tam + 1];
        std::strcpy(dados, outro.dados);
        return *this;
    }
};
```

## A Regra dos Cinco (C++11+)

Adicione duas operacoes para **move semantics** (mover sem copiar):

4. **Construtor de movimento**
5. **Operador de atribuicao por movimento**

```cpp
class StringSimples {
    char*  dados;
    size_t tam;

public:
    // ... (construtor, destrutor, copia como antes)

    // Construtor de movimento - "rouba" os dados de outro
    StringSimples(StringSimples&& outro) noexcept
        : dados{outro.dados}, tam{outro.tam}
    {
        outro.dados = nullptr;     // outro fica vazio mas valido
        outro.tam   = 0;
    }

    // Atribuicao por movimento
    StringSimples& operator=(StringSimples&& outro) noexcept {
        if (this == &outro) return *this;
        delete[] dados;
        dados = outro.dados;
        tam   = outro.tam;
        outro.dados = nullptr;
        outro.tam   = 0;
        return *this;
    }
};
```

### `std::move` - sinaliza "voce pode roubar"

```cpp
StringSimples a{"texto enorme"};
StringSimples b = std::move(a);   // chama o construtor de movimento
                                   // a fica vazia (valid but unspecified)
```

`std::move` nao move nada - apenas faz um cast para `T&&`, dizendo ao compilador "estou abrindo mao deste objeto, voce pode roubar de dentro dele".

### Por que move e mais rapido?

Imagine `std::vector<int>` com 1 milhao de inteiros:

- **Copia**: aloca novo buffer de 1M ints, copia tudo. Lento.
- **Move**: copia 3 ponteiros (data, size, capacity). Rapido O(1).

## Regra dos zero (Rule of Zero)

Em C++ moderno, a melhor pratica e: **nao implemente nada manualmente**. Use tipos que ja gerenciam recursos:

```cpp
class Pessoa {
    std::string                   nome;        // string ja gerencia memoria
    std::vector<int>              notas;       // vector tambem
    std::unique_ptr<Endereco>     endereco;    // smart pointer cuida sozinho
    // sem destrutor, sem construtor de copia, sem move - TUDO automatico!
};
```

A regra dos zero diz: se voce **nao** usa `new`/`delete` manuais, voce nao precisa implementar destrutor, copia, move - o compilador gera o correto.

## `= default` e `= delete`

```cpp
class A {
public:
    A() = default;                       // forca construtor padrao
    A(const A&) = default;               // forca copia padrao
    A(A&&) = default;                    // forca move padrao
    A& operator=(const A&) = default;
    A& operator=(A&&) = default;
    ~A() = default;
};

class NaoCopiavel {
public:
    NaoCopiavel(const NaoCopiavel&) = delete;            // PROIBE copia
    NaoCopiavel& operator=(const NaoCopiavel&) = delete; // PROIBE atribuicao
    NaoCopiavel(NaoCopiavel&&) = default;                // permite move
    NaoCopiavel& operator=(NaoCopiavel&&) = default;
};
```

`std::unique_ptr` e exatamente isso: nao copiavel, mas movivel.

## `noexcept` em moves

```cpp
StringSimples(StringSimples&& outro) noexcept;
StringSimples& operator=(StringSimples&& outro) noexcept;
```

`noexcept` diz ao compilador "esta funcao **nunca** lanca excecao". Isso e **importante** para moves: containers como `std::vector` so usam move (em vez de copia, mais cara) se o move for `noexcept`. Senao usam copia para garantir strong exception safety.

## Self-assignment

```cpp
A a;
a = a;                  // pode acontecer indiretamente!
```

Trate em `operator=`:

```cpp
A& operator=(const A& outro) {
    if (this == &outro) return *this;     // protege
    // ...
}
```

### Padrao copy-and-swap

Estilo elegante que resolve self-assignment e exception safety de uma vez:

```cpp
A& operator=(A outro) {        // recebe POR VALOR (faz copia ou move)
    swap(*this, outro);
    return *this;
}

friend void swap(A& a, A& b) noexcept {
    using std::swap;
    swap(a.dados, b.dados);
    swap(a.tam,   b.tam);
}
```

Quando o argumento sai de escopo, ele leva embora os dados antigos.

## Membros estaticos

Compartilhados por toda a classe:

```cpp
class Contador {
    static int total;
    int meu_id;
public:
    Contador() : meu_id{++total} {}
    static int obter_total() { return total; }
};

int Contador::total = 0;        // definicao em .cpp

Contador a, b, c;
std::cout << Contador::obter_total();  // 3
```

Em C++17, `inline static` permite definir no header sem `.cpp`:

```cpp
class Contador {
public:
    inline static int total = 0;        // C++17
};
```

## Sobrecarga de `new` e `delete`

Voce pode customizar como sua classe aloca memoria:

```cpp
class A {
public:
    void* operator new(size_t sz) {
        std::cout << "alocando " << sz << " bytes\n";
        return std::malloc(sz);
    }

    void operator delete(void* p) {
        std::cout << "liberando\n";
        std::free(p);
    }
};

A* p = new A;     // chama A::operator new
delete p;          // chama A::operator delete
```

Util para pools de memoria, debugging, profiling. Em codigo de aplicacao normal **raramente** precisa.

## Posicionar `new` (placement new)

Constroi um objeto **em memoria ja alocada**:

```cpp
#include <new>

char buffer[sizeof(A)];
A* p = new (buffer) A{};      // constroi A no buffer

p->~A();                       // tem que destruir manualmente!
```

Util pra otimizacoes em alto desempenho (game engines, alocadores customizados).

## Resumo

- Classes que possuem recursos brutos precisam controlar copia/destruicao
- **Regra dos tres**: destrutor + copia + atribuicao
- **Regra dos cinco** (C++11+): + move + atribuicao por move
- **Regra do zero**: use tipos da std (`string`, `vector`, `unique_ptr`) e nao precise implementar nada
- `= default` para gerar a versao padrao; `= delete` para proibir
- Marque moves como **`noexcept`** para que containers usem eles
- Use `std::move` para passar ownership
- Em C++ moderno, raramente voce escreve copia/move manualmente

## Proximo capitulo

[Capitulo 13 - Heranca de Classes](capitulo-13.md): heranca, polimorfismo, funcoes virtuais, classes abstratas, herancia multipla.
