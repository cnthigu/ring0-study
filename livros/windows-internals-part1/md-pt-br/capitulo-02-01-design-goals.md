# Capítulo 2 — Objetivos de Design e Modelo do Sistema

> Livro didático baseado em *Windows Internals, 7th Edition*.
> Atualizado para Windows 11 com exemplos práticos em C++ moderno.
> Repositório: [ring0-study](https://github.com/cnthigu/ring0-study)

> Navegação: [Índice Cap.2](capitulo-02.md) | [Próximo](capitulo-02-02-arquitetura-overview.md)

---

## As perguntas que moldaram o Windows NT

Em 1989, quando David Cutler começou a projetar o Windows NT do zero, ele fez uma aposta arriscada: construir um sistema operacional completamente novo, sem herdar o código do MS-DOS ou do Windows 3.x. Para guiar esse esforço, a equipe estabeleceu requisitos e metas de design que ainda são visíveis na arquitetura do Windows 11 hoje.

Entender esses objetivos originais não é arqueologia — é a chave para entender *por que* o Windows funciona da forma que funciona.

---

## Requisitos originais do Windows NT (1989)

Os requisitos que guiaram a especificação inicial:

1. **32-bit preemptivo com memória virtual** — qualquer thread pode ser interrompida pelo scheduler a qualquer momento; nenhum programa pode "segurar" o sistema
2. **Múltiplas arquiteturas de hardware** — não ficar preso ao x86; a primeira versão rodou em x86 e MIPS simultaneamente
3. **Escalar bem em sistemas SMP** — desde o início projetado para múltiplos processadores
4. **Plataforma de computação distribuída** — excelente como cliente e servidor de rede
5. **Rodar aplicações 16-bit (MS-DOS e Windows 3.1)** — compatibilidade com o ecossistema existente
6. **Conformidade POSIX 1003.1** — requisito do governo americano para contratos federais
7. **Segurança C2 (Orange Book)** — também requisito governamental
8. **Unicode** — suporte global a caracteres desde o início, não como addon

## Metas de design

Para tomar as milhares de decisões cotidianas do desenvolvimento, a equipe adotou cinco metas:

| Meta | Significado prático |
|------|-------------------|
| **Extensibilidade** | O código deve crescer sem reescritas; arquitetura em camadas que você pode adicionar sem quebrar o resto |
| **Portabilidade** | Rodar em diferentes arquiteturas com mudanças mínimas; isolado em módulos separados (kernel, HAL) |
| **Confiabilidade e robustez** | Aplicações não podem derrubar o OS ou outras aplicações; isolamento de falhas |
| **Compatibilidade** | APIs compatíveis com Windows mais antigos; interoperabilidade com UNIX, OS/2, NetWare |
| **Performance** | O mais rápido possível dentro dos outros constraints |

Essas metas entram em conflito entre si o tempo todo. Compatibilidade adiciona código legado que prejudica performance. Portabilidade exige abstrações que têm custo. Confiabilidade exige verificações que têm overhead. As decisões de design do Windows são frequentemente compromissos deliberados entre essas metas — e entender as metas ajuda a entender por que os compromissos foram feitos daquela forma.

---

## O modelo de OS: monolítico ou microkernel?

Nos anos 90, havia um debate acadêmico intenso entre dois modelos de design de OS:

### Microkernel

O kernel fornece apenas os mecanismos mais básicos (scheduling, IPC, gerenciamento de memória física). Todo o resto — sistema de arquivos, drivers, pilha de rede — roda em processos de usuário separados:

```
┌────────────────────────────────────────────────────┐
│  User Mode                                         │
│  ┌─────────┐  ┌──────────┐  ┌─────────────────┐   │
│  │ Driver  │  │ FS Driver│  │  Network Stack  │   │
│  │ Process │  │ Process  │  │  Process        │   │
│  └────┬────┘  └────┬─────┘  └────────┬────────┘   │
│       └────────────┴─────────────────┘             │
│                    │ IPC                            │
├────────────────────┼───────────────────────────────┤
│  Kernel Mode       │                               │
│         ┌──────────┴──────────┐                    │
│         │   Microkernel       │                    │
│         │   (scheduling, IPC) │                    │
│         └─────────────────────┘                    │
└────────────────────────────────────────────────────┘
```

Vantagem: um driver com bug crasha seu processo, não o sistema inteiro.
Desvantagem: cada chamada que atravessa componentes envolve IPC e troca de contexto — pesado demais para performance real.

### Monolítico (o que o Windows usa)

O kernel, drivers, sistema de arquivos e pilha de rede compartilham o mesmo espaço de endereços de kernel mode:

```
┌────────────────────────────────────────────────────┐
│  User Mode — Aplicações, Subsistemas               │
├────────────────────────────────────────────────────┤
│  Kernel Mode — UM ÚNICO ESPAÇO DE ENDEREÇOS        │
│                                                    │
│  ┌──────────────────────────────────────────────┐  │
│  │ ntoskrnl.exe  │ Drivers (.sys) │ Win32k.sys  │  │
│  │ (Executive +  │ (filesystem,   │ (janelas,   │  │
│  │  Kernel)      │  rede, HW)     │  GDI)       │  │
│  └──────────────────────────────────────────────┘  │
└────────────────────────────────────────────────────┘
```

Vantagem: drivers chamam funções do kernel com custo de uma chamada de função simples — performance máxima.
Desvantagem: um driver com bug pode corromper estruturas do kernel e causar BSOD.

### O meio-termo do Windows

O Windows faz o que a maioria dos OSes modernos faz: é tecnicamente monolítico (tudo compartilha o mesmo espaço kernel mode), mas usa técnicas para minimizar a desvantagem:

1. **KMCS (Kernel Mode Code Signing)**: drivers devem ser assinados pela Microsoft. Reduz o número de drivers de origem desconhecida que chegam ao kernel.

2. **WHQL (Windows Hardware Quality Labs)**: drivers passam por testes de certificação. Não garante a ausência de bugs, mas reduz os mais óbvios.

3. **VBS + HVCI**: o hipervisor protege o kernel de drivers que tentam modificar código do OS em memória. Um driver bugado pode ainda causar BSOD, mas tem muito mais dificuldade em comprometer propositalmente a segurança do sistema.

4. **Driver Verifier**: ferramenta que habilita verificações extras em tempo de desenvolvimento, detectando bugs comuns em drivers antes que cheguem a produção.

5. **UMDF (User-Mode Driver Framework)**: para certos tipos de drivers (USB, câmeras, impressoras), permite escrever o driver como um processo user mode. Um crash mata o processo, não o sistema.

---

## Princípios orientados a objetos sem linguagem orientada a objetos

O Windows foi escrito principalmente em C — não em C++. Mas o código interno segue princípios orientados a objetos de forma deliberada:

- **Encapsulamento**: componentes do kernel não acessam diretamente as estruturas internas de outros componentes. Usam interfaces formais (funções). O Memory Manager não "invade" as estruturas do Process Manager — chama funções públicas.

- **Interfaces formais**: cada componente exporta um conjunto bem definido de funções. Outros componentes chamam essas funções, não acessam os dados diretamente.

- **Objetos do kernel**: quase todos os recursos do sistema são representados como objetos com operações bem definidas, contagem de referências e controle de acesso (vimos isso no Capítulo 1).

Por que C em vez de C++? Portabilidade. Em 1989, C era universal. Compiladores C++ eram inconsistentes entre plataformas e arquiteturas. C garantia que o mesmo código compilaria em x86, MIPS, Alpha e PowerPC sem surpresas.

Hoje, partes do Windows moderno (especialmente componentes mais novos e alguns drivers) são escritas em C++. Mas a base do ntoskrnl.exe ainda é majoritariamente C.

---

## O resultado: 30 anos de legado bem-sucedido

As metas de 1989 funcionaram tão bem que a mesma base de código — evolução direta do Windows NT 3.1 — roda hoje no Windows 11, no Xbox Series X, no HoloLens 2 e em dispositivos IoT industriais. Isso é uma das histórias de longevidade de software mais notáveis da computação moderna.

Quando você encontra um comportamento "estranho" no Windows — uma API que parece inconsistente, um mecanismo que parece desnecessariamente complexo — frequentemente a explicação está numa dessas metas originais. Compatibilidade com 30 anos de software legado tem um custo visível.

---

*Próximo: [Visão Geral da Arquitetura](capitulo-02-02-arquitetura-overview.md)*
