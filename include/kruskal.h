#ifndef KRUSKAL_H
#define KRUSKAL_H

#include "grafo_expansao.h"

typedef struct ResultadoAGM {
    ArestaAGM arestas[AGM_MAX_VERTICES - 1];
    int qtd_vertices;
    int qtd_arestas_candidatas;
    int qtd_arestas;
    double custo_total_brl;
    double custo_total_gbp;
    int conectado;
} ResultadoAGM;

/*
 * Executa Kruskal sobre um vetor de arestas não direcionadas.
 * As arestas são ordenadas por custo em reais e, em caso de empate,
 * pelos códigos normalizados de origem e destino.
 *
 * Retorna 1 em caso de sucesso e 0 em caso de erro de entrada ou alocação.
 * Para grafos desconexos, resultado contém a floresta geradora mínima e
 * resultado->conectado recebe zero.
 */
int executar_kruskal_agm(const ArestaAGM *arestas, int qtd_vertices,
                         int qtd_arestas, ResultadoAGM *resultado);

#endif
