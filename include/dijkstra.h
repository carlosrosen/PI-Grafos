#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include "grafo_lista.h"
#include "grafo_matriz.h"

typedef struct NoHeap{
    int vertice;
    double distancia;
} NoHeap;

typedef struct MinHeap{
    NoHeap *itens;
    int tamanho;
    int capacidade;
} MinHeap;

typedef struct ResultadoDijkstra{
    int origem;
    int destino;
    int encontrado;
    double custo;
    int *caminho;
    int tamanho_caminho;
} ResultadoDijkstra;

ResultadoDijkstra *dijkstra_lista(GrafoLista *grafo, int origem, int destino);
ResultadoDijkstra *dijkstra_matriz(GrafoMatriz *grafo, int origem, int destino);

// resolve os codigos das estacoes, calcula e imprime a rota.
// retorna 1 em sucesso, 0 se alguma estacao nao existir e -1 em falha.
int imprimir_rota_por_codigo_lista(GrafoLista *grafo, const char *origem, const char *destino);
int imprimir_rota_por_codigo_matriz(GrafoMatriz *grafo, const char *origem, const char *destino);

void imprimir_resultado_dijkstra_lista(GrafoLista *grafo, ResultadoDijkstra *resultado);
void imprimir_resultado_dijkstra_matriz(GrafoMatriz *grafo, ResultadoDijkstra *resultado);

void exportar_resultado_dijkstra_lista(GrafoLista *grafo, ResultadoDijkstra *resultado, char *output_path);
void exportar_resultado_dijkstra_matriz(GrafoMatriz *grafo, ResultadoDijkstra *resultado, char *output_path);

void liberar_resultado_dijkstra(ResultadoDijkstra **resultado);

#endif
