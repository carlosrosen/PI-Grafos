#include "kruskal.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

#define EPSILON_KRUSKAL 1e-9

typedef struct UnionFind {
    int *pai;
    int *rank;
    int quantidade;
} UnionFind;

static int inicializar_union_find(UnionFind *conjuntos, int quantidade) {
    *conjuntos = (UnionFind){0};
    conjuntos->pai = malloc((size_t)quantidade * sizeof(int));
    conjuntos->rank = calloc((size_t)quantidade, sizeof(int));
    if (conjuntos->pai == NULL || conjuntos->rank == NULL) {
        free(conjuntos->pai);
        free(conjuntos->rank);
        *conjuntos = (UnionFind){0};
        return 0;
    }

    conjuntos->quantidade = quantidade;
    for (int i = 0; i < quantidade; i++) conjuntos->pai[i] = i;
    return 1;
}

static void liberar_union_find(UnionFind *conjuntos) {
    free(conjuntos->pai);
    free(conjuntos->rank);
    *conjuntos = (UnionFind){0};
}

/* Compressão de caminho: cada vértice visitado passa a apontar para a raiz. */
static int encontrar_representante(UnionFind *conjuntos, int vertice) {
    if (conjuntos->pai[vertice] != vertice) {
        conjuntos->pai[vertice] = encontrar_representante(
            conjuntos, conjuntos->pai[vertice]
        );
    }
    return conjuntos->pai[vertice];
}

/* União por rank. Retorna zero quando a aresta criaria um ciclo. */
static int unir_conjuntos(UnionFind *conjuntos, int a, int b) {
    a = encontrar_representante(conjuntos, a);
    b = encontrar_representante(conjuntos, b);
    if (a == b) return 0;

    if (conjuntos->rank[a] < conjuntos->rank[b]) {
        conjuntos->pai[a] = b;
    } else {
        conjuntos->pai[b] = a;
        if (conjuntos->rank[a] == conjuntos->rank[b]) conjuntos->rank[a]++;
    }
    return 1;
}

static int comparar_arestas(const void *p1, const void *p2) {
    const ArestaAGM *a = p1;
    const ArestaAGM *b = p2;

    if (fabs(a->custo_brl - b->custo_brl) > EPSILON_KRUSKAL) {
        return a->custo_brl < b->custo_brl ? -1 : 1;
    }

    int comparacao = strcmp(a->codigo_origem, b->codigo_origem);
    if (comparacao != 0) return comparacao;
    return strcmp(a->codigo_destino, b->codigo_destino);
}

static int entrada_valida(const ArestaAGM *arestas, int qtd_vertices,
                          int qtd_arestas, const ResultadoAGM *resultado) {
    if (resultado == NULL || qtd_vertices <= 0 || qtd_arestas < 0 ||
        qtd_vertices > AGM_MAX_VERTICES || qtd_arestas > AGM_MAX_ARESTAS ||
        (qtd_arestas > 0 && arestas == NULL)) return 0;

    for (int i = 0; i < qtd_arestas; i++) {
        if (arestas[i].origem < 0 || arestas[i].origem >= qtd_vertices ||
            arestas[i].destino < 0 || arestas[i].destino >= qtd_vertices) return 0;
    }
    return 1;
}

int executar_kruskal_agm(const ArestaAGM *arestas, int qtd_vertices,
                         int qtd_arestas, ResultadoAGM *resultado) {
    if (!entrada_valida(arestas, qtd_vertices, qtd_arestas, resultado)) return 0;

    ArestaAGM ordenadas[AGM_MAX_ARESTAS];
    if (qtd_arestas > 0) {
        memcpy(ordenadas, arestas, (size_t)qtd_arestas * sizeof(ArestaAGM));
        qsort(ordenadas, (size_t)qtd_arestas, sizeof(ArestaAGM), comparar_arestas);
    }

    memset(resultado, 0, sizeof *resultado);
    resultado->qtd_vertices = qtd_vertices;
    resultado->qtd_arestas_candidatas = qtd_arestas;

    UnionFind conjuntos;
    if (!inicializar_union_find(&conjuntos, qtd_vertices)) return 0;

    for (int i = 0;
         i < qtd_arestas && resultado->qtd_arestas < qtd_vertices - 1;
         i++) {
        if (unir_conjuntos(&conjuntos, ordenadas[i].origem, ordenadas[i].destino)) {
            resultado->arestas[resultado->qtd_arestas++] = ordenadas[i];
            resultado->custo_total_brl += ordenadas[i].custo_brl;
            resultado->custo_total_gbp += ordenadas[i].custo_gbp;
        }
    }

    resultado->conectado = resultado->qtd_arestas == qtd_vertices - 1;
    liberar_union_find(&conjuntos);
    return 1;
}
