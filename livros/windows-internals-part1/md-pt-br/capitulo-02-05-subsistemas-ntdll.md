# Capítulo 2 — Subsistemas de Ambiente, Ntdll.dll e WSL

> Livro didático baseado em *Windows Internals, 7th Edition*.
> Atualizado para Windows 11 com exemplos práticos em C++ moderno.
> Repositório: [ring0-study](https://github.com/cnthigu/ring0-study)

> Navegação: [Anterior](capitulo-02-04-vbs-arquitetura.md) | [Índice Cap.2](capitulo-02.md) | [Próximo](capitulo-02-06-executive-kernel-hal.md)

---

## O que é um subsistema de ambiente?

O Windows foi projetado para suportar múltiplos "ambientes" de execução — diferentes APIs, diferentes comportamentos de sistema, diferentes convenções. A ideia era que a mesma base de kernel pudesse hospedar não só programas Win32, mas também programas POSIX, OS/2, e qualquer outro ambiente que alguém quisesse implementar.

Um **subsistema de ambiente** faz a ponte entre o ambiente externo (o que o programa "vê") e o kernel interno. Ele expõe a API que programas daquele ambiente esperam, e traduz as chamadas para os serviços nativos do kernel.

Na prática, hoje existe apenas um subsistema relevante: o **Windows subsystem**. POSIX (SUA) foi descontinuado no Windows 8 e OS/2 no Windows 2000. Mas o WSL introduziu um novo modelo — os Pico providers — que é o equivalente moderno.

---

## O Windows Subsystem: Csrss.exe e Win32k.sys

O subsistema Windows é composto de:

### Csrss.exe (Client/Server Runtime Subsystem)

Um processo por sessão de usuário. Carrega quatro DLLs:

| DLL | Responsabilidade |
|-----|-----------------|
| `csrsrv.dll` | Infraestrutura do subsistema (inicialização, conexões) |
| `basesrv.dll` | Tarefas de criação/destruição de processos e threads; mapeamento de drives DOS (A:, C:...) |
| `winsrv.dll` | Input de teclado/mouse; mensagens de janela; raw input thread; desktop thread |
| `sxssrv.dll` | Side-by-Side (SxS/Fusion) — manifests de versão de DLL |

`csrss.exe` é um **processo crítico** — se ele morrer, o sistema trava imediatamente (bugcheck `CRITICAL_PROCESS_DIED`). Isso é necessário porque o input de teclado, gerenciamento de janelas, e outras funções fundamentais da UI dependem dele.

Porém, a maior parte do subsistema Windows **não** passa pelo `csrss.exe`. A maioria das operações vai direto para o kernel via syscall. O `csrss.exe` só é envolvido para operações que requerem estado mantido pelo subsistema:
- Criação/encerramento de processos (para atualizar o estado interno do subsistema)
- Encerramente do Windows (ExitWindowsEx)
- Mapeamento de device letters (subst, net use)

### Win32k.sys

O módulo de kernel mode do subsistema Windows. Contém:

**Window Manager**: o coração da UI do Windows. Gerencia a hierarquia de janelas, processa mensagens (WM_PAINT, WM_KEYDOWN, etc.), gerencia o cursor, implementa o desktop. Esse é o código que "sabe" que existe uma janela — o kernel base não sabe nada sobre UI.

**GDI (Graphics Device Interface)**: funções de desenho 2D — linhas, texto, bitmaps, regiões. É a base sobre a qual toda UI Win32 é construída. GDI fala com drivers gráficos para renderizar na tela ou em impressoras.

> **Por que no kernel?** Colocar o window manager e GDI em kernel mode foi uma decisão controversa feita no Windows NT 4.0 (até o NT 3.51, rodavam em user mode). A razão foi performance: a UI intensiva de janelas envolvia tantas chamadas que o overhead de syscall para cada primitiva de desenho era inaceitável. Em kernel mode, a chamada vira uma chamada de função simples.
> A desvantagem: bugs no GDI ou window manager causam BSODs. E a superfície de ataque do kernel aumenta significativamente — muitas vulnerabilidades encontradas ao longo dos anos estavam em `win32k.sys`.

---

## Console: Conhost.exe e ConDrv.sys

Programas de console (cmd.exe, PowerShell, etc.) têm sua própria arquitetura:

```
cmd.exe (console app)
  │
  ├── abre /Device/ConDrv (via ConDrv.sys)
  │
  └── spawna Conhost.exe como processo filho
            │
            ├── carrega ConhostV2.dll
            │
            └── se comunica com ConDrv.sys para
                renderizar a janela de console
```

**ConDrv.sys**: driver de kernel mode que gerencia a comunicação entre o app de console e o Conhost. O app envia read/write I/Os para o ConDrv, não para o Csrss.

**Conhost.exe**: processo user mode que renderiza a janela do console, gerencia o scrollback buffer, e trata input do usuário. É um filho direto do app de console (não do Csrss como antigamente).

Esta arquitetura foi redesenhada no Windows 8 para eliminar gargalos: antes, input do teclado ia para win32k.sys → Csrss → Conhost → cmd.exe. Agora vai direto: ConDrv.sys → cmd.exe, com Conhost apenas para renderização.

O **Windows Terminal** (wt.exe) é uma implementação moderna de terminal que usa o mesmo ConDrv, mas com um renderizador muito mais avançado (GPU-accelerated, múltiplas abas, etc.).

---

## Ntdll.dll: a camada mais baixa acessível de user mode

`ntdll.dll` é a DLL mais importante do sistema — toda aplicação que roda no Windows a tem carregada no seu espaço de endereços. Ela contém:

### 1. System Service Dispatch Stubs

Funções que fazem a transição user mode → kernel mode. Toda função que começa com `Nt` ou `Zw` em ntdll é um "stub" que:

```asm
; NtCreateFile em ntdll.dll (x64, Windows 11)
NtCreateFile:
    mov r10, rcx          ; salva o 1o argumento (convenção de syscall)
    mov eax, 55h          ; número da syscall (0x55 = NtCreateFile neste Windows)
    test byte [SharedUserData+308h], 1  ; checar se devemos usar int 2Eh (Credential Guard)
    jnz usar_int_2E
    syscall               ; transição para kernel mode!
    ret
usar_int_2E:
    int 2Eh               ; alternativa via interrupção
    ret
```

O número `0x55` é o índice na **SSDT (System Service Descriptor Table)** — uma tabela no kernel que mapeia números de syscall para ponteiros de função.

> **Por que o número pode variar?** O número de syscall de `NtCreateFile` não é fixo — muda entre versões do Windows (e às vezes entre builds). Isso é propositalmente não-documentado. Código que hardcoda números de syscall quebra a cada atualização. Use sempre as funções de `ntdll.dll` em vez de fazer syscalls diretamente.

### 2. Image Loader (LDR...)

As funções `Ldr*` implementam o carregamento de executáveis e DLLs:
- `LdrLoadDll` — carrega uma DLL
- `LdrGetProcedureAddress` — resolve endereços de funções (a base do `GetProcAddress`)
- `LdrInitializeThunk` — ponto de entrada do loader para novos processos

O loader resolve dependências, aplica relocações, e prepara o processo para execução.

### 3. Heap Manager (Heap...)

A implementação do heap de baixo nível. `VirtualAlloc` é para páginas inteiras; o heap manager (acessível via `HeapAlloc`/`malloc`/`new`) gerencia blocos menores dentro de páginas já alocadas.

### 4. Runtime Library (Rtl...)

Funções utilitárias que o kernel e ntdll usam internamente, mas que também são úteis em user mode:
- String operations: `RtlCopyUnicodeString`, `RtlCompareString`
- `RtlGetVersion` — versão real do Windows (vimos no Cap. 1)
- `RtlInitializeCriticalSection` — seções críticas
- Exception handling infrastructure: `RtlRaiseException`, `RtlUnwind`
- Arithmetic: `RtlLargeIntegerAdd`, etc.

### 5. User-mode APC and Exception Dispatcher

Quando o kernel precisa notificar um thread de user mode (APC assíncrono, exception), ele injeta a execução em `ntdll!KiUserApcDispatcher` ou `ntdll!KiUserExceptionDispatcher`. Essas funções são o ponto de entrada de volta para user mode após eventos do kernel.

---

## Imagens nativas (Native Images)

Algumas aplicações linkam apenas contra ntdll.dll — sem kernel32.dll, sem subsistema Windows. São chamadas **native images** e rodam antes do subsistema Windows estar disponível.

Por que isso é necessário? Porque o Smss.exe (Session Manager) é o primeiro processo criado pelo kernel, e ele precisa rodar *antes* de qualquer subsistema estar carregado. Se dependesse da Win32, teria um problema circular: precisaria do Windows subsystem para rodar, mas o Windows subsystem só é lançado pelo Smss.

```
kernel (ntoskrnl.exe)
  └── cria Smss.exe [native image: só ntdll]
        └── inicia Csrss.exe [Windows subsystem]
              └── agora outros processos podem usar Win32 API
```

`autochk.exe` (verificação de disco no boot) também é uma native image pelo mesmo motivo — roda antes do sistema de arquivos estar totalmente disponível.

---

## Pico Providers e WSL

O modelo tradicional de subsistema tem um problema: requer que os executáveis sejam PE (Portable Executables) do Windows. Um binário Linux ELF não pode ser carregado pelo kernel Windows — e mesmo que pudesse, suas chamadas de sistema são incompatíveis.

A Microsoft criou os **Pico Providers** para resolver isso. Um Pico Provider é um driver de kernel mode que se registra via `PsRegisterPicoProvider` e recebe controle completo sobre processos "Pico" — processos especiais que o kernel trata de forma diferente:

```
Linux ELF binary (bash, grep, etc.)
  │ executa instruções
  │ faz syscall Linux (ex: write(1, "hello", 5))
  │
  ▼
Lxss.sys / Lxcore.sys [Pico Provider]
  recebe notificação: "processo Pico fez syscall 1 (write)"
  traduz para semântica Linux
  chama VFS Linux implementado no driver
  │
  ▼
Driver chama Win32/NT APIs para escrever no console
```

O Pico Provider recebe notificações para:
- Syscalls do processo Pico
- Exceções
- Faults de página
- APCs
- Criação/encerramento de thread

Com isso, o driver pode emular completamente um ambiente Linux sem tocar no executável ou recompilar.

O **WSL2** (Windows 11) vai além — usa uma VM Hyper-V leve com um kernel Linux real. O WSL1 usava Pico providers; WSL2 usa uma VM real para compatibilidade total com syscalls Linux:

```
WSL1: User process → Pico Provider (driver no kernel Windows) → tradução
WSL2: User process → Kernel Linux (em VM Hyper-V) → chamadas reais Linux
```

WSL2 tem melhor compatibilidade mas usa mais memória (precisa de uma VM); WSL1 era mais leve mas alguns programas quebravam em edge cases de syscall.

---

## Experimento: inspecionando syscalls com WinDbg

```
> windbg notepad.exe
(no console do WinDbg)
bp ntdll!NtCreateFile
g
(Notepad inicializa e para no breakpoint)
u ntdll!NtCreateFile
```

Você verá o stub de syscall. Note o número em `mov eax, Xh` — esse é o índice da syscall no SSDT. Tente o mesmo com `NtReadFile`, `NtWriteFile`, `NtOpenKey` (para acesso ao registro).

Compare os números entre versões do Windows — eles mudam. Isso é exatamente por que ferramentas como Mimikatz que hardcodam offsets de memória precisam ser atualizadas para cada versão do Windows.

---

*Próximo: [Executive, Kernel e HAL](capitulo-02-06-executive-kernel-hal.md)*
