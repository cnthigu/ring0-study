# Capitulo 7 - Funcoes: Modulos de Programacao do C++
## Parte 3: Funcoes com Strings, Estruturas e Objetos string

> Traducao nao oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Navegacao: [Anterior](capitulo-07-02-funcoes-arrays.md) | [Indice](README.md) | [Proximo](capitulo-07-04-recursao-ponteiros-resumo.md)

---

## Funcoes e Strings no Estilo C

Lembre-se de que uma string no estilo C consiste em uma serie de caracteres terminada pelo caractere nulo. Muito do que voce aprendeu sobre como projetar funcoes de array se aplica a funcoes de string, tambem. Por exemplo, passar uma string como argumento significa passar um endereco, e voce pode usar `const` para proteger um argumento de string de ser alterado. Mas ha algumas peculiaridades especiais das strings que vamos desvendar agora.

### Funcoes com Argumentos de String no Estilo C

Suponha que voce queira passar uma string como argumento para uma funcao. Voce tem tres opcoes para representar uma string:

- Um array de `char`
- Uma constante de string entre aspas (tambem chamada de literal de string)
- Um ponteiro-para-`char` definido para o endereco de uma string

Todas as tres opcoes, no entanto, sao do tipo ponteiro-para-`char` (mais concisamente, tipo `char *`), portanto voce pode usar todas as tres como argumentos para funcoes de processamento de strings:

```cpp
char fantasma[15] = "cavalgando";
char * grito = "ululando";
int n1 = strlen(fantasma);   // fantasma e &fantasma[0]
int n2 = strlen(grito);      // ponteiro para char
int n3 = strlen("gamboleando"); // endereco de string
```

Informalmente, voce pode dizer que esta passando uma string como um argumento, mas na verdade esta passando o endereco do primeiro caractere na string.

Uma diferenca importante entre uma string no estilo C e um array regular e que a string tem um caractere de terminacao incorporado. Isso significa que voce nao tem que passar o tamanho da string como argumento. Em vez disso, a funcao pode usar um loop para examinar cada caractere na string por vez ate o loop atingir o caractere nulo de terminacao.

A Listagem 7.9 ilustra essa abordagem com uma funcao que conta o numero de vezes que um determinado caractere aparece em uma string.

**Listagem 7.9 — strgfun.cpp**

```cpp
// strgfun.cpp -- funcoes com um argumento de string
#include <iostream>
unsigned int c_na_str(const char * str, char ch);
int main()
{
    using namespace std;
    char mmm[15] = "minimo"; // string em um array
    char *grito = "ululou";  // grito aponta para string
    unsigned int ms = c_na_str(mmm, 'm');
    unsigned int us = c_na_str(grito, 'u');
    cout << ms << " caracteres m em " << mmm << endl;
    cout << us << " caracteres u em " << grito << endl;
    return 0;
}
// esta funcao conta o numero de caracteres ch
// na string str
unsigned int c_na_str(const char * str, char ch)
{
    unsigned int contagem = 0;
    while (*str) // termina quando *str e '\0'
    {
        if (*str == ch)
            contagem++;
        str++; // move o ponteiro para o proximo char
    }
    return contagem;
}
```

Aqui esta a saida do programa na Listagem 7.9:

```
3 caracteres m em minimo
2 caracteres u em ululou
```

Como a funcao `c_na_str()` nao deve alterar a string original, ela usa o modificador `const` quando declara o parametro formal `str`. A funcao em si demonstra uma maneira padrao de processar os caracteres em uma string:

```cpp
while (*str)
{
    instrucoes
    str++;
}
```

Inicialmente, `str` aponta para o primeiro caractere na string, portanto `*str` representa o proprio primeiro caractere. Enquanto o caractere nao for o caractere nulo (`\0`), `*str` e diferente de zero, portanto o loop continua. No final de cada loop, a expressao `str++` incrementa o ponteiro em 1 byte para que aponte para o proximo caractere na string. Por fim, `str` aponta para o caractere nulo de terminacao, tornando `*str` igual a 0, que e o codigo numerico do caractere nulo. Essa condicao encerra o loop.

### Funcoes que Retornam Strings no Estilo C

Agora suponha que voce queira escrever uma funcao que retorne uma string. Bem, uma funcao nao pode fazer isso. Mas ela pode retornar o endereco de uma string, e isso e mais eficiente. A Listagem 7.10, por exemplo, define uma funcao chamada `construir_str()` que retorna um ponteiro. Esta funcao recebe dois argumentos: um caractere e um numero. Usando `new`, a funcao cria uma string cujo comprimento e igual ao numero, e entao inicializa cada elemento com o caractere. Em seguida, retorna um ponteiro para a nova string.

**Listagem 7.10 — strgback.cpp**

```cpp
// strgback.cpp -- uma funcao que retorna um ponteiro para char
#include <iostream>
char * construir_str(char c, int n); // prototipo
int main()
{
    using namespace std;
    int vezes;
    char ch;
    cout << "Digite um caractere: ";
    cin >> ch;
    cout << "Digite um inteiro: ";
    cin >> vezes;
    char *ps = construir_str(ch, vezes);
    cout << ps << endl;
    delete [] ps;               // libera memoria
    ps = construir_str('+', 20); // reutiliza o ponteiro
    cout << ps << "-CONCLUIDO-" << ps << endl;
    delete [] ps;               // libera memoria
    return 0;
}
// constroi string composta de n caracteres c
char * construir_str(char c, int n)
{
    char * pstr = new char[n + 1];
    pstr[n] = '\0'; // termina a string
    while (n-- > 0)
        pstr[n] = c; // preenche o resto da string
    return pstr;
}
```

Aqui esta uma execucao de exemplo do programa na Listagem 7.10:

```
Digite um caractere: V
Digite um inteiro: 46
VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV
++++++++++++++++++++-CONCLUIDO-++++++++++++++++++++
```

Para criar uma string de `n` caracteres visiveis, voce precisa de armazenamento para `n + 1` caracteres a fim de ter espaco para o caractere nulo. Entao a funcao pede `n + 1` bytes para armazenar a string. Em seguida, define o byte final como o caractere nulo. Entao preenche o restante do array de tras para frente. No inicio do ciclo final, `n` tem o valor 1. Como `n--` significa usar o valor e entao decrementar, a condicao de teste do loop `while` compara 1 com 0, encontra o teste verdadeiro e continua. Mas apos fazer o teste, a funcao decrementa `n` para 0, portanto `pstr[0]` e o ultimo elemento definido como `c`.

Note que a variavel `pstr` e local para a funcao `construir_str`, portanto quando aquela funcao termina, a memoria usada para `pstr` (mas nao para a string) e liberada. Mas porque a funcao retorna o valor de `pstr`, o programa e capaz de acessar a nova string atraves do ponteiro `ps` em `main()`.

A desvantagem deste tipo de design (ter uma funcao retornar um ponteiro para memoria alocada por `new`) e que faz com que seja responsabilidade do programador lembrar de usar `delete`. No Capitulo 12, "Classes e Alocacao Dinamica de Memoria", voce vera como as classes C++, usando construtores e destrutores, podem cuidar desses detalhes para voce.

---

## Funcoes e Estruturas

Vamos passar de arrays para estruturas. E mais facil escrever funcoes para estruturas do que para arrays. Embora variaveis de estrutura se assemelhem a arrays por conter varios itens de dados, variaveis de estrutura se comportam como variaveis basicas de valor unico quando se trata de funcoes. Ou seja, ao contrario de um array, uma estrutura amarra seus dados em uma unica entidade, ou objeto de dados, que sera tratado como uma unidade. Lembre-se de que voce pode atribuir uma estrutura a outra. Da mesma forma, voce pode passar estruturas por valor, assim como voce faz com variaveis comuns. Nesse caso, a funcao trabalha com uma copia da estrutura original. Alem disso, uma funcao pode retornar uma estrutura. O nome de uma estrutura e simplesmente o nome da estrutura, e se voce quiser seu endereco, voce tem que usar o operador de endereco `&`.

A maneira mais direta de programar usando estruturas e trata-las como voce trataria os tipos basicos — ou seja, passa-las como argumentos e usa-las, se necessario, como valores de retorno. No entanto, ha uma desvantagem em passar estruturas por valor. Se a estrutura for grande, o espaco e o esforco envolvidos em fazer uma copia de uma estrutura podem aumentar os requisitos de memoria e diminuir a velocidade do sistema. Por esses motivos, muitos programadores C preferem passar o endereco de uma estrutura e depois usar um ponteiro para acessar o conteudo da estrutura.

### Passando e Retornando Estruturas

Passar estruturas por valor faz mais sentido quando a estrutura e relativamente compacta. Vamos olhar para um exemplo que lida com tempo de viagem. Suponha que um mapa lhe diga que e 3 horas e 50 minutos de Thunder Falls ate Bingo City e 1 hora e 25 minutos de Bingo City ate Grotesquo. Voce pode usar uma estrutura para representar tais tempos, usando um membro para o valor de hora e um segundo membro para o valor de minuto. Adicionar dois tempos e um pouco complicado porque voce pode ter que transferir alguns dos minutos para a parte de horas.

**Listagem 7.11 — travel.cpp**

```cpp
// travel.cpp -- usando estruturas com funcoes
#include <iostream>
struct tempo_viagem
{
    int horas;
    int minutos;
};
const int Mins_por_hora = 60;
tempo_viagem somar(tempo_viagem t1, tempo_viagem t2);
void mostrar_tempo(tempo_viagem t);
int main()
{
    using namespace std;
    tempo_viagem dia1 = {5, 45}; // 5 hrs, 45 min
    tempo_viagem dia2 = {4, 55}; // 4 hrs, 55 min
    tempo_viagem viagem = somar(dia1, dia2);
    cout << "Total de dois dias: ";
    mostrar_tempo(viagem);
    tempo_viagem dia3 = {4, 32};
    cout << "Total de tres dias: ";
    mostrar_tempo(somar(viagem, dia3));
    return 0;
}
tempo_viagem somar(tempo_viagem t1, tempo_viagem t2)
{
    tempo_viagem total;
    total.minutos = (t1.minutos + t2.minutos) % Mins_por_hora;
    total.horas = t1.horas + t2.horas +
                  (t1.minutos + t2.minutos) / Mins_por_hora;
    return total;
}
void mostrar_tempo(tempo_viagem t)
{
    using namespace std;
    cout << t.horas << " horas, "
         << t.minutos << " minutos\n";
}
```

Aqui esta a saida do programa na Listagem 7.11:

```
Total de dois dias: 10 horas, 40 minutos
Total de tres dias: 15 horas, 12 minutos
```

Aqui `tempo_viagem` age como um nome de tipo padrao; voce pode usa-lo para declarar variaveis, tipos de retorno de funcao e tipos de argumento de funcao. Como a funcao `somar()` retorna uma estrutura `tempo_viagem`, voce pode usa-la como argumento para a funcao `mostrar_tempo()`.

### Outro Exemplo de Uso de Funcoes com Estruturas

Este exemplo define duas estruturas representando duas maneiras diferentes de descrever posicoes e depois desenvolve funcoes para converter uma forma em outra e mostrar o resultado. Uma maneira de descrever a posicao de um ponto e declarar o deslocamento horizontal e o deslocamento vertical do ponto em relacao a uma origem. As variaveis `x` e `y` constituem as **coordenadas retangulares** (rectangular coordinates). Outra maneira de descrever a posicao e declarar o quao longe esta da origem e em que direcao esta. A distancia e o angulo juntos constituem as **coordenadas polares** (polar coordinates).

```cpp
struct polar
{
    double distancia; // distancia da origem
    double angulo;    // direcao da origem
};
struct retal
{
    double x; // distancia horizontal da origem
    double y; // distancia vertical da origem
};
```

As funcoes matematicas na biblioteca C++ assumem que os angulos estao em radianos, portanto voce precisa medir angulos nessa unidade. Para fins de exibicao, voce pode converter a medida de radianos para graus multiplicando por 180/π, que e aproximadamente 57.29577951.

A Listagem 7.12 mostra uma funcao que exibe o conteudo de uma estrutura `polar` e uma funcao que converte coordenadas retangulares em coordenadas polares.

**Listagem 7.12 — strctfun.cpp**

```cpp
// strctfun.cpp -- funcoes com um argumento de estrutura
#include <iostream>
#include <cmath>
// declaracoes de estrutura
struct polar
{
    double distancia; // distancia da origem
    double angulo;    // direcao da origem
};
struct retal
{
    double x; // distancia horizontal da origem
    double y; // distancia vertical da origem
};
// prototipos
polar retal_para_polar(retal pos_retal);
void mostrar_polar(polar pos_polar);
int main()
{
    using namespace std;
    retal rlugar;
    polar plugar;
    cout << "Digite os valores x e y: ";
    while (cin >> rlugar.x >> rlugar.y) // uso elegante de cin
    {
        plugar = retal_para_polar(rlugar);
        mostrar_polar(plugar);
        cout << "Proximos dois numeros (q para sair): ";
    }
    cout << "Concluido.\n";
    return 0;
}
// converte coordenadas retangulares em coordenadas polares
polar retal_para_polar(retal pos_retal)
{
    using namespace std;
    polar resposta;
    resposta.distancia =
        sqrt(pos_retal.x * pos_retal.x + pos_retal.y * pos_retal.y);
    resposta.angulo = atan2(pos_retal.y, pos_retal.x);
    return resposta; // retorna uma estrutura polar
}
// exibe coordenadas polares, convertendo angulo para graus
void mostrar_polar(polar pos_polar)
{
    using namespace std;
    const double Rad_para_grau = 57.29577951;
    cout << "distancia = " << pos_polar.distancia;
    cout << ", angulo = " << pos_polar.angulo * Rad_para_grau;
    cout << " graus\n";
}
```

Aqui esta uma execucao de exemplo do programa na Listagem 7.12:

```
Digite os valores x e y: 30 40
distancia = 50, angulo = 53.1301 graus
Proximos dois numeros (q para sair): -100 100
distancia = 141.421, angulo = 135 graus
Proximos dois numeros (q para sair): q
Concluido.
```

Observe como o programa usa `cin` para controlar um loop `while`:

```cpp
while (cin >> rlugar.x >> rlugar.y)
```

`cin` e um objeto da classe `istream`. O operador de extracao (`>>`) e projetado de tal forma que `cin >> rlugar.x` tambem e um objeto desse tipo. Quando voce aplica o operador de extracao ao objeto `cin >> rlugar.x` (como em `cin >> rlugar.x >> rlugar.y`), voce novamente obtem um objeto da classe `istream`. Assim, toda a expressao de teste do loop `while` eventualmente avalia para `cin`, que, como voce pode se lembrar, quando usado no contexto de uma expressao de teste, e convertido para um valor `bool` de `true` ou `false`, dependendo se a entrada foi bem-sucedida.

### Passando Enderecos de Estrutura

Suponha que voce queira economizar tempo e espaco passando o endereco de uma estrutura em vez de passar a estrutura inteira. Isso requer reescrever as funcoes para que usem ponteiros para estruturas. A Listagem 7.13 mostra o programa refeito.

**Listagem 7.13 — strctptr.cpp**

```cpp
// strctptr.cpp -- funcoes com argumentos de ponteiro para estrutura
#include <iostream>
#include <cmath>
// templates de estrutura
struct polar
{
    double distancia; // distancia da origem
    double angulo;    // direcao da origem
};
struct retal
{
    double x; // distancia horizontal da origem
    double y; // distancia vertical da origem
};
// prototipos
void retal_para_polar(const retal * pxy, polar * pda);
void mostrar_polar(const polar * pda);
int main()
{
    using namespace std;
    retal rlugar;
    polar plugar;
    cout << "Digite os valores x e y: ";
    while (cin >> rlugar.x >> rlugar.y)
    {
        retal_para_polar(&rlugar, &plugar); // passa enderecos
        mostrar_polar(&plugar);             // passa endereco
        cout << "Proximos dois numeros (q para sair): ";
    }
    cout << "Concluido.\n";
    return 0;
}
// exibe coordenadas polares, convertendo angulo para graus
void mostrar_polar(const polar * pda)
{
    using namespace std;
    const double Rad_para_grau = 57.29577951;
    cout << "distancia = " << pda->distancia;
    cout << ", angulo = " << pda->angulo * Rad_para_grau;
    cout << " graus\n";
}
// converte coordenadas retangulares em coordenadas polares
void retal_para_polar(const retal * pxy, polar * pda)
{
    using namespace std;
    pda->distancia =
        sqrt(pxy->x * pxy->x + pxy->y * pxy->y);
    pda->angulo = atan2(pxy->y, pxy->x);
}
```

Do ponto de vista do usuario, o programa na Listagem 7.13 se comporta como o da Listagem 7.12. A diferenca oculta e que a Listagem 7.12 trabalha com copias de estruturas, enquanto a Listagem 7.13 usa ponteiros, permitindo que as funcoes operem nas estruturas originais.

---

## Funcoes e Objetos da Classe string

Embora strings no estilo C e objetos da classe `string` sirvam muito ao mesmo proposito, um objeto da classe `string` e mais intimamente relacionado a uma estrutura do que a um array. Por exemplo, voce pode atribuir uma estrutura a outra estrutura e um objeto a outro objeto. Voce pode passar uma estrutura como uma entidade completa para uma funcao, e voce pode passar um objeto como uma entidade completa. Se voce precisar de varias strings, pode declarar um array unidimensional de objetos `string` em vez de um array bidimensional de `char`.

A Listagem 7.14 fornece um exemplo curto que declara um array de objetos `string` e passa o array para uma funcao que exibe o conteudo.

**Listagem 7.14 — topfive.cpp**

```cpp
// topfive.cpp -- lidando com um array de objetos string
#include <iostream>
#include <string>
using namespace std;
const int TAMANHO = 5;
void exibir(const string sa[], int n);
int main()
{
    string lista[TAMANHO]; // um array contendo 5 objetos string
    cout << "Digite seus " << TAMANHO
         << " fenomenos astronomicos favoritos:\n";
    for (int i = 0; i < TAMANHO; i++)
    {
        cout << i + 1 << ": ";
        getline(cin, lista[i]);
    }
    cout << "Sua lista:\n";
    exibir(lista, TAMANHO);
    return 0;
}
void exibir(const string sa[], int n)
{
    for (int i = 0; i < n; i++)
        cout << i + 1 << ": " << sa[i] << endl;
}
```

Aqui esta uma execucao de exemplo do programa na Listagem 7.14:

```
Digite seus 5 fenomenos astronomicos favoritos:
1: Nebulosa de Orion
2: M13
3: Saturno
4: Jupiter
5: Lua
Sua lista:
1: Nebulosa de Orion
2: M13
3: Saturno
4: Jupiter
5: Lua
```

O ponto principal a notar neste exemplo e que, alem da funcao `getline()`, este programa trata `string` como trataria qualquer um dos tipos integrados, como `int`. Se voce quiser um array de `string`, basta usar o formato usual de declaracao de array. Cada elemento do array `lista`, entao, e um objeto `string` e pode ser usado como tal.

---

## Funcoes e Objetos array

Objetos de classe em C++ sao baseados em estruturas, portanto algumas das mesmas consideracoes de programacao que se aplicam a estruturas tambem se aplicam a classes. Por exemplo, voce pode passar um objeto por valor para uma funcao, caso em que a funcao age em uma copia do objeto original. Como alternativa, voce pode passar um ponteiro para um objeto, o que permite que a funcao atue no objeto original.

Suponha que temos um objeto `array` destinado a armazenar valores de despesas para cada uma das quatro estacoes do ano:

```cpp
std::array<double, 4> gastos;
```

Se queremos uma funcao para exibir o conteudo de `gastos`, podemos passar `gastos` por valor:

```cpp
mostrar(gastos);
```

Mas se queremos uma funcao que modifique o objeto `gastos`, precisamos passar o endereco do objeto para a funcao:

```cpp
preencher(&gastos);
```

Como podemos declarar essas duas funcoes? O tipo de `gastos` e `array<double, 4>`, portanto isso deve aparecer nos prototipos:

```cpp
void mostrar(std::array<double, 4> da); // da e um objeto
void preencher(std::array<double, 4> * pa); // pa e um ponteiro para um objeto
```

A Listagem 7.15 apresenta o programa completo.

**Listagem 7.15 — arrobj.cpp**

```cpp
// arrobj.cpp -- funcoes com objetos array (C++11)
#include <iostream>
#include <array>
#include <string>
// dados constantes
const int Estacoes = 4;
const std::array<std::string, Estacoes> NomesEstacoes =
    {"Primavera", "Verao", "Outono", "Inverno"};
// funcao para modificar o objeto array
void preencher(std::array<double, Estacoes> * pa);
// funcao que usa o objeto array sem modificar
void mostrar(std::array<double, Estacoes> da);
int main()
{
    std::array<double, Estacoes> gastos;
    preencher(&gastos);
    mostrar(gastos);
    return 0;
}
void preencher(std::array<double, Estacoes> * pa)
{
    using namespace std;
    for (int i = 0; i < Estacoes; i++)
    {
        cout << "Digite as despesas de " << NomesEstacoes[i] << ": ";
        cin >> (*pa)[i];
    }
}
void mostrar(std::array<double, Estacoes> da)
{
    using namespace std;
    double total = 0.0;
    cout << "\nDESPESAS\n";
    for (int i = 0; i < Estacoes; i++)
    {
        cout << NomesEstacoes[i] << ": $" << da[i] << endl;
        total += da[i];
    }
    cout << "Total de Despesas: $" << total << endl;
}
```

Aqui esta uma execucao de exemplo:

```
Digite as despesas de Primavera: 212
Digite as despesas de Verao: 256
Digite as despesas de Outono: 208
Digite as despesas de Inverno: 244

DESPESAS
Primavera: $212
Verao: $256
Outono: $208
Inverno: $244
Total de Despesas: $920
```

Na ultima instrucao em `preencher()`, `pa` e um ponteiro para um objeto `array<double, 4>`, portanto `*pa` e o objeto, e `(*pa)[i]` e um elemento no objeto. Os parenteses sao necessarios por causa da precedencia do operador.

A funcao `mostrar()` tem uma desvantagem: `gastos` contem quatro valores `double` e e ineficiente criar um novo objeto daquele tamanho e copiar os valores de `gastos` nele. O problema piora se modificarmos o programa para lidar com despesas mensais ou diarias e expandirmos `gastos` de acordo.

A funcao `preencher()` evita esse problema de ineficiencia usando um ponteiro para que a funcao atue no objeto original. Mas isso tem um custo de notacao que torna a programacao mais complicada:

```cpp
preencher(&gastos); // nao esqueca o &
...
cin >> (*pa)[i];
```

Usar referencias, como discutido no Capitulo 8, ajuda a resolver ambos os problemas de eficiencia e notacao.

---

> Navegacao: [Anterior](capitulo-07-02-funcoes-arrays.md) | [Indice](README.md) | [Proximo](capitulo-07-04-recursao-ponteiros-resumo.md)
