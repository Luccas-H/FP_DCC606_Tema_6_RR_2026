# Guia de Contribuição

Obrigado por seu interesse em contribuir com o projeto de **Detecção de Comunidades Críticas de Atendimento ao Cliente**! Siga as etapas abaixo para colaborar com o desenvolvimento do nosso ecossistema de mineração de redes complexas.

## 🧾 O que está neste repositório?
* **Módulo 1:** Implementação da Heurística Gulosa Construtiva (centralidade, grau e interseção de vizinhanças).
* **Módulo 2:** Implementação do Algoritmo Avançado de Refinamento (Meta-heurística GRASP com RCL e busca local 1-opt).
* **Estruturas de Dados Próprias:** Representações de baixo nível para grafos (Listas de Adjacência Dinâmicas e Bitsets) codificadas do zero.
* **Pipeline de Dados:** Scripts de ETL e parsing do dataset *Customer Support on Twitter* (Kaggle).
* **Ambiente de Benchmarking:** Módulo de validação assintótica utilizando instâncias oficiais do *DIMACS Maximum Clique Benchmark*.

## 📌 Requisitos para contribuir
* Sólido conhecimento em **Teoria dos Grafos**, **Otimização Combinatória** e algoritmos para problemas NP-difíceis.
* Familiaridade com a análise de vanguarda de s-Cliques Conexos (Santiago et al., 2018).
* Ambiente configurado para desenvolvimento de alto desempenho (C/C++ ou Python estruturado de baixo nível, conforme a stack do projeto).
* [Git](https://git-scm.com/) instalado.

## 🚀 Etapas para contribuir
1. **Fork:** Faça um fork do projeto para sua conta.
2. **Clone:** `git clone https://github.com/SeuUsuario/NomeDoRepositorio.git`
3. **Branch:** Crie uma branch para sua alteração: `git checkout -b feature-poda-subgrafo`
4. **Implemente:** Desenvolva sua solução mantendo o foco em eficiência algorítmica e redução do espaço de busca.
5. **Commit:** `git commit -m "Implementa técnica de poda baseada no diâmetro do s-clique"`
6. **Push:** `git push origin feature-poda-subgrafo`
7. **PR:** Abra um Pull Request detalhando as melhorias matemáticas, o impacto na complexidade assintótica e os resultados nos testes DIMACS.

## ✅ Boas práticas e Restrições de Escopo
* 🚫 **Regra de Ouro (Estruturas Próprias):** É terminantemente proibido o uso de bibliotecas de grafos caixa-preta (`NetworkX`, `igraph`, `BGL`, etc.). Qualquer manipulação topológica deve usar as nossas estruturas nativas de baixo nível.
* **Evite o upload de datasets:** Não envie os arquivos brutos de conversas do Kaggle ou arquivos extensos de instâncias DIMACS. Utilize os scripts de download e parsing já estruturados.
* **Validação Dupla:** Toda alteração nos motores de busca combinatória deve passar pelo protocolo duplo de testes: validação funcional no grafo do Twitter e teste de estresse/desempenho nas famílias `brock`, `p_hat` ou `san` do DIMACS.

## 🛠 Sugestões de contribuição
* Otimizações de baixo nível nas operações de interseção de vizinhanças (ex: operações bitwise em Bitsets).
* Novas estratégias de poda (*pruning*) baseadas nas propriedades de conectividade e diâmetro para mitigar a explosão combinatória.
* Aperfeiçoamento do fator de aleatoriedade na Lista Restrita de Candidatos (RCL) do GRASP.
* Implementação de novos critérios de parada ou estratégias de indexação para grafos dinâmicos de grande porte.

---
*Este projeto é parte integrante de estudos acadêmicos na UFRR.*
