# Capitulo 6 - Instrucoes de Ramificacao e Operadores Logicos (Branching Statements and Logical Operators)
## Parte 1: A Instrucao if

> Traducao nao oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Navegacao: [Anterior](capitulo-05-07-loops-aninhados-resumo.md) | [Indice](README.md) | [Proximo](capitulo-06-02-operadores-logicos.md)

---

Neste capitulo voce aprendera sobre o seguinte:

- A instrucao `if`
- A instrucao `if else`
- Operadores logicos: `&&`, `||` e `!`
- A biblioteca `cctype` de funcoes de caractere
- O operador condicional: `?:`
- A instrucao `switch`
- As instrucoes `continue` e `break`
- Loops de leitura numerica
- Entrada e saida basica em arquivos

Uma das chaves para projetar programas inteligentes e dar a eles a capacidade de tomar decisoes. O Capitulo 5, "Loops e Expressoes Relacionais", mostra um tipo de tomada de decisao — o loop — no qual um programa decide se deve continuar fazendo o loop. Este capitulo investiga como o C++ permite que voce use instrucoes de ramificacao para decidir entre acoes alternativas. Qual esquema de protecao contra vampiros (alho ou cruz) o programa deve usar? Qual opcao de menu o usuario selecionou? O usuario inseriu zero? O C++ fornece as instrucoes `if` e `switch` para implementar decisoes, e elas sao os topicos principais deste capitulo. Este capitulo tambem examina o operador condicional, que oferece outra maneira de fazer uma escolha, e os operadores logicos, que permitem combinar dois testes em um. Por fim, o capitulo da uma primeira olhada na entrada e saida de arquivos.

---

## A Instrucao if

Quando um programa C++ precisa escolher se deve executar uma acao especifica, voce geralmente implementa a escolha com uma instrucao `if`. A instrucao `if` vem em duas formas: `if` e `if else`. Vamos investigar o `if` simples primeiro. Ele e modelado com base no ingles comum, como em "Se voce tiver um cartao Captain Cookie, voce recebe um cookie gratis." A instrucao `if` direciona um programa para executar uma instrucao ou bloco de instrucoes se uma condicao de teste for verdadeira e para pular essa instrucao ou bloco se a condicao for falsa. Assim, uma instrucao `if` permite que um programa decida se uma instrucao especifica deve ser executada.

A sintaxe para a instrucao `if` e semelhante a sintaxe do `while`:

```
if (condicao-de-teste)
    instrucao
```

Uma `condicao-de-teste` verdadeira faz com que o programa execute a `instrucao`, que pode ser uma unica instrucao ou um bloco. Uma `condicao-de-teste` falsa faz com que o programa pule a `instrucao`. Assim como nas condicoes de teste de loop, uma condicao de teste `if` e convertida de tipo para um valor `bool`, portanto zero se torna `false` e diferente de zero se torna `true`. A construcao `if` inteira conta como uma unica instrucao.

Na maioria das vezes, a `condicao-de-teste` e uma expressao relacional como as usadas para controlar loops. Suponha, por exemplo, que voce queira um programa que conte os espacos na entrada, bem como o numero total de caracteres. Voce pode usar `cin.get(char)` em um loop `while` para ler os caracteres e, em seguida, usar uma instrucao `if` para identificar e contar os caracteres de espaco. A Listagem 6.1 faz exatamente isso, usando o periodo (.) para reconhecer o fim de uma frase.

**Listagem 6.1 — if.cpp**

```cpp
// if.cpp -- usando a instrucao if
#include <iostream>
int main()
{
    using std::cin; // declaracoes using
    using std::cout;
    char ch;
    int espacos = 0;
    int total = 0;
    cin.get(ch);
    while (ch != '.') // encerra no final da frase
    {
        if (ch == ' ') // verifica se ch e um espaco
            ++espacos;
        ++total;       // executado em cada ciclo
        cin.get(ch);
    }
    cout << espacos << " espacos, " << total;
    cout << " caracteres no total na frase\n";
    return 0;
}
```

Aqui esta uma saida de exemplo do programa na Listagem 6.1:

```
The balloonist was an airhead
with lofty goals.
6 espacos, 46 caracteres no total na frase
```

Como os comentarios na Listagem 6.1 indicam, a instrucao `++espacos;` e executada apenas quando `ch` e um espaco. Como ela esta fora da instrucao `if`, a instrucao `++total;` e executada em cada ciclo do loop. Observe que a contagem total inclui o caractere de nova linha gerado ao pressionar Enter.

---

## A Instrucao if else

Enquanto uma instrucao `if` permite que um programa decida se uma instrucao ou bloco especifico e executado, uma instrucao `if else` permite que um programa decida *qual* de duas instrucoes ou blocos e executado. E uma instrucao inestimavel para criar cursos de acao alternativos. A instrucao `if else` do C++ e modelada com base no ingles simples, como em "Se voce tiver um cartao Captain Cookie, voce recebe um Cookie Plus Plus, caso contrario, apenas um Cookie Ordinario." A instrucao `if else` tem esta forma geral:

```
if (condicao-de-teste)
    instrucao1
else
    instrucao2
```

Se a `condicao-de-teste` for verdadeira, ou diferente de zero, o programa executa a `instrucao1` e pula a `instrucao2`. Caso contrario, quando a `condicao-de-teste` e falsa, ou zero, o programa pula a `instrucao1` e executa a `instrucao2`. Portanto, o seguinte fragmento de codigo imprime a primeira mensagem se `resposta` for 1492 e imprime a segunda mensagem caso contrario:

```cpp
if (resposta == 1492)
    cout << "Isso esta certo!\n";
else
    cout << "Voce deve revisar o Capitulo 1 novamente.\n";
```

Cada instrucao pode ser uma unica instrucao ou um bloco de instrucoes delimitado por chaves. A construcao `if else` inteira conta sintaticamente como uma unica instrucao.

Por exemplo, suponha que voce queira alterar o texto de entrada embaralhando as letras enquanto mantém o caractere de nova linha intacto. Nesse caso, cada linha de entrada e convertida em uma linha de saida de igual comprimento. Isso significa que voce quer que o programa tome um curso de acao para os caracteres de nova linha e um curso de acao diferente para todos os outros caracteres. Como a Listagem 6.2 mostra, `if else` torna essa tarefa facil. A listagem tambem ilustra o qualificador `std::`, uma das alternativas a uma diretiva `using`.

**Listagem 6.2 — ifelse.cpp**

```cpp
// ifelse.cpp -- usando a instrucao if else
#include <iostream>
int main()
{
    char ch;
    std::cout << "Digite, e eu repetirei.\n";
    std::cin.get(ch);
    while (ch != '.')
    {
        if (ch == '\n')
            std::cout << ch; // executado se for nova linha
        else
            std::cout << ++ch; // executado caso contrario
        std::cin.get(ch);
    }
    // tente ch + 1 em vez de ++ch para efeito interessante
    std::cout << "\nPor favor, desculpe a ligeira confusao.\n";
    return 0;
}
```

Aqui esta uma saida de exemplo do programa na Listagem 6.2:

```
Digite, e eu repetirei.
An ineffable joy suffused me as I beheld
Bo!jofggbcmf!kpz!tvggvtfe!nf!bt!J!cfifme
the wonders of modern computing.
uif!xpoefst!pg!npefso!dpnqvujoh
Por favor, desculpe a ligeira confusao.
```

Observe que um dos comentarios na Listagem 6.2 sugere que alterar `++ch` para `ch+1` tem um efeito interessante. Voce consegue deduzir o que sera? Se nao, experimente e veja se consegue explicar o que esta acontecendo. (Dica: pense em como `cout` lida com tipos diferentes.)

## Formatando Instrucoes if else

Tenha em mente que as duas alternativas em uma instrucao `if else` devem ser instrucoes unicas. Se voce precisar de mais de uma instrucao, deve usar chaves para coletá-las em uma unica instrucao de bloco. Ao contrario de algumas linguagens, como BASIC e FORTRAN, o C++ nao considera automaticamente tudo entre `if` e `else` como um bloco, portanto voce deve usar chaves para tornar as instrucoes um bloco. O seguinte codigo, por exemplo, produz um erro de compilacao:

```cpp
if (ch == 'Z')
    zorro++;         // if termina aqui
    cout << "Mais um candidato a Zorro\n";
else                 // errado
    sem_graça++;
    cout << "Nao e um candidato a Zorro\n";
```

O compilador ve isso como uma instrucao `if` simples que termina com a instrucao `zorro++;`. Em seguida, ha uma instrucao `cout`. Ate aqui, tudo bem. Mas entao ha o que o compilador percebe como um `else` sem ligacao, e isso e marcado como um erro de sintaxe.

Voce adiciona chaves para agrupar instrucoes em um unico bloco de instrucoes:

```cpp
if (ch == 'Z')
{             // bloco se verdadeiro
    zorro++;
    cout << "Mais um candidato a Zorro\n";
}
else
{             // bloco se falso
    sem_graça++;
    cout << "Nao e um candidato a Zorro\n";
}
```

Como o C++ e uma linguagem de formato livre, voce pode arranjar as chaves como quiser, desde que elas delimitam as instrucoes. O codigo anterior mostra um formato popular. Aqui esta outro:

```cpp
if (ch == 'Z') {
    zorro++;
    cout << "Mais um candidato a Zorro\n";
    }
else {
    sem_graça++;
    cout << "Nao e um candidato a Zorro\n";
    }
```

A primeira forma enfatiza a estrutura de bloco para as instrucoes, enquanto a segunda forma associa mais de perto os blocos as palavras-chave `if` e `else`. Qualquer estilo e claro e consistente e deve servir bem a voce; no entanto, voce pode encontrar um instrutor ou empregador com visoes fortes e especificas sobre o assunto.

---

## A Construcao if else if else

Os programas de computador, como a vida, podem apresentar a voce uma escolha de mais de duas selecoes. Voce pode estender a instrucao `if else` do C++ para atender a essa necessidade. Como voce ja viu, o `else` deve ser seguido por uma unica instrucao, que pode ser um bloco. Como uma instrucao `if else` em si e uma unica instrucao, ela pode seguir um `else`:

```cpp
if (ch == 'A')
    nota_a++;       // alternativa #1
else
    if (ch == 'B') // alternativa #2
        nota_b++;  // subalternativa #2a
    else
        regular++; // subalternativa #2b
```

Se `ch` nao e `'A'`, o programa vai para o `else`. La, um segundo `if else` subdivide essa alternativa em mais duas escolhas. O formato livre do C++ permite que voce arrange esses elementos em um formato mais facil de ler:

```cpp
if (ch == 'A')
    nota_a++;      // alternativa #1
else if (ch == 'B')
    nota_b++;      // alternativa #2
else
    regular++;     // alternativa #3
```

Isso parece uma nova estrutura de controle — uma estrutura `if else if else`. Mas na verdade e um `if else` contido dentro de um segundo. Este formato revisado parece muito mais limpo e permite que voce percorra o codigo para identificar as diferentes alternativas. Essa construcao inteira ainda conta como uma instrucao.

A Listagem 6.3 usa esse formato preferido para construir um modesto programa de quiz.

**Listagem 6.3 — ifelseif.cpp**

```cpp
// ifelseif.cpp -- usando if else if else
#include <iostream>
const int Favorito = 27;
int main()
{
    using namespace std;
    int n;
    cout << "Digite um numero no intervalo 1-100 para encontrar ";
    cout << "meu numero favorito: ";
    do
    {
        cin >> n;
        if (n < Favorito)
            cout << "Muito baixo -- tente novamente: ";
        else if (n > Favorito)
            cout << "Muito alto -- tente novamente: ";
        else
            cout << Favorito << " esta certo!\n";
    } while (n != Favorito);
    return 0;
}
```

Aqui esta uma saida de exemplo do programa na Listagem 6.3:

```
Digite um numero no intervalo 1-100 para encontrar meu numero favorito: 50
Muito alto -- tente novamente: 25
Muito baixo -- tente novamente: 37
Muito alto -- tente novamente: 31
Muito alto -- tente novamente: 28
Muito alto -- tente novamente: 27
27 esta certo!
```

> **Operadores Condicionais e Prevencao de Erros**
>
> Muitos programadores invertem a expressao mais intuitiva `variavel == valor` para `valor == variavel` a fim de capturar erros em que a igualdade e digitada incorretamente como um operador de atribuicao. Por exemplo, inserir o condicional da seguinte forma e valido e funcionara corretamente:
>
> ```cpp
> if (3 == meuNumero)
> ```
>
> Porem, se voce digitar incorretamente da seguinte forma, o compilador gerara uma mensagem de erro porque acredita que voce esta tentando atribuir um valor a um literal (3 e sempre igual a 3 e nao pode receber outro valor):
>
> ```cpp
> if (3 = meuNumero)
> ```
>
> Suponha que voce cometeu um erro semelhante, usando a notacao anterior:
>
> ```cpp
> if (meuNumero = 3)
> ```
>
> O compilador simplesmente atribuiria o valor 3 a `meuNumero`, e o bloco dentro do `if` seria executado — um erro muito comum e dificil de encontrar. (Porem, muitos compiladores emitirao um aviso, que seria sensato voce observar.) Como regra geral, escrever codigo que permita ao compilador encontrar erros e muito mais facil do que reparar as causas de resultados misteriosamente incorretos.

---

> Navegacao: [Anterior](capitulo-05-07-loops-aninhados-resumo.md) | [Indice](README.md) | [Proximo](capitulo-06-02-operadores-logicos.md)
