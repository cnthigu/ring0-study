# Capitulo 4 - Tipos Compostos (Compound Types)
## Parte 6: Ponteiros e a Area de Armazenamento Livre

> Traducao nao oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Navegacao: [Anterior](capitulo-04-05-unioes-enumeracoes.md) | [Indice](README.md) | [Proximo](capitulo-04-07-alternativas-arrays.md)

---

## Ponteiros e a Area de Armazenamento Livre

O inicio do Capitulo 3 menciona tres propriedades fundamentais das quais um programa de computador deve controlar ao armazenar dados. Para poupar voce de ter que folhear de volta para aquele capitulo, aqui estao essas propriedades novamente:

- Onde a informacao esta armazenada
- Qual valor e mantido la
- Que tipo de informacao e armazenada

Voce usou uma estrategia para atingir esses fins: definir uma variavel simples. A instrucao de declaracao fornece o tipo e um nome simbolico para o valor. Ela tambem faz com que o programa aloque memoria para o valor e controle a localizacao internamente.

Vamos examinar agora uma segunda estrategia, uma que se torna particularmente importante no desenvolvimento de classes em C++. Essa estrategia e baseada em **ponteiros** (pointers), que sao variaveis que armazenam enderecos de valores em vez dos proprios valores. Mas antes de discutir ponteiros, vamos falar sobre como encontrar explicitamente enderecos para variaveis comuns. Voce simplesmente aplica o **operador de endereco**, representado por `&`, a uma variavel para obter sua localizacao; por exemplo, se `casa` e uma variavel, `&casa` e seu endereco. A Listagem 4.14 demonstra esse operador.

> **Ponteiros e a Filosofia do C++**
>
> A programacao orientada a objetos difere da programacao procedural tradicional porque o POO enfatiza a tomada de decisoes durante o tempo de execucao, em vez de durante o tempo de compilacao. Tempo de execucao (runtime) significa enquanto um programa esta em execucao, e tempo de compilacao (compile time) significa quando o compilador esta montando um programa. Uma decisao em tempo de execucao e como, quando estiver de ferias, escolher quais pontos turisticos visitar dependendo do tempo e do seu humor no momento, enquanto uma decisao em tempo de compilacao e mais como seguir uma programacao pre-definida, independentemente das condicoes.
>
> As decisoes em tempo de execucao fornecem a flexibilidade de se ajustar as circunstancias atuais. Por exemplo, considere a alocacao de memoria para um array. A maneira tradicional e declarar um array. Para declarar um array em C++, voce tem que se comprometer com um tamanho de array especifico. Assim, o tamanho do array e definido quando o programa e compilado; e uma decisao em tempo de compilacao. Talvez voce ache que um array de 20 elementos seja suficiente em 80% das vezes, mas que ocasionalmente o programa precisara lidar com 200 elementos. Para ficar seguro, voce usa um array com 200 elementos. Isso resulta no programa desperdicando memoria na maior parte do tempo em que e usado. O POO tenta tornar um programa mais flexivel adiando essas decisoes ate o tempo de execucao. Dessa forma, apos o programa estar em execucao, voce pode dizer que precisa de apenas 20 elementos em uma vez, ou que precisa de 205 elementos em outra.
>
> Em resumo, com o POO voce gostaria de tornar o tamanho do array uma decisao em tempo de execucao. Para tornar essa abordagem possivel, a linguagem tem que permitir que voce crie um array — ou o equivalente — enquanto o programa estiver em execucao. O metodo do C++, como voce vera em breve, envolve o uso da palavra-chave `new` para solicitar a quantidade correta de memoria e o uso de ponteiros para controlar onde a memoria recentemente alocada e encontrada.
>
> Tomar decisoes em tempo de execucao nao e exclusivo do POO. Mas o C++ torna a escrita do codigo um pouco mais direta do que o C.

A nova estrategia para lidar com dados armazenados inverte as coisas, tratando a localizacao como a quantidade nomeada e o valor como uma quantidade derivada. Um tipo especial de variavel — o ponteiro — armazena o endereco de um valor. Assim, o nome do ponteiro representa a localizacao. Aplicar o operador `*`, chamado de **operador de valor indireto** (indirect value) ou de **operador de desreferenciamento** (dereferencing operator), produz o valor na localizacao. (Sim, e o mesmo simbolo `*` usado para multiplicacao; o C++ usa o contexto para determinar se voce quer dizer multiplicacao ou desreferenciamento.) Suponha, por exemplo, que `valente` seja um ponteiro. Nesse caso, `valente` representa um endereco, e `*valente` representa o valor nesse endereco. A combinacao `*valente` se torna equivalente a uma variavel ordinaria do tipo `int`. A Listagem 4.15 demonstra essas ideias. Ela tambem mostra como declarar um ponteiro.

**Listagem 4.14 — address.cpp**

```cpp
// address.cpp -- usando o operador & para encontrar enderecos
#include <iostream>
int main()
{
    using namespace std;
    int rosquinhas = 6;
    double xicaras = 4.5;
    cout << "valor de rosquinhas = " << rosquinhas;
    cout << " e endereco de rosquinhas = " << &rosquinhas << endl;
    // NOTA: pode ser necessario usar unsigned (&rosquinhas)
    // e unsigned (&xicaras)
    cout << "valor de xicaras = " << xicaras;
    cout << " e endereco de xicaras = " << &xicaras << endl;
    return 0;
}
```

Aqui esta a saida do programa na Listagem 4.14 em um sistema:

```
valor de rosquinhas = 6 e endereco de rosquinhas = 0x0065fd40
valor de xicaras = 4.5 e endereco de xicaras = 0x0065fd44
```

A implementacao especifica de `cout` mostrada aqui usa notacao hexadecimal ao exibir valores de endereco porque e a notacao usual usada para especificar um endereco de memoria. (Algumas implementacoes usam notacao base 10.) Nossa implementacao armazena `rosquinhas` em uma localizacao de memoria mais baixa do que `xicaras`. A diferenca entre os dois enderecos e `0x0065fd44 - 0x0065fd40`, ou 4. Isso faz sentido porque `rosquinhas` e do tipo `int`, que usa 4 bytes. Sistemas diferentes, naturalmente, forneceram valores diferentes para o endereco. Tambem alguns podem armazenar `xicaras` primeiro, depois `rosquinhas`, resultando em uma diferenca de 8 bytes porque `xicaras` e `double`. E alguns podem nem usar localizacoes adjacentes.

Usando variaveis comuns, entao, trata-se o valor como uma quantidade nomeada e a localizacao como uma quantidade derivada. Agora vamos examinar a estrategia de ponteiros, uma que e essencial para a filosofia de gerenciamento de memoria do C++ (veja o quadro lateral "Ponteiros e a Filosofia do C++").

**Listagem 4.15 — pointer.cpp**

```cpp
// pointer.cpp -- nossa primeira variavel ponteiro
#include <iostream>
int main()
{
    using namespace std;
    int atualizacoes = 6;  // declara uma variavel
    int * p_atualizacoes;  // declara ponteiro para um int

    p_atualizacoes = &atualizacoes; // atribui endereco do int ao ponteiro
    // expressa valores de duas formas
    cout << "Valores: atualizacoes = " << atualizacoes;
    cout << ", *p_atualizacoes = " << *p_atualizacoes << endl;

    // expressa enderecos de duas formas
    cout << "Enderecos: &atualizacoes = " << &atualizacoes;
    cout << ", p_atualizacoes = " << p_atualizacoes << endl;
    // usa o ponteiro para alterar o valor
    *p_atualizacoes = *p_atualizacoes + 1;
    cout << "Agora atualizacoes = " << atualizacoes << endl;
    return 0;
}
```

Aqui esta a saida do programa na Listagem 4.15:

```
Valores: atualizacoes = 6, *p_atualizacoes = 6
Enderecos: &atualizacoes = 0x0065fd48, p_atualizacoes = 0x0065fd48
Agora atualizacoes = 7
```

Como voce pode ver, a variavel `int` `atualizacoes` e a variavel ponteiro `p_atualizacoes` sao apenas dois lados da mesma moeda. A variavel `atualizacoes` representa o valor como primario e usa o operador `&` para obter o endereco, enquanto a variavel `p_atualizacoes` representa o endereco como primario e usa o operador `*` para obter o valor. Como `p_atualizacoes` aponta para `atualizacoes`, `*p_atualizacoes` e `atualizacoes` sao completamente equivalentes. Voce pode usar `*p_atualizacoes` exatamente como usaria uma variavel do tipo `int`. Como o programa na Listagem 4.15 mostra, voce pode ate mesmo atribuir valores a `*p_atualizacoes`. Fazer isso altera o valor apontado, `atualizacoes`.

## Declarando e Inicializando Ponteiros

Vamos examinar o processo de declaracao de ponteiros. Um computador precisa controlar o tipo de valor para o qual um ponteiro aponta. Por exemplo, o endereco de um `char` tipicamente se parece com o endereco de um `double`, mas `char` e `double` usam numeros diferentes de bytes e formatos internos diferentes para armazenar valores. Portanto, uma declaracao de ponteiro deve especificar para qual tipo de dado o ponteiro aponta.

Por exemplo, o exemplo anterior tem esta declaracao:

```cpp
int * p_atualizacoes;
```

Isso declara que a combinacao `*p_atualizacoes` e do tipo `int`. Como voce usa o operador `*` aplicando-o a um ponteiro, a propria variavel `p_atualizacoes` deve ser um ponteiro. Dizemos que `p_atualizacoes` aponta para o tipo `int`. Tambem dizemos que o tipo para `p_atualizacoes` e ponteiro-para-`int` ou, mais concisamente, `int *`. Para repetir: `p_atualizacoes` e um ponteiro (um endereco), e `*p_atualizacoes` e um `int` e nao um ponteiro.

Alem disso, o uso de espacos em torno do operador `*` e opcional. Tradicionalmente, os programadores C tem usado esta forma:

```cpp
int *pt;
```

Isso enfatiza a ideia de que a combinacao `*pt` e um valor do tipo `int`. Muitos programadores C++, por outro lado, usam esta forma:

```cpp
int* pt;
```

Isso enfatiza a ideia de que `int*` e um tipo, ponteiro-para-`int`. Onde voce coloca os espacos nao faz diferenca para o compilador. Voce poderia ate mesmo fazer assim:

```cpp
int*pt;
```

Esteja ciente, entretanto, de que a declaracao a seguir cria um ponteiro (`p1`) e um `int` ordinario (`p2`):

```cpp
int* p1, p2;
```

Voce precisa de um `*` para cada nome de variavel ponteiro.

> **Nota**
>
> Em C++, a combinacao `int *` e um tipo composto, ponteiro-para-`int`.

Voce usa a mesma sintaxe para declarar ponteiros de outros tipos:

```cpp
double * p_taxa; // p_taxa aponta para o tipo double
char * str;      // str aponta para o tipo char
```

Como voce declara `p_taxa` como um ponteiro-para-`double`, o compilador sabe que `*p_taxa` e um valor do tipo `double`. Ou seja, ele sabe que `*p_taxa` representa um numero armazenado no formato de ponto flutuante que ocupa (na maioria dos sistemas) 8 bytes. Uma variavel ponteiro nunca e simplesmente um ponteiro. Ela e sempre um ponteiro para um tipo especifico. `p_taxa` e do tipo ponteiro-para-`double` (ou tipo `double *`), e `str` e do tipo ponteiro-para-`char` (ou `char *`). Embora ambos sejam ponteiros, eles sao ponteiros de dois tipos diferentes.

Note que, enquanto `p_taxa` e `str` apontam para tipos de dados de dois tamanhos diferentes, as duas variaveis `p_taxa` e `str` em si sao tipicamente do mesmo tamanho. Ou seja, o endereco de um `char` tem o mesmo tamanho que o endereco de um `double`, da mesma forma que o numero 1016 poderia ser o endereco de um grande armazem, enquanto 1024 poderia ser o endereco de uma pequena cabana. O tamanho ou o valor de um endereco nao diz nada sobre o tamanho ou o tipo de variavel ou edificio que voce encontra naquele endereco. Geralmente, os enderecos requerem 2 ou 4 bytes, dependendo do sistema computacional. (Alguns sistemas podem ter enderecos maiores, e um sistema pode usar tamanhos de endereco diferentes para diferentes tipos.)

Voce pode usar uma instrucao de declaracao para inicializar um ponteiro. Nesse caso, o ponteiro, e nao o valor apontado, e inicializado. Ou seja, as instrucoes a seguir definem `pt` e nao `*pt` com o valor `&higgens`:

```cpp
int higgens = 5;
int * pt = &higgens;
```

A Listagem 4.16 demonstra como inicializar um ponteiro para um endereco.

**Listagem 4.16 — init_ptr.cpp**

```cpp
// init_ptr.cpp -- inicializando um ponteiro
#include <iostream>
int main()
{
    using namespace std;
    int higgens = 5;
    int * pt = &higgens;
    cout << "Valor de higgens = " << higgens
         << "; Endereco de higgens = " << &higgens << endl;
    cout << "Valor de *pt = " << *pt
         << "; Valor de pt = " << pt << endl;
    return 0;
}
```

Aqui esta uma saida de exemplo do programa na Listagem 4.16:

```
Valor de higgens = 5; Endereco de higgens = 0012FED4
Valor de *pt = 5; Valor de pt = 0012FED4
```

Voce pode ver que o programa inicializa `pt`, e nao `*pt`, com o endereco de `higgens`. (Seu sistema provavelmente mostrara valores diferentes para os enderecos e pode exibi-los em um formato diferente.)

## Perigo dos Ponteiros

Aguarda aqueles que usam ponteiros descuidosamente. Um ponto extremamente importante e que quando voce cria um ponteiro em C++, o computador aloca memoria para armazenar um endereco, mas nao aloca memoria para armazenar os dados para os quais o endereco aponta. Criar espaco para os dados envolve uma etapa separada. Omitir essa etapa, como a seguir, e um convite ao desastre:

```cpp
long * colega;      // cria um ponteiro para long
*colega = 223323;   // coloca um valor em um lugar inexistente
```

Claro, `colega` e um ponteiro. Mas para onde ele aponta? O codigo falhou em atribuir um endereco a `colega`. Portanto, onde o valor `223323` sera colocado? Nao podemos dizer. Como `colega` nao foi inicializado, ele pode ter qualquer valor. Seja qual for esse valor, o programa o interpreta como o endereco no qual armazenar `223323`. Se `colega` tiver o valor `1200`, por exemplo, entao o computador tentara colocar os dados no endereco `1200`, mesmo que isso seja um endereco no meio do codigo do seu programa. Onde quer que `colega` aponte, esse nao e o lugar onde voce quer colocar o numero `223323`. Esse tipo de erro pode produzir alguns dos bugs mais insidiosos e dificeis de rastrear.

> **Cuidado**
>
> Regra de Ouro dos Ponteiros: sempre inicialize um ponteiro com um endereco definitivo e apropriado antes de aplicar o operador de desreferenciamento (`*`) a ele.

## Ponteiros e Numeros

Ponteiros nao sao tipos inteiros, mesmo que os computadores tipicamente lidem com enderecos como inteiros. Conceitualmente, ponteiros sao tipos distintos dos inteiros. Inteiros sao numeros que voce pode adicionar, subtrair, dividir e assim por diante. Mas um ponteiro descreve uma localizacao, e nao faz sentido, por exemplo, multiplicar duas localizacoes entre si. Em termos das operacoes que voce pode realizar com eles, ponteiros e inteiros sao diferentes um do outro. Consequentemente, voce nao pode simplesmente atribuir um inteiro a um ponteiro:

```cpp
int * pt;
pt = 0xB8000000; // tipos incompativeis
```

Aqui, o lado esquerdo e um ponteiro para `int`, portanto voce pode atribuir-lhe um endereco, mas o lado direito e simplesmente um inteiro. Voce pode saber que `0xB8000000` e o endereco combinado de segmento-deslocamento da memoria de video no seu sistema de computador antigo, mas nada na instrucao diz ao programa que esse numero e um endereco. O C anterior ao C99 permite que voce faca atribuicoes como essa. Mas o C++ impoe de forma mais rigorosa a concordancia de tipos, e o compilador dara uma mensagem de erro dizendo que ha incompatibilidade de tipos. Se voce quiser usar um valor numerico como endereco, deve usar uma conversao de tipo para converter o numero para o tipo de endereco apropriado:

```cpp
int * pt;
pt = (int *) 0xB8000000; // tipos agora sao compativeis
```

Agora ambos os lados da instrucao de atribuicao representam enderecos de inteiros, portanto a atribuicao e valida. Note que apenas porque e o endereco de um valor do tipo `int` nao significa que `pt` em si e do tipo `int`. Por exemplo, uma plataforma pode ter `int` com 2 bytes e enderecos com 4 bytes.

Ponteiros tem algumas outras propriedades interessantes que discutiremos quando se tornarem relevantes. Enquanto isso, vamos examinar como os ponteiros podem ser usados para gerenciar a alocacao de memoria em tempo de execucao.

## Alocando Memoria com new

Agora que voce tem uma nocao de como os ponteiros funcionam, vamos ver como eles podem implementar a importante tecnica de alocar memoria enquanto um programa esta em execucao. Ate agora, voce inicializou ponteiros com os enderecos de variaveis; as variaveis sao memoria nomeada alocada durante o tempo de compilacao, e cada ponteiro fornece apenas um alias para a memoria que voce poderia acessar diretamente pelo nome de qualquer maneira. O verdadeiro valor dos ponteiros entra em cena quando voce aloca memoria **nao nomeada** durante o tempo de execucao para armazenar valores. Nesse caso, os ponteiros se tornam o unico acesso a essa memoria. Em C, voce pode alocar memoria com a funcao de biblioteca `malloc()`. Voce ainda pode fazer isso em C++, mas o C++ tambem tem uma maneira melhor: o operador `new`.

Vamos experimentar essa nova tecnica criando armazenamento em tempo de execucao nao nomeado para um valor do tipo `int` e acessando o valor com um ponteiro. A chave e o operador `new` do C++. Voce diz ao `new` para qual tipo de dado voce quer memoria; `new` encontra um bloco do tamanho correto e retorna o endereco do bloco. Voce atribui esse endereco a um ponteiro, e esta pronto. Aqui esta um exemplo da tecnica:

```cpp
int * pn = new int;
```

A parte `new int` diz ao programa que voce quer algum novo espaco de armazenamento adequado para conter um `int`. O operador `new` usa o tipo para descobrir quantos bytes sao necessarios. Em seguida, encontra a memoria e retorna o endereco. A seguir, voce atribui o endereco a `pn`, que e declarado como do tipo ponteiro-para-`int`. Agora `pn` e o endereco e `*pn` e o valor armazenado la. Compare isso com atribuir o endereco de uma variavel a um ponteiro:

```cpp
int higgens;
int * pt = &higgens;
```

Em ambos os casos (`pn` e `pt`), voce atribui o endereco de um `int` a um ponteiro. No segundo caso, voce tambem pode acessar o `int` pelo nome: `higgens`. No primeiro caso, seu unico acesso e via ponteiro. Isso levanta uma questao: como o `pn` aponta para memoria que nao tem nome, como voce a chama? Dizemos que `pn` aponta para um **objeto de dados** (data object). Isso nao e "objeto" no sentido de "programacao orientada a objetos"; e apenas "objeto" no sentido de "coisa". O termo "objeto de dados" e mais geral do que o termo "variavel" porque significa qualquer bloco de memoria alocado para um item de dados. Assim, uma variavel tambem e um objeto de dados, mas a memoria para a qual `pn` aponta nao e uma variavel. O metodo de ponteiro para lidar com objetos de dados pode parecer mais complicado a principio, mas oferece maior controle sobre como o programa gerencia a memoria.

A forma geral para obter e atribuir memoria para um unico objeto de dados, que pode ser uma estrutura bem como um tipo fundamental, e esta:

```cpp
tipoDado * nome_ponteiro = new tipoDado;
```

Voce usa o tipo de dado duas vezes: uma para especificar o tipo de memoria solicitado e uma para declarar um ponteiro adequado. Claro, se voce ja declarou um ponteiro do tipo correto, pode usa-lo em vez de declarar um novo. A Listagem 4.17 ilustra o uso de `new` com dois tipos diferentes.

**Listagem 4.17 — use_new.cpp**

```cpp
// use_new.cpp -- usando o operador new
#include <iostream>
int main()
{
    using namespace std;
    int noites = 1001;
    int * pt = new int;     // aloca espaco para um int
    *pt = 1001;             // armazena um valor la
    cout << "valor de noites = ";
    cout << noites << ": localizacao " << &noites << endl;
    cout << "int ";
    cout << "valor = " << *pt << ": localizacao = " << pt << endl;

    double * pd = new double; // aloca espaco para um double
    *pd = 10000001.0;         // armazena um double la
    cout << "double ";
    cout << "valor = " << *pd << ": localizacao = " << pd << endl;
    cout << "localizacao do ponteiro pd: " << &pd << endl;
    cout << "tamanho de pt = " << sizeof(pt);
    cout << ": tamanho de *pt = " << sizeof(*pt) << endl;
    cout << "tamanho de pd = " << sizeof pd;
    cout << ": tamanho de *pd = " << sizeof(*pd) << endl;
    return 0;
}
```

Aqui esta a saida do programa na Listagem 4.17:

```
valor de noites = 1001: localizacao 0028F7F8
int valor = 1001: localizacao = 00033A98
double valor = 1e+007: localizacao = 000339B8
localizacao do ponteiro pd: 0028F7FC
tamanho de pt = 4: tamanho de *pt = 4
tamanho de pd = 4: tamanho de *pd = 8
```

Claro, os valores exatos para as localizacoes de memoria diferem de sistema para sistema.

**Notas do Programa**

O programa na Listagem 4.17 usa `new` para alocar memoria para os objetos de dados do tipo `int` e do tipo `double`. Isso ocorre enquanto o programa esta em execucao. Os ponteiros `pt` e `pd` apontam para esses dois objetos de dados. Sem eles, voce nao pode acessar essas localizacoes de memoria. Com eles, voce pode usar `*pt` e `*pd` exatamente como usaria variaveis. Voce atribui valores a `*pt` e `*pd` para atribuir valores aos novos objetos de dados. De forma similar, voce imprime `*pt` e `*pd` para exibir esses valores.

O programa na Listagem 4.17 tambem demonstra uma das razoes pelas quais voce tem que declarar o tipo para o qual um ponteiro aponta. Um endereco em si revela apenas o endereco inicial do objeto armazenado, nao seu tipo ou o numero de bytes usados. Veja os enderecos dos dois valores. Eles sao apenas numeros sem informacao de tipo ou tamanho. Observe tambem que o tamanho de um ponteiro-para-`int` e o mesmo que o tamanho de um ponteiro-para-`double`. Ambos sao apenas enderecos. Mas como `use_new.cpp` declara os tipos de ponteiro, o programa sabe que `*pd` e um valor `double` de 8 bytes, enquanto `*pt` e um valor `int` de 4 bytes. Quando `use_new.cpp` imprime o valor de `*pd`, `cout` pode dizer quantos bytes ler e como interpretá-los.

Outro ponto a notar e que, tipicamente, `new` usa um bloco de memoria diferente do que as definicoes de variaveis comuns que temos usado. Tanto as variaveis `noites` quanto `pd` tem seus valores armazenados em uma regiao de memoria chamada **pilha** (stack), enquanto a memoria alocada por `new` esta em uma regiao chamada **heap** (monte) ou **area de armazenamento livre** (free store). O Capitulo 9 discute isso um pouco mais.

> **Sem Memoria?**
>
> E possivel que um computador nao tenha memoria suficiente disponivel para satisfazer uma solicitacao de `new`. Quando esse e o caso, `new` normalmente responde lancando uma excecao, uma tecnica de tratamento de erros discutida no Capitulo 15, "Amigos, Excecoes e Mais". Em implementacoes mais antigas, `new` retornava o valor `0`. Em C++, um ponteiro com o valor `0` e chamado de ponteiro nulo (null pointer). O C++ garante que o ponteiro nulo nunca aponta para dados validos, portanto e frequentemente usado para indicar falha em operadores ou funcoes que de outra forma retornariam ponteiros utilizaveis. A instrucao `if`, discutida no Capitulo 6, ajuda a lidar com essa possibilidade. Por enquanto, o ponto importante e que o C++ fornece as ferramentas para detectar e responder a falhas de alocacao.

## Liberando Memoria com delete

Usar `new` para solicitar memoria quando voce precisa dela e apenas a metade mais glamourosa do pacote de gerenciamento de memoria do C++. A outra metade e o operador `delete`, que permite retornar memoria ao pool de memoria quando voce terminar de usa-la. Esse e um passo importante para fazer o uso mais eficaz da memoria. A memoria que voce retorna, ou libera, pode ser reutilizada por outras partes do programa. Voce usa `delete` seguindo-o de um ponteiro para um bloco de memoria originalmente alocado com `new`:

```cpp
int * ps = new int; // aloca memoria com new
...                 // usa a memoria
delete ps;          // libera memoria com delete quando terminar
```

Isso remove a memoria para a qual `ps` aponta; nao remove o ponteiro `ps` em si. Voce pode reutilizar `ps`, por exemplo, para apontar para outra alocacao com `new`. Voce deve sempre balancear um uso de `new` com um uso de `delete`; caso contrario, pode acabar com um **vazamento de memoria** (memory leak) — ou seja, memoria que foi alocada mas que nao pode mais ser usada. Se um vazamento de memoria crescer muito, pode paralisar um programa em busca de mais memoria.

Voce nao deve tentar liberar um bloco de memoria que ja liberou anteriormente. O Padrao C++ diz que o resultado de tal tentativa e indefinido, o que significa que as consequencias podem ser qualquer coisa. Alem disso, voce nao pode usar `delete` para liberar memoria criada pela declaracao de variaveis comuns:

```cpp
int * ps = new int; // ok
delete ps;          // ok
delete ps;          // nao ok agora
int jarros = 5;     // ok
int * pi = &jarros; // ok
delete pi;          // nao permitido, memoria nao alocada por new
```

> **Cuidado**
>
> Voce deve usar `delete` apenas para liberar memoria alocada com `new`. Entretanto, e seguro aplicar `delete` a um ponteiro nulo (nada acontece).

Note que o requisito critico para usar `delete` e usá-lo com memoria alocada por `new`. Isso nao significa que voce tem que usar o mesmo ponteiro que usou com `new`; em vez disso, voce tem que usar o mesmo endereco:

```cpp
int * ps = new int; // aloca memoria
int * pq = ps;      // define segundo ponteiro para o mesmo bloco
delete pq;          // deleta com o segundo ponteiro
```

Normalmente, voce nao criaria dois ponteiros para o mesmo bloco de memoria porque isso aumenta a possibilidade de tentar deletar o mesmo bloco duas vezes por engano. Mas, como voce vera em breve, usar um segundo ponteiro faz sentido quando voce trabalha com uma funcao que retorna um ponteiro.

## Usando new para Criar Arrays Dinamicos

Se tudo que um programa precisa e de um unico valor, voce pode usar uma variavel simples porque isso e mais simples, ainda que menos impressionante, do que usar `new` e um ponteiro para gerenciar um unico objeto de dado pequeno. Mais tipicamente, voce usa `new` com pedacos maiores de dados, como arrays, strings e estruturas. E aqui que `new` e util. Suponha, por exemplo, que voce esta escrevendo um programa que pode ou nao precisar de um array, dependendo de informacoes fornecidas ao programa durante sua execucao. Se voce criar um array declarando-o, o espaco sera alocado quando o programa for compilado. Quer o programa use finalmente o array ou nao, o array esta la, usando memoria. Alocar o array durante o tempo de compilacao e chamado de **vinculacao estatica** (static binding), o que significa que o array esta incorporado ao programa em tempo de compilacao. Mas com `new`, voce pode criar um array durante o tempo de execucao se precisar dele e nao criar o array se nao precisar. Ou pode selecionar um tamanho de array depois que o programa estiver em execucao. Isso e chamado de **vinculacao dinamica** (dynamic binding), o que significa que o array e criado enquanto o programa esta em execucao. Esse array e chamado de **array dinamico** (dynamic array). Com a vinculacao estatica, voce deve especificar o tamanho do array ao escrever o programa. Com a vinculacao dinamica, o programa pode decidir o tamanho do array enquanto o programa estiver em execucao.

Por enquanto, vamos examinar dois aspectos basicos sobre arrays dinamicos: como usar o operador `new` do C++ para criar um array e como usar um ponteiro para acessar os elementos do array.

### Criando um Array Dinamico com new

E facil criar um array dinamico em C++; voce diz ao `new` o tipo de elemento do array e o numero de elementos que deseja. A sintaxe requer que voce siga o nome do tipo com o numero de elementos entre colchetes. Por exemplo, se voce precisar de um array de 10 `int`s, voce usa isto:

```cpp
int * palg = new int [10]; // obtem um bloco de 10 ints
```

O operador `new` retorna o endereco do primeiro elemento do bloco. Neste exemplo, esse valor e atribuido ao ponteiro `palg`.

Como sempre, voce deve balancear a chamada a `new` com uma chamada a `delete` quando o programa terminar de usar aquele bloco de memoria. No entanto, usar `new` com colchetes para criar um array requer usar uma forma alternativa de `delete` ao liberar o array:

```cpp
delete [] palg; // libera um array dinamico
```

A presenca dos colchetes diz ao programa que ele deve liberar o array inteiro, nao apenas o elemento apontado pelo ponteiro. Note que os colchetes estao entre `delete` e o ponteiro. Se voce usar `new` sem colchetes, deve usar `delete` sem colchetes. Se voce usar `new` com colchetes, deve usar `delete` com colchetes. Versoes anteriores do C++ podem nao reconhecer a notacao de colchetes. Para o Padrao ANSI/ISO, no entanto, o efeito de combinar erroneamente as formas de `new` e `delete` e indefinido, o que significa que voce nao pode confiar em algum comportamento especifico. Aqui esta um exemplo:

```cpp
int * pt = new int;
short * ps = new short [500];
delete [] pt; // efeito indefinido, nao faca isso
delete ps;    // efeito indefinido, nao faca isso
```

Resumindo, voce deve observar estas regras ao usar `new` e `delete`:

- Nao use `delete` para liberar memoria que `new` nao alocou.
- Nao use `delete` para liberar o mesmo bloco de memoria duas vezes seguidas.
- Use `delete []` se voce usou `new []` para alocar um array.
- Use `delete` (sem colchetes) se voce usou `new` para alocar uma unica entidade.
- E seguro aplicar `delete` ao ponteiro nulo (nada acontece).

Agora vamos voltar ao array dinamico. Note que `palg` e um ponteiro para um unico `int`, o primeiro elemento do bloco. E sua responsabilidade controlar quantos elementos estao no bloco. Ou seja, como o compilador nao controla o fato de que `palg` aponta para o primeiro de 10 inteiros, voce tem que escrever seu programa para controlar o numero de elementos.

Na verdade, o programa controla a quantidade de memoria alocada para que ela possa ser liberada corretamente mais tarde quando voce usar o operador `delete []`. Mas essa informacao nao esta publicamente disponivel; voce nao pode usar o operador `sizeof`, por exemplo, para descobrir o numero de bytes em um array alocado dinamicamente.

A forma geral para alocar e atribuir memoria para um array e esta:

```cpp
tipo_dado * nome_ponteiro = new tipo_dado [num_elementos];
```

Invocar o operador `new` garante um bloco de memoria grande o suficiente para conter `num_elementos` elementos do tipo `tipo_dado`, com `nome_ponteiro` apontando para o primeiro elemento. Como voce vai ver logo em seguida, voce pode usar `nome_ponteiro` em muitas das mesmas formas que pode usar um nome de array.

### Usando um Array Dinamico

Depois de criar um array dinamico, como voce o usa? Primeiro, pense no problema conceitualmente. A instrucao a seguir cria um ponteiro, `palg`, que aponta para o primeiro elemento de um bloco de 10 valores `int`:

```cpp
int * palg = new int [10]; // obtem um bloco de 10 ints
```

Pense nisso como um dedo apontando para esse elemento. Suponha que um `int` ocupe 4 bytes. Entao, movendo seu dedo 4 bytes na direcao correta, voce pode apontar para o segundo elemento. Ao todo, ha 10 elementos, que e o intervalo sobre o qual voce pode mover seu dedo. Assim, a instrucao `new` fornece todas as informacoes necessarias para identificar cada elemento no bloco.

Agora pense no problema de forma pratica. Como voce acessa um desses elementos? A maneira mais simples de acessar os elementos pode surpreendê-lo se voce nao trabalhou com C: basta usar o ponteiro como se fosse o nome de um array. Ou seja, voce pode usar `palg[0]` em vez de `*palg` para o primeiro elemento, `palg[1]` para o segundo elemento e assim por diante. E bem simples usar um ponteiro para acessar um array dinamico, mesmo que possa nao ser imediatamente obvio por que esse metodo funciona. A razao pela qual voce pode fazer isso e que o C e o C++ internamente lidam com arrays usando ponteiros de qualquer maneira. Essa quase equivalencia entre arrays e ponteiros e uma das belezas do C e C++. (Tambem e as vezes um problema, mas essa e outra historia.) Voce vai aprender mais sobre essa equivalencia em breve. Primeiro, a Listagem 4.18 mostra como voce pode usar `new` para criar um array dinamico e, em seguida, usar a notacao de array para acessar os elementos. Ela tambem aponta uma diferenca fundamental entre um ponteiro e um verdadeiro nome de array.

**Listagem 4.18 — arraynew.cpp**

```cpp
// arraynew.cpp -- usando o operador new para arrays
#include <iostream>
int main()
{
    using namespace std;
    double * p3 = new double [3]; // espaco para 3 doubles
    p3[0] = 0.2;    // trata p3 como nome de array
    p3[1] = 0.5;
    p3[2] = 0.8;
    cout << "p3[1] e " << p3[1] << ".\n";
    p3 = p3 + 1;    // incrementa o ponteiro
    cout << "Agora p3[0] e " << p3[0] << " e ";
    cout << "p3[1] e " << p3[1] << ".\n";
    p3 = p3 - 1;    // volta para o inicio
    delete [] p3;   // libera a memoria
    return 0;
}
```

Aqui esta a saida do programa na Listagem 4.18:

```
p3[1] e 0.5.
Agora p3[0] e 0.5 e p3[1] e 0.8.
```

Como voce pode ver, `arraynew.cpp` usa o ponteiro `p3` como se fosse o nome de um array, com `p3[0]` como o primeiro elemento e assim por diante. A diferenca fundamental entre um nome de array e um ponteiro aparece na seguinte linha:

```cpp
p3 = p3 + 1; // ok para ponteiros, errado para nomes de array
```

Voce nao pode alterar o valor de um nome de array. Mas um ponteiro e uma variavel, portanto voce pode alterar seu valor. Note o efeito de adicionar 1 a `p3`. A expressao `p3[0]` agora se refere ao antigo segundo elemento do array. Assim, adicionar 1 a `p3` faz com que ele aponte para o segundo elemento em vez do primeiro. Subtrair um leva o ponteiro de volta ao seu valor original para que o programa possa fornecer ao `delete []` o endereco correto.

## Ponteiros, Arrays e Aritmetica de Ponteiros

A quase equivalencia de ponteiros e nomes de array decorre da **aritmetica de ponteiros** (pointer arithmetic) e de como o C++ lida com arrays internamente. Primeiro, vamos verificar a aritmetica. Adicionar um a uma variavel inteira aumenta seu valor em um, mas adicionar um a uma variavel ponteiro aumenta seu valor pelo numero de bytes do tipo para o qual ela aponta. Adicionar um a um ponteiro para `double` adiciona 8 ao valor numerico em sistemas com `double` de 8 bytes, enquanto adicionar um a um ponteiro para `short` adiciona dois ao valor do ponteiro se `short` tiver 2 bytes. A Listagem 4.19 demonstra esse ponto surpreendente. Ela tambem mostra um segundo ponto importante: o C++ interpreta o nome de um array como um endereco.

**Listagem 4.19 — addpntrs.cpp**

```cpp
// addpntrs.cpp -- adicao de ponteiros
#include <iostream>
int main()
{
    using namespace std;
    double salarios[3] = {10000.0, 20000.0, 30000.0};
    short pilhas[3] = {3, 2, 1};
    // duas formas de obter o endereco de um array
    double * pw = salarios;       // nome do array = endereco
    short * ps = &pilhas[0];      // ou usa operador de endereco com elemento
    cout << "pw = " << pw << ", *pw = " << *pw << endl;
    pw = pw + 1;
    cout << "adiciona 1 ao ponteiro pw:\n";
    cout << "pw = " << pw << ", *pw = " << *pw << "\n\n";

    cout << "ps = " << ps << ", *ps = " << *ps << endl;
    ps = ps + 1;
    cout << "adiciona 1 ao ponteiro ps:\n";
    cout << "ps = " << ps << ", *ps = " << *ps << "\n\n";
    cout << "acessa dois elementos com notacao de array\n";
    cout << "pilhas[0] = " << pilhas[0]
         << ", pilhas[1] = " << pilhas[1] << endl;
    cout << "acessa dois elementos com notacao de ponteiro\n";
    cout << "*pilhas = " << *pilhas
         << ", *(pilhas + 1) = " << *(pilhas + 1) << endl;
    cout << sizeof(salarios) << " = tamanho do array salarios\n";
    cout << sizeof(pw) << " = tamanho do ponteiro pw\n";
    return 0;
}
```

Aqui esta a saida do programa na Listagem 4.19:

```
pw = 0x28ccf0, *pw = 10000
adiciona 1 ao ponteiro pw:
pw = 0x28ccf8, *pw = 20000

ps = 0x28ccea, *ps = 3
adiciona 1 ao ponteiro ps:
ps = 0x28ccec, *ps = 2

acessa dois elementos com notacao de array
pilhas[0] = 3, pilhas[1] = 2
acessa dois elementos com notacao de ponteiro
*pilhas = 3, *(pilhas + 1) = 2
24 = tamanho do array salarios
4 = tamanho do ponteiro pw
```

**Notas do Programa**

Na maioria dos contextos, o C++ interpreta o nome de um array como o endereco de seu primeiro elemento. Assim, a instrucao a seguir torna `pw` um ponteiro para o tipo `double` e entao inicializa `pw` com `salarios`, que e o endereco do primeiro elemento do array `salarios`:

```cpp
double * pw = salarios;
```

Para `salarios`, como para qualquer array, temos a seguinte igualdade:

```cpp
salarios == &salarios[0] == endereco do primeiro elemento do array
```

Apenas para mostrar que isso e de fato assim, o programa usa explicitamente o operador de endereco na expressao `&pilhas[0]` para inicializar o ponteiro `ps` com o primeiro elemento do array `pilhas`.

Em seguida, o programa inspeciona os valores de `pw` e `*pw`. O primeiro e um endereco, e o segundo e o valor naquele endereco. Como `pw` aponta para o primeiro elemento, o valor exibido para `*pw` e o do primeiro elemento, `10000`. Entao o programa adiciona um a `pw`. Como prometido, isso adiciona oito ao valor numerico do endereco porque `double` neste sistema tem 8 bytes. Isso torna `pw` igual ao endereco do segundo elemento. Portanto, `*pw` agora e `20000`, o valor do segundo elemento.

Depois disso, o programa realiza etapas semelhantes para `ps`. Desta vez, como `ps` aponta para o tipo `short` e como `short` tem 2 bytes, adicionar 1 ao ponteiro aumenta seu valor em 2 (`0x28ccea + 2 = 0x28ccec` em hexadecimal). Novamente, o resultado e fazer o ponteiro apontar para o proximo elemento do array.

> **Nota**
>
> Adicionar um a uma variavel ponteiro aumenta seu valor pelo numero de bytes do tipo para o qual ela aponta.

Agora considere a expressao de array `pilhas[1]`. O compilador C++ trata essa expressao exatamente como se voce a tivesse escrito como `*(pilhas + 1)`. A segunda expressao significa calcular o endereco do segundo elemento do array e, em seguida, encontrar o valor armazenado la. O resultado final e precisamente o que `pilhas[1]` significa. (A precedencia dos operadores requer que voce use os parenteses. Sem eles, `1` seria adicionado a `*pilhas` em vez de a `pilhas`.)

A saida do programa demonstra que `*(pilhas + 1)` e `pilhas[1]` sao iguais. De forma similar, `*(pilhas + 2)` e o mesmo que `pilhas[2]`. Em geral, onde quer que voce use notacao de array, o C++ faz a seguinte conversao:

```cpp
nomeArray[i] equivale a *(nomeArray + i)
```

E se voce usar um ponteiro em vez de um nome de array, o C++ faz a mesma conversao:

```cpp
nomePonteiro[i] equivale a *(nomePonteiro + i)
```

Assim, em muitos aspectos voce pode usar nomes de ponteiros e nomes de arrays da mesma forma. Voce pode usar a notacao de colchetes de array com qualquer um. Voce pode aplicar o operador de desreferenciamento (`*`) a qualquer um. Na maioria das expressoes, cada um representa um endereco. Uma diferenca e que voce pode alterar o valor de um ponteiro, enquanto o nome de um array e uma constante:

```cpp
nomePonteiro = nomePonteiro + 1; // valido
nomeArray = nomeArray + 1;       // nao permitido
```

Uma segunda diferenca e que aplicar o operador `sizeof` ao nome de um array retorna o tamanho do array, mas aplicar `sizeof` a um ponteiro retorna o tamanho do ponteiro, mesmo que o ponteiro aponte para o array. Por exemplo, em Listagem 4.19, tanto `pw` quanto `salarios` se referem ao mesmo array. Mas aplicar o operador `sizeof` a eles produz os seguintes resultados:

```
24 = tamanho do array salarios  <<exibindo sizeof salarios
4 = tamanho do ponteiro pw       <<exibindo sizeof pw
```

Esse e um caso em que o C++ nao interpreta o nome do array como um endereco.

> **O Endereco de um Array**
>
> Obter o endereco de um array e outro caso em que o nome do array nao e interpretado como seu endereco. Mas espere — o nome de um array nao e interpretado como o endereco do array? Nao exatamente — o nome do array e interpretado como o endereco do primeiro elemento de um array, enquanto aplicar o operador de endereco produz o endereco do array inteiro:
>
> ```cpp
> short aviso[10]; // aviso e um array de 20 bytes
> cout << aviso << endl;  // exibe &aviso[0]
> cout << &aviso << endl; // exibe o endereco do array inteiro
> ```
>
> Numericamente, esses dois enderecos sao os mesmos, mas conceitualmente `&aviso[0]`, e portanto `aviso`, e o endereco de um bloco de memoria de 2 bytes, enquanto `&aviso` e o endereco de um bloco de memoria de 20 bytes. Portanto, a expressao `aviso + 1` adiciona 2 ao valor do endereco, enquanto `&aviso + 1` adiciona 20 ao valor do endereco. Outra forma de expressar isso e dizer que `aviso` e do tipo ponteiro-para-`short`, ou `short *`, e `&aviso` e do tipo ponteiro-para-array-de-20-`short`s, ou `short (*)[20]`.

## Resumo dos Ponteiros

Voce foi exposto a bastante conhecimento sobre ponteiros recentemente, entao vamos resumir o que foi revelado sobre ponteiros e arrays ate agora.

**Declarando Ponteiros**

Para declarar um ponteiro para um tipo especifico, use esta forma:

```cpp
tipoDado * nomePonteiro;
```

Alguns exemplos:

```cpp
double * pn; // pn pode apontar para um valor double
char * pc;   // pc pode apontar para um valor char
```

Aqui `pn` e `pc` sao ponteiros, e `double *` e `char *` sao as notacoes do C++ para os tipos ponteiro-para-`double` e ponteiro-para-`char`.

**Atribuindo Valores a Ponteiros**

Voce deve atribuir um endereco de memoria a um ponteiro. Voce pode aplicar o operador `&` ao nome de uma variavel para obter o endereco de memoria nomeada, e o operador `new` retorna o endereco de memoria nao nomeada.

Alguns exemplos:

```cpp
double * pn; // pn pode apontar para um valor double
double * pa; // pa tambem pode
char * pc;   // pc pode apontar para um valor char
double bolha = 3.2;
pn = &bolha; // atribui endereco de bolha a pn
pc = new char; // atribui endereco de memoria char recentemente alocada a pc
pa = new double[30]; // atribui endereco do 1o elemento do array de 30 doubles a pa
```

**Desreferenciando Ponteiros**

Desreferenciar um ponteiro significa se referir ao valor apontado. Voce aplica o operador de desreferenciamento, ou valor indireto, (`*`) a um ponteiro para desreferencia-lo. Portanto, se `pn` e um ponteiro para `bolha`, como no exemplo anterior, entao `*pn` e o valor apontado, ou `3.2`, neste caso.

Alguns exemplos:

```cpp
cout << *pn; // imprime o valor de bolha
*pc = 'S';   // coloca 'S' na localizacao de memoria cujo endereco e pc
```

A notacao de array e uma segunda forma de desreferenciar um ponteiro; por exemplo, `pn[0]` e o mesmo que `*pn`. Voce nunca deve desreferenciar um ponteiro que nao foi inicializado com um endereco adequado.

**Distinguindo entre um Ponteiro e o Valor Apontado**

Lembre-se, se `pt` e um ponteiro-para-`int`, `*pt` nao e um ponteiro-para-`int`; em vez disso, `*pt` e o equivalente completo de uma variavel do tipo `int`. E `pt` que e o ponteiro.

Alguns exemplos:

```cpp
int * pt = new int; // atribui um endereco ao ponteiro pt
*pt = 5;            // armazena o valor 5 naquele endereco
```

**Nomes de Arrays**

Na maioria dos contextos, o C++ trata o nome de um array como equivalente ao endereco do primeiro elemento de um array.

Um exemplo:

```cpp
int tacos[10]; // agora tacos e o mesmo que &tacos[0]
```

Uma excecao ocorre quando voce usa o nome do array com o operador `sizeof`. Nesse caso, `sizeof` retorna o tamanho do array inteiro, em bytes.

**Aritmetica de Ponteiros**

O C++ permite adicionar um inteiro a um ponteiro. O resultado de adicionar um e igual ao valor original do endereco mais um valor igual ao numero de bytes no objeto apontado. Voce tambem pode subtrair um inteiro de um ponteiro para obter a diferenca entre dois ponteiros. A ultima operacao, que resulta em um inteiro, tem significado apenas se os dois ponteiros apontam para o mesmo array (apontar para uma posicao alem do final tambem e permitido); ela retorna entao a separacao entre os dois elementos.

Alguns exemplos:

```cpp
int tacos[10] = {5, 2, 8, 4, 1, 2, 2, 4, 6, 8};
int * pt = tacos;  // suponha que pt e tacos sao o endereco 3000
pt = pt + 1;       // agora pt e 3004 se int tem 4 bytes
int *pe = &tacos[9]; // pe e 3036 se int tem 4 bytes
pe = pe - 1;       // agora pe e 3032, o endereco de tacos[8]
int diff = pe - pt; // diff e 7, a separacao entre tacos[8] e tacos[1]
```

**Vinculacao Dinamica e Estatica para Arrays**

Voce pode usar uma declaracao de array para criar um array com vinculacao estatica — ou seja, um array cujo tamanho e definido durante o processo de compilacao:

```cpp
int tacos[10]; // vinculacao estatica, tamanho fixo em tempo de compilacao
```

Voce usa o operador `new []` para criar um array com vinculacao dinamica (um array dinamico) — ou seja, um array que e alocado e cujo tamanho pode ser definido durante o tempo de execucao. Voce libera a memoria com `delete []` quando terminar:

```cpp
int tamanho;
cin >> tamanho;
int * pz = new int [tamanho]; // vinculacao dinamica, tamanho definido em tempo de execucao
...
delete [] pz; // libera memoria quando terminar
```

**Notacao de Array e Notacao de Ponteiro**

Usar a notacao de colchetes de array e equivalente a desreferenciar um ponteiro:

```cpp
tacos[0]  equivale a  *tacos   (valor no endereco tacos)
tacos[3]  equivale a  *(tacos + 3) (valor no endereco tacos + 3)
```

Isso e valido tanto para nomes de arrays quanto para variaveis de ponteiros, portanto voce pode usar notacao de ponteiro ou notacao de array com ponteiros e nomes de arrays.

Alguns exemplos:

```cpp
int * pt = new int [10]; // pt aponta para um bloco de 10 ints
*pt = 5;        // define o elemento numero 0 como 5
pt[0] = 6;      // redefine o elemento numero 0 como 6
pt[9] = 44;     // define o decimo elemento (elemento numero 9) como 44
int casacos[10];
*(casacos + 4) = 12; // define casacos[4] como 12
```

## Ponteiros e Strings

A relacao especial entre arrays e ponteiros se estende as strings no estilo C. Considere o seguinte codigo:

```cpp
char flor[10] = "rosa";
cout << flor << "s sao vermelhas\n";
```

O nome de um array e o endereco de seu primeiro elemento, portanto `flor` na instrucao `cout` e o endereco do elemento `char` contendo o caractere `r`. O objeto `cout` assume que o endereco de um `char` e o endereco de uma string, portanto ele imprime o caractere naquele endereco e continua imprimindo caracteres ate encontrar o caractere nulo (`\0`). Resumindo, se voce fornecer ao `cout` o endereco de um caractere, ele imprime tudo desde aquele caractere ate o primeiro caractere nulo que o segue.

O elemento crucial aqui nao e que `flor` e um nome de array, mas que `flor` age como o endereco de um `char`. Isso implica que voce pode usar uma variavel ponteiro-para-`char` como argumento para `cout` tambem porque ela, tambem, e o endereco de um `char`. Claro, esse ponteiro deve apontar para o inicio de uma string. Vamos verificar isso em breve.

Mas quanto a parte final da instrucao `cout` anterior? Se `flor` e na verdade o endereco do primeiro caractere de uma string, o que e a expressao `"s sao vermelhas\n"`? Para ser consistente com o tratamento de `cout` para saida de strings, essa string entre aspas deve tambem ser um endereco. E e, pois em C++ uma string entre aspas, como um nome de array, serve como o endereco de seu primeiro elemento. O codigo anterior nao envia realmente uma string inteira para `cout`; ele apenas envia o endereco da string. Isso significa que strings em um array, constantes de string entre aspas e strings descritas por ponteiros sao tratadas de forma equivalente. Cada uma e repassada como um endereco.

> **Nota**
>
> Com `cout` e com a maioria das expressoes C++, o nome de um array de `char`, um ponteiro-para-`char` e uma constante de string entre aspas sao todos interpretados como o endereco do primeiro caractere de uma string.

A Listagem 4.20 ilustra o uso das diferentes formas de strings. Ela usa duas funcoes da biblioteca de strings. A funcao `strlen()`, que voce ja usou, retorna o comprimento de uma string. A funcao `strcpy()` copia uma string de um lugar para outro. Ambas tem prototipos de funcao no arquivo de cabecalho `cstring` (ou `string.h`, em implementacoes menos atualizadas). O programa tambem usa comentarios para destacar alguns usos incorretos de ponteiros que voce deve tentar evitar.

**Listagem 4.20 — ptrstr.cpp**

```cpp
// ptrstr.cpp -- usando ponteiros para strings
#include <iostream>
#include <cstring>  // declara strlen(), strcpy()
int main()
{
    using namespace std;
    char animal[20] = "urso";   // animal contem urso
    const char * passaro = "wren"; // passaro contem endereco da string
    char * ps;                  // nao inicializado
    cout << animal << " e ";    // exibe urso
    cout << passaro << "\n";    // exibe wren
    // cout << ps << "\n";      // pode exibir lixo, pode causar crash
    cout << "Digite um tipo de animal: ";
    cin >> animal;              // ok se entrada < 20 chars
    // cin >> ps;  Erro terrivel; ps nao aponta para espaco alocado
    ps = animal;                // define ps para apontar para string
    cout << ps << "!\n";        // ok, mesmo que usar animal
    cout << "Antes de usar strcpy():\n";
    cout << animal << " em " << (int *) animal << endl;
    cout << ps << " em " << (int *) ps << endl;
    ps = new char[strlen(animal) + 1]; // obtem novo armazenamento
    strcpy(ps, animal);         // copia string para novo armazenamento
    cout << "Apos usar strcpy():\n";
    cout << animal << " em " << (int *) animal << endl;
    cout << ps << " em " << (int *) ps << endl;
    delete [] ps;
    return 0;
}
```

Aqui esta uma execucao de exemplo do programa na Listagem 4.20:

```
urso e wren
Digite um tipo de animal: raposa
raposa!
Antes de usar strcpy():
raposa em 0x0065fd30
raposa em 0x0065fd30
Apos usar strcpy():
raposa em 0x0065fd30
raposa em 0x004301c8
```

**Notas do Programa**

O programa na Listagem 4.20 cria um array `char` (`animal`) e duas variaveis ponteiro-para-`char` (`passaro` e `ps`). O programa comeca inicializando o array `animal` com a string `"urso"`, assim como voce ja inicializou arrays antes. Em seguida, o programa faz algo novo. Ele inicializa um ponteiro-para-`char` com uma string:

```cpp
const char * passaro = "wren"; // passaro contem endereco da string
```

Lembre-se, `"wren"` na verdade representa o endereco da string, portanto essa instrucao atribui o endereco de `"wren"` ao ponteiro `passaro`. (Tipicamente, um compilador reserva uma area na memoria para conter todas as strings entre aspas usadas no codigo-fonte do programa, associando cada string armazenada com seu endereco.) Isso significa que voce pode usar o ponteiro `passaro` exatamente como usaria a string `"wren"`, como neste exemplo:

```cpp
cout << "Um preocupado " << passaro << " fala\n";
```

Os literais de string sao constantes, razao pela qual o codigo usa a palavra-chave `const` na declaracao. Usar `const` dessa forma significa que voce pode usar `passaro` para acessar a string, mas nao para alterar seu conteudo. Finalmente, o ponteiro `ps` permanece nao inicializado, portanto ele nao aponta para nenhuma string. (Como voce sabe, isso geralmente e uma ma ideia, e este exemplo nao e excecao.)

Em seguida, o programa ilustra que voce pode usar o nome de array `animal` e o ponteiro `passaro` de forma equivalente com `cout`. Ambos, afinal, sao enderecos de strings, e `cout` exibe as duas strings (`"urso"` e `"wren"`) armazenadas nesses enderecos. Se voce ativar o codigo que comete o erro de tentar exibir `ps`, voce pode obter uma linha em branco, pode obter lixo exibido e pode ter um crash do programa. Criar um ponteiro nao inicializado e um pouco como distribuir um cheque assinado em branco: voce nao tem controle sobre como sera usado.

Para entrada, a situacao e um pouco diferente. E seguro usar o array `animal` para entrada, desde que a entrada seja curta o suficiente para caber no array. Nao seria adequado usar `passaro` para entrada, no entanto:

- Alguns compiladores tratam literais de string como constantes somente leitura, levando a um erro em tempo de execucao se voce tentar escrever novos dados sobre eles. Que literais de string sejam constantes e o comportamento exigido em C++, mas nem todos os compiladores ja fizeram essa mudanca do comportamento mais antigo.
- Alguns compiladores usam apenas uma copia de um literal de string para representar todas as ocorrencias daquele literal em um programa.

Vamos ampliar o segundo ponto. O C++ nao garante que literais de string sejam armazenados de forma exclusiva. Ou seja, se voce usar um literal de string `"wren"` varias vezes em um programa, o compilador pode armazenar varias copias da string ou apenas uma copia. Se ele fizer o ultimo, entao definir `passaro` para apontar para um `"wren"` faz com que ele aponte para a unica copia dessa string. Ler um valor em uma string pode afetar o que voce pensava ser uma string independente em outro lugar. Em qualquer caso, como o ponteiro `passaro` e declarado como `const`, o compilador impede qualquer tentativa de alterar o conteudo da localizacao apontada por `passaro`.

Pior ainda e tentar ler informacoes para a localizacao para a qual `ps` aponta. Como `ps` nao e inicializado, voce nao sabe onde as informacoes serao parar. Elas podem ate sobrescrever informacoes que ja estao na memoria. Felizmente, e facil evitar esses problemas: basta usar um array `char` suficientemente grande para receber entrada e nao usar constantes de string nem ponteiros nao inicializados para receber entrada. (Ou voce pode ignorar todos esses problemas e usar objetos `std::string` em vez de arrays.)

> **Cuidado**
>
> Ao ler uma string em um programa, voce deve sempre usar o endereco de memoria previamente alocada. Esse endereco pode ser na forma de um nome de array ou de um ponteiro que foi inicializado usando `new`.

Em seguida, observe o que o codigo a seguir realiza:

```cpp
ps = animal;  // define ps para apontar para string
...
cout << animal << " em " << (int *) animal << endl;
cout << ps << " em " << (int *) ps << endl;
```

Ele produz a seguinte saida:

```
raposa em 0x0065fd30
raposa em 0x0065fd30
```

Normalmente, se voce fornecer ao `cout` um ponteiro, ele imprime um endereco. Mas se o ponteiro for do tipo `char *`, `cout` exibe a string apontada. Se voce quiser ver o endereco da string, tem que converter o ponteiro para outro tipo de ponteiro, como `int *`, que e o que este codigo faz. Entao `ps` e exibido como a string `"raposa"`, mas `(int *) ps` e exibido como o endereco onde a string e encontrada. Note que atribuir `animal` a `ps` nao copia a string; copia o endereco. Isso resulta em dois ponteiros (`animal` e `ps`) para a mesma localizacao de memoria e string.

Para obter uma copia de uma string, voce precisa fazer mais. Primeiro, voce precisa alocar memoria para armazenar a string. Voce pode fazer isso declarando um segundo array ou usando `new`. A segunda abordagem permite que voce personalize o armazenamento para a string:

```cpp
ps = new char[strlen(animal) + 1]; // obtem novo armazenamento
```

A string `"raposa"` nao preenche completamente o array `animal`, portanto essa abordagem desperdicaria espaco. Este trecho de codigo usa `strlen()` para encontrar o comprimento da string; ele adiciona um para obter o comprimento incluindo o caractere nulo. Em seguida, o programa usa `new` para alocar exatamente espaco suficiente para conter a string.

Em seguida, voce precisa de uma forma de copiar a string do array `animal` para o espaco recentemente alocado. Nao funciona atribuir `animal` a `ps` porque isso apenas muda o endereco armazenado em `ps` e, assim, perde a unica forma que o programa tinha de acessar a memoria recentemente alocada. Em vez disso, voce precisa usar a funcao de biblioteca `strcpy()`:

```cpp
strcpy(ps, animal); // copia string para novo armazenamento
```

A funcao `strcpy()` recebe dois argumentos. O primeiro e o endereco de destino, e o segundo e o endereco da string a ser copiada. E sua responsabilidade garantir que o destino realmente seja alocado e tenha espaco suficiente para a copia. Isso e realizado aqui usando `strlen()` para encontrar o tamanho correto e usando `new` para obter memoria livre.

Note que ao usar `strcpy()` e `new`, voce obtem duas copias separadas de `"raposa"`:

```
raposa em 0x0065fd30
raposa em 0x004301c8
```

Observe tambem que `new` localizou o novo armazenamento em uma localizacao de memoria bem distante da do array `animal`.

Frequentemente voce encontra a necessidade de colocar uma string em um array. Voce usa o operador `=` quando inicializa um array; caso contrario, voce usa `strcpy()` ou `strncpy()`. Voce ja viu a funcao `strcpy()`; ela funciona assim:

```cpp
char comida[20] = "cenouras";  // inicializacao
strcpy(comida, "flan");        // caso contrario
```

Note que algo como o seguinte pode causar problemas porque o array `comida` e menor que a string:

```cpp
strcpy(comida, "uma cesta de piquenique cheia de guloseimas");
```

Nesse caso, a funcao copiaria o restante da string nos bytes de memoria imediatamente apos o array, o que pode sobrescrever outra memoria que o programa esta usando. Para evitar esse problema, voce deve usar `strncpy()`. Ela recebe um terceiro argumento: o numero maximo de caracteres a serem copiados. Esteja ciente, entretanto, de que se essa funcao ficar sem espaco antes de atingir o fim da string, ela nao adiciona o caractere nulo. Portanto, voce deve usar a funcao assim:

```cpp
strncpy(comida, "uma cesta de piquenique cheia de guloseimas", 19);
comida[19] = '\0';
```

Isso copia ate 19 caracteres no array e, em seguida, define o ultimo elemento como o caractere nulo. Se a string for menor que 19 caracteres, `strncpy()` adiciona um caractere nulo antes para marcar o verdadeiro fim da string.

> **Cuidado**
>
> Use `strcpy()` ou `strncpy()`, e nao o operador de atribuicao, para atribuir uma string a um array.

Agora que voce viu alguns aspectos de usar strings no estilo C e a biblioteca `cstring`, voce pode apreciar a relativa simplicidade de usar o tipo `string` do C++. Voce (normalmente) nao precisa se preocupar com uma string ultrapassando um array, e voce pode usar o operador de atribuicao em vez de `strcpy()` ou `strncpy()`.

## Usando new para Criar Estruturas Dinamicas

Voce viu como pode ser vantajoso criar arrays durante o tempo de execucao em vez de em tempo de compilacao. O mesmo vale para estruturas. Voce precisa alocar espaco apenas para tantas estruturas quantas um programa precisar durante uma determinada execucao. Novamente, o operador `new` e a ferramenta a usar. Com ele, voce pode criar estruturas dinamicas. Novamente, *dinamico* significa que a memoria e alocada durante o tempo de execucao, nao em tempo de compilacao. Como as classes sao muito semelhantes a estruturas, voce pode usar as tecnicas que vai aprender nesta secao para estruturas tambem com classes.

Usar `new` com estruturas tem duas partes: criar a estrutura e acessar seus membros. Para criar uma estrutura, voce usa o tipo de estrutura com `new`. Por exemplo, para criar uma estrutura nao nomeada do tipo `inflavel` e atribuir seu endereco a um ponteiro adequado, voce pode usar o seguinte:

```cpp
inflavel * ps = new inflavel;
```

Isso atribui a `ps` o endereco de um pedaco de memoria livre grande o suficiente para conter uma estrutura do tipo `inflavel`. Note que a sintaxe e exatamente a mesma que para os tipos nativos do C++.

A parte complicada e acessar os membros. Quando voce cria uma estrutura dinamica, nao pode usar o operador de membro ponto com o nome da estrutura porque a estrutura nao tem nome. Tudo que voce tem e seu endereco. O C++ fornece um operador especificamente para essa situacao: o **operador de membro de ponteiro** (arrow membership operator) (`->`). Esse operador, formado digitando um hifen e, em seguida, um sinal de maior-que, faz para ponteiros de estruturas o que o operador ponto faz para nomes de estruturas. Por exemplo, se `ps` aponta para uma estrutura do tipo `inflavel`, entao `ps->preco` e o membro `preco` da estrutura apontada.

> **Dica**
>
> As vezes novos usuarios de C++ ficam confusos sobre quando usar o operador ponto e quando usar o operador de seta para especificar um membro de estrutura. A regra e simples: se o identificador de estrutura e o nome de uma estrutura, use o operador ponto. Se o identificador e um ponteiro para a estrutura, use o operador de seta.

Uma segunda abordagem, mais feia, para acessar membros de estrutura e perceber que se `ps` e um ponteiro para uma estrutura, entao `*ps` representa o valor apontado — a propria estrutura. Entao, como `*ps` e uma estrutura, `(*ps).preco` e o membro `preco` da estrutura. As regras de precedencia de operadores do C++ exigem que voce use parenteses nessa construcao.

A Listagem 4.21 usa `new` para criar uma estrutura sem nome e demonstra as duas notacoes de ponteiro para acessar membros de estrutura.

**Listagem 4.21 — newstrct.cpp**

```cpp
// newstrct.cpp -- usando new com uma estrutura
#include <iostream>
struct inflavel   // definicao da estrutura
{
    char nome[20];
    float volume;
    double preco;
};
int main()
{
    using namespace std;
    inflavel * ps = new inflavel; // aloca memoria para a estrutura
    cout << "Digite o nome do item inflavel: ";
    cin.get(ps->nome, 20);        // metodo 1 de acesso a membro
    cout << "Digite o volume em pes cubicos: ";
    cin >> (*ps).volume;          // metodo 2 de acesso a membro
    cout << "Digite o preco: $";
    cin >> ps->preco;
    cout << "Nome: " << (*ps).nome << endl;          // metodo 2
    cout << "Volume: " << ps->volume << " pes cubicos\n"; // metodo 1
    cout << "Preco: $" << ps->preco << endl;          // metodo 1
    delete ps;   // libera memoria usada pela estrutura
    return 0;
}
```

Aqui esta uma execucao de exemplo do programa na Listagem 4.21:

```
Digite o nome do item inflavel: Fabuloso Frodo
Digite o volume em pes cubicos: 1.4
Digite o preco: $27.99
Nome: Fabuloso Frodo
Volume: 1.4 pes cubicos
Preco: $27.99
```

## Um Exemplo de Uso de new e delete

Vamos examinar um exemplo que usa `new` e `delete` para gerenciar o armazenamento de entradas de strings do teclado. A Listagem 4.22 define uma funcao `obterNome()` que retorna um ponteiro para uma string de entrada. Essa funcao le a entrada em um grande array temporario e, em seguida, usa `new []` com um tamanho adequado para criar um pedaco de memoria dimensionado para a string de entrada. Entao a funcao retorna o ponteiro para o bloco. Essa abordagem pode economizar muita memoria em programas que leem um grande numero de strings. (Na vida real, onde muitos de nos vivemos, seria mais facil usar a classe `string`, que tem o uso de `new` e `delete` incorporado ao seu design.)

Suponha que o seu programa tenha que ler 1.000 strings e que a maior string possa ter 79 caracteres, mas a maioria das strings e muito mais curta que isso. Se voce usasse arrays `char` para armazenar as strings, precisaria de 1.000 arrays de 80 caracteres cada. Isso sao 80.000 bytes, e muito desse bloco de memoria acabaria sendo inutilizado. Alternativamente, voce poderia criar um array de 1.000 ponteiros para `char` e usar `new` para alocar apenas a quantidade de memoria necessaria para cada string. Isso poderia economizar dezenas de milhares de bytes. Em vez de ter que usar um array grande para cada string, voce adapta a memoria a entrada. Melhor ainda, voce tambem poderia usar `new` para encontrar espaco para armazenar apenas tantos ponteiros quantos forem necessarios. Bem, isso e um pouco ambicioso demais por enquanto. Ate usar um array de 1.000 ponteiros e um pouco ambicioso demais por agora, mas a Listagem 4.22 ilustra um pouco da tecnica. Tambem apenas para ilustrar como `delete` funciona, o programa usa-o para liberar memoria para reutilizacao.

**Listagem 4.22 — delete.cpp**

```cpp
// delete.cpp -- usando o operador delete
#include <iostream>
#include <cstring>  // ou string.h
using namespace std;
char * obterNome(void); // prototipo da funcao
int main()
{
    char * nome;          // cria ponteiro mas sem armazenamento
    nome = obterNome();   // atribui endereco da string a nome
    cout << nome << " em " << (int *) nome << "\n";
    delete [] nome;       // memoria liberada
    nome = obterNome();   // reutiliza memoria liberada
    cout << nome << " em " << (int *) nome << "\n";
    delete [] nome;       // memoria liberada novamente
    return 0;
}
char * obterNome() // retorna ponteiro para nova string
{
    char temp[80];  // armazenamento temporario
    cout << "Digite o sobrenome: ";
    cin >> temp;
    char * pn = new char[strlen(temp) + 1];
    strcpy(pn, temp); // copia string para espaco menor
    return pn;        // temp e perdido quando funcao termina
}
```

Aqui esta uma execucao de exemplo do programa na Listagem 4.22:

```
Digite o sobrenome: Fredeldumpkin
Fredeldumpkin em 0x004326b8
Digite o sobrenome: Pook
Pook em 0x004301c8
```

**Notas do Programa**

Considere a funcao `obterNome()` no programa na Listagem 4.22. Ela usa `cin` para colocar uma palavra de entrada no array `temp`. Em seguida, ela usa `new` para alocar nova memoria para armazenar a palavra. Incluindo o caractere nulo, o programa precisa de `strlen(temp) + 1` caracteres para armazenar a string, entao esse e o valor passado para `new`. Apos o espaco se tornar disponivel, `obterNome()` usa a funcao padrao da biblioteca `strcpy()` para copiar a string de `temp` para o novo bloco. A funcao nao verifica se a string cabe, mas `obterNome()` cuida disso solicitando o numero correto de bytes com `new`. Por fim, a funcao retorna `pn`, o endereco da copia da string.

Em `main()`, o valor de retorno (o endereco) e atribuido ao ponteiro `nome`. Esse ponteiro e definido em `main()`, mas aponta para o bloco de memoria alocado na funcao `obterNome()`. O programa entao imprime a string e o endereco da string.

Em seguida, apos liberar o bloco apontado por `nome`, `main()` chama `obterNome()` uma segunda vez. O C++ nao garante que a memoria recentemente liberada sera a primeira a ser escolhida na proxima vez que `new` for usado, e nesta execucao de exemplo, nao e.

Note neste exemplo que `obterNome()` aloca memoria e `main()` a libera. Geralmente nao e uma boa ideia colocar `new` e `delete` em funcoes separadas porque isso torna mais facil esquecer de usar `delete`. Mas este exemplo separa `new` de `delete` apenas para mostrar que e possivel.

## Armazenamento Automatico, Estatico e Dinamico

O C++ tem tres formas de gerenciar memoria para dados, dependendo do metodo usado para alocar memoria: armazenamento automatico, armazenamento estatico e armazenamento dinamico, as vezes chamado de **area de armazenamento livre** (free store) ou **heap** (monte). Objetos de dados alocados nessas tres formas diferem entre si em quanto tempo permanecem existindo. Vamos dar uma rapida olhada em cada tipo. (O C++11 acrescenta uma quarta forma chamada armazenamento de thread que discutiremos brevemente no Capitulo 9.)

### Armazenamento Automatico

Variaveis comuns definidas dentro de uma funcao usam armazenamento automatico e sao chamadas de **variaveis automaticas** (automatic variables). Esses termos significam que as variaveis entram em existencia automaticamente quando a funcao que as contem e invocada, e expiram quando a funcao termina. Por exemplo, o array `temp` na Listagem 4.22 existe apenas enquanto a funcao `obterNome()` esta ativa. Quando o controle do programa retorna a `main()`, a memoria usada para `temp` e liberada automaticamente. Se `obterNome()` retornasse o endereco de `temp`, o ponteiro `nome` em `main()` estaria apontando para uma localizacao de memoria que em breve seria reutilizada. Essa e uma das razoes pelas quais voce tem que usar `new` em `obterNome()`. Na verdade, os valores automaticos sao locais para o **bloco** (block) que os contem. Um bloco e uma secao de codigo entre chaves. Ate agora, todos os nossos blocos eram funcoes inteiras. Mas, como voce vera no proximo capitulo, voce pode ter blocos dentro de uma funcao. Se voce definir uma variavel dentro de um desses blocos, ela existe apenas enquanto o programa estiver executando instrucoes dentro do bloco.

As variaveis automaticas tipicamente sao armazenadas em uma **pilha** (stack). Isso significa que quando a execucao do programa entra em um bloco de codigo, suas variaveis sao adicionadas consecutivamente a pilha na memoria e, em seguida, sao liberadas na ordem inversa quando a execucao sai do bloco. (Isso e chamado de processo LIFO — ultimo a entrar, primeiro a sair.) Portanto, a pilha cresce e encolhe a medida que a execucao avanca.

### Armazenamento Estatico

O armazenamento estatico e armazenamento que existe ao longo de toda a execucao de um programa inteiro. Ha duas maneiras de tornar uma variavel estatica. Uma e defini-la externamente, fora de uma funcao. A outra e usar a palavra-chave `static` ao declarar uma variavel:

```cpp
static double taxa = 56.50;
```

No K&R C, voce pode inicializar apenas arrays e estruturas estaticos, enquanto o C++ Release 2.0 (e posterior) e o ANSI C permitem tambem inicializar arrays e estruturas automaticos. No entanto, algumas implementacoes do C++ ainda nao implementam a inicializacao para arrays e estruturas automaticos.

O Capitulo 9 discute o armazenamento estatico com mais detalhes. O ponto principal que voce deve notar agora sobre armazenamento automatico e estatico e que esses metodos definem rigidamente o tempo de vida de uma variavel. Ou a variavel existe durante toda a duracao de um programa (uma variavel estatica) ou existe apenas enquanto uma determinada funcao esta sendo executada (uma variavel automatica).

### Armazenamento Dinamico

Os operadores `new` e `delete` fornecem uma abordagem mais flexivel do que as variaveis automaticas e estaticas. Eles gerenciam um pool de memoria, ao qual o C++ se refere como a area de armazenamento livre ou heap. Esse pool e separado da memoria usada para variaveis estaticas e automaticas. Como a Listagem 4.22 mostra, `new` e `delete` permitem que voce aloque memoria em uma funcao e a libere em outra. Assim, o tempo de vida dos dados nao e vinculado arbitrariamente a vida do programa ou a vida de uma funcao. Usar `new` e `delete` juntos oferece muito mais controle sobre como um programa usa memoria do que usar variaveis comuns. No entanto, o gerenciamento de memoria se torna mais complexo. Em uma pilha, o mecanismo automatico de adicao e remocao resulta na parte da memoria em uso sendo sempre contiguous. Mas a interacao entre `new` e `delete` pode deixar buracos na area de armazenamento livre, tornando mais dificil controlar onde alocar novas solicitacoes de memoria.

> **Pilhas, Heaps e Vazamentos de Memoria**
>
> O que acontece se voce nao chamar `delete` apos criar uma variavel na area de armazenamento livre (ou heap) com o operador `new`? A variavel ou construcao alocada dinamicamente na area de armazenamento livre continua a existir se `delete` nao for chamado, mesmo que a memoria que contem o ponteiro tenha sido liberada devido as regras de escopo e tempo de vida do objeto. Em essencia, voce nao tem mais como acessar a construcao na area de armazenamento livre porque o ponteiro para a memoria que a contem sumiu. Voce acabou de criar um **vazamento de memoria** (memory leak). A memoria que vazou permanece inutilizavel durante toda a vida do programa; foi alocada mas nao pode ser desalocada.
>
> Em casos extremos (embora nao incomuns), os vazamentos de memoria podem ser tao graves que consomem toda a memoria disponivel para a aplicacao, fazendo-a falhar com um erro de falta de memoria. Alem disso, esses vazamentos podem afetar negativamente alguns sistemas operacionais ou outros aplicativos em execucao no mesmo espaco de memoria, fazendo com que, por sua vez, eles falhem.
>
> Ate os melhores programadores e empresas de software criam vazamentos de memoria. Para evitá-los, e melhor criar o habito de juntar imediatamente seus operadores `new` e `delete`, planejando e inserindo a exclusao da sua construcao assim que voce a alocar dinamicamente na area de armazenamento livre. Os ponteiros inteligentes (smart pointers) do C++ (Capitulo 16) ajudam a automatizar essa tarefa.

> **Nota**
>
> Os ponteiros estao entre as ferramentas mais poderosas do C++. Eles tambem sao as mais perigosas porque permitem acoes desfavoraveis ao computador, como usar um ponteiro nao inicializado para acessar memoria ou tentar liberar o mesmo bloco de memoria duas vezes. Alem disso, ate voce se acostumar com a notacao de ponteiros e os conceitos de ponteiros atraves da pratica, os ponteiros podem ser confusos. Como os ponteiros sao uma parte importante da programacao C++, eles aparecem repetidamente em discussoes futuras neste livro. A esperanca e que cada exposicao o torne mais confortavel com eles.

---

> Navegacao: [Anterior](capitulo-04-05-unioes-enumeracoes.md) | [Indice](README.md) | [Proximo](capitulo-04-07-alternativas-arrays.md)
