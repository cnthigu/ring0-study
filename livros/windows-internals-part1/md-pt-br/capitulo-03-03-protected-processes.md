# Capítulo 3 — Processos Protegidos e PPL

> Livro didático baseado em *Windows Internals, 7th Edition*.
> Atualizado para Windows 11 com exemplos práticos em C++ moderno.

> Navegação: [Anterior](capitulo-03-02-internos-processo.md) | [Índice Cap.3](capitulo-03.md) | [Próximo](capitulo-03-04-flow-createprocess.md)

---

## O problema com o modelo tradicional

No modelo de segurança padrão do Windows, qualquer processo com `SeDebugPrivilege` pode:
- Abrir um handle para qualquer processo com `PROCESS_ALL_ACCESS`
- Ler e escrever na memória desse processo
- Injetar threads, modificar código, terminar o processo

Isso funciona bem para desenvolvedores e administradores, mas cria um problema sério para conteúdo DRM (Digital Rights Management): o player de mídia que decodifica um Blu-ray em HD é completamente vulnerável — qualquer aplicação elevada pode extrair o conteúdo deprotegido da memória.

A solução foi os **processos protegidos** — processos que o kernel protege contra acesso mesmo de processos com SeDebugPrivilege.

---

## Processos Protegidos Clássicos

Introduzidos no Windows Vista para DRM. Um processo protegido clássico:
- Só aceita handles com máscaras de acesso muito limitadas (mesmo de processos com SeDebugPrivilege)
- Só pode carregar DLLs assinadas com um certificado especial da Microsoft
- O `Protected Media Path (PMP)` usa isso para reprodução de conteúdo premium

```
kd> dt nt!_EPROCESS <addr> Protection
   +0x87a Protection : _PS_PROTECTION
     +0x000 Level : 0x61 ''
       Type  : 0y00000001 (0x1)  → ProtectedLight
       Audit : 0y0
       Signer: 0y00110   (0x6) → WinTcb
```

Quando um processo tenta abrir um handle para um processo protegido, o kernel verifica se o abridor também é protegido e com nível de proteção ≥ ao alvo. Se não for, o acesso é negado mesmo com SeDebugPrivilege.

```cpp
// Tentar abrir um processo PPL sem ser PPL — vai falhar
HANDLE h = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pidDeProcessoPPL);
// GetLastError() == ERROR_ACCESS_DENIED mesmo sendo admin/debug privilege
```

---

## Protected Process Light (PPL)

O Windows 8.1 generalizou o modelo para usos além do DRM com o **PPL (Protected Process Light)**. O campo `Protection` em EPROCESS tem dois sub-campos:

- **Type**: nível de proteção (0=None, 1=PPL, 2=Protected)
- **Signer**: quem pode ser o assinante do binário

Tabela de Signers e níveis de acesso:

| Signer | Valor | Quem usa |
|--------|-------|---------|
| `None` | 0 | Processo normal |
| `Authenticode` | 1 | PPL via certificado de terceiros (ELAM) |
| `CodeGen` | 2 | .NET native (NGEN) |
| `Antimalware` | 3 | Antivírus com ELAM driver |
| `Lsa` | 4 | LSASS com Credential Guard |
| `Windows` | 5 | Componentes do OS |
| `WinTcb` | 6 | Componentes críticos (TCB = Trusted Computer Base) |

Hierarquia de proteção (mais alto protege mais):
```
WinTcb Protected > WinTcb PPL > Windows Protected > Windows PPL > Antimalware PPL > ...
```

Um processo só pode abrir handle para outro processo se seu Signer for ≥ ao do alvo.

### PPL na prática

Processos que rodam como PPL no Windows 11:
- `lsass.exe` — Signer=Lsa, Type=PPL (se Credential Guard ativo)
- `sppsvc.exe` — Software Protection Platform, Signer=Windows
- `MsMpEng.exe` — Windows Defender, Signer=Antimalware
- `svchost.exe` (alguns) — Signer=Windows

```powershell
# Listando proteção de todos os processos (requer Sysinternals AccessChk ou Process Explorer)
# No Process Explorer: coluna "Protection" (View → Select Columns)
# Via PowerShell com Get-Process não é possível diretamente — usa kernel driver

# Verificar via WinDbg:
# kd> !process 0 0
# Cada processo mostra "Protected=1" se PPL
```

---

## Requisitos para ser PPL: assinatura de código

Para ser executado como PPL, um binário precisa:

1. **Assinatura com certificado adequado**: o nível de proteção é determinado pelo EKU (Extended Key Usage) presente no certificado de assinatura:
   - `1.3.6.1.4.1.311.10.3.22` → Windows System Component Verification → Signer=Windows
   - `1.3.6.1.4.1.311.10.3.24` → Protected Process Light Verification → Signer=Antimalware (para ELAM)

2. **Verificação pela Code Integrity**: o `ci.dll` verifica a assinatura no momento de carregamento. Se inválida, o carregamento é bloqueado.

3. **Lista mínima TCB**: processos com Signer=WinTcb devem aparecer em uma lista hardcoded no kernel (`MiniPVtableList`). Você não pode criar um binário WinTcb — só a Microsoft pode.

### Suporte a PPL de terceiros via ELAM

O ELAM (Early Launch Anti-Malware) permite que fornecedores de antivírus qualificados tenham seus drivers carregados muito cedo no boot (antes de outros drivers de terceiros) e signem seus processos de proteção como PPL com Signer=Antimalware.

Para obter um certificado ELAM:
1. Participar do programa WDSI (Windows Defender Security Intelligence)
2. Submeter o driver para certificação WHQL
3. A Microsoft emite um certificado ELAM com EKUs específicas

---

## O que PPL realmente protege

| Ação | Processo normal (Admin) | Processo PPL |
|------|------------------------|--------------|
| Ler memória de lsass.exe (PPL) | Bloqueado | Só se Signer ≥ Lsa |
| Terminar processo PPL | Bloqueado | Só se Signer ≥ do alvo |
| Injetar thread | Bloqueado | Bloqueado |
| Abrir com PROCESS_ALL_ACCESS | Bloqueado | Depende do Signer |

O kernel verifica essas regras em `PsGrantedAccess` durante `NtOpenProcess`. Mesmo um processo SYSTEM não pode abrir handles privilegiados para um processo protegido com Signer superior.

### Limitações do PPL

PPL protege contra ataques de user mode. **Não protege contra**:
- Ataques de kernel mode (drivers maliciosos)
- Ataques via Hyper-V ou VTL 1
- Dump de memória via debug de kernel (WinDbg em modo kernel)

Com **VBS + Credential Guard**, o lsaiso.exe em VTL 1 oferece proteção muito mais forte — não basta comprometer o kernel.

---

## Inspecionando proteção com ferramentas

```
# Process Explorer: coluna "Protection" mostra o nível PPL
# Cor roxa: processo PPL ou Protected

# WinDbg kernel debug:
kd> dt nt!_PS_PROTECTION
   +0x000 Level : UChar
     Type  (bits 2:0) 
     Audit (bit 3)
     Signer (bits 7:4)

kd> !process 0 0
# Procure "Protected=1" ou o campo Protection na listagem de processos

# AccessChk (Sysinternals):
accesschk -p -l   ← lista processos com suas proteções
```

---

*Próximo: [O Fluxo de CreateProcess](capitulo-03-04-flow-createprocess.md)*
