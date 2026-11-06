#ifndef GRAFO_EXPANSAO_H
#define GRAFO_EXPANSAO_H

#define AGM_MAX_VERTICES 11
#define AGM_MAX_ARESTAS 25
#define AGM_TAMANHO_CODIGO 16

typedef struct ArestaAGM {
    int origem;
    int destino;
    char codigo_origem[AGM_TAMANHO_CODIGO];
    char codigo_destino[AGM_TAMANHO_CODIGO];
    char ancora[AGM_TAMANHO_CODIGO];
    double custo_brl;
    double custo_gbp;
} ArestaAGM;

typedef struct VizinhoExpansao {
    int destino;
    int indice_aresta;
    struct VizinhoExpansao *proximo;
} VizinhoExpansao;

typedef struct GrafoExpansaoLista {
    int n;
    int m;
    ArestaAGM arestas[AGM_MAX_ARESTAS];
    VizinhoExpansao *adj[AGM_MAX_VERTICES];
} GrafoExpansaoLista;

typedef struct GrafoExpansaoMatriz {
    int n;
    int m;
    int indice_aresta[AGM_MAX_VERTICES][AGM_MAX_VERTICES];
    ArestaAGM arestas[AGM_MAX_ARESTAS];
} GrafoExpansaoMatriz;

/* Grafos simples não direcionados. Criação copia as arestas recebidas.
 * Retorna 1 em sucesso, 0 para entrada inválida ou falha de alocação.
 * Não recriar uma lista antes de liberá-la. Extração exige espaço para m arestas. */
int criar_grafo_expansao_lista(GrafoExpansaoLista *grafo, int qtd_vertices,
                              const ArestaAGM *arestas, int qtd_arestas);
void liberar_grafo_expansao_lista(GrafoExpansaoLista *grafo);
int extrair_arestas_expansao_lista(const GrafoExpansaoLista *grafo, ArestaAGM *arestas);
int criar_grafo_expansao_matriz(GrafoExpansaoMatriz *grafo, int qtd_vertices,
                               const ArestaAGM *arestas, int qtd_arestas);
void liberar_grafo_expansao_matriz(GrafoExpansaoMatriz *grafo);
int extrair_arestas_expansao_matriz(const GrafoExpansaoMatriz *grafo, ArestaAGM *arestas);

#endif
