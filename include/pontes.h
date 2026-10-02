#ifndef PONTES_H
#define PONTES_H

#include "grafo_lista.h"
#include "grafo_matriz.h"

/* u e v sao indices 0-indexados no array original do grafo. */
typedef struct {
    int u;
    int v;
} Ponte;

typedef struct {
    Ponte *itens;
    int quantidade;
    int capacidade;
} ListaPontes;

/* Retorna as pontes encontradas. Liberar com liberar_pontes(). */
ListaPontes *encontrar_pontes_lista(GrafoLista *grafo);
ListaPontes *encontrar_pontes_matriz(GrafoMatriz *grafo);

void imprimir_pontes_lista(GrafoLista *grafo, ListaPontes *pontes);
void imprimir_pontes_matriz(GrafoMatriz *grafo, ListaPontes *pontes);

/* Mesma saida, mas escrita em arquivo (ex.: output/pontes_lista.txt). */
void exportar_pontes_lista(GrafoLista *grafo, ListaPontes *pontes, char *output_path);
void exportar_pontes_matriz(GrafoMatriz *grafo, ListaPontes *pontes, char *output_path);

void liberar_pontes(ListaPontes *pontes);

#endif