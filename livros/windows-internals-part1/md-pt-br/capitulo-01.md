# Capítulo 1 — Conceitos e Ferramentas

> Livro didático baseado em *Windows Internals, 7th Edition* (Yosifovich, Ionescu, Russinovich, Solomon).
> Conteúdo reescrito e atualizado para Windows 11, com exemplos práticos em C++ moderno.

---

Este capítulo apresenta os blocos fundamentais do Windows que são referenciados em todos os capítulos seguintes. Não pule seções — cada uma constrói sobre a anterior.

## Seções

| # | Título | Conteúdo |
|---|--------|---------|
| 1 | [Introdução e Contexto](capitulo-01-01-introducao.md) | Por que estudar internals, Windows 11 como plataforma, OneCore, ambiente de estudo |
| 2 | [A Camada de APIs do Windows](capitulo-01-02-apis-windows.md) | Win32, ANSI/Unicode, Native API, COM, WinRT, .NET/CLR |
| 3 | [Processos e Jobs](capitulo-01-03-processos-jobs.md) | O que é um processo, handles, tokens, hierarquia, `CreateProcess`, Job Objects |
| 4 | [Threads](capitulo-01-04-threads.md) | Componentes de thread, contexto, TLS, `CreateThread`, Fibers, UMS, race conditions |
| 5 | [Memória Virtual e Modos do Processador](capitulo-01-05-memoria-virtual.md) | Espaço de endereços, paginação, user mode vs kernel mode, rings, DEP/NX, `VirtualAlloc` |
| 6 | [Hipervisor e VBS](capitulo-01-06-hipervisor-vbs.md) | Hyper-V, VTLs, HVCI, Credential Guard, HyperGuard, UEFI Secure Boot, TPM 2.0 |
| 7 | [Objetos, Handles e Segurança](capitulo-01-07-objetos-handles-seguranca.md) | Object Manager, handles, SIDs, tokens, DAC, MIC, AppContainers, Registro, Unicode |
| 8 | [Ferramentas Essenciais](capitulo-01-08-ferramentas.md) | Process Explorer, ProcMon, WinDbg Preview, AccessChk, Autoruns, Performance Monitor |

---

## Conceitos-chave do capítulo

Ao final deste capítulo, você deve ser capaz de responder:

- Por que o Windows tem múltiplas camadas de API (Win32, COM, WinRT, .NET)?
- Qual é a diferença fundamental entre um processo e um thread?
- O que é um handle, e por que não recebemos ponteiros diretos para objetos do kernel?
- Por que o espaço de endereços tem uma divisão user mode / kernel mode?
- O que o hipervisor garante que o kernel sozinho não consegue?
- Como SIDs, tokens, DACLs e níveis de integridade formam um modelo de segurança em camadas?

---

*[Início do capítulo →](capitulo-01-01-introducao.md)*
