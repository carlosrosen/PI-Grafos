#include "grafo_lista.h"
#include <stdio.h>

int main(){
    GrafoLista * grafo = inicializar_grafo_l();

    // IMPRESSÃO DAS ESTAÇÕES COM SUAS ARESTAS :
    puts("");
    for(unsigned int i = 0; i < grafo->qtd_no; i++){
        printf("code: %s, lat: %f, long: %f, nome: %s, qtd_alloc: %d\n",grafo->lista[i]->code,grafo->lista[i]->latitude,grafo->lista[i]->longitude,grafo->lista[i]->nome,grafo->lista[i]->qtd_alloc_aresta);
        for(unsigned int j = 0; j < grafo->lista[i]->qtd_alloc_aresta; j++){
            if(grafo->lista[i]->proximos[j] != NULL){
                printf("\tj: %d | source: %s, target: %s, distance: %f\n",j,grafo->lista[i]->proximos[j]->source->code,grafo->lista[i]->proximos[j]->target->code,grafo->lista[i]->proximos[j]->distance);
            }
        }
    }

    Liberar_Grafo(grafo);
    return 0;
}