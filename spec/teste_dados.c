#include "grafo_lista.h"
#include <stdio.h>

int main(){
    GrafoLista * grafo = inicializar_grafo_l();

    // IMPRESSÃO DAS ESTAÇÕES COM SUAS ARESTAS :
    puts("");
    for(unsigned int i = 0; i < grafo->qtd_no; i++){
        No* no = grafo->lista[i];
        printf("code: %s, lat: %f, long: %f, nome: %s\n",no->dados->code, no->dados->latitude, no->dados->longitude, no->dados->nome);
        while(no != NULL){
            if(no->proximo != NULL)printf("[ %d | %s ] -> ",no->vertice, no->dados->code);
            else printf("[ %d | %s ]\n",no->vertice, no->dados->code);
            no = no->proximo;
        }
    }
    // printf("\tsource: %s, target: %s, distance: %f\n",no->proximos[j]->source->code,no->proximos[j]->target->code,no->proximos[j]->distance);

    Liberar_Grafo(grafo);
    return 0;
}