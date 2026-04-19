# Introdução

> *Tradução do livro Windows Internals, 7a Edição*
> *Pavel Yosifovich, Alex Ionescu, Mark E. Russinovich, David A. Solomon*
> *Microsoft Press, 2017*

> Esta é uma tradução não oficial para o português brasileiro, feita com auxílio de IA para fins de estudo.
> Parte do repositório [cpp-cnt-study](https://github.com/cnthigu/cpp-cnt-study).

---

*Windows Internals, Sétima Edição* é destinado a profissionais avançados de computação — desenvolvedores, pesquisadores de segurança e administradores de sistemas — que desejam compreender como os componentes centrais dos sistemas operacionais Microsoft Windows 10 e Windows Server 2016 funcionam internamente. Com esse conhecimento, desenvolvedores conseguem entender melhor a lógica por trás das decisões de design ao criar aplicações específicas para a plataforma Windows. Esse conhecimento também pode ajudar desenvolvedores a depurar (debug) problemas complexos. Administradores de sistemas também se beneficiam dessas informações, pois entender como o sistema operacional funciona "por baixo dos panos" facilita a compreensão do comportamento de desempenho do sistema e torna muito mais fácil diagnosticar problemas quando algo dá errado. Pesquisadores de segurança podem descobrir como aplicativos e o sistema operacional podem se comportar de forma inesperada e ser mal utilizados, causando comportamentos indesejados, além de entender as mitigações e funcionalidades de segurança que o Windows moderno oferece contra esses cenários. Após ler este livro, você terá uma compreensão muito melhor de como o Windows funciona e por que ele se comporta da forma que se comporta.

## Histórico do livro

Esta é a sétima edição de um livro que originalmente se chamava *Inside Windows NT* (Microsoft Press, 1992), escrito por Helen Custer (antes do lançamento inicial do Microsoft Windows NT 3.1). *Inside Windows NT* foi o primeiro livro publicado sobre o Windows NT e ofereceu percepções fundamentais sobre a arquitetura e o design do sistema. *Inside Windows NT, Segunda Edição* (Microsoft Press, 1998) foi escrito por David Solomon. Ele atualizou o livro original para cobrir o Windows NT 4.0 e trouxe um nível de profundidade técnica muito maior.

*Inside Windows 2000, Terceira Edição* (Microsoft Press, 2000) foi escrito por David Solomon e Mark Russinovich. Ele adicionou muitos tópicos novos, como inicialização e desligamento do sistema, funcionamento interno de serviços (service internals), funcionamento interno do registro (registry internals), drivers de sistema de arquivos e redes. Também cobriu mudanças no kernel do Windows 2000, como o Windows Driver Model (WDM), Plug and Play, gerenciamento de energia (power management), Windows Management Instrumentation (WMI), criptografia, o objeto job e Terminal Services. *Windows Internals, Quarta Edição* (Microsoft Press, 2004) foi a atualização para Windows XP e Windows Server 2003 e adicionou mais conteúdo voltado a ajudar profissionais de TI a aproveitar seu conhecimento sobre os internos do Windows, como o uso das principais ferramentas do Windows SysInternals e a análise de dumps de memória (crash dumps).

*Windows Internals, Quinta Edição* (Microsoft Press, 2009) foi a atualização para Windows Vista e Windows Server 2008. Ela marcou a saída de Mark Russinovich para um cargo em tempo integral na Microsoft (onde hoje é CTO do Azure) e a entrada de um novo coautor, Alex Ionescu. Os novos conteúdos incluíram o carregador de imagens (image loader), o mecanismo de depuração em modo usuário (user-mode debugging facility), o Advanced Local Procedure Call (ALPC) e o Hyper-V. A próxima versão, *Windows Internals, Sexta Edição* (Microsoft Press, 2012), foi totalmente atualizada para cobrir as muitas mudanças no kernel do Windows 7 e Windows Server 2008 R2, com muitos experimentos práticos novos para refletir as mudanças nas ferramentas.

## Mudanças na sétima edição

Desde a última atualização deste livro, o Windows passou por diversas versões, chegando ao Windows 10 e Windows Server 2016. O próprio Windows 10, sendo o nome atual para o Windows como plataforma em evolução contínua, teve várias versões desde seu lançamento inicial para manufatura (RTM). Cada uma é identificada por um número de versão de quatro dígitos indicando o ano e o mês de lançamento — por exemplo, Windows 10, versão 1703, que foi concluída em março de 2017. Isso implica que o Windows passou por pelo menos seis versões desde o Windows 7 (no momento da escrita deste livro).

A partir do Windows 8, a Microsoft iniciou um processo de convergência de sistemas operacionais, benéfico tanto do ponto de vista do desenvolvimento quanto para a equipe de engenharia do Windows. Windows 8 e Windows Phone 8 tinham kernels convergidos, com a convergência de apps modernos chegando no Windows 8.1 e Windows Phone 8.1. A história de convergência se completou com o Windows 10, que roda em desktops/laptops, servidores, Xbox One, telefones (Windows Mobile 10), HoloLens e vários dispositivos de Internet das Coisas (IoT). Com essa grande unificação concluída, chegou o momento certo para uma nova edição da série, que finalmente poderia se atualizar com quase meia década de mudanças no que será, daqui em diante, uma arquitetura de kernel mais estável. Assim, este livro cobre aspectos do Windows desde o Windows 8 até o Windows 10, versão 1703. Além disso, esta edição recebe Pavel Yosifovich como novo coautor.

## Experimentos práticos

Mesmo sem acesso ao código-fonte do Windows, é possível extrair muito conhecimento sobre os internos do Windows a partir do depurador do kernel (kernel debugger), das ferramentas do SysInternals e das ferramentas desenvolvidas especificamente para este livro. Quando uma ferramenta pode ser usada para expor ou demonstrar algum aspecto do comportamento interno do Windows, os passos para experimentar você mesmo são listados em seções especiais chamadas **"EXPERIMENTO"**. Essas seções aparecem ao longo do livro, e incentivamos você a executá-las enquanto lê. Ver provas concretas de como o Windows funciona internamente causará uma impressão muito mais duradoura do que simplesmente ler sobre isso.

## Tópicos não cobertos

O Windows é um sistema operacional grande e complexo. Este livro não cobre tudo relacionado aos internos do Windows, mas se concentra nos componentes base do sistema. Por exemplo, este livro não descreve o COM+, a infraestrutura de programação orientada a objetos distribuída do Windows, nem o Microsoft .NET Framework, a base das aplicações de código gerenciado (managed code). Por ser um livro de "internos" e não um livro para usuários, programadores ou administradores de sistemas, ele não descreve como usar, programar ou configurar o Windows.

## Um aviso e uma ressalva

Como este livro descreve comportamentos não documentados da arquitetura interna e da operação do sistema operacional Windows (como estruturas e funções internas do kernel), esse conteúdo está sujeito a mudanças entre versões.

"Sujeito a mudanças" não significa necessariamente que os detalhes descritos neste livro vão mudar entre versões, mas você não pode contar que não vão mudar. Qualquer software que use essas interfaces não documentadas, ou conhecimento interno sobre o sistema operacional, pode não funcionar em versões futuras do Windows. Pior ainda: software que roda em modo kernel (como device drivers) e usa essas interfaces não documentadas pode causar falhas no sistema ao executar em uma versão mais nova do Windows, resultando em potencial perda de dados para os usuários desse software.

Em resumo: **nunca use qualquer funcionalidade interna do Windows, chave de registro, comportamento, API ou outro detalhe não documentado mencionado neste livro durante o desenvolvimento de qualquer tipo de software projetado para sistemas de usuários finais**, ou para qualquer finalidade além de pesquisa e documentação. Sempre consulte a Microsoft Software Development Network (MSDN) para documentação oficial sobre um determinado tópico primeiro.

## Suposições sobre você

O livro assume que o leitor se sente confortável trabalhando com o Windows em nível de usuário avançado (power user) e possui conhecimento básico de conceitos de sistemas operacionais e hardware, como registradores de CPU, memória, processos e threads. O conhecimento básico de funções, ponteiros e construções similares da linguagem de programação C é benéfico em algumas seções.

## Organização deste livro

O livro está dividido em duas partes (como foi a sexta edição), sendo que a primeira é a que você está lendo agora.

- **Capítulo 1, "Conceitos e ferramentas"** — fornece uma introdução geral aos conceitos internos do Windows e apresenta as principais ferramentas usadas ao longo do livro. É fundamental ler este capítulo primeiro, pois ele fornece o contexto necessário para o restante do livro.
- **Capítulo 2, "Arquitetura do sistema"** — mostra a arquitetura e os principais componentes que compõem o Windows e os discute com alguma profundidade. Vários desses conceitos são abordados com mais detalhes nos capítulos seguintes.
- **Capítulo 3, "Processos e jobs"** — detalha como os processos são implementados no Windows e as várias formas de manipulá-los. Jobs também são discutidos como um meio de controlar um conjunto de processos e habilitar o suporte a Windows Containers.
- **Capítulo 4, "Threads"** — detalha como as threads são gerenciadas, agendadas e manipuladas no Windows.
- **Capítulo 5, "Gerenciamento de memória"** — mostra como o gerenciador de memória (memory manager) usa a memória física e virtual, e as várias formas pelas quais a memória pode ser manipulada e usada por processos e drivers.
- **Capítulo 6, "Sistema de I/O"** — mostra como o sistema de I/O no Windows funciona e se integra com device drivers para fornecer os mecanismos de trabalho com periféricos de I/O.
- **Capítulo 7, "Segurança"** — detalha os vários mecanismos de segurança integrados ao Windows, incluindo mitigações que agora fazem parte do sistema para combater exploits.

## Convenções

As seguintes convenções são usadas neste livro:

- **Negrito** é usado para indicar texto que você digita, bem como itens de interface que você deve clicar ou botões que deve pressionar.
- *Itálico* é usado para indicar novos termos.
- Elementos de código aparecem em `fonte monoespaçada`.
- As primeiras letras dos nomes de caixas de diálogo e seus elementos são capitalizadas — por exemplo, a caixa de diálogo Salvar Como.
- Atalhos de teclado são indicados por um sinal de mais (+) separando os nomes das teclas. Por exemplo, Ctrl+Alt+Delete significa que você pressiona as teclas Ctrl, Alt e Delete simultaneamente.

## Sobre o conteúdo complementar

Incluímos conteúdo complementar para enriquecer sua experiência de aprendizado. O conteúdo complementar deste livro pode ser baixado em:

https://aka.ms/winint7ed/downloads

Também disponibilizamos o código-fonte das ferramentas escritas especificamente para este livro em:

https://github.com/zodiacon/windowsinternals

## Errata e suporte ao livro

Fizemos todos os esforços para garantir a precisão deste livro e de seu conteúdo complementar. Quaisquer erros reportados desde a publicação estão listados em nosso site da Microsoft Press em:

https://aka.ms/winint7ed/errata

Se precisar de suporte adicional, envie e-mail ao Suporte de Livros da Microsoft Press em mspinput@microsoft.com.

## Queremos ouvir você

Na Microsoft Press, sua satisfação é nossa principal prioridade. Conte-nos o que você achou deste livro em:

https://aka.ms/tellpress

## Mantenha contato

Vamos continuar a conversa! Estamos no Twitter: @MicrosoftPress.
