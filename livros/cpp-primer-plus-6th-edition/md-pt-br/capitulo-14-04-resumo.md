# Capítulo 14 — Resumo, Revisão e Exercícios

> Tradução não oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Navegação: [Anterior](capitulo-14-03-templates-classe.md) | [Índice](README.md) | [Próximo](capitulo-15-01-amigos-excecoes.md)

## Resumo

O C++ fornece vários meios de reutilizar código. A herança pública, descrita no Capítulo 13, "Herança de Classes", permite modelar relacionamentos *is-a*, com as classes derivadas sendo capazes de reutilizar o código das classes base. A herança privada e protegida também permite reutilizar o código da classe base, desta vez modelando relacionamentos *has-a*. Com herança privada, membros públicos e protegidos da classe base tornam-se membros privados da classe derivada. Com herança protegida, membros públicos e protegidos da classe base tornam-se membros protegidos da classe derivada. Assim, em ambos os casos, a interface pública da classe base torna-se uma interface interna para a classe derivada. Isso é às vezes descrito como herdar a implementação mas não a interface, pois um objeto derivado não pode usar explicitamente a interface da classe base. Devido a isso, um ponteiro ou referência de classe base não tem permissão de se referir a um objeto derivado sem uma conversão de tipo explícita.

Você também pode reutilizar código de classe desenvolvendo uma classe com membros que são eles próprios objetos. Essa abordagem, chamada de composição (containment), camadas ou composição, também modela o relacionamento *has-a*. A composição é mais simples de implementar e usar do que a herança privada ou protegida, portanto geralmente é preferida. No entanto, a herança privada e protegida têm capacidades ligeiramente diferentes. Por exemplo, a herança permite a uma classe derivada acesso a membros protegidos de uma classe base. Além disso, permite que uma classe derivada redefina uma função virtual herdada da classe base. Como a composição não é uma forma de herança, nenhuma dessas capacidades é uma opção quando você reutiliza código de classe via composição. Por outro lado, a composição é mais adequada se você precisar de vários objetos de uma determinada classe. Por exemplo, uma classe `Estado` poderia conter um array de objetos `Condado`.

A herança múltipla (MI) permite reutilizar código para mais de uma classe num design de classe. A herança múltipla privada ou protegida modela o relacionamento *has-a*, e a herança múltipla pública modela o relacionamento *is-a*. A herança múltipla pode criar problemas com nomes multidefinidos e bases multi-herdadas. Você pode usar qualificadores de classe para resolver ambiguidades de nome e classes base virtuais para evitar bases multi-herdadas. No entanto, usar classes base virtuais introduz novas regras para escrever listas de inicialização para construtores e para resolver ambiguidades.

Os templates de classe permitem criar um design de classe genérico no qual um tipo, geralmente um tipo membro, é representado por um parâmetro de tipo. Um template típico tem a seguinte aparência:

```cpp
template <class T>
class Ic
{
    T v;
    ...
public:
    Ic(const T & val) : v(val) {}
    ...
};
```

Aqui `T` é o parâmetro de tipo, e ele atua como substituto para um tipo real a ser especificado posteriormente. (Este parâmetro pode ter qualquer nome C++ válido, mas `T` e `Tipo` são escolhas comuns.) Você também pode usar `typename` em vez de `class` neste contexto:

```cpp
template <typename T>  // igual a template <class T>
class Rev {...};
```

Definições de classe (instanciações) são geradas quando você declara um objeto de classe e especifica um tipo particular. Por exemplo, a seguinte declaração faz o compilador gerar uma declaração de classe na qual cada ocorrência do parâmetro de tipo `T` no template é substituída pelo tipo real `short` na declaração de classe:

```cpp
Ic<short> sic;   // instanciação implícita
```

Neste caso, o nome da classe é `Ic<short>`, não `Ic`. `Ic<short>` é chamada de *especialização* do template. Em particular, é uma instanciação implícita.

Uma instanciação explícita ocorre quando você declara uma especialização específica da classe, usando a palavra-chave `template`:

```cpp
template class Ic<int>;  // instanciação explícita
```

Nesta situação, o compilador usa o template geral para gerar uma especialização `int` de `Ic<int>`, mesmo que nenhum objeto daquela classe tenha ainda sido solicitado.

Você pode fornecer especializações explícitas, que são declarações de classe especializadas que substituem uma definição de template. Você apenas define a classe, começando com `template<>`, e depois usa o nome da classe template, seguido de colchetes angulares contendo o tipo para o qual você quer uma especialização. Por exemplo, você poderia fornecer uma classe `Ic` especializada para ponteiros de caractere da seguinte forma:

```cpp
template <> class Ic<char *>
{
    char * str;
    ...
public:
    Ic(const char * s) : str(s) {}
    ...
};
```

Então uma declaração da seguinte forma usaria a definição especializada em vez de usar o template geral:

```cpp
Ic<char *> chic;
```

Um template de classe pode especificar mais de um tipo genérico e também pode ter parâmetros não-tipo:

```cpp
template <class T, class TT, int n>
class Parceiros {...};
```

A seguinte declaração geraria uma instanciação implícita usando `double` para `T`, `string` para `TT` e `6` para `n`:

```cpp
Parceiros<double, string, 6> mistura;
```

Um template de classe também pode ter parâmetros que são templates:

```cpp
template <template <typename T> class CL, typename U, int z>
class Trofeu {...};
```

Aqui `z` representa um valor `int`, `U` representa o nome de um tipo, e `CL` representa uma classe template declarada usando `template <typename T>`.

Classes template podem ser parcialmente especializadas:

```cpp
template <class T> class Parceiros<T, T, 10> {...};
template <class T, class TT> class Parceiros<T, TT, 100> {...};
template <class T, int n> class Parceiros<T, T*, n> {...};
```

O primeiro exemplo aqui cria uma especialização na qual ambos os tipos são iguais e `n` tem o valor 10. Da mesma forma, o segundo cria uma especialização para `n` igual a 100, e o terceiro cria uma especialização para a qual o segundo tipo é um ponteiro para o primeiro tipo.

Classes template podem ser membros de outras classes, estruturas e templates.

O objetivo de todos esses métodos é permitir que os programadores reutilizem código testado sem ter que copiá-lo manualmente. Isso simplifica a tarefa de programação e torna os programas mais confiáveis.

## Revisão do Capítulo

1. Para cada um dos seguintes conjuntos de classes, indique se a derivação pública ou privada é mais apropriada para a Coluna B:

   | A | B |
   |---|---|
   | classe `Urso` | classe `UrsoPolar` |
   | classe `Cozinha` | classe `Casa` |
   | classe `Pessoa` | classe `Programador` |
   | classe `Pessoa` | classe `CavaleiroeJoquei` |
   | classe `Pessoa`, classe `Automovel` | classe `Motorista` |

2. Suponha que você tenha as seguintes definições:

   ```cpp
   class Frabjous {
   private:
       char fab[20];
   public:
       Frabjous(const char * s = "C++") : fab(s) {}
       virtual void tell() { cout << fab; }
   };
   class Gloam {
   private:
       int glip;
       Frabjous fb;
   public:
       Gloam(int g = 0, const char * s = "C++");
       Gloam(int g, const Frabjous & f);
       void tell();
   };
   ```

   Dado que a versão de `tell()` de `Gloam` deve exibir os valores de `glip` e `fb`, forneça definições para os três métodos de `Gloam`.

3. Suponha que você tenha as seguintes definições:

   ```cpp
   class Frabjous {
   private:
       char fab[20];
   public:
       Frabjous(const char * s = "C++") : fab(s) {}
       virtual void tell() { cout << fab; }
   };
   class Gloam : private Frabjous {
   private:
       int glip;
   public:
       Gloam(int g = 0, const char * s = "C++");
       Gloam(int g, const Frabjous & f);
       void tell();
   };
   ```

   Dado que a versão de `tell()` de `Gloam` deve exibir os valores de `glip` e `fab`, forneça definições para os três métodos de `Gloam`.

4. Suponha que você tenha a seguinte definição, baseada no template `Pilha` da Listagem 14.13 e na classe `Trabalhador` da Listagem 14.10:

   ```cpp
   Pilha<Trabalhador *> sw;
   ```

   Escreva a declaração de classe que será gerada. Escreva apenas a declaração da classe, não os métodos de classe não inline.

5. Use as definições de template neste capítulo para definir o seguinte:

   - Um array de objetos `string`
   - Uma pilha de arrays de `double`
   - Um array de pilhas de ponteiros para objetos `Trabalhador`

   Quantas definições de classe template são produzidas na Listagem 14.18?

6. Descreva as diferenças entre classes base virtuais e não virtuais.

## Exercícios de Programação

1. A classe `Vinho` tem um objeto membro da classe `string` (veja o Capítulo 4) que armazena o nome de um vinho e um objeto `Par` (conforme discutido neste capítulo) de objetos `valarray<int>` (conforme discutido neste capítulo). O primeiro membro de cada objeto `Par` armazena os anos de safra, e o segundo membro armazena os números de garrafas possuídas para o correspondente ano de safra particular. Por exemplo, o primeiro objeto `valarray` do objeto `Par` poderia armazenar os anos 1988, 1992 e 1996, e o segundo objeto `valarray` poderia armazenar as contagens de garrafas 24, 48 e 144. Pode ser conveniente para `Vinho` ter um membro `int` que armazena o número de anos. Além disso, alguns `typedef`s podem ser úteis para simplificar a codificação:

   ```cpp
   typedef std::valarray<int> ArrayInt;
   typedef Par<ArrayInt, ArrayInt> ArrayPar;
   ```

   Assim, o tipo `ArrayPar` representa o tipo `Par<std::valarray<int>, std::valarray<int>>`. Implemente a classe `Vinho` usando composição. A classe deve ter um construtor padrão e pelo menos os seguintes construtores:

   ```cpp
   // inicializa rotulo para l, numero de anos para a, anos de safra para yr[], garrafas para bot[]
   Vinho(const char * l, int a, const int yr[], const int bot[]);
   // inicializa rotulo para l, numero de anos para a,
   // cria objetos array de comprimento a
   Vinho(const char * l, int a);
   ```

   A classe `Vinho` deve ter um método `ObterGarrafas()` que, dado um objeto `Vinho` com `a` anos, solicita ao usuário inserir o número correspondente de anos de safra e contagens de garrafas. Um método `Rotulo()` deve retornar uma referência para o nome do vinho. Um método `soma()` deve retornar o total de garrafas no segundo objeto `valarray<int>` no objeto `Par`.

   O programa deve solicitar ao usuário inserir um nome de vinho, o número de elementos do array, e as informações de ano e contagem de garrafas para cada elemento do array. O programa deve usar esses dados para construir um objeto `Vinho` e depois exibir as informações armazenadas no objeto. Para orientação, aqui está um programa de teste de exemplo:

   ```cpp
   // pe14-1.cpp -- usando a classe Vinho com composição
   #include <iostream>
   #include "vinhoc.h"
   int main(void)
   {
       using std::cin;
       using std::cout;
       using std::endl;
       cout << "Digite o nome do vinho: ";
       char lab[50];
       cin.getline(lab, 50);
       cout << "Digite o numero de anos: ";
       int anos;
       cin >> anos;
       Vinho holding(lab, anos);  // armazena rotulo, anos, dimensiona arrays
       holding.ObterGarrafas();   // solicita entrada de ano, contagem de garrafas
       holding.Mostrar();         // exibe conteudo do objeto
       const int ANOS = 3;
       int a[ANOS] = {1993, 1995, 1998};
       int b[ANOS] = {48, 60, 72};
       // cria novo objeto, inicializa usando dados nos arrays a e b
       Vinho mais("Uva Espumante Vermelha", ANOS, a, b);
       mais.Mostrar();
       cout << "Total de garrafas para " << mais.Rotulo()  // usa metodo Rotulo()
            << ": " << mais.soma() << endl;                // usa metodo soma()
       cout << "Ate logo\n";
       return 0;
   }
   ```

   E aqui está um exemplo de saída:

   ```
   Digite o nome do vinho: Gully Wash
   Digite o numero de anos: 4
   Digite dados de Gully Wash para 4 ano(s):
   Digite o ano: 1988
   Digite as garrafas para aquele ano: 42
   Digite o ano: 1994
   Digite as garrafas para aquele ano: 58
   Digite o ano: 1998
   Digite as garrafas para aquele ano: 122
   Digite o ano: 2001
   Digite as garrafas para aquele ano: 144
   Vinho: Gully Wash
        Ano Garrafas
        1988 42
        1994 58
        1998 122
        2001 144
   Vinho: Uva Espumante Vermelha
        Ano Garrafas
        1993 48
        1995 60
        1998 72
   Total de garrafas para Uva Espumante Vermelha: 180
   Ate logo
   ```

2. Este exercício é o mesmo que o Exercício de Programação 1, exceto que você deve usar herança privada em vez de composição. Novamente, alguns `typedef`s podem ser úteis. Você também pode contemplar o significado de declarações como:

   ```cpp
   ArrayPar::operator=(ArrayPar(ArrayInt(), ArrayInt()));
   cout << (const string &)(*this);
   ```

   A classe deve funcionar com o mesmo programa de teste mostrado no Exercício de Programação 1.

3. Defina um template `FilaTp`. Teste-o criando uma fila de ponteiros-para-`Trabalhador` (conforme definido na Listagem 14.10) e usando a fila num programa similar ao da Listagem 14.12.

4. Uma classe `Pessoa` armazena o primeiro nome e o sobrenome de uma pessoa. Além de seus construtores, ela tem um método `Mostrar()` que exibe ambos os nomes. Uma classe `Pistoleiro` deriva virtualmente da classe `Pessoa`. Ela tem um membro `Sacar()` que retorna um valor do tipo `double` representando o tempo de saque de um pistoleiro. A classe também tem um membro `int` representando o número de entalhes na arma de um pistoleiro. Por fim, ela tem uma função `Mostrar()` que exibe todas essas informações.

   Uma classe `JogadorDePoquer` deriva virtualmente da classe `Pessoa`. Ela tem um membro `Sacar()` que retorna um número aleatório no intervalo de 1 a 52, representando um valor de carta. A classe `JogadorDePoquer` usa a função `Mostrar()` de `Pessoa`. A classe `Valentao` deriva publicamente das classes `Pistoleiro` e `JogadorDePoquer`. Ela tem um membro `SacarArma()` que retorna o tempo de saque de um valentão e um membro `SacarCarta()` que retorna a próxima carta sorteada. Ela tem uma função `Mostrar()` apropriada. Defina todas essas classes e métodos, junto com quaisquer outros métodos necessários (como métodos para definir valores de objetos) e teste-os num programa simples similar ao da Listagem 14.12.

5. Aqui estão algumas declarações de classe:

   ```cpp
   // emp.h -- arquivo de cabeçalho para a classe abstr_emp e filhas
   #include <iostream>
   #include <string>
   class abstr_emp
   {
   private:
       std::string fname;  // primeiro nome de abstr_emp
       std::string lname;  // sobrenome de abstr_emp
       std::string job;
   public:
       abstr_emp();
       abstr_emp(const std::string & fn, const std::string & ln,
                 const std::string & j);
       virtual void ShowAll() const;  // exibe com rótulos todos os dados
       virtual void SetAll();         // solicita ao usuário valores
       friend std::ostream &
           operator<<(std::ostream & os, const abstr_emp & e);
                                      // apenas exibe nome e sobrenome
       virtual ~abstr_emp() = 0;      // classe base virtual
   };
   class employee : public abstr_emp
   {
   public:
       employee();
       employee(const std::string & fn, const std::string & ln,
                const std::string & j);
       virtual void ShowAll() const;
       virtual void SetAll();
   };
   class manager : virtual public abstr_emp
   {
   private:
       int inchargeof;  // número de abstr_emps gerenciados
   protected:
       int InChargeOf() const { return inchargeof; }  // saída
       int & InChargeOf() { return inchargeof; }      // entrada
   public:
       manager();
       manager(const std::string & fn, const std::string & ln,
               const std::string & j, int ico = 0);
       manager(const abstr_emp & e, int ico);
       manager(const manager & m);
       virtual void ShowAll() const;
       virtual void SetAll();
   };
   class fink : virtual public abstr_emp
   {
   private:
       std::string reportsto;  // a quem fink se reporta
   protected:
       const std::string ReportsTo() const { return reportsto; }
       std::string & ReportsTo() { return reportsto; }
   public:
       fink();
       fink(const std::string & fn, const std::string & ln,
            const std::string & j, const std::string & rpo);
       fink(const abstr_emp & e, const std::string & rpo);
       fink(const fink & e);
       virtual void ShowAll() const;
       virtual void SetAll();
   };
   class highfink : public manager, public fink  // fink de gerência
   {
   public:
       highfink();
       highfink(const std::string & fn, const std::string & ln,
                const std::string & j, const std::string & rpo,
                int ico);
       highfink(const abstr_emp & e, const std::string & rpo, int ico);
       highfink(const fink & f, int ico);
       highfink(const manager & m, const std::string & rpo);
       highfink(const highfink & h);
       virtual void ShowAll() const;
       virtual void SetAll();
   };
   ```

   Observe que a hierarquia de classes usa herança múltipla com uma classe base virtual, portanto tenha em mente as regras especiais para listas de inicialização de construtores nesse caso. Observe também a presença de alguns métodos de acesso protegido. Isso simplifica o código para alguns dos métodos de `highfink`. (Note, por exemplo, que se `highfink::ShowAll()` simplesmente chamar `fink::ShowAll()` e `manager::ShowAll()`, acabará chamando `abstr_emp::ShowAll()` duas vezes.) Forneça as implementações dos métodos da classe e teste as classes num programa. Aqui está um programa mínimo de teste:

   ```cpp
   // pe14-5.cpp
   // usaemp1.cpp -- usando as classes abstr_emp
   #include <iostream>
   using namespace std;
   #include "emp.h"
   int main(void)
   {
       employee em("Trip", "Harris", "Thumper");
       cout << em << endl;
       em.ShowAll();

       manager ma("Amorphia", "Spindragon", "Nuancer", 5);
       cout << ma << endl;
       ma.ShowAll();

       fink fi("Matt", "Oggs", "Oiler", "Juno Barr");
       cout << fi << endl;
       fi.ShowAll();
       highfink hf(ma, "Curly Kew");  // recrutamento?
       hf.ShowAll();
       cout << "Pressione uma tecla para a proxima fase:\n";
       cin.get();
       highfink hf2;
       hf2.SetAll();
       cout << "Usando um ponteiro abstr_emp *:\n";
       abstr_emp * tri[4] = {&em, &fi, &hf, &hf2};
       for (int i = 0; i < 4; i++)
           tri[i]->ShowAll();
       return 0;
   }
   ```

   Por que nenhum operador de atribuição é definido? Por que `ShowAll()` e `SetAll()` são virtuais? Por que `abstr_emp` é uma classe base virtual? Por que a classe `highfink` não tem seção de dados? Por que apenas uma versão de `operator<<()` é necessária? O que aconteceria se o final do programa fosse substituído por este código?

   ```cpp
   abstr_emp tri[4] = {em, fi, hf, hf2};
   for (int i = 0; i < 4; i++)
       tri[i].ShowAll();
   ```

---

[Anterior](capitulo-14-03-templates-classe.md) | [Índice](README.md) | [Próximo](capitulo-15-01-amigos-excecoes.md)
