# FP_DCC606_Tema_6_RR_2026

> **Projeto Final — Análise de Algoritmos (DCC606)**  
> Universidade Federal de Roraima — Departamento de Ciência da Computação  
> Tema 6: Detecção de Comunidades Críticas de Atendimento ao Cliente em Redes Sociais

---

## Sobre o Projeto

Este projeto implementa um **motor de busca combinatória** para detecção de comunidades críticas em redes sociais de atendimento ao cliente. A abordagem modela as interações entre usuários como um grafo não-direcionado G = (V, E) e aplica algoritmos de busca pelo **Clique Máximo** para identificar grupos densamente conectados — as chamadas *coortes de crise*: conjuntos de usuários que todos interagiram entre si discutindo o mesmo problema.

O projeto foi desenvolvido inteiramente em **C (C11)**, sem uso de bibliotecas externas de manipulação de grafos, seguindo os requisitos da disciplina.

---

## Problema Abordado

Em redes de suporte ao cliente, surtos de insatisfação geram grupos de usuários altamente interconectados discutindo o mesmo problema sistêmico. Identificar esses grupos é equivalente a encontrar o **Clique Máximo** no grafo de interações — um problema NP-difícil que exige heurísticas eficientes para instâncias de larga escala.

A formulação matemática busca o maior subconjunto C ⊆ V tal que todo par (u, v) ∈ C possua aresta direta, maximizando |C|.

---

## Estrutura do Repositório

```
FP_DCC606_Tema_6_RR_2026/
├── LICENSE
├── CODE_OF_CONDUCT.md
├── pull_request_template.md
├── README.md
├── relatorio/                # Entregáveis acadêmicos (LaTeX, template SBC)
│   ├── relatorio.tex         # Relatório IEEE/SBC do projeto
│   ├── relatorio.pdf         # Relatório compilado (7 páginas)
│   ├── resenha.tex           # Resenha crítica do artigo (Behar & Cohen, EDBT 2018)
│   ├── resenha.pdf           # Resenha compilada (4 páginas)
│   ├── sbc-template.sty      # Estilo SBC para compilação
│   └── README.md             # Instruções de compilação (Overleaf / local)
└── impl_estrutura/
   ├── Makefile
   ├── include/
   │   ├── graph.h            # Estrutura do grafo (Bitset + lista de adjacência)
   │   ├── clique.h           # Interface dos algoritmos de clique
   │   ├── dimacs.h           # Leitor de benchmarks DIMACS .clq
   │   ├── twitter_parser.h   # Parser do dataset Twitter CSV
   │   └── dot_export.h       # Exportador GraphViz .dot
   ├── src/
   │   ├── graph.c            # Implementação do grafo
   │   ├── clique.c           # Algoritmo Guloso + GRASP
   │   ├── dimacs.c           # Leitor DIMACS
   │   ├── twitter_parser.c   # Parser CSV Twitter
   │   ├── dot_export.c       # Gerador de arquivo .dot
   │   └── main.c             # Ponto de entrada e modos de execução
   └── benchmarks/
       ├── brock200_2.clq     # Benchmark DIMACS (200 vértices, ω=12)
       ├── p_hat300-1.clq     # Benchmark DIMACS (300 vértices, ω=8)
      ├── sample.csv         # Amostra do dataset Twitter (99 tweets)
      └── twcs.csv           # Dataset completo Twitter (3M tweets) *
```

> \* O arquivo `twcs.csv` não está versionado por exceder o limite do GitHub (516 MB).

---

## Documentos do Projeto

Os entregáveis acadêmicos estão na pasta [`relatorio/`](relatorio/), escritos em LaTeX com o template oficial da SBC:

- **`relatorio.pdf`** — Relatório IEEE/SBC do projeto (problema, complexidade, implementação, estudo comparativo de desempenho, comparação teórica com Bron–Kerbosch).
- **`resenha.pdf`** — Resenha crítica do artigo *Finding All Maximal Connected s-Cliques in Social Networks* (Behar & Cohen, EDBT 2018), cobrindo a distinção entre cliques/s-plexos/s-cliques, as estratégias de poda e a aplicabilidade a grafos dinâmicos.

Instruções de compilação (Overleaf ou local) em [`relatorio/README.md`](relatorio/README.md).

---

## Decisões de Implementação

### Representação do Grafo

O grafo utiliza **duas estruturas simultâneas**:

- **Bitset (vetor de bits)** — estrutura principal. Cada vértice `i` possui um vetor de `uint64_t` onde o bit `j` indica adjacência com `j`. A operação de interseção de vizinhanças — coração da busca por cliques — se reduz a um AND bit a bit, executando em O(n/64) em vez de O(n).

- **Lista de Adjacência** — estrutura auxiliar para navegação geral e percurso de vizinhos.

### Algoritmos Implementados

**Módulo 1 — Guloso**  
Seleciona o vértice de maior grau como semente e expande o clique escolhendo a cada passo o candidato com maior grau dentro do conjunto de candidatos atual (interseção de vizinhanças via AND de Bitsets). Complexidade: O(n²/64).

**Módulo 2 — GRASP**  
Executa `max_iter` iterações, cada uma com:
1. **Fase Construtiva (RCL):** igual ao guloso, mas sorteia entre os `rcl_size` melhores candidatos em vez de sempre tomar o melhor. Diversifica a busca.
2. **Fase de Busca Local (1-opt):** tenta trocar vértices do clique por pares externos que aumentem sua cardinalidade.

Guarda o melhor clique encontrado entre todas as iterações. Complexidade: O(max\_iter × n²/64).

---

## Bases de Dados

### Benchmarks DIMACS

Os arquivos `.clq` são instâncias padrão da competição DIMACS de coloração e clique máximo, amplamente usados na literatura como referência de avaliação.

| Arquivo | Vértices | Arestas | Ótimo Conhecido |
|---|---|---|---|
| `brock200_2.clq` | 200 | 9.876 | 12 |
| `p_hat300-1.clq` | 300 | 10.933 | 8 |

Download direto:
```bash
# De dentro da pasta benchmarks/
wget https://raw.githubusercontent.com/vcoppe/mdd-solver/master/data/misp/easy/brock200_2.clq
wget https://raw.githubusercontent.com/vcoppe/mdd-solver/master/data/misp/easy/p_hat300-1.clq
```

### Dataset Twitter — Customer Support

Dataset público disponível no Kaggle com tweets reais de suporte ao cliente de grandes empresas.

- **Link:** https://www.kaggle.com/datasets/thoughtvector/customer-support-on-twitter
- **Arquivo utilizado:** `twcs.csv` (516 MB, ~3 milhões de tweets) e `sample.csv` (amostra de 99 tweets)
- **Colunas relevantes:** `tweet_id`, `author_id`, `text`, `in_response_to_tweet_id`

Após baixar, coloque os arquivos em `impl_estrutura/benchmarks/`.

---

## Dependências

- **GCC** >= 7.0 com suporte a C11
- **Make**
- **GraphViz** (opcional, para visualização)

```bash
# Ubuntu/Debian
sudo apt install gcc make graphviz

# macOS
brew install gcc make graphviz
```

---

## Compilação

```bash
cd impl_estrutura/
make
```

O binário gerado é `proj_exec`.

Para limpar os arquivos compilados:
```bash
make clean
```

### Sobre o Makefile

O `Makefile` compila todos os `.c` de `src/` com as flags `-Wall -Wextra -O2 -std=c11 -Iinclude`, gerando os `.o` individuais e linkando no binário final `proj_exec`. Atalhos de execução estão disponíveis via `make test`, `make greedy`, `make grasp` e `make twitter`.

---

## Uso

```bash
# Teste da estrutura interna do grafo
./proj_exec test

# Algoritmo Guloso em benchmark DIMACS
./proj_exec greedy benchmarks/brock200_2.clq

# GRASP em benchmark DIMACS
./proj_exec grasp benchmarks/brock200_2.clq

# Parser Twitter com dataset real
./proj_exec twitter benchmarks/sample.csv
./proj_exec twitter benchmarks/twcs.csv

# Tabela comparativa completa (bench)
./proj_exec bench benchmarks/brock200_2.clq 12
./proj_exec bench benchmarks/p_hat300-1.clq 8
```

---

## Visualização

Após qualquer execução que gere um arquivo `.dot`, renderize com GraphViz:

```bash
dot -Tpng output_twitter.dot -o saida.png
```

Vértices do clique aparecem em **vermelho**, os demais em cinza.

---

## Integrantes

| Nome | Matrícula |
|---|---|
| Matheus Garcia | 2023010191 |
| Luccas Henrique | 2023010182 |

---

## Referências

- BEHAR, R.; COHEN, S. *Finding All Maximal Connected s-Cliques in Social Networks*. In: Proceedings of the 21st International Conference on Extending Database Technology (EDBT), p. 61–72, 2018. DOI: 10.5441/002/edbt.2018.07.
- DIMACS Graph Coloring Benchmark: http://www.cs.unc.edu/Research/Coloring/
- Kaggle — Customer Support on Twitter: https://www.kaggle.com/datasets/thoughtvector/customer-support-on-twitter
