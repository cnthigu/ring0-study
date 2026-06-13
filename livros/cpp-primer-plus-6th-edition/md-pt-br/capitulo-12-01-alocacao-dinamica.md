# Capítulo 12 — Classes e Alocação Dinâmica de Memória (Parte 1: StringRuim, Funções Especiais e Classe String)

> Tradução não oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Navegação: [Anterior](capitulo-11-03-conversoes-resumo.md) | [Índice](README.md) | [Próximo](capitulo-12-02-retorno-ponteiros.md)

---

Neste capítulo você aprenderá sobre os seguintes tópicos:

- Utilização de alocação dinâmica de memória para membros de classes
- Construtores e operadores de cópia implícitos e explícitos
- Operadores de atribuição sobrecarregados implícitos e explícitos
- O que você deve fazer ao usar `new` em um construtor
- Utilização de membros de classe `static`
- Utilização de `placement new` com objetos
- Utilização de ponteiros para objetos
- Implementação de um tipo de dado abstrato (ADT) de fila

---

Este capítulo examina como usar `new` e `delete` com classes e como lidar com alguns dos problemas sutis que a memória dinâmica pode causar. Isso pode parecer uma lista curta de tópicos, mas esses tópicos afetam o design de construtores, destrutores e a sobrecarga de operadores.

Vamos olhar um exemplo específico de como C++ pode aumentar a carga sobre sua memória. Suponha que você queira criar uma classe com um membro que represente o sobrenome de alguém. A maneira mais simples e primitiva é usar um membro de array de caracteres para armazenar o nome. Porém, isso tem algumas desvantagens. Você pode usar um array de 14 caracteres e depois esbarrar em "Bartholomew Smeadsbury-Crafthovingham". Ou, para ter mais segurança, pode usar um array de 40 caracteres. Mas se depois criar um array de 2.000 desses objetos, você desperdiçará muita memória com arrays de caracteres parcialmente preenchidos. Existe uma alternativa.

Muitas vezes é muito melhor decidir vários assuntos — como quanto armazenamento usar — em tempo de execução, e não em tempo de compilação. A abordagem usual em C++ para armazenar um nome em um objeto é usar o operador `new` em um construtor de classe para alocar a quantidade correta de memória enquanto o programa está em execução. A maneira usual de fazer isso é usar a classe `string`, que cuida dos detalhes do gerenciamento de memória para você. Mas assim você não aprenderá muito sobre gerenciamento de memória, então vamos atacar o problema diretamente. Introduzir `new` em um construtor de classe levanta vários novos problemas, a menos que você lembre de tomar uma série de passos adicionais, como expandir o destrutor da classe, harmonizar todos os construtores com o novo destrutor e escrever métodos de classe adicionais para facilitar a inicialização e atribuição corretas. (Este capítulo explica todos esses passos, é claro.)

## Memória Dinâmica e Classes

O que você gostaria de comer no café da manhã, almoço e jantar pelo próximo mês? Quantas onças de leite no jantar do 3º dia? Quantas passas no cereal do café da manhã do 15º dia? Se você é como a maioria das pessoas, preferiria adiar algumas dessas decisões para as refeições em si. Parte da estratégia em C++ é adotar a mesma postura em relação à alocação de memória, deixando que o programa decida sobre a memória em tempo de execução em vez de em tempo de compilação. Dessa forma, o uso de memória pode depender das necessidades de um programa em vez de um conjunto rígido de regras de classe de armazenamento. Lembre-se de que, para obter controle dinâmico da memória, C++ utiliza os operadores `new` e `delete`. Infelizmente, usar esses operadores com classes pode apresentar novos problemas de programação. Como você verá, destrutores podem se tornar necessários em vez de meramente ornamentais. E às vezes você deve sobrecarregar um operador de atribuição para que um programa se comporte corretamente. Vamos examinar essas questões agora.

## Um Exemplo de Revisão e Membros de Classe Estáticos

Não usamos `new` e `delete` por um tempo, então vamos revisá-los com um programa curto. Ao mesmo tempo, vamos examinar uma nova classe de armazenamento: o membro de classe `static`. O veículo será uma classe `StringRuim`, que mais tarde será substituída pela classe `String`, ligeiramente mais capaz. (Você já viu a classe `string` padrão do C++ e aprenderá mais sobre ela no Capítulo 16, "A Classe `string` e a Biblioteca de Templates Padrão". Enquanto isso, as humildes classes `StringRuim` e `String` deste capítulo fornecem uma visão interna do que está por baixo de tal classe. Muitas técnicas de programação entram em jogo para fornecer uma interface tão amigável.)

Os objetos das classes `StringRuim` e `String` manterão um ponteiro para uma string e um valor representando o comprimento da string. Usaremos as classes `StringRuim` e `String` principalmente para dar uma visão interna de como `new`, `delete` e membros de classe `static` operam. Por essa razão, os construtores e destrutores exibirão mensagens quando chamados, para que você possa acompanhar a ação. Também omitiremos várias funções membro e amigas úteis, como os operadores sobrecarregados `++` e `>>` e uma função de conversão, a fim de simplificar a interface da classe. (Mas alegre-se! As perguntas de revisão deste capítulo dão a você a oportunidade de adicionar essas funções de suporte úteis.) A Listagem 12.1 mostra a declaração da classe.

### Listagem 12.1 — strngbad.h

```cpp
// strngbad.h -- definição de classe string defeituosa
#include <iostream>
#ifndef STRNGBAD_H_
#define STRNGBAD_H_
class StringRuim
{
private:
    char * cad;          // ponteiro para a string
    int tam;             // comprimento da string
    static int num_cadeias; // número de objetos
public:
    StringRuim(const char * s); // construtor
    StringRuim();               // construtor padrão
    ~StringRuim();              // destrutor
// função amiga
    friend std::ostream & operator<<(std::ostream & os,
                   const StringRuim & st);
};
#endif
```

Por que chamar a classe de `StringRuim`? Para lembrá-lo de que `StringRuim` é um exemplo em desenvolvimento. É o primeiro estágio do desenvolvimento de uma classe que usa alocação dinâmica de memória, e ela faz as coisas óbvias corretamente; por exemplo, usa `new` e `delete` corretamente nos construtores e no destrutor. Na verdade ela não faz coisas ruins, mas o design omite algumas coisas boas adicionais que são necessárias mas não são nada óbvias. Ver os problemas que a classe tem deve ajudá-lo a entender e lembrar as mudanças não óbvias que você fará depois, ao convertê-la para a classe `String` mais funcional.

Você deve observar dois pontos sobre essa declaração. Primeiro, ela usa um ponteiro para `char` em vez de um array de `char` para representar um nome. Isso significa que a declaração da classe não aloca espaço de armazenamento para a string em si. Em vez disso, ela usa `new` nos construtores para alocar espaço para a string. Esse arranjo evita limitar a declaração da classe com um tamanho de string predefinido.

Segundo, a definição declara o membro `num_cadeias` como pertencente à classe de armazenamento `static`. Um **membro de classe estático** tem uma propriedade especial: o programa cria apenas uma cópia de uma variável de classe estática, independentemente do número de objetos criados. Ou seja, um membro `static` é compartilhado entre todos os objetos daquela classe, assim como um número de telefone pode ser compartilhado por todos os membros de uma família. Se você criar 10 objetos `StringRuim`, haverá 10 membros `cad` e 10 membros `tam`, mas apenas 1 membro `num_cadeias` compartilhado (ver Figura 12.1). Isso é conveniente para dados que devem ser privados à classe mas que devem ter o mesmo valor para todos os objetos. O membro `num_cadeias`, por exemplo, destina-se a acompanhar o número de objetos criados.

### Listagem 12.2 — strngbad.cpp

```cpp
// strngbad.cpp -- métodos da classe StringRuim
#include <cstring>   // string.h em alguns compiladores
#include "strngbad.h"
using std::cout;
// inicializando membro de classe estático
int StringRuim::num_cadeias = 0;
// métodos da classe

// constrói StringRuim a partir de uma string C
StringRuim::StringRuim(const char * s)
{
    tam = std::strlen(s);       // define o tamanho
    cad = new char[tam + 1];    // aloca armazenamento
    std::strcpy(cad, s);        // inicializa o ponteiro
    num_cadeias++;              // atualiza contagem de objetos
    cout << num_cadeias << ": \"" << cad
       << "\" objeto criado\n"; // Para sua informação
}
StringRuim::StringRuim()       // construtor padrão
{
    tam = 4;
    cad = new char[4];
    std::strcpy(cad, "C++"); // string padrão
    num_cadeias++;
    cout << num_cadeias << ": \"" << cad
       << "\" objeto padrão criado\n"; // PYI
}
StringRuim::~StringRuim()     // destrutor necessário
{
    cout << "\"" << cad << "\" objeto deletado, "; // PYI
    --num_cadeias;              // obrigatório
    cout << num_cadeias << " restando\n"; // PYI
    delete [] cad;              // obrigatório
}
std::ostream & operator<<(std::ostream & os, const StringRuim & st)
{
    os << st.cad;
    return os;
}
```

Primeiro, observe a seguinte instrução da Listagem 12.2:

```cpp
int StringRuim::num_cadeias = 0;
```

Essa instrução inicializa o membro estático `num_cadeias` com 0. Observe que você não pode inicializar um membro de variável estático dentro da declaração de classe. Isso ocorre porque a declaração é uma descrição de como a memória deve ser alocada, mas ela não aloca memória. Você aloca e inicializa a memória criando um objeto usando aquele formato. No caso de um membro de classe estático, você inicializa o membro estático independentemente, com uma instrução separada fora da declaração de classe. Isso ocorre porque o membro de classe estático é armazenado separadamente, e não como parte de um objeto. Observe que a instrução de inicialização fornece o tipo e usa o operador de escopo, mas não usa a palavra-chave `static`.

Essa inicialização vai no arquivo de métodos, não no arquivo de declaração de classe. Isso ocorre porque a declaração de classe está em um arquivo de cabeçalho, e um programa pode incluir um arquivo de cabeçalho em vários outros arquivos. Isso resultaria em múltiplas cópias da instrução de inicialização, o que é um erro.

A exceção para a não inicialização de um membro de dado estático dentro da declaração de classe é se o membro de dado estático for uma constante do tipo integral ou enumeração.

> **Nota**
> Um membro de dado estático é declarado na declaração de classe e é inicializado no arquivo que contém os métodos da classe. O operador de escopo é usado na inicialização para indicar a qual classe o membro estático pertence. Porém, se o membro estático for um tipo integral constante ou um tipo enumeração, ele poderá ser inicializado na própria declaração de classe.

A seguir, observe que cada construtor contém a expressão `num_cadeias++`. Isso garante que cada vez que um programa cria um novo objeto, a variável compartilhada `num_cadeias` aumenta em um, mantendo registro do número total de objetos `StringRuim`. Além disso, o destrutor contém a expressão `--num_cadeias`. Assim, a classe `StringRuim` também mantém o controle dos objetos deletados, mantendo o valor do membro `num_cadeias` atualizado.

Agora observe o primeiro construtor na Listagem 12.2, que inicializa um objeto `StringRuim` com uma string C comum:

```cpp
StringRuim::StringRuim(const char * s)
{
    tam = std::strlen(s);    // define o tamanho
    cad = new char[tam + 1]; // aloca armazenamento
    std::strcpy(cad, s);     // inicializa o ponteiro
    num_cadeias++;           // atualiza contagem de objetos
    cout << num_cadeias << ": \"" << cad
       << "\" objeto criado\n"; // Para sua informação
}
```

Lembre-se de que o membro `cad` da classe é apenas um ponteiro, portanto o construtor deve fornecer a memória para armazenar uma string. Você pode passar um ponteiro de string ao construtor ao inicializar um objeto:

```cpp
StringRuim boston("Boston");
```

O construtor deve então alocar memória suficiente para armazenar a string e copiá-la para aquele local. Vamos percorrer o processo passo a passo.

Primeiro, a função inicializa o membro `tam`, usando a função `strlen()` para calcular o comprimento da string. A seguir, usa `new` para alocar espaço suficiente para armazenar a string e atribui o endereço da nova memória ao membro `cad`. (Lembre-se que `strlen()` retorna o comprimento de uma string sem contar o caractere nulo de terminação, então o construtor adiciona 1 a `tam` para ter espaço para a string incluindo o caractere nulo.) Em seguida, o construtor usa `strcpy()` para copiar a string passada para a nova memória. Depois atualiza o contador de objetos.

Para entender essa abordagem, você deve perceber que a string não é armazenada no objeto. A string é armazenada separadamente, na memória heap, e o objeto apenas armazena informações que indicam onde encontrá-la.

Observe que você não usa isto:

```cpp
cad = s; // não é o caminho correto
```

Isso apenas armazena o endereço sem fazer uma cópia da string.

O construtor padrão se comporta de maneira similar, exceto que fornece uma string padrão `"C++"`.

O destrutor contém a adição mais importante do exemplo ao tratamento de classes:

```cpp
StringRuim::~StringRuim()   // destrutor necessário
{
    cout << "\"" << cad << "\" objeto deletado, "; // PYI
    --num_cadeias;           // obrigatório
    cout << num_cadeias << " restando\n"; // PYI
    delete [] cad;           // obrigatório
}
```

O destrutor começa anunciando quando é chamado. Essa parte é informativa, mas não essencial. Porém, a instrução `delete` é vital. Lembre-se de que o membro `cad` da classe aponta para memória alocada com `new`. Quando um objeto `StringRuim` expira, o ponteiro `cad` expira. Mas a memória para a qual `cad` apontava permanece alocada, a menos que você use `delete` para liberá-la. Deletar um objeto libera a memória ocupada pelo próprio objeto, mas não libera automaticamente a memória apontada por ponteiros que eram membros do objeto. Para isso, você deve usar o destrutor. Ao colocar a instrução `delete` no destrutor, você garante que a memória alocada com `new` por um construtor seja liberada quando o objeto expirar.

> **Aviso**
> Sempre que você usar `new` em um construtor para alocar memória, você deve usar `delete` no destrutor correspondente para liberar essa memória. Se você usar `new[]` (com colchetes), você deve usar `delete[]` (com colchetes).

A Listagem 12.3, extraída de um programa em desenvolvimento no The Daily Vegetable, ilustra quando e como os construtores e destrutores de `StringRuim` funcionam. O programa coloca as declarações de objetos dentro de um bloco interno porque o destrutor é chamado quando a execução sai do bloco no qual um objeto é definido. Sem o bloco interno, os destrutores seriam chamados após a execução sair de `main()`, o que impediria você, em alguns ambientes, de ver as mensagens do destrutor antes da janela de execução fechar. Certifique-se de compilar a Listagem 12.2 junto com a Listagem 12.3.

### Listagem 12.3 — vegnews.cpp

```cpp
// vegnews.cpp -- usando new e delete com classes
// compilar com strngbad.cpp
#include <iostream>
using std::cout;
#include "strngbad.h"
void chame1(StringRuim &); // passagem por referência
void chame2(StringRuim);   // passagem por valor
int main()
{
    using std::endl;
    {
        cout << "Iniciando um bloco interno.\n";
        StringRuim manchete1("Celery Stalks at Midnight");
        StringRuim manchete2("Lettuce Prey");
        StringRuim esportes("Spinach Leaves Bowl for Dollars");
        cout << "manchete1: " << manchete1 << endl;
        cout << "manchete2: " << manchete2 << endl;
        cout << "esportes: " << esportes << endl;
        chame1(manchete1);
        cout << "manchete1: " << manchete1 << endl;
        chame2(manchete2);
        cout << "manchete2: " << manchete2 << endl;
        cout << "Inicializando um objeto a partir de outro:\n";
        StringRuim marinheiro = esportes;
        cout << "marinheiro: " << marinheiro << endl;
        cout << "Atribuindo um objeto a outro:\n";
        StringRuim no;
        no = manchete1;
        cout << "no: " << no << endl;
        cout << "Saindo do bloco.\n";
    }
    cout << "Fim de main()\n";
    return 0;
}
void chame1(StringRuim & rsb)
{
    cout << "String passada por referência:\n";
    cout << " \"" << rsb << "\"\n";
}
void chame2(StringRuim sb)
{
    cout << "String passada por valor:\n";
    cout << " \"" << sb << "\"\n";
}
```

> **Nota**
> Este primeiro rascunho do design para `StringRuim` tem algumas falhas deliberadas que tornam a saída exata indefinida. Alguns compiladores usados, por exemplo, produziram versões que abortaram antes de completar. Porém, embora os detalhes da saída possam diferir, os problemas básicos e as soluções (em breve a serem reveladas!) são os mesmos.

Aqui está a saída produzida após compilar o programa da Listagem 12.3 com o compilador de linha de comando Borland C++ 5.5:

```
Iniciando um bloco interno.
1: "Celery Stalks at Midnight" objeto criado
2: "Lettuce Prey" objeto criado
3: "Spinach Leaves Bowl for Dollars" objeto criado
manchete1: Celery Stalks at Midnight
manchete2: Lettuce Prey
esportes: Spinach Leaves Bowl for Dollars
String passada por referência:
   "Celery Stalks at Midnight"
manchete1: Celery Stalks at Midnight
String passada por valor:
   "Lettuce Prey"
"Lettuce Prey" objeto deletado, 2 restando
manchete2: Dû°
Inicializando um objeto a partir de outro:
marinheiro: Spinach Leaves Bowl for Dollars
Atribuindo um objeto a outro:
3: "C++" objeto padrão criado
no: Celery Stalks at Midnight
Saindo do bloco.
"Celery Stalks at Midnight" objeto deletado, 2 restando
"Spinach Leaves Bowl for Dollars" objeto deletado, 1 restando
"Spinach Leaves Bowl for Doll8" objeto deletado, 0 restando
"@g" objeto deletado, -1 restando
"-|" objeto deletado, -2 restando
Fim de main()
```

Os vários caracteres não padrão que aparecem na saída variarão de sistema para sistema; eles são um dos sinais de que `StringRuim` merece ser chamada de ruim. Outro sinal é a contagem negativa de objetos. Combinações mais novas de compilador/sistema operacional tipicamente abortam o programa logo antes de exibir a linha sobre ter -1 objetos restando, e algumas delas relatam uma Falha de Proteção Geral (GPF — *General Protection Fault*). Uma GPF indica que um programa tentou acessar um local de memória proibido para ele; isso é outro sinal ruim.

### Notas do Programa

O programa da Listagem 12.3 começa bem, mas tropeça chegando a uma conclusão estranha e em última análise desastrosa. Vamos começar pelos pontos bons. O construtor anuncia que criou três objetos `StringRuim`, os numera, e o programa usa o operador `<<` sobrecarregado para listá-los:

```
1: "Celery Stalks at Midnight" objeto criado
2: "Lettuce Prey" objeto criado
3: "Spinach Leaves Bowl for Dollars" objeto criado
manchete1: Celery Stalks at Midnight
manchete2: Lettuce Prey
esportes: Spinach Leaves Bowl for Dollars
```

Então o programa passa `manchete1` para a função `chame1()` e exibe `manchete1` novamente após a chamada. Aqui está o código:

```cpp
chame1(manchete1);
cout << "manchete1: " << manchete1 << endl;
```

E aqui está o resultado:

```
String passada por referência:
  "Celery Stalks at Midnight"
manchete1: Celery Stalks at Midnight
```

Essa seção do código parece funcionar bem também.

Mas então o programa executa o seguinte código:

```cpp
chame2(manchete2);
cout << "manchete2: " << manchete2 << endl;
```

Aqui, `chame2()` passa `manchete2` por valor em vez de por referência, e o resultado indica um problema grave:

```
String passada por valor:
  "Lettuce Prey"
"Lettuce Prey" objeto deletado, 2 restando
manchete2: Dû°
```

Primeiro, passar `manchete2` como argumento de função de alguma forma faz com que o destrutor seja chamado. Segundo, embora passar por valor deva proteger o argumento original de alterações, a função bagunça a string original além do reconhecimento, e alguns caracteres não padrão são exibidos. (O texto exato exibido dependerá do que acontecer de estar na memória.)

Ainda pior, olhe para o final da saída quando o destrutor é chamado automaticamente para cada um dos objetos criados anteriormente:

```
Saindo do bloco.
"Celery Stalks at Midnight" objeto deletado, 2 restando
"Spinach Leaves Bowl for Dollars" objeto deletado, 1 restando
"Spinach Leaves Bowl for Doll8" objeto deletado, 0 restando
"@g" objeto deletado, -1 restando
"-|" objeto deletado, -2 restando
Fim de main()
```

Como os objetos de armazenamento automático são deletados na ordem inversa em que foram criados, os três primeiros objetos deletados são `no`, `marinheiro` e `esportes`. As deleções de `no` e `marinheiro` parecem corretas, mas para `esportes`, "Dollars" tornou-se "Doll8". A única coisa que o programa faz com `esportes` é usá-lo para inicializar `marinheiro`, mas esse ato parece ter alterado `esportes`. E os dois últimos objetos deletados, `manchete2` e `manchete1`, são irreconhecíveis. Algo bagunça essas strings antes de serem deletadas. Além disso, a contagem é bizarra. Como pode haver -2 objetos restando?

Na verdade, a contagem peculiar é uma pista. Cada objeto é construído uma vez e destruído uma vez, portanto o número de chamadas ao construtor deve ser igual ao número de chamadas ao destrutor. Como a contagem de objetos (`num_cadeias`) é decrementada duas vezes mais do que é incrementada, um construtor que não incrementa `num_cadeias` deve estar criando dois objetos. A definição de classe declara e define dois construtores (ambos incrementam `num_cadeias`), mas acaba que o programa usa três construtores! Por exemplo, considere esta linha:

```cpp
StringRuim marinheiro = esportes;
```

Qual construtor é usado aqui? Não o construtor padrão, e não o construtor com um parâmetro `const char *`. Lembre-se, a inicialização usando essa forma é outra sintaxe para o seguinte:

```cpp
StringRuim marinheiro = StringRuim(esportes); // construtor usando esportes
```

Como `esportes` é do tipo `StringRuim`, um construtor correspondente poderia ter este protótipo:

```cpp
StringRuim(const StringRuim &);
```

E acontece que o compilador gera automaticamente esse construtor (chamado **construtor de cópia** porque faz uma cópia de um objeto) se você inicializar um objeto a partir de outro. A versão automática não saberia atualizar a variável estática `num_cadeias`, portanto bagunçaria o esquema de contagem. De fato, todos os problemas exibidos por esse exemplo derivam de funções membro que o compilador gera automaticamente, então vamos examinar esse tópico agora.

## Funções Membro Especiais

Os problemas com a classe `StringRuim` derivam de **funções membro especiais**. São funções membro definidas automaticamente. No caso de `StringRuim`, o comportamento dessas funções membro é inadequado para este design de classe particular. Especificamente, C++ fornece automaticamente as seguintes funções membro:

- Um construtor padrão, se você não definir nenhum construtor
- Um destrutor padrão, se você não definir um
- Um construtor de cópia, se você não definir um
- Um operador de atribuição, se você não definir um
- Um operador de endereço, se você não definir um

Mais precisamente, o compilador gera definições para os três últimos itens se um programa usa objetos de forma a exigi-los. Por exemplo, se você atribuir um objeto a outro, o programa fornece uma definição para o operador de atribuição.

Acontece que o construtor de cópia implícito e o operador de atribuição implícito causam problemas à classe `StringRuim`.

O operador de endereço implícito retorna o endereço do objeto invocante (ou seja, o valor do ponteiro `this`). Isso é adequado para nossos propósitos e não discutiremos essa função membro mais. O destrutor padrão não faz nada, e também não o discutiremos, a não ser para apontar que a classe já forneceu um substituto para ele. Mas os outros merecem uma discussão mais aprofundada.

C++11 fornece duas funções membro especiais adicionais — o **construtor de movimentação** e o **operador de atribuição de movimentação**. O Capítulo 18, "Visitando o Novo Padrão C++", discute esses tópicos mais adiante.

### Construtores Padrão

Se você não fornecer nenhum construtor, C++ fornece um construtor padrão. Por exemplo, suponha que você define uma classe `Klunk` e omite qualquer construtor. Nesse caso, o compilador fornece o seguinte padrão:

```cpp
Klunk::Klunk() { } // construtor padrão implícito
```

Ou seja, ele fornece um construtor (o *construtor padrão* padrão) que não recebe argumentos e não faz nada. Ele é necessário porque criar um objeto sempre invoca um construtor:

```cpp
Klunk lunk; // invoca o construtor padrão
```

O construtor padrão torna `lunk` semelhante a uma variável automática comum; ou seja, seu valor na inicialização é desconhecido.

Depois que você define qualquer construtor, C++ não se preocupa em definir um construtor padrão. Se você quiser criar objetos que não são inicializados explicitamente, terá que definir um construtor padrão explicitamente. É um construtor sem argumentos, mas você pode usá-lo para definir valores particulares:

```cpp
Klunk::Klunk() // construtor padrão explícito
{
    klunk_ct = 0;
    ...
}
```

Um construtor com argumentos ainda pode ser um construtor padrão se todos os seus argumentos tiverem valores padrão. Por exemplo, a classe `Klunk` poderia ter o seguinte construtor inline:

```cpp
Klunk(int n = 0) { klunk_ct = n; }
```

Porém, você só pode ter um construtor padrão. Ou seja, você não pode fazer isto:

```cpp
Klunk() { klunk_ct = 0 }          // construtor #1
Klunk(int n = 0) { klunk_ct = n; } // construtor #2 ambíguo
```

Por que isso é ambíguo? Considere as duas declarações seguintes:

```cpp
Klunk kar(10); // claramente corresponde ao Klunk(int n)
Klunk bus;     // poderia corresponder a qualquer construtor
```

A segunda declaração corresponde ao construtor #1 (sem argumento), mas também corresponde ao construtor #2 (usando o argumento padrão 0). Isso fará com que o compilador emita uma mensagem de erro.

### Construtores de Cópia

Um **construtor de cópia** é usado para copiar um objeto para um objeto recém-criado. Ou seja, ele é usado durante a inicialização, inclusive ao passar argumentos de função por valor, e não durante a atribuição ordinária. Um construtor de cópia para uma classe normalmente tem este protótipo:

```cpp
NomeClasse(const NomeClasse &);
```

Observe que ele recebe uma referência constante para um objeto de classe como argumento. Por exemplo, um construtor de cópia para a classe `StringRuim` teria este protótipo:

```cpp
StringRuim(const StringRuim &);
```

Você deve saber duas coisas sobre um construtor de cópia: quando ele é usado e o que ele faz.

**Quando um Construtor de Cópia É Usado**

Um construtor de cópia é invocado sempre que um novo objeto é criado e inicializado a partir de um objeto existente do mesmo tipo. Isso acontece em várias situações. A situação mais óbvia é quando você inicializa explicitamente um novo objeto a partir de um objeto existente. Por exemplo, dado que `lema` é um objeto `StringRuim`, as quatro declarações de definição a seguir invocam um construtor de cópia:

```cpp
StringRuim ditto(lema);    // chama StringRuim(const StringRuim &)
StringRuim eu_tambem = lema; // chama StringRuim(const StringRuim &)
StringRuim tambem = StringRuim(lema);
                           // chama StringRuim(const StringRuim &)
StringRuim * pStringRuim = new StringRuim(lema);
                           // chama StringRuim(const StringRuim &)
```

Dependendo da implementação, as duas declarações do meio podem usar um construtor de cópia diretamente para criar `eu_tambem` e `tambem`, ou podem usar um construtor de cópia para gerar objetos temporários cujos conteúdos são então atribuídos a `eu_tambem` e `tambem`. O último exemplo inicializa um objeto anônimo a partir de `lema` e atribui o endereço do novo objeto ao ponteiro `pStringRuim`.

Menos obviamente, o compilador usa um construtor de cópia sempre que um programa gera cópias de um objeto. Em particular, ele é usado quando uma função passa um objeto por valor (como `chame2()` faz na Listagem 12.3) ou quando uma função retorna um objeto. Lembre-se, passar por valor significa criar uma cópia da variável original. O compilador também usa um construtor de cópia sempre que gera objetos temporários. Por exemplo, um compilador pode gerar um objeto `Vetor` temporário para armazenar um resultado intermediário ao somar três objetos `Vetor`. Os compiladores variam quanto a quando geram objetos temporários, mas todos invocam um construtor de cópia ao passar objetos por valor e ao retorná-los. Em particular, esta chamada de função na Listagem 12.3 invoca um construtor de cópia:

```cpp
chame2(manchete2);
```

O programa usa um construtor de cópia para inicializar `sb`, o parâmetro formal do tipo `StringRuim` para a função `chame2()`.

A propósito, o fato de que passar um objeto por valor envolve invocar um construtor de cópia é uma boa razão para passar por referência. Isso economiza o tempo de invocar o construtor e o espaço para armazenar o novo objeto.

**O que um Construtor de Cópia Padrão Faz**

O construtor de cópia padrão realiza uma cópia membro a membro dos membros não estáticos (cópia membro a membro, às vezes chamada de **cópia superficial** — *shallow copy*). Cada membro é copiado por valor. Na Listagem 12.3, a instrução

```cpp
StringRuim marinheiro = esportes;
```

equivale ao seguinte (à parte do fato de que não compila porque o acesso a membros privados não é permitido):

```cpp
StringRuim marinheiro;
marinheiro.cad = esportes.cad;
marinheiro.tam = esportes.tam;
```

Se um membro é em si mesmo um objeto de classe, o construtor de cópia para aquela classe é usado para copiar um objeto membro para outro. Membros estáticos, como `num_cadeias`, não são afetados porque pertencem à classe como um todo em vez de a objetos individuais.

### De Volta a StringBad: Onde o Construtor de Cópia Erra

Agora você está em posição de entender a dupla estranheza da Listagem 12.3. O primeiro estranhamento é que a saída do programa indica dois objetos a mais destruídos do que construídos. A explicação é que o programa de fato cria dois objetos adicionais usando o construtor de cópia padrão. O construtor de cópia é usado para inicializar o parâmetro formal de `chame2()` quando aquela função é chamada, e é usado para inicializar o objeto `marinheiro` a partir de `esportes`. O construtor de cópia padrão não anuncia suas atividades, portanto não anuncia suas criações, e não incrementa o contador `num_cadeias`. Porém, o destrutor atualiza a contagem, e ele é invocado com o falecimento de todos os objetos, independentemente de como foram construídos. Esse estranhamento é um problema porque significa que o programa não mantém uma contagem precisa de objetos. A solução é fornecer um construtor de cópia explícito que atualize a contagem:

```cpp
StringRuim::StringRuim(const StringRuim & s)
{
    num_cadeias++;
    ...// coisas importantes virão aqui
}
```

> **Dica**
> Se sua classe tem um membro de dado estático cujo valor muda quando novos objetos são criados, você deve fornecer um construtor de cópia explícito que cuide da contabilidade.

O segundo estranhamento é mais sutil e perigoso. Um sintoma é o conteúdo da string embaralhado:

```
manchete2: Dû°
```

A causa é que o construtor de cópia implícito copia por valor. Considere a Listagem 12.3, por exemplo. O efeito, lembre-se, é este:

```cpp
marinheiro.cad = esportes.cad;
```

Isso não copia a string; copia o ponteiro para uma string. Ou seja, depois que `marinheiro` é inicializado a partir de `esportes`, você termina com dois ponteiros para a mesma string. Isso não é um problema quando a função `operator<<()` usa o ponteiro para exibir a string. É um problema quando o destrutor é chamado. Lembre-se de que o destrutor de `StringRuim` libera a memória apontada pelo ponteiro `cad`. O efeito de destruir `marinheiro` é este:

```cpp
delete [] marinheiro.cad; // deleta a string para a qual esportes.cad aponta
```

O ponteiro `marinheiro.cad` aponta para `"Spinach Leaves Bowl for Dollars"` porque lhe foi atribuído o valor de `esportes.cad`, que aponta para aquela string. Então a instrução `delete` libera a memória ocupada pela string `"Spinach Leaves Bowl for Dollars"`.

A seguir, o efeito de destruir `esportes` é este:

```cpp
delete [] esportes.cad; // o efeito é indefinido
```

Aqui, `esportes.cad` aponta para o mesmo local de memória que já foi liberado pelo destrutor de `marinheiro`, e isso resulta em comportamento indefinido, possivelmente danoso. No caso da Listagem 12.3, o programa produz strings embaralhadas, o que geralmente é sinal de gerenciamento incorreto de memória.

Outro sintoma perturbador é que tentar deletar a mesma memória duas vezes pode fazer com que o programa seja abortado. O Microsoft Visual C++ 2010 (modo depuração), por exemplo, exibe uma janela de mensagem de erro dizendo "Debug Assertion Failed!", e o g++ 4.4.1 no Linux reporta "double free or corruption" e aborta. Outros sistemas podem fornecer mensagens diferentes ou mesmo nenhuma mensagem, mas o mesmo mal está dentro dos programas.

### Corrigindo o Problema com um Construtor de Cópia Explícito

A cura para os problemas no design de classe é fazer uma **cópia profunda** (*deep copy*). Ou seja, em vez de apenas copiar o endereço da string, o construtor de cópia deve duplicar a string e atribuir o endereço da duplicata ao membro `cad`. Dessa forma, cada objeto obtém sua própria string em vez de referenciar a string de outro objeto. E cada chamada ao destrutor libera uma string diferente em vez de fazer tentativas duplicadas de liberar a mesma string. Veja como você pode codificar o construtor de cópia de `StringRuim`:

```cpp
StringRuim::StringRuim(const StringRuim & st)
{
    num_cadeias++;              // cuida da atualização do membro estático
    tam = st.tam;               // mesmo comprimento
    cad = new char [tam + 1];   // aloca espaço
    std::strcpy(cad, st.cad);   // copia a string para o novo local
    cout << num_cadeias << ": \"" << cad
       << "\" objeto criado\n"; // Para sua informação
}
```

O que torna necessário definir o construtor de cópia é o fato de que alguns membros da classe são ponteiros inicializados com `new` para dados em vez dos próprios dados.

> **Cuidado**
> Se uma classe contém membros que são ponteiros inicializados por `new`, você deve definir um construtor de cópia que copie os dados apontados em vez de copiar apenas os ponteiros em si. Isso é denominado cópia profunda. A forma alternativa de cópia (cópia membro a membro, ou cópia superficial) apenas copia os valores de ponteiros. Uma cópia superficial é exatamente isso — o "raspado superficial" da informação de ponteiro para cópia, em vez da "mineração mais profunda" necessária para copiar as construções referenciadas pelos ponteiros.

## Mais Problemas com StringBad: Operadores de Atribuição

Nem todos os problemas da Listagem 12.3 podem ser atribuídos ao construtor de cópia padrão; você também deve examinar o operador de atribuição padrão. Assim como o ANSI C permite atribuição de estruturas, C++ permite a atribuição de objetos de classe. Ele faz isso sobrecarregando automaticamente um operador de atribuição para uma classe. Esse operador tem o seguinte protótipo:

```cpp
NomeClasse & NomeClasse::operator=(const NomeClasse &);
```

Ou seja, ele recebe e retorna uma referência para um objeto da classe. Por exemplo, aqui está o protótipo para a classe `StringRuim`:

```cpp
StringRuim & StringRuim::operator=(const StringRuim &);
```

**Quando um Operador de Atribuição É Usado e o que Ele Faz**

Um operador de atribuição sobrecarregado é usado quando você atribui um objeto a outro objeto existente:

```cpp
StringRuim manchete1("Celery Stalks at Midnight");
...
StringRuim no;
no = manchete1; // operador de atribuição invocado
```

Um operador de atribuição não é necessariamente usado ao inicializar um objeto:

```cpp
StringRuim eu_tambem = no; // usa construtor de cópia, possivelmente atribuição também
```

Aqui `eu_tambem` é um objeto recém-criado sendo inicializado com os valores de `no`; portanto, o construtor de cópia é usado. Porém, conforme mencionado antes, as implementações têm a opção de lidar com essa instrução em dois passos: usar o construtor de cópia para criar um objeto temporário e depois usar atribuição para copiar os valores para o novo objeto. Ou seja, a inicialização sempre invoca um construtor de cópia, e as formas que usam o operador `=` também podem invocar um operador de atribuição.

Como um construtor de cópia, uma implementação implícita de um operador de atribuição realiza uma cópia membro a membro. Se um membro for ele mesmo um objeto de alguma classe, o programa usa o operador de atribuição definido para aquela classe para fazer a cópia para aquele membro particular. Membros de dados estáticos não são afetados.

**Onde a Atribuição de StringBad Erra**

A Listagem 12.3 atribui `manchete1` a `no`:

```cpp
no = manchete1;
```

Quando o destrutor é chamado para `no`, ele exibe esta mensagem:

```
"Celery Stalks at Midnight" objeto deletado, 2 restando
```

Quando o destrutor é chamado para `manchete1`, ele exibe esta mensagem:

```
"-|" objeto deletado, -2 restando
```

(Muitas implementações abortam antes de chegar a esse ponto.)

Aqui você vê o mesmo problema que o construtor de cópia implícito causou: dados corrompidos. Mais uma vez, o problema é a cópia membro a membro, que faz com que tanto `manchete1.cad` quanto `no.cad` apontem para o mesmo endereço. Assim, quando o destrutor é chamado para `no`, ele deleta a string `"Celery Stalks at Midnight"`, e quando é chamado para `manchete1`, ele tenta deletar a string previamente deletada. Como mencionado anteriormente, o efeito de tentar deletar dados previamente deletados é indefinido, podendo alterar o conteúdo da memória e causar a interrupção do programa.

**Corrigindo a Atribuição**

A solução para os problemas criados por um operador de atribuição padrão inadequado é fornecer sua própria definição de operador de atribuição, uma que faça uma cópia profunda. A implementação é similar à do construtor de cópia, mas há algumas diferenças:

- Como o objeto alvo pode já se referir a dados previamente alocados, a função deve usar `delete[]` para liberar as obrigações anteriores.
- A função deve proteger contra a atribuição de um objeto a si mesmo; caso contrário, a liberação da memória descrita anteriormente poderia apagar o conteúdo do objeto antes de ele ser reatribuído.
- A função retorna uma referência para o objeto invocante.

Ao retornar um objeto, a função pode emular a forma como a atribuição ordinária para tipos embutidos pode ser encadeada. Ou seja, se `C0`, `C1` e `C2` são objetos `StringRuim`, você pode escrever o seguinte:

```cpp
C0 = C1 = C2;
```

Em notação de função, isso se torna o seguinte:

```cpp
C0.operator=(C1.operator=(C2));
```

Assim, o valor de retorno de `C1.operator=(C2)` torna-se o argumento da função `C0.operator=()`. Como o valor de retorno é uma referência para um objeto `StringRuim`, ele é o tipo de argumento correto.

Veja como você poderia escrever um operador de atribuição para a classe `StringRuim`:

```cpp
StringRuim & StringRuim::operator=(const StringRuim & st)
{
    if (this == &st)  // objeto atribuído a si mesmo
        return *this; // pronto
    delete [] cad;    // libera a string antiga
    tam = st.tam;
    cad = new char [tam + 1]; // obtém espaço para a nova string
    std::strcpy(cad, st.cad); // copia a string
    return *this;     // retorna referência para o objeto invocante
}
```

Primeiro, o código verifica se há autoatribuição. Faz isso verificando se o endereço do lado direito da atribuição (`&st`) é o mesmo que o endereço do objeto receptor (`this`). Se for, o programa retorna `*this` e termina. Você pode se lembrar do Capítulo 10 que o operador de atribuição é um dos operadores que pode ser sobrecarregado apenas por uma função membro de classe.

Caso contrário, a função prossegue para liberar a memória para a qual `cad` apontava. A razão para isso é que em breve `cad` receberá o endereço de uma nova string. Se você não aplicar o operador `delete` antes, a string anterior permanecerá na memória. Como o programa não tem mais um ponteiro para a string antiga, essa memória será desperdiçada.

A seguir, o programa prossegue como um construtor de cópia, alocando espaço suficiente para a nova string e depois copiando a string do objeto do lado direito para o novo local.

Quando termina, o programa retorna `*this` e termina.

A atribuição não cria um novo objeto, portanto você não precisa ajustar o valor do membro de dado estático `num_cadeias`.

Adicionar o construtor de cópia e o operador de atribuição descritos anteriormente à classe `StringRuim` elimina todos os problemas. Aqui, por exemplo, estão as últimas linhas da saída após essas mudanças terem sido feitas:

```
Fim de main()
"Celery Stalks at Midnight" objeto deletado, 4 restando
"Spinach Leaves Bowl for Dollars" objeto deletado, 3 restando
"Spinach Leaves Bowl for Dollars" objeto deletado, 2 restando
"Lettuce Prey" objeto deletado, 1 restando
"Celery Stalks at Midnight" objeto deletado, 0 restando
```

A contagem de objetos está correta agora, e nenhuma das strings foi embaralhada.

## A Classe String Nova e Melhorada

Agora que somos um pouco mais sábios, podemos revisar a classe `StringRuim`, renomeando-a para `String`. Primeiro, adicione o construtor de cópia e o operador de atribuição recém-discutidos para que a classe gerencie corretamente a memória usada pelos objetos de classe. A seguir, agora que você viu quando os objetos são construídos e destruídos, podemos silenciar os construtores e destrutores da classe para que eles não mais anunciem cada vez que são usados. Além disso, agora que você não está mais observando os construtores em ação, podemos simplificar o construtor padrão para que ele construa uma string vazia em vez de `"C++"`.

A seguir, podemos adicionar algumas capacidades à classe. Uma classe `String` útil incorporaria toda a funcionalidade da biblioteca padrão `cstring` de funções de string, mas adicionaremos apenas o suficiente para ver o que acontece. (Tenha em mente que esta classe `String` é um exemplo ilustrativo e que a classe `string` padrão do C++ é muito mais extensa.) Em particular, adicionaremos os seguintes métodos:

```cpp
int comprimento() const { return tam; }
friend bool operator<(const String &st, const String &st2);
friend bool operator>(const String &st1, const String &st2);
friend bool operator==(const String &st, const String &st2);
friend operator>>(istream & is, String & st);
char & operator[](int i);
const char & operator[](int i) const;
static int Quantos();
```

O primeiro novo método retorna o comprimento da string armazenada. As próximas três funções amigas permitem comparar strings. A função `operator>>()` fornece capacidades simples de entrada. As duas funções `operator[]()` fornecem acesso por notação de array a caracteres individuais em uma string. O método de classe estático `Quantos()` complementa o membro de dado de classe estático `num_cadeias`.

**O Construtor Padrão Revisado**

O novo construtor padrão merece atenção. Ele se parece com isto:

```cpp
String::String()
{
    tam = 0;
    cad = new char[1];
    cad[0] = '\0'; // string padrão
}
```

Você pode se perguntar por que o código usa

```cpp
cad = new char[1];
```

e não isto:

```cpp
cad = new char;
```

Ambas as formas alocam a mesma quantidade de memória. A diferença é que a primeira forma é compatível com o destrutor da classe, e a segunda não. Lembre-se de que o destrutor contém este código:

```cpp
delete [] cad;
```

Usar `delete[]` é compatível com ponteiros inicializados usando `new[]` e com o ponteiro nulo. Então, outra possibilidade seria substituir

```cpp
cad = new char[1];
cad[0] = '\0';
```

por isto:

```cpp
cad = 0; // define cad como ponteiro nulo
```

O efeito de usar `delete[]` com quaisquer ponteiros inicializados de outra forma é indefinido:

```cpp
char palavras[15] = "má ideia";
char * p1 = palavras;
char * p2 = new char;
char * p3;
delete [] p1; // indefinido, não faça isso
delete [] p2; // indefinido, não faça isso
delete [] p3; // indefinido, não faça isso
```

> **Ponteiro Nulo no C++11**
> No C++98, o literal `0` tinha dois significados — podia ser o valor numérico 0 ou o ponteiro nulo —, tornando difícil para o leitor e o compilador distinguir entre os dois. Às vezes os programadores usam `(void *) 0` para identificar a versão de ponteiro. (O próprio ponteiro nulo pode ter uma representação interna não nula.) Outros programadores usam `NULL`, uma macro C definida para representar o ponteiro nulo. Porém, isso provou ser uma solução incompleta. C++11 fornece uma solução melhor ao introduzir a nova palavra-chave `nullptr` para denotar o ponteiro nulo. Você ainda pode usar `0` como antes — caso contrário, uma enorme quantidade de código existente seria invalidada — mas daí em diante a recomendação é usar `nullptr`:
> ```cpp
> cad = nullptr; // notação de ponteiro nulo do C++11
> ```

**Membros de Comparação**

Três dos métodos na classe `String` realizam comparações. A função `operator<()` retorna `true` se a primeira string vem antes da segunda string em ordem alfabética (ou, mais precisamente, na sequência de ordenação da máquina). A maneira mais simples de implementar as funções de comparação de strings é usar a função padrão `strcmp()`, que retorna um valor negativo se seu primeiro argumento precede o segundo em ordem alfabética, `0` se as strings são iguais, e um valor positivo se o primeiro vem após o segundo em ordem alfabética. Assim, você pode usar `strcmp()` desta forma:

```cpp
bool operator<(const String &st1, const String &st2)
{
    return (std::strcmp(st1.cad, st2.cad) < 0);
}
```

Como o operador `<` embutido já retorna um valor do tipo `bool`, você pode simplificar ainda mais o código para isto. Da mesma forma, você pode codificar as outras duas funções de comparação assim:

```cpp
bool operator>(const String &st1, const String &st2)
{
    return st2 < st1;
}
bool operator==(const String &st1, const String &st2)
{
    return (std::strcmp(st1.cad, st2.cad) == 0);
}
```

A primeira definição expressa o operador `>` em termos do operador `<` e seria uma boa escolha para uma função inline.

Tornar as funções de comparação amigas facilita comparações entre objetos `String` e strings C comuns. Por exemplo, suponha que `resposta` é um objeto `String` e que você tem o seguinte código:

```cpp
if ("amor" == resposta)
```

Isso é traduzido para o seguinte:

```cpp
if (operator==("amor", resposta))
```

O compilador então usa um dos construtores para converter o código, efetivamente para isto:

```cpp
if (operator==(String("amor"), resposta))
```

E isso corresponde ao protótipo.

**Acessando Caracteres pela Notação de Colchetes**

Com uma string C-style padrão, você pode usar colchetes para acessar caracteres individuais:

```cpp
char cidade[40] = "Amsterdam";
cout << cidade[0] << endl; // exibe a letra A
```

Em C++, os dois símbolos de colchete constituem um único operador, o **operador de colchete** (*bracket operator*), e você pode sobrecarregar esse operador usando um método chamado `operator[]()`. Tipicamente, um operador C++ binário (com dois operandos) coloca o operador entre os dois operandos, como em `2 + 5`. Mas o operador de colchete coloca um operando antes do primeiro colchete e o outro operando entre os dois colchetes. Assim, na expressão `cidade[0]`, `cidade` é o primeiro operando, `[]` é o operador, e `0` é o segundo operando.

Suponha que `opera` seja um objeto `String`:

```cpp
String opera("The Magic Flute");
```

Se você usa a expressão `opera[4]`, C++ procura um método com este nome e assinatura:

```cpp
String::operator[](int i)
```

Se encontrar um protótipo correspondente, o compilador substitui a expressão `opera[4]` por esta chamada de função:

```cpp
opera.operator[](4)
```

O objeto `opera` invoca o método, e o índice de array `4` torna-se o argumento da função.

Aqui está uma implementação simples:

```cpp
char & String::operator[](int i)
{
    return cad[i];
}
```

Com essa definição, a instrução

```cpp
cout << opera[4];
```

torna-se isto:

```cpp
cout << opera.operator[](4);
```

O valor de retorno é `opera.cad[4]`, ou o caractere `'M'`. Então o método público fornece acesso a dados privados.

Declarar o tipo de retorno como `char &` permite atribuir valores a um elemento particular. Por exemplo, você pode usar o seguinte:

```cpp
String significa("might");
significa[0] = 'r';
```

A segunda instrução é convertida para uma chamada de função de operador sobrecarregado:

```cpp
significa.operator[][0] = 'r';
```

Isso atribui `'r'` ao valor de retorno do método. Mas a função retorna uma referência para `significa.cad[0]`, tornando o código equivalente a

```cpp
significa.cad[0] = 'r';
```

Esta última linha de código viola o acesso privado, mas como `operator[]()` é um método de classe, é permitida alterar o conteúdo do array. O efeito líquido do código é que `"might"` torna-se `"right"`.

Suponha que você tenha um objeto constante:

```cpp
const String resposta("fútil");
```

Então, se a única definição disponível para `operator[]()` for a que você acabou de ver, o seguinte código é rotulado como erro:

```cpp
cout << resposta[1]; // erro em tempo de compilação
```

A razão é que `resposta` é `const`, e o método não promete não alterar os dados. (Na verdade, às vezes o trabalho do método é alterar dados, portanto ele não pode prometer não fazê-lo.)

Porém, C++ distingue entre assinaturas de função `const` e não `const` ao sobrecarregar, portanto você pode fornecer uma segunda versão de `operator[]()` que é usada apenas por objetos `String const`:

```cpp
// para uso com objetos String const
const char & String::operator[](int i) const
{
    return cad[i];
}
```

Com as definições, você tem acesso de leitura e escrita a objetos `String` regulares e acesso somente leitura a dados `String const`:

```cpp
String texto("Era uma vez");
const String resposta("fútil");
cout << texto[1];    // ok, usa versão não-const de operator[]()
cout << resposta[1]; // ok, usa versão const de operator[]()
cin >> texto[1];     // ok, usa versão não-const de operator[]()
cin >> resposta[1];  // erro em tempo de compilação
```

**Funções Membro de Classe Estáticas**

É possível declarar uma função membro como sendo estática. (A palavra-chave `static` deve aparecer na declaração da função, mas não na definição da função se esta for separada.) Isso tem duas consequências importantes.

Primeiro, uma função membro estática não precisa ser invocada por um objeto; na verdade, ela nem obtém um ponteiro `this` para usar. Se a função membro estática é declarada na seção pública, ela pode ser invocada usando o nome da classe e o operador de resolução de escopo. Por exemplo, você pode dar à classe `String` uma função membro estática chamada `Quantos()` com o seguinte protótipo/definição na declaração de classe:

```cpp
static int Quantos() { return num_cadeias; }
```

Ela poderia ser invocada assim:

```cpp
int contagem = String::Quantos(); // invocando uma função membro estática
```

A segunda consequência é que, como uma função membro estática não está associada a um objeto particular, os únicos membros de dados que ela pode usar são os membros de dados estáticos. Por exemplo, o método estático `Quantos()` pode acessar o membro estático `num_cadeias`, mas não `cad` ou `tam`.

Da mesma forma, uma função membro estática pode ser usada para definir um sinalizador de toda a classe que controla como algum aspecto da interface de classe se comporta. Por exemplo, ela pode controlar a formatação usada por um método que exibe o conteúdo de uma classe.

**Mais Sobrecarga do Operador de Atribuição**

Antes de olhar as novas listagens para o exemplo da classe `String`, vamos considerar outro assunto. Suponha que você queira copiar uma string comum para um objeto `String`. Por exemplo, suponha que você use `getline()` para ler uma string e queira colocá-la em um objeto `String`. Os métodos de classe já permitem fazer o seguinte:

```cpp
String nome;
char temp[40];
cin.getline(temp, 40);
nome = temp; // use o construtor para converter o tipo
```

Porém, isso pode não ser uma solução satisfatória se você precisar fazê-lo com frequência. Para ver por quê, vamos revisar como a instrução final funciona:

1. O programa usa o construtor `String(const char *)` para construir um objeto `String` temporário contendo uma cópia da string armazenada em `temp`. Lembre-se do Capítulo 11, "Trabalhando com Classes", que um construtor com um único argumento serve como uma função de conversão.
2. O programa usa a função `String & String::operator=(const String &)` para copiar as informações do objeto temporário para o objeto `nome`.
3. O programa chama o destrutor `~String()` para deletar o objeto temporário.

A maneira mais simples de tornar o processo mais eficiente é sobrecarregar o operador de atribuição para que funcione diretamente com strings comuns. Isso remove os passos extras de criar e destruir um objeto temporário. Aqui está uma possível implementação:

```cpp
String & String::operator=(const char * s)
{
    delete [] cad;
    tam = std::strlen(s);
    cad = new char[tam + 1];
    std::strcpy(cad, s);
    return *this;
}
```

Como de costume, você deve desalocar a memória gerenciada anteriormente por `cad` e alocar memória suficiente para a nova string.

A Listagem 12.4 mostra a declaração de classe revisada. Além das mudanças já mencionadas, ela define a constante `CINLIM`, que é usada na implementação de `operator>>()`.

### Listagem 12.4 — string1.h

```cpp
// string1.h -- definição de classe string corrigida e aprimorada
#ifndef STRING1_H_
#define STRING1_H_
#include <iostream>
using std::ostream;
using std::istream;

class String
{
private:
    char * cad;          // ponteiro para a string
    int tam;             // comprimento da string
    static int num_cadeias;      // número de objetos
    static const int CINLIM = 80; // limite de entrada do cin
public:
// construtores e outros métodos
    String(const char * s); // construtor
    String();               // construtor padrão
    String(const String &); // construtor de cópia
    ~String();              // destrutor
    int comprimento() const { return tam; }
// métodos de operador sobrecarregados
    String & operator=(const String &);
    String & operator=(const char *);
    char & operator[](int i);
    const char & operator[](int i) const;
// amigos de operador sobrecarregados
    friend bool operator<(const String &st, const String &st2);
    friend bool operator>(const String &st1, const String &st2);
    friend bool operator==(const String &st, const String &st2);
    friend ostream & operator<<(ostream & os, const String & st);
    friend istream & operator>>(istream & is, String & st);
// função estática
    static int Quantos();
};
#endif
```

A Listagem 12.5 apresenta as definições de método revisadas.

### Listagem 12.5 — string1.cpp

```cpp
// string1.cpp -- métodos da classe String
#include <cstring> // string.h em alguns compiladores
#include "string1.h" // inclui <iostream>
using std::cin;
using std::cout;
// inicializando membro de classe estático
int String::num_cadeias = 0;

// método estático
int String::Quantos()
{
    return num_cadeias;
}
// métodos da classe
String::String(const char * s) // constrói String a partir de string C
{
    tam = std::strlen(s);       // define o tamanho
    cad = new char[tam + 1];    // aloca armazenamento
    std::strcpy(cad, s);        // inicializa o ponteiro
    num_cadeias++;              // atualiza contagem de objetos
}
String::String()               // construtor padrão
{
    tam = 0;
    cad = new char[1];
    cad[0] = '\0';             // string padrão
    num_cadeias++;
}
String::String(const String & st)
{
    num_cadeias++;              // cuida da atualização do membro estático
    tam = st.tam;               // mesmo comprimento
    cad = new char [tam + 1];   // aloca espaço
    std::strcpy(cad, st.cad);   // copia string para o novo local
}
String::~String()              // destrutor necessário
{
    --num_cadeias;              // obrigatório
    delete [] cad;              // obrigatório
}
// métodos de operador sobrecarregados
// atribui uma String a uma String
String & String::operator=(const String & st)
{
    if (this == &st)
        return *this;
    delete [] cad;
    tam = st.tam;
    cad = new char[tam + 1];
    std::strcpy(cad, st.cad);
    return *this;
}
// atribui uma string C a uma String
String & String::operator=(const char * s)
{
    delete [] cad;
    tam = std::strlen(s);
    cad = new char[tam + 1];
    std::strcpy(cad, s);
    return *this;
}
// acesso de leitura e escrita por char para String não-const
char & String::operator[](int i)
{
    return cad[i];
}
// acesso somente leitura por char para String const
const char & String::operator[](int i) const
{
    return cad[i];
}
// amigos de operador sobrecarregados
bool operator<(const String &st1, const String &st2)
{
    return (std::strcmp(st1.cad, st2.cad) < 0);
}
bool operator>(const String &st1, const String &st2)
{
    return st2 < st1;
}
bool operator==(const String &st1, const String &st2)
{
    return (std::strcmp(st1.cad, st2.cad) == 0);
}
// saída simples de String
ostream & operator<<(ostream & os, const String & st)
{
    os << st.cad;
    return os;
}
// entrada rápida e suja de String
istream & operator>>(istream & is, String & st)
{
    char temp[String::CINLIM];
    is.get(temp, String::CINLIM);
    if (is)
        st = temp;
    while (is && is.get() != '\n')
        continue;
    return is;
}
```

O operador `>>` sobrecarregado fornece uma maneira simples de ler uma linha de entrada do teclado em um objeto `String`. Ele presume uma linha de entrada de `String::CINLIM` ou menos caracteres e descarta quaisquer caracteres além desse limite. Tenha em mente que o valor de um objeto `istream` em uma condição `if` avalia como `false` se a entrada falhar por algum motivo, como encontrar uma condição de fim de arquivo, ou no caso de `get(char *, int)`, ao ler uma linha vazia.

A Listagem 12.6 exercita a classe `String` com um programa curto que permite inserir algumas strings. O programa faz o usuário inserir provérbios (*sayings*), coloca as strings em objetos `String`, exibe-as e reporta qual string é a mais curta e qual vem primeiro em ordem alfabética.

### Listagem 12.6 — sayings1.cpp

```cpp
// sayings1.cpp -- usando a classe String expandida
// compilar com string1.cpp
#include <iostream>
#include "string1.h"
const int TamArr = 10;
const int CompMax = 81;
int main()
{
    using std::cout;
    using std::cin;
    using std::endl;
    String nome;
    cout << "Oi, qual é o seu nome?\n>> ";
    cin >> nome;
    cout << nome << ", insira até " << TamArr
       << " provérbios curtos <linha vazia para sair>:\n";
    String dizeres[TamArr]; // array de objetos
    char temp[CompMax];     // armazenamento temporário de string
    int i;
    for (i = 0; i < TamArr; i++)
    {
        cout << i+1 << ": ";
        cin.get(temp, CompMax);
        while (cin && cin.get() != '\n')
            continue;
        if (!cin || temp[0] == '\0') // linha vazia?
            break;      // i não é incrementado
        else
            dizeres[i] = temp; // atribuição sobrecarregada
    }
    int total = i; // número total de linhas lidas
    if (total > 0)
    {
        cout << "Aqui estão seus provérbios:\n";
        for (i = 0; i < total; i++)
            cout << dizeres[i][0] << ": " << dizeres[i] << endl;
        int menor = 0;
        int primeiro = 0;
        for (i = 1; i < total; i++)
        {
            if (dizeres[i].comprimento() < dizeres[menor].comprimento())
                menor = i;
            if (dizeres[i] < dizeres[primeiro])
                primeiro = i;
        }
        cout << "Provérbio mais curto:\n" << dizeres[menor] << endl;
        cout << "Primeiro em ordem alfabética:\n" << dizeres[primeiro] << endl;
        cout << "Este programa usou " << String::Quantos()
           << " objetos String. Até logo.\n";
    }
    else
        cout << "Nenhuma entrada! Até logo.\n";
    return 0;
}
```

> **Nota**
> Versões mais antigas de `get(char *, int)` não avaliam como `false` ao ler uma linha vazia. Para essas versões, no entanto, o primeiro caractere na string é um caractere nulo se uma linha vazia for inserida. Este exemplo usa o seguinte código:
> ```cpp
> if (!cin || temp[0] == '\0') // linha vazia?
>     break; // i não é incrementado
> ```
> Se a implementação segue o padrão C++ atual, o primeiro teste na instrução `if` detecta uma linha vazia, enquanto o segundo teste detecta a linha vazia para implementações mais antigas.

O programa na Listagem 12.6 pede ao usuário que insira até 10 provérbios. Cada provérbio é lido em um array de caracteres temporário e depois copiado para um objeto `String`. Se o usuário inserir uma linha em branco, uma instrução `break` termina o loop de entrada. Após ecoar a entrada, o programa usa as funções membro `comprimento()` e `operator<()` para localizar a string mais curta e a string em ordem alfabética mais inicial. O programa também usa o operador de subscrição (`[]`) para prefixar cada provérbio com seu caractere inicial. Aqui está uma execução de amostra:

```
Oi, qual é o seu nome?
>> Misty Gutz
Misty Gutz, insira até 10 provérbios curtos <linha vazia para sair>:
1: a fool and his money are soon parted
2: penny wise, pound foolish
3: the love of money is the root of much evil
4: out of sight, out of mind
5: absence makes the heart grow fonder
6: absinthe makes the hart grow fonder
7:
Aqui estão seus provérbios:
a: a fool and his money are soon parted
p: penny wise, pound foolish
t: the love of money is the root of much evil
o: out of sight, out of mind
a: absence makes the heart grow fonder
a: absinthe makes the hart grow fonder
Provérbio mais curto:
penny wise, pound foolish
Primeiro em ordem alfabética:
a fool and his money are soon parted
Este programa usou 11 objetos String. Até logo.
```

---

[Anterior](capitulo-11-03-conversoes-resumo.md) | [Índice](README.md) | [Próximo](capitulo-12-02-retorno-ponteiros.md)
