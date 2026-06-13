# Capitulo 4 - Tipos Compostos (Compound Types)
## Parte 3: Introducao a Classe string

> Traducao nao oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Navegacao: [Anterior](capitulo-04-02-strings.md) | [Indice](README.md) | [Proximo](capitulo-04-04-estruturas.md)

---

## Apresentando a Classe string

O Padrao ISO/ANSI C++98 expandiu a biblioteca do C++ adicionando a classe `string`. Portanto, em vez de usar um array de caracteres para armazenar uma string, voce pode usar uma variavel do tipo `string` (ou objeto, para usar a terminologia do C++). Como voce vai ver, a classe `string` e mais simples de usar do que o array e tambem fornece uma representacao mais fiel de uma string como tipo.

Para usar a classe `string`, um programa deve incluir o arquivo de cabecalho `string`. A classe `string` e parte do namespace `std`, portanto voce precisa fornecer uma diretiva `using` ou uma declaracao `using`, ou se referenciar a classe como `std::string`. A definicao da classe oculta a natureza de array de uma string e permite tratar uma string muito como uma variavel comum. A Listagem 4.7 ilustra algumas das semelhancas e diferencas entre objetos `string` e arrays de caracteres.

**Listagem 4.7 — strtype1.cpp**

```cpp
// strtype1.cpp -- usando a classe string do C++
#include <iostream>
#include <string>   // torna a classe string disponivel
int main()
{
    using namespace std;
    char charr1[20];              // cria array vazio
    char charr2[20] = "jaguar";   // cria array inicializado
    string str1;                  // cria objeto string vazio
    string str2 = "pantera";      // cria string inicializada
    cout << "Digite um tipo de felino: ";
    cin >> charr1;
    cout << "Digite outro tipo de felino: ";
    cin >> str1;                  // usa cin para entrada
    cout << "Aqui estao alguns felinos:\n";
    cout << charr1 << " " << charr2 << " "
         << str1 << " " << str2   // usa cout para saida
         << endl;
    cout << "A terceira letra de " << charr2 << " e "
         << charr2[2] << endl;
    cout << "A terceira letra de " << str2 << " e "
         << str2[2] << endl;      // usa notacao de array
    return 0;
}
```

Aqui esta uma execucao de exemplo do programa na Listagem 4.7:

```
Digite um tipo de felino: ocelote
Digite outro tipo de felino: tigre
Aqui estao alguns felinos:
ocelote jaguar tigre pantera
A terceira letra de jaguar e g
A terceira letra de pantera e n
```

Voce deve aprender com este exemplo que, em muitos aspectos, e possivel usar um objeto `string` da mesma maneira que um array de caracteres:

- Voce pode inicializar um objeto `string` com uma string no estilo C.
- Voce pode usar `cin` para armazenar a entrada do teclado em um objeto `string`.
- Voce pode usar `cout` para exibir um objeto `string`.
- Voce pode usar a notacao de array para acessar caracteres individuais armazenados em um objeto `string`.

A principal diferenca entre objetos `string` e arrays de caracteres mostrada na Listagem 4.7 e que voce declara um objeto `string` como uma variavel simples, nao como um array:

```cpp
string str1;          // cria objeto string vazio
string str2 = "pantera"; // cria string inicializada
```

O design da classe permite que o programa cuide do dimensionamento automaticamente. Por exemplo, a declaracao de `str1` cria um objeto `string` de tamanho zero, mas o programa redimensiona automaticamente `str1` ao ler a entrada nele:

```cpp
cin >> str1; // str1 e redimensionado para caber na entrada
```

Isso torna o uso de um objeto `string` mais conveniente e seguro do que usar um array. Conceitualmente, pensa-se em um array de `char` como uma colecao de unidades de armazenamento `char` usadas para guardar uma string, mas em uma variavel de classe `string` como uma unica entidade que representa a string.

## Inicializacao de string em C++11

Como voce ja deve esperar, o C++11 permite a inicializacao por lista para strings no estilo C e para objetos `string`:

```cpp
char primeira_data[] = {"Le Chapon Dodu"};
char segunda_data[] {"The Elegant Plate"};
string terceira_data = {"The Bread Bowl"};
string quarta_data {"Hank's Fine Eats"};
```

## Atribuicao, Concatenacao e Acrescimo

A classe `string` torna algumas operacoes mais simples do que no caso de arrays. Por exemplo, voce nao pode simplesmente atribuir um array a outro. Mas voce pode atribuir um objeto `string` a outro:

```cpp
char charr1[20];           // cria array vazio
char charr2[20] = "jaguar"; // cria array inicializado
string str1;               // cria objeto string vazio
string str2 = "pantera";   // cria string inicializada
charr1 = charr2;           // INVALIDO, sem atribuicao de arrays
str1 = str2;               // VALIDO, atribuicao de objetos ok
```

A classe `string` simplifica a combinacao de strings. Voce pode usar o operador `+` para adicionar dois objetos `string` e o operador `+=` para acrescentar uma string ao final de um objeto `string` existente. Continuando com o codigo anterior, temos as seguintes possibilidades:

```cpp
string str3;
str3 = str1 + str2;  // atribui as strings unidas a str3
str1 += str2;        // acrescenta str2 ao final de str1
```

A Listagem 4.8 ilustra esses usos. Note que voce pode adicionar e acrescentar tanto strings no estilo C quanto objetos `string` a um objeto `string`.

**Listagem 4.8 — strtype2.cpp**

```cpp
// strtype2.cpp -- atribuindo, adicionando e acrescentando
#include <iostream>
#include <string>   // torna a classe string disponivel
int main()
{
    using namespace std;
    string s1 = "pinguim";
    string s2, s3;
    cout << "Voce pode atribuir um objeto string a outro: s2 = s1\n";
    s2 = s1;
    cout << "s1: " << s1 << ", s2: " << s2 << endl;
    cout << "Voce pode atribuir uma string estilo C a um objeto string.\n";
    cout << "s2 = \"urubu\"\n";
    s2 = "urubu";
    cout << "s2: " << s2 << endl;
    cout << "Voce pode concatenar strings: s3 = s1 + s2\n";
    s3 = s1 + s2;
    cout << "s3: " << s3 << endl;
    cout << "Voce pode acrescentar strings.\n";
    s1 += s2;
    cout << "s1 += s2 resulta em s1 = " << s1 << endl;
    s2 += " por um dia";
    cout << "s2 += \" por um dia\" resulta em s2 = " << s2 << endl;
    return 0;
}
```

Lembre-se de que a sequencia de escape `\"` representa uma aspa dupla usada como caractere literal, em vez de marcar os limites de uma string. Aqui esta a saida do programa na Listagem 4.8:

```
Voce pode atribuir um objeto string a outro: s2 = s1
s1: pinguim, s2: pinguim
Voce pode atribuir uma string estilo C a um objeto string.
s2 = "urubu"
s2: urubu
Voce pode concatenar strings: s3 = s1 + s2
s3: pinguimurubu
Voce pode acrescentar strings.
s1 += s2 resulta em s1 = pinguimurubu
s2 += " por um dia" resulta em s2 = urubu por um dia
```

## Mais Operacoes da Classe string

Mesmo antes de a classe `string` ser adicionada ao C++, os programadores precisavam realizar tarefas como atribuir strings. Para strings no estilo C, eles usavam funcoes da biblioteca C para essas tarefas. O arquivo de cabecalho `cstring` (anteriormente `string.h`) oferece suporte a essas funcoes. Por exemplo, voce pode usar a funcao `strcpy()` para copiar uma string em um array de caracteres, e pode usar a funcao `strcat()` para acrescentar uma string a um array de caracteres:

```cpp
strcpy(charr1, charr2); // copia charr2 em charr1
strcat(charr1, charr2); // acrescenta o conteudo de charr2 em charr1
```

A Listagem 4.9 compara tecnicas usadas com objetos `string` com tecnicas usadas com arrays de caracteres.

**Listagem 4.9 — strtype3.cpp**

```cpp
// strtype3.cpp -- mais recursos da classe string
#include <iostream>
#include <string>   // torna a classe string disponivel
#include <cstring>  // biblioteca de strings estilo C
int main()
{
    using namespace std;
    char charr1[20];
    char charr2[20] = "jaguar";
    string str1;
    string str2 = "pantera";
    // atribuicao para objetos string e arrays de caracteres
    str1 = str2;              // copia str2 em str1
    strcpy(charr1, charr2);   // copia charr2 em charr1
    // acrescimo para objetos string e arrays de caracteres
    str1 += " pasta";         // acrescenta pasta ao final de str1
    strcat(charr1, " suco");  // acrescenta suco ao final de charr1
    // obtendo o comprimento de um objeto string e de uma string estilo C
    int comp1 = str1.size();    // obtem comprimento de str1
    int comp2 = strlen(charr1); // obtem comprimento de charr1
    cout << "A string " << str1 << " contem "
         << comp1 << " caracteres.\n";
    cout << "A string " << charr1 << " contem "
         << comp2 << " caracteres.\n";
    return 0;
}
```

Aqui esta a saida:

```
A string pantera pasta contem 13 caracteres.
A string jaguar suco contem 11 caracteres.
```

A sintaxe para trabalhar com objetos `string` tende a ser mais simples do que usar as funcoes de string do C. Isso e especialmente verdadeiro para operacoes mais complexas. Por exemplo, o equivalente na biblioteca C de:

```cpp
str3 = str1 + str2;
```

seria isto:

```cpp
strcpy(charr3, charr1);
strcat(charr3, charr2);
```

Alem disso, com arrays, ha sempre o risco de o array de destino ser pequeno demais para conter as informacoes, como neste exemplo:

```cpp
char local[10] = "casa";
strcat(local, " de panquecas"); // problema de memoria
```

A funcao `strcat()` tentaria copiar todos os 13 caracteres no array `local`, ultrapassando a memoria adjacente. Isso pode fazer o programa abortar, ou o programa pode continuar a execucao com dados corrompidos. A classe `string`, com seu redimensionamento automatico conforme necessario, evita esse tipo de problema. A biblioteca C fornece funcoes primas de `strcat()` e `strcpy()`, chamadas `strncat()` e `strncpy()`, que funcionam com mais seguranca ao receber um terceiro argumento para indicar o tamanho maximo permitido do array de destino, mas usá-las acrescenta outra camada de complexidade na escrita de programas.

Note a diferente sintaxe usada para obter o numero de caracteres em uma string:

```cpp
int comp1 = str1.size();    // obtem comprimento de str1
int comp2 = strlen(charr1); // obtem comprimento de charr1
```

A funcao `strlen()` e uma funcao regular que recebe uma string no estilo C como argumento e retorna o numero de caracteres na string. A funcao `size()` basicamente faz a mesma coisa, mas a sintaxe e diferente. Em vez de aparecer como argumento de funcao, `str1` precede o nome da funcao e e conectada a ela com um ponto. Como voce viu com o metodo `put()` no Capitulo 3, essa sintaxe indica que `str1` e um objeto e que `size()` e um metodo de classe. Um metodo e uma funcao que pode ser invocada apenas por um objeto pertencente a mesma classe que o metodo. Neste caso especifico, `str1` e um objeto `string`, e `size()` e um metodo de `string`. Resumindo, as funcoes C usam um argumento de funcao para identificar qual string usar, e os objetos da classe C++ `string` usam o nome do objeto e o operador ponto para indicar qual string usar.

## Mais sobre E/S da Classe string

Como voce viu, e possivel usar `cin` com o operador `>>` para ler um objeto `string` e `cout` com o operador `<<` para exibir um objeto `string` usando a mesma sintaxe que se usa com uma string no estilo C. Mas ler uma linha por vez em vez de uma palavra por vez usa uma sintaxe diferente. A Listagem 4.10 mostra essa diferenca.

**Listagem 4.10 — strtype4.cpp**

```cpp
// strtype4.cpp -- entrada de linha
#include <iostream>
#include <string>   // torna a classe string disponivel
#include <cstring>  // biblioteca de strings estilo C
int main()
{
    using namespace std;
    char charr[20];
    string str;
    cout << "Comprimento da string em charr antes da entrada: "
         << strlen(charr) << endl;
    cout << "Comprimento da string em str antes da entrada: "
         << str.size() << endl;
    cout << "Digite uma linha de texto:\n";
    cin.getline(charr, 20);     // indica comprimento maximo
    cout << "Voce digitou: " << charr << endl;
    cout << "Digite outra linha de texto:\n";
    getline(cin, str);           // cin agora e um argumento; sem especificador de comprimento
    cout << "Voce digitou: " << str << endl;
    cout << "Comprimento da string em charr apos a entrada: "
         << strlen(charr) << endl;
    cout << "Comprimento da string em str apos a entrada: "
         << str.size() << endl;
    return 0;
}
```

Aqui esta uma execucao de exemplo do programa na Listagem 4.10:

```
Comprimento da string em charr antes da entrada: 27
Comprimento da string em str antes da entrada: 0
Digite uma linha de texto:
manteiga de amendoim
Voce digitou: manteiga de amendoim
Digite outra linha de texto:
geleia de mirtilo
Voce digitou: geleia de mirtilo
Comprimento da string em charr apos a entrada: 20
Comprimento da string em str apos a entrada: 17
```

Note que o programa diz que o comprimento da string no array `charr` antes da entrada e 27, o que e maior que o tamanho do array! Duas coisas estao acontecendo aqui. A primeira e que o conteudo de um array nao inicializado e indefinido. A segunda e que a funcao `strlen()` funciona comecando no primeiro elemento do array e contando bytes ate chegar a um caractere nulo. Neste caso, o primeiro caractere nulo nao aparece ate varios bytes apos o fim do array. Onde o primeiro caractere nulo aparece em dados nao inicializados e essencialmente aleatorio, portanto voce pode muito bem obter um resultado numerico diferente usando este programa.

Observe tambem que o comprimento da string em `str` antes da entrada e 0. Isso porque um objeto `string` nao inicializado e automaticamente definido com tamanho zero.

Este e o codigo para ler uma linha em um array:

```cpp
cin.getline(charr, 20);
```

A notacao de ponto indica que a funcao `getline()` e um metodo de classe para a classe `istream`. (Lembre-se de que `cin` e um objeto `istream`.) Como mencionado anteriormente, o primeiro argumento indica o array de destino, e o segundo argumento e o tamanho do array, que `getline()` usa para evitar ultrapassar o array.

Este e o codigo para ler uma linha em um objeto `string`:

```cpp
getline(cin, str);
```

Nao ha notacao de ponto, o que indica que este `getline()` nao e um metodo de classe. Portanto, ele recebe `cin` como argumento que lhe diz onde encontrar a entrada. Alem disso, nao ha argumento para o tamanho da string porque o objeto `string` e redimensionado automaticamente para caber na string.

Entao por que um `getline()` e um metodo de classe de `istream` e o outro `getline()` nao e? A classe `istream` fazia parte do C++ muito antes de a classe `string` ser adicionada. Portanto, o design de `istream` reconhece tipos basicos do C++ como `double` e `int`, mas desconhece o tipo `string`. Por isso, ha metodos da classe `istream` para processar `double`, `int` e os outros tipos basicos, mas nao ha metodos da classe `istream` para processar objetos `string`.

Como nao ha metodos da classe `istream` para processar objetos `string`, voce pode se perguntar por que codigo como este funciona:

```cpp
cin >> str; // le uma palavra no objeto str
```

Acontece que codigo como o seguinte usa (em notacao disfarada) uma funcao membro da classe `istream`:

```cpp
cin >> x; // le um valor em um tipo basico do C++
```

Mas o equivalente para a classe `string` usa uma funcao amiga (tambem em notacao disfarada) da classe `string`. Voce tera que aguardar o Capitulo 11 para ver o que e uma funcao amiga e como essa tecnica funciona. Enquanto isso, voce pode usar `cin` e `cout` com objetos `string` sem se preocupar com o funcionamento interno.

## Outras Formas de Literais de String

O C++ tem o tipo `wchar_t` alem do `char`. E o C++11 acrescenta os tipos `char16_t` e `char32_t`. E possivel criar arrays desses tipos e literais de string desses tipos. O C++ usa os prefixos `L`, `u` e `U`, respectivamente, para literais de string desses tipos. Aqui esta um exemplo de como podem ser usados:

```cpp
wchar_t titulo[] = L"Chefe Astronavegador";  // string w_char
char16_t nome[] = u"Felonia Ripova";          // string char_16
char32_t carro[] = U"Humber Super Snipe";     // string char_32
```

O C++11 tambem oferece suporte a um esquema de codificacao para caracteres Unicode chamado UTF-8. Nesse esquema, um determinado caractere pode ser armazenado em qualquer lugar de uma unidade de 8 bits (octet) a quatro unidades de 8 bits, dependendo do valor numerico. O C++ usa o prefixo `u8` para indicar literais de string desse tipo.

Outra adicao do C++11 e a **string bruta** (raw string). Em uma string bruta, os caracteres simplesmente representam a si mesmos. Por exemplo, a sequencia `\n` nao e interpretada como o caractere de nova linha; em vez disso, sao dois caracteres comuns, uma barra invertida e um `n`, e seriam exibidos como esses dois caracteres na tela. Como outro exemplo, voce pode usar um `"` simples dentro de uma string em vez do mais trabalhoso `\"` que usamos na Listagem 4.8. Claro, se voce permite um `"` dentro de um literal de string, nao pode mais usá-lo para delimitar o inicio e o fim da string. Portanto, strings brutas usam `"(` e `)"` como delimitadores, e usam o prefixo `R` para identificá-las como strings brutas:

```cpp
cout << R"(Jim "Rei" Tutt usa "\n" em vez de endl.)" << '\n';
```

Isso exibiria o seguinte:

```
Jim "Rei" Tutt usa \n em vez de endl.
```

O equivalente em literal de string padrao seria isto:

```cpp
cout << "Jim \"Rei\" Tutt usa \"\\n\" em vez de endl." << '\n';
```

Aqui tivemos que usar `\\` para exibir `\` porque um unico `\` e interpretado como o primeiro caractere de uma sequencia de escape.

Se voce pressionar Enter ou Return enquanto digita uma string bruta, isso nao apenas move o cursor para a proxima linha na tela, mas tambem coloca um caractere de retorno de carro (carriage return) na string bruta.

E se voce quiser exibir a combinacao `)"` dentro de uma string bruta? O compilador interpretaria a primeira ocorrencia de `)"` como o fim da string? Sim, interpretaria. Mas a sintaxe de string bruta permite que voce coloque caracteres adicionais entre o `"` de abertura e o `(`. Isso implica que os mesmos caracteres adicionais devem aparecer entre o `)` final e o `"`. Portanto, uma string bruta comecando com `R"+*(` deve terminar com `)+*"`. Assim, a instrucao:

```cpp
cout << R"+*("(Quem nao gostaria?)", ela sussurrou.)+*" << endl;
```

exibiria o seguinte:

```
"(Quem nao gostaria?)", ela sussurrou.
```

Em suma, os delimitadores padrao `"(` e `)"` foram substituidos por `"+*(` e `)+*"`. Voce pode usar qualquer um dos membros do conjunto de caracteres basico como parte do delimitador, exceto o espaco, o parentese esquerdo, o parentese direito, a barra invertida e caracteres de controle como tabulacao ou nova linha.

O prefixo `R` pode ser combinado com outros prefixos de string para produzir strings brutas de `wchar_t` e assim por diante. Ele pode ser a primeira ou a ultima parte de um prefixo composto: `Ru`, `UR` e assim por diante.

---

> Navegacao: [Anterior](capitulo-04-02-strings.md) | [Indice](README.md) | [Proximo](capitulo-04-04-estruturas.md)
