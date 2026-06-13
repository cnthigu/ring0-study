# Capítulo 10 — Objetos e Classes (Objects and Classes)

> Tradução não oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Navegação: [Anterior](capitulo-09-03-namespaces-resumo.md) | [Índice](README.md) | [Próximo](capitulo-10-02-construtores-destrutores-this.md)

Neste capítulo você aprenderá sobre os seguintes tópicos:

- Programação procedural e orientada a objetos
- O conceito de classes
- Como definir e implementar uma classe
- Acesso público e privado a classes
- Membros de dados de classes
- Métodos de classes (também chamados de membros de função de classe)
- Criando e usando objetos de classe
- Construtores e destrutores de classes
- Funções membros `const`
- O ponteiro `this`
- Criando arrays de objetos
- Escopo de classe
- Tipos de dados abstratos

A *programação orientada a objetos* (OOP, object-oriented programming) é uma abordagem conceitual particular para projetar programas, e o C++ aprimorou o C com recursos que facilitam a aplicação dessa abordagem. A seguir estão as características OOP mais importantes:

- Abstração
- Encapsulamento e ocultação de dados
- Polimorfismo
- Herança
- Reutilização de código

A classe é o aprimoramento C++ mais importante para implementar esses recursos e conectá-los. Este capítulo começa o exame das classes. Ele explica abstração, encapsulamento e ocultação de dados, e mostra como as classes implementam esses recursos. Ele discute como definir uma classe, fornecer a uma classe seções públicas e privadas, e criar funções membros que trabalham com os dados da classe. Além disso, este capítulo apresenta construtores e destrutores, que são funções membros especiais para criar e descartar objetos pertencentes a uma classe. Por fim, você conhece o ponteiro `this`, um componente importante de alguma programação de classe. Os capítulos seguintes estendem essa discussão para sobrecarga de operadores (outra variedade de polimorfismo) e herança, a base para reutilizar código.

## Programação Procedural e Orientada a Objetos (Procedural and Object-Oriented Programming)

Embora neste livro tenhamos ocasionalmente explorado a perspectiva OOP sobre programação, geralmente ficamos bem próximos da abordagem procedural padrão de linguagens como C, Pascal e BASIC. Vamos examinar um exemplo que esclarece como a perspectiva OOP difere da programação procedural.

Como o mais novo membro do time de softball Genre Giants, foi pedido que você mantivesse as estatísticas do time. Naturalmente, você recorre ao computador para ajuda. Se você fosse um programador procedural, poderia pensar assim:

*Vamos ver, quero inserir o nome, vezes no bastão, número de rebatidas, médias de rebatidas (para aqueles que não acompanham beisebol ou softball, a média de rebatidas é o número de acertos dividido pelo número oficial de vezes no bastão do jogador; uma rebatida oficial termina quando um jogador chega à base ou é eliminado, mas certos eventos, como receber quatro bolas, não contam como rebatidas oficiais), e todas essas outras grandes estatísticas básicas para cada jogador. Espera, o computador deveria facilitar minha vida, então quero que ele calcule parte desse material, como a média de rebatidas. Também quero que o programa relate os resultados. Como devo organizar isso? Acho que devo fazer as coisas direito e usar funções. Sim, vou fazer `main()` chamar uma função para obter a entrada, chamar outra função para fazer os cálculos e depois chamar uma terceira função para relatar os resultados. Hmm, o que acontece quando recebo dados do próximo jogo? Não quero começar do zero novamente. Ok, posso adicionar uma função para atualizar as estatísticas. Caramba, talvez eu precise de um menu em `main()` para selecionar entre inserir, calcular, atualizar e mostrar os dados. Hmm... como vou representar os dados? Posso usar um array de strings para manter os nomes dos jogadores, outro array para manter as rebatidas de cada jogador, mais outro array para manter os acertos, e assim por diante. Não, isso é burrice. Posso projetar uma estrutura para manter todas as informações de um único jogador e depois usar um array dessas estruturas para representar todo o time.*

Em suma, com uma abordagem procedural, você primeiro se concentra nos procedimentos que seguirá e depois pensa em como representar os dados. (Para não ter que manter o programa em execução durante toda a temporada, provavelmente também vai querer poder salvar dados em um arquivo e lê-los de um arquivo.)

Agora vamos ver como sua perspectiva muda quando você coloca o chapéu OOP (em um design polimórfico atraente). Você começa pensando nos dados. Além disso, pensa nos dados não apenas em termos de como representá-los, mas em termos de como eles serão usados:

*Vamos ver, o que estou acompanhando? Um jogador de beisebol, é claro. Então quero um objeto que represente o jogador inteiro, não apenas sua média de rebatidas ou vezes no bastão. Sim, essa será minha unidade de dados fundamental, um objeto representando o nome e as estatísticas de um jogador. Vou precisar de alguns métodos para lidar com esse objeto. Hmm, acho que preciso de um método para obter informações básicas nessa unidade. O computador deve calcular algumas coisas, como as médias de rebatidas — posso adicionar métodos para fazer os cálculos. E o programa deve fazer esses cálculos automaticamente, sem que o usuário precise lembrar de pedir que sejam feitos. Também precisarei de métodos para atualizar e exibir as informações. Portanto, o usuário tem três maneiras de interagir com os dados: inicialização, atualização e relatório. Essa é a interface do usuário.*

Em suma, com uma abordagem OOP, você se concentra no objeto como o usuário o percebe, pensando nos dados que precisa para descrever o objeto e nas operações que descreverão a interação do usuário com os dados. Depois de desenvolver uma descrição dessa interface, você decide como implementar a interface e o armazenamento de dados. Por fim, você monta um programa para usar seu novo design.

## Abstração e Classes (Abstraction and Classes)

A vida está cheia de complexidades, e uma maneira de lidar com a complexidade é enquadrar abstrações simplificadoras. Você é uma coleção de mais de um octilhão de átomos. Alguns estudiosos da mente diriam que sua mente é uma coleção de vários agentes semiautônomos. Mas é muito mais simples pensar em si mesmo como uma entidade única. Na computação, a abstração é o passo crucial de representar informações em termos de sua interface com o usuário. Ou seja, você abstrai as características operacionais essenciais de um problema e expressa uma solução nesses termos. No exemplo de estatísticas de softball, a interface descreve como o usuário inicializa, atualiza e exibe os dados. Da abstração, é um curto passo para o tipo definido pelo usuário, que em C++ é um design de classe que implementa a interface abstrata.

### O Que É um Tipo? (What Is a Type?)

Vamos pensar um pouco mais sobre o que constitui um tipo. Por exemplo, o que é um nerd? Se você assina o estereótipo popular, pode pensar em um nerd em termos visuais — óculos grossos de aro preto, protetor de bolso cheio de canetas e assim por diante. Após uma reflexão, você pode concluir que um nerd é melhor definido operacionalmente — por exemplo, em termos de como ele ou ela responde a uma situação social estranha. Você tem uma situação semelhante, se não se importar com analogias forçadas, com uma linguagem procedural como C. A princípio, você tende a pensar em um tipo de dado em termos de sua aparência — como é armazenado na memória. Um `char`, por exemplo, é 1 byte de memória, e um `double` é frequentemente 8 bytes de memória. Mas uma reflexão leva você a concluir que um tipo de dado também é definido em termos das operações que podem ser realizadas nele. Por exemplo, o tipo `int` pode usar todas as operações aritméticas. Você pode adicionar, subtrair, multiplicar e dividir inteiros. Você também pode usar o operador módulo (`%`) com eles.

Por outro lado, considere ponteiros. Um ponteiro pode muito bem requerer a mesma quantidade de memória que um `int`. Pode até ser representado internamente como um inteiro. Mas um ponteiro não permite as mesmas operações que um inteiro. Você não pode, por exemplo, multiplicar dois ponteiros entre si. O conceito não faz sentido, então o C++ não o implementa. Assim, quando você declara uma variável como `int` ou como um ponteiro para `float`, você não está apenas alocando memória — você também está estabelecendo quais operações podem ser realizadas com a variável. Em suma, especificar um tipo básico faz três coisas:

- Determina quanta memória é necessária para um objeto de dados.
- Determina como os bits na memória são interpretados. (Um `long` e um `float` podem usar o mesmo número de bits na memória, mas são traduzidos para valores numéricos de forma diferente.)
- Determina quais operações, ou métodos, podem ser executados usando o objeto de dados.

Para tipos embutidos, as informações sobre operações estão embutidas no compilador. Mas quando você define um tipo definido pelo usuário em C++, você tem que fornecer o mesmo tipo de informação você mesmo. Em troca desse trabalho extra, você ganha o poder e a flexibilidade de adaptar novos tipos de dados para corresponder aos requisitos do mundo real.

### Classes em C++ (Classes in C++)

Uma *classe* (class) é um veículo C++ para traduzir uma abstração em um tipo definido pelo usuário. Ela combina representação de dados e métodos para manipular esses dados em um único pacote organizado. Vamos examinar uma classe que representa ações de bolsa.

Primeiro, você tem que pensar um pouco em como representar ações. Você poderia tomar uma ação como unidade básica e definir uma classe para representar uma ação. No entanto, isso implica que você precisaria de 100 objetos para representar 100 ações, o que não é prático. Em vez disso, você pode representar as posições atuais de uma pessoa em uma ação específica como unidade básica. O número de ações possuídas seria parte da representação de dados. Uma abordagem realista teria que manter registros de coisas como preço de compra inicial e data de compra para fins fiscais. Também teria que gerenciar eventos como divisão de ações. Isso parece um pouco ambicioso para um primeiro esforço em definir uma classe, então em vez disso você pode ter uma visão idealizada e simplificada das coisas. Em particular, você pode limitar as operações que pode realizar para o seguinte:

- Adquirir ações de uma empresa.
- Comprar mais ações da mesma ação.
- Vender ações.
- Atualizar o valor por ação de uma ação.
- Exibir informações sobre as posições.

Você pode usar esta lista para definir a interface pública para a classe de ações. (E pode adicionar recursos adicionais mais tarde, se estiver interessado.) Para suportar esta interface, você precisa armazenar algumas informações. Novamente, você pode usar uma abordagem simplificada. Por exemplo, não se preocupe com a prática brasileira de avaliar ações em múltiplos específicos. Aqui está uma lista de informações para armazenar:

- Nome da empresa
- Número de ações possuídas
- Valor de cada ação
- Valor total de todas as ações

Em seguida, você pode definir a classe. Geralmente, uma especificação de classe tem duas partes:

- Uma *declaração de classe* (class declaration), que descreve o componente de dados, em termos de membros de dados, e a interface pública, em termos de funções membros, chamados *métodos* (methods)
- As *definições dos métodos da classe* (class method definitions), que descrevem como certas funções membros da classe são implementadas

Por falar, a declaração da classe fornece uma visão geral da classe, enquanto as definições dos métodos fornecem os detalhes.

> **O Que É uma Interface?**
>
> Uma *interface* (interface) é uma estrutura compartilhada para interações entre dois sistemas — por exemplo, entre um computador e uma impressora ou entre um usuário e um programa de computador. Para as classes, falamos da interface pública. Nesse caso, o público é o programa que usa a classe, o sistema interagente consiste nos objetos da classe, e a interface consiste nos métodos fornecidos por quem escreveu a classe. A interface permite que você, o programador, escreva código que interage com objetos da classe, e assim permite que o programa use os objetos da classe. Por exemplo, para encontrar o número de caracteres em um objeto `string`, você não abre o objeto para ver o que está dentro; você simplesmente usa o método `size()` fornecido pelos criadores da classe. A classe nega acesso direto ao usuário público. Mas o público tem permissão para usar o método `size()`. O método `size()`, então, é parte da interface pública entre o usuário e um objeto da classe `string`.

Desenvolver uma classe e um programa que a usa requer vários passos. Em vez de dar todos de uma vez, vamos dividir o desenvolvimento em etapas menores. Tipicamente, programadores C++ colocam a interface, na forma de uma definição de classe, em um arquivo de cabeçalho e colocam a implementação, na forma de código para os métodos da classe, em um arquivo de código-fonte. A Listagem 10.1 apresenta a primeira etapa, uma declaração de classe tentativa para uma classe chamada `Acao`. O arquivo usa `#ifndef`, e assim por diante, como descrito no Capítulo 9, para proteger contra múltiplas inclusões de arquivo.

Para ajudar a identificar classes, este livro segue uma convenção comum, mas não universal, de capitalizar nomes de classes. Você notará que a Listagem 10.1 parece uma declaração de estrutura com algumas rugas adicionais, como funções membros e seções públicas e privadas. Vamos melhorar esta declaração em breve (portanto, não a use como modelo), mas primeiro vamos ver como essa definição funciona.

**Listagem 10.1** acao00.h

```cpp
// acao00.h -- interface da classe Acao
// versão 00
#ifndef ACAO00_H_
#define ACAO00_H_
#include <string>
class Acao  // declaração da classe
{
private:
    std::string empresa;
    long num_acoes;
    double val_acao;
    double val_total;
    void def_total() { val_total = num_acoes * val_acao; }
public:
    void adquirir(const std::string & emp, long n, double pr);
    void comprar(long num, double preco);
    void vender(long num, double preco);
    void atualizar(double preco);
    void exibir();
};  // note o ponto e vírgula no final
#endif
```

Você verá os detalhes da classe mais de perto mais tarde, mas primeiro vamos examinar as características mais gerais. Para começar, a palavra-chave C++ `class` identifica o código na Listagem 10.1 como definindo o design de uma classe. (Neste contexto, as palavras-chave `class` e `typename` não são sinônimas da forma como eram nos parâmetros de template; `typename` não pode ser usado aqui.) A sintaxe identifica `Acao` como o nome do tipo para esta nova classe. Essa declaração permite que você declare variáveis, chamadas *objetos* (objects), ou instâncias (instances), do tipo `Acao`. Cada objeto individual representa uma única posição. Por exemplo, as seguintes declarações criam dois objetos `Acao` chamados `ana` e `sol`:

```cpp
Acao ana;
Acao sol;
```

O objeto `ana`, por exemplo, poderia representar as posições de Ana em ações de uma empresa específica.

Em seguida, observe que as informações que você decidiu armazenar aparecem na forma de membros de dados de classe, como `empresa` e `num_acoes`. O membro `empresa` de `ana`, por exemplo, mantém o nome da empresa, o membro `num_acoes` mantém o número de ações que Ana possui, o membro `val_acao` mantém o valor de cada ação, e o membro `val_total` mantém o valor total de todas as ações. Da mesma forma, as operações desejadas aparecem como funções membros de classe (ou métodos), como `vender()` e `atualizar()`. Uma função membro pode ser definida no lugar — por exemplo, `def_total()` — ou pode ser representada por um protótipo, como as outras funções membros nesta classe. As definições completas para as outras funções membros vêm mais tarde no arquivo de implementação, mas os protótipos são suficientes para descrever as interfaces de função. A vinculação de dados e métodos em uma única unidade é a característica mais marcante da classe.

### Controle de Acesso (Access Control)

Também novas são as palavras-chave `private` e `public`. Esses rótulos descrevem o *controle de acesso* (access control) para membros da classe. Qualquer programa que usa um objeto de uma classe específica pode acessar as partes públicas diretamente. Um programa pode acessar os membros privados de um objeto apenas usando as funções membros públicas (ou, como você verá no Capítulo 11, "Trabalhando com Classes", via uma função `friend`). Por exemplo, a única maneira de alterar o membro `num_acoes` da classe `Acao` é usar uma das funções membros de `Acao`. Assim, as funções membros públicas atuam como intermediárias entre um programa e os membros privados de um objeto; elas fornecem a interface entre o objeto e o programa. Esse isolamento de dados do acesso direto por um programa é chamado de *ocultação de dados* (data hiding). (O C++ fornece uma terceira palavra-chave de controle de acesso, `protected`, que discutiremos quando cobrir herança de classe no Capítulo 13, "Herança de Classe".)

Um design de classe tenta separar a interface pública dos detalhes específicos da implementação. A interface pública representa o componente de abstração do design. Reunir os detalhes de implementação e separá-los da abstração é chamado de *encapsulamento* (encapsulation). A ocultação de dados (colocar dados na seção privada de uma classe) é uma instância de encapsulamento, assim como ocultar os detalhes funcionais de uma implementação na seção privada, como a classe `Acao` faz com `def_total()`. Outro exemplo de encapsulamento é a prática usual de colocar definições de funções de classe em um arquivo separado da declaração de classe.

> **Classes e Estruturas**
>
> As descrições de classes parecem muito com declarações de estruturas com a adição de funções membros e rótulos de visibilidade `public` e `private`. Na verdade, o C++ estende às estruturas os mesmos recursos que as classes têm. A única diferença é que o tipo de acesso padrão para uma estrutura é `public`, enquanto o tipo padrão para uma classe é `private`. Programadores C++ comumente usam classes para implementar descrições de classes enquanto restringem estruturas a representar objetos de dados puros (muitas vezes chamados de estruturas de dados simples-e-antigas, ou estruturas POD).

### Controle de Acesso de Membros: Público ou Privado? (Member Access Control)

Você pode declarar membros da classe, sejam itens de dados ou funções membros, na seção pública ou privada de uma classe. Mas porque um dos principais preceitos da OOP é ocultar os dados, os itens de dados normalmente vão para a seção privada. As funções membros que constituem a interface da classe vão para a seção pública; caso contrário, você não pode chamar essas funções de um programa. Como a declaração de `Acao` mostra, você também pode colocar funções membros na seção privada. Você não pode chamar essas funções diretamente de um programa, mas os métodos públicos podem usá-las. Tipicamente, você usa funções membros privadas para lidar com detalhes de implementação que não formam parte da interface pública.

Você não precisa usar a palavra-chave `private` em declarações de classe porque esse é o controle de acesso padrão para objetos de classe:

```cpp
class Mundo
{
    float massa;       // private por padrão
    char nome[20];     // private por padrão
public:
    void contar_tudo(void);
    // ...
};
```

No entanto, este livro usa explicitamente o rótulo `private` para enfatizar o conceito de ocultação de dados.

## Implementando Funções Membros de Classe (Implementing Class Member Functions)

Ainda temos que criar a segunda parte da especificação de classe: fornecer código para as funções membros representadas por um protótipo na declaração de classe. As definições de funções membros são muito parecidas com as definições de funções regulares. Cada uma tem um cabeçalho de função e um corpo de função. As definições de funções membros podem ter tipos de retorno e argumentos. Mas elas também têm duas características especiais:

- Quando você define uma função membro, você usa o operador de resolução de escopo (`::`) para identificar a classe à qual a função pertence.
- Métodos de classe podem acessar os componentes `private` da classe.

Vamos examinar esses pontos agora.

Primeiro, o cabeçalho de função para uma função membro usa o operador de resolução de escopo (`::`) para indicar a qual classe a função pertence. Por exemplo, o cabeçalho para a função membro `atualizar()` é assim:

```cpp
void Acao::atualizar(double preco)
```

Essa notação significa que você está definindo a função `atualizar()` que é membro da classe `Acao`. Não apenas isso identifica `atualizar()` como uma função membro, mas significa que você pode usar o mesmo nome para uma função membro de uma classe diferente. Por exemplo, uma função `atualizar()` para uma classe `Bobagem` teria este cabeçalho de função:

```cpp
void Bobagem::atualizar()
```

Assim, o operador de resolução de escopo resolve a identidade da classe à qual uma definição de método se aplica. Dizemos que o identificador `atualizar()` tem *escopo de classe* (class scope). Outras funções membros da classe `Acao` podem, se necessário, usar o método `atualizar()` sem usar o operador de resolução de escopo. Isso porque elas pertencem à mesma classe, tornando `atualizar()` no escopo.

O nome completo de um método de classe inclui o nome da classe. `Acao::atualizar()` é chamado de *nome qualificado* (qualified name) da função. Um simples `atualizar()`, por outro lado, é uma abreviação (o nome não qualificado) para o nome completo — uma que pode ser usada apenas em escopo de classe.

A segunda característica especial dos métodos é que um método pode acessar os membros privados de uma classe. Por exemplo, o método `exibir()` pode usar código como este:

```cpp
std::cout << "Empresa: " << empresa
          << " Acoes: " << num_acoes << endl
          << " Preco por Acao: R$" << val_acao
          << " Valor Total: R$" << val_total << endl;
```

Aqui `empresa`, `num_acoes`, e assim por diante são membros de dados privados da classe `Acao`. Se você tentar usar uma função não-membro para acessar esses membros de dados, o compilador o impedirá imediatamente.

Com esses dois pontos em mente, podemos implementar os métodos da classe conforme mostrado na Listagem 10.2. Colocamos em um arquivo de implementação separado, portanto o arquivo precisa incluir o arquivo de cabeçalho `acao00.h` para que o compilador possa acessar a definição da classe.

**Listagem 10.2** acao00.cpp

```cpp
// acao00.cpp -- implementando a classe Acao
// versão 00
#include <iostream>
#include "acao00.h"
void Acao::adquirir(const std::string & emp, long n, double pr)
{
    empresa = emp;
    if (n < 0)
    {
        std::cout << "O numero de acoes nao pode ser negativo; "
                  << empresa << " acoes definidas como 0.\n";
        num_acoes = 0;
    }
    else
        num_acoes = n;
    val_acao = pr;
    def_total();
}
void Acao::comprar(long num, double preco)
{
    if (num < 0)
    {
        std::cout << "O numero de acoes compradas nao pode ser negativo. "
                  << "Transacao cancelada.\n";
    }
    else
    {
        num_acoes += num;
        val_acao = preco;
        def_total();
    }
}
void Acao::vender(long num, double preco)
{
    using std::cout;
    if (num < 0)
    {
        cout << "O numero de acoes vendidas nao pode ser negativo. "
             << "Transacao cancelada.\n";
    }
    else if (num > num_acoes)
    {
        cout << "Voce nao pode vender mais do que tem! "
             << "Transacao cancelada.\n";
    }
    else
    {
        num_acoes -= num;
        val_acao = preco;
        def_total();
    }
}
void Acao::atualizar(double preco)
{
    val_acao = preco;
    def_total();
}
void Acao::exibir()
{
    std::cout << "Empresa: " << empresa
              << " Acoes: " << num_acoes << '\n'
              << " Preco por Acao: R$" << val_acao
              << " Valor Total: R$" << val_total << '\n';
}
```

### Observações sobre Funções Membros (Member Function Notes)

A função `adquirir()` gerencia a primeira aquisição de ações de uma determinada empresa, enquanto `comprar()` e `vender()` gerenciam a adição ou subtração de uma posição existente. Os métodos `comprar()` e `vender()` garantem que o número de ações compradas ou vendidas não seja um número negativo. Além disso, se o usuário tentar vender mais ações do que tem, a função `vender()` encerra a transação. A técnica de tornar os dados privados e limitar o acesso a funções públicas dá a você controle sobre como os dados podem ser usados; neste caso, permite inserir essas salvaguardas contra transações defeituosas.

Quatro das funções membros definem ou redefinem o valor do membro `val_total`. Em vez de escrever este cálculo quatro vezes, a classe tem cada função chamando a função `def_total()`. Como essa função é meramente um meio de implementar o código e não parte da interface pública, a classe torna `def_total()` uma função membro privada. Se o cálculo fosse longo, isso poderia economizar alguma digitação e espaço de código. Aqui, entretanto, o principal valor é que, ao usar uma chamada de função em vez de redigitar o cálculo cada vez, você garante que exatamente o mesmo cálculo seja feito. Além disso, se você tiver que revisar o cálculo (o que não é provável neste caso particular), você tem que revisá-lo em apenas um local.

### Métodos Inline (Inline Methods)

Qualquer função com uma definição na declaração de classe torna-se automaticamente uma função inline. Assim, `Acao::def_total()` é uma função inline. Declarações de classe muitas vezes usam funções inline para funções membros curtas, e `def_total()` se qualifica com base nisso.

Você pode, se quiser, definir uma função membro fora da declaração de classe e ainda torná-la inline. Para fazer isso, você usa o qualificador `inline` quando define a função na seção de implementação da classe:

```cpp
class Acao
{
private:
    // ...
    void def_total();    // definição mantida separada
public:
    // ...
};
inline void Acao::def_total()  // use inline na definição
{
    val_total = num_acoes * val_acao;
}
```

As regras especiais para funções inline exigem que sejam definidas em cada arquivo em que são usadas. A maneira mais fácil de garantir que as definições inline estejam disponíveis para todos os arquivos em um programa de múltiplos arquivos é incluir a definição inline no mesmo arquivo de cabeçalho no qual a classe correspondente é definida.

Incidentalmente, de acordo com a regra de reescrita, definir um método dentro de uma declaração de classe é equivalente a substituir a definição do método por um protótipo e depois reescrever a definição como uma função inline imediatamente após a declaração da classe. Ou seja, a definição inline original de `def_total()` na Listagem 10.1 é equivalente à mostrada, com a definição seguindo a declaração da classe.

### Qual Objeto um Método Usa? (Which Object Does a Method Use?)

Agora chegamos a um dos aspectos mais importantes do uso de objetos: como você aplica um método de classe a um objeto. Código como este usa o membro `num_acoes` de um objeto:

```cpp
num_acoes += num;
```

Mas de qual objeto? Essa é uma excelente pergunta! Para respondê-la, primeiro considere como você cria um objeto. A maneira mais simples é declarar variáveis de classe:

```cpp
Acao ana, jose;
```

Isso cria dois objetos da classe `Acao`, um chamado `ana` e um chamado `jose`.

Em seguida, considere como usar uma função membro com um desses objetos. A resposta, como com estruturas e membros de estrutura, é usar o operador de associação (`.`):

```cpp
ana.exibir();   // o objeto ana chama a função membro
jose.exibir();  // o objeto jose chama a função membro
```

A primeira chamada aqui invoca `exibir()` como membro do objeto `ana`. Isso significa que o método interpreta `num_acoes` como `ana.num_acoes` e `val_acao` como `ana.val_acao`. Da mesma forma, a chamada `jose.exibir()` faz o método `exibir()` interpretar `num_acoes` e `val_acao` como `jose.num_acoes` e `jose.val_acao`, respectivamente.

> **Nota:** Quando você chama uma função membro, ela usa os membros de dados do objeto específico usado para invocar a função membro.

Cada novo objeto que você cria contém armazenamento para suas próprias variáveis internas, os membros da classe. Mas todos os objetos da mesma classe compartilham o mesmo conjunto de métodos de classe, com apenas uma cópia de cada método. Suponha, por exemplo, que `ana` e `jose` são objetos `Acao`. Nesse caso, `ana.num_acoes` ocupa um bloco de memória, e `jose.num_acoes` ocupa um segundo bloco de memória. Mas `ana.exibir()` e `jose.exibir()` ambos invocam o mesmo método — ou seja, ambos executam o mesmo bloco de código. Eles apenas aplicam o código a diferentes dados. Chamar uma função membro é o que algumas linguagens OOP chamam de enviar uma mensagem. Assim, enviar a mesma mensagem a dois objetos diferentes invoca o mesmo método, mas o aplica a dois objetos diferentes.

## Usando Classes (Using Classes)

Neste capítulo você viu como definir uma classe e seus métodos de classe. O próximo passo é produzir um programa que cria e usa objetos de uma classe. O objetivo do C++ é tornar o uso de classes o mais semelhante possível ao uso de tipos básicos embutidos, como `int` e `char`. Você pode criar um objeto de classe declarando uma variável de classe ou usando `new` para alocar um objeto de um tipo de classe. Você pode passar objetos como argumentos, retorná-los como valores de retorno de função e atribuir um objeto a outro. A Listagem 10.3 fornece um programa para usar os arquivos de interface e implementação. Ele cria um objeto `Acao` chamado `gato_fofo`. O programa é simples, mas testa os recursos incorporados na classe. Para compilar o programa completo, use as técnicas para programas de múltiplos arquivos descritas no Capítulo 1 e no Capítulo 9. Em particular, compile-o com `acao00.cpp` e tenha `acao00.h` presente no mesmo diretório ou pasta.

**Listagem 10.3** usaacao0.cpp

```cpp
// usaacao0.cpp -- o programa cliente
// compile com acao00.cpp
#include <iostream>
#include "acao00.h"
int main()
{
    Acao gato_fofo;
    gato_fofo.adquirir("NanoSmart", 20, 12.50);
    gato_fofo.exibir();
    gato_fofo.comprar(15, 18.125);
    gato_fofo.exibir();
    gato_fofo.vender(400, 20.00);
    gato_fofo.exibir();
    gato_fofo.comprar(300000, 40.125);
    gato_fofo.exibir();
    gato_fofo.vender(300000, 0.125);
    gato_fofo.exibir();
    return 0;
}
```

Aqui está a saída do programa na Listagem 10.3:

```
Empresa: NanoSmart Acoes: 20
 Preco por Acao: R$12.5 Valor Total: R$250
Empresa: NanoSmart Acoes: 35
 Preco por Acao: R$18.125 Valor Total: R$634.375
Voce nao pode vender mais do que tem! Transacao cancelada.
Empresa: NanoSmart Acoes: 35
 Preco por Acao: R$18.125 Valor Total: R$634.375
Empresa: NanoSmart Acoes: 300035
 Preco por Acao: R$40.125 Valor Total: R$1.20389e+007
Empresa: NanoSmart Acoes: 35
 Preco por Acao: R$0.125 Valor Total: R$4.375
```

Observe que `main()` é apenas um veículo para testar o design da classe `Acao`. Quando a classe `Acao` funcionar como você deseja, pode usá-la como um tipo definido pelo usuário em outros programas. O ponto crítico ao usar o novo tipo é entender o que as funções membros fazem; você não deve ter que pensar nos detalhes de implementação.

> **O Modelo Cliente/Servidor**
>
> Programadores OOP frequentemente discutem o design do programa em termos de um modelo cliente/servidor. Nessa conceituação, o *cliente* é um programa que usa a classe. A declaração da classe, incluindo os métodos da classe, constitui o *servidor*, que é um recurso disponível para os programas que precisam dele. O cliente usa o servidor apenas através da interface pública definida. Isso significa que a única responsabilidade do cliente, e, por extensão, a única responsabilidade do programador do cliente, é conhecer essa interface. A responsabilidade do servidor, e, por extensão, a responsabilidade do designer do servidor, é ver que o servidor realiza de forma confiável e precisa de acordo com essa interface. Quaisquer alterações que o designer do servidor faça no design da classe devem ser detalhes de implementação, não na interface.

### Alterando a Implementação (Changing the Implementation)

Pode haver algo na saída do programa, além da falta de perspicácia financeira de Gato Fofo, que o incomoda — a formatação um tanto inconsistente dos números. Aqui está uma oportunidade de refinar uma implementação mantendo a interface inalterada. A classe `ostream` inclui funções membros que controlam a formatação. Sem entrar em muitos detalhes, você pode evitar a notação-e usando o método `setf()` muito da forma como fizemos na Listagem 8.8:

```cpp
std::cout.setf(std::ios_base::fixed, std::ios_base::floatfield);
```

Isso define um flag no objeto `cout` instruindo-o a usar notação de ponto fixo. Da mesma forma, a seguinte instrução faz `cout` mostrar três casas à direita do decimal ao usar notação de ponto fixo:

```cpp
std::cout.precision(3);
```

Essas ferramentas podem ser usadas no método `exibir()` para controlar a formatação, mas há outro ponto a considerar. Quando você muda a implementação de um método, as alterações não devem afetar outras partes do programa cliente. As mudanças de formato mencionadas permanecem em vigor até serem alteradas novamente, portanto poderiam afetar a saída subsequente no programa cliente. Portanto, o correto para `exibir()` é redefinir as informações de formatação para o estado que existia antes de `exibir()` ser chamado. Isso pode ser feito, como na Listagem 8.8, usando valores de retorno para as instruções de configuração:

```cpp
std::streamsize prec =
    std::cout.precision(3);  // salva o valor anterior de precision
// ...
std::cout.precision(prec);   // redefine para o valor antigo
// armazena os flags originais
std::ios_base::fmtflags orig = std::cout.setf(std::ios_base::fixed);
// ...
// redefine para os valores armazenados
std::cout.setf(orig, std::ios_base::floatfield);
```

Colocando essas informações em uso, podemos substituir a definição de `exibir()` no arquivo de implementação por esta:

```cpp
void Acao::exibir()
{
    using std::cout;
    using std::ios_base;
    // define formato para #.###
    ios_base::fmtflags orig =
        cout.setf(ios_base::fixed, ios_base::floatfield);
    std::streamsize prec = cout.precision(3);
    cout << "Empresa: " << empresa
         << " Acoes: " << num_acoes << '\n';
    cout << " Preco por Acao: R$" << val_acao;
    // define formato para #.##
    cout.precision(2);
    cout << " Valor Total: R$" << val_total << '\n';
    // restaura o formato original
    cout.setf(orig, ios_base::floatfield);
    cout.precision(prec);
}
```

Após essa substituição e deixando o arquivo de cabeçalho e o arquivo cliente inalterados, você pode recompilar o programa. Agora a saída ficaria assim:

```
Empresa: NanoSmart Acoes: 20
 Preco por Acao: R$12.500 Valor Total: R$250.00
Empresa: NanoSmart Acoes: 35
 Preco por Acao: R$18.125 Valor Total: R$634.38
Voce nao pode vender mais do que tem! Transacao cancelada.
Empresa: NanoSmart Acoes: 35
 Preco por Acao: R$18.125 Valor Total: R$634.38
Empresa: NanoSmart Acoes: 300035
 Preco por Acao: R$40.125 Valor Total: R$12038904.38
Empresa: NanoSmart Acoes: 35
 Preco por Acao: R$0.125 Valor Total: R$4.38
```

### Revisando Nossa História até Agora (Reviewing Our Story to Date)

O primeiro passo ao especificar um design de classe é fornecer uma declaração de classe. A declaração de classe é modelada após uma declaração de estrutura e pode incluir membros de dados e membros de função. A declaração tem uma seção privada, e membros declarados nessa seção só podem ser acessados através das funções membros. A declaração também tem uma seção pública, e membros declarados lá podem ser acessados diretamente por um programa que usa objetos de classe.

Tipicamente, membros de dados vão para a seção privada e funções membros vão para a seção pública, então uma declaração de classe típica tem este formulário:

```cpp
class NomeClasse
{
private:
    declarações de membros de dados
public:
    protótipos de funções membros
};
```

O conteúdo da seção pública constitui a parte abstrata do design, a interface pública. Encapsular dados na seção privada protege a integridade dos dados e é chamado de ocultação de dados. Assim, usar uma classe é a maneira C++ de facilitar a implementação dos recursos OOP abstração, ocultação de dados e encapsulamento.

O segundo passo ao especificar um design de classe é implementar as funções membros da classe. Você pode usar uma definição de função completa em vez de um protótipo de função na declaração da classe, mas a prática usual, exceto com funções muito breves, é fornecer as definições de função separadamente. Nesse caso, você precisa usar o operador de resolução de escopo para indicar a qual classe uma função membro pertence. Por exemplo, suponha que a classe `Bozo` tenha uma função membro chamada `Resposta()` que retorna um ponteiro para um `char`. O cabeçalho da função seria assim:

```cpp
char * Bozo::Resposta()
```

Em outras palavras, `Resposta()` não é apenas uma função de tipo `char *`; é uma função de tipo `char *` que pertence à classe `Bozo`. O nome completo, ou qualificado, da função é `Bozo::Resposta()`. O nome `Resposta()`, por outro lado, é uma abreviação do nome qualificado — que pode ser usada apenas em certas circunstâncias, como no código para os métodos da classe.

Para criar um objeto, que é um exemplo particular de uma classe, você usa o nome da classe como se fosse um nome de tipo:

```cpp
Bozo bozeta;
```

Isso funciona porque uma classe *é* um tipo definido pelo usuário.

Você invoca uma função membro de classe, ou método, usando um objeto de classe. Você faz isso usando o operador de associação ponto:

```cpp
cout << bozeta.Resposta();
```

---

Navegação: [Anterior](capitulo-09-03-namespaces-resumo.md) | [Índice](README.md) | [Próximo](capitulo-10-02-construtores-destrutores-this.md)
