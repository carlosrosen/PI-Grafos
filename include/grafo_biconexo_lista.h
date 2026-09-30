#ifndef GRAFO_BICONEXO_LISTA_H
#define GRAFO_BICONEXO_LISTA_H

#include "grafo_lista.h"

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

void imprimir_resultado_biconexo_lista(GrafoLista* grafo, ResultadoBiconexo* resultado);

void liberar_resultado_biconexo_lista(ResultadoBiconexo** resultado);

#endif