# Capitulo 11 - Cache e Sistemas de Arquivos

> *Traducao do livro Windows Internals, Part 2, Seventh Edition*
> *Andrea Allievi, Alex Ionescu, Mark E. Russinovich, David A. Solomon*
> *Microsoft Press, 2022*

---

Este capitulo mostra como os componentes de armazenamento mais importantes do Windows - o gerenciador de cache e os drivers de sistema de arquivos - interagem para fornecer a capacidade de trabalhar com arquivos, diretorios e dispositivos de disco de forma eficiente e a prova de falhas. O capitulo tambem apresenta os sistemas de arquivos suportados pelo Windows, com detalhes especiais sobre NTFS e ReFS.

---

## Terminologia

Antes de examinar como o gerenciador de cache do Windows funciona, e util definir alguns termos:

- **Cache de arquivo:** Uma area da memoria fisica que contem dados de arquivo copiados do disco.
- **Cache hit:** Quando os dados solicitados ja estao no cache e nao precisam ser lidos do disco.
- **Cache miss:** Quando os dados solicitados nao estao no cache e precisam ser lidos do disco.
- **Write-back caching:** Escritas vao primeiro para o cache e sao gravadas no disco mais tarde (de forma lazy).
- **Write-through caching:** Escritas vao simultaneamente para o cache e para o disco.
- **Lazy writing:** O processo de gravar dados do cache para o disco em um momento posterior.
- **Read-ahead:** Leitura antecipada de dados do disco para o cache antes de serem solicitados.
- **Dirty page:** Uma pagina do cache que foi modificada e precisa ser gravada no disco.
- **Clean page:** Uma pagina do cache que e identica ao conteudo correspondente no disco.

---

## Caracteristicas principais do gerenciador de cache

O gerenciador de cache do Windows tem varias caracteristicas importantes que o diferenciam de outros sistemas operacionais.

### Cache centralizado unico

O Windows usa um **cache centralizado unico** para todos os dados de arquivo. Isso significa que apenas uma copia de um dado bloco de arquivo existe na memoria, independente de quantos processos estejam acessando o arquivo. Isso contrasta com abordagens onde cada processo tem seu proprio cache privado.

O cache centralizado permite:

- Menor uso de memoria (sem duplicacao).
- Compartilhamento implicito entre processos.
- Gerenciamento de consistencia mais simples.

### O gerenciador de memoria

O gerenciador de cache do Windows nao implementa seu proprio mecanismo de gerenciamento de memoria. Em vez disso, ele usa o **gerenciador de memoria do Windows** para mapear visualizacoes de arquivos em memoria e confiar no mecanismo de paginacao para decidir quais paginas de cache manter na memoria.

O cache e essencialmente um conjunto de **secoes do gerenciador de memoria** (file-backed sections) - objetos de memoria mapeada que representam visualizacoes de arquivos.

### Coerencia do cache

O gerenciador de cache garante que todos os acessos ao mesmo arquivo vejam dados consistentes, independente de como o arquivo foi aberto:

- Acesso via `ReadFile`/`WriteFile` usa o cache.
- Acesso via mapeamento de memoria (memory-mapped files) tambem usa o mesmo cache.
- Acessos diretos de I/O (nao cache) bypassam o gerenciador de cache mas ainda precisam ser coordenados.

### Cache de bloco virtual

O gerenciador de cache usa **cache de bloco virtual** - o cache e organizado em termos de offsets de arquivo (posicoes no arquivo), nao em termos de blocos de disco fisicos. Isso simplifica o cache para sistemas de arquivos que podem ter layouts de disco complexos.

### Cache baseado em stream

O cache e organizado por **stream** - a unidade fundamental de dados de arquivo. Em NTFS, um arquivo pode ter multiplos streams (o stream de dados principal e streams de dados alternativos). Cada stream e cacheado separadamente.

### Suporte a sistemas de arquivos recuperaveis

O gerenciador de cache trabalha em conjunto com o sistema de arquivos para suportar a recuperacao apos falhas. Ele expoe interfaces que permitem ao sistema de arquivos:

- Bloquear paginas de cache na memoria enquanto uma transacao esta em progresso.
- Forcar a gravacao de dados de cache no disco quando necessario para garantir durabilidade.

### Melhorias no MFT working set do NTFS

O gerenciador de cache tem suporte especial para o **Master File Table (MFT)** do NTFS - a estrutura de metadados mais critica do NTFS. O Windows tenta manter o maior numero possivel de paginas do MFT no working set do cache para minimizar faltas de pagina durante operacoes de arquivo.

---

## Gerenciamento de memoria virtual do cache

O gerenciador de cache usa a memoria virtual para mapear os dados de arquivo. Internamente, o cache consiste em um conjunto de **visualizacoes de mapeamento** (mapped views) de secoes de arquivo.

### Tamanho do cache

O tamanho do cache e dinamico e controlado pelo gerenciador de memoria. Nao existe um tamanho fixo de cache configuravel. Em vez disso:

- O gerenciador de memoria aloca memoria para o cache conforme necessario.
- Quando a memoria se torna escassa, o gerenciador de memoria pode recuperar paginas de cache limpas (sem perda de dados).
- Paginas sujas do cache sao gravadas no disco antes de serem liberadas.

#### Tamanho virtual do cache

O **tamanho virtual do cache** e a quantidade total de espaco de enderecamento virtual alocado para o cache. Em sistemas de 64 bits, isso pode ser virtualmente ilimitado.

#### Tamanho do working set do cache

O **tamanho do working set do cache** e a quantidade de memoria fisica realmente usada pelo cache. O gerenciador de memoria controla esse valor para equilibrar as necessidades do cache com as necessidades dos processos.

#### Tamanho fisico do cache

O **tamanho fisico do cache** e a quantidade real de RAM usada para armazenar dados de cache. Pode ser menor que o tamanho do working set se algumas paginas foram paginadas para o arquivo de paginacao.

---

## Estruturas de dados do cache

### Estruturas de dados do cache para todo o sistema

O gerenciador de cache mantem varias estruturas de dados globais:

- **CcVacbLookasideList:** Uma lookaside list de `VACB` (Virtual Address Control Block) - as estruturas basicas de mapeamento do cache.
- **CcVacbSpinLock:** Um spinlock que protege as estruturas globais do cache.
- **CcCleanSharedCacheMapList:** Uma lista de todos os `SHARED_CACHE_MAP` limpos (sem dados sujos).
- **CcLazyWriterList:** A lista de arquivos que precisam ter seus dados sujos gravados no disco.

### Estruturas de dados do cache por arquivo

Cada arquivo que esta no cache tem um **`SHARED_CACHE_MAP`** - a estrutura central do gerenciador de cache por arquivo. Ela contem:

- O tamanho atual do cache para o arquivo.
- O tamanho valido dos dados (ate onde os dados foram escritos).
- Informacoes de estado de leitura antecipada.
- Um ponteiro para o objeto de secao do gerenciador de memoria.

Cada visualizacao mapeada do cache e representada por um **`VACB` (Virtual Address Control Block)**, que mapeia uma fatia de 256 KB do arquivo para um intervalo de enderecos virtuais no espaco do cache.

---

## Interfaces do sistema de arquivos

O gerenciador de cache expoe tres interfaces para os drivers de sistema de arquivos:

### Interface de copia (Copy interface)

A interface de copia e o metodo mais simples. O sistema de arquivos chama `CcCopyRead` e `CcCopyWrite` para transferir dados entre o cache e os buffers do usuario.

- **`CcCopyRead`:** Copia dados do cache para um buffer de usuario. Se os dados nao estiverem no cache, dispara uma leitura do disco.
- **`CcCopyWrite`:** Copia dados de um buffer de usuario para o cache. Os dados sao marcados como sujos para serem gravados mais tarde.

### Interface de mapeamento e fixacao (Mapping and Pinning interface)

Esta interface e mais sofisticada e e usada principalmente por sistemas de arquivos que precisam acesso direto as paginas de cache:

- **`CcMapData`:** Mapeia um intervalo de bytes do arquivo para um ponteiro de memoria.
- **`CcPinRead`:** Fixa uma pagina do cache na memoria (impede que seja liberada) e retorna um ponteiro.
- **`CcUnpinData`:** Remove a fixacao das paginas.
- **`CcSetDirtyData`:** Marca as paginas como sujas.

Essa interface e usada pelo NTFS para acessar metadados (MFT, bitmaps, etc.) diretamente como memoria.

### Interface de acesso direto a memoria (DMA interface)

Esta interface e usada por sistemas de arquivos que querem transferir dados diretamente para/de buffers de DMA de hardware, sem copia intermediaria:

- **`CcGetVacbMdl`:** Obtem uma MDL (Memory Descriptor List) para um intervalo de dados no cache.

---

## I/O Rapido (Fast I/O)

O **Fast I/O** e um mecanismo de otimizacao que permite que operacoes de I/O simples de arquivo bypassem o overhead normal de criacao de IRP (I/O Request Packet) quando os dados ja estao no cache.

Quando um processo faz uma chamada `ReadFile` ou `WriteFile`:

1. O I/O Manager verifica se o driver de sistema de arquivos tem um vetor de Fast I/O.
2. Se sim, e se os dados estao no cache e a operacao e simples, o Fast I/O e tentado.
3. Se bem-sucedido, a operacao e concluida sem criar um IRP.
4. Se falhar (por exemplo, dados nao no cache), o caminho normal de IRP e usado.

O Fast I/O pode ser entre 5-10x mais rapido que o caminho normal de IRP para pequenas operacoes de leitura e escrita em dados cacheados.

---

## Leitura antecipada e escrita retardada (Read-ahead e Write-behind)

### Leitura antecipada inteligente

O gerenciador de cache implementa **leitura antecipada inteligente** - le dados para o cache antes de serem solicitados, baseado em padroes de acesso detectados.

O algoritmo de leitura antecipada detecta:

- **Acesso sequencial:** Se um arquivo esta sendo lido sequencialmente, le mais dados adiante.
- **Acesso com stride fixo:** Se um arquivo e lido em intervalos fixos, antecipa os proximos intervalos.
- **Acesso aleatorio:** Nenhuma leitura antecipada e feita (seria inutil).

A leitura antecipada e executada em um contexto separado (via DPC e thread de trabalho) para nao bloquear o solicitante original.

### Escrita retardada e lazy writing

O **write-back caching** e o modo padrao do Windows - escritas vao primeiro para o cache e sao gravadas no disco mais tarde. O componente responsavel por isso e o **lazy writer**.

O lazy writer e uma thread de sistema que:

1. Periodicamente verifica a lista de paginas sujas.
2. Grava dados sujos no disco em ordem.
3. Marca as paginas como limpas apos a escrita bem-sucedida.

O lazy writer tenta escrever dados em clusters maiores para maximizar a eficiencia do I/O.

#### Leitura e escrita de cache com baixa prioridade (Aggressive write behind)

O Windows implementa **I/O de baixa prioridade** para operacoes de cache que nao sao urgentes. Isso permite que operacoes de I/O normais (responsivas) preemptem operacoes de cache de baixa prioridade.

---

## Sistemas de arquivos

### Formatos de sistema de arquivos do Windows

O Windows suporta varios sistemas de arquivos:

| Sistema de arquivos | Descricao | Uso tipico |
|--------------------|-----------|-----------|
| **FAT12/FAT16** | File Allocation Table de 12/16 bits | Disquetes, dispositivos legados |
| **FAT32** | File Allocation Table de 32 bits | Discos removiveis, compatibilidade cruzada |
| **exFAT** | Extended FAT para midia removivel | Cartoes de memoria, pen drives grandes |
| **NTFS** | NT File System | Volumes de sistema e dados do Windows |
| **ReFS** | Resilient File System | Servidores, armazenamento de grande escala |
| **CDFS** | CD-ROM File System | CDs de dados |
| **UDF** | Universal Disk Format | DVDs, Blu-rays |

### CDFS

O **CDFS** e o sistema de arquivos para CD-ROM, baseado no padrao ISO 9660 com extensoes da Microsoft. E somente leitura.

### UDF

O **UDF** (Universal Disk Format) e o sistema de arquivos padrao para DVDs e Blu-rays. O Windows suporta as versoes 1.02, 1.5, 2.0, 2.01 e 2.5 do UDF.

### FAT12, FAT16 e FAT32

Os sistemas de arquivos **FAT** usam uma **File Allocation Table** - uma tabela que mapeia cada cluster de disco para o proximo cluster do mesmo arquivo (formando uma lista encadeada). FAT e simples, rapido e amplamente compativel, mas nao tem recursos como journaling, permissoes de arquivo ou suporte a arquivos grandes.

- **FAT12:** Ate ~32 MB por volume.
- **FAT16:** Ate ~4 GB por volume.
- **FAT32:** Ate ~8 TB teoricamente, mas limitado a 4 GB por arquivo.

### exFAT

O **exFAT** (Extended FAT) foi projetado para midia flash (pen drives, cartoes SD). Principais melhorias sobre FAT32:

- Suporte a arquivos maiores que 4 GB.
- Melhor desempenho em dispositivos flash.
- Suporte a permissoes de arquivo basicas.
- Eliminacao da limitacao de 512 entradas por diretorio.

---

## Arquitetura de driver de sistema de arquivos

### FSDs locais (Local FSDs)

Um **FSD local** (File System Driver) gerencia volumes diretamente acessiveis via barramento de armazenamento local (SATA, NVMe, USB, etc.). O Windows inclui os seguintes FSDs locais:

- `ntfs.sys`: Driver do NTFS
- `refs.sys`: Driver do ReFS
- `fastfat.sys`: Driver do FAT12/FAT16/FAT32
- `exfat.sys`: Driver do exFAT
- `cdfs.sys`: Driver do CDFS
- `udfs.sys`: Driver do UDF

### FSDs remotos (Remote FSDs)

Um **FSD remoto** gerencia volumes em compartilhamentos de rede. O Windows inclui:

- `mrxsmb.sys` e `mrxsmb20.sys`: Cliente SMB 1.0 e SMB 2.x/3.x para compartilhamentos Windows.
- `rdbss.sys`: Redirector Block Structure - camada comum para FSDs remotos.
- `WebClient`: Cliente WebDAV para compartilhamentos HTTP.

---

## O Sistema de Arquivos NT (NTFS)

O **NTFS** (NT File System) e o sistema de arquivos principal do Windows desde o Windows NT 3.1. Ele foi projetado para atender aos requisitos de um sistema operacional de nivel empresarial.

### Requisitos de alto nivel para sistemas de arquivos

O NTFS foi projetado com os seguintes requisitos:

- **Recuperabilidade:** O sistema de arquivos deve ser capaz de recuperar dados apos uma falha de energia ou queda do sistema.
- **Seguranca:** Cada arquivo e diretorio pode ter uma lista de controle de acesso (ACL) definindo quem pode acessa-lo.
- **Redundancia e tolerancia a falhas:** Suporte a volumes em espelho e RAID.
- **Escalabilidade:** Suporte a volumes de ate 256 TB (e maior com clusters maiores).

### Recursos avancados do NTFS

#### Multiplos streams de dados

O NTFS suporta **alternate data streams (ADS)** - um arquivo pode ter multiplos streams de dados nomeados, alem do stream de dados principal. Por exemplo:

```
arquivo.txt          <- stream de dados principal (anonimo)
arquivo.txt:metadado <- stream alternativo chamado "metadado"
```

O Windows usa ADS para varias finalidades internas, como:

- Zonas de seguranca (zone.identifier) para arquivos baixados da internet.
- Thumbnails em cache.
- Informacoes de autor e outros metadados.

> **Rode no Visual Studio**
> Este exemplo cria um arquivo com um alternate data stream e depois le seu conteudo.
>
> ```cpp
> #include <windows.h>
> #include <stdio.h>
>
> int main() {
>     // Cria um arquivo com ADS (alternate data stream)
>     HANDLE h = CreateFileW(L"teste.txt:segredo",
>         GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
>
>     if (h != INVALID_HANDLE_VALUE) {
>         const char* dados = "Dado secreto no ADS!";
>         DWORD escritos;
>         WriteFile(h, dados, (DWORD)strlen(dados), &escritos, NULL);
>         CloseHandle(h);
>         printf("ADS criado: teste.txt:segredo\n");
>     }
>
>     // Le o ADS
>     h = CreateFileW(L"teste.txt:segredo",
>         GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
>
>     if (h != INVALID_HANDLE_VALUE) {
>         char buffer[256] = {};
>         DWORD lidos;
>         ReadFile(h, buffer, sizeof(buffer) - 1, &lidos, NULL);
>         printf("Conteudo do ADS: %s\n", buffer);
>         CloseHandle(h);
>     }
>
>     DeleteFileW(L"teste.txt"); // Remove o arquivo e todos os seus ADS
>     return 0;
> }
> ```

#### Nomes baseados em Unicode

O NTFS armazena nomes de arquivo em Unicode (UTF-16), suportando qualquer caractere em qualquer idioma. O limite de comprimento de nome e 255 caracteres Unicode.

#### Indexacao geral

O NTFS tem um mecanismo de indexacao geral baseado em **B+ trees** que pode ser usado para qualquer atributo de arquivo, nao apenas nomes. Isso e a base para recursos como:

- Indice de nomes de arquivo (para busca rapida de arquivos no diretorio).
- Rastreamento de seguranca (security descriptor index).
- Indice de nomes de arquivo curtos (8.3).

#### Remapeamento dinamico de bad clusters

Se o hardware detectar um bad cluster durante uma escrita, o NTFS pode automaticamente realocar os dados para outro cluster bom e marcar o cluster ruim como inacessivel. Isso funciona em conjunto com o driver de armazenamento para lidar com realocacao de setores em nivel de hardware.

#### Hard links

Um **hard link** e uma entrada de diretorio adicional que aponta para o mesmo arquivo (o mesmo inode/MFT record). Todos os hard links de um arquivo compartilham o mesmo conteudo, atributos e contagem de referencias.

```cpp
// Cria um hard link
CreateHardLinkW(L"link.txt", L"original.txt", NULL);
```

#### Links simbolicos e juncoes

Um **link simbolico** e um arquivo especial que aponta para outro arquivo ou diretorio pelo nome. Ao contrario de hard links, pode apontar para locais em outros volumes ou mesmo em outros sistemas.

Uma **juncao** e o equivalente de link simbolico para diretorios em NTFS. E uma forma mais antiga de redirecionamento que so funciona para diretorios.

```cpp
// Cria um link simbolico (requer privilegios elevados ou Developer Mode)
CreateSymbolicLinkW(L"link_sym", L"alvo.txt", 0); // para arquivo
CreateSymbolicLinkW(L"link_dir", L"C:\\Pasta", SYMBOLIC_LINK_FLAG_DIRECTORY);
```

#### Compressao e arquivos esparsos

O NTFS suporta **compressao transparente** de arquivos - os dados sao automaticamente comprimidos/descomprimidos pelo driver NTFS. E util para arquivos com alta razao de compressao, mas tem impacto no desempenho.

**Arquivos esparsos** (sparse files) sao arquivos logicamente grandes mas que armazenam fisicamente apenas as regioes que contem dados nao nulos. Isso e util para bancos de dados e outros aplicativos que pre-alocam grandes arquivos.

> **Rode no Visual Studio**
> Este exemplo cria um arquivo esparso de 1 GB que ocupa muito menos espaco em disco.
>
> ```cpp
> #include <windows.h>
> #include <stdio.h>
>
> int main() {
>     HANDLE h = CreateFileW(L"esparso.bin",
>         GENERIC_READ | GENERIC_WRITE, 0, NULL,
>         CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
>
>     // Marca o arquivo como esparso
>     DWORD bytesRetornados;
>     DeviceIoControl(h, FSCTL_SET_SPARSE, NULL, 0, NULL, 0, &bytesRetornados, NULL);
>
>     // Define o tamanho logico como 1 GB
>     FILE_END_OF_FILE_INFO info;
>     info.EndOfFile.QuadPart = 1024LL * 1024 * 1024; // 1 GB
>     SetFileInformationByHandle(h, FileEndOfFileInfo, &info, sizeof(info));
>
>     // A regiao 0-1GB e "esparsa" (virtualmente zero, sem ocupar espaco real)
>     LARGE_INTEGER zero = {};
>     FILE_ZERO_DATA_INFORMATION zeroInfo = { zero, info.EndOfFile };
>     DeviceIoControl(h, FSCTL_SET_ZERO_DATA, &zeroInfo, sizeof(zeroInfo),
>         NULL, 0, &bytesRetornados, NULL);
>
>     // Escreve apenas no offset 500 MB
>     LARGE_INTEGER pos;
>     pos.QuadPart = 500LL * 1024 * 1024;
>     SetFilePointerEx(h, pos, NULL, FILE_BEGIN);
>     const char* dados = "Dado real no meio do arquivo esparso";
>     DWORD escritos;
>     WriteFile(h, dados, (DWORD)strlen(dados), &escritos, NULL);
>
>     printf("Arquivo esparso de 1 GB criado (ocupa muito menos espaco)\n");
>     CloseHandle(h);
>     DeleteFileW(L"esparso.bin");
>     return 0;
> }
> ```

#### Log de alteracoes (Change journal)

O NTFS mantem um **diario de alteracoes** (change journal ou USN journal - Update Sequence Number journal). Ele registra um log de todas as modificacoes no sistema de arquivos (criacao, exclusao, renomeacao, alteracao de atributos, etc.).

Aplicativos como indexadores de arquivos e software de backup usam o USN journal para detectar eficientemente o que mudou desde o ultimo backup.

---

## O driver de sistema de arquivos NTFS

O driver NTFS (`ntfs.sys`) e um dos drivers mais complexos do Windows. Ele implementa:

- O parser do formato on-disk do NTFS.
- O gerenciamento de transacoes (com o kernel Transaction Manager).
- A interface com o gerenciador de cache.
- O log de recuperacao.
- Operacoes de reparse points e links simbolicos.
- Criptografia (EFS) via callbacks do driver EFS.

---

## Estrutura on-disk do NTFS

### Volumes e clusters

Um **volume** NTFS e uma area de disco que foi formatada com NTFS. A unidade de alocacao do NTFS e o **cluster** - um grupo de setores consecutivos. O tamanho padrao de cluster para NTFS e:

| Tamanho do volume | Tamanho padrao do cluster |
|------------------|--------------------------|
| Ate 512 MB | 512 bytes |
| 512 MB - 1 GB | 1 KB |
| 1 GB - 2 GB | 2 KB |
| 2 GB - 4 GB | 4 KB |
| 4 GB+ | 4 KB (padrao) |
| >16 TB | 8 KB ate 64 KB |

### Master File Table (MFT)

A **MFT** e o coração do NTFS - ela armazena informacoes sobre todos os arquivos e diretorios no volume. Cada arquivo tem pelo menos um **MFT record** de 1 KB (por padrao) que armazena:

- Todos os atributos do arquivo (nome, timestamps, seguranca).
- Para arquivos pequenos: os dados do proprio arquivo (resident data).
- Para arquivos maiores: ponteiros para as **runs** (extensoes de dados) no disco.

A propria MFT e um arquivo especial chamado `$MFT`. Os primeiros 16 records da MFT sao reservados para arquivos de metadados do sistema:

| Record | Nome | Descricao |
|--------|------|-----------|
| 0 | `$MFT` | A propria MFT |
| 1 | `$MFTMirr` | Espelho dos primeiros 4 records da MFT |
| 2 | `$LogFile` | O arquivo de log de transacoes |
| 3 | `$Volume` | Informacoes do volume |
| 4 | `$AttrDef` | Definicoes de atributos |
| 5 | `.` (ponto) | Diretorio raiz |
| 6 | `$Bitmap` | Bitmap de clusters alocados |
| 7 | `$Boot` | O setor de boot |
| 8 | `$BadClus` | Clusters ruins |
| 9 | `$Secure` | Banco de dados de descritores de seguranca |
| 10 | `$UpCase` | Tabela de maiusculas Unicode |
| 11 | `$Extend` | Diretorio de extensoes NTFS |

### Numeros de record de arquivo

Cada arquivo e identificado por um **File Reference Number** de 64 bits que consiste em:

- **Numero de record MFT** (48 bits): O indice na MFT.
- **Numero de sequencia** (16 bits): Incrementado cada vez que o record e reutilizado apos a exclusao de um arquivo.

O numero de sequencia permite detectar referencias obsoletas - se um handle aponta para um record que foi reutilizado (numero de sequencia diferente), o handle e invalido.

### Records de arquivo

Um **record de arquivo** da MFT tem um cabecalho fixo seguido de uma lista de **atributos**. Cada atributo tem um tipo, nome opcional e dados.

Os atributos padroes de um arquivo sao:

| Tipo de atributo | Nome | Descricao |
|-----------------|------|-----------|
| `$STANDARD_INFORMATION` | - | Timestamps, flags, proprietario |
| `$FILE_NAME` | - | Nome do arquivo (um record por cada nome hardlink) |
| `$SECURITY_DESCRIPTOR` | - | ACL de seguranca (ou referencia ao $Secure) |
| `$DATA` | - (ou nome) | Conteudo do arquivo |
| `$INDEX_ROOT` | `$I30` | Indice de diretorio (raiz de B+ tree) |
| `$INDEX_ALLOCATION` | `$I30` | Blocks de indice de diretorio |

### Nomes de arquivo

Um arquivo no NTFS pode ter ate tres nomes:

1. **Nome POSIX:** O nome completo, case-sensitive, pode incluir qualquer caractere Unicode.
2. **Nome Win32:** O nome no estilo Windows, case-insensitive.
3. **Nome DOS 8.3:** Um nome curto gerado automaticamente para compatibilidade.

Em muitos casos, os nomes POSIX e Win32 sao identicos e armazenados em um unico atributo.

### Atributos residentes e nao-residentes

Um atributo e **residente** se seus dados cabem dentro do proprio record MFT de 1 KB. Para arquivos pequenos, o conteudo do arquivo inteiro pode ser residente na MFT - sem necessidade de acessar dados externos.

Um atributo e **nao-residente** se seus dados sao muito grandes para caber no record MFT. Nesse caso, o atributo armazena uma **lista de runs** (run list) - uma lista compacta de pares (offset, comprimento) descrevendo onde os dados estao no disco.

### O Change Journal (USN Journal)

O **USN Journal** armazena uma sequencia de registros de mudancas no sistema de arquivos. Cada registro contem:

- **USN (Update Sequence Number):** Um identificador monotonicamente crescente.
- **FileReferenceNumber:** O arquivo que foi modificado.
- **ParentFileReferenceNumber:** O diretorio pai.
- **Timestamp:** Quando a mudanca ocorreu.
- **Reason:** O tipo de mudanca (arquivo criado, excluido, renomeado, etc.).
- **FileName:** O nome do arquivo.

> **Rode no Visual Studio**
> Este exemplo consulta o USN Journal para ver as ultimas mudancas em um volume.
>
> ```cpp
> #include <windows.h>
> #include <winioctl.h>
> #include <stdio.h>
>
> int main() {
>     HANDLE hVol = CreateFileW(L"\\\\.\\C:",
>         GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
>         NULL, OPEN_EXISTING, 0, NULL);
>
>     if (hVol == INVALID_HANDLE_VALUE) {
>         printf("Erro: %lu (requer privilegios de admin)\n", GetLastError());
>         return 1;
>     }
>
>     USN_JOURNAL_DATA_V0 journalData = {};
>     DWORD bytesRetornados = 0;
>     if (DeviceIoControl(hVol, FSCTL_QUERY_USN_JOURNAL,
>             NULL, 0, &journalData, sizeof(journalData),
>             &bytesRetornados, NULL)) {
>         printf("USN Journal ID: %llu\n", journalData.UsnJournalID);
>         printf("Primeiro USN: %llu\n", journalData.FirstUsn);
>         printf("Ultimo USN: %llu\n", journalData.NextUsn);
>         printf("Max size: %llu bytes\n", journalData.MaximumSize);
>     } else {
>         printf("USN Journal nao ativo ou erro: %lu\n", GetLastError());
>     }
>
>     CloseHandle(hVol);
>     return 0;
> }
> ```

---

## Suporte a recuperacao do NTFS

### Design

O NTFS usa **logging** para garantir que o sistema de arquivos possa ser recuperado para um estado consistente apos uma falha de energia ou queda do sistema. Ele usa o padrao de **WAL (Write-Ahead Logging)** - todas as mudancas nos metadados sao registradas em um log antes de serem aplicadas.

O arquivo de log do NTFS e `$LogFile` - armazenado nos primeiros records da MFT.

### Logging de metadados

Apenas as mudancas nos **metadados** do NTFS (estruturas como MFT, bitmaps, indices de diretorio) sao registradas no log. O conteudo dos arquivos de dados do usuario nao e registrado (isso seria muito caro em desempenho).

### O servico de arquivo de log

O **LFS (Log File Service)** e a parte do NTFS responsavel pelo logging. Ele opera sobre o arquivo `$LogFile` e fornece:

- **LSN (Log Sequence Number):** Um identificador unico para cada registro de log.
- Operacoes atomicas de escrita no log.
- Leitura e iteracao de registros para recuperacao.

### Tipos de registros de log

O log NTFS armazena dois tipos de registros:

- **Registros de redo:** Descrevem como reAplicar uma mudanca (o que fazer apos uma falha).
- **Registros de undo:** Descrevem como reverter uma mudanca (o que fazer para cancelar).

### Recuperacao

Na inicializacao do sistema apos uma falha, o NTFS executa um processo de recuperacao em tres passagens:

**Passagem de analise:** Le o log do inicio ao fim para determinar quais transacoes estavam ativas no momento da falha e o estado das paginas de metadados.

**Passagem de redo:** Reaplica todas as mudancas confirmadas que podem nao ter chegado ao disco (as "redo" operations).

**Passagem de undo:** Desfaz as mudancas de transacoes que estavam em progresso mas nao foram confirmadas no momento da falha.

Apos as tres passagens, o volume esta em um estado consistente e pode ser montado normalmente.

---

## Sistema de arquivos criptografado (EFS)

O **EFS (Encrypting File System)** e uma extensao do NTFS que permite criptografar arquivos individuais de forma transparente. Quando um arquivo EFS e acessado pelo proprietario, o NTFS automaticamente o descriptografa antes de fornecer os dados.

A criptografia usa uma chave de criptografia de arquivo (FEK - File Encryption Key) criptografada com a chave publica do usuario.

---

## Discos com acesso direto (DAX)

Os **DAX disks** sao dispositivos de armazenamento persistente (como NVMe SSDs ou Intel Optane) que podem ser mapeados diretamente no espaco de enderecamento virtual, sem passar pelo gerenciador de cache. Isso elimina uma copia de dados desnecessaria para dispositivos de armazenamento muito rapidos.

---

## Resilient File System (ReFS)

O **ReFS** (Resilient File System) foi introduzido no Windows Server 2012 como um novo sistema de arquivos projetado para maxima integridade de dados e escalabilidade, especialmente para grandes volumes de armazenamento.

O ReFS nao foi projetado como substituto direto do NTFS (nao suporta todos os recursos do NTFS, como EFS, ADS e quotas de usuario), mas como um sistema de arquivos para cenarios especificos onde integridade e disponibilidade sao prioritarias.

### Arquitetura Minstore

O ReFS e construido sobre uma camada de armazenamento chamada **Minstore** - uma arvore B+ generica que gerencia todas as estruturas de dados do ReFS.

O Minstore foi projetado com os seguintes principios:

- **Integridade por design:** Todos os dados e metadados tem checksums.
- **Copy-on-write (CoW):** Modificacoes nao sobrescrevem dados existentes - novas copias sao criadas e depois as referencias sao atualizadas.
- **Transacional:** Todas as operacoes sao atomicas.

### Layout fisico da arvore B+

O Minstore usa uma arvore B+ onde cada pagina tem 16 KB de tamanho. Cada pagina tem um checksum armazenado em um **page table** separado, permitindo a deteccao rapida de corrupcao.

### Alocadores

O Minstore usa dois tipos de alocadores:

- **Object Table Allocator:** Gerencia a alocacao de objetos da arvore B+.
- **Container Allocator:** Gerencia a alocacao de clusters no volume para armazenar os dados.

### Arquitetura do ReFS

O ReFS em si e construido sobre o Minstore e adiciona:

- **Namespace:** O sistema de diretórios e nomes de arquivo.
- **Security:** ACLs e controle de acesso.
- **Data streams:** O stream de dados de arquivo.
- **Metadata tables:** Tabelas de objetos, IDs, etc.

---

## Storage Spaces

O **Storage Spaces** e uma tecnologia de virtualizacao de armazenamento do Windows que permite agrupar multiplos discos fisicos em um **storage pool** e criar **virtual disks** (storage spaces) sobre esse pool.

Os Storage Spaces fornecem:

- **Resiliencia:** Espelhamento (mirror) ou paridade (parity) para proteger contra falha de disco.
- **Thin provisioning:** Os virtual disks podem ser maiores que o espaco fisico disponivel.
- **Tiering:** Combinacao de discos SSD e HDD, com dados quentes automaticamente movidos para SSD.

---

## Conclusao

Este capitulo examinou a arquitetura de cache e sistemas de arquivos do Windows em profundidade.

O gerenciador de cache fornece uma implementacao eficiente de cache de arquivo baseada no gerenciador de memoria do Windows, com recursos como leitura antecipada inteligente, lazy writing e Fast I/O que minimizam o impacto no desempenho.

O NTFS continua sendo o sistema de arquivos mais completo para sistemas Windows, com recursos avancados como journaling transacional, EFS, alternate data streams, links simbolicos e suporte a arquivos esparsos. A sua arquitetura de recuperacao em tres passagens garante consistencia apos falhas.

O ReFS complementa o NTFS para cenarios de armazenamento de grande escala onde integridade e resiliencia sao prioridades, usando sua arquitetura Minstore baseada em copy-on-write e checksums pervasivos.

No proximo e ultimo capitulo, examinaremos o processo de inicializacao e desligamento do Windows.
