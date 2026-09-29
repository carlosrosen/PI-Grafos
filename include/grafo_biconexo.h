#ifndef GRAFO_BICONEXO_H
#define GRAFO_BICONEXO_H

#include "grafo_matriz.h"

typedef struct ArestaSimples{
    int u, v;
} ArestaSimples;

typedef struct ComponenteBiconexo{
    ArestaSimples *arestas;
    int qtd_arestas;
} ComponenteBiconexo;

typedef struct ResultadoBiconexo{
    int *articulacoes;      // vetor de tamanho qtd_estacoes: 1 se é articulação, 0 se não é
    int qtd_articulacoes;

    ComponenteBiconexo *componentes;
    int qtd_componentes;
} ResultadoBiconexo;

// Roda a DFS com low-link (Tarjan) sobre o grafo (tratado como não dirigido)
// e devolve os vértices de articulação e os componentes biconexos.
ResultadoBiconexo* encontrar_biconexos(GrafoMatriz* grafo);

// Imprime o resultado no terminal, usando o code de cada estação.
void imprimir_resultado_biconexo(GrafoMatriz* grafo, ResultadoBiconexo* resultado);

// Libera toda a memória do resultado e zera o ponteiro do chamador.
void liberar_resultado_biconexo(ResultadoBiconexo** resultado);

#endif