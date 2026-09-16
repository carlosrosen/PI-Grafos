#include "dataset.h"
#include "grafo_lista.h"
#include "string_functions.h"


#include<stdio.h>
#include<stdlib.h>
#include<string.h>


FILE* open_file_read_mode(char path[]);
No* get_station(No** stations, int stations_length, char *code);

/*
    Essa função irá pegar todas as estações da cidade especificada,
    da qual poderá ser acessada por como se fosse um array, sem estarem interligadas
*/
ResponseObjectLength* inicializa_stations(){
    printf("./dataset/stations.csv");
    FILE* stations = open_file_read_mode("./dataset/stations.csv");

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
        station->proximos = (Aresta**)calloc(station->qtd_alloc_aresta, sizeof(Aresta*));
        if(count_lines + 1 >= all_stations_length){
            all_stations_length *= 2;
            all_stations = realloc(all_stations,sizeof(No*) * all_stations_length);
            if(all_stations == NULL){
                perror("Falha ao realocar memória para all_stations.");
                exit(EXIT_FAILURE);
            }
        }
        all_stations[count_lines - 2] = station;
    }
    
    ResponseObjectLength *response = (ResponseObjectLength*)malloc(sizeof(ResponseObjectLength));

    response->object = (void**)all_stations;
    response->length = count_lines - 1;

    fclose(stations);
    return response;
}

// Conecta as estações as suas devidas arestas enquanto as inicializa em formato de array
ResponseObjectLength* Inicializar_edges(No** stations,int stations_length){
    FILE* edges = open_file_read_mode("./dataset/edges.csv");

    char buffer[BUFFER_SIZE];
    unsigned int count_lines = 0;
 
    unsigned int array_arestas_length = 1000;
    Aresta **array_arestas = (Aresta**)malloc(sizeof(Aresta*) * array_arestas_length);
    while(fgets(buffer, BUFFER_SIZE, edges)){
        count_lines++;
        if(count_lines == 1)continue;

        char source[4] = {'\0','\0','\0','\0'};
        char target[4] = {'\0','\0','\0','\0'};
        float distance = -1.0;
        
        sscanf(buffer,"%[^,],%[^,],%f\n",source,target, &distance);

        No *no_source = NULL, *no_target = NULL;
        Aresta *aresta = (Aresta*)malloc(sizeof(Aresta));
        if(aresta == NULL){perror("Erro ao alocar memória para {aresta} dos edges"); exit(EXIT_FAILURE);}
        
        if(no_source == NULL || strcmp(no_source->code, source)){
            no_source = get_station(stations, stations_length, source);
        }
        if(no_source == NULL){printf("\nFalha ao encontrar a estação de código: %s\n",source); exit(EXIT_FAILURE);}
        aresta->source = no_source;
        
        if(no_target == NULL || strcmp(no_target->code, target)){
            no_target = get_station(stations, stations_length, target);
        }
        if(no_target == NULL){printf("\nFalha ao encontrar a estação de codigo: %s\n",target); exit(EXIT_FAILURE);}
        aresta->target = no_target;

        aresta->distance = distance;

        if(count_lines + 1 >= array_arestas_length){
            array_arestas_length *= 2;
            array_arestas = (Aresta**)realloc(array_arestas,sizeof(Aresta*) * array_arestas_length);
            if(array_arestas == NULL){
                perror("Erro ao realocar mais memória para {array_arestas}");
                exit(EXIT_FAILURE);
            }
        }
        array_arestas[count_lines - 2] = aresta;
        _Insert_Aresta_No(no_source,aresta);
    }

    ResponseObjectLength *response = (ResponseObjectLength*)malloc(sizeof(ResponseObjectLength));

    response->object = (void**)array_arestas;
    response->length = count_lines -1;

    return response;
}

//Busca uma estação especifica a partir de seu code
No* get_station(No** stations, int stations_length, char *code){
    for(int idx_station = 0; idx_station < stations_length; idx_station++){
        if(!strcmp(stations[idx_station]->code,code)){
            return stations[idx_station];
        }
    }
    return NULL;
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