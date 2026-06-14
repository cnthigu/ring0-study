# Capítulo 5 — Gerenciamento de Memória

> Livro didático baseado em *Windows Internals, 7th Edition* (Yosifovich, Ionescu, Russinovich, Solomon).
> Conteúdo reescrito e atualizado para Windows 11, com exemplos práticos em C++ moderno.

---

## Seções

| # | Título | Conteúdo |
|---|--------|---------|
| 1 | [Visão Geral: Memória Virtual](capitulo-05-01-virtual-memory.md) | Layout do espaço de endereços x64, memória virtual vs. física, page tables, VAD, estados de página, kernel pools |
| 2 | [Alocação: VirtualAlloc, Heaps e Large Pages](capitulo-05-02-alocacao.md) | VirtualAlloc, granularidade, Heap Manager, Segment Heap, Large Pages, AWE, medição de memória |
| 3 | [Memória Compartilhada e Seções](capitulo-05-03-shared-memory.md) | Section Objects, file mapping, shared memory IPC, Copy-on-Write, DLLs compartilhadas, segurança |
| 4 | [Working Sets, Paginação e Memória Física](capitulo-05-04-working-sets.md) | PFN Database, working set trimming, Memory Compression, page file, VirtualLock, NUMA, diagnóstico |

---

## Conceitos-chave

Ao final deste capítulo, você deve ser capaz de responder:

- Por que reservar memória com `MEM_RESERVE` não usa RAM?
- O que é um page fault e quais são seus tipos (hard, soft, CoW)?
- Por que 100 processos carregando `ntdll.dll` não usam 100× a RAM da DLL?
- O que é Copy-on-Write e quando ele é disparado?
- Como o Segment Heap difere do NT Heap clássico?
- O que é o PFN Database e quais listas ele mantém?
- Como o Memory Compression do Windows 10+ difere do paging clássico?
- Quando Large Pages valem a pena e o que as impede (requer `SeLockMemoryPrivilege`)?
- Qual a diferença entre `Working Set`, `Private Bytes` e `Virtual Size` no Task Manager?
- Como diagnosticar paginação excessiva com Performance Monitor?

---

*[Início do capítulo →](capitulo-05-01-virtual-memory.md)*
