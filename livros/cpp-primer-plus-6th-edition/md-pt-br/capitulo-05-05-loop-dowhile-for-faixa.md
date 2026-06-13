# Capitulo 5 - Loops e Expressoes Relacionais (Loops and Relational Expressions)
## Parte 5: O Loop do while e o Loop for de Faixa (C++11)

> Traducao nao oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Navegacao: [Anterior](capitulo-05-04-loop-while.md) | [Indice](README.md) | [Proximo](capitulo-05-06-entrada-texto-eof.md)

---

## O Loop do while

Voce ja viu o loop `for` e o loop `while`. O terceiro loop C++ e o `do while`. Ele e diferente dos outros dois porque e um **loop de condicao de saida** (exit-condition loop). Isso significa que esse loop impetuoso primeiro executa o corpo do loop e so entao avalia a expressao de teste para ver se deve continuar em loop. Se a condicao avaliar como `false`, o loop termina; caso contrario, um novo ciclo de execucao e teste comeca. Esse loop sempre executa pelo menos uma vez porque o fluxo do programa deve passar pelo corpo do loop antes de chegar ao teste. Aqui esta a sintaxe para o loop `do while`:

```cpp
do
    corpo
while (expressao-de-teste);
```

A parte `corpo` pode ser uma unica instrucao ou um bloco de instrucoes delimitado por chaves. A Figura 5.4 resume o fluxo do programa para loops `do while`.

Geralmente, um loop de condicao de entrada e uma escolha melhor do que um loop de condicao de saida porque o loop de condicao de entrada verifica antes de executar o loop. Por exemplo, suponha que a Listagem 5.13 tivesse usado `do while` em vez de `while`. Nesse caso, o loop imprimiria o caractere nulo e seu codigo antes de descobrir que ja havia atingido o fim da string. Mas as vezes um teste `do while` faz sentido. Por exemplo, se voce esta solicitando entrada ao usuario, o programa precisa obter a entrada antes de testa-la. A Listagem 5.15 mostra como usar `do while` em tal situacao.

**Listagem 5.15 — dowhile.cpp**

```cpp
// dowhile.cpp -- loop de condicao de saida
#include <iostream>
int main()
{
    using namespace std;
    int n;
    cout << "Insira numeros no intervalo 1-10 para encontrar ";
    cout << "meu numero favorito\n";
    do
    {
        cin >> n; // executa o corpo
    } while (n != 7); // depois testa
    cout << "Sim, 7 e o meu favorito.\n";
    return 0;
}
```

Aqui esta uma execucao de exemplo do programa na Listagem 5.15:

```
Insira numeros no intervalo 1-10 para encontrar meu numero favorito
3
5
7
Sim, 7 e o meu favorito.
```

---

> **Loops for Estranhos**
>
> Nao e muito comum, mas voce pode ocasionalmente ver codigo que se assemelha ao seguinte:
>
> ```cpp
> int i = 0;
> for(;;) // as vezes chamado de "loop eterno"
> {
>     i++;
>     // faz algo ...
>     if (30 >= i) break; // instrucao if e break (Capitulo 6)
> }
> ```
>
> Ou aqui esta outra variacao:
>
> ```cpp
> int i = 0;
> for(;;i++)
> {
>     if (30 >= i) break;
>     // faz algo ...
> }
> ```
>
> O codigo se baseia no fato de que uma condicao de teste vazia em um loop `for` e tratada como sendo `true`. Nenhum desses exemplos e facil de ler, e nenhum deles deve ser usado como modelo geral para escrever um loop. A funcionalidade do primeiro exemplo pode ser mais claramente expressa em um loop `do while`:
>
> ```cpp
> int i = 0;
> do {
>     i++;
>     // faz algo;
> } while (30 > i);
> ```
>
> Da mesma forma, o segundo exemplo pode ser expresso mais claramente como um loop `while`:
>
> ```cpp
> while (i < 30)
> {
>     // faz algo
>     i++;
> }
> ```
>
> Em geral, escrever codigo claro e facilmente compreensivel e um objetivo mais util do que demonstrar a capacidade de explorar recursos obscuros da linguagem.

---

## O Loop for de Faixa (C++11)

O C++11 adiciona uma nova forma de loop chamada de **loop for de faixa** (range-based for loop). Ela simplifica uma tarefa comum de loop — a de fazer algo com cada elemento de um array, ou, de forma mais geral, de uma das classes de conteineres, como `vector` ou `array`. Aqui esta um exemplo:

```cpp
double precos[5] = {4.99, 10.99, 6.87, 7.99, 8.49};
for (double x : precos)
    cout << x << std::endl;
```

Aqui, `x` representa inicialmente o primeiro membro do array `precos`. Apos exibir o primeiro elemento, o loop entao cicla `x` para representar os elementos restantes do array por sua vez, portanto este codigo imprimiria todos os cinco membros, um por linha. Em suma, este loop exibe todos os valores incluidos no intervalo (range) do array.

Para modificar os valores do array, voce precisa de uma sintaxe diferente para a variavel do loop:

```cpp
for (double &x : precos)
    x = x * 0.80; // desconto de 20%
```

O simbolo `&` identifica `x` como uma variavel de referencia (reference variable), um topico que discutiremos no Capitulo 8, "Aventuras em Funcoes". A importancia aqui e que essa forma de declaracao permite que o codigo subsequente modifique o conteudo do array, enquanto a primeira forma nao permite.

O loop `for` de faixa tambem pode ser usado com listas de inicializacao:

```cpp
for (int x : {3, 5, 2, 8, 6})
    cout << x << " ";
cout << '\n';
```

Porem, este loop provavelmente sera usado com mais frequencia com as diversas classes de conteineres de templates discutidas no Capitulo 16.

---

> Navegacao: [Anterior](capitulo-05-04-loop-while.md) | [Indice](README.md) | [Proximo](capitulo-05-06-entrada-texto-eof.md)
