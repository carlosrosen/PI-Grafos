#include "grafo_matriz.h"
#include <stdio.h>

int main(){
    printf("===================================  TESTES GRAFO MATRIZ  ===================================\n\n");
    GrafoMatriz * grafo = inicializar_grafo_matriz("./dataset/stations.csv", "./dataset/edges.csv");

    // exibir_matriz(grafo);
    imprimir_matriz(grafo,"output/matriz.txt");
    imprimir_matriz_dot(grafo,"output/grafo_sem_aresta_duplicada.dot");

    puts("");
    liberar_grafo_matriz(&grafo);
    return 0;
}