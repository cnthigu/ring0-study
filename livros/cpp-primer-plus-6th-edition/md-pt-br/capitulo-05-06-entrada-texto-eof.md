# Capitulo 5 - Loops e Expressoes Relacionais (Loops and Relational Expressions)
## Parte 6: Loops e Entrada de Texto / Condicao de Fim de Arquivo

> Traducao nao oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Navegacao: [Anterior](capitulo-05-05-loop-dowhile-for-faixa.md) | [Indice](README.md) | [Proximo](capitulo-05-07-loops-aninhados-resumo.md)

---

## Loops e Entrada de Texto

Agora que voce viu como os loops funcionam, vamos examinar uma das tarefas mais comuns e importantes atribuidas a loops: ler texto caractere por caractere de um arquivo ou do teclado. Por exemplo, voce pode querer escrever um programa que conta o numero de caracteres, linhas e palavras na entrada. Tradicionalmente, o C++, como o C, usa o loop `while` para esse tipo de tarefa. Investigaremos a seguir como isso e feito. Se voce ja conhece C, nao passe pelas secoes seguintes muito rapidamente. Embora o loop `while` do C++ seja o mesmo que o do C, as facilidades de E/S (I/O) do C++ sao diferentes. Isso pode dar ao loop C++ uma aparencia um tanto diferente do loop C. Na verdade, o objeto `cin` suporta tres modos distintos de entrada de caractere unico, cada um com uma interface de usuario diferente. Vamos ver como usar essas opcoes com loops `while`.

## Usando cin sem Adornos para Entrada

Se um programa vai usar um loop para ler entrada de texto do teclado, ele precisa ter alguma forma de saber quando parar. Como ele pode saber quando parar? Uma maneira e escolher algum caractere especial, as vezes chamado de **caractere sentinela** (sentinel character), para atuar como sinal de parada. Por exemplo, a Listagem 5.16 para de ler a entrada quando o programa encontra um caractere `#`. O programa conta o numero de caracteres que le e os exibe novamente. Ou seja, ele reexibe os caracteres que foram lidos. (Pressionar uma tecla do teclado nao coloca automaticamente um caractere na tela; os programas precisam fazer esse trabalho tedioso ecoando o caractere de entrada. Tipicamente, o sistema operacional lida com essa tarefa. Neste caso, tanto o sistema operacional quanto o programa de teste ecoam a entrada.) Quando termina, o programa relata o numero total de caracteres processados. A Listagem 5.16 mostra o programa.

**Listagem 5.16 — textin1.cpp**

```cpp
// textin1.cpp -- lendo chars com um loop while
#include <iostream>
int main()
{
    using namespace std;
    char ch;
    int contagem = 0; // usa entrada basica
    cout << "Insira caracteres; insira # para sair:\n";
    cin >> ch;        // obtem um caractere
    while (ch != '#') // testa o caractere
    {
        cout << ch;   // ecoa o caractere
        ++contagem;   // conta o caractere
        cin >> ch;    // obtem o proximo caractere
    }
    cout << endl << contagem << " caracteres lidos\n";
    return 0;
}
```

Aqui esta uma execucao de exemplo do programa na Listagem 5.16:

```
Insira caracteres; insira # para sair:
see ken run#really fast
seekenrun
9 caracteres lidos
```

Aparentemente, Ken corre tao rapido que apaga o proprio espaco — ou pelo menos os caracteres de espaco na entrada.

**Notas do Programa**

Observe a estrutura do programa na Listagem 5.16. O programa le o primeiro caractere de entrada antes de chegar ao loop. Dessa forma, o primeiro caractere pode ser testado quando o programa alcanca a instrucao do loop. Isso e importante porque o primeiro caractere pode ser `#`. Como `textin1.cpp` usa um loop de condicao de entrada, o programa pula corretamente o loop inteiro nesse caso. E como a variavel `contagem` foi previamente definida como 0, `contagem` tem o valor correto.

Suponha que o primeiro caractere lido nao seja `#`. Nesse caso, o programa entra no loop, exibe o caractere, incrementa a contagem e le o proximo caractere. Esta ultima etapa e vital. Sem ela, o loop processaria repetidamente o primeiro caractere de entrada eternamente. Com a ultima etapa, o programa avanca para o proximo caractere.

Observe que o design do loop segue as diretrizes mencionadas anteriormente. A condicao que encerra o loop e se o ultimo caractere lido for `#`. Essa condicao e inicializada lendo um caractere antes do inicio do loop. A condicao e atualizada lendo um novo caractere ao final do loop.

Isso tudo parece razoavel. Entao por que o programa omite os espacos na saida? A culpa e de `cin`. Ao ler valores do tipo `char`, assim como ao ler outros tipos basicos, `cin` ignora espacos e caracteres de nova linha. Os espacos na entrada nao sao ecoados, portanto nao sao contados.

Para complicar ainda mais as coisas, a entrada para `cin` e armazenada em buffer. Isso significa que os caracteres que voce digita nao sao enviados ao programa ate que voce pressione Enter. E por isso que voce pode digitar caracteres apos o `#` ao executar o programa na Listagem 5.16. Apos voce pressionar Enter, toda a sequencia de caracteres e enviada ao programa, mas o programa para de processar a entrada apos alcanzar o caractere `#`.

## cin.get(char) ao Resgate

Geralmente, programas que leem entrada caractere por caractere precisam examinar cada caractere, incluindo espacos, tabulacoes e novas linhas. A classe `istream` (definida em `iostream`), a qual `cin` pertence, inclui funcoes membro que atendem a essa necessidade. Em particular, a funcao membro `cin.get(ch)` le o proximo caractere, mesmo que seja um espaco, da entrada e o atribui a variavel `ch`. Substituindo `cin >> ch` por esta chamada de funcao, voce pode corrigir a Listagem 5.16. A Listagem 5.17 mostra o resultado.

**Listagem 5.17 — textin2.cpp**

```cpp
// textin2.cpp -- usando cin.get(char)
#include <iostream>
int main()
{
    using namespace std;
    char ch;
    int contagem = 0;
    cout << "Insira caracteres; insira # para sair:\n";
    cin.get(ch); // usa a funcao cin.get(ch)
    while (ch != '#')
    {
        cout << ch;
        ++contagem;
        cin.get(ch); // usa de novo
    }
    cout << endl << contagem << " caracteres lidos\n";
    return 0;
}
```

Aqui esta uma execucao de exemplo do programa na Listagem 5.17:

```
Insira caracteres; insira # para sair:
Did you use a #2 pencil?
Did you use a
14 caracteres lidos
```

Agora o programa ecoa e conta todos os caracteres, incluindo os espacos. A entrada ainda e armazenada em buffer, portanto ainda e possivel digitar mais entrada do que eventualmente chega ao programa.

Se voce esta familiarizado com C, este programa pode parecer terrivelmente errado. A chamada `cin.get(ch)` coloca um valor na variavel `ch`, o que significa que altera o valor da variavel. Em C, voce deve passar o endereco de uma variavel para uma funcao se quiser alterar o valor dessa variavel. Mas a chamada a `cin.get()` na Listagem 5.17 passa `ch`, nao `&ch`. Em C, codigo como esse nao funcionaria. Em C++, pode funcionar, desde que a funcao declare o argumento como uma referencia (reference). O tipo de referencia e algo que o C++ adicionou ao C. O arquivo de cabecalho `iostream` declara o argumento de `cin.get(ch)` como um tipo de referencia, portanto essa funcao pode alterar o valor de seu argumento. Voce aprendera os detalhes no Capitulo 8. Enquanto isso, os experts em C entre voces podem relaxar; normalmente, a passagem de argumentos em C++ funciona exatamente como em C. Para `cin.get(ch)`, porem, nao.

## Qual cin.get() Voce Deve Usar?

A Listagem 4.5 no Capitulo 4 usa este codigo:

```cpp
char nome[TamArr];
...
cout << "Digite seu nome:\n";
cin.get(nome, TamArr).get();
```

A ultima linha e equivalente a duas chamadas de funcao consecutivas:

```cpp
cin.get(nome, TamArr);
cin.get();
```

Uma versao de `cin.get()` recebe dois argumentos: o nome do array, que e o endereco da string (tecnicamente, tipo `char *`), e `TamArr`, que e um inteiro do tipo `int`. Em seguida, o programa usa `cin.get()` sem argumentos. E mais recentemente, usamos `cin.get()` desta forma:

```cpp
char ch;
cin.get(ch);
```

Desta vez, `cin.get()` tem um argumento e ele e do tipo `char`.

Mais uma vez e hora de aqueles familiarizados com C ficarem animados ou confusos. Em C, se uma funcao recebe um ponteiro-para-`char` e um `int` como argumentos, voce nao pode usar com sucesso a mesma funcao com um unico argumento de um tipo diferente. Mas voce pode fazer isso em C++ porque a linguagem suporta um recurso OOP chamado **sobrecarga de funcao** (function overloading). A sobrecarga de funcao permite que voce crie diferentes funcoes que tem o mesmo nome, desde que tenham diferentes listas de argumentos. Se, por exemplo, voce usar `cin.get(nome, TamArr)` em C++, o compilador encontra a versao de `cin.get()` que usa um `char *` e um `int` como argumentos. Mas se voce usar `cin.get(ch)`, o compilador busca a versao que usa um unico argumento do tipo `char`. E se o codigo nao fornecer argumentos, o compilador usa a versao de `cin.get()` que nao recebe argumentos.

A sobrecarga de funcao permite que voce use o mesmo nome para funcoes relacionadas que realizam a mesma tarefa basica de maneiras diferentes ou para tipos diferentes. Este e outro topico que te espera no Capitulo 8. Enquanto isso, voce pode se acostumar com a sobrecarga de funcao usando os exemplos de `get()` que vem com a classe `istream`. Para distinguir entre as diferentes versoes da funcao, incluiremos a lista de argumentos ao nos referirmos a elas. Assim, `cin.get()` significa a versao que nao recebe argumentos, e `cin.get(char)` significa a versao que recebe um argumento.

---

## A Condicao de Fim de Arquivo

Como a Listagem 5.17 mostra, usar um simbolo como `#` para sinalizar o final da entrada nem sempre e satisfatorio porque tal simbolo pode fazer parte de uma entrada legitima. O mesmo e verdade para outros simbolos arbitrariamente escolhidos, como `@` e `%`. Se a entrada vier de um arquivo, voce pode empregar uma tecnica muito mais poderosa — detectar o **fim de arquivo** (end-of-file, EOF). As facilidades de entrada do C++ cooperam com o sistema operacional para detectar quando a entrada alcanca o final de um arquivo e relatar essas informacoes de volta ao programa.

A principio, ler informacoes de arquivos parece ter pouco a ver com `cin` e entrada pelo teclado, mas ha duas conexoes. Primeira, muitos sistemas operacionais, incluindo Unix, Linux e o modo de Prompt de Comando do Windows, suportam **redirecionamento** (redirection), que permite que voce substitua um arquivo pela entrada do teclado. Por exemplo, suponha que no Windows voce tenha um programa executavel chamado `gofish.exe` e um arquivo de texto chamado `fishtale`. Nesse caso, voce pode dar este comando na linha de comando no modo de prompt de comando:

```
gofish <fishtale
```

Isso faz com que o programa tome a entrada do arquivo `fishtale` em vez do teclado. O simbolo `<` e o operador de redirecionamento tanto para Unix quanto para o modo de Prompt de Comando do Windows.

Segunda, muitos sistemas operacionais permitem que voce simule a condicao de EOF a partir do teclado. Em Unix, voce faz isso pressionando Ctrl+D no inicio de uma linha. No modo de Prompt de Comando do Windows, voce pressiona Ctrl+Z e depois Enter em qualquer lugar na linha. Algumas implementacoes de C++ suportam comportamento semelhante mesmo que o sistema operacional subjacente nao suporte. O conceito de EOF para entrada pelo teclado e na verdade um legado de ambientes de linha de comando.

Se o seu ambiente de programacao pode testar o EOF, voce pode usar um programa similar ao da Listagem 5.17 com arquivos redirecionados e pode usa-lo para entrada pelo teclado na qual voce simula o EOF. Isso soa util, entao vamos ver como e feito.

Quando `cin` detecta o EOF, ele define dois bits (o `eofbit` e o `failbit`) como 1. Voce pode usar uma funcao membro chamada `eof()` para ver se o `eofbit` foi definido; a chamada `cin.eof()` retorna o valor `bool` `true` se o EOF foi detectado e `false` caso contrario. Da mesma forma, a funcao membro `fail()` retorna `true` se o `eofbit` ou o `failbit` foi definido como 1, e `false` caso contrario. Observe que os metodos `eof()` e `fail()` relatam o resultado da tentativa de leitura mais recente; ou seja, eles relatam sobre o passado em vez de olhar para a frente. Portanto, um teste `cin.eof()` ou `cin.fail()` deve sempre seguir uma tentativa de leitura. O design da Listagem 5.18 reflete esse fato. Ela usa `fail()` em vez de `eof()` porque o primeiro metodo parece funcionar com uma gama mais ampla de implementacoes.

> **Nota:** Alguns sistemas nao suportam EOF simulado a partir do teclado. Outros suportam-no imperfeitamente. Se voce tem usado `cin.get()` para congelar a tela ate que voce possa le-la, isso nao funcionara aqui porque detectar o EOF desativa leituras adicionais de entrada. No entanto, voce pode usar um loop de contagem de tempo como o da Listagem 5.14 para manter a tela visivel por um momento. Ou voce pode usar `cin.clear()`, como mencionado nos Capitulos 6 e 17, para redefinir o fluxo de entrada.

**Listagem 5.18 — textin3.cpp**

```cpp
// textin3.cpp -- lendo chars ate o fim do arquivo
#include <iostream>
int main()
{
    using namespace std;
    char ch;
    int contagem = 0;
    cin.get(ch); // tenta ler um char
    while (cin.fail() == false) // testa o EOF
    {
        cout << ch; // ecoa o caractere
        ++contagem;
        cin.get(ch); // tenta ler outro char
    }
    cout << endl << contagem << " caracteres lidos\n";
    return 0;
}
```

Aqui esta uma saida de exemplo do programa na Listagem 5.18:

```
The green bird sings in the winter.<ENTER>
The green bird sings in the winter.
Yes, but the crow flies in the dawn.<ENTER>
Yes, but the crow flies in the dawn.
<CTRL>+<Z><ENTER>
73 caracteres lidos
```

Como executei o programa em um sistema Windows 7, pressionei Ctrl+Z e depois Enter para simular a condicao de EOF. Os usuarios de Unix e Linux pressionariam Ctrl+D em vez disso. Observe que em sistemas Unix e parecidos com Unix, incluindo Linux e Cygwin, Ctrl+Z suspende a execucao do programa; o comando `fg` permite que a execucao seja retomada.

Usando redirecionamento, voce pode usar o programa na Listagem 5.18 para exibir um arquivo de texto e relatar quantos caracteres ele tem.

## EOF Encerra a Entrada

Lembre-se de que quando um metodo `cin` detecta o EOF, ele define um sinalizador no objeto `cin`, indicando a condicao de EOF. Quando esse sinalizador e definido, `cin` nao le mais nenhuma entrada, e chamadas adicionais a `cin` nao tem efeito. Para entrada de arquivo, isso faz sentido porque voce nao deveria ler alem do final de um arquivo. Para entrada pelo teclado, porem, voce pode usar um EOF simulado para encerrar um loop, mas depois querer ler mais entrada. O metodo `cin.clear()` limpa o sinalizador de EOF e permite que a entrada prossiga novamente. O Capitulo 17, "Entrada, Saida e Arquivos", discute isso mais detalhadamente. Tenha em mente, porem, que em alguns sistemas, digitar Ctrl+Z encerra efetivamente tanto a entrada quanto a saida alem dos poderes de `cin.clear()` de restaura-las.

## Idiomas Comuns para Entrada de Caracteres

O seguinte e o design essencial de um loop destinado a ler texto um caractere de cada vez ate o EOF:

```cpp
cin.get(ch); // tenta ler um char
while (cin.fail() == false) // testa o EOF
{
    ...         // faz algo
    cin.get(ch); // tenta ler outro char
}
```

Ha alguns atalhos que voce pode usar com este codigo. O Capitulo 6 apresenta o operador `!`, que alterna `true` para `false` e vice-versa. Voce pode usa-lo para reescrever o teste `while` desta forma:

```cpp
while (!cin.fail()) // enquanto a entrada nao falhou
```

O valor de retorno para o metodo `cin.get(char)` e `cin`, um objeto. Porem, a classe `istream` fornece uma funcao que pode converter um objeto `istream` como `cin` em um valor `bool`; essa funcao de conversao e chamada quando `cin` ocorre em um local onde um `bool` e esperado, como na condicao de teste de um loop `while`. Alem disso, o valor `bool` para a conversao e `true` se a ultima tentativa de leitura foi bem-sucedida e `false` caso contrario. Isso significa que voce pode reescrever o teste `while` desta forma:

```cpp
while (cin) // enquanto a entrada e bem-sucedida
```

Isso e um pouco mais geral do que usar `!cin.fail()` ou `!cin.eof()` porque detecta outras possiveis causas de falha, como falha no disco.

Por fim, como o valor de retorno de `cin.get(char)` e `cin`, voce pode condensar o loop para este formato:

```cpp
while (cin.get(ch)) // enquanto a entrada e bem-sucedida
{
    ...     // faz algo
}
```

Aqui, `cin.get(char)` e chamado uma vez na condicao de teste em vez de duas — uma vez antes do loop e uma vez ao final do loop. Para avaliar o teste do loop, o programa primeiro tem que executar a chamada a `cin.get(ch)`, que, se bem-sucedida, coloca um valor em `ch`. Em seguida, o programa obtem o valor de retorno da chamada de funcao, que e `cin`. Entao ele aplica a conversao `bool` a `cin`, que resulta em `true` se a entrada funcionou e `false` caso contrario. As tres diretrizes (identificar a condicao de terminacao, inicializar a condicao e atualizar a condicao) sao todas comprimidas em uma condicao de teste de loop.

## Mais Uma Versao de cin.get()

Os usuarios nostalgicos de C podem ansiar pelas funcoes de E/S de caractere do C, `getchar()` e `putchar()`. Elas estao disponiveis em C++ se voce as quiser. Voce apenas usa o arquivo de cabecalho `stdio.h` como usaria em C (ou usa o mais atual `cstdio`). Ou voce pode usar funcoes membro das classes `istream` e `ostream` que funcionam de maneira muito semelhante. Vamos examinar essa abordagem a seguir.

A funcao membro `cin.get()` sem argumentos retorna o proximo caractere da entrada. Ou seja, voce a usa desta forma:

```cpp
ch = cin.get();
```

(Lembre-se que `cin.get(ch)` retorna um objeto, nao o caractere lido.) Essa funcao funciona de maneira muito semelhante a `getchar()` do C, retornando o codigo do caractere como um valor do tipo `int`. Da mesma forma, voce pode usar a funcao `cout.put()` (veja o Capitulo 3, "Lidando com Dados") para exibir o caractere:

```cpp
cout.put(ch);
```

Ela funciona de forma semelhante a `putchar()` do C, exceto que seu argumento deve ser do tipo `char` em vez de `int`.

> **Nota:** Originalmente, o membro `put()` tinha o unico prototipo `put(char)`. Voce podia passar a ele um argumento `int`, que entao seria convertido para `char`. O Padrao tambem exige um unico prototipo. No entanto, algumas implementacoes C++ fornecem tres prototipos: `put(char)`, `put(signed char)` e `put(unsigned char)`. Usar `put()` com um argumento `int` nessas implementacoes gera uma mensagem de erro porque ha mais de uma escolha para converter o `int`. Uma conversao de tipo explicita, como `cout.put(char(ch))`, funciona para tipos `int`.

Para usar `cin.get()` com sucesso, voce precisa saber como ele trata a condicao de EOF. Quando a funcao alcanca o EOF, nao ha mais caracteres para serem retornados. Em vez disso, `cin.get()` retorna um valor especial, representado pela constante simbolica `EOF`. Essa constante e definida no arquivo de cabecalho `iostream`. O valor de `EOF` deve ser diferente de qualquer valor de caractere valido para que o programa nao confunda `EOF` com um caractere comum. Tipicamente, `EOF` e definido com o valor -1 porque nenhum caractere tem um codigo ASCII de -1, mas voce nao precisa saber o valor real. Voce pode usar apenas `EOF` em um programa. Por exemplo, o nucleo da Listagem 5.18 parece assim:

```cpp
char ch;
cin.get(ch);
while (cin.fail() == false) // testa o EOF
{
    cout << ch;
    ++contagem;
    cin.get(ch);
}
```

Voce pode usar `int ch`, substituir `cin.get(char)` por `cin.get()`, substituir `cout` por `cout.put()`, e substituir o teste `cin.fail()` por um teste de `EOF`:

```cpp
int ch; // para compatibilidade com o valor EOF
ch = cin.get();
while (ch != EOF)
{
    cout.put(char(ch)); // cout.put(char(ch)) para algumas implementacoes
    ++contagem;
    ch = cin.get();
}
```

Se `ch` e um caractere, o loop o exibe. Se `ch` e `EOF`, o loop termina.

> **Dica:** Voce deve perceber que `EOF` nao representa um caractere na entrada. Em vez disso, e um sinal de que nao ha mais caracteres.

Ha um ponto sutil mas importante sobre o uso de `cin.get()` alem das mudancas feitas ate agora. Como `EOF` representa um valor fora dos codigos de caractere validos, e possivel que ele nao seja compativel com o tipo `char`. Por exemplo, em alguns sistemas, o tipo `char` e sem sinal (unsigned), portanto uma variavel `char` nunca poderia ter o valor usual de `EOF` igual a -1. Por essa razao, se voce usar `cin.get()` (sem argumento) e testar o `EOF`, voce deve atribuir o valor de retorno ao tipo `int` em vez de ao tipo `char`. Alem disso, se voce tornar `ch` do tipo `int` em vez de `char`, pode ser necessario fazer uma conversao para `char` ao exibir `ch`.

A Listagem 5.19 incorpora a abordagem `cin.get()` em uma nova versao da Listagem 5.18. Ela tambem condensa o codigo combinando a entrada de caractere com o teste do loop `while`.

**Listagem 5.19 — textin4.cpp**

```cpp
// textin4.cpp -- lendo chars com cin.get()
#include <iostream>
int main(void)
{
    using namespace std;
    int ch;            // deve ser int, nao char
    int contagem = 0;
    while ((ch = cin.get()) != EOF) // testa o fim de arquivo
    {
        cout.put(char(ch));
        ++contagem;
    }
    cout << endl << contagem << " caracteres lidos\n";
    return 0;
}
```

> **Nota:** Alguns sistemas ou nao suportam EOF simulado a partir do teclado ou o suportam imperfeitamente, e isso pode impedir que o exemplo na Listagem 5.19 funcione como descrito. Se voce tem usado `cin.get()` para congelar a tela ate poder le-la, isso nao funcionara aqui porque detectar o EOF desativa leituras adicionais de entrada. No entanto, voce pode usar um loop de contagem de tempo como o da Listagem 5.14 para manter a tela visivel por um momento. Ou voce pode usar `cin.clear()`, conforme descrito no Capitulo 17, para redefinir o fluxo de entrada.

Aqui esta uma execucao de exemplo do programa na Listagem 5.19:

```
The sullen mackerel sulks in the shadowy shallows.<ENTER>
The sullen mackerel sulks in the shadowy shallows.
Yes, but the blue bird of happiness harbors secrets.<ENTER>
Yes, but the blue bird of happiness harbors secrets.
<CTRL>+<Z><ENTER>
104 caracteres lidos
```

Vamos analisar a condicao do loop:

```cpp
while ((ch = cin.get()) != EOF)
```

Os parenteses que delimitam a subexpressao `ch = cin.get()` fazem com que o programa avalie essa expressao primeiro. Para fazer a avaliacao, o programa primeiro tem que chamar a funcao `cin.get()`. Em seguida, ele atribui o valor de retorno da funcao a `ch`. Como o valor de uma instrucao de atribuicao e o valor do operando esquerdo, a subexpressao inteira se reduz ao valor de `ch`. Se esse valor for `EOF`, o loop termina; caso contrario, ele continua. A condicao de teste precisa de todos os parenteses. Suponha que voce deixe alguns parenteses de fora:

```cpp
while (ch = cin.get() != EOF)
```

O operador `!=` tem precedencia mais alta do que `=`, portanto primeiro o programa compara o valor de retorno de `cin.get()` com `EOF`. Uma comparacao produz um resultado `false` ou `true`; esse valor `bool` e convertido para 0 ou 1, e esse e o valor atribuido a `ch`.

Usar `cin.get(ch)` (com um argumento) para entrada, por outro lado, nao cria quaisquer problemas de tipo. Lembre-se de que a funcao `cin.get(char)` nao atribui um valor especial a `ch` no EOF. Na verdade, ela nao atribui nada a `ch` nesse caso. `ch` nunca e chamada a conter um valor nao-`char`.

A Tabela 5.3 resume as diferencas entre `cin.get(char)` e `cin.get()`.

**Tabela 5.3 — cin.get(ch) Versus cin.get()**

| Propriedade                          | `cin.get(ch)`                              | `ch = cin.get()`                              |
|--------------------------------------|--------------------------------------------|-----------------------------------------------|
| Metodo para transmitir o caractere   | Atribui ao argumento `ch`                  | Usa o valor de retorno da funcao para atribuir a `ch` |
| Valor de retorno para entrada de caractere | Objeto da classe `istream` (`true` apos conversao `bool`) | Codigo para caractere como valor `int`     |
| Valor de retorno no EOF              | Objeto da classe `istream` (`false` apos conversao `bool`) | `EOF`                                    |

Entao qual voce deve usar, `cin.get()` ou `cin.get(char)`? A forma com o argumento de caractere esta integrada mais completamente na abordagem orientada a objetos porque seu valor de retorno e um objeto `istream`. Isso significa, por exemplo, que voce pode encadear usos. Por exemplo, o seguinte codigo significa ler o proximo caractere de entrada em `ch1` e o caractere de entrada seguinte em `ch2`:

```cpp
cin.get(ch1).get(ch2);
```

Isso funciona porque a chamada de funcao `cin.get(ch1)` retorna o objeto `cin`, que entao age como o objeto ao qual `get(ch2)` e anexado.

Provavelmente o principal uso da forma `get()` e permitir que voce faca conversoes rapidas e faceis das funcoes `getchar()` e `putchar()` de `stdio.h` para os metodos `cin.get()` e `cout.put()` de `iostream`. Voce apenas substitui um arquivo de cabecalho pelo outro e substitui globalmente `getchar()` e `putchar()` por seus equivalentes metodologicos. (Se o codigo antigo usa uma variavel do tipo `int` para entrada, voce tem que fazer mais ajustes se sua implementacao tiver multiplos prototipos para `put()`.)

---

> Navegacao: [Anterior](capitulo-05-05-loop-dowhile-for-faixa.md) | [Indice](README.md) | [Proximo](capitulo-05-07-loops-aninhados-resumo.md)
