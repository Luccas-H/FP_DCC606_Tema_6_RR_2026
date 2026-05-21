# Política de Segurança - Detecção de Comunidades Críticas em Redes Sociais

## 🔐 Visão Geral
Este repositório é dedicado ao processamento, mineração e otimização combinatória de redes complexas para a detecção de comunidades críticas de atendimento ao cliente, utilizando dados reais do Twitter e benchmarks do DIMACS.

Por se tratar de um ecossistema que manipula estruturas de dados de baixo nível codificadas do zero e realiza o parsing de grandes volumes de arquivos textuais externos, a segurança, a estabilidade e a integridade do código são fundamentais. Esta política descreve como reportar vulnerabilidades e as práticas recomendadas para mitigar riscos durante a execução dos algoritmos e pipelines de dados.

## 🚀 Versões Suportadas
As correções de segurança e otimizações contra travamentos sistêmicos são aplicadas prioritariamente na branch principal:

| Versão | Suportado |
| :--- | :--- |
| Main (Latest) | ✅ Sim |
| < 1.0.0 | ❌ Não |

## 🛡️ Escopo de Segurança
Consideramos como vulnerabilidades relevantes para este ecossistema:
* **Estouro de Memória e Buffer Overflow:** Falhas de segmentação ou vazamento de memória nas estruturas de dados nativas (Listas de Adjacência Dinâmicas, Vetores de Bits/Bitsets) ao processar nós de altíssimo grau (*hubs*) ou instâncias massivas do DIMACS.
* **Injeção de Código/Negação de Serviço no Pipeline de Dados:** Arquivos de input adulterados (Kaggle/Twitter CSVs ou arquivos DIMACS malformados) projetados para quebrar o parser de texto, estourar buffers de leitura ou causar travamento de loops (*Infinite Loop DoS*).
* **Negação de Serviço Local (DoS) por Explosão Combinatória Incontrolável:** Falhas nas heurísticas de poda (*pruning*) que permitam que uma entrada específica force intencionalmente o pior caso assintótico exponencial, exaurindo completamente a RAM/CPU do sistema.

**Fora de Escopo:** Convergência prematura do GRASP em ótimos locais, subotimalidade da heurística gulosa ou pequenos desvios em métricas de centralidade (devem ser tratados via Issues normais).

## 🧑‍💻 Relatando uma Vulnerabilidade
Se você identificar uma falha de segurança ou uma vulnerabilidade crítica nas estruturas de baixo nível, por favor, não abra uma Issue pública.

Envie um relatório detalhado com o cenário de teste (*Proof of Concept*) para os mantenedores:
* **Matheus Garcia Sampaio:** [matheusgarciasam@gmail.com](mailto:matheusgarciasam@gmail.com)

Você receberá um retorno sobre a análise do problema em até **5 dias úteis**.

## 🔄 Processo de Correção
1. **Triagem:** Validaremos a vulnerabilidade em um ambiente sandbox isolado.
2. **Correção Contida:** Desenvolveremos o patch focando na robustez das estruturas de dados e na segurança de memória, sem comprometer a performance assintótica.
3. **Release:** Publicaremos a atualização na branch principal e documentaremos a mitigação.

## 🔐 Recomendações de Uso
* **Sanitização de Inputs:** Certifique-se de que os dados baixados do repositório *Customer Support on Twitter* estão íntegros antes de rodar o pipeline de parsing e geração de grafos.
* **Gerenciamento de Memória:** Se estiver implementando os Bitsets e matrizes em C/C++, compile utilizando flags de higienização (como `-fsanitize=address,undefined`) em ambiente de desenvolvimento para capturar falhas de ponteiro precocemente.
* **Isolamento de Execução:** Execute os testes de estresse com instâncias DIMACS pesadas em ambientes controlados para evitar que o consumo de memória afete outros processos essenciais do seu sistema operacional (dual-boot).

---
*Este projeto faz parte de estudos acadêmicos na Universidade Federal de Roraima (UFRR).*
