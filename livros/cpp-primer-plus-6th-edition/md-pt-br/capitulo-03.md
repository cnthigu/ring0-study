# Capitulo 3 - Lidando com Dados (Dealing with Data)

> Traducao nao oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Texto adaptado para portugues brasileiro com fins educacionais. Paginas originais: 65-114.

> Navegacao: [Anterior](capitulo-02.md) | [Indice](README.md) | [Proximo](capitulo-04.md)

---

Neste capitulo voce vai aprender sobre:

- Regras para nomear variaveis C++
- Os tipos inteiros nativos do C++: `unsigned long`, `long`, `unsigned int`, `int`, `unsigned short`, `short`, `char`, `unsigned char`, `signed char`, `bool`
- Adicoes do C++11: `unsigned long long` e `long long`
- O arquivo `climits`, que representa os limites do sistema para varios tipos inteiros
- Literais numericos (constantes) de varios tipos inteiros
- Usando o qualificador `const` para criar constantes simbolicas
- Os tipos de ponto flutuante nativos do C++: `float`, `double` e `long double`
- O arquivo `cfloat`, que representa os limites do sistema para varios tipos de ponto flutuante
- Literais numericos de varios tipos de ponto flutuante
- Os operadores aritmeticos do C++
- Conversoes de tipo automaticas
- Conversoes de tipo forcadas (type casts)

A essencia da programacao orientada a objetos (POO) e projetar e estender seus proprios tipos de dados. Projetar seus proprios tipos de dados representa um esforco para fazer um tipo corresponder aos dados. Se voce fizer isso corretamente, descobrira que e muito mais simples trabalhar com os dados posteriormente. Mas antes que voce possa criar seus proprios tipos, deve conhecer e entender os tipos que estao embutidos no C++ porque esses tipos serao seus blocos de construcao.

Os tipos nativos do C++ vem em dois grupos: tipos fundamentais e tipos compostos. Neste capitulo voce conhecera os tipos fundamentais, que representam inteiros e numeros de ponto flutuante. Isso pode soar como apenas dois tipos; no entanto, o C++ reconhece que nenhum tipo inteiro e nenhum tipo de ponto flutuante atende a todos os requisitos de programacao, entao ele oferece varias variantes nesses dois temas de dados. O Capitulo 4, "Tipos Compostos", continua cobrindo varios tipos que sao construidos sobre os tipos basicos; esses tipos compostos adicionais incluem arrays, strings, ponteiros e estruturas.

E claro, um programa tambem precisa de um meio para identificar dados armazenados. Neste capitulo voce examinara um metodo para fazer isso — usando variaveis. Em seguida, voce vera como fazer aritmetica em C++. Por fim, voce vera como o C++ converte valores de um tipo para outro.

## Variaveis Simples

Programas tipicamente precisam armazenar informacao — talvez o preco atual de uma acao, a umidade media de uma cidade em agosto, a letra mais comum em uma constituicao e sua frequencia relativa, ou o numero de imitadores de Elvis disponiveis. Para armazenar um item de informacao em um computador, o programa deve rastrear tres propriedades fundamentais:

- Onde a informacao e armazenada
- Qual valor e mantido la
- Que tipo de informacao e armazenada

A estrategia que os exemplos neste livro usaram ate agora e declarar uma variavel. O tipo usado na declaracao descreve o tipo de informacao, e o nome da variavel representa o valor simbolicamente. Por exemplo, suponha que o Assistente-Chefe de Laboratorio Igor use as seguintes instrucoes:

```cpp
int contagemCerebros;
contagemCerebros = 5;
```

Essas instrucoes dizem ao programa que ele esta armazenando um inteiro e que o nome `contagemCerebros` representa o valor do inteiro, `5` neste caso. Em essencia, o programa localiza um pedaco de memoria grande o suficiente para conter um inteiro, anota o local e copia o valor `5` para o local. Voce pode entao usar `contagemCerebros` mais adiante no seu programa para acessar aquele local de memoria. Essas instrucoes nao dizem a voce (ou a Igor) onde na memoria o valor e armazenado, mas o programa rastreia essa informacao tambem. Na verdade, voce pode usar o operador `&` para recuperar o endereco de `contagemCerebros` na memoria. Voce aprendera sobre esse operador no proximo capitulo, quando investigar uma segunda estrategia para identificar dados — usando ponteiros.

### Nomes para Variaveis

O C++ encoraja voce a usar nomes significativos para variaveis. Se uma variavel representa o custo de uma viagem, voce deve chama-la de `custo_da_viagem` ou `custoDaViagem`, nao apenas de `x` ou `cdt`. Voce tem que seguir algumas regras simples de nomenclatura do C++:

- Os unicos caracteres que voce pode usar em nomes sao caracteres alfabeticos, digitos numericos e o caractere de sublinhado (`_`).
- O primeiro caractere em um nome nao pode ser um digito numerico.
- Caracteres maiusculos sao considerados distintos de caracteres minusculos.
- Voce nao pode usar uma palavra-chave do C++ como nome.
- Nomes que comecam com dois caracteres de sublinhado ou com um caractere de sublinhado seguido de uma letra maiuscula sao reservados para uso pela implementacao — isto e, pelo compilador e pelos recursos que ele usa. Nomes que comecam com um unico caractere de sublinhado sao reservados para uso como identificadores globais pela implementacao.
- O C++ nao impos limites no comprimento de um nome, e todos os caracteres em um nome sao significativos. No entanto, algumas plataformas podem ter seus proprios limites de comprimento.

O penultimo ponto e um pouco diferente dos pontos anteriores porque usar um nome como `__hora_parada` ou `_Rosquinha` nao produz um erro de compilador; em vez disso, leva a um comportamento indefinido. Em outras palavras, nao ha como dizer qual sera o resultado. A razao pela qual nao ha erro de compilador e que os nomes nao sao ilegais, mas sim reservados para a implementacao usar.

O ponto final diferencia o C++ do ANSI C (C99), que garante apenas que os primeiros 63 caracteres em um nome sao significativos.

Aqui estao alguns nomes validos e invalidos em C++:

```cpp
int poodle;   // valido
int Poodle;   // valido e distinto de poodle
int POODLE;   // valido e ainda mais distinto
Int terrier;  // INVALIDO -- tem que ser int, nao Int
int minha_variavel3;  // valido
int _Minha_variavel;  // valido mas reservado -- comeca com sublinhado
int 4ever;    // INVALIDO porque comeca com um digito
int double;   // INVALIDO -- double e palavra-chave do C++
int begin;    // valido -- begin e palavra-chave do Pascal
int __tolos;  // valido mas reservado -- comeca com dois sublinhados
int a_variavel_muito_boa_versao_112; // valido
int honky-tonk; // INVALIDO -- hifens nao sao permitidos
```

Se voce quiser formar um nome com duas ou mais palavras, a pratica usual e separar as palavras com um caractere de sublinhado, como em `minhas_cebolas`, ou capitalizar o caractere inicial de cada palavra apos a primeira, como em `meuDenteDeElefante`. Ambas as formas facilitam ver as palavras individuais e distinguir entre, digamos, `gotaDeCarro` e `cartaDeGota`, ou `esporte_de_barco` e `barcos_de_esporte`.

> **Esquemas de Nomenclatura:**
> Esquemas para nomear variaveis, como esquemas para nomear funcoes, fornecem terreno fertil para discussoes acaloradas. Na verdade, esse topico produz alguns dos desentendimentos mais fervorosos na programacao. Novamente, como com nomes de funcoes, o compilador C++ nao se importa com seus nomes de variaveis, desde que satisfacam as regras, mas uma convencao de nomenclatura pessoal consistente e precisa lhe servira bem.

## Tipos Inteiros

Inteiros sao numeros sem parte fracionaria, como 2, 98, -5286 e 0. Ha muitos inteiros, supondo que voce considere um numero infinito como sendo muito, entao nenhuma quantidade finita de memoria de computador pode representar todos os inteiros possiveis. Assim, uma linguagem pode representar apenas um subconjunto de todos os inteiros. Algumas linguagens oferecem apenas um tipo inteiro (um tipo para tudo!), mas o C++ fornece varias opcoes. Isso lhe da a opcao de escolher o tipo inteiro que melhor atende aos requisitos especificos de um programa.

Os varios tipos inteiros do C++ diferem na quantidade de memoria que usam para conter um inteiro. Um bloco de memoria maior pode representar um intervalo maior de valores inteiros. Alem disso, alguns tipos (tipos com sinal) podem representar valores positivos e negativos, enquanto outros (tipos sem sinal) nao podem representar valores negativos. O termo usual para descrever a quantidade de memoria usada para um inteiro e *largura* (width). Quanto mais memoria um valor usar, mais largo ele sera. Os tipos inteiros basicos do C++, em ordem de largura crescente, sao `char`, `short`, `int`, `long` e, com C++11, `long long`. Cada um vem em versoes com sinal (signed) e sem sinal (unsigned). Isso lhe da uma opcao de dez tipos inteiros diferentes! Vamos examinar esses tipos inteiros em mais detalhes. Como o tipo `char` tem algumas propriedades especiais (e mais frequentemente usado para representar caracteres em vez de numeros), este capitulo cobre os outros tipos primeiro.

### Os Tipos Inteiros short, int, long e long long

A memoria do computador consiste em unidades chamadas bits. Ao usar diferentes numeros de bits para armazenar valores, os tipos `short`, `int`, `long` e `long long` do C++ podem representar ate quatro larguras inteiras diferentes. Seria conveniente se cada tipo fosse sempre um tamanho especifico para todos os sistemas — por exemplo, se `short` fosse sempre 16 bits, `int` fosse sempre 32 bits e assim por diante. Mas a vida nao e tao simples.

Nenhuma escolha e adequada para todos os designs de computador. O C++ oferece um padrao flexivel com alguns tamanhos minimos garantidos, que ele toma do C. Aqui esta o que voce obtem:

- Um inteiro `short` tem pelo menos 16 bits de largura.
- Um inteiro `int` e pelo menos tao grande quanto `short`.
- Um inteiro `long` tem pelo menos 32 bits de largura e e pelo menos tao grande quanto `int`.
- Um inteiro `long long` tem pelo menos 64 bits de largura e e pelo menos tao grande quanto `long`.

> **Bits e Bytes:**
> A unidade fundamental de memoria de computador e o bit. Pense em um bit como uma chave eletronica que voce pode definir para desligada ou ligada. Desligado representa o valor 0, e ligado representa o valor 1. Um pedaco de memoria de 8 bits pode ser definido para 256 combinacoes diferentes. O numero 256 vem do fato de que cada bit tem duas configuracoes possiveis, fazendo o numero total de combinacoes para 8 bits ser 2x2x2x2x2x2x2x2, ou 256. Assim, uma unidade de 8 bits pode representar, digamos, os valores 0 a 255 ou os valores -128 a 127. Cada bit adicional dobra o numero de combinacoes. Isso significa que voce pode definir uma unidade de 16 bits para 65.536 valores diferentes, uma unidade de 32 bits para 4.294.967.296 valores diferentes e uma unidade de 64 bits para 18.446.744.073.709.551.616 valores diferentes.
>
> Um *byte* geralmente significa uma unidade de 8 bits de memoria. Byte neste sentido e a unidade de medicao que descreve a quantidade de memoria em um computador, com um kilobyte igual a 1.024 bytes e um megabyte igual a 1.024 kilobytes. No entanto, o C++ define byte de forma diferente. O byte do C++ consiste em pelo menos bits adjacentes suficientes para acomodar o conjunto de caracteres basico para a implementacao. Isso significa que o numero de valores possiveis deve igualar ou exceder o numero de caracteres distintos. Nos Estados Unidos, os conjuntos de caracteres basicos sao geralmente os conjuntos ASCII e EBCDIC, cada um dos quais pode ser acomodado por 8 bits, portanto o byte do C++ e tipicamente 8 bits em sistemas que usam esses conjuntos de caracteres. No entanto, a programacao internacional pode exigir conjuntos de caracteres muito maiores, como Unicode, entao algumas implementacoes podem usar um byte de 16 bits ou mesmo um byte de 32 bits. Alguns usam o termo *octeto* para denotar um byte de 8 bits.

Muitos sistemas atualmente usam a garantia minima, fazendo `short` ter 16 bits e `long` ter 32 bits. Isso ainda deixa varias opcoes abertas para `int`. Ele pode ter 16, 24 ou 32 bits de largura e atender ao padrao. Poderia ate ter 64 bits, desde que `long` e `long long` sejam pelo menos tao largos. Tipicamente, `int` e 16 bits (o mesmo que `short`) para implementacoes mais antigas de IBM PC e 32 bits (o mesmo que `long`) para Windows XP, Windows Vista, Windows 7, macOS, VAX e muitas outras implementacoes de minicomputador.

Voce usa esses nomes de tipo para declarar variaveis assim como usaria `int`:

```cpp
short pontuacao;      // cria uma variavel inteira do tipo short
int temperatura;      // cria uma variavel inteira do tipo int
long posicao;         // cria uma variavel inteira do tipo long
```

Na verdade, `short` e abreviacao de `short int` e `long` e abreviacao de `long int`, mas quase ninguem usa as formas mais longas.

Os quatro tipos — `int`, `short`, `long` e `long long` — sao tipos com sinal (signed types), o que significa que cada um divide seu intervalo aproximadamente igualmente entre valores positivos e negativos. Por exemplo, um `int` de 16 bits pode ir de -32.768 a +32.767.

Se voce quiser saber como os inteiros do seu sistema se comparam em tamanho, pode usar ferramentas C++ para investigar tamanhos de tipos com um programa. Primeiro, o operador `sizeof` retorna o tamanho, em bytes, de um tipo ou de uma variavel. Segundo, o arquivo de cabecalho `climits` (ou, para implementacoes mais antigas, o arquivo de cabecalho `limits.h`) contem informacoes sobre os limites de tipos inteiros. Em particular, ele define nomes simbolicos para representar diferentes limites. Por exemplo, ele define `INT_MAX` como o maior valor `int` possivel e `CHAR_BIT` como o numero de bits em um byte. O Listagem 3.1 demonstra como usar essas facilidades. O programa tambem ilustra a *inicializacao* (initialization), que e o uso de uma instrucao de declaracao para atribuir um valor a uma variavel.

**Listagem 3.1 — limites.cpp**

```cpp
// limites.cpp -- alguns limites de inteiros
#include <iostream>
#include <climits> // use limits.h para sistemas mais antigos
int main()
{
    using namespace std;
    int n_int = INT_MAX;        // inicializa n_int com o valor maximo de int
    short n_short = SHRT_MAX;   // simbolos definidos no arquivo climits
    long n_long = LONG_MAX;
    long long n_llong = LLONG_MAX;
    // operador sizeof retorna o tamanho do tipo ou da variavel
    cout << "int tem " << sizeof (int) << " bytes." << endl;
    cout << "short tem " << sizeof n_short << " bytes." << endl;
    cout << "long tem " << sizeof n_long << " bytes." << endl;
    cout << "long long tem " << sizeof n_llong << " bytes." << endl;
    cout << endl;
    cout << "Valores maximos:" << endl;
    cout << "int: " << n_int << endl;
    cout << "short: " << n_short << endl;
    cout << "long: " << n_long << endl;
    cout << "long long: " << n_llong << endl << endl;
    cout << "Valor minimo de int = " << INT_MIN << endl;
    cout << "Bits por byte = " << CHAR_BIT << endl;
    return 0;
}
```

> **Nota:** Se o seu sistema nao suportar o tipo `long long`, voce deve remover as linhas que usam esse tipo.

Aqui esta uma saida de exemplo do programa no Listagem 3.1:

```
int tem 4 bytes.
short tem 2 bytes.
long tem 4 bytes.
long long tem 8 bytes.

Valores maximos:
int: 2147483647
short: 32767
long: 2147483647
long long: 9223372036854775807
Valor minimo de int = -2147483648
Bits por byte = 8
```

Esses valores particulares vieram de um sistema rodando o Windows 7 de 64 bits.

### O Operador sizeof e o Arquivo de Cabecalho climits

O operador `sizeof` relata que `int` tem 4 bytes no sistema base, que usa um byte de 8 bits. Voce pode aplicar o operador `sizeof` a um nome de tipo ou a um nome de variavel. Quando voce usa o operador `sizeof` com um nome de tipo, como `int`, voce coloca o nome entre parenteses. Mas quando voce usa o operador com o nome de uma variavel, como `n_short`, os parenteses sao opcionais:

```cpp
cout << "int tem " << sizeof (int) << " bytes.\n";
cout << "short tem " << sizeof n_short << " bytes.\n";
```

O arquivo de cabecalho `climits` define constantes simbolicas (veja o quadro "Constantes Simbolicas pela Via do Preprocessador", mais adiante neste capitulo) para representar os limites de tipo. Como mencionado anteriormente, `INT_MAX` representa o maior valor que o tipo `int` pode conter; isso resultou em 2.147.483.647 para nosso sistema Windows 7. O fabricante do compilador fornece um arquivo `climits` que reflete os valores apropriados para aquele compilador. A Tabela 3.1 resume as constantes simbolicas definidas no arquivo `climits`.

**Tabela 3.1 — Constantes Simbolicas de climits**

| Constante Simbolica | Representa                             |
|---------------------|----------------------------------------|
| `CHAR_BIT`          | Numero de bits em um `char`            |
| `CHAR_MAX`          | Valor maximo de `char`                 |
| `CHAR_MIN`          | Valor minimo de `char`                 |
| `SCHAR_MAX`         | Valor maximo de `signed char`          |
| `SCHAR_MIN`         | Valor minimo de `signed char`          |
| `UCHAR_MAX`         | Valor maximo de `unsigned char`        |
| `SHRT_MAX`          | Valor maximo de `short`                |
| `SHRT_MIN`          | Valor minimo de `short`                |
| `USHRT_MAX`         | Valor maximo de `unsigned short`       |
| `INT_MAX`           | Valor maximo de `int`                  |
| `INT_MIN`           | Valor minimo de `int`                  |
| `UINT_MAX`          | Valor maximo de `unsigned int`         |
| `LONG_MAX`          | Valor maximo de `long`                 |
| `LONG_MIN`          | Valor minimo de `long`                 |
| `ULONG_MAX`         | Valor maximo de `unsigned long`        |
| `LLONG_MAX`         | Valor maximo de `long long`            |
| `LLONG_MIN`         | Valor minimo de `long long`            |
| `ULLONG_MAX`        | Valor maximo de `unsigned long long`   |

> **Constantes Simbolicas pela Via do Preprocessador:**
> O arquivo `climits` contem linhas semelhantes ao seguinte:
> ```cpp
> #define INT_MAX 32767
> ```
> O processo de compilacao do C++ primeiro passa o codigo-fonte por um preprocessador. Aqui `#define`, como `#include`, e uma diretiva do preprocessador. O que essa diretiva especifica ao preprocessador e: procure no programa instancias de `INT_MAX` e substitua cada ocorrencia por `32767`. Assim, a diretiva `#define` funciona como um comando global de busca e substituicao em um editor de texto. O programa alterado e compilado apos essas substituicoes ocorrerem. O preprocessador procura tokens independentes (palavras separadas) e pula palavras embutidas. Voce pode usar `#define` para definir suas proprias constantes simbolicas tambem. No entanto, a diretiva `#define` e uma reliquia do C. O C++ tem uma forma melhor de criar constantes simbolicas (usando a palavra-chave `const`, discutida em uma secao posterior), entao voce nao usara `#define` muito. Mas alguns arquivos de cabecalho, particularmente aqueles projetados para serem usados com C e C++, usam-no.

### Inicializacao

A *inicializacao* (initialization) combina atribuicao com declaracao. Por exemplo, a instrucao a seguir declara a variavel `n_int` e a define com o maior valor possivel do tipo `int`:

```cpp
int n_int = INT_MAX;
```

Voce tambem pode usar constantes literais, como `255`, para inicializar valores. Voce pode inicializar uma variavel com outra variavel, desde que a outra variavel tenha sido definida primeiro. Voce pode ate inicializar uma variavel com uma expressao, desde que todos os valores na expressao sejam conhecidos quando a execucao do programa alcancar a declaracao:

```cpp
int tios = 5;       // inicializa tios com 5
int tias = tios;    // inicializa tias com 5
int cadeiras = tias + tios + 4; // inicializa cadeiras com 14
```

A sintaxe de inicializacao mostrada anteriormente vem do C; o C++ tem uma sintaxe de inicializacao que nao e compartilhada com o C:

```cpp
int corujas = 101; // inicializacao C tradicional, define corujas como 101
int tentilhoes(432); // sintaxe C++ alternativa, define tentilhoes como 432
```

> **Cuidado:** Se voce nao inicializar uma variavel que e definida dentro de uma funcao, o valor da variavel sera indeterminado. Isso significa que o valor e qualquer coisa que estava naquele local de memoria antes da criacao da variavel.

Se voce sabe qual deve ser o valor inicial de uma variavel, inicialize-a. E verdade que separar a declaracao de uma variavel de atribuir-lhe um valor pode criar suspense momentaneo, mas inicializar a variavel quando voce a declara protege voce de esquecer de atribuir o valor posteriormente.

### Inicializacao com C++11

Ha outro formato de inicializacao que e usado com arrays e estruturas, mas no C++98 tambem pode ser usado com variaveis de valor unico:

```cpp
int hambuergueres = {24}; // define hambuergueres como 24
```

Usar um inicializador entre chaves para uma variavel de valor unico nao era particularmente comum, mas o padrao C++11 o esta expandindo de varias maneiras. Primeiro, pode ser usado com ou sem o sinal `=`:

```cpp
int emus{7};     // define emus como 7
int rheas = {12}; // define rheas como 12
```

Segundo, as chaves podem ser deixadas vazias, caso em que a variavel e inicializada com 0:

```cpp
int rocs = {}; // define rocs como 0
int psiquicos{}; // define psiquicos como 0
```

Terceiro, ele fornece melhor protecao contra erros de conversao de tipo, um topico ao qual retornaremos perto do final deste capitulo.

O C++ adicionou a forma de parenteses da inicializacao para tornar a inicializacao de variaveis ordinarias mais parecida com a inicializacao de variaveis de classe. O C++11 torna possivel usar a sintaxe de chaves (com ou sem o `=`) com todos os tipos — uma sintaxe de inicializacao universal.

### Tipos sem Sinal (Unsigned)

Cada um dos quatro tipos inteiros que voce acabou de conhecer vem em uma variedade sem sinal (unsigned) que nao pode conter valores negativos. Isso tem a vantagem de aumentar o maior valor que a variavel pode conter. Por exemplo, se `short` representa o intervalo de -32.768 a +32.767, a versao sem sinal pode representar o intervalo de 0 a 65.535. E claro, voce deve usar tipos sem sinal apenas para quantidades que nunca sao negativas, como populacoes, contagens de feijoes e manifestacoes de carinhas felizes. Para criar versoes sem sinal dos tipos inteiros basicos, voce usa a palavra-chave `unsigned` para modificar as declaracoes:

```cpp
unsigned short troco;        // tipo unsigned short
unsigned int rovert;         // tipo unsigned int
unsigned zagueiro;           // tambem unsigned int
unsigned long desapareceu;   // tipo unsigned long
unsigned long long lang_lang; // tipo unsigned long long
```

Observe que `unsigned` por si so e abreviacao de `unsigned int`.

O Listagem 3.2 ilustra o uso de tipos sem sinal. Ele tambem mostra o que pode acontecer se seu programa tentar ultrapassar os limites para tipos inteiros. Por fim, ele fornece uma ultima olhada na instrucao `#define` do preprocessador.

**Listagem 3.2 — exceder.cpp**

```cpp
// exceder.cpp -- excedendo alguns limites de inteiros
#include <iostream>
#define ZERO 0        // define o simbolo ZERO com valor 0
#include <climits>    // define INT_MAX como o maior valor int
int main()
{
    using namespace std;
    short sam = SHRT_MAX;     // inicializa variavel com o valor maximo
    unsigned short sue = sam; // ok se a variavel sam ja esta definida
    cout << "Sam tem " << sam << " dolares e Sue tem " << sue;
    cout << " dolares depositados." << endl
         << "Adicione R$1 a cada conta." << endl << "Agora ";
    sam = sam + 1;
    sue = sue + 1;
    cout << "Sam tem " << sam << " dolares e Sue tem " << sue;
    cout << " dolares depositados.\nPobre Sam!" << endl;
    sam = ZERO;
    sue = ZERO;
    cout << "Sam tem " << sam << " dolares e Sue tem " << sue;
    cout << " dolares depositados." << endl;
    cout << "Retire R$1 de cada conta." << endl << "Agora ";
    sam = sam - 1;
    sue = sue - 1;
    cout << "Sam tem " << sam << " dolares e Sue tem " << sue;
    cout << " dolares depositados." << endl << "Sortuda Sue!" << endl;
    return 0;
}
```

Aqui esta a saida do programa no Listagem 3.2:

```
Sam tem 32767 dolares e Sue tem 32767 dolares depositados.
Adicione R$1 a cada conta.
Agora Sam tem -32768 dolares e Sue tem 32768 dolares depositados.
Pobre Sam!
Sam tem 0 dolares e Sue tem 0 dolares depositados.
Retire R$1 de cada conta.
Agora Sam tem -1 dolares e Sue tem 65535 dolares depositados.
Sortuda Sue!
```

O programa define uma variavel `short` (`sam`) e uma variavel `unsigned short` (`sue`) para o maior valor `short`, que e 32.767 no nosso sistema. Em seguida, adiciona 1 a cada valor. Isso nao causa problemas para `sue` porque o novo valor ainda e muito menor que o valor maximo para um inteiro sem sinal. Mas `sam` vai de 32.767 para -32.768! Da mesma forma, subtrair 1 de 0 nao cria problemas para `sam`, mas faz a variavel sem sinal `sue` ir de 0 para 65.535. Como voce pode ver, esses inteiros se comportam muito como um odometro. Se voce ultrapassar o limite, os valores simplesmente recomecarao no outro extremo do intervalo (veja a Figura 3.1). O C++ garante que os tipos sem sinal se comportam dessa maneira. No entanto, o C++ nao garante que os tipos inteiros com sinal possam exceder seus limites (overflow e underflow) sem reclamacao, mas esse e o comportamento mais comum nas implementacoes atuais.

### Escolhendo um Tipo Inteiro

Com a riqueza dos tipos inteiros do C++, qual voce deve usar? Geralmente, `int` e definido como o tamanho inteiro mais "natural" para o computador alvo. Tamanho natural refere-se a forma de inteiro que o computador lida com mais eficiencia. Se nao houver razao convincente para escolher outro tipo, voce deve usar `int`.

Agora veja as razoes pelas quais voce pode usar outro tipo. Se uma variavel representa algo que nunca e negativo, como o numero de palavras em um documento, voce pode usar um tipo sem sinal; dessa forma, a variavel pode representar valores maiores.

Se voce sabe que a variavel pode ter que representar valores inteiros muito grandes para um inteiro de 16 bits, voce deve usar `long`. Isso e verdade mesmo que `int` seja 32 bits no seu sistema. Dessa forma, se voce transferir seu programa para um sistema com `int` de 16 bits, seu programa nao o embaracara falhando subitamente. E se um simples dois bilhoes for inadequado para suas necessidades, voce pode subir para `long long`.

Usar `short` pode conservar memoria se `short` for menor que `int`. Isso e mais importante apenas se voce tiver um grande array de inteiros. Se for importante conservar espaco, voce deve usar `short` em vez de `int`, mesmo que os dois sejam do mesmo tamanho.

### Literais Inteiros

Um literal inteiro, ou constante, e aquele que voce escreve explicitamente, como `212` ou `1776`. O C++, como o C, permite escrever inteiros em tres bases numericas diferentes: base 10 (o favorito do publico), base 8 (o antigo favorito Unix) e base 16 (o favorito do hacker de hardware). O Apendice A, "Bases Numericas", descreve essas bases; aqui veremos as representacoes do C++. O C++ usa o primeiro digito ou dois para identificar a base de uma constante numerica. Se o primeiro digito esta no intervalo 1-9, o numero e base 10 (decimal); assim, `93` e base 10. Se o primeiro digito e `0` e o segundo digito esta no intervalo 1-7, o numero e base 8 (octal); assim, `042` e octal e igual a 34 decimal. Se os dois primeiros caracteres sao `0x` ou `0X`, o numero e base 16 (hexadecimal); assim, `0x42` e hex e igual a 66 decimal. Para valores hexadecimais, os caracteres a-f e A-F representam os digitos hexadecimais correspondentes aos valores 10-15. `0xF` e 15 e `0xA5` e 165 (10 dezesseis mais 5 uns). O Listagem 3.3 e feito para mostrar as tres bases.

**Listagem 3.3 — hexoct1.cpp**

```cpp
// hexoct1.cpp -- mostra literais hexadecimais e octais
#include <iostream>
int main()
{
    using namespace std;
    int peito = 42;        // literal inteiro decimal
    int cintura = 0x42;    // literal inteiro hexadecimal
    int costura = 042;     // literal inteiro octal
    cout << "O Monsieur tem uma figura marcante!\n";
    cout << "peito = " << peito << " (42 em decimal)\n";
    cout << "cintura = " << cintura << " (0x42 em hex)\n";
    cout << "costura = " << costura << " (042 em octal)\n";
    return 0;
}
```

Por padrao, `cout` exibe inteiros em forma decimal, independente de como eles sao escritos em um programa, como a saida a seguir mostra:

```
O Monsieur tem uma figura marcante!
peito = 42 (42 em decimal)
cintura = 66 (0x42 em hex)
costura = 34 (042 em octal)
```

Tenha em mente que essas notacoes sao meramente conveniencias de notacao. Por exemplo, se voce pertence a um clube de PC vintage e le que a memoria de video CGA esta em B000 em hexadecimal, voce nao precisa converter o valor para base 10 45.056 antes de usa-lo em seu programa. Em vez disso, voce pode simplesmente usar `0xB000`. Mas se voce escreve o valor dez como `10`, `012` ou `0xA`, ele e armazenado da mesma forma no computador — como um valor binario (base 2).

A proposito, se voce quiser exibir um valor em forma hexadecimal ou octal, voce pode usar alguns recursos especiais de `cout`. O arquivo de cabecalho `iostream` fornece os manipuladores `dec`, `hex` e `oct` para dar a `cout` as mensagens de exibir inteiros nos formatos decimal, hexadecimal e octal, respectivamente. O Listagem 3.4 usa `hex` e `oct` para exibir o valor decimal 42 em tres formatos.

**Listagem 3.4 — hexoct2.cpp**

```cpp
// hexoct2.cpp -- exibe valores em hex e octal
#include <iostream>
int main()
{
    using namespace std;
    int peito = 42;
    int cintura = 42;
    int costura = 42;
    cout << "O Monsieur tem uma figura marcante!" << endl;
    cout << "peito = " << peito << " (decimal para 42)" << endl;
    cout << hex;     // manipulador para alterar a base numerica
    cout << "cintura = " << cintura << " (hexadecimal para 42)" << endl;
    cout << oct;     // manipulador para alterar a base numerica
    cout << "costura = " << costura << " (octal para 42)" << endl;
    return 0;
}
```

Aqui esta a saida do programa do Listagem 3.4:

```
O Monsieur tem uma figura marcante!
peito = 42 (decimal para 42)
cintura = 2a (hexadecimal para 42)
costura = 52 (octal para 42)
```

Observe que o codigo como o seguinte nao exibe nada na tela:

```cpp
cout << hex;
```

Em vez disso, ele altera a forma como `cout` exibe inteiros. Assim, o manipulador `hex` e realmente uma mensagem a `cout` que diz como se comportar. Observe tambem que, como o identificador `hex` e parte do namespace `std`, e o programa usa esse namespace, este programa nao pode usar `hex` como nome de uma variavel. No entanto, se voce omitir a diretiva `using` e em vez disso usar `std::cout`, `std::endl`, `std::hex` e `std::oct`, voce poderia ainda usar o simples `hex` como nome de uma variavel.

### Como o C++ Decide Qual Tipo uma Constante E

As declaracoes de um programa dizem ao compilador C++ o tipo de uma variavel inteira especifica. Mas e as constantes? Ou seja, suponha que voce represente um numero com uma constante em um programa:

```cpp
cout << "Ano = " << 1492 << "\n";
```

O programa armazena `1492` como um `int`, um `long` ou algum outro tipo inteiro? A resposta e que o C++ armazena constantes inteiras como tipo `int`, a menos que haja razao para nao faze-lo. Duas dessas razoes sao se voce usar um sufixo especial para indicar um tipo especifico ou se um valor for grande demais para ser um `int`.

Primeiro, veja os sufixos. Eles sao letras colocadas no final de uma constante numerica para indicar o tipo. Um sufixo `l` ou `L` em um inteiro significa que o inteiro e uma constante do tipo `long`, um sufixo `u` ou `U` indica uma constante `unsigned int`, e `ul` (em qualquer combinacao de ordens e maiusculas e minusculas) indica um tipo `unsigned long`. (Como um `l` minusculo pode parecer muito com o digito 1, voce deve usar o `L` maiusculo para sufixos.) O C++11 fornece os sufixos `ll` e `LL` para o tipo `long long`, e `ull`, `Ull`, `uLL` e `ULL` para `unsigned long long`.

Em seguida, veja o tamanho. O C++ tem regras ligeiramente diferentes para inteiros decimais do que para inteiros hexadecimais e octais. Um inteiro decimal sem sufixo e representado pelo menor dos seguintes tipos que pode conter: `int`, `long` ou `long long`. Um inteiro hexadecimal ou octal sem sufixo e representado pelo menor dos seguintes tipos que pode conter: `int`, `unsigned int`, `long`, `unsigned long`, `long long` ou `unsigned long long`.

### O Tipo char: Caracteres e Inteiros Pequenos

E hora de passar ao tipo inteiro final: `char`. Como voce provavelmente suspeitou a partir do nome, o tipo `char` e projetado para armazenar caracteres, como letras e digitos numericos. Linguagens de programacao tomam o caminho mais facil usando codigos numericos para letras. Assim, o tipo `char` e outro tipo inteiro. E garantido que seja grande o suficiente para representar toda a gama de simbolos basicos — todas as letras, digitos, pontuacao e similares — para o sistema de computador alvo. Na pratica, muitos sistemas suportam menos de 128 tipos de caracteres, entao um unico byte pode representar toda a faixa. Portanto, embora `char` seja usado com mais frequencia para lidar com caracteres, voce tambem pode usa-lo como um tipo inteiro que e tipicamente menor que `short`.

O conjunto de simbolos mais comum nos Estados Unidos e o conjunto de caracteres ASCII, descrito no Apendice C, "O Conjunto de Caracteres ASCII". Um codigo numerico (o codigo ASCII) representa cada caractere no conjunto. Por exemplo, 65 e o codigo para o caractere A, e 77 e o codigo para o caractere M. Uma implementacao C++ usa qualquer codigo nativo ao seu sistema hospedeiro — por exemplo, EBCDIC em um mainframe IBM. O C++ suporta um tipo de caractere largo que pode conter um intervalo maior de valores, como os usados pelo conjunto de caracteres internacional Unicode. Voce aprendera sobre esse tipo `wchar_t` mais adiante neste capitulo.

Experimente o tipo `char` no Listagem 3.5.

**Listagem 3.5 — tipochar.cpp**

```cpp
// tipochar.cpp -- o tipo char
#include <iostream>
int main( )
{
    using namespace std;
    char c;     // declara uma variavel char
    cout << "Digite um caractere: " << endl;
    cin >> c;
    cout << "Ola! ";
    cout << "Obrigado pelo caractere " << c << "." << endl;
    return 0;
}
```

Aqui esta a saida do programa no Listagem 3.5:

```
Digite um caractere:
M
Ola! Obrigado pelo caractere M.
```

A coisa interessante e que voce digita um M, nao o codigo de caractere correspondente, 77. Alem disso, o programa imprime um M, nao 77. A magica, por assim dizer, nao esta no tipo `char`, mas em `cin` e `cout`. Essas facilidades dignas fazem conversoes em seu nome. Na entrada, `cin` converte a entrada de tecla M para o valor 77. Na saida, `cout` converte o valor 77 para o caractere exibido M; `cin` e `cout` sao guiados pelo tipo de variavel. Se voce colocar o mesmo valor 77 em uma variavel `int`, `cout` o exibira como 77. O Listagem 3.6 ilustra esse ponto. Ele tambem mostra como escrever um literal de caractere em C++: coloque o caractere entre duas aspas simples, como em `'M'`. Por fim, o programa apresenta um recurso de `cout`, a funcao `cout.put()`, que exibe um unico caractere.

**Listagem 3.6 — maischar.cpp**

```cpp
// maischar.cpp -- o tipo char e o tipo int em contraste
#include <iostream>
int main()
{
    using namespace std;
    char c = 'M';      // atribui o codigo ASCII de M a c
    int i = c;         // armazena o mesmo codigo em um int
    cout << "O codigo ASCII para " << c << " e " << i << endl;
    cout << "Adicione um ao codigo do caractere:" << endl;
    c = c + 1;         // altera o codigo do caractere em c
    i = c;             // salva o novo codigo do caractere em i
    cout << "O codigo ASCII para " << c << " e " << i << endl;
    // usando a funcao membro cout.put() para exibir um char
    cout << "Exibindo char c usando cout.put(c): ";
    cout.put(c);
    // usando cout.put() para exibir uma constante char
    cout.put('!');
    cout << endl << "Pronto" << endl;
    return 0;
}
```

Aqui esta a saida do programa no Listagem 3.6:

```
O codigo ASCII para M e 77
Adicione um ao codigo do caractere:
O codigo ASCII para N e 78
Exibindo char c usando cout.put(c): N!
Pronto
```

#### Notas do Programa

No programa no Listagem 3.6, a notacao `'M'` representa o codigo numerico para o caractere M, portanto inicializar a variavel `char` `c` com `'M'` define `c` para o valor 77. O programa entao atribui o valor identico a variavel `int` `i`, portanto tanto `c` quanto `i` tem o valor 77. Em seguida, `cout` exibe `c` como M e `i` como 77. Como declarado anteriormente, um tipo de valor guia `cout` ao escolher como exibir esse valor — apenas mais um exemplo de objetos inteligentes.

Como `c` e realmente um inteiro, voce pode aplicar operacoes de inteiro a ele, como adicionar 1. Isso muda o valor de `c` para 78. O programa entao redefine `i` para o novo valor. Novamente, `cout` exibe a versao `char` desse valor como um caractere e a versao `int` como um numero.

O fato de que o C++ representa caracteres como inteiros e uma conveniencia genuina que facilita a manipulacao de valores de caracteres. Voce nao precisa usar funcoes de conversao estranhas para converter caracteres para ASCII e vice-versa.

### Uma Funcao Membro: cout.put()

O que e `cout.put()`, e por que ele tem um ponto em seu nome? A funcao `cout.put()` e seu primeiro exemplo de um conceito OOP C++ importante, a *funcao membro* (member function). Lembre-se de que uma classe define como representar dados e como manipula-los. Uma funcao membro pertence a uma classe e descreve um metodo para manipular dados de classe. A classe `ostream`, por exemplo, tem uma funcao membro `put()` projetada para exibir caracteres. Voce pode usar uma funcao membro apenas com um objeto especifico daquela classe, como o objeto `cout`, neste caso. Para usar uma funcao membro de classe com um objeto como `cout`, voce usa um ponto para combinar o nome do objeto (`cout`) com o nome da funcao (`put()`). O ponto e chamado de *operador de associacao* (membership operator). A notacao `cout.put()` significa usar a funcao membro de classe `put()` com o objeto de classe `cout`.

### Literais char

Voce tem varias opcoes para escrever literais de caractere em C++. A escolha mais simples para caracteres comuns, como letras, pontuacao e digitos, e colocar o caractere entre aspas simples. Essa notacao representa o codigo numerico para o caractere. Por exemplo, em um sistema ASCII tem-se as seguintes correspondencias:

- `'A'` e 65, o codigo ASCII para A.
- `'a'` e 97, o codigo ASCII para a.
- `'5'` e 53, o codigo ASCII para o digito 5.
- `' '` e 32, o codigo ASCII para o caractere de espaco.
- `'!'` e 33, o codigo ASCII para o ponto de exclamacao.

Ha alguns caracteres que voce nao pode inserir em um programa diretamente do teclado. O C++ tem notacoes especiais, chamadas de *sequencias de escape* (escape sequences), para varios desses caracteres, como mostrado na Tabela 3.2.

**Tabela 3.2 — Codigos de Sequencia de Escape do C++**

| Nome do Caractere   | Simbolo ASCII | Codigo C++ | Codigo ASCII Decimal | Codigo ASCII Hex |
|---------------------|---------------|------------|----------------------|-----------------|
| Nova linha          | NL (LF)       | `\n`       | 10                   | 0xA             |
| Tabulacao horizontal| HT            | `\t`       | 9                    | 0x9             |
| Tabulacao vertical  | VT            | `\v`       | 11                   | 0xB             |
| Retrocesso          | BS            | `\b`       | 8                    | 0x8             |
| Retorno de carro    | CR            | `\r`       | 13                   | 0xD             |
| Alerta              | BEL           | `\a`       | 7                    | 0x7             |
| Barra invertida     | \             | `\\`       | 92                   | 0x5C            |
| Ponto de interrogacao| ?            | `\?`       | 63                   | 0x3F            |
| Aspas simples       | '             | `\'`       | 39                   | 0x27            |
| Aspas duplas        | "             | `\"`       | 34                   | 0x22            |

Voce pode usar essas notacoes em strings ou em constantes de caractere. Por exemplo:

```cpp
char alarme = '\a';
cout << alarme << "Nao faca isso de novo!\a\n";
cout << "Ben \"Buggsie\" Hacker\nesteve aqui!\n";
```

A ultima linha produz a seguinte saida:

```
Ben "Buggsie" Hacker
esteve aqui!
```

Observe que voce trata uma sequencia de escape, como `\n`, como um caractere comum, como Q. Ou seja, voce a coloca entre aspas simples para criar uma constante de caractere e nao usa aspas simples quando a inclui como parte de uma string.

> **Dica:** Quando voce tem uma escolha entre usar uma sequencia de escape numerica ou uma sequencia de escape simbolica, como `\0x8` versus `\b`, use o codigo simbolico. A representacao numerica esta ligada a um codigo especifico, como ASCII, mas a representacao simbolica funciona com todos os codigos e e mais legivel.

O Listagem 3.7 demonstra algumas sequencias de escape. Ele usa o caractere de alerta para chamar sua atencao, o caractere de nova linha para avancar o cursor e o caractere de retrocesso para recuar o cursor um espaco para a esquerda.

**Listagem 3.7 — bondini.cpp**

```cpp
// bondini.cpp -- usando sequencias de escape
#include <iostream>
int main()
{
    using namespace std;
    cout << "\aOperacao \"HyperHype\" esta agora ativada!\n";
    cout << "Digite seu codigo de agente:________\b\b\b\b\b\b\b\b";
    long codigo;
    cin >> codigo;
    cout << "\aVoce digitou " << codigo << "...\n";
    cout << "\aCodigo verificado! Prossiga com o Plano Z3!\n";
    return 0;
}
```

> **Nota:** Alguns sistemas podem se comportar de forma diferente, exibindo o `\b` como um pequeno retangulo em vez de retroceder, por exemplo, ou talvez apagando ao retroceder, talvez ignorando `\a`.

Quando voce inicia o programa no Listagem 3.7, ele coloca o seguinte texto na tela:

```
Operacao "HyperHype" esta agora ativada!
Digite seu codigo de agente:________
```

Apos imprimir os caracteres de sublinhado, o programa usa o caractere de retrocesso para retroceder o cursor ate o primeiro sublinhado. Voce pode entao digitar seu codigo secreto e continuar. Aqui esta uma execucao completa:

```
Operacao "HyperHype" esta agora ativada!
Digite seu codigo de agente:42007007
Voce digitou 42007007...
Codigo verificado! Prossiga com o Plano Z3!
```

### Nomes Universais de Caractere

Implementacoes C++ suportam um conjunto de caracteres-fonte basico — isto e, o conjunto de caracteres que voce pode usar para escrever codigo-fonte. Ele consiste nas letras (maiusculas e minusculas) e digitos encontrados em um teclado padrao dos EUA, os simbolos, como `{` e `=`, usados na linguagem C, e alguns outros caracteres, como o caractere de espaco. Depois ha um conjunto de caracteres de execucao basico, que inclui caracteres que podem ser processados durante a execucao de um programa.

O C++ tem um mecanismo para representar tais caracteres internacionais que e independente de qualquer teclado especifico: o uso de *nomes universais de caractere* (universal character names). Usar nomes universais de caractere e semelhante a usar sequencias de escape. Um nome universal de caractere comeca com `\u` ou `\U`. A forma `\u` e seguida por 8 digitos hexadecimais, e a forma `\U` por 16 digitos hexadecimais. Esses digitos representam o ponto de codigo ISO 10646 para o caractere.

```cpp
int körper;
cout << "Let them eat gâteau.\n";
```

O ponto de codigo ISO 10646 para o caractere o com trema (o-umlaut) e 00F6, e o ponto de codigo para a com acento circunflexo e 00E2.

> **Unicode e ISO 10646:**
> O Unicode fornece uma solucao para a representacao de varios conjuntos de caracteres fornecendo um sistema de numeracao padrao para um grande numero de caracteres e simbolos, agrupando-os por tipo. Por exemplo, o codigo ASCII e incorporado como um subconjunto do Unicode, de modo que caracteres latinos americanos como A e Z tem a mesma representacao em ambos os sistemas. Mas o Unicode tambem incorpora outros caracteres latinos, como os usados em linguas europeias; caracteres de outros alfabetos, incluindo grego, cirilico, hebraico, cherokee, arabe, tailandes e bengali; e ideogramas, como os usados para chines e japones. Ate agora, o Unicode representa mais de 109.000 simbolos e mais de 90 scripts. Se voce quiser saber mais, pode visitar o site do Consorcio Unicode em www.unicode.org.

### signed char e unsigned char

Ao contrario de `int`, `char` nao e com sinal por padrao. Nem e sem sinal por padrao. A escolha e deixada para a implementacao C++ para que o desenvolvedor do compilador possa adaptar melhor o tipo as propriedades do hardware. Se for vital para voce que `char` tenha um comportamento especifico, voce pode usar `signed char` ou `unsigned char` explicitamente como tipos:

```cpp
char fodo;          // pode ser com sinal, pode ser sem sinal
unsigned char bar;  // definitivamente sem sinal
signed char snark;  // definitivamente com sinal
```

Essas distincoes sao particularmente importantes se voce usar `char` como um tipo numerico. O tipo `unsigned char` tipicamente representa o intervalo 0 a 255, e `signed char` tipicamente representa o intervalo -128 a 127.

### Para Quando Voce Precisar de Mais: wchar_t

Programas podem ter que lidar com conjuntos de caracteres que nao cabem nos limites de um unico byte de 8 bits (por exemplo, o sistema de kanji japones). O tipo `wchar_t` (de *wide character type*, tipo de caractere largo) pode representar o conjunto de caracteres estendido. O tipo `wchar_t` e um tipo inteiro com espaco suficiente para representar o maior conjunto de caracteres estendido usado no sistema. Este tipo tem o mesmo tamanho e propriedades de sinal que um dos outros tipos inteiros, que e chamado de *tipo subjacente* (underlying type).

As familias `cin` e `cout` consideram entrada e saida como consistindo de fluxos de chars, portanto nao sao adequadas para lidar com o tipo `wchar_t`. O arquivo de cabecalho `iostream` fornece facilidades paralelas na forma de `wcin` e `wcout` para lidar com fluxos `wchar_t`. Alem disso, voce pode indicar uma constante de caractere largo ou string precedendo-a com um `L`:

```cpp
wchar_t bob = L'P';    // uma constante de caractere largo
wcout << L"alto" << endl; // exibindo uma string de caractere largo
```

### Novos Tipos do C++11: char16_t e char32_t

Conforme a comunidade de programacao ganhou mais experiencia com Unicode, ficou claro que o tipo `wchar_t` nao era suficiente. Descobriu-se que codificar caracteres e strings de caracteres em um sistema de computador e mais complexo do que apenas usar os valores numericos Unicode (chamados de pontos de codigo). O C++11 introduz os tipos `char16_t`, que e sem sinal e de 16 bits, e `char32_t`, que e sem sinal e de 32 bits. O C++11 usa o prefixo `u` para constantes de caractere e string `char16_t`, como em `u'C'` e `u"seja bom"`. Da mesma forma, usa o prefixo `U` para constantes `char32_t`, como em `U'R'` e `U"rato sujo"`.

```cpp
char16_t c1 = u'q';           // caractere basico em forma de 16 bits
char32_t c2 = U'\U0000222B';  // nome universal de caractere em forma de 32 bits
```

### O Tipo bool

O Padrao ANSI/ISO C++ adicionou um novo tipo (novo para o C++, isto e), chamado `bool`. Ele recebe este nome em homenagem ao matematico ingles George Boole, que desenvolveu uma representacao matematica das leis da logica. Em computacao, uma variavel booleana e aquela cujo valor pode ser `true` (verdadeiro) ou `false` (falso). No passado, o C++, como o C, nao tinha um tipo booleano. Em vez disso, como voce vera em maior detalhe nos Capitulos 5 e 6, o C++ interpretava valores nao-zero como verdadeiro e valores zero como falso. Agora, no entanto, voce pode usar o tipo `bool` para representar verdadeiro e falso, e os literais predefinidos `true` e `false` representam esses valores. Ou seja, voce pode fazer instrucoes como a seguinte:

```cpp
bool pronto = true;
```

Os literais `true` e `false` podem ser convertidos para o tipo `int` por promocao, com `true` sendo convertido para 1 e `false` para 0:

```cpp
int resp = true;    // resp recebe 1
int promessa = false; // promessa recebe 0
```

Alem disso, qualquer valor numerico ou de ponteiro pode ser convertido implicitamente (isto e, sem um type cast explicito) para um valor `bool`. Qualquer valor nao-zero converte para `true`, enquanto um valor zero converte para `false`:

```cpp
bool inicio = -100; // inicio recebe true
bool parada = 0;    // parada recebe false
```

## O Qualificador const

Agora vamos voltar ao topico de nomes simbolicos para constantes. Um nome simbolico pode sugerir o que a constante representa. Alem disso, se o programa usa a constante em varios lugares e voce precisa alterar o valor, voce pode apenas alterar a definicao de simbolo unica.

A forma melhor do C++ de lidar com constantes simbolicas e usar a palavra-chave `const` para modificar uma declaracao e inicializacao de variavel. Suponha, por exemplo, que voce queira uma constante simbolica para o numero de meses em um ano. Basta inserir esta linha em um programa:

```cpp
const int Meses = 12; // Meses e constante simbolica para 12
```

Agora voce pode usar `Meses` em um programa em vez de `12`. Apos voce inicializar uma constante como `Meses`, seu valor e definido. O compilador nao deixa voce alterar subsequentemente o valor de `Meses`. Se voce tentar, por exemplo, o g++ da uma mensagem de erro dizendo que o programa usou uma atribuicao de uma variavel somente para leitura.

A forma geral para criar uma constante e esta:

```cpp
const tipo nome = valor;
```

Observe que voce inicializa um `const` na declaracao. A seguinte sequencia nao funciona:

```cpp
const int Dedos;  // valor de Dedos indefinido neste ponto
Dedos = 10;       // tarde demais!
```

> **Dica:** Se voce esta vindo ao C++ do C e esta prestes a usar `#define` para definir uma constante simbolica, use `const` em vez disso.

Se o seu historico e em C, voce pode sentir que a instrucao `#define`, discutida anteriormente, ja faz o trabalho adequadamente. Mas `const` e melhor. Por um lado, ele permite que voce especifique o tipo explicitamente. Em segundo lugar, voce pode usar as regras de escopo do C++ para limitar a definicao a funcoes ou arquivos especificos. Terceiro, voce pode usar `const` com tipos mais elaborados, como arrays e estruturas.

## Numeros de Ponto Flutuante

Agora que voce viu a linha completa de tipos inteiros do C++, vamos ver os tipos de ponto flutuante (floating-point types), que compoe o segundo grupo principal de tipos fundamentais do C++. Esses numeros permitem representar numeros com partes fracionarias, como a quilometragem de um tanque de guerra (0,56 km/L). Eles tambem fornecem um intervalo muito maior de valores. Se um numero e muito grande para ser representado como tipo `long` — por exemplo, o numero de celulas bacterianas no corpo humano (estimado em mais de 100.000.000.000.000) — voce pode usar um dos tipos de ponto flutuante.

Com tipos de ponto flutuante, voce pode representar numeros como 2,5 e 3,14159 e 122442,32 — isto e, numeros com partes fracionarias. Um computador armazena esses valores em duas partes. Uma parte representa um valor, e a outra parte escala esse valor para cima ou para baixo. O C++ usa um metodo semelhante para representar numeros de ponto flutuante internamente, exceto que e baseado em numeros binarios, portanto o escalonamento e por fatores de 2 em vez de por fatores de 10.

### Escrevendo Numeros de Ponto Flutuante

O C++ tem duas formas de escrever numeros de ponto flutuante. A primeira e usar a notacao padrao de ponto decimal que voce usou a maior parte da vida:

```
12.34      // ponto flutuante
939001.32  // ponto flutuante
0.00023    // ponto flutuante
8.0        // ainda ponto flutuante
```

Mesmo que a parte fracionaria seja 0, como em `8.0`, o ponto decimal garante que o numero seja representado em formato de ponto flutuante e nao como um inteiro.

O segundo metodo para representar valores de ponto flutuante e chamado de *notacao E* (E notation), e se parece com isto: `3.45E6`. Isso significa que o valor 3,45 e multiplicado por 1.000.000; o E6 significa 10 elevado a 6a potencia, que e 1 seguido de 6 zeros. Assim, `3.45E6` significa 3.450.000. O 6 e chamado de *expoente* (exponent), e o 3,45 e chamado de *mantissa* (mantissa). Aqui estao mais exemplos:

```
2.52e+8     // pode usar E ou e, + e opcional
8.33E-4     // o expoente pode ser negativo
7E5         // o mesmo que 7.0E+05
-18.32e13   // pode ter sinal + ou - na frente
1.69e12     // divida publica brasileira de 2010 em reais
5.98E24     // massa da Terra em quilogramas
9.11e-31    // massa de um eletron em quilogramas
```

Usar um expoente negativo significa dividir por uma potencia de 10 em vez de multiplicar por uma potencia de 10. Assim, `8.33E-4` significa `8,33 / 10^4`, ou `0,000833`. Da mesma forma, a massa do eletron `9,11e-31` kg significa `0,000000000000000000000000000000911` kg.

> **Nota:** A forma `d.dddE+n` significa mover o ponto decimal `n` casas para a direita, e a forma `d.dddE-n` significa mover o ponto decimal `n` casas para a esquerda. Este ponto decimal movel e a origem do termo "ponto flutuante".

### Tipos de Ponto Flutuante

Como o ANSI C, o C++ tem tres tipos de ponto flutuante: `float`, `double` e `long double`. Esses tipos sao descritos em termos do numero de figuras significativas que podem representar e do intervalo minimo permitido de expoentes. *Figuras significativas* (significant figures) sao os digitos significativos em um numero.

Com efeito, os requisitos do C e C++ para digitos significativos equivalem a `float` sendo pelo menos 32 bits, `double` sendo pelo menos 48 bits e certamente nao menor que `float`, e `long double` sendo pelo menos tao grande quanto `double`. Todos os tres podem ser do mesmo tamanho. Tipicamente, porem, `float` e 32 bits, `double` e 64 bits, e `long double` e 80, 96 ou 128 bits. Alem disso, o intervalo nos expoentes para todos os tres tipos e pelo menos -37 a +37. Voce pode verificar nos arquivos de cabecalho `cfloat` ou `float.h` os limites para seu sistema.

O Listagem 3.8 examina os tipos `float` e `double` e como eles podem diferir na precisao com que representam numeros. O programa previa um metodo `ostream` chamado `setf()`. Esta chamada especifica forca a saida a permanecer na notacao de ponto fixo para que voce possa ver melhor a precisao. Ela evita que o programa mude para notacao E para valores grandes.

**Listagem 3.8 — numpf.cpp**

```cpp
// numpf.cpp -- tipos de ponto flutuante
#include <iostream>
int main()
{
    using namespace std;
    cout.setf(ios_base::fixed, ios_base::floatfield); // ponto fixo
    float balde = 10.0 / 3.0;    // bom para cerca de 6 casas
    double menta = 10.0 / 3.0;   // bom para cerca de 15 casas
    const float milhao = 1.0e6;
    cout << "balde = " << balde;
    cout << ", um milhao de baldes = " << milhao * balde;
    cout << ",\ne dez milhoes de baldes = ";
    cout << 10 * milhao * balde << endl;
    cout << "menta = " << menta << " e um milhao de mentas = ";
    cout << milhao * menta << endl;
    return 0;
}
```

Aqui esta a saida do programa no Listagem 3.8:

```
balde = 3.333333, um milhao de baldes = 3333333.250000,
e dez milhoes de baldes = 33333332.000000
menta = 3.333333 e um milhao de mentas = 3333333.333333
```

#### Notas do Programa

Normalmente, `cout` remove zeros finais. Por exemplo, exibiria `3333333.250000` como `3333333.25`. A chamada a `cout.setf()` substitui esse comportamento, pelo menos em novas implementacoes. O ponto principal a notar no Listagem 3.8 e como `float` tem menos precisao que `double`. Tanto `balde` quanto `menta` sao inicializados para `10.0 / 3.0`. Isso deve ser avaliado como `3.33333333333333333...` (etc.). Como `cout` imprime seis figuras a direita do ponto decimal, voce pode ver que tanto `balde` quanto `menta` sao precisos ate la. Mas apos o programa multiplicar cada numero por um milhao, voce ve que `balde` diverge do valor correto apos o setimo tres. `balde` e bom para sete figuras significativas. O tipo `double`, no entanto, mostra 13 tres, portanto e bom para pelo menos 13 figuras significativas.

> **Lendo Arquivos de Inclusao:**
> As diretivas de inclusao encontradas no topo dos arquivos de codigo-fonte C++ muitas vezes assumem o ar de uma encantacao magica; programadores C++ iniciantes aprendem, atraves de leitura e experiencia, quais arquivos de cabecalho adicionam funcionalidades especificas, e os incluem somente para fazer seus programas funcionar. Nao confie nos arquivos de inclusao apenas como fonte de conhecimento mistico; fique a vontade para abri-los e le-los. Eles sao arquivos de texto, portanto voce pode le-los facilmente. Todos os arquivos que voce inclui em seus programas existem no seu computador ou em um lugar onde seu computador pode usa-los. Encontre os includes que voce usa e veja o que eles contem.

### Constantes de Ponto Flutuante

Quando voce escreve uma constante de ponto flutuante em um programa, em qual tipo de ponto flutuante o programa a armazena? Por padrao, constantes de ponto flutuante como `8.24` e `2.4E8` sao do tipo `double`. Se voce quiser que uma constante seja do tipo `float`, voce usa um sufixo `f` ou `F`. Para o tipo `long double`, voce usa um sufixo `l` ou `L`. Aqui estao alguns exemplos:

```cpp
1.234f    // uma constante float
2.45E20F  // uma constante float
2.345324E28 // uma constante double
2.2L      // uma constante long double
```

### Vantagens e Desvantagens dos Numeros de Ponto Flutuante

Os numeros de ponto flutuante tem duas vantagens sobre os inteiros. Primeiro, eles podem representar valores entre inteiros. Segundo, por causa do fator de escalonamento, eles podem representar um intervalo muito maior de valores. Por outro lado, as operacoes de ponto flutuante geralmente sao um pouco mais lentas que as operacoes de inteiros, e voce pode perder precisao. O Listagem 3.9 ilustra o ultimo ponto.

**Listagem 3.9 — somaflut.cpp**

```cpp
// somaflut.cpp -- problemas de precisao com float
#include <iostream>
int main()
{
    using namespace std;
    float a = 2.34E+22f;
    float b = a + 1.0f;
    cout << "a = " << a << endl;
    cout << "b - a = " << b - a << endl;
    return 0;
}
```

O programa no Listagem 3.9 pega um numero, adiciona 1 e entao subtrai o numero original. Isso deve resultar em um valor de 1. Mas a saida e:

```
a = 2.34e+022
b - a = 0
```

O problema e que `2.34E+22` representa um numero com 23 digitos a esquerda do ponto decimal. Ao adicionar 1, voce esta tentando adicionar 1 ao 23o digito nesse numero. Mas o tipo `float` pode representar apenas os primeiros 6 ou 7 digitos em um numero, portanto tentar alterar o 23o digito nao tem efeito sobre o valor.

> **Classificando Tipos de Dados:**
> O C++ traz alguma ordem a seus tipos basicos classificando-os em familias. Os tipos `signed char`, `short`, `int` e `long` sao chamados de *tipos inteiros com sinal* (signed integer types). O C++11 adiciona `long long` a essa lista. As versoes sem sinal sao chamadas de *tipos inteiros sem sinal* (unsigned integer types). Os tipos `bool`, `char`, `wchar_t`, inteiro com sinal e inteiro sem sinal juntos sao chamados de *tipos integrais* (integral types) ou *tipos inteiros* (integer types). O C++11 adiciona `char16_t` e `char32_t` a essa lista. Os tipos `float`, `double` e `long double` sao chamados de *tipos de ponto flutuante* (floating-point types). Tipos inteiros e de ponto flutuante sao coletivamente chamados de *tipos aritmeticos* (arithmetic types).

## Operadores Aritmeticos do C++

Talvez voce tenha boas memorias de fazer exercicios de aritmetica na escola. Voce pode dar esse mesmo prazer ao seu computador. O C++ usa operadores para fazer aritmetica. Ele fornece operadores para cinco calculos aritmeticos basicos: adicao, subtracao, multiplicacao, divisao e obtencao do modulo. Cada um desses operadores usa dois valores (chamados *operandos*) para calcular uma resposta final. Juntos, o operador e seus operandos constituem uma *expressao* (expression). Por exemplo, considere a seguinte instrucao:

```cpp
int rodas = 4 + 2;
```

Os valores `4` e `2` sao operandos, o simbolo `+` e o operador de adicao, e `4 + 2` e uma expressao cujo valor e `6`.

Aqui estao os cinco operadores aritmeticos basicos do C++:

- O operador `+` adiciona seus operandos. Por exemplo, `4 + 20` e avaliado como `24`.
- O operador `-` subtrai o segundo operando do primeiro. Por exemplo, `12 - 3` e avaliado como `9`.
- O operador `*` multiplica seus operandos. Por exemplo, `28 * 4` e avaliado como `112`.
- O operador `/` divide seu primeiro operando pelo segundo. Por exemplo, `1000 / 5` e avaliado como `200`. Se ambos os operandos sao inteiros, o resultado e a parte inteira do quociente. Por exemplo, `17 / 3` e `5`, com a parte fracionaria descartada.
- O operador `%` encontra o modulo do primeiro operando em relacao ao segundo. Ou seja, ele produz o resto da divisao do primeiro pelo segundo. Por exemplo, `19 % 6` e `1` porque `6` entra em `19` tres vezes, com um resto de `1`. Ambos os operandos devem ser tipos inteiros; usar o operador `%` com valores de ponto flutuante causa um erro em tempo de compilacao. Se um dos operandos for negativo, o sinal do resultado satisfaz a seguinte regra: `(a/b)*b + a%b` e igual a `a`.

E claro, voce pode usar variaveis assim como constantes para operandos. O Listagem 3.10 faz exatamente isso.

**Listagem 3.10 — aritm.cpp**

```cpp
// aritm.cpp -- aritmetica C++
#include <iostream>
int main()
{
    using namespace std;
    float chapeus, cabecas;
    cout.setf(ios_base::fixed, ios_base::floatfield); // ponto fixo
    cout << "Digite um numero: ";
    cin >> chapeus;
    cout << "Digite outro numero: ";
    cin >> cabecas;
    cout << "chapeus = " << chapeus << "; cabecas = " << cabecas << endl;
    cout << "chapeus + cabecas = " << chapeus + cabecas << endl;
    cout << "chapeus - cabecas = " << chapeus - cabecas << endl;
    cout << "chapeus * cabecas = " << chapeus * cabecas << endl;
    cout << "chapeus / cabecas = " << chapeus / cabecas << endl;
    return 0;
}
```

Aqui esta uma saida de exemplo do programa no Listagem 3.10:

```
Digite um numero: 50.25
Digite outro numero: 11.17
chapeus = 50.250000; cabecas = 11.170000
chapeus + cabecas = 61.419998
chapeus - cabecas = 39.080002
chapeus * cabecas = 561.292480
chapeus / cabecas = 4.498657
```

Bem, talvez voce nao possa confiar completamente nisso. Adicionar 11,17 a 50,25 deveria resultar em 61,42, mas a saida informa 61,419998. Este nao e um problema aritmetico; e um problema com a capacidade limitada do tipo `float` de representar figuras significativas. Lembre-se, o C++ garante apenas seis figuras significativas para `float`. Se voce arredondar 61,419998 para seis figuras, obtera 61,4200, que e o valor correto para a precisao garantida. A moral e que se voce precisar de maior precisao, deve usar `double` ou `long double`.

### Ordem das Operacoes: Precedencia e Associatividade de Operadores

Voce pode confiar no C++ para fazer aritmetica complicada? Sim, mas voce deve conhecer as regras que o C++ usa. Por exemplo, muitas expressoes envolvem mais de um operador. Isso pode levantar questoes sobre qual operador e aplicado primeiro. Por exemplo, considere esta instrucao:

```cpp
int porcosVoando = 3 + 4 * 5; // 35 ou 23?
```

O 4 parece ser um operando tanto para os operadores `+` quanto para `*`. Quando mais de um operador pode ser aplicado ao mesmo operando, o C++ usa regras de *precedencia* (precedence) para decidir qual operador e usado primeiro. Os operadores aritmeticos seguem a precedencia algebrica usual, com multiplicacao, divisao e obtencao do modulo feitos antes da adicao e subtracao. Assim, `3 + 4 * 5` significa `3 + (4 * 5)`, nao `(3 + 4) * 5`. Portanto, a resposta e `23`, nao `35`. E claro, voce pode usar parenteses para impor suas proprias prioridades.

As vezes a lista de precedencia nao e suficiente. Considere a seguinte instrucao:

```cpp
float registros = 120 / 4 * 5; // 150 ou 6?
```

Quando dois operadores tem a mesma precedencia, o C++ olha se os operadores tem associatividade da esquerda para a direita ou da direita para a esquerda. *Associatividade da esquerda para a direita* (left-to-right associativity) significa que se dois operadores que agem no mesmo operando tem a mesma precedencia, voce aplica o operador mais a esquerda primeiro. O Apendice D, "Precedencia de Operadores", mostra que multiplicacao e divisao se associam da esquerda para a direita. Isso significa que voce usa 4 com o operador mais a esquerda primeiro. Ou seja, voce divide 120 por 4, obtem 30 como resultado e entao multiplica o resultado por 5 para obter 150.

Observe que as regras de precedencia e associatividade entram em jogo apenas quando dois operadores compartilham o mesmo operando.

### Diversoes com Divisao

Voce ainda nao viu o restante da historia sobre o operador de divisao (`/`). O comportamento desse operador depende do tipo dos operandos. Se ambos os operandos sao inteiros, o C++ realiza divisao inteira. Isso significa que qualquer parte fracionaria da resposta e descartada, tornando o resultado um inteiro. Se um ou ambos os operandos sao valores de ponto flutuante, a parte fracionaria e mantida, tornando o resultado ponto flutuante. O Listagem 3.11 ilustra como a divisao do C++ funciona com diferentes tipos de valores.

**Listagem 3.11 — divisao.cpp**

```cpp
// divisao.cpp -- divisao inteira e de ponto flutuante
#include <iostream>
int main()
{
    using namespace std;
    cout.setf(ios_base::fixed, ios_base::floatfield);
    cout << "Divisao inteira: 9/5 = " << 9 / 5 << endl;
    cout << "Divisao de ponto flutuante: 9.0/5.0 = ";
    cout << 9.0 / 5.0 << endl;
    cout << "Divisao mista: 9.0/5 = " << 9.0 / 5 << endl;
    cout << "Constantes double: 1e7/9.0 = ";
    cout << 1.e7 / 9.0 << endl;
    cout << "Constantes float: 1e7f/9.0f = ";
    cout << 1.e7f / 9.0f << endl;
    return 0;
}
```

Aqui esta a saida do programa no Listagem 3.11 para uma implementacao:

```
Divisao inteira: 9/5 = 1
Divisao de ponto flutuante: 9.0/5.0 = 1.800000
Divisao mista: 9.0/5 = 1.800000
Constantes double: 1e7/9.0 = 1111111.111111
Constantes float: 1e7f/9.0f = 1111111.125000
```

A primeira linha de saida mostra que dividir o inteiro 9 pelo inteiro 5 resulta no inteiro 1. A parte fracionaria de 4/5 (ou 0,8) e descartada. As proximas duas linhas mostram que quando pelo menos um dos operandos e ponto flutuante, voce obtem uma resposta de ponto flutuante de 1,8.

> **Um Vislumbre da Sobrecarga de Operadores:**
> No Listagem 3.11, o operador de divisao representa tres operacoes distintas: divisao `int`, divisao `float` e divisao `double`. O C++ usa o contexto — neste caso, o tipo dos operandos — para determinar qual operador e pretendido. O processo de usar o mesmo simbolo para mais de uma operacao e chamado de *sobrecarga de operadores* (operator overloading). O C++ tem alguns exemplos de sobrecarga embutidos na linguagem. O C++ tambem permite que voce estenda a sobrecarga de operadores para classes definidas pelo usuario.

### O Operador de Modulo

A maioria das pessoas esta mais familiarizada com adicao, subtracao, multiplicacao e divisao do que com a operacao de modulo, entao vamos tirar um momento para ver o operador de modulo em acao. O operador de modulo retorna o resto de uma divisao inteira. Em combinacao com a divisao inteira, a operacao de modulo e particularmente util em problemas que requerem dividir uma quantidade em diferentes unidades integrais, como converter polegadas em pes e polegadas ou converter dolares em moedas de 25, 10, 5 e 1 centavo.

No Capitulo 2, o Listagem 2.6 converte peso em stone britanico para libras. O Listagem 3.12 inverte o processo, convertendo peso em libras para stone. Uma stone, lembre-se, e 14 libras, e a maioria das balancas de banheiro britanicas sao calibradas nessa unidade. O programa usa divisao inteira para encontrar o maior numero de stone inteiros no peso, e usa o operador de modulo para encontrar o numero de libras restantes.

**Listagem 3.12 — modulo.cpp**

```cpp
// modulo.cpp -- usa o operador % para converter libras em stone
#include <iostream>
int main()
{
    using namespace std;
    const int LibrasPorStone = 14;
    int libras;
    cout << "Informe seu peso em libras: ";
    cin >> libras;
    int pedras = libras / LibrasPorStone;  // stone inteiros
    int resto = libras % LibrasPorStone;   // resto em libras
    cout << libras << " libras sao " << pedras
         << " stone, " << resto << " libra(s).\n";
    return 0;
}
```

Aqui esta uma execucao de exemplo do programa no Listagem 3.12:

```
Informe seu peso em libras: 181
181 libras sao 12 stone, 13 libra(s).
```

## Conversoes de Tipo

A profusao de tipos do C++ permite que voce corresponda o tipo a necessidade. Ela tambem complica a vida para o computador. Por exemplo, adicionar dois valores `short` pode envolver instrucoes de hardware diferentes de adicionar dois valores `long`. Com 11 tipos inteiros e 3 tipos de ponto flutuante, o computador pode ter muito casos diferentes para lidar, especialmente se voce comecar a misturar tipos. Para ajudar a lidar com essa potencial bagunca, o C++ faz muitas conversoes de tipo automaticamente:

- O C++ converte valores quando voce atribui um valor de um tipo aritmetico a uma variavel de outro tipo aritmetico.
- O C++ converte valores quando voce combina tipos mistos em expressoes.
- O C++ converte valores quando voce passa argumentos para funcoes.

### Conversao na Inicializacao e Atribuicao

O C++ e bastante liberal ao permitir que voce atribua um valor numerico de um tipo a uma variavel de outro tipo. Sempre que voce fizer isso, o valor e convertido para o tipo da variavel receptora. Por exemplo, suponha que `mto_longo` e do tipo `long`, `trinta` e do tipo `short`, e voce tem a seguinte instrucao em um programa:

```cpp
mto_longo = trinta; // atribuindo um short a um long
```

O programa pega o valor de `trinta` (tipicamente um valor de 16 bits) e o expande para um valor `long` (tipicamente um valor de 32 bits) ao fazer a atribuicao. Note que a expansao cria um novo valor para colocar em `mto_longo`; o conteudo de `trinta` nao e alterado.

Atribuir um valor a um tipo com maior intervalo geralmente nao cria problema. Por exemplo, atribuir um valor `short` a uma variavel `long` nao altera o valor; apenas da ao valor mais alguns bytes para se acomodar. No entanto, atribuir um grande valor `long` como `2111222333` a uma variavel `float` resulta na perda de alguma precisao.

A Tabela 3.3 aponta alguns possiveis problemas de conversao.

**Tabela 3.3 — Potenciais Problemas de Conversao Numerica**

| Tipo de Conversao                                     | Problemas Potenciais                                                                        |
|-------------------------------------------------------|---------------------------------------------------------------------------------------------|
| Tipo de ponto flutuante maior para menor, como `double` para `float` | Perda de precisao (figuras significativas); valor pode estar fora do intervalo para o tipo alvo |
| Tipo de ponto flutuante para tipo inteiro             | Perda da parte fracionaria; valor original pode estar fora do intervalo para o tipo alvo   |
| Tipo inteiro maior para menor, como `long` para `short` | Valor original pode estar fora do intervalo para o tipo alvo; tipicamente apenas os bytes de ordem inferior sao copiados |

Um valor zero atribuido a uma variavel `bool` e convertido para `false`, e um valor nao-zero e convertido para `true`.

Atribuir valores de ponto flutuante a tipos inteiros apresenta um par de problemas. Primeiro, converter ponto flutuante para inteiro resulta em truncamento do numero (descartando a parte fracionaria). Segundo, um valor `float` pode ser muito grande para caber em uma variavel `int` apertada.

O Listagem 3.13 mostra algumas conversoes por inicializacao.

**Listagem 3.13 — inicconv.cpp**

```cpp
// inicconv.cpp -- mudancas de tipo na inicializacao
#include <iostream>
int main()
{
    using namespace std;
    cout.setf(ios_base::fixed, ios_base::floatfield);
    float arvore = 3;      // int convertido para float
    int palpite(3.9832);   // double convertido para int
    int divida = 7.2E12;   // resultado nao definido em C++
    cout << "arvore = " << arvore << endl;
    cout << "palpite = " << palpite << endl;
    cout << "divida = " << divida << endl;
    return 0;
}
```

Aqui esta a saida do programa no Listagem 3.13 para um sistema:

```
arvore = 3.000000
palpite = 3
divida = 1634811904
```

Neste caso, `arvore` recebe o valor de ponto flutuante `3,0`. Atribuir `3,9832` a variavel `int` `palpite` faz com que o valor seja truncado para `3`; o C++ usa truncamento (descartando a parte fracionaria) e nao arredondamento ao converter tipos de ponto flutuante para tipos inteiros. Por fim, observe que a variavel `int` `divida` e incapaz de conter o valor `7.2E12`. Isso cria uma situacao em que o C++ nao define o resultado.

### Conversoes de Inicializacao Quando {} E Usado (C++11)

O C++11 chama uma inicializacao que usa chaves de *inicializacao de lista* (list-initialization). E mais restritiva em conversoes de tipo do que as formas usadas no Listagem 3.13. Em particular, a inicializacao de lista nao permite *estreitamento* (narrowing), que e quando o tipo da variavel pode nao ser capaz de representar o valor atribuido. Por exemplo, conversoes de tipos de ponto flutuante para tipos inteiros nao sao permitidas:

```cpp
const int codigo = 66;
int x = 66;
char c1 {31325};   // estreitamento, nao permitido
char c2 = {66};    // permitido porque char pode conter 66
char c3 {codigo};  // idem
char c4 = {x};     // nao permitido, x nao e constante
x = 31325;
char c5 = x;       // permitido por esta forma de inicializacao
```

### Conversoes em Expressoes

Considere o que acontece quando voce combina dois tipos aritmeticos diferentes em uma expressao. O C++ faz dois tipos de conversoes automaticas nesse caso. Primeiro, alguns tipos sao convertidos automaticamente sempre que ocorrem. Segundo, alguns tipos sao convertidos quando sao combinados com outros tipos em uma expressao.

Primeiro, vamos examinar as conversoes automaticas. Quando avalia expressoes, o C++ converte valores `bool`, `char`, `unsigned char`, `signed char` e `short` para `int`. Em particular, `true` e promovido para 1 e `false` para 0. Essas conversoes sao chamadas de *promocoes integrais* (integral promotions). Por exemplo, considere as seguintes instrucoes:

```cpp
short galinhas = 20; // linha 1
short patos = 35;    // linha 2
short aves = galinhas + patos; // linha 3
```

Para executar a instrucao na linha 3, um programa C++ pega os valores de `galinhas` e `patos` e os converte ambos para `int`. Entao o programa converte o resultado de volta para o tipo `short` porque a resposta e atribuida a uma variavel do tipo `short`.

Em seguida, ha as conversoes que ocorrem quando voce combina aritmeticamente diferentes tipos, como adicionar um `int` a um `float`. Quando uma operacao envolve dois tipos, o menor e convertido para o maior. O compilador passa por uma lista de verificacao para determinar quais conversoes fazer em uma expressao aritmetica. Aqui esta a versao C++11 da lista, que o compilador percorre em ordem:

1. Se qualquer operando for do tipo `long double`, o outro operando sera convertido para `long double`.
2. Caso contrario, se qualquer operando for `double`, o outro operando sera convertido para `double`.
3. Caso contrario, se qualquer operando for `float`, o outro operando sera convertido para `float`.
4. Caso contrario, os operandos sao tipos inteiros e as promocoes integrais sao feitas.
5. Nesse caso, se ambos os operandos forem com sinal ou se ambos forem sem sinal, e um for de posto inferior ao outro, ele sera convertido para o posto mais alto.
6. Caso contrario, um operando e com sinal e um e sem sinal. Se o operando sem sinal for de posto mais alto que o operando com sinal, o ultimo sera convertido para o tipo do operando sem sinal.
7. Caso contrario, se o tipo com sinal pode representar todos os valores do tipo sem sinal, o operando sem sinal e convertido para o tipo do tipo com sinal.
8. Caso contrario, ambos os operandos sao convertidos para a versao sem sinal do tipo com sinal.

### Conversoes na Passagem de Argumentos

Normalmente, a prototipagem de funcoes do C++ controla as conversoes de tipo para a passagem de argumentos, como voce aprendera no Capitulo 7. No entanto, e possivel, embora geralmente imprudente, dispensar o controle de prototipo para a passagem de argumentos. Nesse caso, o C++ aplica as promocoes integrais aos tipos `char` e `short` (com sinal e sem sinal). Alem disso, para preservar a compatibilidade com enormes quantidades de codigo no C classico, o C++ promove argumentos `float` para `double` ao passa-los para uma funcao que dispensa a prototipagem.

### Type Casts

O C++ lhe empodera para forcar conversoes de tipo explicitamente atraves do mecanismo de *type cast* (conversao de tipo forcada). O type cast vem em duas formas. Por exemplo, para converter um valor `int` armazenado em uma variavel chamada `espinho` para o tipo `long`, voce pode usar qualquer uma das expressoes a seguir:

```cpp
(long) espinho   // retorna uma conversao do tipo long de espinho
long (espinho)   // retorna uma conversao do tipo long de espinho
```

O type cast nao altera a variavel `espinho` em si; em vez disso, ele cria um novo valor do tipo indicado, que voce pode entao usar em uma expressao, como no seguinte:

```cpp
cout << int('Q'); // exibe o codigo inteiro para 'Q'
```

Mais geralmente, voce pode fazer o seguinte:

```cpp
(nomeDoTipo) valor // converte valor para o tipo nomeDoTipo
nomeDoTipo (valor) // converte valor para o tipo nomeDoTipo
```

O C++ tambem introduz quatro operadores de type cast que sao mais restritivos em como podem ser usados. O Capitulo 15, "Amigos, Excecoes e Mais", os cobre. Dos quatro, o operador `static_cast<>` pode ser usado para converter valores de um tipo numerico para outro. Por exemplo, usar para converter `espinho` para um valor do tipo `long` parece com isto:

```cpp
static_cast<long> (espinho) // retorna uma conversao do tipo long de espinho
```

Mais geralmente, voce pode fazer o seguinte:

```cpp
static_cast<nomeDoTipo> (valor) // converte valor para o tipo nomeDoTipo
```

O Listagem 3.14 ilustra brevemente tanto o type cast basico (duas formas) quanto o `static_cast<>`.

**Listagem 3.14 — tipocv.cpp**

```cpp
// tipocv.cpp -- forcando mudancas de tipo
#include <iostream>
int main()
{
    using namespace std;
    int alcas, morcegos, mergulhoes;
    // a instrucao a seguir soma os valores como double,
    // depois converte o resultado para int
    alcas = 19.99 + 11.99;
    // essas instrucoes somam valores como int
    morcegos = (int) 19.99 + (int) 11.99; // sintaxe C antiga
    mergulhoes = int (19.99) + int (11.99); // sintaxe C++ nova
    cout << "alcas = " << alcas << ", morcegos = " << morcegos;
    cout << ", mergulhoes = " << mergulhoes << endl;
    char c = 'Z';
    cout << "O codigo para " << c << " e ";   // imprime como char
    cout << int(c) << endl;                    // imprime como int
    cout << "Sim, o codigo e ";
    cout << static_cast<int>(c) << endl;       // usando static_cast
    return 0;
}
```

Aqui esta o resultado do programa no Listagem 3.14:

```
alcas = 31, morcegos = 30, mergulhoes = 30
O codigo para Z e 90
Sim, o codigo e 90
```

Primeiro, adicionar `19.99` a `11.99` resulta em `31.98`. Quando esse valor e atribuido a variavel `int` `alcas`, ele e truncado para `31`. Mas usar type casts trunca os mesmos dois valores para `19` e `11` antes da adicao, fazendo `30` ser o resultado para `morcegos` e `mergulhoes`. Em seguida, duas instrucoes `cout` usam type casts para converter um valor do tipo `char` para `int` antes de exibirem o resultado. Essas conversoes fazem `cout` imprimir o valor como um inteiro em vez de como um caractere.

Este programa ilustra duas razoes para usar type casting. Primeiro, voce pode ter valores armazenados como tipo `double`, mas usados para calcular um valor do tipo `int`. O type casting permite que voce faca isso diretamente. Segundo, a parte mais comum de usar um type cast: a capacidade de obrigar dados em uma forma a atender uma expectativa diferente.

## Declaracoes auto no C++11

O C++11 introduz uma facilidade que permite que o compilador deduza um tipo a partir do tipo de um valor de inicializacao. Para este proposito, ele redefine o significado de `auto`, uma palavra-chave que data do C, mas quase nunca usada. Basta usar `auto` em vez do nome do tipo em uma declaracao de inicializacao, e o compilador atribui a variavel o mesmo tipo que o do inicializador:

```cpp
auto n = 100;    // n e int
auto x = 1.5;    // x e double
auto y = 1.3e12L; // y e long double
```

No entanto, essa deducao de tipo automatica nao e realmente destinada a casos tao simples. Na verdade, voce pode ate se perder. Por exemplo, suponha que `x`, `y` e `z` devem ser do tipo `double`:

```cpp
auto x = 0.0;   // ok, x e double porque 0.0 e double
double y = 0;   // ok, 0 e convertido automaticamente para 0.0
auto z = 0;     // oops, z e int porque 0 e int
```

A deducao de tipo automatica se torna muito mais util ao lidar com tipos complicados, como aqueles na STL (Standard Template Library). Por exemplo, o codigo do C++98 poderia ter isso:

```cpp
std::vector<double> pontuacoes;
std::vector<double>::iterator pv = pontuacoes.begin();
```

O C++11 permite que voce escreva isso em vez disso:

```cpp
std::vector<double> pontuacoes;
auto pv = pontuacoes.begin();
```

Mencionaremos esse novo significado de `auto` novamente mais tarde, quando ele se tornar mais relevante para os topicos em questao.

## Resumo

Os tipos basicos do C++ se enquadram em dois grupos. Um grupo consiste em valores que sao armazenados como inteiros. O segundo grupo consiste em valores que sao armazenados em formato de ponto flutuante. Os tipos inteiros diferem uns dos outros na quantidade de memoria usada para armazenar valores e em se sao com sinal ou sem sinal. Do menor ao maior, os tipos inteiros sao `bool`, `char`, `signed char`, `unsigned char`, `short`, `unsigned short`, `int`, `unsigned int`, `long`, `unsigned long` e, com C++11, `long long` e `unsigned long long`. Ha tambem um tipo `wchar_t` cujo posicionamento nessa sequencia de tamanho depende da implementacao. O C++11 adiciona os tipos `char16_t` e `char32_t`, que sao amplos o suficiente para conter codigos de caractere de 16 bits e 32 bits, respectivamente.

O C++ garante que `char` e grande o suficiente para conter qualquer membro do conjunto de caracteres basico do sistema, `wchar_t` pode conter qualquer membro do conjunto de caracteres estendido do sistema, `short` tem pelo menos 16 bits, `int` e pelo menos tao grande quanto `short`, e `long` tem pelo menos 32 bits e e pelo menos tao grande quanto `int`.

Caracteres sao representados por seus codigos numericos. O sistema de E/S determina se um codigo e interpretado como um caractere ou como um numero.

Os tipos de ponto flutuante podem representar valores fracionarios e valores muito maiores que os inteiros podem representar. Os tres tipos de ponto flutuante sao `float`, `double` e `long double`. O C++ garante que `float` nao e maior que `double` e que `double` nao e maior que `long double`. Tipicamente, `float` usa 32 bits de memoria, `double` usa 64 bits e `long double` usa 80 a 128 bits.

Ao fornecer uma variedade de tipos em diferentes tamanhos e em variedades com sinal e sem sinal, o C++ permite que voce corresponda o tipo aos requisitos especificos de dados.

O C++ usa operadores para fornecer o suporte aritmetico usual para tipos numericos: adicao, subtracao, multiplicacao, divisao e obtencao do modulo. Quando dois operadores buscam operar sobre o mesmo valor, as regras de precedencia e associatividade do C++ determinam qual operacao ocorre primeiro.

O C++ converte valores de um tipo para outro quando voce atribui valores a uma variavel, mistura tipos na aritmetica e usa type casts para forcar conversoes de tipo. Muitas conversoes de tipo sao "seguras", o que significa que voce pode faze-las sem perda ou alteracao de dados. Por exemplo, voce pode converter um valor `int` para um valor `long` sem problemas. Outras, como conversoes de tipos de ponto flutuante para tipos inteiros, requerem mais cuidado.

A principio, voce pode achar o grande numero de tipos basicos do C++ um pouco excessivo, particularmente quando voce leva em conta as varias regras de conversao. Mas provavelmente voce acabara encontrando ocasioes em que um dos tipos e exatamente o que voce precisa no momento, e agradecera ao C++ por te-lo.

## Revisao do Capitulo

1. Por que o C++ tem mais de um tipo inteiro?
2. Declare variaveis que correspondam as seguintes descricoes:
   - a. Um inteiro `short` com o valor 80
   - b. Um inteiro `unsigned int` com o valor 42110
   - c. Um inteiro com o valor 3.000.000.000
3. Que salvaguardas o C++ fornece para impedir que voce exceda os limites de um tipo inteiro?
4. Qual e a distincao entre `33L` e `33`?
5. Considere as duas instrucoes C++ que seguem:
   ```cpp
   char nota = 65;
   char nota = 'A';
   ```
   Elas sao equivalentes?
6. Como voce poderia usar o C++ para descobrir qual caractere o codigo 88 representa? Crie pelo menos duas maneiras.
7. Atribuir um valor `long` a um `float` pode resultar em erro de arredondamento. E quanto a atribuir `long` a `double`? `long long` a `double`?
8. Avalie as seguintes expressoes como o C++ faria:
   - a. `8 * 9 + 2`
   - b. `6 * 3 / 4`
   - c. `3 / 4 * 6`
   - d. `6.0 * 3 / 4`
   - e. `15 % 4`
9. Suponha que `x1` e `x2` sejam duas variaveis do tipo `double` que voce quer adicionar como inteiros e atribuir a uma variavel inteira. Construa uma instrucao C++ para fazer isso. E se voce quiser adicioná-las como tipo `double` e entao converter para `int`?
10. Qual e o tipo de variavel para cada uma das seguintes declaracoes?
    - a. `auto carros = 15;`
    - b. `auto debito = 150.37f;`
    - c. `auto nivel = 'B';`
    - d. `auto crat = U'\U00002155';`
    - e. `auto frac = 8.25f / 2.5;`

## Exercicios de Programacao

1. Escreva um programa curto que peca sua altura em polegadas inteiras e entao a converta para pes e polegadas. Faca o programa usar o caractere de sublinhado para indicar onde digitar a resposta. Alem disso, use uma constante simbolica `const` para representar o fator de conversao.

2. Escreva um programa curto que peca sua altura em pes e polegadas e seu peso em libras. (Use tres variaveis para armazenar as informacoes.) Faca o programa relatar seu indice de massa corporal (IMC). Para calcular o IMC, primeiro converta sua altura em pes e polegadas para altura em polegadas (1 pe = 12 polegadas). Em seguida, converta sua altura em polegadas para sua altura em metros multiplicando por 0,0254. Em seguida, converta seu peso em libras em sua massa em quilogramas dividindo por 2,2. Por fim, calcule seu IMC dividindo sua massa em quilogramas pelo quadrado de sua altura em metros. Use constantes simbolicas para representar os varios fatores de conversao.

3. Escreva um programa que peca ao usuario para inserir uma latitude em graus, minutos e segundos e que entao exiba a latitude em formato decimal. Ha 60 segundos de arco em um minuto e 60 minutos de arco em um grau; represente esses valores com constantes simbolicas. Voce deve usar uma variavel separada para cada valor de entrada. Uma execucao de exemplo deve se parecer com isto:
   ```
   Informe uma latitude em graus, minutos e segundos:
   Primeiro, informe os graus: 37
   Em seguida, informe os minutos de arco: 51
   Por fim, informe os segundos de arco: 19
   37 graus, 51 minutos, 19 segundos = 37.8553 graus
   ```

4. Escreva um programa que peca ao usuario para inserir o numero de segundos como um valor inteiro (use o tipo `long`, ou, se disponivel, `long long`) e que entao exiba o tempo equivalente em dias, horas, minutos e segundos. Use constantes simbolicas para representar o numero de horas em um dia, o numero de minutos em uma hora e o numero de segundos em um minuto. A saida deve se parecer com isto:
   ```
   Informe o numero de segundos: 31600000
   31600000 segundos = 365 dias, 17 horas, 46 minutos, 40 segundos
   ```

5. Escreva um programa que peca ao usuario para inserir a populacao atual mundial e a populacao atual do Brasil (ou de outra nacao de sua escolha). Armazene as informacoes em variaveis do tipo `long long`. Faca o programa exibir o percentual que a populacao do Brasil representa da populacao mundial. A saida deve se parecer com algo assim:
   ```
   Informe a populacao mundial: 6898758899
   Informe a populacao do Brasil: 214300000
   A populacao do Brasil e 3.10508% da populacao mundial.
   ```

6. Escreva um programa que pergunte quantos quilometros voce dirigiu e quantos litros de gasolina voce usou e entao reporte os quilometros por litro que seu carro fez. Ou, se preferir, o programa pode solicitar a distancia em milhas e o combustivel em galoes e entao relatar o resultado em milhas por galao.

7. Escreva um programa que peca voce para inserir uma figura de consumo de combustivel de automovel no estilo europeu (litros por 100 quilometros) e que converta para o estilo americano de milhas por galao. Note que, alem de usar diferentes unidades de medicao, a abordagem americana (distancia / combustivel) e o inverso da abordagem europeia (combustivel / distancia). Note que 100 quilometros e 62,14 milhas, e 1 galao e 3,875 litros. Assim, 19 mpg e cerca de 12,4 l/100 km, e 27 mpg e cerca de 8,7 l/100 km.

---

> Navegacao: [Anterior](capitulo-02.md) | [Indice](README.md) | [Proximo](capitulo-04.md)
