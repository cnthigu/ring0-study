# Capítulo 9 — Duração de Armazenamento, Escopo e Ligação

> Tradução não oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Navegação: [Anterior](capitulo-09-01-compilacao-separada.md) | [Índice](README.md) | [Próximo](capitulo-09-03-namespaces-resumo.md)

## Duração de Armazenamento, Escopo e Ligação (Storage Duration, Scope, and Linkage)

Agora é hora de examinar mais de perto os conceitos de C++ de duração de armazenamento, escopo e ligação. A *duração de armazenamento* (storage duration) descreve por quanto tempo algo permanece na memória. O *escopo* (scope) descreve quanto de um programa pode ver um nome. A *ligação* (linkage) descreve como um nome pode ser compartilhado entre diferentes unidades. Um nome com *ligação externa* (external linkage) pode ser compartilhado entre arquivos, enquanto um nome com *ligação interna* (internal linkage) pode ser compartilhado apenas entre as funções de um único arquivo. Nomes com *sem ligação* (no linkage) são privados para a função ou bloco que os define. Vamos examinar as categorias de duração de armazenamento disponíveis no C++:

- **Duração de armazenamento automática** (automatic storage duration) — Variáveis declaradas dentro de uma definição de função, incluindo parâmetros de função, têm duração de armazenamento automática. Elas são criadas e destruídas automaticamente quando a execução do programa entra e sai do bloco que as contém. O C++ tem dois tipos de variáveis com armazenamento automático: local e register.
- **Duração de armazenamento estática** (static storage duration) — Variáveis definidas fora de uma definição de função ou definidas usando a palavra-chave `static` têm duração de armazenamento estática. Elas persistem enquanto o programa é executado. O C++ tem três tipos de variáveis com duração de armazenamento estática: ligação externa, ligação interna e sem ligação.
- **Duração de armazenamento de thread** (thread storage duration) — Este é um novo desenvolvimento do C++11. Os dados com essa duração existem durante o tempo de vida de um thread. Quaisquer variáveis declaradas com a palavra-chave `thread_local` têm duração armazenamento de thread; caso contrário, elas têm a duração de armazenamento que normalmente teriam.
- **Duração de armazenamento dinâmico** (dynamic storage duration) — Memória alocada com o operador `new` persiste até ser liberada com `delete` ou até o término do programa. Essa memória tem duração de armazenamento dinâmico e às vezes é chamada de *armazenamento livre* (free store) ou *heap* (heap).

### Escopo e Ligação (Scope and Linkage)

O *escopo* descreve a visibilidade de um nome dentro de uma unidade de tradução (um arquivo). Uma variável pode ter escopo local, de arquivo ou de classe. Uma variável com *escopo local* (local scope) é conhecida apenas dentro do bloco em que é definida. (Lembre-se de que um bloco é uma série de instruções entre chaves.) Por exemplo, as variáveis definidas dentro de uma função têm escopo local. Uma variável com *escopo de arquivo* (file scope) é conhecida em todo o arquivo após o ponto em que é definida. Por exemplo, as variáveis definidas fora de uma função têm escopo de arquivo. Escopo de arquivo também é chamado de *escopo global* (global scope) ou *escopo de namespace* (namespace scope). Variáveis de membro de classe (member variables) têm *escopo de classe* (class scope), discutido no Capítulo 10. O C++11 adiciona mais dois tipos de escopo: *escopo de enumerador* (enumerator scope), discutido no Capítulo 10, e escopo de *instrução de namespace* (namespace statement scope), discutido mais adiante neste capítulo.

Funções C++ não podem ser definidas dentro de outras funções, portanto, as funções não podem ter escopo local; qualquer função em um arquivo tem escopo de arquivo. Para funções, *ligação* descreve a medida em que elas podem ser compartilhadas. Uma função com *ligação externa* pode ser usada por outros arquivos, enquanto uma com *ligação interna* pode ser usada somente no arquivo que a contém. Por padrão, as funções têm ligação externa, podendo ser acessadas por código de outros arquivos. Mas é possível declará-las com a palavra-chave `static`, o que lhes dá ligação interna, tornando-as privadas ao arquivo que as contém. Funções e variáveis de escopo de arquivo também podem ter *ligação interna* (internal linkage) ou *ligação externa* (external linkage). Uma variável de arquivo de escopo local tem ligação interna, mas uma de escopo de arquivo tem ligação interna ou externa, dependendo de como é declarada.

### Duração de Armazenamento Automática (Automatic Storage Duration)

As variáveis de função e os parâmetros de função são chamados de variáveis automáticas porque sua duração de armazenamento é automática. Quando uma função inicia, os valores de essas variáveis automáticas são automaticamente alocados na pilha (stack), e quando a função termina, a memória usada por essas variáveis é liberada automaticamente, ou desempilhada (unstacked). Um bloco de código (instruções entre chaves) também pode definir uma região de escopo para variáveis automáticas. Se você definir uma variável dentro de um bloco, ela existirá e será acessível apenas por instruções dentro do bloco. Variáveis automáticas cobrem apenas o bloco em que estão definidas. Código fora do bloco não pode ver a variável usando o nome dela. Mas suponha que você defina uma variável em um bloco que está aninhado dentro de outro bloco. O bloco de inner scope (bloco interno) pode ver (ou seja, usar) as variáveis do outer scope (bloco externo), mas não o contrário.

A Listagem 9.4 ilustra o escopo de variáveis automáticas locais.

**Listagem 9.4** autoscp.cpp

```cpp
// autoscp.cpp -- escopo de variáveis locais
#include <iostream>
void petroleo(int x);
int main()
{
    using namespace std;
    int texas = 31;
    int x = -2;
    cout << "main(): texas = " << texas << ", x = " << x << endl;
    petroleo(x);
    cout << "main() novamente: texas = " << texas
         << ", x = " << x << endl;
    petroleo(texas);
    {                   // abre bloco interno
        int texas = 122;
        cout << "Bloco interno: texas = " << texas;
        cout << ", x = " << x << endl;
    }                   // fecha bloco interno
    cout << "Pos bloco: texas = " << texas
         << ", x = " << x << endl;
    return 0;
}

void petroleo(int x)
{
    using namespace std;
    int texas = 5;
    cout << "petroleo(): texas = " << texas << ", x = " << x << endl;
}
```

Aqui está a saída do programa:

```
main(): texas = 31, x = -2
petroleo(): texas = 5, x = -2
main() novamente: texas = 31, x = -2
petroleo(): texas = 5, x = 31
Bloco interno: texas = 122, x = -2
Pos bloco: texas = 31, x = -2
```

A variável `texas` definida em `main()` é diferente da variável `texas` definida em `petroleo()`. Quando `petroleo()` é chamada na Listagem 9.4, ela define sua própria variável `texas`. Essa variável é inicializada com o valor 5 e existe apenas enquanto a função `petroleo()` está sendo executada. O programa pode usar ambas as variáveis `texas` porque elas têm escopos diferentes. A variável `texas` da função `main()` e a variável `texas` do bloco interno da função `main()` também são variáveis distintas, cada uma com seu próprio escopo.

Quando a execução entra no bloco interno, a nova definição de `texas` substitui a definição anterior para a variável de mesmo nome, e `texas` então se refere à nova variável com o valor 122 até que o bloco interno termine. Como mostra a saída, a variável `texas` do bloco externo de `main()` recupera seu valor de 31 quando o bloco interno termina.

> **Variáveis Register**
>
> O C++11 eliminou qualquer uso de `register` como modificador de armazenamento. Porém, para evitar interromper código legado, o C++11 manteve `register` como uma palavra-chave, mas sem nenhum efeito especial. O uso de `register` como modificador de armazenamento era uma herança do C, onde servia como *dica* para o compilador, sugerindo que o programador queria que uma variável específica fosse acessível rapidamente e que o compilador devesse manter a variável num registrador em vez de na memória. Com os compiladores de otimização modernos, não é mais necessário que o programador forneça essas dicas.

### Variáveis de Duração de Armazenamento Estática (Static Duration Storage)

Como menciona o Capítulo 4, "Tipos Compostos", o C++ fornece três tipos de ligação para variáveis de armazenamento estático:

- Ligação externa (acessível de outros arquivos)
- Ligação interna (acessível apenas dentro do arquivo atual)
- Sem ligação (acessível apenas dentro da função ou bloco atual)

Todos os três tipos de armazenamento estático duram o programa inteiro, mas o compilador aloca um número fixo de blocos de memória para manter esses valores e não recicla os blocos. Variáveis de escopo de arquivo têm ligação externa ou interna; variáveis de escopo de bloco criadas com `static` não têm ligação.

Para criar uma variável de armazenamento estático de ligação externa, você a declara fora de qualquer bloco. Para criar uma variável estática com ligação interna, você a declara fora de qualquer bloco e usa o modificador `static`. Para criar uma variável estática sem ligação, você a declara dentro de um bloco, usando o modificador `static`. A duração de armazenamento desses três tipos é estática (eles existem durante todo o tempo de vida do programa), mas diferem em termos de escopo (onde podem ser vistos) e ligação.

Por exemplo:

```cpp
...
int global = 1000;        // ligação externa
static int um_arquivo = 50; // ligação interna
int main()
{
    ...
}
void func1(int n)
{
    static int count = 0; // sem ligação
    int custo = 1;
    ...
}
void func2(int n)
{
    ...
}
```

A variável `global` pode ser usada por qualquer arquivo do programa. A variável `um_arquivo` pode ser usada somente no arquivo que a contém (usando a palavra-chave `static` torna-a local ao arquivo). A variável `count` pode ser usada apenas dentro de `func1()`, mas mantém seu valor entre chamadas a essa função.

#### Inicializando Variáveis Estáticas (Initializing Static Variables)

Variáveis estáticas podem ser *inicializadas com zero* (zero-initialized) ou com uma *inicialização constante* (constant initialization), seguidas opcionalmente por *inicialização dinâmica* (dynamic initialization). Inicialização zero-e-constante é chamada coletivamente de *inicialização estática* (static initialization). Isso acontece antes de qualquer inicialização dinâmica ocorrer. A inicialização zero é simples: a variável é inicializada para zero. Para tipos escalares, isso significa que o valor é definido como 0, para ponteiros isso significa null pointer e assim por diante. Com C++11, a inicialização constante pode ser realizada explicitamente fornecendo uma expressão constante para inicializar a variável ou implicitamente se os tipos necessários e suas inicializações padrão forem qualificadas como constantes.

Se você não inicializar uma variável de duração de armazenamento estática, ela será inicializada com zero por padrão. Isso é diferente das variáveis automáticas, que não são inicializadas automaticamente — seus valores dependerão do que quer que esteja naquele local de memória.

### Duração de Armazenamento Estática com Ligação Externa (Static Duration, External Linkage)

Variáveis com ligação externa são frequentemente chamadas de *variáveis externas* (external variables). Elas devem ser definidas fora de qualquer definição de função e têm ligação externa e escopo de arquivo. Às vezes, variáveis externas são chamadas de *variáveis globais* (global variables) para distingui-las das variáveis automáticas locais. Note que variáveis com ligação interna também têm escopo de arquivo, mas não são globais no sentido de estarem disponíveis para outros arquivos.

Para usar uma variável externa em um arquivo que não a contém, você deve declarar a variável com a palavra-chave `extern`:

```cpp
extern int cats = 20; // definição porque é inicializada
extern int dogs;      // declaração, não definição
```

Há uma diferença entre uma *definição de variável* (variable definition) e uma *declaração de variável* (variable declaration). Uma definição de variável aloca memória e possivelmente atribui um valor de inicialização a ela. Uma declaração de variável torna um nome conhecido a um programa — não aloca memória. Variáveis externas têm implicações especiais. Para usar uma variável externa em vários arquivos, você pode definir a variável em apenas um arquivo. Em todos os outros arquivos que usam a variável, você declara a variável com `extern`:

As Listagens 9.5 e 9.6 fornecem um exemplo de programa que usa variáveis externas.

**Listagem 9.5** external.cpp

```cpp
// external.cpp -- variáveis externas
// compile com support.cpp
#include <iostream>
using namespace std;
// variáveis externas
double aquecimento = 0.3;        // variável global
char estacao[80] = "Verão";      // nome da estação

// protótipos de função
void atualizar(double dt);
void local();

int main()
{
    cout << "Temperatura global = " << aquecimento << " graus.\n";
    cout << "Estacao global: " << estacao << endl;
    local();                       // função que usa locais
    atualizar(0.1);                // função que atualiza global
    cout << "Temperatura global = " << aquecimento << " graus.\n";
    cout << "Estacao global: " << estacao << endl;
    local();
    atualizar(0.2);
    cout << "Temperatura global = " << aquecimento << " graus.\n";
    cout << "Estacao global: " << estacao << endl;
    cout << "Encerrado.\n";
    return 0;
}
```

**Listagem 9.6** support.cpp

```cpp
// support.cpp -- suporte para external.cpp
#include <iostream>
#include <cstring>
extern double aquecimento;         // usa variável definida em outro arquivo
extern char estacao[80];           // usa variável definida em outro arquivo

void atualizar(double dt)
{
    using namespace std;
    aquecimento += dt;             // altera a variável global
    cout << "Temperatura global atualizada para " << aquecimento;
    cout << " graus.\n";
}

void local()
{
    using namespace std;
    double aquecimento = 0.8;      // nova variável local, oculta global
    cout << "Temperatura local = " << aquecimento << " graus.\n";
    cout << "Mas a estacao global ainda e: " << estacao << endl;
}
```

Aqui está uma execução de exemplo deste programa de dois arquivos:

```
Temperatura global = 0.3 graus.
Estacao global: Verão
Temperatura local = 0.8 graus.
Mas a estacao global ainda e: Verão
Temperatura global atualizada para 0.4 graus.
Temperatura global = 0.4 graus.
Estacao global: Verão
Temperatura local = 0.8 graus.
Mas a estacao global ainda e: Verão
Temperatura global atualizada para 0.6 graus.
Temperatura global = 0.6 graus.
Estacao global: Verão
Encerrado.
```

Note que a variável local `aquecimento` em `local()` oculta a variável global `aquecimento`. C++ usa uma regra chamada *escopo de bloco* (block scope): uma variável em um bloco mais interno que tem o mesmo nome que uma variável em um bloco mais externo oculta a variável externa dentro do bloco interno. Contudo, dentro de `local()`, você ainda pode acessar `estacao` porque ela é global e não tem correspondente local que a oculte.

> **Aviso de Inicialização Global**
>
> Um uso comum de variáveis globais é compartilhar informações entre diferentes funções em diferentes arquivos, como fizemos com `aquecimento` e `estacao`. Note, entretanto, que o uso excessivo de variáveis globais pode levar a programas difíceis de manter. Quando qualquer função pode alterar qualquer variável, é muito difícil rastrear bugs. Uma alternativa à variável global é passar a informação como argumento de função. Outra alternativa é encapsular dados em um objeto de classe, que é a abordagem OOP.

#### O Operador de Resolução de Escopo (Scope Resolution Operator)

C++ provê o *operador de resolução de escopo* (scope resolution operator), `::`, que quando prefixado a uma variável de nome, significa a versão de escopo de arquivo dessa variável. Portanto, `::versao` significa a versão com escopo de arquivo de uma variável chamada `versao`, mesmo que o código seja executado em um bloco com uma versão local de `versao`. O uso do operador de resolução de escopo fica mais claro com os namespaces, a ser discutidos mais tarde neste capítulo.

### Duração de Armazenamento Estática com Ligação Interna (Static Duration, Internal Linkage)

Aplicar o modificador `static` a uma variável de escopo de arquivo dá à variável ligação interna. A diferença entre uma variável com ligação interna e outra com ligação externa é que a variável com ligação interna está disponível somente dentro de um arquivo, enquanto a com ligação externa pode ser usada em outros arquivos também.

Há outra forma de criar variável de escopo de arquivo com ligação interna: se você usar uma expressão `const` estática de escopo de arquivo. Se duas funções em dois arquivos diferentes precisam usar a mesma variável global estática, cada arquivo simplesmente pode definir uma cópia da constante. Não há conflito porque as constantes da classe de armazenamento estática têm ligação interna.

As Listagens 9.7 e 9.8 fornecem um exemplo de variável com ligação interna.

**Listagem 9.7** twofile1.cpp

```cpp
// twofile1.cpp -- variáveis de ligação interna
// compile com twofile2.cpp
#include <iostream>
int total_geral = 10;        // ligação externa
static int um_arquivo = 50; // ligação interna

// protótipos de função
void acesso_remoto();
void local();

using namespace std;
int main()
{
    cout << "total_geral = " << total_geral << ", um_arquivo = "
         << um_arquivo << endl;
    local();
    acesso_remoto();
    cout << "Apos funções: total_geral = " << total_geral
         << endl;
    return 0;
}

void local()
{
    using namespace std;
    cout << "Funcao local() exibe total_geral = " << total_geral << endl;
}
```

**Listagem 9.8** twofile2.cpp

```cpp
// twofile2.cpp -- ligação externa
// compile com twofile1.cpp
#include <iostream>
extern int total_geral;   // usa total_geral de twofile1.cpp
// static int um_arquivo;   // seria variável separada de twofile1

// protótipos
void acesso_remoto();

void acesso_remoto()
{
    using namespace std;
    total_geral += 5; // usa total_geral de twofile1.cpp
    cout << "Funcao acesso_remoto() altera total_geral para " << total_geral << endl;
}
```

A saída do programa é:

```
total_geral = 10, um_arquivo = 50
Funcao local() exibe total_geral = 10
Funcao acesso_remoto() altera total_geral para 15
Apos funções: total_geral = 15
```

Se você descomentar a linha `static int um_arquivo;` em `twofile2.cpp`, o arquivo teria sua própria variável separada chamada `um_arquivo`, com ligação interna, que não entraria em conflito com a de `twofile1.cpp`. Mas o arquivo também poderia, se necessário, usar `extern int um_arquivo;` para declarar que quer acessar `um_arquivo` de `twofile1.cpp` — exceto que `um_arquivo` tem ligação interna, portanto, não pode ser compartilhada com outros arquivos.

### Duração de Armazenamento Estática sem Ligação (Static Storage Duration, No Linkage)

O terceiro tipo de variável de duração de armazenamento estática é o tipo local estático (local static type). Você cria esse tipo aplicando o modificador `static` a uma variável local (definida dentro de um bloco). Quando você inicializa uma variável local estática, o programa inicializa a variável apenas uma vez, quando a função é chamada pela primeira vez. Subsequentemente, o valor da variável é preservado entre chamadas de função. A Listagem 9.9 usa uma variável estática local para acompanhar quantas vezes uma função é chamada.

**Listagem 9.9** static.cpp

```cpp
// static.cpp -- use uma variável local estática
#include <iostream>

const int TamStr = 1024; // tamanho do buffer de string

void contar_str(const char * str)
{
    using namespace std;
    static int contagem = 0;     // conta total de palavras
    int entrada = 0;             // conta palavras desta chamada
    while (*str)
    {
        while (*str == ' ' || *str == '\n' || *str == '\t')
            str++;               // pula espaços em branco
        if (*str)                // tem uma palavra
        {
            entrada++;
            while (*str != ' ' && *str != '\n'
                   && *str != '\t' && *str)
                str++;           // percorre a palavra
        }
    }
    contagem += entrada;
    cout << entrada << " palavras nesta string\n";
    cout << contagem << " palavras no total\n";
}

int main()
{
    using namespace std;
    char prox[TamStr];
    while (cin.getline(prox, TamStr))
        contar_str(prox);
    return 0;
}
```

Aqui está uma execução de exemplo:

```
O rápido corvo vermelho
4 palavras nesta string
4 palavras no total
salta sobre o cachorro preguiçoso
5 palavras nesta string
9 palavras no total
```

A variável local estática `contagem` mantém seu valor entre chamadas. Ela é inicializada com zero apenas uma vez, na primeira vez que `contar_str()` é chamada. Nas chamadas subsequentes, ela retém o valor atribuído ao final da chamada anterior. A variável local automática `entrada`, por outro lado, é recriada a cada vez que `contar_str()` é chamada.

### Especificadores e Qualificadores (Specifiers and Qualifiers)

Certos palavras-chave C++ são chamados de especificadores de classe de armazenamento (storage class specifiers) e de qualificadores cv (cv-qualifiers). Já vimos vários especificadores. Aqui está a lista completa:

- `auto` (removido como especificador de duração de armazenamento em C++11)
- `register`
- `static`
- `extern`
- `thread_local` (adicionado no C++11)
- `mutable`

Os qualificadores cv são `const` e `volatile`. A lista de qualificadores cv é necessária para compreender melhor o uso de `const`.

O qualificador `volatile` indica que o valor de uma variável pode ser alterado mesmo que o código do programa não o mude. Por exemplo, você pode fazer um ponteiro apontar para um local de hardware, e o hardware pode alterar o valor armazenado nesse local. O qualificador diz ao compilador para não fazer suposições sobre o valor dessa variável. Sem `volatile`, o compilador pode otimizar o código através do cache: ele pode armazenar o valor em um registrador e reutilizar esse valor para cada acesso, sem verificar o local de memória de onde originalmente carregou o valor. O qualificador `volatile` impede que o compilador faça isso. Por isso, `volatile` é principalmente usado em programação embarcada (embedded programming) e em código de sistema (system code).

A palavra-chave `mutable` é usada para indicar que um membro particular de uma `struct` ou `class` pode ser modificado mesmo que o objeto em particular seja `const`. Por exemplo, considere o seguinte:

```cpp
struct produto
{
    int numero;
    mutable int acessos;
    ...
};
const produto bolsa = {537, 0, ...}; // objeto const
bolsa.acessos++;   // isso é permitido -- acessos é mutable
```

O modificador `mutable` é necessário porque sem ele, a palavra-chave `const` em `const produto bolsa` tornaria todos os membros da estrutura somente leitura, impedindo que `acessos` fosse modificado. A palavra-chave `mutable` possibilita flexibilidade local a essa regra.

#### Mais Sobre const (More About const)

Na era do C, uma declaração como:

```cpp
const int TAMANHOS = 5;
```

criava uma constante de escopo de arquivo com ligação externa por padrão. Portanto, se você tivesse essa declaração em um arquivo de cabeçalho e o incluísse em vários arquivos do mesmo programa, cada arquivo receberia uma cópia da variável, o que causaria erros de múltipla definição.

Em C++, a palavra-chave `const` altera esse comportamento. Em C++, uma constante com o modificador `const` em escopo de arquivo tem ligação interna. Isso significa que você pode usar a seguinte declaração em vários arquivos de um único programa:

```cpp
const int TAMANHOS = 5;
```

sem nenhum erro. Isso é diferente do comportamento do C, onde a ligação padrão para variáveis de escopo de arquivo é externa. A razão pela qual C++ usa ligação interna para `const` é que esse esquema facilita a criação de constantes usadas por vários arquivos: você coloca as definições em um arquivo de cabeçalho que outros arquivos incluem com `#include`. Se as constantes `const` de escopo de arquivo tivessem ligação externa, elas seriam definidas externamente e causariam erros de link. Com ligação interna, cada arquivo tem sua própria cópia.

Se, por alguma razão, você quiser fazer uma constante ter ligação externa, você pode usar a palavra-chave `extern` para sobrescrever o padrão interno:

```cpp
extern const int ESTADOS = 50; // definição com ligação externa
```

Agora você teria que usar `extern const int ESTADOS;` em todos os outros arquivos que usam a constante, para usar a definição compartilhada de `ESTADOS`.

### Funções e Ligação (Functions and Linkage)

Como você pode ter percebido a esta altura, C++ tem opções de ligação para funções bem como para variáveis. Como mencionado antes, funções C++ têm duração de armazenamento estática por padrão. Como a duração de armazenamento padrão das funções é estática, o único aspecto de duração de armazenamento que você realmente precisa especificar para funções é a ligação.

Por padrão, funções têm ligação externa, o que significa que elas podem ser compartilhadas entre arquivos. Na verdade, você pode usar `extern` para indicar isso explicitamente, mas isso não é necessário. Você também pode usar a palavra-chave `static` para dar a uma função ligação interna, tornando-a privada para um arquivo:

```cpp
static int private_func() // somente usado neste arquivo
{...}
```

Isso faz com que a função seja conhecida apenas no arquivo que a contém. Como com variáveis, uma função estática de ligação interna sobrescreve uma definição de ligação externa, se os dois arquivos tiverem a mesma função estática:

```cpp
// file1
static int calc(int a, int b) {return a + b;}
// file2
static int calc(int a, int b) {return a * b;} // diferente de file1
```

Isso implica que se o arquivo1.cpp e arquivo2.cpp contiverem ambos uma definição de uma função chamada `calc()`, e se pelo menos uma delas for declarada `static`, não haverá conflito.

O padrão de ligação de funções significa que o vinculador não precisa distinguir entre a mesma função em arquivos diferentes. Isso é por isso que você deve usar nomes de protótipos de função que incluam os tipos de parâmetros, para que o vinculador possa verificar que a função de chamada e a função chamada concordam com os tipos de argumento. Isso é uma regra de Uma Definição (One-Definition Rule, ODR): o programa como um todo pode ter apenas uma definição de uma função não-inline com ligação externa. Mas um arquivo pode ter uma função estática (ligação interna) com o mesmo nome de uma função de ligação externa em outro arquivo.

#### Ligação de Linguagem (Language Linking)

Outro tipo de ligação, chamado de *ligação de linguagem* (language linking), afeta as funções. Considere o seguinte: o vinculador precisa de um nome simbólico diferente para cada função distinta. Em C, isso é fácil porque você não pode ter duas funções com o mesmo nome. Mas o C++ permite sobrecarregamento de funções, o que significa que você pode ter várias funções com o mesmo nome. O C++ lida com isso fazendo uso de decoração de nomes ou *embaralhamento de nomes* (name mangling), como descrito no Capítulo 8, para gerar um nome simbólico diferente para cada sobrecarga de função. Mas, suponha que você queira usar uma função C em um programa C++. Se a função C `spiff()` for compilada com C, o nome simbólico para ela seria `_spiff`, enquanto se compilada com C++, o nome simbólico poderia ser algo como `_spiff_v`, `_spiff_i`, dependendo dos tipos de argumento.

Para tratar esse tipo de problema, você pode usar o especificador de vinculação de função (`function linkage specifier`) para indicar ao compilador que use convenção de nomeação C para funções. O C++ pode usar as seguintes especificações:

```cpp
extern "C" void spiff(int); // vinculação C
extern void spiff(int);     // vinculação C++
extern "C++" void spiff(int); // vinculação C++ (redundante)
```

O `extern "C"` em `extern "C" void spiff(int);` diz ao compilador para use convenção de nomeação C para a função `spiff()`. O padrão C++ suporta `extern "C"` e `extern "C++"`. Outras especificações, como `extern "FORTRAN"`, também são possíveis dependendo da implementação.

### Esquemas de Armazenamento e Alocação Dinâmica (Storage Schemes and Dynamic Allocation)

Você pode criar e destruir variáveis durante a execução do programa, algo que os esquemas de armazenamento anteriores não permitem. C++ fornece dois operadores, `new` e `delete`, para fazer isso. Discutimos esses operadores anteriormente, mas vamos revisitá-los à luz dos esquemas de armazenamento e ligação.

O operador `new` aloca memória na *heap* ou no *armazenamento livre* (free store). A alocação é temporária no sentido de que o bloco de memória permanece alocado até ser explicitamente liberado. Com isso dito, pense na alocação como sendo semelhante a declararação de uma variável de escopo de arquivo, exceto que a variável não tem nome. Em vez disso, `new` fornece um ponteiro que é o endereço do bloco de memória e que também pode ser armazenado em uma variável de ponteiro.

#### Inicialização com new (Initialization with new)

Se você aloca um tipo simples variável (escalar), como `int` ou `double`, você pode inicializá-la colocando um valor de inicialização entre parênteses após o tipo:

```cpp
int *pi = new int(6);       // *pi = 6
double *pd = new double(99.99); // *pd = 99.99
```

A seguinte sintaxe de chaves funciona para tipos escalares e para classes:

```cpp
struct ponto {double x, y, z;};
double *pdd = new double (99.99); // inicializa *pdd com 99.99
ponto *ppd = new ponto {4, 2, 2}; // inicializa membros de ponto com 4, 2, 2
int *pia = new int [20];           // pia aponta para um array de 20 ints
int *pi = new int ();              // inicializa *pi com 0 (C++11)
```

#### Quando new Falha (When new Fails)

Pode acontecer que `new` não consiga encontrar a memória solicitada. Por padrão, se `new` não conseguir encontrar memória suficiente, ele lança uma exceção `std::bad_alloc`. Mas isso pode ser tratado. Se você adicionar `(std::nothrow)` após a palavra-chave `new`, `new` retornará `nullptr` em vez de lançar uma exceção se a alocação falhar:

```cpp
int *pi = new (std::nothrow) int;       // retorna nullptr se falhar
int *pa = new (std::nothrow) int[500];  // retorna nullptr se falhar
```

#### new: Operadores, Funções e Funções de Substituição (new: Operators, Functions, and Replacement Functions)

Os operadores `new` e `new[]` chamam funções de alocação:

```cpp
void *operator new(std::size_t);       // usado por new
void *operator new[](std::size_t);     // usado por new[]
```

Essas são chamadas de *funções de alocação* (allocation functions) e fazem parte do namespace global. Da mesma forma, existem funções de desalocação (deallocation functions) usadas por `delete` e `delete[]`:

```cpp
void operator delete(void *);
void operator delete[](void *);
```

As funções de alocação e desalocação para `new`, `new[]`, `delete` e `delete[]` são chamadas coletivamente de *funções de gerenciamento de memória* (memory management functions). Você pode substituí-las por versões personalizadas para personalizar o gerenciamento de memória. Isso é discutido mais profundamente em capítulos posteriores.

#### Placement new (Placement new)

Normalmente, o operador `new` encontra um bloco de memória não utilizada no *heap* (armazenamento livre) e o retorna. O operador `new` tem uma variante chamada *placement new* que permite especificar o local de memória a ser utilizado. O programador pode usar esse recurso para configurar sua própria estrutura de gerenciamento de memória, para lidar com memória administrada por hardware ou para construir um objeto em um local específico.

Para usar o placement new, primeiro inclua o arquivo de cabeçalho `<new>`, que fornece a declaração prototípica para esta versão de `new`. Em seguida, use `new` com um argumento que forneça o endereço do local a ser usado:

```cpp
#include <new>
struct coisas {
    int n;
    double x;
};
char buffer1[50];
char buffer2[500];
int main()
{
    coisas *p1, *p2;
    p1 = new coisas;              // alocação padrão no heap
    p2 = new (buffer1) coisas;   // placement new — usa buffer1
    // ...
}
```

A Listagem 9.10 fornece um exemplo mais elaborado.

**Listagem 9.10** newplace.cpp

```cpp
// newplace.cpp -- novo placement
#include <iostream>
#include <new>         // para placement new
const int TAM = 512;   // tamanho do buffer
int buffer[TAM];       // buffer estático de ints

int main()
{
    using namespace std;
    double *pd1, *pd2;
    int *pi;
    short *ps;
    // como alocar mem no heap
    pd1 = new double[TAM]; // vai para o heap
    // como usar placement new para alocar em buffer
    pd2 = new (buffer) double[TAM]; // coloca no buffer estático
    // como usar ints no buffer
    pi = new (buffer) int[TAM]; // coloca no buffer estático
    // ps compartilha memória do buffer com pd2, pi
    ps = new (buffer + TAM/2) short[TAM/2];

    for (int i = 0; i < TAM; i++)
        pd2[i] = pd1[i] = 1000 + 20.0 * i;
    cout << "Memoria 1 (pd1):\n";
    for (int i = 0; i < TAM; i+=64)
        cout << pd1[i] << " ";
    cout << endl;
    cout << "Memoria 2 (pd2):\n";
    for (int i = 0; i < TAM; i+=64)
        cout << pd2[i] << " ";
    cout << endl;

    delete [] pd1;
    // delete [] pd2;  // NÃO faça isso! buffer é estático, não heap

    return 0;
}
```

Aqui está uma amostra da saída do programa:

```
Memoria 1 (pd1):
1000 2280 3560 4840 6120 7400 8680 9960
Memoria 2 (pd2):
1000 2280 3560 4840 6120 7400 8680 9960
```

Observe que o programa usa `delete [] pd1` mas não `delete [] pd2`. Isso ocorre porque `pd2` aponta para o buffer estático, não para a memória alocada pelo heap. O uso de `delete` com um ponteiro não-heap causaria um comportamento indefinido. Portanto, quando você usa o placement new para colocar um objeto em memória estática ou em outra memória que você gerencia, você deve assumir a responsabilidade de limpar os objetos dessa memória. Ou seja, você deve chamar o destrutor do objeto explicitamente, se necessário, mas não deve usar `delete` naquele ponteiro.

O placement new funciona colocando o objeto em um local especificado pelo programador, e retorna o endereço desse local. A função tem uma implementação simples:

```cpp
void *operator new(size_t, void *ptr)
{
    return ptr;
}
```

Há uma coisa que você deve ter em mente ao usar o placement new: você precisa garantir que a memória sendo usada seja grande o suficiente para o objeto. E se você alocar dois objetos de tamanhos diferentes no mesmo buffer, precisará garantir que cada objeto comece em um alinhamento adequado.

---

Navegação: [Anterior](capitulo-09-01-compilacao-separada.md) | [Índice](README.md) | [Próximo](capitulo-09-03-namespaces-resumo.md)
