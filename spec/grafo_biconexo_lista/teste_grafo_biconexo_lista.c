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
    imprimir_resultado_biconexo_lista(grafo, resultado);

    liberar_resultado_biconexo_lista(&resultado);
    Liberar_Grafo(grafo); // <--- Corrigido aqui (sem '&' e com o nome exato do .h)
    return 0;
}