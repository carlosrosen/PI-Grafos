#include "grafo_matriz.h"
#include "grafo_biconexo.h"
#include <stdio.h>

int main(){
    GrafoMatriz* grafo = inicializar_grafo_matriz("./dataset/stations.csv", "./dataset/edges.csv");
    if(!grafo){
        printf("Falha ao inicializar o grafo\n");
        return 1;
    }

    ResultadoBiconexo* resultado = encontrar_biconexos(grafo);
    imprimir_resultado_biconexo(grafo, resultado);

    liberar_resultado_biconexo(&resultado);
    liberar_grafo_matriz(&grafo);
    return 0;
}