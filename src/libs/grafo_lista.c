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
    fprintf(fp,"Quantidade de Nós: %d | Quantidade de arestas: %d\n\n",grafo->qtd_no, grafo->qtd_arestas);
    for(unsigned int i = 0; i < grafo->qtd_no; i++){
        No* no = grafo->lista[i];
        fprintf(fp,"code: %s, lat: %f, long: %f, nome: %s\n",no->dados->code, no->dados->latitude, no->dados->longitude, no->dados->nome);
        fprintf(fp,"\t");
        while(no != NULL){
            if(no->proximo != NULL)fprintf(fp,"[ %d | %s ] -> ",no->vertice, no->dados->code);
            else fprintf(fp,"[ %d | %s ]\n",no->vertice, no->dados->code);
            no = no->proximo;
        }
    }
    fclose(fp);
}


void imprimir_lista_dot(GrafoLista *grafo,char* output_path){
    if(!grafo)return;
    FILE* fp = fopen(output_path, "w");
    if(!fp) return;
    fprintf(fp, "graph G1{\n");
    for(unsigned int i = 0; i < grafo->qtd_no; i++){
        int vertice = grafo->lista[i]->vertice;
        fprintf(fp, "\t%d [shape=\"circle\"]\n",vertice);
    }
    fprintf(fp, "\n");
    for(unsigned int i = 0; i < grafo->qtd_arestas; i++){
        int vertice_source = grafo->arestas[i]->source->vertice;
        int vertice_target = grafo->arestas[i]->target->vertice;
        fprintf(fp,"\t%d -- %d\n", vertice_source, vertice_target);
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
// Garante grafo NÃO orientado: para cada linha do CSV verifica se source->target e target->source
// já existem. Cria apenas os apontamentos faltantes.
ResponseObjectLength* Inicializar_edges(No** stations,int stations_length){
    FILE* edges = open_file_read_mode("./dataset/edges.csv");

    char buffer[BUFFER_SIZE];
    unsigned int count_lines = 0;
 
    unsigned int array_arestas_length = 1000;
    unsigned int total_arestas = 0; // contador real de arestas inseridas
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

        no_source = get_station(stations, stations_length, source);
        if(no_source == NULL){printf("\nFalha ao encontrar a estação de código: %s\n",source); exit(EXIT_FAILURE);}

        no_target = get_station(stations, stations_length, target);
        if(no_target == NULL){printf("\nFalha ao encontrar a estação de codigo: %s\n",target); exit(EXIT_FAILURE);}

        // Verificar se já existem arestas source->target e target->source
        int existe_source_target = 0;
        int existe_target_source = 0;
        for(unsigned int i = 0; i < total_arestas; i++){
            if(array_arestas[i]->source->vertice == no_source->vertice &&
               array_arestas[i]->target->vertice == no_target->vertice){
                existe_source_target = 1;
            }
            if(array_arestas[i]->source->vertice == no_target->vertice &&
               array_arestas[i]->target->vertice == no_source->vertice){
                existe_target_source = 1;
            }
            if(existe_source_target && existe_target_source) break;
        }

        // Se ambas já existem, pula para a próxima linha
        if(existe_source_target && existe_target_source){
            continue;
        }

        // Criar aresta source->target se não existe
        if(!existe_source_target){
            Aresta *aresta_st = (Aresta*)malloc(sizeof(Aresta));
            if(aresta_st == NULL){perror("Erro ao alocar memória para {aresta} dos edges"); exit(EXIT_FAILURE);}
            DadosAresta* dados_st = (DadosAresta*)calloc(1,sizeof(DadosAresta));
            dados_st->distancia = distance;
            dados_st->tempo_viagem = travel_time;
            aresta_st->source = no_source;
            aresta_st->target = no_target;
            aresta_st->dados = dados_st;

            // Garantir espaço no array
            if(total_arestas + 2 >= array_arestas_length){
                array_arestas_length *= 2;
                array_arestas = (Aresta**)realloc(array_arestas,sizeof(Aresta*) * array_arestas_length);
                if(array_arestas == NULL){
                    perror("Erro ao realocar mais memória para {array_arestas}");
                    exit(EXIT_FAILURE);
                }
            }
            array_arestas[total_arestas] = aresta_st;
            total_arestas++;

            // Inserir no_target na lista de adjacência de no_source
            No* new_node_st = (No*)malloc(sizeof(No));
            new_node_st->dados = no_target->dados;
            new_node_st->vertice = no_target->vertice;
            new_node_st->proximo = NULL;
            No* aux_st = stations[no_source->vertice - 1];
            while(aux_st->proximo != NULL){
                aux_st = aux_st->proximo;
            }
            aux_st->proximo = new_node_st;
        }

        // Criar aresta target->source se não existe (garante bidirecionalidade)
        if(!existe_target_source){
            Aresta *aresta_ts = (Aresta*)malloc(sizeof(Aresta));
            if(aresta_ts == NULL){perror("Erro ao alocar memória para {aresta} dos edges"); exit(EXIT_FAILURE);}
            DadosAresta* dados_ts = (DadosAresta*)calloc(1,sizeof(DadosAresta));
            dados_ts->distancia = distance;
            dados_ts->tempo_viagem = travel_time;
            aresta_ts->source = no_target;
            aresta_ts->target = no_source;
            aresta_ts->dados = dados_ts;

            // Garantir espaço no array
            if(total_arestas + 1 >= array_arestas_length){
                array_arestas_length *= 2;
                array_arestas = (Aresta**)realloc(array_arestas,sizeof(Aresta*) * array_arestas_length);
                if(array_arestas == NULL){
                    perror("Erro ao realocar mais memória para {array_arestas}");
                    exit(EXIT_FAILURE);
                }
            }
            array_arestas[total_arestas] = aresta_ts;
            total_arestas++;

            // Inserir no_source na lista de adjacência de no_target
            No* new_node_ts = (No*)malloc(sizeof(No));
            new_node_ts->dados = no_source->dados;
            new_node_ts->vertice = no_source->vertice;
            new_node_ts->proximo = NULL;
            No* aux_ts = stations[no_target->vertice - 1];
            while(aux_ts->proximo != NULL){
                aux_ts = aux_ts->proximo;
            }
            aux_ts->proximo = new_node_ts;
        }
    }

    fclose(edges);

    ResponseObjectLength *response = (ResponseObjectLength*)malloc(sizeof(ResponseObjectLength));

    response->object = (void**)array_arestas;
    response->length = total_arestas;

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