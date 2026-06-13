# Capítulo 16 — A Classe string e a Standard Template Library

> Tradução não oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> [Anterior](capitulo-15-03-rtti-casts-resumo.md) | [Índice](README.md) | [Próximo](capitulo-16-02-stl-iteradores.md)

Neste capítulo você aprenderá sobre:

- A classe `string` padrão do C++
- Os templates `auto_ptr`, `unique_ptr` e `shared_ptr`
- A Standard Template Library (STL)
- Classes de container
- Iteradores
- Objetos função (functors)
- Algoritmos da STL
- O template `initializer_list`

A essa altura você já está familiarizado com o objetivo de reusabilidade de código do C++. Um dos grandes frutos desse objetivo é quando você pode reutilizar código escrito por outras pessoas — e é aí que as bibliotecas de classes entram. Existem muitas bibliotecas de classes C++ disponíveis comercialmente, e há também as que vêm como parte do pacote C++. Por exemplo, você já usou as classes de entrada e saída suportadas pelo arquivo de cabeçalho `ostream`. Este capítulo examina outros códigos reutilizáveis disponíveis para seu uso.

Você já conheceu a classe `string`, e este capítulo a examina com mais profundidade. Em seguida, o capítulo apresenta os templates de "ponteiros inteligentes" (smart pointers), que facilitam o gerenciamento de memória dinâmica. Depois, o capítulo aborda a Standard Template Library (STL), uma coleção de templates úteis para lidar com vários tipos de objetos container. A STL exemplifica o paradigma de programação chamado *programação genérica* (generic programming). Por fim, o capítulo examina o template `initializer_list`, a adição do C++11 que permite usar a sintaxe de lista inicializadora com objetos da STL.

---

## A Classe string

Muitas aplicações precisam processar cadeias de texto. O C fornece algum suporte por meio da família de funções de string do `string.h` (`cstring` em C++), e muitas implementações anteriores de C++ forneciam classes caseiras para lidar com strings. O Capítulo 4 introduziu a classe `string` do ANSI/ISO C++. O Capítulo 12, com sua modesta classe `String`, ilustrou alguns aspectos do projeto de uma classe para representar strings.

Lembre-se de que a classe `string` é suportada pelo arquivo de cabeçalho `string`. (Os arquivos `string.h` e `cstring` suportam as funções de string da biblioteca C para strings no estilo C, não a classe `string`.) A chave para usar uma classe é conhecer sua interface pública, e a classe `string` tem um extenso conjunto de métodos, incluindo vários construtores, operadores sobrecarregados para atribuição de strings, concatenação, comparação e acesso a elementos individuais, além de utilitários para localizar caracteres e substrings em uma string, e mais. Em resumo, a classe `string` tem muito a oferecer.

### Construindo uma String

Vamos examinar os construtores da classe `string`. Afinal, uma das coisas mais importantes a saber sobre uma classe é quais são as opções ao criar objetos dela. A Listagem 16.1 usa sete dos construtores de `string` (rotulados como `ctor`, a abreviação tradicional do C++ para construtor). A Tabela 16.1 descreve brevemente os construtores. A tabela começa com os sete construtores usados na Listagem 16.1, nessa ordem. Ela também lista algumas adições do C++11. As representações dos construtores são simplificadas — ocultam o fato de que `string` é na verdade um `typedef` para a especialização de template `basic_string<char>` — e omitem um argumento opcional relacionado ao gerenciamento de memória. O tipo `size_type` é um tipo inteiro dependente da implementação definido no arquivo de cabeçalho `string`. A classe define `string::npos` como o comprimento máximo possível da string. Tipicamente, isso equivale ao valor máximo de `unsigned int`. A tabela também usa a abreviação NBTS (null-byte-terminated string) para string terminada em byte nulo — ou seja, a string tradicional do C, terminada com um caractere nulo.

**Tabela 16.1 — Construtores da Classe `string`**

| Construtor | Descrição |
|---|---|
| `string(const char *s)` | Inicializa um objeto `string` com a NBTS apontada por `s`. |
| `string(size_type n, char c)` | Cria um objeto `string` de `n` elementos, cada um inicializado com o caractere `c`. |
| `string(const string &str)` | Inicializa um objeto `string` com o objeto `string` `str` (construtor de cópia). |
| `string()` | Cria um objeto `string` padrão de tamanho 0 (construtor padrão). |
| `string(const char *s, size_type n)` | Inicializa com a NBTS apontada por `s`, usando `n` caracteres, mesmo que isso ultrapasse o tamanho da NBTS. |
| `template<class Iter> string(Iter begin, Iter end)` | Inicializa com os valores no intervalo `[begin, end)`, onde `begin` e `end` atuam como ponteiros e especificam localizações; o intervalo inclui `begin` e vai até, mas não incluindo, `end`. |
| `string(const string &str, size_type pos, size_type n = npos)` | Inicializa com o objeto `str`, começando na posição `pos` de `str` e indo até o final de `str` ou usando `n` caracteres, o que vier primeiro. |
| `string(string &&str) noexcept` (C++11) | Inicializa com o objeto `str`; `str` pode ser alterado (construtor de movimento). |
| `string(initializer_list<char> il)` (C++11) | Inicializa com os caracteres na lista inicializadora `il`. |

**Listagem 16.1 — str1.cpp**

```cpp
// str1.cpp -- apresentando a classe string
#include <iostream>
#include <string>
// usando construtores de string
int main()
{
    using namespace std;
    string um("Ganhador da Loteria!"); // ctor #1
    cout << um << endl;               // << sobrecarregado
    string dois(20, '$');             // ctor #2
    cout << dois << endl;
    string tres(um);                  // ctor #3
    cout << tres << endl;
    um += " Ops!";                    // += sobrecarregado
    cout << um << endl;
    dois = "Desculpe! Aquilo foi ";
    tres[0] = 'P';
    string quatro;                    // ctor #4
    quatro = dois + tres;             // + e = sobrecarregados
    cout << quatro << endl;
    char todos[] = "Tudo bem que termina bem";
    string cinco(todos, 20);          // ctor #5
    cout << cinco << "!\n";
    string seis(todos + 5, todos + 9); // ctor #6
    cout << seis << ", ";
    string sete(&cinco[5], &cinco[9]); // ctor #6 novamente
    cout << sete << "...\n";
    string oito(quatro, 7, 16);       // ctor #7
    cout << oito << " em movimento!" << endl;
    return 0;
}
```

O programa da Listagem 16.1 também usa o operador `+=` sobrecarregado, que acrescenta uma string a outra; o operador `=` sobrecarregado para atribuir uma string a outra; o operador `<<` sobrecarregado para exibir um objeto `string`; e o operador `[]` sobrecarregado para acessar um caractere individual de uma string.

Aqui está a saída do programa da Listagem 16.1:

```
Lottery Winner!
$$$$$$$$$$$$$$$$$$$$
Lottery Winner!
Lottery Winner! Oops!
Sorry! That was Pottery Winner!
All's well that ends!
well, well...
That was Pottery in motion!
```

**Notas do Programa**

O início do programa da Listagem 16.1 ilustra que é possível inicializar um objeto `string` com uma string C-style comum e exibi-lo usando o operador `<<` sobrecarregado:

```cpp
string um("Ganhador da Loteria!"); // ctor #1
cout << um << endl;                // << sobrecarregado
```

O próximo construtor inicializa o objeto `string` `dois` com uma string de 20 caracteres `$`:

```cpp
string dois(20, '$'); // ctor #2
```

O construtor de cópia inicializa o objeto `string` `tres` com o objeto `string` `um`:

```cpp
string tres(um); // ctor #3
```

O operador `+=` sobrecarregado acrescenta a string `" Ops!"` à string `um`:

```cpp
um += " Ops!"; // += sobrecarregado
```

Esse exemplo específico acrescenta uma string C-style a um objeto `string`. Entretanto, o operador `+=` é sobrecarregado de múltiplas formas, de modo que também é possível acrescentar objetos `string` e caracteres individuais:

```cpp
um += dois; // acrescentar um objeto string (não está no programa)
um += '!';  // acrescentar um valor do tipo char (não está no programa)
```

De forma análoga, o operador `=` é sobrecarregado de modo que é possível atribuir um objeto `string` a um objeto `string`, uma string C-style a um objeto `string`, ou um valor `char` simples a um objeto `string`:

```cpp
dois = "Desculpe! Aquilo foi "; // atribuir uma string C-style
dois = um;                       // atribuir um objeto string (não está no programa)
dois = '?';                      // atribuir um valor char (não está no programa)
```

Sobrecarregar o operador `[]`, como o exemplo da classe `String` do Capítulo 12 fez, permite acesso a caracteres individuais de um objeto `string` usando a notação de array:

```cpp
tres[0] = 'P';
```

Um construtor padrão cria uma string vazia que pode receber um valor depois:

```cpp
string quatro;                // ctor #4
quatro = dois + tres;         // + e = sobrecarregados
```

A segunda linha aqui usa o operador `+` sobrecarregado para criar um objeto `string` temporário, que depois é atribuído, usando o operador `=` sobrecarregado, ao objeto `quatro`. Como seria de esperar, o operador `+` concatena seus dois operandos em um único objeto `string`. O operador é sobrecarregado de múltiplas formas, de modo que o segundo operando pode ser um objeto `string`, uma string C-style ou um valor `char`.

O quinto construtor recebe uma string C-style e um inteiro como argumentos, com o inteiro indicando quantos caracteres copiar:

```cpp
char todos[] = "Tudo bem que termina bem";
string cinco(todos, 20); // ctor #5
```

O sexto construtor tem um argumento template:

```cpp
template<class Iter> string(Iter begin, Iter end);
```

A intenção é que `begin` e `end` atuem como ponteiros apontando para dois locais na memória. (Em geral, `begin` e `end` podem ser iteradores, generalizações de ponteiros amplamente usadas na STL.) O construtor então usa os valores entre os locais apontados por `begin` e `end` para inicializar o objeto `string` que constrói. A notação `[begin, end)`, emprestada da matemática, significa que o intervalo inclui `begin` mas não inclui `end`. Por exemplo:

```cpp
string seis(todos + 5, todos + 9); // ctor #6
```

Como o nome de um array é um ponteiro, tanto `todos + 5` como `todos + 9` são do tipo `char *`, de modo que o template é usado com `Iter` substituído pelo tipo `char *`. O primeiro argumento aponta para o primeiro caractere e o segundo aponta para a posição logo após o quarto. Assim, `seis` é inicializado com a substring de quatro caracteres correspondente.

Agora suponha que você queira usar esse construtor para inicializar um objeto com parte de outro objeto `string` — digamos, o objeto `cinco`. O seguinte não funciona:

```cpp
string sete(cinco + 5, cinco + 9);
```

O motivo é que o nome de um objeto, diferentemente do nome de um array, não é tratado como o endereço do objeto; portanto, `cinco` não é um ponteiro e `cinco + 5` não tem sentido. Porém, `cinco[5]` é um valor `char`, então `&cinco[5]` é um endereço e pode ser usado como argumento do construtor:

```cpp
string sete(&cinco[5], &cinco[9]); // ctor #6 novamente
```

O sétimo construtor copia uma parte de um objeto `string` para o objeto construído:

```cpp
string oito(quatro, 7, 16); // ctor #7
```

Essa declaração copia 16 caracteres de `quatro` para `oito`, começando na posição 7 (o oitavo caractere) em `quatro`.

**Construtores do C++11**

O construtor `string(string &&str) noexcept` é semelhante ao construtor de cópia no sentido de que a nova `string` é uma cópia de `str`. Porém, diferentemente do construtor de cópia, ele não garante que `str` será tratada como `const`. Essa forma de construtor é chamada de *construtor de movimento* (move constructor). O compilador pode usá-la em algumas situações em vez do construtor de cópia para otimizar o desempenho. O Capítulo 18 discute esse tópico na seção "Semântica de Movimento e a Referência rvalue".

O construtor `string(initializer_list<char> il)` permite a inicialização por lista para a classe `string`. Ou seja, ele torna possíveis declarações como:

```cpp
string pianista = {'L', 'i', 's', 'z', 't'};
string linguagem {'L', 'i', 's', 'p'};
```

Isso pode não ser tão útil para a classe `string`, pois usar strings C-style é mais fácil, mas satisfaz a intenção de tornar a sintaxe de inicialização por lista universal. Este capítulo discutirá o template `initializer_list` mais adiante.

### Entrada para a Classe string

Outra coisa útil a saber sobre uma classe são as opções de entrada disponíveis. Para strings C-style, lembre-se, você tem três opções:

```cpp
char info[100];
cin >> info;            // ler uma palavra
cin.getline(info, 100); // ler uma linha, descartar \n
cin.get(info, 100);     // ler uma linha, deixar \n na fila
```

Para objetos `string`, lembre-se, você tem duas opções:

```cpp
string entrada;
cin >> entrada;          // ler uma palavra
getline(cin, entrada);   // ler uma linha, descartar \n
```

Ambas as versões de `getline()` aceitam um argumento opcional que especifica qual caractere usar como delimitador de entrada:

```cpp
cin.getline(info, 100, ':'); // ler até :, descartar :
getline(entrada, ':');        // ler até :, descartar :
```

A principal diferença operacional é que as versões para `string` ajustam automaticamente o tamanho do objeto `string` alvo para conter os caracteres de entrada:

```cpp
char nome_arq[10];
string sobrenome;
cin >> nome_arq;          // pode ser um problema se o tamanho da entrada > 9 caracteres
cin >> sobrenome;         // pode ler uma palavra muito longa
cin.getline(nome_arq, 10); // pode truncar a entrada
getline(cin, sobrenome);   // sem truncamento
```

A funcionalidade de ajuste automático de tamanho permite que a versão de `getline()` para `string` dispense o parâmetro numérico que limita o número de caracteres de entrada a serem lidos.

Uma diferença de projeto é que as funções de entrada para strings C-style são métodos da classe `istream`, enquanto as versões para `string` são funções autônomas. É por isso que `cin` é um objeto invocador para entrada de strings C-style e um argumento de função para entrada de objetos `string`. Isso vale também para a forma `>>`, o que fica evidente se o código for escrito em forma de função:

```cpp
cin.operator>>(nome_arq);  // método da classe ostream
operator>>(cin, sobrenome); // função normal
```

Vamos examinar as funções de entrada para `string` um pouco mais de perto. Ambas, como mencionado, ajustam o tamanho da string alvo para caber a entrada. Há limites. O primeiro fator limitante é o tamanho máximo permitido para uma string, representado pela constante `string::npos`. Normalmente esse é o valor máximo de `unsigned int`, portanto não representa um limite prático para entrada interativa comum. Entretanto, pode ser um fator se você tentar ler o conteúdo de um arquivo inteiro em um único objeto `string`. O segundo fator limitante é a quantidade de memória disponível para o programa.

A função `getline()` para a classe `string` lê caracteres da entrada e os armazena em um objeto `string` até que uma das três coisas ocorra:

- O fim do arquivo é encontrado, caso em que o `eofbit` do fluxo de entrada é definido, o que implica que os métodos `fail()` e `eof()` retornarão `true`.
- O caractere delimitador (`\n`, por padrão) é alcançado, caso em que ele é removido do fluxo de entrada mas não é armazenado.
- O número máximo possível de caracteres é lido, caso em que o `failbit` do fluxo de entrada é definido, o que implica que o método `fail()` retornará `true`.

A função `operator>>()` para a classe `string` comporta-se de forma semelhante, exceto que, em vez de ler até um caractere delimitador e descartá-lo, ela lê até um caractere de espaço em branco e o deixa na fila de entrada. Um caractere de espaço em branco é um espaço, nova linha ou tabulação — ou mais geralmente, qualquer caractere para o qual `isspace()` retorna verdadeiro.

Como as funções de entrada para objetos `string` trabalham com fluxos e reconhecem o fim do arquivo, você também pode usá-las para entrada de arquivo. A Listagem 16.2 mostra um exemplo curto que lê strings de um arquivo. Ele supõe que o arquivo contém strings separadas pelo caractere dois-pontos e usa o método `getline()` especificando um delimitador. Em seguida, numera e exibe as strings, uma por linha de saída.

**Listagem 16.2 — strfile.cpp**

```cpp
// strfile.cpp -- ler strings de um arquivo
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
int main()
{
    using namespace std;
    ifstream fin;
    fin.open("comprar.txt");
    if (fin.is_open() == false)
    {
        cerr << "Não foi possível abrir o arquivo. Encerrando.\n";
        exit(EXIT_FAILURE);
    }
    string item;
    int contagem = 0;
    getline(fin, item, ':');
    while (fin) // enquanto a entrada for válida
    {
        ++contagem;
        cout << contagem << ": " << item << endl;
        getline(fin, item, ':');
    }
    cout << "Concluído\n";
    fin.close();
    return 0;
}
```

Aqui está um arquivo `comprar.txt` de exemplo:

```
sardinha:sorvete de chocolate:pipoca:alho-poró:
queijo cottage:azeite de oliva:manteiga:tofu:
```

Observe que com `:` especificado como caractere delimitador, o caractere de nova linha torna-se apenas um caractere comum. Assim, o caractere de nova linha ao final da primeira linha do arquivo torna-se o primeiro caractere da string que continua como "queijo cottage". De forma semelhante, o caractere de nova linha ao final da segunda linha de entrada, se presente, torna-se o único conteúdo da nona string de entrada.

### Trabalhando com Strings

Até agora, você aprendeu que é possível criar objetos `string` de diversas formas, exibir o conteúdo de um objeto `string`, ler dados para um objeto `string`, acrescentar a um objeto `string`, atribuir a um objeto `string` e concatenar dois objetos `string`. O que mais se pode fazer?

Você pode comparar strings. Todos os seis operadores relacionais são sobrecarregados para objetos `string`, sendo um objeto considerado menor do que outro se ele ocorrer antes na sequência de agrupamento da máquina. Se a sequência de agrupamento for o código ASCII, isso implica que os dígitos são menores do que os caracteres maiúsculos e que os maiúsculos são menores do que os minúsculos. Cada operador relacional é sobrecarregado de três formas para que você possa comparar um objeto `string` com outro objeto `string`, um objeto `string` com uma string C-style e uma string C-style com um objeto `string`:

```cpp
string cobra1("cobra");
string cobra2("coral");
char cobra3[20] = "anaconda";
if (cobra1 < cobra2)  // operator<(const string &, const string &)
    ...
if (cobra1 == cobra3) // operator==(const string &, const char *)
    ...
if (cobra3 != cobra2) // operator!=(const char *, const string &)
    ...
```

Você pode determinar o tamanho de uma string. Tanto a função membro `size()` como a `length()` retornam o número de caracteres em uma string:

```cpp
if (cobra1.length() == cobra2.size())
    cout << "As duas strings têm o mesmo comprimento.\n";
```

Por que duas funções fazendo a mesma coisa? O método `length()` vem de versões anteriores da classe `string`, e `size()` foi adicionado para compatibilidade com a STL.

Você pode pesquisar uma string em busca de uma determinada substring ou caractere de diversas formas. A Tabela 16.2 fornece uma descrição breve de quatro variações do método `find()`. Lembre-se de que `string::npos` é o número máximo possível de caracteres em uma string — tipicamente o maior valor de `unsigned int` ou `unsigned long`.

**Tabela 16.2 — O Método `find()` Sobrecarregado**

| Protótipo | Descrição |
|---|---|
| `size_type find(const string &str, size_type pos = 0) const` | Encontra a primeira ocorrência da substring `str`, iniciando a busca na localização `pos` na string invocadora. Retorna o índice do primeiro caractere da substring se encontrada e `string::npos` caso contrário. |
| `size_type find(const char *s, size_type pos = 0) const` | Encontra a primeira ocorrência da substring `s`, iniciando a busca na localização `pos`. Retorna o índice do primeiro caractere se encontrado e `string::npos` caso contrário. |
| `size_type find(const char *s, size_type pos = 0, size_type n) const` | Encontra a primeira ocorrência da substring formada pelos primeiros `n` caracteres de `s`, iniciando na posição `pos`. Retorna o índice do primeiro caractere se encontrado e `string::npos` caso contrário. |
| `size_type find(char ch, size_type pos = 0) const` | Encontra a primeira ocorrência do caractere `ch`, iniciando a busca na localização `pos`. Retorna o índice do caractere se encontrado e `string::npos` caso contrário. |

A biblioteca `string` também fornece os métodos relacionados `rfind()`, `find_first_of()`, `find_last_of()`, `find_first_not_of()` e `find_last_not_of()`, cada um com o mesmo conjunto de assinaturas de função sobrecarregadas que o método `find()`. O método `rfind()` encontra a última ocorrência de uma substring ou caractere. O método `find_first_of()` encontra a primeira ocorrência na string invocadora de qualquer um dos caracteres no argumento. Por exemplo, a seguinte instrução retornaria a localização do `r` em `"cobra"` (ou seja, o índice 3), porque essa é a primeira ocorrência de qualquer uma das letras de `"hark"` em `"cobra"`:

```cpp
int onde = cobra1.find_first_of("hark");
```

O método `find_last_of()` funciona da mesma forma, exceto que encontra a última ocorrência. O método `find_first_not_of()` encontra o primeiro caractere na string invocadora que não é um caractere no argumento.

Existem muitos outros métodos, mas esses são suficientes para montar um programa de exemplo que é uma versão graficamente humilde do jogo de palavras Forca. O jogo armazena uma lista de palavras em um array de objetos `string`, escolhe uma palavra aleatoriamente e deixa você adivinhar letras da palavra. Seis erros e você perde. O programa usa a função `find()` para verificar seus palpites e o operador `+=` para construir um objeto `string` para acompanhar seus erros. Para acompanhar os acertos, o programa cria uma palavra do mesmo comprimento que a palavra secreta, mas composta de hifens. Os hifens são então substituídos pelos acertos corretos. A Listagem 16.3 mostra o programa.

**Listagem 16.3 — forca.cpp**

```cpp
// forca.cpp -- alguns métodos de string
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <cctype>
using std::string;
const int NUM = 26;
const string listaPalavras[NUM] = {"apiary", "beetle", "cereal",
    "danger", "ensign", "florid", "garage", "health", "insult",
    "jackal", "keeper", "loaner", "manage", "nonce", "onset",
    "plaid", "quilt", "remote", "stolid", "train", "useful",
    "valid", "whence", "xenon", "yearn", "zippy"};
int main()
{
    using std::cout;
    using std::cin;
    using std::tolower;
    using std::endl;
    std::srand(std::time(0));
    char jogar;
    cout << "Quer jogar um jogo de palavras? <s/n> ";
    cin >> jogar;
    jogar = tolower(jogar);
    while (jogar == 's')
    {
        string alvo = listaPalavras[std::rand() % NUM];
        int comprimento = alvo.length();
        string tentativa(comprimento, '-');
        string letrasErradas;
        int tentativas = 6;
        cout << "Adivinhe minha palavra secreta. Ela tem " << comprimento
             << " letras, e você\nadivinhe uma letra por vez. "
             << "Você tem " << tentativas << " erros permitidos.\n";
        cout << "Sua palavra: " << tentativa << endl;
        while (tentativas > 0 && tentativa != alvo)
        {
            char letra;
            cout << "Adivinhe uma letra: ";
            cin >> letra;
            if (letrasErradas.find(letra) != string::npos
                || tentativa.find(letra) != string::npos)
            {
                cout << "Você já tentou essa. Tente outra.\n";
                continue;
            }
            int pos = alvo.find(letra);
            if (pos == string::npos)
            {
                cout << "Ah, palpite errado!\n";
                --tentativas;
                letrasErradas += letra; // adicionar à string
            }
            else
            {
                cout << "Bom palpite!\n";
                tentativa[pos] = letra;
                // verificar se a letra aparece novamente
                pos = alvo.find(letra, pos + 1);
                while (pos != string::npos)
                {
                    tentativa[pos] = letra;
                    pos = alvo.find(letra, pos + 1);
                }
            }
            cout << "Sua palavra: " << tentativa << endl;
            if (tentativa != alvo)
            {
                if (letrasErradas.length() > 0)
                    cout << "Letras erradas: " << letrasErradas << endl;
                cout << tentativas << " erros restantes\n";
            }
        }
        if (tentativas > 0)
            cout << "Acertou!\n";
        else
            cout << "Desculpe, a palavra era " << alvo << ".\n";
        cout << "Quer jogar de novo? <s/n> ";
        cin >> jogar;
        jogar = tolower(jogar);
    }
    cout << "Até logo\n";
    return 0;
}
```

**Notas do Programa**

Na Listagem 16.3, o fato de os operadores relacionais serem sobrecarregados permite tratar strings da mesma forma que variáveis numéricas:

```cpp
while (tentativas > 0 && tentativa != alvo)
```

Isso é mais fácil de entender do que, por exemplo, usar `strcmp()` com strings C-style.

O programa usa `find()` para verificar se um caractere já foi selecionado; se foi, ele será encontrado na string `letrasErradas` (erros) ou na string `tentativa` (acertos):

```cpp
if (letrasErradas.find(letra) != string::npos
    || tentativa.find(letra) != string::npos)
```

A variável `npos` é um membro estático da classe `string`. Seu valor, lembre-se, é o número máximo permitido de caracteres para um objeto `string`. Portanto, como a indexação começa em zero, é um valor maior do que o maior índice possível e pode ser usado para indicar falha ao localizar um caractere ou uma string.

O programa aproveita o fato de que uma das versões sobrecarregadas do operador `+=` permite acrescentar caracteres individuais a uma string:

```cpp
letrasErradas += letra; // acrescentar um char a um objeto string
```

O coração do programa começa verificando se a letra escolhida está na palavra secreta:

```cpp
int pos = alvo.find(letra);
```

Se `pos` for um valor válido, a letra pode ser colocada na posição correspondente na string de resposta:

```cpp
tentativa[pos] = letra;
```

Entretanto, uma determinada letra pode ocorrer mais de uma vez na palavra secreta, portanto o programa continua verificando. Ele usa o segundo argumento opcional de `find()`, que especifica um ponto de partida na string de onde iniciar a busca. Como a letra foi encontrada na localização `pos`, a próxima busca deve começar em `pos + 1`. Um laço `while` mantém a busca em andamento até que não sejam encontradas mais ocorrências daquele caractere:

```cpp
pos = alvo.find(letra, pos + 1);
while (pos != string::npos)
{
    tentativa[pos] = letra;
    pos = alvo.find(letra, pos + 1);
}
```

### O que Mais a Classe string Oferece?

A biblioteca `string` fornece muitas outras funcionalidades. Há funções para apagar parte ou toda uma string, para substituir parte ou toda uma string por parte ou toda outra string, para inserir material em uma string ou remover material de uma string, para comparar parte ou toda uma string com parte ou toda outra string e para extrair uma substring de uma string. Há uma função para copiar parte de uma string para outra e uma função para trocar o conteúdo de duas strings. A maioria dessas funções é sobrecarregada para que possa trabalhar com strings C-style e com objetos `string`.

Primeiro, pense no recurso de ajuste automático de tamanho. Na Listagem 16.3, o que acontece cada vez que o programa acrescenta uma letra a uma string? Não é possível simplesmente expandir a string no lugar, pois isso pode atingir memória vizinha que já está em uso. Portanto, pode ser necessário alocar um novo bloco e depois copiar o conteúdo antigo para um novo local. Seria ineficiente fazer isso com frequência, portanto muitas implementações de C++ alocam um bloco de memória maior do que a string real, dando à string espaço para crescer. Se a string eventualmente ultrapassar esse tamanho, o programa aloca um novo bloco com o dobro do tamanho para oferecer mais espaço para crescimento sem redimensionamento contínuo. O método `capacity()` retorna o tamanho do bloco atual, e o método `reserve()` permite solicitar um tamanho mínimo para o bloco. A Listagem 16.4 mostra um exemplo que usa esses métodos.

**Listagem 16.4 — str2.cpp**

```cpp
// str2.cpp -- capacity() e reserve()
#include <iostream>
#include <string>
int main()
{
    using namespace std;
    string vazio;
    string pequeno = "bit";
    string maior = "Elefantes são os melhores amigos da menina";
    cout << "Tamanhos:\n";
    cout << "\tvazio: " << vazio.size() << endl;
    cout << "\tpequeno: " << pequeno.size() << endl;
    cout << "\tmaior: " << maior.size() << endl;
    cout << "Capacidades:\n";
    cout << "\tvazio: " << vazio.capacity() << endl;
    cout << "\tpequeno: " << pequeno.capacity() << endl;
    cout << "\tmaior: " << maior.capacity() << endl;
    vazio.reserve(50);
    cout << "Capacidade após vazio.reserve(50): "
         << vazio.capacity() << endl;
    return 0;
}
```

Aqui está a saída do programa da Listagem 16.4 para uma implementação de C++:

```
Sizes:
    empty: 0
    small: 3
    larger: 34
Capacities:
    empty: 15
    small: 15
    larger: 47
Capacity after empty.reserve(50): 63
```

Observe que essa implementação usa uma capacidade mínima de 15 caracteres e parece usar 1 a menos do que múltiplos de 16 como escolhas padrão de capacidade. Outras implementações podem fazer escolhas diferentes.

E se você tiver um objeto `string` mas precisar de uma string C-style? Por exemplo, você pode querer abrir um arquivo cujo nome está em um objeto `string`:

```cpp
string nomeArquivo;
cout << "Digite o nome do arquivo: ";
cin >> nomeArquivo;
ofstream fout;
```

A má notícia é que o método `open()` exige um argumento de string C-style. A boa notícia é que o método `c_str()` retorna um ponteiro para uma string C-style com o mesmo conteúdo do objeto `string` invocador. Portanto, você pode usar:

```cpp
fout.open(nomeArquivo.c_str());
```

### Variedades de String

Esta seção trata a classe `string` como se fosse baseada no tipo `char`. Na verdade, como mencionado anteriormente, a biblioteca `string` é realmente baseada em uma classe template:

```cpp
template<class charT, class traits = char_traits<charT>,
         class Allocator = allocator<charT> >
basic_string {...};
```

O template `basic_string` vem com quatro especializações, cada uma com um nome `typedef`:

```cpp
typedef basic_string<char>     string;
typedef basic_string<wchar_t>  wstring;
typedef basic_string<char16_t> u16string; // C++11
typedef basic_string<char32_t> u32string;  // C++11
```

Isso permite usar strings baseadas nos tipos `wchar_t`, `char16_t` e `char32_t`, além do tipo `char`. Você poderia até desenvolver algum tipo de classe semelhante a caractere e usar o template de classe `basic_string` com ela, desde que sua classe satisfaça certos requisitos. A classe `traits` descreve fatos específicos sobre o tipo de caractere escolhido, como como comparar valores. Há especializações predefinidas do template `char_traits` para os tipos `char`, `wchar_t`, `char16_t` e `char32_t`, e essas são os valores padrão para `traits`. A classe `Allocator` representa uma classe para gerenciar a alocação de memória. Há especializações predefinidas do template `allocator` para os vários tipos de caractere, e essas são os padrões. Elas usam `new` e `delete`.

---

## Templates de Classe de Ponteiros Inteligentes

Um *ponteiro inteligente* (smart pointer) é um objeto de classe que age como um ponteiro mas possui recursos adicionais. Aqui examinaremos três templates de ponteiros inteligentes que podem ajudar a gerenciar o uso de alocação de memória dinâmica. Vamos começar observando o que pode ser necessário e como isso pode ser realizado. Considere a seguinte função:

```cpp
void remodelar(std::string & str)
{
    std::string * ps = new std::string(str);
    ...
    str = ps;
    return;
}
```

Provavelmente você vê sua falha. Cada vez que a função é chamada, ela aloca memória do heap mas nunca a libera, criando assim um vazamento de memória (memory leak). Você também conhece a solução — basta se lembrar de liberar a memória alocada adicionando a seguinte instrução logo antes da instrução `return`:

```cpp
delete ps;
```

Porém, uma solução envolvendo a frase "basta se lembrar de" raramente é a melhor solução. Às vezes você não vai se lembrar. Ou vai se lembrar mas acidentalmente removerá ou comentará o código. E mesmo que você se lembre, ainda pode haver problemas. Considere a seguinte variação:

```cpp
void remodelar(std::string & str)
{
    std::string * ps = new std::string(str);
    ...
    if (coisa_estranha())
        throw exception();
    str = *ps;
    delete ps;
    return;
}
```

Se a exceção for lançada, a instrução `delete` não é alcançada e, novamente, há um vazamento de memória.

Você pode corrigir essa omissão, como ilustrado no Capítulo 14, mas seria bom ter uma solução mais elegante. Vamos pensar no que é necessário. Quando uma função como `remodelar()` termina, seja normalmente ou lançando uma exceção, as variáveis locais são removidas da memória de pilha — portanto, a memória ocupada pelo ponteiro `ps` é liberada. Seria bom se, além disso, a memória apontada por `ps` também fosse liberada. Se `ps` tivesse um destrutor, esse destrutor poderia liberar a memória apontada quando `ps` expirasse. Portanto, o problema com `ps` é que ele é apenas um ponteiro comum e não um objeto de classe com destrutor. Se fosse um objeto, poderíamos ter seu destrutor excluindo a memória apontada quando o objeto expirar. E essa é a ideia por trás de `auto_ptr`, `unique_ptr` e `shared_ptr`. O template `auto_ptr` é a solução do C++98. O C++11 depreca `auto_ptr` e fornece os outros dois como alternativas. Porém, embora deprecado, `auto_ptr` foi usado por anos e pode ser sua única opção se seu compilador não suportar os outros dois.

### Usando Ponteiros Inteligentes

Esses três templates de ponteiros inteligentes (`auto_ptr`, `unique_ptr` e `shared_ptr`) cada um define um objeto semelhante a um ponteiro destinado a receber um endereço obtido (direta ou indiretamente) por `new`. Quando o ponteiro inteligente expira, seu destrutor usa `delete` para liberar a memória. Portanto, se você atribuir um endereço retornado por `new` a um desses objetos, não precisará se lembrar de liberar a memória mais tarde — ela será liberada automaticamente quando o objeto ponteiro inteligente expirar. A Figura 16.2 ilustra a diferença comportamental entre `auto_ptr` e um ponteiro regular.

Para criar um desses objetos de ponteiro inteligente, você inclui o arquivo de cabeçalho `memory`, que contém as definições de template. Em seguida, você usa a sintaxe de template usual para instanciar o tipo de ponteiro que você precisa. O template `auto_ptr`, por exemplo, inclui o seguinte construtor:

```cpp
template<class X> class auto_ptr {
public:
    explicit auto_ptr(X* p = 0) throw();
...};
```

(A notação `throw()` significa que esse construtor não lança uma exceção. Assim como `auto_ptr`, ela é deprecada.) Assim, pedir um objeto `auto_ptr` do tipo `X` lhe dá um objeto `auto_ptr` que aponta para o tipo `X`:

```cpp
auto_ptr<double> pd(new double);
    // pd é um auto_ptr para double (use em vez de double * pd)
auto_ptr<string> ps(new string);
    // ps é um auto_ptr para string (use em vez de string * ps)
```

Os outros dois ponteiros inteligentes usam a mesma sintaxe:

```cpp
unique_ptr<double> pdu(new double); // pdu é um unique_ptr para double
shared_ptr<string> pss(new string); // pss é um shared_ptr para string
```

Assim, para converter a função `remodelar()`, você seguiria estas três etapas:

1. Incluir o arquivo de cabeçalho `memory`.
2. Substituir o ponteiro para `string` por um objeto de ponteiro inteligente que aponta para `string`.
3. Remover a instrução `delete`.

Aqui está a função com essas alterações feitas usando `auto_ptr`:

```cpp
#include <memory>
void remodelar(std::string & str)
{
    std::auto_ptr<std::string> ps(new std::string(str));
    ...
    if (coisa_estranha())
        throw exception();
    str = *ps;
    // delete ps; NÃO É MAIS NECESSÁRIO
    return;
}
```

Observe que os ponteiros inteligentes pertencem ao namespace `std`. A Listagem 16.5 apresenta um programa simples usando todos os três tipos de ponteiros inteligentes. Cada uso é colocado dentro de um bloco de modo que o ponteiro expire quando a execução sair do bloco. A classe `Relatorio` usa métodos detalhados para reportar quando um objeto é criado ou destruído.

**Listagem 16.5 — ponteiros_int.cpp**

```cpp
// ponteiros_int.cpp -- usando três tipos de ponteiros inteligentes
// requer suporte para shared_ptr e unique_ptr do C++11
#include <iostream>
#include <string>
#include <memory>
class Relatorio
{
private:
    std::string str;
public:
    Relatorio(const std::string s) : str(s)
        { std::cout << "Objeto criado!\n"; }
    ~Relatorio() { std::cout << "Objeto destruído!\n"; }
    void comentario() const { std::cout << str << "\n"; }
};
int main()
{
    {
        std::auto_ptr<Relatorio> ps(new Relatorio("usando auto_ptr"));
        ps->comentario(); // usar -> para invocar uma função membro
    }
    {
        std::shared_ptr<Relatorio> ps(new Relatorio("usando shared_ptr"));
        ps->comentario();
    }
    {
        std::unique_ptr<Relatorio> ps(new Relatorio("usando unique_ptr"));
        ps->comentario();
    }
    return 0;
}
```

Aqui está a saída:

```
Object created!
using auto_ptr
Object deleted!
Object created!
using shared_ptr
Object deleted!
Object created!
using unique_ptr
Object deleted!
```

Cada uma dessas classes tem um construtor `explicit` que recebe um ponteiro como argumento. Portanto, não há conversão automática de tipo de um ponteiro para um objeto de ponteiro inteligente:

```cpp
shared_ptr<double> pd;
double *p_reg = new double;
pd = p_reg;                         // não permitido (conversão implícita)
pd = shared_ptr<double>(p_reg);     // permitido (conversão explícita)
shared_ptr<double> pCompartilhado = p_reg;  // não permitido
shared_ptr<double> pCompartilhado(p_reg);   // permitido
```

As classes de template de ponteiros inteligentes são definidas de modo que, na maioria dos aspectos, um objeto de ponteiro inteligente age como um ponteiro regular. Por exemplo, dado que `ps` é um objeto de ponteiro inteligente, você pode desreferenciá-lo (`*ps`), usá-lo para acessar membros de estrutura (`ps->indiceBuffer`), e atribuí-lo a um ponteiro regular que aponta para o mesmo tipo.

Mas aqui está algo que você deve evitar com todos esses três ponteiros inteligentes:

```cpp
string ferias("Errei minha solidão como uma nuvem.");
shared_ptr<string> pvac(&ferias); // NÃO!
```

Quando `pvac` expirar, o programa aplicaria o operador `delete` a memória fora do heap, o que é errado.

### Considerações sobre Ponteiros Inteligentes

Por que três ponteiros inteligentes? (Na verdade, há quatro, mas não discutiremos `weak_ptr`.) E por que `auto_ptr` está sendo deprecado?

Comece considerando a atribuição:

```cpp
auto_ptr<string> ps(new string("Reguei minha solidão como uma nuvem."));
auto_ptr<string> vocacao;
vocacao = ps;
```

O que a instrução de atribuição deve fazer? Se `ps` e `vocacao` fossem ponteiros comuns, o resultado seriam dois ponteiros apontando para o mesmo objeto `string`. Isso não é aceitável aqui porque o programa acabaria tentando excluir o mesmo objeto duas vezes — uma quando `ps` expira, e outra quando `vocacao` expira. Há formas de evitar esse problema:

- Definir o operador de atribuição de modo que ele faça uma cópia profunda (deep copy). Isso resulta em dois ponteiros apontando para dois objetos distintos, um dos quais é uma cópia do outro.
- Instituir o conceito de *propriedade* (ownership), permitindo que apenas um ponteiro inteligente seja proprietário de um objeto específico. Somente se o ponteiro inteligente for proprietário do objeto é que seu destrutor excluirá o objeto. Em seguida, a atribuição transfere a propriedade. Essa é a estratégia usada para `auto_ptr` e para `unique_ptr`, embora `unique_ptr` seja um pouco mais restritivo.
- Criar um ponteiro ainda mais inteligente que mantém o controle de quantos ponteiros inteligentes se referem a um objeto específico. Isso é chamado de *contagem de referências* (reference counting). A atribuição, por exemplo, aumenta a contagem em um, e a expiração de um ponteiro diminui a contagem em um. Apenas quando o último ponteiro expirar é que `delete` será invocado. Essa é a estratégia do `shared_ptr`.

As mesmas estratégias discutidas para atribuição naturalmente também se aplicam aos construtores de cópia.

Cada abordagem tem seus usos. A Listagem 16.6 mostra um exemplo para o qual `auto_ptr` é inadequado.

**Listagem 16.6 — aves.cpp**

```cpp
// aves.cpp -- auto_ptr é uma escolha ruim aqui
#include <iostream>
#include <string>
#include <memory>
int main()
{
    using namespace std;
    auto_ptr<string> filmes[5] =
    {
        auto_ptr<string>(new string("Penas no Ar")),
        auto_ptr<string>(new string("Patos em Marcha")),
        auto_ptr<string>(new string("Galinhas Fugindo")),
        auto_ptr<string>(new string("Erros de Perus")),
        auto_ptr<string>(new string("Ovos de Ganso"))
    };
    auto_ptr<string> vencedor;
    vencedor = filmes[2]; // filmes[2] perde a propriedade
    cout << "Os indicados ao melhor filme de baseball de aves são\n";
    for (int i = 0; i < 5; i++)
        cout << *filmes[i] << endl;
    cout << "O vencedor é " << *vencedor << "!\n";
    cin.get();
    return 0;
}
```

Aqui está uma saída de exemplo:

```
Os indicados ao melhor filme de baseball de aves são
Penas no Ar
Patos em Marcha
Falha de segmentação (núcleo despejado)
```

A mensagem "núcleo despejado" deve ajudar a fixar em sua memória que um `auto_ptr` mal utilizado pode ser um problema. (O comportamento para esse tipo de código é indefinido, portanto você pode encontrar comportamento diferente dependendo de seu sistema.) Aqui o problema é que a seguinte instrução transfere a propriedade de `filmes[2]` para `vencedor`:

```cpp
vencedor = filmes[2]; // filmes[2] perde a propriedade
```

Isso faz com que `filmes[2]` não se refira mais à string. Quando o programa vai imprimir a string apontada por `filmes[2]`, ele encontra o ponteiro nulo, o que aparentemente é uma surpresa desagradável.

Suponha que você volte à Listagem 16.6 mas use `shared_ptr` em vez de `auto_ptr`. Então o programa funciona bem e dá esta saída:

```
Os indicados ao melhor filme de baseball de aves são
Penas no Ar
Patos em Marcha
Galinhas Fugindo
Erros de Perus
Ovos de Ganso
O vencedor é Galinhas Fugindo!
```

A diferença ocorre nesta parte do programa:

```cpp
shared_ptr<string> vencedor;
vencedor = filmes[2];
```

Desta vez, tanto `vencedor` quanto `filmes[2]` apontam para o mesmo objeto, e a contagem de referências é aumentada de 1 para 2. No final do programa, `vencedor`, que foi declarado por último, é o primeiro objeto a ter seu destrutor chamado. O destrutor diminui a contagem de referências para 1. Em seguida, os membros do array de `shared_ptr` são liberados. O destrutor para `filmes[2]` decrementa a contagem para 0 e libera o espaço anteriormente alocado.

### Por que `unique_ptr` É Melhor que `auto_ptr`

Considere as seguintes instruções:

```cpp
auto_ptr<string> p1(new string("auto")); // #1
auto_ptr<string> p2;                     // #2
p2 = p1;                                 // #3
```

Na instrução #3, `p2` assume a propriedade do objeto `string`, e `p1` é privado de propriedade. Isso é bom porque impede que os destrutores de `p1` e `p2` tentem excluir o mesmo objeto. Mas também é ruim se o programa subsequentemente tentar usar `p1`, pois `p1` não aponta mais para dados válidos.

Agora considere o equivalente com `unique_ptr`:

```cpp
unique_ptr<string> p3(new string("auto")); // #4
unique_ptr<string> p4;                     // #5
p4 = p3;                                   // #6
```

Neste caso, o compilador não permite a instrução #6, evitando assim o problema de `p3` não apontar para dados válidos. Portanto, `unique_ptr` é mais seguro (erro em tempo de compilação versus potencial travamento do programa) do que `auto_ptr`.

Mas há situações em que atribuir um ponteiro inteligente a outro não deixa para trás um órfão perigoso. Suponha que tenhamos esta definição de função:

```cpp
unique_ptr<string> demo(const char * s)
{
    unique_ptr<string> temp(new string(s));
    return temp;
}
```

E suponha que tenhamos este código:

```cpp
unique_ptr<string> ps;
ps = demo("Uniquamente especial");
```

Aqui, `demo()` retorna um `unique_ptr` temporário, e então `ps` assume a propriedade do objeto originalmente pertencente ao `unique_ptr` retornado. Então o `unique_ptr` retornado é destruído. Isso é aceitável porque `ps` agora tem a propriedade do objeto `string`. E outra coisa boa aconteceu: porque o `unique_ptr` temporário retornado por `demo()` logo é destruído, não há possibilidade de ele ser mal utilizado em uma tentativa de acessar dados inválidos. Em outras palavras, não há razão para proibir a atribuição neste caso. E, milagrosamente, o compilador permite!

Em resumo, se um programa tentar atribuir um `unique_ptr` a outro, o compilador permite se o objeto-fonte for um rvalue temporário e desautoriza se o objeto-fonte tiver alguma duração:

```cpp
using namespace std;
unique_ptr<string> pu1(new string("Oi ho!"));
unique_ptr<string> pu2;
pu2 = pu1;                                  // #1 não permitido
unique_ptr<string> pu3;
pu3 = unique_ptr<string>(new string("Ei!")); // #2 permitido
```

A atribuição #1 deixaria um `unique_ptr` pendurado para trás (ou seja, `pu1`), uma possível fonte de problemas. A atribuição #2 não deixa nenhum `unique_ptr` para trás porque invoca o construtor de `unique_ptr`, que constrói um objeto temporário que é destruído quando a propriedade é transferida para `pu3`. Esse comportamento seletivo é uma indicação de que `unique_ptr` é superior a `auto_ptr`, que permitiria ambas as formas de atribuição.

Claro, pode ser possível que você realmente queira fazer algo como a instrução #1. A atribuição só é insegura se você usar o ponteiro inteligente abandonado de forma imprudente, como desreferenciando-o. Mas você poderia reutilizar com segurança o ponteiro atribuindo um novo valor a ele. O C++ tem uma função de biblioteca padrão chamada `std::move()` que permite atribuir um `unique_ptr` a outro. Aqui está um exemplo usando a função `demo()` previamente definida:

```cpp
using namespace std;
unique_ptr<string> ps1, ps2;
ps1 = demo("Uniquamente especial");
ps2 = move(ps1);    // habilitar atribuição
ps1 = demo(" e mais");
cout << *ps2 << *ps1 << endl;
```

Você pode estar se perguntando como `unique_ptr`, diferentemente de `auto_ptr`, é capaz de distinguir entre usos seguros e possivelmente inseguros. A resposta é que ele usa as adições do C++11 de construtores de movimento e referências rvalue, discutidos no Capítulo 18.

Além disso, `unique_ptr` tem outra vantagem sobre `auto_ptr`: ele tem uma variante que pode ser usada com arrays. Lembre-se, você deve parear `delete` com `new` e `delete []` com `new []`. O template `auto_ptr` usa `delete`, não `delete []`, portanto só pode ser usado com `new`, não com `new []`. Mas `unique_ptr` tem uma versão `new[]`, `delete[]`:

```cpp
std::unique_ptr<double[]> pda(new double(5)); // usará delete []
```

> **Cuidado:** Você deve usar um objeto `auto_ptr` ou `shared_ptr` apenas para memória alocada por `new`, não para memória alocada por `new []`. Não use `auto_ptr`, `shared_ptr` ou `unique_ptr` para memória não alocada via `new` ou, no caso de `unique_ptr`, via `new` ou `new []`.

### Selecionando um Ponteiro Inteligente

Qual tipo de ponteiro usar? Se o programa usa mais de um ponteiro para um objeto, `shared_ptr` é a sua escolha. Por exemplo, você pode ter um array de ponteiros e usar alguns ponteiros auxiliares para identificar elementos específicos, como o maior e o menor. Ou você pode ter dois tipos de objetos que contêm ponteiros para o mesmo terceiro objeto. Ou você pode ter um container da STL de ponteiros. Muitos dos algoritmos da STL incluem operações de cópia ou atribuição que funcionarão com `shared_ptr` mas não com `unique_ptr` (você receberá um aviso do compilador) ou `auto_ptr` (você terá comportamento indefinido).

Se o programa não precisar de múltiplos ponteiros para o mesmo objeto, `unique_ptr` funciona. É uma boa escolha como tipo de retorno de uma função que retorna um ponteiro para memória alocada por `new`. Dessa forma, a propriedade é transferida para o `unique_ptr` atribuído ao valor de retorno, e esse ponteiro inteligente assume a responsabilidade de chamar `delete`. Você pode armazenar objetos `unique_ptr` em um container da STL contanto que não invoque métodos ou algoritmos, como `sort()`, que copiam ou atribuem um `unique_ptr` a outro. Por exemplo, assumindo os includes e declarações `using` adequados, fragmentos de código como os seguintes poderiam ser usados em um programa:

```cpp
unique_ptr<int> criar_int(int n)
{
    return unique_ptr<int>(new int(n));
}
void exibir(unique_ptr<int> & pi) // passar por referência
{
    cout << *pi << ' ';
}
int main()
{
...
    vector<unique_ptr<int> > vp(tamanho);
    for (int i = 0; i < vp.size(); i++)
        vp[i] = criar_int(rand() % 1000); // copiar unique_ptr temporário
    vp.push_back(criar_int(rand() % 1000)); // ok, pois o arg é temporário
    for_each(vp.begin(), vp.end(), exibir); // usar for_each()
...
}
```

Você pode atribuir um `unique_ptr` a um `shared_ptr` sob as mesmas condições que você pode atribuir um `unique_ptr` a outro — a fonte tem que ser um rvalue. Como mencionado anteriormente, no seguinte código `criar_int()` é uma função com tipo de retorno `unique_ptr<int>`:

```cpp
unique_ptr<int> pup(criar_int(rand() % 1000)); // ok
shared_ptr<int> spp(pup);                      // não permitido, pup é lvalue
shared_ptr<int> spr(criar_int(rand() % 1000)); // ok
```

O template `shared_ptr` inclui um construtor explícito para converter um `unique_ptr` rvalue para um `shared_ptr`. O `shared_ptr` assume a propriedade do objeto anteriormente pertencente ao `unique_ptr`.

Você usaria `auto_ptr` na mesma situação que `unique_ptr`, mas o segundo é preferido. Se seu compilador não fornecer `unique_ptr`, você pode considerar o `scoped_ptr` da biblioteca Boost, que é semelhante.

---

[Anterior](capitulo-15-03-rtti-casts-resumo.md) | [Índice](README.md) | [Próximo](capitulo-16-02-stl-iteradores.md)
