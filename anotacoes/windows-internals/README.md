# Windows Internals (anotações)

Anotações pessoais de estudo sobre Windows Internals e APIs Win32. Figuras de apoio ficam em [`_img/`](./_img/) (caminhos relativos nos artigos).

**Convenção de pastas (repo `cpp-cnt-study`):** nomes em **kebab-case**, minúsculas, sem espaços: `windows-internals`, `intro`, `arquitetura-sistema`, `processos-jobs`, `processos` (artigos), `threads`, `codes/processos-jobs`, `source-codes` (apontadores a trechos no Visual Studio). Os ficheiros `.md` dos artigos mantêm os títulos originais (às vezes com espaços).

**Nota:** estes tópicos não cobrem 100% do livro Windows Internals nem do material oficial da Microsoft.

## Livro em português (mesmo repositório)

A tradução em Markdown fica em [`livros/windows-internals-part1/md-pt-br/`](../../livros/windows-internals-part1/md-pt-br/README.md) (índice com capítulos 1 a 7). Use os links abaixo para abrir o capítulo certo e, na sequência, a anotação correspondente.

| Ordem | Capítulo (PT-BR) | Foco e anotações locais |
| --- | --- | --- |
| 0 | [Introdução do livro](../../livros/windows-internals-part1/md-pt-br/introducao.md) | Panorama; depois alinhe com a pasta [intro](./intro/) |
| 1 | [Capítulo 1 – Conceitos e ferramentas](../../livros/windows-internals-part1/md-pt-br/capitulo-01.md) | [intro](./intro/) (inclui [Dependency Walker](./intro/Dependency%20Walker.md)) |
| 2 | [Capítulo 2 – Arquitetura do sistema](../../livros/windows-internals-part1/md-pt-br/capitulo-02.md) | [System Service Descriptor Table (SSDT)](./arquitetura-sistema/System%20Service%20Descriptor%20Table.md) |
| 3 | [Capítulo 3 – Processos e jobs](../../livros/windows-internals-part1/md-pt-br/capitulo-03.md) | [Processos e jobs](./processos-jobs/): [índice de processos](./processos-jobs/processos/Readme.md) |
| 4 | [Capítulo 4 – Threads](../../livros/windows-internals-part1/md-pt-br/capitulo-04.md) | [threads](./threads/): [noções básicas](./threads/1.%20Basics%20Of%20Threads.md) |
| 5–7 | [Cap. 5 – Memória](../../livros/windows-internals-part1/md-pt-br/capitulo-05.md), [Cap. 6 – I/O](../../livros/windows-internals-part1/md-pt-br/capitulo-06.md), [Cap. 7 – Segurança](../../livros/windows-internals-part1/md-pt-br/capitulo-07.md) | Ainda sem pasta dedicada aqui: leia o `.md` do livro e, quando criar anotações, encaixe na tabela acima. |

**Códigos de apoio:** experimentos e snippets alinhados aos textos estão em [`codes/`](./codes/).

## Lógica de estudo (sugestão)

1. **Ler o capítulo no livro** (`.md` em `md-pt-br`) na ordem da tabela: você ganha a narrativa completa e os EXPERIMENTO do original.
2. **Abrir a anotação da mesma linha** para fixar o que você quis extrair, diagramas, e links extras.
3. **Rodar ou inspecionar código** em [`codes/`](./codes/) quando o assunto tiver snippet no repositório (processos, Toolhelp, etc.).
4. **Revisar** com ferramentas citadas (Sysinternals, WinDbg) conforme o capítulo 1 e as notas de ferramentas.

Essa ordem evita pular base (conceitos, ferramentas) antes de arquitetura, processos e threads.

## Índice das pastas

1. [intro](./intro/) (ferramentas, Dependency Walker, alinhado ao cap. 1)
2. [arquitetura-sistema](./arquitetura-sistema/) (ex.: SSDT, cap. 2)
3. [processos-jobs](./processos-jobs/)
4. [codes](./codes/) (C++ alinhado aos artigos)
5. [threads](./threads/)

## Recursos oficiais (fora do repositório)

- [Windows Internals (Microsoft / recurso de apoio)](https://learn.microsoft.com/en-us/sysinternals/resources/windows-internals)

## Observações

1. Outras fontes técnicas (documentação, blogs, repositórios) são citadas nos textos quando relevantes.
2. Cada anexo costuma listar recursos no final.
