#include "aux_functions.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

char* converter_inteiro_string(int num){
    char *string = (char*)malloc(sizeof(char) * 10);
    int i;
    for(i = 0;num > 0; i++){
        int mod = num % 10;
        string[i] = mod + '0';
        num /= 10;
    }
    string[i] = '\0';
    int string_length = strlen(string);
    for(i = 0; i < string_length/2; i++){
        int aux = string[i];
        string[i] = string[string_length - i - 1];
        string[string_length - i - 1] = aux;
    }
    return string;
}

// abre o arquivo em modo leitura, caso não for possivel a leitura o programa será fechado.
FILE* open_file_read_mode(char path[]){
    FILE* fp = fopen(path, "r");
    if(fp == NULL){
        fprintf(stderr,"Erro ao ler o arquivo %s\n", path);
        exit(EXIT_FAILURE);
    }
    return fp;
}

//MEDICAO DO TEMPO DE EXECU��O

double obter_tempo(){

    return (double)clock() / CLOCKS_PER_SEC;
}

double medir_tempo_ms(void (*funcao)(void *), void *grafo){

    double inicio;
    double fim;

    inicio = obter_tempo();

    funcao(grafo);
    fim = obter_tempo(); //executa o algoritmo e mede o tempo de execucao

    return (fim - inicio) * 1000.0; //converte para milissegundos
}

//MEDICAO DE MEMORIA

size_t medir_memoria_lista(GrafoLista *grafo){

    if(grafo == NULL){
        return 0;
    }

    size_t memoria = 0;

    memoria += sizeof(GrafoLista); //memoria principal do grafo
    memoria += grafo->qtd_no * sizeof(No*); //memoria do vetor dos ponteiros para os nos
    memoria += grafo->qtd_no * sizeof(No); //memoria dos nos
    memoria += grafo->qtd_no * sizeof(DadosEstacao); //memoria dos dados das estacoes
    memoria += grafo->qtd_arestas * sizeof(Aresta*);//memoria do vetor dos ponteiros para as arestas
    memoria += grafo->qtd_arestas * sizeof(Aresta);//memoria das arestas
    memoria += grafo->qtd_arestas * sizeof(DadosAresta); //memoria dos dados das arestas
    memoria += grafo->qtd_arestas * sizeof(No);//memoria dos nos de origem e destino das arestas

    return memoria;
}

size_t medir_memoria_matriz(GrafoMatriz *grafo)
{
    size_t memoria = 0;

    memoria += sizeof(GrafoMatriz);

    memoria += grafo->qtd_estacoes * sizeof(int *);

    memoria +=
        grafo->qtd_estacoes *
        grafo->qtd_estacoes *
        sizeof(int);

    memoria +=
        grafo->qtd_estacoes *
        sizeof(DadosEstacao *);

    memoria +=
        grafo->qtd_estacoes *
        sizeof(DadosEstacao);

    memoria +=
        grafo->qtd_arestas *
        sizeof(DadosAresta *);

    memoria +=
        grafo->qtd_arestas *
        sizeof(DadosAresta);

    return memoria;
}


    void gerar_log(
    GrafoLista *lista,
    GrafoMatriz *matriz,
    const char *arquivo
){
    FILE *fp;

    size_t memoria_lista =
        medir_memoria_lista(lista);

    size_t memoria_matriz =
        medir_memoria_matriz(matriz);

        fp = fopen(arquivo, "w"); //abre o arquivo em modo escrita (Write), se o arquivo nao existir ele sera criado, se existir ele sera sobrescrito

    if(fp == NULL){

        printf("Erro ao criar arquivo de log.\n");

        return;
    }


    /* LISTA */

    printf("\n====================================\n");
    printf("      RESULTADOS DO BENCHMARK\n");
    printf("====================================\n");


    printf("\nLISTA DE ADJACENCIA\n");

    printf("Quantidade de vertices: %u\n",
        lista->qtd_no);

    printf("Quantidade de arestas: %u\n",
        lista->qtd_arestas);

    printf("Memoria estimada: %zu bytes\n",
        memoria_lista);


    /* MATRIZ */

    printf("\nMATRIZ DE ADJACENCIA\n");

    printf("Quantidade de vertices: %d\n",
        matriz->qtd_estacoes);

    printf("Quantidade de arestas: %d\n",
        matriz->qtd_arestas);

    printf("Memoria estimada: %zu bytes\n",
        memoria_matriz);


    /* ARQUIVO */

    fprintf(fp, "====================================\n");
    fprintf(fp, "      RESULTADOS DO BENCHMARK\n");
    fprintf(fp, "====================================\n");


    fprintf(fp, "\nLISTA DE ADJACENCIA\n");

    fprintf(fp,
            "Quantidade de vertices: %u\n",
            lista->qtd_no);

    fprintf(fp,
            "Quantidade de arestas: %u\n",
            lista->qtd_arestas);

    fprintf(fp,
            "Memoria estimada: %zu bytes\n",
            memoria_lista);


    fprintf(fp, "\nMATRIZ DE ADJACENCIA\n");

    fprintf(fp,
            "Quantidade de vertices: %d\n",
            matriz->qtd_estacoes);

    fprintf(fp,
            "Quantidade de arestas: %d\n",
            matriz->qtd_arestas);

    fprintf(fp,
            "Memoria estimada: %zu bytes\n",
            memoria_matriz);


    fclose(fp);
}


// void verificar_alocacao(void* alloc, char* var_name){
//     if(alloc == NULL){
//         if(!var_name){
//             perror("Não foi possivel alocar memória para uma variável\n");
//         }else{
//             fprintf(stderr,"Não foi possivel alocar memória para %s\n",var_name);
//         }
//         exit(EXIT_FAILURE);
//     }
// }


// int is_equal(char* s1, char* s2){
//     int count = 0;
//     while(s1[count] != '\0' || s2[count] != '\0'){
//         if(s1[count] != s2[count])return 0;
//     }
//     if(s1[count] == '\0' && s2 == '\0') return 1;
//     return 0;
// }