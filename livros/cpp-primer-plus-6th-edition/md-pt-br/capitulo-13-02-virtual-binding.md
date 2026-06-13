# Capítulo 13 — Ligação Estática e Dinâmica, Classes Base Abstratas

> Tradução não oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Navegação: [Anterior](capitulo-13-01-heranca.md) | [Índice](README.md) | [Próximo](capitulo-13-03-revisao-resumo.md)

---

## Ligação Estática e Dinâmica (Static and Dynamic Binding)

Qual bloco de código executável é usado quando um programa chama uma função? O compilador tem a responsabilidade de responder a essa pergunta. Interpretar uma chamada de função no código-fonte como a execução de um bloco específico de código de função é chamado de *ligar* (binding) o nome da função. Com C, a tarefa é simples porque cada nome de função corresponde a uma função distinta. Com C++, a tarefa é mais complexa por causa da sobrecarga de funções. O compilador deve examinar os argumentos da função, bem como o nome da função, para descobrir qual função usar. No entanto, esse tipo de ligação é uma tarefa que um compilador C ou C++ poderia realizar durante o processo de compilação; a ligação que ocorre durante a compilação é chamada de *ligação estática* (static binding) ou *ligação antecipada* (early binding). No entanto, as funções virtuais tornam o trabalho ainda mais difícil. Como mostrado na Listagem 13.10, a decisão de qual função usar não pode ser feita no momento da compilação porque o compilador não sabe que tipo de objeto o usuário vai escolher criar. Portanto, o compilador deve gerar código que permita selecionar o método virtual correto enquanto o programa é executado; isso é chamado de *ligação dinâmica* (dynamic binding) ou *ligação tardia* (late binding).

Agora que você viu os métodos virtuais em funcionamento, vamos examinar esse processo em maior profundidade, começando por como o C++ lida com compatibilidade de tipos para ponteiros e referências.

### Compatibilidade de Tipos com Ponteiros e Referências

A ligação dinâmica em C++ está associada a métodos invocados por ponteiros e referências, e isso é governado, em parte, pelo processo de herança. Uma maneira pela qual a herança pública modela a relação *é-um* é na forma como ela trata ponteiros e referências para objetos. Normalmente, o C++ não permite que você atribua um endereço de um tipo a um ponteiro de outro tipo. Nem permite que uma referência para um tipo se refira a outro tipo:

```cpp
double x = 2.5;
int * pi = &x; // atribuição inválida, tipos de ponteiro incompatíveis
long & rl = x; // atribuição inválida, tipo de referência incompatível
```

No entanto, como você viu, uma referência ou um ponteiro para uma classe base pode se referir a um objeto de classe derivada sem usar uma conversão de tipo explícita. Por exemplo, as seguintes inicializações são permitidas:

```cpp
LataoPlus dilly ("Annie Dill", 493222, 2000);
Latao * pb = &dilly; // ok
Latao & rb = dilly;  // ok
```

Converter uma referência ou ponteiro de classe derivada para uma referência ou ponteiro de classe base é chamado de *upcasting* (conversão ascendente), e é sempre permitido para herança pública sem necessidade de uma conversão de tipo explícita. Essa regra faz parte de expressar a relação *é-um*. Um objeto `LataoPlus` é um objeto `Latao` no sentido de que herda todos os membros de dados e funções membro de um objeto `Latao`. Portanto, tudo que você pode fazer com um objeto `Latao`, você pode fazer com um objeto `LataoPlus`. Uma função projetada para lidar com uma referência a `Latao` pode, sem receio de criar problemas, executar as mesmas ações em um objeto `LataoPlus`. O mesmo raciocínio se aplica quando você passa um ponteiro para um objeto como argumento de função. O upcasting é transitivo. Ou seja, se você derivar uma classe `LataoMaisPlus` de `LataoPlus`, então um ponteiro ou referência a `Latao` pode se referir a um objeto `Latao`, um objeto `LataoPlus` ou um objeto `LataoMaisPlus`.

O processo oposto, converter um ponteiro ou referência de classe base para um ponteiro ou referência de classe derivada, é chamado de *downcasting* (conversão descendente), e não é permitido sem uma conversão de tipo explícita. A razão para essa restrição é que a relação *é-um* não é, em geral, simétrica. Uma classe derivada poderia adicionar novos membros de dados, e as funções membro de classe que usam esses membros de dados não se aplicariam à classe base. Por exemplo, suponha que você derive uma classe `Cantor` de uma classe `Funcionario`, adicionando um membro de dados representando a extensão vocal de um cantor e uma função membro, chamada `extensao()`, que informa o valor da extensão vocal. Não faria sentido aplicar o método `extensao()` a um objeto `Funcionario`. Mas se o downcasting implícito fosse permitido, você poderia acidentalmente definir um ponteiro-para-`Cantor` para o endereço de um objeto `Funcionario` e usar o ponteiro para invocar o método `extensao()`.

O upcasting também ocorre para chamadas de função com parâmetros formais de referência ou ponteiro para a classe base. Considere o seguinte fragmento de código, e suponha que cada função chame o método virtual `VerConta()`:

```cpp
void fr(Latao & rb);  // usa rb.VerConta()
void fp(Latao * pb);  // usa pb->VerConta()
void fv(Latao b);     // usa b.VerConta()
int main()
{
    Latao b("Billy Bee", 123432, 10000.0);
    LataoPlus bp("Betty Beep", 232313, 12345.0);
    fr(b);  // usa Latao::VerConta()
    fr(bp); // usa LataoPlus::VerConta()
    fp(b);  // usa Latao::VerConta()
    fp(bp); // usa LataoPlus::VerConta()
    fv(b);  // usa Latao::VerConta()
    fv(bp); // usa Latao::VerConta()
...
}
```

Passar por valor faz com que apenas o componente `Latao` de um objeto `LataoPlus` seja passado para a função `fv()`. Mas o upcasting implícito que ocorre com referências e ponteiros faz com que as funções `fr()` e `fp()` usem `Latao::VerConta()` para objetos `Latao` e `LataoPlus::VerConta()` para objetos `LataoPlus`.

O upcasting implícito torna possível que um ponteiro ou referência para a classe base se refira a um objeto da classe base ou a um objeto da classe derivada, e isso produz a necessidade de ligação dinâmica. As funções membro virtuais são a resposta do C++ a essa necessidade.

### Funções Membro Virtuais e Ligação Dinâmica

Vamos revisitar o processo de invocar um método com uma referência ou ponteiro. Considere o seguinte código:

```cpp
LataoPlus ofelia; // objeto de classe derivada
Latao * bp;       // ponteiro de classe base
bp = &ofelia;     // ponteiro Latao para objeto LataoPlus
bp->VerConta();   // qual versão?
```

Como discutido antes, se `VerConta()` não é declarado como virtual na classe base, `bp->VerConta()` vai pelo tipo do ponteiro (`Latao *`) e invoca `Latao::VerConta()`. O tipo do ponteiro é conhecido no momento da compilação, portanto o compilador pode ligar `VerConta()` a `Latao::VerConta()` no momento da compilação. Em resumo, o compilador usa ligação estática para métodos não virtuais.

Mas se `VerConta()` é declarado como virtual na classe base, `bp->VerConta()` vai pelo tipo do objeto (`LataoPlus`) e invoca `LataoPlus::VerConta()`. Neste exemplo, você pode ver que o tipo do objeto é `LataoPlus`, mas, em geral (como na Listagem 13.10), o tipo do objeto pode ser determinado somente quando o programa está em execução. Portanto, o compilador gera código que liga `VerConta()` a `Latao::VerConta()` ou `LataoPlus::VerConta()`, dependendo do tipo do objeto, enquanto o programa executa. Em resumo, o compilador usa ligação dinâmica para métodos virtuais.

Na maioria dos casos, a ligação dinâmica é uma coisa boa porque permite que um programa escolha o método projetado para um tipo específico. Dado esse fato, você pode estar se perguntando sobre o seguinte:

- Por que ter dois tipos de ligação?
- Se a ligação dinâmica é tão boa, por que ela não é o padrão?
- Como ela funciona?

Examinaremos as respostas a essas perguntas a seguir.

### Por Que Dois Tipos de Ligação e Por Que Estática É o Padrão

Se a ligação dinâmica permite que você redefina métodos de classe, mas a ligação estática a estraga parcialmente, por que ter ligação estática? Há duas razões: eficiência e modelo conceitual.

Primeiro, vamos considerar a eficiência. Para que um programa possa tomar uma decisão em tempo de execução, ele deve ter alguma forma de rastrear que tipo de objeto um ponteiro ou referência para a classe base se refere, e isso implica alguma sobrecarga de processamento adicional. Se, por exemplo, você projetar uma classe que não será usada como classe base para herança, não precisará de ligação dinâmica. Da mesma forma, se você tiver uma classe derivada, como no exemplo `JogadorAvaliado`, que não redefine nenhum método, não precisará de ligação dinâmica. Nesses casos, faz sentido usar ligação estática e ganhar um pouco de eficiência. O fato de que a ligação estática é mais eficiente é a razão pela qual ela é a escolha padrão para C++. Stroustrup diz que um dos princípios norteadores do C++ é que você não deve ter que pagar (em uso de memória ou tempo de processamento) por funcionalidades que não usa. Portanto, você deve recorrer às funções virtuais somente se o projeto do programa precisar delas.

Em seguida, vamos considerar o modelo conceitual. Quando você projeta uma classe, pode ter funções membro que não deseja que sejam redefinidas nas classes derivadas. Por exemplo, a função `Latao::Saldo()`, que retorna o saldo da conta, parece ser uma função que não deve ser redefinida. Ao tornar essa função não virtual, você realiza duas coisas. Primeiro, você a torna mais eficiente. Segundo, você anuncia que é sua intenção que essa função não seja redefinida. Isso sugere reservar o rótulo `virtual` apenas para métodos que você espera que sejam redefinidos.

> **Dica**
> Se um método em uma classe base será redefinido em uma classe derivada, você deve torná-lo virtual. Se o método não deve ser redefinido, você deve torná-lo não virtual.

Claro, ao projetar uma classe, nem sempre é óbvio em qual categoria um método se encaixa. Como muitos aspectos da vida real, o projeto de classes não é um processo linear.

### Como Funções Virtuais Funcionam

O C++ especifica como funções virtuais devem se comportar, mas deixa a implementação a cargo do escritor do compilador. Você não precisa conhecer o método de implementação para usar funções virtuais, mas ver como ele é feito pode ajudar a compreender melhor os conceitos, então vamos dar uma olhada.

A maneira usual com que os compiladores lidam com funções virtuais é adicionar um membro oculto a cada objeto. O membro oculto contém um ponteiro para um array de endereços de funções. Esse array é geralmente chamado de *tabela de funções virtuais* (vtbl — virtual function table). A vtbl contém os endereços das funções virtuais declaradas para objetos daquela classe. Por exemplo, um objeto de uma classe base contém um ponteiro para uma tabela de endereços de todas as funções virtuais para aquela classe. Um objeto de uma classe derivada contém um ponteiro para uma tabela separada de endereços. Se a classe derivada fornece uma nova definição de uma função virtual, a vtbl contém o endereço da nova função. Se a classe derivada não redefine a função virtual, a vtbl contém o endereço da versão original da função. Se a classe derivada define uma nova função e a torna virtual, seu endereço é adicionado à vtbl. Observe que independentemente de você definir 1 ou 10 funções virtuais para uma classe, você adiciona apenas um membro de endereço a um objeto; é o tamanho da tabela que varia.

Quando você chama uma função virtual, o programa procura o endereço da vtbl armazenado em um objeto e vai para a tabela correspondente de endereços de funções. Se você usa a primeira função virtual definida na declaração de classe, o programa usa o primeiro endereço de função no array e executa a função que tem aquele endereço. Se você usa a terceira função virtual na declaração de classe, o programa usa a função cujo endereço está no terceiro elemento do array.

Em resumo, usar funções virtuais tem os seguintes custos modestos em memória e velocidade de execução:

- Cada objeto tem seu tamanho aumentado pela quantidade necessária para armazenar um endereço.
- Para cada classe, o compilador cria uma tabela (um array) de endereços de funções virtuais.
- Para cada chamada de função, há um passo extra de ir a uma tabela para procurar um endereço.

Lembre-se de que, embora funções não virtuais sejam ligeiramente mais eficientes do que funções virtuais, elas não fornecem ligação dinâmica.

---

## O Que Saber Sobre Métodos Virtuais

Já discutimos os pontos principais sobre métodos virtuais:

- Iniciar a declaração de um método de classe com a palavra-chave `virtual` em uma classe base torna a função virtual para a classe base e para todas as classes derivadas dela, incluindo classes derivadas das classes derivadas, e assim por diante.
- Se um método virtual é invocado usando uma referência a um objeto ou usando um ponteiro para um objeto, o programa usa o método definido para o tipo do objeto em vez do método definido para o tipo da referência ou ponteiro. Isso é chamado de ligação dinâmica, ou tardia. Esse comportamento é importante porque é sempre válido que um ponteiro ou referência de classe base se refira a um objeto de um tipo derivado.
- Se você está definindo uma classe que será usada como classe base para herança, você deve declarar como funções virtuais os métodos de classe que podem ter que ser redefinidos nas classes derivadas.

Há várias outras coisas que você pode precisar saber sobre métodos virtuais, algumas das quais já foram mencionadas de passagem. Vamos examiná-las a seguir.

### Construtores

Construtores não podem ser virtuais. Criar um objeto derivado invoca um construtor da classe derivada, não um construtor da classe base. O construtor da classe derivada usa então um construtor da classe base, mas essa sequência é distinta do mecanismo de herança. Assim, uma classe derivada não herda os construtores da classe base, portanto geralmente não há muito sentido em torná-los virtuais de qualquer maneira.

### Destrutores

Destrutores devem ser virtuais, a menos que uma classe não deva ser usada como classe base. Por exemplo, suponha que `Funcionario` é uma classe base e `Cantor` é uma classe derivada que adiciona um membro `char *` que aponta para memória alocada por `new`. Então, quando um objeto `Cantor` expira, é vital que o destrutor `~Cantor()` seja chamado para liberar essa memória.

Agora considere o seguinte código:

```cpp
Funcionario * fe = new Cantor; // válido porque Funcionario é base de Cantor
...
delete fe; // ~Funcionario() ou ~Cantor()?
```

Se a ligação estática padrão se aplica, a instrução `delete` invoca o destrutor `~Funcionario()`. Isso libera a memória apontada pelos componentes `Funcionario` do objeto `Cantor`, mas não a memória apontada pelos novos membros de classe. No entanto, se os destrutores são virtuais, o mesmo código invoca o destrutor `~Cantor()`, que libera a memória apontada pelo componente `Cantor`, e então chama o destrutor `~Funcionario()` para liberar a memória apontada pelo componente `Funcionario`.

Observe que isso implica que mesmo que uma classe base não precise dos serviços de um destrutor explícito, você não deve depender do construtor padrão. Em vez disso, você deve fornecer um destrutor virtual, mesmo que ele não tenha nada a fazer:

```cpp
virtual ~ClasseBase() { }
```

Aliás, não é um erro que uma classe tenha um destrutor virtual mesmo que não se destine a ser uma classe base; é apenas uma questão de eficiência.

> **Dica**
> Normalmente, você deve fornecer uma classe base com um destrutor virtual, mesmo que a classe não precise de destrutor.

### Funções Amigas (Friends)

*Friends* (funções amigas) não podem ser funções virtuais porque friends não são membros de classe, e apenas membros podem ser virtuais. Se isso apresenta um problema para um projeto, você pode contorná-lo fazendo a função amiga usar funções membro virtuais internamente.

### Sem Redefinição

Se uma classe derivada não define uma função (virtual ou não), a classe usará a versão da classe base da função. Se uma classe derivada faz parte de uma longa cadeia de derivações, ela usará a versão mais recentemente definida da função. A exceção é se as versões base estão ocultas, como descrito a seguir.

### Redefinição Oculta de Métodos

Suponha que você crie algo como o seguinte:

```cpp
class Moradia
{
public:
    virtual void mostrarVantagens(int a) const;
...
};
class Cabana : public Moradia
{
public:
    virtual void mostrarVantagens() const;
...
};
```

Isso causa um problema. Você pode receber um aviso do compilador similar ao seguinte:

```
Warning: Cabana::mostrarVantagens(void) esconde Moradia::mostrarVantagens(int)
```

Ou talvez você não receba um aviso. De qualquer maneira, o código tem as seguintes implicações:

```cpp
Cabana casinha;
casinha.mostrarVantagens();  // válido
casinha.mostrarVantagens(5); // inválido
```

A nova definição define uma função `mostrarVantagens()` que não recebe argumentos. Em vez de resultar em duas versões sobrecarregadas da função, essa redefinição *oculta* a versão da classe base que recebe um argumento `int`. Em resumo, redefinir métodos herdados não é uma variação de sobrecarga. Se você redefine uma função em uma classe derivada, ela não apenas substitui a declaração da classe base com a mesma assinatura de função. Em vez disso, ela oculta *todos* os métodos da classe base com o mesmo nome, independentemente das assinaturas dos argumentos.

Esse fato leva a alguns princípios práticos. Primeiro, se você redefinir um método herdado, precisa garantir que o protótipo original seja correspondido exatamente. Uma exceção relativamente nova a essa regra é que um tipo de retorno que é referência ou ponteiro para uma classe base pode ser substituído por uma referência ou ponteiro para a classe derivada. Essa funcionalidade é chamada de *covariância do tipo de retorno* (covariance of return type) porque o tipo de retorno pode variar em paralelo com o tipo da classe:

```cpp
class Moradia
{
public:
    // um método base
    virtual Moradia & construir(int n);
    ...
};
class Cabana : public Moradia
{
public:
    // um método derivado com tipo de retorno covariante
    virtual Cabana & construir(int n); // mesma assinatura de função
    ...
};
```

Observe que essa exceção se aplica apenas aos valores de retorno, não aos argumentos.

Segundo, se a declaração da classe base é sobrecarregada, você precisa redefinir todas as versões da classe base na classe derivada:

```cpp
class Moradia
{
public:
    // três mostrarVantagens() sobrecarregados
    virtual void mostrarVantagens(int a) const;
    virtual void mostrarVantagens(double x) const;
    virtual void mostrarVantagens() const;
    ...
};
class Cabana : public Moradia
{
public:
    // três mostrarVantagens() redefinidos
    virtual void mostrarVantagens(int a) const;
    virtual void mostrarVantagens(double x) const;
    virtual void mostrarVantagens() const;
    ...
};
```

Se você redefine apenas uma versão, as outras duas ficam ocultas e não podem ser usadas por objetos da classe derivada. Observe que se nenhuma mudança for necessária, a redefinição pode simplesmente chamar a versão da classe base:

```cpp
void Cabana::mostrarVantagens() const { Moradia::mostrarVantagens(); }
```

---

## Controle de Acesso: protected

Até agora, os exemplos de classe neste livro usaram as palavras-chave `public` e `private` para controlar o acesso a membros de classe. Há mais uma categoria de acesso, indicada com a palavra-chave `protected`. A palavra-chave `protected` é semelhante a `private` no sentido de que o mundo exterior pode acessar membros de classe em uma seção `protected` apenas usando membros públicos de classe. A diferença entre `private` e `protected` entra em jogo apenas dentro de classes derivadas da classe base. Membros de uma classe derivada podem acessar membros protegidos de uma classe base diretamente, mas não podem acessar diretamente membros privados da classe base. Portanto, membros na categoria protegida se comportam como membros `private` no que diz respeito ao mundo exterior, mas se comportam como membros `public` no que diz respeito às classes derivadas.

Por exemplo, suponha que a classe `Latao` declarasse o membro `saldo` como protegido:

```cpp
class Latao
{
protected:
    double saldo;
...
};
```

Nesse caso, a classe `LataoPlus` poderia acessar `saldo` diretamente sem usar métodos de `Latao`. Por exemplo, o núcleo de `LataoPlus::Sacar()` poderia ser escrito desta forma:

```cpp
void LataoPlus::Sacar(double val)
{
    if (val < 0)
        cout << "O valor do saque deve ser positivo; "
             << "saque cancelado.\n";
    else if (val <= saldo) // acessa saldo diretamente
        saldo -= val;
    else if ( val <= saldo + limiteEmprestimo - deveAoBanco)
    {
        double adiantamento = val - saldo;
        deveAoBanco += adiantamento * (1.0 + taxa);
        cout << "Adiantamento do banco: $" << adiantamento << endl;
        cout << "Encargo financeiro: $" << adiantamento * taxa << endl;
        Depositar(adiantamento);
        saldo -= val;
    }
    else
        cout << "Limite de crédito excedido. Transação cancelada.\n";
}
```

Usar membros de dados protegidos pode simplificar a escrita do código, mas tem uma falha de projeto. Por exemplo, continuando com o exemplo de `LataoPlus`, se `saldo` fosse protegido, você poderia escrever código como este:

```cpp
void LataoPlus::Redefinir(double val)
{
    saldo = val;
}
```

A classe `Latao` foi projetada de forma que a interface `Depositar()` e `Sacar()` fornece o único meio para alterar `saldo`. Mas o método `Redefinir()` essencialmente torna `saldo` uma variável pública no que diz respeito a objetos `LataoPlus`, ignorando, por exemplo, as salvaguardas encontradas em `Sacar()`.

> **Cuidado**
> Você deve preferir o controle de acesso privado em vez de protegido para membros de dados de classe, e deve usar métodos da classe base para fornecer às classes derivadas acesso aos dados da classe base.

No entanto, o controle de acesso protegido pode ser bastante útil para funções membro, dando às classes derivadas acesso a funções internas que não estão disponíveis publicamente.

---

## Classes Base Abstratas (Abstract Base Classes)

Até agora, você viu herança simples e herança pública polimórfica mais intrincada. O próximo passo em sofisticação crescente é a *classe base abstrata* (ABC — abstract base class). Vamos examinar algumas situações de programação que fornecem o contexto para as ABCs.

Às vezes, aplicar a regra *é-um* não é tão simples quanto pode parecer. Suponha, por exemplo, que você está desenvolvendo um programa gráfico que deve representar, entre outras coisas, círculos e elipses. Um círculo é um caso especial de elipse: é uma elipse cujo eixo maior é o mesmo que o eixo menor. Portanto, todos os círculos são elipses, e é tentador derivar uma classe `Circulo` de uma classe `Elipse`. Mas ao entrar nos detalhes, você pode encontrar problemas.

Para ver isso, primeiro considere o que você pode incluir como parte de uma classe `Elipse`. Os membros de dados podem incluir as coordenadas do centro da elipse, o semieixo maior (metade do diâmetro longo), o semieixo menor (metade do diâmetro curto) e um ângulo de orientação que fornece o ângulo do eixo de coordenada horizontal até o semieixo maior. Além disso, a classe poderia incluir métodos para mover a elipse, retornar a área da elipse, rotacionar a elipse e escalar os semieixos maior e menor:

```cpp
class Elipse
{
private:
    double x;     // coordenada x do centro da elipse
    double y;     // coordenada y do centro da elipse
    double a;     // semieixo maior
    double b;     // semieixo menor
    double angulo; // ângulo de orientação em graus
    ...
public:
    ...
    void Mover(int nx, int ny) { x = nx; y = ny; }
    virtual double Area() const { return 3.14159 * a * b; }
    virtual void Rotacionar(double nang) { angulo += nang; }
    virtual void Escalar(double sa, double sb) { a *= sa; b *= sb; }
    ...
};
```

Agora suponha que você derive uma classe `Circulo` da classe `Elipse`:

```cpp
class Circulo : public Elipse
{
    ...
};
```

Embora um círculo seja uma elipse, essa derivação é estranha. Por exemplo, um círculo precisa de apenas um valor, seu raio, para descrever seu tamanho e forma, em vez de ter um semieixo maior (`a`) e um semieixo menor (`b`). Os construtores de `Circulo` podem cuidar disso atribuindo o mesmo valor a ambos os membros `a` e `b`, mas então você tem representação redundante da mesma informação. O parâmetro `angulo` e o método `Rotacionar()` não fazem realmente sentido para um círculo, e o método `Escalar()`, como está, pode transformar um círculo em não-círculo escalando os dois eixos de forma diferente. Você pode tentar consertar as coisas com truques, como colocar um método `Rotacionar()` redefinido na seção privada da classe `Circulo`, para que `Rotacionar()` não possa ser usado publicamente com um círculo, mas, no geral, parece mais simples definir uma classe `Circulo` sem usar herança:

```cpp
class Circulo // sem herança
{
private:
    double x; // coordenada x do centro do círculo
    double y; // coordenada y do centro do círculo
    double r; // raio
    ...
public:
    ...
    void Mover(int nx, int ny) { x = nx; y = ny; }
    double Area() const { return 3.14159 * r * r; }
    void Escalar(double sr) { r *= sr; }
    ...
};
```

Agora a classe tem apenas os membros de que precisa. No entanto, essa solução também parece fraca. As classes `Circulo` e `Elipse` têm muito em comum, mas defini-las separadamente ignora esse fato.

Há outra solução: você pode abstrair das classes `Elipse` e `Circulo` o que elas têm em comum e colocar essas funcionalidades em uma ABC. Em seguida, você deriva tanto a classe `Circulo` quanto a classe `Elipse` da ABC. Então, por exemplo, você pode usar um array de ponteiros para a classe base para gerenciar uma mistura de objetos `Elipse` e `Circulo` — ou seja, você pode usar uma abordagem polimórfica. Nesse caso, o que as duas classes têm em comum são as coordenadas do centro da forma; um método `Mover()`, que é o mesmo para as duas; e um método `Area()`, que funciona de forma diferente para as duas classes. De fato, o método `Area()` não pode sequer ser implementado para a ABC porque ela não tem os membros de dados necessários. O C++ tem uma maneira de fornecer uma função não implementada usando uma *função virtual pura* (pure virtual function). Uma função virtual pura tem `= 0` no final de sua declaração, como mostrado para o método `Area()`:

```cpp
class BaseElipse // classe base abstrata
{
private:
    double x; // coordenada x do centro
    double y; // coordenada y do centro
    ...
public:
    BaseElipse(double x0 = 0, double y0 = 0) : x(x0), y(y0) {}
    virtual ~BaseElipse() {}
    void Mover(int nx, int ny) { x = nx; y = ny; }
    virtual double Area() const = 0; // uma função virtual pura
    ...
};
```

Quando uma declaração de classe contém uma função virtual pura, você não pode criar um objeto daquela classe. A ideia é que classes com funções virtuais puras existem somente para servir como classes base. Para que uma classe seja genuinamente uma ABC, ela deve ter pelo menos uma função virtual pura. É o `= 0` no protótipo que torna uma função virtual uma função virtual pura. No caso do método `Area()`, a função não tem definição, mas o C++ permite até que uma função virtual pura tenha uma definição. Por exemplo, talvez todos os métodos base sejam como `Mover()`, no sentido de que eles podem ser definidos para a classe base, mas você ainda precisa tornar a classe abstrata. Você poderia então tornar o protótipo virtual:

```cpp
void Mover(int nx, int ny) = 0;
```

Isso torna a classe base abstrata. Mas então você ainda pode fornecer uma definição no arquivo de implementação:

```cpp
void BaseElipse::Mover(int nx, int ny) { x = nx; y = ny; }
```

Em resumo, o `= 0` no protótipo indica que a classe é uma classe base abstrata e que a classe não precisa necessariamente definir a função.

Agora você pode derivar as classes `Elipse` e `Circulo` da classe `BaseElipse`, adicionando os membros necessários para completar cada classe. Um ponto a notar é que a classe `Circulo` sempre representa círculos, enquanto a classe `Elipse` representa elipses que também podem ser círculos. No entanto, um círculo da classe `Elipse` pode ser redimensionado para um não-círculo, enquanto um círculo da classe `Circulo` deve permanecer um círculo.

Um programa usando essas classes poderia criar objetos `Elipse` e objetos `Circulo`, mas nenhum objeto `BaseElipse`. Como objetos `Circulo` e `Elipse` têm a mesma classe base, uma coleção de tais objetos pode ser gerenciada com um array de ponteiros para `BaseElipse`. Classes como `Circulo` e `Elipse` às vezes são chamadas de *classes concretas* (concrete classes) para indicar que você pode criar objetos desses tipos.

Em resumo, uma ABC descreve uma interface que usa pelo menos uma função virtual pura, e classes derivadas de uma ABC usam funções virtuais regulares para implementar a interface em termos das propriedades da classe derivada específica.

### Aplicando o Conceito ABC

Você provavelmente quer ver um exemplo completo de uma ABC, então vamos aplicar o conceito para representar as contas `Latao` e `LataoPlus`, começando com uma ABC chamada `ContaABC`. Essa classe deve conter todos os métodos e membros de dados que são comuns às classes `Latao` e `LataoPlus`. Os métodos que devem funcionar de forma diferente para a classe `LataoPlus` em vez de para a classe `Latao` devem ser declarados como funções virtuais. Pelo menos uma função virtual deve ser uma função virtual pura para tornar a classe `ContaABC` abstrata.

A Listagem 13.11 é um arquivo de cabeçalho que declara a classe `ContaABC` (uma ABC) e as classes `Latao` e `LataoPlus` (ambas classes concretas). Para facilitar o acesso da classe derivada aos dados da classe base, `ContaABC` fornece alguns métodos protegidos. Lembre-se de que métodos protegidos são métodos que métodos de classe derivada podem chamar, mas que não fazem parte da interface pública para objetos de classe derivada. `ContaABC` também fornece uma função membro protegida para lidar com a formatação anteriormente tratada por funções não membro. Além disso, a classe `ContaABC` tem duas funções virtuais puras, portanto é, de fato, uma classe abstrata.

**Listagem 13.11 — contaabc.h**

```cpp
// contaabc.h -- classes de contas bancárias
#ifndef CONTAABC_H_
#define CONTAABC_H_
#include <iostream>
#include <string>
// Classe Base Abstrata
class ContaABC
{
private:
    std::string nomeCompleto;
    long numConta;
    double saldo;
protected:
    struct Formatacao
    {
        std::ios_base::fmtflags flag;
        std::streamsize pr;
    };
    const std::string & NomeCompleto() const {return nomeCompleto;}
    long NumConta() const {return numConta;}
    Formatacao DefinirFormato() const;
    void Restaurar(Formatacao & f) const;
public:
    ContaABC(const std::string & s = "Ninguém", long nc = -1,
             double sal = 0.0);
    void Depositar(double val);
    virtual void Sacar(double val) = 0; // função virtual pura
    double Saldo() const {return saldo;};
    virtual void VerConta() const = 0;  // função virtual pura
    virtual ~ContaABC() {}
};
// Classe Conta Latão
class Latao : public ContaABC
{
public:
    Latao(const std::string & s = "Ninguém", long nc = -1,
         double sal = 0.0) : ContaABC(s, nc, sal) { }
    virtual void Sacar(double val);
    virtual void VerConta() const;
    virtual ~Latao() {}
};
// Classe Conta Latão Plus
class LataoPlus : public ContaABC
{
private:
    double limiteEmprestimo;
    double taxa;
    double deveAoBanco;
public:
    LataoPlus(const std::string & s = "Ninguém", long nc = -1,
         double sal = 0.0, double le = 500,
         double t = 0.10);
    LataoPlus(const Latao & la, double le = 500, double t = 0.1);
    virtual void VerConta() const;
    virtual void Sacar(double val);
    void RedefinirLimite(double m) { limiteEmprestimo = m; }
    void RedefinirTaxa(double t) { taxa = t; };
    void RedefinirDebito() { deveAoBanco = 0; }
};
#endif
```

O próximo passo é implementar os métodos que ainda não têm definições inline. A Listagem 13.12 faz isso.

**Listagem 13.12 — contaabc.cpp**

```cpp
// contaabc.cpp -- métodos das classes de contas bancárias
#include <iostream>
#include "contaabc.h"
using std::cout;
using std::ios_base;
using std::endl;
using std::string;
// Classe Base Abstrata
ContaABC::ContaABC(const string & s, long nc, double sal)
{
    nomeCompleto = s;
    numConta = nc;
    saldo = sal;
}
void ContaABC::Depositar(double val)
{
    if (val < 0)
        cout << "Depósito negativo não permitido; "
             << "depósito cancelado.\n";
    else
        saldo += val;
}
void ContaABC::Sacar(double val)
{
    saldo -= val;
}
// métodos protegidos de formatação
ContaABC::Formatacao ContaABC::DefinirFormato() const
{
    // configura formato ###.##
    Formatacao f;
    f.flag =
        cout.setf(ios_base::fixed, ios_base::floatfield);
    f.pr = cout.precision(2);
    return f;
}
void ContaABC::Restaurar(Formatacao & f) const
{
    cout.setf(f.flag, ios_base::floatfield);
    cout.precision(f.pr);
}
// Métodos de Latao
void Latao::Sacar(double val)
{
    if (val < 0)
        cout << "O valor do saque deve ser positivo; "
             << "saque cancelado.\n";
    else if (val <= Saldo())
        ContaABC::Sacar(val);
    else
        cout << "O valor do saque de $" << val
             << " excede seu saldo.\n"
             << "Saque cancelado.\n";
}
void Latao::VerConta() const
{
    Formatacao f = DefinirFormato();
    cout << "Cliente Latão: " << NomeCompleto() << endl;
    cout << "Número de Conta: " << NumConta() << endl;
    cout << "Saldo: $" << Saldo() << endl;
    Restaurar(f);
}
// Métodos de LataoPlus
LataoPlus::LataoPlus(const string & s, long nc, double sal,
        double le, double t) : ContaABC(s, nc, sal)
{
    limiteEmprestimo = le;
    deveAoBanco = 0.0;
    taxa = t;
}
LataoPlus::LataoPlus(const Latao & la, double le, double t)
        : ContaABC(la) // usa construtor de cópia implícito
{
    limiteEmprestimo = le;
    deveAoBanco = 0.0;
    taxa = t;
}
void LataoPlus::VerConta() const
{
    Formatacao f = DefinirFormato();
    cout << "Cliente Latão Plus: " << NomeCompleto() << endl;
    cout << "Número de Conta: " << NumConta() << endl;
    cout << "Saldo: $" << Saldo() << endl;
    cout << "Limite de empréstimo: $" << limiteEmprestimo << endl;
    cout << "Deve ao banco: $" << deveAoBanco << endl;
    cout.precision(3);
    cout << "Taxa de empréstimo: " << 100 * taxa << "%\n";
    Restaurar(f);
}
void LataoPlus::Sacar(double val)
{
    Formatacao f = DefinirFormato();
    double sal = Saldo();
    if (val <= sal)
        ContaABC::Sacar(val);
    else if ( val <= sal + limiteEmprestimo - deveAoBanco)
    {
        double adiantamento = val - sal;
        deveAoBanco += adiantamento * (1.0 + taxa);
        cout << "Adiantamento do banco: $" << adiantamento << endl;
        cout << "Encargo financeiro: $" << adiantamento * taxa << endl;
        Depositar(adiantamento);
        ContaABC::Sacar(val);
    }
    else
        cout << "Limite de crédito excedido. Transação cancelada.\n";
    Restaurar(f);
}
```

Os métodos protegidos `NomeCompleto()` e `NumConta()` fornecem acesso somente leitura aos membros de dados `nomeCompleto` e `numConta` e tornam possível personalizar `VerConta()` um pouco mais individualmente para cada classe derivada.

Esta versão faz um par de melhorias na implementação da formatação. A versão anterior usava duas chamadas de função para definir a formatação e uma para restaurá-la:

```cpp
formato estadoInicial = definirFormato();
precis prec = cout.precision(2);
...
restaurar(estadoInicial, prec); // restaura o formato original
```

A nova versão define uma estrutura para armazenar os dois valores de formatação e usa essa estrutura para definir e restaurar formatos com apenas duas chamadas:

```cpp
struct Formatacao
{
    std::ios_base::fmtflags flag;
    std::streamsize pr;
};
...
Formatacao f = DefinirFormato();
...
Restaurar(f);
```

É uma apresentação mais organizada.

Um problema com a versão anterior era que as funções originais `definirFormato()` e `restaurar()` eram funções independentes, então esses nomes de funções poderiam conflitar com funções definidas pelo cliente de mesmo nome. Há várias formas de resolver esse problema. Uma é declarar ambas as funções como estáticas, tornando-as privadas para o arquivo `latao.cpp` ou para seu sucessor, `contaabc.cpp`. Uma segunda é colocar ambas as funções e a definição de `struct Formatacao` em um namespace. Mas um dos tópicos para este exemplo é o acesso protegido, então este exemplo coloca a definição da estrutura e as funções na parte protegida da definição de classe. Isso as torna disponíveis para a classe base e a classe derivada ao mesmo tempo em que as oculta do mundo exterior.

Esta nova implementação das contas `Latao` e `LataoPlus` pode ser usada da mesma forma que a anterior porque os métodos de classe têm os mesmos nomes e interfaces de antes. Por exemplo, para converter a Listagem 13.10 para usar a nova implementação, você só precisa executar estes passos para converter `usarlatao2.cpp` em um arquivo `usarlatao3.cpp`:

- Linke `usarlatao2.cpp` com `contaabc.cpp` em vez de com `latao.cpp`.
- Inclua `contaabc.h` em vez de `latao.h`.
- Substitua `Latao * p_clientes[CLIENTES];` por `ContaABC * p_clientes[CLIENTES];`

A Listagem 13.13 mostra o arquivo resultante, renomeado `usarlatao3.cpp`.

**Listagem 13.13 — usarlatao3.cpp**

```cpp
// usarlatao3.cpp -- exemplo polimórfico usando uma classe base abstrata
// compilar com contaabc.cpp
#include <iostream>
#include <string>
#include "contaabc.h"
const int CLIENTES = 4;
int main()
{
    using std::cin;
    using std::cout;
    using std::endl;
    ContaABC * p_clientes[CLIENTES];
    std::string temp;
    long tempnum;
    double tempsal;
    char tipo;
    for (int i = 0; i < CLIENTES; i++)
    {
        cout << "Digite o nome do cliente: ";
        getline(cin, temp);
        cout << "Digite o número da conta do cliente: ";
        cin >> tempnum;
        cout << "Digite o saldo inicial: $";
        cin >> tempsal;
        cout << "Digite 1 para Conta Latão ou "
             << "2 para Conta Latão Plus: ";
        while (cin >> tipo && (tipo != '1' && tipo != '2'))
            cout << "Digite 1 ou 2: ";
        if (tipo == '1')
            p_clientes[i] = new Latao(temp, tempnum, tempsal);
        else
        {
            double tmax, ttaxa;
            cout << "Digite o limite de cheque especial: $";
            cin >> tmax;
            cout << "Digite a taxa de juros "
                 << "como fração decimal: ";
            cin >> ttaxa;
            p_clientes[i] = new LataoPlus(temp, tempnum, tempsal,
                                    tmax, ttaxa);
        }
        while (cin.get() != '\n')
            continue;
    }
    cout << endl;
    for (int i = 0; i < CLIENTES; i++)
    {
        p_clientes[i]->VerConta();
        cout << endl;
    }
    for (int i = 0; i < CLIENTES; i++)
    {
        delete p_clientes[i]; // libera memória
    }
    cout << "Concluído.\n";
    return 0;
}
```

O programa em si se comporta da mesma forma que a versão sem classe base abstrata, portanto, dada a mesma entrada que para a Listagem 13.10, a saída seria a mesma.

### Filosofia ABC

A metodologia ABC é uma maneira muito mais sistemática e disciplinada de abordar a herança do que a abordagem mais ad hoc e improvisada usada pelo exemplo de `JogadorAvaliado`. Antes de projetar uma ABC, você deve primeiro desenvolver um modelo do que classes são necessárias para representar um problema de programação e como elas se relacionam. Uma escola de pensamento sustenta que se você projetar uma hierarquia de herança de classes, as únicas classes concretas devem ser aquelas que nunca servem como classe base. Essa abordagem tende a produzir projetos mais limpos com menos complicações.

Uma forma de pensar sobre as ABCs é considerá-las como uma imposição de interface. Uma ABC exige que suas funções virtuais puras sejam substituídas em quaisquer classes derivadas concretas — forçando a classe derivada a obedecer às regras de interface que a ABC definiu. Esse modelo é comum em paradigmas de programação baseados em componentes, nos quais o uso de ABCs permite que o designer de componentes crie um "contrato de interface" onde todos os componentes derivados da ABC têm a garantia de suportar pelo menos a funcionalidade comum especificada pela ABC.

---

## Herança e Alocação Dinâmica de Memória

Como a herança interage com a alocação dinâmica de memória (o uso de `new` e `delete`)? Por exemplo, se uma classe base usa alocação dinâmica de memória e redefine operadores de atribuição e construtores de cópia, como isso afeta a implementação da classe derivada? A resposta depende da natureza da classe derivada. Se a classe derivada não usa alocação dinâmica de memória por si mesma, você não precisa tomar nenhuma medida especial. Se a classe derivada também usa alocação dinâmica de memória, há alguns novos truques para aprender. Vamos examinar esses dois casos.

### Caso 1: Classe Derivada Não Usa `new`

Suponha que você comece com a seguinte classe base que usa alocação dinâmica de memória (DMA — dynamic memory allocation):

```cpp
// Classe Base Usando DMA
class baseAD
{
private:
    char * rotulo;
    int avaliacao;
public:
    baseAD(const char * r = "null", int av = 0);
    baseAD(const baseAD & rs);
    virtual ~baseAD();
    baseAD & operator=(const baseAD & rs);
...
};
```

A declaração contém os métodos especiais que são requeridos quando construtores usam `new`: um destrutor, um construtor de cópia e um operador de atribuição sobrecarregado.

Agora suponha que você derive uma classe `semAD` de `baseAD` e que `semAD` não usa `new` nem tem outras funcionalidades de projeto incomuns que exijam tratamento especial:

```cpp
// classe derivada sem DMA
// sem necessidade de destrutor
// usa construtor de cópia implícito
// usa operador de atribuição implícito
class semAD : public baseAD
{
private:
    char cor[40];
public:
...
};
```

Você precisa definir um destrutor explícito, construtor de cópia e operador de atribuição para a classe `semAD`? A resposta é não.

Primeiro, considere a necessidade de um destrutor. Se você não definir um, o compilador define um destrutor padrão que não faz nada. Na verdade, o destrutor padrão para uma classe derivada sempre faz algo: ele chama o destrutor da classe base depois de executar seu próprio código. Como os membros de `semAD`, presumimos, não requerem nenhuma ação especial, o destrutor padrão está correto.

Em seguida, considere o construtor de cópia. Como você viu no Capítulo 12, o construtor de cópia padrão faz cópia membro a membro, que é inadequada para alocação dinâmica de memória. No entanto, a cópia membro a membro está correta para o novo membro de `semAD`. Isso deixa a questão do objeto `baseAD` herdado. O que você precisa saber é que a cópia membro a membro usa a forma de cópia que é definida para o tipo de dado em questão. Então copiar um `long` para um `long` é feito usando atribuição ordinária. Mas copiar um membro de classe ou um componente de classe herdado é feito usando o construtor de cópia para aquela classe. Assim, o construtor de cópia padrão para a classe `semAD` usa o construtor de cópia explícito de `baseAD` para copiar a parte `baseAD` de um objeto `semAD`. Portanto, o construtor de cópia padrão está correto para o novo membro de `semAD`, e também está correto para o objeto `baseAD` herdado.

Essencialmente, a mesma situação vale para a atribuição. O operador de atribuição padrão para uma classe usa automaticamente o operador de atribuição da classe base para o componente da classe base. Portanto, ele também está correto.

Essas propriedades de objetos herdados também valem para membros de classe que são eles mesmos objetos. Por exemplo, o Capítulo 10, "Objetos e Classes", implementa a classe `Estoque` usando um objeto `string` para representar o nome da empresa. A classe `string` padrão, como nosso exemplo `String`, usa alocação dinâmica de memória. Agora você vê por que isso não criaria problemas. O construtor de cópia padrão de `Estoque` usaria o construtor de cópia de `string` para copiar o membro `empresa` de um objeto, o operador de atribuição padrão de `Estoque` usaria o operador de atribuição de `string` para atribuir o membro `empresa` de um objeto, e o destrutor de `Estoque` (padrão ou não) chamaria automaticamente o destrutor de `string`.

### Caso 2: Classe Derivada Usa `new`

Suponha que a classe derivada usa `new`:

```cpp
// classe derivada com DMA
class comAD : public baseAD
{
private:
    char * estilo; // usa new em construtores
public:
...
};
```

Neste caso, você deve, é claro, definir um destrutor explícito, construtor de cópia e operador de atribuição para a classe derivada. Vamos considerar esses métodos separadamente.

Um destrutor de classe derivada chama automaticamente o destrutor da classe base, portanto sua própria responsabilidade é limpar o que os construtores da classe derivada fizeram. Assim, o destrutor de `comAD` deve liberar a memória gerenciada pelo ponteiro `estilo` e pode confiar no destrutor de `baseAD` para liberar a memória gerenciada pelo ponteiro `rotulo`:

```cpp
baseAD::~baseAD() // cuida dos elementos de baseAD
{
    delete [] rotulo;
}
comAD::~comAD() // cuida dos elementos de comAD
{
    delete [] estilo;
}
```

Em seguida, considere os construtores de cópia. O construtor de cópia de `baseAD` segue o padrão usual para arrays de `char`. O construtor de cópia de `comAD` tem acesso apenas aos dados de `comAD`, portanto deve invocar o construtor de cópia de `baseAD` para tratar da parte `baseAD` dos dados:

```cpp
comAD::comAD(const comAD & hs)
    : baseAD(hs)  // invoca o construtor de cópia da classe base
{
    estilo = new char[std::strlen(hs.estilo) + 1];
    std::strcpy(estilo, hs.estilo);
}
```

O ponto a notar é que a lista de inicializadores de membros passa uma referência a `comAD` para um construtor de `baseAD`. Não existe nenhum construtor de `baseAD` com um parâmetro de referência do tipo `comAD`, mas nenhum é necessário. Isso é porque o construtor de cópia de `baseAD` tem um parâmetro de referência a `baseAD`, e uma referência de classe base pode se referir a um tipo derivado. Assim, o construtor de cópia de `baseAD` usa a parte `baseAD` do argumento `comAD` para construir a parte `baseAD` do novo objeto.

Em seguida, considere os operadores de atribuição. O operador de atribuição de `baseAD` segue o padrão usual. Como `comAD` também usa alocação dinâmica de memória, ele também precisa de um operador de atribuição explícito. Sendo um método de `comAD`, ele tem acesso direto apenas aos dados de `comAD`. No entanto, um operador de atribuição explícito para uma classe derivada também deve cuidar da atribuição para o objeto `baseAD` herdado. Você pode fazer isso chamando explicitamente o operador de atribuição da classe base, como mostrado aqui:

```cpp
comAD & comAD::operator=(const comAD & hs)
{
    if (this == &hs)
        return *this;
    baseAD::operator=(hs);    // copia a parte base
    delete [] estilo;         // prepara para novo estilo
    estilo = new char[std::strlen(hs.estilo) + 1];
    std::strcpy(estilo, hs.estilo);
    return *this;
}
```

A seguinte instrução pode parecer um pouco estranha:

```cpp
baseAD::operator=(hs); // copia a parte base
```

Mas usar notação de função em vez de notação de operador permite usar o operador de resolução de escopo. Com efeito, a instrução significa o seguinte:

```cpp
*this = hs; // usa baseAD::operator=()
```

Mas, é claro, o compilador ignora comentários, portanto, se você usar o último código, o compilador usará `comAD::operator=()` e criará uma chamada recursiva. Usar notação de função chama o operador de atribuição correto.

Em resumo, quando tanto a classe base quanto a classe derivada usam alocação dinâmica de memória, o destrutor da classe derivada, o construtor de cópia e o operador de atribuição devem usar suas contrapartes da classe base para lidar com o componente da classe base. Esse requisito comum é cumprido de três maneiras diferentes. Para um destrutor, é feito automaticamente. Para um construtor, é realizado invocando o construtor de cópia da classe base na lista de inicialização de membros, ou então o construtor padrão é invocado automaticamente. Para o operador de atribuição, é realizado usando o operador de resolução de escopo em uma chamada explícita do operador de atribuição da classe base.

### Um Exemplo de Herança com Alocação Dinâmica de Memória e Funções Amigas

Para ilustrar essas ideias de herança e alocação dinâmica de memória, vamos integrar as classes `baseAD`, `semAD` e `comAD` em um único exemplo. A Listagem 13.14 é um arquivo de cabeçalho para essas classes. Para o que já discutimos, ela adiciona uma função amiga que ilustra como classes derivadas podem acessar funções amigas de uma classe base.

**Listagem 13.14 — alocdinamica.h**

```cpp
// alocdinamica.h -- herança e alocação dinâmica de memória
#ifndef ALOCDINAMICA_H_
#define ALOCDINAMICA_H_
#include <iostream>
// Classe Base Usando DMA
class baseAD
{
private:
    char * rotulo;
    int avaliacao;
public:
    baseAD(const char * r = "null", int av = 0);
    baseAD(const baseAD & rs);
    virtual ~baseAD();
    baseAD & operator=(const baseAD & rs);
    friend std::ostream & operator<<(std::ostream & os,
                        const baseAD & rs);
};
// classe derivada sem DMA
// sem destrutor necessário
// usa construtor de cópia implícito
// usa operador de atribuição implícito
class semAD : public baseAD
{
private:
    enum { COMP_COR = 40};
    char cor[COMP_COR];
public:
    semAD(const char * c = "branco", const char * r = "null",
           int av = 0);
    semAD(const char * c, const baseAD & rs);
    friend std::ostream & operator<<(std::ostream & os,
                        const semAD & rs);
};
// classe derivada com DMA
class comAD : public baseAD
{
private:
    char * estilo;
public:
    comAD(const char * e = "nenhum", const char * r = "null",
           int av = 0);
    comAD(const char * e, const baseAD & rs);
    comAD(const comAD & hs);
    ~comAD();
    comAD & operator=(const comAD & rs);
    friend std::ostream & operator<<(std::ostream & os,
                        const comAD & rs);
};
#endif
```

A Listagem 13.15 fornece as definições dos métodos para as classes `baseAD`, `semAD` e `comAD`.

**Listagem 13.15 — alocdinamica.cpp**

```cpp
// alocdinamica.cpp -- métodos das classes com alocação dinâmica
#include "alocdinamica.h"
#include <cstring>
// Métodos de baseAD
baseAD::baseAD(const char * r, int av)
{
    rotulo = new char[std::strlen(r) + 1];
    std::strcpy(rotulo, r);
    avaliacao = av;
}
baseAD::baseAD(const baseAD & rs)
{
    rotulo = new char[std::strlen(rs.rotulo) + 1];
    std::strcpy(rotulo, rs.rotulo);
    avaliacao = rs.avaliacao;
}
baseAD::~baseAD()
{
    delete [] rotulo;
}
baseAD & baseAD::operator=(const baseAD & rs)
{
    if (this == &rs)
        return *this;
    delete [] rotulo;
    rotulo = new char[std::strlen(rs.rotulo) + 1];
    std::strcpy(rotulo, rs.rotulo);
    avaliacao = rs.avaliacao;
    return *this;
}
std::ostream & operator<<(std::ostream & os, const baseAD & rs)
{
    os << "Rótulo: " << rs.rotulo << std::endl;
    os << "Avaliação: " << rs.avaliacao << std::endl;
    return os;
}
// Métodos de semAD
semAD::semAD(const char * c, const char * r, int av)
    : baseAD(r, av)
{
    std::strncpy(cor, c, 39);
    cor[39] = '\0';
}
semAD::semAD(const char * c, const baseAD & rs)
    : baseAD(rs)
{
    std::strncpy(cor, c, COMP_COR - 1);
    cor[COMP_COR - 1] = '\0';
}
std::ostream & operator<<(std::ostream & os, const semAD & ls)
{
    os << (const baseAD &) ls;
    os << "Cor: " << ls.cor << std::endl;
    return os;
}
// Métodos de comAD
comAD::comAD(const char * e, const char * r, int av)
        : baseAD(r, av)
{
    estilo = new char[std::strlen(e) + 1];
    std::strcpy(estilo, e);
}
comAD::comAD(const char * e, const baseAD & rs)
        : baseAD(rs)
{
    estilo = new char[std::strlen(e) + 1];
    std::strcpy(estilo, e);
}
comAD::comAD(const comAD & hs)
        : baseAD(hs) // invoca construtor de cópia da classe base
{
    estilo = new char[std::strlen(hs.estilo) + 1];
    std::strcpy(estilo, hs.estilo);
}
comAD::~comAD()
{
    delete [] estilo;
}
comAD & comAD::operator=(const comAD & hs)
{
    if (this == &hs)
        return *this;
    baseAD::operator=(hs); // copia a parte base
    delete [] estilo;      // prepara para novo estilo
    estilo = new char[std::strlen(hs.estilo) + 1];
    std::strcpy(estilo, hs.estilo);
    return *this;
}
std::ostream & operator<<(std::ostream & os, const comAD & hs)
{
    os << (const baseAD &) hs;
    os << "Estilo: " << hs.estilo << std::endl;
    return os;
}
```

O novo recurso a observar nas Listagens 13.14 e 13.15 é como as classes derivadas podem usar uma função amiga de uma classe base. Considere, por exemplo, a seguinte função amiga da classe `comAD`:

```cpp
friend std::ostream & operator<<(std::ostream & os,
                    const comAD & rs);
```

Sendo amiga da classe `comAD`, essa função tem acesso ao membro `estilo`. Mas há um problema: essa função não é amiga da classe `baseAD`, então como ela pode acessar os membros `rotulo` e `avaliacao`? A solução é usar a função `operator<<()` que é amiga da classe `baseAD`. O próximo problema é que, como as funções amigas não são funções membro, você não pode usar o operador de resolução de escopo para indicar qual função usar. A solução para esse problema é usar um *type cast* (conversão de tipo) para que a correspondência de protótipo selecione a função correta. Assim, o código converte o parâmetro `const comAD &` para um argumento `const baseAD &`:

```cpp
std::ostream & operator<<(std::ostream & os, const comAD & hs)
{
    // type cast para corresponder a operator<<(ostream & , const baseAD &)
    os << (const baseAD &) hs;
    os << "Estilo: " << hs.estilo << std::endl;
    return os;
}
```

A Listagem 13.16 testa as classes `baseAD`, `semAD` e `comAD` em um programa curto.

**Listagem 13.16 — usaralocdinamica.cpp**

```cpp
// usaralocdinamica.cpp -- herança, amigos e DMA
// compilar com alocdinamica.cpp
#include <iostream>
#include "alocdinamica.h"
int main()
{
    using std::cout;
    using std::endl;
    baseAD camisa("Portabelly", 8);
    semAD balao("vermelho", "Blimpo", 4);
    comAD mapa("Mercator", "Buffalo Keys", 5);
    cout << "Exibindo objeto baseAD:\n";
    cout << camisa << endl;
    cout << "Exibindo objeto semAD:\n";
    cout << balao << endl;
    cout << "Exibindo objeto comAD:\n";
    cout << mapa << endl;
    semAD balao2(balao);
    cout << "Resultado da cópia de semAD:\n";
    cout << balao2 << endl;
    comAD mapa2;
    mapa2 = mapa;
    cout << "Resultado da atribuição de comAD:\n";
    cout << mapa2 << endl;
    return 0;
}
```

Aqui está a saída do programa das Listagens 13.14, 13.15 e 13.16:

```
Exibindo objeto baseAD:
Rótulo: Portabelly
Avaliação: 8
Exibindo objeto semAD:
Rótulo: Blimpo
Avaliação: 4
Cor: vermelho
Exibindo objeto comAD:
Rótulo: Buffalo Keys
Avaliação: 5
Estilo: Mercator
Resultado da cópia de semAD:
Rótulo: Blimpo
Avaliação: 4
Cor: vermelho
Resultado da atribuição de comAD:
Rótulo: Buffalo Keys
Avaliação: 5
Estilo: Mercator
```

---

[Anterior](capitulo-13-01-heranca.md) | [Índice](README.md) | [Próximo](capitulo-13-03-revisao-resumo.md)
