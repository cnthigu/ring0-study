# Capítulo 7 — Segurança

> Livro didático baseado em *Windows Internals, 7th Edition* (Yosifovich, Ionescu, Russinovich, Solomon).
> Conteúdo reescrito e atualizado para Windows 11, com exemplos práticos em C++ moderno.

---

## Seções

| # | Título | Conteúdo |
|---|--------|---------|
| 1 | [Modelo de Segurança](capitulo-07-01-modelo-seguranca.md) | SRM, SIDs, ACL/ACE, DACL/SACL, Object Manager, impersonation, SDDL |
| 2 | [Tokens, Privilégios e UAC](capitulo-07-02-tokens-privilegios.md) | Access tokens, privilégios do sistema, UAC split token, tokens restritos, MIC em detalhe |
| 3 | [Logon e Autenticação](capitulo-07-03-logon-autenticacao.md) | Fluxo de logon, NTLM, Kerberos, SAM, Credential Guard, Windows Hello/FIDO2 |
| 4 | [Proteções Modernas no Windows 11](capitulo-07-04-protecoes-modernas.md) | PatchGuard, DSE, HVCI, AppContainers, CFG/CET, AMSI, Smart App Control, ETW |

---

## Conceitos-chave

Ao final deste capítulo, você deve ser capaz de responder:

- O que é um SID e por que ele é usado em vez do nome de usuário nos ACLs?
- Como o algoritmo SeAccessCheck avalia um DACL de cima para baixo?
- Qual a diferença entre um token primário e um token de impersonation?
- O que é o "split token" do UAC e como ele permite que admins rodem com menos privilégio?
- Por que `SeImpersonatePrivilege` é perigoso em serviços e como as "Potato" attacks o exploram?
- Como o protocolo Kerberos garante mutual authentication que o NTLM não garante?
- O que o Credential Guard faz de diferente que impede o ataque mimikatz clássico?
- O que é PatchGuard e por que ele tornou hooks de SSDT obsoletos?
- Como o HVCI impede o carregamento de código de kernel não assinado mesmo com kernel comprometido?
- O que é CFG e como ele difere de CET (hardware-based shadow stack)?
- Como o AMSI permite que antivírus escaneiem scripts PowerShell obfuscados?

---

*[Início do capítulo →](capitulo-07-01-modelo-seguranca.md)*
