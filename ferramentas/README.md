# Ferramentas

Setup do ambiente de estudo para C++ e Windows Internals.

---

## Compilador e IDE

| Ferramenta | Descricao | Link |
|------------|-----------|------|
| Visual Studio 2022 Community | IDE principal para C++ no Windows | [download](https://visualstudio.microsoft.com/vs/community/) |
| MSVC | Compilador C++ da Microsoft, instalado com o Visual Studio | - |
| CMake | Build system multiplataforma | [download](https://cmake.org/download/) |

> No Visual Studio, instalar o workload **"Desenvolvimento para desktop com C++"** e o componente **"SDK do Windows 11"**.

---

## Debugging

| Ferramenta | Descricao | Link |
|------------|-----------|------|
| WinDbg Preview | Debugger para user-mode e kernel-mode | [Microsoft Store](https://apps.microsoft.com/detail/9pgjgd53tn86) |
| x64dbg | Debugger open source para reverse engineering | [download](https://x64dbg.com/) |
| OllyDbg | Debugger classico para analise de binarios 32-bit | [download](https://www.ollydbg.de/) |
| Cheat Engine | Debugger e scanner de memoria, util para entender memory internals | [download](https://www.cheatengine.org/) |

---

## Sysinternals

| Ferramenta | Descricao | Link |
|------------|-----------|------|
| Process Explorer | Visualizacao avancada de processos, handles e DLLs | [download](https://learn.microsoft.com/en-us/sysinternals/downloads/process-explorer) |
| Suite completa | Todas as ferramentas Sysinternals | [download](https://learn.microsoft.com/en-us/sysinternals/downloads/sysinternals-suite) |

---

## Analise e Reverse Engineering

| Ferramenta | Descricao | Link |
|------------|-----------|------|
| Dependency Walker | Cadeia de modulos, imports e exports de EXE/DLL (ferramenta classica) | [README e uso no repo](./Dependency-Walker/README.md) |
| PE-bear | Visualizador de arquivos PE (EXE, DLL, SYS) | [download](https://github.com/hasherezade/pe-bear) |
| CFF Explorer | Editor e visualizador de PE | [download](https://ntcore.com/explorer-suite/) |
| DIE (Detect It Easy) | Identificador de packers e compiladores | [download](https://github.com/horsicq/Detect-It-Easy) |
| HxD | Editor hexadecimal | [download](https://mh-nexus.de/en/hxd/) |
| System Informer | Visualizacao avancada de processos, handles, drivers e memoria (sucessor do Process Hacker) | [download](https://systeminformer.io/) |

---

## Referencia

| Recurso | Descricao | Link |
|---------|-----------|------|
| Windows SDK | Headers, libs e ferramentas para desenvolvimento Windows | [download](https://developer.microsoft.com/en-us/windows/downloads/windows-sdk/) |
| WDK (Windows Driver Kit) | Para desenvolvimento de drivers | [download](https://learn.microsoft.com/en-us/windows-hardware/drivers/download-the-wdk) |
| Microsoft Learn - Win32 API | Referencia completa da WinAPI | [link](https://learn.microsoft.com/en-us/windows/win32/api/) |
