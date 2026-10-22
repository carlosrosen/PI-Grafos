#include "topologia.h"

#include "grafo_biconexo.h"
#include "pontes.h"

#include <stdio.h>

static void imprimir_resumo(ResultadoBiconexo *biconexo, ListaPontes *pontes, const char *arquivo_biconexo, const char *arquivo_articulacoes, const char *arquivo_pontes){
    printf("\nComponentes biconexos: %d", biconexo->qtd_componentes);
    printf("\nVertices de articulacao: %d", biconexo->qtd_articulacoes);
    printf("\nPontes: %d", pontes->quantidade);
    printf("\nArquivos gerados:\n");
    printf("  %s\n", arquivo_biconexo);
    printf("  %s\n", arquivo_articulacoes);
    printf("  %s\n", arquivo_pontes);
}

void exportar_topologia_lista(GrafoLista *grafo) {
    char arquivo_biconexo[] = "output/biconexo_lista.txt";
    char arquivo_articulacoes[] = "output/vertice_articulacao_lista.txt";
    char arquivo_pontes[] = "output/pontes_lista.txt";
    ResultadoBiconexo *biconexo = encontrar_biconexos_lista(grafo);
    ListaPontes *pontes = encontrar_pontes_lista(grafo);

    if (biconexo == NULL || pontes == NULL){
        printf("\nFalha ao calcular a topologia da malha.\n");
    } else{
        exportar_componentes_biconexos_lista(grafo, biconexo, arquivo_biconexo);
        exportar_vertices_articulacao_lista(grafo, biconexo, arquivo_articulacoes);
        exportar_pontes_lista(grafo, pontes, arquivo_pontes);
        imprimir_resumo(biconexo, pontes, arquivo_biconexo, arquivo_articulacoes, arquivo_pontes);
    }

    liberar_resultado_biconexo(&biconexo);
    liberar_pontes(pontes);
}

void exportar_topologia_matriz(GrafoMatriz *grafo) {
    char arquivo_biconexo[] = "output/biconexo_matriz.txt";
    char arquivo_articulacoes[] = "output/vertice_articulacao_matriz.txt";
    char arquivo_pontes[] = "output/pontes_matriz.txt";
    ResultadoBiconexo *biconexo = encontrar_biconexos(grafo);
    ListaPontes *pontes = encontrar_pontes_matriz(grafo);

    if (biconexo == NULL || pontes == NULL) {
        printf("\nFalha ao calcular a topologia da malha.\n");
    } else {
        exportar_componentes_biconexos_matriz(grafo, biconexo, arquivo_biconexo);
        exportar_vertices_articulacao_matriz(grafo, biconexo, arquivo_articulacoes);
        exportar_pontes_matriz(grafo, pontes, arquivo_pontes);
        imprimir_resumo(biconexo, pontes, arquivo_biconexo, arquivo_articulacoes, arquivo_pontes);
    }

    liberar_resultado_biconexo(&biconexo);
    liberar_pontes(pontes);
}
