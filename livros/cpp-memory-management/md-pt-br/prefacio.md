# Prefacio (Preface)

> Traducao nao oficial do livro *C++ Memory Management* (Patrice Roy, 2025).
> Texto adaptado para portugues brasileiro com fins educacionais. Paginas originais: xvii-xxi.
> Repositorio: [ring0-study](https://github.com/cnthigu/ring0-study)

> Navegacao: [Indice](README.md) | [Proximo](capitulo-01.md)

---

Programas precisam alocar e gerenciar memoria com regularidade, independentemente da linguagem em que sao escritos. Por que e como fazemos isso depende da linguagem e do dominio da aplicacao: sistemas de tempo real, sistemas embarcados, jogos e aplicacoes desktop convencionais possuem necessidades e restricoes distintas, e nao existe uma unica abordagem universal para todos os problemas.

Este livro mostra como o C++ moderno permite que programadores escrevam programas mais simples e seguros, mas tambem como essa linguagem torna possivel assumir o controle dos mecanismos de alocacao de memoria e garantir que os programas respeitem as restricoes que enfrentam. Partindo dos conceitos basicos de *lifetime* (tempo de vida) de objetos e organizacao de memoria da linguagem, voce aprendera a escrever seus proprios containers e alocadores, e a adaptar o proprio comportamento dos operadores de alocacao as suas necessidades. Dependendo do que precisar, voce sera capaz de criar programas menores, mais rapidos, mais previsveis... e mais seguros.

## Para quem e este livro

Este livro foi escrito para pessoas com alguma experiencia em programacao que apreciam tanto programacao de alto nivel quanto de baixo nivel. Ter experiencia previa com programacao generica e programacao concorrente tornara a leitura mais proveitosa.

Mais especificamente, este livro foi escrito para voce se: (a) voce acha que gerenciar memoria em C++ e dificil, mas esta disposto a rever essa percepcao; (b) voce quer ter mais controle sobre a forma como seus programas gerenciam memoria; ou (c) voce quer que seus programas sejam menores, mais rapidos e mais seguros. Voce pode se beneficiar deste livro vindo de um contexto C++, claro, mas tambem se normalmente programa em outras linguagens e gostaria de ver o que o C++ permite fazer. Este livro sera util para qualquer programador, mas pode ser especialmente valioso se voce programa em ambientes com restricoes (como sistemas embarcados ou consoles de jogos) ou em outros dominios de aplicacao onde e necessario controle rigido sobre os mecanismos de alocacao de recursos. Quem sabe, voce pode ate gostar!

## O que este livro cobre

**Capitulo 1, Objetos, Ponteiros e Referencias**, discute os conceitos basicos do modelo de objetos na linguagem C++, fornecendo um vocabulario basico comum.

**Capitulo 2, Cuidados Necessarios**, examina alguns aspectos complicados do C++, com um olhar mais especifico sobre manobras de programacao de baixo nivel que podem nos trazer problemas; veremos que tipo de problemas essas manobras podem causar.

**Capitulo 3, Casts e Qualificacoes cv**, examina as ferramentas a nossa disposicao para forcar o sistema de tipos a se adaptar as nossas necessidades e discute como usar essas ferramentas, as vezes cortantes, de formas razoaveis.

**Capitulo 4, Usando Destrutores**, examina esse importante aspecto do C++ que torna possivel escrever objetos responsaveis pelo gerenciamento de recursos em geral e de memoria em particular.

**Capitulo 5, Usando Smart Pointers Padrao**, oferece um olhar sobre como podemos nos beneficiar dessa parte importante da programacao C++ contemporanea, que inscreve a responsabilidade sobre a memoria no sistema de tipos.

**Capitulo 6, Escrevendo Smart Pointers**, explora maneiras de escrever versoes caseiras dos smart pointers padrao, bem como projetar nossos proprios smart pointers para cobrir nichos que ainda nao sao atendidos pelos fornecidos pela biblioteca padrao.

**Capitulo 7, Sobrecarregando Operadores de Alocacao de Memoria**, mostra algumas das muitas maneiras pelas quais podemos fornecer nossas proprias versoes dos operadores de alocacao de memoria e explica por que isso pode ser uma boa ideia.

**Capitulo 8, Escrevendo um Detector de Vazamentos Simples**, coloca nossas novas habilidades de gerenciamento de memoria em uso para escrever uma ferramenta funcional (simples) de deteccao de vazamentos de memoria de forma essencialmente transparente para o codigo do usuario.

**Capitulo 9, Mecanismos Atipicos de Alocacao**, faz um tour por algumas aplicacoes incomuns (e sobrecargas) dos operadores de alocacao de memoria padrao, incluindo versoes sem excecao e outras que lidam com memoria "exotica".

**Capitulo 10, Gerenciamento de Memoria Baseado em Arena e Outras Otimizacoes**, usa nossas habilidades de gerenciamento de memoria para fazer programas executarem mais rapidamente e se comportarem de forma mais deterministica, aproveitando conhecimentos especificos do dominio ou da aplicacao.

**Capitulo 11, Reclamacao Diferida**, explora maneiras de escrever programas que recuperam automaticamente objetos alocados dinamicamente em momentos escolhidos durante a execucao do programa.

**Capitulo 12, Containers Genericos com Gerenciamento Explicito de Memoria**, explica como escrever dois containers genericos eficientes que gerenciam memoria por conta propria e discute os trade-offs de seguranca a excecoes e complexidade dessa pratica.

**Capitulo 13, Containers Genericos com Gerenciamento Implicito de Memoria**, revisita os containers escritos no capitulo anterior para ver os impactos de passar de uma abordagem de gerenciamento explicito de memoria para uma implicita que depende de smart pointers.

**Capitulo 14, Containers Genericos com Suporte a Alocadores**, revisita nossos containers caseiros para ver como o gerenciamento de memoria pode ser customizado por meio de alocadores, cobrindo alocadores de antes do C++11 ate os alocadores contemporaneos, assim como os PMR allocators.

**Capitulo 15, Questoes Contemporaneas**, olha em direcao ao futuro proximo e examina alguns recursos recentes (na epoca da escrita do livro) do C++ relacionados ao gerenciamento de memoria, bem como algumas interessantes adicoes candidatas para o C++26 e C++29.

**Anexo, Coisas que Voce Deve Saber**, fornece algum embasamento tecnico que pode ajudar voce a aproveitar ao maximo este livro, mas que pode nao ser de conhecimento comum. Consulte conforme necessario, esta la para voce!

## Para aproveitar ao maximo este livro

Voce precisara de um compilador C++ contemporaneo, idealmente que suporte pelo menos C++20 e, de preferencia, C++23. Este livro nao exige outras ferramentas, mas voce e claro bem-vindo a usar seu editor de codigo favorito e experimentar os exemplos que encontrara ao longo do caminho.

Cuidado foi tomado para seguir o C++ padrao de uma perspectiva portatil e segura. Os poucos lugares onde voce encontrara exemplos que usam codigo nao portatil sao identificados como tal.

Os exemplos de codigo foram testados em tres compiladores distintos, e as amostras no repositorio GitHub do livro contem, alem do codigo-fonte real, links para versoes online (nos comentarios) que compilam e que voce pode modificar e adaptar como desejar.

Se voce estiver usando a versao digital deste livro, recomendamos que voce digite o codigo voce mesmo ou acesse o codigo no repositorio GitHub do livro. Fazer isso vai ajudar a evitar possiveis erros relacionados ao copiar e colar codigo.

Espero que voce aprecie a experiencia e encontre nos exemplos um ponto de partida interessante para suas proprias exploracoes.

## Baixando os arquivos de codigo de exemplo

Voce pode baixar os arquivos de codigo de exemplo deste livro no GitHub em [https://github.com/PacktPublishing/C-Plus-Plus-Memory-Management](https://github.com/PacktPublishing/C-Plus-Plus-Memory-Management). Se houver uma atualizacao no codigo, ela sera atualizada no repositorio GitHub.
