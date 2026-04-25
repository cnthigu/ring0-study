# Exemplos de codigo (Win32)

Exemplos **minimos** em C++ para testar ideias e APIs no Windows, sem projetos completos de produto. Compilar no **Visual Studio** (ou outra toolchain com Windows SDK) no Windows x64, salvo nota em contrario.

| Pasta | API principal | O que demonstra | Documentacao |
|--------|----------------|-----------------|----------------|
| [`ex-winapi/get-module-file-name/...`](./ex-winapi/get-module-file-name/get-module-file-name/) | [GetModuleFileNameW](https://learn.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-getmodulefilenamew) | Caminho do executavel deste processo | [Module](https://learn.microsoft.com/en-us/windows/win32/api/libloaderapi/) |
| [`ex-winapi/create-toolhelp32-snapshot/...`](./ex-winapi/create-toolhelp32-snapshot/create-toolhelp32-snapshot/) | [CreateToolhelp32Snapshot](https://learn.microsoft.com/en-us/windows/win32/api/tlhelp32/nf-tlhelp32-createtoolhelp32snapshot), [Process32First](https://learn.microsoft.com/en-us/windows/win32/api/tlhelp32/nf-tlhelp32-process32first) / `Process32Next` | Listar processos a partir de um snapshot | [Tlhelp32](https://learn.microsoft.com/en-us/windows/win32/api/tlhelp32/) |

Cada `main.cpp` comeca com um comentario curto: objetivo e API em destaque.
