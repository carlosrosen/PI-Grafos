#include "pontes.h"
#include "acesso_grafo.h"

#include <stdio.h>
#include <stdlib.h>

/*
    DFS com low-link (Tarjan) para deteccao de pontes, unica para as duas
    representacoes de grafo (lista e matriz de adjacencia). A diferenca de
    iteracao dos vizinhos fica nos adaptadores de acesso_grafo.c.

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
} EstadoDFSPontes;

static EstadoDFSPontes *criar_estado_dfs(int V) {
    EstadoDFSPontes *estado = malloc(sizeof(EstadoDFSPontes));
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

static void liberar_estado_dfs(EstadoDFSPontes *estado) {
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

static void dfs_pontes(const AcessoGrafo *acesso, EstadoDFSPontes *estado, int u) {
    estado->disc[u] = estado->low[u] = estado->tempo++;

    CursorVizinhos cursor;
    for (int v = acesso->iniciar_vizinhos(acesso->grafo, u, &cursor); v != -1;
         v = acesso->proximo_vizinho(acesso->grafo, u, &cursor)) {

        if (v == u) continue; /* laco (ex.: LST -> LST) */

        if (estado->disc[v] == -1) {
            estado->pai[v] = u;
            dfs_pontes(acesso, estado, v);

            if (estado->low[v] < estado->low[u]) estado->low[u] = estado->low[v];
            if (estado->low[v] > estado->disc[u]) adicionar_ponte(estado->pontes, u, v);
        } else if (v != estado->pai[u]) {
            if (estado->disc[v] < estado->low[u]) estado->low[u] = estado->disc[v];
        }
    }
}

// Roda a DFS a partir de todos os vertices ainda nao visitados e devolve a lista de pontes.
static ListaPontes *executar_pontes(const AcessoGrafo *acesso) {
    int V = acesso->qtd_vertices;
    EstadoDFSPontes *estado = criar_estado_dfs(V);

    for (int i = 0; i < V; i++) {
        if (estado->disc[i] == -1) dfs_pontes(acesso, estado, i);
    }

    ListaPontes *pontes = estado->pontes;
    liberar_estado_dfs(estado);
    return pontes;
}

ListaPontes *encontrar_pontes_lista(GrafoLista *grafo) {
    if (!grafo) return NULL;
    AcessoGrafo acesso = criar_acesso_lista(grafo);
    return executar_pontes(&acesso);
}

ListaPontes *encontrar_pontes_matriz(GrafoMatriz *grafo) {
    if (!grafo) return NULL;
    AcessoGrafo acesso = criar_acesso_matriz(grafo);
    return executar_pontes(&acesso);
}

/* Escreve uma ponte no destino (stdout ou arquivo) no formato padrao. */
static void imprimir_uma_ponte(FILE *saida, const AcessoGrafo *acesso, ListaPontes *pontes, int i) {
    int u = pontes->itens[i].u, v = pontes->itens[i].v;
    DadosEstacao *du = acesso->obter_dados_estacao(acesso->grafo, u);
    DadosEstacao *dv = acesso->obter_dados_estacao(acesso->grafo, v);
    fprintf(saida, "%d. [%d <-> %d] (%s) %s <-> (%s) %s\n",
            i + 1, u, v, du->code, du->nome, dv->code, dv->nome);
}

static void imprimir_pontes(const AcessoGrafo *acesso, ListaPontes *pontes, const char *representacao) {
    if (!acesso || !acesso->grafo || !pontes) return;

    printf("\n=== Pontes (%s) ===\n", representacao);
    printf("Quantidade de pontes encontradas: %d\n\n", pontes->quantidade);
    for (int i = 0; i < pontes->quantidade; i++)
        imprimir_uma_ponte(stdout, acesso, pontes, i);
}

void imprimir_pontes_lista(GrafoLista *grafo, ListaPontes *pontes) {
    AcessoGrafo acesso = criar_acesso_lista(grafo);
    imprimir_pontes(&acesso, pontes, "lista de adjacencia");
}

void imprimir_pontes_matriz(GrafoMatriz *grafo, ListaPontes *pontes) {
    AcessoGrafo acesso = criar_acesso_matriz(grafo);
    imprimir_pontes(&acesso, pontes, "matriz de adjacencia");
}

static void exportar_pontes(const AcessoGrafo *acesso, ListaPontes *pontes, char *output_path, const char *representacao) {
    if (!acesso || !acesso->grafo || !pontes || !output_path) return;

    FILE *fp = fopen(output_path, "w"); /* "w" apaga o conteudo anterior */
    if (fp == NULL) {
        perror("Falha ao abrir arquivo de saida das pontes");
        return;
    }
    fprintf(fp, "=== Pontes (%s) ===\n", representacao);
    fprintf(fp, "Quantidade de pontes encontradas: %d\n\n", pontes->quantidade);
    for (int i = 0; i < pontes->quantidade; i++)
        imprimir_uma_ponte(fp, acesso, pontes, i);
    fclose(fp);
}

void exportar_pontes_lista(GrafoLista *grafo, ListaPontes *pontes, char *output_path) {
    AcessoGrafo acesso = criar_acesso_lista(grafo);
    exportar_pontes(&acesso, pontes, output_path, "lista de adjacencia");
}

void exportar_pontes_matriz(GrafoMatriz *grafo, ListaPontes *pontes, char *output_path) {
    AcessoGrafo acesso = criar_acesso_matriz(grafo);
    exportar_pontes(&acesso, pontes, output_path, "matriz de adjacencia");
}

void liberar_pontes(ListaPontes *pontes) {
    if (pontes == NULL) return;
    free(pontes->itens);
    free(pontes);
}
