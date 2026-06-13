# Capitulo 2 - Primeiros Passos em C++ (Setting Out to C++)

> Traducao nao oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Texto adaptado para portugues brasileiro com fins educacionais. Paginas originais: 27-64.
> Repositorio: [ring0-study](https://github.com/cnthigu/ring0-study)

> Navegacao: [Anterior](capitulo-01.md) | [Indice](README.md) | [Proximo](capitulo-03.md)

---

Neste capitulo voce vai aprender sobre:

- Como criar um programa C++
- O formato geral de um programa C++
- A diretiva `#include`
- A funcao `main()`
- O uso do objeto `cout` para saida
- Como inserir comentarios em um programa C++
- Como e quando usar `endl`
- Como declarar e usar variaveis
- O uso do objeto `cin` para entrada
- Como definir e usar funcoes simples

Quando voce constroi uma casa simples, começa com a fundacao e a estrutura. Se voce nao tiver uma estrutura solida desde o inicio, tera problemas posteriormente ao preencher os detalhes, como janelas, esquadrias de portas, cupulas de observatorio e salas de baile com parque. Da mesma forma, ao aprender uma linguagem de computador, voce deve comecar aprendendo a estrutura basica de um programa. Somente entao voce podera ir para os detalhes, como loops e objetos. Este capitulo fornece uma visao geral da estrutura essencial de um programa C++ e previa alguns topicos — especialmente funcoes e classes — cobertos com muito mais detalhes em capitulos posteriores. (A ideia e introduzir pelo menos alguns dos conceitos basicos gradualmente a caminho dos grandes despertamentos que virao mais tarde.)

## Iniciacao ao C++

Vamos comecar com um programa C++ simples que exibe uma mensagem. O Listagem 2.1 usa a facilidade `cout` (pronunciado "ci-out") do C++ para produzir saida de caracteres. O codigo-fonte inclui varios comentarios para o leitor; essas linhas comecam com `//`, e o compilador as ignora. O C++ e sensivel a maiusculas e minusculas; isto e, ele discrimina entre caracteres maiusculos e minusculos. Isso significa que voce deve ter cuidado para usar o mesmo caso que nos exemplos. Por exemplo, este programa usa `cout`, e se voce substituir por `Cout` ou `COUT`, o compilador rejeitara sua tentativa e o acusara de usar identificadores desconhecidos. (O compilador tambem e sensivel a ortografia, portanto nao tente `kout` ou `coot` tampouco.) A extensao de arquivo `cpp` e uma forma comum de indicar um programa C++; pode ser necessario usar uma extensao diferente, conforme descrito no Capitulo 1, "Comecando com C++".

**Listagem 2.1 — myfirst.cpp**

```cpp
// myfirst.cpp -- exibe uma mensagem
#include <iostream>        // diretiva do PREPROCESSADOR
int main()                 // cabecalho da funcao
{                          // inicio do corpo da funcao
    using namespace std;   // torna definicoes visiveis
    cout << "Come up and C++ me some time."; // mensagem
    cout << endl;          // inicia nova linha
    cout << "You won't regret it!" << endl;  // mais saida
    return 0;              // termina main()
}                          // fim do corpo da funcao
```

> **Ajustes no programa:**
> Voce pode precisar alterar os exemplos deste livro para rodar no seu sistema. O motivo mais comum e uma questao do ambiente de programacao. Alguns ambientes de janelas rodam o programa em uma janela separada e entao fecham automaticamente a janela quando o programa termina. Para manter a janela aberta ate voce pressionar uma tecla, adicione a seguinte linha de codigo antes da instrucao `return`:
> ```cpp
> cin.get();
> ```
> Para alguns programas voce deve adicionar duas dessas linhas para manter a janela aberta ate pressionar uma tecla.

Depois de usar seu editor de escolha para copiar este programa (ou usar os arquivos de codigo-fonte disponiveis online na pagina web do livro), voce pode usar seu compilador C++ para criar o codigo executavel, conforme o Capitulo 1 descreve. Aqui esta a saida ao rodar o programa compilado do Listagem 2.1:

```
Come up and C++ me some time.
You won't regret it!
```

> **Entrada e Saida em C:**
> Se voce esta acostumado a programar em C, ver `cout` em vez da funcao `printf()` pode ser um pequeno choque. O C++, de fato, pode usar `printf()`, `scanf()` e todas as outras funcoes padrao de entrada e saida de C, desde que voce inclua o arquivo `stdio.h` habitual do C. Mas este e um livro de C++, portanto ele usa as facilidades de entrada do C++, que melhoram em muitos aspectos as versoes de C.

Voce constroi programas C++ a partir de blocos de construcao chamados funcoes. Tipicamente, voce organiza um programa em tarefas principais e entao projeta funcoes separadas para lidar com essas tarefas. O exemplo mostrado no Listagem 2.1 e simples o suficiente para consistir em uma unica funcao chamada `main()`. O exemplo `myfirst.cpp` tem os seguintes elementos:

- Comentarios, indicados pelo prefixo `//`
- Uma diretiva `#include` do preprocessador
- Um cabecalho de funcao: `int main()`
- Uma diretiva `using namespace`
- Um corpo de funcao, delimitado por `{` e `}`
- Instrucoes que usam a facilidade `cout` do C++ para exibir uma mensagem
- Uma instrucao `return` para terminar a funcao `main()`

Vamos examinar esses varios elementos em mais detalhes. A funcao `main()` e um bom lugar para comecar porque alguns dos recursos que precedem `main()`, como a diretiva do preprocessador, sao mais faceis de entender depois que voce ve o que `main()` faz.

## Recursos da Funcao main()

Despido dos enfeites, o programa de exemplo mostrado no Listagem 2.1 tem a seguinte estrutura fundamental:

```cpp
int main()
{
    instrucoes
    return 0;
}
```

Essas linhas declaram que ha uma funcao chamada `main()` e descrevem como ela se comporta. Juntas, elas constituem uma definicao de funcao. Essa definicao tem duas partes: a primeira linha, `int main()`, que e chamada de *cabecalho de funcao* (function header), e a parte entre chaves (`{` e `}`), que e o *corpo da funcao* (function body). A funcao `main()` e ilustrada na Figura 2.1. O cabecalho da funcao e um resumo capsular da interface da funcao com o restante do programa, e o corpo da funcao representa instrucoes ao computador sobre o que a funcao deve fazer. Em C++ cada instrucao completa e chamada de *instrucao* (statement). Voce deve terminar cada instrucao com um ponto e virgula, portanto nao omita os pontos e virgula ao digitar os exemplos.

```
          nome da funcao

          int main()   <- cabecalho da funcao
          {
               instrucoes
                             <- corpo da funcao
               return 0;
          }  <- termina a funcao
```
*Figura 2.1 A funcao `main()`.*

A instrucao final em `main()`, chamada de *instrucao return* (return statement), termina a funcao. Voce aprendera mais sobre a instrucao `return` ao longo deste capitulo.

> **Instrucoes e Ponto e Virgula:**
> Uma instrucao representa uma acao a ser executada. Para entender seu codigo-fonte, um compilador precisa saber quando uma instrucao termina e outra comeca. Algumas linguagens usam um separador de instrucoes. FORTRAN, por exemplo, usa o fim da linha para separar uma instrucao da proxima. Pascal usa um ponto e virgula para separar uma instrucao da proxima. Em Pascal voce pode omitir o ponto e virgula em certos casos, como apos uma instrucao logo antes de um END, quando na verdade voce nao esta separando duas instrucoes. Mas o C++, como o C, usa um ponto e virgula como *terminador*, nao como separador. A diferenca e que um ponto e virgula agindo como terminador faz parte da instrucao em vez de ser um marcador entre instrucoes. O resultado pratico e que em C++ voce nunca deve omitir o ponto e virgula.

### O Cabecalho da Funcao como Interface

Por enquanto, o principal ponto a lembrar e que a sintaxe C++ exige que voce comece a definicao da funcao `main()` com este cabecalho: `int main()`. Este capitulo discute a sintaxe do cabecalho da funcao em mais detalhes posteriormente, na secao "Funcoes", mas para os que nao conseguem adiar a curiosidade, aqui vai uma previa.

Em geral, uma funcao C++ e ativada, ou chamada, por outra funcao, e o cabecalho da funcao descreve a interface entre uma funcao e a funcao que a chama. A parte que precede o nome da funcao e chamada de *tipo de retorno da funcao* (function return type); ela descreve o fluxo de informacoes de uma funcao de volta para a funcao que a chamou. A parte entre os parenteses que seguem o nome da funcao e chamada de *lista de argumentos* (argument list) ou *lista de parametros* (parameter list); ela descreve o fluxo de informacoes da funcao chamadora para a funcao chamada. Essa descricao geral e um pouco confusa quando voce a aplica a `main()` porque normalmente voce nao chama `main()` de outras partes do seu programa. Tipicamente, no entanto, `main()` e chamada pelo codigo de inicializacao que o compilador adiciona ao seu programa para mediar entre o programa e o sistema operacional (Unix, Windows 7, Linux ou qualquer outro). Com efeito, o cabecalho da funcao descreve a interface entre `main()` e o sistema operacional.

Considere a descricao da interface para `main()`, comecando com a parte `int`. Uma funcao C++ chamada por outra funcao pode retornar um valor para a funcao ativadora (chamadora). Esse valor e chamado de *valor de retorno* (return value). Nesse caso, `main()` pode retornar um valor inteiro, conforme indicado pela palavra-chave `int`. Em seguida, observe os parenteses vazios. Em geral, uma funcao C++ pode passar informacoes para outra funcao quando a chama. A parte do cabecalho da funcao entre parenteses descreve essas informacoes. Neste caso, os parenteses vazios significam que a funcao `main()` nao recebe informacoes, ou na terminologia usual, `main()` nao recebe argumentos.

Resumindo, o seguinte cabecalho de funcao declara que a funcao `main()` retorna um valor inteiro para a funcao que a chamou e que `main()` nao recebe informacoes da funcao que a chamou:

```cpp
int main()
```

Muitos programas existentes usam o cabecalho classico de funcao C em vez disso:

```cpp
main() // estilo C original
```

No C classico, omitir o tipo de retorno era o mesmo que dizer que a funcao e do tipo `int`. No entanto, o C++ eliminou esse uso.

Voce tambem pode usar essa variante:

```cpp
int main(void) // estilo muito explicito
```

Usar a palavra-chave `void` nos parenteses e uma forma explicita de dizer que a funcao nao recebe argumentos. Em C++ (mas nao em C), deixar os parenteses vazios e o mesmo que usar `void` nos parenteses. (Em C, deixar os parenteses vazios significa que voce nao esta sendo explicito sobre os argumentos.)

Alguns programadores usam este cabecalho e omitem a instrucao `return`:

```cpp
void main()
```

Isso e logicamente consistente porque um tipo de retorno `void` significa que a funcao nao retorna um valor. No entanto, embora essa variante funcione em alguns sistemas, ela nao faz parte do Padrao C++. Assim, em outros sistemas ela falha. Portanto, voce deve evitar essa forma e usar a forma do Padrao C++.

Por fim, o Padrao ISO C++ faz uma concessao para aqueles que reclamam da necessidade de ter que colocar uma instrucao `return` no final de `main()`. Se o compilador chegar ao fim de `main()` sem encontrar uma instrucao `return`, o efeito sera o mesmo que se voce terminasse `main()` com esta instrucao:

```cpp
return 0;
```

Esse `return` implicito e fornecido apenas para `main()` e nao para nenhuma outra funcao.

### Por Que main() Tem Esse Nome e Nao Outro

Ha uma razao extremamente convincente para nomear a funcao no programa `myfirst.cpp` como `main()`: voce deve faze-lo. Normalmente, um programa C++ requer uma funcao chamada `main()`. (E nao, a proposito, `Main()`, `MAIN()` ou `mane()`. Lembre-se, maiusculas e minusculas e ortografia contam.) Como o programa `myfirst.cpp` tem apenas uma funcao, essa funcao deve ter a responsabilidade de ser `main()`. Quando voce roda um programa C++, a execucao sempre começa no inicio da funcao `main()`. Portanto, se voce nao tiver `main()`, nao tera um programa completo, e o compilador aponta que voce nao definiu uma funcao `main()`.

Ha excecoes. Por exemplo, na programacao Windows voce pode escrever um modulo de biblioteca de vinculacao dinamica (DLL). Esse e um codigo que outros programas Windows podem usar. Como um modulo DLL nao e um programa independente, ele nao precisa de `main()`. Programas para ambientes especializados, como para um chip controlador em um robo, podem nao precisar de `main()`. Alguns ambientes de programacao fornecem um programa esqueleto que chama alguma funcao nao padrao, como `_tmain()`; nesse caso, ha um `main()` oculto que chama `_tmain()`. Mas seu programa independente comum precisa de `main()`; este livro discute esse tipo de programa.

## Comentarios em C++

A barra dupla (`//`) introduz um comentario C++. Um *comentario* e uma observacao do programador para o leitor que geralmente identifica uma secao de um programa ou explica algum aspecto do codigo. O compilador ignora os comentarios. Afinal, ele sabe C++ pelo menos tao bem quanto voce, e de qualquer forma e incapaz de entender comentarios. Do ponto de vista do compilador, o Listagem 2.1 parece ter sido escrito sem comentarios, assim:

```cpp
#include <iostream>
int main()
{
    using namespace std;
    cout << "Come up and C++ me some time.";
    cout << endl;
    cout << "You won't regret it!" << endl;
    return 0;
}
```

Os comentarios C++ vao do `//` ate o final da linha. Um comentario pode estar em sua propria linha ou na mesma linha que o codigo. Alias, observe a primeira linha do Listagem 2.1:

```cpp
// myfirst.cpp -- exibe uma mensagem
```

Neste livro, todos os programas comecam com um comentario que fornece o nome do arquivo para o codigo-fonte e um breve resumo do programa. Conforme mencionado no Capitulo 1, a extensao do nome do arquivo para o codigo-fonte depende do seu sistema C++. Outros sistemas podem usar `myfirst.C` ou `myfirst.cxx` para nomes.

> **Dica:** Voce deve usar comentarios para documentar seus programas. Quanto mais complexo o programa, mais valiosos sao os comentarios. Eles nao so ajudam outros a entender o que voce fez, mas tambem ajudam voce a entender o que fez, especialmente se voce nao olhou o programa por algum tempo.

> **Comentarios estilo C:**
> O C++ tambem reconhece os comentarios de C, que ficam entre os simbolos `/*` e `*/`:
> ```cpp
> #include <iostream> /* um comentario estilo C */
> ```
> Como o comentario estilo C e terminado por `*/` em vez de pelo fim de uma linha, voce pode espalhá-lo por mais de uma linha. Voce pode usar um ou ambos os estilos em seus programas. No entanto, tente ficar com o estilo C++. Como ele nao envolve lembrar de emparelhar corretamente um simbolo de fim com um de inicio, e menos provavel que cause problemas. De fato, o C99 adicionou o comentario `//` a linguagem C.

## O Preprocessador C++ e o Arquivo iostream

Aqui esta a versao resumida do que voce precisa saber. Se o seu programa for usar as facilidades habituais de entrada ou saida do C++, voce fornece estas duas linhas:

```cpp
#include <iostream>
using namespace std;
```

Ha algumas alternativas para usar a segunda linha, mas vamos manter as coisas simples por enquanto. (Se o seu compilador nao gostar dessas linhas, ele nao e compativel com C++98 e tera muitos outros problemas com os exemplos deste livro.) Isso e tudo que voce realmente precisa saber para fazer seus programas funcionar, mas agora vamos dar uma olhada mais aprofundada.

O C++, como o C, usa um *preprocessador*. Este e um programa que processa um arquivo de codigo-fonte antes da compilacao principal ocorrer. Voce nao precisa fazer nada de especial para invocar esse preprocessador. Ele opera automaticamente quando voce compila o programa.

O Listagem 2.1 usa a diretiva `#include`:

```cpp
#include <iostream> // uma diretiva do PREPROCESSADOR
```

Essa diretiva faz com que o preprocessador adicione o conteudo do arquivo `iostream` ao seu programa. Essa e uma acao tipica do preprocessador: adicionar ou substituir texto no codigo-fonte antes que ele seja compilado.

Isso levanta a questao de por que voce deve adicionar o conteudo do arquivo `iostream` ao programa. A resposta diz respeito a comunicacao entre o programa e o mundo externo. O *io* em `iostream` refere-se a entrada (input), que e informacao trazida para o programa, e a saida (output), que e informacao enviada para fora do programa. O esquema de entrada/saida do C++ envolve varias definicoes encontradas no arquivo `iostream`. Seu primeiro programa precisa dessas definicoes para usar a facilidade `cout` para exibir uma mensagem. A diretiva `#include` faz com que o conteudo do arquivo `iostream` seja enviado junto com o conteudo do seu arquivo para o compilador. Em essencia, o conteudo do arquivo `iostream` substitui a linha `#include <iostream>` no programa. Seu arquivo original nao e alterado, mas um arquivo composto formado pelo seu arquivo e `iostream` vai para o proximo estagio de compilacao.

> **Nota:** Programas que usam `cin` e `cout` para entrada e saida devem incluir o arquivo `iostream`.

### Nomes de Arquivos de Cabecalho

Arquivos como `iostream` sao chamados de *arquivos de inclusao* (include files) — porque sao incluidos em outros arquivos — ou *arquivos de cabecalho* (header files) — porque sao incluidos no inicio de um arquivo. Os compiladores C++ vem com muitos arquivos de cabecalho, cada um suportando uma familia especifica de facilidades. A tradicao do C era usar a extensao `h` com arquivos de cabecalho como uma forma simples de identificar o tipo de arquivo pelo nome. Por exemplo, o arquivo de cabecalho `math.h` do C suporta varias funcoes matematicas de C. Inicialmente, o C++ fazia o mesmo. Por exemplo, o arquivo de cabecalho que suporta entrada e saida era chamado `iostream.h`. Mas o uso do C++ mudou. Agora a extensao `h` e reservada para os antigos arquivos de cabecalho do C (que programas C++ ainda podem usar), enquanto os arquivos de cabecalho do C++ nao tem extensao. Ha tambem arquivos de cabecalho do C que foram convertidos para C++. Esses arquivos foram renomeados eliminando a extensao `h` (tornando-o um nome estilo C++) e prefixando o nome do arquivo com um `c` (indicando que vem do C). Por exemplo, a versao C++ de `math.h` e o arquivo de cabecalho `cmath`.

A Tabela 2.1 resume as convencoes de nomenclatura para arquivos de cabecalho.

**Tabela 2.1 — Convencoes de nomenclatura para arquivos de cabecalho**

| Tipo de cabecalho    | Convencao            | Exemplo       | Comentarios                                                      |
|----------------------|----------------------|---------------|------------------------------------------------------------------|
| C++ estilo antigo    | Termina em `.h`      | `iostream.h`  | Usavel por programas C++                                         |
| C estilo antigo      | Termina em `.h`      | `math.h`      | Usavel por programas C e C++                                     |
| C++ novo estilo      | Sem extensao         | `iostream`    | Usavel por programas C++, usa namespace std                      |
| C convertido         | Prefixo c, sem ext.  | `cmath`       | Usavel por programas C++, pode usar recursos nao-C como namespace std |

### Namespaces

Se voce usar `iostream` em vez de `iostream.h`, deve usar a seguinte diretiva de namespace para tornar as definicoes em `iostream` disponiveis ao seu programa:

```cpp
using namespace std;
```

Isso e chamado de *diretiva using* (using directive). A coisa mais simples a fazer e aceitar isso por enquanto e se preocupar com isso mais tarde (por exemplo, no Capitulo 9, "Modelos de Memoria e Namespaces"). Mas para que voce nao fique completamente no escuro, aqui esta uma visao geral do que esta acontecendo.

O suporte a namespace (espaco de nomes) e um recurso do C++ projetado para simplificar a escrita de programas grandes e de programas que combinam codigo preexistente de varios fornecedores e para ajudar a organizar programas. Um problema potencial e que voce pode usar dois produtos preempacotados que ambos tenham, digamos, uma funcao chamada `wanda()`. Se voce usar a funcao `wanda()`, o compilador nao saberá qual versao voce quer dizer. A facilidade de namespace permite que um fornecedor empacote seus produtos em uma unidade chamada *namespace*, de modo que voce possa usar o nome do namespace para indicar qual produto do fornecedor voce quer. Assim, Microflop Industries poderia colocar suas definicoes em um namespace chamado `Microflop`. Entao `Microflop::wanda()` se tornaria o nome completo de sua funcao `wanda()`. Da mesma forma, `Piscine::wanda()` poderia denotar a versao da Piscine Corporation de `wanda()`. Assim, seu programa agora poderia usar os namespaces para discriminar entre varias versoes:

```cpp
Microflop::wanda("ir dancar?");         // usa versao do namespace Microflop
Piscine::wanda("um peixe chamado Desejo"); // usa versao do namespace Piscine
```

Nesse espirito, as classes, funcoes e variaveis que sao um componente padrao dos compiladores C++ agora sao colocadas em um namespace chamado `std`. Isso ocorre nos arquivos de cabecalho sem extensao `h`. Isso significa, por exemplo, que a variavel `cout` usada para saida e definida em `iostream` e na verdade e chamada de `std::cout`, e que `endl` e na verdade `std::endl`. Portanto, voce pode omitir a diretiva `using` e, em vez disso, codificar no seguinte estilo:

```cpp
std::cout << "Come up and C++ me some time.";
std::cout << std::endl;
```

No entanto, muitos usuarios nao querem converter o codigo pre-namespace, que usa `iostream.h` e `cout`, para codigo de namespace, que usa `iostream` e `std::cout`, a menos que possam faze-lo sem muito trabalho. E aqui que a diretiva `using` entra. A seguinte linha significa que voce pode usar nomes definidos no namespace `std` sem usar o prefixo `std::`:

```cpp
using namespace std;
```

Essa diretiva `using` torna todos os nomes no namespace `std` disponiveis. A pratica moderna considera isso um tanto preguicoso e potencialmente um problema em projetos grandes. As abordagens preferidas sao usar o qualificador `std::` ou usar algo chamado de *declaracao using* (using declaration) para tornar apenas nomes especificos disponiveis:

```cpp
using std::cout;  // torna cout disponivel
using std::endl;  // torna endl disponivel
using std::cin;   // torna cin disponivel
```

Se voce usar essas diretivas em vez de `using namespace std;`, podera usar `cin` e `cout` sem o `std::`. Mas se precisar usar outros nomes de `iostream`, tera que adicioná-los a lista individualmente. Este livro inicialmente usa a abordagem preguicosa por alguns motivos. Primeiro, para programas simples, nao e realmente um grande problema qual tecnica de gerenciamento de namespace voce usa. Segundo, prefiro enfatizar os aspectos mais basicos sobre o aprendizado de C++. Mais tarde, o livro usa as outras tecnicas de namespace.

## Saida C++ com cout

Agora vamos ver como exibir uma mensagem. O programa `myfirst.cpp` usa a seguinte instrucao C++:

```cpp
cout << "Come up and C++ me some time.";
```

A parte entre aspas duplas e a mensagem a ser impressa. Em C++, qualquer serie de caracteres entre aspas duplas e chamada de *string de caracteres* (character string), presumivelmente porque consiste em varios caracteres enfileirados formando uma unidade maior. A notacao `<<` indica que a instrucao esta enviando a string para `cout`; os simbolos apontam na direcao em que a informacao flui. E o que e `cout`? E um objeto predefinido que sabe como exibir uma variedade de coisas, incluindo strings, numeros e caracteres individuais. (Um objeto, como voce deve lembrar do Capitulo 1, e uma instancia especifica de uma classe, e uma classe define como os dados sao armazenados e usados.)

Bem, usar objetos tao cedo e um pouco estranho porque voce nao aprendera sobre objetos por mais alguns capitulos. Na verdade, isso revela um dos pontos fortes dos objetos. Voce nao precisa conhecer o interior de um objeto para usa-lo. Tudo que voce precisa saber e sua interface — ou seja, como usa-lo. O objeto `cout` tem uma interface simples. Se `str` representa uma string, voce pode fazer o seguinte para exibi-la:

```cpp
cout << str;
```

Isso e tudo que voce precisa saber para exibir uma string, mas agora veja como a visao conceitual do C++ representa o processo. Nessa visao, a saida e um *fluxo* (stream) — isto e, uma serie de caracteres fluindo do programa. O objeto `cout`, cujas propriedades sao definidas no arquivo `iostream`, representa esse fluxo. As propriedades do objeto para `cout` incluem um *operador de insercao* (`<<`) que insere as informacoes a sua direita no fluxo. Considere a seguinte instrucao (observe o ponto e virgula terminador):

```cpp
cout << "Come up and C++ me some time.";
```

Ela insere a string `"Come up and C++ me some time."` no fluxo de saida. Assim, em vez de dizer que seu programa exibe uma mensagem, voce pode dizer que ele insere uma string no fluxo de saida.

> **Um Primeiro Olhar sobre a Sobrecarga de Operadores:**
> Se voce vem ao C++ do C, provavelmente notou que o operador de insercao (`<<`) se parece exatamente com o operador de deslocamento a esquerda bit a bit (`<<`). Este e um exemplo de *sobrecarga de operadores* (operator overloading), pelo qual o mesmo simbolo de operador pode ter significados diferentes. O compilador usa o contexto para descobrir qual significado se pretende. O proprio C tem alguma sobrecarga de operadores. Por exemplo, o simbolo `&` representa tanto o operador de endereco quanto o operador AND bit a bit. O simbolo `*` representa tanto a multiplicacao quanto o desreferenciamento de um ponteiro. O ponto importante aqui nao e a funcao exata desses operadores, mas que o mesmo simbolo pode ter mais de um significado, com o compilador determinando o significado correto a partir do contexto. O C++ estende o conceito de sobrecarga de operadores permitindo que voce redefina os significados dos operadores para os tipos definidos pelo usuario chamados classes.

## O Manipulador endl

Agora vamos examinar uma notacao de aparencia estranha que aparece na segunda instrucao de saida no Listagem 2.1:

```cpp
cout << endl;
```

`endl` e uma notacao especial do C++ que representa o importante conceito de comecar uma nova linha. Inserir `endl` no fluxo de saida faz com que o cursor da tela se mova para o inicio da proxima linha. Notacoes especiais como `endl`, que tem significados especificos para `cout`, sao chamadas de *manipuladores* (manipulators). Como `cout`, `endl` e definido no arquivo de cabecalho `iostream` e faz parte do namespace `std`.

Observe que a facilidade `cout` nao se move automaticamente para a proxima linha quando imprime uma string, portanto a primeira instrucao `cout` do Listagem 2.1 deixa o cursor posicionado logo apos o ponto no final da string de saida. A saida de cada instrucao `cout` comeca onde a ultima saida terminou, portanto omitir `endl` resultaria nesta saida para o Listagem 2.1:

```
Come up and C++ me some time.You won't regret it!
```

Observe que o `Y` segue imediatamente o ponto. Vamos ver outro exemplo. Suponha que voce tente este codigo:

```cpp
cout << "The Good, the";
cout << "Bad, ";
cout << "and the Ukulele";
cout << endl;
```

Ele produz a seguinte saida:

```
The Good, theBad, and the Ukulele
```

Novamente, observe que o inicio de uma string vem imediatamente apos o fim da string anterior. Se voce quiser um espaco onde duas strings se unem, deve inclui-lo em uma das strings.

### O Caractere de Nova Linha

O C++ tem outra maneira, mais antiga, de indicar uma nova linha na saida — a notacao `\n` do C:

```cpp
cout << "What's next?\n"; // \n significa iniciar nova linha
```

A combinacao `\n` e considerada um unico caractere chamado de *caractere de nova linha* (newline character).

Se voce estiver exibindo uma string, voce precisa de menos digitacao para incluir a nova linha como parte da string do que adicionar `endl` no final:

```cpp
cout << "Pluto is a dwarf planet.\n"; // exibe texto, vai para proxima linha
cout << "Pluto is a dwarf planet." << endl; // exibe texto, vai para proxima linha
```

Por outro lado, se voce quiser gerar uma nova linha por si so, ambas as abordagens requerem a mesma quantidade de digitacao, mas a maioria das pessoas acha as teclas para `endl` mais confortaveis:

```cpp
cout << "\n";    // inicia nova linha
cout << endl;    // inicia nova linha
```

Normalmente, este livro usa um caractere de nova linha embutido (`\n`) ao exibir strings entre aspas e o manipulador `endl` em outros casos. Uma diferenca e que `endl` garante que a saida sera descarregada (flushed) — neste caso, exibida imediatamente na tela — antes de o programa continuar. Voce nao tem essa garantia com `"\n"`, o que significa que e possivel que em alguns sistemas e em algumas circunstancias um prompt nao seja exibido ate depois que voce inserir a informacao que esta sendo solicitada.

O caractere de nova linha e um exemplo de combinacoes especiais de teclas chamadas de *sequencias de escape* (escape sequences); elas sao discutidas mais no Capitulo 3, "Lidando com Dados".

## Formatacao do Codigo-Fonte C++

Algumas linguagens, como FORTRAN, sao orientadas por linha, com uma instrucao por linha. Para essas linguagens, o retorno de carro (gerado pressionando a tecla Enter ou Return) serve para separar instrucoes. Em C++, no entanto, o ponto e virgula marca o fim de cada instrucao. Isso deixa o C++ livre para tratar o retorno de carro da mesma forma que um espaco ou uma tabulacao. Ou seja, em C++ voce normalmente pode usar um espaco onde usaria um retorno de carro e vice-versa. Isso significa que voce pode distribuir uma unica instrucao por varias linhas ou colocar varias instrucoes em uma linha. Por exemplo, voce poderia reformatar `myfirst.cpp` assim:

```cpp
#include <iostream>
   int
main
() { using
   namespace
      std; cout
          <<
"Come up and C++ me some time."
;  cout <<
endl; cout <<
"You won't regret it!" <<
endl;return 0; }
```

Isso e visualmente feio, mas codigo valido. Voce tem que observar algumas regras. Em particular, em C e C++ voce nao pode colocar um espaco, tabulacao ou retorno de carro no meio de um elemento como um nome, nem pode colocar um retorno de carro no meio de uma string. Aqui estao exemplos do que voce nao pode fazer:

```cpp
int ma in()      // INVALIDO -- espaco no nome
re
turn 0;          // INVALIDO -- retorno de carro na palavra
cout << "Behold the Beans
 of Beauty!";    // INVALIDO -- retorno de carro na string
```

(No entanto, a string bruta (raw string), adicionada pelo C++11 e discutida brevemente no Capitulo 4, permite incluir um retorno de carro em uma string.)

### Tokens e Espacos em Branco no Codigo-Fonte

Os elementos indivisiveis em uma linha de codigo sao chamados de *tokens* (veja a Figura 2.3). Em geral, voce deve separar um token do proximo com um espaco, tabulacao ou retorno de carro, que coletivamente sao chamados de *espaco em branco* (white space). Alguns caracteres isolados, como parenteses e virgulas, sao tokens que nao precisam ser separados por espaco em branco. Aqui estao alguns exemplos que ilustram quando o espaco em branco pode ser usado e quando pode ser omitido:

```cpp
return0;    // INVALIDO, deve ser return 0;
return(0);  // VALIDO, espaco em branco omitido
return (0); // VALIDO, espaco em branco usado
intmain();  // INVALIDO, espaco em branco omitido
int main()  // VALIDO, espaco em branco omitido em ()
int main ( ) // TAMBEM VALIDO, espaco em branco usado em ( )
```

### Estilo do Codigo-Fonte C++

Embora o C++ lhe de muita liberdade de formatacao, seus programas serao mais faceis de ler se voce seguir um estilo razoavel. A maioria dos programadores usa estilos semelhantes ao do Listagem 2.1, que observa estas regras:

- Uma instrucao por linha
- Uma chave de abertura e uma chave de fechamento para uma funcao, cada uma em sua propria linha
- Instrucoes em uma funcao recuadas em relacao as chaves
- Sem espaco em branco ao redor dos parenteses associados a um nome de funcao

As tres primeiras regras tem a simples intencao de manter o codigo limpo e legivel. A quarta ajuda a diferenciar funcoes de algumas estruturas internas do C++, como loops, que tambem usam parenteses.

## Instrucoes C++

Um programa C++ e uma colecao de funcoes, e cada funcao e uma colecao de instrucoes. O C++ tem varios tipos de instrucoes, entao vamos ver algumas das possibilidades. O Listagem 2.2 fornece dois novos tipos de instrucoes. Primeiro, uma *instrucao de declaracao* (declaration statement) cria uma variavel. Segundo, uma *instrucao de atribuicao* (assignment statement) fornece um valor para essa variavel. Alem disso, o programa mostra uma nova capacidade para `cout`.

**Listagem 2.2 — cenouras.cpp**

```cpp
// cenouras.cpp -- programa de processamento de alimentos
// usa e exibe uma variavel
#include <iostream>

int main()
{
    using namespace std;
    int cenouras;           // declara uma variavel inteira

    cenouras = 25;          // atribui um valor a variavel
    cout << "Tenho ";
    cout << cenouras;       // exibe o valor da variavel
    cout << " cenouras.";
    cout << endl;
    cenouras = cenouras - 1; // modifica a variavel
    cout << "Nhac, nhac. Agora tenho " << cenouras << " cenouras." << endl;
    return 0;
}
```

Uma linha em branco separa a declaracao do restante do programa. Essa pratica e a convencao usual do C, mas e um pouco menos comum em C++. Aqui esta a saida do programa para o Listagem 2.2:

```
Tenho 25 cenouras.
Nhac, nhac. Agora tenho 24 cenouras.
```

### Instrucoes de Declaracao e Variaveis

Os computadores sao maquinas precisas e ordenadas. Para armazenar um item de informacao em um computador, voce deve identificar tanto o local de armazenamento quanto a quantidade de espaco de memoria que a informacao requer. Uma forma relativamente indolor de fazer isso em C++ e usar uma *instrucao de declaracao* para indicar o tipo de armazenamento e fornecer um rotulo para o local. Por exemplo, o programa no Listagem 2.2 tem esta instrucao de declaracao (observe o ponto e virgula):

```cpp
int cenouras;
```

Essa instrucao fornece dois tipos de informacao: o tipo de armazenamento de memoria necessario e um rotulo a ser anexado a esse armazenamento. Em particular, a instrucao declara que o programa requer espaco suficiente para armazenar um inteiro, para o qual o C++ usa o rotulo `int`. O compilador cuida dos detalhes de alocacao e rotulagem de memoria para essa tarefa. O C++ pode lidar com varios tipos de dados, e o `int` e o tipo de dado mais basico. Ele corresponde a um inteiro, um numero sem parte fracionaria. O tipo `int` do C++ pode ser positivo ou negativo, mas o intervalo de tamanho depende da implementacao. O Capitulo 3 fornece os detalhes sobre `int` e os outros tipos basicos.

Nomear o armazenamento e a segunda tarefa realizada. Neste caso, a instrucao de declaracao declara que, a partir de agora, o programa usara o nome `cenouras` para identificar o valor armazenado naquele local. `cenouras` e chamada de *variavel* (variable) porque voce pode alterar seu valor. Em C++ voce deve declarar todas as variaveis. Se voce omitir a declaracao em `cenouras.cpp`, o compilador relatara um erro quando o programa tentar usar `cenouras` mais adiante.

> **Por Que as Variaveis Devem Ser Declaradas?**
> Algumas linguagens, notadamente BASIC, criam uma nova variavel sempre que voce usa um novo nome, sem a ajuda de declaracoes explicitas. Isso pode parecer mais amigavel ao usuario e e — a curto prazo. O problema e que, se voce escrever incorretamente o nome de uma variavel, pode inadvertidamente criar uma nova variavel sem perceber. Isso e, em BASIC, voce pode fazer algo como o seguinte:
> ```
> CasteloEscuro = 34
> ...
> CasteloEscru = CasteloEscuro + MaisFantasmas
> ...
> PRINT CasteloEscuro
> ```
> Como `CasteloEscru` esta com erro de grafia (o `o` foi digitado como nada), as alteracoes feitas nele deixam `CasteloEscuro` inalterado. Esse tipo de erro pode ser dificil de rastrear porque nao viola nenhuma regra em BASIC. No entanto, em C++, `CasteloEscuro` seria declarado enquanto o mal-soletrado `CasteloEscru` nao seria declarado. Portanto, o codigo C++ equivalente viola a regra sobre a necessidade de declarar uma variavel para usa-la, portanto o compilador captura o erro.

Em geral, entao, uma declaracao indica o tipo de dados a serem armazenados e o nome que o programa usara para os dados armazenados. Neste caso especifico, o programa cria uma variavel chamada `cenouras` na qual pode armazenar um inteiro.

A instrucao de declaracao no programa e chamada de *instrucao de declaracao definidora* (defining declaration statement), ou simplesmente *definicao* (definition), que significa que sua presenca faz com que o compilador aloque espaco de memoria para a variavel. Em situacoes mais complexas, voce tambem pode ter declaracoes de referencia. Essas dizem ao computador para usar uma variavel que ja foi definida em outro lugar. Em geral, uma declaracao nao precisa ser uma definicao, mas neste exemplo ela e.

Se voce esta familiarizado com C ou Pascal, ja conhece as declaracoes de variaveis. Voce tambem pode ter uma pequena surpresa. Em C e Pascal, todas as declaracoes de variaveis normalmente vem no inicio de uma funcao ou procedimento. Mas o C++ nao tem tal restricao. Na verdade, o estilo usual do C++ e declarar uma variavel logo antes de ser usada pela primeira vez. Dessa forma, voce nao precisa vasculhar o programa para ver qual e o tipo. Voce vera um exemplo disso mais adiante neste capitulo.

> **Dica:** O estilo C++ para declarar variaveis e declara-las o mais proximo possivel de seu primeiro uso.

### Instrucoes de Atribuicao

Uma instrucao de atribuicao atribui um valor a um local de armazenamento. Por exemplo, a instrucao a seguir atribui o inteiro `25` ao local representado pela variavel `cenouras`:

```cpp
cenouras = 25;
```

O simbolo `=` e chamado de *operador de atribuicao* (assignment operator). Uma caracteristica incomum do C++ (e do C) e que voce pode usar o operador de atribuicao em serie. Por exemplo, o seguinte e um codigo valido:

```cpp
int teclado;
int piano;
int violao;
violao = piano = teclado = 88;
```

A atribuicao funciona da direita para a esquerda. Primeiro, `88` e atribuido a `teclado`; em seguida, o valor de `teclado`, que agora e `88`, e atribuido a `piano`; em seguida, o valor de `piano` de `88` e atribuido a `violao`.

A segunda instrucao de atribuicao no Listagem 2.2 demonstra que voce pode alterar o valor de uma variavel:

```cpp
cenouras = cenouras - 1; // modifica a variavel
```

A expressao a direita do operador de atribuicao (`cenouras - 1`) e um exemplo de expressao aritmetica. O computador subtrairá `1` de `25`, o valor de `cenouras`, obtendo `24`. O operador de atribuicao entao armazena esse novo valor no local `cenouras`.

### Um Novo Truque para cout

Ate agora, os exemplos neste capitulo deram a `cout` strings para imprimir. O Listagem 2.2 tambem da a `cout` uma variavel cujo valor e um inteiro:

```cpp
cout << cenouras;
```

O programa nao imprime a palavra `cenouras`; em vez disso, imprime o valor inteiro armazenado em `cenouras`, que e `25`. Na verdade, isso sao dois truques em um. Primeiro, `cout` substitui `cenouras` por seu valor numerico atual de `25`. Segundo, ele traduz o valor para os caracteres de saida apropriados.

Como voce pode ver, `cout` funciona com strings e inteiros. Isso pode nao parecer particularmente notavel para voce, mas tenha em mente que o inteiro `25` e algo bastante diferente da string `"25"`. A string contem os caracteres com os quais voce escreve o numero (isto e, um caractere `2` e um caractere `5`). O programa armazena internamente os codigos numericos para o caractere `2` e o caractere `5`. Para imprimir a string, `cout` simplesmente imprime cada caractere na string. Mas o inteiro `25` e armazenado como um valor numerico. Em vez de armazenar cada digito separadamente, o computador armazena `25` como um numero binario. (O Apendice A, "Bases Numericas", discute essa representacao.) O principal ponto aqui e que `cout` deve traduzir um numero em forma de inteiro para forma de caractere antes de poder imprimi-lo. Alem disso, `cout` e inteligente o suficiente para reconhecer que `cenouras` e um inteiro que requer conversao.

Talvez o contraste com o C antigo indique o quanto `cout` e inteligente. Para imprimir a string `"25"` e o inteiro `25` em C, voce poderia usar a funcao de saida multiproposito de C, `printf()`:

```cpp
printf("Imprimindo uma string: %s\n", "25");
printf("Imprimindo um inteiro: %d\n", 25);
```

Sem entrar nas intricacias de `printf()`, observe que voce deve usar codigos especiais (`%s` e `%d`) para indicar se vai imprimir uma string ou um inteiro. E se voce disser a `printf()` para imprimir uma string, mas der a ela um inteiro por engano, `printf()` e sofisticado demais para notar seu erro. Ele apenas segue em frente e exibe lixo.

A maneira inteligente em que `cout` se comporta deriva dos recursos orientados a objetos do C++. Em essencia, o operador de insercao do C++ (`<<`) ajusta seu comportamento para se adequar ao tipo de dado que o segue. Este e um exemplo de sobrecarga de operadores. Em capitulos posteriores, quando voce estudar a sobrecarga de funcoes e a sobrecarga de operadores, aprenderá como implementar esses designs inteligentes voce mesmo.

## Mais Instrucoes C++

Vamos ver mais alguns exemplos de instrucoes. O programa no Listagem 2.3 expande o exemplo anterior, permitindo que voce insira um valor enquanto o programa esta sendo executado. Para isso, ele usa `cin` (pronunciado "ci-in"), a contraparte de entrada para `cout`. Alem disso, o programa mostra mais uma maneira de usar o versatil objeto `cout`.

**Listagem 2.3 — obterinfo.cpp**

```cpp
// obterinfo.cpp -- entrada e saida
#include <iostream>

int main()
{
    using namespace std;
    int cenouras;

    cout << "Quantas cenouras voce tem?" << endl;
    cin >> cenouras;                                // entrada C++
    cout << "Aqui estao mais duas. ";
    cenouras = cenouras + 2;
    // a proxima linha concatena a saida
    cout << "Agora voce tem " << cenouras << " cenouras." << endl;
    return 0;
}
```

Aqui esta um exemplo de saida do programa no Listagem 2.3:

```
Quantas cenouras voce tem?
12
Aqui estao mais duas. Agora voce tem 14 cenouras.
```

O programa tem dois novos recursos: usar `cin` para ler entrada do teclado e combinar quatro instrucoes de saida em uma. Vamos dar uma olhada.

### Usando cin

Conforme a saida do Listagem 2.3 demonstra, o valor digitado no teclado (`12`) e eventualmente atribuido a variavel `cenouras`. A instrucao a seguir realiza essa maravilha:

```cpp
cin >> cenouras;
```

Olhando para essa instrucao, voce pode praticamente ver as informacoes fluindo de `cin` para `cenouras`. Naturalmente, ha uma descricao um pouco mais formal desse processo. Assim como o C++ considera a saida como um fluxo de caracteres saindo do programa, ele considera a entrada como um fluxo de caracteres entrando no programa. O arquivo `iostream` define `cin` como um objeto que representa esse fluxo. Para saida, o operador `<<` insere caracteres no fluxo de saida. Para entrada, `cin` usa o operador `>>` para extrair caracteres do fluxo de entrada. Tipicamente, voce fornece uma variavel a direita do operador para receber as informacoes extraidas. (Os simbolos `<<` e `>>` foram escolhidos para sugerir visualmente a direcao em que as informacoes fluem.)

Como `cout`, `cin` e um objeto inteligente. Ele converte a entrada, que e apenas uma serie de caracteres digitados no teclado, em uma forma aceitavel para a variavel que recebe as informacoes. Neste caso, o programa declara `cenouras` como uma variavel inteira, portanto a entrada e convertida para a forma numerica que o computador usa para armazenar inteiros.

### Concatenando com cout

O segundo novo recurso de `obterinfo.cpp` e combinar quatro instrucoes de saida em uma. O arquivo `iostream` define o operador `<<` de modo que voce possa combinar (ou seja, concatenar) a saida da seguinte forma:

```cpp
cout << "Agora voce tem " << cenouras << " cenouras." << endl;
```

Isso permite combinar saida de string e saida de inteiro em uma unica instrucao. A saida resultante e a mesma que o seguinte codigo produziria:

```cpp
cout << "Agora voce tem ";
cout << cenouras;
cout << " cenouras";
cout << endl;
```

Voce tambem pode reescrever a versao concatenada assim, distribuindo a unica instrucao por quatro linhas:

```cpp
cout << "Agora voce tem "
     << cenouras
     << " cenouras."
     << endl;
```

Isso ocorre porque as regras de formato livre do C++ tratam novas linhas e espacos entre tokens de forma intercambivel. Essa ultima tecnica e conveniente quando a largura da linha limita seu estilo.

## cin e cout: Uma Pitada de Classes

Voce ja viu o suficiente de `cin` e `cout` para justificar sua exposicao a um pouco de lore de objetos. Em particular, nesta secao voce aprendera mais sobre a nocao de classes. Conforme o Capitulo 1 descreveu brevemente, as classes sao um dos conceitos centrais para a programacao orientada a objetos (POO) em C++.

Uma *classe* e um tipo de dado definido pelo usuario. Para definir uma classe, voce descreve que tipo de informacao ela pode representar e que tipo de acoes voce pode executar com esses dados. Uma classe tem a mesma relacao com um objeto que um tipo tem com uma variavel. Ou seja, uma definicao de classe descreve uma forma de dado e como ela pode ser usada, enquanto um objeto e uma entidade criada de acordo com a especificacao da forma de dado. Ou, em termos nao computacionais, se uma classe e analoga a uma categoria como atores famosos, entao um objeto e analogo a um exemplo especifico dessa categoria, como Kermit the Frog.

Para ser mais especifico: lembre-se da seguinte declaracao de uma variavel:

```cpp
int cenouras;
```

Isso cria uma variavel especifica (`cenouras`) que tem as propriedades do tipo `int`. Isso e, `cenouras` pode armazenar um inteiro e pode ser usado de maneiras especificas — para adicao e subtracao, por exemplo. Agora considere `cout`. E um objeto criado para ter as propriedades da classe `ostream`. A definicao de classe `ostream` (outro habitante do arquivo `iostream`) descreve o tipo de dado que um objeto `ostream` representa e as operacoes que voce pode executar com e nele, como inserir um numero ou string em um fluxo de saida. Da mesma forma, `cin` e um objeto criado com as propriedades da classe `istream`, tambem definida em `iostream`.

> **Nota:** A classe descreve todas as propriedades de um tipo de dado, incluindo acoes que podem ser executadas com ele, e um objeto e uma entidade criada de acordo com essa descricao.

Voce aprendeu que as classes sao tipos definidos pelo usuario, mas como usuario, certamente voce nao projetou as classes `ostream` e `istream`. Assim como as funcoes podem vir em bibliotecas de funcoes, as classes podem vir em bibliotecas de classes. Esse e o caso das classes `ostream` e `istream`. Tecnicamente, elas nao estao embutidas na linguagem C++; em vez disso, sao exemplos de classes que o padrao da linguagem especifica. As definicoes de classe estao dispostas no arquivo `iostream` e nao estao embutidas no compilador. Voce pode ate modificar essas definicoes de classe se quiser, embora isso nao seja uma boa ideia. (Mais precisamente, e uma ideia verdadeiramente terrivel.) A familia de classes `iostream` e a familia relacionada `fstream` (ou I/O de arquivo) sao os unicos conjuntos de definicoes de classe que vieram com todas as implementacoes iniciais do C++. No entanto, o comite ANSI/ISO C++ adicionou mais algumas bibliotecas de classe ao Padrao. Alem disso, a maioria das implementacoes fornece definicoes de classe adicionais como parte do pacote.

A descricao da classe especifica todas as operacoes que podem ser executadas em objetos dessa classe. Para executar essa acao permitida em um objeto especifico, voce envia uma mensagem ao objeto. Por exemplo, se voce quiser que o objeto `cout` exiba uma string, voce envia a ele uma mensagem que diz, em essencia, "Objeto! Exiba isso!" O C++ fornece algumas maneiras de enviar mensagens. Uma maneira, usando um metodo de classe, e essencialmente uma chamada de funcao. A outra maneira, que e a usada com `cin` e `cout`, e redefinir um operador. Assim, a instrucao a seguir usa o operador `<<` redefinido para enviar a mensagem "exibir mensagem" a `cout`:

```cpp
cout << "I am not a crook."
```

Neste caso, a mensagem vem com um argumento, que e a string a ser exibida.

## Funcoes

Como as funcoes sao os modulos a partir dos quais os programas C++ sao construidos e sao essenciais para as definicoes de POO do C++, voce deve se familiarizar completamente com elas. Alguns aspectos das funcoes sao topicos avancados, portanto a discussao principal de funcoes vem mais adiante, no Capitulo 7, "Funcoes: Modulos de Programacao do C++", e no Capitulo 8, "Aventuras com Funcoes". No entanto, se lidarmos agora com algumas caracteristicas basicas das funcoes, voce ficara mais a vontade e mais praticado com funcoes mais tarde. O restante deste capitulo o apresenta a esses fundamentos de funcoes.

As funcoes C++ vem em duas variedades: aquelas com valores de retorno e aquelas sem. Voce pode encontrar exemplos de cada tipo na biblioteca padrao de funcoes do C++, e pode criar suas proprias funcoes de cada tipo. Vamos ver uma funcao de biblioteca que tem um valor de retorno e depois examinar como voce pode escrever suas proprias funcoes simples.

### Usando uma Funcao com Valor de Retorno

Uma funcao que tem um valor de retorno produz um valor que voce pode atribuir a uma variavel ou usar em alguma outra expressao. Por exemplo, a biblioteca padrao C/C++ inclui uma funcao chamada `sqrt()` que retorna a raiz quadrada de um numero. Suponha que voce queira calcular a raiz quadrada de `6.25` e atribui-la a variavel `x`. Voce pode usar a seguinte instrucao no seu programa:

```cpp
x = sqrt(6.25); // retorna o valor 2.5 e atribui a x
```

A expressao `sqrt(6.25)` invoca, ou chama, a funcao `sqrt()`. A expressao `sqrt(6.25)` e chamada de *chamada de funcao* (function call), a funcao invocada e chamada de *funcao chamada* (called function), e a funcao que contem a chamada de funcao e chamada de *funcao chamadora* (calling function).

O valor entre parenteses (`6.25`, neste exemplo) e informacao que e enviada para a funcao; diz-se que ela e *passada* para a funcao. Um valor que e enviado para uma funcao dessa forma e chamado de *argumento* (argument) ou *parametro* (parameter). A funcao `sqrt()` calcula a resposta como `2.5` e envia esse valor de volta para a funcao chamadora; o valor enviado de volta e chamado de *valor de retorno* (return value) da funcao. Pense no valor de retorno como o que e substituido pela chamada de funcao na instrucao apos a funcao terminar seu trabalho. Assim, este exemplo atribui o valor de retorno a variavel `x`. Em resumo, um argumento e informacao enviada para a funcao, e o valor de retorno e um valor enviado de volta da funcao.

Isso e praticamente tudo que ha para isso, exceto que antes de o compilador C++ usar uma funcao, ele deve saber que tipo de argumentos a funcao usa e que tipo de valor de retorno ela tem. A maneira do C++ de transmitir essa informacao e usar uma instrucao de *prototipo de funcao* (function prototype).

> **Nota:** Um programa C++ deve fornecer um prototipo para cada funcao usada no programa.

Um prototipo de funcao faz para funcoes o que uma declaracao de variavel faz para variaveis: ele diz quais tipos estao envolvidos. Por exemplo, a biblioteca C++ define a funcao `sqrt()` para receber um numero com (potencialmente) uma parte fracionaria (como `6.25`) como argumento e retornar um numero do mesmo tipo. Algumas linguagens chamam esses numeros de numeros reais, mas o nome que o C++ usa para esse tipo e `double`. O prototipo de funcao para `sqrt()` se parece com isto:

```cpp
double sqrt(double); // prototipo de funcao
```

O `double` inicial significa que `sqrt()` retorna um valor do tipo `double`. O `double` entre parenteses significa que `sqrt()` requer um argumento `double`. Portanto, este prototipo descreve `sqrt()` exatamente como usada no seguinte codigo:

```cpp
double x; // declara x como variavel do tipo double
x = sqrt(6.25);
```

O ponto e virgula terminador no prototipo o identifica como uma instrucao e, portanto, o torna um prototipo em vez de um cabecalho de funcao. Se voce omitir o ponto e virgula, o compilador interpreta a linha como um cabecalho de funcao e espera que voce a siga com um corpo de funcao que defina a funcao.

Quando voce usa `sqrt()` em um programa, voce tambem deve fornecer o prototipo. Voce pode fazer isso de duas maneiras:

- Voce pode digitar o prototipo de funcao no seu arquivo de codigo-fonte voce mesmo.
- Voce pode incluir o arquivo de cabecalho `cmath` (ou `math.h` em sistemas mais antigos), que tem o prototipo nele.

A segunda maneira e melhor porque o arquivo de cabecalho e ainda mais propenso do que voce a acertar o prototipo. Cada funcao na biblioteca C++ tem um prototipo em um ou mais arquivos de cabecalho. Basta verificar a descricao da funcao no seu manual ou com a ajuda online, e a descricao diz qual arquivo de cabecalho usar.

Nao confunda o prototipo de funcao com a definicao de funcao. O prototipo, como voce viu, apenas descreve a interface da funcao. A definicao, no entanto, inclui o codigo para o funcionamento da funcao. O C e C++ dividem essas duas funcionalidades — prototipo e definicao — para funcoes de biblioteca. Os arquivos de biblioteca contem o codigo compilado para as funcoes, enquanto os arquivos de cabecalho contem os prototipos.

Voce deve colocar um prototipo de funcao antes de onde voce usa a funcao pela primeira vez. A pratica usual e colocar prototipos logo antes da definicao da funcao `main()`. O Listagem 2.4 demonstra o uso da funcao de biblioteca `sqrt()`; ele fornece um prototipo incluindo o arquivo `cmath`.

**Listagem 2.4 — raizquad.cpp**

```cpp
// raizquad.cpp -- usando a funcao sqrt()
#include <iostream>
#include <cmath>   // ou math.h
int main()
{
    using namespace std;
    double area;
    cout << "Informe a area do piso, em pes quadrados, da sua casa: ";
    cin >> area;
    double lado;
    lado = sqrt(area);
    cout << "Equivale a um quadrado de " << lado
         << " pes de lado." << endl;
    cout << "Que fascinante!" << endl;
    return 0;
}
```

> **Usando Funcoes de Biblioteca:**
> As funcoes de biblioteca C++ sao armazenadas em arquivos de biblioteca. Quando o compilador compila um programa, ele deve pesquisar os arquivos de biblioteca para as funcoes que voce usou. Os compiladores diferem em quais arquivos de biblioteca eles pesquisam automaticamente. Se voce tentar rodar o Listagem 2.4 e receber uma mensagem de que `_sqrt` e um externo indefinido, e provavel que seu compilador nao pesquise automaticamente a biblioteca matematica. Se isso ocorrer em uma implementacao Unix, pode ser necessario usar a opcao `-lm` (para library math) no final da linha de comando: `CC raizquad.C -lm`. Algumas versoes do compilador GNU no Linux se comportam de forma semelhante: `g++ raizquad.C -lm`. Apenas incluir o arquivo de cabecalho `cmath` fornece o prototipo, mas nao necessariamente faz o compilador pesquisar o arquivo de biblioteca correto.

Aqui esta um exemplo de execucao do programa no Listagem 2.4:

```
Informe a area do piso, em pes quadrados, da sua casa: 1536
Equivale a um quadrado de 39.1918 pes de lado.
Que fascinante!
```

Como `sqrt()` trabalha com valores do tipo `double`, o exemplo torna as variaveis desse tipo. Observe que voce declara uma variavel do tipo `double` usando a mesma forma, ou sintaxe, de quando declara uma variavel do tipo `int`:

```
nome-do-tipo nome-da-variavel;
```

O tipo `double` permite que as variaveis `area` e `lado` contenham valores com fracoes decimais, como `1536.0` e `39.1918`. Um inteiro aparente, como `1536`, e armazenado como um valor real com uma parte fracionaria decimal de `.0` quando armazenado em uma variavel do tipo `double`.

O C++ permite que voce declare novas variaveis em qualquer parte de um programa, portanto `raizquad.cpp` nao declarou `lado` ate logo antes de usa-la. O C++ tambem permite que voce atribua um valor a uma variavel quando a cria, portanto voce poderia ter feito isso:

```cpp
double lado = sqrt(area);
```

Voce aprendera mais sobre esse processo, chamado de *inicializacao* (initialization), no Capitulo 3. Observe que `cin` sabe como converter informacoes do fluxo de entrada para o tipo `double`, e `cout` sabe como inserir o tipo `double` no fluxo de saida.

### Variacoes de Funcoes

Algumas funcoes requerem mais de um item de informacao. Essas funcoes usam multiplos argumentos separados por virgulas. Por exemplo, a funcao matematica `pow()` recebe dois argumentos e retorna um valor igual ao primeiro argumento elevado a potencia dada pelo segundo argumento. Ela tem este prototipo:

```cpp
double pow(double, double); // prototipo de funcao com dois argumentos
```

Se, digamos, voce quiser encontrar 5^8 (5 elevado a oitava potencia), usaria a funcao assim:

```cpp
resultado = pow(5.0, 8.0); // chamada de funcao com lista de argumentos
```

Outras funcoes nao recebem argumentos. Por exemplo, uma das bibliotecas C (a associada ao arquivo de cabecalho `cstdlib` ou `stdlib.h`) tem uma funcao `rand()` que nao tem argumentos e retorna um inteiro aleatorio. Seu prototipo se parece com isto:

```cpp
int rand(void); // prototipo de funcao que nao recebe argumentos
```

A palavra-chave `void` indica explicitamente que a funcao nao recebe argumentos. Voce pode usar a funcao assim:

```cpp
meuPalpite = rand(); // chamada de funcao sem argumentos
```

Observe que, ao contrario de algumas linguagens de computador, em C++ voce deve usar os parenteses na chamada de funcao mesmo que nao haja argumentos.

Tambem ha funcoes que nao tem valor de retorno. Por exemplo, suponha que voce escreveu uma funcao que exibe um numero em formato de reais e centavos. Voce poderia enviar a ela um argumento de, digamos, `23.5`, e ela exibiria `R$23,50` na tela. Como essa funcao envia um valor para a tela em vez de para o programa chamador, ela nao requer um valor de retorno. Voce indica isso no prototipo usando a palavra-chave `void` para o tipo de retorno:

```cpp
void exibirReais(double); // prototipo para funcao sem valor de retorno
```

Como `exibirReais()` nao retorna um valor, voce nao pode usar essa funcao como parte de uma instrucao de atribuicao ou de alguma outra expressao. Em vez disso, voce tem uma instrucao de chamada de funcao pura:

```cpp
exibirReais(1234.56); // chamada de funcao, sem valor de retorno
```

Algumas linguagens reservam o termo *funcao* para funcoes com valores de retorno e usam os termos *procedimento* ou *subrotina* para aquelas sem valores de retorno, mas o C++, como o C, usa o termo funcao para ambas as variacoes.

### Funcoes Definidas pelo Usuario

A biblioteca padrao C fornece mais de 140 funcoes predefinidas. Se uma delas atender as suas necessidades, use-a com certeza. Mas muitas vezes voce tem que escrever as suas proprias, especialmente quando projeta classes. De qualquer forma, e divertido projetar suas proprias funcoes, entao vamos examinar esse processo. Voce ja usou varias funcoes definidas pelo usuario, e todas foram chamadas `main()`. Todo programa C++ deve ter uma funcao `main()`, que o usuario deve definir.

Suponha que voce queira adicionar uma segunda funcao definida pelo usuario. Assim como com uma funcao de biblioteca, voce pode chamar uma funcao definida pelo usuario usando seu nome. E, como com uma funcao de biblioteca, voce deve fornecer um prototipo de funcao antes de usar a funcao, o que voce normalmente faz colocando o prototipo acima da definicao de `main()`. Mas agora voce, e nao o fornecedor da biblioteca, deve fornecer o codigo-fonte para a nova funcao. A maneira mais simples e colocar o codigo no mesmo arquivo apos o codigo de `main()`. O Listagem 2.5 ilustra esses elementos.

**Listagem 2.5 — nossafunc.cpp**

```cpp
// nossafunc.cpp -- definindo sua propria funcao
#include <iostream>
void simao(int); // prototipo de funcao para simao()

int main()
{
    using namespace std;
    simao(3);    // chama a funcao simao()
    cout << "Escolha um inteiro: ";
    int contagem;
    cin >> contagem;
    simao(contagem); // chama novamente
    cout << "Pronto!" << endl;
    return 0;
}

void simao(int n) // define a funcao simao()
{
    using namespace std;
    cout << "Simao diz: toque seus pes " << n << " vezes." << endl;
}           // funcoes void nao precisam de instrucao return
```

A funcao `main()` chama a funcao `simao()` duas vezes, uma com um argumento de `3` e outra com uma variavel de argumento `contagem`. No intervalo, o usuario insere um inteiro que e usado para definir o valor de `contagem`. O exemplo nao usa um caractere de nova linha na mensagem de solicitacao `cout`. Isso resulta na entrada do usuario aparecendo na mesma linha que o prompt. Aqui esta um exemplo de execucao do programa no Listagem 2.5:

```
Simao diz: toque seus pes 3 vezes.
Escolha um inteiro: 512
Simao diz: toque seus pes 512 vezes.
Pronto!
```

### Forma de Funcao

A definicao da funcao `simao()` no Listagem 2.5 segue a mesma forma geral que a definicao de `main()`. Primeiro, ha um cabecalho de funcao. Em seguida, entre chaves, vem o corpo da funcao. Voce pode generalizar a forma de uma definicao de funcao da seguinte forma:

```cpp
tipo nomeDaFuncao(listadeargumentos)
{
    instrucoes
}
```

Observe que o codigo-fonte que define `simao()` segue a chave de fechamento de `main()`. Como o C, e ao contrario do Pascal, o C++ nao permite que voce aninhe uma definicao de funcao dentro de outra. Cada definicao de funcao fica separada de todas as outras; todas as funcoes sao criadas iguais.

### Cabecalhos de Funcao

A funcao `simao()` no Listagem 2.5 tem este cabecalho:

```cpp
void simao(int n)
```

O `void` inicial significa que `simao()` nao tem valor de retorno. Portanto, chamar `simao()` nao produz um numero que voce pode atribuir a uma variavel em `main()`. Assim, a primeira chamada de funcao se parece com isto:

```cpp
simao(3);    // ok para funcoes void
```

Como a pobre `simao()` nao tem valor de retorno, voce nao pode usa-la desta forma:

```cpp
resultado = simao(3); // nao permitido para funcoes void
```

O `int n` dentro dos parenteses significa que voce deve usar `simao()` com um unico argumento do tipo `int`. O `n` e uma nova variavel a qual e atribuido o valor passado durante uma chamada de funcao. Assim, a seguinte chamada de funcao atribui o valor `3` a variavel `n` definida no cabecalho de `simao()`:

```cpp
simao(3);
```

Quando a instrucao `cout` no corpo da funcao usa `n`, ela usa o valor passado na chamada de funcao. E por isso que `simao(3)` exibe um `3` em sua saida. A chamada a `simao(contagem)` na execucao de exemplo faz a funcao exibir `512` porque esse foi o valor inserido para `contagem`. Em resumo, o cabecalho de `simao()` diz que esta funcao recebe um unico argumento do tipo `int` e que ela nao tem valor de retorno.

Vamos rever o cabecalho de funcao de `main()`:

```cpp
int main()
```

O `int` inicial significa que `main()` retorna um valor inteiro. Os parenteses vazios (que opcionalmente poderiam conter `void`) significa que `main()` nao tem argumentos. Funcoes que tem valores de retorno devem usar a palavra-chave `return` para fornecer o valor de retorno e terminar a funcao. E por isso que voce tem usado a instrucao a seguir no final de `main()`:

```cpp
return 0;
```

Isso e logicamente consistente: `main()` deveria retornar um valor do tipo `int`, e voce faz ela retornar o inteiro `0`. Mas voce pode se perguntar: para o que voce esta retornando um valor? Afinal, em nenhum lugar em qualquer um dos seus programas voce viu algo chamando `main()`.

A resposta e que voce pode pensar no sistema operacional do seu computador (Unix, digamos, ou Windows) como chamando seu programa. Portanto, o valor de retorno de `main()` e retornado nao para outra parte do programa, mas para o sistema operacional. Muitos sistemas operacionais podem usar o valor de retorno do programa. Por exemplo, scripts shell do Unix e arquivos em lote da interface de linha de comando do Windows podem ser projetados para rodar programas e testar seus valores de retorno, geralmente chamados de *valores de saida* (exit values). A convencao normal e que um valor de saida de zero significa que o programa foi executado com sucesso, enquanto um valor diferente de zero significa que houve um problema.

> **Palavras-chave:**
> Palavras-chave (keywords) sao o vocabulario de uma linguagem de computador. Este capitulo usou quatro palavras-chave do C++: `int`, `void`, `return` e `double`. Como essas palavras-chave sao especiais para o C++, voce nao pode usa-las para outros propositos. Ou seja, voce nao pode usar `return` como nome de uma variavel ou `double` como nome de uma funcao. Mas voce pode usa-las como parte de um nome. O Apendice B, "Palavras Reservadas do C++", fornece uma lista completa das palavras-chave do C++. Incidentalmente, `main` nao e uma palavra-chave porque nao faz parte da linguagem. Em vez disso, e o nome de uma funcao obrigatoria. Voce pode usar `main` como nome de variavel. (Isso pode causar um problema em circunstancias esotericasdemais para descrever aqui, e como e confuso em qualquer caso, e melhor nao faze-lo.)

### Usando uma Funcao Definida pelo Usuario com Valor de Retorno

Vamos um passo adiante e escrever uma funcao que usa a instrucao `return`. A funcao `main()` ja ilustra o plano para uma funcao com um valor de retorno: fornecer o tipo de retorno no cabecalho da funcao e usar `return` no final do corpo da funcao. Voce pode usar essa forma para resolver um problema de peso para quem visita o Reino Unido. No Reino Unido, muitas balancas de banheiro sao calibradas em *stone* em vez de em libras americanas ou quilogramas internacionais. Uma *stone* equivale a 14 libras, e o programa no Listagem 2.6 usa uma funcao para fazer essa conversao.

**Listagem 2.6 — converter.cpp**

```cpp
// converter.cpp -- converte stone para libras
#include <iostream>
int pedraParaLibras(int); // prototipo de funcao

int main()
{
    using namespace std;
    int pedra;
    cout << "Informe o peso em stone: ";
    cin >> pedra;
    int libras = pedraParaLibras(pedra);
    cout << pedra << " stone = ";
    cout << libras << " libras." << endl;
    return 0;
}

int pedraParaLibras(int ped)
{
    return 14 * ped;
}
```

Aqui esta um exemplo de execucao do programa no Listagem 2.6:

```
Informe o peso em stone: 15
15 stone = 210 libras.
```

Em `main()`, o programa usa `cin` para fornecer um valor para a variavel inteira `pedra`. Esse valor e passado para a funcao `pedraParaLibras()` como argumento e e atribuido a variavel `ped` nessa funcao. `pedraParaLibras()` entao usa a palavra-chave `return` para retornar o valor de `14 * ped` para `main()`. Isso ilustra que voce nao esta limitado a seguir `return` com um numero simples. Aqui, usando uma expressao mais complexa, voce evita o trabalho de ter que criar uma nova variavel para atribuir o valor antes de retorna-lo. O programa calcula o valor dessa expressao (`210`, neste exemplo) e retorna o valor resultante. Se retornar o valor de uma expressao o incomodar, voce pode tomar a rota mais longa:

```cpp
int pedraParaLibras(int ped)
{
    int libras = 14 * ped;
    return libras;
}
```

Ambas as versoes produzem o mesmo resultado. A segunda versao, porque separa o processo de calculo do processo de retorno, e mais facil de ler e modificar.

Em geral, voce pode usar uma funcao com um valor de retorno em qualquer lugar onde usaria uma constante simples do mesmo tipo. Por exemplo, `pedraParaLibras()` retorna um valor do tipo `int`. Isso significa que voce pode usar a funcao das seguintes formas:

```cpp
int tia = pedraParaLibras(20);
int tias = tia + pedraParaLibras(10);
cout << "Ferdie pesa " << pedraParaLibras(16) << " libras." << endl;
```

Em cada caso, o programa calcula o valor de retorno e entao usa esse numero nessas instrucoes.

Como esses exemplos mostram, o prototipo de funcao descreve a interface da funcao — ou seja, como a funcao interage com o restante do programa. A lista de argumentos mostra que tipo de informacao entra na funcao, e o tipo da funcao mostra o tipo de valor retornado.

A funcao `pedraParaLibras()` e curta e simples, mas ainda assim incorpora uma gama completa de recursos funcionais:

- Tem um cabecalho e um corpo.
- Aceita um argumento.
- Retorna um valor.
- Requer um prototipo.

Considere `pedraParaLibras()` como uma forma padrao para design de funcoes. Voce explorara mais funcoes nos Capitulos 7 e 8. Enquanto isso, o material deste capitulo deve lhe dar uma boa sensacao de como as funcoes funcionam e como elas se encaixam no C++.

## Posicionando a Diretiva using em Programas com Multiplas Funcoes

Observe que o Listagem 2.5 coloca uma diretiva `using` em cada uma das duas funcoes:

```cpp
using namespace std;
```

Isso ocorre porque cada funcao usa `cout` e, portanto, precisa de acesso a definicao de `cout` do namespace `std`.

Ha outra maneira de tornar o namespace `std` disponivel para ambas as funcoes no Listagem 2.5, e isso e colocar a diretiva fora e acima de ambas as funcoes:

```cpp
// nossafunc1.cpp -- reposicionando a diretiva using
#include <iostream>
using namespace std; // afeta todas as definicoes de funcao neste arquivo
void simao(int);

int main()
{
    simao(3);
    cout << "Escolha um inteiro: ";
    int contagem;
    cin >> contagem;
    simao(contagem);
    cout << "Pronto!" << endl;
    return 0;
}

void simao(int n)
{
    cout << "Simao diz: toque seus pes " << n << " vezes." << endl;
}
```

A filosofia predominante atual e que e preferivel ser mais criterioso e limitar o acesso ao namespace `std` apenas para as funcoes que precisam de acesso. Por exemplo, no Listagem 2.6, apenas `main()` usa `cout`, portanto nao ha necessidade de tornar o namespace `std` disponivel para a funcao `pedraParaLibras()`. Assim, a diretiva `using` e colocada dentro da funcao `main()` apenas, limitando o acesso ao namespace `std` apenas para essa funcao.

Em resumo, voce tem varias opcoes para tornar os elementos do namespace `std` disponiveis para um programa. Aqui estao algumas:

- Voce pode colocar o seguinte acima das definicoes de funcao em um arquivo, tornando todo o conteudo do namespace `std` disponivel para cada funcao no arquivo: `using namespace std;`
- Voce pode colocar o seguinte em uma definicao de funcao especifica, tornando todo o conteudo do namespace `std` disponivel para aquela funcao especifica: `using namespace std;`
- Em vez de usar `using namespace std;`, voce pode colocar declaracoes `using` como a seguinte em uma definicao de funcao especifica e tornar um elemento especifico, como `cout`, disponivel para aquela funcao: `using std::cout;`
- Voce pode omitir as diretivas e declaracoes `using` completamente e usar o prefixo `std::` sempre que usar elementos do namespace `std`: `std::cout << "Usando cout e endl do namespace std" << std::endl;`

> **Convencoes de Nomenclatura:**
> Programadores C++ sao abencados (ou amaldicoados) com inumeras opcoes ao nomear funcoes, classes e variaveis. Programadores tem opinioes fortes e variadas sobre estilo, e essas frequentemente surgem como guerras santas em foruns publicos. Partindo da mesma ideia basica para um nome de funcao, um programador pode selecionar qualquer uma das seguintes: `MinhaFuncao()`, `minhafuncao()`, `minhaFuncao()`, `minha_funcao()`, `minha_func()`. A escolha dependera da equipe de desenvolvimento, das idiossincrasias das tecnologias ou bibliotecas usadas e dos gostos e preferencias do programador individual. Uma convencao de nomenclatura pessoal precisa — uma que auxilie voce atraves da consistencia e precisao — vale a pena perseguir. Uma convencao de nomenclatura pessoal precisa e reconhecivel e uma marca de uma boa engenharia de software.

## Resumo

Um programa C++ consiste em um ou mais modulos chamados funcoes. Os programas comecam a executar no inicio da funcao chamada `main()` (toda em minusculas), portanto voce deve sempre ter uma funcao com esse nome. Uma funcao, por sua vez, consiste em um cabecalho e um corpo. O cabecalho da funcao diz que tipo de valor de retorno, se houver, a funcao produz e que tipo de informacao ela espera que os argumentos lhe passem. O corpo da funcao consiste em uma serie de instrucoes C++ entre chaves (`{}`).

Os tipos de instrucoes C++ incluem os seguintes:

- **Instrucao de declaracao** — Uma instrucao de declaracao anuncia o nome e o tipo de uma variavel usada em uma funcao.
- **Instrucao de atribuicao** — Uma instrucao de atribuicao usa o operador de atribuicao (`=`) para atribuir um valor a uma variavel.
- **Instrucao de mensagem** — Uma instrucao de mensagem envia uma mensagem a um objeto, iniciando algum tipo de acao.
- **Chamada de funcao** — Uma chamada de funcao ativa uma funcao. Quando a funcao chamada termina, o programa retorna para a instrucao na funcao chamadora imediatamente apos a chamada de funcao.
- **Prototipo de funcao** — Um prototipo de funcao declara o tipo de retorno de uma funcao, juntamente com o numero e o tipo de argumentos que a funcao espera.
- **Instrucao return** — Uma instrucao return envia um valor de uma funcao chamada de volta para a funcao chamadora.

Uma classe e uma especificacao definida pelo usuario para um tipo de dado. Essa especificacao detalha como as informacoes devem ser representadas e tambem as operacoes que podem ser executadas com os dados. Um objeto e uma entidade criada de acordo com uma prescricao de classe, assim como uma variavel simples e uma entidade criada de acordo com uma descricao de tipo de dado.

O C++ fornece dois objetos predefinidos (`cin` e `cout`) para lidar com entrada e saida. Eles sao exemplos das classes `istream` e `ostream`, que sao definidas no arquivo `iostream`. Essas classes veem entrada e saida como fluxos de caracteres. O operador de insercao (`<<`), que e definido para a classe `ostream`, permite inserir dados no fluxo de saida, e o operador de extracao (`>>`), que e definido para a classe `istream`, permite extrair informacoes do fluxo de entrada. Tanto `cin` quanto `cout` sao objetos inteligentes, capazes de converter automaticamente informacoes de uma forma para outra de acordo com o contexto do programa.

O C++ pode usar o extenso conjunto de funcoes de biblioteca C. Para usar uma funcao de biblioteca, voce deve incluir o arquivo de cabecalho que fornece o prototipo para a funcao.

Agora que voce tem uma visao geral de programas simples em C++, voce pode ir nos proximos capitulos para preencher os detalhes e expandir horizontes.

## Revisao do Capitulo

Voce pode encontrar as respostas para as perguntas de revisao do capitulo no final de cada capitulo no Apendice J, "Respostas para a Revisao do Capitulo".

1. Quais sao os modulos dos programas C++ chamados?
2. O que a seguinte diretiva do preprocessador faz? `#include <iostream>`
3. O que a seguinte instrucao faz? `using namespace std;`
4. Que instrucao voce usaria para imprimir a frase "Hello, world" e depois iniciar uma nova linha?
5. Que instrucao voce usaria para criar uma variavel inteira com o nome `queijos`?
6. Que instrucao voce usaria para atribuir o valor `32` a variavel `queijos`?
7. Que instrucao voce usaria para ler um valor da entrada do teclado na variavel `queijos`?
8. Que instrucao voce usaria para imprimir "Temos X variedades de queijo", onde o valor atual da variavel `queijos` substitui X?
9. O que os seguintes prototipos de funcao dizem sobre as funcoes?
   ```cpp
   int froop(double t);
   void chocalhar(int n);
   int podar(void);
   ```
10. Quando voce nao precisa usar a palavra-chave `return` ao definir uma funcao?
11. Suponha que sua funcao `main()` tenha a seguinte linha:
    ```cpp
    cout << "Por favor, informe seu PIN: ";
    ```
    E suponha que o compilador reclame que `cout` e um identificador desconhecido. Qual e a causa provavel dessa reclamacao, e quais sao tres formas de corrigir o problema?

## Exercicios de Programacao

1. Escreva um programa C++ que exiba seu nome e endereco (ou, se voce valoriza sua privacidade, um nome e endereco ficticio).

2. Escreva um programa C++ que peca uma distancia em jardas e a converta para metros. (Um jarda equivale a 0,9144 metro.)

3. Escreva um programa C++ que use tres funcoes definidas pelo usuario (contando `main()` como uma) e produza a seguinte saida:
   ```
   Tres ratos cegos
   Tres ratos cegos
   Veja como correm
   Veja como correm
   ```
   Uma funcao, chamada duas vezes, deve produzir as duas primeiras linhas, e a funcao restante, tambem chamada duas vezes, deve produzir a saida restante.

4. Escreva um programa que peca ao usuario para inserir sua idade. O programa entao deve exibir a idade em meses:
   ```
   Informe sua idade: 29
   Sua idade em meses e 348.
   ```

5. Escreva um programa que tenha `main()` chamando uma funcao definida pelo usuario que recebe um valor de temperatura em Celsius como argumento e entao retorna o valor equivalente em Fahrenheit. O programa deve solicitar o valor Celsius como entrada do usuario e exibir o resultado, conforme mostrado no seguinte codigo:
   ```
   Por favor, informe um valor em Celsius: 20
   20 graus Celsius equivalem a 68 graus Fahrenheit.
   ```
   Para referencia, aqui esta a formula para fazer a conversao: Fahrenheit = 1.8 × graus Celsius + 32.0

6. Escreva um programa que tenha `main()` chamando uma funcao definida pelo usuario que recebe uma distancia em anos-luz como argumento e entao retorna a distancia em unidades astronomicas. O programa deve solicitar o valor em anos-luz como entrada do usuario e exibir o resultado, conforme mostrado no seguinte codigo:
   ```
   Informe o numero de anos-luz: 4.2
   4.2 anos-luz = 265608 unidades astronomicas.
   ```
   Uma unidade astronomica e a distancia media da Terra ao Sol (cerca de 150.000.000 km ou 93.000.000 milhas), e um ano-luz e a distancia que a luz percorre em um ano (cerca de 10 trilhoes de quilometros ou 6 trilhoes de milhas). Use o tipo `double` (como no Listagem 2.4) e este fator de conversao: 1 ano-luz = 63.240 unidades astronomicas.

7. Escreva um programa que peca ao usuario para inserir um valor de horas e um valor de minutos. A funcao `main()` deve entao passar esses dois valores para uma funcao do tipo `void` que exiba os dois valores no formato mostrado no seguinte exemplo de execucao:
   ```
   Informe o numero de horas: 9
   Informe o numero de minutos: 28
   Hora: 9:28
   ```

---

> Navegacao: [Anterior](capitulo-01.md) | [Indice](README.md) | [Proximo](capitulo-03.md)
