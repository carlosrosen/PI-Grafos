#include "pontes.h"

#include <stdio.h>
#include <stdlib.h>

/*
    DFS com low-link (Tarjan) para deteccao de pontes.
    disc[u] = tempo de descoberta; low[u] = menor disc alcancavel pela
    subarvore de u usando no maximo uma aresta de retorno.
    (u, v) e ponte quando low[v] > disc[u].
*/

typedef struct {
    int *disc;
    int *low;
    int *pai;
    int tempo;
    ListaPontes *pontes;
} EstadoDFS;

static EstadoDFS *criar_estado_dfs(int V) {
    EstadoDFS *estado = malloc(sizeof(EstadoDFS));
    estado->disc = malloc(sizeof(int) * V);
    estado->low = malloc(sizeof(int) * V);
    estado->pai = malloc(sizeof(int) * V);
    if (!estado->disc || !estado->low || !estado->pai) {
        perror("Falha ao alocar memoria para o estado da DFS de pontes");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < V; i++) {
        estado->disc[i] = -1;
        estado->low[i] = -1;
        estado->pai[i] = -1;
    }
    estado->tempo = 0;
    estado->pontes = malloc(sizeof(ListaPontes));
    estado->pontes->itens = NULL;
    estado->pontes->quantidade = 0;
    estado->pontes->capacidade = 0;
    return estado;
}

static void liberar_estado_dfs(EstadoDFS *estado) {
    free(estado->disc);
    free(estado->low);
    free(estado->pai);
    free(estado); /* pontes e devolvido ao chamador, nao e liberado aqui */
}

/* Crescimento geometrico: amortiza o custo total de insercao para O(n). */
static void adicionar_ponte(ListaPontes *pontes, int u, int v) {
    if (pontes->quantidade == pontes->capacidade) {
        int nova_capacidade = pontes->capacidade ? pontes->capacidade * 2 : 8;
        Ponte *novo = realloc(pontes->itens, sizeof(Ponte) * nova_capacidade);
        if (novo == NULL) {
            perror("Falha ao realocar memoria para a lista de pontes");
            exit(EXIT_FAILURE);
        }
        pontes->itens = novo;
        pontes->capacidade = nova_capacidade;
    }
    pontes->itens[pontes->quantidade].u = u;
    pontes->itens[pontes->quantidade].v = v;
    pontes->quantidade++;
}

/* No->vertice (grafo_lista.c) comeca em 1; o array grafo->lista[] comeca em 0. */
#define AJUSTE_INDICE_VERTICE 1

static void dfs_pontes_lista(int u, EstadoDFS *estado, GrafoLista *grafo) {
    estado->disc[u] = estado->low[u] = estado->tempo++;

    for (No *no = grafo->lista[u]->proximo; no != NULL; no = no->proximo) {
        int v = no->vertice - AJUSTE_INDICE_VERTICE;
        if (v == u) continue; /* laco (ex.: LST -> LST) */

        if (estado->disc[v] == -1) {
            estado->pai[v] = u;
            dfs_pontes_lista(v, estado, grafo);

            if (estado->low[v] < estado->low[u]) estado->low[u] = estado->low[v];
            if (estado->low[v] > estado->disc[u]) adicionar_ponte(estado->pontes, u, v);
        } else if (v != estado->pai[u]) {
            if (estado->disc[v] < estado->low[u]) estado->low[u] = estado->disc[v];
        }
    }
}

ListaPontes *encontrar_pontes_lista(GrafoLista *grafo) {
    int V = (int)grafo->qtd_no;
    EstadoDFS *estado = criar_estado_dfs(V);

    if(!estado) return NULL;

    for (int i = 0; i < V; i++) {
        if (estado->disc[i] == -1) dfs_pontes_lista(i, estado, grafo);
    }

    ListaPontes *pontes = estado->pontes;
    liberar_estado_dfs(estado);
    return pontes;
}

static void dfs_pontes_matriz(int u, EstadoDFS *estado, GrafoMatriz *grafo) {
    estado->disc[u] = estado->low[u] = estado->tempo++;

    for (int v = 0; v < grafo->qtd_estacoes; v++) {
        if (v == u || !grafo->matriz[u][v]) continue;

        if (estado->disc[v] == -1) {
            estado->pai[v] = u;
            dfs_pontes_matriz(v, estado, grafo);

            if (estado->low[v] < estado->low[u]) estado->low[u] = estado->low[v];
            if (estado->low[v] > estado->disc[u]) adicionar_ponte(estado->pontes, u, v);
        } else if (v != estado->pai[u]) {
            if (estado->disc[v] < estado->low[u]) estado->low[u] = estado->disc[v];
        }
    }
}

ListaPontes *encontrar_pontes_matriz(GrafoMatriz *grafo) {
    int V = grafo->qtd_estacoes;
    EstadoDFS *estado = criar_estado_dfs(V);

    if(!estado) return NULL;

    for (int i = 0; i < V; i++) {
        if (estado->disc[i] == -1) dfs_pontes_matriz(i, estado, grafo);
    }

    ListaPontes *pontes = estado->pontes;
    liberar_estado_dfs(estado);
    return pontes;
}

void imprimir_pontes_lista(GrafoLista *grafo, ListaPontes *pontes) {
    printf("\n=== Pontes (lista de adjacencia) ===\n");
    printf("Quantidade de pontes encontradas: %d\n\n", pontes->quantidade);
    for (int i = 0; i < pontes->quantidade; i++) {
        int u = pontes->itens[i].u, v = pontes->itens[i].v;
        DadosEstacao *du = grafo->lista[u]->dados;
        DadosEstacao *dv = grafo->lista[v]->dados;
        printf("%d. [%d <-> %d] (%s) %s <-> (%s) %s\n",
               i + 1, u, v, du->code, du->nome, dv->code, dv->nome);
    }
}

void imprimir_pontes_matriz(GrafoMatriz *grafo, ListaPontes *pontes) {
    printf("\n=== Pontes (matriz de adjacencia) ===\n");
    printf("Quantidade de pontes encontradas: %d\n\n", pontes->quantidade);
    for (int i = 0; i < pontes->quantidade; i++) {
        int u = pontes->itens[i].u, v = pontes->itens[i].v;
        DadosEstacao *du = grafo->dados_estacao[u];
        DadosEstacao *dv = grafo->dados_estacao[v];
        printf("%d. [%d <-> %d] (%s) %s <-> (%s) %s\n",
               i + 1, u, v, du->code, du->nome, dv->code, dv->nome);
    }
}

void exportar_pontes_lista(GrafoLista *grafo, ListaPontes *pontes, char *output_path) {
    FILE *fp = fopen(output_path, "w"); /* "w" apaga o conteudo anterior */
    if (fp == NULL) {
        perror("Falha ao abrir arquivo de saida das pontes (lista)");
        return;
    }
    fprintf(fp, "=== Pontes (lista de adjacencia) ===\n");
    fprintf(fp, "Quantidade de pontes encontradas: %d\n\n", pontes->quantidade);
    for (int i = 0; i < pontes->quantidade; i++) {
        int u = pontes->itens[i].u, v = pontes->itens[i].v;
        DadosEstacao *du = grafo->lista[u]->dados;
        DadosEstacao *dv = grafo->lista[v]->dados;
        fprintf(fp, "%d. [%d <-> %d] (%s) %s <-> (%s) %s\n",
                i + 1, u, v, du->code, du->nome, dv->code, dv->nome);
    }
    fclose(fp);
}

void exportar_pontes_matriz(GrafoMatriz *grafo, ListaPontes *pontes, char *output_path) {
    FILE *fp = fopen(output_path, "w"); 
    if (fp == NULL) {
        perror("Falha ao abrir arquivo de saida das pontes (matriz)");
        return;
    }
    fprintf(fp, "=== Pontes (matriz de adjacencia) ===\n");
    fprintf(fp, "Quantidade de pontes encontradas: %d\n\n", pontes->quantidade);
    for (int i = 0; i < pontes->quantidade; i++) {
        int u = pontes->itens[i].u, v = pontes->itens[i].v;
        DadosEstacao *du = grafo->dados_estacao[u];
        DadosEstacao *dv = grafo->dados_estacao[v];
        fprintf(fp, "%d. [%d <-> %d] (%s) %s <-> (%s) %s\n",
                i + 1, u, v, du->code, du->nome, dv->code, dv->nome);
    }
    fclose(fp);
}

void liberar_pontes(ListaPontes *pontes) {
    if (pontes == NULL) return;
    free(pontes->itens);
    free(pontes);
}