# Windows Internals 7a Edicao - Traducao PT-BR

> Traducao nao oficial do livro *Windows Internals, Part 1, Seventh Edition*
> Pavel Yosifovich, Alex Ionescu, Mark E. Russinovich, David A. Solomon
> Microsoft Press, 2017 - ISBN: 978-0-7356-8418-8

## Sobre esta traducao

Traducao do livro **Windows Internals** para o **portugues brasileiro**, feita com auxilio de IA e revisada manualmente. Voltada para quem quer estudar os internos do Windows sem depender do ingles.

Cada capitulo inclui **exemplos de codigo C++ prontos para rodar no Visual Studio** que demonstram os conceitos na pratica - algo que a versao original nao tem.

Criterios de traducao:

- Todo texto narrativo e explicativo traduzido para PT-BR natural e fluente
- Termos tecnicos consagrados mantidos em ingles (kernel, thread, handle, driver, buffer, heap, stack, pool, etc.)
- Nomes de APIs, funcoes e estruturas mantidos em ingles (`CreateProcess()`, `EPROCESS`, `KTHREAD`)
- Codigo e pseudocodigo mantidos exatamente como no original
- Nomes de ferramentas mantidos em ingles (WinDbg, Process Explorer, ProcMon)
- Primeira ocorrencia de termos importantes: "termo em portugues (termo em ingles)"

## Indice

| Arquivo | Conteudo | Paginas orig. |
|---|---|---|
| [introducao.md](introducao.md) | Introducao | 14-18 |
| [capitulo-01.md](capitulo-01.md) | Capitulo 1 - Conceitos e Ferramentas | 19-61 |
| [capitulo-02.md](capitulo-02.md) | Capitulo 2 - Arquitetura do Sistema | 62-112 |
| [capitulo-03.md](capitulo-03.md) | Capitulo 3 - Processos e Jobs | 113-201 |
| [capitulo-04.md](capitulo-04.md) | Capitulo 4 - Threads | 202-314 |
| [capitulo-05.md](capitulo-05.md) | Capitulo 5 - Gerenciamento de Memoria | 315-499 |
| [capitulo-06.md](capitulo-06.md) | Capitulo 6 - Sistema de I/O | 500-624 |
| [capitulo-07.md](capitulo-07.md) | Capitulo 7 - Seguranca | 625-793 |

## Aviso de direitos autorais

Esta traducao e feita para fins educacionais e de estudo pessoal. O conteudo original e copyright 2017 de Pavel Yosifovich, Alex Ionescu, Mark E. Russinovich e David A. Solomon, publicado pela Microsoft Press. Para uso oficial ou comercial, adquira o livro original.

## Notas de estudo neste repositório

Há anotações em português e tabela *livro ↔ tópico* em [anotacoes/Windows-Internals/README.md](../../../anotacoes/Windows-Internals/README.md) (mesmo repositório `cpp-cnt-study`).

## Recursos relacionados

- [Livro original (Microsoft Press)](https://www.microsoftpressstore.com/store/windows-internals-part-1-system-architecture-processes-9780735684188)
- [Ferramentas Sysinternals](https://docs.microsoft.com/en-us/sysinternals/)
- [WinDbg](https://docs.microsoft.com/en-us/windows-hardware/drivers/debugger/)
- [Codigo-fonte das ferramentas do livro](https://github.com/zodiacon/windowsinternals)
- [Conteudo complementar](https://aka.ms/winint7ed/downloads)
