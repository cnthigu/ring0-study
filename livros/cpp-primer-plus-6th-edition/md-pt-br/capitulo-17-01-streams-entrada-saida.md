# Capítulo 17 — Entrada, Saída e Arquivos (Input, Output, and Files)

> Tradução não oficial de *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Navegação: [Anterior](capitulo-16-04-lista-init-resumo.md) | [Índice](README.md) | [Próximo](capitulo-17-02-istream-estados-arquivo.md)

## Objetivos deste capítulo

Neste capítulo você aprenderá sobre os seguintes tópicos:

- A visão do C++ sobre entrada e saída
- A família de classes `iostream`
- Redirecionamento
- Métodos da classe `ostream`
- Formatação de saída
- Métodos da classe `istream`
- Estados de fluxo
- E/S de arquivos
- Uso da classe `ifstream` para entrada de arquivos
- Uso da classe `ofstream` para saída em arquivos
- Uso da classe `fstream` para entrada e saída em arquivos
- Processamento de argumentos de linha de comando
- Arquivos binários
- Acesso aleatório em arquivos
- Formatação interna (incore)

Discutir entrada e saída (E/S) em C++ apresenta um dilema. Por um lado, praticamente todo programa usa entrada e saída, e aprender como utilizá-las é uma das primeiras tarefas para quem está aprendendo uma linguagem de computador. Por outro lado, o C++ usa muitos de seus recursos mais avançados para implementar entrada e saída, incluindo classes, classes derivadas, sobrecarga de funções, funções virtuais, templates e herança múltipla. Portanto, para realmente compreender a E/S em C++, é preciso conhecer muito do C++. Para dar uma base inicial, os primeiros capítulos deste livro descreveram as formas básicas de usar o objeto `cin` da classe `istream` e o objeto `cout` da classe `ostream` para entrada e saída, e, em menor grau, o uso de objetos `ifstream` e `ofstream` para entrada e saída em arquivos. Este capítulo examina com mais profundidade as classes de entrada e saída do C++, mostrando como elas são projetadas e explicando como controlar o formato da saída. (Se você pulou alguns capítulos apenas para aprender formatação avançada, pode ler as seções de formatação, anotando as técnicas e ignorando as explicações.)

Os recursos do C++ para entrada e saída em arquivos se baseiam nas mesmas definições de classes em que `cin` e `cout` se apoiam, de modo que este capítulo usa a discussão sobre E/S de console (teclado e tela) como ponto de partida para investigar a E/S de arquivos.

O comitê de normas ANSI/ISO C++ trabalhou para tornar a E/S do C++ mais compatível com a E/S existente em C, e isso produziu algumas mudanças em relação às práticas tradicionais do C++.

## Uma Visão Geral da Entrada e Saída em C++

A maioria das linguagens de computador incorpora entrada e saída à própria linguagem. Por exemplo, ao examinar as palavras-chave de linguagens como BASIC e Pascal, encontram-se instruções `PRINT`, `writeln` e similares como parte do vocabulário da linguagem. Mas nem C nem C++ incorporam entrada e saída à linguagem. Ao examinar as palavras-chave dessas linguagens, encontra-se `for` e `if`, mas nada relacionado a E/S. O C originalmente deixou a E/S a cargo dos implementadores de compiladores. Uma das razões para isso era dar aos implementadores a liberdade de projetar funções de E/S mais adequadas aos requisitos de hardware do computador-alvo. Na prática, a maioria dos implementadores baseou a E/S em um conjunto de funções de biblioteca originalmente desenvolvido para o ambiente Unix. O ANSI C formalizou o reconhecimento desse pacote de E/S, denominado Pacote de Entrada/Saída Padrão (Standard Input/Output), tornando-o um componente obrigatório da biblioteca padrão do C. O C++ também reconhece esse pacote; portanto, se você está familiarizado com a família de funções do C declaradas no arquivo `stdio.h`, pode usá-las em programas C++. (As implementações mais recentes usam o arquivo de cabeçalho `cstdio` para suportar essas funções.)

Entretanto, o C++ utiliza uma solução C++ — não uma solução C — para E/S, e essa solução é um conjunto de classes definidas nos arquivos de cabeçalho `iostream` (antigamente `iostream.h`) e `fstream` (antigamente `fstream.h`). Essa biblioteca de classes não faz parte da definição formal da linguagem (`cin` e `istream` não são palavras-chave); afinal, uma linguagem de computador define regras sobre como fazer as coisas, como criar classes, e não define o que você deve criar seguindo essas regras. Mas assim como as implementações de C vêm acompanhadas de uma biblioteca padrão de funções, o C++ vem com uma biblioteca padrão de classes. A princípio, essa biblioteca padrão de classes era um padrão informal, constituído apenas das classes definidas nos arquivos de cabeçalho `iostream` e `fstream`. O comitê ANSI/ISO C++ decidiu formalizar essa biblioteca como uma biblioteca padrão de classes e acrescentar mais algumas classes padrão, como as discutidas no Capítulo 16, "A Classe `string` e a Biblioteca de Templates Padrão". Este capítulo discute a E/S padrão do C++. Mas primeiro, vamos examinar o arcabouço conceitual para a E/S em C++.

### Fluxos e Buffers

Um programa C++ enxerga a entrada ou saída como um fluxo (stream) de bytes. Na entrada, um programa extrai bytes de um fluxo de entrada, e na saída, um programa insere bytes em um fluxo de saída. Para um programa orientado a texto, cada byte pode representar um caractere. De modo mais geral, os bytes podem formar uma representação binária de dados de caractere ou numéricos. Os bytes de um fluxo de entrada podem vir do teclado, mas também podem vir de um dispositivo de armazenamento, como um disco rígido, ou de outro programa. Da mesma forma, os bytes de um fluxo de saída podem fluir para a tela, para uma impressora, para um dispositivo de armazenamento, ou para outro programa. Um fluxo atua como intermediário entre o programa e a origem ou destino do fluxo. Essa abordagem permite que um programa C++ trate a entrada proveniente de um teclado da mesma maneira que trata a entrada de um arquivo; o programa C++ simplesmente examina o fluxo de bytes sem precisar saber de onde eles vêm. Da mesma forma, usando fluxos, um programa C++ pode processar a saída de maneira independente de para onde os bytes estão indo. Gerenciar a entrada envolve dois estágios:

- Associar um fluxo a uma entrada do programa
- Conectar o fluxo a um arquivo

Em outras palavras, um fluxo de entrada precisa de duas conexões, uma em cada extremidade. A conexão do lado do arquivo fornece uma fonte para o fluxo, e a conexão do lado do programa despeja o fluxo de saída no programa. (A conexão do lado do arquivo pode ser um arquivo, mas também pode ser um dispositivo, como um teclado.) Da mesma forma, gerenciar a saída envolve conectar um fluxo de saída ao programa e associar algum destino de saída ao fluxo. É como encanamento com bytes em vez de água.

Geralmente, a entrada e a saída podem ser tratadas com mais eficiência usando um buffer (armazenamento intermediário). Um buffer é um bloco de memória usado como armazenamento intermediário temporário para a transferência de informações entre um dispositivo e um programa ou de um programa para um dispositivo. Tipicamente, dispositivos como drives de disco transferem informações em blocos de 512 bytes ou mais, enquanto os programas frequentemente processam informações 1 byte de cada vez. O buffer ajuda a equilibrar essas duas taxas distintas de transferência de informações. Por exemplo, suponha que um programa precise contar o número de cifrões em um arquivo de disco rígido. O programa poderia ler um caractere do arquivo, processá-lo, ler o próximo caractere do arquivo e assim por diante. Ler um arquivo caractere por caractere do disco exige muita atividade de hardware e é lento. A abordagem com buffer consiste em ler um grande bloco do disco, armazená-lo no buffer e lê-lo um caractere de cada vez. Como é muito mais rápido ler bytes individuais de dados da memória do que de um disco rígido, essa abordagem é muito mais rápida e menos desgastante para o hardware. É claro que, depois que o programa chega ao fim do buffer, deve então ler outro bloco de dados do disco. Na saída, um programa pode primeiro preencher o buffer e depois transferir todo o bloco de dados para um disco rígido, liberando o buffer para o próximo lote de saída. Isso é chamado de descarregar o buffer (flushing the buffer).

A entrada pelo teclado fornece um caractere de cada vez; portanto, nesse caso, um programa não precisa de um buffer para equilibrar diferentes taxas de transferência de dados. No entanto, a entrada de teclado com buffer permite ao usuário voltar e corrigir a entrada antes de transmiti-la ao programa. Um programa C++ normalmente descarrega o buffer de entrada quando o usuário pressiona Enter. É por isso que os exemplos deste livro não começam a processar a entrada até que você pressione Enter. Para a saída na tela, um programa C++ normalmente descarrega o buffer de saída ao transmitir um caractere de nova linha. Dependendo da implementação, o programa pode descarregar a entrada em outras ocasiões também, como quando há entrada pendente. Ou seja, quando um programa chega a uma instrução de entrada, ele descarrega qualquer saída atualmente no buffer de saída. As implementações de C++ compatíveis com o ANSI C devem se comportar dessa maneira.

### Fluxos, Buffers e o Arquivo `iostream`

O gerenciamento de fluxos e buffers pode ser um pouco complicado, mas incluir o arquivo `iostream` (antigamente `iostream.h`) em um programa traz diversas classes projetadas para implementar e gerenciar fluxos e buffers por você. A versão C++98 da E/S em C++ define templates de classe para suportar dados dos tipos `char` e `wchar_t`. O C++11 adiciona especializações para `char16_t` e `char32_t`. Usando o recurso `typedef`, o C++ faz as especializações para `char` desses templates imitar a implementação tradicional de E/S sem templates. Eis algumas dessas classes:

- A classe `streambuf` fornece memória para um buffer, juntamente com métodos de classe para preencher o buffer, acessar seu conteúdo, descarregar o buffer e gerenciar sua memória.
- A classe `ios_base` representa propriedades gerais de um fluxo, como se ele está aberto para leitura e se é um fluxo binário ou de texto.
- A classe `ios` é baseada em `ios_base` e inclui um membro ponteiro para um objeto `streambuf`.
- A classe `ostream` deriva da classe `ios` e fornece métodos de saída.
- A classe `istream` deriva da classe `ios` e fornece métodos de entrada.
- A classe `iostream` é baseada nas classes `istream` e `ostream` e herda tanto métodos de entrada quanto de saída.

Para usar essas facilidades, você utiliza objetos das classes apropriadas. Por exemplo, você usa um objeto `ostream` como `cout` para tratar a saída. Criar tal objeto abre um fluxo, cria automaticamente um buffer e o associa ao fluxo. Também disponibiliza para você as funções-membro da classe.

> **Nota — Redefinindo a E/S:** O padrão ISO/ANSI C++98 revisou a E/S de algumas formas. Primeiro, houve a mudança de `ostream.h` para `ostream`, com `ostream` colocando as classes no namespace `std`. Segundo, as classes de E/S foram reescritas. Para ser uma linguagem internacional, o C++ precisa ser capaz de lidar com conjuntos de caracteres internacionais que exigem um tipo de caractere de 16 bits ou mais largo. Assim, a linguagem adicionou o tipo `wchar_t` (ou caractere "largo") ao tipo `char` tradicional de 8 bits (ou "estreito"). O C++11 adiciona os tipos `char16_t` e `char32_t`. Cada tipo precisa de suas próprias instalações de E/S. Em vez de desenvolver dois (ou agora quatro) conjuntos distintos de classes, o comitê de normas desenvolveu um conjunto de templates de classes de E/S, incluindo `basic_istream<charT, traits<charT>>` e `basic_ostream<charT, traits<charT>>`. O template `traits<charT>`, por sua vez, é um template de classe que define características particulares de um tipo de caractere, como comparar por igualdade e seu valor de EOF. O padrão C++11 fornece especializações de `char` e `wchar_t` das classes de E/S. Por exemplo, `istream` e `ostream` são `typedef`s para especializações de `char`. Da mesma forma, `wistream` e `wostream` são especializações de `wchar_t`. Por exemplo, existe um objeto `wcout` para saída de fluxos de caracteres largos. O arquivo de cabeçalho `ostream` contém essas definições. Certas informações independentes de tipo que costumavam ser mantidas na classe `ios` foram movidas para a nova classe `ios_base`. Isso inclui as diversas constantes de formatação, como `ios::fixed`, que agora é `ios_base::fixed`. Além disso, `ios_base` contém algumas opções que não estavam disponíveis no antigo `ios`.

A biblioteca de classes `iostream` do C++ cuida de muitos detalhes por você. Por exemplo, incluir o arquivo `iostream` em um programa cria automaticamente oito objetos de fluxo (quatro para fluxos de caracteres estreitos e quatro para fluxos de caracteres largos):

- O objeto `cin` corresponde ao fluxo de entrada padrão. Por padrão, esse fluxo está associado ao dispositivo de entrada padrão, tipicamente o teclado. O objeto `wcin` é similar, mas trabalha com o tipo `wchar_t`.
- O objeto `cout` corresponde ao fluxo de saída padrão. Por padrão, esse fluxo está associado ao dispositivo de saída padrão, tipicamente um monitor. O objeto `wcout` é similar, mas trabalha com o tipo `wchar_t`.
- O objeto `cerr` corresponde ao fluxo de erro padrão, que pode ser usado para exibir mensagens de erro. Por padrão, esse fluxo está associado ao dispositivo de saída padrão, tipicamente um monitor, e o fluxo não é armazenado em buffer. Isso significa que as informações são enviadas diretamente para a tela, sem aguardar o preenchimento de um buffer ou a ocorrência de um caractere de nova linha. O objeto `wcerr` é similar, mas trabalha com o tipo `wchar_t`.
- O objeto `clog` também corresponde ao fluxo de erro padrão. Por padrão, esse fluxo está associado ao dispositivo de saída padrão, tipicamente um monitor, e o fluxo é armazenado em buffer. O objeto `wclog` é similar, mas trabalha com o tipo `wchar_t`.

O que significa dizer que um objeto representa um fluxo? Por exemplo, quando o arquivo `iostream` declara um objeto `cout` para um programa, esse objeto contém membros de dados que armazenam informações relacionadas à saída, como as larguras de campo a serem usadas na exibição de dados, o número de casas decimais, qual base numérica usar para exibir inteiros, e o endereço de um objeto `streambuf` que descreve o buffer usado para lidar com o fluxo de saída. Uma instrução como a seguinte coloca os caracteres da string `"Bjarne free"` no buffer gerenciado por `cout` por meio do objeto `streambuf` apontado:

```cpp
cout << "Bjarne free";
```

A classe `ostream` define a função `operator<<()` usada nessa instrução, e a classe `ostream` também oferece suporte aos membros de dados de `cout` por meio de vários outros métodos de classe, como os discutidos mais adiante neste capítulo. Além disso, o C++ garante que a saída do buffer seja direcionada para a saída padrão, geralmente um monitor, fornecida pelo sistema operacional. Em resumo, uma extremidade de um fluxo está conectada ao programa, a outra está conectada à saída padrão, e o objeto `cout`, com a ajuda de um objeto `streambuf`, gerencia o fluxo de bytes pelo fluxo.

### Redirecionamento

Os fluxos de entrada e saída padrão normalmente se conectam ao teclado e à tela. Mas muitos sistemas operacionais, incluindo Unix, Linux e Windows, suportam o redirecionamento (redirection), uma facilidade que permite mudar as associações da entrada padrão e da saída padrão. Suponha, por exemplo, que você tenha um programa C++ executável no prompt de comando do Windows chamado `contador.exe` que conta o número de caracteres em sua entrada e reporta o resultado. Uma execução de exemplo pode ser assim:

```
C>contador
Hello
and goodbye!
Control-Z  << fim de arquivo simulado
A entrada continha 19 caracteres.
C>
```

Neste caso, a entrada veio do teclado e a saída foi para a tela.

Com o redirecionamento de entrada (`<`) e de saída (`>`), você pode usar o mesmo programa para contar o número de caracteres no arquivo `oklahoma` e colocar os resultados no arquivo `cont_vacas`:

```
C>contador <oklahoma >cont_vacas
C>
```

A parte `<oklahoma` da linha de comando associa a entrada padrão ao arquivo `oklahoma`, fazendo com que `cin` leia a entrada desse arquivo em vez do teclado. Em outras palavras, o sistema operacional muda a conexão na extremidade de influxo do fluxo de entrada, enquanto a extremidade de saída permanece conectada ao programa. A parte `>cont_vacas` da linha de comando associa a saída padrão ao arquivo `cont_vacas`, fazendo com que `cout` envie a saída para esse arquivo em vez de para a tela. Isto é, o sistema operacional muda a conexão de saída do fluxo de saída, deixando sua extremidade de influxo ainda conectada ao programa. O DOS, o modo de prompt de comando do Windows, Linux e Unix reconhecem automaticamente essa sintaxe de redirecionamento. (Todos esses, exceto as formas antigas do DOS, permitem espaços opcionais entre os operadores de redirecionamento e os nomes de arquivo.)

O fluxo de saída padrão, representado por `cout`, é o canal normal para a saída do programa. Os fluxos de erro padrão (representados por `cerr` e `clog`) destinam-se às mensagens de erro do programa. Por padrão, todos esses três objetos são normalmente enviados para o monitor. Mas redirecionar a saída padrão não afeta `cerr` nem `clog`; portanto, se você usar um desses objetos para imprimir uma mensagem de erro, um programa exibirá a mensagem na tela mesmo que a saída regular de `cout` esteja redirecionada para outro lugar. Por exemplo, considere este fragmento de código:

```cpp
if (sucesso)
    std::cout << "Aqui vêm as novidades!\n";
else
{
    std::cerr << "Algo terrível aconteceu.\n";
    exit(1);
}
```

Se o redirecionamento não estiver em vigor, qualquer que seja a mensagem selecionada, ela será exibida na tela. Se, no entanto, a saída do programa tiver sido redirecionada para um arquivo, a primeira mensagem, se selecionada, iria para o arquivo, mas a segunda mensagem, se selecionada, iria para a tela. Aliás, alguns sistemas operacionais também permitem o redirecionamento do erro padrão. No Unix e Linux, por exemplo, o operador `2>` redireciona o erro padrão.

## Saída com `cout`

Como mencionado anteriormente, o C++ considera a saída como um fluxo de bytes. Mas muitos tipos de dados em um programa são organizados em unidades maiores do que um único byte. Um tipo `int`, por exemplo, pode ser representado por um valor binário de 16 ou 32 bits. E um valor `double` pode ser representado por 64 bits de dados binários. Mas quando você envia um fluxo de bytes para uma tela, deseja que cada byte represente um valor de caractere. Ou seja, para exibir o número `-2.34` na tela, você deveria enviar os cinco caracteres `-`, `2`, `.`, `3` e `4` para a tela, e não a representação interna de 64 bits em ponto flutuante desse valor. Portanto, uma das tarefas mais importantes da classe `ostream` é converter tipos numéricos, como `int` ou `float`, em um fluxo de caracteres que representa os valores em forma de texto. Ou seja, a classe `ostream` traduz a representação interna dos dados como padrões de bits binários em um fluxo de saída de bytes de caracteres. Para realizar essas tarefas de tradução, a classe `ostream` fornece vários métodos de classe. Vamos examiná-los agora, resumindo os métodos usados ao longo do livro e descrevendo métodos adicionais que oferecem controle mais fino sobre a aparência da saída.

### O Operador `<<` Sobrecarregado

Na maioria das vezes, este livro usou `cout` com o operador `<<`, também chamado de operador de inserção (insertion operator):

```cpp
int clientes = 22;
cout << clientes;
```

Em C++, assim como em C, por padrão o operador `<<` é usado como operador de deslocamento à esquerda de bits (ver Apêndice E, "Outros Operadores"). Uma expressão como `x << 3` significa tomar a representação binária de `x` e deslocar todos os bits três posições à esquerda. Obviamente, isso não tem muito a ver com saída. Mas a classe `ostream` redefine o operador `<<` por meio de sobrecarga para uso com a classe `ostream`. Nesse papel, o operador `<<` é chamado de operador de inserção em vez de operador de deslocamento à esquerda. (O operador de deslocamento à esquerda ganhou essa nova função por seu aspecto visual, que sugere um fluxo de informações para a esquerda.) O operador de inserção é sobrecarregado para reconhecer todos os tipos básicos do C++:

- `unsigned char`
- `signed char`
- `char`
- `short`
- `unsigned short`
- `int`
- `unsigned int`
- `long`
- `unsigned long`
- `long long` (C++11)
- `unsigned long long` (C++11)
- `float`
- `double`
- `long double`

A classe `ostream` fornece uma definição para a função `operator<<()` para cada um desses tipos de dados. (Funções que têm `operator` em seus nomes são usadas para sobrecarregar operadores, conforme discutido no Capítulo 11, "Trabalhando com Classes".) Portanto, se você usar uma instrução da seguinte forma, e `valor` for um dos tipos listados anteriormente, um programa C++ pode associá-la a uma função operador com a assinatura correspondente:

```cpp
cout << valor;
```

Por exemplo, a expressão `cout << 88` corresponde ao seguinte protótipo de método:

```cpp
ostream & operator<<(int);
```

Lembre-se de que esse protótipo indica que a função `operator<<()` recebe um argumento do tipo `int`. Esse é o componente que corresponde ao `88` na instrução anterior. O protótipo também indica que a função retorna uma referência a um objeto `ostream`. Essa propriedade torna possível concatenar a saída:

```cpp
cout << "I'm feeling sedimental over " << boundary << "\n";
```

Se você é um programador C que sofreu com os inúmeros especificadores `%tipo` do C e os problemas que surgem quando você faz uma incompatibilidade entre um tipo de especificador e um valor, usar `cout` é quase singelamente fácil.

### Saída e Ponteiros

A classe `ostream` define funções do operador de inserção para os seguintes tipos de ponteiro:

- `const signed char *`
- `const unsigned char *`
- `const char *`
- `void *`

O C++ representa uma string, não se esqueça, usando um ponteiro para a localização da string. O ponteiro pode assumir a forma do nome de um array de `char`, ou de um ponteiro explícito para `char`, ou de uma string entre aspas. Portanto, todas as instruções `cout` a seguir exibem strings:

```cpp
char nome[20] = "Dudly Diddlemore";
char * pn = "Violet D'Amore";
cout << "Olá!";
cout << nome;
cout << pn;
```

Os métodos usam o caractere nulo de terminação na string para determinar quando parar de exibir caracteres.

O C++ associa um ponteiro de qualquer outro tipo ao tipo `void *` e imprime uma representação numérica do endereço. Se você quiser o endereço da string, deve convertê-la para outro tipo, como mostrado no seguinte fragmento de código:

```cpp
int ovos = 12;
char * quantidade = "dezena";
cout << &ovos;               // imprime o endereço da variável ovos
cout << quantidade;          // imprime a string "dezena"
cout << (void *) quantidade; // imprime o endereço da string "dezena"
```

### Concatenação de Saída

Todas as versões do operador de inserção são definidas para retornar o tipo `ostream &`. Ou seja, os protótipos têm esta forma:

```cpp
ostream & operator<<(tipo);
```

(Aqui, `tipo` é o tipo a ser exibido.) O tipo de retorno `ostream &` significa que usar esse operador retorna uma referência a um objeto `ostream`. Qual objeto? As definições de função dizem que a referência é para o objeto usado para invocar o operador. Em outras palavras, o valor de retorno de uma função operador é o mesmo objeto que invoca o operador. Por exemplo, `cout << "jantar"` retorna o objeto `cout`. Essa é a propriedade que permite concatenar a saída por meio de inserção. Por exemplo, considere a seguinte instrução:

```cpp
cout << "Temos " << contagem << " frangos não nascidos.\n";
```

A expressão `cout << "Temos "` exibe a string e retorna o objeto `cout`, reduzindo a instrução a:

```cpp
cout << contagem << " frangos não nascidos.\n";
```

Em seguida, a expressão `cout << contagem` exibe o valor da variável `contagem` e retorna `cout`, que pode então lidar com o último argumento da instrução. Essa técnica de design é realmente um belo recurso, razão pela qual os exemplos de sobrecarga do operador `<<` nos capítulos anteriores imitam-na descaradamente.

### Os Outros Métodos de `ostream`

Além das várias funções `operator<<()`, a classe `ostream` fornece o método `put()` para exibir caracteres e o método `write()` para exibir strings.

Originalmente, o método `put()` tinha o seguinte protótipo:

```cpp
ostream & put(char);
```

O padrão atual é equivalente, exceto que é um template para permitir `wchar_t`. Você o invoca usando a notação usual de método de classe:

```cpp
cout.put('W'); // exibe o caractere W
```

Aqui `cout` é o objeto invocador e `put()` é a função-membro da classe. Como as funções `operator<<()`, essa função retorna uma referência ao objeto invocador, de modo que você pode concatenar a saída com ela:

```cpp
cout.put('I').put('t'); // exibe It com duas chamadas a put()
```

A chamada de função `cout.put('I')` retorna `cout`, que então atua como o objeto invocador para a chamada `put('t')`.

Com o protótipo adequado, você pode usar `put()` com argumentos de tipos numéricos diferentes de `char`, como `int`, e deixar que a prototipagem de função converta automaticamente o argumento para o valor `char` correto. Por exemplo, você poderia usar o seguinte:

```cpp
cout.put(65);   // exibe o caractere A
cout.put(66.3); // exibe o caractere B
```

A primeira instrução converte o valor `int` 65 para um valor `char` e então exibe o caractere cujo código ASCII é 65. Da mesma forma, a segunda instrução converte o valor `double` 66.3 para o valor `char` 66 e exibe o caractere correspondente.

Esse comportamento é útil em versões anteriores ao Release 2.0 do C++; nessas versões, a linguagem representa constantes de caractere com valores do tipo `int`. Assim, uma instrução como a seguinte interpretaria `'W'` como um valor `int` e, portanto, o exibiria como o inteiro 87, o valor ASCII do caractere:

```cpp
cout << 'W';
```

Mas a seguinte instrução funciona corretamente:

```cpp
cout.put('W');
```

Como o C++ atual representa constantes `char` como tipo `char`, agora você pode usar qualquer um dos dois métodos.

Alguns compiladores sobrecarregam incorretamente `put()` para três tipos de argumento: `char`, `unsigned char` e `signed char`. Isso torna o uso de `put()` com um argumento `int` ambíguo, porque um `int` pode ser convertido para qualquer um desses três tipos.

O método `write()` escreve uma string inteira e tem o seguinte protótipo de template:

```cpp
basic_ostream<charT,traits>& write(const char_type* s, streamsize n);
```

O primeiro argumento de `write()` fornece o endereço da string a ser exibida, e o segundo argumento indica quantos caracteres exibir. Usar `cout` para invocar `write()` aciona a especialização para `char`, de modo que o tipo de retorno é `ostream &`. A Listagem 17.1 mostra como o método `write()` funciona.

**Listagem 17.1 — `escrever.cpp`**

```cpp
// escrever.cpp -- usando cout.write()
#include <iostream>
#include <cstring> // ou string.h

int main()
{
    using std::cout;
    using std::endl;
    const char * estado1 = "Florida";
    const char * estado2 = "Kansas";
    const char * estado3 = "Euphoria";
    int tam = std::strlen(estado2);
    cout << "Índice de loop crescente:\n";
    int i;
    for (i = 1; i <= tam; i++)
    {
        cout.write(estado2, i);
        cout << endl;
    }
    // concatenar saída
    cout << "Índice de loop decrescente:\n";
    for (i = tam; i > 0; i--)
        cout.write(estado2, i) << endl;
    // ultrapassar o comprimento da string
    cout << "Ultrapassando o comprimento da string:\n";
    cout.write(estado2, tam + 5) << endl;
    return 0;
}
```

Alguns compiladores podem observar que o programa define mas não usa os arrays `estado1` e `estado3`. Tudo bem, pois esses dois arrays estão ali apenas para fornecer dados antes e depois do array `estado2`, para que você possa ver o que acontece quando o programa acessa `estado2` com um tamanho maior do que o real. Eis a saída do programa da Listagem 17.1:

```
Índice de loop crescente:
K
Ka
Kan
Kans
Kansa
Kansas
Índice de loop decrescente:
Kansas
Kansa
Kans
Kan
Ka
K
Ultrapassando o comprimento da string:
Kansas Euph
```

Observe que a chamada `cout.write()` retorna o objeto `cout`. Isso ocorre porque o método `write()` retorna uma referência ao objeto que o invoca, e nesse caso o objeto `cout` é o invocador. Isso torna possível concatenar a saída, pois `cout.write()` é substituído por seu valor de retorno, `cout`:

```cpp
cout.write(estado2, i) << endl;
```

Observe também que o método `write()` não para de imprimir caracteres automaticamente ao atingir o caractere nulo. Ele simplesmente imprime quantos caracteres você determinar, mesmo que isso ultrapasse os limites de uma determinada string! Nesse caso, o programa coloca a string `"Kansas"` entre duas outras strings para que as posições de memória adjacentes contenham dados. Os compiladores diferem na ordem em que armazenam os dados na memória e no modo como alinham a memória. Por exemplo, `"Kansas"` ocupa 6 bytes, mas este compilador em particular parece alinhar strings usando múltiplos de 4 bytes, de modo que `"Kansas"` é preenchida para 8 bytes. Alguns compiladores armazenam `"Florida"` após `"Kansas"`. Portanto, em razão das diferenças entre compiladores, você pode obter um resultado diferente para a última linha de saída.

O método `write()` também pode ser usado com dados numéricos. Você passaria a ele o endereço de um número, convertido para `char *`:

```cpp
long val = 560031841;
cout.write((char *) &val, sizeof(long));
```

Isso não traduz um número para os caracteres corretos; em vez disso, transmite a representação de bits conforme armazenada na memória. Por exemplo, um valor `long` de 4 bytes como `560031841` seria transmitido como 4 bytes separados. Um dispositivo de saída como um monitor tentaria então interpretar cada byte como se fosse código ASCII (ou qualquer outro). Portanto, `560031841` apareceria na tela como alguma combinação de 4 caracteres, muito provavelmente um conjunto sem sentido. No entanto, `write()` oferece uma maneira compacta e precisa de armazenar dados numéricos em um arquivo. Voltaremos a essa possibilidade mais adiante neste capítulo.

### Descarregando o Buffer de Saída

Considere o que acontece quando um programa usa `cout` para enviar bytes ao dispositivo de saída padrão. Como a classe `ostream` armazena em buffer a saída tratada pelo objeto `cout`, a saída não é enviada ao seu destino imediatamente. Em vez disso, ela se acumula no buffer até que ele esteja cheio. Então o programa descarrega o buffer (flushes the buffer), enviando o conteúdo e limpando o buffer para novos dados. Tipicamente, um buffer tem 512 bytes ou um múltiplo inteiro desse valor. O armazenamento em buffer é uma enorme economia de tempo quando a saída padrão está conectada a um arquivo em disco rígido. Afinal, você não quer que um programa acesse o disco rígido 512 vezes para enviar 512 bytes. É muito mais eficaz coletar 512 bytes em um buffer e gravá-los no disco rígido em uma única operação de disco.

Para a saída na tela, porém, preencher primeiro o buffer é menos crítico. Na verdade, seria inconveniente ter que reformular a mensagem "Pressione qualquer tecla para continuar" de modo que consumisse os 512 bytes necessários para preencher um buffer. Felizmente, no caso da saída para tela, o programa não necessariamente aguarda até que o buffer esteja cheio. Enviar um caractere de nova linha para o buffer, por exemplo, normalmente descarrega o buffer. Além disso, como mencionado antes, a maioria das implementações C++ descarrega o buffer quando a entrada está pendente. Ou seja, suponha que você tenha o seguinte código:

```cpp
cout << "Digite um número: ";
float num;
cin >> num;
```

O fato de o programa esperar entrada faz com que ele exiba a mensagem `cout` (isto é, descarregue a mensagem `"Digite um número: "`) imediatamente, mesmo que a string de saída não tenha um caractere de nova linha. Sem esse recurso, o programa esperaria a entrada sem exibir o prompt com a mensagem `cout` para o usuário.

Se sua implementação não descarregar a saída quando você quiser, você pode forçar o descarregamento usando um dos dois manipuladores. O manipulador `flush` descarrega o buffer, e o manipulador `endl` descarrega o buffer e insere um caractere de nova linha. Você usa esses manipuladores da mesma forma que usaria um nome de variável:

```cpp
cout << "Olá, bela aparência! " << flush;
cout << "Aguarde apenas um momento, por favor." << endl;
```

Os manipuladores são, de fato, funções. Por exemplo, você pode descarregar o buffer de `cout` chamando diretamente a função `flush()`:

```cpp
flush(cout);
```

No entanto, a classe `ostream` sobrecarrega o operador de inserção `<<` de tal modo que a expressão a seguir é substituída pela chamada à função `flush(cout)`:

```cpp
cout << flush
```

Assim, você pode usar a notação de inserção mais conveniente para descarregar com sucesso.

### Formatação com `cout`

Os operadores de inserção de `ostream` convertem valores para forma de texto. Por padrão, eles formatam os valores da seguinte maneira:

- Um valor do tipo `char`, se representar um caractere imprimível, é exibido como um caractere em um campo de um caractere de largura.
- Os tipos numéricos inteiros são exibidos como inteiros decimais em um campo apenas largo o suficiente para conter o número e, se presente, o sinal de menos.
- Strings são exibidas em um campo igual em largura ao comprimento da string.

O comportamento padrão para tipos de ponto flutuante mudou. A seguir estão as diferenças entre implementações antigas e atuais do C++:

- **Estilo novo** — Os tipos de ponto flutuante são exibidos com um total de seis dígitos, exceto que zeros à direita não são exibidos. (Observe que o número de dígitos exibidos não tem nenhuma relação com a precisão com que o número é armazenado.) O número é exibido na notação de ponto fixo (fixed-point notation) ou então na notação E (E notation; ver Capítulo 3, "Trabalhando com Dados"), dependendo do valor do número. Em particular, a notação E é usada se o expoente for 6 ou maior, ou -5 ou menor. Novamente, o campo é apenas largo o suficiente para conter o número e, se presente, um sinal de menos. O comportamento padrão corresponde ao uso da função padrão da biblioteca C `fprintf()` com o especificador `%g`.
- **Estilo antigo** — Os tipos de ponto flutuante são exibidos com seis casas à direita do ponto decimal, exceto que zeros à direita não são exibidos. O número é exibido na notação de ponto fixo ou então na notação E, dependendo do valor do número. Novamente, o campo é apenas largo o suficiente para conter o número e, se presente, um sinal de menos.

Como cada valor é exibido em uma largura igual ao seu tamanho, você deve fornecer espaços entre os valores explicitamente; caso contrário, os valores consecutivos seriam exibidos juntos.

A Listagem 17.2 ilustra os padrões de saída. Ela exibe dois pontos (`:`) após cada valor para que você possa ver a largura do campo usado em cada caso. O programa usa a expressão `1.0 / 9.0` para gerar uma fração não terminante e ver quantas casas são impressas.

> **Nota:** Nem todos os compiladores geram saída formatada de acordo com o padrão C++ atual. Além disso, o padrão atual permite variações regionais. Por exemplo, uma implementação europeia pode seguir o costume continental de usar vírgula em vez de ponto para exibir frações decimais. A biblioteca de localidade (arquivo de cabeçalho `locale`) fornece um mecanismo para imbuir um fluxo de entrada ou saída com um estilo específico. Este capítulo usa a localidade dos EUA.

**Listagem 17.2 — `padroes.cpp`**

```cpp
// padroes.cpp -- formatos padrão do cout
#include <iostream>
int main()
{
    using std::cout;
    cout << "12345678901234567890\n";
    char ch = 'K';
    int t = 273;
    cout << ch << ":\n";
    cout << t << ":\n";
    cout << -t << ":\n";
    double f1 = 1.200;
    cout << f1 << ":\n";
    cout << (f1 + 1.0 / 9.0) << ":\n";
    double f2 = 1.67E2;
    cout << f2 << ":\n";
    f2 += 1.0 / 9.0;
    cout << f2 << ":\n";
    cout << (f2 * 1.0e4) << ":\n";
    double f3 = 2.3e-4;
    cout << f3 << ":\n";
    cout << f3 / 10 << ":\n";

    return 0;
}
```

Eis a saída do programa da Listagem 17.2:

```
K:
273:
-273:
1.2:
1.31111:
167:
167.111:
1.67111e+006:
0.00023:
2.3e-005:
```

Cada valor preenche seu campo. Observe que os zeros à direita de `1.200` não são exibidos, mas que os valores de ponto flutuante sem zeros finais têm seis casas exibidas. Além disso, essa implementação específica exibe três dígitos no expoente; outras podem usar dois.

### Alterando a Base Numérica Usada na Exibição

A classe `ostream` herda da classe `ios`, que herda da classe `ios_base`. A classe `ios_base` armazena informações que descrevem o estado de formato. Por exemplo, certos bits em um membro da classe determinam a base numérica usada, enquanto outro membro determina a largura do campo. Usando manipuladores, você pode controlar a base numérica usada para exibir inteiros. Usando funções-membro de `ios_base`, você pode controlar a largura do campo e o número de casas exibidas à direita do ponto decimal. Como a classe `ios_base` é uma classe base indireta de `ostream`, você pode usar seus métodos com objetos `ostream` (ou descendentes), como `cout`.

> **Nota:** Os membros e métodos encontrados na classe `ios_base` eram antigamente encontrados na classe `ios`. Agora `ios_base` é uma classe base de `ios`. No novo sistema, `ios` é um template de classe com especializações para `char` e `wchar_t`, e `ios_base` contém os recursos sem template.

Para controlar se os inteiros são exibidos na base 10, na base 16 ou na base 8, você pode usar os manipuladores `dec`, `hex` e `oct`. Por exemplo, a seguinte chamada de função define o estado de formato da base numérica para o objeto `cout` como hexadecimal:

```cpp
hex(cout);
```

Depois de fazer isso, um programa imprimirá valores inteiros na forma hexadecimal até que você defina o estado de formato para outra opção. Observe que os manipuladores não são funções-membro; portanto, não precisam ser invocados por um objeto.

Embora os manipuladores sejam de fato funções, você normalmente os vê usados desta forma:

```cpp
cout << hex;
```

A classe `ostream` sobrecarrega o operador `<<` para tornar esse uso equivalente à chamada de função `hex(cout)`. Os manipuladores estão no namespace `std`. A Listagem 17.3 ilustra o uso desses manipuladores. Ela exibe o valor de um inteiro e seu quadrado em três bases numéricas diferentes. Observe que você pode usar um manipulador separadamente ou como parte de uma série de inserções.

**Listagem 17.3 — `manipulador.cpp`**

```cpp
// manipulador.cpp -- usando manipuladores de formato
#include <iostream>
int main()
{
    using namespace std;
    cout << "Digite um inteiro: ";
    int n;
    cin >> n;
    cout << "n   n*n\n";
    cout << n << " " << n * n << " (decimal)\n";
    // definir modo hexadecimal
    cout << hex;
    cout << n << " ";
    cout << n * n << " (hexadecimal)\n";
    // definir modo octal
    cout << oct << n << " " << n * n << " (octal)\n";
    // forma alternativa de chamar um manipulador
    dec(cout);
    cout << n << " " << n * n << " (decimal)\n";

    return 0;
}
```

Eis uma saída de exemplo do programa da Listagem 17.3:

```
Digite um inteiro: 13
n   n*n
13   169 (decimal)
d   a9 (hexadecimal)
15   251 (octal)
13   169 (decimal)
```

### Ajustando a Largura dos Campos

Você provavelmente observou que as colunas na saída da Listagem 17.3 não ficam alinhadas; isso ocorre porque os números têm larguras de campo diferentes. Você pode usar a função-membro `width` para colocar números de tamanhos diferentes em campos de largura igual. O método tem esses protótipos:

```cpp
int width();
int width(int i);
```

A primeira forma retorna a configuração atual de largura de campo. A segunda define a largura do campo como `i` espaços e retorna o valor anterior de largura de campo. Isso permite salvar o valor anterior caso você queira restaurar a largura para esse valor posteriormente.

O método `width()` afeta apenas o próximo item exibido, e a largura do campo reverte para o valor padrão depois. Por exemplo, considere as seguintes instruções:

```cpp
cout << '#';
cout.width(12);
cout << 12 << "#" << 24 << "#\n";
```

Como `width()` é uma função-membro, você deve usar um objeto (`cout`, neste caso) para invocá-la. A instrução de saída produz a seguinte exibição:

```
#      12#24#
```

O `12` é colocado em um campo de 12 caracteres de largura, na extremidade direita do campo. Isso se chama justificação à direita (right-justification). Depois disso, a largura do campo reverte para o padrão, e os dois caracteres `#` e o `24` são impressos em campos iguais ao seu próprio tamanho.

> **Atenção:** O método `width()` afeta apenas o próximo item exibido, e a largura do campo reverte para o valor padrão depois.

O C++ nunca trunca dados; portanto, se você tentar imprimir um valor de sete dígitos em um campo de largura dois, o C++ expande o campo para ajustar os dados. (Algumas linguagens simplesmente preenchem o campo com asteriscos se os dados não couberem. A filosofia C/C++ é que mostrar todos os dados é mais importante do que manter as colunas organizadas; o C++ coloca a substância acima da forma.) A Listagem 17.4 mostra como a função-membro `width()` funciona.

**Listagem 17.4 — `largura.cpp`**

```cpp
// largura.cpp -- usando o método width
#include <iostream>
int main()
{
    using std::cout;
    int l = cout.width(30);
    cout << "largura padrão do campo = " << l << ":\n";
    cout.width(5);
    cout << "N" << ':';
    cout.width(8);
    cout << "N * N" << ":\n";
    for (long i = 1; i <= 100; i *= 10)
    {
        cout.width(5);
        cout << i << ':';
        cout.width(8);
        cout << i * i << ":\n";
    }
    return 0;
}
```

Eis a saída do programa da Listagem 17.4:

```
      largura padrão do campo = 0:
    N: N * N:
    1:       1:
   10:     100:
  100:   10000:
```

A saída exibe valores justificados à direita em seus campos. A saída é preenchida com espaços. Ou seja, `cout` atinge a largura total do campo adicionando espaços. Com a justificação à direita, os espaços são inseridos à esquerda dos valores. O caractere usado para preenchimento é denominado caractere de preenchimento (fill character). A justificação à direita é o padrão.

Observe que o programa da Listagem 17.4 aplica a largura de campo de 30 à string exibida pela primeira instrução `cout`, mas não ao valor de `l`. Isso ocorre porque o método `width()` afeta apenas o próximo item único exibido. Observe também que `l` tem o valor 0. Isso ocorre porque `cout.width(30)` retorna a largura de campo anterior, não a largura para a qual acabou de ser definida. O fato de `l` ser 0 significa que zero é a largura de campo padrão. Como o C++ sempre expande um campo para ajustar os dados, esse tamanho serve para todos. Por fim, o programa usa `width()` para alinhar cabeçalhos de colunas e dados, usando uma largura de cinco caracteres para a primeira coluna e uma largura de oito caracteres para a segunda coluna.

### Caracteres de Preenchimento

Por padrão, `cout` preenche as partes não utilizadas de um campo com espaços. Você pode usar a função-membro `fill()` para alterar isso. Por exemplo, a chamada a seguir muda o caractere de preenchimento para um asterisco:

```cpp
cout.fill('*');
```

Isso pode ser útil para, digamos, imprimir cheques de modo que os destinatários não possam adicionar facilmente um dígito ou dois. A Listagem 17.5 ilustra o uso dessa função-membro.

**Listagem 17.5 — `preenchimento.cpp`**

```cpp
// preenchimento.cpp -- alterando o caractere de preenchimento dos campos
#include <iostream>
int main()
{
    using std::cout;
    cout.fill('*');
    const char * equipe[2] = {"Waldo Whipsnade", "Wilmarie Wooper"};
    long bonus[2] = {900, 1350};
    for (int i = 0; i < 2; i++)
    {
        cout << equipe[i] << ": $";
        cout.width(7);
        cout << bonus[i] << "\n";
    }
    return 0;
}
```

Eis a saída do programa da Listagem 17.5:

```
Waldo Whipsnade: $****900
Wilmarie Wooper: $***1350
```

Observe que, diferentemente da largura do campo, o novo caractere de preenchimento permanece em vigor até você alterá-lo.

### Definindo a Precisão da Exibição de Ponto Flutuante

O significado da precisão de ponto flutuante (floating-point precision) depende do modo de saída. No modo padrão, significa o número total de dígitos exibidos. Nos modos fixo e científico, a serem discutidos em breve, precisão significa o número de dígitos exibidos à direita do ponto decimal. A precisão padrão para o C++, como você já viu, é 6. (Lembre-se, no entanto, de que os zeros finais são descartados.) A função-membro `precision()` permite selecionar outros valores. Por exemplo, a instrução a seguir faz com que `cout` defina a precisão como 2:

```cpp
cout.precision(2);
```

Diferentemente do caso de `width()`, mas como no caso de `fill()`, uma nova configuração de precisão permanece em vigor até ser redefinida. A Listagem 17.6 demonstra exatamente esse ponto.

**Listagem 17.6 — `precisao.cpp`**

```cpp
// precisao.cpp -- definindo a precisão
#include <iostream>

int main()
{
    using std::cout;
    float preco1 = 20.40;
    float preco2 = 1.9 + 8.0 / 9.0;
    cout << "\"Amigos Felpudos\" custa $" << preco1 << "!\n";
    cout << "\"Inimigos Furiosos\" custa $" << preco2 << "!\n";
    cout.precision(2);
    cout << "\"Amigos Felpudos\" custa $" << preco1 << "!\n";
    cout << "\"Inimigos Furiosos\" custa $" << preco2 << "!\n";

    return 0;
}
```

Eis a saída do programa da Listagem 17.6:

```
"Amigos Felpudos" custa $20.4!
"Inimigos Furiosos" custa $2.78889!
"Amigos Felpudos" custa $20!
"Inimigos Furiosos" custa $2.8!
```

Observe que a terceira linha dessa saída não inclui um ponto decimal à direita. Além disso, a quarta linha exibe um total de dois dígitos.

### Imprimindo Zeros à Direita e Pontos Decimais

Certas formas de saída, como preços ou números em colunas, ficam melhores se os zeros à direita forem mantidos. Por exemplo, a saída da Listagem 17.6 ficaria melhor como `$20.40` do que como `$20.4`. A família de classes `iostream` não fornece uma função cujo único propósito seja realizar isso. No entanto, a classe `ios_base` fornece uma função `setf()` (para definir flag, ou *set flag*) que controla vários recursos de formatação. A classe também define diversas constantes que podem ser usadas como argumentos para essa função. Por exemplo, a seguinte chamada de função faz com que `cout` exiba pontos decimais à direita:

```cpp
cout.setf(ios_base::showpoint);
```

No modo de ponto flutuante padrão, isso também faz com que os zeros à direita sejam exibidos. Ou seja, em vez de exibir `2.00` como `2`, `cout` o exibirá como `2.00000` se a precisão padrão de 6 estiver em vigor. A Listagem 17.7 adiciona essa instrução à Listagem 17.6.

Caso você esteja se perguntando sobre a notação `ios_base::showpoint`, `showpoint` é uma constante estática de escopo de classe definida na declaração da classe `ios_base`. Escopo de classe significa que você deve usar o operador de resolução de escopo (`::`) com o nome da constante se usá-la fora de uma definição de função-membro. Portanto, `ios_base::showpoint` nomeia uma constante definida na classe `ios_base`.

**Listagem 17.7 — `pontoflutuante.cpp`**

```cpp
// pontoflutuante.cpp -- definindo precisão e exibindo ponto decimal final
#include <iostream>
int main()
{
    using std::cout;
    using std::ios_base;

    float preco1 = 20.40;
    float preco2 = 1.9 + 8.0 / 9.0;
    cout.setf(ios_base::showpoint);
    cout << "\"Amigos Felpudos\" custa $" << preco1 << "!\n";
    cout << "\"Inimigos Furiosos\" custa $" << preco2 << "!\n";
    cout.precision(2);
    cout << "\"Amigos Felpudos\" custa $" << preco1 << "!\n";
    cout << "\"Inimigos Furiosos\" custa $" << preco2 << "!\n";

    return 0;
}
```

Eis a saída do programa da Listagem 17.7, usando a formatação C++ atual:

```
"Amigos Felpudos" custa $20.4000!
"Inimigos Furiosos" custa $2.78889!
"Amigos Felpudos" custa $20.!
"Inimigos Furiosos" custa $2.8!
```

Essa saída mostra os zeros à direita para a primeira linha. A terceira linha mostra o ponto decimal, mas sem zeros à direita, porque a precisão foi definida como 2 e dois dígitos já foram exibidos.

### Mais Sobre `setf()`

O método `setf()` controla vários outros aspectos de formatação além de quando o ponto decimal é exibido, portanto vamos examiná-lo mais de perto. A classe `ios_base` possui um membro de dado protegido no qual bits individuais (chamados de flags nesse contexto) controlam diferentes aspectos de formatação, como a base numérica e se os zeros à direita são exibidos. Ativar um flag é chamado de definir o flag (ou bit) e significa definir o bit como 1. (Os flags de bits são o equivalente programático de configurar as chaves DIP para configurar o hardware do computador.) Os manipuladores `hex`, `dec` e `oct`, por exemplo, ajustam os três bits de flag que controlam a base numérica. A função `setf()` fornece outra maneira de ajustar os bits de flag.

A função `setf()` tem dois protótipos. O primeiro é este:

```cpp
fmtflags setf(fmtflags);
```

Aqui, `fmtflags` é um nome `typedef` para um tipo de máscara de bits (bitmask type; ver Nota a seguir) usado para armazenar os flags de formato. O nome é definido na classe `ios_base`. Essa versão de `setf()` é usada para definir informações de formato controladas por um único bit. O argumento é um valor `fmtflags` que indica qual bit definir. O valor de retorno é um número do tipo `fmtflags` que indica as configurações anteriores de todos os flags. Você pode então salvar esse valor se quiser restaurar as configurações originais posteriormente. Que valor você passa para `setf()`? Se quiser definir o bit número 11 como 1, você passa um número que tem seu bit número 11 definido como 1. O valor de retorno teria seu bit número 11 atribuído ao valor anterior para esse bit. Controlar bits soa (e é) tedioso. No entanto, você não precisa fazer esse trabalho; a classe `ios_base` define constantes (mostradas na Tabela 17.1) que representam os valores de bits.

> **Nota — Tipo de máscara de bits:** Um tipo de máscara de bits (bitmask type) é um tipo que é usado para armazenar valores individuais de bits. Pode ser um tipo inteiro, uma enumeração ou um contêiner STL `bitset`. A ideia principal é que cada bit é individualmente acessível e tem seu próprio significado. O pacote `iostream` usa tipos de máscara de bits para armazenar informações de estado.

**Tabela 17.1 — Constantes de Formatação**

| Constante                  | Significado                                                      |
|----------------------------|------------------------------------------------------------------|
| `ios_base::boolalpha`      | Entrada e saída de valores `bool` como `true` e `false`.        |
| `ios_base::showbase`       | Usar prefixos de base C++ (`0`, `0x`) na saída.                 |
| `ios_base::showpoint`      | Exibir ponto decimal à direita.                                  |
| `ios_base::uppercase`      | Usar letras maiúsculas para saída hex e notação E.              |
| `ios_base::showpos`        | Usar `+` antes de números positivos.                            |

Como essas constantes de formatação são definidas dentro da classe `ios_base`, você deve usar o operador de resolução de escopo com elas. Ou seja, deve usar `ios_base::uppercase`, não apenas `uppercase`. Se você não usar uma diretiva `using` ou declaração `using`, pode usar o operador de resolução de escopo para indicar que esses nomes estão no namespace `std`. Ou seja, pode usar `std::ios_base::showpos`, e assim por diante. As alterações permanecem em vigor até serem substituídas. A Listagem 17.8 ilustra o uso de algumas dessas constantes.

**Listagem 17.8 — `definirflag.cpp`**

```cpp
// definirflag.cpp -- usando setf() para controlar a formatação
#include <iostream>

int main()
{
    using std::cout;
    using std::endl;
    using std::ios_base;
    int temperatura = 63;

    cout << "Temperatura da água hoje: ";
    cout.setf(ios_base::showpos); // exibir sinal de mais
    cout << temperatura << endl;
    cout << "Para nossos amigos programadores, isso é\n";
    cout << std::hex << temperatura << endl; // usar hex
    cout.setf(ios_base::uppercase); // usar maiúsculas em hex
    cout.setf(ios_base::showbase);  // usar prefixo 0X para hex
    cout << "ou\n";
    cout << temperatura << endl;
    cout << "Como " << true << "! ops -- Como ";
    cout.setf(ios_base::boolalpha);
    cout << true << "!\n";
    return 0;
}
```

Eis a saída do programa da Listagem 17.8:

```
Temperatura da água hoje: +63
Para nossos amigos programadores, isso é
3f
ou
0X3F
Como 0X1! ops -- Como true!
```

Observe que o sinal de mais é usado apenas com a versão de base 10. O C++ trata valores hexadecimais e octais como sem sinal; portanto, nenhum sinal é necessário para eles. (No entanto, algumas implementações do C++ ainda podem exibir um sinal de mais.)

O segundo protótipo de `setf()` recebe dois argumentos e retorna a configuração anterior:

```cpp
fmtflags setf(fmtflags, fmtflags);
```

Essa forma sobrecarregada da função é usada para opções de formato controladas por mais de 1 bit. O primeiro argumento, como antes, é um valor `fmtflags` que contém a configuração desejada. O segundo argumento é um valor que primeiro limpa os bits apropriados. Por exemplo, suponha que definir o bit 3 como 1 signifique base 10, definir o bit 4 como 1 signifique base 8 e definir o bit 5 como 1 signifique base 16. Suponha que a saída esteja na base 10 e você queira defini-la para base 16. Não só você deve definir o bit 5 como 1, mas também deve definir o bit 3 como 0; isso é chamado de limpar o bit (clearing the bit). O manipulador inteligente `hex` faz automaticamente as duas tarefas. Usar a função `setf()` requer um pouco mais de trabalho, porque você usa o segundo argumento para indicar quais bits limpar e depois usa o primeiro argumento para indicar qual bit definir. Isso não é tão complicado quanto parece, porque a classe `ios_base` define constantes (mostradas na Tabela 17.2) para essa finalidade. Em particular, você deve usar a constante `ios_base::basefield` como segundo argumento e `ios_base::hex` como primeiro argumento se estiver mudando de base. Ou seja, a seguinte chamada de função tem o mesmo efeito que usar o manipulador `hex`:

```cpp
cout.setf(ios_base::hex, ios_base::basefield);
```

**Tabela 17.2 — Argumentos para `setf(long, long)`**

| Segundo Argumento          | Primeiro Argumento          | Significado                                                  |
|----------------------------|-----------------------------|--------------------------------------------------------------|
| `ios_base::basefield`      | `ios_base::dec`             | Usar base 10.                                                |
|                            | `ios_base::oct`             | Usar base 8.                                                 |
|                            | `ios_base::hex`             | Usar base 16.                                                |
| `ios_base::floatfield`     | `ios_base::fixed`           | Usar notação de ponto fixo.                                  |
|                            | `ios_base::scientific`      | Usar notação científica.                                     |
| `ios_base::adjustfield`    | `ios_base::left`            | Usar justificação à esquerda.                                |
|                            | `ios_base::right`           | Usar justificação à direita.                                 |
|                            | `ios_base::internal`        | Justificar sinal/prefixo à esquerda, valor à direita.        |

A classe `ios_base` define três conjuntos de flags de formatação que podem ser tratados desta forma. Cada conjunto consiste em uma constante para ser usada como segundo argumento e duas ou três constantes para serem usadas como primeiro argumento. O segundo argumento limpa um lote de bits relacionados; então o primeiro argumento define um desses bits como 1. A Tabela 17.2 mostra os nomes das constantes usadas para o segundo argumento de `setf()`, a escolha associada de constantes para o primeiro argumento e seus significados. Por exemplo, para selecionar a justificação à esquerda, você usa `ios_base::adjustfield` para o segundo argumento e `ios_base::left` como primeiro argumento. A justificação à esquerda (left-justification) significa iniciar um valor na extremidade esquerda do campo, e a justificação à direita (right-justification) significa terminar um valor na extremidade direita do campo. A justificação interna (internal justification) significa colocar quaisquer sinais ou prefixos de base à esquerda do campo e o restante do número à direita do campo.

A notação de ponto fixo (fixed-point notation) significa usar o estilo `123.4` para valores de ponto flutuante, independentemente do tamanho do número, e a notação científica (scientific notation) significa usar o estilo `1.23e04`, independentemente do tamanho do número. Se você está familiarizado com os especificadores `printf()` do C, pode ser útil saber que o modo padrão do C++ corresponde ao especificador `%g`, `fixed` corresponde ao especificador `%f` e `scientific` corresponde ao especificador `%e`.

Sob o Padrão C++, tanto a notação fixa quanto a científica têm as seguintes duas propriedades:

- Precisão significa o número de dígitos à direita do ponto decimal, e não o número total de dígitos.
- Zeros à direita são exibidos.

A função `setf()` é uma função-membro da classe `ios_base`. Como essa é uma classe base da classe `ostream`, você pode invocar a função usando o objeto `cout`. Por exemplo, para solicitar a justificação à esquerda, use esta chamada:

```cpp
ios_base::fmtflags antiga = cout.setf(ios::left, ios::adjustfield);
```

Para restaurar a configuração anterior, use isto:

```cpp
cout.setf(antiga, ios::adjustfield);
```

A Listagem 17.9 ilustra outros exemplos do uso de `setf()` com dois argumentos.

> **Nota:** O programa da Listagem 17.9 usa uma função matemática, e alguns sistemas C++ não pesquisam automaticamente na biblioteca matemática. Por exemplo, alguns sistemas Unix exigem o seguinte:
>
> ```
> $ CC definirflag2.C -lm
> ```
>
> A opção `-lm` instrui o vinculador a pesquisar na biblioteca matemática. Da mesma forma, alguns sistemas Linux que usam `g++` exigem o mesmo flag.

**Listagem 17.9 — `definirflag2.cpp`**

```cpp
// definirflag2.cpp -- usando setf() com 2 argumentos para controlar a formatação
#include <iostream>
#include <cmath>

int main()
{
    using namespace std;
    // usar justificação à esquerda, exibir sinal de mais, mostrar zeros
    // finais, com precisão de 3
    cout.setf(ios_base::left, ios_base::adjustfield);
    cout.setf(ios_base::showpos);
    cout.setf(ios_base::showpoint);
    cout.precision(3);
    // usar notação e e salvar configuração antiga
    ios_base::fmtflags antiga = cout.setf(ios_base::scientific,
        ios_base::floatfield);
    cout << "Justificação à Esquerda:\n";
    long n;
    for (n = 1; n <= 41; n += 10)
    {
        cout.width(4);
        cout << n << "|";
        cout.width(12);
        cout << sqrt(double(n)) << "|\n";
    }
    // mudar para justificação interna
    cout.setf(ios_base::internal, ios_base::adjustfield);
    // restaurar estilo padrão de exibição de ponto flutuante
    cout.setf(antiga, ios_base::floatfield);
    cout << "Justificação Interna:\n";
    for (n = 1; n <= 41; n += 10)
    {
        cout.width(4);
        cout << n << "|";
        cout.width(12);
        cout << sqrt(double(n)) << "|\n";
    }
    // usar justificação à direita, notação fixa
    cout.setf(ios_base::right, ios_base::adjustfield);
    cout.setf(ios_base::fixed, ios_base::floatfield);
    cout << "Justificação à Direita:\n";
    for (n = 1; n <= 41; n += 10)
    {
        cout.width(4);
        cout << n << "|";
        cout.width(12);
        cout << sqrt(double(n)) << "|\n";
    }
    return 0;
}
```

Eis a saída do programa da Listagem 17.9:

```
Justificação à Esquerda:
+1  |+1.000e+00  |
+11 |+3.317e+00  |
+21 |+4.583e+00  |
+31 |+5.568e+00  |
+41 |+6.403e+00  |
Justificação Interna:
+ 1|+   1.00|
+ 11|+  3.32|
+ 21|+  4.58|
+ 31|+  5.57|
+ 41|+  6.40|
Justificação à Direita:
  +1|  +1.000|
 +11|  +3.317|
 +21|  +4.583|
 +31|  +5.568|
 +41|  +6.403|
```

Observe como uma precisão de 3 faz com que a exibição padrão de ponto flutuante (usada para justificação interna neste programa) mostre um total de três dígitos, enquanto os modos fixo e científico exibem três dígitos à direita do ponto decimal. (O número de dígitos exibidos no expoente da notação e depende da implementação.)

Os efeitos de chamar `setf()` podem ser desfeitos com `unsetf()`, que tem o seguinte protótipo:

```cpp
void unsetf(fmtflags mask);
```

Aqui, `mask` é um padrão de bits. Todos os bits definidos como 1 em `mask` fazem os bits correspondentes serem desmarcados. Ou seja, `setf()` define bits como 1 e `unsetf()` define bits de volta para 0. Eis um exemplo:

```cpp
cout.setf(ios_base::showpoint);   // exibir ponto decimal à direita
cout.unsetf(ios_base::showpoint); // não exibir ponto decimal à direita
cout.setf(ios_base::boolalpha);   // exibir true, false
cout.unsetf(ios_base::boolalpha); // exibir 1, 0
```

Você pode ter percebido que não há um flag especial para indicar o modo padrão de exibição de números de ponto flutuante. Veja como o sistema funciona. A notação fixa é usada se o bit `fixed`, e somente o bit `fixed`, estiver definido. A notação científica é usada se o bit `scientific`, e somente o bit `scientific`, estiver definido. Qualquer outra combinação, como nenhum bit definido ou ambos os bits definidos, resulta no uso do modo padrão. Portanto, uma maneira de invocar o modo padrão é esta:

```cpp
cout.setf(0, ios_base::floatfield); // ir para o modo padrão
```

O segundo argumento desativa ambos os bits, e o primeiro argumento não define nenhum bit. Uma maneira mais curta de obter o mesmo resultado é usar `unsetf()` com `ios_base::floatfield`:

```cpp
cout.unsetf(ios_base::floatfield); // ir para o modo padrão
```

Se você soubesse com certeza que `cout` estivesse no estado fixo, poderia usar `ios_base::fixed` como argumento para `unsetf()`, mas usar `ios_base::floatfield` funciona independentemente do estado atual de `cout`, portanto é uma escolha melhor.

### Manipuladores Padrão

Usar `setf()` não é a abordagem mais amigável para a formatação, portanto o C++ oferece vários manipuladores para invocar `setf()` por você, fornecendo automaticamente os argumentos certos. Você já viu `dec`, `hex` e `oct`. Esses manipuladores, a maioria dos quais não está disponível em implementações C++ mais antigas, funcionam como `hex`. Por exemplo, a seguinte instrução ativa a justificação à esquerda e a opção de ponto decimal fixo:

```cpp
cout << left << fixed;
```

A Tabela 17.3 lista esses e vários outros manipuladores.

**Tabela 17.3 — Alguns Manipuladores Padrão**

| Manipulador    | Chamada equivalente                                                  |
|----------------|----------------------------------------------------------------------|
| `boolalpha`    | `setf(ios_base::boolalpha)`                                          |
| `noboolalpha`  | `unsetf(ios_base::boolalpha)`                                        |
| `showbase`     | `setf(ios_base::showbase)`                                           |
| `noshowbase`   | `unsetf(ios_base::showbase)`                                         |
| `showpoint`    | `setf(ios_base::showpoint)`                                          |
| `noshowpoint`  | `unsetf(ios_base::showpoint)`                                        |
| `showpos`      | `setf(ios_base::showpos)`                                            |
| `noshowpos`    | `unsetf(ios_base::showpos)`                                          |
| `uppercase`    | `setf(ios_base::uppercase)`                                          |
| `nouppercase`  | `unsetf(ios_base::uppercase)`                                        |
| `internal`     | `setf(ios_base::internal, ios_base::adjustfield)`                    |
| `left`         | `setf(ios_base::left, ios_base::adjustfield)`                        |
| `right`        | `setf(ios_base::right, ios_base::adjustfield)`                       |
| `dec`          | `setf(ios_base::dec, ios_base::basefield)`                           |
| `hex`          | `setf(ios_base::hex, ios_base::basefield)`                           |
| `oct`          | `setf(ios_base::oct, ios_base::basefield)`                           |
| `fixed`        | `setf(ios_base::fixed, ios_base::floatfield)`                        |
| `scientific`   | `setf(ios_base::scientific, ios_base::floatfield)`                   |

> **Dica:** Se o seu sistema suportar esses manipuladores, aproveite-os; se não, você ainda tem a opção de usar `setf()`.

### O Arquivo de Cabeçalho `iomanip`

Definir alguns valores de formato, como a largura do campo, pode ser trabalhoso usando as ferramentas de `iostream`. Para facilitar a vida, o C++ oferece manipuladores adicionais no arquivo de cabeçalho `iomanip`. Eles fornecem os mesmos serviços já discutidos, mas de maneira notativamente mais conveniente. Os três mais comumente usados são `setprecision()` para definir a precisão, `setfill()` para definir o caractere de preenchimento e `setw()` para definir a largura do campo. Diferentemente dos manipuladores discutidos anteriormente, esses recebem argumentos. O manipulador `setprecision()` recebe um argumento inteiro que especifica a precisão, o manipulador `setfill()` recebe um argumento `char` que indica o caractere de preenchimento, e o manipulador `setw()` recebe um argumento inteiro que especifica a largura do campo. Como são manipuladores, podem ser concatenados em uma instrução `cout`. Isso torna o manipulador `setw()` particularmente conveniente quando você está exibindo várias colunas de valores. A Listagem 17.10 ilustra isso, alterando a largura do campo e o caractere de preenchimento várias vezes em uma linha de saída. Ela também usa alguns dos manipuladores padrão mais recentes.

> **Nota:** Alguns sistemas C++ não pesquisam automaticamente na biblioteca matemática. Como mencionado antes, alguns sistemas Unix exigem o seguinte para acessar a biblioteca matemática:
>
> ```
> $ CC iomanip.C -lm
> ```

**Listagem 17.10 — `iomanip.cpp`**

```cpp
// iomanip.cpp -- usando manipuladores de iomanip
// alguns sistemas exigem vinculação explícita à biblioteca matemática
#include <iostream>
#include <iomanip>
#include <cmath>
int main()
{
    using namespace std;
    // usar novos manipuladores padrão
    cout << fixed << right;

    // usar manipuladores de iomanip
    cout << setw(6) << "N" << setw(14) << "raiz quadrada"
         << setw(15) << "raiz quarta\n";
    double raiz;
    for (int n = 10; n <= 100; n += 10)
    {
        raiz = sqrt(double(n));
        cout << setw(6) << setfill('.') << n << setfill(' ')
             << setw(12) << setprecision(3) << raiz
             << setw(14) << setprecision(4) << sqrt(raiz)
             << endl;
    }
    return 0;
}
```

Eis a saída do programa da Listagem 17.10:

```
     N  raiz quadrada    raiz quarta
....10    3.162    1.7783
....20    4.472    2.1147
....30    5.477    2.3403
....40    6.325    2.5149
....50    7.071    2.6591
....60    7.746    2.7832
....70    8.367    2.8925
....80    8.944    2.9907
....90    9.487    3.0801
...100   10.000    3.1623
```

Agora você pode produzir colunas ordenadamente alinhadas. O uso do manipulador `fixed` faz com que os zeros à direita sejam exibidos.

---

[Anterior](capitulo-16-04-lista-init-resumo.md) | [Índice](README.md) | [Próximo](capitulo-17-02-istream-estados-arquivo.md)
