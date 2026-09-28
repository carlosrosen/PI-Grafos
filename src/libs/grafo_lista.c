#include "grafo_lista.h"
#include "aux_functions.h"

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define BUFFER_SIZE 4096


typedef struct response_object_length{
    void **object;
    unsigned int length;
} ResponseObjectLength;

void Liberar_Nos(No** nos, unsigned int no_length);
ResponseObjectLength* inicializa_stations();
ResponseObjectLength* Inicializar_edges(No** stations,int stations_length);
No* get_station(No** stations, int stations_length, char *code);



GrafoLista* inicializar_grafo_lista(){
    GrafoLista* grafo = (GrafoLista *)malloc(sizeof(GrafoLista));
    if(grafo == NULL){
        fprintf(stderr, "Falha ao alocar memória para o grafo\n"); //Direciona a mensagem para a stream de erro.
        exit(EXIT_FAILURE);
    }
    
    ResponseObjectLength *response = inicializa_stations();
    No **stations = (No**)response->object;
    unsigned int stations_length = response->length;
    free(response);
    
    response = Inicializar_edges(stations,stations_length);
    
    Aresta **array_arestas = (Aresta**)response->object;
    unsigned int arestas_length = response->length;
    free(response);
    
    grafo->lista = stations;
    grafo->arestas = array_arestas;
    grafo->qtd_no = stations_length;
    grafo->qtd_arestas = arestas_length;
    
    return grafo;
}



void exibir_lista(GrafoLista* grafo){
    if(!grafo)return;
    puts("");
    for(unsigned int i = 0; i < grafo->qtd_no; i++){
        No* no = grafo->lista[i];
        printf("code: %s, lat: %f, long: %f, nome: %s\n",no->dados->code, no->dados->latitude, no->dados->longitude, no->dados->nome);
        while(no != NULL){
            if(no->proximo != NULL)printf("[ %d | %s ] -> ",no->vertice, no->dados->code);
            else printf("[ %d | %s ]\n",no->vertice, no->dados->code);
            no = no->proximo;
        }
    }
    puts("");
    // printf("\tsource: %s, target: %s, distance: %f\n",no->proximos[j]->source->code,no->proximos[j]->target->code,no->proximos[j]->distance);
}

void imprimir_lista(GrafoLista *grafo,char* output_path){
    if(!grafo)return;
    FILE* fp = fopen(output_path, "w");
    if(!fp){
        fprintf(stderr, "Erro ao abrir o arquivo %s", output_path);
        return;
    }
    for(unsigned int i = 0; i < grafo->qtd_no; i++){
        No* no = grafo->lista[i];
        printf("code: %s, lat: %f, long: %f, nome: %s\n",no->dados->code, no->dados->latitude, no->dados->longitude, no->dados->nome);
        while(no != NULL){
            if(no->proximo != NULL)printf("[ %d | %s ] -> ",no->vertice, no->dados->code);
            else printf("[ %d | %s ]\n",no->vertice, no->dados->code);
            no = no->proximo;
        }
    }
    fclose(fp);
}


void imprimir_lista_dot(GrafoLista *grafo,char* output_path){
    if(!grafo)return;
    FILE* fp = fopen(output_path, "w");
    if(!fp) return;
    fprintf(fp, "digraph G1{\n");
    for(unsigned int i = 0; i < grafo->qtd_no; i++){
        int vertice = grafo->lista[i]->vertice;
        fprintf(fp, "\t%d [shape=\"circle\"]\n",vertice);
    }
    fprintf(fp, "\n");
    for(unsigned int i = 0; i < grafo->qtd_arestas; i++){
        int vertice_source = grafo->arestas[i]->source->vertice;
        int vertice_target = grafo->arestas[i]->source->vertice;
        fprintf(fp,"\t%d -> %d\n", vertice_source, vertice_target);
    }
    fprintf(fp,"\n}");
    fclose(fp);
}


void Liberar_Nos(No** nos, unsigned int no_length){
    if(nos == NULL)return;
    for(unsigned int i = 0; i < no_length; i++){
        free(nos[i]->dados->nome);
        free(nos[i]->dados->code);
        free(nos[i]->dados);
        free(nos[i]);
    }
    free(nos);
}

void Liberar_Arestas(Aresta** arestas, unsigned int qtd_arestas){
    if(arestas == NULL)return;
    for(unsigned int i = 0; i < qtd_arestas; i++){
        free(arestas[i]);
    }
    free(arestas);
}

void Liberar_Grafo(GrafoLista *grafo){
    if(grafo == NULL)return;
    Liberar_Nos(grafo->lista, grafo->qtd_no);
    Liberar_Arestas(grafo->arestas, grafo->qtd_arestas);
    free(grafo);
}






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
        double longitude, latitude, transfer_time;
        char *nome = (char*)calloc(50,sizeof(char));
        if(nome == NULL){perror("Erro ao alocar memória para {nome} da estação"); exit(EXIT_FAILURE);}
        char *code = (char*)calloc(4,sizeof(char));
        if(code == NULL){perror("Erro ao alocar memória para {code} da estação"); exit(EXIT_FAILURE);}
        sscanf(buffer,"%[^,],%lf,%lf,%[^,],%lf\n",code,&latitude,&longitude,nome, &transfer_time);

        No* station = (No*)malloc(sizeof(No));
        DadosEstacao *dados = (DadosEstacao*)malloc(sizeof(DadosEstacao));
        dados->code = code;
        dados->latitude = latitude;
        dados->longitude = longitude;
        dados->nome = nome;
        dados->tempo_transferencia = transfer_time;
        dados->id = count_lines -1;

        station->vertice = count_lines - 1;
        station->dados = dados;
        station->proximo = NULL;
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
        double distance = -1.0;
        double travel_time = -1.0;
        
        sscanf(buffer,"%[^,],%[^,],%lf,%lf\n",source,target, &distance, &travel_time);

        No *no_source = NULL, *no_target = NULL;
        Aresta *aresta = (Aresta*)malloc(sizeof(Aresta));
        if(aresta == NULL){perror("Erro ao alocar memória para {aresta} dos edges"); exit(EXIT_FAILURE);}

        DadosAresta* dados = (DadosAresta*)calloc(1,sizeof(DadosAresta));
        
        if(no_source == NULL || strcmp(no_source->dados->code, source)){
            no_source = get_station(stations, stations_length, source);
        }
        if(no_source == NULL){printf("\nFalha ao encontrar a estação de código: %s\n",source); exit(EXIT_FAILURE);}
        aresta->source = no_source;
        
        if(no_target == NULL || strcmp(no_target->dados->code, target)){
            no_target = get_station(stations, stations_length, target);
        }
        if(no_target == NULL){printf("\nFalha ao encontrar a estação de codigo: %s\n",target); exit(EXIT_FAILURE);}
        aresta->target = no_target;

        dados->distancia = distance;
        dados->tempo_viagem = travel_time;
        aresta->dados = dados;

        if(count_lines + 1 >= array_arestas_length){
            array_arestas_length *= 2;
            array_arestas = (Aresta**)realloc(array_arestas,sizeof(Aresta*) * array_arestas_length);
            if(array_arestas == NULL){
                perror("Erro ao realocar mais memória para {array_arestas}");
                exit(EXIT_FAILURE);
            }
        }
        array_arestas[count_lines - 2] = aresta;

        No* new_node = (No*)malloc(sizeof(No));
        new_node->dados = no_target->dados;
        new_node->vertice = no_target->vertice;
        new_node->proximo = NULL;
        No* aux = stations[no_source->vertice - 1];
        while(aux->proximo != NULL){
            aux = aux->proximo;
        }
        aux->proximo = new_node;
    }

    ResponseObjectLength *response = (ResponseObjectLength*)malloc(sizeof(ResponseObjectLength));

    response->object = (void**)array_arestas;
    response->length = count_lines -1;

    return response;
}

//Busca uma estação especifica a partir de seu code
No* get_station(No** stations, int stations_length, char *code){
    for(int idx_station = 0; idx_station < stations_length; idx_station++){
        if(!strcmp(stations[idx_station]->dados->code,code)){
            return stations[idx_station];
        }
    }
    return NULL;
}