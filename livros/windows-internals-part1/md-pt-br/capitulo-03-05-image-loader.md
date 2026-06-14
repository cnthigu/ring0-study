# Capítulo 3 — Image Loader, DLL Search e API Sets

> Livro didático baseado em *Windows Internals, 7th Edition*.
> Atualizado para Windows 11 com exemplos práticos em C++ moderno.

> Navegação: [Anterior](capitulo-03-04-flow-createprocess.md) | [Índice Cap.3](capitulo-03.md) | [Próximo](capitulo-03-06-jobs.md)

---

## O Image Loader: o coração de ntdll.dll

O Image Loader (`Ldr*` em ntdll.dll) é responsável por tudo relacionado ao carregamento de código em um processo. É ele que transforma um arquivo PE em disco em código executável na memória.

Funções principais:
```
LdrLoadDll          → CarreCarta uma DLL (base de LoadLibrary)
LdrGetProcedureAddress → resolve endereço de função (base de GetProcAddress)
LdrUnloadDll        → descarrega uma DLL
LdrFindEntryForAddress → encontra qual DLL contém um endereço
LdrInitializeThunk  → entry point inicial de toda thread
```

---

## A ordem de busca de DLL

Quando o loader precisa carregar `minhadll.dll`, ele procura nos seguintes locais **em ordem**:

1. **DLLs já carregadas** no processo (PEB.Ldr) — se já está em memória, usa a cópia existente
2. **Known DLLs** (`HKLM\SYSTEM\CurrentControlSet\Control\Session Manager\KnownDLLs`) — DLLs fundamentais do sistema pré-carregadas como seções compartilhadas (`ntdll.dll`, `kernel32.dll`, etc.)
3. **Diretório do executável** — onde o .exe está
4. **System32** (`C:\Windows\System32\`)
5. **Windows** (`C:\Windows\`)
6. **Diretório de trabalho atual** do processo
7. **PATH** — variável de ambiente

### DLL Search Order Hijacking

A ordem 3 (diretório do executável) antes de System32 (ponto 4) cria uma vulnerabilidade clássica: se você colocar uma DLL maliciosa com o mesmo nome que uma DLL do sistema no diretório do executável, o loader a carrega em vez da DLL legítima.

```
C:\Programa\
  programa.exe     ← legítimo
  version.dll      ← MALICIOSA — será carregada em vez de C:\Windows\System32\version.dll
```

Mitigações modernas:
- `SetDllDirectory(L"")` — remove o diretório de trabalho atual da busca
- `AddDllDirectory` + `LOAD_LIBRARY_SEARCH_*` flags — controle explícito da ordem de busca
- Manifests de aplicativo com `<file>` explícito

---

## API Sets: o namespace moderno de DLLs

Nas versões antigas do Windows, você importava diretamente de `kernel32.dll`. Com o OneCore (Windows 10+), as APIs foram reorganizadas em **API Sets** — um esquema de indireção que separa a API pública da implementação.

```
Seu código importa: "api-ms-win-core-processthreads-l1-1-0.dll"
                         ↓
         Loader resolve via API Set Map
                         ↓
           Implementação real: "kernelbase.dll"
```

O API Set Map está em `ntdll.dll` e mapeia nomes de "contratos" de API para DLLs reais. Você pode vê-lo:

```
kd> dt ntdll!_API_SET_NAMESPACE
lkd> ? ntdll!ApiSetMap     ← endereço do mapa
```

Por que isso existe? Para permitir que o mesmo conjunto de DLLs de sistema (kernel32.dll, advapi32.dll) existam em todas as plataformas OneCore (PC, Xbox, HoloLens, IoT), enquanto a implementação real varia por plataforma.

Exemplo de resolução:
```
"api-ms-win-core-processthreads-l1-1-0.dll"  → kernelbase.dll
"api-ms-win-core-file-l1-1-0.dll"            → kernelbase.dll  
"api-ms-win-core-registry-l1-1-0.dll"        → kernelbase.dll
"api-ms-win-security-base-l1-1-0.dll"        → kernelbase.dll
```

Na prática: kernel32.dll hoje é uma "forwarding DLL" que não implementa quase nada — reencaminha quase tudo para kernelbase.dll.

---

## Relocações e ASLR

Executáveis e DLLs têm um **preferred base address** — o endereço onde preferem ser carregados. Se esse endereço estiver ocupado, o loader precisa aplicar **relocações** — ajustar todos os endereços absolutos no código para refletir o novo endereço base.

**ASLR (Address Space Layout Randomization)**: o loader randomiza o endereço base de cada módulo. Isso torna exploits baseados em endereços hardcoded muito mais difíceis, pois o invasor não sabe onde o código estará em memória.

```cpp
// Para desabilitar ASLR em um processo específico (não recomendado em produção):
// No linker: /DYNAMICBASE:NO
// Via API: SetProcessMitigationPolicy(ProcessASLRPolicy, ...)
```

ASLR modes no Windows 11:
- **ASLR normal**: aleatoriza dentro de uma faixa (alguns bits)
- **High-Entropy ASLR**: aleatoriza mais bits para maior entropia; requer imagem compilada com `/HIGHENTROPYVA`
- **Mandatory ASLR**: força ASLR mesmo para imagens que desativaram `DYNAMICBASE`; configurável via `ProcessASLRPolicy`

---

## Side-by-Side (SxS) e Manifests

O **SxS** resolve o problema do "DLL hell" — versões incompatíveis de DLLs instaladas no sistema. Um executável pode declarar em seu **manifest** que versão específica de cada DLL quer usar:

```xml
<!-- Embed em resources do .exe ou como arquivo .manifest externo -->
<?xml version="1.0" encoding="UTF-8"?>
<assembly xmlns="urn:schemas-microsoft-com:asm.v1" manifestVersion="1.0">
  <assemblyIdentity
    version="1.0.0.0"
    processorArchitecture="amd64"
    name="MinhaApp"
    type="win32"
  />
  <dependency>
    <dependentAssembly>
      <assemblyIdentity
        type="win32"
        name="Microsoft.Windows.Common-Controls"
        version="6.0.0.0"
        processorArchitecture="amd64"
        publicKeyToken="6595b64144ccf1df"
        language="*"
      />
    </dependentAssembly>
  </dependency>
  <trustInfo xmlns="urn:schemas-microsoft-com:asm.v3">
    <security>
      <requestedPrivileges>
        <requestedExecutionLevel level="asInvoker" uiAccess="false"/>
      </requestedPrivileges>
    </security>
  </trustInfo>
</assembly>
```

O `requestedExecutionLevel` é o que determina o comportamento de UAC:
- `asInvoker` — roda com o nível atual, sem prompts UAC
- `requireAdministrator` — exige prompt UAC elevação
- `highestAvailable` — usa o token mais elevado disponível

---

## Delay Loading e LoadLibrary

Nem todas as DLLs precisam ser carregadas no início. **Delay loading** carrega uma DLL apenas quando uma função dela é chamada pela primeira vez:

```cpp
// Com /DELAYLOAD:minhadll.dll no linker:
// A DLL não é carregada até que FuncaoDaMinhadll seja chamada

// Você pode pré-carregar explicitamente:
__HrLoadAllImportsForDll("minhadll.dll");

// Ou descarregar:
__FUnloadDelayLoadedDLL2("minhadll.dll");
```

`LoadLibrary` / `FreeLibrary` para carregamento dinâmico em runtime:

```cpp
// Carregamento dinâmico completo — sem import lib
HMODULE hLib = LoadLibraryW(L"minhadll.dll");
if (hLib) {
    // Obter ponteiro para função
    auto pfunc = (void(*)())GetProcAddress(hLib, "MinhaFuncao");
    if (pfunc) pfunc();
    FreeLibrary(hLib);
}
```

---

## Inspecionando o loader em ação

```
# WinDbg: ver todas as DLLs carregadas em um processo
0:000> lm          ← list modules
0:000> lmDvm ntdll ← detalhe verbose de ntdll

# Processos de loader em WinDbg:
0:000> !lmi ntdll  ← loader module info

# Ver o PEB.Ldr programaticamente:
kd> !peb            ← mostra PEB incluindo lista de módulos
```

Com **Process Monitor** (ProcMon), você pode ver toda a busca de DLL em tempo real:
1. Filtro: `Operation → is → Load Image`
2. Você vê cada DLL que o processo carrega, com o caminho completo
3. Path `NAME NOT FOUND` antes do path real revelam a sequência de busca

Isso é essencial para diagnosticar problemas de "DLL não encontrada" e para analisar quais DLLs um executável suspeito carrega.

---

*Próximo: [Job Objects](capitulo-03-06-jobs.md)*
