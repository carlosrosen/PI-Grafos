#include "grafo_lista.h"
#include "grafo_matriz.h"
#include "dijkstra.h"

#include <stdio.h>
#include <string.h>


// Para teste 

static const char *origem = "AAP";
static const char *destino = "ABD";

static int indice_estacao_lista(GrafoLista *grafo, const char *codigo){
    for (unsigned int i = 0; i < grafo->qtd_no; i++){
        if (strcmp(grafo->lista[i]->dados->code, codigo) == 0) return (int)i;
    }
    return -1;
}

static int indice_estacao_matriz(GrafoMatriz *grafo, const char *codigo){
    for (int i = 0; i < grafo->qtd_estacoes; i++){
        if (strcmp(grafo->dados_estacao[i]->code, codigo) == 0) return i;
    }
    return -1;
}

int main(){
    printf("===================================  TESTE DIJKSTRA  ===================================\n\n");

    GrafoLista *grafo_lista = inicializar_grafo_lista("./dataset/stations.csv", "./dataset/edges.csv");
    GrafoMatriz *grafo_matriz = inicializar_grafo_matriz("./dataset/stations.csv", "./dataset/edges.csv");

    if (grafo_lista == NULL || grafo_matriz == NULL){
        printf("Falha ao inicializar os grafos\n");
        return 1;
    }

    int origem_lista = indice_estacao_lista(grafo_lista, origem);
    int destino_lista = indice_estacao_lista(grafo_lista, destino);
    int origem_matriz = indice_estacao_matriz(grafo_matriz, origem);
    int destino_matriz = indice_estacao_matriz(grafo_matriz, destino);

    printf("Rota de %s ate %s\n", origem, destino);

    ResultadoDijkstra *resultado_lista = dijkstra_lista(grafo_lista, origem_lista, destino_lista);
    ResultadoDijkstra *resultado_matriz = dijkstra_matriz(grafo_matriz, origem_matriz, destino_matriz);

    imprimir_resultado_dijkstra_lista(grafo_lista, resultado_lista);
    imprimir_resultado_dijkstra_matriz(grafo_matriz, resultado_matriz);

    liberar_resultado_dijkstra(&resultado_lista);
    liberar_resultado_dijkstra(&resultado_matriz);

    Liberar_Grafo(grafo_lista);
    liberar_grafo_matriz(&grafo_matriz);

    puts("");
    return 0;
}
