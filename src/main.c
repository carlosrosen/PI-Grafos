#include <stdio.h>
#include "grafo_lista.h"
#include "grafo_matriz.h"
#include "aux_functions.h"

int main(void) {
    printf("Projeto Malha Metroviaria\n");

    GrafoLista *lista =
        inicializar_grafo_lista(); //cria o grafo de lista

        GrafoMatriz *matriz =
        inicializar_grafo_matriz(
            "./dataset/stations.csv",
            "./dataset/edges.csv"
        ); //cria o grafo de matriz

        gerar_log(
        lista,
        matriz,
        "output/benchmark.txt"
    );//gera o arquivo de log com os resultados do benchmark

    Liberar_Grafo(lista);

    liberar_grafo_matriz(&matriz); //libera a memoria alocada para o grafo de matriz

    return 0;
}