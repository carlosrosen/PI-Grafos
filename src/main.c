#include <stdio.h>

#include "grafo_matriz.h"
#include "grafo_biconexo.h"

int main(void) {
    printf("Projeto Malha Metroviaria\n");

    GrafoMatriz* grafo = inicializar_grafo_matriz("dataset/stations.csv", "caminho/para/edges.csv");
    if(!grafo){
        printf("Falha ao inicializar o grafo\n");
        return 1;
    }

    // Relatório de vértices de articulação e componentes biconexos
    ResultadoBiconexo* resultado = encontrar_biconexos(grafo);
    imprimir_resultado_biconexo(grafo, resultado);
    liberar_resultado_biconexo(&resultado);

    liberar_grafo_matriz(&grafo);
    return 0;
}