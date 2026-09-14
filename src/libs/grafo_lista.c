#include "grafo_lista.h"
#include "dataset.h"

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define BUFFER_SIZE 4096

GrafoLista* inicializar_grafo_l(){
    GrafoLista* grafo = (GrafoLista *)malloc(sizeof(GrafoLista));
    if(grafo == NULL){
        fprintf(stderr, "Falha ao alocar memória para o grafo\n"); //Direciona a mensagem para a stream de erro.
        exit(EXIT_FAILURE);
    }

    ResponseObjectLength *res_station = get_stations();
    No **stations = (No**)res_station->object;
    unsigned int stations_length = res_station->length;

    ResponseObjectLength *res_edges = get_edges();

    Aresta **array_arestas = (Aresta**)res_edges->object;
    unsigned int arestas_length = res_edges->length;



    return grafo;
}

// Insere uma aresta na lista de proximo do nó
void _Insert_Aresta_No(No* no, Aresta* aresta){
        char is_connected = 'n';
        for(int idx_no = 0; idx_no < source->qtd_alloc_aresta; idx_no++){
            if(no->proximos[idx_no] == NULL){
                no->proximos[idx_no] = aresta; 
                is_connected = 's';
            }
        }
        if(is_connected == 'n'){
            no->proximos->qtd_alloc_aresta += ARESTA_DEFAULT_INCREASE;
            no->proximos = realloc(no->proximos, sizeof(Aresta*) * no->qtd_alloc_aresta);
            if(no->proximos == NULL){
                perror("Falha ao realocar memória para {no->proximo}");
                exit(EXIT_FAILURE);
            }
            no->proximos[no->proximos->qtd_alloc_aresta - ARESTA_DEFAULT_INCREASE + 1];
        }
}