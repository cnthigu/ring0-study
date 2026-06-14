# Capítulo 2 — Arquitetura do Sistema

> Livro didático baseado em *Windows Internals, 7th Edition* (Yosifovich, Ionescu, Russinovich, Solomon).
> Conteúdo reescrito e atualizado para Windows 11, com exemplos práticos em C++ moderno.

---

Este capítulo desmonta a arquitetura interna do Windows — como os componentes se conectam, por que foram projetados dessa forma, e o que isso significa na prática para quem escreve código, drivers ou ferramentas de segurança.

## Seções

| # | Título | Conteúdo |
|---|--------|---------|
| 1 | [Objetivos de Design e Modelo do Sistema](capitulo-02-01-design-goals.md) | Requisitos de 1989, metas de design, monolítico vs microkernel, por que C |
| 2 | [Visão Geral da Arquitetura](capitulo-02-02-arquitetura-overview.md) | Tipos de processos, componentes kernel mode, a cadeia de chamadas user→kernel |
| 3 | [Portabilidade, SMP e Escalabilidade](capitulo-02-03-portabilidade-smp.md) | HAL, multicore, SMT/Hyper-Threading, NUMA, Processor Groups, client vs server |
| 4 | [VBS: Arquitetura de Segurança](capitulo-02-04-vbs-arquitetura.md) | Hyper-V, VTLs, SLAT, I/O MMU, Secure Kernel, Trustlets, IUM, cadeia de boot |
| 5 | [Subsistemas, Ntdll.dll e WSL](capitulo-02-05-subsistemas-ntdll.md) | Csrss.exe, Win32k.sys, Console (Conhost/ConDrv), Ntdll internos, Pico Providers, WSL1 vs WSL2 |
| 6 | [Executive, Kernel, HAL e Drivers](capitulo-02-06-executive-kernel-hal.md) | Prefixos e componentes do Executive, KPCR/KPRCB, HAL extensions, WDM/WDF, Universal Drivers |
| 7 | [Processos de Sistema](capitulo-02-07-processos-sistema.md) | Idle, System, Smss, Csrss, Wininit, Winlogon, Services/Svchost, Lsass, Explorer |

---

## Conceitos-chave do capítulo

Ao final deste capítulo, você deve ser capaz de responder:

- Por que o Windows é monolítico, e quais mecanismos compensam as desvantagens?
- Qual é a diferença entre o Executive e o Kernel (ambos em ntoskrnl.exe)?
- Como o HAL permite que o mesmo ntoskrnl.exe rode em hardware diferente?
- O que é um Pico Provider e como o WSL usa um para rodar Linux sem recompilação?
- Como funciona a cadeia de boot com VBS: quem carrega quem, e que confiança cada componente tem?
- Por que o lsass.exe não contém credenciais quando Credential Guard está ativo?
- O que diferencia operacionalmente o Windows 11 Home do Windows Server 2022?

---

*[Início do capítulo →](capitulo-02-01-design-goals.md)*
