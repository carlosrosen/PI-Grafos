#include "../../include/grafo_matriz.h"
#include "../../include/aux_functions.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define BUFFER_SIZE 4096

typedef struct InitializeMatrizResponse{
    int qtd_estacoes;
    int qtd_arestas;
    DadosAresta **dados_aresta;
    DadosEstacao **dados_estacao;
}InitMatrizResponse;



InitMatrizResponse* inicializar_matriz(char* path_stations, char* path_edges);

GrafoMatriz* inicializar_grafo_matriz(char* path_stations, char* path_edges){
    GrafoMatriz* grafo = (GrafoMatriz*)malloc(sizeof(GrafoMatriz));
    if(grafo == NULL){
        perror("Erro ao alocar memória para o grafo matriz");
        exit(EXIT_FAILURE);
    }
    
    InitMatrizResponse* response = inicializar_matriz(path_stations,path_edges);
    int capacidade = response->qtd_estacoes;
    grafo->dados_arestas = response->dados_aresta;
    grafo->dados_estacao = response->dados_estacao;
    grafo->qtd_arestas = response->qtd_arestas;
    free(response);response = NULL;

    grafo->qtd_estacoes = capacidade;
    grafo->matriz = (int**)calloc(capacidade,sizeof(int*));
    if(grafo->matriz == NULL){
        free(grafo);
        perror("Erro ao alocar memória para a lista do grafo matriz");
        exit(EXIT_FAILURE);
    }
    for(int row = 0; row < capacidade; row++){
        grafo->matriz[row] = calloc(capacidade, sizeof(int));
        
        if(grafo->matriz[row] == NULL){
            for(int i = 0; i < row; i ++) free(grafo->matriz[i]);
            free(grafo->matriz);
            free(grafo);
            exit(EXIT_FAILURE);
        }
    }
    return grafo;
}


void exibir_matriz(GrafoMatriz* grafo);
void imprimir_matriz(GrafoMatriz* grafo);
void liberar_grafo_matriz(GrafoMatriz** grafo);


int get_station_matriz(DadosEstacao** dados, int tamanho, char* search){
    for(int i = 0; i< tamanho; i++)
        if(!strcmp(dados[i]->code, search)) return i;
    return -1;
}

// lê os arquivos e retorna o tamanho das estações criar o grafo matriz
InitMatrizResponse* inicializar_matriz(char* path_stations, char* path_edges){
    if(path_stations == NULL || strlen(path_stations) == 0){
        printf("Grafo Matriz não foi inicializado pois o caminho não foi identificado\n");
        return NULL;
    }
    FILE* stations = open_file_read_mode(path_stations);
    
    size_t array_dados_length = 1000;
    InitMatrizResponse* response = (InitMatrizResponse*)calloc(1,sizeof(InitMatrizResponse));
    DadosEstacao** array_dados_estacao = (DadosEstacao**)calloc(array_dados_length, sizeof(DadosEstacao*));
    
    
    char *buffer = (char*)calloc(BUFFER_SIZE,sizeof(char));
    size_t count_lines = 0;
    
    
    // Realiza a leitura das estações e coloca os dados em array_dados_estacao para ser acessado como vetor;
    
    while(fgets(buffer,BUFFER_SIZE,stations) != NULL){
        count_lines++;
        if(count_lines == 1)continue;
        
        double latitude = -1, longitude = -1;
        char *nome = (char*)calloc(50, sizeof(char));
        if(nome == NULL){perror("Falha ao alocar memoria para \'nome\'"); exit(EXIT_FAILURE);}
        char *code = (char*)calloc(4,sizeof(char));
        if(code == NULL){perror("Falha ao alocar memoria para \'code\'"); exit(EXIT_FAILURE);}
        double tempo_transferencia = -1;
        
        sscanf(buffer,"\"%[^\"]\",\"%lf\",\"%lf\",\"%[^\"]\",\"%lf\"\n",code,&latitude,&longitude,nome,&tempo_transferencia);
        
        DadosEstacao* dados = (DadosEstacao*)malloc(sizeof(DadosEstacao));
        
        dados->id = count_lines;
        dados->code = code;
        dados->latitude = latitude;
        dados->longitude = longitude;
        dados->nome = nome;
        dados->tempo_transferencia = tempo_transferencia;
        
        if(count_lines >= array_dados_length -1){
            array_dados_length *= 2;
            array_dados_estacao = (DadosEstacao**)realloc(array_dados_estacao, array_dados_length);
            if(!array_dados_estacao){
                perror("não foi possivel realocar a memoria do array_dados_estacao");
                fclose(stations);
                for(size_t i = 0; i < count_lines; i ++){free(array_dados_estacao[i]->code);free(array_dados_estacao[i]->nome); free(array_dados_estacao[i]);}
                free(array_dados_estacao);free(dados);
                exit(EXIT_FAILURE);
            }
        }
        array_dados_estacao[count_lines -1] = dados;
    }
    fclose(stations);
    response->qtd_estacoes = count_lines-1;


    count_lines = 0;
    FILE* edges = open_file_read_mode(path_edges);

    size_t array_dados_aresta_length = 1000;    
    DadosAresta **array_dados_aresta = (DadosAresta**)calloc(array_dados_aresta_length,sizeof(DadosAresta*));

    while(fgets(buffer,BUFFER_SIZE,edges) != NULL){
        count_lines++;
        if(count_lines == 1)continue;

        char *source = (char*)calloc(4,sizeof(char));
        char *target = (char*)calloc(4,sizeof(char));
        double distance = -1.0;
        double travel_time = -1.0;
        
        sscanf(buffer,"%[^,],%[^,],%lf,%lf\n",source,target, &distance, &travel_time);

        DadosEstacao *dados_source = NULL, *dados_target = NULL;
        DadosAresta *dados = (DadosAresta*)calloc(1,sizeof(DadosAresta));
        if(dados == NULL){perror("Erro ao alocar memória para {dados} dos edges"); exit(EXIT_FAILURE);}
        
        if(dados_source == NULL || strcmp(dados_source->code, source)){
            int idx = get_station_matriz(array_dados_estacao, response->qtd_estacoes, source);
            if(idx == -1){printf("\nFalha ao encontrar a estação de código: %s\n",source); exit(EXIT_FAILURE);}
            dados_source = array_dados_estacao[idx];
        }
        dados->source = dados_source->code;
        
        if(dados_target == NULL || strcmp(dados_target->code, target)){
            int idx = get_station_matriz(array_dados_estacao, response->qtd_estacoes, target);
            if(idx == -1){printf("\nFalha ao encontrar a estação de codigo: %s\n",target); exit(EXIT_FAILURE);}
            dados_target = array_dados_estacao[idx];
        }
        dados->target = dados_target->code;

        dados->distancia = distance;
        dados->tempo_viagem = travel_time;

        if(count_lines >= array_dados_aresta_length -1){
            array_dados_aresta_length *= 2;
            array_dados_aresta = (DadosAresta**)realloc(array_dados_aresta,sizeof(DadosAresta*) * array_dados_aresta_length);
            if(array_dados_aresta == NULL){
                perror("Erro ao realocar mais memória para {array_arestas}");
                exit(EXIT_FAILURE);
            }
        }
        array_dados_aresta[count_lines - 1] = dados;
    }
    fclose(edges);

    response->dados_aresta = array_dados_aresta;
    response->qtd_arestas = count_lines -1;
    return response;
}