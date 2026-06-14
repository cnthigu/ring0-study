# Capítulo 1 — Conceitos e Ferramentas: Introdução e Contexto

> Livro didático baseado em *Windows Internals, 7th Edition* (Yosifovich, Ionescu, Russinovich, Solomon).
> Conteúdo reescrito e atualizado para Windows 11, com exemplos práticos em C++ moderno.
> Repositório: [ring0-study](https://github.com/cnthigu/ring0-study)

> Navegação: [Índice](README.md) | [Próximo — APIs do Windows](capitulo-01-02-apis-windows.md)

---

## Por que estudar Windows Internals?

Antes de mergulhar em estruturas de dados e mecanismos do kernel, vale a pena responder a pergunta mais importante: **por que isso importa para mim?**

Imagine que você está depurando um travamento intermitente num servidor de produção. O código da sua aplicação está correto, os testes passam, mas em carga o processo simplesmente congela. Sem entender como o scheduler do Windows decide quem roda quando, como handles funcionam ou o que acontece quando memória virtual é paginada para disco, você está olhando para um painel de controle sem saber o que os botões fazem.

Ou imagine que você quer proteger sua aplicação contra injeção de DLL, ou entender por que um determinado antivírus interfere no seu driver, ou por que a sua ferramenta de segurança precisa de um certificado EV assinado pela Microsoft para carregar no kernel do Windows 11. Todas essas perguntas têm respostas nos internos do sistema.

Estudar Windows Internals serve a três tipos de profissionais:

- **Desenvolvedores de software**: entendem por que determinadas APIs existem, como o sistema gerencia seus recursos e como escrever código que coopera bem com o OS — em vez de brigar com ele.
- **Pesquisadores de segurança**: descobrem como exploits funcionam, onde existem superfícies de ataque e quais mecanismos de defesa o Windows oferece.
- **Administradores e engenheiros de infraestrutura**: entendem o comportamento de desempenho do sistema, diagnosticam problemas complexos e tomam decisões de configuração com base em fatos, não em suposições.

Este livro cobre o **Windows 11** como plataforma de referência. Quando mencionamos comportamentos específicos de versão, explicitamos. A base — o kernel, o gerenciador de memória, o agendador, os mecanismos de segurança — é fundamentalmente a mesma desde o Windows NT 3.1, mas com décadas de evolução que este livro vai destrinchar.

---

## Windows 11: a plataforma de referência

O Windows passou por uma transformação significativa de nomenclatura. O livro original (2017) dizia "não haverá Windows 11". Isso envelheceu mal: o Windows 11 foi lançado em outubro de 2021.

Internamente, o Windows 11 é a continuação direta do Windows 10. Versão interna: **10.0.22000** em diante. Não é um sistema operacional novo — é o mesmo kernel OneCore com novas políticas de UI e requisitos de hardware mais rígidos (TPM 2.0 obrigatório, Secure Boot obrigatório, HVCI habilitado por padrão).

Para verificar a versão exata do seu Windows hoje:

```cpp
#include <windows.h>
#include <VersionHelpers.h>
#include <stdio.h>

int main() {
    // GetVersionEx() foi DEPRECIADA desde Windows 8 — nunca use isso
    // A forma correta é a RTL_OSVERSIONINFOW via ntdll
    RTL_OSVERSIONINFOW info = { sizeof(info) };
    
    using RtlGetVersionFn = NTSTATUS(WINAPI*)(PRTL_OSVERSIONINFOW);
    auto RtlGetVersion = reinterpret_cast<RtlGetVersionFn>(
        GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "RtlGetVersion")
    );
    
    if (RtlGetVersion && RtlGetVersion(&info) == 0) {
        printf("Windows %lu.%lu, Build %lu\n",
            info.dwMajorVersion,
            info.dwMinorVersion,
            info.dwBuildNumber);
    }
    return 0;
}
```

> **Por que `RtlGetVersion` em vez de `GetVersionEx`?**
> 
> `GetVersionEx` retorna números falsos desde o Windows 8.1 — ela retorna 6.2 mesmo no Windows 11 a menos que o executável declare compatibilidade explícita no manifesto. `RtlGetVersion` (da ntdll.dll) sempre retorna a versão real. Esta distinção é intencional: a Microsoft quer que desenvolvedores detectem *capabilities*, não versões. Mas quando você precisa saber a versão real — para diagnóstico, para segurança, para telemetria — `RtlGetVersion` é a resposta.

Outra forma rápida: execute `winver` em qualquer Windows 11. Você verá a build (por exemplo, `22H2` = versão 22621) e o número de revisão.

### O calendário de versões do Windows

```
Windows NT 3.1 (1993) → 3.51 → 4.0 → 2000 → XP → Vista (6.0)
→ 7 (6.1) → 8 (6.2) → 8.1 (6.3) → 10 (10.0.10240) → 11 (10.0.22000+)
```

A anomalia de numeração a partir do Vista (onde Windows 7 = versão interna 6.1) foi uma decisão pragmática: tantos programas checavam "versão >= 6" que incrementar o major para 7 quebraria compatibilidade. O Windows 10 rompeu com isso ao ir para 10.0 — e o Windows 11 continuou como 10.0 para não voltar a quebrar nada.

---

## OneCore: um kernel para tudo

Uma das mudanças mais importantes das últimas décadas no Windows foi a convergência da base de código num único kernel chamado **OneCore**.

Por anos, existiam variantes paralelas do Windows: o Windows desktop, o Windows Phone (baseado no Windows CE), o Xbox 360 (um fork do Windows 2000). Cada um evoluía separado, com equipes de kernel diferentes, binários diferentes, e a dificuldade enorme de manter tudo sincronizado.

A convergência começou com o Windows 8 / Windows Phone 8 (kernel compartilhado) e ficou completa no Windows 10. Hoje, **o mesmo kernel roda em**:

- PCs e laptops com Windows 11
- Servidores com Windows Server 2022
- Xbox Series X/S
- HoloLens 2
- Dispositivos IoT com Windows IoT

O que muda entre essas plataformas não é o kernel em si, mas quais **capabilities** estão habilitadas — geralmente controladas por chaves de registro e políticas. O conceito de "API Sets" (grupos de funcionalidades que podem ou não existir num determinado device form factor) será detalhado no Capítulo 3.

Isso tem uma implicação prática importante: **tudo que você aprender sobre o kernel neste livro é transferível**. Se você entende como threads são agendadas no Windows 11 Desktop, você entende como elas são agendadas no Xbox Series X. O mecanismo é o mesmo.

---

## Configurando seu ambiente de estudo

Para acompanhar os experimentos deste livro, você vai precisar de:

### 1. Process Explorer (Sysinternals)

Baixe em [sysinternals.com](https://docs.microsoft.com/en-us/sysinternals/). É a ferramenta que você vai usar mais do que qualquer outra. Versão gratuita, sem instalação, mostra muito mais do que o Task Manager.

Configure os símbolos no primeiro uso: menu Options → Configure Symbols, use o symbol server da Microsoft:
```
srv*C:\Symbols*https://msdl.microsoft.com/download/symbols
```

### 2. WinDbg Preview

A versão moderna do WinDbg está disponível na **Microsoft Store** como "WinDbg Preview". É a mesma engine de debugging mas com interface melhorada, suporte a time-travel debugging e script JavaScript. Preferimos ela ao WinDbg clássico (kd.exe/WinDbg.exe do SDK).

### 3. Windows SDK + WDK

Para compilar os exemplos de código, você precisa do **Visual Studio 2022** (Community é gratuito) com os workloads:
- Desktop development with C++
- Windows SDK (incluído automaticamente)

Para experimentos com drivers: instale o **Windows Driver Kit (WDK)** — disponível em docs.microsoft.com.

### 4. Suite completa do Sysinternals

Baixe o pacote completo: inclui ProcMon, Autoruns, TCPView, AccessChk e dezenas de outras ferramentas que usaremos ao longo do livro.

---

## Como este livro está organizado

Cada capítulo cobre um subsistema do Windows em profundidade. O Capítulo 1 (este) apresenta os conceitos fundamentais que serão referenciados em todos os outros. Não pule partes — cada seção constrói sobre a anterior.

As seções deste capítulo:

1. **Introdução e contexto** (você está aqui)
2. **APIs do Windows** — a camada que seu código vê
3. **Processos e Jobs** — unidades de isolamento
4. **Threads** — unidades de execução
5. **Memória virtual** — como o espaço de endereços funciona
6. **Hipervisor e VBS** — a camada de segurança mais profunda
7. **Objetos, handles e segurança** — os pilares do modelo de segurança
8. **Ferramentas essenciais** — como explorar tudo isso na prática

> **Nota:** O restante do livro assume que você entendeu este capítulo. Se algo não ficou claro, releia antes de continuar.

---

*Próximo: [A Camada de APIs do Windows](capitulo-01-02-apis-windows.md)*
