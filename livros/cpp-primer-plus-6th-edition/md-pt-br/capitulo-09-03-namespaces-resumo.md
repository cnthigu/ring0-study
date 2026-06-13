# Capítulo 9 — Namespaces, Resumo e Exercícios

> Tradução não oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Navegação: [Anterior](capitulo-09-02-duracao-escopo-ligacao.md) | [Índice](README.md) | [Próximo](capitulo-10-01-objetos-classes.md)

## Namespaces (Namespaces)

Nomes em C++, como todos os nomes, podem entrar em conflito se diferentes partes de um programa usarem nomes idênticos para designar conceitos diferentes. O C++ introduz namespaces como uma forma de gerenciar e controlar o escopo de nomes. A necessidade de namespaces se intensifica à medida que os projetos crescem e passam a usar código de vários fornecedores.

### Namespaces Tradicionais do C++ (Traditional C++ Namespaces)

Para entender os novos recursos de namespace do C++, é útil primeiro identificar as formas existentes de namespace em C++. A declaração de uma variável em um bloco define um *escopo local* (local scope) para esse nome; a variável é conhecida apenas dentro do bloco. Uma variável declarada externamente a qualquer função tem *escopo de arquivo* (file scope) ou *escopo de namespace* (namespace scope); ela é conhecida em todo o arquivo onde é declarada. Um nome de membro de classe tem escopo de classe; é parte da classe.

O C++ tem um conceito chamado *região declarativa* (declarative region), que é uma região em que declarações podem ser feitas. Por exemplo, você pode declarar variáveis globais em qualquer ponto fora de uma função, e a declaração pode existir em qualquer ponto do arquivo após o local onde é feita. Esse espaço é a região declarativa. Uma parte de uma região declarativa em que um nome pode ser usado é chamada de *escopo potencial* (potential scope). Esse escopo começa no ponto da declaração e termina no final da região declarativa que o contém.

Considere o seguinte pseudocódigo:

```cpp
int x = 10;   // x declarado em escopo de arquivo (namespace global)
int main()
{
    int goo;
    for (int i = 0; i < x; i++)
    {
        int temp = 0;
        int goo = temp * i;   // oculta a variável goo acima
    }
    return 0;
}
void orp(int ex)
{
    int m;
    {
        int ro = 2;
    }
}
```

A variável `goo` declarada dentro do `for` oculta a variável `goo` declarada dentro de `main()`. O escopo potencial de `goo` interna começa na sua declaração e termina no fechamento do bloco `for`. O escopo efetivo de `goo` interna não inclui o bloco externo de `main()`, onde a variável `goo` externa está visível.

### Novos Recursos de Namespace (New Namespace Features)

O C++ adicionou a capacidade de criar *namespaces nomeados* (named namespaces) ao fornecer uma nova forma de região declarativa. Um namespace pode ser global ou dentro de outro namespace, mas não pode estar dentro de um bloco. Portanto, um nome declarado em um namespace tem ligação externa por padrão (a menos que seja uma constante).

A declaração de namespace tem a seguinte forma geral:

```cpp
namespace Jack {
    double pail;              // variável
    void fetch();             // protótipo de função
    int pal;                  // variável
    struct Hill { ... };      // declaração de estrutura
}
namespace Jill {
    double bucket(double n) { ... } // definição de função
    double fetch;
    int pal;
    struct Hill { ... };
}
```

Os namespaces podem ser colocados no nível global ou dentro de outros namespaces, mas não dentro de um bloco. Nomes em qualquer namespace não conflitam com nomes de outro namespace. Portanto, o `fetch` em `Jack` pode coexistir com o `fetch` em `Jill`, e o `Hill` em `Jill` pode coexistir com um `Hill` externo ao namespace. As regras que regem declarações e definições em um namespace são as mesmas que regem declarações e definições globais.

Além dos namespaces definidos pelo usuário, há mais um namespace: o *namespace global* (global namespace). Ele corresponde à região declarativa em nível de arquivo, de modo que o que antes se chamava de variáveis globais agora são descritas como parte do namespace global.

Namespaces são *abertos* (open), o que significa que você pode adicionar nomes a um namespace existente. Por exemplo, a seguinte instrução adiciona o nome `ganso` à lista existente de nomes em `Jill`:

```cpp
namespace Jill {
    char * ganso(const char *);
}
```

Da mesma forma, o namespace `Jack` original fornece um protótipo para uma função `fetch()`. Você pode fornecer o código para a função mais tarde no arquivo (ou em outro arquivo) usando o namespace `Jack` novamente:

```cpp
namespace Jack {
    void fetch()
    {
        // ...
    }
}
```

Para usar nomes de um namespace, você pode qualificar o nome com o operador de resolução de escopo (`::`, scope-resolution operator):

```cpp
Jack::pail = 12.34;  // usa uma variável
Jill::Hill mole;     // cria uma estrutura do tipo Hill
Jack::fetch();       // usa uma função
```

Um nome não qualificado, como `pail`, é chamado de *nome não qualificado* (unqualified name), enquanto um nome com o namespace, como `Jack::pail`, é chamado de *nome qualificado* (qualified name).

### Declarações using e Diretivas using (using Declarations and using Directives)

Ter que qualificar nomes toda vez que são usados não é sempre algo apelativo, portanto o C++ fornece dois mecanismos — a *declaração using* (using declaration) e a *diretiva using* (using directive) — para simplificar o uso de nomes de namespace. A declaração `using` permite que você disponibilize identificadores específicos, e a diretiva `using` torna todo o namespace acessível.

A declaração `using` envolve preceder um nome qualificado com a palavra-chave `using`:

```cpp
using Jill::fetch;  // uma declaração using
```

Uma declaração `using` adiciona um nome específico à região declarativa na qual ela ocorre. Por exemplo, a declaração `using` de `Jill::fetch` em `main()` adiciona `fetch` à região declarativa definida por `main()`. Após fazer essa declaração, você pode usar o nome `fetch` em vez de `Jill::fetch`. O código a seguir ilustra esses pontos:

```cpp
namespace Jill {
    double bucket(double n) { ... }
    double fetch;
    struct Hill { ... };
}
char fetch;
int main()
{
    using Jill::fetch;  // coloca fetch no namespace local
    double fetch;       // Erro! Já existe um fetch local
    cin >> fetch;       // lê um valor em Jill::fetch
    cin >> ::fetch;     // lê um valor no fetch global
    // ...
}
```

Como uma declaração `using` adiciona o nome à região declarativa local, este exemplo impede a criação de outra variável local com o nome `fetch`. Também, como qualquer outra variável local, `fetch` ocultaria uma variável global de mesmo nome.

Colocar uma declaração `using` no nível externo adiciona o nome ao namespace global:

```cpp
void outra();
namespace Jill {
    double bucket(double n) { ... }
    double fetch;
    struct Hill { ... };
}
using Jill::fetch;  // coloca fetch no namespace global
int main()
{
    cin >> fetch;   // lê um valor em Jill::fetch
    outra();
    // ...
}
void outra()
{
    cout << fetch;  // exibe Jill::fetch
    // ...
}
```

Uma declaração `using`, portanto, disponibiliza um único nome. Em contraste, a diretiva `using` torna *todos* os nomes disponíveis. Uma diretiva `using` envolve preceder um nome de namespace com as palavras-chave `using namespace`, e torna todos os nomes no namespace disponíveis sem o uso do operador de resolução de escopo:

```cpp
using namespace Jack;  // torna todos os nomes em Jack disponíveis
```

Colocar uma diretiva `using` no nível global torna os nomes do namespace globalmente disponíveis. Você viu isso em ação várias vezes neste livro na seguinte forma:

```cpp
#include <iostream>            // coloca nomes no namespace std
using namespace std;           // torna os nomes disponíveis globalmente
```

Colocar uma diretiva `using` em uma função específica torna os nomes disponíveis apenas naquela função. Uma coisa que deve ser mantida em mente sobre diretivas e declarações `using` é que elas aumentam a possibilidade de conflitos de nomes. Ou seja, se você tiver tanto o namespace `jack` quanto o namespace `jill` disponíveis e usar o operador de resolução de escopo, não há ambiguidade:

```cpp
jack::pal = 3;
jill::pal = 10;
```

As variáveis `jack::pal` e `jill::pal` são identificadores distintos para locais de memória distintos. No entanto, se você empregar declarações `using`, a situação muda:

```cpp
using jack::pal;
using jill::pal;
pal = 4;          // qual? agora há um conflito
```

Na verdade, o compilador não vai deixar você usar ambas as declarações `using` por causa da ambiguidade que seria criada.

### Diretivas using Versus Declarações using (using Directives Versus using Declarations)

Usar uma diretiva `using` para importar todos os nomes de um namespace em bloco não é o mesmo que usar múltiplas declarações `using`. É mais como a aplicação em massa de um operador de resolução de escopo. Quando você usa uma declaração `using`, é como se o nome fosse declarado na localização da declaração `using`. Se um nome específico já estiver declarado em uma função, você não pode importar o mesmo nome com uma declaração `using`. Quando você usa uma diretiva `using`, entretanto, a resolução de nome ocorre como se você tivesse declarado os nomes na menor região declarativa contendo tanto a diretiva `using` quanto o próprio namespace. Para o seguinte exemplo, isso seria o namespace global. Se você usar uma diretiva `using` para importar um nome que já esteja declarado em uma função, a versão local do nome ocultará a versão do namespace, assim como ocultaria uma variável global de mesmo nome. No entanto, você ainda pode usar o operador de resolução de escopo, como no exemplo a seguir:

```cpp
namespace Jill {
    double bucket(double n) { ... }
    double fetch;
    struct Hill { ... };
}
char fetch;           // namespace global
int main()
{
    using namespace Jill;  // importa todos os nomes do namespace
    Hill Thrill;           // cria uma estrutura do tipo Jill::Hill
    double water = bucket(2);  // usa Jill::bucket()
    double fetch;          // não é erro; oculta Jill::fetch
    cin >> fetch;          // lê um valor no fetch local
    cin >> ::fetch;        // lê um valor no fetch global
    cin >> Jill::fetch;    // lê um valor em Jill::fetch
    // ...
}
int foom()
{
    Hill top;              // ERRO
    Jill::Hill crest;      // válido
}
```

Aqui, em `main()`, o nome `Jill::fetch` é colocado no namespace local. Ele não tem escopo local, portanto não substitui o `fetch` global. Mas o `fetch` declarado localmente oculta tanto `Jill::fetch` quanto o `fetch` global. No entanto, ambas as últimas duas variáveis `fetch` estão disponíveis se você usar o operador de resolução de escopo. Observe também que, embora uma diretiva `using` em uma função trate os nomes do namespace como se fossem declarados fora da função, ela não torna esses nomes disponíveis para outras funções do arquivo. Portanto, no exemplo anterior, a função `foom()` não pode usar o identificador não qualificado `Hill`.

> **Nota:** Suponha que um namespace e uma região declarativa definem o mesmo nome. Se você tentar usar uma declaração `using` para trazer o nome do namespace para a região declarativa, os dois nomes conflitarão, e você obterá um erro. Se você usar uma diretiva `using` para trazer o nome do namespace para a região declarativa, a versão local do nome ocultará a versão do namespace.

Em geral, a declaração `using` é mais segura de usar do que uma diretiva `using` porque mostra exatamente quais nomes você está tornando disponíveis. E se o nome conflitar com um nome local, o compilador avisa você. A diretiva `using` adiciona todos os nomes, mesmo aqueles de que você pode não precisar. Se um nome local conflitar, ele substitui a versão do namespace, e você não recebe aviso. Além disso, a natureza aberta dos namespaces significa que a lista completa de nomes em um namespace pode estar espalhada por vários locais, tornando difícil saber exatamente quais nomes você está adicionando.

Aqui está a abordagem usada para a maioria dos exemplos deste livro:

```cpp
#include <iostream>
int main()
{
    using namespace std;
    // ...
}
```

Primeiro, o arquivo de cabeçalho `iostream` coloca tudo no namespace `std`. Então, a diretiva `using` torna os nomes disponíveis dentro de `main()`. Alguns exemplos fazem isso em vez disso:

```cpp
#include <iostream>
using namespace std;
int main()
{
    // ...
}
```

Isso exporta tudo do namespace `std` para o namespace global. A principal razão para essa abordagem é a comodidade. No entanto, os proponentes de namespaces esperam que você seja mais seletivo e use o operador de resolução de escopo ou a declaração `using`. Ou seja, você não deveria usar o seguinte:

```cpp
using namespace std;  // evite — é indiscriminado
```

Em vez disso, você deveria usar isto:

```cpp
int x;
std::cin >> x;
std::cout << x << std::endl;
```

Ou poderia usar isto:

```cpp
using std::cin;
using std::cout;
using std::endl;
int x;
cin >> x;
cout << x << endl;
```

Você pode usar namespaces aninhados, como descrito na seção a seguir, para criar um namespace que contenha as declarações `using` que você usa comumente.

### Mais Recursos de Namespace (More Namespace Features)

Você pode aninhar declarações de namespace, assim:

```cpp
namespace elements
{
    namespace fire
    {
        int chama;
        // ...
    }
    float agua;
}
```

Nesse caso, você se refere à variável `chama` como `elements::fire::chama`. Da mesma forma, você pode tornar os nomes internos disponíveis com esta diretiva `using`:

```cpp
using namespace elements::fire;
```

Além disso, você pode usar diretivas `using` e declarações `using` dentro de namespaces, assim:

```cpp
namespace mito
{
    using Jill::fetch;
    using namespace elements;
    using std::cout;
    using std::cin;
}
```

Suponha que você queira acessar `Jill::fetch`. Como `Jill::fetch` agora faz parte do namespace `mito`, onde pode ser chamado de `fetch`, você pode acessá-lo assim:

```cpp
std::cin >> mito::fetch;
```

Claro, como também é parte do namespace `Jill`, você ainda pode chamá-lo de `Jill::fetch`:

```cpp
std::cout << Jill::fetch;  // exibe o valor lido em mito::fetch
```

Ou você pode fazer isso, desde que nenhuma variável local conflite:

```cpp
using namespace mito;
cin >> fetch;  // realmente std::cin e Jill::fetch
```

A diretiva `using` é *transitiva* (transitive). Dizemos que uma operação `op` é transitiva se A `op` B e B `op` C implica A `op` C. Por exemplo, o operador `>` é transitivo. Nesse contexto, o resultado é que a seguinte instrução coloca tanto o namespace `mito` quanto o namespace `elements` em escopo:

```cpp
using namespace mito;
```

Essa única diretiva tem o mesmo efeito que as seguintes duas diretivas:

```cpp
using namespace mito;
using namespace elements;
```

Você pode criar um *alias* (alias) para um namespace. Por exemplo, suponha que você tenha um namespace definido assim:

```cpp
namespace meus_itens_favoritos { ... };
```

Você pode fazer `mif` um alias para `meus_itens_favoritos` usando a seguinte instrução:

```cpp
namespace mif = meus_itens_favoritos;
```

Você pode usar essa técnica para simplificar o uso de namespaces aninhados:

```cpp
namespace MEF = mito::elements::fire;
using MEF::chama;
```

### Namespaces Não Nomeados (Unnamed Namespaces)

Você pode criar um *namespace não nomeado* (unnamed namespace) omitindo o nome do namespace:

```cpp
namespace  // namespace não nomeado
{
    int gelo;
    int bando;
}
```

Este código se comporta como se fosse seguido por uma diretiva `using`; ou seja, os nomes declarados neste namespace estão em escopo potencial até o final da região declarativa que contém o namespace não nomeado. Nesse aspecto, nomes em um namespace não nomeado são como variáveis globais. No entanto, se um namespace não tem nome, você não pode usar explicitamente uma diretiva `using` ou declaração `using` para tornar os nomes disponíveis em outro lugar. Em particular, você não pode usar nomes de um namespace não nomeado em um arquivo diferente do que contém a declaração do namespace. Isso fornece uma alternativa ao uso de variáveis estáticas com ligação interna. Suponha, por exemplo, que você tenha este código:

```cpp
static int contagens;    // armazenamento estático, ligação interna
int outra();
int main()
{
    // ...
}
int outra()
{
    // ...
}
```

A abordagem de namespace é fazer isso em vez disso:

```cpp
namespace
{
    int contagens;       // armazenamento estático, ligação interna
}
int outra();
int main()
{
    // ...
}
int outra()
{
    // ...
}
```

### Um Exemplo de Namespace (A Namespace Example)

Vejamos um exemplo de múltiplos arquivos que demonstra alguns dos recursos dos namespaces. O primeiro arquivo deste exemplo (veja a Listagem 9.11) é um arquivo de cabeçalho que contém alguns itens normalmente encontrados em arquivos de cabeçalho — constantes, definições de estrutura e protótipos de função. Nesse caso, os itens são colocados em dois namespaces. O primeiro namespace, `pers`, contém uma definição de uma estrutura `Pessoa`, mais protótipos para uma função que preenche uma estrutura com o nome de uma pessoa e uma função que exibe o conteúdo da estrutura. O segundo namespace, `debts`, define uma estrutura para armazenar o nome de uma pessoa e a quantia de dinheiro devida a essa pessoa. Esta estrutura usa a estrutura `Pessoa`, portanto o namespace `debts` tem uma diretiva `using` para tornar os nomes no namespace `pers` disponíveis no namespace `debts`. O namespace `debts` também contém alguns protótipos.

**Listagem 9.11** namesp.h

```cpp
// namesp.h
#include <string>
// cria os namespaces pers e debts
namespace pers
{
    struct Pessoa
    {
        std::string nome;
        std::string sobrenome;
    };
    void obterPessoa(Pessoa &);
    void exibirPessoa(const Pessoa &);
}
namespace debts
{
    using namespace pers;
    struct Divida
    {
        Pessoa nome;
        double valor;
    };
    void obterDivida(Divida &);
    void exibirDivida(const Divida &);
    double somarDividas(const Divida ar[], int n);
}
```

O segundo arquivo neste exemplo (veja a Listagem 9.12) segue o padrão usual de ter um arquivo de código-fonte fornecendo definições para funções com protótipo em um arquivo de cabeçalho. Os nomes de função, que são declarados em um namespace, têm escopo de namespace, portanto as definições precisam estar no mesmo namespace que as declarações. É aqui que a natureza aberta dos namespaces é útil. Os namespaces originais são trazidos incluindo `namesp.h` (veja a Listagem 9.11). O arquivo então adiciona as definições de função para os dois namespaces, como mostrado na Listagem 9.12. Além disso, o arquivo `namesp.cpp` ilustra trazendo elementos do namespace `std` com a declaração `using` e o operador de resolução de escopo.

**Listagem 9.12** namesp.cpp

```cpp
// namesp.cpp -- namespaces
#include <iostream>
#include "namesp.h"
namespace pers
{
    using std::cout;
    using std::cin;
    void obterPessoa(Pessoa & rp)
    {
        cout << "Digite o primeiro nome: ";
        cin >> rp.nome;
        cout << "Digite o sobrenome: ";
        cin >> rp.sobrenome;
    }
    void exibirPessoa(const Pessoa & rp)
    {
        std::cout << rp.sobrenome << ", " << rp.nome;
    }
}
namespace debts
{
    void obterDivida(Divida & rd)
    {
        obterPessoa(rd.nome);
        std::cout << "Digite a divida: ";
        std::cin >> rd.valor;
    }
    void exibirDivida(const Divida & rd)
    {
        exibirPessoa(rd.nome);
        std::cout << ": R$ " << rd.valor << std::endl;
    }
    double somarDividas(const Divida ar[], int n)
    {
        double total = 0;
        for (int i = 0; i < n; i++)
            total += ar[i].valor;
        return total;
    }
}
```

Finalmente, o terceiro arquivo deste programa (veja a Listagem 9.13) é um arquivo de código-fonte que usa as estruturas e funções declaradas e definidas nos namespaces. A Listagem 9.13 mostra vários métodos de tornar os identificadores de namespace disponíveis.

**Listagem 9.13** usenmsp.cpp

```cpp
// usenmsp.cpp -- usando namespaces
#include <iostream>
#include "namesp.h"
void outra(void);
void mais_uma(void);
int main(void)
{
    using debts::Divida;
    using debts::exibirDivida;
    Divida golfe = { {"Benny", "Goatsniff"}, 120.0 };
    exibirDivida(golfe);
    outra();
    mais_uma();
    return 0;
}
void outra(void)
{
    using std::cout;
    using std::endl;
    using namespace debts;
    Pessoa dp = {"Doodles", "Glister"};
    exibirPessoa(dp);
    cout << endl;
    Divida rapido[3];
    int i;
    for (i = 0; i < 3; i++)
        obterDivida(rapido[i]);
    for (i = 0; i < 3; i++)
        exibirDivida(rapido[i]);
    cout << "Divida total: R$ " << somarDividas(rapido, 3) << endl;
    return;
}
void mais_uma(void)
{
    using pers::Pessoa;
    Pessoa cobrador = { "Milo", "Rightshift" };
    pers::exibirPessoa(cobrador);
    std::cout << std::endl;
}
```

Em `usenmsp.cpp`, `main()` começa usando duas declarações `using`:

```cpp
using debts::Divida;         // torna a definição da estrutura Divida disponível
using debts::exibirDivida;   // torna a função exibirDivida disponível
```

Observe que as declarações `using` usam apenas o nome; por exemplo, o segundo exemplo aqui não descreve o tipo de retorno ou a assinatura de função de `exibirDivida`; ele apenas fornece o nome. (Assim, se uma função estivesse sobrecarregada, uma única declaração `using` importaria todas as versões.) Além disso, embora tanto `Divida` quanto `exibirDivida()` usem o tipo `Pessoa`, não é necessário importar nenhum dos nomes de `Pessoa` porque o namespace `debts` já tem uma diretiva `using` que inclui o namespace `pers`.

Em seguida, a função `outra()` adota a abordagem menos desejável de importar todo o namespace com uma diretiva `using`:

```cpp
using namespace debts;  // torna todos os nomes de debts e pers disponíveis para outra()
```

Como a diretiva `using` em `debts` importa o namespace `pers`, a função `outra()` pode usar o tipo `Pessoa` e a função `exibirPessoa()`.

Finalmente, a função `mais_uma()` usa uma declaração `using` e o operador de resolução de escopo para acessar nomes específicos:

```cpp
using pers::Pessoa;
pers::exibirPessoa(cobrador);
```

Aqui está uma execução de amostra do programa construído a partir das Listagens 9.11, 9.12 e 9.13:

```
Goatsniff, Benny: R$ 120
Glister, Doodles
Digite o primeiro nome: Arabella
Digite o sobrenome: Binx
Digite a divida: 100
Digite o primeiro nome: Cleve
Digite o sobrenome: Delaproux
Digite a divida: 120
Digite o primeiro nome: Eddie
Digite o sobrenome: Fiotox
Digite a divida: 200
Binx, Arabella: R$ 100
Delaproux, Cleve: R$ 120
Fiotox, Eddie: R$ 200
Divida total: R$ 420
Rightshift, Milo
```

### Namespaces e o Futuro (Namespaces and the Future)

À medida que os programadores se familiarizarem mais com namespaces, idiomas de programação comuns emergirão. Aqui estão algumas diretrizes atuais:

- Use variáveis em um namespace nomeado em vez de usar variáveis globais externas.
- Use variáveis em um namespace não nomeado em vez de usar variáveis globais estáticas.
- Se você desenvolver uma biblioteca de funções ou classes, coloque-as em um namespace. Na verdade, o C++ já pede que as funções da biblioteca padrão sejam colocadas em um namespace chamado `std`. Isso se estende às funções trazidas do C. Por exemplo, o arquivo de cabeçalho `math.c`, compatível com C, não usa namespaces, mas o arquivo de cabeçalho C++ `cmath` deve colocar as várias funções da biblioteca matemática no namespace `std`.
- Use a diretiva `using` apenas como um meio temporário de converter código antigo para uso de namespace.
- Não use diretivas `using` em arquivos de cabeçalho; por um lado, isso oculta quais nomes estão sendo disponibilizados. Além disso, a ordem dos arquivos de cabeçalho pode afetar o comportamento. Se você usar uma diretiva `using`, coloque-a após todas as diretivas de pré-processador `#include`.
- Prefira importar nomes usando o operador de resolução de escopo ou uma declaração `using`.
- Prefira usar escopo local em vez de escopo global para declarações `using`.

Lembre-se de que a principal motivação para o uso de namespaces é simplificar o gerenciamento de grandes projetos de programação. Para programas simples de um único arquivo, o uso de uma diretiva `using` não é grande pecado.

Como mencionado anteriormente, as mudanças nos nomes dos arquivos de cabeçalho refletem mudanças de namespace. Os arquivos de cabeçalho de estilo mais antigo, como `iostream.h`, não usam namespaces, mas o novo arquivo de cabeçalho `iostream` deve usar o namespace `std`.

---

## Resumo (Summary)

O C++ encoraja o uso de múltiplos arquivos no desenvolvimento de programas. Uma estratégia organizacional eficaz é usar um arquivo de cabeçalho para definir tipos de usuário e fornecer protótipos de função para funções que manipulam os tipos de usuário. Você deve usar um arquivo de código-fonte separado para as definições de função. Juntos, o arquivo de cabeçalho e o arquivo de código-fonte definem e implementam o tipo definido pelo usuário e como ele pode ser usado. Então, `main()` e outras funções que usam essas funções podem ir para um terceiro arquivo.

Os esquemas de armazenamento do C++ determinam por quanto tempo as variáveis permanecem na memória (duração de armazenamento) e que partes de um programa têm acesso a elas (escopo e ligação). Variáveis automáticas são variáveis definidas dentro de um bloco, como um corpo de função ou um bloco dentro do corpo. Elas existem e são conhecidas apenas enquanto o programa executa instruções no bloco que contém a definição. Variáveis automáticas podem ser declaradas usando o especificador de classe de armazenamento `register` ou sem nenhum especificador, caso em que a variável é automaticamente automática. O especificador `register` era uma dica ao compilador de que a variável é muito usada, mas esse uso está descontinuado no C++11.

Variáveis estáticas existem durante toda a duração de um programa. Uma variável definida fora de qualquer função é conhecida por todas as funções no arquivo após sua definição (escopo de arquivo) e é disponibilizada para outros arquivos no programa (ligação externa). Para que outro arquivo use essa variável, esse arquivo deve declará-la usando a palavra-chave `extern`. Uma variável que é compartilhada entre arquivos deve ter uma declaração de definição em um arquivo (não é necessário usar `extern`, mas pode ser usado se combinado com inicialização) e declarações de referência nos outros arquivos (`extern` é usado sem inicialização). Uma variável definida fora de qualquer função mas qualificada com a palavra-chave `static` tem escopo de arquivo, mas não é disponibilizada para outros arquivos (ligação interna). Uma variável definida dentro de um bloco mas qualificada com a palavra-chave `static` é local a esse bloco (escopo local, sem ligação), mas mantém seu valor durante toda a duração do programa.

Por padrão, funções C++ têm ligação externa, portanto podem ser compartilhadas entre arquivos. Mas funções qualificadas com a palavra-chave `static` têm ligação interna e são confinadas ao arquivo de definição.

A alocação e desalocação dinâmicas de memória, usando `new` e `delete`, usa o armazenamento livre, ou heap, para dados. A memória começa a ser usada quando `new` é chamado e é liberada quando `delete` é chamado. O programa usa ponteiros para acompanhar esses locais de memória.

Namespaces permitem que você defina regiões nomeadas nas quais pode declarar identificadores. A intenção é reduzir conflitos de nomes, particularmente em programas grandes que usam código de vários fornecedores. Você pode tornar identificadores em um namespace disponíveis usando o operador de resolução de escopo, usando uma declaração `using` ou usando uma diretiva `using`.

---

## Revisão do Capítulo (Chapter Review)

**1.** Que esquema de armazenamento você usaria para as seguintes situações?

a. `homer` é um argumento formal (parâmetro) de uma função.

b. A variável `secret` deve ser compartilhada por dois arquivos.

c. A variável `topsecret` deve ser compartilhada pelas funções de um arquivo, mas escondida de outros arquivos.

d. `beencalled` controla quantas vezes a função que a contém foi chamada.

**2.** Descreva as diferenças entre uma declaração `using` e uma diretiva `using`.

**3.** Reescreva o seguinte para que ele não use declarações `using` nem diretivas `using`:

```cpp
#include <iostream>
using namespace std;
int main()
{
    double x;
    cout << "Enter value: ";
    while (! (cin >> x) )
    {
        cout << "Bad input. Please enter a number: ";
        cin.clear();
        while (cin.get() != '\n')
            continue;
    }
    cout << "Value = " << x << endl;
    return 0;
}
```

**4.** Reescreva o seguinte para que ele use declarações `using` em vez da diretiva `using`:

```cpp
#include <iostream>
using namespace std;
int main()
{
    double x;
    cout << "Enter value: ";
    while (! (cin >> x) )
    {
        cout << "Bad input. Please enter a number: ";
        cin.clear();
        while (cin.get() != '\n')
            continue;
    }
    cout << "Value = " << x << endl;
    return 0;
}
```

**5.** Suponha que você queira que a função `average(3, 6)` retorne uma média `int` dos dois argumentos `int` quando chamada em um arquivo, e você queira que ela retorne uma média `double` dos dois argumentos `int` quando chamada em um segundo arquivo do mesmo programa. Como você poderia configurar isso?

**6.** O que o seguinte programa de dois arquivos irá exibir?

```cpp
// file1.cpp
#include <iostream>
using namespace std;
void other();
void another();
int x = 10;
int y;
int main()
{
    cout << x << endl;
    {
        int x = 4;
        cout << x << endl;
        cout << y << endl;
    }
    other();
    another();
    return 0;
}
void other()
{
    int y = 1;
    cout << "Other: " << x << ", " << y << endl;
}
// file2.cpp
#include <iostream>
using namespace std;
extern int x;
namespace
{
    int y = -4;
}
void another()
{
    cout << "another(): " << x << ", " << y << endl;
}
```

**7.** O que o seguinte programa irá exibir?

```cpp
#include <iostream>
using namespace std;
void other();
namespace n1
{
    int x = 1;
}
namespace n2
{
    int x = 2;
}
int main()
{
    using namespace n1;
    cout << x << endl;
    {
        int x = 4;
        cout << x << ", " << n1::x << ", " << n2::x << endl;
    }
    using n2::x;
    cout << x << endl;
    other();
    return 0;
}
void other()
{
    using namespace n2;
    cout << x << endl;
    {
        int x = 4;
        cout << x << ", " << n1::x << ", " << n2::x << endl;
    }
    using n2::x;
    cout << x << endl;
}
```

---

## Exercícios de Programação (Programming Exercises)

**1.** Aqui está um arquivo de cabeçalho:

```cpp
// golfe.h -- para pe9-1.cpp
const int Len = 40;
struct golfe
{
    char nome_completo[Len];
    int handicap;
};
// versão não interativa:
// a função define a estrutura golfe para o nome e handicap fornecidos
// usando valores passados como argumentos à função
void definir_golfe(golfe & g, const char * nome, int hc);

// versão interativa:
// a função solicita nome e handicap do usuário
// e define os membros de g com os valores inseridos
// retorna 1 se o nome for inserido, 0 se o nome for string vazia
int definir_golfe(golfe & g);

// função redefine o handicap para um novo valor
void handicap(golfe & g, int hc);
// função exibe o conteúdo da estrutura golfe
void exibir_golfe(const golfe & g);
```

Observe que `definir_golfe()` está sobrecarregada. Usar a primeira versão de `definir_golfe()` seria assim:

```cpp
golfe ann;
definir_golfe(ann, "Ann Birdfree", 24);
```

A chamada de função fornece as informações que são armazenadas na estrutura `ann`. Usar a segunda versão de `definir_golfe()` seria assim:

```cpp
golfe andy;
definir_golfe(andy);
```

A função solicitaria ao usuário que inserisse o nome e o handicap e os armazenaria na estrutura `andy`. Esta função poderia (mas não precisa) usar a primeira versão internamente.

Monte um programa de múltiplos arquivos baseado neste cabeçalho. Um arquivo, chamado `golfe.cpp`, deve fornecer definições de função adequadas para corresponder aos protótipos no arquivo de cabeçalho. Um segundo arquivo deve conter `main()` e demonstrar todas as funcionalidades das funções com protótipo. Por exemplo, um loop deve solicitar entrada para um array de estruturas de golfe e terminar quando o array estiver cheio ou o usuário inserir uma string vazia para o nome do golfista. A função `main()` deve usar apenas as funções com protótipo para acessar as estruturas de golfe.

**2.** Refaça a Listagem 9.9, substituindo o array de caracteres por um objeto `string`. O programa não precisa mais verificar se a string de entrada cabe, e pode comparar a string de entrada com `""` para verificar uma linha vazia.

**3.** Comece com a seguinte declaração de estrutura:

```cpp
struct chaff
{
    char dross[20];
    int slag;
};
```

Escreva um programa que use `placement new` para colocar um array de duas dessas estruturas em um buffer. Depois atribua valores aos membros da estrutura (lembrando de usar `strcpy()` para o array `char`) e use um loop para exibir o conteúdo. A Opção 1 é usar um array estático, como o da Listagem 9.10, para o buffer. A Opção 2 é usar `new` regular para alocar o buffer.

**4.** Escreva um programa de três arquivos baseado no seguinte namespace:

```cpp
namespace SALES
{
    const int QUARTERS = 4;
    struct Sales
    {
        double sales[QUARTERS];
        double average;
        double max;
        double min;
    };
    // copia os menores dentre 4 ou n itens do array ar
    // para o membro sales de s e calcula e armazena os
    // valores médio, máximo e mínimo dos itens inseridos;
    // elementos restantes de sales, se houver, definidos como 0
    void setSales(Sales & s, const double ar[], int n);
    // coleta as vendas de 4 trimestres interativamente, armazena-as
    // no membro sales de s e calcula e armazena os
    // valores médio, máximo e mínimo
    void setSales(Sales & s);
    // exibe todas as informações na estrutura s
    void showSales(const Sales & s);
}
```

O primeiro arquivo deve ser um arquivo de cabeçalho que contém o namespace. O segundo arquivo deve ser um arquivo de código-fonte que estende o namespace para fornecer definições para as três funções com protótipo. O terceiro arquivo deve declarar dois objetos `Sales`. Deve usar a versão interativa de `setSales()` para fornecer valores para uma estrutura e a versão não interativa de `setSales()` para fornecer valores para a segunda estrutura. Deve exibir o conteúdo de ambas as estruturas usando `showSales()`.

---

Navegação: [Anterior](capitulo-09-02-duracao-escopo-ligacao.md) | [Índice](README.md) | [Próximo](capitulo-10-01-objetos-classes.md)
