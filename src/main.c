#include <stdio.h>

#include "grafo_matriz.h"
#include "grafo_biconexo.h"
#include "grafo_lista.h"
#include "pontes.h"
#include "medicao.h"

#define REPETICOES 100

int main(void) {
    printf("Projeto Malha Metroviaria\n");

    GrafoMatriz* grafo = inicializar_grafo_matriz("dataset/stations.csv", "dataset/edges.csv");
    GrafoLista* grafo_lista = inicializar_grafo_lista("dataset/stations.csv", "dataset/edges.csv");
    
    if(!grafo){
        printf("Falha ao inicializar o grafo\n");
        return 1;
    }   

    ResultadoBiconexo* resultado = encontrar_biconexos(grafo);

    exportar_componentes_biconexos_matriz(grafo, resultado, "output/biconexo_matriz.txt");
    exportar_vertices_articulacao_matriz(grafo, resultado, "output/vertice_articulacao_matriz.txt");

    printf("Resultado salvo em output/biconexo_matriz.txt\n");
    printf("Resultado salvo em output/vertice_articulacao_matriz.txt\n");

        MedicaoTempo tempos[4];
    tempos[0] = medir_pontes_matriz(grafo, 100);
    tempos[1] = medir_pontes_lista(grafo_lista, 100);
    tempos[2] = medir_biconexos_matriz(grafo, 100);
    tempos[3] = medir_biconexos_lista(grafo_lista, 100);
    MedicaoMemoria mem_matriz = estimar_memoria_matriz(grafo);
    MedicaoMemoria mem_lista = estimar_memoria_lista(grafo_lista);
    gerar_log_medicoes("output/medicoes.txt", tempos, 4, &mem_matriz, &mem_lista);

    liberar_resultado_biconexo(&resultado);
    liberar_grafo_matriz(&grafo);
    Liberar_Grafo(grafo_lista);
    return 0;
}