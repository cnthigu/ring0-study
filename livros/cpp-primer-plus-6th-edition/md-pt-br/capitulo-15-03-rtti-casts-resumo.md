# Capítulo 15 — Amigos, Exceções e Mais (Friends, Exceptions, and More) — Parte 3: RTTI, Operadores de Conversão, Resumo e Revisão

> Tradução não oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Navegação: [Anterior](capitulo-15-02-excecoes-avancadas.md) | [Índice](README.md) | [Próximo](capitulo-16-01-string-stl.md)

## Identificação de Tipo em Tempo de Execução (Runtime Type Identification — RTTI)

A identificação de tipo em tempo de execução (RTTI) é uma das adições mais recentes ao C++, e não é suportada por muitas implementações mais antigas. Outras implementações podem ter configurações do compilador para ativar e desativar o RTTI. A intenção do RTTI é fornecer uma maneira padrão para um programa determinar o tipo de objeto durante a execução. Muitas bibliotecas de classe já forneceram maneiras de fazer isso para seus próprios objetos de classe, mas na ausência de suporte embutido em C++, o mecanismo de cada fornecedor é tipicamente incompatível com os de outros fornecedores. A criação de um padrão de linguagem para RTTI deve permitir que futuras bibliotecas sejam compatíveis entre si.

### Para Que Serve o RTTI?

Suponha que você tenha uma hierarquia de classes descendentes de uma classe base comum. Você pode definir um ponteiro de classe base para apontar para um objeto de qualquer uma das classes desta hierarquia. Em seguida, você chama uma função que, após processar algumas informações, seleciona uma dessas classes, cria um objeto desse tipo e retorna seu endereço, que é atribuído a um ponteiro de classe base. Como você pode saber que tipo de objeto ele aponta?

Antes de responder a essa pergunta, você precisa pensar por que gostaria de saber o tipo. Talvez você queira invocar a versão correta de um método de classe. Se for esse o caso, você realmente não precisa saber o tipo do objeto, desde que essa função seja uma função virtual possuída por todos os membros da hierarquia de classe. Mas pode ser que um objeto derivado tenha um método não herdado. Nesse caso, apenas alguns objetos poderiam usar o método. Ou talvez, para fins de depuração, você gostaria de acompanhar quais tipos de objetos foram gerados. Para esses dois últimos casos, o RTTI fornece uma resposta.

### Como Funciona o RTTI?

C++ tem três componentes suportando RTTI:

- O operador `dynamic_cast` gera um ponteiro para um tipo derivado a partir de um ponteiro para um tipo base, se possível. Caso contrário, o operador retorna `0`, o ponteiro nulo.
- O operador `typeid` retorna um valor identificando o tipo exato de um objeto.
- Uma estrutura `type_info` contém informações sobre um tipo particular.

Você pode usar RTTI apenas com uma hierarquia de classes que tem funções virtuais. A razão é que essas são as únicas hierarquias de classe para as quais você deve estar atribuindo os endereços de objetos derivados a ponteiros de classe base.

> **Atenção:** RTTI funciona apenas para classes que têm funções virtuais.

### O Operador dynamic_cast

O operador `dynamic_cast` é destinado a ser o componente RTTI mais amplamente usado. Ele não responde à pergunta de que tipo de objeto um ponteiro aponta. Em vez disso, responde à pergunta de se você pode atribuir com segurança o endereço de um objeto a um ponteiro de um tipo específico. Suponha que você tenha a seguinte hierarquia:

```cpp
class Grande { // tem métodos virtuais };
class Soberba : public Grande { ... };
class Magnifico : public Soberba { ... };
```

Em seguida, suponha que você tenha os seguintes ponteiros:

```cpp
Grande * pg = new Grande;
Grande * ps = new Soberba;
Grande * pm = new Magnifico;
```

Finalmente, considere as seguintes conversões de tipo:

```cpp
Magnifico * p1 = (Magnifico *) pm;    // #1
Magnifico * p2 = (Magnifico *) pg;    // #2
Soberba * p3 = (Magnifico *) pm;      // #3
```

Quais dessas conversões de tipo são seguras? Dependendo das declarações das classes, todas elas poderiam ser seguras, mas as únicas garantidamente seguras são as nas quais o ponteiro é do mesmo tipo que o objeto ou é um tipo base direto ou indireto para o objeto. Por exemplo, a Conversão #1 é segura porque define um ponteiro do tipo `Magnifico` para apontar para um objeto do tipo `Magnifico`. A Conversão #2 não é segura porque atribui o endereço de um objeto de classe base (`Grande`) a um ponteiro de classe derivada (`Magnifico`). Assim, o programa esperaria que o objeto de classe base tivesse propriedades de classe derivada, e isso, em geral, é falso. A Conversão #3, no entanto, é segura porque atribui o endereço de um objeto de classe derivada a um ponteiro de classe base. Ou seja, a derivação pública promete que um objeto `Magnifico` também é um objeto `Soberba` (base direta) e um objeto `Grande` (base indireta). Assim, é seguro atribuir seu endereço a ponteiros de todos os três tipos. Funções virtuais garantem que usar ponteiros de qualquer um dos três tipos para um objeto `Magnifico` invocará métodos de `Magnifico`.

Observe que a questão de se uma conversão de tipo é segura é tanto mais geral quanto mais útil do que a questão de que tipo de objeto é apontado. A razão usual para querer saber o tipo é para que você saiba se é seguro invocar um método particular. Você não necessariamente precisa de uma correspondência de tipo exata para invocar um método. O tipo pode ser um tipo base para o qual uma versão virtual do método está definida.

Primeiro, vejamos a sintaxe de `dynamic_cast`. O operador é usado assim, onde `pg` aponta para um objeto:

```cpp
Soberba * pm = dynamic_cast<Soberba *>(pg);
```

Este código pergunta se o ponteiro `pg` pode ser convertido com segurança (conforme descrito anteriormente) para o tipo `Soberba *`. Se puder, o operador retorna o endereço do objeto. Caso contrário, retorna `0`, o ponteiro nulo.

> **Nota:** Em geral, a seguinte expressão converte o ponteiro `pt` para um ponteiro do tipo `Tipo *` se o objeto apontado (`*pt`) for do tipo `Tipo` ou então derivado direta ou indiretamente do tipo `Tipo`:
> ```cpp
> dynamic_cast<Tipo *>(pt)
> ```
> Caso contrário, a expressão é avaliada como `0`, o ponteiro nulo.

A Listagem 15.17 ilustra o processo. Primeiro, ela define três classes, coincidentemente chamadas `Grande`, `Soberba` e `Magnifico`. A classe `Grande` define uma função virtual `Falar()`, que cada uma das outras classes redefine. A classe `Soberba` define uma função virtual `Dizer()`, que `Magnifico` redefine. O programa define uma função `ObterUm()` que cria e inicializa aleatoriamente um objeto de um desses três tipos e então retorna o endereço como um ponteiro do tipo `Grande *`. (A função `ObterUm()` simula um usuário interativo tomando decisões.) Um loop atribui esse ponteiro a uma variável do tipo `Grande *` chamada `pg` e então usa `pg` para invocar a função `Falar()`. Como essa função é virtual, o código invoca corretamente a versão de `Falar()` apropriada para o objeto apontado:

```cpp
for (int i = 0; i < 5; i++)
{
    pg = ObterUm();
    pg->Falar();
    ...
}
```

Você não pode usar essa abordagem exata (usando um ponteiro-para-Grande) para invocar a função `Dizer()`; ela não está definida para a classe `Grande`. No entanto, você pode usar o operador `dynamic_cast` para ver se `pg` pode ser convertido para um ponteiro de `Soberba`. Isso será verdadeiro se o objeto for do tipo `Soberba` ou `Magnifico`. Em ambos os casos, você pode invocar a função `Dizer()` com segurança:

```cpp
if (ps = dynamic_cast<Soberba *>(pg))
    ps->Dizer();
```

Lembre-se de que o valor de uma expressão de atribuição é o valor de seu lado esquerdo. Assim, o valor da condição do `if` é `ps`. Se a conversão de tipo tiver sucesso, `ps` é diferente de zero, ou `true`. Se a conversão de tipo falhar, o que acontecerá se `pg` apontar para um objeto `Grande`, `ps` é zero, ou `false`.

**Listagem 15.17 — rtti1.cpp**

```cpp
// rtti1.cpp -- usando o operador dynamic_cast do RTTI
#include <iostream>
#include <cstdlib>
#include <ctime>
using std::cout;

class Grande
{
private:
    int valor;
public:
    Grande(int h = 0) : valor(h) {}
    virtual void Falar() const { cout << "Sou uma classe grande!\n";}
    virtual int Valor() const { return valor; }
};
class Soberba : public Grande
{
public:
    Soberba(int h = 0) : Grande(h) {}
    void Falar() const {cout << "Sou uma classe soberba!!\n"; }
    virtual void Dizer() const
        { cout << "Tenho o valor soberbo de " << Valor() << "!\n";}
};
class Magnifico : public Soberba
{
private:
    char ch;
public:
    Magnifico(int h = 0, char c = 'A') : Soberba(h), ch(c) {}
    void Falar() const {cout << "Sou uma classe magnifica!!!\n";}
    void Dizer() const {cout << "Tenho o caractere " << ch <<
           " e o inteiro " << Valor() << "!\n"; }
};
Grande * ObterUm();
int main()
{
    std::srand(std::time(0));
    Grande * pg;
    Soberba * ps;
    for (int i = 0; i < 5; i++)
    {
        pg = ObterUm();
        pg->Falar();
        if( ps = dynamic_cast<Soberba *>(pg))
            ps->Dizer();
    }
    return 0;
}

Grande * ObterUm()    // gera aleatoriamente um de três tipos de objetos
{
    Grande * p;
    switch( std::rand() % 3)
    {
        case 0: p = new Grande(std::rand() % 100);
                break;
        case 1: p = new Soberba(std::rand() % 100);
                break;
        case 2: p = new Magnifico(std::rand() % 100,
                        'A' + std::rand() % 26);
                break;
    }
    return p;
}
```

> **Nota:** Mesmo que seu compilador suporte RTTI, ele pode ter esse recurso desativado por padrão. Se o recurso estiver inativo, o programa ainda pode compilar, mas produzir erros em tempo de execução. Se você encontrar isso, verifique sua documentação ou explore as opções de menu.

O programa na Listagem 15.17 ilustra um ponto importante. Você deve usar funções virtuais quando possível e RTTI apenas quando necessário. Aqui está uma saída de exemplo:

```
Sou uma classe soberba!!
Tenho o valor soberbo de 68!
Sou uma classe magnifica!!!
Tenho o caractere R e o inteiro 68!
Sou uma classe magnifica!!!
Tenho o caractere D e o inteiro 12!
Sou uma classe magnifica!!!
Tenho o caractere V e o inteiro 59!
Sou uma classe grande!
```

Como você pode ver, os métodos `Dizer()` foram invocados apenas para as classes `Soberba` e `Magnifico`. (A saída vai variar de execução para execução porque o programa usa `rand()` para selecionar o tipo de objeto.)

Você também pode usar `dynamic_cast` com referências. O uso é ligeiramente diferente; não há valor de referência correspondente ao tipo de ponteiro nulo, portanto não há valor especial de referência que possa ser usado para indicar falha. Em vez disso, quando estimulado por uma solicitação incorreta, `dynamic_cast` lança uma exceção do tipo `bad_cast`, que é derivada da classe `exception` e definida no arquivo de cabeçalho `typeinfo`. Assim, o operador pode ser usado da seguinte forma, onde `rg` é uma referência para um objeto `Grande`:

```cpp
#include <typeinfo>    // para bad_cast
...
try {
    Soberba & rs = dynamic_cast<Soberba &>(rg);
    ...
}
catch(bad_cast &){
    ...
};
```

### O Operador typeid e a Classe type_info

O operador `typeid` permite que você determine se dois objetos são do mesmo tipo. De certa forma, como `sizeof`, ele aceita dois tipos de argumentos:

- O nome de uma classe
- Uma expressão que é avaliada para um objeto

O operador `typeid` retorna uma referência para um objeto `type_info`, onde `type_info` é uma classe definida no arquivo de cabeçalho `typeinfo` (anteriormente `typeinfo.h`). A classe `type_info` sobrecarrega os operadores `==` e `!=` para que você possa usar esses operadores para comparar tipos. Por exemplo, a seguinte expressão é avaliada como o valor `bool` `true` se `pg` apontar para um objeto `Magnifico` e como `false` caso contrário:

```cpp
typeid(Magnifico) == typeid(*pg)
```

Se `pg` for um ponteiro nulo, o programa lança uma exceção `bad_typeid`. Este tipo de exceção é derivado da classe `exception` e é declarado no arquivo de cabeçalho `typeinfo`.

A implementação da classe `type_info` varia entre fornecedores, mas inclui um membro `name()` que retorna uma string dependente da implementação que tipicamente (mas não necessariamente) é o nome da classe. Por exemplo, a seguinte instrução exibe a string definida para a classe do objeto para o qual o ponteiro `pg` aponta:

```cpp
cout << "Agora processando tipo " << typeid(*pg).name() << ".\n";
```

A Listagem 15.18 modifica a Listagem 15.17 para que ela use o operador `typeid` e a função membro `name()`. Observe que eles são usados para situações que `dynamic_cast` e funções virtuais não tratam. O teste `typeid` é usado para selecionar uma ação que nem mesmo é um método de classe, portanto não pode ser invocada por um ponteiro de classe. A instrução do método `name()` mostra como o método pode ser usado na depuração. Observe que o programa inclui o arquivo de cabeçalho `typeinfo`.

**Listagem 15.18 — rtti2.cpp**

```cpp
// rtti2.cpp -- usando dynamic_cast, typeid e type_info
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <typeinfo>
using namespace std;

class Grande
{
private:
    int valor;
public:
    Grande(int h = 0) : valor(h) {}
    virtual void Falar() const { cout << "Sou uma classe grande!\n";}
    virtual int Valor() const { return valor; }
};
class Soberba : public Grande
{
public:
    Soberba(int h = 0) : Grande(h) {}
    void Falar() const {cout << "Sou uma classe soberba!!\n"; }
    virtual void Dizer() const
        { cout << "Tenho o valor soberbo de " << Valor() << "!\n";}
};
class Magnifico : public Soberba
{
private:
    char ch;
public:
    Magnifico(int h = 0, char cv = 'A') : Soberba(h), ch(cv) {}
    void Falar() const {cout << "Sou uma classe magnifica!!!\n";}
    void Dizer() const {cout << "Tenho o caractere " << ch <<
           " e o inteiro " << Valor() << "!\n"; }
};
Grande * ObterUm();
int main()
{
    srand(time(0));
    Grande * pg;
    Soberba * ps;
    for (int i = 0; i < 5; i++)
    {
        pg = ObterUm();
        cout << "Agora processando tipo " << typeid(*pg).name() << ".\n";
        pg->Falar();
        if( ps = dynamic_cast<Soberba *>(pg))
            ps->Dizer();
        if (typeid(Magnifico) == typeid(*pg))
            cout << "Sim, voce e realmente magnifico.\n";
    }
    return 0;
}
Grande * ObterUm()
{
    Grande * p;

    switch( rand() % 3)
    {
        case 0: p = new Grande(rand() % 100);
                break;
        case 1: p = new Soberba(rand() % 100);
                break;
        case 2: p = new Magnifico(rand() % 100, 'A' + rand() % 26);
                break;
    }
    return p;
}
```

Aqui está uma execução de exemplo do programa na Listagem 15.18:

```
Agora processando tipo Magnifico.
Sou uma classe magnifica!!!
Tenho o caractere P e o inteiro 52!
Sim, voce e realmente magnifico.
Agora processando tipo Soberba.
Sou uma classe soberba!!
Tenho o valor soberbo de 37!
Agora processando tipo Grande.
Sou uma classe grande!
Agora processando tipo Soberba.
Sou uma classe soberba!!
Tenho o valor soberbo de 18!
Agora processando tipo Grande.
Sou uma classe grande!
```

Como no exemplo anterior, a saída exata variará de execução para execução porque o programa usa `rand()` para selecionar tipos. Além disso, alguns compiladores podem fornecer saída diferente quando `name()` é chamado, por exemplo, `5Grande` em vez de `Grande`.

### Uso Indevido do RTTI

O RTTI tem muitos críticos vocais na comunidade C++. Eles veem o RTTI como desnecessário, uma fonte potencial de ineficiência do programa e um possível contribuidor para práticas ruins de programação. Sem se aprofundar no debate sobre RTTI, vejamos o tipo de programação que você deve evitar.

Considere o núcleo da Listagem 15.17:

```cpp
Grande * pg;
Soberba * ps;
for (int i = 0; i < 5; i++)
{
    pg = ObterUm();
    pg->Falar();
    if( ps = dynamic_cast<Soberba *>(pg))
        ps->Dizer();
}
```

Usando `typeid` e ignorando `dynamic_cast` e funções virtuais, você pode reescrever este código da seguinte forma:

```cpp
Grande * pg;
Soberba * ps;
Magnifico * pm;
for (int i = 0; i < 5; i++)
{
    pg = ObterUm();
    if (typeid(Magnifico) == typeid(*pg))
    {
        pm = (Magnifico *) pg;
        pm->Falar();
        pm->Dizer();
    }
    else if (typeid(Soberba) == typeid(*pg))
    {
        ps = (Soberba *) pg;
        ps->Falar();
        ps->Dizer();
    }
    else
        pg->Falar();
}
```

Não apenas isso é mais feio e mais longo que o original, mas tem o sério defeito de nomear cada classe explicitamente. Suponha, por exemplo, que você ache necessário derivar uma classe `Insuportavel` da classe `Magnifico`. E suponha que a nova classe redefina `Falar()` e `Dizer()`. Com a versão que usa `typeid` para testar explicitamente cada tipo, você teria que modificar o código do loop `for`, adicionando uma nova seção `else if`. A versão original, no entanto, não requer nenhuma alteração. A seguinte instrução funciona para todas as classes derivadas de `Grande`:

```cpp
pg->Falar();
```

E esta instrução funciona para todas as classes derivadas de `Soberba`:

```cpp
if( ps = dynamic_cast<Soberba *>(pg))
    ps->Dizer();
```

> **Dica:** Se você se encontrar usando `typeid` em uma série extensa de instruções `if else`, verifique se você deveria usar funções virtuais e `dynamic_cast` em vez disso.

## Operadores de Conversão de Tipo (Type Cast Operators)

O operador de conversão de tipo de C, na visão de Bjarne Stroustrup, é muito leniente. Por exemplo, considere o seguinte:

```cpp
struct Dados
{
    double dados[200];
};
struct Lixo
{
    int lixo[100];
};
Dados d = {2.5e33, 3.5e-19, 20.2e32};
char * pch = (char *) (&d);       // conversão #1 – converte para string
char ch = char (&d);              // conversão #2 – converte endereço para char
Lixo * pj = (Lixo *) (&d);       // conversão #3 – converte para ponteiro Lixo
```

Primeiro, qual dessas três conversões faz algum sentido? A menos que você recorra ao implausível, nenhuma delas faz muito sentido. Segundo, qual dessas três conversões é permitida? Em C, todas elas são. A resposta de Stroustrup a essa indulgência foi apertar o que é permitido para uma conversão de tipo geral e adicionar quatro operadores de conversão de tipo que fornecem mais disciplina para o processo de conversão:

- `dynamic_cast`
- `const_cast`
- `static_cast`
- `reinterpret_cast`

Em vez de usar uma conversão de tipo geral, você pode selecionar um operador adequado a um propósito específico. Isso documenta a razão pretendida para a conversão de tipo e dá ao compilador a chance de verificar se você fez o que pensou ter feito.

Você já viu o operador `dynamic_cast`. Em resumo, suponha que `Alto` e `Baixo` sejam duas classes, que `ph` seja do tipo `Alto *`, e que `pb` seja do tipo `Baixo *`. Então a seguinte instrução atribui um ponteiro `Baixo *` a `pb` apenas se `Baixo` for uma classe base acessível (direta ou indireta) de `Alto`:

```cpp
pb = dynamic_cast<Baixo *> ph;
```

Caso contrário, a instrução atribui o ponteiro nulo a `pb`. Em geral, o operador tem esta sintaxe:

```cpp
dynamic_cast < nome-do-tipo > (expressão)
```

O propósito deste operador é permitir upcasts dentro de uma hierarquia de classes (tais conversões de tipo sendo seguras devido ao relacionamento is-a) e não permitir outras conversões.

### const_cast

O operador `const_cast` é para fazer uma conversão de tipo com o único propósito de alterar se um valor é `const` ou `volatile`. Ele tem a mesma sintaxe que o operador `dynamic_cast`:

```cpp
const_cast < nome-do-tipo > (expressão)
```

O resultado de fazer tal conversão de tipo é um erro se qualquer outro aspecto do tipo for alterado. Ou seja, `nome-do-tipo` e `expressão` devem ser do mesmo tipo, exceto que podem diferir na presença ou ausência de `const` ou `volatile`. Suponha novamente que `Alto` e `Baixo` sejam duas classes:

```cpp
Alto bar;
const Alto * pbar = &bar;
    ...
Alto * pb = const_cast<Alto *> (pbar);          // válido
const Baixo * pl = const_cast<const Baixo *> (pbar);  // inválido
```

A primeira conversão de tipo torna `*pb` um ponteiro que pode ser usado para alterar o valor do objeto `bar`; ela remove o rótulo `const`. A segunda conversão de tipo é inválida porque tenta alterar o tipo de `const Alto *` para `const Baixo *`.

A razão para este operador é que ocasionalmente você pode precisar de um valor que é constante na maior parte do tempo, mas que pode ser alterado ocasionalmente. Nesse caso, você pode declarar o valor como `const` e usar `const_cast` quando precisar alterar o valor. Isso poderia ser feito usando a conversão de tipo geral, mas a conversão de tipo geral também pode alterar simultaneamente o tipo:

```cpp
Alto bar;
const Alto * pbar = &bar;
...
Alto * pb = (Alto *) (pbar);     // válido
Baixo * pl = (Baixo *) (pbar);  // também válido
```

Como a alteração simultânea de tipo e constância pode ser um deslize de programação não intencional, usar o operador `const_cast` é mais seguro.

O `const_cast` não é todo-poderoso. Ele pode alterar o acesso do ponteiro a uma quantidade, mas o efeito de tentar alterar uma quantidade que é declarada como `const` é indefinido. A Listagem 15.19 clarifica essa afirmação com um exemplo curto.

**Listagem 15.19 — constcast.cpp**

```cpp
// constcast.cpp -- usando const_cast<>
#include <iostream>
using std::cout;
using std::endl;

void alterar(const int * pt, int n);
int main()
{
    int pop1 = 38383;
    const int pop2 = 2000;

    cout << "pop1, pop2: " << pop1 << ", " << pop2 << endl;
    alterar(&pop1, -103);
    alterar(&pop2, -103);
    cout << "pop1, pop2: " << pop1 << ", " << pop2 << endl;
    return 0;
}
void alterar(const int * pt, int n)
{
    int * pc;
    pc = const_cast<int *>(pt);
    *pc += n;
}
```

O operador `const_cast` pode remover o `const` de `const int * pt`, permitindo assim que o compilador aceite a seguinte instrução em `alterar()`:

```cpp
*pc += n;
```

No entanto, como `pop2` é declarado como `const`, o compilador pode protegê-lo de qualquer alteração, como mostrado pela seguinte saída de exemplo:

```
pop1, pop2: 38383, 2000
pop1, pop2: 38280, 2000
```

Como você pode ver, as chamadas a `alterar()` alteram `pop1` mas não `pop2`. O ponteiro em `alterar()` é declarado como `const int *`, portanto não pode ser usado para alterar o valor do `int` apontado. O ponteiro `pc` tem o `const` removido pela conversão, portanto pode ser usado para alterar o valor apontado, mas apenas se esse valor não fosse em si `const`. Portanto, `pc` pode ser usado para alterar `pop1` mas não `pop2`.

### static_cast

O operador `static_cast` tem a mesma sintaxe que os outros operadores:

```cpp
static_cast < nome-do-tipo > (expressão)
```

Ele é válido apenas se `nome-do-tipo` puder ser convertido implicitamente para o mesmo tipo que `expressão` tem, ou vice-versa. Caso contrário, a conversão de tipo é um erro. Suponha que `Alto` seja uma classe base para `Baixo` e que `Lago` seja uma classe não relacionada. Então as conversões de `Alto` para `Baixo` e de `Baixo` para `Alto` são válidas, mas uma conversão de `Baixo` para `Lago` não é permitida:

```cpp
Alto bar;
Baixo blow;
    ...
Alto * pb = static_cast<Alto *> (&blow);    // upcast válido
Baixo * pl = static_cast<Baixo *> (&bar);  // downcast válido
Lago * pmer = static_cast<Lago *> (&blow); // inválido, Lago não relacionado
```

A primeira conversão aqui é válida porque um upcast pode ser feito explicitamente. A segunda conversão, de um ponteiro de classe base para um ponteiro de classe derivada, não pode ser feita sem uma conversão de tipo explícita. Mas como a conversão na outra direção pode ser feita sem uma conversão de tipo, é válido usar `static_cast` para um downcast.

Da mesma forma, como um valor de enumeração pode ser convertido para um tipo integral sem uma conversão de tipo, um tipo integral pode ser convertido para um valor de enumeração com `static_cast`. Além disso, você pode usar `static_cast` para converter `double` para `int`, para converter `float` para `long`, e para realizar as várias outras conversões numéricas.

### reinterpret_cast

O operador `reinterpret_cast` é para conversões de tipo inerentemente arriscadas. Ele não permite que você remova `const`, mas permite outras coisas desagradáveis. Às vezes um programador precisa fazer coisas desagradáveis dependentes da implementação, e usar o operador `reinterpret_cast` torna mais simples acompanhar tais atos. Ele tem a mesma sintaxe que os outros três operadores:

```cpp
reinterpret_cast < nome-do-tipo > (expressão)
```

Aqui está um uso de exemplo:

```cpp
struct dat {short a; short b;};
long valor = 0xA224B118;
dat * pd = reinterpret_cast< dat *> (&valor);
cout << hex << pd->a;    // exibe os primeiros 2 bytes de valor
```

Tipicamente, tais conversões de tipo seriam usadas para programação de baixo nível dependente da implementação e não seriam portáveis. Por exemplo, um sistema pode armazenar os bytes em um valor de múltiplos bytes em uma ordem diferente de um segundo sistema.

O operador `reinterpret_cast` não permite qualquer coisa, no entanto. Por exemplo, você pode converter um tipo de ponteiro para um tipo inteiro grande o suficiente para conter a representação do ponteiro, mas você não pode converter um ponteiro para um tipo inteiro menor ou para um tipo de ponto flutuante. Outra restrição é que você não pode converter um ponteiro de função para um ponteiro de dados ou vice-versa.

A conversão de tipo simples em C++ também é restrita. Basicamente, ela pode fazer qualquer coisa que as outras conversões de tipo podem fazer, além de algumas combinações, como um `static_cast` ou `reinterpret_cast` seguido por um `const_cast`, mas não pode fazer nada mais. Assim, a seguinte conversão de tipo é permitida em C mas, tipicamente, não em C++ porque para a maioria das implementações de C++ o tipo `char` é pequeno demais para conter uma implementação de ponteiro:

```cpp
char ch = char (&d);    // conversão #2 - converte endereço para char
```

Essas restrições fazem sentido, mas se você achar tal bom senso imposto opressivo, ainda tem C disponível.

## Resumo

Amigos permitem que você desenvolva uma interface mais flexível para classes. Uma classe pode ter outras funções, outras classes e funções membro de outras classes como amigas. Em alguns casos, você pode precisar usar declarações prévias e ter cuidado com a ordenação de declarações e métodos de classe para que os amigos se encaixem corretamente.

Classes aninhadas são classes declaradas dentro de outras classes. Classes aninhadas facilitam o design de classes auxiliares que implementam outras classes, mas não precisam fazer parte de uma interface pública.

O mecanismo de exceção de C++ fornece uma maneira flexível de lidar com eventos de programação complicados, como valores inapropriados, tentativas de E/S com falha e similares. Lançar uma exceção encerra a função atualmente em execução e transfere o controle para um bloco `catch` correspondente. Os blocos `catch` seguem imediatamente um bloco `try`, e para que uma exceção seja capturada, a chamada de função que direta ou indiretamente levou à exceção deve estar no bloco `try`. O programa então executa o código no bloco `catch`. Este código pode tentar corrigir o problema, ou pode encerrar o programa. Uma classe pode ser projetada com classes de exceção aninhadas que podem ser lançadas quando problemas específicos da classe são detectados. Uma função pode incluir uma especificação de exceção que identifica as exceções que podem ser lançadas nessa função, embora C++11 reprove esse recurso. Exceções não capturadas (aquelas sem bloco `catch` correspondente) por padrão encerram um programa. O mesmo ocorre com exceções inesperadas (aquelas que não correspondem a uma especificação de exceção).

Os recursos de RTTI permitem que um programa detecte o tipo de um objeto. O operador `dynamic_cast` é usado para converter um ponteiro de classe derivada para um ponteiro de classe base; seu objetivo principal é garantir que é correto invocar uma chamada de função virtual. O operador `typeid` retorna um objeto `type_info`. Dois valores de retorno de `typeid` podem ser comparados para determinar se um objeto é de um tipo específico, e o objeto `type_info` retornado pode ser usado para obter informações sobre um objeto.

Os operadores `dynamic_cast`, `static_cast`, `const_cast` e `reinterpret_cast` fornecem conversões de tipo mais seguras e melhor documentadas do que o mecanismo de conversão de tipo geral.

## Revisão do Capítulo

**1.** O que há de errado com as seguintes tentativas de estabelecer amigos?

a. ```cpp
class snap {
    friend clasp;
    ...
};
class clasp { ... };
```

b. ```cpp
class cuff {
public:
    void snip(muff &) { ... }
    ...
};
class muff {
    friend void cuff::snip(muff &);
    ...
};
```

c. ```cpp
class muff {
    friend void cuff::snip(muff &);
    ...
};
class cuff {
public:
    void snip(muff &) { ... }
    ...
};
```

**2.** Você viu como criar amigos de classe mútua. Você pode criar uma forma mais restrita de amizade na qual apenas alguns membros da Classe B são amigos da Classe A e alguns membros de A são amigos de B? Explique.

**3.** Que problemas a seguinte declaração de classe aninhada pode ter?

```cpp
class Costelas
{
private:
    class Molho
    {
        int soja;
        int acucar;
    public:
        Molho(int s1, int s2) : soja(s1), acucar(s2) { }
    };
    ...
};
```

**4.** Como `throw` difere de `return`?

**5.** Suponha que você tenha uma hierarquia de classes de exceção derivadas de uma classe de exceção base. Em que ordem você deve colocar os blocos `catch`?

**6.** Considere as classes `Grande`, `Soberba` e `Magnifico` definidas neste capítulo. Suponha que `pg` seja um ponteiro do tipo `Grande *` que recebe o endereço de um objeto de uma dessas três classes e que `ps` seja um ponteiro do tipo `Soberba *`. Qual é a diferença no comportamento dos dois exemplos de código a seguir?

```cpp
if (ps = dynamic_cast<Soberba *>(pg))
    ps->Dizer();    // exemplo #1
```

```cpp
if (typeid(*pg) == typeid(Soberba))
    ((Soberba *) pg)->Dizer();    // exemplo #2
```

**7.** Como o operador `static_cast` difere do operador `dynamic_cast`?

### Respostas

**1a.** A declaração `friend clasp;` deveria ser `friend class clasp;`.

**1b.** Para que o compilador processe `friend void cuff::snip(muff &);` dentro de `muff`, ele precisa já ter visto a declaração completa de `cuff`. Mas `cuff` usa `muff`, então `muff` precisa ser declarado antes de `cuff`. A solução é usar uma declaração prévia de `muff`, mover a declaração de método para fora da classe e defini-lo depois da declaração de `muff`, da forma:
```cpp
class muff;        // declaração prévia
class cuff { void snip(muff &); ... };
class muff { friend void cuff::snip(muff &); ... };
```

**1c.** A declaração de `muff` usa `cuff::snip()`, mas `cuff` não foi declarado ainda. Inverta a ordem ou use uma declaração prévia.

**2.** Sim. Para que apenas alguns membros de B sejam amigos de A e alguns de A sejam amigos de B, coloque as declarações `friend` individualmente para cada função membro, e use a técnica de declaração prévia com definições de métodos fora das classes para garantir que cada declaração de amigo veja a classe completa do método referenciado.

**3.** A classe `Molho` é declarada como privada dentro de `Costelas`. O problema é que `Molho` não tem membros de dados públicos (os membros `soja` e `acucar` são privados por padrão em uma classe), mas o construtor é público. No entanto, como `Molho` está na seção privada de `Costelas`, ele não é visível externamente. Isso em si pode ser intencional. Mas dentro de `Costelas`, os métodos podem criar objetos `Molho`, porém não podem acessar `soja` e `acucar` diretamente porque são privados em `Molho`. Se `Costelas` precisar de acesso completo a `Molho`, `Molho` deveria declarar `Costelas` como amiga, ou tornar seus membros públicos.

**4.** Uma instrução `return` em uma função retorna o controle para a função chamadora. Uma instrução `throw` retrocede pela pilha de chamadas até encontrar um bloco `try` com um bloco `catch` correspondente; destruidores de objetos automáticos são chamados ao longo do caminho.

**5.** Os blocos `catch` devem ser ordenados do mais derivado ao menos derivado (classe base). Assim, classes mais especializadas são capturadas primeiro e a classe base é capturada por último.

**6.** A diferença é que o exemplo #1 com `dynamic_cast` captura tanto objetos `Soberba` quanto objetos de qualquer classe derivada de `Soberba` (como `Magnifico`). O exemplo #2 com `typeid` captura apenas objetos exatamente do tipo `Soberba` — não objetos do tipo `Magnifico`.

**7.** `static_cast` pode ser usado para conversões que são válidas implicitamente (upcasts) ou que podem ser feitas explicitamente (downcasts dentro de uma hierarquia), mas não verifica em tempo de execução se a conversão é segura. `dynamic_cast` verifica em tempo de execução e retorna `0` (ou lança `bad_cast` para referências) se a conversão não for segura.

## Exercícios de Programação

**1.** Modifique as classes `Televisao` e `Controle` da seguinte forma:

   a. Torne-as amigas mútuas.
   b. Adicione uma variável membro de estado à classe `Controle` que descreve se o controle remoto está no modo normal ou interativo.
   c. Adicione um método de `Controle` que exibe o modo.
   d. Forneça à classe `Televisao` um método para alternar o novo membro de `Controle`. Este método deve funcionar apenas se a TV estiver no estado ligado.

   Escreva um programa curto que teste essas novas funcionalidades.

**2.** Modifique a Listagem 15.11 para que os dois tipos de exceção sejam classes derivadas da classe `logic_error` fornecida pelo arquivo de cabeçalho `<stdexcept>`. Faça com que cada método `what()` reporte o nome da função e a natureza do problema. Os objetos de exceção não precisam conter os valores ruins; eles devem apenas suportar o método `what()`.

**3.** Este exercício é o mesmo que o Exercício de Programação 2, exceto que as exceções devem ser derivadas de uma classe base (ela própria derivada de `logic_error`) que armazena os dois valores de argumento, as exceções devem ter um método que reporte esses valores assim como o nome da função, e um único bloco `catch` que captura a exceção de classe base deve ser usado para ambas as exceções, com qualquer exceção causando o encerramento do loop.

**4.** A Listagem 15.16 usa dois blocos `catch` após cada bloco `try` para que a exceção `nindice_invalido` leve ao método `val_rotulo()` sendo invocado. Modifique o programa para que ele use um único bloco `catch` após cada bloco `try` e use RTTI para tratar a invocação de `val_rotulo()` apenas quando apropriado.

---

[Anterior](capitulo-15-02-excecoes-avancadas.md) | [Índice](README.md) | [Próximo](capitulo-16-01-string-stl.md)
