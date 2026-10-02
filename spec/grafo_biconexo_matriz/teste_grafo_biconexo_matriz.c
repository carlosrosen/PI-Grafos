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

    exportar_componentes_biconexos_matriz(grafo, resultado, "./output/biconexo_matriz.txt");
    exportar_vertices_articulacao_matriz(grafo, resultado, "./output/vertice_articulacao_matriz.txt");

    printf("Componentes biconexos (matriz): %d\n", resultado->qtd_componentes);
    printf("Vertices de articulacao (matriz): %d\n", resultado->qtd_articulacoes);
    printf("Saída gerada em output/biconexo_matriz.txt e output/vertice_articulacao_matriz.txt\n");

    liberar_resultado_biconexo(&resultado);
    liberar_grafo_matriz(&grafo);
    return 0;
}