# Capítulo 12 — Classes e Alocação Dinâmica de Memória (Parte 2: new em Construtores, Retorno de Objetos e Placement new)

> Tradução não oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Navegação: [Anterior](capitulo-12-01-alocacao-dinamica.md) | [Índice](README.md) | [Próximo](capitulo-12-03-fila-resumo.md)

---

## O que Lembrar Ao Usar new em Construtores

A esta altura você já percebeu que deve ter cuidado especial ao usar `new` para inicializar membros ponteiro de um objeto. Em particular, você deve fazer o seguinte:

- Se você usar `new` para inicializar um membro ponteiro em um construtor, deve usar `delete` no destrutor.
- Os usos de `new` e `delete` devem ser compatíveis. Você deve parear `new` com `delete` e `new[]` com `delete[]`.
- Se houver múltiplos construtores, todos devem usar `new` da mesma forma — seja todos com colchetes ou todos sem colchetes. Há apenas um destrutor, portanto todos os construtores devem ser compatíveis com aquele destrutor. Porém, é permitido inicializar um ponteiro com `new` em um construtor e com o ponteiro nulo (`0`, ou, com C++11, `nullptr`) em outro construtor, porque é correto aplicar a operação `delete` (com ou sem colchetes) ao ponteiro nulo.

> **`NULL` ou `0` ou `nullptr`?**
> Historicamente, o ponteiro nulo pode ser representado por `0` ou por `NULL`, uma constante simbólica definida como `0` em vários arquivos de cabeçalho. Programadores C frequentemente usam `NULL` em vez de `0` como um lembrete visual de que o valor é um valor de ponteiro, assim como usam `'\0'` em vez de `0` para o caractere nulo como um lembrete visual de que esse valor é um caractere. A tradição do C++, porém, favoreceu um simples `0` em vez do `NULL` equivalente. E, como mencionado anteriormente, C++11 oferece a palavra-chave `nullptr` como uma alternativa melhor.

- Você deve definir um construtor de cópia que inicialize um objeto a partir de outro, fazendo uma cópia profunda. Tipicamente, o construtor deve emular o exemplo a seguir:

```cpp
String::String(const String & st)
{
    num_cadeias++; // cuida da atualização do membro estático, se necessário
    tam = st.tam;  // mesmo comprimento que a string copiada
    cad = new char [tam + 1]; // aloca espaço
    std::strcpy(cad, st.cad); // copia a string para o novo local
}
```

Em particular, o construtor de cópia deve alocar espaço para armazenar os dados copiados e deve copiar os dados, não apenas o endereço dos dados. Além disso, deve atualizar quaisquer membros de classe estáticos cujo valor seria afetado pelo processo.

- Você deve definir um operador de atribuição que copie um objeto para outro fazendo uma cópia profunda. Tipicamente, o método de classe deve emular o exemplo a seguir:

```cpp
String & String::operator=(const String & st)
{
    if (this == &st) // objeto atribuído a si mesmo
        return *this; // pronto
    delete [] cad;    // libera a string antiga
    tam = st.tam;
    cad = new char [tam + 1]; // obtém espaço para a nova string
    std::strcpy(cad, st.cad); // copia a string
    return *this;     // retorna referência para o objeto invocante
}
```

Em particular, o método deve verificar a autoatribuição; deve liberar a memória anteriormente apontada pelo ponteiro membro; deve copiar os dados, não apenas o endereço dos dados; e deve retornar uma referência ao objeto invocante.

### Proibições e Práticas Corretas

O seguinte trecho contém dois exemplos do que não fazer e um exemplo de um construtor correto:

```cpp
String::String()
{
    cad = "string padrão"; // ops, sem new[]
    tam = std::strlen(cad);
}
String::String(const char * s)
{
    tam = std::strlen(s);
    cad = new char; // ops, sem []
    std::strcpy(cad, s); // ops, sem espaço suficiente
}
String::String(const String & st)
{
    tam = st.tam;
    cad = new char[tam + 1]; // bom, aloca espaço
    std::strcpy(cad, st.cad); // bom, copia o valor
}
```

O primeiro construtor não usa `new` para inicializar `cad`. O destrutor, quando chamado para um objeto padrão, aplica `delete` a `cad`. O resultado de aplicar `delete` a um ponteiro não inicializado por `new` é indefinido, mas provavelmente será ruim. Qualquer um dos seguintes estaria correto:

```cpp
String::String()
{
    tam = 0;
    cad = new char[1]; // usa new com []
    cad[0] = '\0';
}
```

```cpp
String::String()
{
    tam = 0;
    cad = 0; // ou, com C++11, cad = nullptr;
}
```

```cpp
String::String()
{
    static const char * s = "C++"; // inicializado apenas uma vez
    tam = std::strlen(s);
    cad = new char[tam + 1]; // usa new com []
    std::strcpy(cad, s);
}
```

A seguir, o segundo construtor no trecho original aplica `new`, mas não solicita a quantidade correta de memória; portanto, `new` retorna um bloco contendo espaço para apenas um caractere. Tentar copiar uma string mais longa para aquele local está pedindo por problemas de memória. Além disso, o uso de `new` sem colchetes é inconsistente com a forma correta dos outros construtores.

O terceiro construtor está correto.

Finalmente, aqui está um destrutor que não funciona corretamente com os construtores anteriores:

```cpp
String::~String()
{
    delete cad; // ops, deveria ser delete [] cad;
}
```

O destrutor usa `delete` incorretamente. Como os construtores solicitam arrays de caracteres, o destrutor deve deletar um array.

### Cópia Membro a Membro para Classes com Membros de Classe

Suponha que você use a classe `String`, ou, de fato, a classe `string` padrão, como um tipo para membros de classe:

```cpp
class Revista
{
private:
    String titulo;
    string editora;
...
};
```

Tanto `String` quanto `string` usam alocação dinâmica de memória. Isso significa que você precisa escrever um construtor de cópia e um operador de atribuição para a classe `Revista`? Não — pelo menos não por si só. O comportamento padrão de cópia e atribuição membro a membro tem alguma inteligência. Se você copiar ou atribuir um objeto `Revista` para outro, a cópia membro a membro usa os construtores de cópia e operadores de atribuição definidos para os tipos membros. Ou seja, o construtor de cópia de `String` será usado para copiar o membro `titulo` de um objeto `Revista` para outro, o operador de atribuição de `String` será usado para atribuir o membro `titulo` de um objeto `Revista` a outro, e assim por diante. As coisas ficam mais complicadas, porém, se a classe `Revista` precisar de um construtor de cópia e um operador de atribuição para algum outro membro de classe. Nesse caso, essas funções teriam que chamar explicitamente os construtores de cópia e operadores de atribuição de `String` e `string`. Mas isso é um conto para o Capítulo 13, "Herança de Classes".

## Observações Sobre o Retorno de Objetos

Quando uma função membro ou função independente retorna um objeto, você tem escolhas. A função pode retornar uma referência a um objeto, uma referência constante a um objeto, um objeto ou um objeto constante. Agora que você viu exemplos de todos exceto o último, é um bom momento para rever essas opções.

### Retornando uma Referência a um Objeto const

A razão usual para usar uma referência `const` é a eficiência, mas há restrições sobre quando essa escolha pode ser usada. Se uma função retorna um objeto que lhe é passado, seja por invocação de objeto ou como argumento de método, você pode aumentar a eficiência do método tendo-o retornar uma referência. Por exemplo, suponha que você queira escrever uma função `Max()` que retornasse o maior de dois objetos `Vetor`, onde `Vetor` é a classe desenvolvida no Capítulo 11. A função seria usada desta maneira:

```cpp
Vetor forca1(50,60);
Vetor forca2(10,70);
Vetor maximo;
maximo = Max(forca1, forca2);
```

Qualquer das duas implementações a seguir funcionaria:

```cpp
// versão 1
Vetor Max(const Vetor & v1, const Vetor & v2)
{
    if (v1.valmag() > v2.valmag())
        return v1;
    else
        return v2;
}
// versão 2
const Vetor & Max(const Vetor & v1, const Vetor & v2)
{
    if (v1.valmag() > v2.valmag())
        return v1;
    else
        return v2;
}
```

Há três pontos importantes aqui. Primeiro, lembre-se de que retornar um objeto invoca o construtor de cópia, enquanto que retornar uma referência não. Assim, a Versão 2 faz menos trabalho e é mais eficiente. Segundo, a referência deve ser para um objeto que existe quando a função chamadora está em execução. Neste exemplo, a referência é para `forca1` ou `forca2`, e ambos são objetos definidos na função chamadora, portanto esse requisito é atendido. Terceiro, tanto `v1` quanto `v2` são declarados como referências const, portanto o tipo de retorno deve ser `const` para corresponder.

### Retornando uma Referência a um Objeto Não-const

Dois exemplos comuns de retorno de um objeto não-`const` são a sobrecarga do operador de atribuição e a sobrecarga do operador `<<` para uso com `cout`. O primeiro é feito por razões de eficiência, e o segundo por razões de necessidade.

O valor de retorno de `operator=()` é usado para atribuição encadeada:

```cpp
String s1("Coisa boa");
String s2, s3;
s3 = s2 = s1;
```

Neste código, o valor de retorno de `s2.operator=(s1)` é atribuído a `s3`. Retornar um objeto `String` ou uma referência a um objeto `String` funcionaria, mas, como no exemplo do `Vetor`, usar uma referência permite que a função evite chamar o construtor de cópia de `String` para criar um novo objeto `String`. Neste caso, o tipo de retorno não é `const` porque o método `operator=()` retorna uma referência a `s2`, que ele modifica.

O valor de retorno de `operator<<()` é usado para saída encadeada:

```cpp
String s1("Coisa boa");
cout << s1 << " está chegando!";
```

Aqui, o valor de retorno de `operator<<(cout, s1)` torna-se o objeto usado para exibir a string `"está chegando!"`. Aqui, o tipo de retorno deve ser `ostream &` e não simplesmente `ostream`. Usar um tipo de retorno `ostream` exigiria chamar o construtor de cópia de `ostream`, e, como acontece, a classe `ostream` não tem um construtor de cópia público. Felizmente, retornar uma referência a `cout` não apresenta nenhum problema porque `cout` já está em escopo na função chamadora.

### Retornando um Objeto

Se o objeto retornado é local à função chamada, então ele não deve ser retornado por referência porque o objeto local tem seu destrutor chamado quando a função termina. Assim, quando o controle retorna à função chamadora, não há objeto ao qual a referência possa se referir. Nessas circunstâncias, você deve retornar um objeto, não uma referência. Tipicamente, operadores aritméticos sobrecarregados se enquadram nesta categoria. Considere este exemplo, que usa a classe `Vetor` novamente:

```cpp
Vetor forca1(50,60);
Vetor forca2(10,70);
Vetor rede;
rede = forca1 + forca2;
```

O valor retornado não é `forca1`, que deve ficar inalterado pelo processo, nem `forca2`, que também deve ficar inalterado. Portanto, o valor de retorno não pode ser uma referência a um objeto que já está presente na função chamadora. Em vez disso, a soma é um novo objeto temporário computado em `Vetor::operator+()`, e a função não deve retornar uma referência a um objeto temporário também. Em vez disso, deve retornar um objeto de vetor real, não uma referência:

```cpp
Vetor Vetor::operator+(const Vetor & b) const
{
    return Vetor(x + b.x, y + b.y);
}
```

Há a despesa adicional de chamar o construtor de cópia para criar o objeto retornado, mas isso é inevitável.

### Retornando um Objeto const

A definição anterior de `Vetor::operator+()` tem uma propriedade bizarra. O uso pretendido é este:

```cpp
rede = forca1 + forca2;   // 1: três objetos Vetor
```

Porém, a definição também permite que você use o seguinte:

```cpp
forca1 + forca2 = rede;   // 2: programação dislética
cout << (forca1 + forca2 = rede).valmag() << endl; // 3: programação demente
```

Três perguntas surgem imediatamente. Por que alguém escreveria tais instruções? Por que elas são possíveis? O que elas fazem?

Primeiro, não há razão sensata para escrever tal código, mas nem todo código é escrito por razões sensatas. Pessoas, mesmo programadores, cometem erros. Por exemplo, se o operador de comparação `==()` fosse definido para a classe `Vetor`, você poderia erroneamente digitar

```cpp
if (forca1 + forca2 = rede)
```

em vez de isto:

```cpp
if (forca1 + forca2 == rede)
```

Segundo, esse código é possível porque o construtor de cópia constrói um objeto temporário para representar o valor de retorno. Assim, no código anterior, a expressão `forca1 + forca2` representa aquele objeto temporário. Na Instrução 1, o objeto temporário é atribuído a `rede`. Nas Instruções 2 e 3, `rede` é atribuída ao objeto temporário.

Terceiro, o objeto temporário é usado e então descartado. Por exemplo, na Instrução 2, o programa computa a soma de `forca1` e `forca2`, copia a resposta no objeto de retorno temporário, sobrescreve o conteúdo com o conteúdo de `rede`, e então descarta o objeto temporário. Os vetores originais ficam todos inalterados.

Se você está preocupado com o potencial para uso indevido criado por esse comportamento, tem um recurso simples: declarar o tipo de retorno como um objeto `const`. Por exemplo, se `Vetor::operator+()` é declarado para ter o tipo de retorno `const Vetor`, então a Instrução 1 ainda é permitida, mas as Instruções 2 e 3 tornam-se inválidas.

Em resumo, se um método ou função retorna um objeto local, ele deve retornar um objeto, não uma referência. Neste exemplo, o programa usa o construtor de cópia para gerar o objeto retornado. Se um método ou função retorna um objeto de uma classe para a qual não há construtor de cópia público, como a classe `ostream`, ele deve retornar uma referência a um objeto. Finalmente, alguns métodos e funções, como o operador de atribuição sobrecarregado, podem retornar um objeto ou uma referência a um objeto. Neste exemplo, a referência é preferida por razões de eficiência.

## Usando Ponteiros para Objetos

Programas C++ frequentemente usam ponteiros para objetos, então vamos praticar um pouco. A Listagem 12.6 usa valores de índice de array para acompanhar a string mais curta e a primeira em ordem alfabética. Outra abordagem é usar ponteiros para apontar para os líderes atuais nessas categorias. A Listagem 12.7 implementa essa abordagem, usando dois ponteiros para `String`. Inicialmente, o ponteiro `menor` aponta para o primeiro objeto no array. Cada vez que o programa encontra um objeto com uma string mais curta, ele redefine `menor` para apontar para aquele objeto. Da mesma forma, um ponteiro `primeiro` rastreia a string mais inicial em ordem alfabética. Note que esses dois ponteiros *não* criam novos objetos; eles meramente apontam para objetos existentes. Portanto, eles não requerem o uso de `new` para alocar memória adicional.

Para variedade, o programa na Listagem 12.7 usa um ponteiro que de fato controla um novo objeto:

```cpp
String * favorito = new String(dizeres[escolha]);
```

Aqui o ponteiro `favorito` fornece o único acesso ao objeto sem nome criado por `new`. Esta sintaxe particular significa inicializar o novo objeto `String` usando o objeto `dizeres[escolha]`. Isso invoca o construtor de cópia porque o tipo de argumento para o construtor de cópia (`const String &`) corresponde ao valor de inicialização (`dizeres[escolha]`). O programa usa `srand()`, `rand()` e `time()` para selecionar um valor para `escolha` aleatoriamente.

### Listagem 12.7 — sayings2.cpp

```cpp
// sayings2.cpp -- usando ponteiros para objetos
// compilar com string1.cpp
#include <iostream>
#include <cstdlib> // (ou stdlib.h) para rand(), srand()
#include <ctime>   // (ou time.h) para time()
#include "string1.h"
const int TamArr = 10;
const int CompMax = 81;
int main()
{
    using namespace std;
    String nome;
    cout << "Oi, qual é o seu nome?\n>> ";
    cin >> nome;
    cout << nome << ", insira até " << TamArr
       << " provérbios curtos <linha vazia para sair>:\n";
    String dizeres[TamArr];
    char temp[CompMax]; // armazenamento temporário de string
    int i;
    for (i = 0; i < TamArr; i++)
    {
        cout << i+1 << ": ";
        cin.get(temp, CompMax);
        while (cin && cin.get() != '\n')
            continue;
        if (!cin || temp[0] == '\0') // linha vazia?
            break; // i não é incrementado
        else
            dizeres[i] = temp; // atribuição sobrecarregada
    }
    int total = i; // número total de linhas lidas
    if (total > 0)
    {
        cout << "Aqui estão seus provérbios:\n";
        for (i = 0; i < total; i++)
            cout << dizeres[i] << "\n";
    // usa ponteiros para rastrear strings menor e primeira
        String * menor = &dizeres[0]; // inicializa para o primeiro objeto
        String * primeiro = &dizeres[0];
        for (i = 1; i < total; i++)
        {
            if (dizeres[i].comprimento() < menor->comprimento())
                menor = &dizeres[i];
            if (dizeres[i] < *primeiro) // compara valores
                primeiro = &dizeres[i]; // atribui endereço
        }
        cout << "Provérbio mais curto:\n" << *menor << endl;
        cout << "Primeiro em ordem alfabética:\n" << *primeiro << endl;
        srand(time(0));
        int escolha = rand() % total; // escolhe índice aleatoriamente
    // usa new para criar e inicializar novo objeto String
        String * favorito = new String(dizeres[escolha]);
        cout << "Meu provérbio favorito:\n" << *favorito << endl;
        delete favorito;
    }
    else
        cout << "Pouco a dizer, hein?\n";
    cout << "Até logo.\n";
    return 0;
}
```

> **Inicialização de Objetos com new**
> Em geral, se `NomeClasse` é uma classe e se `valor` é do tipo `NomeTipo`, a instrução
> ```cpp
> NomeClasse * pclasse = new NomeClasse(valor);
> ```
> invoca este construtor:
> ```cpp
> NomeClasse(NomeTipo);
> ```
> Pode haver conversões triviais, como para este:
> ```cpp
> NomeClasse(const NomeTipo &);
> ```
> Além disso, as conversões usuais invocadas por correspondência de protótipos, como de `int` para `double`, ocorrem desde que não haja ambiguidade. Uma inicialização na forma a seguir invoca o construtor padrão:
> ```cpp
> NomeClasse * ptr = new NomeClasse;
> ```

Aqui está uma execução de amostra do programa na Listagem 12.7:

```
Oi, qual é o seu nome?
>> Kirt Rood
Kirt Rood, insira até 10 provérbios curtos <linha vazia para sair>:
1: a friend in need is a friend indeed
2: neither a borrower nor a lender be
3: a stitch in time saves nine
4: a niche in time saves stine
5: it takes a crook to catch a crook
6: cold hands, warm heart
7:
Aqui estão seus provérbios:
a friend in need is a friend indeed
neither a borrower nor a lender be
a stitch in time saves nine
a niche in time saves stine
it takes a crook to catch a crook
cold hands, warm heart
Provérbio mais curto:
cold hands, warm heart
Primeiro em ordem alfabética:
a friend in need is a friend indeed
Meu provérbio favorito:
a stitch in time saves nine
Até logo.
```

Como o programa seleciona o provérbio favorito aleatoriamente, diferentes execuções do programa mostrarão escolhas diferentes, mesmo para entrada idêntica.

### Examinando Novamente new e delete

Note que o programa gerado a partir das Listagens 12.4, 12.5 e 12.7 usa `new` e `delete` em dois níveis. Primeiro, ele usa `new` para alocar espaço de armazenamento para as strings de nome de cada objeto criado. Isso acontece nas funções construtoras, então a função destrutor usa `delete` para liberar aquela memória. Como cada string é um array de caracteres, o destrutor usa `delete` com colchetes. Assim, a memória usada para armazenar os conteúdos das strings é liberada automaticamente quando um objeto é destruído. Segundo, o código na Listagem 12.7 usa `new` para alocar um objeto inteiro:

```cpp
String * favorito = new String(dizeres[escolha]);
```

Isso aloca espaço não para a string a ser armazenada, mas para o objeto — ou seja, para o ponteiro `cad` que mantém o endereço da string e para o membro `tam`. (Ele não aloca espaço para o membro `num_cadeias` porque ele é um membro estático armazenado separadamente dos objetos.) Criar o objeto, por sua vez, chama o construtor, que aloca espaço para armazenar a string e atribui o endereço da string a `cad`. O programa então usa `delete` para deletar esse objeto quando termina com ele. O objeto é um único objeto, então o programa usa `delete` sem colchetes. Novamente, isso libera apenas o espaço usado para armazenar o ponteiro `cad` e o membro `tam`. Não libera a memória usada para armazenar a string para a qual `cad` aponta, mas o destrutor cuida dessa tarefa final.

Novamente, os destrutores são chamados nas seguintes situações:

- Se um objeto é uma variável automática, o destrutor do objeto é chamado quando o programa sai do bloco no qual o objeto é definido.
- Se um objeto é uma variável estática (externa, estática, estática externa ou de um namespace), seu destrutor é chamado quando o programa termina.
- Se um objeto é criado por `new`, seu destrutor é chamado apenas quando você usa explicitamente `delete` no objeto.

### Resumo de Ponteiros e Objetos

Você deve observar vários pontos sobre o uso de ponteiros para objetos:

- Você declara um ponteiro para um objeto usando a notação usual:
  ```cpp
  String * glamour;
  ```
- Você pode inicializar um ponteiro para apontar para um objeto existente:
  ```cpp
  String * primeiro = &dizeres[0];
  ```
- Você pode inicializar um ponteiro usando `new`. O seguinte cria um novo objeto:
  ```cpp
  String * favorito = new String(dizeres[escolha]);
  ```
- Usar `new` com uma classe invoca o construtor de classe apropriado para inicializar o objeto recém-criado:
  ```cpp
  // invoca o construtor padrão
  String * gleep = new String;
  // invoca o construtor String(const char *)
  String * glop = new String("meu meu meu");
  // invoca o construtor String(const String &)
  String * favorito = new String(dizeres[escolha]);
  ```
- Você usa o operador `->` para acessar um método de classe via ponteiro:
  ```cpp
  if (dizeres[i].comprimento() < menor->comprimento())
  ```
- Você aplica o operador de desreferenciamento (`*`) a um ponteiro para um objeto para obter um objeto:
  ```cpp
  if (dizeres[i] < *primeiro) // compara valores de objeto
      primeiro = &dizeres[i]; // atribui endereço do objeto
  ```

## Examinando Novamente o Placement new

Lembre-se de que o `placement new` permite especificar o local de memória usado para alocar memória. O Capítulo 9, "Modelos de Memória e Namespaces", discute o `placement new` no contexto de tipos embutidos. Usar `placement new` com objetos adiciona algumas novidades. A Listagem 12.8 usa `placement new` junto com `new` regular para alocar memória para objetos. Ela define uma classe com um construtor e destrutor faladores para que você possa acompanhar a história dos objetos.

### Listagem 12.8 — placenew1.cpp

```cpp
// placenew1.cpp -- new, placement new, sem delete
#include <iostream>
#include <string>
#include <new>
using namespace std;
const int BUF = 512;

class ApenasTeste
{
private:
    string palavras;
    int numero;
public:
    ApenasTeste(const string & s = "Apenas Testando", int n = 0)
    {palavras = s; numero = n; cout << palavras << " construído\n"; }
    ~ApenasTeste() { cout << palavras << " destruído\n";}
    void Exibir() const { cout << palavras << ", " << numero << endl;}
};
int main()
{
    char * buffer = new char[BUF]; // obtém um bloco de memória
    ApenasTeste *pc1, *pc2;
    pc1 = new (buffer) ApenasTeste;     // coloca objeto no buffer
    pc2 = new ApenasTeste("Heap1", 20); // coloca objeto no heap
    cout << "Endereços do bloco de memória:\n" << "buffer: "
       << (void *) buffer << " heap: " << pc2 << endl;
    cout << "Conteúdo da memória:\n";
    cout << pc1 << ": ";
    pc1->Exibir();
    cout << pc2 << ": ";
    pc2->Exibir();
    ApenasTeste *pc3, *pc4;
    pc3 = new (buffer) ApenasTeste("Má Ideia", 6);
    pc4 = new ApenasTeste("Heap2", 10);
    cout << "Conteúdo da memória:\n";
    cout << pc3 << ": ";
    pc3->Exibir();
    cout << pc4 << ": ";
    pc4->Exibir();
    delete pc2;           // libera Heap1
    delete pc4;           // libera Heap2
    delete [] buffer;     // libera buffer
    cout << "Feito\n";
    return 0;
}
```

O programa na Listagem 12.8 usa `new` para criar um buffer de memória de 512 bytes. Ele então usa `new` para criar dois objetos do tipo `ApenasTeste` no heap e tenta usar `placement new` para criar dois objetos do tipo `ApenasTeste` no buffer de memória. Finalmente, ele usa `delete` para liberar a memória alocada por `new`. Aqui está a saída:

```
Apenas Testando construído
Heap1 construído
Endereços do bloco de memória:
buffer: 00320AB0 heap: 00320CE0
Conteúdo da memória:
00320AB0: Apenas Testando, 0
00320CE0: Heap1, 20
Má Ideia construído
Heap2 construído
Conteúdo da memória:
00320AB0: Má Ideia, 6
00320EC8: Heap2, 10
Heap1 destruído
Heap2 destruído
Feito
```

Como de costume, a formatação e os valores exatos dos endereços de memória variarão de sistema para sistema.

Há alguns problemas com o `placement new` como usado na Listagem 12.8. Primeiro, ao criar um segundo objeto, o `placement new` simplesmente sobrescreve o mesmo local usado para o primeiro objeto com um novo objeto. Isso não é apenas indelicado; significa que o destrutor nunca foi chamado para o primeiro objeto. Isso, é claro, criaria problemas reais se, digamos, a classe usasse alocação dinâmica de memória para seus membros.

Segundo, usar `delete` com `pc2` e `pc4` invoca automaticamente os destrutores para os dois objetos para os quais `pc2` e `pc4` apontam. Mas usar `delete[]` com `buffer` não invoca os destrutores para os objetos criados com `placement new`.

Uma lição a ser aprendida aqui é a mesma lição aprendida no Capítulo 9: cabe a você gerenciar os locais de memória em um buffer que o `placement new` popula. Para usar dois locais diferentes, você fornece dois endereços diferentes dentro do buffer, certificando-se de que os locais não se sobreponham. Você pode, por exemplo, usar isto:

```cpp
pc1 = new (buffer) ApenasTeste;
pc3 = new (buffer + sizeof(ApenasTeste)) ApenasTeste("Ideia Melhor", 6);
```

Aqui o ponteiro `pc3` está deslocado de `pc1` pelo tamanho de um objeto `ApenasTeste`.

A segunda lição a ser aprendida aqui é que, se você usa `placement new` para armazenar objetos, precisa providenciar a chamada de seus destrutores. Mas como? Para objetos criados no heap, você pode usar isto:

```cpp
delete pc2; // delete o objeto apontado por pc2
```

Mas você não pode usar isto:

```cpp
delete pc1; // delete o objeto apontado por pc1? NÃO!
delete pc3; // delete o objeto apontado por pc3? NÃO!
```

A razão é que `delete` funciona em conjunto com `new`, mas não com `placement new`. O ponteiro `pc3`, por exemplo, não recebe um endereço retornado por `new`, portanto `delete pc3` lança um erro em tempo de execução. O ponteiro `pc1`, por outro lado, tem o mesmo valor numérico que `buffer`, mas `buffer` é inicializado usando `new[]`, portanto é liberado usando `delete[]`, não `delete`. Mesmo se `buffer` fosse inicializado por `new` em vez de `new[]`, `delete pc1` liberaria `buffer`, não `pc1`. Isso ocorre porque o sistema `new`/`delete` conhece o bloco de 512 bytes que é alocado, mas não sabe nada sobre o que o `placement new` faz com o bloco.

Observe que o programa libera o buffer:

```cpp
delete [] buffer; // libera o buffer
```

Como este comentário sugere, `delete [] buffer;` deleta o bloco de memória inteiro alocado por `new`. Mas não chama os destrutores para quaisquer objetos que o `placement new` construiu no bloco. Você pode perceber isso porque este programa usa destrutores faladores, que relatam o desaparecimento de `"Heap1"` e `"Heap2"`, mas que permanecem silenciosos sobre `"Apenas Testando"` e `"Má Ideia"`.

A solução para esse dilema é que você deve chamar o destrutor explicitamente para qualquer objeto criado por `placement new`. Normalmente, os destrutores são chamados automaticamente; este é um dos raros casos que exige uma chamada explícita. Uma chamada explícita a um destrutor requer identificar o objeto a ser destruído. Como há ponteiros para os objetos, você pode usar esses ponteiros:

```cpp
pc3->~ApenasTeste(); // destrói o objeto apontado por pc3
pc1->~ApenasTeste(); // destrói o objeto apontado por pc1
```

A Listagem 12.9 corrige a Listagem 12.8 gerenciando os locais de memória usados pelo `placement new` e adicionando usos apropriados de `delete` e de chamadas explícitas ao destrutor. Um fato importante é a ordem correta de deleção. Os objetos construídos pelo `placement new` devem ser destruídos em ordem inversa àquela em que foram construídos. A razão é que, em princípio, um objeto posterior pode ter dependências de um objeto anterior. E o buffer usado para armazenar os objetos só deve ser liberado depois que todos os objetos contidos forem destruídos.

### Listagem 12.9 — placenew2.cpp

```cpp
// placenew2.cpp -- new, placement new, com delete adequado
#include <iostream>
#include <string>
#include <new>
using namespace std;
const int BUF = 512;
class ApenasTeste
{
private:
    string palavras;
    int numero;
public:
    ApenasTeste(const string & s = "Apenas Testando", int n = 0)
    {palavras = s; numero = n; cout << palavras << " construído\n"; }
    ~ApenasTeste() { cout << palavras << " destruído\n";}
    void Exibir() const { cout << palavras << ", " << numero << endl;}
};
int main()
{
    char * buffer = new char[BUF]; // obtém um bloco de memória
    ApenasTeste *pc1, *pc2;
    pc1 = new (buffer) ApenasTeste;     // coloca objeto no buffer
    pc2 = new ApenasTeste("Heap1", 20); // coloca objeto no heap
    cout << "Endereços do bloco de memória:\n" << "buffer: "
       << (void *) buffer << " heap: " << pc2 << endl;
    cout << "Conteúdo da memória:\n";
    cout << pc1 << ": ";
    pc1->Exibir();
    cout << pc2 << ": ";
    pc2->Exibir();
    ApenasTeste *pc3, *pc4;
    // corrige o local do placement new
    pc3 = new (buffer + sizeof(ApenasTeste))
             ApenasTeste("Ideia Melhor", 6);
    pc4 = new ApenasTeste("Heap2", 10);
    cout << "Conteúdo da memória:\n";
    cout << pc3 << ": ";
    pc3->Exibir();
    cout << pc4 << ": ";
    pc4->Exibir();
    delete pc2;  // libera Heap1
    delete pc4;  // libera Heap2
    // destrói explicitamente os objetos criados com placement new
    pc3->~ApenasTeste(); // destrói o objeto apontado por pc3
    pc1->~ApenasTeste(); // destrói o objeto apontado por pc1
    delete [] buffer; // libera o buffer
    cout << "Feito\n";
    return 0;
}
```

Aqui está a saída do programa na Listagem 12.9:

```
Apenas Testando construído
Heap1 construído
Endereços do bloco de memória:
buffer: 00320AB0 heap: 00320CE0
Conteúdo da memória:
00320AB0: Apenas Testando, 0
00320CE0: Heap1, 20
Ideia Melhor construído
Heap2 construído
Conteúdo da memória:
00320AD0: Ideia Melhor, 6
00320EC8: Heap2, 10
Heap1 destruído
Heap2 destruído
Ideia Melhor destruído
Apenas Testando destruído
Feito
```

O programa na Listagem 12.9 coloca os dois objetos de `placement new` em locais adjacentes e chama os destrutores adequados.

## Revisando as Técnicas

A esta altura, você encontrou várias técnicas de programação para lidar com vários problemas relacionados a classes, e pode estar tendo dificuldades para acompanhar todas elas. Então as seções a seguir resumem várias técnicas e quando elas são usadas.

### Sobrecarregando o Operador <<

Para redefinir o operador `<<` de modo a usá-lo com `cout` para exibir o conteúdo de um objeto, você define uma função de operador amiga que tem a seguinte forma:

```cpp
ostream & operator<<(ostream & os, const NomeClasse & obj)
{
    os << ... ; // exibe o conteúdo do objeto
    return os;
}
```

Aqui `NomeClasse` representa o nome da classe. Se a classe fornece métodos públicos que retornam os conteúdos necessários, você pode usar esses métodos na função operadora e dispensar o status de amiga.

### Funções de Conversão

Para converter um único valor para um tipo de classe, você cria um construtor de classe que tem o seguinte protótipo:

```cpp
NomeClasse(NomeTipo valor);
```

Aqui `NomeClasse` representa o nome da classe, e `NomeTipo` representa o nome do tipo que você quer converter.

Para converter de um tipo de classe para algum outro tipo, você cria uma função membro de classe que tem o seguinte protótipo:

```cpp
operator NomeTipo();
```

Embora esta função não tenha tipo de retorno declarado, ela deve retornar um valor do tipo desejado.

Lembre-se de usar funções de conversão com cuidado. Você pode usar a palavra-chave `explicit` ao declarar um construtor para impedir que ele seja usado para conversões implícitas.

### Classes Cujos Construtores Usam new

Você precisa tomar várias precauções ao projetar classes que usam o operador `new` para alocar memória apontada por um membro de classe (sim, resumimos essas precauções recentemente, mas as regras são muito importantes de lembrar, particularmente porque o compilador não as conhece e portanto não capturará seus erros):

- Qualquer membro de classe que aponta para memória alocada por `new` deve ter o operador `delete` aplicado a ele no destrutor de classe. Isso libera a memória alocada.
- Se um destrutor libera memória aplicando `delete` a um ponteiro que é membro de classe, todos os construtores daquela classe devem inicializar aquele ponteiro, seja usando `new` ou definindo o ponteiro como o ponteiro nulo.
- Os construtores devem se decidir entre usar `new[]` ou `new`, mas não uma mistura de ambos. O destrutor deve usar `delete[]` se os construtores usam `new[]`, e deve usar `delete` se os construtores usam `new`.
- Você deve definir um construtor de cópia que aloca nova memória em vez de copiar um ponteiro para a memória existente. Isso permite que um programa inicialize um objeto de classe a partir de outro. O construtor normalmente deve ter o seguinte protótipo:
  ```cpp
  NomeClasse(const NomeClasse &)
  ```
- Você deve definir uma função membro de classe que sobrecarrega o operador de atribuição e que tem uma definição de função com o seguinte protótipo (onde `c_ptr` é um membro da classe `NomeClasse` e tem o tipo ponteiro-para-`NomeTipo`). O exemplo a seguir assume que os construtores inicializam a variável `c_ptr` usando `new[]`:
  ```cpp
  NomeClasse & NomeClasse::operator=(const NomeClasse & cn)
  {
      if (this == &cn)
          return *this; // encerrado se autoatribuição
      delete [] c_ptr;
      // define o tamanho para o número de unidades NomeTipo a serem copiadas
      c_ptr = new NomeTipo[tamanho];
      // então copia os dados apontados por cn.c_ptr para
      // o local apontado por c_ptr
      ...
      return *this;
  }
  ```

---

[Anterior](capitulo-12-01-alocacao-dinamica.md) | [Índice](README.md) | [Próximo](capitulo-12-03-fila-resumo.md)
