# Capitulo 4 - Tipos Compostos (Compound Types)
## Parte 2: Strings no Estilo C

> Traducao nao oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Navegacao: [Anterior](capitulo-04-01-arrays.md) | [Indice](README.md) | [Proximo](capitulo-04-03-classe-string.md)

---

## Strings

Uma **string** (cadeia de caracteres) e uma serie de caracteres armazenados em bytes consecutivos de memoria. O C++ tem duas formas de lidar com strings. A primeira, herdada do C e frequentemente chamada de string no estilo C, e a primeira que este capitulo examina. Mais adiante, o capitulo discute um metodo alternativo baseado na biblioteca de classes `string`.

A ideia de uma serie de caracteres armazenados em bytes consecutivos implica que voce pode armazenar uma string em um array de `char`, com cada caractere mantido em seu proprio elemento do array. As strings oferecem uma forma conveniente de armazenar informacoes de texto, como mensagens para o usuario ("Por favor, informe o numero da sua conta bancaria secreta na Suica") ou respostas do usuario ("Voce deve estar brincando"). As strings no estilo C tem uma caracteristica especial: o ultimo caractere de toda string e o **caractere nulo** (null character). Esse caractere, escrito como `\0`, e o caractere com codigo ASCII 0, e serve para marcar o fim da string. Por exemplo, considere as duas declaracoes a seguir:

```cpp
char cachorro[8] = { 'b', 'e', 'a', 'u', 'x', ' ', 'I', 'I'}; // nao e uma string!
char gato[8] = {'f', 'a', 't', 'e', 's', 's', 'a', '\0'};      // uma string!
```

Ambos os arrays sao arrays de `char`, mas apenas o segundo e uma string. O caractere nulo desempenha um papel fundamental nas strings no estilo C. Por exemplo, o C++ tem muitas funcoes que lidam com strings, incluindo as usadas por `cout`. Todas elas funcionam processando a string caractere por caractere ate chegar ao caractere nulo. Se voce pedir para `cout` exibir uma string bonita como `gato` no exemplo acima, ele exibe os primeiros sete caracteres, detecta o caractere nulo e para. Mas se voce mandar `cout` exibir o array `cachorro` do exemplo acima, que nao e uma string, `cout` imprime os oito caracteres do array e continua avancando pela memoria byte a byte, interpretando cada byte como um caractere a ser impresso, ate encontrar um caractere nulo. Como caracteres nulos, que na verdade sao bytes definidos como zero, tendem a ser comuns na memoria, o dano geralmente e contido rapidamente; ainda assim, voce nao deve tratar arrays de caracteres que nao sao strings como strings.

O exemplo do array `gato` torna a inicializacao de um array como string algo tedioso — todas aquelas aspas simples e ainda ter que lembrar do caractere nulo. Nao se preocupe. Ha uma maneira melhor de inicializar um array de caracteres como string. Basta usar uma string entre aspas, chamada de **constante de string** (string constant) ou **literal de string** (string literal), como no exemplo a seguir:

```cpp
char passaro[11] = "Sr. Gorjeia";  // o \0 e automatico
char peixe[] = "Bolhas";           // deixa o compilador contar
```

Strings entre aspas sempre incluem o caractere nulo terminador implicitamente, entao voce nao precisa digitá-lo explicitamente. Alem disso, os varios recursos de entrada do C++ para leitura de uma string do teclado em um array de `char` adicionam automaticamente o caractere nulo terminador para voce.

Claro, voce deve garantir que o array seja grande o suficiente para conter todos os caracteres da string, incluindo o caractere nulo. Inicializar um array de caracteres com uma constante de string e um caso em que pode ser mais seguro deixar o compilador contar o numero de elementos. Nao ha nenhum problema, alem do desperdicio de espaco, em fazer um array maior que a string. Isso porque as funcoes que trabalham com strings sao guiadas pela localizacao do caractere nulo, nao pelo tamanho do array. O C++ nao impoe limites ao comprimento de uma string.

> **Cuidado**
>
> Ao determinar o tamanho minimo do array necessario para armazenar uma string, lembre-se de incluir o caractere nulo terminador na sua contagem.

Note que uma constante de string (com aspas duplas) nao e intercambivel com uma constante de caractere (com aspas simples). Uma constante de caractere, como `'S'`, e uma notacao abreviada para o codigo de um caractere. Em um sistema ASCII, `'S'` e apenas outra forma de escrever `83`. Portanto, a instrucao a seguir atribui o valor `83` a `tamanho_camisa`:

```cpp
char tamanho_camisa = 'S'; // isso esta correto
```

Mas `"S"` nao e uma constante de caractere; representa a string composta por dois caracteres, `S` e `\0`. Pior ainda, `"S"` na verdade representa o endereco de memoria no qual a string esta armazenada. Portanto, uma instrucao como a seguir tenta atribuir um endereco de memoria a `tamanho_camisa`:

```cpp
char tamanho_camisa = "S"; // tipo incompativel, invalido
```

Como um endereco e um tipo separado em C++, o compilador nao permitira esse tipo de erro. (Voltaremos a esse ponto mais adiante neste capitulo, apos discutirmos ponteiros.)

## Concatenando Literais de String

As vezes uma string pode ser longa demais para caber convenientemente em uma linha de codigo. O C++ permite concatenar literais de string — ou seja, combinar duas strings entre aspas em uma unica. De fato, quaisquer duas constantes de string separadas apenas por espaco em branco (espacos, tabulacoes e novas linhas) sao automaticamente unidas em uma. Assim, todas as instrucoes de saida a seguir sao equivalentes entre si:

```cpp
cout << "Eu daria meu braco direito para ser" " um grande violinista.\n";
cout << "Eu daria meu braco direito para ser um grande violinista.\n";
cout << "Eu daria meu braco"
     " direito para ser um grande violinista.\n";
```

Note que a uniao nao adiciona nenhum espaco as strings unidas. O primeiro caractere da segunda string vem imediatamente apos o ultimo caractere (sem contar o `\0`) da primeira string. O caractere `\0` da primeira string e substituido pelo primeiro caractere da segunda string.

## Usando Strings em um Array

As duas formas mais comuns de colocar uma string em um array sao inicializar o array com uma constante de string e ler a entrada do teclado ou de arquivo em um array. A Listagem 4.2 demonstra essas abordagens: inicializando um array com uma string entre aspas e usando `cin` para colocar uma string de entrada em um segundo array. O programa tambem usa a funcao padrao da biblioteca C `strlen()` para obter o comprimento de uma string. O arquivo de cabecalho padrao `cstring` (ou `string.h` para implementacoes mais antigas) fornece declaracoes para essa e muitas outras funcoes relacionadas a strings.

**Listagem 4.2 — strings.cpp**

```cpp
// strings.cpp -- armazenando strings em um array
#include <iostream>
#include <cstring>  // para a funcao strlen()
int main()
{
    using namespace std;
    const int Tamanho = 15;
    char nome1[Tamanho];              // array vazio
    char nome2[Tamanho] = "C++owboy"; // array inicializado
    // NOTA: algumas implementacoes podem exigir a palavra-chave static
    // para inicializar o array nome2
    cout << "Ola! Eu sou " << nome2;
    cout << "! Qual e o seu nome?\n";
    cin >> nome1;
    cout << "Bem, " << nome1 << ", seu nome tem ";
    cout << strlen(nome1) << " letras e esta armazenado\n";
    cout << "em um array de " << sizeof(nome1) << " bytes.\n";
    cout << "Sua inicial e " << nome1[0] << ".\n";
    nome2[3] = '\0';   // define como caractere nulo
    cout << "Aqui estao os primeiros 3 caracteres do meu nome: ";
    cout << nome2 << endl;
    return 0;
}
```

Aqui esta uma execucao de exemplo do programa na Listagem 4.2:

```
Ola! Eu sou C++owboy! Qual e o seu nome?
Basicman
Bem, Basicman, seu nome tem 8 letras e esta armazenado
em um array de 15 bytes.
Sua inicial e B.
Aqui estao os primeiros 3 caracteres do meu nome: C++
```

**Notas do Programa**

O que voce pode aprender com a Listagem 4.2? Primeiro, note que o operador `sizeof` retorna o tamanho do array inteiro, 15 bytes, mas a funcao `strlen()` retorna o tamanho da string armazenada no array e nao o tamanho do array em si. Alem disso, `strlen()` conta apenas os caracteres visiveis e nao o caractere nulo. Portanto, ela retorna um valor de 8, nao de 9, para o comprimento de `Basicman`. Se `cosmico` e uma string, o tamanho minimo do array para armazená-la e `strlen(cosmico) + 1`.

Como `nome1` e `nome2` sao arrays, voce pode usar um indice para acessar caracteres individuais no array. Por exemplo, o programa usa `nome1[0]` para encontrar o primeiro caractere nesse array. O programa tambem define `nome2[3]` como o caractere nulo. Isso faz com que a string termine apos tres caracteres, mesmo que mais caracteres permanecam no array.

Note que o programa na Listagem 4.2 usa uma constante simbolica para o tamanho do array. Frequentemente o tamanho de um array aparece em varias instrucoes de um programa. Usar uma constante simbolica para representar o tamanho de um array simplifica a revisao do programa para usar um tamanho diferente; voce so precisa mudar o valor uma vez, onde a constante simbolica e definida.

## Aventuras na Entrada de Strings

O programa `strings.cpp` tem uma falha que e ocultada atraves da tecnica frequentemente util de selecionar cuidadosamente a entrada de exemplo. A Listagem 4.3 remove os veus e mostra que a entrada de strings pode ser complicada.

**Listagem 4.3 — instr1.cpp**

```cpp
// instr1.cpp -- lendo mais de uma string
#include <iostream>
int main()
{
    using namespace std;
    const int TamArr = 20;
    char nome[TamArr];
    char sobremesa[TamArr];
    cout << "Digite seu nome:\n";
    cin >> nome;
    cout << "Digite sua sobremesa favorita:\n";
    cin >> sobremesa;
    cout << "Tenho uma deliciosa " << sobremesa;
    cout << " para voce, " << nome << ".\n";
    return 0;
}
```

A intencao do programa na Listagem 4.3 e simples: ler o nome do usuario e a sobremesa favorita do teclado e, em seguida, exibir as informacoes. Aqui esta uma execucao de exemplo:

```
Digite seu nome:
Alistair Dreeb
Digite sua sobremesa favorita:
Tenho uma deliciosa Dreeb para voce, Alistair.
```

Nem tivemos a chance de responder ao prompt de sobremesa! O programa o exibiu e imediatamente passou para a exibicao da linha final.

O problema esta em como `cin` determina quando voce terminou de digitar uma string. Voce nao consegue inserir o caractere nulo pelo teclado, portanto `cin` precisa de outro meio para localizar o fim de uma string. A tecnica do `cin` e usar espaco em branco — espacos, tabulacoes e novas linhas — para delimitar uma string. Isso significa que `cin` le apenas uma palavra quando obtem entrada para um array de caracteres. Apos ler essa palavra, `cin` adiciona automaticamente o caractere nulo terminador ao colocar a string no array.

O resultado pratico neste exemplo e que `cin` le `Alistair` como a primeira string inteira e a coloca no array `nome`. Isso deixa o pobre `Dreeb` ainda na fila de entrada. Quando `cin` pesquisa a fila de entrada em busca de uma resposta para a pergunta sobre a sobremesa favorita, ele encontra `Dreeb` ainda la. Entao `cin` consume `Dreeb` e o coloca no array `sobremesa`.

Outro problema, que nao apareceu na execucao de exemplo, e que a string de entrada pode ser mais longa que o array de destino. Usar `cin` como neste exemplo nao oferece nenhuma protecao contra colocar uma string de 30 caracteres em um array de 20 caracteres.

Muitos programas dependem de entrada de strings, portanto vale a pena explorar mais esse topico. Teremos que recorrer a alguns recursos mais avancados do `cin`, que sao descritos no Capitulo 17, "Entrada, Saida e Arquivos".

## Lendo Entrada de String uma Linha por Vez

Ler a entrada de string uma palavra por vez frequentemente nao e a escolha mais desejavel. Por exemplo, suponha que um programa peca ao usuario que digite uma cidade, e o usuario responde com `Nova York` ou `Sao Paulo`. Voce gostaria que o programa lesse e armazenasse os nomes completos, e nao apenas `Nova` e `Sao`. Para poder inserir frases inteiras em vez de palavras isoladas como string, voce precisa de uma abordagem diferente para a entrada de strings. Especificamente, voce precisa de um metodo orientado a linha em vez de um metodo orientado a palavra. Voce tem sorte, pois a classe `istream`, da qual `cin` e um exemplo, tem alguns metodos de classe orientados a linha: `getline()` e `get()`. Ambos lem uma linha inteira de entrada — ou seja, ate o caractere de nova linha. No entanto, `getline()` descarta o caractere de nova linha, enquanto `get()` o deixa na fila de entrada. Vamos examinar os detalhes, comecando com `getline()`.

## Entrada Orientada a Linha com getline()

A funcao `getline()` le uma linha inteira, usando o caractere de nova linha transmitido pela tecla Enter para marcar o fim da entrada. Voce invoca esse metodo usando `cin.getline()` como uma chamada de funcao. A funcao recebe dois argumentos. O primeiro argumento e o nome do destino (ou seja, o array destinado a armazenar a linha de entrada), e o segundo argumento e um limite para o numero de caracteres a serem lidos. Se esse limite for, digamos, `20`, a funcao le no maximo 19 caracteres, deixando espaco para adicionar automaticamente o caractere nulo no final. A funcao membro `getline()` para de ler a entrada quando atinge esse limite numerico ou quando le um caractere de nova linha, o que ocorrer primeiro.

Por exemplo, suponha que voce queira usar `getline()` para ler um nome no array `nome` de 20 elementos. Voce usaria esta chamada:

```cpp
cin.getline(nome, 20);
```

Isso le a linha inteira no array `nome`, desde que a linha consista de 19 ou menos caracteres. (A funcao membro `getline()` tambem tem um terceiro argumento opcional, que o Capitulo 17 discute.)

A Listagem 4.4 modifica a Listagem 4.3 para usar `cin.getline()` em vez de um simples `cin`. Caso contrario, o programa nao e alterado.

**Listagem 4.4 — instr2.cpp**

```cpp
// instr2.cpp -- lendo mais de uma palavra com getline
#include <iostream>
int main()
{
    using namespace std;
    const int TamArr = 20;
    char nome[TamArr];
    char sobremesa[TamArr];
    cout << "Digite seu nome:\n";
    cin.getline(nome, TamArr);      // le ate a nova linha
    cout << "Digite sua sobremesa favorita:\n";
    cin.getline(sobremesa, TamArr);
    cout << "Tenho uma deliciosa " << sobremesa;
    cout << " para voce, " << nome << ".\n";
    return 0;
}
```

Aqui esta uma saida de exemplo para a Listagem 4.4:

```
Digite seu nome:
Dirk Hammernose
Digite sua sobremesa favorita:
Torta de Rabanete
Tenho uma deliciosa Torta de Rabanete para voce, Dirk Hammernose.
```

O programa agora le nomes completos e entrega ao usuario a sobremesa merecida! A funcao `getline()` obtem convenientemente uma linha por vez. Ela le a entrada ate o caractere de nova linha que marca o fim da linha, mas nao salva o caractere de nova linha. Em vez disso, ela o substitui por um caractere nulo ao armazenar a string.

## Entrada Orientada a Linha com get()

Vamos tentar outra abordagem. A classe `istream` tem outra funcao membro, `get()`, que existe em diversas variacoes. Uma variante funciona de forma muito semelhante a `getline()`. Ela recebe os mesmos argumentos, os interpreta da mesma forma e le ate o final de uma linha. Mas em vez de ler e descartar o caractere de nova linha, `get()` deixa esse caractere na fila de entrada. Suponha que voce use duas chamadas a `get()` em sequencia:

```cpp
cin.get(nome, TamArr);
cin.get(sobremesa, TamArr); // um problema
```

Como a primeira chamada deixa o caractere de nova linha na fila de entrada, esse caractere e o primeiro que a segunda chamada ve. Assim, `get()` conclui que chegou ao fim da linha sem ter encontrado nada para ler. Sem ajuda, `get()` simplesmente nao consegue passar por esse caractere de nova linha.

Felizmente, ha ajuda na forma de uma variacao de `get()`. A chamada `cin.get()` (sem argumentos) le o proximo caractere, mesmo que seja uma nova linha, entao voce pode usá-la para descartar o caractere de nova linha e se preparar para a proxima linha de entrada. Ou seja, esta sequencia funciona:

```cpp
cin.get(nome, TamArr);      // le a primeira linha
cin.get();                  // le a nova linha
cin.get(sobremesa, TamArr); // le a segunda linha
```

Outra forma de usar `get()` e concatenar, ou encadear, as duas funcoes membro de classe, como a seguir:

```cpp
cin.get(nome, TamArr).get(); // concatenar funcoes membro
```

O que torna isso possivel e que `cin.get(nome, TamArr)` retorna o objeto `cin`, que e entao usado como o objeto que invoca a funcao `get()`. De forma similar, a instrucao a seguir le duas linhas consecutivas de entrada nos arrays `nome1` e `nome2`; e equivalente a fazer duas chamadas separadas a `cin.getline()`:

```cpp
cin.getline(nome1, TamArr).getline(nome2, TamArr);
```

A Listagem 4.5 usa concatenacao. No Capitulo 11, "Trabalhando com Classes", voce aprendera como incorporar esse recurso nas definicoes das suas proprias classes.

**Listagem 4.5 — instr3.cpp**

```cpp
// instr3.cpp -- lendo mais de uma palavra com get() & get()
#include <iostream>
int main()
{
    using namespace std;
    const int TamArr = 20;
    char nome[TamArr];
    char sobremesa[TamArr];
    cout << "Digite seu nome:\n";
    cin.get(nome, TamArr).get(); // le a string e a nova linha
    cout << "Digite sua sobremesa favorita:\n";
    cin.get(sobremesa, TamArr).get();
    cout << "Tenho uma deliciosa " << sobremesa;
    cout << " para voce, " << nome << ".\n";
    return 0;
}
```

Aqui esta uma execucao de exemplo do programa na Listagem 4.5:

```
Digite seu nome:
Mai Parfait
Digite sua sobremesa favorita:
Mousse de Chocolate
Tenho uma deliciosa Mousse de Chocolate para voce, Mai Parfait.
```

Uma coisa a notar e como o C++ permite multiplas versoes de funcoes, desde que elas tenham listas de argumentos diferentes. Se voce usa, por exemplo, `cin.get(nome, TamArr)`, o compilador percebe que voce esta usando a forma que coloca uma string em um array e configura a funcao membro adequada. Se, em vez disso, voce usa `cin.get()`, o compilador percebe que voce quer a forma que le um caractere. O Capitulo 8, "Aventuras em Funcoes", explora esse recurso, chamado de sobrecarga de funcao (function overloading).

Por que usar `get()` em vez de `getline()` afinal? Primeiro, implementacoes mais antigas podem nao ter `getline()`. Segundo, `get()` permite ser um pouco mais cuidadoso. Suponha, por exemplo, que voce tenha usado `get()` para ler uma linha em um array. Como saber se ele leu a linha inteira em vez de parar porque o array estava cheio? Olhe para o proximo caractere de entrada. Se for um caractere de nova linha, entao a linha inteira foi lida. Se nao for, ainda ha mais entrada nessa linha. O Capitulo 17 investiga essa tecnica. Resumindo, `getline()` e um pouco mais simples de usar, mas `get()` torna a verificacao de erros mais simples. Voce pode usar qualquer um para ler uma linha de entrada; apenas tenha em mente os comportamentos ligeiramente diferentes.

## Linhas Vazias e Outros Problemas

O que acontece depois que `getline()` ou `get()` le uma linha vazia? A pratica original era que a proxima instrucao de entrada continuava de onde a ultima `getline()` ou `get()` parou. No entanto, a pratica atual e que depois que `get()` (mas nao `getline()`) le uma linha vazia, ele define algo chamado `failbit`. As implicacoes desse ato sao que a entrada posterior e bloqueada, mas voce pode restaurar a entrada com o seguinte comando:

```cpp
cin.clear();
```

Outro problema potencial e que a string de entrada pode ser mais longa que o espaco alocado. Se a linha de entrada for mais longa que o numero de caracteres especificado, tanto `getline()` quanto `get()` deixam os caracteres restantes na fila de entrada. No entanto, `getline()` tambem define o `failbit` e desativa a entrada posterior.

Os Capitulos 5, 6 e 17 investigam essas propriedades e como contorná-las em programas.

## Combinando Entrada de Strings e Numerica

Combinar entrada numerica com entrada de string orientada a linha pode causar problemas. Considere o programa simples na Listagem 4.6.

**Listagem 4.6 — numstr.cpp**

```cpp
// numstr.cpp -- entrada numerica seguida de entrada de linha
#include <iostream>
int main()
{
    using namespace std;
    cout << "Em que ano sua casa foi construida?\n";
    int ano;
    cin >> ano;
    cout << "Qual e o endereco da rua?\n";
    char endereco[80];
    cin.getline(endereco, 80);
    cout << "Ano de construcao: " << ano << endl;
    cout << "Endereco: " << endereco << endl;
    cout << "Concluido!\n";
    return 0;
}
```

Executar o programa na Listagem 4.6 resultaria em algo assim:

```
Em que ano sua casa foi construida?
Qual e o endereco da rua?
Ano de construcao: 1966
Endereco:
Concluido!
```

Voce nao tem a oportunidade de inserir o endereco. O problema e que quando `cin` le o ano, ele deixa o caractere de nova linha gerado pela tecla Enter na fila de entrada. Em seguida, `cin.getline()` le a nova linha como uma linha vazia e atribui uma string nula ao array `endereco`. A solucao e ler e descartar a nova linha antes de ler o endereco. Isso pode ser feito de varias formas, incluindo o uso de `get()` com um argumento `char` ou sem argumentos, como descrito no exemplo anterior. Voce pode fazer essas chamadas separadamente:

```cpp
cin >> ano;
cin.get(); // ou cin.get(ch);
```

Ou voce pode concatenar as chamadas, aproveitando o fato de que a expressao `cin >> ano` retorna o objeto `cin`:

```cpp
(cin >> ano).get(); // ou (cin >> ano).get(ch);
```

Se voce fizer uma dessas alteracoes na Listagem 4.6, ela funciona corretamente:

```
Em que ano sua casa foi construida?
Qual e o endereco da rua?
43821 Rua Unsigned Short
Ano de construcao: 1966
Endereco: 43821 Rua Unsigned Short
Concluido!
```

Programas C++ frequentemente usam ponteiros em vez de arrays para lidar com strings. Abordaremos esse aspecto das strings apos falar um pouco sobre ponteiros. Enquanto isso, vamos dar uma olhada em uma forma mais recente de lidar com strings: a classe `string` do C++.

---

> Navegacao: [Anterior](capitulo-04-01-arrays.md) | [Indice](README.md) | [Proximo](capitulo-04-03-classe-string.md)
