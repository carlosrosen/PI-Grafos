#include "agm.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

typedef struct ArestaEsperada {
    const char *origem;
    const char *destino;
    double brl;
    double gbp;
    const char *ancora;
} ArestaEsperada;

static const ArestaEsperada ESPERADAS[] = {
    {"NB04", "NETWORK", 0.88, 0.1180, "NDL"},
    {"NB01", "NETWORK", 1.21, 0.1622, "CRH"},
    {"NB01", "NB03",    2.19, 0.2936, ""},
    {"NB01", "NB02",    2.39, 0.3204, ""},
    {"NB08", "NETWORK", 3.34, 0.4478, "DNM"},
    {"NB02", "NB06",    3.62, 0.4853, ""},
    {"NB02", "NB10",    5.66, 0.7588, ""},
    {"NB09", "NB10",    5.92, 0.7936, ""},
    {"NB04", "NB05",   18.34, 2.4587, ""},
    {"NB04", "NB07",   19.35, 2.5941, ""},
};

static int encontrar(int *pai, int vertice) {
    if (pai[vertice] != vertice) pai[vertice] = encontrar(pai, pai[vertice]);
    return pai[vertice];
}

static void verificar_ausencia_de_ciclos(const ResultadoAGM *resultado) {
    int pai[AGM_MAX_VERTICES];
    for (int i = 0; i < resultado->qtd_vertices; i++) pai[i] = i;

    for (int i = 0; i < resultado->qtd_arestas; i++) {
        int origem = encontrar(pai, resultado->arestas[i].origem);
        int destino = encontrar(pai, resultado->arestas[i].destino);
        assert(origem != destino);
        pai[destino] = origem;
    }
}

static void verificar_arestas_e_custos(const ResultadoAGM *resultado) {
    double total_brl = 0.0;
    double total_gbp = 0.0;

    assert(resultado->qtd_arestas == (int)(sizeof ESPERADAS / sizeof ESPERADAS[0]));
    for (int i = 0; i < resultado->qtd_arestas; i++) {
        const ArestaAGM *obtida = &resultado->arestas[i];
        const ArestaEsperada *esperada = &ESPERADAS[i];
        assert(strcmp(obtida->codigo_origem, esperada->origem) == 0);
        assert(strcmp(obtida->codigo_destino, esperada->destino) == 0);
        assert(strcmp(obtida->ancora, esperada->ancora) == 0);
        assert(fabs(obtida->custo_brl - esperada->brl) < 1e-9);
        assert(fabs(obtida->custo_gbp - esperada->gbp) < 1e-9);
        total_brl += obtida->custo_brl;
        total_gbp += obtida->custo_gbp;
    }
    assert(fabs(resultado->custo_total_brl - total_brl) < 1e-9);
    assert(fabs(resultado->custo_total_gbp - total_gbp) < 1e-9);
}

static void verificar_arquivo(const char *caminho) {
    FILE *arquivo = fopen(caminho, "r");
    assert(arquivo != NULL);
    assert(fgetc(arquivo) != EOF);
    fclose(arquivo);
}

static void imprimir_resultado(const ResultadoAGM *resultado) {
    puts("\nArestas selecionadas:");
    for (int i = 0; i < resultado->qtd_arestas; i++) {
        const ArestaAGM *a = &resultado->arestas[i];
        printf("  %s -- %s | R$ %.2f bilhoes | £ %.4f bilhoes",
               a->codigo_origem, a->codigo_destino, a->custo_brl, a->custo_gbp);
        if (a->ancora[0] != '\0') printf(" | ancora %s", a->ancora);
        putchar('\n');
    }
    printf("Total: R$ %.2f bilhoes | £ %.4f bilhoes\n",
           resultado->custo_total_brl, resultado->custo_total_gbp);
}

int main(void) {
    ResultadoAGM lista, matriz;

    puts("=== TESTE DA ARVORE GERADORA MINIMA ===");
    assert(executar_agm_lista("dataset/expansion_neighborhoods.csv",
                              "dataset/expansion_tunnels.csv", &lista));
    assert(executar_agm_matriz("dataset/expansion_neighborhoods.csv",
                               "dataset/expansion_tunnels.csv", &matriz));

    assert(lista.qtd_vertices == 11);
    assert(lista.qtd_arestas_candidatas == 25);
    assert(matriz.qtd_vertices == 11);
    assert(matriz.qtd_arestas_candidatas == 25);
    puts("Dados carregados: 10 bairros, superno NETWORK e 25 arestas.");

    assert(lista.qtd_arestas == 10);
    assert(matriz.qtd_arestas == 10);
    assert(lista.conectado);
    assert(matriz.conectado);
    verificar_ausencia_de_ciclos(&lista);
    verificar_ausencia_de_ciclos(&matriz);
    puts("Estrutura resultante: 10 arestas, sem ciclos.");

    verificar_arestas_e_custos(&lista);
    verificar_arestas_e_custos(&matriz);
    puts("Custos individuais e totais verificados.");

    assert(resultados_agm_iguais(&lista, &matriz));
    puts("Resultados equivalentes para lista e matriz.");

    assert(fabs(lista.custo_total_brl - 62.90) < 1e-9);
    assert(fabs(lista.custo_total_gbp - 8.4325) < 1e-9);
    imprimir_resultado(&lista);

    assert(exportar_agm(&lista, "output/agm_lista.txt", "output/agm_lista.dot", "lista de adjacencia"));
    assert(exportar_agm(&matriz, "output/agm_matriz.txt", "output/agm_matriz.dot", "matriz de adjacencia"));
    verificar_arquivo("output/agm_lista.txt");
    verificar_arquivo("output/agm_matriz.txt");
    verificar_arquivo("output/agm_lista.dot");
    verificar_arquivo("output/agm_matriz.dot");
    puts("\nArquivos TXT e DOT gerados para as duas representacoes.");
    puts("Teste concluido com sucesso.");
    return 0;
}
