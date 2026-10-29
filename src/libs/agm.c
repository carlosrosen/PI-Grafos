#include "agm.h"
#include "kruskal.h"

#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#endif

#define LINHA_CSV 512
#define EPSILON 1e-9

typedef struct VizinhoExpansao {
    int destino;
    int indice_aresta;
    struct VizinhoExpansao *proximo;
} VizinhoExpansao;

typedef struct GrafoExpansaoLista {
    int n;
    int m;
    char codigos[AGM_MAX_VERTICES][AGM_TAMANHO_CODIGO];
    ArestaAGM arestas[AGM_MAX_ARESTAS];
    VizinhoExpansao *adj[AGM_MAX_VERTICES];
} GrafoExpansaoLista;

typedef struct GrafoExpansaoMatriz {
    int n;
    int m;
    char codigos[AGM_MAX_VERTICES][AGM_TAMANHO_CODIGO];
    int indice_aresta[AGM_MAX_VERTICES][AGM_MAX_VERTICES];
    ArestaAGM arestas[AGM_MAX_ARESTAS];
} GrafoExpansaoMatriz;

static void remover_quebra_linha(char *texto) {
    texto[strcspn(texto, "\r\n")] = '\0';
}

static int carregar_vertices(FILE *arquivo, char codigos[][AGM_TAMANHO_CODIGO]) {
    char linha[LINHA_CSV];
    int n = 0;
    if (fgets(linha, sizeof linha, arquivo) == NULL) return -1;
    while (fgets(linha, sizeof linha, arquivo) != NULL) {
        char codigo[AGM_TAMANHO_CODIGO];
        char nome[128];
        double latitude, longitude;
        if (sscanf(linha, "%15[^,],%127[^,],%lf,%lf", codigo, nome,
                   &latitude, &longitude) != 4 || n >= AGM_MAX_VERTICES - 1) return -1;
        strcpy(codigos[n++], codigo);
    }
    strcpy(codigos[n++], "NETWORK");
    return n;
}

static int indice_codigo(char codigos[][AGM_TAMANHO_CODIGO], int n, const char *codigo) {
    for (int i = 0; i < n; i++) {
        if (strcmp(codigos[i], codigo) == 0) return i;
    }
    return -1;
}

static void normalizar_codigos(ArestaAGM *aresta) {
    if (strcmp(aresta->codigo_origem, aresta->codigo_destino) > 0) {
        char codigo[AGM_TAMANHO_CODIGO];
        strcpy(codigo, aresta->codigo_origem);
        strcpy(aresta->codigo_origem, aresta->codigo_destino);
        strcpy(aresta->codigo_destino, codigo);
        int indice = aresta->origem;
        aresta->origem = aresta->destino;
        aresta->destino = indice;
    }
}

static int carregar_arestas(FILE *arquivo, char codigos[][AGM_TAMANHO_CODIGO],
                            int n, ArestaAGM *arestas) {
    char linha[LINHA_CSV];
    int m = 0;
    if (fgets(linha, sizeof linha, arquivo) == NULL) return -1;
    while (fgets(linha, sizeof linha, arquivo) != NULL) {
        char origem[AGM_TAMANHO_CODIGO], destino[AGM_TAMANHO_CODIGO];
        char ancora[AGM_TAMANHO_CODIGO] = "";
        double distancia, sinuosidade, brl, gbp;
        remover_quebra_linha(linha);
        int campos = sscanf(linha, "%15[^,],%15[^,],%lf,%lf,%lf,%lf,%15s",
                            origem, destino, &distancia, &sinuosidade, &brl, &gbp, ancora);
        if (campos < 6 || m >= AGM_MAX_ARESTAS) return -1;
        ArestaAGM *a = &arestas[m];
        memset(a, 0, sizeof *a);
        a->origem = indice_codigo(codigos, n, origem);
        a->destino = indice_codigo(codigos, n, destino);
        if (a->origem < 0 || a->destino < 0) return -1;
        strcpy(a->codigo_origem, origem);
        strcpy(a->codigo_destino, destino);
        if (campos == 7) strcpy(a->ancora, ancora);
        a->custo_brl = brl;
        a->custo_gbp = gbp;
        normalizar_codigos(a);
        m++;
    }
    return m;
}

static int abrir_csvs(const char *bairros_path, const char *tuneis_path,
                      FILE **bairros, FILE **tuneis) {
    *bairros = fopen(bairros_path, "r");
    *tuneis = fopen(tuneis_path, "r");
    if (*bairros == NULL || *tuneis == NULL) {
        fprintf(stderr, "Erro ao abrir dataset de expansao.\n");
        if (*bairros != NULL) fclose(*bairros);
        if (*tuneis != NULL) fclose(*tuneis);
        return 0;
    }
    return 1;
}

int executar_agm_lista(const char *bairros_path, const char *tuneis_path,
                       ResultadoAGM *resultado) {
    if (resultado == NULL) return 0;
    FILE *bairros, *tuneis;
    if (!abrir_csvs(bairros_path, tuneis_path, &bairros, &tuneis)) return 0;
    GrafoExpansaoLista g = {0};
    g.n = carregar_vertices(bairros, g.codigos);
    g.m = g.n > 0 ? carregar_arestas(tuneis, g.codigos, g.n, g.arestas) : -1;
    fclose(bairros);
    fclose(tuneis);
    if (g.n != AGM_MAX_VERTICES || g.m != AGM_MAX_ARESTAS) return 0;
    for (int i = 0; i < g.m; i++) {
        VizinhoExpansao *uv = malloc(sizeof *uv);
        VizinhoExpansao *vu = malloc(sizeof *vu);
        if (uv == NULL || vu == NULL) {
            free(uv); free(vu);
            for (int v = 0; v < g.n; v++) while (g.adj[v]) {
                VizinhoExpansao *p = g.adj[v]; g.adj[v] = p->proximo; free(p);
            }
            return 0;
        }
        *uv = (VizinhoExpansao){g.arestas[i].destino, i, g.adj[g.arestas[i].origem]};
        g.adj[g.arestas[i].origem] = uv;
        *vu = (VizinhoExpansao){g.arestas[i].origem, i, g.adj[g.arestas[i].destino]};
        g.adj[g.arestas[i].destino] = vu;
    }
    ArestaAGM coletadas[AGM_MAX_ARESTAS];
    int total = 0;
    for (int u = 0; u < g.n; u++) {
        for (VizinhoExpansao *v = g.adj[u]; v != NULL; v = v->proximo) {
            if (u < v->destino) coletadas[total++] = g.arestas[v->indice_aresta];
        }
    }
    int sucesso = total == g.m &&
                  executar_kruskal_agm(coletadas, g.n, total, resultado);
    for (int v = 0; v < g.n; v++) while (g.adj[v]) {
        VizinhoExpansao *p = g.adj[v]; g.adj[v] = p->proximo; free(p);
    }
    return sucesso;
}

int executar_agm_matriz(const char *bairros_path, const char *tuneis_path,
                        ResultadoAGM *resultado) {
    if (resultado == NULL) return 0;
    FILE *bairros, *tuneis;
    if (!abrir_csvs(bairros_path, tuneis_path, &bairros, &tuneis)) return 0;
    GrafoExpansaoMatriz g = {0};
    for (int i = 0; i < AGM_MAX_VERTICES; i++)
        for (int j = 0; j < AGM_MAX_VERTICES; j++) g.indice_aresta[i][j] = -1;
    g.n = carregar_vertices(bairros, g.codigos);
    g.m = g.n > 0 ? carregar_arestas(tuneis, g.codigos, g.n, g.arestas) : -1;
    fclose(bairros);
    fclose(tuneis);
    if (g.n != AGM_MAX_VERTICES || g.m != AGM_MAX_ARESTAS) return 0;
    for (int i = 0; i < g.m; i++) {
        int u = g.arestas[i].origem, v = g.arestas[i].destino;
        g.indice_aresta[u][v] = g.indice_aresta[v][u] = i;
    }
    ArestaAGM coletadas[AGM_MAX_ARESTAS];
    int total = 0;
    for (int u = 0; u < g.n; u++) for (int v = u + 1; v < g.n; v++) {
        int indice = g.indice_aresta[u][v];
        if (indice >= 0) coletadas[total++] = g.arestas[indice];
    }
    return total == g.m &&
           executar_kruskal_agm(coletadas, g.n, total, resultado);
}

static int criar_diretorio_output(void) {
#ifdef _WIN32
    int status = _mkdir("output");
#else
    int status = mkdir("output", 0777);
#endif
    return status == 0 || errno == EEXIST;
}

int exportar_agm(const ResultadoAGM *r, const char *txt_path,
                 const char *dot_path, const char *representacao) {
    if (r == NULL || !criar_diretorio_output()) return 0;
    FILE *txt = fopen(txt_path, "w");
    FILE *dot = fopen(dot_path, "w");
    if (txt == NULL || dot == NULL) {
        if (txt) fclose(txt);
        if (dot) fclose(dot);
        return 0;
    }
    fprintf(txt, "AGM da expansao - %s\n", representacao);
    fprintf(txt, "Vertices: %d (10 bairros + NETWORK)\n", r->qtd_vertices);
    fprintf(txt, "Arestas candidatas: %d\n", r->qtd_arestas_candidatas);
    fprintf(txt, "Arestas selecionadas: %d\n\n", r->qtd_arestas);
    for (int i = 0; i < r->qtd_arestas; i++) {
        const ArestaAGM *a = &r->arestas[i];
        fprintf(txt, "%s -- %s: R$ %.2f bilhoes | £ %.4f bilhoes",
                a->codigo_origem, a->codigo_destino, a->custo_brl, a->custo_gbp);
        if (a->ancora[0]) fprintf(txt, " | ancora %s", a->ancora);
        fputc('\n', txt);
    }
    fprintf(txt, "\nCusto total: R$ %.2f bilhoes | £ %.4f bilhoes\n",
            r->custo_total_brl, r->custo_total_gbp);
    fprintf(txt, "Status: %s\n", r->conectado ? "todos os bairros conectados" : "ha bairros impossiveis de conectar");

    fprintf(dot, "graph AGM {\n  label=\"AGM da expansao - %s\";\n", representacao);
    fprintf(dot, "  labelloc=\"t\";\n  node [shape=ellipse];\n  NETWORK [shape=box, style=filled, fillcolor=lightgray];\n");
    for (int i = 0; i < r->qtd_arestas; i++) {
        const ArestaAGM *a = &r->arestas[i];
        fprintf(dot, "  \"%s\" -- \"%s\" [label=\"R$ %.2f | £ %.4f",
                a->codigo_origem, a->codigo_destino, a->custo_brl, a->custo_gbp);
        if (a->ancora[0]) fprintf(dot, " | %s", a->ancora);
        fprintf(dot, "\"];\n");
    }
    fprintf(dot, "}\n");
    fclose(txt);
    fclose(dot);
    return 1;
}

int resultados_agm_iguais(const ResultadoAGM *a, const ResultadoAGM *b) {
    if (a == NULL || b == NULL || a->qtd_arestas != b->qtd_arestas ||
        fabs(a->custo_total_brl - b->custo_total_brl) > EPSILON ||
        fabs(a->custo_total_gbp - b->custo_total_gbp) > EPSILON) return 0;
    for (int i = 0; i < a->qtd_arestas; i++) {
        if (strcmp(a->arestas[i].codigo_origem, b->arestas[i].codigo_origem) != 0 ||
            strcmp(a->arestas[i].codigo_destino, b->arestas[i].codigo_destino) != 0 ||
            strcmp(a->arestas[i].ancora, b->arestas[i].ancora) != 0 ||
            fabs(a->arestas[i].custo_brl - b->arestas[i].custo_brl) > EPSILON ||
            fabs(a->arestas[i].custo_gbp - b->arestas[i].custo_gbp) > EPSILON) return 0;
    }
    return 1;
}

void exibir_resultado_agm(const ResultadoAGM *r, const char *representacao) {
    printf("\nAGM da expansao (%s)\n", representacao);
    if (!r->conectado) printf("Aviso: ha bairros impossiveis de conectar.\n");
    for (int i = 0; i < r->qtd_arestas; i++) {
        const ArestaAGM *a = &r->arestas[i];
        printf("  %s -- %s: R$ %.2f bilhoes | £ %.4f bilhoes",
               a->codigo_origem, a->codigo_destino, a->custo_brl, a->custo_gbp);
        if (a->ancora[0]) printf(" | ancora %s", a->ancora);
        putchar('\n');
    }
    printf("Custo total: R$ %.2f bilhoes | £ %.4f bilhoes\n",
           r->custo_total_brl, r->custo_total_gbp);
}
