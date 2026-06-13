# Capítulo 9 — Modelos de Memória e Namespaces (Memory Models and Namespaces)

> Tradução não oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Navegação: [Anterior](capitulo-08-03-templates-resumo.md) | [Índice](README.md) | [Próximo](capitulo-09-02-duracao-escopo-ligacao.md)

Neste capítulo você aprenderá sobre os seguintes tópicos:

- Compilação separada de programas
- Duração de armazenamento, escopo e ligação
- O operador `placement new`
- Namespaces

O C++ oferece muitas opções para armazenar dados na memória. Você tem opções de quanto tempo os dados permanecem na memória (duração de armazenamento, storage duration) e opções de quais partes de um programa têm acesso aos dados (escopo e ligação, scope and linkage). Você pode alocar memória dinamicamente usando `new`, e o `placement new` oferece uma variação dessa técnica. O recurso de namespace do C++ fornece controle adicional sobre o acesso. Programas maiores geralmente consistem em vários arquivos de código-fonte que podem compartilhar alguns dados em comum. Esses programas envolvem a compilação separada dos arquivos do programa, e este capítulo começa com esse tópico.

## Compilação Separada (Separate Compilation)

O C++, assim como o C, permite e até encoraja que você localize as funções componentes de um programa em arquivos separados. Como o Capítulo 1, "Começando com C++", descreve, você pode compilar os arquivos separadamente e depois vinculá-los ao programa executável final. (Um compilador C++ geralmente compila programas e também gerencia o programa vinculador, linker.) Se você modificar apenas um arquivo, pode recompilar apenas aquele arquivo e depois vinculá-lo às versões previamente compiladas dos outros arquivos. Esse recurso facilita o gerenciamento de programas grandes. Além disso, a maioria dos ambientes C++ fornece recursos adicionais para ajudar no gerenciamento. Sistemas Unix e Linux, por exemplo, têm programas `make`, que acompanham quais arquivos um programa depende e quando foram modificados pela última vez. Se você executar `make`, e ele detectar que você alterou um ou mais arquivos de origem desde a última compilação, o `make` lembra os passos adequados necessários para reconstituir o programa. A maioria dos ambientes de desenvolvimento integrado (IDEs), incluindo Embarcadero C++ Builder, Microsoft Visual C++, Apple Xcode e Freescale CodeWarrior, fornece recursos semelhantes com seus menus de Projeto.

Vamos examinar um exemplo simples. Em vez de examinar detalhes de compilação, que dependem da implementação, vamos nos concentrar em aspectos mais gerais, como o design.

Suponha, por exemplo, que você decida dividir o programa da Listagem 7.12 colocando as duas funções de suporte em um arquivo separado. Lembre-se de que a Listagem 7.12 converte coordenadas retangulares em coordenadas polares e depois exibe o resultado. Você não pode simplesmente cortar o arquivo original em uma linha pontilhada após o final de `main()`. O problema é que `main()` e as outras duas funções usam as mesmas declarações de estrutura, portanto você precisa colocá-las em ambos os arquivos. Simplesmente digitá-las é um convite ao erro. Mesmo que você copie as declarações de estrutura corretamente, precisa lembrar de modificar ambos os conjuntos de declarações se fizer alterações posteriormente. Em suma, espalhar um programa por múltiplos arquivos cria novos problemas.

Quem quer mais problemas? Os desenvolvedores de C e C++ não queriam, então forneceram o recurso `#include` para lidar com essa situação. Em vez de colocar as declarações de estrutura em cada arquivo, você pode colocá-las em um arquivo de cabeçalho (header file) e depois incluir esse arquivo de cabeçalho em cada arquivo de código-fonte. Dessa forma, se você modificar a declaração de estrutura, pode fazê-lo apenas uma vez, no arquivo de cabeçalho. Além disso, você pode colocar os protótipos de função no arquivo de cabeçalho. Assim, você pode dividir o programa original em três partes:

- Um arquivo de cabeçalho que contém as declarações de estrutura e os protótipos para funções que usam essas estruturas
- Um arquivo de código-fonte que contém o código para as funções relacionadas à estrutura
- Um arquivo de código-fonte que contém o código que chama as funções relacionadas à estrutura

Esta é uma estratégia útil para organizar um programa. Se, por exemplo, você escrever outro programa que usa essas mesmas funções, pode simplesmente incluir o arquivo de cabeçalho e adicionar o arquivo de funções ao projeto ou lista de make. Além disso, essa organização é consistente com a abordagem OOP. Um arquivo, o arquivo de cabeçalho, contém a definição dos tipos definidos pelo usuário. Um segundo arquivo contém o código de função para manipular os tipos definidos pelo usuário. Juntos, eles formam um pacote que você pode usar para uma variedade de programas.

Você não deve colocar definições de função ou declarações de variáveis em um arquivo de cabeçalho. Isso pode funcionar para uma configuração simples, mas geralmente leva a problemas. Por exemplo, se você tivesse uma definição de função em um arquivo de cabeçalho e depois incluísse o arquivo de cabeçalho em dois outros arquivos que fazem parte de um único programa, você acabaria com duas definições da mesma função em um único programa, o que é um erro, a menos que a função seja inline. Aqui estão algumas coisas comumente encontradas em arquivos de cabeçalho:

- Protótipos de funções
- Constantes simbólicas definidas usando `#define` ou `const`
- Declarações de estruturas
- Declarações de classes
- Declarações de templates
- Funções inline

É correto colocar declarações de estrutura em um arquivo de cabeçalho porque elas não criam variáveis; elas apenas dizem ao compilador como criar uma variável de estrutura quando você declara uma em um arquivo de código-fonte. Da mesma forma, declarações de template não são código a ser compilado; são instruções ao compilador sobre como gerar definições de função para corresponder às chamadas de função encontradas no código-fonte. Dados declarados `const` e funções inline têm propriedades de ligação especiais (descritas em breve) que permitem que sejam colocados em arquivos de cabeçalho sem causar problemas.

As Listagens 9.1, 9.2 e 9.3 mostram o resultado da divisão da Listagem 7.12 em partes separadas. Observe que você usa `"coordin.h"` em vez de `<coordin.h>` ao incluir o arquivo de cabeçalho. Se o nome do arquivo estiver entre colchetes angulares, o compilador C++ procura na parte do sistema de arquivos do sistema host que contém os arquivos de cabeçalho padrão. Mas se o nome do arquivo estiver entre aspas duplas, o compilador primeiro procura no diretório de trabalho atual ou no diretório do código-fonte. Se não encontrar o arquivo de cabeçalho lá, então procura no local padrão. Portanto, você deve usar aspas, não colchetes angulares, ao incluir seus próprios arquivos de cabeçalho.

**Listagem 9.1** coordin.h

```cpp
// coordin.h -- templates de estrutura e protótipos de função
// templates de estrutura
#ifndef COORDIN_H_
#define COORDIN_H_
struct polar
{
    double distancia; // distância da origem
    double angulo;    // direção da origem
};
struct rect
{
    double x; // distância horizontal da origem
    double y; // distância vertical da origem
};
// protótipos
polar ret_para_polar(rect posxy);
void exibir_polar(polar posda);
#endif
```

**Listagem 9.2** file1.cpp

```cpp
// file1.cpp -- exemplo de programa de três arquivos
#include <iostream>
#include "coordin.h" // templates de estrutura, protótipos de função
using namespace std;
int main()
{
    rect rlugar;
    polar plugar;
    cout << "Digite os valores x e y: ";
    while (cin >> rlugar.x >> rlugar.y) // uso elegante de cin
    {
        plugar = ret_para_polar(rlugar);
        exibir_polar(plugar);
        cout << "Proximos dois numeros (q para sair): ";
    }
    cout << "Ate logo!\n";
    return 0;
}
```

**Listagem 9.3** file2.cpp

```cpp
// file2.cpp -- contém funções chamadas em file1.cpp
#include <iostream>
#include <cmath>
#include "coordin.h" // templates de estrutura, protótipos de função

// converte coordenadas retangulares para polares
polar ret_para_polar(rect posxy)
{
    using namespace std;
    polar resposta;
    resposta.distancia =
        sqrt(posxy.x * posxy.x + posxy.y * posxy.y);
    resposta.angulo = atan2(posxy.y, posxy.x);
    return resposta; // retorna uma estrutura polar
}
// exibe coordenadas polares, convertendo ângulo para graus
void exibir_polar(polar posda)
{
    using namespace std;
    const double Rad_para_grau = 57.29577951;
    cout << "distancia = " << posda.distancia;
    cout << ", angulo = " << posda.angulo * Rad_para_grau;
    cout << " graus\n";
}
```

Compilar e vincular esses dois arquivos de código-fonte junto com o novo arquivo de cabeçalho produz um programa executável. Aqui está uma execução de exemplo:

```
Digite os valores x e y: 120 80
distancia = 144.222, angulo = 33.6901 graus
Proximos dois numeros (q para sair): 120 50
distancia = 130, angulo = 22.6199 graus
Proximos dois numeros (q para sair): q
```

A propósito, embora tenhamos discutido compilação separada em termos de arquivos, o Padrão C++ usa o termo *unidade de tradução* (translation unit) em vez de arquivo, para preservar maior generalidade; a metáfora de arquivo não é a única forma possível de organizar informações para um computador. Por simplicidade, este livro usará o termo arquivo.

> **Gerenciamento de Arquivos de Cabeçalho**
>
> Você deve incluir um arquivo de cabeçalho apenas uma vez em um arquivo. Isso pode parecer algo fácil de lembrar, mas é possível incluir um arquivo de cabeçalho várias vezes sem saber que fez isso. Por exemplo, você pode usar um arquivo de cabeçalho que inclui outro arquivo de cabeçalho. Há uma técnica padrão em C/C++ para evitar múltiplas inclusões de arquivos de cabeçalho. É baseada na diretiva de pré-processador `#ifndef` (para "se não definido", if not defined). Um segmento de código como o seguinte significa "processe as instruções entre o `#ifndef` e o `#endif` apenas se o nome `COORDIN_H_` não tiver sido definido anteriormente pela diretiva de pré-processador `#define`":
>
> ```cpp
> #ifndef COORDIN_H_
> ...
> #endif
> ```
>
> Normalmente, você usa a instrução `#define` para criar constantes simbólicas. Mas simplesmente usar `#define` com um nome é suficiente para estabelecer que um nome está definido.
>
> A técnica que a Listagem 9.1 usa é envolver o conteúdo do arquivo em um `#ifndef`:
>
> ```cpp
> #ifndef COORDIN_H_
> #define COORDIN_H_
> // coloque o conteúdo do arquivo de inclusão aqui
> #endif
> ```
>
> Na primeira vez que o compilador encontra o arquivo, o nome `COORDIN_H_` deve estar indefinido. (Escolhi um nome baseado no nome do arquivo de inclusão, com alguns caracteres de sublinhado adicionados para criar um nome que provavelmente não será definido em outro lugar.) Sendo esse o caso, o compilador examina o material entre o `#ifndef` e o `#endif`. No processo de examinar o material, o compilador lê a linha que define `COORDIN_H_`. Se encontrar uma segunda inclusão de `coordin.h` no mesmo arquivo, o compilador nota que `COORDIN_H_` está definido e pula para a linha seguinte ao `#endif`. Observe que esse método não impede o compilador de incluir um arquivo duas vezes. Em vez disso, faz o compilador ignorar o conteúdo de todas as inclusões exceto a primeira. A maioria dos arquivos de cabeçalho padrão de C e C++ usa esse esquema de proteção.

> **Vinculação de Múltiplas Bibliotecas**
>
> O Padrão C++ permite que cada projetista de compilador implemente a decoração de nome (name decoration, ou name mangling, como discutido no Capítulo 8) da forma que achar adequada, portanto você deve estar ciente de que módulos binários (arquivos de código-objeto) criados com diferentes compiladores provavelmente não se vincularão corretamente. Ou seja, os dois compiladores gerarão diferentes nomes decorados para a mesma função. Essa diferença de nomes impedirá que o vinculador corresponda a chamada de função gerada por um compilador com a definição de função gerada por um segundo compilador. Ao tentar vincular módulos compilados, certifique-se de que cada arquivo objeto ou biblioteca foi gerado com o mesmo compilador. Se você tiver o código-fonte disponível, geralmente pode resolver erros de vinculação recompilando o código-fonte com seu compilador.

---

Navegação: [Anterior](capitulo-08-03-templates-resumo.md) | [Índice](README.md) | [Próximo](capitulo-09-02-duracao-escopo-ligacao.md)
