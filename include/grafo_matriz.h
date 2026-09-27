#include "./grafo.h"

#ifndef GRAFO_MATRIZ_H
#define GRAFO_MATRIZ_H

typedef struct GrafoMatriz{
    int qtd_estacoes, qtd_arestas;
    int **matriz;
    DadosEstacao **dados_estacao;
    DadosAresta **dados_arestas;
} GrafoMatriz;

GrafoMatriz* inicializar_grafo_matriz(char* path_stations, char* path_edges);
void exibir_matriz(GrafoMatriz* grafo);
void imprimir_lista(GrafoMatriz *grafo,char* output_path);
void liberar_grafo_matriz(GrafoMatriz** grafo);
#endif