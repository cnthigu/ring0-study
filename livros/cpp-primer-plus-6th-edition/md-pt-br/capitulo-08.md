# Capitulo 8 - Aventuras com Funcoes (Adventures in Functions)

> Traducao nao oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Texto adaptado para portugues brasileiro com fins educacionais. Paginas originais: 379-446.
> Repositorio: [ring0-study](https://github.com/cnthigu/ring0-study)

## Visao geral do capitulo

- Funcoes `inline`
- **Referencias** em detalhe
- Argumentos default
- Sobrecarga de funcoes (`overloading`)
- Function templates (templates de funcao)
- Especializacao de templates
- `decltype` e trailing return type (C++11)

## Funcoes `inline`

Quando voce chama uma funcao, o programa pula para o codigo dela, executa, e volta. Isso tem custo (push/pop na stack). Para funcoes muito pequenas, esse overhead pode ser maior que o trabalho da funcao.

```cpp
inline double quadrado(double x) {
    return x * x;
}

int main()
{
    double r = quadrado(5.0);   // o compilador PODE substituir por: double r = 5.0 * 5.0
}
```

`inline` e uma **sugestao** ao compilador para inserir o corpo da funcao no lugar da chamada (eliminando a chamada). O compilador moderno **decide sozinho** o que e bom inlinar e geralmente ignora a sugestao explicita.

> **O uso real moderno de `inline`** e diferente: ele permite **definir a mesma funcao em multiplos arquivos** (incluindo headers) sem violar a One Definition Rule (ODR). Por isso voce ve `inline` em metodos definidos no `.h`.

```cpp
// utils.h - inline permite definicao no header
inline int dobrar(int x) { return 2 * x; }
```

C++17 ainda adicionou `inline` para variaveis em headers:

```cpp
// constantes.h
inline constexpr double PI = 3.14159265358979;
```

## Referencias - revisao detalhada

Uma referencia e um **alias** (apelido) para outra variavel:

```cpp
int n = 10;
int& ref = n;       // ref e outro nome pra n
ref = 20;           // mudou n tambem
std::cout << n;     // 20
```

Regras importantes de referencias:

1. **Precisa ser inicializada** ao ser criada
2. **Nao pode ser mudada** pra apontar pra outra variavel
3. **Nao existe "referencia nula"** (use ponteiro pra isso)

```cpp
int& r;          // ERRO - sem inicializacao
int& r = 5;      // ERRO - referencia para temporario nao-const
const int& r = 5;// OK - const& aceita temporario
```

### Por que usar `const T&` em parametros?

```cpp
void imprimir(std::string s);          // copia (caro!)
void imprimir(const std::string& s);   // referencia, sem copia, nao pode modificar
```

Para `std::string`, `std::vector` e qualquer tipo grande, **sempre** prefira `const T&` em parametros de leitura.

### Referencias retornadas

```cpp
int& maior(int& a, int& b) {
    return (a > b) ? a : b;
}

int x = 5, y = 10;
maior(x, y) = 100;     // modifica y!
```

> **Cuidado**: nunca retorne referencia para variavel local!
>
> ```cpp
> int& bug() {
>     int x = 42;
>     return x;       // BUG: x morre quando a funcao termina
> }
> ```

### `&&` - referencia rvalue (C++11)

```cpp
void f(int& x);          // aceita lvalue (variaveis nomeadas)
void f(const int& x);    // aceita lvalue e rvalue
void f(int&& x);         // aceita rvalue (temporarios)
```

`&&` e a base de **move semantics** (semantica de movimento), que evita copias caras quando voce sabe que o objeto vai ser descartado.

```cpp
std::string a = "ola";
std::string b = std::move(a);   // move (rouba) o conteudo de a; a fica vazia
```

Veja capitulo 18 para detalhes.

## Argumentos default

```cpp
void cumprimentar(const std::string& nome = "Desconhecido", int idade = 0)
{
    std::cout << "Ola, " << nome;
    if (idade > 0) std::cout << " (" << idade << " anos)";
    std::cout << "\n";
}

cumprimentar();              // Ola, Desconhecido
cumprimentar("Carniato");    // Ola, Carniato
cumprimentar("Carniato", 24); // Ola, Carniato (24 anos)
```

Regras:

- Defaults vao **da direita pra esquerda**: se um parametro tem default, todos a direita tambem precisam ter
- Defaults vao no **prototipo** (header), nao na definicao

```cpp
void f(int a, int b = 5, int c = 10);   // OK
void g(int a = 1, int b, int c);         // ERRO
```

## Sobrecarga (overloading)

Voce pode ter **varias funcoes com o mesmo nome**, desde que difiram em **tipo ou numero de parametros**:

```cpp
int    abs(int x)            { return x < 0 ? -x : x; }
double abs(double x)         { return x < 0 ? -x : x; }
long   abs(long x)           { return x < 0 ? -x : x; }

abs(5);       // chama int
abs(5.0);     // chama double
abs(5L);      // chama long
```

O compilador escolhe a versao certa pelo tipo dos argumentos (resolucao de sobrecarga).

> **Atencao**: o tipo de **retorno NAO conta** para sobrecarga. Estes sao ambiguos:
>
> ```cpp
> int    f(int x);
> double f(int x);   // ERRO - mesma assinatura
> ```

### Sobrecarga vs default arguments

```cpp
// Versao 1 - sobrecarga
void log(const std::string& msg);
void log(const std::string& msg, int nivel);

// Versao 2 - default
void log(const std::string& msg, int nivel = 0);
```

Use **default** quando os comportamentos sao essencialmente iguais. Use **sobrecarga** quando as versoes fazem coisas diferentes.

## Function templates

Templates permitem escrever **uma versao generica** de uma funcao que funciona para varios tipos:

```cpp
template <typename T>
T maior(T a, T b) {
    return (a > b) ? a : b;
}

int main()
{
    std::cout << maior(3, 5)         << "\n";   // T = int
    std::cout << maior(2.5, 1.7)     << "\n";   // T = double
    std::cout << maior<long>(100, 5) << "\n";   // forca T = long

    std::string a = "alfa", b = "beta";
    std::cout << maior(a, b);                   // T = std::string
}
```

> **Rode no Visual Studio ou compile com `g++ -std=c++17 template.cpp -o template`**

`template <typename T>` (ou `template <class T>` - mesma coisa) declara um parametro de tipo `T`.

### Template com multiplos parametros

```cpp
template <typename T, typename U>
auto somar(T a, U b) -> decltype(a + b) {       // C++11: trailing return type
    return a + b;
}

somar(1, 2.5);    // T=int, U=double, retorno = double
```

### Em C++14/17/20

```cpp
template <typename T, typename U>
auto somar(T a, U b) {           // C++14: deduz retorno automaticamente
    return a + b;
}

auto somar20(auto a, auto b) {   // C++20: abreviado, sem template explicito
    return a + b;
}
```

### Especializacao de template

Voce pode dar uma implementacao especifica para um tipo particular:

```cpp
template <typename T>
T maior(T a, T b) { return (a > b) ? a : b; }

// Especializacao para const char* (compara strings, nao ponteiros!)
template <>
const char* maior<const char*>(const char* a, const char* b) {
    return std::strcmp(a, b) > 0 ? a : b;
}
```

## `decltype` (C++11)

`decltype(expressao)` da o **tipo** da expressao em tempo de compilacao:

```cpp
int x = 5;
decltype(x) y = 10;          // y e int

const int& r = x;
decltype(r) z = x;           // z e const int& (mantem as qualificacoes)

std::vector<int> v;
decltype(v.begin()) it;      // it e std::vector<int>::iterator
```

Util para escrever templates onde o tipo certo depende dos parametros.

## Trailing return type

```cpp
auto somar(int a, int b) -> int {
    return a + b;
}
```

Equivalente a `int somar(int a, int b)`. Util quando o tipo de retorno depende dos parametros:

```cpp
template <typename T, typename U>
auto multiplicar(T a, U b) -> decltype(a * b) {
    return a * b;
}
```

## `auto` em parametros (C++14 - lambdas; C++20 - funcoes)

```cpp
auto soma = [](auto a, auto b) { return a + b; };  // lambda generica (C++14)

soma(1, 2);            // int + int
soma(1.5, 2.5);        // double + double
soma(std::string{"a"}, std::string{"b"});  // string + string!

// C++20 - funcoes normais aceitam auto
auto multiplicar(auto a, auto b) {
    return a * b;
}
```

## Templates variadicos (C++11)

```cpp
template <typename T>
void imprimir(T valor) {
    std::cout << valor << "\n";
}

template <typename T, typename... Resto>
void imprimir(T primeiro, Resto... resto) {
    std::cout << primeiro << " ";
    imprimir(resto...);          // chamada recursiva
}

int main()
{
    imprimir(1, "ola", 3.14, 'X', std::string{"final"});
}
```

`Resto...` (com ticks) e o **parameter pack**: aceita zero ou mais parametros de tipos diferentes. Foi a base pro `printf` type-safe e pro `std::format`.

### Em C++17: fold expression

```cpp
template <typename... Args>
void imprimir(Args... args) {
    ((std::cout << args << " "), ...);  // fold expression - bem mais simples!
}
```

## Resumo

- `inline` hoje serve mais pra ODR em headers do que pra performance
- Use referencias `T&`/`const T&`/`T&&` em vez de ponteiros quando possivel
- Argumentos default vao da direita pra esquerda no prototipo
- Sobrecarga: mesmo nome, parametros diferentes
- **Templates** sao um dos pilares de C++: codigo generico, type-safe, sem overhead
- C++14+ deixou templates muito mais simples (`auto` no retorno e em parametros)
- Templates variadicos + fold expressions resolvem casos antes impossiveis

## Proximo capitulo

[Capitulo 9 - Memory Models e Namespaces](capitulo-09.md): compilacao separada, escopo, classes de armazenamento (`auto`, `static`, `extern`, `thread_local`), `const` em detalhe, e namespaces.
