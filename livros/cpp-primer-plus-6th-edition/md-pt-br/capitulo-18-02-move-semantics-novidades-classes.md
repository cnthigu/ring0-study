# Capítulo 18 — Visitando o Novo Padrão C++ (parte 2)

> Tradução não oficial de *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Navegação: [Anterior](capitulo-18-01-novidades-cpp11-revisitadas.md) | [Índice](README.md) | [Próximo](capitulo-18-03-lambdas-wrappers-variadicos.md)

## Semântica de Move e a Referência Rvalue

Vamos agora tratar dos tópicos que ainda não discutimos. O C++11 habilita uma técnica chamada semântica de move (move semantics). Isso levanta algumas questões, como o que é semântica de move, como o C++11 a habilita e por que precisamos dela? Começaremos com a última questão.

### A Necessidade de Semântica de Move

Vamos examinar o processo de cópia como funcionava antes do C++11. Suponha que comecemos com algo assim:

```cpp
vector<string> vstr;
// construir um vetor de 20.000 strings, cada uma de 1000 caracteres
// ...
vector<string> copia_vstr1(vstr); // fazer copia_vstr1 uma cópia de vstr
```

Tanto a classe `vector` quanto a classe `string` usam alocação dinâmica de memória, portanto terão construtores de cópia definidos que usam alguma versão de `new`. Para inicializar o objeto `copia_vstr1`, o construtor de cópia de `vector<string>` usará `new` para alocar memória para 20.000 objetos `string`, e cada objeto `string`, por sua vez, chamará o construtor de cópia de `string`, que usará `new` para alocar memória para 1.000 caracteres. Em seguida, todos os 20.000.000 caracteres serão copiados da memória controlada por `vstr` para a memória controlada por `copia_vstr1`. Isso é muito trabalho, mas se tiver que ser feito, terá que ser feito.

Mas "tem que ser feito"? Às vezes, a resposta é não. Por exemplo, suponha que tenhamos uma função que retorne um objeto `vector<string>`:

```cpp
vector<string> tudoMaiusculo(const vector<string> & vs)
{
    vector<string> temp;
    // código que armazena uma versão toda em maiúsculas de vs em temp
    return temp;
}
```

Agora suponha que a usemos desta forma:

```cpp
vector<string> vstr;
// construir um vetor de 20.000 strings, cada uma de 1000 caracteres
vector<string> copia_vstr1(vstr);               // #1
vector<string> copia_vstr2(tudoMaiusculo(vstr)); // #2
```

Superficialmente, as instruções `#1` e `#2` são semelhantes; cada uma usa um objeto existente para inicializar um novo objeto `vector<string>`. Se tomarmos esse código pelo valor de face, `tudoMaiusculo()` cria um objeto `temp` gerenciando 20.000.000 caracteres, os construtores de cópia de `vector` e `string` passam pelo esforço de criar uma duplicata de 20.000.000 de caracteres e, então, o programa exclui o objeto temporário retornado por `tudoMaiusculo()`. (Um compilador verdadeiramente desinspirante poderia até copiar `temp` para um objeto de retorno temporário, excluir `temp` e depois excluir o objeto de retorno.) O ponto principal é que muito esforço é desperdiçado aqui. Como o objeto temporário é excluído, não seria melhor se o compilador pudesse simplesmente transferir a propriedade dos dados para `copia_vstr2`? Ou seja, em vez de copiar 20.000.000 de caracteres para um novo local e depois excluir o local antigo, apenas deixar os caracteres no lugar e anexar o rótulo `copia_vstr2` a eles. Isso seria semelhante ao que acontece quando você move um arquivo de um diretório para outro: o arquivo real permanece onde está no disco rígido, e apenas a contabilidade é alterada. Essa abordagem é chamada de semântica de move. Um tanto paradoxalmente, a semântica de move na verdade evita mover os dados primários; ela apenas ajusta a contabilidade.

Para implementar a semântica de move, precisamos de uma maneira de permitir que o compilador saiba quando precisa fazer uma cópia real e quando não precisa. É aqui que a referência rvalue entra em jogo. Podemos definir dois construtores. Um, o construtor de cópia regular, pode usar a referência lvalue `const` usual como parâmetro. Essa referência se vinculará a argumentos lvalue, como `vstr` na instrução `#1`. O outro, chamado de construtor de move (move constructor), pode usar uma referência rvalue e se vincular a argumentos rvalue, como o valor de retorno de `tudoMaiusculo(vstr)` na instrução `#2`. O construtor de cópia pode fazer a cópia profunda usual, enquanto o construtor de move pode apenas ajustar a contabilidade. Um construtor de move pode alterar seu argumento no processo de transferência de propriedade para um novo objeto, e isso implica que um parâmetro de referência rvalue não deve ser `const`.

### Um Exemplo de Move

Vamos ver um exemplo para ver como a semântica de move e as referências rvalue funcionam. A Listagem 18.2 define e usa a classe `Inutil`, que incorpora alocação dinâmica de memória, um construtor de cópia regular e um construtor de move, que usa semântica de move e uma referência rvalue. Para ilustrar os processos envolvidos, os construtores e o destruidor são inusitadamente verbosos, e a classe usa uma variável de estado para rastrear o número de objetos. Além disso, alguns métodos importantes, como o operador de atribuição, são omitidos.

**Listagem 18.2 — `inutil.cpp`**

```cpp
// inutil.cpp -- uma classe inutil com semântica de move
#include <iostream>
using namespace std;
// interface
class Inutil
{
private:
    int n;      // número de elementos
    char * pc;  // ponteiro para dados
    static int cont; // número de objetos
    void ExibirObjeto() const;
public:
    Inutil();
    explicit Inutil(int k);
    Inutil(int k, char ch);
    Inutil(const Inutil & f); // construtor de cópia regular
    Inutil(Inutil && f);      // construtor de move
    ~Inutil();
    Inutil operator+(const Inutil & f) const;
    void ExibirDados() const;
};
// implementação
int Inutil::cont = 0;
Inutil::Inutil()
{
    ++cont;
    n = 0;
    pc = nullptr;
    cout << "construtor padrão chamado; número de objetos: " << cont << endl;
    ExibirObjeto();
}
Inutil::Inutil(int k) : n(k)
{
    ++cont;
    cout << "construtor int chamado; número de objetos: " << cont << endl;
    pc = new char[n];
    ExibirObjeto();
}
Inutil::Inutil(int k, char ch) : n(k)
{
    ++cont;
    cout << "construtor int,char chamado; número de objetos: " << cont << endl;
    pc = new char[n];
    for (int i = 0; i < n; i++)
        pc[i] = ch;
    ExibirObjeto();
}
Inutil::Inutil(const Inutil & f) : n(f.n)
{
    ++cont;
    cout << "construtor de cópia chamado; número de objetos: " << cont << endl;
    pc = new char[n];
    for (int i = 0; i < n; i++)
        pc[i] = f.pc[i];
    ExibirObjeto();
}
Inutil::Inutil(Inutil && f) : n(f.n)
{
    ++cont;
    cout << "construtor de move chamado; número de objetos: " << cont << endl;
    pc = f.pc;         // roubar endereço
    f.pc = nullptr;    // não dar nada ao objeto antigo em troca
    f.n = 0;
    ExibirObjeto();
}
Inutil::~Inutil()
{
    cout << "destruidor chamado; objetos restantes: " << --cont << endl;
    cout << "objeto excluído:\n";
    ExibirObjeto();
    delete [] pc;
}
Inutil Inutil::operator+(const Inutil & f) const
{
    cout << "Entrando em operator+()\n";
    Inutil temp = Inutil(n + f.n);
    for (int i = 0; i < n; i++)
        temp.pc[i] = pc[i];
    for (int i = n; i < temp.n; i++)
        temp.pc[i] = f.pc[i - n];
    cout << "objeto temp:\n";
    cout << "Saindo de operator+()\n";
    return temp;
}
void Inutil::ExibirObjeto() const
{
    cout << "Número de elementos: " << n;
    cout << " Endereço dos dados: " << (void *) pc << endl;
}
void Inutil::ExibirDados() const
{
    if (n == 0)
        cout << "(objeto vazio)";
    else
        for (int i = 0; i < n; i++)
            cout << pc[i];
    cout << endl;
}
// aplicação
int main()
{
    {
        Inutil um(10, 'x');
        Inutil dois = um;           // chama construtor de cópia
        Inutil tres(20, 'o');
        Inutil quatro(um + tres);   // chama operator+(), construtor de move
        cout << "objeto um: ";
        um.ExibirDados();
        cout << "objeto dois: ";
        dois.ExibirDados();
        cout << "objeto três: ";
        tres.ExibirDados();
        cout << "objeto quatro: ";
        quatro.ExibirDados();
    }
}
```

As definições cruciais são as dos dois construtores de cópia/move. Primeiro, desprovido das instruções de saída, aqui está o construtor de cópia:

```cpp
Inutil::Inutil(const Inutil & f) : n(f.n)
{
    ++cont;
    pc = new char[n];
    for (int i = 0; i < n; i++)
        pc[i] = f.pc[i];
}
```

Ele faz a cópia profunda usual, e é o construtor usado pela seguinte instrução:

```cpp
Inutil dois = um; // chama construtor de cópia
```

A referência `f` refere-se ao objeto lvalue `um`.

Agora, aqui está o construtor de move:

```cpp
Inutil::Inutil(Inutil && f) : n(f.n)
{
    ++cont;
    pc = f.pc;      // roubar endereço
    f.pc = nullptr; // não dar nada ao objeto antigo em troca
    f.n = 0;
}
```

Ele assume a propriedade dos dados existentes definindo `pc` para apontar para os dados. A esse ponto, tanto `pc` quanto `f.pc` apontam para os mesmos dados. Isso seria problemático quando os destruidores fossem chamados, porque um programa não deveria chamar `delete []` duas vezes para o mesmo endereço. Para evitar esse problema, o construtor define o ponteiro original para o ponteiro nulo, porque não é um erro aplicar `delete []` ao ponteiro nulo. Essa apropriação de propriedade é frequentemente chamada de pilhagem (pilfering). O código também define a contagem de elementos no objeto original como 0. Isso não é necessário, mas torna a saída do nosso exemplo mais auto-consistente. Observe que as mudanças no objeto `f` exigem não usar `const` na declaração do parâmetro.

É esse construtor que é usado na seguinte instrução:

```cpp
Inutil quatro(um + tres); // chama construtor de move
```

A expressão `um + tres` invoca `Inutil::operator+()`, e a referência rvalue `f` se vincula ao objeto temporário rvalue retornado pelo método.

### Observações sobre Construtores de Move

Embora usar uma referência rvalue habilite a semântica de move, ela não a faz acontecer magicamente. Há duas etapas para a habilitação. A primeira etapa é que a referência rvalue permite que o compilador identifique quando a semântica de move pode ser usada:

```cpp
Inutil dois = um;           // corresponde a Inutil::Inutil(const Inutil &)
Inutil quatro(um + tres);   // corresponde a Inutil::Inutil(Inutil &&)
```

O objeto `um` é um lvalue, portanto corresponde à referência lvalue, e a expressão `um + tres` é um rvalue, portanto corresponde à referência rvalue. Assim, a referência rvalue direciona a inicialização do objeto `quatro` para o construtor de move. A segunda etapa para habilitar a semântica de move é codificar o construtor de move para que ele forneça o comportamento desejado.

Em suma, a presença de um construtor com uma referência lvalue e um segundo construtor com uma referência rvalue classifica as possíveis inicializações em dois grupos. Objetos inicializados com um objeto lvalue usam o construtor de cópia, e objetos inicializados com um objeto rvalue usam o construtor de move. O escritor de código pode então dotar esses construtores de comportamentos diferentes.

### Atribuição

As mesmas considerações que tornam a semântica de move apropriada para construtores a tornam apropriada para atribuição. Aqui está como você poderia codificar os operadores de atribuição por cópia e por move para a classe `Inutil`:

```cpp
Inutil & Inutil::operator=(const Inutil & f) // atribuição por cópia
{
    if (this == &f)
        return *this;
    delete [] pc;
    n = f.n;
    pc = new char[n];
    for (int i = 0; i < n; i++)
        pc[i] = f.pc[i];
    return *this;
}
Inutil & Inutil::operator=(Inutil && f) // atribuição por move
{
    if (this == &f)
        return *this;
    delete [] pc;
    n = f.n;
    pc = f.pc;
    f.n = 0;
    f.pc = nullptr;
    return *this;
}
```

O operador de atribuição por cópia segue o padrão usual dado no Capítulo 12. O operador de atribuição por move exclui os dados originais no destino e pilha o objeto fonte. É importante que apenas um ponteiro aponte para os dados, portanto o método redefine o ponteiro no objeto fonte para o ponteiro nulo.

Assim como no construtor de move, o parâmetro do operador de atribuição por move não é uma referência `const` porque o método altera o objeto fonte.

### Forçando um Move

Construtores de move e operadores de atribuição por move funcionam com rvalues. E se você quiser usá-los com lvalues? Por exemplo, um programa poderia analisar um array de algum tipo de objetos candidatos, selecionar um objeto para uso posterior e descartar o array. Seria conveniente se você pudesse usar um construtor de move ou um operador de atribuição por move para preservar o objeto selecionado. No entanto, suponha que você tente o seguinte:

```cpp
Inutil escolhas[10];
Inutil melhor;
int escolhido;
// ... // selecionar um objeto, definir escolhido para o índice
melhor = escolhas[escolhido];
```

O objeto `escolhas[escolhido]` é um lvalue, portanto a instrução de atribuição usará o operador de atribuição por cópia, não o operador de atribuição por move. Mas se você pudesse fazer `escolhas[escolhido]` parecer um rvalue, o operador de atribuição por move seria usado. Isso pode ser feito usando o operador `static_cast<>` para converter o objeto para o tipo `Inutil &&`. O C++11 fornece uma maneira mais simples de fazer isso — use a função `std::move()`, que é declarada no arquivo de cabeçalho `utility`. A Listagem 18.3 ilustra essa técnica. Ela adiciona versões verbosas dos operadores de atribuição à classe `Inutil` enquanto silencia os construtores e destruidor anteriormente verbosos.

**Listagem 18.3 — `mover_std.cpp`**

```cpp
// mover_std.cpp -- usando std::move()
#include <iostream>
#include <utility>
// interface
class Inutil
{
private:
    int n;      // número de elementos
    char * pc;  // ponteiro para dados
    static int cont; // número de objetos
    void ExibirObjeto() const;
public:
    Inutil();
    explicit Inutil(int k);
    Inutil(int k, char ch);
    Inutil(const Inutil & f);  // construtor de cópia regular
    Inutil(Inutil && f);       // construtor de move
    ~Inutil();
    Inutil operator+(const Inutil & f) const;
    Inutil & operator=(const Inutil & f);  // atribuição por cópia
    Inutil & operator=(Inutil && f);       // atribuição por move
    void ExibirDados() const;
};
// implementação
int Inutil::cont = 0;
Inutil::Inutil()
{
    ++cont;
    n = 0;
    pc = nullptr;
}
Inutil::Inutil(int k) : n(k)
{
    ++cont;
    pc = new char[n];
}
Inutil::Inutil(int k, char ch) : n(k)
{
    ++cont;
    pc = new char[n];
    for (int i = 0; i < n; i++)
        pc[i] = ch;
}
Inutil::Inutil(const Inutil & f) : n(f.n)
{
    ++cont;
    pc = new char[n];
    for (int i = 0; i < n; i++)
        pc[i] = f.pc[i];
}
Inutil::Inutil(Inutil && f) : n(f.n)
{
    ++cont;
    pc = f.pc;
    f.pc = nullptr;
    f.n = 0;
}
Inutil::~Inutil()
{
    delete [] pc;
}
Inutil & Inutil::operator=(const Inutil & f) // atribuição por cópia
{
    std::cout << "operador de atribuição por cópia chamado:\n";
    if (this == &f)
        return *this;
    delete [] pc;
    n = f.n;
    pc = new char[n];
    for (int i = 0; i < n; i++)
        pc[i] = f.pc[i];
    return *this;
}
Inutil & Inutil::operator=(Inutil && f) // atribuição por move
{
    std::cout << "operador de atribuição por move chamado:\n";
    if (this == &f)
        return *this;
    delete [] pc;
    n = f.n;
    pc = f.pc;
    f.n = 0;
    f.pc = nullptr;
    return *this;
}
Inutil Inutil::operator+(const Inutil & f) const
{
    Inutil temp = Inutil(n + f.n);
    for (int i = 0; i < n; i++)
        temp.pc[i] = pc[i];
    for (int i = n; i < temp.n; i++)
        temp.pc[i] = f.pc[i - n];
    return temp;
}
void Inutil::ExibirObjeto() const
{
    std::cout << "Número de elementos: " << n;
    std::cout << " Endereço dos dados: " << (void *) pc << std::endl;
}
void Inutil::ExibirDados() const
{
    if (n == 0)
        std::cout << "(objeto vazio)";
    else
        for (int i = 0; i < n; i++)
            std::cout << pc[i];
    std::cout << std::endl;
}
// aplicação
int main()
{
    using std::cout;
    {
        Inutil um(10, 'x');
        Inutil dois = um + um; // chama construtor de move
        cout << "objeto um: ";
        um.ExibirDados();
        cout << "objeto dois: ";
        dois.ExibirDados();
        Inutil tres, quatro;
        cout << "tres = um\n";
        tres = um;    // atribuição por cópia automática
        cout << "agora objeto três = ";
        tres.ExibirDados();
        cout << "e objeto um = ";
        um.ExibirDados();
        cout << "quatro = um + dois\n";
        quatro = um + dois;       // atribuição por move automática
        cout << "agora objeto quatro = ";
        quatro.ExibirDados();
        cout << "quatro = move(um)\n";
        quatro = std::move(um);   // atribuição por move forçada
        cout << "agora objeto quatro = ";
        quatro.ExibirDados();
        cout << "e objeto um = ";
        um.ExibirDados();
    }
}
```

Eis uma execução de exemplo:

```
objeto um: xxxxxxxxxx
objeto dois: xxxxxxxxxxxxxxxxxxxx
tres = um
operador de atribuição por cópia chamado:
agora objeto três = xxxxxxxxxx
e objeto um = xxxxxxxxxx
quatro = um + dois
operador de atribuição por move chamado:
agora objeto quatro = xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
quatro = move(um)
operador de atribuição por move chamado:
agora objeto quatro = xxxxxxxxxx
e objeto um = (objeto vazio)
```

Como você pode ver, atribuir `um` a `tres` invoca a atribuição por cópia, mas atribuir `move(um)` a `quatro` invoca a atribuição por move.

Você deve perceber que a função `std::move()` não produz necessariamente uma operação de move. Suponha, por exemplo, que `Bloco` seja uma classe com dados privados e que tenhamos o seguinte código:

```cpp
Bloco um;
// ...
Bloco dois;
dois = std::move(um); // semântica de move?
```

A expressão `std::move(um)` é um rvalue, portanto a instrução de atribuição invocará o operador de atribuição por move para `Bloco`, desde que um tenha sido definido. Mas se a classe `Bloco` não definir um operador de atribuição por move, o compilador usará o operador de atribuição por cópia. E se esse também não for definido, então a atribuição não é permitida.

O principal benefício que as referências rvalue trazem para a maioria dos programadores não é a oportunidade de escrever código que as utilize. Em vez disso, é a oportunidade de usar código de biblioteca que utiliza referências rvalue para implementar semântica de move. Por exemplo, as classes da STL agora têm construtores de cópia, construtores de move, operadores de atribuição por cópia e operadores de atribuição por move.

---

## Novas Funcionalidades de Classe

O C++11 adiciona várias funcionalidades a classes além daquelas já mencionadas neste capítulo — ou seja, operadores de conversão `explicit` e inicialização de membro em classe.

### Funções-Membro Especiais

O C++11 adiciona duas funções-membro especiais (o construtor de move e o operador de atribuição por move) às quatro anteriores (o construtor padrão, o construtor de cópia, o operador de atribuição por cópia e o destruidor). Essas são funções-membro que o compilador fornece automaticamente, sujeitas a uma variedade de condições.

O construtor padrão, recorde-se, é um construtor que pode ser chamado sem argumentos. O compilador fornece um se você não definir nenhum construtor para a classe. Essa versão padrão de um construtor padrão é denominada construtor padrão padronizado (defaulted default constructor). O construtor padrão padronizado deixa membros dos tipos embutidos não inicializados, e invoca os construtores padrão para membros que são objetos de classe.

Além disso, o compilador fornece um construtor de cópia padronizado se você não fornecer um e se o seu código exigir seu uso, e agora fornece um construtor de move padronizado se você não fornecer um e se o seu código exigir seu uso. Se o nome da classe for `AlgumaClasse`, esses dois construtores padronizados têm os seguintes protótipos:

```cpp
AlgumaClasse::AlgumaClasse(const AlgumaClasse &);  // construtor de cópia padronizado
AlgumaClasse::AlgumaClasse(AlgumaClasse &&);        // construtor de move padronizado
```

Em circunstâncias semelhantes, o compilador fornece um operador de atribuição por cópia padronizado e um operador de atribuição por move padronizado com os seguintes protótipos:

```cpp
AlgumaClasse & AlgumaClasse::operator=(const AlgumaClasse &); // atribuição por cópia padronizada
AlgumaClasse & AlgumaClasse::operator=(AlgumaClasse &&);      // atribuição por move padronizada
```

Por fim, o compilador fornece um destruidor se você não fornecer um.

Há várias exceções a essa descrição. Se você fornecer um destruidor, um construtor de cópia ou um operador de atribuição por cópia, o compilador não fornecerá automaticamente um construtor de move ou um operador de atribuição por move. Se você fornecer um construtor de move ou um operador de atribuição por move, o compilador não fornecerá automaticamente um construtor de cópia ou um operador de atribuição por cópia.

### Métodos Padronizados e Excluídos

O C++11 fornece mais controle sobre quais métodos são usados. Suponha que você deseje usar uma função padronizada que, devido às circunstâncias, não é criada automaticamente. Por exemplo, se você fornecer um construtor de move, então o construtor padrão, o construtor de cópia e o operador de atribuição por cópia não são fornecidos. Nesse caso, você pode usar a palavra-chave `default` para declarar explicitamente as versões padronizadas desses métodos:

```cpp
class AlgumaClasse
{
public:
    AlgumaClasse(AlgumaClasse &&);
    AlgumaClasse() = default;    // usar construtor padrão gerado pelo compilador
    AlgumaClasse(const AlgumaClasse &) = default;
    AlgumaClasse & operator=(const AlgumaClasse &) = default;
    // ...
};
```

O compilador fornece o mesmo construtor que teria fornecido automaticamente caso você não tivesse fornecido o construtor de move.

A palavra-chave `delete`, por outro lado, pode ser usada para evitar que o compilador use um método específico. Por exemplo, para evitar que um objeto seja copiado, você pode desabilitar o construtor de cópia e o operador de atribuição por cópia:

```cpp
class AlgumaClasse
{
public:
    AlgumaClasse() = default; // usar construtor padrão gerado pelo compilador
    // desabilitar construtor de cópia e operador de atribuição por cópia:
    AlgumaClasse(const AlgumaClasse &) = delete;
    AlgumaClasse & operator=(const AlgumaClasse &) = delete;
    // usar construtor de move e operador de atribuição por move gerados pelo compilador:
    AlgumaClasse(AlgumaClasse &&) = default;
    AlgumaClasse & operator=(AlgumaClasse &&) = default;
    AlgumaClasse & operator+(const AlgumaClasse &) const;
    // ...
};
```

Você pode lembrar (do Capítulo 12) que pode desabilitar a cópia colocando o construtor de cópia e o operador de atribuição na seção `private` de uma classe. Mas usar `delete` é uma maneira menos sorrateira e mais facilmente compreendida de realizar esse objetivo.

Qual é o efeito de desabilitar métodos de cópia enquanto habilita métodos de move? Recorde que uma referência rvalue, como a usada por operações de move, se vincula apenas a expressões rvalue. Isso implica o seguinte:

```cpp
AlgumaClasse um;
AlgumaClasse dois;
AlgumaClasse tres(um);      // não permitido, um é um lvalue
AlgumaClasse quatro(um + dois); // permitido, expressão é um rvalue
```

Apenas as seis funções-membro especiais podem ser padronizadas, mas você pode usar `delete` com qualquer função-membro. Um possível uso é desabilitar certas conversões. Suponha, por exemplo, que a classe `AlgumaClasse` tenha um método com um parâmetro do tipo `double`:

```cpp
class AlgumaClasse
{
public:
    // ...
    void refazer(double);
    // ...
};
```

Então suponha que tenhamos o seguinte código:

```cpp
AlgumaClasse sc;
sc.refazer(5);
```

O valor `int` `5` será promovido para `5.0`, e o método `refazer()` será executado.

Agora suponha que a definição de `AlgumaClasse` seja modificada desta forma:

```cpp
class AlgumaClasse
{
public:
    // ...
    void refazer(double);
    void refazer(int) = delete;
    // ...
};
```

Nesse caso, a chamada de método `sc.refazer(5)` corresponde ao protótipo `refazer(int)`. O compilador detectará esse fato e também detectará que `refazer(int)` está excluído, e então sinalizará a chamada como um erro em tempo de compilação.

### Construtores Delegadores (Delegating Constructors)

Se você fornecer a uma classe vários construtores, pode se encontrar escrevendo o mesmo código repetidamente. Ou seja, alguns dos construtores podem exigir que você duplique código já presente em outros construtores. Para tornar a codificação mais simples e confiável, o C++11 permite que você use um construtor como parte da definição de outro construtor. Esse processo é denominado delegação (delegation) porque um construtor delega temporariamente a responsabilidade a outro construtor para trabalhar no objeto que está construindo. A delegação usa uma variante da sintaxe da lista de inicialização de membros:

```cpp
class Notas {
    int k;
    double x;
    std::string st;
public:
    Notas();
    Notas(int);
    Notas(int, double);
    Notas(int, double, std::string);
};
Notas::Notas(int kk, double xx, std::string stt) : k(kk), x(xx), st(stt) {/* fazer coisas */}
Notas::Notas() : Notas(0, 0.01, "Oh") {/* fazer outras coisas */}
Notas::Notas(int kk) : Notas(kk, 0.01, "Ah") {/* fazer mais coisas */}
Notas::Notas(int kk, double xx) : Notas(kk, xx, "Uh") {/* idem */}
```

O construtor padrão, por exemplo, usa o primeiro construtor na lista para inicializar os membros de dados e também faz o que o corpo daquele construtor solicitar. Então ele termina fazendo o que seu próprio corpo solicitar.

### Herança de Construtores (Inheriting Constructors)

Em mais uma medida para simplificar a codificação, o C++11 fornece um mecanismo para classes derivadas herdarem construtores da classe base. O C++98 já tinha uma sintaxe para disponibilizar funções de um namespace:

```cpp
namespace Caixa
{
    int fn(int) { ... }
    int fn(double) { ... }
    int fn(const char *) { ... }
}
using Caixa::fn;
```

Isso disponibiliza todas as funções `fn` sobrecarregadas. A mesma técnica funciona para disponibilizar funções-membro não especiais de uma classe base para uma classe derivada. Por exemplo, considere o seguinte código:

```cpp
class C1
{
    // ...
public:
    // ...
    int fn(int j) { ... }
    double fn(double w) { ... }
    void fn(const char * s) { ... }
};
class C2 : public C1
{
    // ...
public:
    // ...
    using C1::fn;
    double fn(double) { ... };
};
// ...
C2 c2;
int k = c2.fn(3);    // usa C1::fn(int)
double z = c2.fn(2.4); // usa C2::fn(double)
```

A declaração `using` em `C2` disponibiliza os três métodos `fn()` em `C1` para um objeto `C2`. No entanto, o método `fn(double)` definido em `C2` é escolhido em detrimento daquele de `C1`.

O C++11 traz a mesma técnica para construtores. Todos os construtores da classe base, exceto os padrão, de cópia e de move, são incorporados como possíveis construtores para a classe derivada, mas aqueles com assinaturas de função correspondentes aos construtores da classe derivada não são usados:

```cpp
class Base {
    int q;
    double w;
public:
    Base() : q(0), w(0) {}
    Base(int k) : q(k), w(100) {}
    Base(double x) : q(-1), w(x) {}
    Base(int k, double x) : q(k), w(x) {}
    void Exibir() const {std::cout << q << ", " << w << '\n';}
};
class Derivada : public Base
{
    short j;
public:
    using Base::Base;
    Derivada() : j(-100) {}    // Derivada precisa de seu próprio construtor padrão
    Derivada(double x) : Base(2*x), j(int(x)) {}
    Derivada(int i) : j(-2), Base(i, 0.5*i) {}
    void Exibir() const {std::cout << j << ", "; Base::Exibir();}
};
int main()
{
    Derivada o1;           // usa Derivada()
    Derivada o2(18.81);    // usa Derivada(double) em vez de Base(double)
    Derivada o3(10, 1.8);  // usa Base(int, double)
    // ...
}
```

Como não há construtor `Derivada(int, double)`, o `Base(int, double)` herdado é usado para `o3`. Observe que um construtor de classe base herdado inicializa apenas membros da classe base. Se você também precisar inicializar membros da classe derivada, pode usar a sintaxe de inicialização de lista de membros em vez da herança:

```cpp
Derivada(int i, int k, double x) : j(i), Base(k, x) {}
```

### Gerenciando Métodos Virtuais: `override` e `final`

Métodos virtuais são um componente importante para implementar hierarquias de classes polimórficas, nas quais uma referência ou ponteiro de classe base pode invocar o método específico apropriado ao tipo de objeto referenciado. Os métodos virtuais apresentam algumas armadilhas de programação. Por exemplo, suponha que a classe base declare um método virtual específico e você decida fornecer uma versão diferente para uma classe derivada. Isso é chamado de substituição (overriding) da versão antiga. Mas, como discutido no Capítulo 13, "Herança de Classe," se você não corresponder corretamente a assinatura da função, você oculta em vez de substituir a versão antiga:

```cpp
class Acao
{
    int a;
public:
    Acao(int i = 0) : a(i) {}
    int val() const {return a;};
    virtual void f(char ch) const { std::cout << val() << ch << "\n";}
};
class Bingo : public Acao
{
public:
    Bingo(int i = 0) : Acao(i) {}
    virtual void f(char * ch) const { std::cout << val() << ch << "!\n"; }
};
```

Como a classe `Bingo` usa `f(char * ch)` em vez de `f(char ch)`, `f(char ch)` fica oculto para um objeto `Bingo`. Isso impede que um programa use código como o seguinte:

```cpp
Bingo b(10);
b.f('@'); // funciona para objeto Acao, falha para objeto Bingo
```

Com o C++11, você pode usar o especificador virtual `override` para indicar que pretende substituir uma função virtual. Coloque-o após a lista de parâmetros. Se sua declaração não corresponder a um método da base, o compilador objeta. Assim, a seguinte versão de `Bingo::f()` geraria uma mensagem de erro em tempo de compilação:

```cpp
virtual void f(char * ch) const override { std::cout << val() << ch << "!\n"; }
```

O especificador `final` aborda um problema diferente. Você pode descobrir que quer proibir classes derivadas de substituir um método virtual específico. Para fazer isso, coloque `final` após a lista de parâmetros. Por exemplo, o código a seguir impediria que classes baseadas em `Acao` redefinissem a função `f()`:

```cpp
virtual void f(char ch) const final { std::cout << val() << ch << "\n"; }
```

Os especificadores `override` e `final` não têm bem o status de palavras-chave. Em vez disso, são rotulados de "identificadores com significado especial". Isso significa que o compilador usa o contexto em que aparecem para decidir se têm um significado especial. Em outros contextos, eles podem ser usados como identificadores comuns (por exemplo, como nomes de variáveis ou enumerações).

---

[Anterior](capitulo-18-01-novidades-cpp11-revisitadas.md) | [Índice](README.md) | [Próximo](capitulo-18-03-lambdas-wrappers-variadicos.md)
