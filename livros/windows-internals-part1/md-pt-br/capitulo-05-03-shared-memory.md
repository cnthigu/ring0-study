# Capítulo 5 — Memória Compartilhada e Seções

> Livro didático baseado em *Windows Internals, 7th Edition*.
> Atualizado para Windows 11 com exemplos práticos em C++ moderno.

> Navegação: [Anterior](capitulo-05-02-alocacao.md) | [Índice Cap.5](capitulo-05.md) | [Próximo](capitulo-05-04-working-sets.md)

---

## Section Objects: o mecanismo de compartilhamento do kernel

No kernel do Windows, o compartilhamento de memória é implementado via **Section Objects** (objeto kernel do tipo `\Section`). Uma section é um objeto que representa um bloco de memória ou um arquivo que pode ser mapeado no espaço de endereços de um ou mais processos.

```
Section Object (kernel)
  ├── Segment (Control Area)     ← descreve o backing store
  │     ├── FileObject           ← arquivo de respaldo (ou page file)
  │     ├── SubSection[N]        ← segmentos do arquivo mapeado
  │     └── PfnDatabase entries  ← entradas no banco de page frames
  └── ViewList                   ← lista de mapeamentos ativos

Cada processo que mapeia a section tem uma "View":
  View = entrada no VAD do processo com ponteiro para a Section
  Múltiplos processos → mesmos frames físicos → IPC sem cópia!
```

---

## File Mapping: arquivos mapeados em memória

```cpp
#include <windows.h>

// Servidor: cria o arquivo mapeado
HANDLE hFile = CreateFileW(
    L"dados.bin",
    GENERIC_READ | GENERIC_WRITE,
    FILE_SHARE_READ | FILE_SHARE_WRITE,
    nullptr, OPEN_EXISTING, 0, nullptr);

// Criar section para o arquivo (64 KB máximo de mapeamento)
HANDLE hMapping = CreateFileMappingW(
    hFile,          // ou INVALID_HANDLE_VALUE → usa page file (memória anônima)
    nullptr,        // segurança
    PAGE_READWRITE, // proteção
    0, 0,           // tamanho alto e baixo (0,0 = usar tamanho do arquivo)
    L"MinhaSectionNomeada");   // nome no namespace de objetos

// Mapear uma view (fatia) do arquivo no espaço de endereços
void* pView = MapViewOfFile(
    hMapping,
    FILE_MAP_ALL_ACCESS,
    0, 0,           // offset alto e baixo (início do arquivo)
    0);             // tamanho (0 = mapear tudo)

// Usar como se fosse memória normal:
int* dados = (int*)pView;
dados[0] = 42;

// Garantir que mudanças vão para disco:
FlushViewOfFile(pView, 0);  // 0 = flush tudo

// Limpar:
UnmapViewOfFile(pView);
CloseHandle(hMapping);
CloseHandle(hFile);
```

---

## Shared Memory: IPC entre processos

A forma mais eficiente de IPC é memória compartilhada — zero cópia entre processos:

```cpp
// === PROCESSO SERVIDOR ===
HANDLE hMap = CreateFileMappingW(
    INVALID_HANDLE_VALUE,  // backed pelo page file (memória, não arquivo)
    nullptr,
    PAGE_READWRITE,
    0, 4096,               // 4 KB de memória compartilhada
    L"Global\\MeuSharedMem");  // "Global\" → visível em todas as sessões

void* pShared = MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);

// Escrever dados para o cliente ler:
strcpy((char*)pShared, "Olá do servidor!");

// === PROCESSO CLIENTE (outro processo) ===
HANDLE hMap2 = OpenFileMappingW(FILE_MAP_ALL_ACCESS, FALSE, L"Global\\MeuSharedMem");
void* pShared2 = MapViewOfFile(hMap2, FILE_MAP_ALL_ACCESS, 0, 0, 0);

// Lê o mesmo frame físico — sem cópia de dados!
printf("Recebido: %s\n", (char*)pShared2);

UnmapViewOfFile(pShared2);
CloseHandle(hMap2);
```

### Namespace de objetos: Local vs Global

```
Local\NomeDaSection    → visível apenas na sessão atual (sessão 1, 2, etc.)
Global\NomeDaSection   → visível em todas as sessões (requer SeCreateGlobalPrivilege)
\BaseNamedObjects\Nome → caminho completo no Object Manager namespace
```

Por segurança, processos em sessões diferentes não podem ver objetos uns dos outros sem `Global\`.

---

## Copy-on-Write: DLLs compartilhadas sem sacrificar isolamento

Quando múltiplos processos carregam a mesma DLL, eles **compartilham os frames físicos** das páginas de código e dados read-only. Os dados que a DLL modifica recebem tratamento especial:

```
DLL carregada por 3 processos:
  ├── Código (PAGE_EXECUTE_READ) → 1 frame físico compartilhado pelos 3
  ├── Dados read-only (.rdata)   → 1 frame físico compartilhado pelos 3
  └── Dados read-write (.data)   → 1 frame físico compartilhado (COW pending)

Quando Processo A escreve em .data[offset]:
  1. Page fault → CoW triggered
  2. Kernel aloca novo frame físico
  3. Copia o conteúdo original do frame compartilhado
  4. Mapeia o novo frame apenas para o Processo A
  5. Processo A escreve no frame privado
  → Processos B e C continuam vendo o original
  → Processo A vê sua cópia modificada
```

Você pode ver CoW em ação com VMMap: páginas marcadas como "Shareable" que se tornam "Private" após a primeira escrita.

---

## Memory-Mapped I/O: arquivos como arrays

```cpp
// Caso de uso: processar um arquivo grande sem carregar tudo na RAM
HANDLE hFile = CreateFileW(L"bigfile.bin", GENERIC_READ,
    FILE_SHARE_READ, nullptr, OPEN_EXISTING,
    FILE_FLAG_RANDOM_ACCESS, nullptr);

LARGE_INTEGER fileSize;
GetFileSizeEx(hFile, &fileSize);

HANDLE hMap = CreateFileMappingW(hFile, nullptr, PAGE_READONLY,
    fileSize.HighPart, fileSize.LowPart, nullptr);

// Mapear todo o arquivo (apenas o espaço de endereços — não carrega em RAM):
const uint8_t* p = (const uint8_t*)MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 0);

// Acessar: o Memory Manager carrega páginas conforme necessário
// Nenhum ReadFile explícito! Cada acesso pode gerar um page fault
// que o MM resolve lendo do arquivo.
uint64_t checksum = 0;
for (size_t i = 0; i < (size_t)fileSize.QuadPart; i++) {
    checksum += p[i];  // Page faults acontecem automaticamente
}

UnmapViewOfFile(p);
CloseHandle(hMap);
CloseHandle(hFile);
```

**Vantagens de file mapping vs ReadFile:**
- Sem cópia dupla (ReadFile: disco → cache do kernel → buffer do usuário; mmap: disco → memória mapeada)
- O OS gerencia o prefetch automaticamente
- Múltiplos processos podem mapear o mesmo arquivo (compartilhamento automático)

**Desvantagens:**
- Não funciona bem com arquivos muito maiores que a RAM disponível (thrashing)
- Menos controle sobre quando o I/O acontece

---

## DLLs: de seção a frame físico

Quando `ntdll.dll` é carregada pela primeira vez:
1. Kernel abre o arquivo como `FileObject`
2. Cria uma `Section` com backing no arquivo PE
3. Mapeia como `MEM_IMAGE` no espaço do processo
4. Páginas de código: `PAGE_EXECUTE_READ`
5. Páginas de dados: `PAGE_WRITECOPY` (CoW quando modificadas)

Quando `ntdll.dll` é carregada pelo segundo processo:
1. Kernel encontra a `Section` já existente (cache de sections)
2. Cria uma nova View para o segundo processo
3. Reutiliza os **mesmos frames físicos** — não duplica em RAM!

Por isso, ter 100 processos rodando todos com `ntdll.dll` carregada usa quase a mesma RAM que ter 1 processo — o código é fisicamente compartilhado.

---

## Segurança em Shared Memory

Seções nomeadas são objetos kernel com Security Descriptors. Por padrão, apenas o criador pode abrir. Para compartilhar entre diferentes usuários:

```cpp
SECURITY_ATTRIBUTES sa;
SECURITY_DESCRIPTOR sd;
InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);

// Permitir acesso a todos (cuidado em produção!):
SetSecurityDescriptorDacl(&sd, TRUE, nullptr, FALSE);
sa.nLength = sizeof(sa);
sa.lpSecurityDescriptor = &sd;
sa.bInheritHandle = FALSE;

HANDLE hMap = CreateFileMappingW(
    INVALID_HANDLE_VALUE, &sa, PAGE_READWRITE,
    0, 4096, L"Global\\MinhaMemoriaCompartilhada");
```

Na prática, use ACLs bem definidas — compartilhar memória entre processos de diferentes usuários é uma superfície de ataque.

---

*Próximo: [Working Sets e Memória Física](capitulo-05-04-working-sets.md)*
