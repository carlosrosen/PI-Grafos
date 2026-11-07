#include "agm.h"

#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#ifdef _WIN32
#include <direct.h>
#else
#endif

#define LINHA_CSV 512

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
        if (strcmp(codigo, "NETWORK") == 0) return -1;
        for (int i = 0; i < n; i++)
            if (strcmp(codigos[i], codigo) == 0) return -1;
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
        if (a->origem < 0 || a->destino < 0 ||
            !isfinite(brl) || !isfinite(gbp) || brl < 0 || gbp < 0) return -1;
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

static int carregar_expansao(const char *bairros_path, const char *tuneis_path,
                             ArestaAGM *arestas, int *n, int *m) {
    if (bairros_path == NULL || tuneis_path == NULL) return 0;
    FILE *bairros, *tuneis;
    if (!abrir_csvs(bairros_path, tuneis_path, &bairros, &tuneis)) return 0;
    char codigos[AGM_MAX_VERTICES][AGM_TAMANHO_CODIGO];
    *n = carregar_vertices(bairros, codigos);
    *m = *n > 0 ? carregar_arestas(tuneis, codigos, *n, arestas) : -1;
    int sucesso = !ferror(bairros) && !ferror(tuneis) &&
                  *n == AGM_MAX_VERTICES && *m >= 0;
    fclose(bairros);
    fclose(tuneis);
    return sucesso;
}

int executar_agm_lista(const char *bairros_path, const char *tuneis_path,
                       ResultadoAGM *resultado) {
    ArestaAGM arestas[AGM_MAX_ARESTAS];
    int n, m;
    if (resultado == NULL ||
        !carregar_expansao(bairros_path, tuneis_path, arestas, &n, &m)) return 0;
    GrafoExpansaoLista grafo;
    if (!criar_grafo_expansao_lista(&grafo, n, arestas, m)) return 0;
    int total = extrair_arestas_expansao_lista(&grafo, arestas);
    int sucesso = executar_kruskal_agm(arestas, n, total, resultado);
    liberar_grafo_expansao_lista(&grafo);
    return sucesso;
}

int executar_agm_matriz(const char *bairros_path, const char *tuneis_path,
                        ResultadoAGM *resultado) {
    ArestaAGM arestas[AGM_MAX_ARESTAS];
    int n, m;
    if (resultado == NULL ||
        !carregar_expansao(bairros_path, tuneis_path, arestas, &n, &m)) return 0;
    GrafoExpansaoMatriz grafo;
    if (!criar_grafo_expansao_matriz(&grafo, n, arestas, m)) return 0;
    int total = extrair_arestas_expansao_matriz(&grafo, arestas);
    int sucesso = executar_kruskal_agm(arestas, n, total, resultado);
    liberar_grafo_expansao_matriz(&grafo);
    return sucesso;
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
    FILE *txt = txt_path ? fopen(txt_path, "w") : NULL;
    FILE *dot = dot_path ? fopen(dot_path, "w") : NULL;
    if ((txt_path && txt == NULL) || (dot_path && dot == NULL)) {
        if (txt) fclose(txt);
        if (dot) fclose(dot);
        return 0;
    }
    if (txt) {
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
        fclose(txt);
    }

    if (dot) {
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
        fclose(dot);
    }
    return 1;
}

void exibir_resultado_agm(const ResultadoAGM *r, const char *representacao) {
    printf("\n============== EXPANSAO DA MALHA (%s) ==============\n", representacao);
    printf("Arestas selecionadas:\n\n");
    printf("#    Origem     Destino     Custo (R$ bi)  Custo (£ bi) Ancora\n");
    printf("---- ---------- ---------- -------------- -------------- --------\n");
    for (int i = 0; i < r->qtd_arestas; i++) {
        const ArestaAGM *a = &r->arestas[i];
        printf("%-4d %-10s %-10s %14.2f %14.4f %s\n", i + 1,
               a->codigo_origem, a->codigo_destino, a->custo_brl, a->custo_gbp,
               a->ancora[0] ? a->ancora : "-");
    }
    printf("-----------------------------------------------------------------\n");
    printf("Custo total minimo: R$ %.2f bilhoes | £ %.4f bilhoes\n",
           r->custo_total_brl, r->custo_total_gbp);
    if (!r->conectado)
        printf("Aviso: ha bairros impossiveis de conectar; resultado e uma floresta.\n");
}
