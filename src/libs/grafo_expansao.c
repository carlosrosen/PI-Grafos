#include "grafo_expansao.h"

#include <stdlib.h>
#include <string.h>

static int validar_arestas(int n, const ArestaAGM *arestas, int m) {
    if (n <= 0 || n > AGM_MAX_VERTICES || m < 0 || m > AGM_MAX_ARESTAS ||
        (m > 0 && arestas == NULL)) return 0;
    for (int i = 0; i < m; i++) {
        int u = arestas[i].origem, v = arestas[i].destino;
        if (u < 0 || u >= n || v < 0 || v >= n || u == v) return 0;
        for (int j = 0; j < i; j++) {
            if ((arestas[j].origem == u && arestas[j].destino == v) ||
                (arestas[j].origem == v && arestas[j].destino == u)) return 0;
        }
    }
    return 1;
}

void liberar_grafo_expansao_lista(GrafoExpansaoLista *grafo) {
    if (grafo == NULL) return;
    for (int i = 0; i < grafo->n; i++) {
        VizinhoExpansao *vizinho = grafo->adj[i];
        while (vizinho != NULL) {
            VizinhoExpansao *proximo = vizinho->proximo;
            free(vizinho);
            vizinho = proximo;
        }
    }
    *grafo = (GrafoExpansaoLista){0};
}

int criar_grafo_expansao_lista(GrafoExpansaoLista *grafo, int qtd_vertices,
                              const ArestaAGM *arestas, int qtd_arestas) {
    if (grafo == NULL) return 0;
    *grafo = (GrafoExpansaoLista){0};
    if (!validar_arestas(qtd_vertices, arestas, qtd_arestas)) return 0;
    grafo->n = qtd_vertices;
    grafo->m = qtd_arestas;
    for (int i = 0; i < qtd_arestas; i++) {
        int u = arestas[i].origem, v = arestas[i].destino;
        VizinhoExpansao *uv = malloc(sizeof *uv);
        VizinhoExpansao *vu = malloc(sizeof *vu);
        if (uv == NULL || vu == NULL) {
            free(uv);
            free(vu);
            liberar_grafo_expansao_lista(grafo);
            return 0;
        }
        grafo->arestas[i] = arestas[i];
        *uv = (VizinhoExpansao){v, i, grafo->adj[u]};
        *vu = (VizinhoExpansao){u, i, grafo->adj[v]};
        grafo->adj[u] = uv;
        grafo->adj[v] = vu;
    }
    return 1;
}

int extrair_arestas_expansao_lista(const GrafoExpansaoLista *grafo, ArestaAGM *arestas) {
    if (grafo == NULL || arestas == NULL) return -1;
    int total = 0;
    for (int u = 0; u < grafo->n; u++) {
        for (const VizinhoExpansao *v = grafo->adj[u]; v != NULL; v = v->proximo) {
            if (u < v->destino) arestas[total++] = grafo->arestas[v->indice_aresta];
        }
    }
    return total;
}

void liberar_grafo_expansao_matriz(GrafoExpansaoMatriz *grafo) {
    /* A matriz usa armazenamento fixo e não possui alocações dinâmicas. */
    if (grafo != NULL) *grafo = (GrafoExpansaoMatriz){0};
}

int criar_grafo_expansao_matriz(GrafoExpansaoMatriz *grafo, int qtd_vertices,
                               const ArestaAGM *arestas, int qtd_arestas) {
    if (grafo == NULL) return 0;
    *grafo = (GrafoExpansaoMatriz){0};
    if (!validar_arestas(qtd_vertices, arestas, qtd_arestas)) return 0;
    grafo->n = qtd_vertices;
    grafo->m = qtd_arestas;
    for (int u = 0; u < qtd_vertices; u++)
        for (int v = 0; v < qtd_vertices; v++) grafo->indice_aresta[u][v] = -1;
    for (int i = 0; i < qtd_arestas; i++) {
        int u = arestas[i].origem, v = arestas[i].destino;
        grafo->arestas[i] = arestas[i];
        grafo->indice_aresta[u][v] = grafo->indice_aresta[v][u] = i;
    }
    return 1;
}

int extrair_arestas_expansao_matriz(const GrafoExpansaoMatriz *grafo, ArestaAGM *arestas) {
    if (grafo == NULL || arestas == NULL) return -1;
    int total = 0;
    for (int u = 0; u < grafo->n; u++) {
        for (int v = u + 1; v < grafo->n; v++) {
            int indice = grafo->indice_aresta[u][v];
            if (indice >= 0) arestas[total++] = grafo->arestas[indice];
        }
    }
    return total;
}
