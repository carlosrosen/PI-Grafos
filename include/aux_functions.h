#include<stdio.h>
#include "grafo_lista.h"
#include "grafo_matriz.h"

#ifndef AUX_FUNCTIONS_H
#define AUX_FUNCTIONS_H


char* converter_inteiro_string(int num);
FILE* open_file_read_mode(char path[]);
// void verificar_alocacao(void* alloc, char *var_name);

//FUNCOES PARA MEDIR O TEMPO

double obter_tempo(); //tempo atual em segundos
double medir_tempo_ms(void (*funcao)(void *), void *grafo); //mede o tempo de execução de uma função em milissegundos


size_t medir_memoria_lista(GrafoLista *grafo); //estimativa de memoria usada
size_t medir_memoria_matriz(GrafoMatriz *grafo); //estimativa de memoria usada

void gerar_log(
    GrafoLista *lista,
    GrafoMatriz *matriz,
    const char *arquivo
);  //cria um arquivo de log com os resultados





#endif