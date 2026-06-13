# Capitulo 6 - Instrucoes de Ramificacao e Operadores Logicos
## Parte 5: As Instrucoes break e continue, e Loops de Leitura Numerica

> Traducao nao oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Navegacao: [Anterior](capitulo-06-04-instrucao-switch.md) | [Indice](README.md) | [Proximo](capitulo-06-06-arquivo-resumo.md)

---

## As Instrucoes break e continue

As instrucoes `break` e `continue` permitem que um programa pule partes do codigo. Voce pode usar a instrucao `break` em uma instrucao `switch` ou em qualquer dos loops. Ela faz com que o programa pule para a instrucao seguindo ao final da instrucao `switch` ou do corpo do loop. A instrucao `continue` e usada em loops e faz com que o programa pule o restante do corpo do loop e entao avance a avaliacao da expressao de condicao do loop e execute outra iteracao do loop, se a condicao permitir.

A Listagem 6.12 mostra como as duas instrucoes funcionam. O programa permite que o usuario insira uma linha de texto. O loop le cada caractere e exibe-o, mas para no primeiro ponto final. A instrucao `break` causa isso. Em seguida, o programa conta o numero de espacos na entrada, ignorando digitos. A instrucao `continue` ignora os digitos pulando para a proxima iteracao do loop.

**Listagem 6.12 — jump.cpp**

```cpp
// jump.cpp -- usando continue e break
#include <iostream>
const int Tamanho = 80;
int main()
{
    using namespace std;
    char linha[Tamanho];
    int espacos = 0;
    cout << "Insira uma linha de texto:\n";
    cin.get(linha, Tamanho);
    cout << "Texto completo:\n";
    for (int i = 0; linha[i] != '\0'; i++)
    {
        cout << linha[i]; // exibe caractere
        if (linha[i] == '.') // para no ponto final
            break;
        if (linha[i] != ' ') // conta somente espacos
            continue;
        espacos++;
    }
    cout << "\n" << espacos << " espacos\n";
    cout << "Concluido.\n";
    return 0;
}
```

Aqui esta uma saida de exemplo do programa na Listagem 6.12:

```
Insira uma linha de texto:
The dog leaped over the fence. He then ran away.
Texto completo:
The dog leaped over the fence.
5 espacos
Concluido.
```

Aqui esta o que acontece: o loop `for` le os caracteres de `linha[]` um por um. Quando chega ao ponto final, `break` faz com que o programa salte para a instrucao apos o loop `for`. A instrucao `continue` na sequencia `if` faz com que o programa pule a instrucao `espacos++` (que vem depois do `continue`) e avance para a proxima iteracao de loop — ou seja, apenas espacos verdadeiros sao contados.

Observe que quando `linha[i]` for um espaco, a condicao `linha[i] != ' '` sera `false` e o programa nao executara `continue`. Em vez disso, ele avanca para `espacos++`, que so e incrementado para caracteres que sao realmente espacos.

Tenha em mente que `continue` faz o programa pular para a avaliacao da condicao de teste, nao para o inicio do corpo do loop. Em um loop `for`, isso significa que o programa pula para a parte de atualizacao do `for` (o `i++`, neste caso) e depois avalia a condicao. Em um loop `while`, `continue` pula diretamente para a avaliacao da condicao.

A filosofia do bom uso de `break` e `continue` e debatida. Alguns argumentam que eles tornam o codigo menos legivel e de mais dificil manutencao porque interrompem o fluxo natural de controle. Outros argumentam que eles simplificam programas que de outro modo precisariam de codigo de controle mais complexo. De forma geral, e melhor usa-los moderadamente, quando eles simplificam realmente o codigo.

---

## Loops de Leitura Numerica

Suponha que voce esta escrevendo um programa que le uma serie de numeros em um array. Voce quer que o programa pare quando o array estiver cheio, ou quando o usuario inserir uma entrada nao numerica. Uma instrucao `if else` dentro de um loop `while` pode cuidar dessa situacao, mas o C++ deixa esse problema se tornar ainda mais complicado. Quando `cin` falha em ler entradas que nao sao do tipo esperado, ele define um indicador de erro e nao le mais nenhuma entrada.

Vamos primeiro considerar um programa que simplesmente le `float`s de um stream de entrada e os adiciona a um array de `double`, parando quando o array esta cheio. A Listagem 6.13 faz isso, ignorando o problema de entradas invalidas.

**Listagem 6.13 — cinfish.cpp**

```cpp
// cinfish.cpp -- entradas nao numericas terminam o loop
#include <iostream>
const int Max = 5;
int main()
{
    using namespace std;
    // obtem dados
    double peixe[Max];
    cout << "Por favor, insira os pesos (lb) de " << Max;
    cout << " peixes <q para terminar>: \n";
    cout << "peixe #1: ";
    int i = 0;
    while (i < Max && cin >> peixe[i]) // le se ainda houver espaco
    {
        if (++i < Max)                 // incrementa e verifica
            cout << "peixe #" << i+1 << ": ";
    }
    // calcula a media
    double total = 0.0;
    for (int j = 0; j < i; j++)
        total += peixe[j];
    // relata
    if (i == 0)
        cout << "Sem pesos inseridos.\n";
    else
        cout << total / i << " = media de pesos lb\n";
    cout << "Concluido.\n";
    return 0;
}
```

Aqui esta uma execucao de exemplo do programa na Listagem 6.13:

```
Por favor, insira os pesos (lb) de 5 peixes <q para terminar>:
peixe #1: 19.4
peixe #2: 18.0
peixe #3: q
19.9333 = media de pesos lb
Concluido.
```

Quando `cin >> peixe[i]` falha porque o usuario inseriu `q`, o valor retornado pela expressao converte para `false` (o valor de retorno de `cin>>` e o proprio objeto `cin`, que quando avaliado converte para `false` se o estado do stream indica falha). Isso termina o loop enquanto ainda ha espaco no array.

O problema, no entanto, e que a entrada `q` permanece na fila de entrada. Suponha que o programa continue e tente ler mais entradas. Como `q` ainda esta na fila e o indicador de erro de `cin` foi definido, o programa nao lera mais nada.

Para corrigir esse problema, o programa deve fazer tres coisas:
1. Resetar `cin` para aceitar novas entradas.
2. Descartar a entrada ruim.
3. Alertar o usuario.

A Listagem 6.14 mostra como isso pode ser feito.

**Listagem 6.14 — cingolf.cpp**

```cpp
// cingolf.cpp -- lida com entradas nao numericas
#include <iostream>
const int Max = 5;
int main()
{
    using namespace std;
    // obtem dados
    int golfe[Max];
    cout << "Por favor, insira suas 5 pontuacoes de golfe.\n";
    cout << "Voce deve inserir somente numeros inteiros positivos: ";
    int i;
    for (i = 0; i < Max; i++)
    {
        golfe[i] = -1; // inicializa golfe com valor invalido
        while (!(cin >> golfe[i]))  // nao leremos um int valido
        {
            cin.clear();            // reseta o indicador de entrada
            while (cin.get() != '\n') // descarta a entrada ruim
                continue;           // (ate o fim da linha)
            cout << "Por favor, insira um numero inteiro positivo: ";
        }
    }
    // calcula a media
    double total = 0.0;
    for (int j = 0; j < Max; j++)
        total += golfe[j];
    // relata
    cout << total / Max << " = media de pontuacao\n";
    return 0;
}
```

Aqui esta uma execucao de exemplo do programa na Listagem 6.14:

```
Por favor, insira suas 5 pontuacoes de golfe.
Voce deve inserir somente numeros inteiros positivos: 88
76
a
Por favor, insira um numero inteiro positivo: 100
134
99
88 76 100 134 99 = pontuacoes
79.4 = media de pontuacao
```

Veja o que acontece quando o usuario insere `a` em resposta ao prompt:
- `cin >> golfe[i]` falha porque `a` nao e um inteiro. A expressao `!(cin >> golfe[i])` torna-se `true`, de modo que o corpo do `while` e executado.
- `cin.clear()` resetar o estado de erro de `cin`, habilitando-o a aceitar novas entradas novamente.
- `while (cin.get() != '\n') continue;` le e descarta todos os caracteres na linha de entrada atual, incluindo o `a` problemático.
- O programa enta solicita ao usuario que tente novamente.

O metodo `cin.clear()` reseta o indicador de erro de cin. Sem ele, `cin` continuaria a recusar qualquer entrada. O loop `while (cin.get() != '\n')` le e descarta todos os caracteres ate o fim da linha de entrada atual. Sem isso, o `a` ruim permaneceria na fila de entrada e causaria falhas repetidas.

Observe que `golfe[i]` e inicializado para `-1` antes do loop `while`. Se a leitura `cin` falhar e nao ocorrer inicializacao, `golfe[i]` conteria lixo (garbage value) de qualquer que fosse o valor previamente armazenado naquela localizacao de memoria.

---

> Navegacao: [Anterior](capitulo-06-04-instrucao-switch.md) | [Indice](README.md) | [Proximo](capitulo-06-06-arquivo-resumo.md)
