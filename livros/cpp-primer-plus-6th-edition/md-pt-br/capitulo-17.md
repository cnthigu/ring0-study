# Capitulo 17 - Input, Output e Arquivos (Input, Output, and Files)

> Traducao nao oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Texto adaptado para portugues brasileiro com fins educacionais. Paginas originais: 1061-1152.
> Repositorio: [ring0-study](https://github.com/cnthigu/ring0-study)

## Visao geral do capitulo

- Streams: o que sao e como funcionam
- `cout`, `cin`, `cerr`, `clog`
- Formatacao de saida (manipuladores, flags)
- Leitura de entrada e suas armadilhas
- Arquivos texto: `ifstream`, `ofstream`, `fstream`
- Arquivos binarios: `read`/`write`, `seekg`/`tellg`
- Streams em memoria: `stringstream`

## Streams

Em C++, **input/output** e feito atraves de **streams** - sequencias de bytes que fluem entre o programa e algum dispositivo (terminal, arquivo, memoria, rede).

| Stream | Direcao | Ligado a |
|---|---|---|
| `std::cout` | saida | stdout (terminal) |
| `std::cin`  | entrada | stdin (teclado) |
| `std::cerr` | saida (erros, sem buffer) | stderr |
| `std::clog` | saida (logs, com buffer) | stderr |

Hierarquia simplificada:

```
ios_base
   |
  ios
  /  \
istream  ostream
  |        |    \
  |     ofstream stringstream...
  |
ifstream
```

`iostream` e a fusao de `istream` + `ostream`.

## Saida formatada com `cout`

```cpp
#include <iostream>
#include <iomanip>

int main() {
    std::cout << "texto" << "\n";
    std::cout << 42 << " " << 3.14 << "\n";

    std::cout << std::hex << 255 << "\n";       // ff
    std::cout << std::oct << 255 << "\n";       // 377
    std::cout << std::dec << 255 << "\n";       // 255

    std::cout << std::showbase << std::hex << 255 << "\n";  // 0xff
    std::cout << std::uppercase << 255 << "\n"; // 0XFF

    std::cout << std::fixed << std::setprecision(2) << 3.14159 << "\n";  // 3.14
    std::cout << std::scientific << 0.000123 << "\n";

    std::cout << std::setw(10) << "ola" << "|\n";              // padding
    std::cout << std::setw(10) << std::left << "ola" << "|\n"; // alinhado a esquerda
    std::cout << std::setfill('.') << std::setw(10) << 42 << "\n"; // ........42

    std::cout << std::boolalpha << true << "\n";   // true
    std::cout << std::noboolalpha << true << "\n"; // 1
}
```

### Em C++20: `std::format`

`<iomanip>` e poderoso mas verboso. C++20 trouxe `std::format`, baseado no formato Python:

```cpp
#include <format>
#include <iostream>

std::cout << std::format("nome={}, idade={}\n", "cnt", 24);
std::cout << std::format("hex: {:x}, octal: {:o}\n", 255, 255);
std::cout << std::format("{:>10}|\n", "ola");           // direita
std::cout << std::format("{:.<10}\n", 42);              // padding com ponto
std::cout << std::format("{:.2f}\n", 3.14159);
std::cout << std::format("{:08.3f}\n", 3.14);           // 0003.140
```

C++23: `std::print` (parecido com Python):

```cpp
#include <print>
std::print("nome={}, idade={}\n", "cnt", 24);
std::println("nome={}, idade={}", "cnt", 24);    // adiciona \n
```

## Entrada com `cin`

### Operador `>>`

Pula whitespace, le ate o proximo whitespace:

```cpp
int n;
std::cin >> n;          // le um inteiro

double d;
std::string palavra;
std::cin >> d >> palavra;
```

### Lendo linha inteira

```cpp
std::string linha;
std::getline(std::cin, linha);                // ate '\n'
std::getline(std::cin, linha, '|');           // ate '|'
```

### Misturando `>>` e `getline`: armadilha classica

```cpp
int n;
std::string nome;

std::cin >> n;                       // le 42, deixa '\n' no buffer
std::getline(std::cin, nome);        // le "" imediatamente!
```

**Solucao**: descartar o `\n` antes do `getline`:

```cpp
#include <limits>

std::cin >> n;
std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
std::getline(std::cin, nome);
```

### Estados do stream

```cpp
std::cin.good();    // tudo ok
std::cin.eof();     // chegou ao fim
std::cin.fail();    // erro de formato (ex: digitou letra esperando int)
std::cin.bad();     // erro grave de I/O

if (!(std::cin >> n)) {
    std::cin.clear();           // limpa flags de erro
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << "Entrada invalida!\n";
}
```

### Padrao de loop de leitura

```cpp
int x;
while (std::cin >> x) {
    std::cout << "li: " << x << "\n";
}
// loop termina em EOF ou erro de formato
```

## Arquivos

`<fstream>` traz tres classes:

- `std::ifstream` (input file stream) - leitura
- `std::ofstream` (output file stream) - escrita
- `std::fstream` (file stream) - leitura e escrita

### Escrita

```cpp
#include <fstream>

std::ofstream out("dados.txt");         // abre (cria se nao existir, trunca se existir)
if (!out) {
    std::cerr << "Erro ao abrir!\n";
    return 1;
}
out << "linha 1\n";
out << 42 << " " << 3.14 << "\n";
// destrutor fecha automaticamente (RAII)
```

### Leitura

```cpp
std::ifstream in("dados.txt");
if (!in) { /* erro */ }

std::string linha;
while (std::getline(in, linha)) {
    std::cout << linha << "\n";
}

// ou ler valores estruturados
int n; double d;
while (in >> n >> d) {
    std::cout << n << " " << d << "\n";
}
```

### Modos de abertura

```cpp
#include <ios>

std::ofstream out("log.txt", std::ios::app);          // append
std::ofstream out2("data.bin", std::ios::binary);     // binario
std::fstream  fs("rw.txt", std::ios::in | std::ios::out);

// Combinacoes uteis:
// std::ios::in     - leitura
// std::ios::out    - escrita
// std::ios::app    - append (escreve sempre no fim)
// std::ios::ate    - posiciona no fim ao abrir (mas pode escrever em qualquer lugar)
// std::ios::trunc  - apaga conteudo ao abrir (padrao para ofstream)
// std::ios::binary - modo binario (sem traducao de \r\n no Windows)
```

## Arquivos binarios

```cpp
struct Pessoa {
    char nome[32];
    int  idade;
    double salario;
};

// Escrita binaria
Pessoa p = {"Carniato", 24, 5000.0};
std::ofstream out("pessoas.bin", std::ios::binary);
out.write(reinterpret_cast<const char*>(&p), sizeof(p));

// Leitura binaria
Pessoa p2;
std::ifstream in("pessoas.bin", std::ios::binary);
in.read(reinterpret_cast<char*>(&p2), sizeof(p2));
std::cout << p2.nome << " " << p2.idade << " " << p2.salario << "\n";
```

> **Cuidado**: escrever structs cruas funciona mas nao e portavel. Endianness, padding e alinhamento variam entre maquinas. Para formatos de verdade, use **serializacao explicita** (JSON, Protobuf, MessagePack, etc.).

## `seekg`/`tellg`/`seekp`/`tellp`

Para arquivos binarios e streams seekable, voce pode posicionar o "ponteiro" de leitura/escrita:

```cpp
std::ifstream in("dados.bin", std::ios::binary);

in.seekg(0, std::ios::end);          // vai pro fim
auto tamanho = in.tellg();           // posicao atual = tamanho do arquivo
in.seekg(0, std::ios::beg);          // volta pro inicio

in.seekg(100);                       // pula para o byte 100
in.seekg(50, std::ios::cur);         // avanca 50 bytes da posicao atual
```

`seekg`/`tellg` para input, `seekp`/`tellp` para output.

## Streams em memoria: `stringstream`

`<sstream>` traz streams que leem/escrevem em uma `std::string`:

```cpp
#include <sstream>

// Construindo strings com formatacao
std::ostringstream oss;
oss << "x=" << 42 << ", y=" << 3.14;
std::string resultado = oss.str();

// Parseando uma string
std::istringstream iss{"42 3.14 ola"};
int n; double d; std::string s;
iss >> n >> d >> s;

// Bidirecional
std::stringstream ss;
ss << "100 200";
int a, b;
ss >> a >> b;
```

Util para:
- Construir strings formatadas (antes de C++20)
- Tokenizar strings facilmente
- Testar codigo que normalmente leria de stdin/arquivo

## Em C++17: `std::filesystem`

Manipular caminhos, criar diretorios, listar conteudo, copiar/mover/remover arquivos:

```cpp
#include <filesystem>
namespace fs = std::filesystem;

fs::path p = "C:/Users/cnt/projeto";

if (fs::exists(p) && fs::is_directory(p)) {
    for (const auto& entry : fs::directory_iterator{p}) {
        std::cout << entry.path() << "\n";
        if (fs::is_regular_file(entry)) {
            std::cout << "  " << fs::file_size(entry) << " bytes\n";
        }
    }
}

fs::create_directories("nova/sub/pasta");
fs::copy("origem.txt", "destino.txt");
fs::rename("velho.txt", "novo.txt");
fs::remove("temp.txt");
fs::remove_all("pasta_inteira");

// Recursivo
for (const auto& entry : fs::recursive_directory_iterator{p}) {
    std::cout << entry.path() << "\n";
}

// Composicao de paths (cross-platform!)
fs::path arquivo = p / "subpasta" / "arquivo.txt";
auto ext = arquivo.extension();        // ".txt"
auto nome = arquivo.stem();             // "arquivo"
auto pai = arquivo.parent_path();
```

## Exemplo completo: contar palavras de um arquivo

```cpp
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <filesystem>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " <arquivo>\n";
        return 1;
    }

    std::filesystem::path caminho{argv[1]};
    if (!std::filesystem::exists(caminho)) {
        std::cerr << "Arquivo nao encontrado: " << caminho << "\n";
        return 1;
    }

    std::ifstream in{caminho};
    std::map<std::string, int> contagem;
    std::string palavra;

    while (in >> palavra) {
        ++contagem[palavra];
    }

    std::cout << "Total de palavras unicas: " << contagem.size() << "\n";
    for (const auto& [p, n] : contagem) {
        std::cout << p << ": " << n << "\n";
    }
}
```

## Em C++23: `std::print`/`std::println`

Substituem boa parte do uso de `cout` no dia a dia:

```cpp
#include <print>

std::println("Ola, {}!", "mundo");
std::println("x={}, y={:.2f}", 42, 3.14159);
std::print("sem newline");
```

Vantagens:
- Sintaxe mais curta
- Type-safe (erro de compilacao se voce passar tipo errado)
- Mais rapido que `cout` em varios cenarios
- Suporte a Unicode mais consistente

## Resumo

- Streams sao a abstracao de I/O em C++
- `cout`/`cin` para terminal, `ifstream`/`ofstream` para arquivos
- `<iomanip>` para formatacao tradicional, `std::format`/`std::print` (C++20/23) para o moderno
- Sempre cheque o estado do stream antes de usar o valor lido
- Modo binario (`std::ios::binary`) e essencial no Windows para arquivos nao-texto
- `seekg`/`seekp` para acesso aleatorio
- `stringstream` para I/O em memoria
- `std::filesystem` (C++17) para manipular arquivos e pastas de forma portavel

## Proximo capitulo

[Capitulo 18 - Visitando o Novo Padrao C++](capitulo-18.md): visao geral das features mais importantes de C++11 (com mencoes ao que veio em 14/17/20/23).
