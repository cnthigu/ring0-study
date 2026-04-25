# Dependency Walker

Encontrei uma ferramenta padrão chamada **Dependency Walker**, que ajuda a entender arquivos EXE e DLL e suas dependências.

![Uso do Dependency Walker com Kernel32.dll](dependencywalker.png)

Na captura, abri o conhecido arquivo `Kernel32.dll`, que permite que o aplicativo interaja com as APIs Win32. Também podemos ver que ela lista todas as subdependências dos arquivos DLL, juntamente com as funções de importação e exportação à direita. Essa ferramenta dá uma ideia geral de qual função da API Win32 um EXE ou DLL está chamando. Comecei a usá-la para ter uma visão geral de alto nível de como um aplicativo interage com o kernel do Windows. Embora existam muitas ferramentas avançadas, esta é ideal para iniciantes que desejam entender o assunto de uma maneira mais simples.

Espero que todos vocês achem isso útil. 😀
