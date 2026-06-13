# Capitulo 6 - Instrucoes de Ramificacao e Operadores Logicos
## Parte 2: Operadores Logicos

> Traducao nao oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Navegacao: [Anterior](capitulo-06-01-instrucao-if.md) | [Indice](README.md) | [Proximo](capitulo-06-03-cctype-operador-condicional.md)

---

## Operadores Logicos

Com frequencia, a decisao de executar uma parte de um programa depende de mais de uma condicao. Voce pode combinar essas necessidades no C++ usando os operadores logicos (logical operators). O C++ fornece tres operadores logicos para combinar ou negar expressoes: `&&`, `||` e `!`. Vamos discutir cada um deles.

---

## O Operador OR Logico: ||

Na vida cotidiana, a palavra *ou* as vezes significa uma coisa e as vezes outra. A frase "Vou usar minha capa de chuva ou meu guarda-chuva" implica usar uma ou a outra, mas nao os dois. A frase "Voce precisa satisfazer os prerequisitos ou obter permissao do instrutor" implica que qualquer alternativa sera satisfatoria; mesmo as duas alternativas em conjunto sao aceitaveis. O segundo significado de *ou* — uma ou a outra ou ambas — e o que o C++ usa para o **operador OR** (operador OU logico, `||`). Este operador combina duas expressoes em uma. Se a expressao original a esquerda ou a expressao original a direita, ou ambas, forem verdadeiras (ou diferente de zero), a expressao resultante sera verdadeira. Caso contrario, a expressao sera falsa. Aqui estao alguns exemplos:

```cpp
5 == 5 || 5 == 9    // verdadeiro porque 5 == 5 eh verdadeiro
5 > 3 || 5 > 10    // verdadeiro porque 5 > 3 eh verdadeiro
5 > 8 || 5 < 10    // verdadeiro porque 5 < 10 eh verdadeiro
5 < 8 || 5 > 2     // verdadeiro porque ambos sao verdadeiros
5 > 8 || 5 < 2     // falso porque ambos sao falsos
```

Como os operadores relacionais tem precedencia maior do que `||`, voce pode escrever essas expressoes sem parenteses.

O C++ oferece a voce um recurso interessante chamado **avaliacao de curto-circuito** (short-circuit evaluation) para o operador `||`. Isso significa que o C++ avalia o operando da esquerda primeiro, e se ele for `true`, o C++ nao se preocupa em avaliar o operando da direita. Isso e logico porque basta que um dos operandos seja verdadeiro para que toda a expressao seja verdadeira. (Ah, o profundo utilitarismo dos computadores!)

A Listagem 6.4 usa o operador `||` em um programa que determina se uma entrada e a letra `q` (minuscula) ou `Q` (maiuscula). Nesse caso, voce deve sair do programa se o usuario pressionar qualquer variante de `q`.

**Listagem 6.4 — or.cpp**

```cpp
// or.cpp -- usando o operador OR
#include <iostream>
int main()
{
    using namespace std;
    cout << "Esta mensagem continuara a aparecer ate que voce\n";
    cout << "insira a letra Q (maiuscula) ou q (minuscula).\n";
    char ch;
    cin.get(ch);
    while (ch != 'Q' && ch != 'q') // dois testes em um
    {
        cout << "Obrigado por pressionar " << ch << "\n";
        cin.get(ch);
    }
    cout << "Ate mais!\n";
    return 0;
}
```

O loop `while` da Listagem 6.4 continua enquanto `ch` nao for `Q` e nao for `q`. Outra forma de dizer isso e que o loop termina quando `ch` for `Q` ou quando `ch` for `q`. A expressao de teste usa o operador AND (`&&`), que o proximo topico descreve. Voce pode reescrever a condicao usando `||`:

```cpp
while (ch != 'Q' || ch != 'q') // ERRADO — sempre verdadeiro
```

Se `ch` for `Q`, entao `ch != 'q'` ainda sera verdadeiro, portanto o lado direito de `||` fara toda a expressao ser verdadeira independentemente do que esteja no lado esquerdo. A forma original com `&&` e a correta aqui.

---

## O Operador AND Logico: &&

O **operador AND logico** (`&&`), tambem escrito como "ampersand duplo", combina duas expressoes em uma. A expressao resultante sera verdadeira somente se ambas as expressoes originais (os dois operandos) forem verdadeiras. Aqui estao alguns exemplos:

```cpp
5 == 5 && 4 == 4   // verdadeiro porque ambos sao verdadeiros
5 == 3 && 4 == 4   // falso porque 5 == 3 eh falso
5 > 3 && 5 > 10   // falso porque 5 > 10 eh falso
5 < 8 && 5 > 2    // verdadeiro porque ambos sao verdadeiros
5 > 8 && 5 < 2    // falso porque ambos sao falsos
```

Como os operadores relacionais tem precedencia maior do que `&&`, voce pode escrever essas expressoes sem parenteses.

O C++ tambem usa avaliacao de curto-circuito para o operador `&&`. Nesse caso, o C++ avalia o operando da esquerda primeiro, e se ele for `false`, o C++ nao avalia o operando da direita. Novamente, isso e logico porque a expressao inteira sera falsa se pelo menos um dos operandos for falso.

A Listagem 6.5 usa `&&` em um programa que relata se uma nota de avaliacao de software (na faixa de 0 a 10) e considerada aceitavel. Uma nota entre 5 e 9 (incluindo-as), inclusive, e aceitavel.

**Listagem 6.5 — and.cpp**

```cpp
// and.cpp -- usando o operador AND
#include <iostream>
const int Minimo = 5;
const int Maximo = 9;
int main()
{
    using namespace std;
    int nota;
    cout << "Digite a pontuacao NAAQ (0-10), (-1 encerra):\n";
    cin >> nota;
    while (nota != -1)
    {
        if (nota >= Minimo && nota <= Maximo) // dois testes
            cout << nota << " eh aceitavel.\n";
        else
            cout << nota << " nao eh aceitavel.\n";
        cin >> nota; // obtem proximo valor
    }
    cout << "Tchau.\n";
    return 0;
}
```

Aqui esta uma execucao de exemplo do programa na Listagem 6.5:

```
Digite a pontuacao NAAQ (0-10), (-1 encerra):
8
8 eh aceitavel.
3
3 nao eh aceitavel.
10
10 nao eh aceitavel.
5
5 eh aceitavel.
-1
Tchau.
```

A instrucao `if` do programa usa o operador `&&` para combinar dois testes em uma instrucao `if` em vez de usar um `if` aninhado dentro de outro `if`. O operador `&&` tem precedencia menor do que os operadores relacionais `<` e `>=`, portanto a expressao nao precisa de parenteses — mas eles sao permitidos, claro.

### Configurando Intervalos com &&

O operador `&&` tambem permite que voce estabeleca uma serie de intervalos em um programa. A Listagem 6.6 cria intervalos usando a construcao `if else if else` junto com `&&`. Ela faz com que o usuario insira um maximo de quatro tentativas para qualificar (qualify) um candidato para um emprego. Uma qualificacao requer que a idade esteja entre 17 e 35 anos e que o pontuacao esteja em 50 ou mais.

**Listagem 6.6 — more_and.cpp**

```cpp
// more_and.cpp -- mais sobre o operador AND
#include <iostream>
const int ErroArq = 20; // numero de arquivo de pontuacao
const int Aprovado = 50;
const int IdadeMin = 17;
const int IdadeMax = 35;
int main()
{
    using namespace std;
    int qualifica[ErroArq] = {0};
    int age;
    int pontuacao;
    int i = 0;
    int max = ErroArq;
    cout << "Entre age e score (ingresse -1 -1 para encerrar):\n";
    do
    {
        cin >> age >> pontuacao;
        if (age == -1) break;
        i++;
        if (age >= IdadeMin && age <= IdadeMax && pontuacao >= Aprovado)
        {
            cout << "Candidato qualificado\n";
            qualifica[i - 1] = 1;
        }
        else
        {
            cout << "Candidato nao qualificado\n";
            qualifica[i - 1] = 0;
        }
    } while (i < max);
    cout << i << " candidatos processados.\n";
    return 0;
}
```

Aqui esta uma execucao de exemplo do programa na Listagem 6.6:

```
Entre age e score (ingresse -1 -1 para encerrar):
25 75
Candidato qualificado
32 45
Candidato nao qualificado
15 80
Candidato nao qualificado
16 44
Candidato nao qualificado
-1 -1
4 candidatos processados.
```

Observe que neste caso o programa usa a instrucao `break` dentro de um loop `do while`, que voce vera em maior detalhe na Parte 5 deste capitulo. Quando `age` e `-1`, `break` termina o loop imediatamente.

---

## O Operador NOT Logico: !

O operador `!` nega, ou inverte, a verdade de uma expressao que o segue. Ou seja, se `expressao` for verdadeira, entao `!expressao` e falsa, e se `expressao` for falsa, entao `!expressao` e verdadeira. Mais precisamente, se `expressao` for `true` ou diferente de zero, entao `!expressao` e `false`. Se `expressao` for `false` ou zero, entao `!expressao` sera `true`.

Este operador e mais util quando voce quer testar a condicao "nao verdadeiro" de uma funcao que retorna um valor `true/false` (ou diferente de zero/zero). Por exemplo, `isalpha(ch)` retorna um valor diferente de zero se `ch` for uma letra e zero caso contrario. Voce pode usar `if (!isalpha(ch))` para testar o caso em que `ch` nao e uma letra. Vamos dar uma olhada em um programa de exemplo.

O programa na Listagem 6.7 pede ao usuario que insira um numero inteiro. A funcao `e_inteiro()` retorna `true` se `num` pode ser representado como um inteiro e `false` caso contrario. O programa usa `!e_inteiro(num)` como a condicao de teste do loop `while` para continuar solicitando ao usuario entradas validas ate que ele insira uma.

**Listagem 6.7 — not.cpp**

```cpp
// not.cpp -- usando o operador NOT
#include <iostream>
#include <climits>
bool e_inteiro(double num);
int main()
{
    using namespace std;
    double num;
    cout << "Suas atribuicoes inteiras\n";
    cout << "Digite um numero inteiro: ";
    cin >> num;
    while (!e_inteiro(num)) // continua enquanto nao for inteiro valido
    {
        cout << "Fora do intervalo -- tente novamente: ";
        cin >> num;
    }
    int val = (int) num;    // converte para int
    cout << "O valor inteiro e " << val << "\n";
    return 0;
}

bool e_inteiro(double num)
{
    if (num <= INT_MAX && num >= INT_MIN) // defincoes de climits
        return true;
    else
        return false;
}
```

Aqui esta uma execucao de exemplo do programa na Listagem 6.7:

```
Suas atribuicoes inteiras
Digite um numero inteiro: 6234128679
Fora do intervalo -- tente novamente: -8000222333
Fora do intervalo -- tente novamente: 99999
O valor inteiro e 99999
```

A funcao `e_inteiro()` usa o fato de que o arquivo de cabecalho `climits` define `INT_MAX` como o maior valor `int` e `INT_MIN` como o menor valor `int`. Se `num` se enquadrar nesse intervalo, a funcao retornara `true`; caso contrario, retornara `false`.

### Detalhes da Precedencia do Operador !

O operador `!` tem precedencia mais alta do que qualquer um dos operadores aritmeticos ou relacionais. Portanto, para negar uma expressao voce precisa colocar a expressao entre parenteses, assim:

```cpp
!(x > 5)   // nao e verdadeiro que x eh maior do que 5
!x > 5     // e verdadeiro que (!x) eh maior do que 5
```

Em geral, `x > 5` nao e o mesmo que `!(x > 5)`. Na segunda expressao, `!x` sera `0` (se `x` for diferente de zero) ou `1` (se `x` for `0`), e `!x > 5` sera sempre `false`.

---

## Fatos sobre os Operadores Logicos

Como voce ja viu em varios exemplos, os operadores relacionais tem precedencia maior do que os operadores logicos `&&` e `||`. Isso significa que expressoes como `x > 5 && y > 5` sao interpretadas como `(x > 5) && (y > 5)`.

O operador `!` tem precedencia mais alta do que qualquer operador aritmetico ou relacional. Portanto, para aplicar `!` a uma expressao, voce deve colocar a expressao entre parenteses.

O operador `&&` tem precedencia mais alta do que o operador `||`. Assim, a expressao:

```cpp
age > 30 && age < 45 || weight > 300
```

e interpretada como:

```cpp
(age > 30 && age < 45) || weight > 300
```

Isso e, a expressao e verdadeira se o `age` estiver no intervalo de 30 a 45, ou se `weight` for maior do que 300.

O C++ garante que as expressoes logicas sejam avaliadas da esquerda para a direita e que a avaliacao pare assim que a verdade ou a falsidade do resultado seja determinada. Essa garantia e importante se o lado direito de um operador logico tiver um efeito colateral como a modificacao de um valor:

```cpp
while (cin.get(ch) && !isspace(ch))
    conta++;
```

O programa chama `cin.get(ch)` e avalia a chamada como verdadeira somente se a leitura for bem-sucedida. Se a condicao for verdadeira, o programa avalia o lado direito: `!isspace(ch)`. Porque o C++ garante que o lado esquerdo seja avaliado primeiro e que a avaliacao pare se o lado esquerdo for `false`, voce pode ter certeza de que `!isspace(ch)` e avaliado apenas se `cin.get(ch)` for bem-sucedido. Sem essa garantia, `ch` poderia ser um valor indefinido quando `!isspace(ch)` o utilizasse.

---

## Representacoes Alternativas

Nem todos os teclados fornecem todos os simbolos usados para os operadores logicos, portanto o padrao C++ oferece representacoes alternativas, conforme mostrado na Tabela 6.3. As palavras reservadas `and`, `or` e `not` sao palavras reservadas do C++, o que significa que voce nao pode usa-las como nomes para outras entidades, como variaveis. Elas *nao* sao palavras reservadas do C, mas um programa C pode usa-las como operadores logicos se o programa incluir o arquivo de cabecalho `iso646.h`. O C++ nao requer o uso deste arquivo de cabecalho.

**Tabela 6.3 — Representacoes Alternativas dos Operadores Logicos**

| Operador | Representacao Alternativa |
|----------|--------------------------|
| `&&`     | `and`                    |
| `\|\|`   | `or`                     |
| `!`      | `not`                    |

---

> Navegacao: [Anterior](capitulo-06-01-instrucao-if.md) | [Indice](README.md) | [Proximo](capitulo-06-03-cctype-operador-condicional.md)
