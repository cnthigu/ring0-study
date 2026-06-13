# Capitulo 6 - Instrucoes de Ramificacao e Operadores Logicos
## Parte 3: A Biblioteca cctype e o Operador Condicional

> Traducao nao oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Navegacao: [Anterior](capitulo-06-02-operadores-logicos.md) | [Indice](README.md) | [Proximo](capitulo-06-04-instrucao-switch.md)

---

## A Biblioteca de Funcoes de Caractere cctype

O C++ herda do C uma familia conveniente de funcoes relacionadas a caracteres, com prototipo no arquivo de cabecalho `cctype` (o equivalente do C++ para o arquivo `ctype.h` do C). Essas funcoes facilitam a determinacao de se um caractere e uma letra maiuscula ou minuscula, ou um digito, ou pontuacao, e assim por diante. Alem disso, existem funcoes para converter entre letras maiusculas e minusculas. Esta familia de funcoes de caractere oferece um metodo mais conveniente e mais portatil de fazer essas tarefas do que o uso de operadores AND e OR.

Suponha, por exemplo, que voce queira escrever uma funcao que converta um caractere para maiusculo. Se voce usar o operador AND, teria que usar codigo parecido com:

```cpp
if (ch >= 'a' && ch <= 'z') // funciona apenas para o ASCII!
    ch -= 'a' - 'A';
```

Isso assume um conjunto de caracteres especifico no qual os codigos para letras minusculas e maiusculas diferem em um valor constante. Esse e o caso para o ASCII, mas nao e verdade em geral. A funcao `toupper()` da familia `cctype`, porem, funciona para todos os conjuntos de caracteres:

```cpp
ch = toupper(ch); // funciona para qualquer codigo de caractere
```

A Listagem 6.8 usa algumas dessas funcoes para analisar os conteudos da entrada do usuario.

**Listagem 6.8 — cctypes.cpp**

```cpp
// cctypes.cpp -- usando funcoes de caractere
#include <iostream>
#include <cctype>           // prototipo das funcoes de caractere
int main()
{
    using namespace std;
    cout << "Insira o texto para analise, e depois insira\n"
            " uma linha vazia para terminar:\n";
    char ch;
    int espacos_brancos = 0;
    int digitos = 0;
    int letras = 0;
    int pontuacao = 0;
    int outros = 0;
    cin.get(ch);            // le primeiro caractere
    while (ch != '\n')      // linha vazia termina o loop
    {
        if (isalpha(ch))    // eh uma letra?
            letras++;
        else if (isspace(ch)) // eh um espaco, nova linha ou tabulacao?
            espacos_brancos++;
        else if (isdigit(ch)) // eh um digito 0-9?
            digitos++;
        else if (ispunct(ch)) // eh pontuacao?
            pontuacao++;
        else
            outros++;
        cin.get(ch);        // le o proximo caractere
    }
    cout << letras << " letras, "
         << espacos_brancos << " espacos brancos, "
         << digitos << " digitos, "
         << pontuacao << " pontuacoes, "
         << outros << " outros.\n";
    return 0;
}
```

Aqui esta uma execucao de exemplo do programa na Listagem 6.8:

```
Insira o texto para analise, e depois insira
 uma linha vazia para terminar:
AdrenalEnergy@#$34
14 letras, 0 espacos brancos, 2 digitos, 3 pontuacoes, 0 outros.
```

A Tabela 6.4 descreve as funcoes da familia `cctype`.

**Tabela 6.4 — Funcoes da Biblioteca cctype**

| Funcao       | Retorna verdadeiro se...                                      |
|--------------|---------------------------------------------------------------|
| `isalnum(c)` | `c` for uma letra ou digito                                   |
| `isalpha(c)` | `c` for uma letra                                             |
| `iscntrl(c)` | `c` for um caractere de controle                              |
| `isdigit(c)` | `c` for um digito decimal (0-9)                               |
| `isgraph(c)` | `c` for um caractere imprimivel que nao seja espaco           |
| `islower(c)` | `c` for uma letra minuscula                                   |
| `isprint(c)` | `c` for um caractere imprimivel (incluindo espaco)            |
| `ispunct(c)` | `c` for um caractere de pontuacao                             |
| `isspace(c)` | `c` for um caractere de espaco em branco (espaco, avanco de formulario, nova linha, retorno de carro, tabulacao horizontal ou vertical) |
| `isupper(c)` | `c` for uma letra maiuscula                                   |
| `isxdigit(c)`| `c` for um caractere hexadecimal                              |
| `tolower(c)` | Retorna a versao minuscula de `c` se `c` for uma letra maiuscula; caso contrario, retorna `c` sem modificacao |
| `toupper(c)` | Retorna a versao maiuscula de `c` se `c` for uma letra minuscula; caso contrario, retorna `c` sem modificacao |

---

## O Operador ? :

O C++ tem um operador que pode frequentemente ser usado em vez de instrucoes `if else`. Esse operador e chamado de **operador condicional** (conditional operator), `?:`, e, sendo o unico operador do C++ com tres operandos, e chamado de **operador ternario** (ternary operator). Aqui esta a sintaxe geral para o operador condicional:

```
expressao1 ? expressao2 : expressao3
```

Se `expressao1` for verdadeira, o valor da expressao condicional inteira e o valor de `expressao2`. Caso contrario, o valor da expressao inteira e o valor de `expressao3`. Aqui esta um exemplo de uso tipico:

```cpp
a = (b < 0) ? -b : b; // valor absoluto de b
```

Se `b < 0`, entao `-b` e calculado e atribuido a `a`. Se `b` nao for negativo, entao `b` e atribuido a `a`. Esse fragmento de codigo faz o mesmo que o seguinte:

```cpp
if (b < 0)
    a = -b;
else
    a = b;
```

O operador `?:` oferece uma forma compacta de codificar uma escolha de dois valores. Programadores experientes em C e C++ usam esse operador extensamente, mas voce nao precisa se preocupar em usa-lo se achar que o formato `if else` e mais claro.

A Listagem 6.9 usa o operador condicional para determinar o maior de dois valores.

**Listagem 6.9 — condit.cpp**

```cpp
// condit.cpp -- usando o operador condicional
#include <iostream>
int main()
{
    using namespace std;
    int a, b, c;
    cout << "Insira dois numeros inteiros: ";
    cin >> a >> b;
    cout << "O maior dos dois numeros e ";
    c = (a > b) ? a : b; // usa o operador condicional
    cout << c << endl;
    return 0;
}
```

Aqui esta uma saida de exemplo do programa na Listagem 6.9:

```
Insira dois numeros inteiros: 25 28
O maior dos dois numeros e 28
```

A instrucao de atribuicao na Listagem 6.9 avalia a expressao `a > b`. Se esta expressao for verdadeira (nao zero), a expressao inteira obtém o valor `a` e `a` e atribuido a `c`. Se `a > b` for falsa (zero), a expressao inteira obtém o valor `b` e `b` e atribuido a `c`.

Muitas vezes o operador condicional e usado diretamente como argumento para funcao, em vez de primeiro atribuir o resultado a uma variavel:

```cpp
cout << ((a > b) ? a : b);  // imprime o maior de a ou b
```

(Os parenteses externos sao opcionais, mas sao uma boa pratica.) O operador `?:` e frequentemente um recurso importante na escrita de codigo C++ conciso.

---

> Navegacao: [Anterior](capitulo-06-02-operadores-logicos.md) | [Indice](README.md) | [Proximo](capitulo-06-04-instrucao-switch.md)
