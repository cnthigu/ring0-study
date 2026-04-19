# Capitulo 1 - Comecando com C++ (Getting Started with C++)

> Traducao nao oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Texto adaptado para portugues brasileiro com fins educacionais. Paginas originais: 9-26.
> Repositorio: [ring0-study](https://github.com/cnthigu/ring0-study)

## Visao geral do capitulo

Neste capitulo voce vai conhecer:

- A historia e a filosofia por tras de C e C++
- Programacao procedural versus orientada a objetos
- Como C++ adiciona conceitos de OO sobre o C
- Como C++ adiciona programacao generica via templates
- Os padroes da linguagem (C++98, C++03, C++11)
- O fluxo basico de criacao de um programa: codigo-fonte, compilacao, link e executavel

C++ junta tres mundos: o **procedural** (vindo do C), o **orientado a objetos** (classes) e a **programacao generica** (templates). Essa heranca tripla deixa a linguagem poderosa, mas tambem significa que tem bastante coisa para aprender.

## Aprendendo C++: o que esperar

Se voce ja sabe C, esta com meio caminho andado, mas tera que **desaprender alguns vicios** ao migrar. Se nao sabe C, vai aprender as bases do C dentro do livro mesmo, sem dependencia de conhecimento previo. O livro cobre primeiro o que C++ herda do C, depois entra em classes, e por fim em templates.

Nao e um livro de referencia completo, mas cobre quase todos os recursos importantes da linguagem, incluindo `templates`, `exceptions` e `namespaces`.

## A origem do C

Em meados dos anos 1970, **Dennis Ritchie**, no Bell Labs, estava trabalhando no sistema operacional Unix e precisava de uma linguagem que fosse:

- Compacta e rapida
- Capaz de acessar o hardware diretamente (igual `assembly`)
- Mas portatil entre diferentes maquinas (igual uma linguagem de alto nivel)

A solucao foi criar o C, combinando eficiencia de baixo nivel com generalidade de alto nivel.

### Filosofia procedural do C

O C e uma linguagem **procedural**: o programador define o **algoritmo** (a sequencia de passos) que opera sobre os **dados**. Programa = dados + algoritmos.

Para evitar o "spaghetti code" das primeiras linguagens (FORTRAN, BASIC), o C adotou:

- **Programacao estruturada**: poucas estruturas de controle bem definidas (`for`, `while`, `do while`, `if else`).
- **Top-down design**: dividir o problema grande em funcoes menores.

## A virada do C++: programacao orientada a objetos

Mesmo com programacao estruturada, sistemas grandes continuavam dificeis de manter. **Bjarne Stroustrup**, tambem do Bell Labs, comecou nos anos 80 a estender o C com ideias da linguagem Simula67. Surgiu o **C++**.

A ideia central da OOP e parar de pensar primeiro nos algoritmos e passar a pensar primeiro nos **dados** que representam o problema:

- Uma `class` descreve um novo **tipo de dado** (quais campos ele tem e quais operacoes ele permite).
- Um **objeto** e uma instancia daquela `class`.

Por exemplo, em um programa de desenho voce poderia ter uma classe `Rectangle` com dados (posicao, largura, altura, cor) e operacoes (mover, redimensionar, rotacionar). Cada retangulo desenhado e um objeto dessa classe.

OOP traz tambem:

- **Information hiding**: proteger dados de acesso indevido (`private`).
- **Polymorphism**: o mesmo nome de funcao/operador se comporta diferente conforme o contexto.
- **Inheritance**: derivar uma classe nova a partir de outra ja existente.

Em vez de top-down, OOP costuma ser **bottom-up**: voce projeta as classes primeiro e depois monta o programa.

## Programacao generica e templates

OOP foca em **dados** e organizacao de programas grandes. **Programacao generica** foca em **independencia do tipo de dado**.

Exemplo: voce quer escrever uma funcao de ordenacao que funcione tanto para `int`, quanto para `double`, quanto para `std::string`. Sem genericidade, voce escreveria uma versao para cada tipo. Com **templates**, voce escreve uma versao so:

```cpp
#include <iostream>
#include <string>

template <typename T>
T menor(T a, T b) {
    return (a < b) ? a : b;
}

int main() {
    std::cout << menor(3, 7) << "\n";
    std::cout << menor(2.5, 1.7) << "\n";
    std::cout << menor(std::string{"alfa"}, std::string{"beta"}) << "\n";
    return 0;
}
```

> **Rode no Visual Studio ou compile com `g++ -std=c++17 menor.cpp -o menor`**

Esse `menor` aceita qualquer tipo `T` que suporte o operador `<`.

### Em C++ moderno (20)

Em C++20 voce pode usar **conceitos** (`concepts`) para deixar explicito o requisito sobre `T`:

```cpp
#include <concepts>

template <typename T>
requires std::totally_ordered<T>
T menor(T a, T b) {
    return (a < b) ? a : b;
}
```

Isso da mensagens de erro muito mais legiveis quando alguem passa um tipo que nao tem `<`.

## Padroes da linguagem (Standards)

C++ e uma linguagem padronizada por ISO. As versoes principais sao:

| Versao | Ano | Principais novidades |
|---|---|---|
| C++98 | 1998 | Padrao original: STL, exceptions, RTTI, templates |
| C++03 | 2003 | Revisao tecnica (correcoes, sem novos recursos) |
| C++11 | 2011 | `auto`, lambdas, `nullptr`, range-based `for`, smart pointers, move semantics |
| C++14 | 2014 | Refinamentos (lambdas genericas, `std::make_unique`) |
| C++17 | 2017 | `std::optional`, `std::variant`, `std::string_view`, structured bindings, `if constexpr` |
| C++20 | 2020 | `concepts`, `ranges`, `coroutines`, `modules`, `std::span` |
| C++23 | 2023 | `std::expected`, `std::print`, monadic operations |

O livro original (2012) cobre principalmente **C++98** e introduz partes do **C++11**. Quando o codigo do livro envelheceu, esta traducao adiciona blocos **"Em C++ moderno"**.

## A mecanica de criar um programa

Independente do compilador, os passos para rodar um programa C++ sao:

1. **Escrever o codigo-fonte** em um arquivo `.cpp` (texto puro).
2. **Compilar**: o compilador transforma o `.cpp` em codigo objeto (`.o` ou `.obj`).
3. **Linkar**: o linker junta seu codigo objeto com o codigo das bibliotecas (incluindo a STL) e gera o **executavel**.

```
codigo-fonte  ->  [COMPILADOR]  ->  codigo objeto
                                         |
                                         v
                                    [LINKER]  ->  executavel
                                         ^
                                         |
                                  bibliotecas (.lib/.a)
```

### No Visual Studio

1. Crie um projeto **Console Application (Empty Project)** (Win32 ou x64).
2. Adicione um arquivo `.cpp` com seu codigo.
3. Build > Build Solution (`Ctrl+Shift+B`).
4. Run (`Ctrl+F5` para rodar sem debugger).

### Na linha de comando (g++ ou clang++)

```bash
# compila + linka em um passo, gera o executavel "ola"
g++ -std=c++20 -Wall ola.cpp -o ola
./ola
```

Para C++11 em compiladores antigos pode ser necessario passar `-std=c++0x` (nome antigo do C++11). Hoje use sempre `-std=c++17`, `-std=c++20` ou `-std=c++23` se disponivel.

### Dica: use Compiler Explorer

[godbolt.org](https://godbolt.org/) deixa voce testar codigo C++ direto no navegador, ver o assembly gerado e comparar varios compiladores. Excelente para entender o que o compilador faz com seu codigo.

## Primeiro programa: "Ola, mundo!"

Este programa nao aparece exatamente assim no capitulo 1 (o livro deixa para o cap. 2), mas para sentir o gosto de C++ ja agora:

```cpp
#include <iostream>

int main() {
    std::cout << "Ola, mundo!" << std::endl;
    return 0;
}
```

> **Rode no Visual Studio ou compile com `g++ ola.cpp -o ola`**

Se rodar no Windows e a janela fechar imediatamente apos a execucao, adicione antes do `return`:

```cpp
std::cin.get();
```

Para esperar voce apertar Enter antes de fechar.

## Resumo

- C nasceu nos anos 70 (Dennis Ritchie) como uma linguagem de baixo nivel portatil.
- C++ nasceu nos anos 80 (Bjarne Stroustrup) adicionando OOP e depois templates ao C.
- A linguagem segue padroes ISO; o livro cobre C++98/C++11, mas hoje (2026) o mainstream e C++17 / C++20.
- O ciclo de desenvolvimento e sempre: editar -> compilar -> linkar -> executar.
- Use Visual Studio (Windows), `g++` ou `clang++` (Linux/macOS) ou Compiler Explorer (qualquer lugar).

## Proximo capitulo

[Capitulo 2 - Primeiros Passos em C++ (Setting Out to C++)](capitulo-02.md): a estrutura basica de um programa, `cout`, `cin`, comentarios, variaveis e funcoes simples.
