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

    // Relatório de vértices de articulação e componentes biconexos
ResultadoBiconexo* resultado = encontrar_biconexos(grafo);

FILE* arquivo = fopen("resultado_biconexo.txt", "w");

if (!arquivo) {
    printf("Erro ao criar o arquivo de resultado.\n");

    liberar_resultado_biconexo(&resultado);
    liberar_grafo_matriz(&grafo);

    return 1;
}

imprimir_resultado_biconexo(grafo, resultado, arquivo);

fclose(arquivo);

printf("Resultado salvo em resultado_biconexo.txt\n");
    return 0;
}