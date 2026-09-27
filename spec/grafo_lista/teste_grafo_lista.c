#include "grafo_lista.h"
#include <stdio.h>

int main(){
    GrafoLista * grafo = inicializar_grafo_lista();

    // IMPRESSÃO DAS ESTAÇÕES COM SUAS ARESTAS :
    exibir_lista(grafo);
    

    Liberar_Grafo(grafo);
    return 0;
}