# Capítulo 4 — Threads

> Livro didático baseado em *Windows Internals, 7th Edition* (Yosifovich, Ionescu, Russinovich, Solomon).
> Conteúdo reescrito e atualizado para Windows 11, com exemplos práticos em C++ moderno.

---

## Seções

| # | Título | Conteúdo |
|---|--------|---------|
| 1 | [Internos de Thread: ETHREAD, KTHREAD e TEB](capitulo-04-01-internos-thread.md) | Estrutura ETHREAD/KTHREAD, estados de thread, TEB, CreateThread, CreateRemoteThread, guard pages |
| 2 | [Scheduling: Prioridades e Quantum](capitulo-04-02-scheduling.md) | 32 níveis de prioridade, quantum, boosts dinâmicos, ready queues, context switch, NUMA scheduling |
| 3 | [APCs, DPCs e IRQL](capitulo-04-03-apcs-dpcs.md) | IRQL hierarchy, APCs de kernel/user mode, DPCs, ISR → DPC pipeline, diagnóstico |
| 4 | [Wait States e Sincronização](capitulo-04-04-wait-sync.md) | Dispatcher objects, events, mutex, semáforo, CRITICAL_SECTION, SRW Lock, Condition Variables, deadlocks |

---

## Conceitos-chave

Ao final deste capítulo, você deve ser capaz de responder:

- O que é KTHREAD e como ele se relaciona com ETHREAD?
- Quais campos do TEB são acessíveis sem syscall, e por que `GetCurrentThreadId()` é rápido?
- Como o scheduler decide qual thread executar a seguir?
- O que é um boost de prioridade dinâmico e em que situações ele ocorre?
- Qual a diferença entre APC de kernel mode e APC de user mode?
- Por que código em IRQL >= DISPATCH_LEVEL não pode acessar memória paginável?
- Em que situação um DPC é usado em vez de processar diretamente na ISR?
- Qual a diferença entre evento manual-reset e auto-reset?
- Por que `CRITICAL_SECTION` é mais rápida que um mutex de kernel?
- Como diagnosticar um deadlock com WinDbg ou Resource Monitor?

---

*[Início do capítulo →](capitulo-04-01-internos-thread.md)*
