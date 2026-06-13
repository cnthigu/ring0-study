# Capitulo 7 - Funcoes: Modulos de Programacao do C++
## Parte 2: Funcoes e Arrays

> Traducao nao oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Navegacao: [Anterior](capitulo-07-01-revisao-funcoes.md) | [Indice](README.md) | [Proximo](capitulo-07-03-funcoes-strings-estruturas.md)

---

## Funcoes e Arrays

Ate agora as funcoes de exemplo neste livro usaram tipos basicos para argumentos e valores de retorno. Mas funcoes podem ser a chave para lidar com tipos mais complexos, como arrays e estruturas. Vamos ver agora como arrays e funcoes convivem entre si.

Suponha que voce use um array para manter o controle de quantos biscoitos cada pessoa comeu em um piquenique de familia. Agora voce quer o total. Isso e facil de encontrar; basta usar um loop para adicionar todos os elementos do array. Mas adicionar elementos de array e uma tarefa tao comum que faz sentido projetar uma funcao para fazer o trabalho.

Vamos considerar o que envolve a interface da funcao. Como a funcao calcula uma soma, ela deve retornar a resposta. Para que a funcao saiba qual array somar, voce quer passar o nome do array como um argumento. E para tornar a funcao geral, voce passa o tamanho do array. O unico novo ingrediente aqui e que voce tem que declarar que um dos argumentos formais e um nome de array:

```cpp
int soma_arr(int arr[], int n) // arr = nome do array, n = tamanho
```

Isso parece plausivel. Os colchetes parecem indicar que `arr` e um array, e o fato de os colchetes estarem vazios parece indicar que voce pode usar a funcao com um array de qualquer tamanho. Mas as coisas nem sempre sao o que parecem: `arr` na verdade nao e um array; e um ponteiro! A boa noticia e que voce pode escrever o resto da funcao como se `arr` fosse um nome de array.

A Listagem 7.5 ilustra o uso de um ponteiro como se fosse um nome de array. O programa inicializa o array com alguns valores e usa a funcao `soma_arr()` para calcular a soma.

**Listagem 7.5 — arrfun1.cpp**

```cpp
// arrfun1.cpp -- funcoes com um argumento de array
#include <iostream>
const int TamArr = 8;
int soma_arr(int arr[], int n); // prototipo
int main()
{
    using namespace std;
    int biscoitos[TamArr] = {1,2,4,8,16,32,64,128};
    int soma = soma_arr(biscoitos, TamArr);
    cout << "Total de biscoitos comidos: " << soma << "\n";
    return 0;
}
// retorna a soma de um array de inteiros
int soma_arr(int arr[], int n)
{
    int total = 0;
    for (int i = 0; i < n; i++)
        total = total + arr[i];
    return total;
}
```

Aqui esta a saida do programa na Listagem 7.5:

```
Total de biscoitos comidos: 255
```

Como voce pode ver, o programa funciona. Agora vamos ver por que funciona.

### Como Ponteiros Habilitam Funcoes de Processamento de Arrays

A chave para o programa na Listagem 7.5 e que o C++, como C, na maioria dos contextos, trata o nome de um array como se fosse um ponteiro. Lembre-se do Capitulo 4, "Tipos Compostos", que o C++ interpreta um nome de array como o endereco de seu primeiro elemento:

```cpp
biscoitos == &biscoitos[0] // nome do array e o endereco do primeiro elemento
```

A chamada de funcao `soma_arr(biscoitos, TamArr)` passa o endereco do primeiro elemento do array `biscoitos` e o numero de elementos do array para a funcao `soma_arr()`. A funcao `soma_arr()` inicializa o endereco de `biscoitos` na variavel de ponteiro `arr` e inicializa `TamArr` na variavel `int` `n`. Isso significa que a Listagem 7.5 na verdade nao passa o conteudo do array para a funcao. Em vez disso, ela diz para a funcao onde o array esta (o endereco), que tipo de elementos ele tem (o tipo) e quantos elementos ele tem (a variavel `n`).

Dado que a variavel `arr` e na verdade um ponteiro, o restante da funcao faz sentido. Como voce deve se lembrar da discussao de arrays dinamicos no Capitulo 4, voce pode usar a notacao de colchetes de array igualmente bem com nomes de array ou com ponteiros para acessar elementos de um array. Quer `arr` seja um ponteiro ou um nome de array, a expressao `arr[3]` significa o quarto elemento do array. E provavelmente nao fara mal nenhum aqui lembrar voce das seguintes duas identidades:

```cpp
arr[i] == *(arr + i) // valores nas duas notacoes
&arr[i] == arr + i   // enderecos nas duas notacoes
```

### As Implicacoes de Usar Arrays como Argumentos

Vamos olhar para as implicacoes da Listagem 7.5. A chamada de funcao `soma_arr(biscoitos, TamArr)` passa o endereco do primeiro elemento do array `biscoitos` e o numero de elementos. Isso significa que a Listagem 7.5 nao passa realmente o conteudo do array para a funcao. Em vez disso, ela informa a funcao onde o array esta, que tipo de elementos ele tem e quantos elementos ele tem. Se voce passar uma variavel comum, a funcao trabalha com uma copia. Mas se voce passar um array, a funcao trabalha com o original.

A decisao de design de usar enderecos de array como argumentos economiza o tempo e a memoria necessarios para copiar um array inteiro. O overhead para usar copias pode ser proibitivo se voce estiver trabalhando com arrays grandes.

A Listagem 7.6 demonstra que `biscoitos` e `arr` tem o mesmo valor. Ela tambem mostra como o conceito de ponteiro torna a funcao `soma_arr` mais versatil do que poderia ter parecido a principio.

**Listagem 7.6 — arrfun2.cpp**

```cpp
// arrfun2.cpp -- funcoes com um argumento de array
#include <iostream>
const int TamArr = 8;
int soma_arr(int arr[], int n);
// usa std:: em vez da diretiva using
int main()
{
    int biscoitos[TamArr] = {1,2,4,8,16,32,64,128};
    std::cout << biscoitos << " = endereco do array, ";
    std::cout << sizeof biscoitos << " = sizeof biscoitos\n";
    int soma = soma_arr(biscoitos, TamArr);
    std::cout << "Total de biscoitos comidos: " << soma << std::endl;
    soma = soma_arr(biscoitos, 3); // "mentira" sobre o tamanho
    std::cout << "Tres primeiros comeram " << soma << " biscoitos.\n";
    soma = soma_arr(biscoitos + 4, 4); // outra "mentira"
    std::cout << "Ultimos quatro comeram " << soma << " biscoitos.\n";
    return 0;
}
// retorna a soma de um array de inteiros
int soma_arr(int arr[], int n)
{
    int total = 0;
    std::cout << arr << " = arr, ";
    std::cout << sizeof arr << " = sizeof arr\n";
    for (int i = 0; i < n; i++)
        total = total + arr[i];
    return total;
}
```

Aqui esta a saida do programa na Listagem 7.6:

```
003EF9FC = endereco do array, 32 = sizeof biscoitos
003EF9FC = arr, 4 = sizeof arr
Total de biscoitos comidos: 255
003EF9FC = arr, 4 = sizeof arr
Tres primeiros comeram 7 biscoitos.
003EFA0C = arr, 4 = sizeof arr
Ultimos quatro comeram 240 biscoitos.
```

Observe que `biscoitos` e `arr` avaliam para o mesmo endereco, exatamente como declarado. Mas `sizeof biscoitos` e 32, enquanto `sizeof arr` e apenas 4. Isso e porque `sizeof biscoitos` e o tamanho de todo o array, enquanto `sizeof arr` e o tamanho da variavel de ponteiro.

Porque `soma_arr()` so sabe o numero de elementos no array pelo que voce le com o segundo argumento, voce pode "mentir" para a funcao. Por exemplo, passando `3` como segundo argumento:

```cpp
soma = soma_arr(biscoitos, 3);
```

O programa calcula a soma dos primeiros tres elementos. E ao "mentir" sobre onde o array comeca:

```cpp
soma = soma_arr(biscoitos + 4, 4);
```

Como `biscoitos` age como o endereco do primeiro elemento, `biscoitos + 4` age como o endereco do quinto elemento. Esta instrucao soma o quinto, sexto, setimo e oitavo elementos do array.

---

## Mais Exemplos de Funcoes de Array

Suponha que voce queira usar um array para manter controle dos valores em dolares de seus imoveis. Voce tem que decidir que tipo usar. Certamente, `double` e menos restritivo em seu intervalo do que `int` ou `long`, e fornece digitos significativos suficientes para representar os valores com precisao. Digamos que voce nao tenha mais de cinco propriedades, portanto pode usar um array de cinco `doubles`.

Agora considere as possiveis operacoes que voce pode querer executar com o array: ler valores no array, exibir o conteudo do array e reavaliacao do valor das propriedades.

**Preenchendo o Array:**

```cpp
int preencher_array(double ar[], int limite);
```

A funcao toma um argumento de nome de array e um argumento especificando o numero maximo de itens a serem lidos, e a funcao retorna o numero real de itens lidos.

**Mostrando o Array e Protegendo-o com const:**

Para garantir que a funcao de exibicao nao altere o array original, podemos usar a palavra-chave `const`:

```cpp
void mostrar_array(const double ar[], int n);
```

A declaracao afirma que o ponteiro `ar` aponta para dados constantes. Isso significa que voce nao pode usar `ar` para alterar os dados. Observe que isso nao significa que o array original precise ser constante; significa apenas que voce nao pode usar `ar` na funcao `mostrar_array()` para alterar os dados. Assim, `mostrar_array()` trata o array como dados somente leitura.

**Modificando o Array:**

```cpp
void reavaliar(double r, double ar[], int n);
{
    for (int i = 0; i < n; i++)
        ar[i] *= r;
}
```

Como esta funcao deve alterar os valores do array, voce nao usa `const` ao declarar `ar`.

A Listagem 7.7 coloca todos esses pedacos juntos.

**Listagem 7.7 — arrfun3.cpp**

```cpp
// arrfun3.cpp -- funcoes de array e const
#include <iostream>
const int Max = 5;
// prototipos de funcoes
int preencher_array(double ar[], int limite);
void mostrar_array(const double ar[], int n); // nao muda os dados
void reavaliar(double r, double ar[], int n);
int main()
{
    using namespace std;
    double propriedades[Max];
    int tamanho = preencher_array(propriedades, Max);
    mostrar_array(propriedades, tamanho);
    if (tamanho > 0)
    {
        cout << "Digite o fator de reavaliacao: ";
        double fator;
        while (!(cin >> fator)) // entrada ruim
        {
            cin.clear();
            while (cin.get() != '\n')
                continue;
            cout << "Entrada invalida; Por favor, insira um numero: ";
        }
        reavaliar(fator, propriedades, tamanho);
        mostrar_array(propriedades, tamanho);
    }
    cout << "Concluido.\n";
    cin.get();
    cin.get();
    return 0;
}
int preencher_array(double ar[], int limite)
{
    using namespace std;
    double temp;
    int i;
    for (i = 0; i < limite; i++)
    {
        cout << "Digite o valor #" << (i + 1) << ": ";
        cin >> temp;
        if (!cin) // entrada ruim
        {
            cin.clear();
            while (cin.get() != '\n')
                continue;
            cout << "Entrada invalida; processo de entrada encerrado.\n";
            break;
        }
        else if (temp < 0) // sinal para encerrar
            break;
        ar[i] = temp;
    }
    return i;
}
// a funcao a seguir pode usar, mas nao alterar,
// o array cujo endereco e ar
void mostrar_array(const double ar[], int n)
{
    using namespace std;
    for (int i = 0; i < n; i++)
    {
        cout << "Propriedade #" << (i + 1) << ": $";
        cout << ar[i] << endl;
    }
}
// multiplica cada elemento de ar[] por r
void reavaliar(double r, double ar[], int n)
{
    for (int i = 0; i < n; i++)
        ar[i] *= r;
}
```

Aqui estao duas execucoes de exemplo do programa na Listagem 7.7:

```
Digite o valor #1: 100000
Digite o valor #2: 80000
Digite o valor #3: 222000
Digite o valor #4: 240000
Digite o valor #5: 118000
Propriedade #1: $100000
Propriedade #2: $80000
Propriedade #3: $222000
Propriedade #4: $240000
Propriedade #5: $118000
Digite o fator de reavaliacao: 0.8
Propriedade #1: $80000
Propriedade #2: $64000
Propriedade #3: $177600
Propriedade #4: $192000
Propriedade #5: $94400
Concluido.
```

---

## Funcoes Usando Intervalos de Array

Ha outra abordagem para fornecer as informacoes de que uma funcao precisa: especificar um intervalo de elementos. Isso pode ser feito passando dois ponteiros — um identificando o inicio do array e um identificando o fim do array. O STL (Standard Template Library), por exemplo, generaliza a abordagem de intervalo. A abordagem STL usa o conceito de "um alem do fim" para indicar uma extensao. Ou seja, no caso de um array, o argumento que identifica o fim do array seria um ponteiro para o local logo apos o ultimo elemento.

A Listagem 7.8 modifica a Listagem 7.6 para usar dois ponteiros para especificar um intervalo.

**Listagem 7.8 — arrfun4.cpp**

```cpp
// arrfun4.cpp -- funcoes com um intervalo de array
#include <iostream>
const int TamArr = 8;
int soma_arr(const int * inicio, const int * fim);
int main()
{
    using namespace std;
    int biscoitos[TamArr] = {1,2,4,8,16,32,64,128};
    int soma = soma_arr(biscoitos, biscoitos + TamArr);
    cout << "Total de biscoitos comidos: " << soma << endl;
    soma = soma_arr(biscoitos, biscoitos + 3); // primeiros 3 elementos
    cout << "Tres primeiros comeram " << soma << " biscoitos.\n";
    soma = soma_arr(biscoitos + 4, biscoitos + 8); // ultimos 4 elementos
    cout << "Ultimos quatro comeram " << soma << " biscoitos.\n";
    return 0;
}
// retorna a soma de um array de inteiros
int soma_arr(const int * inicio, const int * fim)
{
    const int * pt;
    int total = 0;
    for (pt = inicio; pt != fim; pt++)
        total = total + *pt;
    return total;
}
```

Aqui esta a saida do programa na Listagem 7.8:

```
Total de biscoitos comidos: 255
Tres primeiros comeram 7 biscoitos.
Ultimos quatro comeram 240 biscoitos.
```

Observe o loop `for` na funcao `soma_arr()`:

```cpp
for (pt = inicio; pt != fim; pt++)
    total = total + *pt;
```

Ele define `pt` para apontar para o primeiro elemento a ser processado (aquele apontado por `inicio`) e adiciona `*pt` (o valor do elemento) ao `total`. Entao o loop atualiza `pt` incrementando-o, fazendo-o apontar para o proximo elemento. O processo continua enquanto `pt != fim`. Quando `pt` finalmente iguala `fim`, ele esta apontando para o local seguindo o ultimo elemento do intervalo, entao o loop e encerrado.

Observe como as diferentes chamadas de funcao especificam diferentes intervalos dentro do array:

```cpp
int soma = soma_arr(biscoitos, biscoitos + TamArr); // todo o array
soma = soma_arr(biscoitos, biscoitos + 3);          // primeiros 3 elementos
soma = soma_arr(biscoitos + 4, biscoitos + 8);      // ultimos 4 elementos
```

---

## Ponteiros e const

Usar `const` com ponteiros tem alguns aspectos sutis, entao vamos dar uma olhada mais detalhada. Voce pode usar a palavra-chave `const` de duas maneiras diferentes com ponteiros. A primeira maneira e fazer um ponteiro apontar para um objeto constante, o que impede que voce use o ponteiro para alterar o valor apontado. A segunda maneira e tornar o proprio ponteiro constante, o que impede que voce altere para onde o ponteiro aponta.

**Primeiro**, vamos declarar um ponteiro `pt` que aponta para uma constante:

```cpp
int idade = 39;
const int * pt = &idade;
```

Esta declaracao afirma que `pt` aponta para um `int` constante (39, neste caso). Portanto, voce nao pode usar `pt` para alterar esse valor. Em outras palavras, o valor `*pt` e `const` e nao pode ser modificado:

```cpp
*pt += 1;  // INVALIDO porque pt aponta para um int const
cin >> *pt; // INVALIDO pela mesma razao
```

Agora para um ponto sutil. Esta declaracao para `pt` nao significa necessariamente que o valor para o qual aponta seja realmente uma constante; apenas significa que o valor e uma constante no que diz respeito a `pt`. Por exemplo, `pt` aponta para `idade`, e `idade` nao e `const`. Voce pode alterar o valor de `idade` diretamente usando a variavel `idade`, mas nao pode alterar o valor indiretamente via o ponteiro `pt`:

```cpp
*pt = 20;  // INVALIDO porque pt aponta para um int const
idade = 20; // VALIDO porque idade nao e declarada como const
```

> **Usando const Sempre que Possivel**
>
> Ha duas razoes fortes para declarar argumentos de ponteiro como ponteiros para dados constantes:
>
> - Protege voce contra erros de programacao que alteram inadvertidamente os dados.
> - Usar `const` permite que uma funcao processe argumentos reais tanto `const` quanto nao `const`, enquanto uma funcao que omite `const` no prototipo pode aceitar apenas dados nao `const`.
>
> Voce deve declarar argumentos de ponteiro formal como ponteiros para `const` sempre que for apropriado.

A segunda maneira de usar `const` torna impossivel mudar o valor do proprio ponteiro:

```cpp
int preguica = 3;
const int * ps = &preguica; // um ponteiro para int const
int * const dedo = &preguica; // um ponteiro const para int
```

Note que a ultima declaracao reposicionou a palavra-chave `const`. Esta forma de declaracao restringe `dedo` a apontar apenas para `preguica`. No entanto, permite que voce use `dedo` para alterar o valor de `preguica`. A declaracao do meio nao permite que voce use `ps` para alterar o valor de `preguica`, mas permite que `ps` aponte para outro local. Em suma, `dedo` e `*ps` sao ambos `const`, e `*dedo` e `ps` nao sao `const`.

Se voce quiser, pode declarar um ponteiro `const` para um objeto `const`:

```cpp
double problema = 2.0E30;
const double * const fixo = &problema;
```

Aqui `fixo` pode apontar apenas para `problema`, e `fixo` nao pode ser usado para alterar o valor de `problema`. Em suma, tanto `fixo` quanto `*fixo` sao `const`.

---

## Funcoes e Arrays Bidimensionais

Para escrever uma funcao que tem um array bidimensional como argumento, voce precisa lembrar que o nome de um array e tratado como seu endereco, portanto o parametro formal correspondente e um ponteiro, assim como para arrays unidimensionais. A parte complicada e declarar o ponteiro corretamente. Suponha, por exemplo, que voce comece com este codigo:

```cpp
int dados[3][4] = {{1,2,3,4}, {9,8,7,6}, {2,4,6,8}};
int total = soma(dados, 3);
```

Como deve ser o prototipo para `soma()`? Bem, `dados` e o nome de um array com tres elementos. O primeiro elemento e, em si, um array de quatro valores `int`. Assim, o tipo de `dados` e ponteiro-para-array-de-quatro-int, portanto um prototipo apropriado seria este:

```cpp
int soma(int (*ar2)[4], int tamanho);
```

Os parenteses sao necessarios porque a declaracao a seguir declararia um array de quatro ponteiros-para-int em vez de um unico ponteiro-para-array-de-quatro-int:

```cpp
int *ar2[4]
```

Aqui esta um formato alternativo que significa exatamente a mesma coisa que o primeiro prototipo, mas que talvez seja mais facil de ler:

```cpp
int soma(int ar2[][4], int tamanho);
```

O tipo de ponteiro especifica o numero de colunas, e e por isso que o numero de colunas nao e passado como um argumento de funcao separado. Como o tipo de ponteiro especifica o numero de colunas, a funcao `soma()` so funciona com arrays com quatro colunas. Mas o numero de linhas e especificado pela variavel `tamanho`, portanto `soma()` pode trabalhar com um numero variavel de linhas:

```cpp
int a[100][4];
int b[6][4];
...
int total1 = soma(a, 100); // soma todo a
int total2 = soma(b, 6);   // soma todo b
int total3 = soma(a, 10);  // soma as primeiras 10 linhas de a
int total4 = soma(a+10, 20); // soma as proximas 20 linhas de a
```

A maneira mais simples de usar `ar2` na definicao da funcao e usa-lo como se fosse o nome de um array bidimensional. Aqui esta uma possivel definicao de funcao:

```cpp
int soma(int ar2[][4], int tamanho)
{
    int total = 0;
    for (int r = 0; r < tamanho; r++)
        for (int c = 0; c < 4; c++)
            total += ar2[r][c];
    return total;
}
```

---

> Navegacao: [Anterior](capitulo-07-01-revisao-funcoes.md) | [Indice](README.md) | [Proximo](capitulo-07-03-funcoes-strings-estruturas.md)
