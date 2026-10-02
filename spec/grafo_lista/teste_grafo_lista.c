#include "grafo_lista.h"
#include <stdio.h>

int main(){
    GrafoLista * grafo = inicializar_grafo_lista("./dataset/stations.csv", "./dataset/edges.csv");

    // exibir_lista(grafo);
    imprimir_lista(grafo, "output/lista.txt");
    imprimir_lista_dot(grafo,"output/grafo_com_arestas_duplicadas.dot");
    Liberar_Grafo(grafo);
    return 0;
}