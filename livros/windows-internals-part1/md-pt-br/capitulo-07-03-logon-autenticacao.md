# Capítulo 7 — Logon e Autenticação

> Livro didático baseado em *Windows Internals, 7th Edition*.
> Atualizado para Windows 11 com exemplos práticos em C++ moderno.

> Navegação: [Anterior](capitulo-07-02-tokens-privilegios.md) | [Índice Cap.7](capitulo-07.md) | [Próximo](capitulo-07-04-protecoes-modernas.md)

---

## O processo de logon do Windows

Quando você digita usuário + senha na tela de login:

```
Sequência de logon interativo:

1. Winlogon.exe
   → GINA (Graphical Identification and Authentication) ou Credential Providers
   → LogonUI.exe coleta credenciais na "secure desktop" (desktop isolada)
   → Secure Desktop: área de desktop onde somente SYSTEM e processos confiáveis podem desenhar
     (impede que malware capture keystrokes ou apresente falsa tela de login)

2. Winlogon chama LsaLogonUser() no Lsass
   → Envia credenciais para o LSA (Local Security Authority)

3. Lsass seleciona o pacote de autenticação correto:
   → Kerberos (domínio): verifica com o Domain Controller
   → NTLM (local ou sem DC): verifica contra SAM local
   → Outros pacotes registrados (smartcard, biometria, etc.)

4. Pacote autentica e retorna:
   → User SID, Group SIDs, Privileges
   → LogonId único para esta sessão de logon

5. Lsass cria o Access Token com base nessas informações
   → Dois tokens se for admin (UAC split token)

6. Lsass retorna o token para Winlogon

7. Winlogon chama CreateProcessAsUser com o token do usuário
   → Cria userinit.exe → que cria explorer.exe (shell do usuário)
```

---

## NTLM: autenticação por hash

**NTLM (NT LAN Manager)** é o protocolo de autenticação legado do Windows, ainda amplamente usado para autenticação local e em redes sem Kerberos:

```
Protocolo NTLM Challenge-Response:

  Cliente                          Servidor
    │                                 │
    │──── NEGOTIATE_MESSAGE ─────────►│ (versão NTLM suportada)
    │                                 │
    │◄─── CHALLENGE_MESSAGE ──────────│ (challenge de 8 bytes aleatórios)
    │                                 │
    │  Cliente computa:               │
    │  NT Hash = MD4(UTF16LE(senha))  │
    │  Response = HMAC-MD5(NT Hash,   │
    │             challenge + outros) │
    │                                 │
    │──── AUTHENTICATE_MESSAGE ──────►│ (username + domínio + response)
    │                                 │
    │                    Servidor verifica response:
    │                    - Envia para DC (autenticação de domínio)
    │                    - Ou verifica contra SAM local (local)
    │                                 │
    │◄─── Success/Failure ────────────│
```

**Vulnerabilidades NTLM**:
- NT Hash pode ser extraído do SAM com Admin → Pass-the-Hash attack
- Sem mutual authentication → relay attacks (NTLM relay)
- Windows 11 pode desabilitar NTLM via Group Policy para mitigar

---

## Kerberos: autenticação de domínio

Em ambientes de domínio Active Directory, **Kerberos** é o protocolo padrão:

```
Protocolo Kerberos (simplificado):

  Cliente                  KDC (Key Distribution Center - Domain Controller)
    │                                    │
    │──── AS-REQ (Authentication) ──────►│ (solicitar TGT)
    │     (pré-autenticação com hash)    │
    │                                    │  Verificar credenciais
    │◄─── AS-REP (TGT) ──────────────────│
    │     Ticket Granting Ticket         │  Criptografado com chave do KDC
    │     (válido por 10 horas padrão)   │
    │                                    │
    │  Agora pode acessar serviços:      │
    │──── TGS-REQ ───────────────────────►│ (solicitar ticket para Serviço X)
    │     Apresenta TGT                  │
    │◄─── TGS-REP (Service Ticket) ──────│ (criptografado com chave do serviço)
    │                                    │
    │                          Servidor de Serviço
    │                                    │
    │──── AP-REQ ────────────────────────►│ (apresentar service ticket)
    │                                    │  Descriptografar com sua chave
    │◄─── AP-REP ─────────────────────────│ (mutual authentication opcional)
```

**Vantagens sobre NTLM**:
- **Mutual authentication**: servidor prova identidade para o cliente também
- **Delegation**: cliente pode delegar credenciais para o servidor acessar recursos em seu nome (Kerberos Constrained Delegation)
- **Ticket-based**: senha nunca é transmitida pela rede
- **Single Sign-On**: com TGT válido, acesso transparente a todos os serviços do domínio

---

## LSA Secrets e credenciais em memória

O Lsass mantém credenciais em memória para Single Sign-On:

```
Credenciais no Lsass (por pacote):
  msv1_0.dll:
    → NT Hash do logon interativo (NTLM authentication)
  
  kerberos.dll:
    → TGT e tickets Kerberos
    → Necessário para SSO sem reautenticar
  
  wdigest.dll (DESABILITADO por padrão no Windows 8.1+):
    → Senha em texto claro (!!) — necessário para autenticação HTTP Digest
    → Razão histórica do ataque "mimikatz sekurlsa::logonpasswords"

Defesas:
  Credential Guard (Hyper-V VBS): move o armazenamento de credenciais para
  o VTL1 (Isolated User Mode) → mimikatz não consegue extrair do Lsass
  porque Lsass em VTL0 não tem as credenciais, só uma referência ao VTL1.
```

---

## SAM: Security Account Manager

Para contas locais, o SAM armazena os hashes de senha:

```
HKLM\SAM (chave de registro protegida):
  → Inacessível mesmo para administradores via WinAPI normal
  → Requer SeBackupPrivilege ou raw disk access para ler
  → Hashes criptografados com SysKey (DPAPI do sistema)

Formato do hash NT:
  NT Hash = MD4(UTF16LE(senha))
  → MD4 é rápido → brute force possível para senhas curtas
  → Recomendação: Windows Hello + FIDO2 para eliminar senhas tradicionais

Atacar o SAM:
  1. Extrair SYSTEM hive + SAM hive (via Volume Shadow Copy ou offline)
  2. Usar Impacket/mimikatz para decifrar com o SysKey do SYSTEM hive
  3. Crack offline com hashcat (mas senhas longas e complexas resistem)
```

---

## Logon Types: tipos de autenticação

O token criado no logon varia de acordo com o **Logon Type**:

```
Logon Types (SECURITY_LOGON_TYPE):

Tipo 2 - Interactive:
  → Login na tela física (teclado local)
  → Credenciais cacheadas localmente (domain logon cached credentials)
  → Token com NetworkCleartext para SSO

Tipo 3 - Network:
  → Acesso via rede (\\servidor\compartilhamento)
  → Token sem credenciais (não pode re-autenticar para outros serviços)

Tipo 4 - Batch:
  → Tarefas agendadas (Task Scheduler)

Tipo 5 - Service:
  → Serviços do Windows rodando sob uma conta

Tipo 7 - Unlock:
  → Desbloqueio de workstation

Tipo 8 - NetworkCleartext:
  → Autenticação com senha em claro via rede (Basic Auth)

Tipo 9 - NewCredentials:
  → RunAs /netonly: credenciais de rede diferente mas herda token local

Tipo 10 - RemoteInteractive:
  → RDP (Remote Desktop Protocol)

Tipo 11 - CachedInteractive:
  → Login com credenciais cacheadas (sem acesso ao DC)
```

---

## Logon Interativo via API

```cpp
// Fazer logon programático:
HANDLE hToken;
BOOL ok = LogonUserW(
    L"usuario",      // nome de usuário
    L"DOMINIO",      // domínio (ou "." para máquina local)
    L"senha",        // senha
    LOGON32_LOGON_INTERACTIVE,    // tipo de logon
    LOGON32_PROVIDER_DEFAULT,     // provedor de autenticação
    &hToken);         // token resultante

if (ok) {
    // Criar processo como este usuário:
    STARTUPINFOW si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    CreateProcessAsUserW(hToken, L"C:\\Windows\\System32\\cmd.exe",
        nullptr, nullptr, nullptr, FALSE, CREATE_NEW_CONSOLE,
        nullptr, nullptr, &si, &pi);
    
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    CloseHandle(hToken);
}

// Ou impersonar o token:
ImpersonateLoggedOnUser(hToken);
// ... executar como o usuário autenticado ...
RevertToSelf();
```

---

## Windows Hello e FIDO2 (Windows 11)

O Windows 11 promove autenticação **sem senha** via:

```
Windows Hello:
  → Biometria (face, impressão digital) ou PIN
  → PIN é LOCAL ao dispositivo — não é transmitido pela rede
  → Protegido por TPM 2.0 (chip de segurança físico)
  → Cria par de chaves assimétrico por serviço/conta
  → Autenticação: desafio criptográfico com chave privada no TPM

FIDO2 / Passkeys:
  → Padrão aberto (W3C WebAuthn + CTAP2)
  → Chave privada no dispositivo (TPM, YubiKey, etc.)
  → Chave pública registrada no serviço
  → Phishing-resistant: chave vinculada ao domínio do site
  → Nenhuma senha para vazar

Diferença de segurança:
  Senha: se o servidor é comprometido → senha vaza
  FIDO2: se o servidor é comprometido → apenas chave pública vaza (inútil)
```

---

*Próximo: [Proteções de Segurança Modernas](capitulo-07-04-protecoes-modernas.md)*
