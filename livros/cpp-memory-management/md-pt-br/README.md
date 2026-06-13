# C++ Memory Management - Traducao PT-BR

> Traducao nao oficial do livro *C++ Memory Management*
> Patrice Roy
> Packt Publishing, 2025 - ISBN: 978-1-80512-980-6

## Sobre esta traducao

Traducao do livro **C++ Memory Management** para o **portugues brasileiro**, feita com auxilio de IA e revisada manualmente. O livro explora como o C++ moderno permite escrever programas mais seguros e eficientes por meio do controle preciso do gerenciamento de memoria, desde os conceitos basicos do modelo de objetos ate a escrita de alocadores e containers proprios.

Criterios de traducao:

- Todo texto narrativo e explicativo traduzido para PT-BR natural e fluente
- Termos tecnicos consagrados mantidos em ingles: `pointer`, `reference`, `ownership`, `lifetime`, `alignment`, `padding`, `RAII`, `smart pointer`, `undefined behavior`, `type punning`, `cv-qualification`, `allocator`, `arena`, `placement new`, `destructor`, etc.
- Nomes de funcoes e tipos da std mantidos em ingles (`std::unique_ptr`, `std::shared_ptr`, `std::memcpy`, `std::start_lifetime_as`, `std::pmr`, etc.)
- Palavras-chave da linguagem nao sao traduzidas (`static_cast`, `reinterpret_cast`, `const_cast`, `bit_cast`, `new`, `delete`, etc.)
- Primeira ocorrencia de termos importantes: "termo em portugues (termo em ingles)"
- Codigo preservado exatamente como no original

## Indice

| Arquivo | Conteudo | Paginas orig. |
|---|---|---|
| [prefacio.md](prefacio.md) | Prefacio (Preface) | xvii-xxi |
| **Parte 1: Memoria em C++** | | |
| [capitulo-01.md](capitulo-01.md) | Capitulo 1 - Objetos, Ponteiros e Referencias (Objects, Pointers, and References) | 1-30 |
| [capitulo-02.md](capitulo-02.md) | Capitulo 2 - Cuidados Necessarios (Things to Be Careful With) | 31-44 |
| [capitulo-03.md](capitulo-03.md) | Capitulo 3 - Casts e Qualificacoes cv (Casts and cv-qualifications) | 45-56 |
| **Parte 2: Tecnicas de Gerenciamento Implicito de Memoria** | | |
| [capitulo-04.md](capitulo-04.md) | Capitulo 4 - Usando Destrutores (Using Destructors) | 59-80 |
| [capitulo-05.md](capitulo-05.md) | Capitulo 5 - Usando Smart Pointers Padrao (Using Standard Smart Pointers) | 81-108 |
| [capitulo-06.md](capitulo-06.md) | Capitulo 6 - Escrevendo Smart Pointers (Writing Smart Pointers) | 109-136 |
| **Parte 3: Assumindo o Controle dos Mecanismos de Gerenciamento de Memoria** | | |
| [capitulo-07.md](capitulo-07.md) | Capitulo 7 - Sobrecarregando Operadores de Alocacao de Memoria (Overloading Memory Allocation Operators) | 139-160 |
| [capitulo-08.md](capitulo-08.md) | Capitulo 8 - Escrevendo um Detector de Vazamentos Simples (Writing a Naive Leak Detector) | 161-184 |
| [capitulo-09.md](capitulo-09.md) | Capitulo 9 - Mecanismos Atipicos de Alocacao (Atypical Allocation Mechanisms) | 185-208 |
| [capitulo-10.md](capitulo-10.md) | Capitulo 10 - Gerenciamento de Memoria Baseado em Arena e Outras Otimizacoes (Arena-Based Memory Management and Other Optimizations) | 209-236 |
| [capitulo-11.md](capitulo-11.md) | Capitulo 11 - Reclamacao Diferida (Deferred Reclamation) | 237-258 |
| **Parte 4: Escrevendo Containers Genericos (e um Pouco Mais)** | | |
| [capitulo-12.md](capitulo-12.md) | Capitulo 12 - Containers Genericos com Gerenciamento Explicito de Memoria (Writing Generic Containers with Explicit Memory Management) | 261-300 |
| [capitulo-13.md](capitulo-13.md) | Capitulo 13 - Containers Genericos com Gerenciamento Implicito de Memoria (Writing Generic Containers with Implicit Memory Management) | 301-322 |
| [capitulo-14.md](capitulo-14.md) | Capitulo 14 - Containers Genericos com Suporte a Alocadores (Writing Generic Containers with Allocator Support) | 323-366 |
| [capitulo-15.md](capitulo-15.md) | Capitulo 15 - Questoes Contemporaneas (Contemporary Issues) | 367-380 |
| [anexo.md](anexo.md) | Anexo - Coisas que Voce Deve Saber (Things You Should Know) | 383-404 |

## Aviso de direitos autorais

Esta traducao e feita para fins educacionais e de estudo pessoal. O conteudo original e copyright 2025 de Patrice Roy, publicado pela Packt Publishing. Para uso oficial ou comercial, adquira o livro original.

## Recursos relacionados

- [Repositorio oficial do livro (GitHub)](https://github.com/PacktPublishing/C-Plus-Plus-Memory-Management)
- [cppreference.com](https://en.cppreference.com/w/) - referencia oficial da linguagem
- [ISO C++ Committee Papers](https://www.open-std.org/jtc1/sc22/wg21/) - propostas recentes de C++
