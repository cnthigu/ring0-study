# Capítulo 3 — O Fluxo Interno de CreateProcess

> Livro didático baseado em *Windows Internals, 7th Edition*.
> Atualizado para Windows 11 com exemplos práticos em C++ moderno.

> Navegação: [Anterior](capitulo-03-03-protected-processes.md) | [Índice Cap.3](capitulo-03.md) | [Próximo](capitulo-03-05-image-loader.md)

---

## Os 6 estágios de criação de um processo

Quando você chama `CreateProcess`, o kernel executa uma sequência bem definida de etapas. Entender essa sequência é fundamental para debugar problemas de inicialização, entender injeção de código, e desenvolver ferramentas de análise.

```
Estágio 1: Abrir o arquivo de imagem
Estágio 2: Criar o objeto EPROCESS
Estágio 3: Criar o espaço de endereços virtual
Estágio 4: Carregar ntdll.dll
Estágio 5: Criar a thread principal
Estágio 6: Completar inicialização e notificar o subsistema
```

---

## Estágio 1 — Abrir e validar a imagem

`CreateProcessInternal` em kernel32.dll:

1. Abre o arquivo executável como um objeto de seção mapeável
2. Verifica se é um PE válido (assinatura `MZ` + header PE)
3. Determina o tipo de imagem:
   - `IMAGE_SUBSYSTEM_WINDOWS_GUI` → app Win32 com janela
   - `IMAGE_SUBSYSTEM_WINDOWS_CUI` → app console
   - `IMAGE_SUBSYSTEM_NATIVE` → bloqueado via Win32 API
4. Se o arquivo for um batch (.cmd, .bat): lança `cmd.exe /c arquivo.cmd`
5. Se for um script: delega ao script host configurado

Depois, chama `NtCreateUserProcess` em ntdll.dll para entrar no kernel.

---

## Estágio 2 — Criar o objeto EPROCESS (kernel)

`NtCreateUserProcess` no kernel chama `PspAllocateProcess`:

```
PspAllocateProcess:
  1. Aloca EPROCESS da NonPagedPool
  2. Copia flags do processo pai (se herança de atributos)
  3. Configura listas (ActiveProcessLinks, ThreadListHead)
  4. Cria o token de acesso:
     - Herda do processo pai (padrão)
     - Ou usa o token fornecido (CreateProcessAsUser)
  5. Configura quotas (memória, CPU)
  6. Inicializa o handle table (ObjectTable)
  7. Configura campos de segurança (MandatoryLevel, AppContainer SID se UWP)
```

O novo EPROCESS é inserido na lista global de processos via `PspInsertProcess` (que atualiza `ActiveProcessLinks`). A partir deste momento, o processo existe no sistema e pode ser visto por ferramentas como Process Explorer.

---

## Estágio 3 — Criar o espaço de endereços virtual

O Memory Manager cria o espaço de endereços do novo processo:

```
MmCreateProcessAddressSpace:
  1. Aloca uma nova CR3 (PML4 — Page Map Level 4)
  2. Copia as entradas de kernel-mode do CR3 do processo atual
     (para que o kernel seja mapeado em todos os processos)
  3. Cria o mapeamento da imagem executável via seção de arquivo
  4. Inicializa a VAD tree (Virtual Address Descriptor tree)
```

Neste ponto, o espaço de endereços contém apenas:
- O executável mapeado
- As páginas de kernel (copiadas do sistema)
- Nenhuma DLL de user mode ainda

---

## Estágio 4 — Carregar ntdll.dll

Antes de criar a thread principal, o kernel mapeia `ntdll.dll` no espaço do novo processo:

```
PspInitializeProcessSecurity / MmInitializeProcessAddressSpace:
  1. Abre ntdll.dll como seção
  2. Mapeia-a no novo espaço de endereços
  3. ntdll.dll é a única DLL que o kernel carrega diretamente
     Todas as outras DLLs são carregadas pelo image loader de ntdll
```

Por que o kernel carrega ntdll diretamente? Porque ntdll contém o ponto de entrada do loader (`LdrInitializeThunk`) — o código inicial que o kernel executa na nova thread. Sem ntdll, a thread não conseguiria nem começar.

---

## Estágio 5 — Criar a thread principal

`PspCreateThread` cria a thread inicial do processo:

```
PspCreateThread:
  1. Aloca ETHREAD na NonPagedPool
  2. Configura KTHREAD (contexto de scheduling)
  3. Aloca stack de user mode (1 MB por padrão)
  4. Aloca stack de kernel mode (~64 KB)
  5. Define o ponto de entrada inicial:
     → LdrInitializeThunk em ntdll.dll
     → (não o main() da aplicação — isso vem depois do loader)
  6. Se CREATE_SUSPENDED: thread criada em estado suspenso
  7. Se não: thread colocada na ready queue do scheduler
```

O ponto de entrada inicial não é `main()` nem `WinMain()`. É `LdrInitializeThunk` em ntdll, que vai:
1. Inicializar o heap do processo
2. Carregar todas as DLLs dependentes (recursivamente)
3. Executar DllMain de cada DLL com DLL_PROCESS_ATTACH
4. Finalmente chamar o entry point real do executável

---

## Estágio 6 — Notificar o subsistema e completar

De volta em user mode (CreateProcessInternal em kernel32.dll):

```
Notificação do subsistema Windows (Csrss.exe):
  1. Envia mensagem ALPC para Csrss: "novo processo criado"
  2. Csrss aloca CSR_PROCESS para rastrear este processo
  3. Csrss registra a janela de console (se processo console)
  4. Csrss associa o processo à sessão correta

PEB Population (kernel32.dll no novo processo, via WriteProcessMemory):
  5. Preenche o PEB do novo processo:
     - ImageBaseAddress
     - ProcessParameters (command line, environment, handles)
     - SessionId
  6. Se CREATE_SUSPENDED: retorna com processo suspenso
  7. Se não: a thread começa a executar LdrInitializeThunk
```

---

## O que LdrInitializeThunk faz (o loader em ação)

Quando a thread principal começa a executar, o primeiro código que roda é:

```
ntdll!LdrInitializeThunk
  → ntdll!_LdrpInitialize
    → ntdll!LdrpInitializeProcess
      1. Inicializa heap do processo
      2. Processa PEB.ProcessParameters
      3. Lê PEB.Ldr (inicialmente vazio)
      4. Para cada DLL importada pelo EXE:
         a. LdrpFindOrMapDll: localiza o arquivo no disco
         b. LdrpMapDll: mapeia como seção de arquivo
         c. Adiciona à PEB.Ldr InMemoryOrderModuleList
         d. Recursivamente carrega DLLs que aquela DLL importa
      5. Para cada DLL carregada (em ordem de dependência):
         a. Resolve relocações se necessário
         b. Chama DllMain(hModule, DLL_PROCESS_ATTACH, nullptr)
      6. Chama o entry point do executável (main, WinMain, etc.)
```

### DllMain e suas restrições

```cpp
// DllMain é chamado pelo loader com o Loader Lock adquirido
BOOL WINAPI DllMain(HMODULE hmod, DWORD reason, LPVOID reserved) {
    switch (reason) {
    case DLL_PROCESS_ATTACH:
        // PERIGO: Loader Lock está adquirido aqui!
        // NÃO faça: criar threads, carregar outras DLLs, chamar LoadLibrary
        // PODE fazer: inicialização simples de dados, HeapAlloc
        break;
    case DLL_PROCESS_DETACH:
        // Cleanup quando a DLL é descarregada
        break;
    }
    return TRUE;
}
```

O **Loader Lock** (`ntdll!LdrpLoaderLock`) é adquirido durante todo o carregamento de DLLs. Criar uma thread em DllMain que tenta carregar outra DLL causa deadlock — a thread nova tentará adquirir o Loader Lock que a thread de loading já tem.

---

## Monitorando criação de processos com callbacks

O kernel oferece um mecanismo para drivers receberem notificação de criação/encerramento de processos:

```c
// Em um driver:
VOID processCallback(
    PEPROCESS processo,
    HANDLE pid,
    PPS_CREATE_NOTIFY_INFO createInfo  // nullptr = processo encerrando
) {
    if (createInfo) {
        // Processo sendo criado
        wprintf(L"Novo processo: %wZ (PID %llu)\n",
            createInfo->ImageFileName, (ULONG64)pid);
        
        // Pode BLOQUEAR a criação:
        // createInfo->CreationStatus = STATUS_ACCESS_DENIED;
    }
}

// Registrando o callback:
PsSetCreateProcessNotifyRoutineEx(processCallback, FALSE);
```

Antivírus usam esse mecanismo extensivamente para escanear novos processos. O campo `createInfo->CreationStatus` permite bloquear a criação de processos — base dos sistemas de whitelisting de aplicações.

---

## Rastreando com Process Monitor

Para ver **toda** a atividade de CreateProcess no sistema em tempo real:
1. Abra ProcMon
2. Filtro: `Operation → is → Process Create`
3. Observe Path (executável), PID pai, PID filho, e código de resultado

Você verá não só aplicações que você abre, mas também `svchost.exe` spawning serviços, `SearchIndexer.exe` lançando processos auxiliares, e atualizações automáticas criando processos de instalação.

---

*Próximo: [Image Loader e API Sets](capitulo-03-05-image-loader.md)*
