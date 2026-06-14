# Capítulo 17 — Entrada, Saída e Arquivos (parte 3)

> Tradução não oficial de *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Navegação: [Anterior](capitulo-17-02-istream-estados-arquivo.md) | [Índice](README.md) | [Próximo](capitulo-18-novo-padrao-cpp.md)

## Modos de Arquivo

Até agora, os programas de E/S de arquivo que examinamos usaram apenas os modos padrão para abrir arquivos. Quando você abre um arquivo fornecendo apenas o nome do arquivo como argumento, o modo padrão é usado. Na verdade, ao usar as classes de arquivo do C++, você pode especificar o modo explicitamente. Conforme mencionado na seção anterior, tanto o construtor quanto o método `open()` têm um segundo argumento opcional que especifica o modo de arquivo. O argumento é do tipo `ios_base::openmode`, que é um tipo de máscara de bits definido na enumeração aninhada `openmode`. Antes do C++ padronizado, os modos de arquivo eram fornecidos como valores `int`. A Tabela 17.7 mostra os valores de modo de arquivo definidos por `ios_base`.

**Tabela 17.7 — Constantes de Modo de Arquivo**

| Constante         | Significado                                                                 |
|-------------------|-----------------------------------------------------------------------------|
| `ios_base::in`    | Abrir arquivo para leitura.                                                 |
| `ios_base::out`   | Abrir arquivo para gravação.                                                |
| `ios_base::ate`   | Posicionar-se ao final do arquivo ao abrir.                                 |
| `ios_base::app`   | Acrescentar ao final do arquivo em toda gravação.                           |
| `ios_base::trunc` | Truncar o arquivo existente ao abrir.                                       |
| `ios_base::binary`| Arquivo binário (em oposição ao arquivo de texto).                          |

Você pode combinar constantes de modo com o operador OR bit a bit (`|`). Por exemplo, para abrir um arquivo chamado `arquivo.dat` para leitura e escrita sem truncamento, você usaria a seguinte instrução:

```cpp
fstream finout;
finout.open("arquivo.dat", ios_base::in | ios_base::out);
```

A Tabela 17.8 mostra algumas combinações de modos e seus equivalentes em linguagem C, para uso com `fopen()`. O C++ garante que esses modos e os modos de arquivo C sejam compatíveis quando você usa esses equivalentes.

**Tabela 17.8 — Modo de Arquivo C++ Versus Modo de Arquivo C**

| Modo C++                                              | Modo C | Significado                                                              |
|-------------------------------------------------------|--------|--------------------------------------------------------------------------|
| `ios_base::out`                                       | "w"    | Criar arquivo de texto para escrita; descartar conteúdo anterior.        |
| `ios_base::out \| ios_base::trunc`                    | "w"    | Criar arquivo de texto para escrita; descartar conteúdo anterior.        |
| `ios_base::out \| ios_base::app`                      | "a"    | Abrir arquivo de texto para acrescentar ao final.                        |
| `ios_base::in`                                        | "r"    | Abrir arquivo de texto para leitura.                                     |
| `ios_base::in \| ios_base::out`                       | "r+"   | Abrir arquivo de texto para leitura e escrita.                           |
| `ios_base::in \| ios_base::out \| ios_base::trunc`    | "w+"   | Abrir arquivo de texto para leitura e escrita; descartar conteúdo anterior. |

Acrescentar `ios_base::binary` a qualquer modo da coluna C++ abre o arquivo no modo binário em vez de no modo texto e substitui "texto" por "binário" na coluna de significado. Por exemplo, para abrir `dados.dat` para leitura e escrita binárias, você poderia usar:

```cpp
fstream finout("dados.dat", ios_base::in | ios_base::out | ios_base::binary);
```

**Nota:** A Tabela 17.8 não lista todas as combinações possíveis. Muitas implementações fornecem modos adicionais que não são exigidos pelo padrão C++.

### Acrescentando a um Arquivo

Vamos ver um exemplo de uso de modo de arquivo. Acrescentar a um arquivo (em oposição a truncá-lo) é uma tarefa comum, e a Listagem 17.18 demonstra como fazer isso. O programa também demonstra como reutilizar um fluxo de arquivo — primeiro para saída e depois para entrada.

**Listagem 17.18 — `acrescentar.cpp`**

```cpp
// acrescentar.cpp -- acrescentar informações a um arquivo
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib> // para exit()
const char * arquivo = "convidados.dat";
int main()
{
    using namespace std;
    char nome[35];
    ofstream fout(arquivo, ios_base::out | ios_base::app);
    if (!fout.is_open())
    {
        cerr << "Não foi possível abrir o arquivo " << arquivo << ".\n";
        exit(EXIT_FAILURE);
    }
    cout << "Adicionar nome ao arquivo " << arquivo << ".\n"
         << "Digite o nome (linha vazia para encerrar): ";
    while (cin.getline(nome, 35) && nome[0] != '\0')
        fout << nome << endl;
    fout.close(); // encerrar uso de arquivo para saída
    // reutilizar fout para entrada
    ifstream fin(arquivo);
    if (!fin.is_open())
    {
        cerr << "Não foi possível abrir o arquivo " << arquivo << " para leitura.\n";
        exit(EXIT_FAILURE);
    }
    cout << "Eis o conteúdo do arquivo " << arquivo << ":\n";
    string linha;
    while (getline(fin, linha))
        cout << linha << endl;
    fin.close();
    return 0;
}
```

Eis o resultado de duas execuções consecutivas do programa da Listagem 17.18:

```
Adicionar nome ao arquivo convidados.dat.
Digite o nome (linha vazia para encerrar): Genghis Kant
Alice Happiness

Eis o conteúdo do arquivo convidados.dat:
Genghis Kant
Alice Happiness
```

```
Adicionar nome ao arquivo convidados.dat.
Digite o nome (linha vazia para encerrar): Bertrand Rustle

Eis o conteúdo do arquivo convidados.dat:
Genghis Kant
Alice Happiness
Bertrand Rustle
```

Na primeira execução, o arquivo `convidados.dat` não existia, portanto o programa o criou e adicionou os dois primeiros nomes. Na segunda execução, o arquivo existia, portanto o programa o abriu e acrescentou o novo nome ao final.

Vamos examinar partes do código. O programa abre o arquivo para saída com o modo `app`:

```cpp
ofstream fout(arquivo, ios_base::out | ios_base::app);
```

Com esse modo, toda escrita vai para o final do arquivo, de modo que o arquivo não é truncado. O programa usa `getline()` para ler um nome de cada vez do teclado, e `fout` para escrever o nome no arquivo:

```cpp
while (cin.getline(nome, 35) && nome[0] != '\0')
    fout << nome << endl;
```

Em seguida, o programa fecha o arquivo de saída e abre o arquivo para entrada:

```cpp
fout.close();
ifstream fin(arquivo);
```

Finalmente, o programa exibe o conteúdo do arquivo. Ele usa `getline()` com o objeto `ifstream`:

```cpp
while (getline(fin, linha))
    cout << linha << endl;
```

Isso funciona porque os métodos de entrada `istream` funcionam igualmente bem se usados com um objeto `ifstream` ou com `cin`.

## Arquivos Binários

Quando você armazena dados em um arquivo de texto, os dados são convertidos para representação de texto. Por exemplo, o valor `-2.324216e+07` pode ser armazenado como os doze caracteres `-2.324216e+07`. Isso fornece uma representação legível, mas o tamanho do arquivo pode ser maior do que o necessário. Além disso, é necessária uma conversão de e para a representação textual quando os dados são lidos e escritos. O armazenamento em arquivo de texto é mais lento do que o armazenamento binário, pois cada número deve ser convertido.

Em um arquivo binário (binary file), os dados são armazenados usando a mesma representação de bit a bit que o computador usa internamente. Um valor de `double` de oito bytes seria armazenado como oito bytes em um arquivo binário, mas poderia ser armazenado como uma sequência de até quinze caracteres em um arquivo de texto. Por outro lado, um arquivo binário é menos portátil; em muitos sistemas, o formato binário de `int`, por exemplo, difere de um sistema para outro.

No contexto de arquivo do C++, o modo de texto (text mode) é a forma padrão de usar arquivos. O modo binário (binary mode) não converte os dados de/para representação de caractere ao gravar/ler.

Para usar o modo binário, você acrescenta `ios_base::binary` ao modo de abertura. Para ler e gravar arquivos binários, usa-se os métodos `write()` e `read()`. Para um arquivo binário, o método `write()` funciona com um ponteiro para `char`:

```cpp
fout.write((char *) &pl, sizeof pl);
```

Isso coloca no arquivo a representação binária de todos os bits em `pl`. A conversão para `char *` é necessária porque `write()` trabalha com sequências de bytes genéricas. Similarmente, para ler de um arquivo binário:

```cpp
fin.read((char *) &pl, sizeof pl);
```

A Listagem 17.19 usa esses métodos para criar e exibir um arquivo binário de dados de planetas.

**Listagem 17.19 — `binario.cpp`**

```cpp
// binario.cpp -- acesso a arquivo binário
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdlib> // para exit()

inline void descartarLinha() { while (std::cin.get() != '\n') continue; }

struct planeta
{
    char nome[20];        // nome do planeta
    double populacao;     // população
    double g;             // aceleração gravitacional em relação à da Terra
};
const char * arquivo = "planetas.dat";
int main()
{
    using namespace std;
    planeta pl;
    cout << fixed << right;
    // criar novo arquivo, adicionar dados
    ifstream fin;
    fin.open(arquivo, ios_base::in | ios_base::binary);
    if (!fin.is_open()) // arquivo NOVO
    {
        ofstream fout(arquivo, ios_base::out | ios_base::binary);
        cout << "Criando novo arquivo " << arquivo << ".\n"
             << "Insira nome do planeta (vazio para encerrar):\n";
        while (cin.get(pl.nome, 20) && pl.nome[0] != '\0')
        {
            descartarLinha();
            cout << "Insira população do planeta: ";
            cin >> pl.populacao;
            cout << "Insira aceleração gravitacional em relação à da Terra: ";
            cin >> pl.g;
            descartarLinha();
            fout.write((char *) &pl, sizeof pl);
        }
        fout.close();
        fin.open(arquivo, ios_base::in | ios_base::binary);
    }
    if (!fin.is_open())
    {
        cerr << "Não foi possível abrir " << arquivo << " para leitura.\n";
        exit(EXIT_FAILURE);
    }
    cout << "Eis o conteúdo de " << arquivo << ":\n"
         << setw(20) << left << "Nome do Planeta" << setw(15) << right
         << "Populacao" << setw(15) << "g/Terra\n";
    while (fin.read((char *) &pl, sizeof pl))
    {
        cout << setw(20) << left << pl.nome << setw(15) << right
             << pl.populacao << setw(15) << pl.g << endl;
    }
    fin.close();
    return 0;
}
```

Eis o resultado de duas execuções do programa da Listagem 17.19:

```
Criando novo arquivo planetas.dat.
Insira nome do planeta (vazio para encerrar):
Sol
Insira população do planeta: 0
Insira aceleração gravitacional em relação à da Terra: 27.9
Marte
Insira população do planeta: 0
Insira aceleração gravitacional em relação à da Terra: 0.37

Eis o conteúdo de planetas.dat:
Nome do Planeta     Populacao        g/Terra
Sol                 0.000000        27.900000
Marte               0.000000         0.370000
```

```
Eis o conteúdo de planetas.dat:
Nome do Planeta     Populacao        g/Terra
Sol                 0.000000        27.900000
Marte               0.000000         0.370000
```

Na segunda execução, o arquivo já existia, portanto o programa o leu e exibiu seu conteúdo. Observe que o arquivo `planetas.dat` não é legível como arquivo de texto porque os dados numéricos estão em formato binário.

## Acesso Aleatório

Até agora, os programas de arquivo que você viu têm feito acesso sequencial — cada operação de leitura ou escrita ocorre imediatamente após a anterior. Às vezes, você precisa de acesso aleatório (random access) — o equivalente a saltar para qualquer posição em um arquivo. Por exemplo, um banco de dados pode precisar alterar um registro específico sem ler todos os registros anteriores.

O C++ fornece os métodos `seekg()` e `seekp()` para acesso aleatório. O método `seekg()` move o ponteiro de leitura de entrada (a posição atual do fluxo de entrada); `seekp()` move o ponteiro de escrita de saída. (Lembre-se de que os fluxos têm buffers internos; `seekp()` posiciona o ponteiro no buffer de saída antes de ser descarregado.)

Esses métodos recebem dois argumentos. O primeiro é o deslocamento (offset) a partir de alguma posição no arquivo; o segundo especifica a qual ponto do arquivo o deslocamento é relativo. A Tabela 17.9 mostra os valores possíveis para o segundo argumento.

**Tabela 17.9 — Referências de Posição de Arquivo**

| Constante          | Descrição                        |
|--------------------|----------------------------------|
| `ios_base::beg`    | Relativo ao início do arquivo.   |
| `ios_base::cur`    | Relativo à posição atual.        |
| `ios_base::end`    | Relativo ao final do arquivo.    |

O tipo do primeiro argumento é `streamoff`, que é um tipo inteiro suficientemente grande para representar o maior deslocamento de posição de arquivo possível no sistema. Os argumentos podem ser negativos para andar de volta no arquivo. O segundo argumento é do tipo `ios_base::seekdir`, uma enumeração com os três valores listados na Tabela 17.9.

Portanto, para ir ao décimo byte de um arquivo, você poderia usar:

```cpp
fin.seekg(10, ios_base::beg); // mover 10 bytes a partir do início
```

Para recuar 10 bytes a partir da posição atual, use:

```cpp
fin.seekg(-10, ios_base::cur); // mover 10 bytes para trás a partir da posição atual
```

Para ir a 10 bytes antes do final do arquivo, use:

```cpp
fin.seekg(-10, ios_base::end); // mover 10 bytes para trás a partir do final
```

Para ir ao início do arquivo, use:

```cpp
fin.seekg(0, ios_base::beg); // ir ao início do arquivo
```

Os métodos `tellg()` e `tellp()` relatam a posição atual de um fluxo de entrada e de saída, respectivamente. O tipo de retorno é `streampos`, que pode ser um tipo inteiro ou uma classe. Você pode usar os valores retornados para salvar uma posição e depois retornar a ela mais tarde:

```cpp
streampos lugar = fin.tellg();
// ... ler alguns dados ...
fin.seekg(lugar); // retornar à posição salva
```

Você pode usar `streampos` com `seekg()` em uma forma de um único argumento, sem o segundo argumento de ponto de referência:

```cpp
fin.seekg(lugar); // ir para a posição salva
```

A Listagem 17.20 usa essas técnicas para fornecer acesso aleatório ao arquivo binário criado pela Listagem 17.19. O programa exibe o conteúdo do arquivo, permite que você selecione um registro para alterar e então exibe o arquivo modificado.

**Listagem 17.20 — `acesso_aleatorio.cpp`**

```cpp
// acesso_aleatorio.cpp -- acesso aleatório a arquivo binário
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdlib> // para exit()

inline void descartarLinha() { while (std::cin.get() != '\n') continue; }

const int LIM = 20;
struct planeta
{
    char nome[20];        // nome do planeta
    double populacao;     // população
    double g;             // aceleração gravitacional em relação à da Terra
};
const char * arquivo = "planetas.dat"; // supõe arquivo criado por binario.cpp
int main()
{
    using namespace std;
    planeta pl;
    cout << fixed << right;
    fstream finout;        // abrir para leitura e escrita
    finout.open(arquivo, ios_base::in | ios_base::out | ios_base::binary);
    if (!finout.is_open())
    {
        cerr << "Não foi possível abrir o arquivo " << arquivo << ".\n";
        exit(EXIT_FAILURE);
    }
    int cont = 0;
    finout.read((char *) &pl, sizeof pl);
    while (finout) // encerrar em caso de falha, como EOF
    {
        cout << cont++ << ": " << setw(20) << left << pl.nome
             << setw(15) << right << pl.populacao
             << setw(15) << pl.g << endl;
        finout.read((char *) &pl, sizeof pl);
    }
    if (finout.eof())
        finout.clear(); // limpar flag eof
    else
    {
        cerr << "Erro ao ler " << arquivo << ".\n";
        exit(EXIT_FAILURE);
    }
    cout << "Digite o número do registro que deseja alterar: ";
    long reg;
    cin >> reg;
    descartarLinha(); // descartar nova linha
    if (reg < 0 || reg >= cont)
    {
        cerr << "Número de registro inválido -- encerrando\n";
        exit(EXIT_FAILURE);
    }
    streampos posicao = reg * sizeof pl; // converter para tipo streampos
    finout.seekg(posicao); // acesso aleatório
    if (!finout.good())
    {
        cerr << "Erro ao tentar posicionar\n";
        exit(EXIT_FAILURE);
    }
    finout.read((char *) &pl, sizeof pl);
    cout << "Sua seleção:\n";
    cout << reg << ": " << setw(20) << left << pl.nome
         << setw(15) << right << pl.populacao
         << setw(15) << pl.g << endl;
    if (finout.eof())
        finout.clear(); // limpar flag eof
    cout << "Insira nome do planeta: ";
    cin.get(pl.nome, 20);
    descartarLinha();
    cout << "Insira população do planeta: ";
    cin >> pl.populacao;
    cout << "Insira aceleração gravitacional em relação à da Terra: ";
    cin >> pl.g;
    finout.seekp(posicao); // retornar ao início do registro
    finout.write((char *) &pl, sizeof pl) << flush;
    cont = 0;
    finout.seekg(0); // ir ao início do arquivo
    cout << "Eis o conteúdo completo de " << arquivo << " após as alterações:\n";
    while (finout.read((char *) &pl, sizeof pl))
    {
        cout << cont++ << ": " << setw(20) << left << pl.nome
             << setw(15) << right << pl.populacao
             << setw(15) << pl.g << endl;
    }
    finout.close();
    return 0;
}
```

Eis uma execução de amostra do programa da Listagem 17.20, supondo que `planetas.dat` tenha os dados criados no exemplo da Listagem 17.19:

```
0: Sol                 0.000000       27.900000
1: Marte               0.000000        0.370000
Digite o número do registro que deseja alterar: 1
Sua seleção:
1: Marte               0.000000        0.370000
Insira nome do planeta: Vênus
Insira população do planeta: 0
Insira aceleração gravitacional em relação à da Terra: 0.9
Eis o conteúdo completo de planetas.dat após as alterações:
0: Sol                 0.000000       27.900000
1: Vênus               0.000000        0.900000
```

**Notas do Programa**

A Listagem 17.20 usa o objeto `fstream` para abrir o arquivo para entrada e saída simultâneas:

```cpp
finout.open(arquivo, ios_base::in | ios_base::out | ios_base::binary);
```

O programa usa `seekp()` para posicionar o ponteiro de gravação antes de atualizar um registro:

```cpp
finout.seekp(posicao);
finout.write((char *) &pl, sizeof pl) << flush;
```

Observe que o valor de retorno de `write()` é uma referência ao objeto `finout`, de modo que `<< flush` é uma operação no objeto `finout`. Descarregar os dados garante que o arquivo seja atualizado antes da próxima leitura.

> **Trabalhando com Arquivos Temporários**
>
> Às vezes, um programa precisa usar um arquivo temporário — um arquivo criado para armazenar dados intermediários que não são necessários após o programa terminar. A função `tmpnam()` do cabeçalho `cstdio` gera um nome temporário exclusivo para um arquivo. Isso é útil para programas que precisam de vários arquivos temporários, pois garante que dois arquivos temporários diferentes não tenham o mesmo nome:
>
> ```cpp
> char nome[L_tmpnam];
> tmpnam(nome); // preencher nome com um nome de arquivo temporário
> ```
>
> A constante `L_tmpnam` é o tamanho necessário para conter o nome do arquivo temporário. A função cria um nome exclusivo para o arquivo, mas não cria o arquivo. Você precisará abrir o arquivo separadamente. Note que C++17 deprecia `tmpnam()`; alternativas modernas incluem funções específicas da plataforma ou a biblioteca `<filesystem>` do C++17.

## Formatação Interna (Incore Formatting)

As classes `istringstream` e `ostringstream` permitem que você use os métodos de fluxo `istream` e `ostream` com strings. São frequentemente denominadas E/S interna (incore I/O) ou formatação de string (string formatting), porque permitem que você formate dados para um destino de string em vez de para um arquivo ou tela, ou que leia dados de uma string formatada em vez de do teclado ou de um arquivo.

O cabeçalho `sstream` define as classes `istringstream` e `ostringstream`. A `ostringstream` herda de `ostream`, e a `istringstream` herda de `istream`. Assim, você pode usar os métodos familiares de ostream e istream com esses objetos.

**`ostringstream`**

O método `str()` de `ostringstream` retorna o objeto `string` para o qual a saída foi direcionada. A Listagem 17.21 ilustra como usar `ostringstream`.

**Listagem 17.21 — `saida_string.cpp`**

```cpp
// saida_string.cpp -- formatação interna com ostringstream
#include <iostream>
#include <sstream>
#include <string>
int main()
{
    using namespace std;
    ostringstream fluxoSaida; // gerencia um fluxo de string
    string hdisco;
    cout << "O que você gostaria de ler no rótulo do disco rígido? ";
    getline(cin, hdisco);
    int cap;
    cout << "Quantos bytes ele tem? ";
    cin >> cap;
    // escrever na string
    fluxoSaida << "Rótulo: " << hdisco << endl;
    fluxoSaida << "Capacidade: " << cap << " bytes\n";
    // ler string de volta
    string resultado = fluxoSaida.str();
    cout << resultado;
    return 0;
}
```

Eis uma execução de exemplo do programa da Listagem 17.21:

```
O que você gostaria de ler no rótulo do disco rígido? Meu Primeiro Disco
Quantos bytes ele tem? 240000000000
Rótulo: Meu Primeiro Disco
Capacidade: 240000000000 bytes
```

Esse programa formata a saída para um objeto de `ostringstream` da mesma forma que você usaria `cout` para formatar a saída para o monitor. O método `str()` converte o fluxo formatado para um objeto `string`. Você pode usar as ferramentas de formatação de `ostream` com `ostringstream`, o que significa que os manipuladores como `setw`, `setfill`, `fixed` e `scientific` estão disponíveis.

**`istringstream`**

A classe `istringstream` permite que você use os métodos de `istream` para ler dados de um objeto `string`. A Listagem 17.22 usa um objeto `istringstream` para ler palavras individuais de uma string.

**Listagem 17.22 — `leitura_string.cpp`**

```cpp
// leitura_string.cpp -- conversões de formato internas
#include <iostream>
#include <sstream>
#include <string>
int main()
{
    using namespace std;
    string frase = "Era uma noite escura e tempestuosa, "
                   "e o número completo dos adj presentes era de 5.";
    istringstream fluxoEntrada(frase); // gerencia um fluxo de string
    string palavra;
    while (fluxoEntrada >> palavra) // ler uma palavra por vez
    {
        string::size_type posicao = palavra.find("adj"); // procurar "adj"
        if (posicao != string::npos)
            palavra.replace(posicao, 3, "noite"); // se encontrado, substituir
        cout << palavra << " ";
    }
    cout << endl;
    return 0;
}
```

Eis a saída do programa da Listagem 17.22:

```
Era uma noite escura e tempestuosa, e o número completo dos noite presentes era de 5.
```

O objeto `istringstream` é inicializado com a string `frase`. O loop `while` usa o operador de extração de `istringstream` para ler palavras, procura e substitui "adj" por "noite" e exibe o resultado. O principal ponto é que você pode usar as ferramentas de leitura de `istream` como `>>` e `getline()` com um objeto `istringstream`.

---

## Resumo

C++ trata a E/S como fluxos de bytes. Da perspectiva do programa, um fluxo é simplesmente uma sequência de bytes. Um fluxo de entrada é uma sequência de bytes fluindo para um programa. Um fluxo de saída é uma sequência de bytes fluindo para fora de um programa. O C++ mapeia entrada e saída para fluxos. Os arquivos `iostream` e `fstream` fornecem definições de classe que suportam E/S de fluxo.

`ios_base` é uma classe base tanto para `istream` quanto para `ostream`. Ela contém informações sobre o estado do fluxo e sobre formatação. A classe `ios`, derivada de `ios_base`, incorpora um ponteiro para um objeto `streambuf`. A classe `streambuf` gerencia o buffer para um fluxo. A classe `istream` deriva de `ios` e fornece vários métodos de entrada. A classe `ostream` deriva de `ios` e fornece vários métodos de saída. A classe `iostream` deriva tanto de `istream` quanto de `ostream` e, portanto, herda métodos de entrada e de saída.

O programa inclui o arquivo de cabeçalho `iostream` para usar `cin` (o fluxo de entrada padrão), `cout` (o fluxo de saída padrão), `cerr` (o fluxo de erro padrão sem buffer) e `clog` (o fluxo de erro padrão com buffer). Alguns sistemas suportam o redirecionamento de entrada e saída na linha de comando, alterando esses fluxos do padrão de teclado e monitor para algum outro dispositivo ou arquivo.

A classe `ostream` define a função `operator<<()` para a saída de `bool`, `char`, `unsigned char`, `signed char`, `short`, `unsigned short`, `int`, `unsigned int`, `long`, `unsigned long`, `float`, `double`, `long double` e `(char *)`, além de ponteiros para `void`. A classe define o método `put()` para saída de um caractere e o método `write()` para saída de um bloco de dados binários.

A classe `ios_base` fornece muitas funções para controlar como a saída é formatada. A função `setf()` permite definir vários sinalizadores de estado de formato, e `unsetf()` permite limpá-los. A função `precision()` define o número de dígitos usados para saída de ponto flutuante, a função `width()` define a largura de um campo e a função `fill()` define o caractere de preenchimento a usar com campos largos.

O arquivo de cabeçalho `iomanip` fornece alguns manipuladores convenientes (`setw()`, `setprecision()` e `setfill()`) que você pode usar com o operador de inserção.

A classe `istream` define o método `operator>>()` para obter `bool`, `char`, `unsigned char`, `signed char`, `short`, `unsigned short`, `int`, `unsigned int`, `long`, `unsigned long`, `long long` (C++11), `unsigned long long` (C++11), `float`, `double`, `long double` e `(char *)`. A classe também fornece vários métodos de entrada: `get()` para entrada de caractere único ou de string, `getline()` para entrada de string, `ignore()` para descartar caracteres, `putback()` para reinserir um caractere no fluxo de entrada e `peek()` para examinar o próximo caractere de entrada sem consumi-lo.

Um objeto de fluxo mantém estado de fluxo em um membro herdado de `ios_base`. `eofbit` é definido quando a entrada chega ao fim de arquivo, `failbit` é definido quando uma operação de entrada falha em encontrar os caracteres esperados ou quando uma operação de saída falha em escrever os caracteres esperados e `badbit` é definido quando alguma falha não diagnosticada pode ter corrompido o fluxo. Os métodos `eof()`, `fail()`, `bad()` e `good()` retornam valores `bool` que refletem os estados dos bits correspondentes. O método `rdstate()` retorna o estado do fluxo. O método `clear()` permite redefinir o estado do fluxo, e `setstate()` permite definir bits de estado. O método `exceptions()` especifica quais condições de estado fazem o método `clear()` lançar uma exceção.

O C++ usa classes de fluxo de arquivo para E/S de arquivo. Você cria um objeto `ifstream` para entrada de arquivo e um objeto `ofstream` para saída de arquivo. Você pode criar um objeto `fstream` para entrada e saída de arquivo ao mesmo tempo. Você associa esses objetos a arquivos passando o nome do arquivo ao construtor ou usando o método `open()`. Você pode usar o método `close()` para encerrar a conexão com o arquivo. Essas classes têm construtores e métodos `open()` que recebem um segundo argumento opcional especificando o modo de arquivo. O modo de arquivo especifica aspectos como se o arquivo deve ser aberto para leitura e/ou escrita, se abrir um arquivo para saída o trunca, se uma operação de escrita sempre acrescenta ao final do arquivo e se o arquivo é binário ou texto. O C++ usa os métodos de fluxo herdados para tratar E/S formatada (usando o operador `<<` e suas variantes) e E/S não formatada (usando os métodos `read()` e `write()`). Os métodos `seekg()` e `seekp()` permitem fazer acesso aleatório a arquivos.

Você pode usar fluxos de string (`ostringstream` e `istringstream`) para formatar saída em uma string e para ler entrada formatada de uma string.

---

## Revisão do Capítulo

**1.** Qual papel o arquivo `iostream` desempenha na E/S do C++?

**2.** Por que digitar um número como `121` como entrada exige que um programa faça uma conversão?

**3.** Qual é a diferença entre a saída padrão e o erro padrão?

**4.** Por que `cout` é capaz de exibir vários tipos do C++ sem receber instruções explícitas para cada tipo?

**5.** Qual característica das definições de método de saída permite concatenar a saída?

**6.** Escreva um programa que solicite um inteiro e então o exiba nas formas decimal, octal e hexadecimal. Exiba cada forma na mesma linha, em campos de 15 caracteres de largura, e use os prefixos de base numérica do C++.

**7.** Escreva um programa que solicite as informações a seguir e as formate conforme mostrado:

```
Digite seu nome: Billy Gruff
Digite seu salário por hora: 12
Digite o número de horas trabalhadas: 7.5
Primeiro formato:
            Billy Gruff: $  12.00: 7.5
Segundo formato:
Billy Gruff       : $12.00 :7.5
```

**8.** Considere o programa a seguir:

```cpp
// rq17-8.cpp
#include <iostream>
int main()
{
    using namespace std;
    char ch;
    int ct1 = 0;
    cin >> ch;
    while (ch != 'q')
    {
        ct1++;
        cin >> ch;
    }
    int ct2 = 0;
    cin.get(ch);
    while (ch != 'q')
    {
        ct2++;
        cin.get(ch);
    }
    cout << "ct1 = " << ct1 << "; ct2 = " << ct2 << "\n";
    return 0;
}
```

O que ele imprime, dada a seguinte entrada?

```
I see a q<Enter>
I see a q<Enter>
```

Aqui `<Enter>` significa pressionar a tecla Enter.

**9.** Ambas as instruções a seguir leem e descartam caracteres até e incluindo o final de uma linha. De que forma o comportamento de uma difere do da outra?

```cpp
while (cin.get() != '\n')
    continue;
cin.ignore(80, '\n');
```

---

## Exercícios de Programação

**1.** Escreva um programa que conte o número de caracteres até o primeiro `$` na entrada e que deixe o `$` no fluxo de entrada.

**2.** Escreva um programa que copie sua entrada do teclado (até o fim de arquivo simulado) para um arquivo cujo nome é especificado na linha de comando.

**3.** Escreva um programa que copie um arquivo para outro. O programa deve receber os nomes dos arquivos da linha de comando. O programa deve relatar se não conseguir abrir um arquivo.

**4.** Escreva um programa que abra dois arquivos de texto para entrada e um para saída. O programa deve concatenar as linhas correspondentes dos arquivos de entrada, usar um espaço como separador e escrever os resultados no arquivo de saída. Se um arquivo for mais curto que o outro, as linhas restantes do arquivo mais longo também devem ser copiadas para o arquivo de saída. Por exemplo, suponha que o primeiro arquivo de entrada tenha este conteúdo:

```
ovos pipas rosquinhas
balões martelos
pedras
```

E suponha que o segundo arquivo de entrada tenha este conteúdo:

```
zero languidez
finanças drama
```

O arquivo resultante teria este conteúdo:

```
ovos pipas rosquinhas zero languidez
balões martelos finanças drama
pedras
```

**5.** Mat e Pat querem convidar seus amigos para uma festa, assim como fizeram no Exercício de Programação 8 do Capítulo 16, exceto que agora eles querem um programa que use arquivos. Eles pediram que você escrevesse um programa que faça o seguinte:

- Leia uma lista de nomes de amigos de Mat de um arquivo de texto chamado `mat.dat`, que lista um amigo por linha. Os nomes são armazenados em um container e depois exibidos em ordem classificada.
- Leia uma lista de nomes de amigos de Pat de um arquivo de texto chamado `pat.dat`, que lista um amigo por linha. Os nomes são armazenados em um container e depois exibidos em ordem classificada.
- Mescle as duas listas, elimine as duplicatas e armazene o resultado no arquivo `matnpat.dat`, um amigo por linha.

**6.** Considere as definições de classe do Exercício de Programação 5 do Capítulo 14, "Reutilizando Código em C++". Se você ainda não fez esse exercício, faça-o agora. Em seguida, faça o seguinte:

Escreva um programa que use a E/S padrão e de arquivo do C++ em conjunto com dados dos tipos `employee` (funcionário), `manager` (gerente), `fink` e `highfink`, conforme definidos no Exercício de Programação 5 do Capítulo 14. O programa deve ser nos moldes gerais da Listagem 17.17 — deve permitir que você adicione novos dados a um arquivo. Na primeira vez que o programa é executado, deve solicitar dados do usuário, exibir todas as entradas e salvar as informações em um arquivo. Em usos subsequentes, o programa deve primeiro ler e exibir os dados do arquivo, permitir ao usuário adicionar dados e exibir todos os dados. Uma diferença é que os dados devem ser tratados por um array de ponteiros para o tipo `employee`. Dessa forma, um ponteiro pode apontar para um objeto `employee` ou para objetos de qualquer um dos três tipos derivados. Mantenha o array pequeno para facilitar a verificação do programa; por exemplo, você pode limitar o array a 10 elementos:

```cpp
const int MAX = 10; // não mais que 10 objetos
// ...
employee * pc[MAX];
```

Para entrada pelo teclado, o programa deve usar um menu para oferecer ao usuário a opção de qual tipo de objeto criar. O menu deve usar um `switch` para usar `new` para criar um objeto do tipo desejado e atribuir o endereço do objeto a um ponteiro no array `pc`. Então esse objeto pode usar a função virtual `setall()` para solicitar os dados apropriados ao usuário:

```cpp
pc[i]->setall(); // invoca a função correspondente ao tipo do objeto
```

Para salvar os dados em um arquivo, crie uma função virtual `writeall()` para esse propósito:

```cpp
for (i = 0; i < indice; i++)
    pc[i]->writeall(fout); // fout é um ofstream conectado ao arquivo de saída
```

> **Nota:** Use E/S de texto, não E/S binária, para o Exercício de Programação 6. (Infelizmente, objetos virtuais incluem ponteiros para tabelas de ponteiros para funções virtuais, e `write()` copia essas informações para um arquivo. Um objeto preenchido usando `read()` do arquivo obtém valores estranhos para os ponteiros de função, o que realmente bagunça o comportamento das funções virtuais.) Use um caractere de nova linha para separar cada campo de dados do próximo; isso torna mais fácil identificar campos na entrada. Ou você ainda poderia usar E/S binária, mas não escrever objetos como um todo. Em vez disso, você poderia fornecer métodos de classe que apliquem as funções `write()` e `read()` a cada membro de classe individualmente, em vez de ao objeto como um todo. Dessa forma, o programa poderia salvar apenas os dados pretendidos em um arquivo.

A parte complicada é recuperar os dados do arquivo. O problema é: como o programa pode saber se o próximo item a ser recuperado é um objeto `employee`, um objeto `manager`, um tipo `fink` ou um tipo `highfink`? Uma abordagem é, ao escrever os dados de um objeto em um arquivo, preceder os dados com um inteiro que indica o tipo do objeto a seguir. Então, na entrada do arquivo, o programa pode ler o inteiro e então usar `switch` para criar o objeto apropriado para receber os dados:

```cpp
enum tipoclasse {Funcionario, Gerente, Fink, Highfink}; // no cabeçalho da classe
// ...
int tipoclasse;
while ((fin >> tipoclasse).get(ch)) // nova linha separa int dos dados
{
    switch(tipoclasse) {
        case Funcionario : pc[i] = new employee;
            // ...
            break;
    // ...
    }
}
```

Você pode então usar o ponteiro para invocar uma função virtual `getall()` para ler as informações:

```cpp
pc[i++]->getall();
```

**7.** Eis parte de um programa que lê entrada do teclado em um vetor de objetos `string`, armazena o conteúdo das strings (não os objetos) em um arquivo e depois copia o conteúdo do arquivo de volta em um vetor de objetos `string`:

```cpp
int main()
{
    using namespace std;
    vector<string> vostr;
    string temp;
    // ler strings
    cout << "Digite as strings (linha vazia para encerrar):\n";
    while (getline(cin, temp) && temp[0] != '\0')
        vostr.push_back(temp);
    cout << "Eis sua entrada.\n";
    for_each(vostr.begin(), vostr.end(), ExibirStr);
    // armazenar em arquivo
    ofstream fout("strings.dat", ios_base::out | ios_base::binary);
    for_each(vostr.begin(), vostr.end(), Armazenar(fout));
    fout.close();
    // recuperar conteúdo do arquivo
    vector<string> vistr;
    ifstream fin("strings.dat", ios_base::in | ios_base::binary);
    if (!fin.is_open())
    {
        cerr << "Não foi possível abrir o arquivo para leitura.\n";
        exit(EXIT_FAILURE);
    }
    ObterStrs(fin, vistr);
    cout << "\nEis as strings lidas do arquivo:\n";
    for_each(vistr.begin(), vistr.end(), ExibirStr);
    return 0;
}
```

Observe que o arquivo é aberto em formato binário e que a intenção é que a E/S seja realizada com `read()` e `write()`. Ainda há bastante a ser feito:

- Escreva uma função `void ExibirStr(const string &)` que exibe um objeto `string` seguido por um caractere de nova linha.
- Escreva um funtor `Armazenar` que escreve informações de string em um arquivo. O construtor de `Armazenar` deve especificar um objeto `ofstream`, e o `operator()(const string &)` sobrecarregado deve indicar a string a escrever. Um plano viável é primeiro escrever o tamanho da string no arquivo e então escrever o conteúdo da string. Por exemplo, se `comp` contém o tamanho da string, você poderia usar isto:

  ```cpp
  os.write((char *)&comp, sizeof(std::size_t)); // armazenar comprimento
  os.write(s.data(), comp);                     // armazenar caracteres
  ```

  O membro `data()` retorna um ponteiro para um array que contém os caracteres na string. É similar ao membro `c_str()`, exceto que o último acrescenta um caractere nulo.

- Escreva uma função `ObterStrs()` que recupera informações do arquivo. Ela pode usar `read()` para obter o tamanho de uma string e então usar um loop para ler esse número de caracteres do arquivo, acrescentando-os a uma string temporária inicialmente vazia. Como os dados de uma string são privados, você tem que usar um método de classe para inserir dados na string em vez de ler diretamente nela.

---

[Anterior](capitulo-17-02-istream-estados-arquivo.md) | [Índice](README.md) | [Próximo](capitulo-18-novo-padrao-cpp.md)
