#include <stdio.h>

#include "grafo_matriz.h"
#include "grafo_biconexo.h"

int main(void) {
    printf("Projeto Malha Metroviaria\n");

    GrafoMatriz* grafo = inicializar_grafo_matriz("dataset/stations.csv", "dataset/edges.csv");
    
    if(!grafo){
        printf("Falha ao inicializar o grafo\n");
        return 1;
    }   

    ResultadoBiconexo* resultado = encontrar_biconexos(grafo);

    exportar_componentes_biconexos_matriz(grafo, resultado, "output/biconexo_matriz.txt");
    exportar_vertices_articulacao_matriz(grafo, resultado, "output/vertice_articulacao_matriz.txt");

    printf("Resultado salvo em output/biconexo_matriz.txt\n");
    printf("Resultado salvo em output/vertice_articulacao_matriz.txt\n");

    liberar_resultado_biconexo(&resultado);
    liberar_grafo_matriz(&grafo);
    return 0;
}