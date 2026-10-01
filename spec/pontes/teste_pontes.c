#include "grafo_lista.h"
#include "grafo_matriz.h"
#include "grafo_biconexo_lista.h"
#include "pontes.h"
#include <stdio.h>

static int contar_componentes_de_uma_aresta(ResultadoBiconexo *resultado) {
    int total = 0;
    for (int c = 0; c < resultado->qtd_componentes; c++) {
        if (resultado->componentes[c].qtd_arestas == 1) total++;
    }
    return total;
}

int main() {
    GrafoLista *grafo_lista = inicializar_grafo_lista("./dataset/stations.csv", "./dataset/edges.csv");
    ListaPontes *pontes_lista = encontrar_pontes_lista(grafo_lista);
    imprimir_pontes_lista(grafo_lista, pontes_lista);
    printf("Total de pontes (lista): %d\n", pontes_lista->quantidade);

    GrafoMatriz *grafo_matriz = inicializar_grafo_matriz("./dataset/stations.csv", "./dataset/edges.csv");
    ListaPontes *pontes_matriz = encontrar_pontes_matriz(grafo_matriz);
    imprimir_pontes_matriz(grafo_matriz, pontes_matriz);
    printf("Total de pontes (matriz): %d\n", pontes_matriz->quantidade);

    ResultadoBiconexo *biconexo = encontrar_biconexos_lista(grafo_lista);
    printf("\nComponentes biconexos de 1 aresta (esperado == total de pontes): %d\n",
           contar_componentes_de_uma_aresta(biconexo));

    liberar_resultado_biconexo_lista(&biconexo);
    liberar_pontes(pontes_lista);
    liberar_pontes(pontes_matriz);
    Liberar_Grafo(grafo_lista);
    liberar_grafo_matriz(&grafo_matriz);

    return 0;
}