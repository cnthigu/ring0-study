# Capítulo 13 — Revisão de Projeto de Classes, Resumo e Exercícios

> Tradução não oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Navegação: [Anterior](capitulo-13-02-virtual-binding.md) | [Índice](README.md) | [Próximo](capitulo-14-01-composicao-heranca-privada.md)

---

## Revisão de Projeto de Classes (Class Design Review)

O C++ pode ser aplicado a uma grande variedade de problemas de programação, e você não pode reduzir o projeto de classes a uma rotina de pintar por números. No entanto, existem algumas diretrizes que frequentemente se aplicam, e este é um bom momento para examiná-las revisando e ampliando discussões anteriores.

### Funções Membro Que o Compilador Gera para Você

Como discutido pela primeira vez no Capítulo 12, o compilador gera automaticamente certas funções membro públicas, chamadas de *funções membro especiais* (special member functions). O fato de que ele o faz sugere que essas funções membro especiais são particularmente importantes. Vamos examiná-las novamente agora.

#### Construtores Padrão

Um *construtor padrão* (default constructor) é aquele sem argumentos, ou aquele para o qual todos os argumentos têm valores padrão. Se você não definir nenhum construtor, o compilador define um construtor padrão para você. Sua existência permite que você crie objetos. Por exemplo, suponha que `Estrela` é uma classe. Você precisa de um construtor padrão para usar o seguinte:

```cpp
Estrela rigel;       // cria um objeto sem inicialização explícita
Estrela pleiades[6]; // cria um array de objetos
```

Mais uma coisa que um construtor padrão automático faz é chamar os construtores padrão de quaisquer classes base e de quaisquer membros que sejam objetos de outra classe.

Além disso, se você escrever um construtor de classe derivada sem invocar explicitamente um construtor de classe base na lista de inicializadores de membros, o compilador usa o construtor padrão da classe base para construir a parte da classe base do novo objeto. Se não houver construtor padrão da classe base, você receberá um erro em tempo de compilação nessa situação.

Se você definir um construtor de qualquer tipo, o compilador não definirá um construtor padrão para você. Nesse caso, cabe a você fornecer um construtor padrão se um for necessário.

Observe que uma das motivações para ter construtores é garantir que os objetos sejam sempre inicializados corretamente. Além disso, se uma classe tiver quaisquer membros ponteiros, eles certamente devem ser inicializados. Portanto, é uma boa ideia fornecer um construtor padrão explícito que inicialize todos os membros de dados da classe com valores razoáveis.

#### Construtores de Cópia

Um *construtor de cópia* (copy constructor) para uma classe é um construtor que recebe um objeto do tipo da classe como argumento. Tipicamente, o parâmetro declarado é uma referência constante para o tipo da classe. Por exemplo, o construtor de cópia para uma classe `Estrela` teria este protótipo:

```cpp
Estrela(const Estrela &);
```

Um construtor de cópia de classe é usado nas seguintes situações:

- Quando um novo objeto é inicializado para um objeto da mesma classe
- Quando um objeto é passado para uma função por valor
- Quando uma função retorna um objeto por valor
- Quando o compilador gera um objeto temporário

Se um programa não usar um construtor de cópia (explícita ou implicitamente), o compilador fornecerá um protótipo, mas não uma definição de função. Caso contrário, o programa define um construtor de cópia que realiza inicialização membro a membro. Ou seja, cada membro do novo objeto é inicializado com o valor do membro correspondente do objeto original. Se um membro é em si um objeto de classe, então a inicialização membro a membro usa o construtor de cópia definido para aquela classe específica.

Em alguns casos, a inicialização membro a membro não é desejável. Por exemplo, membros ponteiros inicializados com `new` geralmente exigem que você institua cópia profunda, como no exemplo da classe `baseAD`. Ou uma classe pode ter uma variável estática que precisa ser modificada. Nesses casos, você precisa definir seu próprio construtor de cópia.

#### Operadores de Atribuição

Um *operador de atribuição* (assignment operator) padrão lida com a atribuição de um objeto a outro objeto da mesma classe. Não confunda atribuição com inicialização. Se uma instrução cria um novo objeto, ela está usando inicialização, e se uma instrução altera o valor de um objeto existente, ela está fazendo atribuição:

```cpp
Estrela sirius;
Estrela alpha = sirius;  // inicialização (uma notação)
Estrela cachorro;
cachorro = sirius;       // atribuição
```

A atribuição padrão usa atribuição membro a membro. Se um membro é em si um objeto de classe, então a atribuição membro a membro padrão usa o operador de atribuição definido para aquela classe específica. Se você precisar definir um construtor de cópia explicitamente, também precisará, pelas mesmas razões, definir o operador de atribuição explicitamente. O protótipo para um operador de atribuição da classe `Estrela` é este:

```cpp
Estrela & Estrela::operator=(const Estrela &);
```

Observe que a função operador de atribuição retorna uma referência a um objeto `Estrela`. A classe `baseAD` mostra um exemplo típico de uma função operador de atribuição explícita.

O compilador não gera operadores de atribuição para atribuir um tipo a outro. Suponha que você queira ser capaz de atribuir uma string a um objeto `Estrela`. Uma abordagem é definir esse operador explicitamente:

```cpp
Estrela & Estrela::operator=(const char *) {...}
```

Uma segunda abordagem é contar com uma função de conversão (veja "Considerações de Conversão" na próxima seção) para converter uma string em um objeto `Estrela` e usar a função de atribuição de `Estrela` para `Estrela`. A primeira abordagem é executada mais rapidamente, mas requer mais código. A abordagem com função de conversão pode levar a situações que confundem o compilador.

O Capítulo 18, "Visitando o Novo Padrão C++", discute dois métodos especiais adicionados pelo C++11: o construtor de movimento (move constructor) e o operador de atribuição de movimento (move assignment operator).

---

### Outras Considerações Sobre Métodos de Classe

Há vários outros pontos a ter em mente ao definir uma classe. As seções a seguir listam alguns deles.

#### Considerações Sobre Construtores

Construtores são diferentes de outros métodos de classe no sentido de que eles criam novos objetos, enquanto outros métodos são invocados por objetos existentes. Essa é uma razão pela qual construtores não são herdados. Herança significa que um objeto derivado pode usar um método de classe base, mas, no caso dos construtores, o objeto não existe até depois de o construtor ter feito seu trabalho.

#### Considerações Sobre Destrutores

Você precisa lembrar de definir um destrutor explícito que exclua qualquer memória alocada por `new` nos construtores da classe e cuide de qualquer outra contabilidade especial que a destruição de um objeto de classe exige. Se a classe deve ser usada como classe base, você deve fornecer um destrutor virtual mesmo que a classe não exija um destrutor.

#### Considerações de Conversão

Qualquer construtor que possa ser invocado com exatamente um argumento define uma conversão do tipo do argumento para o tipo da classe. Por exemplo, considere os seguintes protótipos de construtor para uma classe `Estrela`:

```cpp
Estrela(const char *);         // converte char * para Estrela
Estrela(const Espectral &, int membros = 1); // converte Espectral para Estrela
```

Construtores de conversão são usados, por exemplo, quando um tipo conversível é passado para uma função definida como recebendo um argumento de classe. Por exemplo, suponha que você tenha o seguinte:

```cpp
Estrela norte;
norte = "polaris";
```

A segunda instrução invocaria a função `Estrela::operator=(const Estrela &)`, usando `Estrela::Estrela(const char *)` para gerar um objeto `Estrela` a ser usado como argumento para a função operador de atribuição. Isso assume que você não definiu um operador de atribuição de `(char *)` para `Estrela`.

Usar `explicit` no protótipo de um construtor com um argumento desabilita conversões implícitas, mas ainda permite conversões explícitas:

```cpp
class Estrela
{
...
public:
    explicit Estrela(const char *);
...
};
...
Estrela norte;
norte = "polaris";        // não permitido
norte = Estrela("polaris"); // permitido
```

Para converter de um objeto de classe para algum outro tipo, você define uma função de conversão (veja o Capítulo 11, "Trabalhando com Classes"). Uma função de conversão é uma função membro de classe sem argumentos ou tipo de retorno declarado que tem o nome do tipo para o qual converter. Apesar de não ter tipo de retorno declarado, a função deve retornar o valor de conversão desejado. Aqui estão alguns exemplos:

```cpp
Estrela::operator double() {...}       // converte Estrela para double
Estrela::operator const char * () {...} // converte para const char
```

Você deve ser criterioso com essas funções, usando-as apenas se fizerem bom sentido. Além disso, com alguns projetos de classe, ter funções de conversão aumenta a probabilidade de escrever código ambíguo. Por exemplo, suponha que você defina uma conversão `double` para o tipo `Vetor` do Capítulo 11, e suponha que você tenha o seguinte código:

```cpp
Vetor ius(6.0, 0.0);
Vetor luxo = ius + 20.2; // ambíguo
```

O compilador poderia converter `ius` para `double` e usar adição de `double`, ou poderia converter `20.2` para vetor (usando um dos construtores) e usar adição de vetor. Em vez disso, ele não faria nenhuma das duas e informaria uma construção ambígua.

O C++11 fornece a opção de usar a palavra-chave `explicit` com funções de conversão. Como com construtores, `explicit` permite conversões explícitas usando *type casts*, mas não permite conversões implícitas.

#### Passando um Objeto por Valor Versus Passando por Referência

Em geral, se você escrever uma função usando um argumento de objeto, você deve passar o objeto por referência em vez de por valor. Uma razão para isso é a eficiência. Passar um objeto por valor envolve gerar uma cópia temporária, o que significa chamar o construtor de cópia e depois chamar o destrutor. Chamar essas funções leva tempo, e copiar um objeto grande pode ser consideravelmente mais lento do que passar uma referência. Se a função não modificar o objeto, você deve declarar o argumento como uma referência `const`.

Outra razão para passar objetos por referência é que, no caso de herança usando funções virtuais, uma função definida aceitando um argumento de referência de classe base também pode ser usada com sucesso com classes derivadas, como você viu anteriormente neste capítulo.

#### Retornando um Objeto Versus Retornando uma Referência

Alguns métodos de classe retornam objetos. Você provavelmente percebeu que alguns membros retornam objetos diretamente, enquanto outros retornam referências. Às vezes um método deve retornar um objeto, mas se não for necessário, você deve usar uma referência. Vamos examinar isso mais de perto.

Primeiro, a única diferença de codificação entre retornar um objeto diretamente e retornar uma referência está no protótipo e cabeçalho da função:

```cpp
Estrela nova1(const Estrela &);   // retorna um objeto Estrela
Estrela & nova2(const Estrela &); // retorna uma referência para Estrela
```

Em seguida, a razão para você retornar uma referência em vez de um objeto é que retornar um objeto envolve gerar uma cópia temporária do objeto retornado. É a cópia que é disponibilizada para o programa que chamou. Assim, retornar um objeto envolve o custo de tempo de chamar um construtor de cópia para gerar a cópia e o custo de tempo de chamar o destrutor para se livrar da cópia. Retornar uma referência economiza tempo e uso de memória. Retornar um objeto diretamente é análogo a passar um objeto por valor: ambos os processos geram cópias temporárias. Da mesma forma, retornar uma referência é análogo a passar um objeto por referência: tanto o chamador quanto o chamado operam no mesmo objeto.

No entanto, nem sempre é possível retornar uma referência. Uma função não deve retornar uma referência a um objeto temporário criado na função porque a referência se torna inválida quando a função termina e o objeto desaparece. Nesse caso, o código deve retornar um objeto para gerar uma cópia que estará disponível para o programa que chamou.

Como regra geral, se uma função retorna um objeto temporário criado na função, você não deve usar uma referência. Por exemplo, o seguinte método usa um construtor para criar um novo objeto, e então retorna uma cópia desse objeto:

```cpp
Vetor Vetor::operator+(const Vetor & b) const
{
    return Vetor(x + b.x, y + b.y);
}
```

Se uma função retorna um objeto que foi passado para ela via referência ou ponteiro, você deve retornar o objeto por referência. Por exemplo, o seguinte código retorna, por referência, o objeto que invoca a função ou o objeto passado como argumento:

```cpp
const Estoque & Estoque::topval(const Estoque & s) const
{
    if (s.valor_total > valor_total)
        return s;    // objeto argumento
    else
        return *this; // objeto invocador
}
```

#### Usando `const`

Você precisa estar atento às oportunidades de usar `const`. Você pode usá-lo para garantir que um método não modifique um argumento:

```cpp
Estrela::Estrela(const char * s) {...} // não mudará a string para qual s aponta
```

Você pode usar `const` para garantir que um método não modifique o objeto que o invoca:

```cpp
void Estrela::mostrar() const {...} // não mudará o objeto invocador
```

Aqui `const` significa `const Estrela * this`, onde `this` aponta para o objeto invocador.

Normalmente, uma função que retorna uma referência pode estar no lado esquerdo de uma instrução de atribuição, o que realmente significa que você pode atribuir um valor ao objeto referenciado. Mas você pode usar `const` para garantir que um valor de retorno de referência ou ponteiro não possa ser usado para modificar dados em um objeto:

```cpp
const Estoque & Estoque::topval(const Estoque & s) const
{
    if (s.valor_total > valor_total)
        return s;    // objeto argumento
    else
        return *this; // objeto invocador
}
```

Aqui o método retorna uma referência para `this` ou para `s`. Como `this` e `s` são ambos declarados `const`, a função não pode modificá-los, o que significa que a referência retornada também deve ser declarada `const`.

Observe que se uma função declara um argumento como referência ou ponteiro para um `const`, ela não pode passar esse argumento para outra função a menos que essa função também garanta não alterar o argumento.

---

### Considerações Sobre Herança Pública

Naturalmente, adicionar herança a um programa traz uma série de considerações. Vamos examinar algumas delas.

#### Considerações Sobre a Relação É-Um

Você deve ser guiado pela relação *é-um*. Se a sua classe derivada proposta não é um tipo específico da classe base, você não deve usar derivação pública. Por exemplo, você não deve derivar uma classe `Programador` de uma classe `Cerebro`. Se você quiser representar a crença de que um programador tem um cérebro, você deve usar um objeto `Cerebro` como membro de dados da classe `Programador`.

Em alguns casos, a melhor abordagem pode ser criar uma classe de dados abstrata com funções virtuais puras e derivar outras classes dela.

Lembre-se de que uma expressão da relação *é-um* é que um ponteiro de classe base pode apontar para um objeto de classe derivada e que uma referência de classe base pode se referir a um objeto de classe derivada sem uma conversão de tipo explícita. Também lembre-se de que o inverso não é verdadeiro; assim, você não pode ter um ponteiro ou referência de classe derivada se referir a um objeto de classe base sem uma conversão de tipo explícita. Dependendo das declarações de classe, tal conversão de tipo explícita (um downcast) pode ou não fazer sentido.

#### O Que Não É Herdado

Construtores não são herdados. Ou seja, criar um objeto derivado requer chamar um construtor da classe derivada. No entanto, construtores de classe derivada geralmente usam a sintaxe de lista de inicializadores de membros para chamar construtores de classe base para construir a parte da classe base de um objeto derivado. Se o construtor da classe derivada não chama explicitamente um construtor de classe base usando a sintaxe de lista de inicializadores de membros, ele usa o construtor padrão da classe base. Em uma cadeia de herança, cada classe pode usar uma lista de inicializadores de membros para passar informações de volta para sua classe base imediata. O C++11 adiciona um mecanismo que permite a herança de construtores. No entanto, o comportamento padrão ainda é que construtores não são herdados.

Destrutores também não são herdados. No entanto, quando um objeto é destruído, o programa primeiro chama o destrutor derivado e depois o destrutor base. Se houver um destrutor padrão da classe base, o compilador gera um destrutor padrão da classe derivada. Em geral, se uma classe serve como classe base, seu destrutor deve ser virtual.

Operadores de atribuição não são herdados. A razão é simples. Um método herdado tem a mesma assinatura de função em uma classe derivada que tem na classe base. No entanto, um operador de atribuição tem uma assinatura de função que muda de classe para classe porque tem um parâmetro formal que é o tipo da classe. Os operadores de atribuição têm algumas propriedades interessantes, que examinaremos a seguir.

#### Considerações Sobre Operadores de Atribuição

Se o compilador detecta que um programa atribui um objeto a outro da mesma classe, ele fornece automaticamente a essa classe um operador de atribuição. A versão padrão, ou implícita, desse operador usa atribuição membro a membro, com cada membro do objeto de destino sendo atribuído ao valor do membro correspondente do objeto de origem. No entanto, se o objeto pertence a uma classe derivada, o compilador usa o operador de atribuição da classe base para lidar com a atribuição para a parte da classe base do objeto de classe derivada. Se você forneceu explicitamente um operador de atribuição para a classe base, esse operador é usado. Da mesma forma, se uma classe contém um membro que é um objeto de outra classe, o operador de atribuição para essa classe é usado para esse membro.

Como você já viu várias vezes, você precisa fornecer um operador de atribuição explícito se os construtores de classe usam `new` para inicializar ponteiros. Como o C++ usa o operador de atribuição da classe base para a parte base de objetos derivados, você não precisa redefinir o operador de atribuição para uma classe derivada a menos que ela adicione membros de dados que requerem cuidado especial. Por exemplo, a classe `baseAD` define a atribuição explicitamente, mas a classe derivada `semAD` usa o operador de atribuição implícito gerado para essa classe.

Suponha, no entanto, que uma classe derivada use `new`, e você tem que fornecer um operador de atribuição explícito. O operador deve fornecer para cada membro da classe, não apenas para os novos membros. A classe `comAD` ilustra como isso pode ser feito:

```cpp
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
```

E quanto a atribuir um objeto de classe derivada a um objeto de classe base? (Note que isso não é o mesmo que inicializar uma referência de classe base a um objeto de classe derivada.) Veja este exemplo:

```cpp
Latao blipos;                                              // classe base
LataoPlus snips("Rafe Plosh", 91191, 3993.19, 600.0, 0.12); // classe derivada
blipos = snips; // atribui objeto derivado ao objeto base
```

Qual operador de atribuição é usado? Lembre-se de que a instrução de atribuição é traduzida em um método que é invocado pelo objeto à esquerda:

```cpp
blipos.operator=(snips);
```

Aqui o objeto à esquerda é um objeto `Latao`, portanto ele invoca a função `Latao::operator=(const Latao &)`. A relação *é-um* permite que a referência a `Latao` se refira a um objeto de classe derivada, como `snips`. O operador de atribuição lida apenas com membros da classe base, portanto os membros `limiteEmprestimo` e outros membros de `LataoPlus` de `snips` são ignorados na atribuição. Em resumo, você pode atribuir um objeto derivado a um objeto base, e apenas os membros da classe base estão envolvidos.

E o inverso? Você pode atribuir um objeto de classe base a um objeto derivado? Veja este exemplo:

```cpp
Latao gp("Griff Hexbait", 21234, 1200); // classe base
LataoPlus temp;                          // classe derivada
temp = gp; // possível?
```

Aqui a instrução de atribuição seria traduzida da seguinte forma:

```cpp
temp.operator=(gp);
```

O objeto à esquerda é um objeto `LataoPlus`, portanto ele invoca a função `LataoPlus::operator=(const LataoPlus &)`. No entanto, uma referência de classe derivada não pode se referir automaticamente a um objeto de classe base, portanto esse código não funcionará a menos que haja também um construtor de conversão:

```cpp
LataoPlus(const Latao &);
```

Pode ocorrer, como é o caso para a classe `LataoPlus`, que o construtor de conversão seja um construtor com um argumento de classe base mais argumentos adicionais, desde que os argumentos adicionais tenham valores padrão:

```cpp
LataoPlus(const Latao & la, double le = 500, double t = 0.1);
```

Se houver um construtor de conversão, o programa usa esse construtor para criar um objeto `LataoPlus` temporário a partir de `gp`, que é então usado como argumento para o operador de atribuição.

Alternativamente, você poderia definir um operador de atribuição para atribuir uma classe base a uma classe derivada:

```cpp
LataoPlus & LataoPlus::operator=(const Latao &) {...}
```

Aqui os tipos correspondem exatamente à instrução de atribuição, e nenhuma conversão de tipo é necessária.

Em resumo, a resposta para a pergunta "Você pode atribuir um objeto de classe base a um objeto derivado?" é "Talvez". Você pode se a classe derivada tem um construtor que define a conversão de um objeto de classe base para um objeto de classe derivada. E você pode se a classe derivada define um operador de atribuição para atribuir um objeto de classe base a um objeto derivado. Se nenhuma dessas duas condições se mantiver, então você não pode fazer a atribuição a menos que use uma conversão de tipo explícita.

#### Membros Privados Versus Protegidos

Lembre-se de que membros protegidos agem como membros públicos no que diz respeito a uma classe derivada, mas agem como membros privados para o mundo em geral. Uma classe derivada pode acessar membros protegidos de uma classe base diretamente, mas pode acessar membros privados apenas via funções membro da classe base. Assim, tornar os membros da classe base privados oferece mais segurança, enquanto torná-los protegidos simplifica a codificação e acelera o acesso. Stroustrup, em seu livro *The Design and Evolution of C++*, indica que é melhor usar membros de dados privados em vez de protegidos, mas que métodos protegidos são úteis.

#### Considerações Sobre Métodos Virtuais

Quando você projeta uma classe base, você tem que decidir se deve tornar os métodos de classe virtuais. Se você quiser que uma classe derivada seja capaz de redefinir um método, você define o método como virtual na classe base. Isso permite a ligação tardia, ou dinâmica. Se você não quiser que o método seja redefinido, você não o torna virtual. Isso não impede alguém de redefinir o método, mas deve ser interpretado como significando que você não quer que ele seja redefinido.

Observe que código inapropriado pode contornar a ligação dinâmica. Considere, por exemplo, as duas funções seguintes:

```cpp
void mostrar(const Latao & rba)
{
    rba.VerConta();
    cout << endl;
}
void inadequado(Latao ba)
{
    ba.VerConta();
    cout << endl;
}
```

A primeira função passa um objeto por referência, e a segunda passa um objeto por valor.

Agora suponha que você use cada uma delas com um argumento de classe derivada:

```cpp
LataoPlus zumbido("Buzz Parsec", 00001111, 4300);
mostrar(zumbido);
inadequado(zumbido);
```

A chamada à função `mostrar()` resulta no argumento `rba` sendo uma referência ao objeto `LataoPlus` `zumbido`, portanto `rba.VerConta()` é interpretado como a versão de `LataoPlus`, como deve ser. Mas na função `inadequado()`, que passa um objeto por valor, `ba` é um objeto `Latao` construído pelo construtor `Latao(const Latao &)`. (O upcasting automático permite que o argumento do construtor se refira a um objeto `LataoPlus`.) Assim, em `inadequado()`, `ba.VerConta()` é a versão de `Latao`, portanto apenas o componente `Latao` de `zumbido` é exibido.

#### Considerações Sobre Destrutores

Como mencionado anteriormente, um destrutor de classe base deve ser virtual. Dessa forma, quando você exclui um objeto derivado via um ponteiro ou referência de classe base para o objeto, o programa usa o destrutor de classe derivada seguido pelo destrutor de classe base, em vez de usar apenas o destrutor de classe base.

#### Considerações Sobre Funções Amigas

Como uma função amiga não é realmente um membro de classe, ela não é herdada. No entanto, você ainda pode querer que um amigo de uma classe derivada use um amigo da classe base. A maneira de fazer isso é converter em *type cast* uma referência ou ponteiro de classe derivada para o equivalente de classe base e então usar a referência ou ponteiro convertido para invocar o amigo da classe base:

```cpp
ostream & operator<<(ostream & os, const comAD & hs)
{
    // type cast para corresponder a operator<<(ostream & , const baseAD &)
    os << (const baseAD &) hs;
    os << "Estilo: " << hs.estilo << endl;
    return os;
}
```

Você também pode usar o operador `dynamic_cast<>`, discutido no Capítulo 15, "Friends, Exceções e Mais", para a conversão de tipo:

```cpp
os << dynamic_cast<const baseAD &> (hs);
```

Por razões discutidas no Capítulo 15, essa seria a forma preferida de conversão de tipo.

#### Observações Sobre o Uso de Métodos da Classe Base

Objetos derivados publicamente podem usar métodos da classe base de várias formas:

- Um objeto derivado usa automaticamente métodos herdados da classe base se a classe derivada não redefiniu o método.
- Um destrutor de classe derivada invoca automaticamente o construtor da classe base.
- Um construtor de classe derivada invoca automaticamente o construtor padrão da classe base se você não especificar outro construtor em uma lista de inicialização de membros.
- Um construtor de classe derivada invoca explicitamente o construtor da classe base especificado em uma lista de inicialização de membros.
- Métodos de classe derivada podem usar o operador de resolução de escopo para invocar métodos públicos e protegidos da classe base.
- Friends de uma classe derivada podem converter em *type cast* uma referência ou ponteiro de classe derivada para uma referência ou ponteiro de classe base e então usar essa referência ou ponteiro para invocar um amigo da classe base.

### Resumo das Funções de Classe

As funções de classe C++ vêm em muitas variações. Algumas podem ser herdadas, e outras não. Algumas funções operadores podem ser tanto funções membro quanto friends, e outras só podem ser funções membro. A Tabela 13.1 resume essas propriedades.

**Tabela 13.1 — Propriedades de Funções Membro**

| Função          | Herdada | Membro ou Friend | Gerada por Padrão | Pode Ser Virtual | Pode Ter Tipo de Retorno |
|-----------------|---------|------------------|-------------------|------------------|--------------------------|
| Construtor      | Não     | Membro           | Sim               | Não              | Não                      |
| Destrutor       | Não     | Membro           | Sim               | Sim              | Não                      |
| `=`             | Não     | Membro           | Sim               | Sim              | Sim                      |
| `&`             | Sim     | Qualquer         | Sim               | Sim              | Sim                      |
| Conversão       | Sim     | Membro           | Não               | Sim              | Não                      |
| `()`            | Sim     | Membro           | Não               | Sim              | Sim                      |
| `[]`            | Sim     | Membro           | Não               | Sim              | Sim                      |
| `->`            | Sim     | Membro           | Não               | Sim              | Sim                      |
| `op=`           | Sim     | Qualquer         | Não               | Sim              | Sim                      |
| `new`           | Sim     | Membro estático  | Não               | Não              | `void *`                 |
| `delete`        | Sim     | Membro estático  | Não               | Não              | `void`                   |
| Outros operadores | Sim   | Qualquer         | Não               | Sim              | Sim                      |
| Outros membros  | Sim     | Membro           | Não               | Sim              | Sim                      |
| Friends         | Não     | Friend           | Não               | Não              | Sim                      |

---

## Resumo

A herança (inheritance) permite que você adapte o código de programação às suas necessidades específicas definindo uma nova classe (classe derivada) a partir de uma classe existente (classe base). A herança pública modela uma relação *é-um*, ou seja, um objeto de classe derivada também deve ser uma espécie de objeto de classe base. Como parte do modelo *é-um*, uma classe derivada herda os membros de dados e a maioria dos métodos da classe base. No entanto, uma classe derivada não herda os construtores, destrutores e operadores de atribuição da classe base. Uma classe derivada pode acessar os membros públicos e protegidos da classe base diretamente, e os membros privados da classe base via métodos públicos e protegidos da classe base. Você pode então adicionar novos membros de dados e métodos à classe, e usar a classe derivada como classe base para desenvolvimento posterior. Cada classe derivada requer seus próprios construtores. Quando um programa cria um objeto de classe derivada, ele primeiro chama um construtor de classe base e depois o construtor de classe derivada. Quando um programa exclui um objeto, ele primeiro chama o destrutor de classe derivada e depois o destrutor de classe base.

Se uma classe deve ser uma classe base, você pode escolher usar membros protegidos em vez de membros privados para que as classes derivadas possam acessar esses membros diretamente. No entanto, usar membros privados, em geral, reduz o escopo para erros de programação. Se você pretende que uma classe derivada possa redefinir um método de classe base, você deve torná-lo uma função virtual declarando-a com a palavra-chave `virtual`. Isso permite que objetos acessados por ponteiros ou referências sejam tratados com base no tipo do objeto em vez do tipo da referência ou ponteiro. Em particular, o destrutor para uma classe base normalmente deve ser virtual.

Você pode querer definir uma ABC que define uma interface sem entrar em questões de implementação. Por exemplo, você poderia definir uma classe abstrata `Forma` a partir da qual classes de formas específicas, como `Circulo` e `Quadrado`, serão derivadas. Uma ABC deve incluir pelo menos um método virtual puro. Você declara um método virtual puro colocando `= 0` antes do ponto e vírgula de fechamento da declaração:

```cpp
virtual double area() const = 0;
```

Você não tem que definir métodos virtuais puros, e você não pode criar um objeto de uma classe que contém membros virtuais puros. Em vez disso, métodos virtuais puros servem para definir uma interface comum a ser usada pelas classes derivadas.

---

## Revisão do Capítulo

**1.** O que uma classe derivada herda de uma classe base?

**2.** O que uma classe derivada *não* herda de uma classe base?

**3.** Suponha que o tipo de retorno para a função `baseAD::operator=()` fosse definido como `void` em vez de `baseAD &`. Que efeito, se algum, isso teria? E se o tipo de retorno fosse `baseAD` em vez de `baseAD &`?

**4.** Em que ordem os construtores de classe e os destrutores de classe são chamados quando um objeto de classe derivada é criado e excluído?

**5.** Se uma classe derivada não adiciona nenhum membro de dados à classe base, a classe derivada precisa de construtores?

**6.** Suponha que uma classe base e uma classe derivada definam um método com o mesmo nome e um objeto de classe derivada invoque o método. Qual método é chamado?

**7.** Quando uma classe derivada deve definir um operador de atribuição?

**8.** Você pode atribuir o endereço de um objeto de uma classe derivada a um ponteiro para a classe base? Você pode atribuir o endereço de um objeto de uma classe base a um ponteiro para a classe derivada?

**9.** Você pode atribuir um objeto de uma classe derivada a um objeto da classe base? Você pode atribuir um objeto de uma classe base a um objeto da classe derivada?

**10.** Suponha que você define uma função que recebe uma referência a um objeto de classe base como argumento. Por que essa função também pode usar um objeto de classe derivada como argumento?

**11.** Suponha que você define uma função que recebe um objeto de classe base como argumento (isto é, a função passa um objeto de classe base por valor). Por que essa função também pode usar um objeto de classe derivada como argumento?

**12.** Por que geralmente é melhor passar objetos por referência do que por valor?

**13.** Suponha que `Corporacao` é uma classe base e `CorporacaoPublica` é uma classe derivada. Suponha também que cada classe define uma função membro `cabeca()`, que `ph` é um ponteiro para o tipo `Corporacao`, e que `ph` recebe o endereço de um objeto `CorporacaoPublica`. Como `ph->cabeca()` é interpretado se a classe base define `cabeca()` como:
- a. Método regular não virtual
- b. Método virtual

**14.** O que há de errado, se houver algo, com o seguinte código?

```cpp
class Cozinha
{
private:
    double metragem_coz;
public:
    Cozinha() {metragem_coz = 0.0; }
    virtual double area() const { return metragem_coz * metragem_coz; }
};
class Casa : public Cozinha
{
private:
    double metragem_total;
public:
    Casa() {metragem_total += metragem_coz;}
    double area(const char *s) const { cout << s; return metragem_total; }
};
```

---

## Exercícios de Programação

**1.** Comece com a seguinte declaração de classe:

```cpp
// classe base
class Cd { // representa um CD
private:
    char executores[50];
    char gravadora[20];
    int faixas;        // número de faixas
    double duracao;    // duração em minutos
public:
    Cd(char * s1, char * s2, int n, double x);
    Cd(const Cd & d);
    Cd();
    ~Cd();
    void Report() const; // relata todos os dados do CD
    Cd & operator=(const Cd & d);
};
```

Derive uma classe `Classico` que adiciona um array de membros `char` que armazenará uma string identificando a obra principal no CD. Se a classe base exige que alguma função seja virtual, modifique a declaração da classe base para torná-la assim. Se um método declarado não for necessário, remova-o da definição. Teste seu produto com o seguinte programa:

```cpp
#include <iostream>
using namespace std;
#include "classico.h" // que conterá #include cd.h

void Bravo(const Cd & disco);
int main()
{
    Cd c1("Beatles", "Capitol", 14, 35.5);
    Classico c2 = Classico("Sonata para Piano em Si bemol, Fantasia em Dó",
              "Alfred Brendel", "Philips", 2, 57.17);
    Cd *pcd = &c1;
    cout << "Usando o objeto diretamente:\n";
    c1.Report(); // usa método de Cd
    c2.Report(); // usa método de Classico
    cout << "Usando ponteiro tipo Cd * para objetos:\n";
    pcd->Report(); // usa método de Cd para objeto cd
    pcd = &c2;
    pcd->Report(); // usa método de Classico para objeto classico
    cout << "Chamando função com argumento referência Cd:\n";
    Bravo(c1);
    Bravo(c2);
    cout << "Testando atribuição: ";
    Classico copia;
    copia = c2;
    copia.Report();

    return 0;
}
void Bravo(const Cd & disco)
{
    disco.Report();
}
```

**2.** Faça o Exercício de Programação 1, mas use alocação dinâmica de memória em vez de arrays de tamanho fixo para as várias strings rastreadas pelas duas classes.

**3.** Revise a hierarquia de classes `baseAD`-`semAD`-`comAD` para que todas as três classes sejam derivadas de uma ABC. Teste o resultado com um programa similar ao da Listagem 13.10. Ou seja, ele deve conter um array de ponteiros para a ABC e permitir que o usuário tome decisões em tempo de execução sobre que tipos de objetos são criados. Adicione métodos virtuais `VerDados()` às definições de classe para lidar com a exibição dos dados.

**4.** A Ordem Benevolente dos Programadores mantém uma coleção de vinho do Porto engarrafado. Para descrevê-lo, o Portmestre da BOP projetou uma classe `Porto`, conforme declarada aqui:

```cpp
#include <iostream>
using namespace std;
class Porto
{
private:
    char * marca;
    char estilo[20]; // ex.: Tawny, Ruby, Vintage
    int garrafas;
public:
    Porto(const char * mar = "nenhuma", const char * est = "nenhum",
          int g = 0);
    Porto(const Porto & p); // construtor de cópia
    virtual ~Porto() { delete [] marca; }
    Porto & operator=(const Porto & p);
    Porto & operator+=(int g); // adiciona g a garrafas
    Porto & operator-=(int g); // subtrai g de garrafas, se disponível
    int ContagemGarrafas() const { return garrafas; }
    virtual void Mostrar() const;
    friend ostream & operator<<(ostream & os, const Porto & p);
};
```

O método `Mostrar()` apresenta as informações no seguinte formato:

```
Marca: Gallo
Tipo: Tawny
Garrafas: 20
```

A função `operator<<()` apresenta as informações no seguinte formato (sem caractere de nova linha no final):

```
Gallo, Tawny, 20
```

O Portmestre completou as definições de métodos para a classe `Porto` e então derivou a classe `PortoAntigo` da seguinte forma antes de ser dispensado de seu cargo por acidentalmente redirecionar uma garrafa de '45 Cockburn para alguém preparando um molho de churrasco experimental:

```cpp
class PortoAntigo : public Porto // estilo necessariamente = "vintage"
{
private:
    char * apelido; // ex.: "O Nobre" ou "Veludo Velho", etc.
    int ano;        // ano da safra
public:
    PortoAntigo();
    PortoAntigo(const char * mar, int g, const char * ap, int a);
    PortoAntigo(const PortoAntigo & vp);
    ~PortoAntigo() { delete [] apelido; }
    PortoAntigo & operator=(const PortoAntigo & vp);
    void Mostrar() const;
    friend ostream & operator<<(ostream & os, const PortoAntigo & vp);
};
```

Você assume o trabalho de completar o código de `PortoAntigo`.

- **a.** Sua primeira tarefa é recriar as definições de métodos de `Porto` porque o antigo Portmestre as incinerou ao ser dispensado.
- **b.** Sua segunda tarefa é explicar por que certos métodos são redefinidos e outros não.
- **c.** Sua terceira tarefa é explicar por que `operator=()` e `operator<<()` não são virtuais.
- **d.** Sua quarta tarefa é fornecer definições para os métodos de `PortoAntigo`.

---

[Anterior](capitulo-13-02-virtual-binding.md) | [Índice](README.md) | [Próximo](capitulo-14-01-composicao-heranca-privada.md)
