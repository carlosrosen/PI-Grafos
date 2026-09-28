#include "grafo_matriz.h"
#include <stdio.h>

int main(){
    GrafoMatriz * grafo = inicializar_grafo_matriz("./dataset/stations.csv", "./dataset/edges.csv");

    // exibir_matriz(grafo);
    imprimir_matriz(grafo,"output/matriz.txt");
    imprimir_matriz_dot(grafo,"output/matriz.dot");

    liberar_grafo_matriz(&grafo);
    return 0;
}