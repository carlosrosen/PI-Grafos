# Estudo da Malha Ferroviária do Reino Unido

Projeto em C que modela a malha ferroviária do Reino Unido com grafos, usando duas representações: lista e matriz de adjacência.

## Funcionalidades principais

- **Representações do grafo:** permite alternar entre lista e matriz de adjacência durante a execução.
- **Exportação da estrutura:** gera arquivos `.txt` e `.dot` com a estrutura do grafo para análise e visualização.
- **Topologia da malha:** identifica componentes biconexos, vértices de articulação e pontes.
- **Melhor rota (Dijkstra):** calcula o caminho de menor tempo entre duas estações, considerando os tempos de baldeação.
- **Planejamento de expansão (AGM/Kruskal):** seleciona as conexões de menor custo para expandir a malha e gera o grafo resultante em `.dot`.

## Estrutura de pastas

```text
PI-Grafos/
├── dataset/    # Dados de estações, conexões e expansão
├── include/    # Cabeçalhos (.h) das bibliotecas
├── src/
│   ├── main.c  # Menu e fluxo principal
│   └── libs/   # Grafos, Dijkstra, topologia, AGM e utilidades
├── spec/       # Testes das implementações
├── output/     # Arquivos gerados pelo programa
├── Dockerfile
└── Makefile
```

## Como executar

Requisitos: GCC e Make. Na raiz do projeto, basta rodar:

```bash
make
```

Esse comando compila o projeto e inicia o programa.

Para executar os testes:

```bash
make testes
```

## Docker

```bash
docker build -t pi-grafos .
docker run --rm -it pi-grafos
```

Dentro do container, execute o projeto com:

```bash
make
```

Ou, em um único comando:

```bash
docker run --rm -it pi-grafos make
```

## Fonte dos dados

A base de dados utilizada foi obtida no Kaggle:
**UK Train Stations** — https://www.kaggle.com/datasets/bill2bill/uk-train-stations
