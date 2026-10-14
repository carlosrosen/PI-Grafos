#ifndef MEDICAO_H
#define MEDICAO_H

#include <stddef.h>
#include <stdio.h>

#include "grafo_lista.h"
#include "grafo_matriz.h"
#include "pontes.h"
#include "grafo_biconexo.h"

/* ---------------------------------------------------------------
 * Cronômetro (milissegundos)
 * --------------------------------------------------------------- */

/* Retorna um instante de tempo em ms (relógio monotônico). Só faz sentido
   subtrair dois valores obtidos por esta função. */
double medicao_agora_ms(void);

/* ---------------------------------------------------------------
 * Medição de tempo dos algoritmos
 * --------------------------------------------------------------- */

typedef struct MedicaoTempo {
    char   nome[64];        /* ex.: "Pontes (matriz)"                       */
    int    repeticoes;      /* quantas vezes o algoritmo foi executado      */
    double tempo_medio_ms;  /* média por execução                           */
    double tempo_min_ms;    /* melhor execução                              */
    double tempo_max_ms;    /* pior execução                                */
    int    qtd_resultados;  /* nº de pontes ou de componentes biconexos     */
} MedicaoTempo;

/* Cada função executa o algoritmo `repeticoes` vezes (mínimo 1), libera o
   resultado a cada rodada e devolve as estatísticas de tempo. */
MedicaoTempo medir_pontes_matriz(GrafoMatriz *grafo, int repeticoes);
MedicaoTempo medir_pontes_lista(GrafoLista *grafo, int repeticoes);
MedicaoTempo medir_biconexos_matriz(GrafoMatriz *grafo, int repeticoes);
MedicaoTempo medir_biconexos_lista(GrafoLista *grafo, int repeticoes);

/* ---------------------------------------------------------------
 * Estimativa de memória das estruturas
 * --------------------------------------------------------------- */

typedef struct MedicaoMemoria {
    char   nome[64];            /* ex.: "Matriz de adjacencia"              */
    size_t bytes_estrutura;     /* matriz NxN ou nós da lista encadeada     */
    size_t bytes_estacoes;      /* DadosEstacao + strings code/nome         */
    size_t bytes_arestas;       /* DadosAresta (e Aresta, na lista)         */
    size_t bytes_total;
} MedicaoMemoria;

/* Estimativa baseada em sizeof das structs e nos tamanhos reais alocados
   no código (code = 4 bytes, nome = 50 bytes). Não inclui o overhead interno
   do malloc, então o valor real no sistema é um pouco maior. */
MedicaoMemoria estimar_memoria_matriz(GrafoMatriz *grafo);
MedicaoMemoria estimar_memoria_lista(GrafoLista *grafo);

/* ---------------------------------------------------------------
 * Log
 * --------------------------------------------------------------- */

/* Escreve o relatório em `saida` (stdout ou um FILE*). Qualquer ponteiro
   pode ser NULL / qtd_tempos = 0 para pular aquela seção. */
void escrever_log_medicoes(FILE *saida,
                           const MedicaoTempo *tempos, int qtd_tempos,
                           const MedicaoMemoria *mem_matriz,
                           const MedicaoMemoria *mem_lista);

/* Imprime no console E acrescenta (modo "a") ao arquivo `caminho`, com
   data/hora da execução. Se caminho for NULL, só imprime no console. */
void gerar_log_medicoes(const char *caminho,
                        const MedicaoTempo *tempos, int qtd_tempos,
                        const MedicaoMemoria *mem_matriz,
                        const MedicaoMemoria *mem_lista);

#endif
