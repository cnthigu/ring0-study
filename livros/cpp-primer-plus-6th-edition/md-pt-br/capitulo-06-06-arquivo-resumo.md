# Capitulo 6 - Instrucoes de Ramificacao e Operadores Logicos
## Parte 6: Entrada/Saida em Arquivos, Resumo e Revisao

> Traducao nao oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Navegacao: [Anterior](capitulo-06-05-break-continue-loops-numericos.md) | [Indice](README.md) | [Proximo](capitulo-07-01-funcoes.md)

---

## Entrada/Saida em Arquivos de Texto Simples

Ate agora, o uso de E/S do C++ por este livro se limitou a `cin`, para entrada, e `cout`, para saida. Voce pode adaptar facilmente a abordagem do C++ para trabalhar com arquivos. Esta secao mostra como.

### Escrita em Arquivos de Texto

Para E/S de arquivo, o C++ usa conceitos semelhantes aos de E/S em console. Voce usa um objeto de fluxo (stream object) para representar o arquivo, e voce usa metodos desse objeto para ler e escrever no arquivo.

A E/S de arquivo requer alguns passos:
1. Incluir o arquivo de cabecalho `fstream`.
2. Criar objetos de fluxo de arquivo (`ofstream` para saida ou `ifstream` para entrada).
3. Associar esses objetos com arquivos especificos (abrir os arquivos).
4. Usar os objetos de fluxo da mesma forma que usaria `cin` e `cout`.

Para criar um objeto de fluxo de arquivo para escrita, voce usa a classe `ofstream` (output file stream). Para abrir um arquivo especifico, voce pode usar o metodo `open()` ou fornecer o nome do arquivo no construtor:

```cpp
ofstream arquivoSaida;          // cria objeto ofstream
arquivoSaida.open("peixe.txt"); // associa ao arquivo peixe.txt
```

Ou combina a criacao e abertura em um unico passo:

```cpp
ofstream arquivoSaida("peixe.txt"); // cria objeto e abre arquivo
```

Uma vez que o arquivo esteja aberto, voce usa o objeto da mesma forma que usaria `cout`:

```cpp
arquivoSaida << "Isso eh escrito no arquivo.\n";
```

Quando voce terminar, pode fechar o arquivo:

```cpp
arquivoSaida.close();
```

Nao ha necessidade de fechar o arquivo explicitamente — ele sera fechado automaticamente quando o programa terminar e o objeto `ofstream` for destruido. Porem, fechar o arquivo explicitamente garante que os dados sejam liberados (flushed) para o arquivo.

A Listagem 6.15 coloca esses conceitos em pratica ao escrever informacoes em um arquivo de texto.

**Listagem 6.15 — outfile.cpp**

```cpp
// outfile.cpp -- escrevendo em um arquivo
#include <iostream>
#include <fstream>           // suporte para E/S de arquivo
int main()
{
    using namespace std;
    char automovel[50];
    int ano;
    double preco_pedido;
    double preco_desconto;
    ofstream arquivoSaida;   // cria objeto para output de arquivo
    arquivoSaida.open("carro.txt"); // associa arquivoSaida com carro.txt
    cout << "Insira a marca do automovel: ";
    cin.getline(automovel, 50);
    cout << "Insira o ano do modelo: ";
    cin >> ano;
    cout << "Insira o preco pedido: $";
    cin >> preco_pedido;
    preco_desconto = 0.9 * preco_pedido;
    // send information to file
    arquivoSaida << "Automovel: " << automovel << endl;
    arquivoSaida << "Ano: " << ano << endl;
    arquivoSaida << "Preco pedido: $" << preco_pedido << endl;
    arquivoSaida << "Preco negociado: $" << preco_desconto << endl;
    arquivoSaida.close();     // fecha o arquivo
    cout << "Pronto.\n";
    return 0;
}
```

Aqui esta uma saida de exemplo do programa na Listagem 6.15:

```
Insira a marca do automovel: Flitz Perky
Insira o ano do modelo: 2009
Insira o preco pedido: $13500
Pronto.
```

Apos executar o programa, voce pode abrir o arquivo `carro.txt` e ver algo parecido com isto:

```
Automovel: Flitz Perky
Ano: 2009
Preco pedido: $13500
Preco negociado: $12150
```

A parte mais importante do programa e essa: apos criar o objeto `ofstream` chamado `arquivoSaida` e associa-lo ao arquivo `carro.txt`, voce pode usar `arquivoSaida` exatamente da mesma forma que usaria `cout`. Todo o codigo de formatacao familiar, todos os metodos de iostream, todos os operadores de insercao (`<<`) — eles funcionam da mesma forma que funcionam para `cout`. A classe `ofstream` usa os mecanismos de `ostream` como base de heranca, portanto herda todo esse comportamento.

Se voce tentar abrir um arquivo para saida que nao existe, o `open()` cria um novo arquivo. Se o arquivo ja existir, `open()` o trunca — ou seja, descarta o conteudo existente antes de escrever no arquivo. Mais tarde, o Capitulo 17, "Entrada, Saida e Arquivos", discute opcoes adicionais de abertura de arquivo, como aquelas que permitem adicionar a um arquivo existente.

### Lendo de Arquivos de Texto

Para leitura de arquivo, o C++ usa a classe `ifstream` (input file stream). Voce cria um objeto `ifstream` e o abre da mesma forma que um objeto `ofstream`, e depois usa os metodos e operadores de `istream` (como `>>` e `get()`) para ler o arquivo. Fechar o arquivo e feito com o metodo `close()`.

Alguns metodos e funcoes que voce pode precisar ao ler de um arquivo:

- `ifstream::is_open()` — retorna `true` se o arquivo for aberto com sucesso
- `ifstream::eof()` — retorna `true` se o fim do arquivo (EOF) foi atingido
- `ifstream::fail()` — retorna `true` se uma operacao de E/S falhou
- `ifstream::good()` — retorna `true` se o stream estiver em bom estado

A Listagem 6.16 ilustra como ler do arquivo `carro.txt` que a Listagem 6.15 criou.

**Listagem 6.16 — sumafile.cpp**

```cpp
// sumafile.cpp -- lendo de um arquivo
#include <iostream>
#include <fstream>         // suporte para E/S de arquivo
#include <cstdlib>         // suporte para a funcao exit()
const int TamBuf = 60;
int main()
{
    using namespace std;
    char nomeArquivo[TamBuf];
    ifstream arquivoEntrada;    // cria objeto ifstream
    cout << "Insira o nome do arquivo de dados: ";
    cin.getline(nomeArquivo, TamBuf);
    arquivoEntrada.open(nomeArquivo); // associa arquivoEntrada com arquivo
    if (!arquivoEntrada.is_open())    // falhou ao abrir?
    {
        cout << "Nao foi possivel abrir o arquivo " << nomeArquivo << endl;
        cout << "O programa sera encerrado.\n";
        exit(EXIT_FAILURE);           // termina o programa
    }
    double valor;
    double soma = 0.0;
    int contagem = 0;              // numero de itens lidos
    arquivoEntrada >> valor;       // obtem primeiro valor
    while (arquivoEntrada.good())  // enquanto entrada boa e nao-EOF
    {
        ++contagem;                // incrementa contagem
        soma += valor;             // calcula a soma cumulativa
        arquivoEntrada >> valor;   // obtem proximo valor
    }
    if (arquivoEntrada.eof())
        cout << "Fim do arquivo atingido.\n";
    else if (arquivoEntrada.fail())
        cout << "Entrada terminada por incompatibilidade de tipo.\n";
    else
        cout << "Entrada terminada por razao desconhecida.\n";
    if (contagem == 0)
        cout << "Sem dados processados.\n";
    else
    {
        cout << "Itens lidos: " << contagem << endl;
        cout << "Soma: " << soma << endl;
        cout << "Media: " << soma / contagem << endl;
    }
    arquivoEntrada.close();         // fecha o arquivo — opcional
    return 0;
}
```

Aqui esta uma execucao de exemplo do programa na Listagem 6.16. Suponha que voce crie um arquivo de dados chamado `scores.txt` contendo os numeros:

```
18 19 18.5 13.5 14
16 19.5 20 18 12 18.5
17.5
```

E entao execute o programa:

```
Insira o nome do arquivo de dados: scores.txt
Fim do arquivo atingido.
Itens lidos: 12
Soma: 204
Media: 17
```

O programa declara o objeto `arquivoEntrada` do tipo `ifstream`. Diferentemente do `cin`, que esta conectado a uma fonte de entrada padrao, `arquivoEntrada` precisa ser conectado a um arquivo especifico. O metodo `open()` faz isso.

O programa chama `arquivoEntrada.is_open()` para verificar se o arquivo foi aberto com sucesso. Se o arquivo nao puder ser aberto — porque nao existe ou os direitos de acesso nao permitem isso — o metodo retornara `false` e o programa termina com `exit(EXIT_FAILURE)`. A funcao `exit()` e declarada no arquivo de cabecalho `cstdlib` e termina o programa.

O programa le valores do arquivo para `valor` e os acumula em `soma`. A expressao `arquivoEntrada.good()` retorna `true` se a ultima operacao de E/S foi bem-sucedida e o fim do arquivo ainda nao foi atingido. Apos o loop, o programa usa `eof()` e `fail()` para diagnosticar a razao pela qual o loop terminou.

O programa fecha o arquivo com `arquivoEntrada.close()`. Voce poderia omitir esta chamada porque o arquivo e fechado automaticamente quando o objeto `ifstream` e destruido, mas e boa pratica fechar o arquivo explicitamente.

---

## Resumo

O C++ fornece instrucoes `if`, `if else` e `if else if else` para permitir que um programa execute blocos alternativos de codigo com base em uma condicao de teste. Uma condicao de teste avalia como `true` ou `false`. No C++, inteiros nao zero sao `true` e inteiros zero sao `false`. Expressoes relacionais, que comparam dois valores, frequentemente sao usadas como condicoes de teste para instrucoes `if`.

A execucao repetida de codigo com instrucoes `if else` pode construir uma cascata de alternativas. Quando uma variavel inteira e testada em relacao a varios valores constantes inteiros, a instrucao `switch` e uma alternativa a `if else if else`. Uma instrucao `switch` usa um valor de variavel inteira para saltar para o rotulo `case` correspondente.

Os operadores logicos `&&`, `||` e `!` permitem que voce combine duas expressoes relacionais em uma expressao mais complexa. O operador `&&` (AND logico) significa que ambas as subexpressoes devem ser verdadeiras para que a expressao composta seja verdadeira. O operador `||` (OR logico) significa que pelo menos uma das subexpressoes deve ser verdadeira para que a expressao composta seja verdadeira. O operador `!` (NOT logico) inverte o valor de verdade da expressao que se segue.

O operador condicional `?:` fornece uma maneira de expressar uma de duas alternativas, dependendo do valor de uma expressao de teste.

A familia de funcoes `cctype`, como `isalpha()` e `isdigit()`, fornece ferramentas convenientes para classificar e converter caracteres.

As instrucoes `break` e `continue` fornecem controle adicional de programa. Quando um programa chega a `break` em um loop ou instrucao `switch`, ele sai do loop ou instrucao `switch`. Quando um programa chega a `continue` em um loop, ele pula para a avaliacao da condicao de teste do loop.

E/S de arquivo de texto e muito semelhante a E/S de console. Voce usa um objeto `ofstream` para saida de arquivo e usa operadores e metodos de `ostream` com ele. Voce usa um objeto `ifstream` para entrada de arquivo e usa operadores e metodos de `istream` com ele. Voce pode testar um objeto `ifstream` para detectar EOF e outras condicoes.

---

## Revisao do Capitulo

**1.** Considere o seguinte fragmento de codigo:

```cpp
if (x > 0)
    cout << "x e positivo\n";
else if (x < 0)
    cout << "x e negativo\n";
else
    cout << "x e zero\n";
```

Que saida, se houver, seria produzida para cada um dos seguintes valores de `x`: 100, –4, 0, 0.5, e –0.5?

**2.** Escreva uma instrucao `if` `else` para imprimir `Gigante Gentil` se uma variavel `weight` for maior do que 400, e para imprimir `Criatura Normal` caso contrario.

**3.** Um artista de circo que usa macrobiotics tem um programa de computador para monitorar sua ingesta de alimentos durante a semana. Um de seus calculos envolve calcular a media de um par de semanas de quantidades de alimentos. O artista coloca seus dados em um array de `double`. Escreva uma instrucao `if else` que, dependendo se o artista quer a media da semana 1 ou 2 (opcoes 1 e 2), calcule a media dos cinco primeiros ou dos cinco ultimos elementos do array. Assuma que o array e chamado `macros[10]`, com os valores 1–5 representando a semana 1 e os valores 6–10 representando a semana 2.

**4.** Reescreva o Exercicio 3 usando o operador condicional `?:` em vez de `if` `else`.

**5.** Quais sao os erros no seguinte fragmento?

```cpp
int x;
cin >> x;
if (x == 1)
{
    cout << "Uma Morcela";
}
else if (x == 2)
{
    cout << "Dois Quesos";
}
else if (x == 3)
{
    cout << "Tres Coisas Mais";
}
```

**6.** A instrucao `if else if else` na Questao 5 pode ser convertida em uma instrucao `switch`? Se sim, escreva o equivalente com `switch`.

**7.** Quanto ao seguinte fragmento de codigo, o que voce sabe sobre `k` se o programa imprimir `No Problem` para a entrada `3`?

```cpp
char ch;
int k;
cin >> ch;
switch (ch)
{
    case 'A': k = 0;
    case 'B': k = 1;
    case 'C': k = 2;
    case 'D': k = 3;
    default : k = 4;
}
```

**8.** O seguinte fragmento de codigo e valido? Se sim, o que exatamente ele imprime?

```cpp
int i = 0;
while (i++ < 6)
{
    i += 5;
    cout << i << "\n";
    continue;
}
```

**9.** Considere o seguinte fragmento de codigo:

```cpp
int x;
cin >> x;
if (x > 0 && x < 10)
    cout << "1 ";
if (x > 5 && x < 15)
    cout << "2 ";
```

Que saida, se houver, seria produzida para `x = 6`? Para `x = -3`? Para `x = 12`?

---

## Exercicios de Programacao

**1.** Escreva um programa que le `cin` para obter um inteiro e entao imprime todos os inteiros no intervalo do inteiro ao inteiro somado com 9. Por exemplo, se o usuario inserir `20`, o programa listara de 20 a 29. Nao esqueca de colocar um espaco ou nova linha entre os numeros.

**2.** Escreva um programa que pede para o usuario inserir dois inteiros. Para cada inteiro, o programa deve relatar se e positivo, negativo ou zero. O programa tambem deve relatar qual inteiro e maior. Se forem iguais, o programa deve relatar isso.

**3.** Escreva um programa que le palavras, uma por linha, terminando com o primeiro que comeca com `q`. O programa deve contar o numero de vogais e consoantes nas palavras, e entao exibir os totais.

**4.** Escreva um programa que pede para o usuario inserir numeros inteiros entre 1 e 10, inclusive. Se o usuario inserir um numero fora desse intervalo, o programa deve informar o usuario e tentar novamente. O programa deve continuar ate que o usuario insira `q`. Use um loop `do while`.

**5.** Modifique o Exercicio 4 para fazer com que o programa calcule a soma dos inteiros inseridos. Use um loop `while`, nao `do while`.

**6.** Escreva um programa que leia um array de inteiros de um arquivo de texto. O arquivo deve conter 5 inteiros, um por linha. O programa deve exibir a soma desses numeros.

**7.** Um numero e dito ser par se for divisivel por 2. Escreva um programa que leia uma serie de inteiros do usuario (terminando quando o usuario inserir `0`) e exiba a quantidade de numeros pares e impares na entrada.

**8.** Escreva um programa para calcular o seguinte: o usuario insere o salario base de um funcionario (por hora), as horas trabalhadas, e a aliquota de imposto de renda. O salario total e calculado como: horas normais ate 40 pagas normalmente; horas acima de 40 pagas a uma taxa de 1,5 vezes o normal. O programa deve calcular e exibir a paga bruta, o valor do imposto e a paga liquida.

**9.** Escreva um programa que gera um numero aleatorio entre 1 e 100 (inclusive) e pede ao usuario para adivinhar o numero. Se o usuario adivinhar errado, o programa deve dizer se o palpite e muito alto ou muito baixo. O programa deve continuar ate que o usuario adivinhe corretamente ou ate que o usuario insira um numero negativo para encerrar. Ao final, o programa deve relatar quantos palpites o usuario fez.

---

> Navegacao: [Anterior](capitulo-06-05-break-continue-loops-numericos.md) | [Indice](README.md) | [Proximo](capitulo-07-01-funcoes.md)
