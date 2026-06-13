# Capitulo 5 - Loops e Expressoes Relacionais (Loops and Relational Expressions)
## Parte 1: Introduzindo o Loop for

> Traducao nao oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Navegacao: [Anterior](capitulo-04-08-resumo-revisao.md) | [Indice](README.md) | [Proximo](capitulo-05-02-operadores.md)

---

Neste capitulo voce aprendera sobre o seguinte:

- O loop `for`
- Expressoes e instrucoes
- Os operadores de incremento e decremento: `++` e `--`
- Operadores de atribuicao combinados
- Instrucoes compostas (blocos)
- O operador virgula
- Operadores relacionais: `>`, `>=`, `==`, `<=`, `<` e `!=`
- O loop `while`
- O recurso `typedef`
- O loop `do while`
- O metodo de entrada de caracteres `get()`
- A condicao de fim de arquivo (end-of-file)
- Loops aninhados e arrays bidimensionais

Os computadores fazem mais do que simplesmente armazenar dados. Eles analisam, consolidam, reorganizam, extraem, modificam, extrapolam, sintetizam e de outras formas manipulam dados. As vezes eles ate distorcem e corrompem dados, mas tentaremos nos afastar desse tipo de comportamento. Para realizar seus milagres de manipulacao, os programas precisam de ferramentas para executar acoes repetitivas e para tomar decisoes. Naturalmente, o C++ fornece essas ferramentas. Na verdade, ele usa os mesmos loops `for`, loops `while`, loops `do while`, instrucoes `if` e instrucoes `switch` que o C convencional emprega, de modo que se voce conhece C, pode percorrer este capitulo e o Capitulo 6, "Instrucoes de Ramificacao e Operadores Logicos", rapidamente. (Mas nao tao rapidamente — voce nao vai querer perder como o `cin` trata a entrada de caracteres!) Esses varios controles de fluxo de programa frequentemente usam expressoes relacionais e expressoes logicas para governar seu comportamento. Este capitulo discute loops e expressoes relacionais, e o Capitulo 6 prossegue com instrucoes de ramificacao e expressoes logicas.

---

## Introduzindo o Loop for

As circunstancias frequentemente exigem que um programa execute tarefas repetitivas, como somar os elementos de um array um por um ou imprimir algum elogio a produtividade 20 vezes. O loop `for` do C++ torna essas tarefas faceis de realizar. Vamos examinar um loop na Listagem 5.1, ver o que ele faz e, em seguida, discutir como ele funciona.

**Listagem 5.1 — forloop.cpp**

```cpp
// forloop.cpp -- apresentando o loop for
#include <iostream>
int main()
{
    using namespace std;
    int i; // cria um contador
    // inicializacao; teste; atualizacao
    for (i = 0; i < 5; i++)
        cout << "C++ conhece loops.\n";
    cout << "C++ sabe quando parar.\n";
    return 0;
}
```

Aqui esta a saida do programa na Listagem 5.1:

```
C++ conhece loops.
C++ conhece loops.
C++ conhece loops.
C++ conhece loops.
C++ conhece loops.
C++ sabe quando parar.
```

Este loop comeca definindo o inteiro `i` como 0:

```
i = 0
```

Essa e a parte de **inicializacao do loop** (loop initialization). Em seguida, na parte de **teste do loop** (loop test), o programa verifica se `i` e menor que 5:

```
i < 5
```

Se for, o programa executa a seguinte instrucao, que e denominada o **corpo do loop** (loop body):

```cpp
cout << "C++ conhece loops.\n";
```

Em seguida, o programa usa a parte de **atualizacao do loop** (loop update) para aumentar `i` em 1:

```
i++
```

A parte de atualizacao do loop usa o operador `++`, chamado de **operador de incremento** (increment operator). Ele incrementa o valor de seu operando em 1. (O operador de incremento nao se restringe a loops `for`. Por exemplo, voce pode usar `i++;` em vez de `i = i + 1;` como uma instrucao em um programa.) Incrementar `i` completa o primeiro ciclo do loop.

Em seguida, o loop inicia um novo ciclo comparando o novo valor de `i` com 5. Como o novo valor (1) tambem e menor que 5, o loop imprime outra linha e termina incrementando `i` novamente. Isso prepara o terreno para um novo ciclo de teste, execucao de uma instrucao e atualizacao do valor de `i`. O processo continua ate que o loop atualize `i` para 5. Entao o proximo teste falha, e o programa passa para a proxima instrucao apos o loop.

## Partes de um Loop for

Um loop `for` fornece uma receita passo a passo para executar acoes repetidas. Vamos examinar com mais detalhes como ele e configurado. As partes usuais de um loop `for` lidam com estas etapas:

1. Definir um valor inicialmente
2. Realizar um teste para verificar se o loop deve continuar
3. Executar as acoes do loop
4. Atualizar valor(es) usado(s) para o teste

O design do loop C++ posiciona esses elementos de modo que voce possa identifica-los de relance. A inicializacao, o teste e as acoes de atualizacao constituem uma secao de controle de tres partes delimitada por parenteses. Cada parte e uma expressao, e ponto e virgulas separam as expressoes entre si. A instrucao que segue a secao de controle e chamada de **corpo** (body) do loop, e ela e executada enquanto a expressao de teste permanecer verdadeira:

```
for (inicializacao; expressao-de-teste; expressao-de-atualizacao)
    corpo
```

A sintaxe do C++ conta uma instrucao `for` completa como uma unica instrucao, mesmo que ela possa incorporar uma ou mais instrucoes na parte do corpo. (Ter mais de uma instrucao requer o uso de uma instrucao composta, ou bloco, conforme discutido mais adiante neste capitulo.)

O loop realiza a inicializacao apenas uma vez. Tipicamente, os programas usam essa expressao para definir uma variavel com um valor inicial e, em seguida, usam a variavel para contar ciclos de loop.

A `expressao-de-teste` determina se o corpo do loop sera executado. Normalmente, essa expressao e uma expressao relacional — ou seja, uma que compara dois valores. Nosso exemplo compara o valor de `i` com 5, verificando se `i` e menor que 5. Se a comparacao for verdadeira, o programa executa o corpo do loop. Na verdade, o C++ nao limita a `expressao-de-teste` a comparacoes verdadeiro/falso. Voce pode usar qualquer expressao, e o C++ a convertera de tipo para `bool`. Assim, uma expressao com o valor 0 e convertida para o valor `bool` `false`, e o loop termina. Se a expressao avaliar como diferente de zero, ela e convertida para o valor `bool` `true`, e o loop continua. A Listagem 5.2 demonstra isso usando a expressao `i` como condicao de teste. (Na secao de atualizacao, `i--` e semelhante a `i++`, exceto que diminui o valor de `i` em 1 cada vez que e usado.)

**Listagem 5.2 — num_test.cpp**

```cpp
// num_test.cpp -- usa teste numerico em loop for
#include <iostream>
int main()
{
    using namespace std;
    cout << "Digite o valor inicial da contagem regressiva: ";
    int limite;
    cin >> limite;
    int i;
    for (i = limite; i; i--) // encerra quando i e 0
        cout << "i = " << i << "\n";
    cout << "Pronto, agora i = " << i << "\n";
    return 0;
}
```

Aqui esta a saida do programa na Listagem 5.2:

```
Digite o valor inicial da contagem regressiva: 4
i = 4
i = 3
i = 2
i = 1
Pronto, agora i = 0
```

Observe que o loop termina quando `i` chega a 0.

Como as expressoes relacionais, como `i < 5`, se encaixam nessa estrutura de encerrar um loop com o valor 0? Antes de o tipo `bool` ser introduzido, as expressoes relacionais avaliavam como 1 se verdadeiras e 0 se falsas. Assim, o valor da expressao `3 < 5` era 1, e o valor de `5 < 5` era 0. Agora que o C++ adicionou o tipo `bool`, as expressoes relacionais avaliam para os literais `bool` `true` e `false` em vez de 1 e 0. Essa mudanca nao leva a incompatibilidades, porem, porque um programa C++ converte `true` e `false` para 1 e 0 quando valores inteiros sao esperados, e converte 0 para `false` e diferente de zero para `true` quando valores `bool` sao esperados.

O loop `for` e um **loop de condicao de entrada** (entry-condition loop). Isso significa que a expressao de teste e avaliada *antes* de cada ciclo do loop. O loop nunca executa o corpo do loop quando a expressao de teste e falsa. Por exemplo, suponha que voce execute novamente o programa na Listagem 5.2, mas forneca 0 como valor inicial. Como a condicao de teste falha na primeira vez que e avaliada, o corpo do loop nunca e executado:

```
Digite o valor inicial da contagem regressiva: 0
Pronto, agora i = 0
```

Essa atitude de verificar antes de executar o loop pode ajudar um programa a evitar problemas.

A `expressao-de-atualizacao` e avaliada ao final do loop, depois que o corpo foi executado. Tipicamente, ela e usada para aumentar ou diminuir o valor da variavel que controla o numero de ciclos de loop. Porem, pode ser qualquer expressao C++ valida, assim como as outras expressoes de controle. Isso torna o loop `for` capaz de muito mais do que simplesmente contar de 0 a 5, como o primeiro exemplo de loop faz. Voce vera alguns exemplos disso mais adiante.

O corpo do loop `for` consiste em uma unica instrucao, mas logo voce aprendera como estender essa regra.

Uma instrucao `for` parece um pouco com uma chamada de funcao porque usa um nome seguido de parenteses emparelhados. Porem, o status de `for` como palavra-chave do C++ impede que o compilador pense que `for` e uma funcao. Isso tambem impede que voce nomeie uma funcao como `for`.

> **Dica:** O estilo comum do C++ e colocar um espaco entre `for` e o parentese seguinte e omitir o espaco entre um nome de funcao e o parentese seguinte:
> ```cpp
> for (i = 6; i < 10; i++)
>     funcao_inteligente(i);
> ```
> Outros controles de fluxo, como `if` e `while`, sao tratados de forma semelhante ao `for`. Isso serve para reforcar visualmente a distincao entre uma instrucao de controle e uma chamada de funcao. Tambem e pratica comum recuar o corpo de uma instrucao `for` para destaca-lo visualmente.

---

## Expressoes e Instrucoes

Uma secao de controle `for` usa tres expressoes. Dentro dos limites que se impoem de sintaxe, o C++ e uma linguagem muito expressiva. Qualquer valor ou qualquer combinacao valida de valores e operadores constitui uma expressao. Por exemplo, `10` e uma expressao com o valor 10 (sem surpresas), e `28 * 20` e uma expressao com o valor 560. Em C++, toda expressao tem um valor. Muitas vezes o valor e obvio. Por exemplo, a seguinte expressao e formada por dois valores e o operador de adicao, e tem o valor 49:

```
22 + 27
```

As vezes o valor e menos obvio. Por exemplo, o seguinte e uma expressao porque e formada por dois valores e o operador de atribuicao:

```
x = 20
```

O C++ define o valor de uma expressao de atribuicao como o valor do membro a esquerda, portanto a expressao tem o valor 20. O fato de que expressoes de atribuicao tem valores permite instrucoes como a seguinte:

```cpp
criadas = (cozinheiros = 4) + 3;
```

A expressao `cozinheiros = 4` tem o valor 4, portanto `criadas` recebe o valor 7. No entanto, so porque o C++ permite esse comportamento nao significa que voce deva incentiva-lo. Mas a mesma regra que torna possivel essa instrucao peculiar tambem torna possivel a seguinte instrucao util:

```cpp
x = y = z = 0;
```

Essa e uma maneira rapida de definir diversas variaveis com o mesmo valor. A tabela de precedencia (veja o Apendice D, "Precedencia de Operadores") revela que a atribuicao associa da direita para a esquerda, portanto primeiro 0 e atribuido a `z`, e entao `z = 0` e atribuido a `y`, e assim por diante.

Por fim, como mencionado anteriormente, expressoes relacionais como `x < y` avaliam para os valores `bool` `true` ou `false`. O programa curto na Listagem 5.3 ilustra alguns pontos sobre valores de expressoes. O operador `<<` tem precedencia maior do que os operadores usados nas expressoes, portanto o codigo usa parenteses para impor a ordem correta.

**Listagem 5.3 — express.cpp**

```cpp
// express.cpp -- valores de expressoes
#include <iostream>
int main()
{
    using namespace std;
    int x;
    cout << "A expressao x = 100 tem o valor ";
    cout << (x = 100) << endl;
    cout << "Agora x = " << x << endl;
    cout << "A expressao x < 3 tem o valor ";
    cout << (x < 3) << endl;
    cout << "A expressao x > 3 tem o valor ";
    cout << (x > 3) << endl;
    cout.setf(ios_base::boolalpha); // recurso mais recente do C++
    cout << "A expressao x < 3 tem o valor ";
    cout << (x < 3) << endl;
    cout << "A expressao x > 3 tem o valor ";
    cout << (x > 3) << endl;
    return 0;
}
```

Aqui esta a saida do programa na Listagem 5.3:

```
A expressao x = 100 tem o valor 100
Agora x = 100
A expressao x < 3 tem o valor 0
A expressao x > 3 tem o valor 1
A expressao x < 3 tem o valor false
A expressao x > 3 tem o valor true
```

Normalmente, `cout` converte valores `bool` para `int` antes de exibi-los, mas a chamada de funcao `cout.setf(ios::boolalpha)` define um sinalizador que instrui `cout` a exibir as palavras `true` e `false` em vez de `1` e `0`.

> **Nota:** Uma expressao C++ e um valor ou uma combinacao de valores e operadores, e toda expressao C++ tem um valor.

Para avaliar a expressao `x = 100`, o C++ deve atribuir o valor 100 a `x`. Quando o proprio ato de avaliar uma expressao muda o valor dos dados na memoria, dizemos que a avaliacao tem um **efeito colateral** (side effect). Assim, avaliar uma expressao de atribuicao tem o efeito colateral de alterar o valor do destinatario da atribuicao. Voce pode pensar na atribuicao como o efeito pretendido, mas do ponto de vista de como o C++ e construido, avaliar a expressao e o efeito principal. Nem todas as expressoes tem efeitos colaterais. Por exemplo, avaliar `x + 15` calcula um novo valor, mas nao muda o valor de `x`. Mas avaliar `++x + 15` tem um efeito colateral porque envolve o incremento de `x`.

De expressao para instrucao e um passo curto: basta adicionar um ponto e virgula. Assim, o seguinte e uma expressao:

```
idade = 100
```

Enquanto o seguinte e uma instrucao:

```cpp
idade = 100;
```

Mais especificamente, e uma **instrucao de expressao** (expression statement). Qualquer expressao pode se tornar uma instrucao se voce adicionar um ponto e virgula, mas o resultado pode nao fazer sentido de programacao. Por exemplo, se `roedores` e uma variavel, entao o seguinte e uma instrucao C++ valida:

```cpp
roedores + 6; // instrucao valida, mas inutil
```

O compilador permite, mas a instrucao nao realiza nada util. O programa simplesmente calcula a soma, nao faz nada com ela, e prossegue para a proxima instrucao. (Um compilador inteligente pode ate mesmo pular a instrucao.)

## Nao-expressoes e Instrucoes

Alguns conceitos, como conhecer a estrutura de um loop `for`, sao cruciais para entender o C++. Mas tambem ha aspectos relativamente menores da sintaxe que podem subitamente atormentar voce exatamente quando voce pensa que entende a linguagem. Vamos examinar alguns deles agora.

Embora seja verdade que adicionar um ponto e virgula a qualquer expressao a transforma em uma instrucao, o inverso nao e verdade. Ou seja, remover um ponto e virgula de uma instrucao nao necessariamente a converte em uma expressao. Dos tipos de instrucoes que usamos ate agora, instrucoes `return`, instrucoes de declaracao e instrucoes `for` nao se encaixam no molde instrucao = expressao + ponto e virgula. Por exemplo, o seguinte e uma instrucao:

```cpp
int sapo;
```

Mas o fragmento `int sapo` nao e uma expressao e nao tem um valor. Isso torna codigo como o seguinte invalido:

```cpp
ovos = int sapo * 1000; // invalido, nao e uma expressao
cin >> int sapo;        // nao e possivel combinar declaracao com cin
```

Da mesma forma, voce nao pode atribuir um loop `for` a uma variavel. No seguinte exemplo, o loop `for` nao e uma expressao, portanto nao tem valor e voce nao pode atribui-lo:

```cpp
int fx = for (i = 0; i < 4; i++)
    cout >> i; // impossivel
```

## Adaptando as Regras

O C++ adiciona um recurso aos loops C que requer alguns ajustes articulados na sintaxe do loop `for`. Esta era a sintaxe original:

```
for (expressao; expressao; expressao)
    instrucao
```

Em particular, a secao de controle de uma estrutura `for` consistia em tres expressoes, conforme definido anteriormente neste capitulo, separadas por ponto e virgulas. Os loops C++, porem, permitem que voce faca coisas como a seguinte:

```cpp
for (int i = 0; i < 5; i++)
```

Ou seja, voce pode declarar uma variavel na area de inicializacao de um loop `for`. Isso pode ser conveniente, mas nao se encaixa na sintaxe original porque uma declaracao nao e uma expressao. Esse comportamento antes proibido foi originalmente acomodado definindo um novo tipo de expressao, a expressao de instrucao-de-declaracao (declaration-statement expression), que era uma declaracao sem o ponto e virgula e que so podia aparecer em uma instrucao `for`. Esse ajuste foi abandonado, no entanto. Em vez disso, a sintaxe da instrucao `for` foi modificada para o seguinte:

```
for (instrucao-de-inicializacao-for condicao; expressao)
    instrucao
```

A primeira vista, isso parece estranho porque ha apenas um ponto e virgula em vez de dois. Mas isso esta certo porque a `instrucao-de-inicializacao-for` e identificada como uma instrucao, e uma instrucao tem seu proprio ponto e virgula. Quanto a `instrucao-de-inicializacao-for`, ela e identificada como uma instrucao de expressao ou uma declaracao. Essa regra de sintaxe substitui uma expressao seguida de um ponto e virgula por uma instrucao, que tem seu proprio ponto e virgula. O que isso significa na pratica e que os programadores C++ querem poder declarar e inicializar uma variavel na inicializacao de um loop `for`, e eles farao o que for necessario para possibilitar isso.

Ha um aspecto pratico sobre declarar uma variavel em `instrucao-de-inicializacao-for` que voce deve conhecer. Tal variavel existe apenas dentro da instrucao `for`. Ou seja, depois que o programa sai do loop, a variavel e eliminada:

```cpp
for (int i = 0; i < 5; i++)
    cout << "C++ conhece loops.\n";
cout << i << endl; // ops! i nao esta mais definido
```

Outra coisa que voce deve saber e que algumas implementacoes C++ mais antigas seguem uma regra anterior e tratam o loop anterior como se `i` tivesse sido declarado *antes* do loop, tornando-o assim disponivel apos o termino do loop.

---

## De Volta ao Loop for

Vamos ser um pouco mais ambiciosos com os loops. A Listagem 5.4 usa um loop para calcular e armazenar os primeiros 16 fatoriais. Os fatoriais, que sao uteis para calcular probabilidades, sao calculados da seguinte forma. O fatorial de zero, escrito como `0!`, e definido como 1. Entao, `1!` e `1 * 0!`, ou 1. Em seguida, `2!` e `2 * 1!`, ou 2. Entao, `3!` e `3 * 2!`, ou 6, e assim por diante, com o fatorial de cada inteiro sendo o produto desse inteiro com o fatorial anterior. O programa usa um loop para calcular os valores de fatoriais sucessivos, armazenando-os em um array. Em seguida, usa um segundo loop para exibir os resultados. Alem disso, o programa introduz o uso de declaracoes externas para valores.

**Listagem 5.4 — formore.cpp**

```cpp
// formore.cpp -- mais loops com for
#include <iostream>
const int TamArr = 16; // exemplo de declaracao externa
int main()
{
    long long fatoriais[TamArr];
    fatoriais[1] = fatoriais[0] = 1LL;
    for (int i = 2; i < TamArr; i++)
        fatoriais[i] = i * fatoriais[i-1];
    for (int i = 0; i < TamArr; i++)
        std::cout << i << "! = " << fatoriais[i] << std::endl;
    return 0;
}
```

Aqui esta a saida do programa na Listagem 5.4:

```
0! = 1
1! = 1
2! = 2
3! = 6
4! = 24
5! = 120
6! = 720
7! = 5040
8! = 40320
9! = 362880
10! = 3628800
11! = 39916800
12! = 479001600
13! = 6227020800
14! = 87178291200
15! = 1307674368000
```

Os fatoriais crescem rapido!

> **Nota:** Esta listagem usa o tipo `long long`. Se o seu sistema nao tiver esse tipo disponivel, voce pode usar `double`. Porem, o formato inteiro fornece uma representacao visual mais agradavel de como os numeros crescem.

**Notas do Programa**

O programa na Listagem 5.4 cria um array para conter os valores fatoriais. O elemento 0 e `0!`, o elemento 1 e `1!`, e assim por diante. Como os dois primeiros fatoriais sao iguais a 1, o programa define os dois primeiros elementos do array `fatoriais` como 1. (Lembre-se, o primeiro elemento de um array tem um valor de indice de 0.) Apos isso, o programa usa um loop para definir cada fatorial como o produto do indice com o fatorial anterior. O loop ilustra que voce pode usar o contador de loop como uma variavel no corpo do loop.

O programa na Listagem 5.4 demonstra como o loop `for` funciona em conjunto com arrays, fornecendo um meio conveniente de acessar cada membro do array por sua vez. Alem disso, `formore.cpp` usa `const` para criar uma representacao simbolica (`TamArr`) para o tamanho do array. Em seguida, usa `TamArr` onde quer que o tamanho do array entre em jogo, como na definicao do array e nos limites dos loops que manipulam o array. Agora, se voce quiser estender o programa para, digamos, 20 fatoriais, basta definir `TamArr` como 20 no programa e recompilar. Usando uma constante simbolica, voce evita ter que alterar cada ocorrencia de `16` para `20` individualmente.

> **Dica:** Geralmente e uma boa ideia definir um valor `const` para representar o numero de elementos em um array. Voce pode usar o valor `const` na declaracao do array e em todas as outras referencias ao tamanho do array, como em um loop `for`.

A expressao de limite `i < TamArr` reflete o fato de que os subscitos de um array com `TamArr` elementos vao de 0 a `TamArr - 1`, portanto o indice do array deve parar um abaixo de `TamArr`. Voce poderia usar o teste `i <= TamArr - 1` em vez disso, mas parece estranho em comparacao.

Observe que o programa declara a variavel `const int` chamada `TamArr` fora do corpo de `main()`. Como mencionado ao final do Capitulo 4, "Tipos Compostos", isso torna `TamArr` um dado externo. As duas consequencias de declarar `TamArr` dessa forma sao que `TamArr` existe durante toda a duracao do programa e que todas as funcoes no arquivo do programa podem usa-la. Neste caso particular, o programa tem apenas uma funcao, portanto declarar `TamArr` externamente tem pouco efeito pratico. Mas programas com multiplas funcoes frequentemente se beneficiam do compartilhamento de constantes externas, portanto vamos praticar o uso delas agora.

Alem disso, esse exemplo nos lembra que podemos usar `std::` em vez de uma diretiva `using` para disponibilizar nomes padrao selecionados.

---

## Alterando o Tamanho do Passo

Ate agora, os exemplos de loop neste capitulo aumentaram ou diminuiram o contador do loop em um em cada ciclo. Voce pode alterar isso modificando a expressao de atualizacao. O programa na Listagem 5.5, por exemplo, aumenta o contador do loop em um tamanho de passo selecionado pelo usuario. Em vez de usar `i++` como a expressao de atualizacao, ele usa a expressao `i = i + passo`, onde `passo` e o tamanho de passo selecionado pelo usuario.

**Listagem 5.5 — bigstep.cpp**

```cpp
// bigstep.cpp -- conta conforme indicado
#include <iostream>
int main()
{
    using std::cout; // uma declaracao using
    using std::cin;
    using std::endl;
    cout << "Digite um inteiro: ";
    int passo;
    cin >> passo;
    cout << "Contando de " << passo << " em " << passo << ":\n";
    for (int i = 0; i < 100; i = i + passo)
        cout << i << endl;
    return 0;
}
```

Aqui esta uma execucao de exemplo do programa na Listagem 5.5:

```
Digite um inteiro: 17
Contando de 17 em 17:
0
17
34
51
68
85
```

Quando `i` atinge o valor 102, o loop encerra. O ponto principal aqui e que a expressao de atualizacao pode ser qualquer expressao valida. Por exemplo, se voce quiser elevar `i` ao quadrado e adicionar 10 em cada ciclo, voce pode usar `i = i * i + 10`.

Outro ponto a observar e que frequentemente e uma ideia melhor testar a desigualdade do que a igualdade. Por exemplo, o teste `i == 100` teria falhado neste caso porque `i` pula o valor 100.

Por fim, esse exemplo ilustra o uso de declaracoes `using` em vez de uma diretiva `using`.

---

## Percorrendo Strings com o Loop for

O loop `for` oferece uma maneira direta de acessar cada caractere em uma string por sua vez. Por exemplo, a Listagem 5.6 permite que voce insira uma string e, em seguida, exibe a string caractere por caractere, em ordem inversa. Voce poderia usar um objeto da classe `string` ou um array de `char` neste exemplo, porque ambos permitem que voce use notacao de array para acessar caracteres individuais em uma string; a Listagem 5.6 usa um objeto da classe `string`. O metodo `size()` da classe `string` retorna o numero de caracteres na string; o loop usa esse valor em sua expressao inicializadora para definir `i` como o indice do ultimo caractere na string, sem contar o caractere nulo. Para contar para traz, o programa usa o operador de decremento (`--`) para diminuir o subscito do array em um em cada ciclo. Alem disso, a Listagem 5.6 usa o operador relacional maior-que-ou-igual-a (`>=`) para testar se o loop alcancou o primeiro elemento.

**Listagem 5.6 — forstr1.cpp**

```cpp
// forstr1.cpp -- usando for com uma string
#include <iostream>
#include <string>
int main()
{
    using namespace std;
    cout << "Digite uma palavra: ";
    string palavra;
    cin >> palavra;
    // exibe as letras em ordem inversa
    for (int i = palavra.size() - 1; i >= 0; i--)
        cout << palavra[i];
    cout << "\nAte mais.\n";
    return 0;
}
```

Aqui esta uma execucao de exemplo do programa na Listagem 5.6:

```
Digite uma palavra: animal
lamina
Ate mais.
```

Sim, o programa consegue imprimir `animal` ao contrario; escolher `animal` como palavra de teste ilustra o efeito deste programa mais claramente do que escolher, digamos, um palindromo como `arara` ou `radar`.

---

> Navegacao: [Anterior](capitulo-04-08-resumo-revisao.md) | [Indice](README.md) | [Proximo](capitulo-05-02-operadores.md)
