# Capitulo 6 - Instrucoes de Ramificacao e Operadores Logicos
## Parte 4: A Instrucao switch

> Traducao nao oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Navegacao: [Anterior](capitulo-06-03-cctype-operador-condicional.md) | [Indice](README.md) | [Proximo](capitulo-06-05-break-continue-loops-numericos.md)

---

## A Instrucao switch

Suponha que voce cria um programa no qual o usuario pode selecionar entre cinco opcoes de um menu. Voce pode usar as instrucoes `if else if` para lidar com cada alternativa, mas o C++ tem uma instrucao projetada especificamente para essa situacao: a instrucao `switch`. Aqui esta o formato geral para uma instrucao `switch`:

```
switch (expressao-inteira)
{
    case rotulo1 : instrucao(oes)
    case rotulo2 : instrucao(oes)
    ...
    default      : instrucao(oes)
}
```

Uma instrucao `switch` do C++ direciona o programa a ir para o rotulo listado apos a palavra-chave `case`. Lembre-se, entretanto, de que o objetivo nao e apenas que o programa salte para um rotulo especifico; ele tambem deve parar de executar instrucoes no final de um conjunto de instrucoes de `case`. Voce usa uma instrucao `break` para fazer isso.

Aqui esta como a construcao funciona. O programa avalia `expressao-inteira` e depois salta para a instrucao rotulada com esse valor de inteiro. Assim, se `expressao-inteira` tiver um valor `4`, o programa salta para o rotulo `case 4:`. Como mencionado, a execucao de instrucoes continua ate que uma instrucao `break` seja encontrada. A instrucao `break` faz com que o programa salte para apos a instrucao `switch`. A instrucao `default` e opcional. Se ela for incluida, o programa salta para o rotulo `default` se `expressao-inteira` nao coincidir com nenhum dos rotulos de `case`. Caso contrario, se `default` nao estiver presente e nenhum `case` corresponder, o programa pula todo o bloco `switch` sem fazer nada.

A Listagem 6.10 usa `switch` para implementar um simples menu executivo. Observe que o usuario insere um numero inteiro correspondendo a sua escolha, e nao a propria letra.

**Listagem 6.10 — switch.cpp**

```cpp
// switch.cpp -- usando a instrucao switch
#include <iostream>
using namespace std;
void exibirMenu(); // declaracao de funcao
void relatorio();
void conforto();
int main()
{
    exibirMenu(); // exibe o menu
    int escolha;
    cin >> escolha;
    while (escolha != 5)
    {
        switch (escolha)
        {
            case 1 :
                cout << "\a\n";       // beep!
                break;
            case 2 :
                exibirMenu();         // exibe menu novamente
                break;
            case 3 :
                relatorio();
                break;
            case 4 :
                conforto();
                break;
            default :
                cout << "Essa opcao e invalida\n";
        }
        cin >> escolha;
    }
    cout << "Ate mais!\n";
    return 0;
}

void exibirMenu()
{
    cout << "Por favor, insira uma das seguintes escolhas:\n";
    cout << "1) alerta   2) exibir menu\n";
    cout << "3) relatorio 4) conforto\n";
    cout << "5) sair\n";
}

void relatorio()
{
    cout << "Espaco para melhorias:\n";
    cout << "01. Aqui\n";
    cout << "02. Aqui\n";
    cout << "03. Aqui\n";
}

void conforto()
{
    cout << "Eu acho que voce esta fazendo muito bem!\n";
}
```

Aqui esta uma saida de exemplo do programa na Listagem 6.10:

```
Por favor, insira uma das seguintes escolhas:
1) alerta   2) exibir menu
3) relatorio 4) conforto
5) sair
1

2
Por favor, insira uma das seguintes escolhas:
1) alerta   2) exibir menu
3) relatorio 4) conforto
5) sair
3
Espaco para melhorias:
01. Aqui
02. Aqui
03. Aqui
4
Eu acho que voce esta fazendo muito bem!
5
Ate mais!
```

O `while` continua enquanto a escolha do usuario nao for `5`. A instrucao `switch` dentro do `while` determina qual acao executar e executa essa acao. A instrucao `default` corresponde a qualquer escolha de menu invalida.

### Usando Enumeradores como Rotulos

A Listagem 6.11 usa uma instrucao `switch` com enumeradores como rotulos. Lembre-se de que enumeradores sao convertidos para inteiros quando usados em instrucoes aritmeticas ou de teste.

**Listagem 6.11 — enum.cpp**

```cpp
// enum.cpp -- usando enumeradores e switch
#include <iostream>
// cria variavel Cor com rotulos enumerados
enum Cor {vermelho, laranja, amarelo, verde, azul, violeta, anil};
inline Cor & operator++(Cor & d, int)
{
    return d = (Cor)(d + 1);
}
int main()
{
    using namespace std;
    Cor codigo; // variavel do tipo Cor
    cout << "Insira o codigo da cor (0-6): ";
    int temp;
    cin >> temp;               // int porque nao ha overloading de cin para Cor
    codigo = Cor(temp);        // converte int para Cor
    while (codigo >= vermelho && codigo <= anil)
    {
        switch (codigo)
        {
            case vermelho  :
                cout << "Seu monitor usa vermelho fundo.\n";
                break;
            case laranja   :
                cout << "Seu monitor usa laranja fundo.\n";
                break;
            case amarelo   :
                cout << "Seu monitor usa amarelo fundo.\n";
                break;
            case verde     :
                cout << "Seu monitor usa verde fundo.\n";
                break;
            case azul      :
                cout << "Seu monitor usa azul fundo.\n";
                break;
            case violeta   :
                cout << "Seu monitor usa violeta fundo.\n";
                break;
            case anil      :
                cout << "Seu monitor usa anil fundo.\n";
                break;
            default        :
                cout << "Esse nao eh um codigo de cor.\n";
        }
        cout << "Proximo, por favor (0-6): ";
        cin >> temp;
        codigo = Cor(temp);
    }
    cout << "Bye!\n";
    return 0;
}
```

Aqui esta uma saida de exemplo do programa na Listagem 6.11:

```
Insira o codigo da cor (0-6): 3
Seu monitor usa verde fundo.
Proximo, por favor (0-6): 5
Seu monitor usa violeta fundo.
Proximo, por favor (0-6): 7
Bye!
```

A instrucao `switch` funciona perfeitamente com os valores enumerados de `Cor`. Em geral, o C++ trata os valores enumerados como inteiros, portanto `case vermelho:` equivale a `case 0:`, `case laranja:` equivale a `case 1:`, e assim por diante.

---

## switch Versus if else

Tanto `switch` quanto `if else` permitem que um programa selecione a partir de uma lista de alternativas. O `if else` e mais flexivel. Por exemplo, `if else` pode lidar com intervalos:

```cpp
if (age > 17 && age < 35)
    emite_votacao();
else if (age >= 35 && age < 55)
    emite_votacao_premium();
else
    emite_votacao_sior();
```

O `switch` nao pode lidar com intervalos. Cada rotulo `case` deve ser um valor inteiro unico. Alem disso, esses valores devem ser constantes. Se as alternativas envolvem testar uma variavel de ponto flutuante ou um intervalo de valores, voce deve usar `if else`. Se todas as alternativas podem ser representadas por valores inteiros constantes, voce pode usar `switch` ou `if else`.

Como a instrucao `switch` foi especificamente projetada para lidar com muitas alternativas, muitos compiladores produzem codigo mais eficiente para ela do que para o equivalente `if else`.

---

> **Usando switch com Multiplos Rotulos**
>
> Voce pode usar multiplos rotulos com um unico conjunto de instrucoes:
>
> ```cpp
> switch (escolha)
> {
>     case 'a':
>     case 'A':    // rotulos multiplos
>     case 'e':
>     case 'E':    // mais rotulos
>     case 'i':
>     case 'I':    // mais rotulos ainda
>         contagem_vogais++;
>         break;
>     case 'b':
>     case 'B':    // soh duas possibilidades
>         contagem_b++;
>         break;
>     default:
>         outras_letras++;
> }
> ```
>
> Se `escolha` for `a`, `A`, `e`, `E`, `i` ou `I`, o programa incrementa `contagem_vogais`. Se `escolha` for `b` ou `B`, o programa incrementa `contagem_b`. Qualquer outro valor de `escolha` leva o programa ate o `default` que incrementa `outras_letras`.
>
> Esta tecnica de usar rotulos multiplos para o mesmo conjunto de instrucoes funciona porque o programa apenas salta para o rotulo que corresponde e entao executa todas as instrucoes ate encontrar um `break` — ou o final do `switch`.

---

> Navegacao: [Anterior](capitulo-06-03-cctype-operador-condicional.md) | [Indice](README.md) | [Proximo](capitulo-06-05-break-continue-loops-numericos.md)
