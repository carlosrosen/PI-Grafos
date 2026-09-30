#include "./grafo.h"

#ifndef GRAFO_LISTA_H
#define GRAFO_LISTA_H

typedef struct No{
    int vertice;
    DadosEstacao *dados;
    struct No* proximo;
} No;

typedef struct Aresta{
    DadosAresta *dados;
    No *source;
    No *target;
} Aresta;

typedef struct grafo{
    unsigned int qtd_no;
    unsigned int qtd_arestas;
    Aresta** arestas;
    No **lista;
} GrafoLista;

GrafoLista* inicializar_grafo_lista(char* path_stations, char* path_edges);
void exibir_lista(GrafoLista* grafo);
void imprimir_lista(GrafoLista *grafo, char* output_path);
void imprimir_lista_dot(GrafoLista *grafo, char* output_path);
void Liberar_Grafo(GrafoLista* grafo);

#endif