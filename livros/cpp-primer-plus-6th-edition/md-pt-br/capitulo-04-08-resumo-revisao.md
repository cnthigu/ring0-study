# Capitulo 4 - Tipos Compostos (Compound Types)
## Parte 8: Resumo, Revisao do Capitulo e Exercicios de Programacao

> Traducao nao oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Navegacao: [Anterior](capitulo-04-07-alternativas-arrays.md) | [Indice](README.md) | [Proximo](capitulo-05.md)

---

## Resumo

Arrays, estruturas e ponteiros sao tres tipos compostos do C++. Um array pode conter varios valores, todos do mesmo tipo, em um unico objeto de dados. Usando um indice (subscrito), voce pode acessar os elementos individuais de um array.

Uma estrutura pode conter varios valores de tipos diferentes em um unico objeto de dados, e voce pode usar o operador de membro (`.`) para acessar membros individuais. O primeiro passo no uso de estruturas e criar um template de estrutura que define quais membros a estrutura contem. O nome, ou etiqueta, para esse template se torna entao um novo identificador de tipo. Voce pode entao declarar variaveis de estrutura desse tipo.

Uma uniao pode conter um unico valor, mas pode ser de uma variedade de tipos, com o nome do membro indicando qual modo esta sendo usado.

Ponteiros sao variaveis projetadas para conter enderecos. Dizemos que um ponteiro aponta para o endereco que ele contem. A declaracao do ponteiro sempre informa para qual tipo de objeto um ponteiro aponta. Aplicar o operador de desreferenciamento (`*`) a um ponteiro retorna o valor na localizacao para a qual o ponteiro aponta.

Uma string e uma serie de caracteres terminada por um caractere nulo. Uma string pode ser representada por uma constante de string entre aspas, na qual o caractere nulo esta implicitamente subentendido. Voce pode armazenar uma string em um array de `char`, e voce pode representar uma string com um ponteiro-para-`char` inicializado para apontar para a string. A funcao `strlen()` retorna o comprimento de uma string, sem contar o caractere nulo. A funcao `strcpy()` copia uma string de um lugar para outro. Ao usar essas funcoes, voce inclui o arquivo de cabecalho `cstring` ou `string.h`.

A classe `string` do C++, suportada pelo arquivo de cabecalho `string`, oferece um meio alternativo e mais amigavel de lidar com strings. Em particular, os objetos `string` sao automaticamente redimensionados para acomodar strings armazenadas, e voce pode usar o operador de atribuicao para copiar uma string.

O operador `new` permite solicitar memoria para um objeto de dados enquanto um programa esta em execucao. O operador retorna o endereco da memoria que obtem, e voce pode atribuir esse endereco a um ponteiro. O unico meio de acessar essa memoria e usar o ponteiro. Se o objeto de dados e uma variavel simples, voce pode usar o operador de desreferenciamento (`*`) para indicar um valor. Se o objeto de dados e um array, voce pode usar o ponteiro como se fosse o nome do array para acessar os elementos. Se o objeto de dados e uma estrutura, voce pode usar o operador de desreferenciamento de ponteiro (`->`) para acessar membros da estrutura.

Ponteiros e arrays estao intimamente conectados. Se `ar` for um nome de array, a expressao `ar[i]` e interpretada como `*(ar + i)`, com o nome do array interpretado como o endereco do primeiro elemento do array. Assim, o nome do array desempenha o mesmo papel que um ponteiro. Por sua vez, voce pode usar um nome de ponteiro com notacao de array para acessar elementos em um array alocado com `new`.

Os operadores `new` e `delete` permitem controlar explicitamente quando os objetos de dados sao alocados e quando sao retornados ao pool de memoria. As variaveis automaticas, que sao as declaradas dentro de uma funcao, e as variaveis estaticas, que sao definidas fora de uma funcao ou com a palavra-chave `static`, sao menos flexiveis. Uma variavel automatica entra em existencia quando o bloco que a contem (tipicamente uma definicao de funcao) e entrado, e expira quando o bloco e deixado. Uma variavel estatica persiste durante a duracao de um programa.

A Biblioteca Padrao de Templates (STL), adicionada pelo padrao C++98, fornece uma classe template `vector` que oferece uma alternativa a arrays dinamicos feitos voce mesmo. O C++11 fornece uma classe template `array` que oferece uma alternativa a arrays nativos de tamanho fixo.

---

## Revisao do Capitulo

**1.** Como voce declararia cada um dos seguintes?

   a. `atores` e um array de 30 `char`.
   b. `betinha` e um array de 100 `short`.
   c. `chico` e um array de 13 `float`.
   d. `dipsea` e um array de 64 `long double`.

**2.** A Questao de Revisao 1 usa a classe template `array` em vez de arrays nativos.

**3.** Declare um array de cinco `int`s e inicialize-o com os primeiros cinco inteiros positivos impares.

**4.** Escreva uma instrucao que atribua a soma do primeiro e do ultimo elemento do array na Questao 3 a variavel `par`.

**5.** Escreva uma instrucao que exiba o valor do segundo elemento do array `float` chamado `ideias`.

**6.** Declare um array de `char` e inicialize-o com a string `"cheeseburger"`.

**7.** Declare um objeto `string` e inicialize-o com a string `"Salada Waldorf"`.

**8.** Elabore uma declaracao de estrutura que descreva um peixe. A estrutura deve incluir o tipo, o peso em gramas inteiras e o comprimento em centimetros fracionarios.

**9.** Declare uma variavel do tipo definido na Questao 8 e inicialize-a.

**10.** Use `enum` para definir um tipo chamado `Resposta` com os valores possiveis `Sim`, `Nao` e `Talvez`. `Sim` deve ser `1`, `Nao` deve ser `0` e `Talvez` deve ser `2`.

**11.** Suponha que `ted` seja uma variavel `double`. Declare um ponteiro que aponte para `ted` e use o ponteiro para exibir o valor de `ted`.

**12.** Suponha que `melaco` seja um array de 10 `float`s. Declare um ponteiro que aponte para o primeiro elemento de `melaco` e use o ponteiro para exibir o primeiro e o ultimo elemento do array.

**13.** Escreva um fragmento de codigo que peca ao usuario que insira um inteiro positivo e, em seguida, crie um array dinamico desse numero de `int`s. Faca isso usando `new` e, em seguida, novamente usando um objeto `vector`.

**14.** O seguinte e um codigo valido? Se sim, o que ele imprime?

```cpp
cout << (int *) "Casa dos bytes alegres";
```

**15.** Escreva um fragmento de codigo que aloque dinamicamente uma estrutura do tipo descrito na Questao 8 e, em seguida, leia um valor para o membro `tipo` da estrutura.

**16.** A Listagem 4.6 ilustra um problema criado ao seguir entrada numerica com entrada de string orientada a linha. Como substituir isto:

```cpp
cin.getline(endereco, 80);
```

por isto:

```cpp
cin >> endereco;
```

afetaria o funcionamento desse programa?

**17.** Declare um objeto `vector` de 10 objetos `string` e um objeto `array` de 10 objetos `string`. Mostre os arquivos de cabecalho necessarios e nao use `using`. Use uma `const` para o numero de strings.

---

## Exercicios de Programacao

**1.** Escreva um programa C++ que solicite e exiba informacoes conforme mostrado no seguinte exemplo de saida:

```
Qual e o seu primeiro nome? Betty Sue
Qual e o seu sobrenome? Yewe
Que nota voce merece? B
Qual e a sua idade? 22
Nome: Yewe, Betty Sue
Nota: C
Idade: 22
```

Note que o programa deve ser capaz de aceitar primeiros nomes compostos por mais de uma palavra. Observe tambem que o programa ajusta a nota para baixo — ou seja, sobe uma letra. Assuma que o usuario solicita A, B ou C para que voce nao precise se preocupar com a lacuna entre D e F.

**2.** Reescreva a Listagem 4.4, usando a classe `string` do C++ em vez de arrays `char`.

**3.** Escreva um programa que peca ao usuario que insira seu primeiro nome e, em seguida, seu sobrenome, e que entao construa, armazene e exiba uma terceira string, consistindo do sobrenome do usuario seguido de uma virgula, um espaco e o primeiro nome. Use arrays `char` e funcoes do arquivo de cabecalho `cstring`. Uma execucao de exemplo poderia ser assim:

```
Digite seu primeiro nome: Flip
Digite seu sobrenome: Fleming
Aqui esta a informacao em uma unica string: Fleming, Flip
```

**4.** Escreva um programa que peca ao usuario que insira seu primeiro nome e, em seguida, seu sobrenome, e que entao construa, armazene e exiba uma terceira string consistindo do sobrenome do usuario seguido de uma virgula, um espaco e o primeiro nome. Use objetos `string` e metodos do arquivo de cabecalho `string`. Uma execucao de exemplo poderia ser assim:

```
Digite seu primeiro nome: Flip
Digite seu sobrenome: Fleming
Aqui esta a informacao em uma unica string: Fleming, Flip
```

**5.** A estrutura `BarraDeDoces` contem tres membros. O primeiro membro armazena o nome da marca de uma barra de doces. O segundo membro armazena o peso (que pode ter uma parte fracionaria) da barra de doces, e o terceiro membro armazena o numero de calorias (um valor inteiro) na barra de doces. Escreva um programa que declare essa estrutura e crie uma variavel `BarraDeDoces` chamada `lanche`, inicializando seus membros como `"Mocha Munch"`, `2.3` e `350`, respectivamente. A inicializacao deve fazer parte da declaracao de `lanche`. Por fim, o programa deve exibir o conteudo da variavel `lanche`.

**6.** A estrutura `BarraDeDoces` contem tres membros, conforme descrito no Exercicio de Programacao 5. Escreva um programa que crie um array de tres estruturas `BarraDeDoces`, inicialize-as com os valores de sua escolha e, em seguida, exiba o conteudo de cada estrutura.

**7.** William Wingate dirige um servico de analise de pizzas. Para cada pizza, ele precisa registrar as seguintes informacoes:

- O nome da empresa de pizza, que pode consistir em mais de uma palavra
- O diametro da pizza
- O peso da pizza

Elabore uma estrutura que possa conter essas informacoes e escreva um programa que use uma variavel de estrutura desse tipo. O programa deve pedir ao usuario que insira cada uma das informacoes acima e, em seguida, o programa deve exibir essas informacoes. Use `cin` (ou seus metodos) e `cout`.

**8.** Faca o Exercicio de Programacao 7, mas use `new` para alocar uma estrutura em vez de declarar uma variavel de estrutura. Alem disso, faca o programa solicitar o diametro da pizza antes de solicitar o nome da empresa de pizza.

**9.** Faca o Exercicio de Programacao 6, mas em vez de declarar um array de tres estruturas `BarraDeDoces`, use `new` para alocar o array dinamicamente.

**10.** Escreva um programa que peca ao usuario que insira tres tempos para os 40 jardas (ou 40 metros, se preferir) e, em seguida, exiba os tempos e a media. Use um objeto `array` para armazenar os dados. (Use um array nativo se `array` nao estiver disponivel.)

---

> Navegacao: [Anterior](capitulo-04-07-alternativas-arrays.md) | [Indice](README.md) | [Proximo](capitulo-05.md)
