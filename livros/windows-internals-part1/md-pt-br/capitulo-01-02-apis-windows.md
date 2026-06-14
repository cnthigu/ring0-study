# Capítulo 1 — A Camada de APIs do Windows

> Livro didático baseado em *Windows Internals, 7th Edition*.
> Atualizado para Windows 11 com exemplos práticos em C++ moderno.
> Repositório: [ring0-study](https://github.com/cnthigu/ring0-study)

> Navegação: [Anterior](capitulo-01-01-introducao.md) | [Índice](README.md) | [Próximo](capitulo-01-03-processos-jobs.md)

---

## A camada de APIs do Windows

Quando você escreve `CreateFile()` em C++, você está chamando uma função da **Windows API** — a interface pública que o sistema oferece para que programas em user mode acessem serviços do OS. Mas o Windows não tem uma API única e simples. Tem camadas histórica e funcionalmente distintas que convivem juntas. Entender essas camadas é essencial para não se perder na documentação ou tomar decisões erradas de design.

```
┌──────────────────────────────────────────┐
│           Sua aplicação                  │
├────────────┬──────────────┬──────────────┤
│  Win32 API │   WinRT API  │  .NET (CLR)  │
├────────────┴──────────────┴──────────────┤
│          Ntdll.dll (Native API)          │
├──────────────────────────────────────────┤
│         NTOSKRNL.exe (Kernel)            │
└──────────────────────────────────────────┘
```

---

## Win32 API — a fundação de tudo

A Win32 API é a camada mais antiga e ainda a mais importante. É um conjunto de funções em estilo C exportadas por DLLs do sistema, principalmente:

| DLL | Responsabilidade |
|-----|-----------------|
| `kernel32.dll` | Processos, threads, arquivos, memória |
| `advapi32.dll` | Registro, segurança, serviços |
| `user32.dll` | Janelas, mensagens, input |
| `gdi32.dll` | Primitivas de desenho |
| `ntdll.dll` | Camada nativa (abaixo da Win32) |

Por que C-style e não C++? Porque em 1993 C era o menor denominador comum — qualquer linguagem conseguia chamar funções C através de sua convenção de chamada. Isso criou uma API que pode ser usada de C, C++, Delphi, Rust, Python (ctypes) e qualquer outra linguagem que consiga chamar funções de uma DLL.

### O problema do ANSI vs Unicode

Todo Windows NT foi projetado para Unicode desde o início — internamente, tudo é UTF-16LE. Mas aplicações legadas usavam strings ANSI (1 byte por caractere). Para compatibilidade, a Win32 oferece **dois pontos de entrada** para cada função que aceita string:

```cpp
// CreateFile é uma macro que expande para um desses dois:
HANDLE CreateFileA(LPCSTR lpFileName, ...);   // ANSI (A = Ansi)
HANDLE CreateFileW(LPCWSTR lpFileName, ...);  // Unicode (W = Wide)

// Na prática, SEMPRE use a versão W. Exemplo:
HANDLE h = CreateFileW(
    L"C:\\dados\\arquivo.bin",
    GENERIC_READ | GENERIC_WRITE,
    0, nullptr,
    OPEN_EXISTING,
    FILE_ATTRIBUTE_NORMAL,
    nullptr
);
```

Quando você chama `CreateFileA`, o sistema converte internamente a string ANSI para UTF-16 e chama `CreateFileW`. Isso tem custo de performance e pode perder caracteres fora do ASCII básico. **Regra prática**: use sempre a versão W (ou a macro sem sufixo com `UNICODE` definido no projeto).

No Visual Studio 2022, novos projetos C++ têm `UNICODE` e `_UNICODE` definidos por padrão — então `CreateFile` já expande para `CreateFileW`. Não dependa disso em código portátil; seja explícito.

### Native API (Ntdll.dll)

Abaixo da Win32 existe uma camada que a maioria dos desenvolvedores nunca toca diretamente: a **Native API**, exposta pela `ntdll.dll`. Funções como `NtCreateFile`, `NtCreateProcess`, `NtQuerySystemInformation`.

Essas funções são as que a Win32 chama internamente. Elas são tecnicamente não documentadas (embora muitas estejam documentadas no WDK e em recursos da comunidade) e podem mudar entre versões do Windows.

Por que mencionamos isso? Porque ferramentas de segurança, malware e drivers frequentemente operam nessa camada para evitar hooks na Win32. E porque ao usar o WinDbg para inspecionar o kernel, você vai ver essas chamadas o tempo todo.

---

## COM — Component Object Model

A Win32 clássica tem um problema: ao longo de décadas, acumulou milhares de funções sem uma estrutura lógica clara. Você tem `CreateWindow`, `CreateProcess`, `CreateFile`, `CreateFont` — todos com assinaturas diferentes, comportamentos inconsistentes, e nada que agrupe funcionalidades relacionadas de forma orientada a objetos.

O COM nasceu em 1993 (inicialmente chamado OLE 2) para resolver isso. A ideia central é simples: ao invés de funções soltas, você tem **objetos** que expõem **interfaces**.

### Como o COM funciona internamente

Uma interface COM é uma tabela de ponteiros de função (vtable) — exatamente o que o compilador C++ usa para implementar funções virtuais:

```cpp
// Definição conceitual de uma interface COM
// (na prática, herda de IUnknown)
struct IFileReader {
    virtual HRESULT Read(BYTE* buffer, DWORD size, DWORD* bytesRead) = 0;
    virtual HRESULT Seek(LONGLONG offset, DWORD origin) = 0;
    virtual ULONG AddRef() = 0;
    virtual ULONG Release() = 0;
    // ...
};
```

O cliente não sabe nada sobre a implementação concreta — só sabe que o objeto que recebeu implementa `IFileReader`. Isso permite:

1. **Substituição de implementação** sem quebrar clientes
2. **Cross-language**: qualquer linguagem que consiga chamar funções por ponteiro pode usar COM
3. **Cross-process**: COM tem marshaling automático para chamar objetos em outro processo ou até máquina diferente (DCOM)

### GUIDs e registro

Cada interface e cada classe COM tem um GUID (identificador único de 128 bits). Você registra uma classe COM no registry (`HKLM\SOFTWARE\Classes\CLSID\{...}`) e qualquer processo pode instanciá-la via `CoCreateInstance`:

```cpp
#include <windows.h>
#include <shlobj.h>   // IShellFolder

// Criar um objeto COM do shell
IShellFolder* pDesktop = nullptr;
HRESULT hr = SHGetDesktopFolder(&pDesktop);

if (SUCCEEDED(hr)) {
    // Usar o objeto...
    pDesktop->Release();  // Sempre libere!
}
```

> **Regra de ouro do COM**: todo objeto que você obtém deve ser `Release()`'d quando não precisar mais. COM usa contagem de referências. Não faça isso e você tem memory leaks (ou pior, o objeto fica vivo para sempre em servidores out-of-process).
>
> Use `ComPtr<T>` (Windows Runtime C++ Template Library / WRL) ou `Microsoft::WRL::ComPtr<T>` para gerenciar o lifetime automaticamente.

### Quais APIs são COM?

Muitas APIs importantes do Windows são COM:

- **DirectX / Direct3D** — gráficos
- **DirectShow / Media Foundation** — multimedia
- **Windows Imaging Component (WIC)** — processamento de imagens
- **Shell (IShellFolder, IContextMenu)** — extensões do Explorer
- **BITS** — transferência de arquivos em background
- **TaskScheduler** — agendamento de tarefas
- **WMI** — gerenciamento e monitoramento

---

## Windows Runtime (WinRT)

O Windows 8 introduziu o **Windows Runtime (WinRT)** — não confundir com Windows RT, o Windows baseado em ARM que foi descontinuado.

WinRT é, tecnicamente, uma evolução do COM. Ainda usa vtables e interfaces, mas adiciona:

- **Metadados de tipo completos** em arquivos `.winmd` (baseados no formato de metadados do .NET)
- **Namespace hierárquico** (`Windows.Storage.StorageFile`, `Windows.UI.Xaml.Controls.Button`)
- **Async por padrão** — toda operação lenta é assíncrona, com suporte a `co_await` no C++20
- **Projeções de linguagem** — APIs em C++/WinRT, C#, JavaScript, Python

```cpp
// C++/WinRT: listando arquivos com WinRT assíncrono
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.Foundation.Collections.h>
using namespace winrt;
using namespace Windows::Storage;

IAsyncAction ListarArquivos() {
    auto pasta = co_await StorageFolder::GetFolderFromPathAsync(L"C:\\Users\\Public");
    auto arquivos = co_await pasta.GetFilesAsync();
    
    for (auto const& arquivo : arquivos) {
        wprintf(L"%s\n", arquivo.Name().c_str());
    }
}
```

WinRT é a API preferida para Universal Windows Platform (UWP) e aplicações modernas no Windows. Para desenvolvimento de sistemas e segurança, você continuará usando Win32 na maior parte do tempo.

---

## .NET Framework e CLR

O .NET é uma plataforma de desenvolvimento gerenciada — código escrito em C#, VB.NET ou F# compila para **CIL** (Common Intermediate Language), não para código de máquina diretamente. O **CLR (Common Language Runtime)** executa esse código usando um compilador JIT (Just-In-Time).

Do ponto de vista do Windows Internals, o CLR é implementado como uma DLL COM carregada no processo. Internamente, ele usa a Win32 para criar threads, alocar memória (`VirtualAlloc`), abrir arquivos — tudo passa pela mesma Win32 que seu código C++ nativo usaria.

O garbage collector do CLR cria uma "heap gerenciada" dentro do espaço de memória do processo, separada da heap nativa do Windows. Quando o GC roda, ele pode suspender threads — algo relevante para análise de performance.

Com o advento do **.NET 6/7/8** (o novo .NET Core unificado), a plataforma .NET roda também em Linux e macOS, mas as APIs específicas do Windows (`System.Windows.Forms`, `System.IO.Pipes` no modo Windows) ainda dependem da Win32 por baixo.

---

## Qual API usar quando?

| Cenário | API recomendada |
|---------|----------------|
| Desenvolvimento de sistema, drivers, ferramentas de segurança | Win32 API (C/C++) |
| Aplicações desktop modernas com UI | WinRT / C++/WinRT ou .NET WinForms/WPF |
| Aplicações corporativas, serviços web | .NET (C#) |
| Integração com shell, COM servers | COM |
| UWP / Microsoft Store | WinRT obrigatório |

A regra prática: **para entender o Windows Internals, você precisa dominar a Win32**. WinRT e .NET são abstrações construídas sobre ela. Quando algo dá errado num programa .NET, a ferramenta de diagnóstico vai te mostrar chamadas Win32 e estruturas do kernel.

---

## Serviços, funções e rotinas — terminologia

O Windows usa os termos de forma que pode confundir. Neste livro, os significados são:

| Termo | O que significa |
|-------|----------------|
| **Função da Windows API** | Sub-rotina documentada: `CreateProcess`, `CreateFile` |
| **Serviço nativo / syscall** | Rotinas não documentadas da ntdll: `NtCreateUserProcess` |
| **Rotina de suporte do kernel** | Chamável só em kernel mode: `ExAllocatePool`, `IoCreateDevice` |
| **Serviço do Windows** | Processo gerenciado pelo Service Control Manager: Task Scheduler, BITS |
| **DLL** | Biblioteca carregada dinamicamente: `kernel32.dll`, `ntdll.dll` |

Quando você chama `CreateProcess`, internamente ela chama `NtCreateUserProcess` (serviço nativo), que faz a transição para kernel mode e chama rotinas internas do NTOSKRNL. A cadeia completa:

```
Seu código → CreateProcess (kernel32.dll)
           → NtCreateUserProcess (ntdll.dll)  
           → [transição user → kernel mode via syscall instruction]
           → NtCreateUserProcess (ntoskrnl.exe)
           → PspCreateProcess (ntoskrnl.exe — não exportada)
```

Nos capítulos seguintes, vamos dissecar cada nível dessa cadeia.

---

*Próximo: [Processos e Jobs](capitulo-01-03-processos-jobs.md)*
