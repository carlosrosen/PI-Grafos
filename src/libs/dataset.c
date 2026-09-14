#include "dataset.h"
#include "grafo_lista.h"
#include "string_functions.h"


#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define PATH_DATASET_DIRECTORY "./dataset/"

FILE* open_file_read_mode(char path[]);

/*
    Essa função irá pegar todas as estações da cidade especificada,
    E poderá ser acessada por como se fosse um array.
*/
ResponseObjectLength* get_stations(){
    FILE* stations = open_file_read_mode(strcat(PATH_DATASET_DIRECTORY,"stations.csv"));

    char buffer[BUFFER_SIZE];
    int count_lines = 0;
    int all_stations_length = 1000;
    No** all_stations = (No**)malloc(sizeof(No*) * all_stations_length);
    while(fgets(buffer,BUFFER_SIZE,stations) != NULL){
        count_lines++;
        if(count_lines == 1)continue;
        double longitude, latitude;
        char *nome = (char*)calloc(50,sizeof(char));
        if(nome == NULL){perror("Erro ao alocar memória para {nome} da estação"); exit(EXIT_FAILURE);}
        char *code = (char*)calloc(4,sizeof(char));
        if(code == NULL){perror("Erro ao alocar memória para {code} da estação"); exit(EXIT_FAILURE);}
        sscanf(buffer,"\"%[^\"]\",\"%lf\",\"%lf\",\"%[^\"]\"\n",code,&latitude,&longitude,nome);
        
        No* station = (No*)malloc(sizeof(No));
        station->code = code;
        station->latitude = latitude;
        station->longitude = longitude;
        station->nome = nome;
        station->qtd_alloc_aresta = ARESTA_DEFAULT_INCREASE;
        station->proximos = (Aresta**)malloc(sizeof(Aresta*) * stations->qtd_alloc_aresta);
        if(count_lines + 1 >= all_stations_length){
            all_stations_length *= 2;
            all_stations = realloc(all_stations,sizeof(No*) * all_stations_length);
            if(all_stations == NULL){
                perror("Falha ao realocar memória para all_stations.");
                exit(EXIT_FAILURE);
            }
        }
        all_stations[count_lines-1] = station;
    }
    
    ResponseObjectLength *response = (ResponseObjectLength*)malloc(sizeof(ResponseObjectLength));

    response->object = all_stations;
    response->length = count_lines - 1;

    fclose(stations);
    return response;
}

// int get_quantity_stations(){
//     enum StationsCols{id,name,geometry,buildstart,opening,closure,c_id};
//     FILE* stations = open_file_read_mode(strcat(PATH_DATASET_DIRECTORY,"stations.csv"));
//     char buffer[BUFFER_SIZE];
    
//     int count = 0;
//     while(fgets(buffer,BUFFER_SIZE,stations) != NULL){
//         count++;
//     }
    
//     fclose(stations);
//     return count -1;
// }

// 
ResponseObjectLength* get_edges(){
    FILE* edges = open_file_read_mode(strcat(PATH_DATASET_DIRECTORY,"edges.csv"));

    char buffer[BUFFER_SIZE];
    int count_lines = 0;

    unsigned int array_arestas_length = 1000;
    Aresta **array_arestas = (Aresta*)malloc(sizeof(Aresta) * array_arestas_length);
    while(fgets(buffer, BUFFER_SIZE, edges)){
        count_lines++;
        if(count_lines == 1)continue;
        char* source = (char*)calloc(4,sizeof(char));
        if(source == NULL){perror("Erro ao alocar memória para {source} dos edges"); exit(EXIT_FAILURE);}
        char *target = (char*)calloc(4,sizeof(char));
        if(target == NULL){perror("Erro ao alocar memória para {target} dos edges"); exit(EXIT_FAILURE);}
        float distance = -1.0;
        
        sscan(buffer,"%[^,],%[^,],%f\n",source,target, distance);
        
        Aresta *aresta = (Aresta*)malloc(sizeof(Aresta));
        if(aresta == NULL){perror("Erro ao alocar memória para {aresta} dos edges"); exit(EXIT_FAILURE);}
        aresta->source = source;
        aresta->target = target;
        aresta->distance = distance;

        if(count_lines + 1 >= array_arestas_length){
            array_arestas_length *= 2;
            array_arestas = realloc(array_arestas,sizeof(Aresta*) * array_arestas_length);
            if(array_arestas == NULL){
                perror("Erro ao realocar mais memória para {array_arestas}");
                exit(EXIT_FAILURE);
            }
        }
        array_arestas[count_lines - 1] = aresta;
    }

    ResponseObjectLength *response = (ResponseObjectLength*)malloc(sizeof(ResponseObjectLength));

    response->object = array_arestas;
    response->length = count_lines -1;

    return ResponseObjectLength;
}

void connect_stations(
    No** stations,
    unsigned int stations_length,
    Aresta** edges,
    unsigned int arestas_length
){
    for(int idx_aresta = 0; idx_aresta < arestas_length; idx_aresta++){
        Aresta* aresta = edges[idx_aresta];
        No* source = NULL;
        No* target = NULL;
        for(int idx_stations = 0; idx_stations < stations_length; idx_stations++){
            if(source != NULL && target != NULL){
                break;
            }else if(!strcmp(aresta->source,stations[idx_stations]->code)){
                source = station[idx_stations];
            }else if(!strcmp(aresta->target,stations[idx_stations]->code)){
                target = stations[idx_stations];
            }
        }
        _Insert_Aresta(source, aresta);

    }
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