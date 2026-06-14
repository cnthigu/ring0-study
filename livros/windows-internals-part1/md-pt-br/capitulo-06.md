# Capítulo 6 — Sistema de I/O

> Livro didático baseado em *Windows Internals, 7th Edition* (Yosifovich, Ionescu, Russinovich, Solomon).
> Conteúdo reescrito e atualizado para Windows 11, com exemplos práticos em C++ moderno.

---

## Seções

| # | Título | Conteúdo |
|---|--------|---------|
| 1 | [O I/O Manager e o Modelo WDM](capitulo-06-01-io-manager.md) | I/O Manager, Driver/Device/File Objects, pilha de drivers, DeviceIoControl, WDM/KMDF/UMDF |
| 2 | [IRPs: I/O Request Packets](capitulo-06-02-irp.md) | Estrutura do IRP, IO_STACK_LOCATION, ciclo de vida, major functions, completion routines, filter drivers |
| 3 | [Plug and Play e Power Management](capitulo-06-03-pnp-power.md) | PnP Manager, ciclo de vida de dispositivos, IRP_MJ_PNP, D-states/S-states, Modern Standby |
| 4 | [I/O Assíncrono e Completion Ports](capitulo-06-04-io-async.md) | OVERLAPPED, IOCP, PostQueuedCompletionStatus, cancelamento de I/O, http.sys |

---

## Conceitos-chave

Ao final deste capítulo, você deve ser capaz de responder:

- Qual a diferença entre Driver Object, Device Object e File Object?
- O que é um IRP e como ele "desce" e "sobe" pela pilha de drivers?
- Por que um driver deve sempre tratar `IRP_MN_REMOVE_DEVICE` corretamente?
- O que são Completion Routines e quando elas são chamadas?
- Qual a diferença entre `METHOD_BUFFERED`, `METHOD_DIRECT` e `METHOD_NEITHER` em IOCTLs?
- Por que IOCP escala melhor que "uma thread por conexão" para servidores?
- Como `PostQueuedCompletionStatus` permite trabalho customizado no pool de threads IOCP?
- O que é `http.sys` e por que ter um servidor HTTP no kernel faz sentido?
- Qual a diferença entre D3 Hot e D3 Cold no gerenciamento de energia?
- Em que situação `CancelIoEx` é necessário vs. `CancelIo`?

---

*[Início do capítulo →](capitulo-06-01-io-manager.md)*
