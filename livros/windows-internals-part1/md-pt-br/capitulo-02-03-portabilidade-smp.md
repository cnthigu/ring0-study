# Capítulo 2 — Portabilidade, SMP e Escalabilidade

> Livro didático baseado em *Windows Internals, 7th Edition*.
> Atualizado para Windows 11 com exemplos práticos em C++ moderno.
> Repositório: [ring0-study](https://github.com/cnthigu/ring0-study)

> Navegação: [Anterior](capitulo-02-02-arquitetura-overview.md) | [Índice Cap.2](capitulo-02.md) | [Próximo](capitulo-02-04-vbs-arquitetura.md)

---

## Como o Windows roda em múltiplas arquiteturas

O Windows 11 roda nativamente em x64 (Intel/AMD), ARM64 (Qualcomm Snapdragon, Apple Silicon com Parallels, etc.) e, pelo menos para versões IoT, em x86. O mesmo sistema operacional — sem bifurcações de código base — suporta tudo isso.

Como? Com dois mecanismos complementares:

### 1. Design em camadas: kernel e HAL separam o que varia

```
┌─────────────────────────────────────────┐
│      Código independente de arquitetura │
│  (maioria do executive, drivers, etc.)  │
├─────────────────────────────────────────┤
│  Kernel (ntoskrnl.exe)                  │
│  • Funções arquitetura-específicas      │
│    (context switch, trap handling)      │
│  • Funções idênticas entre arquiteturas │
│    (scheduling algorithm, object mgr)   │
├─────────────────────────────────────────┤
│  HAL (hal.dll)                          │
│  • Variações dentro da mesma arquitetura│
│    (APIC vs outros interrupt controllers│
│     timers, DMA controllers)            │
├─────────────────────────────────────────┤
│      HARDWARE                           │
└─────────────────────────────────────────┘
```

O **kernel** implementa as partes que diferem *entre* arquiteturas — context switching em x64 salva registradores diferentes dos que ARM64 usa. Mas o *algoritmo* de scheduling em si é o mesmo: está no código não-específico do kernel.

O **HAL** implementa o que varia *dentro* da mesma arquitetura — placas-mãe x64 diferentes podem ter controladores de interrupção diferentes, timers diferentes. O HAL é a cola que esconde essas diferenças.

### 2. C como linguagem portátil

A vasta maioria do Windows é escrita em C. Assembly é usado apenas para:
- Código que acessa registradores ou instruções privilegiadas que não têm representação em C (como o handler inicial de interrupção)
- Código extremamente sensível a performance (como o inner loop do context switch)

Resultado: um driver bem escrito é compilado para x64 e ARM64 sem modificações — desde que não use assembly inline desnecessário e chame HAL em vez de tocar hardware diretamente.

---

## Symmetric Multiprocessing (SMP)

O Windows foi projetado para SMP desde o início — em contraste com muitos OSes que adicionaram suporte a múltiplos processadores como afterthought.

### SMP vs ASMP

```
SMP (Symmetric Multiprocessing)      ASMP (Asymmetric)
                                     
CPU 0 ─── executa: OS kernel         CPU 0 ─── SEMPRE o OS kernel
           ou código de app                     
CPU 1 ─── executa: OS kernel         CPU 1 ─── APENAS código de app
           ou código de app          CPU 2 ─── APENAS código de app
CPU N ...                                      
                                     
Qualquer thread (kernel ou app)      OS fica "preso" num processador;
pode rodar em qualquer CPU.          apps sempre nos outros.
```

No Windows (SMP):
- Não há "CPU master" — qualquer CPU pode executar código do kernel
- O scheduler pode mover threads entre CPUs (respeitando afinidade e topologia)
- Múltiplas threads do kernel podem rodar em paralelo em CPUs diferentes simultaneamente

Isso exige que o código do kernel seja **thread-safe** — o que é muito mais difícil de escrever que código para um único processador. Cada estrutura de dados compartilhada precisa de proteção adequada. É por isso que entender spinlocks, mutexes do kernel e IRQLs é essencial para desenvolvedores de drivers.

---

## Os quatro modelos de hardware multiprocessador

### 1. Multicore

Múltiplos núcleos físicos no mesmo pacote (chip). Cada núcleo tem seus próprios registradores e pipeline, mas podem compartilhar caches L3. O Windows os trata como processadores independentes para fins de scheduling.

```
┌────────────────────────────────────┐
│          CPU Package               │
│  ┌──────┐ ┌──────┐ ┌──────┐       │
│  │Core 0│ │Core 1│ │Core 2│ ...   │
│  └──┬───┘ └──┬───┘ └──┬───┘       │
│     └────────┴────┬───┘           │
│               ┌───┴───┐           │
│               │ L3 $  │ (shared)  │
│               └───────┘           │
└────────────────────────────────────┘
```

### 2. SMT — Simultaneous Multithreading

Intel chama de Hyper-Threading; AMD implementação similar. Um núcleo físico apresenta **dois processadores lógicos** para o OS:

```
Núcleo físico 0:
  ├── Logical CPU 0 (Thread 0) ← CPU 0 para o Windows
  └── Logical CPU 1 (Thread 1) ← CPU 1 para o Windows

Cada logical CPU tem:
  ✓ Seu próprio estado de registradores (RIP, RSP, RFLAGS...)
  ✗ Compartilha: execution units, caches L1/L2
```

Quando Thread 0 stalla (cache miss, branch misprediction), Thread 1 usa as unidades de execução que ficariam ociosas. Na prática: ~20-30% de ganho de throughput por par de SMT.

O scheduler do Windows sabe da topologia SMT: prefere alocar threads em CPUs logicas de diferentes núcleos físicos antes de colocar duas threads no mesmo núcleo (pois elas competiriam pelos recursos de execução compartilhados).

### 3. NUMA — Non-Uniform Memory Access

Em servidores grandes (e alguns desktops high-end), múltiplos sockets de CPU têm RAM própria:

```
Socket 0                    Socket 1
┌───────────────────┐      ┌───────────────────┐
│  CPU 0-15         │      │  CPU 16-31        │
│  RAM Local: 64 GB │      │  RAM Local: 64 GB │
└────────┬──────────┘      └────────┬──────────┘
         └─────────────┬────────────┘
                   Interconnect
                   (QPI/UPI - mais lento)
```

Acessar RAM local → ~50-80ns latência
Acessar RAM do outro socket → ~150-200ns latência

O Windows trata NUMA como SMP, mas otimiza:
- Aloca memória preferencialmente da RAM local ao processador que a está usando
- O scheduler prefere manter threads no mesmo nó NUMA que a memória que elas estão acessando

Em WinDbg: `!numa` mostra a topologia NUMA do sistema.

### 4. Heterogeneous (big.LITTLE / big.little)

Presente em Windows ARM64 (Qualcomm, Apple Silicon). Dois tipos de núcleos:
- **Big cores** (P-cores no Intel): alta performance, alto consumo de energia
- **Little cores** (E-cores no Intel / efficiency cores): menor performance, muito mais eficiente energeticamente

O Windows 11 suporta políticas de scheduling heterogêneas — threads podem declarar sua preferência ("quero performance máxima" vs "prefiro eficiência energética"), e o scheduler coloca cada thread no tipo de núcleo adequado.

---

## Limites de processadores por edição

O Windows não tem um limite técnico fixo de CPUs — usa máscaras de afinidade do tamanho do tipo nativo (64-bit em sistemas x64 = 64 bits = 64 processadores por grupo). Para suportar sistemas com mais de 64 CPUs, o Windows usa **Processor Groups**.

| Edição | Sockets máximos | RAM máxima |
|--------|----------------|------------|
| Windows 11 Home | 1 | 128 GB |
| Windows 11 Pro/Enterprise | 2 | 6 TB |
| Windows Server 2022 Datacenter | 64 | 48 TB |

Esses limites são impostos por licença, não por limitação técnica do kernel. Estão armazenados em `%SystemRoot%\ServiceProfiles\LocalService\AppData\Local\Microsoft\WSLicense\tokens.dat`.

### Processor Groups

Um Processor Group é um conjunto de até 64 CPUs logicas representadas por uma máscara de afinidade de 64 bits. Um sistema pode ter até 20 grupos, suportando até 640 CPUs logicas.

```cpp
// Obtendo o número de processor groups
WORD grupos = GetMaximumProcessorGroupCount();
printf("Processor groups: %u\n", grupos);

// Obtendo CPUs num grupo específico
DWORD cpusNoGrupo = GetMaximumProcessorCount(0); // grupo 0
printf("CPUs no grupo 0: %u\n", cpusNoGrupo);
```

Para código legacy que não conhece grupos, o Windows apresenta apenas as CPUs do grupo 0 — garantindo compatibilidade.

---

## Scalability: o que torna o Windows eficiente em muitos cores

Adicionar CPUs não é garantia de performance. Para escalar bem, o kernel precisa:

### Fine-grained locking

A versão original do NT tinha alguns locks globais que serializavam operações mesmo em múltiplos processadores. Ao longo das versões:

- **Windows Server 2003**: filas de scheduling por-CPU com lock fino (thread scheduling em paralelo)
- **Windows 7**: eliminou locks globais em operações de wait/dispatch
- **Windows 8+**: pool allocators por-CPU, sem lock em hot paths
- **Windows 10/11**: objetos de sincronização otimizados para NUMA

A tendência geral foi substituir locks globais por locks por-CPU ou lock-free data structures onde possível.

### I/O Completion Ports

Fundamentais para servidores escaláveis. Em vez de um thread por conexão (que não escala), usa um pool de threads que processa completions de I/O de muitas conexões:

```cpp
// Criando um IO Completion Port
HANDLE hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 0);

// Associando um socket ao IOCP
CreateIoCompletionPort((HANDLE)socket, hIOCP, chaveContexto, 0);

// Thread worker pega completions conforme chegam
while (true) {
    DWORD bytes;
    ULONG_PTR chave;
    LPOVERLAPPED ov;
    
    GetQueuedCompletionStatus(hIOCP, &bytes, &chave, &ov, INFINITE);
    // processar a I/O completada...
}
```

O kernel entrega completions às threads do pool de forma eficiente, minimizando context switches. Um servidor com 4 threads atendendo 10.000 conexões é viável com IOCPs — impossível com modelo thread-per-connection.

---

## Client vs Server: o mesmo kernel, comportamentos diferentes

Windows 11 Home e Windows Server 2022 Datacenter usam o mesmo `ntoskrnl.exe`. A diferença está em configurações que o kernel aplica em boot baseado no tipo de produto:

| Configuração | Client (Windows 11) | Server (Server 2022) |
|-------------|---------------------|---------------------|
| Otimização principal | Responsividade interativa | Throughput de servidor |
| Quantum de thread | Curto (para UI responsiva) | Longo (para servidor) |
| Working set management | Mais agressivo em limpar | Menos agressivo |
| Heap do OS e pools | Menores | Maiores |
| Threads de worker do sistema | Menos | Mais |
| Conexões simultâneas (file/print) | Máximo 10 | Ilimitado |

O registry identifica o tipo:
```
HKLM\SYSTEM\CurrentControlSet\Control\ProductOptions
  ProductType = "WinNT"       → cliente
  ProductType = "ServerNT"    → servidor
  ProductType = "LanmanNT"    → servidor de domínio
```

---

*Próximo: [VBS e Arquitetura de Segurança](capitulo-02-04-vbs-arquitetura.md)*
