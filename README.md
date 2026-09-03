🎯 Objetivo Geral
Aplicar conceitos fundamentais e avançados da Teoria dos Grafos na resolução de problemas reais. Os alunos deverão modelar um problema de domínio específico utilizando grafos, desenvolver soluções computacionais, conduzir testes rigorosos de desempenho e documentar as descobertas em um artigo científico padrão SBC (Sociedade Brasileira de Computação).

📦 Fases de Desenvolvimento
O projeto está dividido em duas fases:

Fase I: Topologia e Conectividade
Foco na construção da estrutura de dados e análise estrutural do problema, ignorando custos ou pesos.
Modelagem de Dados: Leitura e processamento de bases de dados reais (formato CSV, JSON, txt) para instanciar as representações (Lista e Matriz de Adjacência).
Análise Estrutural: Aplicação de algoritmos de busca (BFS/DFS) para responder perguntas de negócio sobre conectividade (ex: identificação de componentes conexos, ciclos, bipartição, pontes ou vértices de articulação).

Fase II: Otimização e Complexidade
Foco na resolução de problemas de custo, distância, capacidade ou alocação, introduzindo arestas valoradas e problemas de alta complexidade.
Modelagem de Pesos: Tradução de variáveis reais (distância, tempo, similaridade, custo financeiro) para pesos das arestas.
Otimização P: Aplicação de algoritmos clássicos de Menor Caminho (Dijkstra ou Bellman-Ford) e/ou Árvore Geradora Mínima (Kruskal ou Prim).
Otimização NP-Difícil: Implementação de uma solução exata (Backtracking/Força Bruta) ou heurística/gulosa para um problema complexo do domínio (ex: Caixeiro Viajante, Cobertura de Vértices, Coloração, Clique Máximo).

🧩 Requisitos Funcionais e Não Funcionais
RF01: O sistema deve carregar grafos a partir de datasets reais contendo, no mínimo, 1.000 vértices (ex: SNAP, OpenStreetMap, Kaggle).
RF02: O sistema deve permitir a alternância entre Lista de Adjacência e Matriz de Adjacência para fins de teste de consumo de memória.
RF03: O sistema deve gerar logs de tempo de execução (em milissegundos) e consumo de memória para cada algoritmo executado.
RNF01: Código implementado em linguagem C sem utilizar bibliotecas prontas de grafos para os algoritmos core. A implementação deve ser autoral.

🔬 Protocolo Experimental
Testes de Estresse: Os grupos devem executar seus algoritmos em subconjuntos do grafo (N=100, N=500, N=1000, etc.) para plotar gráficos de crescimento assintótico temporal O(|V| + |E|).
Análise Comparativa: Comparar o uso de memória entre matriz e lista de adjacência, e o tempo de execução entre heurísticas e soluções exatas na Fase II.

📋 Estudos de Caso

Malha Metroviária e Expansão (Fase I: Pontes/Articulações | Fase II: Dijkstra para passageiro + AGM para expansão de trilhos).

📂 Entrega do Projeto
O projeto deve conter:
Repositório no GitHub;
Commits organizados e coerentes com a evolução do projeto;
Aplicação executável localmente;
Artigo científico de 4 a 6 páginas detalhando a Metodologia, Modelagem e Discussão dos Resultados.
