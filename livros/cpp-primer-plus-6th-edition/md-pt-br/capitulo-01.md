# Capitulo 1 - Comecando com C++ (Getting Started with C++)

> Traducao nao oficial do livro *C++ Primer Plus, Sixth Edition* (Stephen Prata, 2012).
> Texto adaptado para portugues brasileiro com fins educacionais. Paginas originais: 1-26.
> Repositorio: [ring0-study](https://github.com/cnthigu/ring0-study)

> Navegacao: [Indice](README.md) | [Proximo](capitulo-02.md)

---

## Introducao

### A Abordagem do Primer

*C++ Primer Plus* traz diversas qualidades para a tarefa de apresentar todo esse material. Ele se baseia na tradicao de primer iniciada por *C Primer Plus* quase duas decadas atras e adota sua filosofia bem-sucedida:

- Um primer deve ser um guia facil de usar e acessivel.
- Um primer nao pressupoe que voce ja esteja familiarizado com todos os conceitos relevantes de programacao.
- Um primer enfatiza o aprendizado pratico com exemplos breves e facilmente digitados, desenvolvendo seu entendimento um ou dois conceitos por vez.
- Um primer esclarece conceitos com ilustracoes.
- Um primer fornece perguntas e exercicios para testar seu entendimento, tornando o livro adequado para o aprendizado autonomo ou para uso em sala de aula.

Seguindo esses principios, o livro ajuda voce a entender essa linguagem rica e como usa-la. Por exemplo:

- Ele fornece orientacao conceitual sobre quando usar determinados recursos, como usar heranca publica (public inheritance) para modelar o que sao conhecidos como relacionamentos *is-a* (e-um).
- Ele ilustra expressoes e tecnicas comuns de programacao em C++.
- Ele oferece uma variedade de caixas de destaque, incluindo dicas, alertas e notas.

O autor e os editores deste livro fazem o melhor possivel para manter a apresentacao direta, simples e divertida. Nosso objetivo e que, ao final do livro, voce seja capaz de escrever programas solidos e eficazes e de se divertir fazendo isso.

### Codigo de Exemplo Usado neste Livro

Este livro fornece uma grande quantidade de codigo de exemplo, a maior parte na forma de programas completos. Como nas edicoes anteriores, este livro pratica C++ generico, de forma a nao estar vinculado a nenhum tipo especifico de computador, sistema operacional ou compilador. Assim, os exemplos foram testados em um sistema Windows 7, em um sistema Macintosh OS X e em um sistema Linux. Os programas que usam recursos do C++11 requerem compiladores que suportem esses recursos, mas os demais programas devem funcionar com qualquer sistema compativel com C++98/C++03.

O codigo de exemplo dos programas completos descritos neste livro esta disponivel no site do livro. Consulte o link de registro na contracapa para mais informacoes.

### Como Este Livro Esta Organizado

Este livro esta dividido em 18 capitulos e 10 apendices, resumidos aqui:

- **Capitulo 1: Comecando com C++** — O Capitulo 1 relata como Bjarne Stroustrup criou a linguagem de programacao C++ adicionando suporte a programacao orientada a objetos a linguagem C. Voce aprendera as distincoes entre linguagens procedurais, como C, e linguagens orientadas a objetos, como C++. Voce lera sobre o trabalho conjunto ANSI/ISO para desenvolver um padrao C++. Este capitulo discute a mecanica de criacao de um programa C++, descrevendo a abordagem para varios compiladores C++ atuais. Por fim, descreve as convencoes usadas neste livro.

- **Capitulo 2: Primeiros Passos em C++** — O Capitulo 2 guia voce pelo processo de criacao de programas C++ simples. Voce aprendera sobre o papel da funcao `main()` e sobre alguns dos tipos de instrucoes que os programas C++ usam. Voce usara os objetos predefinidos `cout` e `cin` para saida e entrada do programa, e aprendera a criar e usar variaveis. Por fim, voce sera apresentado as funcoes, os modulos de programacao do C++.

- **Capitulo 3: Lidando com Dados** — O C++ fornece tipos embutidos para armazenar dois tipos de dados: inteiros (numeros sem partes fracionarias) e numeros de ponto flutuante (numeros com partes fracionarias). Para atender aos diversos requisitos dos programadores, o C++ oferece varios tipos em cada categoria. O Capitulo 3 discute esses tipos, incluindo a criacao de variaveis e a escrita de constantes de varios tipos. Voce tambem aprendera como o C++ lida com conversoes implicitas e explicitas de um tipo para outro.

- **Capitulo 4: Tipos Compostos** — O C++ permite que voce construa tipos mais elaborados a partir dos tipos basicos embutidos. A forma mais avancada e a classe, discutida nos Capitulos 9 a 13. O Capitulo 4 discute outras formas, incluindo arrays (vetores), que armazenam varios valores de um unico tipo; estruturas (structs), que armazenam varios valores de tipos diferentes; e ponteiros (pointers), que identificam locais na memoria. Voce tambem aprendera como criar e armazenar strings de texto e como tratar I/O de texto usando arrays de caracteres estilo C e a classe `string` do C++. Por fim, voce aprendera algumas das maneiras pelas quais o C++ trata a alocacao de memoria, incluindo o uso dos operadores `new` e `delete` para gerenciar a memoria explicitamente.

- **Capitulo 5: Loops e Expressoes Relacionais** — Programas frequentemente precisam executar acoes repetitivas, e o C++ oferece tres estruturas de repeticao para esse proposito: o loop `for`, o loop `while` e o loop `do while`. Esses loops precisam saber quando devem terminar, e os operadores relacionais do C++ permitem criar testes para guiar esses loops. No Capitulo 5 voce aprende a criar loops que leem e processam entrada caractere por caractere. Por fim, voce aprendera a criar arrays bidimensionais e a usar loops aninhados para processa-los.

- **Capitulo 6: Instrucoes de Desvio e Operadores Logicos** — Programas podem se comportar de forma inteligente se conseguirem adaptar seu comportamento as circunstancias. No Capitulo 6 voce aprendera a controlar o fluxo do programa usando as instrucoes `if`, `if else` e `switch`, e o operador condicional. Voce aprendera a usar operadores logicos para ajudar a expressar testes de tomada de decisao. Alem disso, voce conhecera a biblioteca de funcoes `cctype` para avaliar relacoes de caracteres, como testar se um caractere e um digito ou um caractere nao imprimivel. Por fim, voce tera uma visao introdutoria de I/O de arquivos.

- **Capitulo 7: Funcoes — Modulos de Programacao do C++** — As funcoes sao os blocos de construcao fundamentais da programacao C++. O Capitulo 7 concentra-se nos recursos que as funcoes C++ compartilham com as funcoes C. Em particular, voce revisara o formato geral de uma definicao de funcao e examinara como prototipos de funcao aumentam a confiabilidade dos programas. Tambem investigara como escrever funcoes para processar arrays, strings de caracteres e estruturas. Em seguida, voce aprendera sobre recursao, que ocorre quando uma funcao chama a si mesma, e vera como ela pode ser usada para implementar uma estrategia de divisao e conquista. Por fim, voce conhecera ponteiros para funcoes, que permitem usar um argumento de funcao para dizer a uma funcao que use uma segunda funcao.

- **Capitulo 8: Aventuras com Funcoes** — O Capitulo 8 explora os novos recursos que o C++ adiciona as funcoes. Voce aprendera sobre funcoes inline (em linha), que podem acelerar a execucao do programa ao custo de um tamanho de programa maior. Voce trabalhara com variaveis de referencia (reference variables), que fornecem uma maneira alternativa de passar informacoes para funcoes. Argumentos padrao (default arguments) permitem que uma funcao forneca automaticamente valores para argumentos de funcao que voce omite de uma chamada de funcao. Sobrecarga de funcao (function overloading) permite criar funcoes com o mesmo nome, mas que recebem listas de argumentos diferentes. Todos esses recursos sao usados com frequencia no design de classes. Voce tambem aprendera sobre templates de funcao (function templates), que permitem especificar o design de uma familia de funcoes relacionadas.

- **Capitulo 9: Modelos de Memoria e Namespaces** — O Capitulo 9 discute a montagem de programas com varios arquivos. Ele examina as opcoes de alocacao de memoria, analisando diferentes metodos de gerenciamento de memoria e escopo (scope), ligacao (linkage) e namespaces, que determinam quais partes de um programa conhecem uma variavel.

- **Capitulo 10: Objetos e Classes** — Uma classe e um tipo definido pelo usuario, e um objeto (como uma variavel) e uma instancia de uma classe. O Capitulo 10 apresenta a programacao orientada a objetos e o design de classes. Uma declaracao de classe descreve as informacoes armazenadas em um objeto da classe e tambem as operacoes (metodos de classe) permitidas para objetos da classe. Algumas partes de um objeto sao visiveis para o mundo externo (a parte publica), e algumas sao ocultas (a parte privada). Metodos de classe especiais (construtores e destrutores) entram em acao quando os objetos sao criados e destruidos. Voce aprendera sobre tudo isso e outros detalhes de classes neste capitulo, e vera como as classes podem ser usadas para implementar TADs (Tipos Abstratos de Dados), como uma pilha (stack).

- **Capitulo 11: Trabalhando com Classes** — No Capitulo 11 voce aprofundara seu entendimento das classes. Primeiro, voce aprendera sobre sobrecarga de operadores (operator overloading), que permite definir como operadores como `+` funcionarao com objetos de classe. Voce aprendera sobre funcoes `friend`, que podem acessar dados de classe inacessiveis ao mundo em geral. Voce vera como certos construtores e funcoes membro de operadores sobrecarregados podem ser usados para gerenciar conversoes de e para tipos de classe.

- **Capitulo 12: Classes e Alocacao Dinamica de Memoria** — Muitas vezes e util que um membro de classe aponte para memoria alocada dinamicamente. Se voce usar `new` em um construtor de classe para alocar memoria dinamica, voce assume as responsabilidades de fornecer um destrutor apropriado, definir um construtor de copia explicito e definir um operador de atribuicao explicito. O Capitulo 12 mostra como fazer isso e discute o comportamento das funcoes membro geradas implicitamente se voce nao fornecer definicoes explicitas. Voce tambem ampliara sua experiencia com classes usando ponteiros para objetos e estudando um problema de simulacao de fila.

- **Capitulo 13: Heranca de Classes** — Um dos recursos mais poderosos da programacao orientada a objetos e a heranca (inheritance), pela qual uma classe derivada herda os recursos de uma classe base, permitindo que voce reutilize o codigo da classe base. O Capitulo 13 discute a heranca publica, que modela relacionamentos *is-a*, significando que um objeto derivado e um caso especial de um objeto base. Por exemplo, um fisico e um caso especial de um cientista. Alguns relacionamentos de heranca sao polimorficos, significando que voce pode escrever codigo usando uma mistura de classes relacionadas para as quais o mesmo nome de metodo pode invocar comportamento que depende do tipo do objeto. Implementar esse tipo de comportamento requer o uso de um novo tipo de funcao membro chamado funcao virtual (virtual function). As vezes, usar classes base abstratas (abstract base classes) e a melhor abordagem para relacionamentos de heranca. Este capitulo discute essas questoes, indicando quando a heranca publica e apropriada e quando nao e.

- **Capitulo 14: Reusando Codigo em C++** — A heranca publica e apenas uma maneira de reutilizar codigo. O Capitulo 14 examina varias outras maneiras. Contencao (containment) e quando uma classe contem membros que sao objetos de outra classe. Ela pode ser usada para modelar relacionamentos *has-a* (tem-um), nos quais uma classe tem componentes de outra classe. Por exemplo, um automovel tem um motor. Voce tambem pode usar heranca privada e protegida para modelar esses relacionamentos. Este capitulo mostra como e aponta as diferencas entre as diferentes abordagens. Alem disso, voce aprendera sobre templates de classe (class templates), que permitem definir uma classe em termos de algum tipo generico nao especificado e, em seguida, usar o template para criar classes especificas em termos de tipos especificos. Por exemplo, um template de pilha permite criar uma pilha de inteiros ou uma pilha de strings. Por fim, voce aprendera sobre heranca multipla publica, pela qual uma classe pode ser derivada de mais de uma classe.

- **Capitulo 15: Friends, Excecoes e Mais** — O Capitulo 15 estende a discussao sobre `friend` para incluir classes friend e funcoes membro friend. Em seguida, apresenta varios novos desenvolvimentos em C++, comecando por excecoes (exceptions), que fornecem um mecanismo para lidar com ocorrencias incomuns no programa, como valores inadequados de argumentos de funcao e falta de memoria. Em seguida, voce aprendera sobre RTTI (Run-Time Type Identification — Identificacao de Tipo em Tempo de Execucao), um mecanismo para identificar tipos de objetos. Por fim, voce aprendera sobre as alternativas mais seguras ao typecast irrestrito.

- **Capitulo 16: A Classe string e a STL** — O Capitulo 16 discute algumas bibliotecas de classes uteis adicionadas recentemente a linguagem. A classe `string` e uma alternativa conveniente e poderosa as strings tradicionais estilo C. A classe `auto_ptr` ajuda a gerenciar a memoria alocada dinamicamente. A STL (Standard Template Library) fornece varios containers (conteineres) genericos, incluindo representacoes de template de arrays, filas, listas, conjuntos e mapas. Ela tambem fornece uma biblioteca eficiente de algoritmos genericos que podem ser usados com containers STL e tambem com arrays comuns. O template de classe `valarray` oferece suporte para arrays numericos.

- **Capitulo 17: Input, Output e Arquivos** — O Capitulo 17 revisa o I/O do C++ e discute como formatar a saida. Voce aprendera a usar metodos de classe para determinar o estado de um fluxo de entrada ou saida e ver, por exemplo, se houve uma incompatibilidade de tipo na entrada ou se o fim do arquivo foi detectado. O C++ usa heranca para derivar classes para gerenciar entrada e saida de arquivos. Voce aprendera como abrir arquivos para entrada e saida, como adicionar dados a um arquivo, como usar arquivos binarios e como obter acesso aleatorio a um arquivo. Por fim, voce aprendera como aplicar metodos padrao de I/O para ler e escrever em strings.

- **Capitulo 18: Visitando o Novo Padrao C++** — O Capitulo 18 comeca revisando varios recursos do C++11 introduzidos em capitulos anteriores, incluindo novos tipos, sintaxe de inicializacao uniforme, deducao automatica de tipo, novos smart pointers (ponteiros inteligentes) e enumeracoes com escopo. O capitulo entao discute o novo tipo de referencia rvalue e como ele e usado para implementar um novo recurso chamado semantica de movimento (move semantics). Em seguida, o capitulo aborda novos recursos de classe, expressoes lambda (lambda expressions) e templates variadicos (variadic templates). Por fim, o capitulo descreve muitos novos recursos nao cobertos nos capitulos anteriores do livro.

- **Apendice A: Bases Numericas** — O Apendice A discute numeros octais, hexadecimais e binarios.
- **Apendice B: Palavras Reservadas do C++** — O Apendice B lista as palavras-chave do C++.
- **Apendice C: O Conjunto de Caracteres ASCII** — O Apendice C lista o conjunto de caracteres ASCII, junto com representacoes decimal, octal, hexadecimal e binaria.
- **Apendice D: Precedencia de Operadores** — O Apendice D lista os operadores C++ em ordem de precedencia decrescente.
- **Apendice E: Outros Operadores** — O Apendice E resume os operadores C++, como os operadores bitwise (bit a bit), nao cobertos no corpo principal do texto.
- **Apendice F: O Template de Classe string** — O Apendice F resume os metodos e funcoes da classe `string`.
- **Apendice G: Metodos e Funcoes da STL** — O Apendice G resume os metodos de containers STL e as funcoes gerais de algoritmo STL.
- **Apendice H: Leituras Selecionadas e Recursos da Internet** — O Apendice H lista alguns livros que podem aprofundar seu entendimento sobre C++.
- **Apendice I: Convertendo para ISO C++ Padrao** — O Apendice I fornece diretrizes para migrar de C e implementacoes mais antigas de C++ para ANSI/ISO C++.
- **Apendice J: Respostas as Perguntas de Revisao** — O Apendice J contem as respostas as perguntas de revisao apresentadas ao final de cada capitulo.

### Nota aos Instrutores

Um dos objetivos desta edicao de *C++ Primer Plus* e fornecer um livro que possa ser usado tanto como um livro de autoaprendizagem quanto como um livro didatico. Aqui estao alguns dos recursos relevantes:

- Este livro descreve C++ generico, portanto nao depende de uma implementacao especifica.
- O conteudo acompanha o trabalho do comite de padroes ISO/ANSI C++ e inclui discussoes sobre templates, STL, a classe `string`, excecoes, RTTI e namespaces.
- Ele nao pressupoe conhecimento previo de C, portanto pode ser usado sem um prerequisito de C. (Algum conhecimento de programacao e desejavel, no entanto.)
- Os topicos estao organizados de forma que os primeiros capitulos possam ser cobertos rapidamente como capitulos de revisao para cursos que tenham um prerequisito de C.
- Os capitulos incluem perguntas de revisao e exercicios de programacao. O Apendice J fornece as respostas as perguntas de revisao.
- O livro introduz varios topicos apropriados para cursos de ciencia da computacao, incluindo tipos abstratos de dados (TADs), pilhas, filas, listas simples, simulacoes, programacao generica e uso de recursao para implementar uma estrategia de divisao e conquista.
- A maioria dos capitulos e curta o suficiente para ser coberta em uma semana ou menos.
- O livro discute *quando* usar certos recursos, bem como *como* usa-los. Por exemplo, ele vincula heranca publica a relacionamentos *is-a* e composicao e heranca privada a relacionamentos *has-a*, e discute quando usar funcoes virtuais e quando nao usa-las.

### Convencoes Usadas neste Livro

Este livro usa varias convencoes tipograficas para distinguir entre diferentes tipos de texto:

- Linhas de codigo, comandos, instrucoes, variaveis, nomes de arquivos e saida de programas aparecem em fonte de computador:

```
#include <iostream>
int main()
{
    using namespace std;
    cout << "What's up, Doc!\n";
    return 0;
}
```

- Entrada de programa que voce deve digitar aparece em fonte de computador em negrito:
  ```
  Please enter your name:
  Plato
  ```

- Marcadores de posicao em descricoes de sintaxe aparecem em fonte de computador italico. Voce deve substituir um marcador pelo nome de arquivo, parametro ou qualquer elemento que ele represente.
- *Texto italico* e usado para novos termos.

> **Caixa de destaque:** Uma caixa de destaque fornece uma discussao mais aprofundada ou informacoes adicionais para ajudar a esclarecer um topico.

> **Dica:** Dicas apresentam guias curtos e uteis para situacoes especificas de programacao.

> **Alerta:** Um alerta chama sua atencao para possiveis armadilhas.

> **Nota:** As notas fornecem uma categoria abrangente para comentarios que nao se encaixam em nenhuma das outras categorias.

### Sistemas Usados para Desenvolver os Exemplos de Programacao deste Livro

Para fins de registro, os exemplos C++11 neste livro foram desenvolvidos usando Microsoft Visual C++ 2010 e Cygwin com GNU g++ 4.5.0, ambos rodando em Windows 7 de 64 bits. Os demais exemplos foram testados com esses sistemas, bem como em um iMac usando g++ 4.2.1 no OS X 10.6.8 e em um sistema Ubuntu Linux usando g++ 4.4.1. A maioria dos exemplos pre-C++11 foi originalmente desenvolvida usando Microsoft Visual C++ 2003 e Metrowerks CodeWarrior Development Studio 9 rodando no Windows XP Professional, e verificada usando o compilador de linha de comando Borland C++ 5.5 e GNU gpp 3.3.3 no mesmo sistema, usando Comeau 4.3.3 e GNU g++ 3.3.1 no SuSE 9.0 Linux, e usando Metrowerks Development Studio 9 em um Macintosh G4 com OS 10.3.

C++ tem muito a oferecer ao programador; aprenda e aproveite!

---

## Capitulo 1 — Comecando com C++ (Getting Started with C++)

Neste capitulo voce vai aprender sobre:

- A historia e a filosofia de C e de C++
- Programacao procedural versus programacao orientada a objetos
- Como o C++ adiciona conceitos orientados a objetos a linguagem C
- Como o C++ adiciona conceitos de programacao generica a linguagem C
- Padroes de linguagem de programacao
- A mecanica de criacao de um programa

Bem-vindo ao C++! Essa linguagem empolgante, que combina a linguagem C com suporte a programacao orientada a objetos e a programacao generica, tornou-se uma das linguagens de programacao mais importantes da decada de 1990 e continua forte nos anos 2000. Sua heranca em C traz ao C++ a tradicao de uma linguagem eficiente, compacta, rapida e portavel. Sua heranca orientada a objetos traz ao C++ uma nova metodologia de programacao, projetada para lidar com a crescente complexidade das tarefas de programacao modernas. Seus recursos de template trazem ainda outra nova metodologia de programacao: a programacao generica. Essa tripla heranca e ao mesmo tempo uma bencao e uma maldica. Ela torna a linguagem muito poderosa, mas tambem significa que ha muito a aprender.

Este capitulo explora um pouco mais o historico do C++ e depois aborda algumas das regras basicas para criar programas C++. O restante do livro ensina a usar a linguagem C++, partindo dos modestos fundamentos da linguagem ate a gloria da programacao orientada a objetos (POO) e seu elenco de suporte de novos jargoes — objetos, classes, encapsulamento, ocultamento de dados, polimorfismo e heranca — e depois em direcao ao suporte de programacao generica. (Claro que, a medida que voce aprender C++, esses termos serao transformados de palavras-chave em buzzwords para o vocabulario necessario do discurso cultivado.)

## Aprendendo C++: O Que Esta Por Vir

O C++ une tres categorias separadas de programacao: a linguagem procedural, representada por C; a linguagem orientada a objetos, representada pelas melhorias de classe que o C++ adiciona ao C; e a programacao generica, suportada pelos templates do C++. Este capitulo examina essas tradicoes. Mas primeiro, vamos considerar o que essa heranca implica sobre o aprendizado de C++. Uma das razoes para usar C++ e aproveitar seus recursos orientados a objetos. Para isso, voce precisa de uma solida formacao em C padrao, pois essa linguagem fornece os tipos basicos, operadores, estruturas de controle e regras de sintaxe. Portanto, se voce ja conhece C, esta pronto para aprender C++. Mas nao e apenas uma questao de aprender mais algumas palavras-chave e construcoes. Passar de C para C++ envolve talvez mais trabalho do que aprender C em primeiro lugar. Alem disso, se voce conhece C, precisa desaprender alguns habitos de programacao ao fazer a transicao para C++. Se voce nao conhece C, precisa dominar os componentes de C, os componentes de POO e os componentes genericos para aprender C++, mas pelo menos pode nao ter que desaprender habitos de programacao. Se voce esta comecando a achar que aprender C++ pode envolver algum esforco mental, voce esta certo. Este livro o guiara pelo processo de maneira clara e prestativa, um passo de cada vez, de modo que o esforco mental sera suficientemente suave para deixar seu cerebro resiliente.

*C++ Primer Plus* aborda o C++ ensinando tanto sua base em C quanto seus novos componentes, portanto assume que voce nao tem conhecimento previo de C. Voce comecara aprendendo os recursos que o C++ compartilha com C. Mesmo que voce conheca C, pode achar essa parte do livro uma boa revisao. Ela tambem aponta conceitos que se tornarao importantes mais adiante, e indica onde o C++ difere do C. Depois de ter uma boa base nos fundamentos de C, voce aprendera sobre a superestrutura do C++. Nesse ponto, voce aprendera sobre objetos e classes e como o C++ os implementa. E voce aprendera sobre templates.

Este livro nao pretende ser uma referencia C++ completa; ele nao explora cada canto e recanto da linguagem. Mas voce aprendera a maioria dos principais recursos da linguagem, incluindo templates, excecoes e namespaces.

Agora vamos dar uma breve olhada em alguns dos antecedentes do C++.

## As Origens do C++: Um Pouco de Historia

A tecnologia computacional evoluiu a um ritmo incrivel nas ultimas decadas. Hoje, um notebook pode computar mais rapidamente e armazenar mais informacoes do que os computadores mainframe dos anos 1960. (Muitos programadores conseguem se lembrar de levar oferendas de pilhas de cartoes perfurados para serem submetidos a um poderoso sistema de computador que enchia uma sala inteira com majesticos 100 KB de memoria — muito menos do que ate mesmo um smartphone usa hoje.) As linguagens de computador tambem evoluiram. As mudancas podem nao ser tao dramaticas, mas sao importantes. Computadores maiores e mais poderosos geram programas maiores e mais complexos, que, por sua vez, levantam novos problemas no gerenciamento e manutencao de programas.

Na decada de 1970, linguagens como C e Pascal ajudaram a inaugurar uma era de programacao estruturada, uma filosofia que trouxe alguma ordem e disciplina a um campo que precisava muito dessas qualidades. Alem de fornecer as ferramentas para programacao estruturada, C tambem produzia programas compactos e de execucao rapida, juntamente com a capacidade de tratar questoes de hardware, como gerenciar portas de comunicacao e unidades de disco. Esses dons ajudaram a tornar C a linguagem de programacao dominante nos anos 1980. Enquanto isso, os anos 1980 testemunharam o crescimento de um novo paradigma de programacao: a programacao orientada a objetos, ou POO (OOP), como incorporada em linguagens como SmallTalk e C++. Vamos examinar um pouco mais de perto esses C e POO.

### A Linguagem C

No inicio dos anos 1970, Dennis Ritchie, dos Laboratorios Bell, estava trabalhando em um projeto para desenvolver o sistema operacional Unix. (Um *sistema operacional* e um conjunto de programas que gerenciam os recursos de um computador e lidam com suas interacoes com os usuarios. Por exemplo, e o sistema operacional que exibe o prompt do sistema na tela para uma interface estilo terminal que gerencia janelas e mouses para interfaces graficas e que executa programas para voce.) Para esse trabalho, Ritchie precisava de uma linguagem que fosse concisa, que produzisse programas compactos e rapidos, e que pudesse controlar o hardware com eficiencia.

Tradicionalmente, os programadores atendiam a essas necessidades usando linguagem de montagem (assembly language), que esta estreitamente vinculada a linguagem de maquina interna de um computador. No entanto, a linguagem assembly e uma linguagem de baixo nivel (low-level language) — ou seja, ela trabalha diretamente com o hardware (por exemplo, acessando registradores da CPU e locais de memoria diretamente). Assim, a linguagem assembly e especifica para um determinado processador de computador. Portanto, se voce quiser mover um programa assembly para um tipo diferente de computador, pode ser necessario reescrever completamente o programa usando uma linguagem assembly diferente. Era um pouco como se, cada vez que voce comprasse um carro novo, descobrisse que os projetistas decidiram mudar onde ficam os controles e o que fazem, forcando-o a reaprender como dirigir.

Mas o Unix era destinado a funcionar em uma variedade de tipos de computadores (ou plataformas). Isso sugeria usar uma linguagem de alto nivel. Uma *linguagem de alto nivel* e orientada para resolucao de problemas em vez de para hardware especifico. Programas especiais chamados *compiladores* traduzem uma linguagem de alto nivel para a linguagem interna de um computador especifico. Assim, voce pode usar o mesmo programa de linguagem de alto nivel em diferentes plataformas usando um compilador separado para cada plataforma. Ritchie queria uma linguagem que combinasse eficiencia de baixo nivel e acesso ao hardware com generalidade e portabilidade de alto nivel. Entao, construindo a partir de linguagens mais antigas, ele criou C.

### A Filosofia de Programacao em C

Como o C++ enxerta uma nova filosofia de programacao em C, primeiro devemos dar uma olhada na filosofia mais antiga que C segue. Em geral, as linguagens de computador lidam com dois conceitos — dados e algoritmos. Os dados constituem as informacoes que um programa usa e processa. Os algoritmos sao os metodos que o programa usa (veja a Figura 1.1). Como a maioria das linguagens mais comuns quando C foi criado, C e uma linguagem procedural. Isso significa que ela enfatiza o lado dos algoritmos na programacao. Conceitualmente, a programacao procedural consiste em descobrir as acoes que um computador deve executar e, em seguida, usar a linguagem de programacao para implementar essas acoes. Um programa prescreve um conjunto de procedimentos para o computador seguir a fim de produzir um resultado especifico, assim como uma receita prescreve um conjunto de procedimentos para um cozinheiro seguir a fim de produzir um bolo.

```
     DADOS            ALGORITMOS
  
  1/2 xic. manteiga   bata a manteiga
  1 xic. acucar       adicione acucar gradualmente
  2 ovos              quebre os ovos
  …                   …
  
            PROGRAMA
```
*Figura 1.1 Dados + algoritmos = programa.*

Linguagens procedurais anteriores, como FORTRAN e BASIC, encontraram problemas organizacionais a medida que os programas cresciam. Por exemplo, os programas frequentemente usam instrucoes de desvio (branching statements), que direcionam a execucao para um ou outro conjunto de instrucoes, dependendo do resultado de algum tipo de teste. Muitos programas mais antigos tinham um roteamento tao emaranhado (chamado de "programacao espaguete") que era praticamente impossivel entender um programa lendo-o, e modificar tal programa era um convite ao desastre. Em resposta, os cientistas da computacao desenvolveram um estilo mais disciplinado de programacao chamado programacao estruturada. C inclui recursos para facilitar essa abordagem. Por exemplo, a programacao estruturada limita o desvio (escolher qual instrucao executar a seguir) a um pequeno conjunto de construcoes bem comportadas. O C incorpora essas construcoes (o loop `for`, o loop `while`, o loop `do while` e a instrucao `if else`) em seu vocabulario.

O *design top-down* (de cima para baixo) foi outro dos novos principios. Com C, a ideia e dividir um programa grande em tarefas menores e mais gerenciaveis. Se uma dessas tarefas ainda for muito ampla, voce a divide em tarefas ainda menores. Voce continua com esse processo ate que o programa seja compartimentado em modulos pequenos e facilmente programados. (Organize seu estudo. Argh! Bom, organize sua mesa, a superficie dela, seu arquivo e suas estantes. Argh! Bom, comece pela mesa e organize cada gaveta, comecando pela do meio. Hmm, talvez eu consiga lidar com essa tarefa.) O design do C facilita essa abordagem, encorajando voce a desenvolver unidades de programa chamadas funcoes para representar modulos de tarefas individuais. Como voce deve ter notado, as tecnicas de programacao estruturada refletem uma mentalidade procedural, pensando em um programa em termos das acoes que ele executa.

### O Salto do C++: Programacao Orientada a Objetos

Embora os principios da programacao estruturada tenham melhorado a clareza, a confiabilidade e a facilidade de manutencao dos programas, a programacao em larga escala ainda continua sendo um desafio. A POO traz uma nova abordagem a esse desafio. Ao contrario da programacao procedural, que enfatiza os algoritmos, a POO enfatiza os dados. Em vez de tentar adaptar um problema a abordagem procedural de uma linguagem, a POO tenta adaptar a linguagem ao problema. A ideia e projetar formas de dados que correspondam aos recursos essenciais de um problema.

Em C++, uma *classe* e uma especificacao que descreve tal nova forma de dados, e um *objeto* e uma estrutura de dados especifica construida de acordo com esse plano. Por exemplo, uma classe poderia descrever as propriedades gerais de um executivo corporativo (nome, cargo, salario, habilidades incomuns, por exemplo), enquanto um objeto representaria um executivo especifico. Em geral, uma classe define quais dados sao usados para representar um objeto e as operacoes que podem ser executadas nesses dados. Por exemplo, suponha que voce estivesse desenvolvendo um programa de desenho em computador capaz de desenhar um retangulo. Voce poderia definir uma classe para descrever um retangulo. A parte de dados da especificacao poderia incluir coisas como a localizacao dos cantos, a altura e largura, a cor e o estilo da linha de contorno e a cor e o padrao usados para preencher o retangulo. A parte de operacoes da especificacao poderia incluir metodos para mover o retangulo, redimensiona-lo, gira-lo, alterar cores e padroes e copiar o retangulo para outro local. Se voce entao usasse seu programa para desenhar um retangulo, ele criaria um objeto de acordo com a especificacao da classe. Esse objeto conteria todos os valores de dados que descrevem o retangulo, e voce poderia usar os metodos da classe para modificar aquele retangulo. Se voce desenhasse dois retangulos, o programa criaria dois objetos, um para cada retangulo.

A abordagem POO para o design de programas e primeiro projetar classes que representem com precisao as coisas com que o programa lida. Por exemplo, um programa de desenho pode definir classes para representar retangulos, linhas, circulos, pinceis, canetas e similares. As definicoes de classe, lembre-se, incluem uma descricao das operacoes permitidas para cada classe, como mover um circulo ou girar uma linha. Voce entao procederia a projetar um programa usando objetos dessas classes. O processo de ir de um nivel mais baixo de organizacao, como classes, para um nivel mais alto, como o design do programa, e chamado de *programacao bottom-up* (de baixo para cima).

Ha mais na POO do que a vinculacao de dados e metodos em uma definicao de classe. Por exemplo, a POO facilita a criacao de codigo reutilizavel, o que pode eventualmente economizar muito trabalho. *Ocultamento de informacoes* (information hiding) protege os dados de acesso inadequado. *Polimorfismo* (polymorphism) permite criar multiplas definicoes para operadores e funcoes, com o contexto de programacao determinando qual definicao e usada. *Heranca* (inheritance) permite derivar novas classes a partir das antigas. Como voce pode ver, a POO introduz muitas ideias novas e envolve uma abordagem diferente de programacao do que a programacao procedural. Em vez de se concentrar em tarefas, voce se concentra em representar conceitos. Em vez de adotar uma abordagem de programacao top-down, as vezes voce adota uma abordagem bottom-up. Este livro o guiara por todos esses pontos, com muitos exemplos facilmente compreensíveis.

Projetar uma classe util e confiavel pode ser uma tarefa dificil. Felizmente, as linguagens POO tornam simples incorporar classes existentes em sua propria programacao. Os fornecedores fornecem uma variedade de bibliotecas de classes uteis, incluindo bibliotecas de classes projetadas para simplificar a criacao de programas para ambientes como Windows ou Macintosh. Um dos beneficios reais do C++ e que ele permite reutilizar e adaptar facilmente codigo existente e bem testado.

### C++ e Programacao Generica

A programacao generica (generic programming) e ainda outro paradigma de programacao suportado pelo C++. Ela compartilha com a POO o objetivo de tornar mais simples a reutilizacao de codigo e a tecnica de abstrair conceitos gerais. Mas enquanto a POO enfatiza o aspecto dos dados na programacao, a programacao generica enfatiza a independencia de um tipo de dado especifico. E seu foco e diferente. A POO e uma ferramenta para gerenciar grandes projetos, enquanto a programacao generica fornece ferramentas para realizar tarefas comuns, como classificar dados ou mesclar listas. O termo *generico* se refere a codigo independente de tipo. As representacoes de dados em C++ vem em muitos tipos — inteiros, numeros com partes fracionarias, caracteres, strings de caracteres e estruturas compostas definidas pelo usuario de varios tipos. Se, por exemplo, voce quisesse classificar dados desses varios tipos, normalmente teria que criar uma funcao de classificacao separada para cada tipo. A programacao generica envolve estender a linguagem para que voce possa escrever uma funcao para um tipo generico (ou seja, nao especificado) uma vez e usa-la para uma variedade de tipos reais. Os templates do C++ fornecem um mecanismo para fazer isso.

### A Genesis do C++

Assim como C, o C++ comecou sua vida nos Laboratorios Bell, onde Bjarne Stroustrup desenvolveu a linguagem no inicio dos anos 1980. Nas proprias palavras de Stroustrup: "C++ foi projetado principalmente para que meus amigos e eu nao precisassemos programar em assembly, C ou varias linguagens de alto nivel modernas. Seu principal objetivo era tornar a escrita de bons programas mais facil e prazerosa para o programador individual" (Bjarne Stroustrup, *The C++ Programming Language*, Terceira Edicao. Reading, MA: Addison-Wesley, 1997).

> **A Pagina Pessoal de Bjarne Stroustrup**
> Bjarne Stroustrup projetou e implementou a linguagem de programacao C++ e e o autor dos manuais de referencia definitivos *The C++ Programming Language* e *The Design and Evolution of C++*. Seu site pessoal no AT&T Labs Research deve ser o primeiro favorito de C++ que voce cria: www.research.att.com/~bs. Este site inclui uma interessante perspectiva historica dos porques e comos da linguagem C++, material biografico de Stroustrup e FAQs sobre C++. Surpreendentemente, a pergunta mais frequente de Stroustrup pode ser como pronunciar Bjarne Stroustrup. Consulte o FAQ no site de Stroustrup e baixe o arquivo .WAV para ouvir voce mesmo!

Stroustrup estava mais preocupado em tornar o C++ util do que em impor filosofias ou estilos de programacao especificos. As necessidades reais de programacao sao mais importantes do que a pureza teorica na determinacao dos recursos da linguagem C++. Stroustrup baseou o C++ em C por causa da brevidade do C, sua adequacao para programacao de sistemas, sua ampla disponibilidade e seus lacos estreitos com o sistema operacional Unix. O aspecto POO do C++ foi inspirado em uma linguagem de simulacao por computador chamada Simula67. Stroustrup adicionou recursos POO e suporte a programacao generica ao C sem alterar significativamente o componente C. Assim, o C++ e um superconjunto de C, significando que qualquer programa C valido tambem e um programa C++ valido. Ha algumas discrepancias menores, mas nada crucial. Programas C++ podem usar bibliotecas de software C existentes. *Bibliotecas* sao colecoes de modulos de programacao que voce pode chamar de um programa. Elas fornecem solucoes comprovadas para muitos problemas comuns de programacao, economizando muito tempo e esforco. Isso ajudou a disseminacao do C++.

O nome C++ vem do operador de incremento de C, `++`, que adiciona um ao valor de uma variavel. Portanto, o nome C++ sugere corretamente uma versao aumentada de C.

Um programa de computador traduz um problema da vida real em uma serie de acoes a serem executadas por um computador. O aspecto POO do C++ da a linguagem a capacidade de se relacionar com os conceitos envolvidos no problema, e a parte C do C++ da a linguagem a capacidade de se aproximar do hardware. Essa combinacao de habilidades permitiu a disseminacao do C++. Tambem pode envolver uma mudanca mental de marchas a medida que voce passa de um aspecto de um programa para outro. (De fato, alguns puristas de POO consideram adicionar recursos POO ao C como sendo semelhante a adicionar asas a um porco, embora um porco magro e eficiente.) Alem disso, como o C++ enxerta a POO no C, voce pode ignorar os recursos orientados a objetos do C++. Mas voce perdera muito se e isso tudo que fizer.

Somente depois que o C++ alcancast algum sucesso Stroustrup adicionou templates, possibilitando a programacao generica. E somente depois que o recurso de template foi usado e aprimorado ficou aparente que os templates eram talvez uma adicao tao significativa quanto a POO — ou ainda mais significativa, argumentam alguns. O fato de o C++ incorporar tanto POO quanto programacao generica, bem como a abordagem procedural mais tradicional, demonstra que o C++ enfatiza o utilitarismo sobre a abordagem ideologica, e essa e uma das razoes para o sucesso da linguagem.

## Portabilidade e Padroes

Digamos que voce escreveu um programa C++ interessante para o antigo PC Pentium com Windows 2000 no trabalho, mas o gerenciamento decide substituir a maquina por um novo computador usando um sistema operacional diferente, digamos Mac OS X ou Linux, talvez com um design de processador diferente, como um processador SPARC. Voce pode rodar seu programa na nova plataforma? Claro que voce tera que recompilar o programa usando um compilador C++ projetado para a nova plataforma. Mas voce tera que fazer alguma alteracao no codigo que escreveu? Se voce puder recompilar o programa sem fazer alteracoes e ele rodar sem problemas, dizemos que o programa e *portavel*.

Existem alguns obstaculos a portabilidade, sendo o primeiro o hardware. Um programa que e especifico de hardware provavelmente nao sera portavel. Um que assume o controle direto de um placa de video IBM PC, por exemplo, nao faz sentido para, digamos, um computador Sun. (Voce pode minimizar problemas de portabilidade localizando as partes dependentes de hardware em modulos de funcoes; entao voce so precisa reescrever esses modulos especificos.) Evitamos esse tipo de programacao neste livro.

O segundo obstaculo a portabilidade e a divergencia de linguagem. Certamente, isso pode ser um problema com linguas faladas. A descricao de um Yorkshire sobre os eventos do dia pode nao ser portavel para o Brooklyn, mesmo que o ingles seja falado em ambas as areas. As linguagens de computador tambem podem desenvolver dialetos. Embora a maioria dos implementadores gostaria de tornar suas versoes de C++ compativeis com outras, e dificil faze-lo sem um padrao publicado descrevendo exatamente como a linguagem funciona. Portanto, o American National Standards Institute (ANSI) criou um comite em 1990 (ANSI X3J16) para desenvolver um padrao para C++. (O ANSI ja havia desenvolvido um padrao para C.) A International Organization for Standardization (ISO) logo se juntou ao processo com seu proprio comite (ISO-WG-21), criando um esforco conjunto ANSI/ISO para desenvolver o padrao para C++.

Varios anos de trabalho eventualmente levaram ao Padrao Internacional (ISO/IEC 14882:1998), que foi adotado em 1998 pela ISO, pela International Electrotechnical Commission (IEC) e pelo ANSI. Esse padrao, frequentemente chamado de C++98, nao so refinou a descricao dos recursos existentes do C++ como tambem estendeu a linguagem com excecoes, identificacao de tipo em tempo de execucao (RTTI), templates e a STL (Standard Template Library). O ano de 2003 trouxe a publicacao da segunda edicao do padrao C++ (ISO/IEC 14882:2003); a nova edicao e uma revisao tecnica, o que significa que ela organiza a primeira edicao — corrigindo erros tipograficos, reduzindo ambiguidades e similares — mas nao altera os recursos da linguagem. Essa edicao geralmente e chamada de C++03. Como o C++03 nao alterou os recursos da linguagem, seguiremos um uso comum e usaremos C++98 para nos referir ao C++98/C++03.

O C++ continua a evoluir, e o comite ISO aprovou um novo padrao em agosto de 2011 intitulado ISO/IEC 14882:2011 e informalmente chamado de C++11. Assim como o C++98, o C++11 adiciona muitos recursos a linguagem. Alem disso, tem como objetivos remover inconsistencias e tornar o C++ mais facil de aprender e usar. Esse padrao havia sido chamado de C++0x, com a expectativa original de que x seria 7 ou 8, mas o trabalho de padronizacao e um processo lento, exaustivo e exasperante. Felizmente, logo se percebeu que 0x poderia ser um inteiro hexadecimal (veja o Apendice A, "Bases Numericas"), o que significava que o comite tinha ate 2015 para concluir o trabalho. Portanto, por essa medida, eles terminaram antes do prazo.

O Padrao ISO C++ adicionalmente se baseia no ANSI C Standard porque o C++ deveria ser, na medida do possivel, um superconjunto de C. Isso significa que qualquer programa C valido deveria tambem ser um programa C++ valido. Ha algumas diferencas entre o ANSI C e as regras correspondentes para C++, mas sao menores. De fato, o ANSI C incorpora alguns recursos introduzidos primeiro em C++, como prototipagem de funcao e o qualificador de tipo `const`.

Antes do surgimento do ANSI C, a comunidade C seguia um padrao *de facto* baseado no livro *The C Programming Language*, de Kernighan e Ritchie (Addison-Wesley Publishing Company, Reading, MA, 1978). Esse padrao era frequentemente chamado de K&R C; com o surgimento do ANSI C, o C mais simples K&R e as vezes chamado de C classico.

O ANSI C Standard nao so define a linguagem C como tambem define uma biblioteca C padrao que as implementacoes ANSI C devem suportar. O C++ tambem usa essa biblioteca; este livro a chama de *biblioteca C padrao* ou *biblioteca padrao*. Alem disso, o padrao ISO C++ fornece uma biblioteca padrao de classes C++.

O Padrao C foi revisado pela ultima vez como C99, que foi adotado pela ISO em 1999 e pelo ANSI em 2000. Esse padrao adiciona alguns recursos ao C, como um novo tipo inteiro, que alguns compiladores C++ suportam.

### Crescimento da Linguagem

Originalmente, o padrao *de facto* para C++ era um manual de referencia de 65 paginas incluido no *The C++ Programming Language*, de 328 paginas, de Stroustrup (Addison-Wesley, 1986).

O proximo padrao *de facto* publicado foi *The Annotated C++ Reference Manual*, de Ellis e Stroustrup (Addison-Wesley, 1990). Este e um trabalho de 453 paginas; inclui comentarios substanciais alem do material de referencia.

O padrao C++98, com a adicao de muitos recursos, chegou a quase 800 paginas, mesmo com apenas comentarios minimos.

O padrao C++11 tem mais de 1.350 paginas, portanto aumenta substancialmente o padrao antigo.

### Este Livro e os Padroes C++

Os compiladores contemporaneos fornecem bom suporte para C++98. Alguns compiladores no momento desta escrita tambem suportam alguns recursos C++11, e podemos esperar que o nivel de suporte aumente rapidamente agora que o novo padrao foi adotado. Este livro reflete a situacao atual, cobrindo o C++98 de forma bastante completa e introduzindo varios recursos do C++11. Alguns desses recursos estao integrados com a cobertura de topicos relacionados do C++98. O Capitulo 18, "Visitando o Novo Padrao C++", concentra-se nos novos recursos, resumindo os mencionados anteriormente no livro e apresentando recursos adicionais.

Com o suporte incompleto disponivel no momento desta escrita, seria muito dificil cobrir adequadamente todos os novos recursos do C++11. Mas mesmo quando o novo padrao for completamente suportado, fica claro que uma cobertura abrangente estaria alem do escopo de qualquer livro de volume unico de tamanho razoavel. Este livro adota a abordagem de se concentrar nos recursos que ja estao disponiveis em alguns compiladores e resumir brevemente muitos dos outros recursos.

Antes de chegar a linguagem C++ propriamente dita, vamos cobrir alguns dos trabalhos basicos relacionados a criacao de programas.

## A Mecanica de Criacao de um Programa

Suponha que voce escreveu um programa C++. Como voce o faz rodar? As etapas exatas dependem do seu ambiente de computador e do compilador C++ especifico que voce usa, mas devem ser semelhantes as seguintes etapas (veja a Figura 1.3):

1. Use algum tipo de editor de texto para escrever o programa e salva-lo em um arquivo. Esse arquivo constitui o *codigo-fonte* (source code) do seu programa.

2. Compile o codigo-fonte. Isso significa executar um programa que traduz o codigo-fonte para a linguagem interna, chamada linguagem de maquina (machine language), usada pelo computador host. O arquivo que contem o programa traduzido e o *codigo-objeto* (object code) do seu programa.

3. Vincule (link) o codigo-objeto com codigo adicional. Por exemplo, programas C++ normalmente usam bibliotecas. Uma biblioteca C++ contem codigo-objeto para uma colecao de rotinas de computador, chamadas funcoes, para realizar tarefas como exibir informacoes na tela ou calcular a raiz quadrada de um numero. A vinculacao combina seu codigo-objeto com o codigo-objeto das funcoes que voce usa e com algum codigo de inicializacao padrao para produzir uma versao de tempo de execucao do seu programa. O arquivo que contem esse produto final e chamado de *codigo executavel* (executable code).

Voce encontrara o termo *codigo-fonte* ao longo deste livro, portanto certifique-se de registra-lo em sua memoria de acesso aleatorio pessoal.

```
       codigo-fonte
       
       COMPILADOR
       
       codigo-objeto
startup code ──┐
               LINKER
library code ──┘
       
       codigo executavel
```
*Figura 1.3 Etapas de programacao.*

A maioria dos programas neste livro e generica e deve rodar em qualquer sistema que suporte C++98. No entanto, alguns, especialmente os do Capitulo 18, requerem algum suporte a C++11. No momento desta escrita, alguns compiladores requerem flags adicionais para ativar seu suporte parcial ao C++11. Por exemplo, g++, a partir da versao 4.3, atualmente usa a flag `–std=c++0x` ao compilar um arquivo de codigo-fonte:

```
g++ -std=c++0x use_auto.cpp
```

As etapas para montar um programa podem variar. Vamos olhar um pouco mais para essas etapas.

### Criando o Arquivo de Codigo-Fonte

O restante do livro trata do que vai em um arquivo de codigo-fonte; esta secao discute a mecanica de criar um. Algumas implementacoes C++, como Microsoft Visual C++, Embarcadero C++ Builder, Apple Xcode, Open Watcom C++, Digital Mars C++ e Freescale CodeWarrior, fornecem ambientes de desenvolvimento integrados (IDEs) que permitem gerenciar todas as etapas do desenvolvimento de programas, incluindo edicao, a partir de um programa mestre. Outras implementacoes, como GNU C++ no Unix e Linux, IBM XL C/C++ no AIX e as versoes gratuitas dos compiladores Borland 5.5 (distribuido pela Embarcadero) e Digital Mars, apenas lidam com as etapas de compilacao e vinculacao e esperam que voce digite comandos na linha de comando do sistema. Nesses casos, voce pode usar qualquer editor de texto disponivel para criar e modificar o codigo-fonte. Em um sistema Unix, por exemplo, voce pode usar `vi`, `ed`, `ex` ou `emacs`. Em um sistema Windows rodando no modo Prompt de Comando, voce pode usar `edlin` ou outro editor disponivel.

Ao nomear um arquivo de codigo-fonte, voce deve usar o sufixo correto para identifica-lo como um arquivo C++. Isso nao so informa que o arquivo e codigo-fonte C++, como tambem informa isso ao compilador. (Se um compilador Unix reclamar de um "bad magic number", e apenas sua forma curiosamente obscura de dizer que voce usou o sufixo errado.) O sufixo consiste em um ponto seguido de um caractere ou grupo de caracteres chamado de extensao (veja a Figura 1.4).

```
spiffy.cpp
   |    |
nome   extensao
base
```
*Figura 1.4 As partes de um nome de arquivo de codigo-fonte.*

A extensao que voce usa depende da implementacao C++. A Tabela 1.1 mostra algumas opcoes comuns. Por exemplo, `spiffy.C` e um nome de arquivo de codigo-fonte C++ valido para Unix. Observe que o Unix distingue maiusculas de minusculas, o que significa que voce deve usar o caractere C maiusculo. Na verdade, a extensao c minuscula tambem funciona, mas o C padrao usa essa extensao. Portanto, para evitar confusao em sistemas Unix, voce deve usar `c` com programas C e `C` com programas C++. Se nao se importar de digitar um ou dois caracteres a mais, voce tambem pode usar as extensoes `cc` e `cxx` em alguns sistemas Unix. O DOS, sendo um pouco mais simples comparado ao Unix, nao distingue entre maiusculas e minusculas, portanto as implementacoes DOS usam letras adicionais, conforme mostrado na Tabela 1.1, para distinguir entre programas C e C++.

**Tabela 1.1 — Extensoes de Codigo-Fonte**

| Implementacao C++             | Extensoes de codigo-fonte |
|-------------------------------|--------------------------|
| Unix                          | C, cc, cxx, c            |
| GNU C++                       | C, cc, cxx, cpp, c++     |
| Digital Mars                  | cpp, cxx                 |
| Borland C++                   | cpp                      |
| Watcom                        | cpp                      |
| Microsoft Visual C++          | cpp, cxx, cc             |
| Freestyle CodeWarrior         | cpp, cp, cc, cxx, c++    |

### Compilacao e Vinculacao

Originalmente, Stroustrup implementou o C++ com um programa compilador de C++ para C em vez de desenvolver um compilador direto de C++ para codigo-objeto. Esse programa, chamado `cfront` (de *C front end*), traduzia codigo-fonte C++ para codigo-fonte C, que podia entao ser compilado por um compilador C padrao. Essa abordagem simplificou a introducao do C++ na comunidade C. Outras implementacoes usaram essa abordagem para trazer o C++ a outras plataformas. A medida que o C++ se desenvolveu e ganhou popularidade, cada vez mais implementadores passaram a criar compiladores C++ que geram codigo-objeto diretamente do codigo-fonte C++. Essa abordagem direta acelera o processo de compilacao e enfatiza que o C++ e uma linguagem separada, embora similar.

A mecanica da compilacao depende da implementacao, e as secoes a seguir descrevem algumas formas comuns. Essas secoes descrevem os passos basicos, mas nao substituem a consulta a documentacao do seu sistema.

#### Compilacao e Vinculacao no Unix

Originalmente, o comando `CC` do Unix invocava o `cfront`. No entanto, o `cfront` nao acompanhou a evolucao do C++, e seu ultimo lancamento foi em 1993. Nos dias atuais, um computador Unix pode nao ter compilador, ter um compilador proprietario ou um compilador de terceiros, talvez comercial, talvez freeware, como o compilador GNU `g++`. Em muitos desses outros casos (mas nao no caso de nao haver compilador!), o comando `CC` ainda funciona, sendo o compilador real invocado diferente de sistema para sistema. Para simplicidade, vamos assumir que `CC` esta disponivel, mas tenha em mente que pode ser necessario substituir um comando diferente por `CC` na discussao a seguir.

Voce usa o comando `CC` para compilar seu programa. O nome esta em letras maiusculas para distingui-lo do compilador C padrao do Unix, `cc`. O compilador `CC` e um compilador de linha de comando, o que significa que voce digita os comandos de compilacao na linha de comando do Unix.

Por exemplo, para compilar o arquivo de codigo-fonte C++ `spiffy.C`, voce digitaria este comando no prompt do Unix:

```
CC spiffy.C
```

Se, por habilidade, dedicacao ou sorte, seu programa nao tiver erros, o compilador gera um arquivo de codigo-objeto com uma extensao `o`. Nesse caso, o compilador produz um arquivo chamado `spiffy.o`.

Em seguida, o compilador passa automaticamente o arquivo de codigo-objeto para o vinculador do sistema, um programa que combina seu codigo com o codigo de biblioteca para produzir o arquivo executavel. Por padrao, o arquivo executavel e chamado de `a.out`. Se voce usou apenas um arquivo de codigo-fonte, o vinculador tambem exclui o arquivo `spiffy.o` porque ele nao e mais necessario. Para rodar o programa, basta digitar o nome do arquivo executavel:

```
a.out
```

Observe que se voce compilar um novo programa, o novo arquivo executavel `a.out` substituira o anterior. (Isso ocorre porque os arquivos executaveis ocupam muito espaco, portanto sobrescrever arquivos executaveis antigos ajuda a reduzir a demanda de armazenamento.) Mas se voce desenvolver um programa executavel que deseja manter, basta usar o comando `mv` do Unix para alterar o nome do arquivo executavel.

Em C++, como em C, voce pode distribuir um programa por mais de um arquivo. (Muitos dos programas deste livro nos Capitulos 8 a 16 fazem isso.) Nesse caso, voce pode compilar um programa listando todos os arquivos na linha de comando, assim:

```
CC my.C precious.C
```

Se houver varios arquivos de codigo-fonte, o compilador nao exclui os arquivos de codigo-objeto. Dessa forma, se voce alterar apenas o arquivo `my.C`, podera recompilar o programa com este comando:

```
CC my.C precious.o
```

Isso recompila o arquivo `my.C` e o vincula ao arquivo `precious.o` compilado anteriormente.

Pode ser necessario identificar algumas bibliotecas explicitamente. Por exemplo, para acessar funcoes definidas na biblioteca matematica, pode ser necessario adicionar a flag `-lm` na linha de comando:

```
CC usingmath.C -lm
```

#### Compilacao e Vinculacao no Linux

Os sistemas Linux usam mais comumente o `g++`, o compilador GNU C++ da Free Software Foundation. O compilador e incluido na maioria das distribuicoes Linux, mas pode nem sempre estar instalado. O compilador `g++` funciona de forma muito semelhante ao compilador Unix padrao. Por exemplo, o seguinte produz um arquivo executavel chamado `a.out`:

```
g++ spiffy.cxx
```

Algumas versoes podem exigir que voce vincule a biblioteca C++:

```
g++ spiffy.cxx -lg++
```

Para compilar varios arquivos de codigo-fonte, basta lista-los todos na linha de comando:

```
g++ my.cxx precious.cxx
```

Isso produz um arquivo executavel chamado `a.out` e dois arquivos de codigo-objeto, `my.o` e `precious.o`. Se voce modificar posteriormente apenas um dos arquivos de codigo-fonte, digamos `my.cxx`, podera recompilar usando `my.cxx` e `precious.o`:

```
g++ my.cxx precious.o
```

O compilador GNU esta disponivel para muitas plataformas, incluindo o modo de linha de comando para PCs baseados em Windows, bem como para sistemas Unix em uma variedade de plataformas.

#### Compiladores de Linha de Comando para Windows

Uma opcao economica para compilar programas C++ em um PC com Windows e baixar um compilador de linha de comando gratuito que roda no modo Prompt de Comando do Windows, que abre uma janela semelhante ao MS-DOS. Downloads gratuitos do Windows que incluem o compilador GNU C++ sao Cygwin e MinGW; ambos usam `g++` como nome do compilador.

Para usar o compilador `g++`, voce deve primeiro abrir uma janela de prompt de comando. O Cygwin e o MinGW fazem isso automaticamente quando voce inicia esses programas. Para compilar um arquivo de codigo-fonte chamado `great.cpp`, voce digita o seguinte comando no prompt:

```
g++ great.cpp
```

Se o programa compilar com sucesso, o arquivo executavel resultante e chamado `a.exe`.

#### Compiladores para Windows

Os produtos Windows sao numerosos demais e frequentemente revisados para ser razoavel descrevê-los todos individualmente. Atualmente o mais popular e o Microsoft Visual C++ 2010, disponivel na edicao gratuita Microsoft Visual C++ 2010 Express. Apesar de designs e objetivos diferentes, a maioria dos compiladores C++ baseados em Windows compartilha alguns recursos comuns.

Normalmente, voce deve criar um projeto para um programa e adicionar ao projeto o arquivo ou os arquivos que constituem o programa. Cada fornecedor fornece um IDE com opcoes de menu e, possivelmente, assistencia automatizada para criar um projeto. Uma questao muito importante que voce precisa estabelecer e o tipo de programa que esta criando. Normalmente, o compilador oferece muitas opcoes, como um aplicativo Windows, um aplicativo MFC Windows, uma biblioteca de vinculacao dinamica (DLL), um controle ActiveX, um executavel DOS ou modo caractere, uma biblioteca estatica ou um aplicativo console.

Como os programas deste livro sao genericos, voce deve evitar opcoes que exijam codigo especifico da plataforma, como aplicativos Windows. Em vez disso, voce quer rodar em modo baseado em caracteres. A opcao depende do compilador. Em geral, voce deve procurar uma opcao rotulada como Console, modo caractere ou executavel DOS e tentar essa opcao. Por exemplo, no Microsoft Visual C++ 2010, selecione a opcao *Win32 Console Application*, clique em *Application Settings* e selecione a opcao *Empty Project*. No C++ Builder XE, selecione *Console Application* em *C++ Builder Projects*.

Depois de configurar o projeto, voce precisa compilar e vincular seu programa. O IDE normalmente oferece varias opcoes, como *Compile*, *Build*, *Make*, *Build All*, *Link*, *Execute*, *Run* e *Debug*:

- **Compile** normalmente significa compilar o codigo no arquivo que esta aberto no momento.
- **Build** ou **Make** normalmente significa compilar o codigo de todos os arquivos de codigo-fonte no projeto. Este e frequentemente um processo incremental: se o projeto tiver tres arquivos e voce alterar apenas um, apenas aquele e recompilado.
- **Build All** normalmente significa compilar todos os arquivos de codigo-fonte do zero.
- **Link** significa combinar o codigo-fonte compilado com o codigo de biblioteca necessario.
- **Run** ou **Execute** significa rodar o programa. Normalmente, se voce ainda nao fez as etapas anteriores, o *Run* as realiza antes de tentar rodar o programa.
- **Debug** significa rodar o programa com a opcao de percorre-lo passo a passo.
- Um compilador pode oferecer as versoes *Debug* e *Release*. A primeira contem codigo extra que aumenta o tamanho do programa, torna a execucao mais lenta, mas habilita recursos detalhados de depuracao.

Um compilador gera uma mensagem de erro quando voce viola uma regra de linguagem e identifica a linha que tem o problema. Infelizmente, quando voce e iniciante em uma linguagem, pode ser dificil entender a mensagem. As vezes, o erro real pode ocorrer antes da linha identificada, e as vezes um unico erro gera uma cadeia de mensagens de erro.

> **Dica:** Ao corrigir erros, corrija o primeiro erro primeiro. Se voce nao conseguir encontra-lo na linha identificada como a linha com o erro, verifique a linha anterior.

Esteja ciente de que o fato de um compilador especifico aceitar um programa nao significa necessariamente que o programa e C++ valido. E o fato de um compilador especifico rejeitar um programa nao significa necessariamente que o programa e C++ invalido. No entanto, os compiladores atuais estao mais em conformidade com o Padrao do que seus antecessores de alguns anos atras. Alem disso, os compiladores normalmente tem opcoes para controlar o quao rigoroso e o compilador.

> **Dica:** Ocasionalmente, os compiladores ficam confusos apos construir um programa de forma incompleta e respondem com mensagens de erro sem sentido que nao podem ser corrigidas. Nesses casos, voce pode resolver a situacao selecionando *Build All* para reiniciar o processo do zero. Infelizmente, e dificil distinguir essa situacao da mais comum em que as mensagens de erro simplesmente parecem nao ter sentido.

Normalmente, o IDE permite rodar o programa em uma janela auxiliar. Alguns IDEs fecham a janela assim que o programa termina a execucao, e alguns a mantem aberta. Se o seu compilador fechar a janela, voce tera dificuldade em ver a saida a menos que tenha olhos rapidos e memoria fotografica. Para ver a saida, voce deve colocar algum codigo adicional no final do programa:

```cpp
cin.get(); // adicione esta instrucao
cin.get(); // e talvez esta tambem
return 0;
```

A instrucao `cin.get()` le o proximo pressionamento de tecla, portanto essa instrucao faz o programa aguardar ate voce pressionar Enter. (Nenhum pressionamento de tecla e enviado ao programa ate que voce pressione Enter, portanto nao ha sentido em pressionar outra tecla.) A segunda instrucao e necessaria se o programa deixar um pressionamento de tecla nao processado apos sua entrada regular. Por exemplo, se voce inserir um numero, voce digita o numero e pressiona Enter. O programa le o numero, mas deixa o pressionamento de Enter nao processado, e ele e entao lido pelo primeiro `cin.get()`.

#### C++ no Macintosh

A Apple atualmente fornece um framework de desenvolvedor chamado Xcode com o sistema operacional Mac OS X. E gratuito, mas normalmente nao vem pre-instalado. Voce pode instala-lo a partir dos discos de instalacao do sistema operacional ou pode baixa-lo por uma taxa nominal da Apple. (Esteja ciente de que o download tem mais de 4 GB.) Ele nao apenas fornece um IDE que suporta varias linguagens de programacao, como tambem instala alguns compiladores — `g++` e `clang` — que podem ser usados como programas de linha de comando no modo Unix acessivel por meio do utilitario Terminal.

> **Dica para IDEs:** Para economizar tempo, voce pode usar apenas um projeto para todos os programas de exemplo. Basta excluir o arquivo de codigo-fonte do exemplo anterior da lista do projeto e adicionar o codigo-fonte atual. Isso economiza tempo, esforco e reduz a desordem no disco.

## Resumo

A medida que os computadores se tornaram mais poderosos, os programas de computador se tornaram maiores e mais complexos. Em resposta a essas condicoes, as linguagens de computador evoluiram de modo a facilitar o gerenciamento do processo de programacao. A linguagem C incorporou recursos como estruturas de controle e funcoes para controlar melhor o fluxo de um programa e para possibilitar uma abordagem mais estruturada e modular. A esses recursos, o C++ adiciona suporte para programacao orientada a objetos e programacao generica. Isso possibilita uma modularidade ainda maior e facilita a criacao de codigo reutilizavel, o que economiza tempo e aumenta a confiabilidade do programa.

A popularidade do C++ resultou em um grande numero de implementacoes para muitas plataformas de computacao; os padroes ISO do C++ (C++98/03 e C++11) fornecem uma base para manter essas muitas implementacoes mutuamente compativeis. Os padroes estabelecem os recursos que a linguagem deve ter, o comportamento que a linguagem deve exibir e uma biblioteca padrao de funcoes, classes e templates. Os padroes suportam o objetivo de uma linguagem portavel entre diferentes plataformas de computacao e diferentes implementacoes da linguagem.

Para criar um programa C++, voce cria um ou mais arquivos de codigo-fonte contendo o programa expresso na linguagem C++. Esses sao arquivos de texto que precisam ser compilados e vinculados para produzir os arquivos em linguagem de maquina que constituem programas executaveis. Essas tarefas sao frequentemente realizadas em um IDE que fornece um editor de texto para criar os arquivos de codigo-fonte, um compilador e um vinculador para produzir arquivos executaveis e outros recursos, como gerenciamento de projeto e capacidades de depuracao. Mas as mesmas tarefas tambem podem ser realizadas em um ambiente de linha de comando invocando as ferramentas apropriadas individualmente.

---

> Navegacao: [Indice](README.md) | [Proximo](capitulo-02.md)
