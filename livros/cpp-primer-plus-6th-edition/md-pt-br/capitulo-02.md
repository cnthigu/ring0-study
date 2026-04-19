# Capitulo 2 - Primeiros Passos em C++ (Setting Out to C++)

> Traducao nao oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Texto adaptado para portugues brasileiro com fins educacionais. Paginas originais: 27-64.
> Repositorio: [ring0-study](https://github.com/cnthigu/ring0-study)

## Visao geral do capitulo

Voce vai conhecer:

- Estrutura geral de um programa C++
- A diretiva `#include`
- A funcao `main()`
- O objeto `cout` para saida
- Comentarios em C++
- Como e quando usar `endl`
- Declaracao e uso de variaveis
- O objeto `cin` para entrada
- Definicao e uso de funcoes simples

## Iniciacao em C++ - o primeiro programa

```cpp
// myfirst.cpp -- mostra uma mensagem
#include <iostream>                    // diretiva de pre-processador

int main()                             // cabecalho da funcao
{                                      // inicio do corpo
    using namespace std;               // torna std::cout, std::endl visiveis sem prefixo
    cout << "Come up and C++ me some time.";
    cout << endl;                      // pula para nova linha
    cout << "You won't regret it!" << endl;
    return 0;                          // termina main()
}
```

> **Rode no Visual Studio ou compile com `g++ myfirst.cpp -o myfirst && ./myfirst`**

C++ e **case sensitive**: `cout`, `Cout` e `COUT` sao identificadores diferentes. Use sempre `cout` minusculo.

### Anatomia do programa

| Elemento | O que faz |
|---|---|
| `// comentario` | Comentario de uma linha. O compilador ignora |
| `#include <iostream>` | Inclui o header de stream I/O (define `cout`, `cin`, `endl`) |
| `int main()` | Cabecalho da funcao principal: retorna `int`, sem argumentos |
| `{ ... }` | Corpo da funcao (chaves obrigatorias) |
| `using namespace std;` | Permite usar `cout` em vez de `std::cout`. Conveniente para aprender, evitavel em projeto serio |
| `cout << "texto";` | Insere o texto no fluxo de saida (terminal) |
| `endl` | Insere uma nova linha e faz flush do buffer |
| `return 0;` | Retorna 0 ao SO indicando "executou com sucesso" |

### Sobre `using namespace std`

Conveniente para exemplos pequenos, mas em codigo de producao **prefira** uma destas tres opcoes:

```cpp
// 1) prefixo std:: explicito (mais limpo)
std::cout << "ola\n";

// 2) using declaration apenas para o que voce usa
using std::cout;
cout << "ola\n";

// 3) restringir o using namespace dentro de uma funcao
int main() {
    using namespace std;
    cout << "ola\n";
}
```

O motivo: `using namespace std;` no escopo global puxa **todos** os nomes da `std` e pode causar conflitos (ex: `std::count` versus uma variavel sua chamada `count`).

## Comentarios

```cpp
// comentario de uma linha (estilo C++)

/*
   comentario de varias linhas
   estilo C, ainda valido em C++
*/
```

## A diretiva `#include`

```cpp
#include <iostream>     // header da biblioteca padrao (sem .h)
#include <cmath>        // versao C++ de math.h
#include "minha_lib.h"  // header local (procura primeiro no diretorio atual)
```

Headers de C antigos (`stdio.h`, `math.h`, `string.h`) ainda funcionam em C++, mas o estilo moderno e usar a versao C++ sem `.h` e prefixada com `c`: `<cstdio>`, `<cmath>`, `<cstring>`. Eles colocam tudo dentro do namespace `std`.

## A funcao `main()`

```cpp
int main()
{
    // ...
    return 0;
}
```

Toda execucao comeca em `main()`. O `return 0` indica sucesso ao sistema operacional. Convencao Unix/Windows: 0 = sucesso, qualquer outro valor = erro.

C++ permite omitir o `return 0` em `main()` (apenas em `main`!): se voce nao escrever, o compilador adiciona `return 0;` automaticamente. Mas e bom estilo escrever explicitamente.

## Declaracao de variaveis

```cpp
int carrots;        // declara uma variavel inteira sem inicializar (lixo na memoria!)
carrots = 25;       // atribui valor

int peas = 7;       // declara e inicializa em uma linha (preferivel)
int peas{7};        // sintaxe de inicializacao uniforme (C++11, recomendado hoje)
```

> **Boas praticas (C++ moderno)**: sempre inicialize ao declarar. Use chaves `{ }` (`int x{0};`), pois elas detectam conversoes que perderiam dados (`int n{3.14}` da erro de compilacao, ja `int n = 3.14` aceita silenciosamente).

## Entrada e saida com `cin` e `cout`

```cpp
// getinfo.cpp -- entrada e saida
#include <iostream>

int main()
{
    using namespace std;
    int carrots;
    cout << "Quantas cenouras voce tem? ";
    cin >> carrots;                // le um inteiro do teclado
    carrots = carrots + 2;
    cout << "Agora voce tem " << carrots << " cenouras." << endl;
    return 0;
}
```

> **Rode no Visual Studio ou compile com `g++ getinfo.cpp -o getinfo`**

- `<<` e o **insertion operator** (operador de insercao): empurra dados pra dentro de um stream de saida.
- `>>` e o **extraction operator** (operador de extracao): puxa dados de um stream de entrada para uma variavel.

Os simbolos foram escolhidos pela direcao visual do fluxo. `cout` e `cin` sao **objetos** das classes `ostream` e `istream` definidas em `<iostream>`.

### Concatenando saidas

```cpp
cout << "Agora voce tem " << carrots << " cenouras." << endl;

// equivalente, dividido em linhas
cout << "Agora voce tem "
     << carrots
     << " cenouras."
     << endl;
```

C++ trata espacos em branco e quebras de linha entre tokens como equivalentes (free format).

### `endl` versus `"\n"`

```cpp
cout << "linha 1" << endl;   // pula linha + da flush no buffer
cout << "linha 2" << "\n";   // so pula linha (sem flush)
```

Use `"\n"` por padrao (mais rapido). Use `endl` so quando voce **realmente** precisa garantir que a saida apareca imediatamente (ex: antes de uma operacao demorada, ou debug).

## Funcoes da biblioteca padrao

Para usar `sqrt`, voce inclui `<cmath>` e chama:

```cpp
// sqrt.cpp -- usando sqrt()
#include <iostream>
#include <cmath>

int main()
{
    using namespace std;
    double area;
    cout << "Digite a area do piso em metros quadrados: ";
    cin >> area;
    double lado = sqrt(area);
    cout << "Equivale a um quadrado de " << lado
         << " metros de lado." << endl;
    return 0;
}
```

> **Rode no Visual Studio ou compile com `g++ sqrt.cpp -o sqrt`**

O **prototipo** da funcao (`double sqrt(double);`) ja vem dentro de `<cmath>`. Sem incluir o header certo, o compilador nao sabe que `sqrt` existe nem como ela se comporta.

## Funcoes definidas pelo usuario

Toda funcao precisa de:

1. **Prototipo** (declaracao): tipo de retorno, nome, tipos dos parametros. Termina com `;`.
2. **Definicao**: cabecalho + corpo.
3. **Chamada**: usar a funcao em algum lugar.

```cpp
// ourfunc.cpp -- definindo a sua propria funcao
#include <iostream>

void simon(int);                       // prototipo

int main()
{
    using namespace std;
    simon(3);                          // chamada com literal
    cout << "Escolha um inteiro: ";
    int count;
    cin >> count;
    simon(count);                      // chamada com variavel
    cout << "Pronto!" << endl;
    return 0;
}

void simon(int n)                      // definicao
{
    using namespace std;
    cout << "Simon diz toque os pes " << n << " vezes." << endl;
}
```

> **Rode no Visual Studio ou compile com `g++ ourfunc.cpp -o ourfunc`**

### Anatomia de uma funcao

```
tipo_retorno  nome  ( lista_de_parametros )
{
    corpo
    return valor;     // se tipo_retorno != void
}
```

- `void simon(int)` -> nao retorna nada, recebe um `int`.
- `int main()` -> retorna `int`, nao recebe argumentos.
- `int rand(void)` -> retorna `int`, nao recebe argumentos. (`void` explicito; em C++ pode-se omitir.)

## Funcao com retorno

```cpp
// convert.cpp -- converte stone (medida UK) para libras
#include <iostream>

int stonetolb(int);                   // prototipo

int main()
{
    using namespace std;
    int stone;
    cout << "Peso em stone: ";
    cin >> stone;
    int pounds = stonetolb(stone);
    cout << stone << " stone = " << pounds << " libras." << endl;
    return 0;
}

int stonetolb(int sts)
{
    return 14 * sts;                  // 1 stone = 14 libras
}
```

> **Rode no Visual Studio ou compile com `g++ convert.cpp -o convert`**

A `return` pode retornar uma expressao inteira, nao precisa ser apenas uma variavel.

## Convencoes de nomes

C++ aceita varios estilos. Os mais comuns:

| Estilo | Exemplo | Onde se ve |
|---|---|---|
| `snake_case` | `stone_to_lb` | C, Python, libs do Boost |
| `camelCase` | `stoneToLb` | Java, JavaScript, parte do C++ |
| `PascalCase` | `StoneToLb` | Tipos/classes em C++, C# |
| `UPPER_SNAKE` | `MAX_VALUE` | Constantes, macros |

Em C++ moderno (Microsoft, Google, etc), uma convencao comum:

- `PascalCase` para classes e structs (`MyClass`)
- `camelCase` ou `snake_case` para funcoes e variaveis (`doSomething` ou `do_something`)
- `UPPER_SNAKE` para macros e enums-de-constantes

Escolha um padrao e seja consistente. O importante e o time/projeto manter o mesmo estilo.

## Em C++ moderno (17/20)

O capitulo 2 do livro (2012) ainda nao mostra:

```cpp
#include <iostream>

int main()
{
    int x{42};                         // C++11: brace-init (sem narrowing)
    auto y = 3.14;                     // C++11: tipo deduzido (= double)
    auto& ref = x;                     // referencia deduzida

    if (auto v = stonetolb(10); v > 100) {   // C++17: init em if
        std::cout << v << "\n";
    }

    std::cout << std::format("x = {}, y = {:.2f}\n", x, y);  // C++20: std::format
    return 0;
}
```

`std::format` (C++20) substitui o velho `printf`/`sprintf` com type safety e sintaxe parecida com Python.

## Resumo

- Programa = uma ou mais **funcoes**, comecando por `main()`.
- `#include` puxa headers; `<iostream>` traz `cin` e `cout`.
- `cout << x` insere `x` na saida; `cin >> x` extrai do teclado para `x`.
- Toda funcao precisa de **prototipo** (declaracao) antes da chamada e de **definicao**.
- C++ e case sensitive e tem free format.
- Use `\n` por padrao para nova linha; `endl` so quando precisa de flush.
- `using namespace std;` so dentro de funcoes pequenas; em headers e codigo serio prefira `std::` explicito.

## Proximo capitulo

[Capitulo 3 - Lidando com Dados (Dealing with Data)](capitulo-03.md): tipos basicos (`int`, `char`, `double`, `bool`), literais, `const`, conversoes e `cast`.
