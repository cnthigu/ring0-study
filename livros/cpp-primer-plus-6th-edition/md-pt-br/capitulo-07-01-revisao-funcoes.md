# Capitulo 7 - Funcoes: Modulos de Programacao do C++ (Functions: C++'s Programming Modules)
## Parte 1: Revisao de Funcoes e Passagem de Argumentos

> Traducao nao oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Navegacao: [Anterior](capitulo-06-06-arquivo-resumo.md) | [Indice](README.md) | [Proximo](capitulo-07-02-funcoes-arrays.md)

---

Neste capitulo voce aprendera sobre o seguinte:

- Basicos de funcoes
- Prototipos de funcoes
- Passagem de argumentos por valor
- Projeto de funcoes para processar arrays
- Uso de parametros de ponteiro `const`
- Projeto de funcoes para processar strings de texto
- Projeto de funcoes para processar estruturas
- Projeto de funcoes para processar objetos da classe `string`
- Funcoes que chamam a si mesmas (recursao)
- Ponteiros para funcoes

A diversao esta onde voce a encontra. Observe bem, e voce pode encontra-la nas funcoes. O C++ vem com uma grande biblioteca de funcoes uteis (a biblioteca padrao ANSI C mais diversas classes C++), mas o verdadeiro prazer de programar vem de escrever suas proprias funcoes. (Por outro lado, a verdadeira produtividade de programacao pode vir de aprender mais sobre o que voce pode fazer com o STL e as bibliotecas C++ BOOST.) Este capitulo e o Capitulo 8, "Aventuras em Funcoes", examinam como definir funcoes, transmitir informacoes a elas e recuperar informacoes delas. Apos revisar como as funcoes funcionam, este capitulo se concentra em como usar funcoes em conjunto com arrays, strings e estruturas. Finalmente, toca em recursao e ponteiros para funcoes. Se voce ja passou por C, encontrara grande parte deste capitulo familiar. Mas nao se deixe enganar por um falso senso de expertise. O C++ fez diversas adicoes ao que as funcoes C podem fazer, e o Capitulo 8 trata principalmente dessas. Enquanto isso, vamos abordar os fundamentos.

---

## Revisao de Funcoes

Vamos revisar o que voce ja viu sobre funcoes. Para usar uma funcao C++, voce deve:

- Fornecer uma definicao de funcao
- Fornecer um prototipo de funcao
- Chamar a funcao

Se voce estiver usando uma funcao de biblioteca, a funcao ja foi definida e compilada para voce. Alem disso, voce pode e deve usar um arquivo de cabecalho de biblioteca padrao para fornecer o prototipo. Tudo que resta e chamar a funcao corretamente. Os exemplos ate agora neste livro fizeram isso varias vezes. Por exemplo, a biblioteca C padrao inclui a funcao `strlen()` para encontrar o comprimento de uma string. O arquivo de cabecalho padrao associado `cstring` contem o prototipo de funcao para `strlen()` e diversas outras funcoes relacionadas a strings. Esse trabalho previo permite que voce use a funcao `strlen()` em programas sem mais preocupacoes.

Mas quando voce cria suas proprias funcoes, voce tem que lidar com todos os tres aspectos — definir, prototipar e chamar — voce mesmo. A Listagem 7.1 mostra essas etapas em um exemplo curto.

**Listagem 7.1 — calling.cpp**

```cpp
// calling.cpp -- definindo, prototipando e chamando uma funcao
#include <iostream>

void simples(); // prototipo de funcao

int main()
{
    using namespace std;
    cout << "main() ira chamar a funcao simples():\n";
    simples(); // chamada de funcao
    cout << "main() terminou com a funcao simples().\n";
    return 0;
}
// definicao de funcao
void simples()
{
    using namespace std;
    cout << "Sou uma funcao simples.\n";
}
```

Aqui esta a saida do programa na Listagem 7.1:

```
main() ira chamar a funcao simples():
Sou uma funcao simples.
main() terminou com a funcao simples().
```

A execucao do programa em `main()` e interrompida enquanto o controle e transferido para a funcao `simples()`. Quando `simples()` termina, a execucao do programa em `main()` e retomada. Este exemplo coloca uma diretiva `using` dentro de cada definicao de funcao porque cada funcao usa `cout`. Como alternativa, o programa poderia ter uma unica diretiva `using` colocada acima das definicoes de funcao ou de outra forma usar `std::cout`.

Vamos agora dar uma olhada mais detalhada nessas etapas.

---

## Definindo uma Funcao

Voce pode agrupar funcoes em duas categorias: aquelas que nao tem valores de retorno e aquelas que tem. Funcoes sem valores de retorno sao chamadas de **funcoes void** (void functions) e tem a seguinte forma geral:

```
void nomeDaFuncao(listaDeParametros)
{
    instrucao(oes)
    return;   // opcional
}
```

Aqui, `listaDeParametros` especifica os tipos e o numero de argumentos (parametros) passados para a funcao. A instrucao `return` opcional marca o fim da funcao. Caso contrario, a funcao termina na chave de fechamento. Funcoes do tipo `void` correspondem a procedimentos do Pascal, sub-rotinas do FORTRAN e procedimentos de subprograma do BASIC moderno. Normalmente, voce usa uma funcao `void` para executar algum tipo de acao. Por exemplo, uma funcao para imprimir `Cheers!` um determinado numero (n) de vezes poderia ter esta aparencia:

```cpp
void hurras(int n) // sem valor de retorno
{
    for (int i = 0; i < n; i++)
        std::cout << "Hurra! ";
    std::cout << std::endl;
}
```

A lista de parametros `int n` significa que `hurras()` espera ter um valor `int` passado a ela como argumento quando voce chama esta funcao.

Uma funcao com um valor de retorno produz um valor que ela retorna para a funcao que a chamou. Em outras palavras, se a funcao retorna a raiz quadrada de 9.0 (`sqrt(9.0)`), a chamada de funcao tem o valor 3.0. Tal funcao e declarada como tendo o mesmo tipo que o valor que ela retorna. Aqui esta a forma geral:

```
nomeDoTipo nomeDaFuncao(listaDeParametros)
{
    instrucoes
    return valor; // valor e convertido para o tipo nomeDoTipo
}
```

Funcoes com valores de retorno exigem que voce use uma instrucao `return` para que o valor seja retornado para a funcao chamadora. O valor em si pode ser uma constante, uma variavel ou uma expressao mais geral. O unico requisito e que a expressao seja reduzida a um valor que tenha, ou seja conversivel para, o tipo `nomeDoTipo`. O C++ coloca uma restricao no que voce pode usar como valor de retorno: o valor de retorno nao pode ser um array. Todo o resto e possivel — inteiros, numeros de ponto flutuante, ponteiros e ate mesmo estruturas e objetos!

Aqui esta um exemplo simples que retorna o cubo de um valor do tipo `double`:

```cpp
double cubo(double x) // x vezes x vezes x
{
    return x * x * x; // um valor do tipo double
}
```

Por exemplo, a chamada de funcao `cubo(1.2)` retorna o valor 1.728. Observe que esta instrucao `return` usa uma expressao. A funcao calcula o valor da expressao (1.728, neste caso) e retorna o valor.

---

## Prototipando e Chamando uma Funcao

Agora voce ja esta familiarizado com a realizacao de chamadas de funcao, mas pode estar menos confortavel com a prototipagem de funcoes porque isso frequentemente estava oculto nos arquivos de inclusao. A Listagem 7.2 mostra as funcoes `hurras()` e `cubo()` usadas em um programa; observe os prototipos de funcao.

**Listagem 7.2 — protos.cpp**

```cpp
// protos.cpp -- usando prototipos e chamadas de funcao
#include <iostream>
void hurras(int);        // prototipo: sem valor de retorno
double cubo(double x);  // prototipo: retorna um double
int main()
{
    using namespace std;
    hurras(5);  // chamada de funcao
    cout << "Me de um numero: ";
    double lado;
    cin >> lado;
    double volume = cubo(lado); // chamada de funcao
    cout << "Um cubo de " << lado << " pes tem um volume de ";
    cout << volume << " pes cubicos.\n";
    hurras(cubo(2)); // protecao do prototipo em acao
    return 0;
}
void hurras(int n)
{
    using namespace std;
    for (int i = 0; i < n; i++)
        cout << "Hurra! ";
    cout << endl;
}
double cubo(double x)
{
    return x * x * x;
}
```

Aqui esta uma execucao de exemplo:

```
Hurra! Hurra! Hurra! Hurra! Hurra!
Me de um numero: 5
Um cubo de 5 pes tem um volume de 125 pes cubicos.
Hurra! Hurra! Hurra! Hurra! Hurra! Hurra! Hurra! Hurra!
```

Observe que `main()` chama a funcao de tipo `void` `hurras()` usando o nome da funcao e os argumentos seguidos de um ponto-e-virgula: `hurras(5);`. Porque `cubo()` tem um valor de retorno, `main()` pode usa-lo como parte de uma instrucao de atribuicao:

```cpp
double volume = cubo(lado);
```

### Por que Prototipos?

Um **prototipo** (prototype) descreve a interface da funcao para o compilador. Ou seja, ele informa ao compilador que tipo de valor de retorno, se houver, a funcao tem, e informa ao compilador o numero e o tipo dos argumentos da funcao. Considere como o prototipo afeta esta chamada de funcao da Listagem 7.2:

```cpp
double volume = cubo(lado);
```

Primeiro, o prototipo informa ao compilador que `cubo()` deve ter um argumento do tipo `double`. Se o programa nao fornecer o argumento, a prototipagem permite ao compilador detectar o erro. Segundo, quando a funcao `cubo()` termina seu calculo, ela coloca seu valor de retorno em algum local especificado — talvez em um registrador da CPU, talvez na memoria. Entao a funcao chamadora, `main()` neste caso, recupera o valor desse local. Como o prototipo afirma que `cubo()` e do tipo `double`, o compilador sabe quantos bytes recuperar e como interpreta-los.

Ainda assim, voce pode se perguntar: por que o compilador precisa de um prototipo? Nao pode simplesmente olhar mais adiante no arquivo e ver como as funcoes sao definidas? Um problema com essa abordagem e que ela nao e muito eficiente. O compilador teria que colocar a compilacao de `main()` em espera enquanto pesquisa o resto do arquivo. Um problema ainda mais serio e o fato de que a funcao pode nao estar no arquivo. O C++ permite que voce espalhe um programa por varios arquivos, que voce pode compilar independentemente e depois combinar. Nesse caso, o compilador pode nao ter acesso ao codigo da funcao quando estiver compilando `main()`. O mesmo e verdade se a funcao fizer parte de uma biblioteca. A unica maneira de evitar usar um prototipo de funcao e colocar a definicao da funcao antes de seu primeiro uso.

### Sintaxe do Prototipo

Um prototipo de funcao e uma instrucao, portanto deve ter um ponto-e-virgula de terminacao. A maneira mais simples de obter um prototipo e copiar o cabecalho da funcao da definicao da funcao e adicionar um ponto-e-virgula. E o que o programa na Listagem 7.2 faz para `cubo()`:

```cpp
double cubo(double x); // adiciona ; ao cabecalho para obter o prototipo
```

No entanto, o prototipo de funcao nao requer que voce forneca nomes para as variaveis; uma lista de tipos e suficiente. O programa na Listagem 7.2 protitipa `hurras()` usando apenas o tipo do argumento:

```cpp
void hurras(int); // ok omitir nomes de variaveis no prototipo
```

Em geral, voce pode incluir ou excluir nomes de variaveis nas listas de argumentos para prototipos. Os nomes de variaveis no prototipo atuam apenas como placeholders, portanto, se voce usar nomes, eles nao precisam corresponder aos nomes na definicao da funcao.

> **C++ Versus Prototipagem ANSI C**
>
> O ANSI C tomou emprestada a prototipagem do C++, mas as duas linguagens tem algumas diferencas. A mais importante e que o ANSI C, para preservar a compatibilidade com o C classico, tornou a prototipagem opcional, enquanto o C++ torna a prototipagem obrigatoria. Por exemplo, considere a seguinte declaracao de funcao:
>
> ```cpp
> void dizer_oi();
> ```
>
> Em C++, deixar os parenteses vazios e o mesmo que usar a palavra-chave `void` dentro dos parenteses. Isso significa que a funcao nao tem argumentos. Em ANSI C, deixar os parenteses vazios significa que voce esta declinando a informar quais sao os argumentos. O equivalente em C++ para nao identificar a lista de argumentos e usar uma elipse:
>
> ```cpp
> void dizer_tchau(...); // abdicacao de responsabilidade em C++
> ```
>
> Normalmente, esse uso de elipse e necessario apenas para interfacear com funcoes C que tem um numero variavel de argumentos, como `printf()`.

### O que os Prototipos Fazem por Voce

Voce ja viu que os prototipos ajudam o compilador. Mas o que eles fazem por voce? Eles reduzem muito as chances de erros de programa. Em particular, os prototipos garantem o seguinte:

- O compilador lida corretamente com o valor de retorno da funcao.
- O compilador verifica se voce usa o numero correto de argumentos de funcao.
- O compilador verifica se voce usa o tipo correto de argumentos. Se voce nao usa, ele converte os argumentos para o tipo correto, se possivel.

A prototipagem resulta em conversao de tipo apenas quando faz sentido. Por exemplo, na Listagem 7.2:

```cpp
hurras(cubo(2));
```

Primeiro, o programa passa o valor `int` de `2` para `cubo()`, que espera o tipo `double`. O compilador, notando que o prototipo de `cubo()` especifica um argumento do tipo `double`, converte `2` em `2.0`, um valor do tipo `double`. Entao `cubo()` retorna um valor do tipo `double` (8.0) para ser usado como argumento para `hurras()`. Novamente, o compilador verifica os prototipos e nota que `hurras()` requer um `int`. Ele converte o valor de retorno para o inteiro `8`.

A prototipagem ocorre durante o tempo de compilacao e e chamada de **verificacao de tipo estatica** (static type checking). A verificacao de tipo estatica, como voce acabou de ver, captura muitos erros que sao muito mais dificeis de capturar durante o tempo de execucao.

---

## Argumentos de Funcao e Passagem por Valor

E hora de dar uma olhada mais detalhada nos argumentos de funcao. O C++ normalmente passa argumentos **por valor** (by value). Isso significa que o valor numerico do argumento e passado para a funcao, onde e atribuido a uma nova variavel. Por exemplo, a Listagem 7.2 tem esta chamada de funcao:

```cpp
double volume = cubo(lado);
```

Aqui, `lado` e uma variavel que, na execucao de exemplo, tinha o valor 5. O cabecalho da funcao para `cubo()`, lembre-se, era este:

```cpp
double cubo(double x)
```

Quando esta funcao e chamada, ela cria uma nova variavel do tipo `double` chamada `x` e a inicializa com o valor 5. Isso isola os dados em `main()` das acoes que ocorrem em `cubo()` porque `cubo()` trabalha com uma copia de `lado` em vez de com os dados originais. Uma variavel usada para receber valores passados e chamada de **argumento formal** (formal argument) ou **parametro formal** (formal parameter). O valor passado para a funcao e chamado de **argumento real** (actual argument) ou **parametro real** (actual parameter).

Variaveis, incluindo parametros, declaradas dentro de uma funcao sao privadas da funcao. Quando uma funcao e chamada, o computador aloca a memoria necessaria para essas variaveis. Quando a funcao termina, o computador libera a memoria que foi usada para essas variaveis. Tais variaveis sao chamadas de **variaveis locais** (local variables) porque estao localizadas na funcao. Isso ajuda a preservar a integridade dos dados. Isso tambem significa que se voce declarar uma variavel chamada `x` em `main()` e outra variavel chamada `x` em alguma outra funcao, essas sao duas variaveis distintas e nao relacionadas. Tais variaveis tambem sao chamadas de **variaveis automaticas** (automatic variables) porque sao alocadas e desalocadas automaticamente durante a execucao do programa.

### Multiplos Argumentos

Uma funcao pode ter mais de um argumento. Na chamada de funcao, voce apenas separa os argumentos com virgulas:

```cpp
n_chars('R', 25);
```

Isso passa dois argumentos para a funcao `n_chars()`, que sera definida em breve. Da mesma forma, ao definir a funcao, voce usa uma lista de declaracoes de parametros separadas por virgulas no cabecalho da funcao:

```cpp
void n_chars(char c, int n) // dois argumentos
```

Se uma funcao tem dois parametros do mesmo tipo, voce deve fornecer o tipo de cada parametro separadamente. Voce nao pode combinar declaracoes da forma que voce pode quando declara variaveis regulares:

```cpp
void fifi(float a, float b) // declare cada variavel separadamente
void fufu(float a, b)       // NAO aceitavel
```

A Listagem 7.3 mostra um exemplo de uma funcao com dois argumentos. Ela tambem ilustra como mudar o valor de um parametro formal em uma funcao nao tem efeito sobre os dados no programa chamador.

**Listagem 7.3 — twoarg.cpp**

```cpp
// twoarg.cpp -- uma funcao com 2 argumentos
#include <iostream>
using namespace std;
void n_chars(char, int);
int main()
{
    int vezes;
    char ch;
    cout << "Digite um caractere: ";
    cin >> ch;
    while (ch != 'q') // q para sair
    {
        cout << "Digite um inteiro: ";
        cin >> vezes;
        n_chars(ch, vezes); // funcao com dois argumentos
        cout << "\nDigite outro caractere ou pressione a"
                " tecla q para sair: ";
        cin >> ch;
    }
    cout << "O valor de vezes e " << vezes << ".\n";
    cout << "Ate mais\n";
    return 0;
}
void n_chars(char c, int n) // exibe c n vezes
{
    while (n-- > 0) // continua ate n chegar a 0
        cout << c;
}
```

Aqui esta uma execucao de exemplo do programa na Listagem 7.3:

```
Digite um caractere: W
Digite um inteiro: 50
WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW
Digite outro caractere ou pressione a tecla q para sair: a
Digite um inteiro: 20
aaaaaaaaaaaaaaaaaaaa
Digite outro caractere ou pressione a tecla q para sair: q
O valor de vezes e 20.
Ate mais
```

A funcao `n_chars()` usa um loop para exibir o caractere o numero de vezes que o inteiro especifica. Observe que o programa mantem a contagem decrementando a variavel `n`, onde `n` e o parametro formal da lista de argumentos. Esta variavel e atribuida o valor da variavel `vezes` em `main()`. O loop `while` entao diminui `n` para 0, mas, como a execucao de exemplo demonstra, mudar o valor de `n` nao tem efeito sobre `vezes`. Mesmo se voce usar o nome `n` em vez de `vezes` em `main()`, o valor de `n` em `main()` nao e afetado pelas alteracoes no valor de `n` em `n_chars()`.

### Outra Funcao com Dois Argumentos

Vamos criar uma funcao mais ambiciosa — uma que executa um calculo nao trivial. Muitos estados nos EUA patrocinam um jogo de loteria com alguma forma de Lotto. O Lotto permite que voce escolha um certo numero de selecoes de um cartao. Por exemplo, voce pode escolher seis numeros de um cartao com 51 numeros. A nossa funcao calculara a probabilidade de voce ter uma selecao ganhadora.

Primeiro, voce precisa de uma formula. Se voce tem que escolher seis valores de 51, matematica diz que voce tem uma chance em R de ganhar, onde a seguinte formula da R:

```
    51 × 50 × 49 × 48 × 47 × 46
R = ─────────────────────────────
         6 × 5 × 4 × 3 × 2 × 1
```

Para seis selecoes, o denominador e o produto dos primeiros seis inteiros, ou fatorial de 6. O numerador tambem e o produto de seis numeros consecutivos, desta vez comecando com 51 e descendo. Mais geralmente, se voce escolhe `selecoes` valores de `numeros` numeros, o denominador e o fatorial de `selecoes` e o numerador e o produto de `selecoes` inteiros, comecando com o valor `numeros` e descendo. Voce pode usar um loop `for` para fazer esse calculo:

```cpp
long double resultado = 1.0;
for (n = numeros, p = selecoes; p > 0; n--, p--)
    resultado = resultado * n / p;
```

Em vez de multiplicar todos os termos do numerador primeiro, o loop comeca multiplicando `1.0` pelo primeiro termo do numerador e depois dividindo pelo primeiro termo do denominador. Isso mantem o produto acumulado menor do que se voce fizesse toda a multiplicacao primeiro.

A Listagem 7.4 incorpora esta formula em uma funcao `probabilidade()`. Como o numero de selecoes e o numero total de escolhas devem ser valores positivos, o programa usa o tipo `unsigned int` (ou simplesmente `unsigned`) para essas quantidades. Multiplicar varios inteiros pode produzir resultados bastante grandes, portanto `lotto.cpp` usa o tipo `long double` para o valor de retorno da funcao.

**Listagem 7.4 — lotto.cpp**

```cpp
// lotto.cpp -- probabilidade de ganhar
#include <iostream>
// Nota: algumas implementacoes requerem double em vez de long double
long double probabilidade(unsigned numeros, unsigned selecoes);
int main()
{
    using namespace std;
    double total, escolhas;
    cout << "Digite o numero total de escolhas no cartao do jogo e\n"
            "o numero de selecoes permitidas:\n";
    while ((cin >> total >> escolhas) && escolhas <= total)
    {
        cout << "Voce tem uma chance em ";
        cout << probabilidade(total, escolhas); // calcular as probabilidades
        cout << " de ganhar.\n";
        cout << "Proximos dois numeros (q para sair): ";
    }
    cout << "tchau\n";
    return 0;
}
// a funcao a seguir calcula a probabilidade de selecionar
// selecoes numeros corretamente de numeros escolhas
long double probabilidade(unsigned numeros, unsigned selecoes)
{
    long double resultado = 1.0; // aqui vem algumas variaveis locais
    long double n;
    unsigned p;
    for (n = numeros, p = selecoes; p > 0; n--, p--)
        resultado = resultado * n / p;
    return resultado;
}
```

Aqui esta uma execucao de exemplo do programa na Listagem 7.4:

```
Digite o numero total de escolhas no cartao do jogo e
o numero de selecoes permitidas:
49 6
Voce tem uma chance em 1.39838e+007 de ganhar.
Proximos dois numeros (q para sair): 51 6
Voce tem uma chance em 1.80095e+007 de ganhar.
Proximos dois numeros (q para sair): 38 6
Voce tem uma chance em 2.76068e+006 de ganhar.
Proximos dois numeros (q para sair): q
tchau
```

A funcao `probabilidade()` na Listagem 7.4 ilustra dois tipos de variaveis locais que voce pode ter em uma funcao. Primeiro, ha os parametros formais (`numeros` e `selecoes`), que sao declarados no cabecalho da funcao antes da chave de abertura. Entao vem as outras variaveis locais (`resultado`, `n` e `p`). Elas sao declaradas entre as chaves que delimitam a definicao da funcao. A principal diferenca entre os parametros formais e as outras variaveis locais e que os parametros formais recebem seus valores da funcao que chama `probabilidade()`, enquanto as outras variaveis recebem valores de dentro da funcao.

---

> Navegacao: [Anterior](capitulo-06-06-arquivo-resumo.md) | [Indice](README.md) | [Proximo](capitulo-07-02-funcoes-arrays.md)
