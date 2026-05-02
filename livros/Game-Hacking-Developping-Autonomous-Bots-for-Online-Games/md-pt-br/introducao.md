# Introducao

> Titulo original: *Introduction*

> Navegacao: [Anterior](agradecimentos.md) | [Indice](README.md) | [Proximo](capitulo-01.md)

## Topicos

- Pre-requisitos para o leitor
- Breve historia de game hacking
- Por que hackear games
- Como o livro esta organizado
- Recursos online e como usar o livro

## Abertura

Um equivoco comum no mundo do online gaming e a ideia de que o unico
game que voce pode jogar e o que aparece no titulo. Na pratica, game
hackers gostam de jogar o game que se esconde atras da cortina: um
jogo de gato e rato entre eles e os game developers. Enquanto os
game hackers fazem reverse engineering de game binaries, automatizam
partes do gameplay e modificam ambientes de jogo, os game developers
combatem as ferramentas criadas pelos hackers (em geral chamadas de
*bots*) usando tecnicas de anti-reversing, algoritmos de bot
detection e data mining heuristico.

A medida que essa batalha entre game hackers e developers avanca, os
metodos tecnicos dos dois lados (muitos deles parecidos com as
tecnicas de desenvolvedores de malware e de fornecedores de
antivirus) vao evoluindo e ficando cada vez mais complexos. Este
livro destaca a luta dos game hackers e os metodos avancados que
eles criaram para manipular games enquanto driblam os game developers
nos cantos escuros do proprio software dos jogos.

Embora o foco do livro seja te ensinar a desenvolver ferramentas que
provavelmente seriam consideradas inconvenientes ou ate mesmo
maliciosas pelas empresas de games, voce vai perceber que muitas das
tecnicas sao uteis para desenvolver ferramentas perfeitamente
inofensivas e neutras. Mais ainda, conhecer essas tecnicas tambem e
fundamental para os game developers que tentam impedir o uso delas.

## Pre-requisitos para o leitor

Este livro nao tenta te ensinar software development e, por isso,
parte do principio que voce ja tem, no minimo, um background solido em
desenvolvimento de software. Esse background deve incluir familiaridade
com desenvolvimento nativo no Windows, alguma experiencia leve com
game development e gerenciamento de memoria. Esses conhecimentos sao
suficientes para acompanhar o livro, mas experiencia com assembly x86
e Windows internals garante que detalhes das implementacoes mais
avancadas nao passem batido.

Como todos os hacks avancados discutidos no livro dependem de code
injection, e essencial saber programar em uma linguagem nativa como C
ou C++. Todos os exemplos de codigo do livro sao escritos em C++ e
podem ser compilados com o Microsoft Visual C++ Express Edition. Voce
pode baixar o MSVC++ Express Edition em
<http://www.visualstudio.com/en-US/products/visual-studio-express-vs>.

> NOTA: outras linguagens que compilam para codigo nativo, como Delphi,
> tambem sao capazes de injection, mas nao serao discutidas neste
> livro.

## Uma breve historia de game hacking

Desde o surgimento do online PC gaming no inicio dos anos 1980, existe
uma guerra constante entre game hackers e game developers. Essa briga
quase sem fim levou os game developers a investir incontaveis horas
para impedir que os hackers desmontem seus games e enfiem o dedo entre
as engrenagens. Esses hackers, que revidam com implementacoes
sofisticadas de stealth, tem varios motivos: graficos customizados,
melhor performance, conveniencia, gameplay autonomo, aquisicao de
in-game assets e, claro, lucro real.

O fim dos anos 90 e o comeco dos anos 2000 foram a era de ouro do game
hacking, quando os games online de PC ficaram avancados o bastante
para atrair multidoes mas ainda eram simples o suficiente para serem
revertidos e manipulados. Games online lancados nessa epoca, como
*Tibia* (janeiro de 1997), *Runescape* (janeiro de 2001) e
*Ultima Online* (setembro de 1997), foram alvo pesado de bot
developers. Os developers desses jogos e de outros parecidos ainda
hoje lutam para controlar as comunidades enormes de bot developers e
de bot users. A inacao dos developers somada a tenacidade dos hackers
nao so destruiu economias inteiras dentro dos games como tambem deu
origem a uma industria lucrativa em torno do desenvolvimento e da
defesa contra bots.

Nos anos seguintes, empresas mais maduras passaram a levar a defesa
contra bots muito a serio. Hoje muitas tem times dedicados ao
desenvolvimento de sistemas de bot prevention, e outras tantas
encaram bots como questao judicial: nao hesitam em banir jogadores
que usam bots nem em processar os bot developers que vendem essas
ferramentas.
Resultado: muitos game hackers foram empurrados para tecnicas
avancadas de stealth para manter seus usuarios em seguranca.

A guerra continua, e os numeros dos dois lados devem crescer a
medida que o online gaming for ficando mais comum nos proximos anos.
Os grandes game developers perseguem hackers com determinacao, e
chegam a processar gigantes do game hacking em acoes milionarias.
Isso significa que quem leva o negocio a serio precisa mirar empresas
menores ou vender seus produtos anonimamente nas sombras para escapar
de processos. Pelo menos no futuro previsivel, game hacking e bot
development vao crescer ainda mais, gerando uma industria maior e
mais lucrativa para os game hackers ousados o suficiente para correr
o risco.

## Por que hackear games?

Alem do apelo obvio e do aspecto desafiador, game hacking tem
finalidades praticas e lucrativas. Todo dia milhares de programadores
iniciantes experimentam game hacking em pequena escala como forma de
automatizar tarefas monotonas ou executar acoes repetitivas. Esses
script kiddies costumam usar ferramentas de automacao como AutoIt para
seus hacks pequenos e relativamente inofensivos. Por outro lado, game
hackers profissionais, com toolkits enormes e anos de experiencia em
programacao, dedicam centenas de horas ao desenvolvimento de hacks
avancados. Esse tipo de hack, foco do livro, costuma ser construido
com a intencao de gerar grandes quantidades de dinheiro.

Gaming e uma industria gigantesca: gerou US$ 22,4 bilhoes em vendas em
2014, segundo a Entertainment Software Association. Das dezenas de
milhoes de jogadores que jogam diariamente, 20% jogam massively
multiplayer online role-playing games (MMORPGs). MMORPGs costumam ter
milhares de jogadores trocando bens virtuais em economias ativas
dentro do jogo. Esses jogadores sentem necessidade de in-game assets e
estao dispostos a comprar com dinheiro real. Por isso, players de
MMORPG acabam formando grandes comunidades que prestam servicos de
gold-for-cash. Esses servicos chegam a praticar taxas de cambio entre
gold do jogo e moedas reais.

Aproveitando essa oportunidade, game hackers criam bots capazes de
fazer farm de gold automaticamente e subir nivel de personagens.
Dependendo do objetivo, o hacker monta gold farms enormes para vender
o lucro in-game, ou refina e vende o software para jogadores que
querem subir nivel e acumular gold sem complicacao. Por causa do
tamanho das comunidades de MMORPGs populares, esses game hackers
chegam a faturar entre seis e sete digitos por ano.

Os MMORPGs oferecem a maior superficie de ataque para hackers, mas
tem um publico relativamente pequeno no geral. Cerca de 38% dos
gamers preferem real-time strategy (RTS) e multiplayer online battle
arena (MOBA), e mais 6% jogam principalmente first-person shooter
(FPS). Esses games competitivos do tipo player versus player (PvP)
representam, juntos, 44% do mercado de gaming e oferecem boas
recompensas para os game hackers determinados.

Games PvP costumam ser episodicos: cada partida e um jogo isolado e
em geral nao ha muita progressao lucrativa em deixar um bot rodando
AFK. Por isso, em vez de gold farms ou bots autonomos para upar
personagem, os hackers criam reactive bots que ajudam o jogador no
combate.

Esses games altamente competitivos giram em torno de skill e tatica,
e a maioria dos jogadores participa para provar a propria habilidade.
Por consequencia, o numero de pessoas que procura bots para PvP e
bem menor do que o do mundo grindy dos MMORPGs. Mesmo assim, os
hackers ainda lucram bem vendendo PvP bots, em geral muito mais
faceis de desenvolver do que bots autonomos completos.

## Como o livro esta organizado

O livro esta dividido em quatro partes, cada uma focada em um aspecto
diferente de game hacking.

### Parte 1: Tools of the Trade

Voce recebe a caixa de ferramentas para hackear games.

- **Capitulo 1: Scanning Memory Using Cheat Engine** ensina como
  escanear a memoria de um game em busca de valores importantes
  usando o Cheat Engine.
- **Capitulo 2: Debugging Games with OllyDbg** da um crash course de
  debugging e reverse engineering com OllyDbg. As skills aprendidas
  ai serao muito uteis quando voce comecar a fazer bots avancados e
  injetar codigo.
- **Capitulo 3: Reconnaissance with Process Monitor and Process
  Explorer** ensina a usar duas ferramentas de reconhecimento para
  inspecionar como games interagem com arquivos, outros processos,
  rede e sistema operacional.

Os recursos online de cada capitulo da Parte 1 incluem binarios
customizados que o autor criou para voce ter um lugar seguro para
testar e refinar suas novas skills.

### Parte 2: Game Dissection

Quando voce ja estiver confortavel com cada chave e martelo, a Parte 2
ensina a abrir o capo e entender como games funcionam.

- **Capitulo 4: From Code to Memory: A General Primer** mostra como
  ficam o source code e os dados de um game depois de compilados em
  um game binary.
- **Capitulo 5: Advanced Memory Forensics** se apoia no Capitulo 4.
  Voce aprende a escanear memoria e usar debugging para localizar
  valores complicados em memoria e dissecar classes e estruturas
  complexas.
- **Capitulo 6: Reading from and Writing to Game Memory** mostra
  como ler e modificar dados dentro de um game em execucao.

Esses capitulos trazem bastante codigo de proof-of-concept para voce
verificar tudo o que le.

### Parte 3: Process Puppeteering

Voce vira o titereiro e aprende a transformar qualquer game em uma
marionete.

- **Capitulo 7: Code Injection**, apoiado nas skills das Partes 1 e
  2, descreve como injetar e executar seu proprio codigo no espaco
  de enderecamento de um game.
- **Capitulo 8: Manipulating Control Flow in a Game** ensina a usar
  injection para interceptar, modificar ou desabilitar qualquer
  function call feita pelo game, encerrando com exemplos reais para
  bibliotecas comuns como Adobe AIR e Direct3D.

Para complementar essas aulas de titereiro, esses capitulos vem com
milhares de linhas de codigo prontas para producao que voce pode usar
como library boilerplate para um futuro bot.

### Parte 4: Creating Bots

Voce ve como combinar toolbox, habilidades de dissecacao,
puppeteering e seu background em software engineering para criar bots
poderosos.

- **Capitulo 9: Using Extrasensory Perception to Ward Off Fog of
  War** explora formas de fazer um game exibir informacao util que
  ele nao expoe por padrao, como localizacao de inimigos escondidos
  e quanto de experience voce ganha por hora.
- **Capitulo 10: Responsive Hacks** mostra padroes de codigo para
  detectar in-game events, como queda de health, e fazer bots que
  reagem a esses eventos mais rapido que jogadores humanos.
- **Capitulo 11: Putting It All Together: Writing Autonomous Bots**
  revela como bots que jogam sem interacao humana funcionam.
  Automated bots combinam control theory, state machines, search
  algorithms e modelos matematicos; o capitulo e um crash course
  desses topicos.
- **Capitulo 12: Staying Hidden** ensina algumas das tecnicas
  high-level que voce pode usar para escapar e burlar qualquer
  sistema que tente atrapalhar seus bots.

Como voce ja deve ter percebido, esses capitulos tem muito codigo de
exemplo. Alguns hacks aqui sao construidos sobre o codigo dos
capitulos anteriores. Outros exploram design patterns sucintos e
diretos que voce pode usar para criar seus proprios bots. Apos
terminar as quatro partes do livro, voce sera lancado no mundo
virtual com seu novo superpoder.

## Sobre os recursos online

Voce encontra varios recursos adicionais para o livro em
<https://www.nostarch.com/gamehacking/>. Esses recursos incluem
binarios compilados para voce testar suas skills, uma quantidade
consideravel de codigo de exemplo e bons trechos de codigo de game
hacking prontos para producao. Eles caminham junto com o livro, que
nao fica completo sem eles. Baixe antes de continuar.

## Como usar este livro

O livro deve ser usado, antes de tudo, como guia para te iniciar em
game hacking. A progressao e tal que o conteudo de cada capitulo
introduz novas skills que se apoiam em todos os capitulos anteriores.
A medida que voce conclui capitulos, brinque com o codigo de exemplo
e teste suas skills em um game real antes de seguir adiante. Isso e
importante porque alguns topicos vao ter casos de uso que so ficam
evidentes quando voce esta enfiado ate o pescoco no problema.

Quando voce terminar o livro, espero que ele continue util como
manual de campo. Se voce esbarrar em alguma data structure
desconhecida, talvez os detalhes do Capitulo 5 ajudem. Se fizer
reverse engineering do formato de mapa de um game e decidir criar um
pathfinder, da para voltar ao Capitulo 11, estudar o conteudo e usar
o codigo de exemplo como ponto de partida. Embora seja impossivel
antecipar todos os problemas, eu tentei garantir que voce ache
algumas respostas dentro destas paginas.

> ### Boxe: Nota da editora
>
> Este livro nao apoia pirataria, violacao da DMCA, infracao de
> copyright nem violacao do Terms of Service de games. Game hackers ja
> foram banidos de games para sempre, processados em milhoes de
> dolares e ate presos pelo seu trabalho.
