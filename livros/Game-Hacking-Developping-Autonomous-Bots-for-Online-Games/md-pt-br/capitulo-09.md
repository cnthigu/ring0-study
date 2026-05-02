# Capitulo 9 - Usando ESP para anular o Fog of War

> Titulo original: *Using Extrasensory Perception to Ward Off Fog of War*

> Navegacao: [Anterior](capitulo-08.md) | [Indice](README.md) | [Proximo](capitulo-10.md)

## Topicos

- Lighthacks (revelar ambientes escuros)
- Wallhacks (mostrar inimigos atras de paredes)
- Zoomhacks (ampliar o campo de visao)
- HUDs (exibir info historica/agregada)
- Outros ESP hacks (range, loading-screen, pick-phase, floor spy)

## Abertura

*Fog of war* (ou apenas *fog*) e um mecanismo que devs usam para
limitar a percepcao do jogador e esconder informacoes do
ambiente. Nos MOBA fica literal (falta de visao), mas o conceito
abrange qualquer obscurecimento de informacao relevante: figuras
encobertas (cloaked), salas escuras e inimigos atras de paredes
sao todos formas de fog.

Game hackers reduzem ou eliminam fog com *ESP hacks*
(*extrasensory perception*). ESP hacks usam hooking, manipulacao
de memoria, ou ambos, para forcar o game a revelar informacao
escondida. Eles aproveitam que parte do fog e implementada
client-side: o cliente ainda tem a info (parcial ou completa).

## Conhecimento de fundo

Este capitulo marca a transicao de hacking, puppeteering e reverse
engineering para *codar*. Daqui em diante, voce escreve seus
proprios hacks. Tudo o que veio antes vira "background". Se voce
encontrar uma tecnica que nao lembra (memory scanning, memory
breakpoints, hooking, write em memoria), volte aos capitulos
relevantes.

Especificamente, este capitulo fala bastante de Direct3D. No
Capitulo 8 voce viu como hookar o drawing loop. O exemplo
`GameHackingExamples/Chapter8_Direct3DHook` traz um motor de
hooking funcional, e muitos hacks aqui se apoiam nele (ver
`main.cpp`). Voce pode rodar o app
`GameHackingExamples/Chapter8_Direct3DApplication` para testar.

## Revelando detalhes com lighthacks

*Lighthacks* aumentam a iluminacao em ambientes escuros, deixando
inimigos, treasure chests, caminhos e qualquer coisa normalmente
oculta visivel. A iluminacao e cosmetica, no graphics layer; da
para mexer com hook nessa camada.

A iluminacao otima depende de orientacao da camera, layout do
ambiente e ate caracteristicas do engine. Voce pode mexer em
qualquer um desses fatores, mas o jeito mais simples e adicionar
mais luz ao ambiente.

### Adicionando uma fonte de luz ambiente central

Os exemplos do livro tem dois lighthacks pequenos. O primeiro,
`enableLightHackDirectional()` em `main.cpp` (Listagem 9-1):

```cpp
void enableLightHackDirectional(LPDIRECT3DDEVICE9 pDevice)
{
    D3DLIGHT9 light;
    ZeroMemory(&light, sizeof(light));
    light.Type      = D3DLIGHT_DIRECTIONAL;
    light.Diffuse   = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
    light.Direction = D3DXVECTOR3(-1.0f, -0.5f, -1.0f);
    pDevice->SetLight(0, &light);
    pDevice->LightEnable(0, TRUE);
}
```

> Listagem 9-1: lighthack direcional.

Chamado a partir do hook em `EndScene()`, cria um light source
chamado `light`. `light.Type = D3DLIGHT_DIRECTIONAL` (spotlight
projetando luz numa direcao). RGB de `light.Diffuse` em `0.5,
0.5, 0.5` da brilho off-white. `light.Direction` aponta para um
ponto arbitrario do espaco 3D. Por fim, registra a luz no indice
0 e ativa.

> NOTA: no exemplo, a luz brilha do canto inferior esquerdo para
> cima e direita. Pode precisar ajustar conforme o jogo.

Inserir no indice 0 funciona como prova de conceito, mas nao
sempre. Games costumam ter varias light sources; usar indice
ocupado pode sobrescrever efeitos criticos. Na pratica, use indice
arbitrariamente alto. Limitacao desse tipo: luzes direcionais sao
bloqueadas por paredes, criaturas e terreno; sombras continuam.
Otimo em areas abertas, ruim em corredores estreitos ou cavernas.

### Aumentando a luz ambiente absoluta

A outra abordagem, `enableLightHackAmbient()`, e bem mais
agressiva: afeta o nivel de luz globalmente.

```cpp
void enableLightHackAmbient(LPDIRECT3DDEVICE9 pDevice)
{
    pDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(100, 100, 100));
}
```

Define a luz ambiente absoluta (`D3DRS_AMBIENT`) para um branco
de forca media. `D3DCOLOR_XRGB` recebe valores RGB; aqui
`100, 100, 100`. Ilumina tudo com luz omnidirectional branca,
revelando ambiente as custas das sombras e detalhes baseados em
luz.

### Outros tipos de lighthack

Existem outros caminhos, dependentes de cada game. Um criativo: dar
NOP no codigo que chama `device->SetRenderState()`. Como essa
funcao seta o ambiente global, desabilitar a chamada deixa o
Direct3D na config default e tudo fica visivel. Talvez o
lighthack mais poderoso, mas exige saber o endereco do codigo.

Tambem ha lighthacks por memoria. Em alguns games, players e
criaturas emitem luz de cores e intensidades diferentes (conforme
equipamento, mount ou spells). Conhecendo o struct da creature
list, voce modifica os campos que determinam cor/intensidade da
luz emitida. Imagine um game em que personagens emitem orb azul
quando sob healing/buff: localizando os valores em memoria, da
para fazer todas emitirem orbs. Visual legal em games top-down 2D;
em 3D, transforma criaturas em bolhas luminosas.

Voce tambem pode hookar `SetLight()`, no indice 51 da VF table
da Direct3D device. No callback, modifique a `D3DLIGHT9`
interceptada antes de passar a original. Por exemplo, mude todas
para `D3DLIGHT_POINT`, fazendo cada light source existente
irradiar como uma lampada. Poderoso e preciso, mas pode dar
visuais estranhos; quebra em ambientes sem iluminacao e obstaculos
opacos ainda bloqueiam point lights.

Lighthacks sao poderosos, mas nao revelam *informacao escondida*.
Para isso, voce precisa de um wallhack.

## Mostrando inimigos sorrateiros com wallhacks

*Wallhacks* expoem inimigos atras de paredes, pisos e outros
obstaculos. O metodo mais comum aproveita o *z-buffering*.

### Renderizacao com z-buffering

Direct3D e a maioria dos engines suportam *z-buffering*: garantia
de que, quando objetos se sobrepoem, so o mais proximo aparece. O
processo desenha a cena num array 2D que descreve, para cada
pixel, a distancia (z) ate o viewer. Pense nos indices do array
como eixos x (horizontal) e y (vertical), e cada valor como o z
do pixel.

Quando um novo objeto chega, o z-buffer decide se ele aparece. Se
o (x, y) ja esta preenchido, comparam-se os z's: o novo so entra
se tem z menor. Quando termina, o array e flushado para a tela.

Imagine um espaco 3D rendered em viewport 4x4 pixels. O z-buffer
inicia vazio (Figura 9-1: 16 celulas, todas `z = 0`, sem cor).

Apos o game desenhar:

- Background azul cobrindo tudo, z = 100.
- Retangulo vermelho 2x2 em (0,0), z = 5.
- Retangulo verde 2x2 em (1,1), z = 3.

A Figura 9-2 mostra o z-buffer final: o verde (z = 3) sobrepoe
parte do vermelho (z = 5), que sobrepoe parte do azul (z = 100).

Esse comportamento permite que o game desenhe map, players,
criaturas, detalhes e particulas sem se preocupar com o que e
realmente visivel ao jogador. E uma otimizacao gigante para devs,
mas abre uma superficie de ataque enorme: como *todos* os models
sao entregues ao engine, voce hookea para detectar o que o jogador
nao deveria estar vendo.

### Wallhack Direct3D

Crie wallhacks manipulando z-buffering com hook em
`DrawIndexedPrimitive()`, chamada quando o game desenha um modelo
3D. Quando o modelo de inimigo aparece, o wallhack desliga
z-buffering, chama a funcao original para desenhar, e religa.
Resultado: o modelo aparece em cima de tudo. Variantes pintam
modelos com cor solida (vermelho para inimigos, verde para
aliados).

#### Toggling z-buffering

O Direct3D hook em `main.cpp` traz exemplo no
`onDrawIndexedPrimitive()`:

```cpp
void onDrawIndexedPrimitive(
    DirectXHook* hook,
    LPDIRECT3DDEVICE9 device,
    D3DPRIMITIVETYPE primType,
    INT  baseVertexIndex,
    UINT minVertexIndex,
    UINT numVertices,
    UINT startIndex,
    UINT primCount)
{
    if (numVertices == 24 && primCount == 12) {
        // e um inimigo, aplica wallhack
    }
}
```

E callback do hook em `DrawIndexedPrimitive()` (indice 82 na VF
table do device). Todo modelo passa por aqui, com propriedades
especificas. Inspecionando `numVertices` e `primCount`, o hook
detecta inimigo. Aqui, `24` e `12`. Dentro do `if()`:

```cpp
device->SetRenderState(D3DRS_ZENABLE, false);  // desliga z-buffering
DirectXHook::origDrawIndexedPrimitive(         // desenha modelo
    device, primType, baseVertexIndex,
    minVertexIndex, numVertices, startIndex, primCount);
device->SetRenderState(D3DRS_ZENABLE, true);   // religa z-buffering
```

Sem z-buffering durante o desenho, o inimigo fica em cima de
tudo.

#### Trocando textura do inimigo

Para personalizar como o inimigo aparece, troque a textura usada:

```cpp
// quando o hook inicializa
LPDIRECT3DTEXTURE9 red;
D3DXCreateTextureFromFile(device, "red.png", &red);

// pouco antes de desenhar a primitive
device->SetTexture(0, red);
```

O primeiro bloco roda uma unica vez (no `initialize()` do hook,
chamado na primeira vez que o callback de `EndScene()` dispara). O
segundo, antes do `DrawIndexedPrimitive()` original, faz o modelo
ser desenhado com a textura customizada.

#### Fingerprintando o modelo

A parte trabalhosa e achar os valores certos de `numVertices` e
`primCount`. Construa uma ferramenta que loga toda combinacao
unica e permite percorrer com o teclado.

No escopo global, declare um struct que tenha:

- `numVertices` e `primCount`.
- Um `std::set` desse struct (chame `seenParams`).
- Uma instance dele (chame `currentParams`).

`std::set` precisa de comparator; declare um functor que usa
`memcmp()` para comparar dois structs. A cada chamada do callback
de `DrawIndexedPrimitive()`, monte um struct e passe para
`seenParams.insert()` (so adiciona se ainda nao existe).

Detecte teclas com `GetAsyncKeyState()` e itere:

```cpp
auto current = seenParams.find(currentParams);
if (current == seenParams.end())
    current = seenParams.begin();
else
    current++;
currentParams = *current;
```

Pressionando space, `currentParams` avanca para o proximo par.
Use logica similar ao wallhack para trocar a textura dos modelos
que casam com `currentParams.numVertices` e
`currentParams.primCount`. Voce tambem pode desenhar esses valores
na tela.

Em uso: rode num modo onde nao morre (contra um amigo, modo de
customizacao, etc.), aperte space ate destacar o modelo desejado e
anote os valores. Atualize o `if()` do wallhack com eles.

> NOTA: modelos de personagem costumam ser compostos por sub-models
> (cabeca, torso, pernas), e games mostram modelos diferentes para
> distancias diferentes. Um personagem pode ter 20+ models. Em
> geral basta destacar um (ex.: torso do inimigo).

## Visao mais ampla com zoomhacks

Muitos games MOBA e RTS usam estilo top-down 3D, imune a
wallhacks. Eles tambem usam escuridao no map como fog, mas
lighthack nao ajuda: a info esta no servidor, nao no client. Esses
generos viram ESP hacks quase inuteis... exceto pelo *zoomhack*.

Zoomhacks deixam voce dar zoom out muito alem do permitido,
revelando porcoes maiores do map.

### Zoomhack via NOPing

MOBA/RTS permitem zoom variavel mas limitado. O zoomhack mais
simples acha o *zoom factor* (multiplicador, geralmente `float`
ou `double`) e sobrescreve com valor maior.

Para achar com Cheat Engine, busque um `float` de valor inicial
desconhecido. Em rescans:

1. Va ao game e de zoom in.
2. Procure increased value no Cheat Engine.
3. Va ao game e de zoom out.
4. Procure decreased value no Cheat Engine.

Reduza ate poucos candidatos. Confirme freezando: se travar,
zoom para de funcionar - achou. Se busca em `float` falhar, tente
`double`. Se ainda falhar, inverta a logica (zoom in com decreased
value, zoom out com increased). Achado, escreva valor maior.

Variantes mais avancadas dao NOP no codigo que valida o range do
zoom factor. Use OllyDbg: ponha breakpoint memory on-write no
zoom factor, de zoom no game para disparar e analise. Codigo de
limitacao e facil de reconhecer: constantes que casam com
min/max do zoom denunciam.

Se nao achar com on-write, talvez a limitacao seja aplicada quando
os graficos sao redrawn no novo zoom level (e nao quando o factor
muda). Troque para memory on-read e olhe pelas mesmas pistas.

### Riscando a superficie de zoomhacks por hooking

Da para fazer zoomhack hookando `device->SetTransform(type,
matrix)`, mas exige bom dominio de como o game configura
perspectiva. Tem dois jeitos: *view* (`D3DTS_VIEW`) e *projection*
(`D3DTS_PROJECTION`).

Manipular essas matrizes corretamente exige matematica de graficos
3D. O autor evita esse caminho - nunca teve problema mexendo no
zoom factor direto. Se quiser explorar, leia um livro de 3D game
programming antes.

As vezes, mesmo zoomhack nao basta: parte da info pode estar no
estado interno ou ser dificil de absorver de relance. Para isso,
HUD.

## Mostrando dados ocultos com HUDs

Um *HUD* (heads-up display) e um ESP hack que mostra info critica
do game numa overlay. HUDs muitas vezes lembram a propria interface
do game (ammo, mini-map, vida, cooldowns). Geralmente exibem dados
historicos ou agregados, e sao mais usados em MMORPGs. Costumam ser
text-based, mas podem incluir sprites, formas e efeitos visuais
pequenos.

Os HUDs possiveis dependem dos dados disponiveis. Pontos comuns:

- Experiencia por hora (exp/h)
- Kills por hora (KPH)
- Damage por segundo (DPS)
- Gold lootado por hora (GPH)
- Healing por minuto
- Tempo estimado ate o proximo level
- Gold gasto em supplies
- Valor total de itens lootados

HUDs avancados exibem tabelas com itens lootados, supplies usadas,
kill count por tipo de criatura, lista de players vistos
recentemente.

Alem do que voce ja viu (ler memoria, hookar engine de graficos,
desenhar dados customizados), nao ha muito mais a aprender sobre
HUDs em si. Maioria dos games tem arquitetura simples o bastante
para colher info da memoria. Em cima, calculos basicos
(percentual, soma, taxa horaria) deixam o dado em formato usavel.

### Criando um Experience HUD

Imagine um HUD que mostra seu level atual, exp/h e tempo restante
para level up. Use Cheat Engine para achar variaveis de level e
exp atuais. Depois, com algoritmo do game ou tabela hardcoded,
calcule a exp necessaria para o proximo level.

Conhecida a exp, calcule sua exp/h. Em pseudocodigo:

```cpp
// assume tempo em milissegundos; para segundos, remova "1000 *"
timeUnitsPerHour      = 1000 * 60 * 60;
timePassed            = currentTime - startTime;
timePassedToHourRatio = timeUnitsPerHour / timePassed;       // (1)
expGained             = currentExp - startExp;               // (2)
hourlyExp             = expGained * timePassedToHourRatio;
remainingExp          = nextExp - currentExp;                // (3)
hoursToGo             = remainingExp / hourlyExp;            // (4)
```

`startExp` e `startTime` sao guardados no inicio do HUD.
`currentLevel` e `currentExp` sao o estado atual.

Calcule `hourlyExp` multiplicando o ratio (1) entre time units por
hora e o tempo passado pela exp ganha desde `startTime` (2). Como
a unidade e ms, multiplique por 1000.

Em (3), `remainingExp` e quanto falta. Em (4), divide pelo
`hourlyExp` para obter `hoursToGo`.

Para desenhar na tela, use o motor de hooking Direct3D do livro,
dentro do callback de `EndScene()`:

```cpp
hook->drawText(
    10, 10,
    D3DCOLOR_ARGB(255, 255, 0, 0),
    "Will reach level %d in %0.20f hours (%d exp per hour)",
    currentLevel, hoursToGo, hourlyExp);
```

Pronto, um HUD basico de tracking de exp. Variacoes dessas equacoes
servem para KPH, DPS, GPH e outras medidas time-based. Use
`drawText()` para qualquer info que voce localize e normalize. O
hook tambem traz `addSpriteImage()` e `drawSpriteImage()` para
imagens custom.

### Usando hooks para localizar dados

Memory reading nao e a unica fonte para HUD. Tambem da para
contar quantas vezes um modelo foi desenhado pelo
`DrawIndexedPrimitive()`, hookar funcoes internas do game que
desenham certos tipos de texto, ou interceptar funcoes que processam
pacotes do servidor. O caminho varia muito por jogo.

Por exemplo, um HUD que conta inimigos no map: use
fingerprinting de modelos (igual ao wallhack) para contar. Melhor
do que ler a lista de inimigos da memoria, ja que nao depende de
enderecos novos a cada patch.

Outro exemplo: lista de cooldowns inimigos. Intercepte os pacotes
incoming que dizem ao client quais spell effects exibir. Correlate
spells com inimigos por localizacao, tipo, etc., e rastreie quais
spells cada inimigo usou. Cruzando com base de dados de cooldowns,
mostra exatamente quando o spell volta a estar disponivel. Forte
porque a maioria dos games nao guarda cooldowns inimigos em
memoria.

## Visao geral de outros ESP hacks

Existem outros ESP hacks sem nome consagrado, especificos de
generos ou de games. Resumo rapido:

### Range hacks

Detectam quando modelos de campeoes/heroes sao desenhados (igual
wallhack) e desenham circulos no chao em volta. O raio do circulo
corresponde ao max attack range, mostrando areas onde voce pode
levar dano de cada inimigo.

### Loading-screen HUDs

Comum em MOBA/RTS com loading screen demorada. Aproveitam que esses
games costumam ter sites com estatisticas historicas dos players.
O bot consulta cada player, exibe overlay sobre a loading screen e
voce estuda os inimigos antes da partida.

### Pick-phase HUDs

Parente do anterior, mas exibido na fase de pick (antes do match,
quando todo mundo escolhe campeao). Mostra estatisticas dos
*aliados*, ajudando a avaliar pontos fortes/fracos e escolher
melhor seu personagem.

### Floor spy hacks

Comum em games top-down 2D antigos com andares ou plataformas. Se
voce esta no topo, quer saber o que ha em baixo antes de descer.
Modificam o valor do andar atual (`unsigned int` em geral) para
um andar diferente, deixando voce espionar.

Games costumam recalcular o andar a cada frame com base na posicao
do jogador, exigindo NOPs no codigo de recalculo para o valor nao
voltar. Achar o valor e o codigo a NOPar e similar ao zoom factor
(ver "Zoomhack via NOPing" antes).

## Fechando

ESP hacks sao caminhos poderosos para extrair info extra. Alguns
sao faceis via Direct3D hook ou edicao simples de memoria. Outros
exigem entender estruturas internas do game e hookar funcoes
proprietarias - bom uso para suas habilidades de reverse
engineering.

Para experimentar, estude e adapte o codigo deste capitulo. Para
ESP hacks mais especificos, vale procurar games e brincar.
