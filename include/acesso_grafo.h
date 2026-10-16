#ifndef ACESSO_GRAFO_H
#define ACESSO_GRAFO_H

#include "grafo_lista.h"
#include "grafo_matriz.h"

// Estado de iteracao dos vizinhos de um vertice. Cada representacao usa o
// campo que lhe interessa: a lista avanca pela cadeia de nos e a matriz
// avanca pelas colunas da linha.
typedef struct CursorVizinhos{
    No *no;      // lista: proximo no da cadeia de adjacencia
    int coluna;  // matriz: proxima coluna a examinar
} CursorVizinhos;

// Interface comum que abstrai a representacao do grafo. Os adaptadores de
// lista e de matriz preenchem estes campos, e os algoritmos so conhecem
// esta interface.
typedef struct AcessoGrafo{
    void *grafo;
    int qtd_vertices;

    DadosEstacao* (*obter_dados_estacao)(void *grafo, int indice);
    int (*iniciar_vizinhos)(void *grafo, int u, CursorVizinhos *cursor);
    int (*proximo_vizinho)(void *grafo, int u, CursorVizinhos *cursor);
} AcessoGrafo;

// Adaptadores finos de cada representacao.
AcessoGrafo criar_acesso_lista(GrafoLista* grafo);
AcessoGrafo criar_acesso_matriz(GrafoMatriz* grafo);

#endif
