# Capitulo 4 - Tipos Compostos (Compound Types)
## Parte 4: Estruturas

> Traducao nao oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Navegacao: [Anterior](capitulo-04-03-classe-string.md) | [Indice](README.md) | [Proximo](capitulo-04-05-unioes-enumeracoes.md)

---

## Apresentando Estruturas

Suponha que voce queira armazenar informacoes sobre um jogador de basquete. Voce pode querer armazenar seu nome, salario, altura, peso, media de pontuacao, percentual de lances livres, assistencias e assim por diante. Voce gostaria de ter algum tipo de formulario de dados que pudesse conter todas essas informacoes em uma unica unidade. Um array nao serve. Embora um array possa conter varios itens, cada item tem que ser do mesmo tipo. Ou seja, um array pode conter 20 valores `int` e outro pode conter 10 valores `float`, mas um unico array nao pode armazenar `int`s em alguns elementos e `float`s em outros.

A resposta para o seu desejo (o de armazenar informacoes sobre um jogador de basquete) e a **estrutura** (structure) do C++. Uma estrutura e uma forma de dados mais versatil do que um array porque uma unica estrutura pode conter itens de mais de um tipo de dado. Isso permite unificar a representacao dos dados armazenando todas as informacoes relacionadas ao basquete em uma unica variavel de estrutura. Se voce quiser controlar uma equipe inteira, podera usar um array de estruturas. O tipo estrutura e tambem um degrau para a classe, o baluarte do POO em C++. Aprender um pouco sobre estruturas agora o aproxima do coracao orientado a objetos do C++.

Uma estrutura e um tipo definido pelo usuario, com a declaracao da estrutura servindo para definir as propriedades de dados do tipo. Depois de definir o tipo, voce pode criar variaveis desse tipo. Assim, criar uma estrutura e um processo de duas partes. Primeiro, voce define uma descricao de estrutura que descreve e rotula os diferentes tipos de dados que podem ser armazenados em uma estrutura. Em seguida, voce pode criar variaveis de estrutura, ou de forma mais geral, objetos de dados de estrutura, que seguem o plano da descricao.

Por exemplo, suponha que a empresa Infilaveis Ltda. queira criar um tipo para descrever os membros de sua linha de produtos de infilaveis finos. Em particular, o tipo deve conter o nome do item, seu volume em pes cubicos e seu preco de venda. Aqui esta uma descricao de estrutura que atende a essas necessidades:

```cpp
struct inflavel    // declaracao de estrutura
{
    char nome[20];
    float volume;
    double preco;
};
```

A palavra-chave `struct` indica que o codigo define o layout de uma estrutura. O identificador `inflavel` e o nome, ou tag (etiqueta), para essa forma; isso torna `inflavel` o nome do novo tipo. Assim, voce pode agora criar variaveis do tipo `inflavel` assim como cria variaveis do tipo `char` ou `int`. Em seguida, entre as chaves esta a lista de tipos de dados a serem armazenados na estrutura. Cada item da lista e uma instrucao de declaracao. Voce pode usar qualquer um dos tipos do C++ aqui, incluindo arrays e outras estruturas. Este exemplo usa um array de `char`, adequado para armazenar uma string, um `float` e um `double`. Cada item individual na lista e chamado de **membro de estrutura** (structure member), entao a estrutura `inflavel` tem tres membros. Resumindo, a definicao de estrutura define as caracteristicas de um tipo — neste caso, o tipo `inflavel`.

Depois de definir a estrutura, voce pode criar variaveis desse tipo:

```cpp
inflavel chapeu;          // chapeu e uma variavel de estrutura do tipo inflavel
inflavel almofada_barulho; // variavel do tipo inflavel
inflavel mainframe;        // variavel do tipo inflavel
```

Se voce esta familiarizado com estruturas em C, notara (provavelmente com prazer) que o C++ permite omitir a palavra-chave `struct` ao declarar variaveis de estrutura:

```cpp
struct inflavel ganso; // palavra-chave struct obrigatoria em C
inflavel vicente;      // palavra-chave struct nao obrigatoria em C++
```

Em C++, a etiqueta de estrutura e usada exatamente como o nome de um tipo fundamental. Essa mudanca enfatiza que a declaracao de uma estrutura define um novo tipo. Ela tambem elimina a necessidade de nao esquecer `struct` da lista de erros que causam confusao.

Dado que `chapeu` e do tipo `inflavel`, voce usa o operador de membro (`.`) para acessar membros individuais. Por exemplo, `chapeu.volume` se refere ao membro `volume` da estrutura, e `chapeu.preco` se refere ao membro `preco`. De forma similar, `vicente.preco` e o membro `preco` da variavel `vicente`. Resumindo, os nomes dos membros permitem acessar membros de uma estrutura da mesma forma que os indices permitem acessar elementos de um array. Como o membro `preco` e declarado como tipo `double`, `chapeu.preco` e `vicente.preco` sao ambos equivalentes a variaveis do tipo `double` e podem ser usados de qualquer maneira que uma variavel ordinaria do tipo `double` possa ser usada. Resumindo, `chapeu` e uma estrutura, mas `chapeu.preco` e um `double`. Aliás, o metodo usado para acessar funcoes membro de classe, como `cin.getline()`, tem suas origens no metodo usado para acessar variaveis membro de estrutura, como `vicente.preco`.

## Usando uma Estrutura em um Programa

Agora que cobrimos alguns dos principais recursos das estruturas, e hora de juntar as ideias em um programa que usa estruturas. A Listagem 4.11 ilustra esses pontos sobre uma estrutura e tambem mostra como inicializar uma.

**Listagem 4.11 — structur.cpp**

```cpp
// structur.cpp -- uma estrutura simples
#include <iostream>
struct inflavel    // declaracao de estrutura
{
    char nome[20];
    float volume;
    double preco;
};
int main()
{
    using namespace std;
    inflavel convidado =
    {
        "Gloriosa Gloria",  // valor de nome
        1.88,               // valor de volume
        29.99               // valor de preco
    }; // convidado e uma variavel de estrutura do tipo inflavel
    // ela e inicializada com os valores indicados
    inflavel amigo =
    {
        "Audacioso Arthur",
        3.12,
        32.99
    }; // amigo e uma segunda variavel do tipo inflavel
    // NOTA: algumas implementacoes exigem usar
    // static inflavel convidado =
    cout << "Expanda sua lista de convidados com " << convidado.nome;
    cout << " e " << amigo.nome << "!\n";
    // amigo.nome e o membro nome da variavel amigo
    cout << "Voce pode ter ambos por $";
    cout << convidado.preco + amigo.preco << "!\n";
    return 0;
}
```

Aqui esta a saida do programa na Listagem 4.11:

```
Expanda sua lista de convidados com Gloriosa Gloria e Audacioso Arthur!
Voce pode ter ambos por $62.98!
```

**Notas do Programa**

Uma questao importante relacionada ao programa na Listagem 4.11 e onde colocar a declaracao da estrutura. Ha duas opcoes para `structur.cpp`. Voce poderia colocar a declaracao dentro da funcao `main()`, logo apos a chave de abertura. A segunda opcao, e a escolhida aqui, e coloca-la fora e antes de `main()`. Quando uma declaracao ocorre fora de qualquer funcao, ela e chamada de **declaracao externa** (external declaration). Para este programa, nao ha diferenca pratica entre as duas opcoes. Mas para programas compostos por duas ou mais funcoes, a diferenca pode ser crucial. A declaracao externa pode ser usada por todas as funcoes que a sucedem, enquanto a declaracao interna pode ser usada apenas pela funcao em que a declaracao e encontrada. Na maioria das vezes, voce quer uma declaracao de estrutura externa para que todas as funcoes possam usar estruturas desse tipo.

Em seguida, observe o procedimento de inicializacao:

```cpp
inflavel convidado =
{
    "Gloriosa Gloria", // valor de nome
    1.88,              // valor de volume
    29.99              // valor de preco
};
```

Como com arrays, voce usa uma lista de valores separados por virgulas entre um par de chaves. O programa coloca um valor por linha, mas voce pode coloca-los todos na mesma linha. Basta se lembrar de separar os itens com virgulas:

```cpp
inflavel pato = {"Dafne", 0.12, 9.98};
```

Voce pode inicializar cada membro da estrutura com o tipo de dado adequado. Por exemplo, o membro `nome` e um array de caracteres, portanto voce pode inicializa-lo com uma string.

Cada membro de estrutura e tratado como uma variavel desse tipo. Assim, `amigo.preco` e uma variavel `double`, e `amigo.nome` e um array de `char`. E quando o programa usa `cout` para exibir `amigo.nome`, ele exibe o membro como uma string. Alem disso, como `amigo.nome` e um array de caracteres, podemos usar subscritos para acessar caracteres individuais no array. Por exemplo, `amigo.nome[0]` e o caractere `A`. Mas `amigo[0]` nao tem significado porque `amigo` e uma estrutura, nao um array.

## Inicializacao de Estrutura em C++11

Como com arrays, o C++11 expande os recursos da inicializacao por lista. O sinal `=` e opcional:

```cpp
inflavel pato {"Dafne", 0.12, 9.98}; // pode omitir o = em C++11
```

Em seguida, chaves vazias resultam nos membros individuais sendo definidos como 0. Por exemplo, a declaracao a seguir resulta em `prefeito.volume` e `prefeito.preco` sendo definidos como 0 e todos os bytes em `prefeito.nome` sendo definidos como 0:

```cpp
inflavel prefeito {};
```

Por fim, o estreitamento (narrowing) nao e permitido.

## Uma Estrutura Pode Usar um Membro da Classe string?

Voce pode usar um objeto da classe `string` em vez de um array de caracteres para o membro `nome`? Ou seja, voce pode declarar uma estrutura assim:

```cpp
#include <string>
struct inflavel    // definicao da estrutura
{
    std::string nome;
    float volume;
    double preco;
};
```

A resposta e sim, a menos que voce esteja usando um compilador desatualizado que nao suporte inicializacao de estruturas com membros da classe `string`.

Certifique-se de que a definicao da estrutura tenha acesso ao namespace `std`. Voce pode fazer isso movendo a diretiva `using` para que esteja acima da definicao da estrutura. A escolha melhor, como mostrado anteriormente, e declarar `nome` como do tipo `std::string`.

## Outras Propriedades das Estruturas

O C++ torna os tipos definidos pelo usuario tao similares quanto possivel aos tipos nativos. Por exemplo, voce pode passar estruturas como argumentos para uma funcao, e voce pode ter uma funcao usando uma estrutura como valor de retorno. Alem disso, voce pode usar o operador de atribuicao (`=`) para atribuir uma estrutura a outra do mesmo tipo. Fazer isso faz com que cada membro de uma estrutura seja definido com o valor do membro correspondente na outra estrutura, mesmo que o membro seja um array. Esse tipo de atribuicao e chamado de **atribuicao membro a membro** (memberwise assignment). Adiamos a passagem e retorno de estruturas ate discutirmos funcoes no Capitulo 7, "Funcoes: Os Modulos de Programacao do C++", mas podemos dar uma rapida olhada na atribuicao de estruturas agora. A Listagem 4.12 fornece um exemplo.

**Listagem 4.12 — assgn_st.cpp**

```cpp
// assgn_st.cpp -- atribuindo estruturas
#include <iostream>
struct inflavel
{
    char nome[20];
    float volume;
    double preco;
};
int main()
{
    using namespace std;
    inflavel buque =
    {
        "girassois",
        0.20,
        12.49
    };
    inflavel escolha;
    cout << "buque: " << buque.nome << " por $";
    cout << buque.preco << endl;
    escolha = buque;  // atribui uma estrutura a outra
    cout << "escolha: " << escolha.nome << " por $";
    cout << escolha.preco << endl;
    return 0;
}
```

Aqui esta a saida do programa na Listagem 4.12:

```
buque: girassois por $12.49
escolha: girassois por $12.49
```

Como voce pode ver, a atribuicao membro a membro esta em acao, pois os membros da estrutura `escolha` recebem os mesmos valores armazenados na estrutura `buque`.

Voce pode combinar a definicao de um formulario de estrutura com a criacao de variaveis de estrutura. Para fazer isso, voce segue a chave de fechamento com o nome da variavel ou nomes:

```cpp
struct vantagens
{
    int numero_chave;
    char carro[12];
} sr_silva, sra_jones;  // duas variaveis do tipo vantagens
```

Voce pode ate inicializar uma variavel que criar dessa forma:

```cpp
struct vantagens
{
    int numero_chave;
    char carro[12];
} sr_brilhante =
{
    7,         // valor para o membro sr_brilhante.numero_chave
    "Packard"  // valor para o membro sr_brilhante.carro
};
```

No entanto, manter a definicao da estrutura separada das declaracoes de variaveis geralmente torna um programa mais facil de ler e acompanhar.

Outra coisa que voce pode fazer com estruturas e criar uma estrutura sem nome de tipo. Voce faz isso omitindo um nome de etiqueta ao mesmo tempo que define um formulario de estrutura e uma variavel:

```cpp
struct  // sem etiqueta
{
    int x; // 2 membros
    int y;
} posicao;  // uma variavel de estrutura
```

Isso cria uma variavel de estrutura chamada `posicao`. Voce pode acessar seus membros com o operador de membro, como em `posicao.x`, mas nao ha um nome geral para o tipo. Voce nao pode posteriormente criar outras variaveis do mesmo tipo. Este livro nao usa essa forma limitada de estrutura.

Fora o fato de que um programa C++ pode usar a etiqueta de estrutura como nome de tipo, as estruturas do C tem todos os recursos discutidos ate agora para as estruturas do C++, exceto as mudancas do C++11. Mas as estruturas do C++ vao alem. Ao contrario das estruturas do C, as estruturas do C++ podem ter funcoes membro alem das variaveis membro. Mas esses recursos mais avancados sao usados tipicamente com classes em vez de estruturas, portanto os discutiremos quando abordarmos classes, a partir do Capitulo 10, "Objetos e Classes".

## Arrays de Estruturas

A estrutura `inflavel` contem um array (o array `nome`). Tambem e possivel criar arrays cujos elementos sao estruturas. A tecnica e exatamente a mesma para criar arrays dos tipos fundamentais. Por exemplo, para criar um array de 100 estruturas `inflavel`, voce poderia fazer o seguinte:

```cpp
inflavel presentes[100]; // array de 100 estruturas inflavel
```

Isso torna `presentes` um array de `inflavel`s. Portanto, cada elemento do array, como `presentes[0]` ou `presentes[99]`, e um objeto `inflavel` e pode ser usado com o operador de membro:

```cpp
cin >> presentes[0].volume; // usa membro volume do primeiro struct
cout << presentes[99].preco << endl; // exibe membro preco do ultimo struct
```

Tenha em mente que `presentes` em si e um array, nao uma estrutura, portanto construcoes como `presentes.preco` nao sao validas.

Para inicializar um array de estruturas, voce combina a regra para inicializar arrays (uma lista separada por virgulas de valores entre chaves para cada elemento) com a regra para estruturas (uma lista separada por virgulas de valores entre chaves para cada membro). Como cada elemento do array e uma estrutura, seu valor e representado por uma inicializacao de estrutura. Assim, voce acaba com uma lista separada por virgulas de valores entre chaves, cada um dos quais sendo uma lista separada por virgulas de valores entre chaves:

```cpp
inflavel convidados[2] =  // inicializando um array de structs
{
    {"Bambi", 0.5, 21.99},     // primeira estrutura no array
    {"Godzilla", 2000, 565.99} // proxima estrutura no array
};
```

Como de costume, voce pode formatar isso como quiser. Por exemplo, ambas as inicializacoes podem estar na mesma linha, ou cada inicializacao de membro de estrutura separada pode ter sua propria linha.

A Listagem 4.13 mostra um exemplo curto que usa um array de estruturas. Note que como `convidados` e um array de `inflavel`, `convidados[0]` e do tipo `inflavel`, portanto voce pode usar o operador ponto para acessar um membro da estrutura `inflavel`.

**Listagem 4.13 — arrstruc.cpp**

```cpp
// arrstruc.cpp -- um array de estruturas
#include <iostream>
struct inflavel
{
    char nome[20];
    float volume;
    double preco;
};
int main()
{
    using namespace std;
    inflavel convidados[2] =  // inicializando um array de structs
    {
        {"Bambi", 0.5, 21.99},     // primeira estrutura no array
        {"Godzilla", 2000, 565.99} // proxima estrutura no array
    };
    cout << "Os convidados " << convidados[0].nome << " e " << convidados[1].nome
         << "\ntem um volume combinado de "
         << convidados[0].volume + convidados[1].volume << " pes cubicos.\n";
    return 0;
}
```

Aqui esta a saida do programa na Listagem 4.13:

```
Os convidados Bambi e Godzilla
tem um volume combinado de 2000.5 pes cubicos.
```

## Campos de Bits em Estruturas

O C++, como o C, permite especificar membros de estrutura que ocupam um numero especifico de bits. Isso pode ser util para criar uma estrutura de dados que corresponda, digamos, a um registrador em algum dispositivo de hardware. O tipo de campo deve ser um tipo inteiro ou de enumeracao (as enumeracoes sao discutidas mais adiante neste capitulo), e um dois-pontos seguido de um numero indica o numero real de bits a serem usados. Voce pode usar campos sem nome para fornecer espaco. Cada membro e chamado de **campo de bits** (bit field). Aqui esta um exemplo:

```cpp
struct registrador_torgle
{
    unsigned int NS : 4;        // 4 bits para o valor NS
    unsigned int : 4;           // 4 bits nao utilizados
    bool entradaValida : 1;     // entrada valida (1 bit)
    bool torgleOk : 1;          // torgle bem-sucedido
};
```

Voce pode inicializar os campos da maneira usual, e usa a notacao de estrutura padrao para acessar campos de bits:

```cpp
registrador_torgle tr = {14, true, false};
...
if (tr.entradaValida) // instrucao if abordada no Capitulo 6
...
```

Campos de bits sao tipicamente usados em programacao de baixo nivel. Muitas vezes, usar um tipo inteiro e os operadores bit a bit listados no Apendice E, "Outros Operadores", fornece uma abordagem alternativa.

---

> Navegacao: [Anterior](capitulo-04-03-classe-string.md) | [Indice](README.md) | [Proximo](capitulo-04-05-unioes-enumeracoes.md)
