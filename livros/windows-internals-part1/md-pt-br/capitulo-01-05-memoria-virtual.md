# Capítulo 1 — Memória Virtual e Modos do Processador

> Livro didático baseado em *Windows Internals, 7th Edition*.
> Atualizado para Windows 11 com exemplos práticos em C++ moderno.
> Repositório: [ring0-study](https://github.com/cnthigu/ring0-study)

> Navegação: [Anterior](capitulo-01-04-threads.md) | [Índice](README.md) | [Próximo](capitulo-01-06-hipervisor-vbs.md)

---

## O problema que a memória virtual resolve

Sem memória virtual, todo programa precisaria saber exatamente onde na RAM física seria carregado. Dois programas não poderiam usar os mesmos endereços. Você não poderia executar um programa maior do que a RAM disponível. E um bug em qualquer programa poderia corromper a memória de qualquer outro.

A **memória virtual** resolve todos esses problemas com uma ideia elegante: cada processo acredita ter um espaço de endereços próprio e contíguo. O hardware (a MMU — Memory Management Unit do processador) traduz automaticamente cada endereço virtual para um endereço físico real. Essa tradução é totalmente transparente para o programa.

---

## Arquitetura do espaço de endereços

Em sistemas **64-bit com Windows 11**, o espaço de endereços de cada processo é dividido em duas regiões:

```
Espaço de endereços virtual (x64, Windows 11):
┌─────────────────────────────────────────────┐
│                                             │
│   0x0000 0000 0000 0000                     │
│   ┌─────────────────────────────────────┐   │
│   │                                     │   │
│   │     ESPAÇO DO USUÁRIO (128 TB)      │   │
│   │                                     │   │
│   │  • Código do programa               │   │
│   │  • Heap, pilhas de threads          │   │
│   │  • DLLs mapeadas                    │   │
│   │  • Memória compartilhada            │   │
│   │                                     │   │
│   └─────────────────────────────────────┘   │
│   0x7FFF FFFF FFFF FFFF                     │
│                                             │
│   [gap — endereços inválidos]               │
│                                             │
│   0xFFFF 8000 0000 0000                     │
│   ┌─────────────────────────────────────┐   │
│   │                                     │   │
│   │     ESPAÇO DO KERNEL (128 TB)       │   │
│   │                                     │   │
│   │  • Código do kernel (ntoskrnl.exe)  │   │
│   │  • Drivers                          │   │
│   │  • Pool paginado e não-paginado     │   │
│   │  • Estruturas de sistema (EPROCESS, │   │
│   │    ETHREAD, KPCR, PCBs...)          │   │
│   │                                     │   │
│   └─────────────────────────────────────┘   │
│   0xFFFF FFFF FFFF FFFF                     │
└─────────────────────────────────────────────┘
```

O ponto crucial: **o espaço do kernel é mapeado no mesmo espaço de endereços de todos os processos**, mas inacessível em user mode. Quando uma thread faz uma syscall, o processador troca de modo (user → kernel) e pode agora acessar essa metade do espaço de endereços.

> **Windows 11 + KPTI (Kernel Page Table Isolation):** Após as vulnerabilidades Meltdown/Spectre (2018), o Windows implementou KPTI — quando em user mode, as entradas da tabela de páginas do kernel são removidas completamente. Isso impossibilita ataques especulativos, mas tem custo de performance em context switches. Em hardware moderno com proteção de página nativa (PCID), o overhead é mínimo.

---

## Páginas e paginação

A unidade básica de memória virtual é a **página** — 4 KB em x86/x64. Cada página pode estar em um de três estados:

| Estado | Significado |
|--------|-------------|
| **Free (livre)** | Não mapeada, acessar causa `ACCESS_VIOLATION` |
| **Reserved (reservada)** | O processo "reservou" o intervalo de endereços, mas não há RAM nem espaço em disco associado. Ainda não pode ser acessada. |
| **Committed (comprometida)** | Mapeada para RAM física ou para o arquivo de paginação. Pode ser acessada. |

```cpp
#include <windows.h>
#include <stdio.h>

int main() {
    // Reserva 1 GB de espaço de endereços (sem alocar RAM)
    void* base = VirtualAlloc(
        nullptr,           // deixa o sistema escolher o endereço
        1ULL * 1024 * 1024 * 1024,  // 1 GB
        MEM_RESERVE,       // APENAS reserva, não commit
        PAGE_READWRITE
    );
    printf("Reservado em: %p\n", base);
    
    // Agora commita apenas as primeiras 4 KB (1 página)
    void* prim_pagina = VirtualAlloc(
        base,              // endereço específico
        4096,              // 4 KB = 1 página
        MEM_COMMIT,        // agora sim, aloca RAM ou swap
        PAGE_READWRITE
    );
    
    // Esta página pode ser lida e escrita agora
    *(int*)prim_pagina = 42;
    printf("Valor escrito: %d\n", *(int*)prim_pagina);
    
    // Libera tudo
    VirtualFree(base, 0, MEM_RELEASE);
    return 0;
}
```

Este padrão de reservar primeiro e commitar depois é fundamental para implementar estruturas que crescem (como uma pilha de tamanho variável ou uma arena de memória). Você reserva um espaço grande logo de início para garantir um intervalo de endereços contíguo, e commita páginas conforme necessário.

### O que acontece quando uma página não está na RAM?

Quando o processador tenta acessar uma página que está marcada como committed mas não está na RAM física (foi paginada para o disco), ocorre um **page fault** (falha de página). O hardware notifica o kernel, que:

1. Localiza os dados no arquivo de paginação (pagefile.sys)
2. Encontra um frame de RAM livre (expulsando outra página se necessário)
3. Lê os dados do disco para esse frame
4. Atualiza as tabelas de página
5. Retorna para a instrução que causou o fault — ela é re-executada, desta vez encontrando a página na RAM

Tudo isso é transparente para o programa. O custo, porém, é significativo: acessar o disco é milhares de vezes mais lento que acessar a RAM.

---

## Rings do processador: user mode vs kernel mode

Os processadores x86/x64 implementam quatro níveis de privilégio chamados **rings** (anéis):

```
Ring 0 (mais privilegiado) — Kernel mode
Ring 1 ┐
Ring 2 ┘ — Não usados pelo Windows
Ring 3 (menos privilegiado) — User mode
```

O Windows usa apenas Ring 0 (kernel) e Ring 3 (user mode). Essa separação é imposta em hardware:

- **Ring 0 (Kernel mode)**: pode executar qualquer instrução privilegiada. Pode acessar qualquer endereço de memória física. Pode programar hardware. Pode desabilitar interrupções. Um bug aqui causa blue screen (BSOD) ou compromete todo o sistema.

- **Ring 3 (User mode)**: restrito. Não pode acessar portas de I/O diretamente. Não pode modificar tabelas de página. Não pode acessar o espaço de endereços do kernel. Um bug aqui causa no máximo um crash do próprio processo — outros processos continuam rodando.

```
                  RING 3 (User Mode)
  ┌──────────────────────────────────────────┐
  │  Seu programa, DLLs, C runtime           │
  │  kernel32.dll → ntdll.dll                │
  └────────────────┬─────────────────────────┘
                   │  syscall instruction
                   ▼
  ┌──────────────────────────────────────────┐
  │  RING 0 (Kernel Mode)                    │
  │  ntoskrnl.exe, drivers (.sys), HAL       │
  └──────────────────────────────────────────┘
```

### A transição user → kernel: a syscall

Quando código em user mode precisa de um serviço do kernel (abrir um arquivo, criar um processo, alocar memória), ele executa a instrução `syscall` (x64) ou `sysenter` (x86 legado). Esta instrução:

1. Salva o estado do processador (RIP, RSP, RFLAGS)
2. Troca para o stack do kernel da thread atual
3. Muda o ring de 3 para 0
4. Salta para o handler de syscall do kernel (registrado na Model Specific Register LSTAR)

O handler do kernel valida os argumentos (todos os dados vêm de user mode e são potencialmente maliciosos), executa a operação, e retorna via `sysret`.

Você pode ver números de syscall com WinDbg:
```
kd> u ntdll!NtCreateFile
ntdll!NtCreateFile:
00007ffa`d0e51dd0 4c8bd1          mov     r10,rcx
00007ffa`d0e51dd3 b855000000      mov     eax,55h  ← número da syscall NtCreateFile
00007ffa`d0e51dd8 f604250803fe7f01 test byte ptr [SharedUserData+0x308],1
...
00007ffa`d0e51de4 0f05            syscall
```

O número `0x55` é o índice na System Service Descriptor Table (SSDT) — a tabela que o kernel mantém com ponteiros para cada função. Malware frequentemente tenta sobrescrever esta tabela para interceptar chamadas de sistema (Kernel Patch Protection / PatchGuard no Windows 64-bit detecta e bloqueia isso).

---

## DEP / NX — No-Execute Protection

O DEP (Data Execution Prevention) impede que regiões de memória marcadas como dados sejam executadas como código. É implementado pelo bit NX (No-eXecute) nas entradas da tabela de páginas.

```cpp
// Alocando memória executável (necessário para JIT compilers, por exemplo)
void* mem = VirtualAlloc(
    nullptr,
    4096,
    MEM_COMMIT | MEM_RESERVE,
    PAGE_EXECUTE_READWRITE   // explicitamente marcado como executável
);

// Tentar executar código em PAGE_READWRITE
// → ACCESS_VIOLATION (DEP)

// Tentar escrever em PAGE_EXECUTE_READ
// → ACCESS_VIOLATION
```

DEP é habilitado por padrão no Windows 11 para todos os processos. Pode ser configurado no controle de fluxo de dados:

```
Início rápido: Sistema → Proteções adicionais → DEP
               (Settings → System → Advanced → Performance → DEP)
```

No WinDbg, a proteção de uma página aparece como atributos na listagem `!vtop` e `!pte`.

---

## Inspecionando o espaço de memória de um processo

```cpp
#include <windows.h>
#include <psapi.h>
#include <stdio.h>

void inspecionarMemoria(HANDLE hProc) {
    MEMORY_BASIC_INFORMATION mbi;
    LPVOID endereco = nullptr;
    
    while (VirtualQueryEx(hProc, endereco, &mbi, sizeof(mbi)) == sizeof(mbi)) {
        if (mbi.State == MEM_COMMIT) {
            const char* tipo = "???";
            if (mbi.Type == MEM_IMAGE) tipo = "IMAGE";      // código de DLL/EXE
            else if (mbi.Type == MEM_MAPPED) tipo = "MAPPED"; // file mapping
            else if (mbi.Type == MEM_PRIVATE) tipo = "PRIV"; // heap, stack
            
            const char* prot = "?";
            switch (mbi.Protect & 0xFF) {
                case PAGE_READONLY:          prot = "R--"; break;
                case PAGE_READWRITE:         prot = "RW-"; break;
                case PAGE_EXECUTE_READ:      prot = "R-X"; break;
                case PAGE_EXECUTE_READWRITE: prot = "RWX"; break;
                case PAGE_NOACCESS:          prot = "---"; break;
            }
            
            printf("%p  tamanho=%8zu KB  %s  %s\n",
                mbi.BaseAddress,
                mbi.RegionSize / 1024,
                tipo, prot);
        }
        
        // Avança para a próxima região
        endereco = (LPBYTE)mbi.BaseAddress + mbi.RegionSize;
    }
}

int main() {
    inspecionarMemoria(GetCurrentProcess());
    return 0;
}
```

Este é o mesmo princípio que ferramentas como Process Explorer, VMMap e Winpmem usam para mapear o espaço de memória de um processo.

---

## Pools do kernel: paginado vs não-paginado

No espaço do kernel, a memória é alocada de dois "pools":

| Pool | Pode ser paginado? | Uso |
|------|-------------------|-----|
| **Nonpaged Pool** | Nunca | Drivers que operam em IRQL elevado (interrupções), estruturas que precisam sempre estar na RAM |
| **Paged Pool** | Sim | Maioria dos objetos do kernel, drivers normais |

```
kd> !poolused 2
   NonPagedPool:  [tamanho em uso por tag]
   PagedPool:     [tamanho em uso por tag]
```

Cada alocação de pool tem um **tag** de 4 bytes para diagnóstico:
```c
// Em um driver:
PVOID mem = ExAllocatePool2(POOL_FLAG_NON_PAGED, sizeof(MINHA_ESTRUTURA), 'MINE');
// 'MINE' é o tag — aparece em !poolused, Driver Verifier, etc.
```

Quando o Nonpaged Pool se esgota, o sistema entra em pânico (bugcheck `POOL_EMPTY`). Um dos vetores clássicos de DoS era esgotar o nonpaged pool via handles ou objetos de kernel — as versões modernas do Windows têm limites por processo para mitigar isso.

---

## Como o Process Explorer mostra memória

No Process Explorer:
- Coluna **Working Set**: RAM física que o processo está usando agora
- Coluna **Private Bytes**: memória commited que só pertence a este processo
- Coluna **Virtual Size**: todo o espaço de endereços virtual reservado + committado

A diferença entre Working Set e Private Bytes mostra quanto foi paginado para disco. Se Working Set << Private Bytes, o processo está sendo paginado e provavelmente sofrerá page faults em breve.

A ferramenta **VMMap** (também do Sysinternals) mostra um mapa completo do espaço de endereços, região por região, com o tipo de conteúdo de cada uma.

---

*Próximo: [Hipervisor e Virtualization Based Security](capitulo-01-06-hipervisor-vbs.md)*
