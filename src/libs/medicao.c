#define _POSIX_C_SOURCE 199309L /* clock_gettime */

#include "medicao.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#endif

/* Tamanhos realmente alocados em grafo_lista.c / grafo_matriz.c */
#define TAM_CODE 4
#define TAM_NOME 50

/* ===============================================================
 * Cronômetro
 * =============================================================== */

double medicao_agora_ms(void){
#ifdef _WIN32
    LARGE_INTEGER freq, cont;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&cont);
    return (double)cont.QuadPart * 1000.0 / (double)freq.QuadPart;
#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec * 1000.0 + (double)ts.tv_nsec / 1e6;
#endif
}

/* ===============================================================
 * Tempo dos algoritmos
 * =============================================================== */

static MedicaoTempo nova_medicao(const char *nome, int repeticoes){
    MedicaoTempo m;
    memset(&m, 0, sizeof(m));
    strncpy(m.nome, nome, sizeof(m.nome) - 1);
    m.repeticoes = repeticoes < 1 ? 1 : repeticoes;
    m.tempo_min_ms = -1.0;
    return m;
}

/* Atualiza min/max/soma com o tempo de uma rodada */
static void registrar_rodada(MedicaoTempo *m, double ms, double *soma){
    *soma += ms;
    if(m->tempo_min_ms < 0 || ms < m->tempo_min_ms) m->tempo_min_ms = ms;
    if(ms > m->tempo_max_ms) m->tempo_max_ms = ms;
}

MedicaoTempo medir_pontes_matriz(GrafoMatriz *grafo, int repeticoes){
    MedicaoTempo m = nova_medicao("Pontes (matriz)", repeticoes);
    if(!grafo) return m;
    double soma = 0;

    for(int i = 0; i < m.repeticoes; i++){
        double ini = medicao_agora_ms();
        ListaPontes *r = encontrar_pontes_matriz(grafo);
        double fim = medicao_agora_ms();

        registrar_rodada(&m, fim - ini, &soma);
        if(r) m.qtd_resultados = r->quantidade;
        liberar_pontes(r);
    }
    m.tempo_medio_ms = soma / m.repeticoes;
    return m;
}

MedicaoTempo medir_pontes_lista(GrafoLista *grafo, int repeticoes){
    MedicaoTempo m = nova_medicao("Pontes (lista)", repeticoes);
    if(!grafo) return m;
    double soma = 0;

    for(int i = 0; i < m.repeticoes; i++){
        double ini = medicao_agora_ms();
        ListaPontes *r = encontrar_pontes_lista(grafo);
        double fim = medicao_agora_ms();

        registrar_rodada(&m, fim - ini, &soma);
        if(r) m.qtd_resultados = r->quantidade;
        liberar_pontes(r);
    }
    m.tempo_medio_ms = soma / m.repeticoes;
    return m;
}

MedicaoTempo medir_biconexos_matriz(GrafoMatriz *grafo, int repeticoes){
    MedicaoTempo m = nova_medicao("Biconexos (matriz)", repeticoes);
    if(!grafo) return m;
    double soma = 0;

    for(int i = 0; i < m.repeticoes; i++){
        double ini = medicao_agora_ms();
        ResultadoBiconexo *r = encontrar_biconexos(grafo);
        double fim = medicao_agora_ms();

        registrar_rodada(&m, fim - ini, &soma);
        if(r) m.qtd_resultados = r->qtd_componentes;
        liberar_resultado_biconexo(&r);
    }
    m.tempo_medio_ms = soma / m.repeticoes;
    return m;
}

MedicaoTempo medir_biconexos_lista(GrafoLista *grafo, int repeticoes){
    MedicaoTempo m = nova_medicao("Biconexos (lista)", repeticoes);
    if(!grafo) return m;
    double soma = 0;

    for(int i = 0; i < m.repeticoes; i++){
        double ini = medicao_agora_ms();
        ResultadoBiconexo *r = encontrar_biconexos_lista(grafo);
        double fim = medicao_agora_ms();

        registrar_rodada(&m, fim - ini, &soma);
        if(r) m.qtd_resultados = r->qtd_componentes;
        liberar_resultado_biconexo_lista(&r);
    }
    m.tempo_medio_ms = soma / m.repeticoes;
    return m;
}

/* ===============================================================
 * Memória
 * =============================================================== */

MedicaoMemoria estimar_memoria_matriz(GrafoMatriz *grafo){
    MedicaoMemoria m;
    memset(&m, 0, sizeof(m));
    strncpy(m.nome, "Matriz de adjacencia", sizeof(m.nome) - 1);
    if(!grafo) return m;

    size_t n = (size_t)grafo->qtd_estacoes;

    /* struct do grafo + vetor de ponteiros de linha + N*N inteiros */
    m.bytes_estrutura = sizeof(GrafoMatriz)
                      + n * sizeof(int *)
                      + n * n * sizeof(int);

    /* vetor de ponteiros + DadosEstacao + code + nome, por estação */
    m.bytes_estacoes = n * (sizeof(DadosEstacao *) + sizeof(DadosEstacao)
                            + TAM_CODE + TAM_NOME);

    /* vetor de ponteiros + DadosAresta por aresta dirigida */
    m.bytes_arestas = (size_t)grafo->qtd_arestas
                    * (sizeof(DadosAresta *) + sizeof(DadosAresta));

    m.bytes_total = m.bytes_estrutura + m.bytes_estacoes + m.bytes_arestas;
    return m;
}

MedicaoMemoria estimar_memoria_lista(GrafoLista *grafo){
    MedicaoMemoria m;
    memset(&m, 0, sizeof(m));
    strncpy(m.nome, "Lista de adjacencia", sizeof(m.nome) - 1);
    if(!grafo) return m;

    size_t n = (size_t)grafo->qtd_no;

    /* Conta os nós realmente alocados: a cabeça de cada lista + cada vizinho */
    size_t total_nos = 0;
    for(size_t i = 0; i < n; i++)
        for(No *no = grafo->lista[i]; no != NULL; no = no->proximo)
            total_nos++;

    /* struct do grafo + vetor de cabeças + todos os nós encadeados */
    m.bytes_estrutura = sizeof(GrafoLista)
                      + n * sizeof(No *)
                      + total_nos * sizeof(No);

    /* DadosEstacao + code + nome (ficam só na cabeça de cada lista) */
    m.bytes_estacoes = n * (sizeof(DadosEstacao) + TAM_CODE + TAM_NOME);

    /* vetor de ponteiros + Aresta + DadosAresta por aresta dirigida */
    m.bytes_arestas = (size_t)grafo->qtd_arestas
                    * (sizeof(Aresta *) + sizeof(Aresta) + sizeof(DadosAresta));

    m.bytes_total = m.bytes_estrutura + m.bytes_estacoes + m.bytes_arestas;
    return m;
}

/* ===============================================================
 * Log
 * =============================================================== */

static void escrever_memoria(FILE *f, const MedicaoMemoria *m){
    fprintf(f, "%s\n", m->nome);
    fprintf(f, "  Estrutura de adjacencia : %10zu bytes\n", m->bytes_estrutura);
    fprintf(f, "  Dados das estacoes      : %10zu bytes\n", m->bytes_estacoes);
    fprintf(f, "  Dados das arestas       : %10zu bytes\n", m->bytes_arestas);
    fprintf(f, "  TOTAL                   : %10zu bytes (%.2f KB)\n\n",
            m->bytes_total, (double)m->bytes_total / 1024.0);
}

void escrever_log_medicoes(FILE *saida,
                           const MedicaoTempo *tempos, int qtd_tempos,
                           const MedicaoMemoria *mem_matriz,
                           const MedicaoMemoria *mem_lista){
    if(!saida) return;

    if(tempos && qtd_tempos > 0){
        fprintf(saida, "--- Tempo de execucao (ms) ---\n");
        fprintf(saida, "%-22s %6s %12s %12s %12s %10s\n",
                "Algoritmo", "Rodadas", "Medio", "Minimo", "Maximo", "Resultados");
        for(int i = 0; i < qtd_tempos; i++){
            fprintf(saida, "%-22s %6d %12.4f %12.4f %12.4f %10d\n",
                    tempos[i].nome, tempos[i].repeticoes,
                    tempos[i].tempo_medio_ms, tempos[i].tempo_min_ms,
                    tempos[i].tempo_max_ms, tempos[i].qtd_resultados);
        }
        fprintf(saida, "\n");
    }

    if(mem_matriz || mem_lista){
        fprintf(saida, "--- Memoria estimada das estruturas ---\n");
        if(mem_matriz) escrever_memoria(saida, mem_matriz);
        if(mem_lista)  escrever_memoria(saida, mem_lista);

        if(mem_matriz && mem_lista && mem_lista->bytes_total > 0){
            fprintf(saida, "Razao matriz/lista: %.2fx\n\n",
                    (double)mem_matriz->bytes_total / (double)mem_lista->bytes_total);
        }
    }
}

void gerar_log_medicoes(const char *caminho,
                        const MedicaoTempo *tempos, int qtd_tempos,
                        const MedicaoMemoria *mem_matriz,
                        const MedicaoMemoria *mem_lista){
    char data[64] = "";
    time_t agora = time(NULL);
    strftime(data, sizeof(data), "%Y-%m-%d %H:%M:%S", localtime(&agora));

    /* Console */
    printf("\n========= Medicoes de desempenho (%s) =========\n\n", data);
    escrever_log_medicoes(stdout, tempos, qtd_tempos, mem_matriz, mem_lista);

    /* Arquivo (acrescenta, mantendo o histórico das execuções) */
    if(!caminho) return;

    FILE *fp = fopen(caminho, "a");
    if(!fp){
        perror("Falha ao abrir arquivo de log das medicoes");
        return;
    }
    fprintf(fp, "========= Medicoes de desempenho (%s) =========\n\n", data);
    escrever_log_medicoes(fp, tempos, qtd_tempos, mem_matriz, mem_lista);
    fclose(fp);
}
