# Capitulo 7 - Funcoes: Modulos de Programacao do C++
## Parte 4: Recursao, Ponteiros para Funcoes, Resumo e Revisao

> Traducao nao oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Navegacao: [Anterior](capitulo-07-03-funcoes-strings-estruturas.md) | [Indice](README.md) | [Proximo](capitulo-08-01-funcoes-inline-referencias.md)

---

## Recursao

Uma funcao C++ tem a interessante caracteristica de poder chamar a si mesma. (Ao contrario do C, no entanto, o C++ nao deixa `main()` chamar a si mesma.) Essa capacidade e chamada de **recursao** (recursion). A recursao e uma ferramenta importante em certos tipos de programacao, como inteligencia artificial, mas so daremos uma olhada superficial em como ela funciona.

### Recursao com uma Unica Chamada Recursiva

Se uma funcao recursiva chama a si mesma, a funcao recentemente chamada chama a si mesma, e assim por diante, ao infinito, a menos que o codigo inclua algo para encerrar a cadeia de chamadas. O metodo usual e tornar a chamada recursiva parte de uma instrucao `if`. Por exemplo, uma funcao recursiva do tipo `void` chamada `recurs()` pode ter uma forma assim:

```cpp
void recurs(listaDeArgumentos)
{
    instrucoes1
    if (teste)
        recurs(argumentos)
    instrucoes2
}
```

Com sorte ou previsao, `teste` eventualmente torna-se falso e a cadeia de chamadas e quebrada.

Chamadas recursivas produzem uma cadeia intrigante de eventos. Enquanto a instrucao `if` permanece verdadeira, cada chamada para `recurs()` executa `instrucoes1` e entao invoca uma nova encarnacao de `recurs()` sem atingir `instrucoes2`. Quando a instrucao `if` torna-se falsa, a chamada atual entao prossegue para `instrucoes2`. Entao quando a chamada atual termina, o controle do programa retorna para a versao anterior de `recurs()` que a chamou. Entao aquela versao de `recurs()` completa a execucao de sua secao `instrucoes2` e termina, retornando o controle para a chamada anterior, e assim por diante. Assim, se `recurs()` sofrer cinco chamadas recursivas, primeiro a secao `instrucoes1` e executada cinco vezes na ordem em que as funcoes foram chamadas, e entao a secao `instrucoes2` e executada cinco vezes na ordem inversa.

A Listagem 7.16 ilustra esse comportamento.

**Listagem 7.16 — recur.cpp**

```cpp
// recur.cpp -- usando recursao
#include <iostream>
void contagem_regressiva(int n);
int main()
{
    contagem_regressiva(4); // chama a funcao recursiva
    return 0;
}
void contagem_regressiva(int n)
{
    using namespace std;
    cout << "Contagem regressiva ... " << n << endl;
    if (n > 0)
        contagem_regressiva(n-1); // a funcao chama a si mesma
    cout << n << ": Kabum!\n";
}
```

Aqui esta a saida anotada do programa na Listagem 7.16:

```
Contagem regressiva ... 4  <nivel 1; adicionando niveis de recursao
Contagem regressiva ... 3  <nivel 2
Contagem regressiva ... 2  <nivel 3
Contagem regressiva ... 1  <nivel 4
Contagem regressiva ... 0  <nivel 5; ultima chamada recursiva
0: Kabum!                  <nivel 5; comecando a voltar
1: Kabum!                  <nivel 4
2: Kabum!                  <nivel 3
3: Kabum!                  <nivel 2
4: Kabum!                  <nivel 1
```

Observe que cada chamada recursiva cria seu proprio conjunto de variaveis, portanto na quinta chamada o programa tem cinco variaveis separadas chamadas `n`, cada uma com um valor diferente.

### Recursao com Multiplas Chamadas Recursivas

A recursao e particularmente util para situacoes que exigem subdividir repetidamente uma tarefa em duas tarefas menores semelhantes. Por exemplo, considere esta abordagem para desenhar uma regua. Marque as duas extremidades, localize o ponto medio e marque-o. Entao aplique este mesmo procedimento na metade esquerda da regua e depois na metade direita. Se voce quiser mais subdivisoes, aplique o mesmo procedimento a cada uma das subdivisoes atuais. Esta abordagem recursiva e as vezes chamada de **estrategia de dividir e conquistar** (divide-and-conquer strategy).

A Listagem 7.17 ilustra essa abordagem com a funcao recursiva `subdividir()`. Ela usa uma string inicialmente preenchida com espacos, exceto por um caractere `|` em cada extremidade.

**Listagem 7.17 — ruler.cpp**

```cpp
// ruler.cpp -- usando recursao para subdividir uma regua
#include <iostream>
const int Comp = 66;
const int Divs = 6;
void subdividir(char ar[], int baixo, int alto, int nivel);
int main()
{
    char regua[Comp];
    int i;
    for (i = 1; i < Comp - 2; i++)
        regua[i] = ' ';
    regua[Comp - 1] = '\0';
    int max = Comp - 2;
    int min = 0;
    regua[min] = regua[max] = '|';
    std::cout << regua << std::endl;
    for (i = 1; i <= Divs; i++)
    {
        subdividir(regua, min, max, i);
        std::cout << regua << std::endl;
        for (int j = 1; j < Comp - 2; j++)
            regua[j] = ' '; // reinicia para regua em branco
    }
    return 0;
}
void subdividir(char ar[], int baixo, int alto, int nivel)
{
    if (nivel == 0)
        return;
    int meio = (alto + baixo) / 2;
    ar[meio] = '|';
    subdividir(ar, baixo, meio, nivel - 1);
    subdividir(ar, meio, alto, nivel - 1);
}
```

Aqui esta a saida do programa na Listagem 7.17:

```
|                                     |
|                  |                  |
|         |        |         |        |
|    |    |    |   |    |    |    |   |
|  | | |  | |  | | |  | |  | | |  | | |
| | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | |
|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
```

A funcao `subdividir()` usa a variavel `nivel` para controlar o nivel de recursao. Quando a funcao chama a si mesma, ela reduz `nivel` em um, e a funcao com `nivel` de 0 termina. Observe que `subdividir()` chama a si mesma duas vezes, uma para a subdivisao esquerda e uma para a subdivisao direita. O ponto medio original torna-se a extremidade direita para uma chamada e a extremidade esquerda para a outra chamada.

Observe que o numero de chamadas cresce geometricamente. Ou seja, uma chamada gera duas, que geram quatro chamadas, que geram oito, e assim por diante. E por isso que a chamada de nivel 6 e capaz de preencher 64 elementos (2^6 = 64). Esse continuo dobramente do numero de chamadas de funcao torna essa forma de recursao uma escolha ruim se muitos niveis de recursao forem necessarios. Mas e uma escolha elegante e simples se os niveis necessarios de recursao forem poucos.

---

## Ponteiros para Funcoes

Nenhuma discussao de funcoes C ou C++ estaria completa sem mencao de ponteiros para funcoes. Vamos dar uma rapida olhada neste topico.

Funcoes, como itens de dados, tem enderecos. O endereco de uma funcao e o endereco de memoria no qual o codigo de linguagem de maquina armazenado para a funcao comeca. Normalmente, nao e nem importante nem util para voce ou o usuario saber esse endereco, mas pode ser util para um programa. Por exemplo, e possivel escrever uma funcao que toma o endereco de outra funcao como argumento. Isso permite que a primeira funcao encontre a segunda funcao e a execute. Essa abordagem e mais complicada do que simplesmente ter a primeira funcao chamar a segunda diretamente, mas deixa aberta a possibilidade de passar diferentes enderecos de funcoes em momentos diferentes. Isso significa que a primeira funcao pode usar funcoes diferentes em momentos diferentes.

### Basicos de Ponteiro para Funcao

Vamos esclarecer este processo com um exemplo. Suponha que voce queira projetar uma funcao `estimar()` que estime a quantidade de tempo necessaria para escrever um determinado numero de linhas de codigo, e voce quer que diferentes programadores usem a funcao. Para implementar este plano, voce precisa ser capaz de fazer o seguinte:

- Obter o endereco de uma funcao.
- Declarar um ponteiro para uma funcao.
- Usar um ponteiro para uma funcao para invocar a funcao.

**Obtendo o Endereco de uma Funcao:** Obter o endereco de uma funcao e simples: voce apenas usa o nome da funcao sem parenteses de fechamento. Ou seja, se `pensar()` e uma funcao, entao `pensar` e o endereco da funcao. Para passar uma funcao como argumento, voce passa o nome da funcao. Certifique-se de distinguir entre passar o *endereco* de uma funcao e passar o *valor de retorno* de uma funcao:

```cpp
processo(pensar);    // passa endereco de pensar() para processo()
pensamento(pensar()); // passa valor de retorno de pensar() para pensamento()
```

**Declarando um Ponteiro para uma Funcao:** Para declarar ponteiros para um tipo de dado, a declaracao tinha que especificar exatamente para qual tipo o ponteiro aponta. Da mesma forma, um ponteiro para uma funcao tem que especificar para qual tipo de funcao o ponteiro aponta. Isso significa que a declaracao deve identificar o tipo de retorno da funcao e a assinatura da funcao (sua lista de argumentos). Ou seja, a declaracao deve fornecer as mesmas informacoes sobre uma funcao que um prototipo de funcao faz. Por exemplo, suponha que Pam LeCoder escreveu uma funcao de estimativa de tempo com o seguinte prototipo:

```cpp
double pam(int); // prototipo
```

Aqui esta como uma declaracao de um tipo de ponteiro apropriado parece:

```cpp
double (*pf)(int); // pf aponta para uma funcao que toma
                   // um argumento int e retorna type double
```

> **Dica:** Em geral, para declarar um ponteiro para um tipo especifico de funcao, voce pode primeiro escrever um prototipo para uma funcao regular do tipo desejado e depois substituir o nome da funcao por uma expressao na forma `(*pf)`. Neste caso, `pf` e um ponteiro para uma funcao daquele tipo.

A declaracao requer os parenteses ao redor de `*pf` para fornecer a precedencia de operador adequada. Os parenteses tem uma precedencia mais alta do que o operador `*`, portanto `*pf(int)` significa que `pf()` e uma funcao que retorna um ponteiro, enquanto `(*pf)(int)` significa que `pf` e um ponteiro para uma funcao:

```cpp
double (*pf)(int); // pf aponta para uma funcao que retorna double
double *pf(int);   // pf() uma funcao que retorna um ponteiro-para-double
```

Apos declarar `pf` corretamente, voce pode atribuir a ele o endereco de uma funcao correspondente:

```cpp
double pam(int);
double (*pf)(int);
pf = pam;   // pf agora aponta para a funcao pam()
```

Note que `pam()` tem que corresponder a `pf` tanto em assinatura quanto em tipo de retorno. O compilador rejeita atribuicoes nao correspondentes:

```cpp
double ned(double);
int ted(int);
double (*pf)(int);
pf = ned;  // invalido -- assinatura nao correspondente
pf = ted;  // invalido -- tipos de retorno nao correspondentes
```

**Usando um Ponteiro para Invocar uma Funcao:** Agora chegamos a parte final da tecnica, que e usar um ponteiro para chamar a funcao apontada. A pista vem na declaracao do ponteiro. La, lembre-se, `(*pf)` desempenha o mesmo papel que um nome de funcao. Assim, tudo que voce tem que fazer e usar `(*pf)` como se fosse um nome de funcao:

```cpp
double pam(int);
double (*pf)(int);
pf = pam;             // pf agora aponta para a funcao pam()
double x = pam(4);    // chama pam() usando o nome da funcao
double y = (*pf)(5);  // chama pam() usando o ponteiro pf
```

Na verdade, o C++ tambem permite que voce use `pf` como se fosse um nome de funcao:

```cpp
double y = pf(5); // tambem chama pam() usando o ponteiro pf
```

> **Historia Versus Logica**
>
> Bem, como `pf` e `(*pf)` podem ser equivalentes? Uma escola de pensamento sustenta que como `pf` e um ponteiro para uma funcao, `*pf` e uma funcao; portanto, voce deve usar `(*pf)()` como uma chamada de funcao. Uma segunda escola sustenta que porque o nome de uma funcao e um ponteiro para aquela funcao, um ponteiro para aquela funcao deve agir como o nome da funcao; portanto, voce deve usar `pf()` como uma chamada de funcao. O C++ adota a visao de compromisso de que ambas as formas estao corretas, ou pelo menos podem ser permitidas.

### Um Exemplo de Ponteiro para Funcao

A Listagem 7.18 demonstra usando ponteiros para funcoes em um programa. Ela chama a funcao `estimar()` duas vezes, uma passando o endereco da funcao `betsy()` e uma passando o endereco da funcao `pam()`. No primeiro caso, `estimar()` usa `betsy()` para calcular o numero de horas necessarias, e no segundo caso, `estimar()` usa `pam()` para o calculo.

**Listagem 7.18 — fun_ptr.cpp**

```cpp
// fun_ptr.cpp -- ponteiros para funcoes
#include <iostream>
double betsy(int);
double pam(int);
// o segundo argumento e um ponteiro para uma funcao do tipo double que
// toma um argumento do tipo int
void estimar(int linhas, double (*pf)(int));
int main()
{
    using namespace std;
    int codigo;
    cout << "De quantas linhas de codigo voce precisa? ";
    cin >> codigo;
    cout << "Aqui esta a estimativa de Betsy:\n";
    estimar(codigo, betsy);
    cout << "Aqui esta a estimativa de Pam:\n";
    estimar(codigo, pam);
    return 0;
}
double betsy(int linhas)
{
    return 0.05 * linhas;
}
double pam(int linhas)
{
    return 0.03 * linhas + 0.0004 * linhas * linhas;
}
void estimar(int linhas, double (*pf)(int))
{
    using namespace std;
    cout << linhas << " linhas levarao ";
    cout << (*pf)(linhas) << " hora(s)\n";
}
```

Aqui esta uma execucao de exemplo do programa na Listagem 7.18:

```
De quantas linhas de codigo voce precisa? 30
Aqui esta a estimativa de Betsy:
30 linhas levarao 1.5 hora(s)
Aqui esta a estimativa de Pam:
30 linhas levarao 1.26 hora(s)
```

### Variacoes no Tema de Ponteiros para Funcoes

Com ponteiros para funcoes, a notacao pode se tornar intimidadora. Vamos olhar para um exemplo que ilustra alguns dos desafios dos ponteiros para funcoes e maneiras de lidar com eles. Para comecar, aqui estao prototipos para algumas funcoes que compartilham a mesma assinatura e tipo de retorno:

```cpp
const double * f1(const double ar[], int n);
const double * f2(const double [], int);
const double * f3(const double *, int);
```

As assinaturas podem parecer diferentes, mas sao a mesma. Primeiro, lembre-se de que em uma lista de parametros de prototipo de funcao, `const double ar[]` e `const double * ar` tem exatamente o mesmo significado. Segundo, lembre-se de que em um prototipo voce pode omitir identificadores.

Em seguida, suponha que voce deseje declarar um ponteiro que possa apontar para uma dessas tres funcoes. A tecnica e substituir o nome da funcao por uma expressao na forma `(*pa)`:

```cpp
const double * (*p1)(const double *, int);
```

Isso pode ser combinado com inicializacao:

```cpp
const double * (*p1)(const double *, int) = f1;
```

Com o recurso de deducao automatica de tipo do C++11, voce pode simplificar um pouco:

```cpp
auto p2 = f2; // deducao automatica de tipo C++11
```

Com tres funcoes para trabalhar, pode ser pratico ter um array de ponteiros para funcoes. Entao pode-se usar um loop `for` para chamar cada funcao, por meio de seu ponteiro, por vez:

```cpp
const double *(*pa[3])(const double *, int) = {f1,f2,f3};
```

Por que colocar `[3]` ali? Bem, `pa` e um array de tres coisas, e o ponto de partida para declarar um array de tres coisas e isto: `pa[3]`. O operador de precedencia classifica `[]` acima de `*`, portanto `*pa[3]` diz que `pa` e um array de tres ponteiros. O resto da declaracao indica para o que cada ponteiro aponta: uma funcao com assinatura de `const double *, int` e um tipo de retorno de `const double *`.

Pode-se usar `auto` aqui? Nao. A deducao automatica de tipo funciona com um unico valor inicializador, nao uma lista de inicializacao. Mas agora que temos o array `pa`, e simples declarar um ponteiro do tipo correspondente:

```cpp
auto pb = pa;
```

O nome de um array, como voce deve se lembrar, e um ponteiro para seu primeiro elemento, portanto tanto `pa` quanto `pb` sao ponteiros para um ponteiro para uma funcao.

Algo mais que voce pode fazer e criar um ponteiro para todo o array:

```cpp
auto pc = &pa; // deducao automatica de tipo C++11
```

Se voce preferir fazer isso voce mesmo, note que a declaracao deve se assemelhar a declaracao de `pa`, mas como ha mais um nivel de indirecao, vamos precisar de mais um `*` em algum lugar. Em particular, se chamarmos o novo ponteiro `pd`, precisamos indicar que e um ponteiro, nao um nome de array. Isso sugere que o nucleo da declaracao deve ser `(*pd)[3]`. Os parenteses vinculam o identificador `pd` ao `*`:

```cpp
const double *(*(*pd)[3])(const double *, int) = &pa;
```

A Listagem 7.19 coloca esta discussao em uso. Para fins ilustrativos, as funcoes `f1()`, e assim por diante, foram mantidas embaracosamente simples. O programa mostra, como comentarios, as alternativas do C++98 ao uso de `auto`.

**Listagem 7.19 — arfupt.cpp**

```cpp
// arfupt.cpp -- um array de ponteiros para funcoes
#include <iostream>
// varias notacoes, mesmas assinaturas
const double * f1(const double ar[], int n);
const double * f2(const double [], int);
const double * f3(const double *, int);
int main()
{
    using namespace std;
    double av[3] = {1112.3, 1542.6, 2227.9};
    // ponteiro para uma funcao
    const double *(*p1)(const double *, int) = f1;
    auto p2 = f2; // deducao automatica de tipo C++11
    // pre-C++11 pode usar o seguinte codigo em vez disso
    // const double *(*p2)(const double *, int) = f2;
    cout << "Usando ponteiros para funcoes:\n";
    cout << " Endereco Valor\n";
    cout << (*p1)(av,3) << ": " << *(*p1)(av,3) << endl;
    cout << p2(av,3) << ": " << *p2(av,3) << endl;
    // pa e um array de ponteiros
    // auto nao funciona com inicializacao de lista
    const double *(*pa[3])(const double *, int) = {f1,f2,f3};
    // mas funciona para inicializar para um unico valor
    // pb e um ponteiro para o primeiro elemento de pa
    auto pb = pa;
    cout << "\nUsando um array de ponteiros para funcoes:\n";
    cout << " Endereco Valor\n";
    for (int i = 0; i < 3; i++)
        cout << pa[i](av,3) << ": " << *pa[i](av,3) << endl;
    cout << "\nUsando um ponteiro para um ponteiro para uma funcao:\n";
    cout << " Endereco Valor\n";
    for (int i = 0; i < 3; i++)
        cout << pb[i](av,3) << ": " << *pb[i](av,3) << endl;
    // ponteiro para um array de ponteiros para funcoes
    cout << "\nUsando ponteiros para um array de ponteiros:\n";
    cout << " Endereco Valor\n";
    // maneira facil de declarar pc
    auto pc = &pa;
    cout << (*pc)[0](av,3) << ": " << *(*pc)[0](av,3) << endl;
    // maneira dificil de declarar pd
    const double *(*(*pd)[3])(const double *, int) = &pa;
    // armazena valor de retorno em pdb
    const double * pdb = (*pd)[1](av,3);
    cout << pdb << ": " << *pdb << endl;
    // notacao alternativa
    cout << (*(*pd)[2])(av,3) << ": " << *(*(*pd)[2])(av,3) << endl;
    return 0;
}
// algumas funcoes bastante entediantes
const double * f1(const double * ar, int n)
{
    return ar;
}
const double * f2(const double ar[], int n)
{
    return ar+1;
}
const double * f3(const double ar[], int n)
{
    return ar+2;
}
```

Aqui esta a saida:

```
Usando ponteiros para funcoes:
 Endereco Valor
002AF9E0: 1112.3
002AF9E8: 1542.6
Usando um array de ponteiros para funcoes:
 Endereco Valor
002AF9E0: 1112.3
002AF9E8: 1542.6
002AF9F0: 2227.9
Usando um ponteiro para um ponteiro para uma funcao:
 Endereco Valor
002AF9E0: 1112.3
002AF9E8: 1542.6
002AF9F0: 2227.9
Usando ponteiros para um array de ponteiros:
 Endereco Valor
002AF9E0: 1112.3
002AF9E8: 1542.6
002AF9F0: 2227.9
```

> **Apreciando auto**
>
> Um dos objetivos do C++11 e tornar o C++ mais facil de usar, deixando o programador se concentrar mais no design e menos nos detalhes. A Listagem 7.19 certamente ilustra este ponto:
>
> ```cpp
> auto pc = &pa;  // deducao automatica de tipo C++11
> const double *(*(*pd)[3])(const double *, int) = &pa; // C++98, faca voce mesmo
> ```
>
> O recurso de deducao automatica de tipo reflete uma mudanca filosofica no papel do compilador. No C++98, o compilador usa seu conhecimento para dizer quando voce esta errado. No C++11, pelo menos com esse recurso, ele usa seu conhecimento para ajuda-lo a obter a declaracao correta.

### Simplificando com typedef

O C++ fornece ferramentas alem de `auto` para simplificar declaracoes. Voce pode se lembrar do Capitulo 5, "Loops e Expressoes Relacionais", que a palavra-chave `typedef` permite criar um alias de tipo:

```cpp
typedef double real; // torna real outro nome para double
```

Voce pode fazer isso para criar `p_fun` como um alias para o tipo de ponteiro para funcao usado na Listagem 7.19:

```cpp
typedef const double *(*p_fun)(const double *, int); // p_fun agora e um nome de tipo
p_fun p1 = f1; // p1 aponta para a funcao f1()
```

Voce pode entao usar esse tipo para construir elaboracoes:

```cpp
p_fun pa[3] = {f1,f2,f3}; // pa e um array de 3 ponteiros para funcao
p_fun (*pd)[3] = &pa;      // pd aponta para um array de 3 ponteiros para funcao
```

Nao so `typedef` economiza alguma digitacao, como torna a escrita do codigo menos propensa a erros e torna o programa mais facil de entender.

---

## Resumo

Funcoes sao os modulos de programacao do C++. Para usar uma funcao, voce precisa fornecer uma definicao e um prototipo, e voce tem que usar uma chamada de funcao. A definicao de funcao e o codigo que implementa o que a funcao faz. O prototipo de funcao descreve a interface da funcao: quantos e quais tipos de valores passar para a funcao e que tipo de valor de retorno, se houver, obter dela. A chamada de funcao faz com que o programa passe os argumentos da funcao para a funcao e transfira a execucao do programa para o codigo da funcao.

Por padrao, as funcoes C++ passam argumentos por valor. Isso significa que os parametros formais na definicao da funcao sao novas variaveis inicializadas com os valores fornecidos pela chamada de funcao. Assim, as funcoes C++ protegem a integridade dos dados originais trabalhando com copias.

O C++ trata um argumento de nome de array como o endereco do primeiro elemento do array. Tecnicamente, isso ainda e passar por valor porque o ponteiro e uma copia do endereco original, mas a funcao usa o ponteiro para acessar o conteudo do array original. Quando voce declara parametros formais para uma funcao (e somente entao), as duas declaracoes a seguir sao equivalentes:

```cpp
nomeDoTipo arr[];
nomeDoTipo * arr;
```

Ambas significam que `arr` e um ponteiro para `nomeDoTipo`. Mesmo ao passar ponteiros, voce pode preservar a integridade dos dados originais declarando o argumento formal como um ponteiro para um tipo `const`. Como passar o endereco de um array nao transmite informacoes sobre o tamanho do array, voce normalmente passa o tamanho do array como um argumento separado. Como alternativa, voce pode passar ponteiros para o inicio do array e para uma posicao alem do fim para indicar um intervalo.

O C++ fornece tres maneiras de representar strings no estilo C: usando um array de caracteres, uma constante de string ou um ponteiro para uma string. Todas as tres sao do tipo `char*` (ponteiro-para-`char`), portanto sao passadas para uma funcao como um argumento do tipo `char*`. O C++ usa o caractere nulo (`\0`) para terminar strings, e funcoes de string testam o caractere nulo para determinar o fim de qualquer string que estejam processando.

O C++ tambem fornece a classe `string` para representar strings. Uma funcao pode aceitar objetos `string` como argumentos e usar um objeto `string` como valor de retorno. O metodo `size()` da classe `string` pode ser usado para determinar o comprimento de uma string armazenada.

O C++ trata estruturas como tipos basicos, o que significa que voce pode passa-las por valor e usa-las como tipos de retorno de funcao. No entanto, se uma estrutura for grande, pode ser mais eficiente passar um ponteiro para a estrutura e deixar a funcao trabalhar com os dados originais. Essas mesmas consideracoes se aplicam a objetos de classe.

Uma funcao C++ pode ser recursiva; ou seja, o codigo de uma funcao especifica pode incluir uma chamada de si mesma.

O nome de uma funcao C++ age como o endereco da funcao. Ao usar um argumento de funcao que e um ponteiro para uma funcao, voce pode passar para uma funcao o nome de uma segunda funcao que voce quer que a primeira funcao evoque.

---

## Revisao do Capitulo

**1.** Quais sao os tres passos para usar uma funcao?

**2.** Construa prototipos de funcao que correspondam as seguintes descricoes:

```
a. igor() nao toma argumentos e nao tem valor de retorno.
b. tofu() toma um argumento int e retorna um float.
c. mpg() toma dois argumentos do tipo double e retorna um double.
d. summation() toma o nome de um array long e um tamanho de array como valores
   e retorna um valor long.
e. doctor() toma um argumento de string (a string nao deve ser modificada)
   e retorna um valor double.
f. ofcourse() toma uma estrutura boss como argumento e nao retorna nada.
g. plot() toma um ponteiro para uma estrutura map como argumento e retorna uma string.
```

**3.** Escreva uma funcao que tome tres argumentos: o nome de um array `int`, o tamanho do array e um valor `int`. Faca a funcao definir cada elemento do array para o valor `int`.

**4.** Escreva uma funcao que tome tres argumentos: um ponteiro para o primeiro elemento de um intervalo em um array, um ponteiro para o elemento seguinte ao fim de um intervalo em um array e um valor `int`. Faca a funcao definir cada elemento do array para o valor `int`.

**5.** Escreva uma funcao que tome um nome de array `double` e um tamanho de array como argumentos e retorne o maior valor naquele array. Observe que esta funcao nao deve alterar o conteudo do array.

**6.** Por que voce nao usa o qualificador `const` para argumentos de funcao que sao um dos tipos fundamentais?

**7.** Quais sao as tres formas que uma string no estilo C pode assumir em um programa C++?

**8.** Escreva uma funcao que tem este prototipo:

```cpp
int substituir(char * str, char c1, char c2);
```

Faca a funcao substituir cada ocorrencia de `c1` na string `str` por `c2`, e faca a funcao retornar o numero de substituicoes que ela faz.

**9.** O que a expressao `*"pizza"` significa? E quanto a `"taco"[2]`?

**10.** O C++ permite passar uma estrutura por valor, e permite passar o endereco de uma estrutura. Se `brilho` for uma variavel de estrutura, como voce passaria ela por valor? Como voce passaria seu endereco? Quais sao as vantagens e desvantagens das duas abordagens?

**11.** A funcao `juiz()` tem um tipo de retorno `int`. Como argumento, ela toma o endereco de uma funcao. A funcao cujo endereco e passado, por sua vez, toma um ponteiro para `char` const como argumento e retorna um `int`. Escreva o prototipo da funcao.

**12.** Suponha que tenhamos a seguinte declaracao de estrutura:

```cpp
struct candidato {
    char nome[30];
    int avaliacoes_credito[3];
};
```

a. Escreva uma funcao que toma uma estrutura `candidato` como argumento e exibe seu conteudo.
b. Escreva uma funcao que toma o endereco de uma estrutura `candidato` como argumento e exibe o conteudo da estrutura apontada.

**13.** Suponha que as funcoes `f1()` e `f2()` tenham os seguintes prototipos:

```cpp
void f1(candidato * a);
const char * f2(const candidato * a1, const candidato * a2);
```

Declare `p1` como um ponteiro que aponta para `f1` e `p2` como um ponteiro para `f2`. Declare `ap` como um array de cinco ponteiros do mesmo tipo que `p1`, e declare `pa` como um ponteiro para um array de dez ponteiros do mesmo tipo que `p2`. Use `typedef` como auxilio.

---

## Exercicios de Programacao

**1.** Escreva um programa que peca repetidamente ao usuario para inserir pares de numeros ate que pelo menos um do par seja 0. Para cada par, o programa deve usar uma funcao para calcular a media harmonica dos numeros. A funcao deve retornar a resposta para `main()`, que deve reportar o resultado. A media harmonica dos numeros e o inverso da media dos inversos e pode ser calculada da seguinte forma:

```
media harmonica = 2.0 * x * y / (x + y)
```

**2.** Escreva um programa que peca ao usuario para inserir ate 10 pontuacoes de golfe, que devem ser armazenadas em um array. Voce deve fornecer um meio para o usuario encerrar a entrada antes de inserir 10 pontuacoes. O programa deve exibir todas as pontuacoes em uma linha e relatar a pontuacao media. Lide com a entrada, exibicao e o calculo de media com tres funcoes de processamento de array separadas.

**3.** Aqui esta uma declaracao de estrutura:

```cpp
struct caixa
{
    char fabricante[40];
    float altura;
    float largura;
    float comprimento;
    float volume;
};
```

a. Escreva uma funcao que passa uma estrutura `caixa` por valor e que exibe o valor de cada membro.
b. Escreva uma funcao que passa o endereco de uma estrutura `caixa` e que define o membro `volume` para o produto das outras tres dimensoes.
c. Escreva um programa simples que usa essas duas funcoes.

**4.** Muitas loterias estaduais usam uma variacao da simples loteria retratada pela Listagem 7.4. Nessas variacoes, voce escolhe varios numeros de um conjunto e os chama de numeros de campo. Por exemplo, voce pode selecionar cinco numeros do campo 1-47. Voce tambem escolhe um unico numero (chamado de mega numero ou powerball, etc.) de um segundo intervalo, como 1-27. Para ganhar o grande premio, voce tem que adivinhar todos os picks corretamente. A chance de ganhar e o produto da probabilidade de escolher todos os numeros de campo corretamente vezes a probabilidade de escolher o mega numero. Modifique a Listagem 7.4 para calcular a probabilidade de ganhar neste tipo de loteria.

**5.** Defina uma funcao recursiva que toma um argumento inteiro e retorna o fatorial daquele argumento. Lembre-se de que 3 fatorial, escrito 3!, equivale a 3 * 2!, e assim por diante, com 0! definido como 1. Em geral, se n for maior que zero, n! = n * (n - 1)!. Teste sua funcao em um programa que usa um loop para permitir que o usuario insira varios valores para os quais o programa reporta o fatorial.

**6.** Escreva um programa que usa as seguintes funcoes:

`Preencher_array()` recebe como argumentos o nome de um array de valores `double` e um tamanho de array. Ela solicita ao usuario que insira valores `double` a serem inseridos no array. Ela cessa a entrada quando o array esta cheio ou quando o usuario insere entrada nao numerica, e retorna o numero real de entradas.

`Mostrar_array()` recebe como argumentos o nome de um array de valores `double` e um tamanho de array e exibe o conteudo do array.

`Inverter_array()` recebe como argumentos o nome de um array de valores `double` e um tamanho de array e inverte a ordem dos valores armazenados no array.

O programa deve usar essas funcoes para preencher um array, mostrar o array, inverter o array, mostrar o array, inverter todos menos o primeiro e o ultimo elemento do array e depois mostrar o array.

**7.** Refaca a Listagem 7.7, modificando as tres funcoes de manipulacao de array para cada uma usar dois parametros de ponteiro para representar um intervalo. A funcao `preencher_array()`, em vez de retornar o numero real de itens lidos, deve retornar um ponteiro para o local apos o ultimo local preenchido; as outras funcoes podem usar este ponteiro como o segundo argumento para identificar o fim dos dados.

**8.** Refaca a Listagem 7.15 sem usar a classe `array`. Faca duas versoes:

a. Use um array comum de `const char *` para as strings representando os nomes das estacoes e um array comum de `double` para as despesas.
b. Use um array comum de `const char *` para as strings representando os nomes das estacoes e uma estrutura cujo unico membro e um array comum de `double` para as despesas. (Este design e semelhante ao design basico da classe `array`.)

**9.** Este exercicio fornece pratica em escrever funcoes que lidam com arrays e estruturas. A seguir esta um esqueleto de programa. Complete-o fornecendo as funcoes descritas:

```cpp
#include <iostream>
using namespace std;
const int COMP = 30;
struct aluno {
    char nome_completo[COMP];
    char hobby[COMP];
    int nivel_poo;
};
// obter_info() tem dois argumentos: um ponteiro para o primeiro elemento de
// um array de estruturas aluno e um int representando o numero de elementos
// do array. A funcao solicita e armazena dados sobre os alunos. Ela encerra a
// entrada ao preencher o array ou ao encontrar uma linha em branco para o
// nome do aluno. A funcao retorna o numero real de elementos do array preenchidos.
int obter_info(aluno pa[], int n);
// exibir1() recebe uma estrutura aluno como argumento
// e exibe seu conteudo
void exibir1(aluno st);
// exibir2() recebe o endereco da estrutura aluno como
// argumento e exibe o conteudo da estrutura
void exibir2(const aluno * ps);
// exibir3() recebe o endereco do primeiro elemento de um array
// de estruturas aluno e o numero de elementos do array como
// argumentos e exibe o conteudo das estruturas
void exibir3(const aluno pa[], int n);
int main()
{
    cout << "Digite o tamanho da turma: ";
    int tamanho_turma;
    cin >> tamanho_turma;
    while (cin.get() != '\n')
        continue;
    aluno * ptr_alu = new aluno[tamanho_turma];
    int inseridos = obter_info(ptr_alu, tamanho_turma);
    for (int i = 0; i < inseridos; i++)
    {
        exibir1(ptr_alu[i]);
        exibir2(&ptr_alu[i]);
    }
    exibir3(ptr_alu, inseridos);
    delete [] ptr_alu;
    cout << "Concluido\n";
    return 0;
}
```

**10.** Projete uma funcao `calcular()` que recebe dois valores do tipo `double` e um ponteiro para uma funcao que recebe dois argumentos `double` e retorna um `double`. A funcao `calcular()` deve tambem ser do tipo `double`, e deve retornar o valor que a funcao apontada calcula, usando os argumentos `double` para calcular. Por exemplo, suponha que voce tenha esta definicao para a funcao `somar()`:

```cpp
double somar(double x, double y)
{
    return x + y;
}
```

Entao, a chamada de funcao a seguir faria `calcular()` passar os valores 2.5 e 10.4 para a funcao `somar()` e depois retornar o valor de retorno de `somar()` (12.9):

```cpp
double q = calcular(2.5, 10.4, somar);
```

Use essas funcoes e pelo menos uma funcao adicional no molde de `somar()` em um programa. O programa deve usar um loop que permita ao usuario inserir pares de numeros. Para cada par, use `calcular()` para invocar `somar()` e pelo menos uma outra funcao. Se voce se sentir aventureiro, tente criar um array de ponteiros para funcoes no estilo `somar()` e use um loop para aplicar `calcular()` sucessivamente a uma serie de funcoes usando esses ponteiros.

---

> Navegacao: [Anterior](capitulo-07-03-funcoes-strings-estruturas.md) | [Indice](README.md) | [Proximo](capitulo-08-01-funcoes-inline-referencias.md)
