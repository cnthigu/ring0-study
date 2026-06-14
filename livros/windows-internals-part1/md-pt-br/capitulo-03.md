# Capítulo 3 — Processos e Jobs

> Livro didático baseado em *Windows Internals, 7th Edition* (Yosifovich, Ionescu, Russinovich, Solomon).
> Conteúdo reescrito e atualizado para Windows 11, com exemplos práticos em C++ moderno.

---

## Seções

| # | Título | Conteúdo |
|---|--------|---------|
| 1 | [Criação de Processos](capitulo-03-01-criacao-processo.md) | CreateProcess family, argumentos, UWP, native/minimal/Pico processes |
| 2 | [Internos do Processo](capitulo-03-02-internos-processo.md) | EPROCESS, KPROCESS, PEB, TEB, estruturas paralelas (CSR_PROCESS, W32PROCESS) |
| 3 | [Processos Protegidos e PPL](capitulo-03-03-protected-processes.md) | Protected processes, PPL, Signers, ELAM, code signing requirements |
| 4 | [O Fluxo de CreateProcess](capitulo-03-04-flow-createprocess.md) | 6 estágios internos, PspAllocateProcess, loader lock, notificação do subsistema |
| 5 | [Image Loader e API Sets](capitulo-03-05-image-loader.md) | DLL search order, API Sets/OneCore, ASLR, relocações, SxS manifests, DLL hijacking |
| 6 | [Job Objects em Profundidade](capitulo-03-06-jobs.md) | Job internals, limites de CPU/memória, hierarquia de jobs, IOCP, sandboxing |

---

## Conceitos-chave

Ao final deste capítulo, você deve ser capaz de responder:

- Qual a diferença entre `CreateProcess`, `CreateProcessAsUser` e `CreateProcessWithTokenW`?
- O que é um minimal process, e por que o System process (PID 4) é um?
- Em que partes da memória vivem EPROCESS, KPROCESS e PEB?
- Como PPL e Signers impedem que até administradores acessem processos protegidos?
- Quais são os 6 estágios de criação de processo e o que acontece em cada um?
- Por que não se deve criar threads ou chamar LoadLibrary em DllMain?
- Como API Sets permitem que o mesmo código compile para PC, Xbox e IoT?
- Por que `JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE` é a forma correta de garantir cleanup de processos filhos?

---

*[Início do capítulo →](capitulo-03-01-criacao-processo.md)*
