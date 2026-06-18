# Relatório IEEE/SBC

Relatório do projeto em LaTeX, usando o **template oficial da SBC**.

## Dependência: arquivos do template SBC

O `relatorio.tex` usa `\usepackage{sbc-template}`. Para compilar, é preciso ter o
arquivo **`sbc-template.sty`** (e o `sbc-template.cls`, se for o caso) na mesma
pasta do `.tex`. Eles são distribuídos gratuitamente pela SBC:

- Baixe em: https://www.sbc.org.br/documentos-da-sbc/ (seção "Templates para artigos")
- Ou via Overleaf: crie um projeto a partir do template **"SBC Conferences"**
  e cole o conteúdo de `relatorio.tex` no `main.tex` — o `.sty` já vem incluso.

> A bibliografia está embutida via `thebibliography` (não precisa de `.bib` nem
> de `sbc.bst`).

## Como compilar (forma mais fácil: Overleaf)

1. Acesse https://overleaf.com → New Project → Templates → **SBC**.
2. Substitua o conteúdo do `main.tex` pelo de `relatorio.tex`.
3. Clique em *Recompile*. O PDF sai pronto.

## Como compilar localmente

Instale uma distribuição LaTeX e o `sbc-template.sty` nesta pasta:

```bash
# Ubuntu/Debian
sudo apt install texlive-latex-base texlive-latex-extra texlive-lang-portuguese

# Com o sbc-template.sty nesta pasta:
pdflatex relatorio.tex
pdflatex relatorio.tex   # 2ª passada resolve as referências cruzadas
```

O resultado é `relatorio.pdf`.

## Conteúdo (atende aos requisitos do trabalho)

- Descrição do problema e cenário de aplicação (Seção 1–2)
- Análise de complexidade dos algoritmos (Seção 4)
- Algoritmo exato Bron–Kerbosch para comparação teórica (Seção 4.1)
- Descrição da implementação (Seção 5)
- Estudo comparativo de desempenho — tabelas do `bench` (Seção 6)
- Aplicação prática no dataset Twitter (Seção 7)
- URL do repositório GitHub
