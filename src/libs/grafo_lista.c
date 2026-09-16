#include "grafo_lista.h"
#include "dataset.h"

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<unistd.h>

#define BUFFER_SIZE 4096

GrafoLista* inicializar_grafo_l(){
    GrafoLista* grafo = (GrafoLista *)malloc(sizeof(GrafoLista));
    if(grafo == NULL){
        fprintf(stderr, "Falha ao alocar memória para o grafo\n"); //Direciona a mensagem para a stream de erro.
        exit(EXIT_FAILURE);
    }

    ResponseObjectLength *res_station = inicializa_stations();
    No **stations = (No**)res_station->object;
    unsigned int stations_length = res_station->length;

    ResponseObjectLength *res_edges = Inicializar_edges(stations,stations_length);

    // Aresta **array_arestas = (Aresta**)res_edges->object;
    unsigned int arestas_length = res_edges->length;

    grafo->lista = stations;
    grafo->qtd_no = stations_length;
    grafo->qtd_arestas = arestas_length;

    return grafo;
}

// Insere uma aresta na lista de proximo do nó
void _Insert_Aresta_No(No* no, Aresta* aresta){
        char is_connected = 'n';
        for(unsigned int idx_no = 0; idx_no < no->qtd_alloc_aresta; idx_no++){
            if(no->proximos[idx_no] == NULL){
                no->proximos[idx_no] = aresta; 
                is_connected = 's';
                break;
            }
        }
        if(is_connected == 'n'){
            no->qtd_alloc_aresta += ARESTA_DEFAULT_INCREASE;
            no->proximos = realloc(no->proximos, sizeof(Aresta*) * no->qtd_alloc_aresta);
            if(no->proximos == NULL){
                perror("Falha ao realocar memória para {no->proximo}");
                exit(EXIT_FAILURE);
            }
            no->proximos[no->qtd_alloc_aresta - ARESTA_DEFAULT_INCREASE] = aresta;
            for(int i = 1; i < 5; i++){
                no->proximos[no->qtd_alloc_aresta - ARESTA_DEFAULT_INCREASE + i] = NULL;
            }
        }
}