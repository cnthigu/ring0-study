# Windows Internals — Livro Didático em PT-BR

> Livro didático baseado em *Windows Internals, Part 1, 7th Edition*
> (Yosifovich, Ionescu, Russinovich, Solomon — Microsoft Press, 2017)
>
> Conteúdo reescrito do zero em português brasileiro, atualizado para **Windows 11**,
> com exemplos C++ modernos compiláveis, diagramas ASCII e explicações de professor.

## Sobre este livro

Este não é uma tradução direta — é um **livro didático original** que usa o Windows Internals
como referência e reescreve cada conceito de forma explicativa, com:

- Exemplos de código C++ modernos (C++17) prontos para compilar no MSVC
- Diagramas ASCII das estruturas internas
- Comandos WinDbg para exploração hands-on
- Contextualização para Windows 11 (build 22000+)
- Perspectiva de professor: conceitos explicados do zero com progressão lógica

## Índice

| # | Arquivo | Conteúdo | Seções |
|---|---------|---------|--------|
| 1 | [capitulo-01.md](capitulo-01.md) | Conceitos e Ferramentas | Introdução, APIs Windows, Processos, Threads, Memória, Hypervisor/VBS, Objetos/Segurança, Ferramentas |
| 2 | [capitulo-02.md](capitulo-02.md) | Arquitetura do Sistema | Design Goals, Arquitetura Overview, SMP/NUMA, VBS, Subsistemas/ntdll, Executive/HAL, Processos do Sistema |
| 3 | [capitulo-03.md](capitulo-03.md) | Processos e Jobs | Criação, Internos (EPROCESS/PEB), Protected Processes/PPL, Fluxo de CreateProcess, Image Loader/API Sets, Job Objects |
| 4 | [capitulo-04.md](capitulo-04.md) | Threads | ETHREAD/KTHREAD/TEB, Scheduling/Prioridades, APCs/DPCs/IRQL, Wait States/Sincronização |
| 5 | [capitulo-05.md](capitulo-05.md) | Gerenciamento de Memória | Memória Virtual x64, VirtualAlloc/Heaps, Memória Compartilhada/Seções, Working Sets/Paginação |
| 6 | [capitulo-06.md](capitulo-06.md) | Sistema de I/O | I/O Manager/WDM, IRPs em Detalhe, Plug and Play/Power, I/O Assíncrono/IOCP |
| 7 | [capitulo-07.md](capitulo-07.md) | Segurança | Modelo de Segurança/SRM, Tokens/Privilégios/UAC, Logon/Autenticação, Proteções Modernas |

## Referência original

Pavel Yosifovich, Alex Ionescu, Mark E. Russinovich, David A. Solomon
*Windows Internals, Part 1, 7th Edition* — Microsoft Press, 2017
ISBN: 978-0-7356-8418-8

## Recursos relacionados

- [Ferramentas Sysinternals](https://docs.microsoft.com/en-us/sysinternals/)
- [WinDbg Preview](https://aka.ms/windbg/download)
- [Código-fonte das ferramentas do livro](https://github.com/zodiacon/windowsinternals)
- [Conteúdo complementar oficial](https://aka.ms/winint7ed/downloads)
