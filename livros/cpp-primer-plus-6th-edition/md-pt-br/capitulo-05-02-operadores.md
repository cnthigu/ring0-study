# Capitulo 5 - Loops e Expressoes Relacionais (Loops and Relational Expressions)
## Parte 2: Operadores de Incremento/Decremento, Atribuicao Composta, Blocos e Virgula

> Traducao nao oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Navegacao: [Anterior](capitulo-05-01-loop-for.md) | [Indice](README.md) | [Proximo](capitulo-05-03-expressoes-relacionais.md)

---

## Os Operadores de Incremento (++) e Decremento (--)

O C++ possui varios operadores que sao usados com frequencia em loops; vamos dedicar um momento a examina-los agora. Voce ja viu dois deles: o operador de incremento (`++`), que inspirou o nome C++, e o operador de decremento (`--`). Esses operadores realizam duas operacoes de loop muito comuns: aumentar e diminuir um contador de loop em um. Porem, ha mais em sua historia do que voce ja viu ate este ponto. Cada operador vem em duas variedades. A versao **prefixada** (prefix) fica antes do operando, como em `++x`. A versao **posfixada** (postfix) fica depois do operando, como em `x++`. As duas versoes tem o mesmo efeito sobre o operando, mas diferem em termos de *quando* isso acontece. E como ser pago por cortar o gramado antes ou depois; os dois metodos tem o mesmo efeito final na sua carteira, mas diferem em quando o dinheiro e adicionado. A Listagem 5.7 demonstra essa diferenca para o operador de incremento.

**Listagem 5.7 — plus_one.cpp**

```cpp
// plus_one.cpp -- o operador de incremento
#include <iostream>
int main()
{
    using std::cout;
    int a = 20;
    int b = 20;
    cout << "a = " << a << ": b = " << b << "\n";
    cout << "a++ = " << a++ << ": ++b = " << ++b << "\n";
    cout << "a = " << a << ": b = " << b << "\n";
    return 0;
}
```

Aqui esta a saida do programa na Listagem 5.7:

```
a   = 20: b = 20
a++ = 20: ++b = 21
a   = 21: b = 21
```

De forma aproximada, a notacao `a++` significa "use o valor atual de `a` para avaliar uma expressao e, em seguida, incremente o valor de `a`." Da mesma forma, a notacao `++b` significa "primeiro incremente o valor de `b` e, em seguida, use o novo valor na avaliacao da expressao." Por exemplo, temos os seguintes relacionamentos:

```cpp
int x = 5;
int y = ++x; // altera x, depois atribui a y
           // y vale 6, x vale 6

int z = 5;
int y = z++; // atribui a y, depois altera z
           // y vale 5, z vale 6
```

Usar os operadores de incremento e decremento e uma maneira concisa e conveniente de lidar com a tarefa comum de aumentar ou diminuir valores em um.

Os operadores de incremento e decremento sao operadores pequenos e bacanas, mas nao os utilize em excesso incrementando ou decrementando o mesmo valor mais de uma vez na mesma instrucao. O problema e que as regras de usar-depois-alterar e alterar-depois-usar podem se tornar ambiguas. Ou seja, uma instrucao como a seguinte pode produzir resultados bastante diferentes em diferentes sistemas:

```cpp
x = 2 * x++ * (3 - ++x); // nao faca isso, exceto como experimento
```

O C++ nao define o comportamento correto para esse tipo de instrucao.

## Efeitos Colaterais e Pontos de Sequencia

Vamos examinar mais de perto o que o C++ diz e nao diz sobre quando os operadores de incremento tem efeito. Primeiro, lembre-se de que um **efeito colateral** (side effect) e um efeito que ocorre quando a avaliacao de uma expressao modifica algo, como um valor armazenado em uma variavel. Um **ponto de sequencia** (sequence point) e um ponto na execucao do programa no qual todos os efeitos colaterais sao garantidamente avaliados antes de passar para a proxima etapa. Em C++, o ponto e virgula em uma instrucao marca um ponto de sequencia. Isso significa que todas as alteracoes feitas por operadores de atribuicao, incremento e decremento em uma instrucao devem ocorrer antes que um programa prossiga para a proxima instrucao. Alguns operadores que discutiremos em capitulos posteriores tem pontos de sequencia. Alem disso, o final de qualquer expressao completa e um ponto de sequencia.

O que e uma expressao completa? E uma expressao que nao e uma subexpressao de uma expressao maior. Exemplos de expressoes completas incluem a parte de expressao de uma instrucao de expressao e uma expressao que serve como condicao de teste para um loop `while`.

Os pontos de sequencia ajudam a esclarecer quando ocorre a incrementacao posfixada. Considere, por exemplo, o seguinte codigo:

```cpp
while (convidados++ < 10)
    cout << convidados << endl;
```

(O loop `while`, discutido mais adiante neste capitulo, funciona como um loop `for` que tem apenas uma expressao de teste.) Algumas vezes os iniciantes em C++ assumem que "use o valor, depois incremente-o" significa, neste contexto, incrementar `convidados` apos ser usado na instrucao `cout`. Porem, a expressao `convidados++ < 10` e uma expressao completa porque e uma condicao de teste de loop `while`, portanto o final desta expressao e um ponto de sequencia. Portanto, o C++ garante que o efeito colateral (incrementar `convidados`) ocorra antes que o programa passe para `cout`. Usar a forma posfixada, porem, garante que `convidados` sera incrementado *apos* a comparacao com 10 ser feita.

Agora considere esta instrucao:

```cpp
y = (4 + x++) + (6 + x++);
```

A expressao `4 + x++` nao e uma expressao completa, portanto o C++ nao garante que `x` sera incrementado imediatamente apos a subexpressao `4 + x++` ser avaliada. Aqui a expressao completa e toda a instrucao de atribuicao, e o ponto e virgula marca o ponto de sequencia, portanto tudo que o C++ garante e que `x` tera sido incrementado duas vezes no momento em que o programa passar para a instrucao seguinte. O C++ nao especifica se `x` e incrementado apos cada subexpressao ser avaliada ou somente apos todas as expressoes terem sido avaliadas, razao pela qual voce deve evitar instrucoes desse tipo.

A documentacao do C++11 abandonou o termo "ponto de sequencia" porque o conceito nao se aplica bem quando se discute multiplas linhas de execucao (threads). Em vez disso, as descricoes sao enquadradas em termos de sequenciamento, com alguns eventos sendo descritos como sendo sequenciados *antes* de outros eventos. Essa abordagem descritiva nao pretende mudar as regras; o objetivo e fornecer uma linguagem que possa lidar com mais clareza com a programacao multi-thread.

## Prefixacao Versus Posfixacao

Claramente, se voce usa a forma prefixada ou posfixada faz diferenca se o valor e usado para algum proposito, como argumento de funcao ou atribuicao a uma variavel. Mas e se o valor de uma expressao de incremento ou decremento nao e usado? Por exemplo, sao

```cpp
x++;
```

e

```cpp
++x;
```

diferentes entre si? Ou sao

```cpp
for (n = lim; n > 0; --n)
    ...;
```

e

```cpp
for (n = lim; n > 0; n--)
    ...;
```

diferentes entre si?

Logicamente, se as formas prefixadas ou posfixadas sao usadas nao faz diferenca nessas duas situacoes. Os valores das expressoes nao sao usados, portanto os unicos efeitos sao os efeitos colaterais. Aqui as expressoes usando os operadores sao expressoes completas, portanto os efeitos colaterais de incrementar `x` e decrementar `n` sao garantidamente realizados no momento em que o programa passa para a proxima etapa; a forma prefixada e a posfixada levam ao mesmo resultado final.

Porem, embora a escolha entre as formas prefixada e posfixada nao tenha efeito no comportamento do programa, e possivel que a escolha tenha um pequeno efeito na velocidade de execucao. Para tipos embutidos e compiladores modernos, isso parece ser algo sem importancia. Mas o C++ permite que voce defina esses operadores para classes. Nesse caso, o usuario define uma funcao prefixada que funciona incrementando um valor e depois retornando-o. Mas a versao posfixada funciona primeiro guardando uma copia do valor, incrementando o valor e depois retornando a copia guardada. Assim, para classes, a versao prefixada e um pouco mais eficiente do que a versao posfixada.

Em suma, para tipos embutidos, provavelmente nao faz diferenca qual forma voce usa. Para tipos definidos pelo usuario com operadores de incremento e decremento definidos pelo usuario, a forma prefixada e mais eficiente.

## Os Operadores de Incremento/Decremento e Ponteiros

Voce pode usar operadores de incremento com ponteiros, bem como com variaveis basicas. Lembre-se de que adicionar um operador de incremento a um ponteiro aumenta seu valor pelo numero de bytes no tipo para o qual ele aponta. A mesma regra vale para incrementar e decrementar ponteiros:

```cpp
double arr[5] = {21.1, 32.8, 23.4, 45.2, 37.4};
double *pt = arr; // pt aponta para arr[0], ou seja, para 21.1
++pt;             // pt aponta para arr[1], ou seja, para 32.8
```

Voce tambem pode usar esses operadores para alterar a quantidade para a qual um ponteiro aponta, usando-os em conjunto com o operador `*`. Aplicar tanto `*` quanto `++` a um ponteiro levanta as questoes de o que e desreferenciado e o que e incrementado. Essas acoes sao determinadas pelo posicionamento e pela precedencia dos operadores. Os operadores de incremento prefixado, decremento prefixado e desreferenciamento tem a mesma precedencia e associam da direita para a esquerda. Os operadores de incremento e decremento posfixados tem a mesma precedencia entre si, que e maior do que a precedencia prefixada. Esses dois operadores associam da esquerda para a direita.

A regra de associacao da direita para a esquerda para operadores prefixados implica que `*++pt` significa primeiro aplicar `++` a `pt` (porque `++` esta a direita de `*`) e depois aplicar `*` ao novo valor de `pt`:

```cpp
double x = *++pt; // incrementa o ponteiro, obtem o valor; ou seja, arr[2], ou 23.4
```

Por outro lado, `++*pt` significa obter o valor para o qual `pt` aponta e, em seguida, incrementar esse valor:

```cpp
++*pt;  // incrementa o valor apontado; ou seja, muda 23.4 para 24.4
```

Aqui, `pt` permanece apontando para `arr[2]`.

Em seguida, considere esta combinacao:

```cpp
(*pt)++; // incrementa o valor apontado
```

Os parenteses indicam que primeiro o ponteiro e desreferenciado, produzindo 24.4. Entao o operador `++` incrementa esse valor para 25.4; `pt` permanece apontando em `arr[2]`.

Por fim, considere esta combinacao:

```cpp
x = *pt++; // desreferencia a localizacao original, depois incrementa o ponteiro
```

A precedencia mais alta do operador `++` posfixado significa que o operador `++` opera em `pt`, nao em `*pt`, portanto o ponteiro e incrementado. Mas o fato de que o operador posfixado e usado significa que o endereco que e desreferenciado e o endereco original, `&arr[2]`, nao o novo endereco. Assim, o valor de `*pt++` e `arr[2]`, ou 25.4, mas o valor de `pt` apos a conclusao da instrucao e o endereco de `arr[3]`.

> **Nota:** Incrementar e decrementar ponteiros segue as regras de aritmetica de ponteiros. Assim, se `pt` aponta para o primeiro membro de um array, `++pt` altera `pt` para que ele aponte para o segundo membro.

---

## Operadores de Atribuicao Combinados

A Listagem 5.5 usa a seguinte expressao para atualizar um contador de loop:

```
i = i + passo
```

O C++ tem um operador combinado de adicao e atribuicao que realiza o mesmo resultado de forma mais concisa:

```
i += passo
```

O operador `+=` adiciona os valores de seus dois operandos e atribui o resultado ao operando a esquerda. Isso implica que o operando a esquerda deve ser algo ao qual voce pode atribuir um valor, como uma variavel, um elemento de array, um membro de estrutura ou dados que voce identifica desreferenciando um ponteiro:

```cpp
int k = 5;
k += 3;         // ok, k definido como 8
int *pa = new int[10]; // pa aponta para pa[0]
pa[4] = 12;
pa[4] += 6;     // ok, pa[4] definido como 18
*(pa + 4) += 7; // ok, pa[4] definido como 25
pa += 2;        // ok, pa aponta para o antigo pa[2]
34 += 10;       // totalmente errado
```

Cada operador aritmetico tem um operador de atribuicao correspondente, conforme resumido na Tabela 5.1. Cada operador funciona de forma analoga a `+=`. Assim, por exemplo, a seguinte instrucao substitui o valor atual de `k` por um valor 10 vezes maior:

```cpp
k *= 10;
```

**Tabela 5.1 — Operadores de Atribuicao Combinados**

| Operador | Efeito (E = operando esquerdo, D = operando direito) |
|----------|-----------------------------------------------------|
| `+=`     | Atribui E + D a E                                   |
| `-=`     | Atribui E - D a E                                   |
| `*=`     | Atribui E * D a E                                   |
| `/=`     | Atribui E / D a E                                   |
| `%=`     | Atribui E % D a E                                   |

---

## Instrucoes Compostas, ou Blocos

O formato, ou sintaxe, para escrever uma instrucao `for` C++ pode parecer restritivo porque o corpo do loop deve ser uma unica instrucao. Isso e incomodo se voce quiser que o corpo do loop contenha diversas instrucoes. Felizmente, o C++ fornece uma valvula de escape sintatica pela qual voce pode enfiar quantas instrucoes quiser em um corpo de loop. O truque e usar chaves emparelhadas para construir uma **instrucao composta** (compound statement), ou **bloco** (block). O bloco consiste de chaves emparelhadas e as instrucoes que elas delimitam e, para os propositos da sintaxe, conta como uma unica instrucao. Por exemplo, o programa na Listagem 5.8 usa chaves para combinar tres instrucoes separadas em um unico bloco. Isso permite que o corpo do loop solicite ao usuario, leia a entrada e faca um calculo. O programa calcula a soma acumulada dos numeros que voce insere, e isso oferece uma ocasiao natural para usar o operador `+=`.

**Listagem 5.8 — block.cpp**

```cpp
// block.cpp -- usa instrucao de bloco
#include <iostream>
int main()
{
    using namespace std;
    cout << "O Incrivel Contador somara e tirara a media ";
    cout << "de cinco numeros para voce.\n";
    cout << "Por favor, insira cinco valores:\n";
    double numero;
    double soma = 0.0;
    for (int i = 1; i <= 5; i++)
    {                           // bloco comeca aqui
        cout << "Valor " << i << ": ";
        cin >> numero;
        soma += numero;
    }                           // bloco termina aqui
    cout << "Cinco escolhas exquisitas! ";
    cout << "A soma e " << soma << endl;
    cout << "e a media e " << soma / 5 << ".\n";
    cout << "O Incrivel Contador se despede!\n";
    return 0;
}
```

Aqui esta uma execucao de exemplo do programa na Listagem 5.8:

```
O Incrivel Contador somara e tirara a media de cinco numeros para voce.
Por favor, insira cinco valores:
Valor 1: 1942
Valor 2: 1948
Valor 3: 1957
Valor 4: 1974
Valor 5: 1980
Cinco escolhas exquisitas! A soma e 9801
e a media e 1960.2.
O Incrivel Contador se despede!
```

Suponha que voce mantenha a indentacao mas omita as chaves:

```cpp
for (int i = 1; i <= 5; i++)
    cout << "Valor " << i << ": "; // loop termina aqui
    cin >> numero;                  // apos o loop
    soma += numero;
cout << "Cinco escolhas exquisitas! ";
```

O compilador ignora a indentacao, portanto apenas a primeira instrucao estaria no loop. Assim, o loop imprimiria os cinco prompts e nada mais. Apos o termino do loop, o programa passaria para as linhas seguintes, lendo e somando apenas um numero.

As instrucoes compostas tem outra propriedade interessante. Se voce definir uma nova variavel dentro de um bloco, a variavel persiste apenas enquanto o programa esta executando instrucoes dentro do bloco. Quando a execucao sai do bloco, a variavel e desalocada. Isso significa que a variavel e conhecida apenas dentro do bloco:

```cpp
#include <iostream>
int main()
{
    using namespace std;
    int x = 20;
    {             // bloco comeca
        int y = 100;
        cout << x << endl; // ok
        cout << y << endl; // ok
    }             // bloco termina
    cout << x << endl; // ok
    cout << y << endl; // invalido, nao compila
    return 0;
}
```

Observe que uma variavel definida em um bloco externo ainda esta definida no bloco interno.

O que acontece se voce declarar uma variavel em um bloco que tem o mesmo nome de uma variavel fora do bloco? A nova variavel oculta a antiga desde o ponto em que aparece ate o final do bloco. Entao a antiga volta a ser visivel, como neste exemplo:

```cpp
#include <iostream>
int main()
{
    using std::cout;
    using std::endl;
    int x = 20;    // x original
    {              // bloco comeca
        cout << x << endl; // usa o x original
        int x = 100; // novo x
        cout << x << endl; // usa o novo x
    }              // bloco termina
    cout << x << endl; // usa o x original
    return 0;
}
```

---

## Mais Truques de Sintaxe — O Operador Virgula

Como voce viu, um bloco permite que voce enfie duas ou mais instrucoes em um lugar onde a sintaxe C++ permite apenas uma instrucao. O operador virgula (comma operator) faz o mesmo para expressoes, permitindo que voce enfie duas expressoes em um lugar onde a sintaxe C++ permite apenas uma expressao. Por exemplo, suponha que voce tem um loop em que uma variavel aumenta em um a cada ciclo e uma segunda variavel diminui em um a cada ciclo. Fazer ambas na parte de atualizacao de uma secao de controle de loop `for` seria conveniente, mas a sintaxe do loop permite apenas uma expressao ali. A solucao e usar o operador virgula para combinar as duas expressoes em uma:

```cpp
++j, --i // duas expressoes contam como uma para propositos de sintaxe
```

A virgula nem sempre e um operador virgula. Por exemplo, a virgula nesta declaracao serve para separar nomes adjacentes em uma lista de variaveis:

```cpp
int i, j; // a virgula aqui e um separador, nao um operador
```

A Listagem 5.9 usa o operador virgula duas vezes em um programa que inverte o conteudo de um objeto da classe `string`. (Voce tambem poderia escrever o programa usando um array de `char`, mas o comprimento da palavra seria limitado pelo tamanho do array que voce escolhesse.) Observe que a Listagem 5.6 exibe o conteudo de um array em ordem inversa, mas a Listagem 5.9 realmente move os caracteres pelo array. O programa na Listagem 5.9 tambem usa um bloco para agrupar diversas instrucoes em uma.

**Listagem 5.9 — forstr2.cpp**

```cpp
// forstr2.cpp -- invertendo um array
#include <iostream>
#include <string>
int main()
{
    using namespace std;
    cout << "Digite uma palavra: ";
    string palavra;
    cin >> palavra;
    // modifica fisicamente o objeto string
    char temp;
    int i, j;
    for (j = 0, i = palavra.size() - 1; j < i; --i, ++j)
    {             // inicio do bloco
        temp = palavra[i];
        palavra[i] = palavra[j];
        palavra[j] = temp;
    }             // fim do bloco
    cout << palavra << "\nPronto\n";
    return 0;
}
```

Aqui esta uma execucao de exemplo do programa na Listagem 5.9:

```
Digite uma palavra: stressed
desserts
Pronto
```

Alias, a classe `string` oferece maneiras mais concisas de inverter uma string, mas deixaremos isso para o Capitulo 16, "A Classe `string` e a Biblioteca de Templates Padrao."

**Notas do Programa**

Observe a secao de controle `for` do programa na Listagem 5.9. Primeiro, ela usa o operador virgula para espremer duas inicializacoes em uma expressao para a primeira parte da secao de controle. Em seguida, ela usa o operador virgula novamente para combinar duas atualizacoes em uma unica expressao para a ultima parte da secao de controle.

Em seguida, observe o corpo. O programa usa chaves para combinar diversas instrucoes em uma unica unidade. No corpo, o programa inverte a palavra trocando o primeiro elemento do array com o ultimo elemento. Em seguida, incrementa `j` e decrementa `i` para que agora eles se refiram ao segundo elemento e ao penultimo elemento. Apos isso ser feito, o programa troca esses elementos. Observe que a condicao de teste `j < i` faz o loop parar quando ele atinge o centro do array. Se continuasse alem desse ponto, ele comecaria a trocar os elementos ja trocados de volta para suas posicoes originais.

Outra coisa a observar e a localizacao para declarar as variaveis `temp`, `i` e `j`. O codigo declara `i` e `j` antes do loop porque voce nao pode combinar duas declaracoes com o operador virgula. Isso e porque declaracoes ja usam a virgula para outro proposito — separar itens em uma lista. Voce pode usar uma unica expressao de instrucao de declaracao para criar e inicializar duas variaveis, mas e um pouco confuso visualmente:

```cpp
int j = 0, i = palavra.size() - 1;
```

Neste caso, a virgula e apenas um separador de lista, nao o operador virgula, portanto a expressao declara e inicializa tanto `j` quanto `i`. Porem, parece como se declarasse apenas `j`.

Incidentalmente, voce pode declarar `temp` dentro do loop `for`:

```cpp
int temp = palavra[i];
```

Isso pode resultar em `temp` sendo alocado e desalocado em cada ciclo de loop. Isso pode ser um pouco mais lento do que declarar `temp` uma vez antes do loop. Por outro lado, apos o termino do loop, `temp` e descartado se for declarado dentro do loop.

## Detalhes do Operador Virgula

De longe, o uso mais comum do operador virgula e caber duas ou mais expressoes em uma unica expressao de loop `for`. Mas o C++ fornece ao operador duas propriedades adicionais. Primeiro, ele garante que a primeira expressao e avaliada antes da segunda. (Em outras palavras, o operador virgula e um ponto de sequencia.) Expressoes como a seguinte sao seguras:

```cpp
i = 20, j = 2 * i // i definido como 20, depois j definido como 40
```

Em segundo lugar, o C++ afirma que o valor de uma expressao virgula e o valor da segunda parte da expressao. O valor da expressao anterior, por exemplo, e 40 porque esse e o valor de `j = 2 * i`.

O operador virgula tem a menor precedencia de qualquer operador. Por exemplo, esta instrucao:

```cpp
gatos = 17,240;
```

e lida assim:

```cpp
(gatos = 17), 240;
```

Ou seja, `gatos` e definido como 17, e 240 nao faz nada. Mas como os parenteses tem alta precedencia, o seguinte resulta em `gatos` sendo definido como 240, o valor da expressao a direita da virgula:

```cpp
gatos = (17,240);
```

---

> Navegacao: [Anterior](capitulo-05-01-loop-for.md) | [Indice](README.md) | [Proximo](capitulo-05-03-expressoes-relacionais.md)
