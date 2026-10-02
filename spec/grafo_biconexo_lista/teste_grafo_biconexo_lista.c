#include "grafo_lista.h"
#include "grafo_biconexo.h"
#include <stdio.h>

int main(){
    GrafoLista* grafo = inicializar_grafo_lista("./dataset/stations.csv", "./dataset/edges.csv");
    if(!grafo){
        printf("Falha ao inicializar o grafo\n");
        return 1;
    }

    ResultadoBiconexo* resultado = encontrar_biconexos_lista(grafo);

    exportar_componentes_biconexos_lista(grafo, resultado, "./output/biconexo_lista.txt");
    exportar_vertices_articulacao_lista(grafo, resultado, "./output/vertice_articulacao_lista.txt");

    printf("Componentes biconexos (lista): %d\n", resultado->qtd_componentes);
    printf("Vertices de articulacao (lista): %d\n", resultado->qtd_articulacoes);
    printf("Saída gerada em output/biconexo_lista.txt e output/vertice_articulacao_lista.txt\n");

    liberar_resultado_biconexo_lista(&resultado);
    Liberar_Grafo(grafo);
    return 0;
}