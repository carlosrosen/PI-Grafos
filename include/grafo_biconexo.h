#ifndef GRAFO_BICONEXO_H
#define GRAFO_BICONEXO_H

#include <stdio.h> 
#include "grafo_lista.h"
#include "grafo_matriz.h"

typedef struct ArestaSimples {
    int u;
    int v;
} ArestaSimples;

typedef struct ComponenteBiconexo {
    ArestaSimples *arestas;
    int qtd_arestas;
} ComponenteBiconexo;

typedef struct ResultadoBiconexo {
    int *articulacoes;
    int qtd_articulacoes;

    ComponenteBiconexo *componentes;
    int qtd_componentes;
} ResultadoBiconexo;

ResultadoBiconexo* encontrar_biconexos_lista(GrafoLista* grafo);

void exportar_componentes_biconexos_lista(GrafoLista* grafo, ResultadoBiconexo* resultado, char* output_path);
void exportar_vertices_articulacao_lista(GrafoLista* grafo, ResultadoBiconexo* resultado, char* output_path);

void liberar_resultado_biconexo_lista(ResultadoBiconexo** resultado);

// Roda a DFS com low-link (Tarjan) sobre o grafo (tratado como não dirigido)
// e devolve os vértices de articulação e os componentes biconexos.
ResultadoBiconexo* encontrar_biconexos(GrafoMatriz* grafo);

void exportar_componentes_biconexos_matriz(GrafoMatriz* grafo, ResultadoBiconexo* resultado, char* output_path);
void exportar_vertices_articulacao_matriz(GrafoMatriz* grafo, ResultadoBiconexo* resultado, char* output_path);

// Libera toda a memória do resultado e zera o ponteiro do chamador.
void liberar_resultado_biconexo(ResultadoBiconexo** resultado);

#endif