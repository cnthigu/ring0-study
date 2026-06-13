# Capítulo 8 — Argumentos Padrão e Sobrecarga de Funções

> Tradução não oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Navegação: [Anterior](capitulo-08-01-funcoes-inline-referencias.md) | [Índice](README.md) | [Próximo](capitulo-08-03-templates-resumo.md)

## Argumentos Padrão (Default Arguments)

Vamos examinar outro tópico do repertório de novos truques do C++: o argumento padrão (default argument). Um *argumento padrão* é um valor que é usado automaticamente se você omitir o argumento real correspondente de uma chamada de função. Por exemplo, se você configurar a função `void uau(int n)` de forma que `n` tenha um valor padrão de `1`, a chamada de função `uau()` é equivalente a `uau(1)`. Isso lhe dá flexibilidade na forma como você usa uma função.

Suponha que você tenha uma função chamada `esquerda()` que retorna os primeiros `n` caracteres de uma string, com a string e `n` como argumentos. Mais precisamente, a função retorna um ponteiro para uma nova string consistindo da porção selecionada da string original. Por exemplo, a chamada `esquerda("teoria", 3)` constrói uma nova string `"teo"` e retorna um ponteiro para ela. Agora suponha que você estabeleça um valor padrão de `1` para o segundo argumento. A chamada `esquerda("teoria", 3)` funcionaria como antes, com sua escolha de `3` substituindo o padrão. Mas a chamada `esquerda("teoria")`, em vez de ser um erro, assumiria um segundo argumento de `1` e retornaria um ponteiro para a string `"t"`. Esse tipo de padrão é útil quando seu programa frequentemente precisa extrair uma string de um caractere, mas ocasionalmente precisa extrair strings mais longas.

Como você estabelece um valor padrão? Você deve usar o protótipo da função. Como o compilador examina o protótipo para ver quantos argumentos uma função usa, o protótipo da função também precisa alertar o programa para a possibilidade de argumentos padrão. O método é atribuir um valor ao argumento no protótipo. Por exemplo, aqui está o protótipo que se encaixa nesta descrição de `esquerda()`:

```cpp
char * esquerda(const char * str, int n = 1);
```

Você quer que a função retorne uma nova string, então seu tipo é `char*` (ponteiro-para-char). Você quer deixar a string original inalterada, então usa o qualificador `const` para o primeiro argumento. Você quer que `n` tenha um valor padrão de `1`, então atribui esse valor a `n`. Um valor de argumento padrão é um valor de inicialização. Assim, o protótipo anterior inicializa `n` com o valor `1`. Se você deixar `n` sozinho, ele terá o valor `1`, mas se você passar um argumento, o novo valor substitui o `1`.

Quando você usa uma função com uma lista de argumentos, você deve adicionar padrões da direita para a esquerda. Ou seja, você não pode fornecer um valor padrão para um argumento particular, a menos que também forneça padrões para todos os argumentos à sua direita:

```cpp
int harpo(int n, int m = 4, int j = 5); // VÁLIDO
int chico(int n, int m = 6, int j);     // INVÁLIDO
int groucho(int k = 1, int m = 2, int n = 3); // VÁLIDO
```

Por exemplo, o protótipo `harpo()` permite chamadas com um, dois ou três argumentos:

```cpp
bipes = harpo(2);       // mesmo que harpo(2,4,5)
bipes = harpo(1,8);     // mesmo que harpo(1,8,5)
bipes = harpo(8,7,6);   // nenhum argumento padrão usado
```

Os argumentos reais são atribuídos aos argumentos formais correspondentes da esquerda para a direita; você não pode pular argumentos. Assim, o seguinte não é permitido:

```cpp
bipes = harpo(3, ,8); // inválido, não define m como 4
```

Argumentos padrão não são um grande avanço na programação; em vez disso, são uma conveniência. Quando você começar a trabalhar com design de classes, descobrirá que eles podem reduzir o número de construtores, métodos e sobrecargas de método que você precisa definir.

A Listagem 8.9 coloca argumentos padrão em uso. Observe que apenas o protótipo indica o padrão. A definição da função é a mesma que seria sem argumentos padrão.

**Listagem 8.9** left.cpp

```cpp
// left.cpp -- função de string com argumento padrão
#include <iostream>
const int TamStr = 80;
char * esquerda(const char * str, int n = 1);
int main()
{
    using namespace std;
    char amostra[TamStr];
    cout << "Digite uma string:\n";
    cin.get(amostra, TamStr);
    char * ps = esquerda(amostra, 4);
    cout << ps << endl;
    delete [] ps; // libera string antiga
    ps = esquerda(amostra);
    cout << ps << endl;
    delete [] ps; // libera nova string
    return 0;
}
// Esta função retorna um ponteiro para uma nova string
// consistindo dos primeiros n caracteres da string str.
char * esquerda(const char * str, int n)
{
    if (n < 0)
        n = 0;
    char * p = new char[n + 1];
    int i;
    for (i = 0; i < n && str[i]; i++)
        p[i] = str[i]; // copia caracteres
    while (i <= n)
        p[i++] = '\0'; // define resto da string como '\0'
    return p;
}
```

Execução de exemplo:

```
Digite uma string:
forthcoming
fort
f
```

**Notas do Programa**

O programa na Listagem 8.9 usa `new` para criar uma nova string para conter os caracteres selecionados. Uma possibilidade incomum é que um usuário não cooperativo possa solicitar um número negativo de caracteres. Nesse caso, a função define a contagem de caracteres como `0` e eventualmente retorna a string nula. Outra possibilidade incômoda é que um usuário irresponsável possa solicitar mais caracteres do que a string contém. A função se protege contra isso usando um teste combinado:

```cpp
i < n && str[i]
```

O teste `i < n` para o loop depois de `n` caracteres terem sido copiados. A segunda parte do teste, a expressão `str[i]`, é o código do caractere prestes a ser copiado. Se o loop chegar ao caractere nulo, o código é `0`, e o loop termina. O loop `while` final termina a string com o caractere nulo e então define o resto do espaço alocado, se houver, como caracteres nulos.

Outra abordagem para definir o tamanho da nova string é definir `n` como o menor entre o valor passado e o comprimento da string:

```cpp
int len = strlen(str);
n = (n < len) ? n : len; // o menor entre n e len
char * p = new char[n + 1];
```

Isso garante que `new` não aloque mais espaço do que o necessário para guardar a string. A tradição do C++ dá maior peso à confiabilidade. Afinal, um programa mais lento que funciona corretamente é melhor do que um programa rápido que funciona incorretamente.

## Sobrecarga de Funções (Function Overloading)

Polimorfismo de função (function polymorphism) é uma boa adição do C++ às capacidades do C. Enquanto argumentos padrão permitem que você chame a mesma função usando números variados de argumentos, o polimorfismo de função, também chamado de *sobrecarga de função* (function overloading), permite que você use múltiplas funções compartilhando o mesmo nome. A palavra *polimorfismo* significa ter muitas formas, então o polimorfismo de função permite que uma função tenha muitas formas. Da mesma forma, a expressão *sobrecarga de função* significa que você pode anexar mais de uma função ao mesmo nome, sobrecarregando assim o nome. Você pode usar a sobrecarga de função para projetar uma família de funções que fazem essencialmente a mesma coisa, mas usando diferentes listas de argumentos.

A chave para a sobrecarga de função é a lista de argumentos de uma função, também chamada de *assinatura da função* (function signature). Se duas funções usam o mesmo número e tipos de argumentos na mesma ordem, elas têm a mesma assinatura; os nomes das variáveis não importam. O C++ permite que você defina duas funções com o mesmo nome, desde que as funções tenham assinaturas diferentes. A assinatura pode diferir no número de argumentos ou no tipo de argumentos, ou ambos. Por exemplo, você pode definir um conjunto de funções `imprimir()` com os seguintes protótipos:

```cpp
void imprimir(const char * str, int largura); // #1
void imprimir(double d, int largura);         // #2
void imprimir(long l, int largura);           // #3
void imprimir(int i, int largura);            // #4
void imprimir(const char * str);              // #5
```

Quando você usa uma função `imprimir()`, o compilador combina seu uso com o protótipo que tem a mesma assinatura:

```cpp
imprimir("Panquecas", 15); // usa #1
imprimir("Xarope");        // usa #5
imprimir(1999.0, 10);      // usa #2
imprimir(1999, 12);        // usa #4
imprimir(1999L, 15);       // usa #3
```

Quando você usa funções sobrecarregadas, precisa ter certeza de usar os tipos de argumento adequados na chamada de função. Por exemplo, considere as seguintes instruções:

```cpp
unsigned int ano = 3210;
imprimir(ano, 6); // chamada ambígua
```

Qual protótipo a chamada de `imprimir()` combina aqui? Não combina com nenhum! A falta de um protótipo correspondente não descarta automaticamente o uso de uma das funções porque o C++ tentará usar conversões de tipo padrão para forçar uma correspondência. Se, digamos, o único protótipo de `imprimir()` fosse `#2`, a chamada de função `imprimir(ano, 6)` converteria o valor `ano` para o tipo `double`. Mas no código anterior há três protótipos que recebem um número como primeiro argumento, fornecendo três escolhas diferentes para converter `ano`. Diante dessa situação ambígua, o C++ rejeita a chamada de função como um erro.

Algumas assinaturas que parecem ser diferentes umas das outras, no entanto, não podem coexistir. Por exemplo, considere estes dois protótipos:

```cpp
double cubo(double x);
double cubo(double & x);
```

Você pode pensar que este é um lugar onde poderia usar sobrecarga de função porque as assinaturas das funções parecem ser diferentes. Mas considere as coisas do ponto de vista do compilador. Suponha que você tem código como este:

```cpp
cout << cubo(x);
```

O argumento `x` corresponde tanto ao protótipo `double x` quanto ao protótipo `double &x`. O compilador não tem como saber qual função usar. Portanto, para evitar tal confusão, ao verificar assinaturas de função, o compilador considera uma referência a um tipo e o próprio tipo como a mesma assinatura.

O processo de correspondência de funções discrimina entre variáveis `const` e não-`const`. Considere os seguintes protótipos:

```cpp
void gotejar(char * bits);       // sobrecarregado
void gotejar(const char * cbits); // sobrecarregado
void balançar(char * bits);       // não sobrecarregado
void babar(const char * bits);    // não sobrecarregado
```

Aqui está o que várias chamadas de função combinariam:

```cpp
const char p1[20] = "Como está o tempo?";
char p2[20] = "Como vão os negócios?";
gotejar(p1); // gotejar(const char *);
gotejar(p2); // gotejar(char *);
balançar(p1); // sem correspondência
balançar(p2); // balançar(char *);
babar(p1);   // babar(const char *);
babar(p2);   // babar(const char *);
```

A função `gotejar()` tem dois protótipos — um para ponteiros `const` e outro para ponteiros regulares — e o compilador seleciona um ou outro, dependendo se o argumento real é `const`. A função `balançar()` só combina com uma chamada com argumento não-`const`, mas a função `babar()` combina com chamadas com argumentos `const` ou não-`const`. A razão para essa diferença de comportamento entre `babar()` e `balançar()` é que é válido atribuir um valor não-`const` a uma variável `const`, mas não o contrário.

Tenha em mente que a assinatura, não o tipo da função, é o que habilita a sobrecarga de função. Por exemplo, as seguintes duas declarações são incompatíveis:

```cpp
long gronk(int n, float m);    // mesmas assinaturas,
double gronk(int n, float m);  // portanto não permitido
```

Portanto, o C++ não permite que você sobrecarregue `gronk()` dessa forma. Você pode ter diferentes tipos de retorno, mas somente se as assinaturas também forem diferentes:

```cpp
long gronk(int n, float m);    // assinaturas diferentes,
double gronk(float n, float m); // portanto permitido
```

> **O Que é Decoração de Nome (Name Decoration)?**
>
> Como o C++ mantém o controle de qual função sobrecarregada é qual? Ele atribui uma identidade secreta a cada uma dessas funções. Quando você usa o editor da sua ferramenta de desenvolvimento C++ para escrever e compilar programas, o compilador C++ executa um pouco de magia em seu nome — conhecida como *decoração de nome* (name decoration) ou *name mangling* — por meio da qual cada nome de função é criptografado, com base nos tipos de parâmetros formais especificados no protótipo da função. Considere o seguinte protótipo de função não decorado:
>
> ```cpp
> long MinhaFuncaoFoo(int, float);
> ```
>
> Este formato é adequado para nós humanos; sabemos que a função aceita dois argumentos do tipo `int` e `float` e retorna um valor do tipo `long`. Para seu próprio uso, o compilador documenta essa interface transformando o nome em uma representação interna com uma aparência mais desagradável, talvez algo como isto:
>
> ```
> ?MinhaFuncaoFoo@@YAXH
> ```
>
> O aparente jargão que decora o nome original codifica o número e os tipos de parâmetros. Uma assinatura de função diferente resultaria em um conjunto diferente de símbolos sendo adicionados, e diferentes compiladores usariam diferentes convenções para seus esforços de decoração.

### Sobrecarregando Parâmetros de Referência (Overloading Reference Parameters)

Designs de classes e a STL frequentemente usam parâmetros de referência, e é útil saber como a sobrecarga funciona com diferentes tipos de referência. Considere os seguintes três protótipos:

```cpp
void afundar(double & r1);       // combina com lvalue modificável
void afundou(const double & r2); // combina com lvalue modificável ou const, e rvalue
void afundado(double && r3);     // combina com rvalue
```

O parâmetro de referência lvalue `r1` combina com um argumento lvalue modificável, como uma variável `double`. O parâmetro de referência lvalue `const` `r2` combina com um argumento lvalue modificável, um argumento lvalue `const` e um argumento rvalue. Finalmente, a referência rvalue `r3` combina com um rvalue. Observe como `r2` pode combinar com o mesmo tipo de argumentos que `r1` e `r3` combinam. Isso levanta a questão do que acontece quando você sobrecarrega uma função nesses três tipos de parâmetros. A resposta é que a correspondência mais exata é feita:

```cpp
void fogao(double & r1);       // combina com lvalue modificável
void fogao(const double & r2); // combina com rvalue, lvalue const
void forno(double & r1);       // combina com lvalue modificável
void forno(const double & r2); // combina com lvalue const
void forno(double && r3);      // combina com rvalue
```

Isso permite que você personalize o comportamento de uma função com base na natureza lvalue, `const` ou rvalue do argumento:

```cpp
double x = 55.5;
const double y = 32.0;
forno(x);    // chama forno(double &)
forno(y);    // chama forno(const double &)
forno(x+y);  // chama forno(double &&)
```

Se, digamos, você omitir a função `forno(double &&)`, então `forno(x+y)` chamará a função `forno(const double &)` em vez disso.

### Um Exemplo de Sobrecarga (An Overloading Example)

Neste capítulo, já desenvolvemos uma função `esquerda()` que retorna um ponteiro para os primeiros `n` caracteres em uma string. Vamos adicionar uma segunda função `esquerda()`, uma que retorna os primeiros `n` dígitos em um inteiro. Você pode usá-la, por exemplo, para examinar os três primeiros dígitos de um código postal dos EUA armazenado como um inteiro, o que é útil se você quiser ordenar por áreas urbanas.

A função inteira é um pouco mais difícil de programar do que a versão de string porque você não tem o benefício de cada dígito ser armazenado em seu próprio elemento de array. Uma abordagem é primeiro calcular o número de dígitos no número. Dividir um número por `10` remove um dígito, então você pode usar a divisão para contar dígitos. Mais precisamente, você pode fazer isso com um loop, como este:

```cpp
unsigned digitos = 1;
while (n /= 10)
    digitos++;
```

Esse loop conta quantas vezes você pode remover um dígito de `n` até que não sobrem mais. Se `n` é `238`, o primeiro ciclo define `n` como `238 / 10`, ou `23`. Isso não é zero, então `digitos` aumenta para `2`. O próximo ciclo define `n` como `23 / 10`, ou `2`. Novamente, isso não é zero, então `digitos` cresce para `3`. O próximo ciclo define `n` como `2 / 10`, ou `0`, e o loop sai, deixando `digitos` definido com o valor correto `3`.

Agora suponha que você sabe que o número tem cinco dígitos e quer retornar os três primeiros. Você pode obter esse valor dividindo o número por `10` e depois dividindo a resposta por `10` novamente. Cada divisão por `10` remove mais um dígito da extremidade direita. Para calcular o número de dígitos a remover, você apenas subtrai o número de dígitos a serem mostrados do número total de dígitos. A Listagem 8.10 incorpora esse código em uma nova função `esquerda()`. Como a assinatura da nova `esquerda()` é diferente da antiga, você pode usar ambas as funções no mesmo programa.

**Listagem 8.10** leftover.cpp

```cpp
// leftover.cpp -- sobrecarregando a função esquerda()
#include <iostream>
unsigned long esquerda(unsigned long num, unsigned ct);
char * esquerda(const char * str, int n = 1);
int main()
{
    using namespace std;
    char * viagem = "Hawaii!!"; // valor de teste
    unsigned long n = 12345678; // valor de teste
    int i;
    char * temp;
    for (i = 1; i < 10; i++)
    {
        cout << esquerda(n, i) << endl;
        temp = esquerda(viagem, i);
        cout << temp << endl;
        delete [] temp; // aponta para armazenamento temporário
    }
    return 0;
}
// Esta função retorna os primeiros ct dígitos do número num.
unsigned long esquerda(unsigned long num, unsigned ct)
{
    unsigned digitos = 1;
    unsigned long n = num;
    if (ct == 0 || num == 0)
        return 0; // retorna 0 se não há dígitos
    while (n /= 10)
        digitos++;
    if (digitos > ct)
    {
        ct = digitos - ct;
        while (ct--)
            num /= 10;
        return num; // retorna os ct dígitos à esquerda
    }
    else             // se ct >= número de dígitos
        return num;  // retorna o número inteiro
}
// Esta função retorna um ponteiro para uma nova string
// consistindo dos primeiros n caracteres da string str.
char * esquerda(const char * str, int n)
{
    if (n < 0)
        n = 0;
    char * p = new char[n + 1];
    int i;
    for (i = 0; i < n && str[i]; i++)
        p[i] = str[i]; // copia caracteres
    while (i <= n)
        p[i++] = '\0'; // define resto da string como '\0'
    return p;
}
```

Saída do programa:

```
1
H
12
Ha
123
Haw
1234
Hawa
12345
Hawai
123456
Hawaii
1234567
Hawaii!
12345678
Hawaii!!
12345678
Hawaii!!
```

### Quando Usar Sobrecarga de Funções (When to Use Function Overloading)

Você pode achar a sobrecarga de funções fascinante, mas não deve usá-la em excesso. Você deve reservar a sobrecarga de funções para funções que executam essencialmente a mesma tarefa, mas com diferentes formas de dados. Além disso, você pode querer verificar se pode atingir o mesmo objetivo usando argumentos padrão. Por exemplo, você poderia substituir a única função `esquerda()` orientada a strings por duas funções sobrecarregadas:

```cpp
char * esquerda(const char * str, unsigned n); // dois argumentos
char * esquerda(const char * str);             // um argumento
```

Mas usar a função única com um argumento padrão é mais simples. Há apenas uma função para escrever em vez de duas, e o programa requer memória para apenas uma função em vez de duas. Se você decidir modificar a função, tem que editar apenas uma. No entanto, se você precisar de diferentes tipos de argumentos, argumentos padrão não são de nenhuma ajuda, então nesse caso você deve usar a sobrecarga de funções.

---

Navegação: [Anterior](capitulo-08-01-funcoes-inline-referencias.md) | [Índice](README.md) | [Próximo](capitulo-08-03-templates-resumo.md)
