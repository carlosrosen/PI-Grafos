#ifndef KRUSKAL_H
#define KRUSKAL_H

#include "agm.h"

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
