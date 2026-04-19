# Capitulo 13 - Heranca de Classes (Class Inheritance)

> Traducao nao oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Texto adaptado para portugues brasileiro com fins educacionais. Paginas originais: 707-784.
> Repositorio: [ring0-study](https://github.com/cnthigu/ring0-study)

## Visao geral do capitulo

- Heranca simples (`is-a`)
- Construtores em heranca
- `protected`
- Metodos `virtual` e polimorfismo
- Funcoes virtuais puras e classes abstratas (ABCs)
- Heranca privada vs publica
- `dynamic_cast` e RTTI

## A relacao "is-a"

Heranca expressa "X **e um** Y":

- Um `Cachorro` e um `Animal`
- Um `Carro` e um `Veiculo`
- Um `LivroIlustrado` e um `Livro`

Nao confunda com "has-a" (composicao):

- Um `Carro` **tem** um `Motor` (composicao, nao heranca)

## Heranca simples

```cpp
class TabelaTennis {
protected:
    std::string nome;
    int         vitorias;
    int         derrotas;
public:
    TabelaTennis(const std::string& n, int v = 0, int d = 0)
        : nome{n}, vitorias{v}, derrotas{d} {}

    void mostrar() const {
        std::cout << nome << ": " << vitorias << "V " << derrotas << "D\n";
    }
};

// Classe derivada
class TabelaPremium : public TabelaTennis {
private:
    std::string preferencia;
public:
    TabelaPremium(const std::string& n, const std::string& p,
                  int v = 0, int d = 0)
        : TabelaTennis{n, v, d}, preferencia{p} {}

    void mostrar_completo() const {
        mostrar();                        // metodo herdado
        std::cout << "Pref: " << preferencia << "\n";
    }
};
```

```cpp
int main()
{
    TabelaPremium p{"Carniato", "Forehand", 10, 2};
    p.mostrar();              // herdado de TabelaTennis
    p.mostrar_completo();     // proprio
}
```

> **Rode no Visual Studio ou compile com `g++ -std=c++17 tennis.cpp -o tennis`**

### `public`, `protected`, `private` na heranca

```cpp
class Derivada : public Base { };       // ISA - publica (PADRAO)
class Derivada : protected Base { };    // raro - "implementacao em termos de"
class Derivada : private Base { };      // composicao via heranca, raramente util
```

99% das vezes voce usa `public`. Para "tem um", prefira **composicao** (campo membro).

## `protected`

Membros `protected`:
- Comportam-se como `private` para usuarios externos
- Comportam-se como `public` para classes derivadas

```cpp
class Base {
protected:
    int x;
};

class Derivada : public Base {
public:
    void f() { x = 10; }    // OK - protected da Base
};

Derivada d;
d.x = 5;                     // ERRO - protected
```

## Construtores em heranca

```cpp
class Base {
    int x;
public:
    Base(int v) : x{v} {}
};

class Derivada : public Base {
    int y;
public:
    Derivada(int a, int b)
        : Base{a},        // chama construtor da Base na lista de inicializacao
          y{b}
    {}
};
```

A classe derivada **deve** chamar o construtor apropriado da Base. Se a Base tem construtor padrao, ele e chamado automaticamente. Se nao tem, voce **precisa** chamar explicitamente.

### Em C++ moderno: herdar construtores (C++11)

```cpp
class Base {
public:
    Base(int);
    Base(double);
    Base(const std::string&);
};

class Derivada : public Base {
public:
    using Base::Base;       // herda TODOS os construtores da Base
};

Derivada d1{42};
Derivada d2{3.14};
Derivada d3{"texto"};
```

## Polimorfismo: metodos `virtual`

Sem `virtual`, o metodo chamado depende do **tipo do ponteiro**, nao do objeto:

```cpp
class Animal {
public:
    void falar() { std::cout << "som generico\n"; }
};

class Cachorro : public Animal {
public:
    void falar() { std::cout << "Au au!\n"; }
};

Cachorro c;
Animal* p = &c;
p->falar();              // "som generico" (chamou Animal::falar!)
```

Com `virtual`, **o tipo real do objeto** decide:

```cpp
class Animal {
public:
    virtual void falar() { std::cout << "som generico\n"; }
    virtual ~Animal() = default;       // SEMPRE adicione destrutor virtual!
};

class Cachorro : public Animal {
public:
    void falar() override { std::cout << "Au au!\n"; }   // C++11: override
};

Cachorro c;
Animal* p = &c;
p->falar();              // "Au au!" - polimorfismo funcionou!
```

> **Regras de ouro**:
> 1. Se a classe vai ser **base** de heranca, faca o destrutor `virtual`. Senao, deletar via ponteiro de base nao chama o destrutor da derivada (vazamento).
> 2. Sempre marque overrides com `override` (C++11). O compilador checa que voce realmente esta sobrescrevendo algo.
> 3. Marque com `final` (C++11) classes ou metodos que nao devem ser mais sobrescritos.

```cpp
class B { public: virtual void f(); };
class D : public B {
public:
    void f() override;          // bom - compilador checa
    void g() override;          // ERRO - g nao existe na base, talvez voce errou o nome
    void f() final;             // ninguem mais pode sobrescrever
};
```

## Como `virtual` funciona

Cada classe com metodos virtuais tem uma **vtable** (tabela de funcoes virtuais). Cada objeto tem um ponteiro pra vtable da sua classe real. Chamar `p->f()` virtual:

1. Acha a vtable do objeto (custo: uma indirecao)
2. Procura `f` na vtable (offset fixo, custo zero)
3. Chama a funcao certa

Custo: 1-2 indirecoes a mais por chamada. Despresivel para a maioria dos casos.

## Slicing - cuidado!

Quando voce **copia por valor** uma classe derivada para uma base, **perde** a parte derivada:

```cpp
Cachorro c{...};
Animal a = c;        // SLICING - so a parte Animal de c foi copiada
a.falar();           // chama Animal::falar (a nao e mais Cachorro!)
```

Por isso polimorfismo funciona com **ponteiros** (`Animal*`) e **referencias** (`Animal&`), nao com valores.

## Funcoes virtuais puras e classes abstratas

```cpp
class Forma {
public:
    virtual double area() const = 0;     // virtual pura - sem corpo
    virtual ~Forma() = default;
};

class Circulo : public Forma {
    double raio;
public:
    Circulo(double r) : raio{r} {}
    double area() const override {
        return 3.14159 * raio * raio;
    }
};

class Quadrado : public Forma {
    double lado;
public:
    Quadrado(double l) : lado{l} {}
    double area() const override {
        return lado * lado;
    }
};

// Forma f;                          // ERRO - classe abstrata
Circulo  c{5};
Quadrado q{4};
Forma*   p = &c;
std::cout << p->area();              // 78.54
p = &q;
std::cout << p->area();              // 16.0
```

Classe abstrata = tem pelo menos uma funcao virtual pura. **Nao pode ser instanciada**, so serve como interface para classes derivadas.

### Em C++ moderno: usar polimorfismo

```cpp
#include <vector>
#include <memory>

std::vector<std::unique_ptr<Forma>> formas;
formas.push_back(std::make_unique<Circulo>(5));
formas.push_back(std::make_unique<Quadrado>(4));

double total = 0;
for (const auto& f : formas) {
    total += f->area();
}
```

## RTTI (Run-Time Type Information)

Permite descobrir o tipo real em tempo de execucao:

### `dynamic_cast`

Cast seguro entre tipos relacionados:

```cpp
Animal* a = obter_animal();

if (Cachorro* c = dynamic_cast<Cachorro*>(a)) {
    c->latir();              // c e nao-null se a realmente apontava pra Cachorro
} else {
    std::cout << "nao era um cachorro\n";
}
```

`dynamic_cast` retorna `nullptr` (para ponteiros) ou lanca `std::bad_cast` (para referencias) se a conversao nao e segura.

### `typeid`

```cpp
#include <typeinfo>

Animal* a = obter_animal();
std::cout << typeid(*a).name() << "\n";       // ex: "Cachorro"
if (typeid(*a) == typeid(Cachorro)) { /* ... */ }
```

> **Em C++ moderno**, prefira **virtuais ou `std::variant`** sobre `dynamic_cast`/`typeid`. Casts dinamicos quase sempre indicam que o design pode ser melhor.

## Heranca multipla

```cpp
class A { public: void fa(); };
class B { public: void fb(); };
class C : public A, public B { };

C c;
c.fa();   // de A
c.fb();   // de B
```

Heranca multipla pode causar o "diamond problem" (dois caminhos pra mesma base). Solucao: heranca virtual:

```cpp
class A { public: int x; };
class B : virtual public A { };
class C : virtual public A { };
class D : public B, public C { };       // D tem APENAS um A (nao dois)
```

> **Em C++ moderno**: heranca multipla **de classes concretas** e raramente boa ideia. Use heranca multipla **de interfaces** (classes abstratas com so virtuais puras), ou prefira composicao.

## Resumo

- Heranca expressa "is-a"; composicao expressa "has-a"
- `class Derivada : public Base` - heranca publica e a regra
- `protected` permite acesso a derivadas mas nao a externos
- Use `virtual` para polimorfismo dinamico
- **Sempre** destrutor `virtual` em classes base
- **Sempre** marque overrides com `override` (C++11)
- Classes abstratas tem pelo menos uma virtual pura (`= 0`)
- Cuidado com slicing: polimorfismo so funciona via ponteiro/referencia
- `dynamic_cast` para downcast seguro; `typeid` para tipo real
- Heranca multipla com cuidado, prefira composicao

## Proximo capitulo

[Capitulo 14 - Reusando Codigo em C++](capitulo-14.md): containment (composicao), heranca privada, templates de classe, classes aninhadas.
