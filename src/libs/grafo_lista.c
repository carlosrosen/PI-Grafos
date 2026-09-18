#include "grafo_lista.h"
#include "dataset.h"

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<unistd.h>

#define BUFFER_SIZE 4096

void Liberar_Nos(No** nos, unsigned int no_length);

GrafoLista* inicializar_grafo_l(){
    GrafoLista* grafo = (GrafoLista *)malloc(sizeof(GrafoLista));
    if(grafo == NULL){
        fprintf(stderr, "Falha ao alocar memória para o grafo\n"); //Direciona a mensagem para a stream de erro.
        exit(EXIT_FAILURE);
    }
    
    ResponseObjectLength *response = inicializa_stations();
    No **stations = (No**)response->object;
    unsigned int stations_length = response->length;
    free(response);
    
    response = Inicializar_edges(stations,stations_length);
    
    Aresta **array_arestas = (Aresta**)response->object;
    unsigned int arestas_length = response->length;
    free(response);
    
    grafo->lista = stations;
    grafo->arestas = array_arestas;
    grafo->qtd_no = stations_length;
    grafo->qtd_arestas = arestas_length;
    
    return grafo;
}

void Liberar_Nos(No** nos, unsigned int no_length){
    if(nos == NULL)return;
    for(unsigned int i = 0; i < no_length; i++){
        free(nos[i]->dados->nome);
        free(nos[i]->dados->code);
        free(nos[i]->dados);
        free(nos[i]);
    }
    free(nos);
}

void Liberar_Arestas(Aresta** arestas, unsigned int qtd_arestas){
    if(arestas == NULL)return;
    for(unsigned int i = 0; i < qtd_arestas; i++){
        free(arestas[i]);
    }
    free(arestas);
}

void Liberar_Grafo(GrafoLista *grafo){
    if(grafo == NULL)return;
    Liberar_Nos(grafo->lista, grafo->qtd_no);
    Liberar_Arestas(grafo->arestas, grafo->qtd_arestas);
    free(grafo);
}