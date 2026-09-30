#include "../../include/grafo_lista.h"
#include "../../include/aux_functions.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 4096

/* Estrutura auxiliar para retornar dinamicamente
um vetor genérico e o seu tamanho*/
typedef struct response_object_length{
    void **object;
    unsigned int length;
} ResponseObjectLength;

// Protótipos das funções privadas auxiliares deste módulo
void Liberar_Nos(No** nos, unsigned int no_length);

/*Lê o CSV de estações, inicializa os nós principais 
e retorna o vetor de estações com o total lido.*/ 
ResponseObjectLength* inicializa_stations(char* path_stations);

/* Lê o CSV de conexões, insere os vizinhos na 
lista de adjacência de cada nó e retorna o vetor de arestas.*/
ResponseObjectLength* Inicializar_edges(No** stations, int stations_length, char* path_edges);

/* Busca a estação no vetor pelo seu código identificador
e retorna o nó correspondente (ou NULL se não encontrar).*/

No* get_station(No** stations, int stations_length, char *code);


/* Inicializa e constrói a lista de adjacência
do grafo a partir dos caminhos dos arquivos CSV*/
GrafoLista* inicializar_grafo_lista(char* path_stations, char* path_edges){
    // Aloca memória para a estrutura principal do grafo
    GrafoLista* grafo = (GrafoLista *)malloc(sizeof(GrafoLista));
    if(grafo == NULL){
        fprintf(stderr, "Falha ao alocar memória para o grafo\n");
        exit(EXIT_FAILURE);
    }
    
    // Carrega todas as estações (nós do grafo) a partir do CSV de estações
    ResponseObjectLength *response = inicializa_stations(path_stations);
    No **stations = (No**)response->object;
    unsigned int stations_length = response->length;
    free(response); // Libera o invólucro de resposta
    
    // Conecta as estações criando as arestas a partir do CSV de conexões
    response = Inicializar_edges(stations, stations_length, path_edges);
    
    Aresta **array_arestas = (Aresta**)response->object;
    unsigned int arestas_length = response->length;
    free(response); // Libera o invólucro de resposta
    
    // Atribui os dados carregados para a estrutura do GrafoLista
    grafo->lista = stations;
    grafo->arestas = array_arestas;
    grafo->qtd_no = stations_length;
    grafo->qtd_arestas = arestas_length;
    
    return grafo;
}


// Exibe a lista de adjacência diretamente no console
void exibir_lista(GrafoLista* grafo){
    if(!grafo) return;
    puts("");
    for(unsigned int i = 0; i < grafo->qtd_no; i++){
        No* no = grafo->lista[i];
        printf("code: %s, lat: %f, long: %f, nome: %s\n", no->dados->code, no->dados->latitude, no->dados->longitude, no->dados->nome);
        
        // Percorre a lista encadeada do nó i imprimindo os vizinhos
        while(no != NULL){
            if(no->proximo != NULL) printf("[ %d | %s ] -> ", no->vertice, no->dados->code);
            else printf("[ %d | %s ]\n", no->vertice, no->dados->code);
            no = no->proximo;
        }
    }
    puts("");
}


// Escreve a lista de adjacência em formato texto em um arquivo em disco
void imprimir_lista(GrafoLista *grafo, char* output_path){
    if(!grafo) return;
    FILE* fp = fopen(output_path, "w");
    if(!fp){
        fprintf(stderr, "Erro ao abrir o arquivo %s\n", output_path);
        return;
    }
    for(unsigned int i = 0; i < grafo->qtd_no; i++){
        No* no = grafo->lista[i];
        fprintf(fp, "code: %s, lat: %f, long: %f, nome: %s\n", no->dados->code, no->dados->latitude, no->dados->longitude, no->dados->nome);
        while(no != NULL){
            if(no->proximo != NULL) fprintf(fp, "[ %d | %s ] -> ", no->vertice, no->dados->code);
            else fprintf(fp, "[ %d | %s ]\n", no->vertice, no->dados->code);
            no = no->proximo;
        }
    }
    fclose(fp);
}


// Exporta a estrutura do grafo no formato Graphviz (.dot) para visualização em diagramas
void imprimir_lista_dot(GrafoLista *grafo, char* output_path){
    if(!grafo) return;
    FILE* fp = fopen(output_path, "w");
    if(!fp) return;

    fprintf(fp, "digraph G1{\n");
    // Define todos os vértices
    for(unsigned int i = 0; i < grafo->qtd_no; i++){
        int vertice = grafo->lista[i]->vertice;
        fprintf(fp, "\t%d [shape=\"circle\"]\n", vertice);
    }
    fprintf(fp, "\n");
    // Define todas as arestas direcionadas
    for(unsigned int i = 0; i < grafo->qtd_arestas; i++){
        int vertice_source = grafo->arestas[i]->source->vertice;
        int vertice_target = grafo->arestas[i]->target->vertice;
        fprintf(fp, "\t%d -> %d\n", vertice_source, vertice_target);
    }
    fprintf(fp, "\n}");
    fclose(fp);
}


// Libera a memória alocada para os nós e listas encadeadas das estações
void Liberar_Nos(No** nos, unsigned int no_length){
    if(nos == NULL) return;
    for(unsigned int i = 0; i < no_length; i++){
        No* atual = nos[i];
        
        // Libera os dados da estação (armazenados na cabeça da lista)
        if(atual != NULL && atual->dados != NULL){
            free(atual->dados->nome);
            free(atual->dados->code);
            free(atual->dados);
        }

        // Percorre e desaloca cada nó adjacente da lista encadeada
        while(atual != NULL){
            No* temp = atual;
            atual = atual->proximo;
            free(temp);
        }
    }
    free(nos);
}


// Libera a memória alocada para o vetor de arestas do grafo
void Liberar_Arestas(Aresta** arestas, unsigned int qtd_arestas){
    if(arestas == NULL) return;
    for(unsigned int i = 0; i < qtd_arestas; i++){
        if(arestas[i] != NULL){
            if(arestas[i]->dados != NULL) free(arestas[i]->dados);
            free(arestas[i]);
        }
    }
    free(arestas);
}


// Função principal para desalocar a estrutura completa de GrafoLista
void Liberar_Grafo(GrafoLista *grafo){
    if(grafo == NULL) return;
    Liberar_Nos(grafo->lista, grafo->qtd_no);
    Liberar_Arestas(grafo->arestas, grafo->qtd_arestas);
    free(grafo);
}


// Lê o CSV de estações e cria o vetor de cabeças das listas de adjacência
ResponseObjectLength* inicializa_stations(char* path_stations){
    FILE* stations = open_file_read_mode(path_stations);

    char buffer[BUFFER_SIZE];
    int count_lines = 0;
    int all_stations_length = 1000;
    No** all_stations = (No**)malloc(sizeof(No*) * all_stations_length);

    while(fgets(buffer, BUFFER_SIZE, stations) != NULL){
        count_lines++;
        if(count_lines == 1) continue; // Pula a linha do cabeçalho do CSV

        double longitude, latitude, transfer_time;
        char *nome = (char*)calloc(50, sizeof(char));
        if(nome == NULL){ perror("Erro ao alocar memória para {nome} da estação"); exit(EXIT_FAILURE); }
        char *code = (char*)calloc(4, sizeof(char));
        if(code == NULL){ perror("Erro ao alocar memória para {code} da estação"); exit(EXIT_FAILURE); }

        // Faz o parse dos campos da linha do arquivo CSV
        sscanf(buffer, "%[^,],%lf,%lf,%[^,],%lf\n", code, &latitude, &longitude, nome, &transfer_time);

        // Preenche os metadados da estação e constrói o nó inicial
        No* station = (No*)malloc(sizeof(No));
        DadosEstacao *dados = (DadosEstacao*)malloc(sizeof(DadosEstacao));
        dados->code = code;
        dados->latitude = latitude;
        dados->longitude = longitude;
        dados->nome = nome;
        dados->tempo_transferencia = transfer_time;
        dados->id = count_lines - 1;

        station->vertice = count_lines - 1;
        station->dados = dados;
        station->proximo = NULL;

        // Dobra o tamanho do vetor dinâmico de estações se a capacidade for atingida
        if(count_lines + 1 >= all_stations_length){
            all_stations_length *= 2;
            all_stations = realloc(all_stations, sizeof(No*) * all_stations_length);
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


// Lê o CSV de arestas e insere os vizinhos no final da lista encadeada de cada estação
ResponseObjectLength* Inicializar_edges(No** stations, int stations_length, char* path_edges){
    FILE* edges = open_file_read_mode(path_edges);

    char buffer[BUFFER_SIZE];
    unsigned int count_lines = 0;
 
    unsigned int array_arestas_length = 1000;
    Aresta **array_arestas = (Aresta**)malloc(sizeof(Aresta*) * array_arestas_length);

    while(fgets(buffer, BUFFER_SIZE, edges)){
        count_lines++;
        if(count_lines == 1) continue; // Pula a linha do cabeçalho do CSV

        char source[4] = {'\0', '\0', '\0', '\0'};
        char target[4] = {'\0', '\0', '\0', '\0'};
        double distance = -1.0;
        double travel_time = -1.0;
        
        // Faz o parse das conexões e pesos do CSV
        sscanf(buffer, "%[^,],%[^,],%lf,%lf\n", source, target, &distance, &travel_time);

        No *no_source = NULL, *no_target = NULL;
        Aresta *aresta = (Aresta*)malloc(sizeof(Aresta));
        if(aresta == NULL){ perror("Erro ao alocar memória para {aresta} dos edges"); exit(EXIT_FAILURE); }

        DadosAresta* dados = (DadosAresta*)calloc(1, sizeof(DadosAresta));
        
        // Localiza os ponteiros das estações de origem e destino
        if(no_source == NULL || strcmp(no_source->dados->code, source)){
            no_source = get_station(stations, stations_length, source);
        }
        if(no_source == NULL){ printf("\nFalha ao encontrar a estação de código: %s\n", source); exit(EXIT_FAILURE); }
        aresta->source = no_source;
        
        if(no_target == NULL || strcmp(no_target->dados->code, target)){
            no_target = get_station(stations, stations_length, target);
        }
        if(no_target == NULL){ printf("\nFalha ao encontrar a estação de código: %s\n", target); exit(EXIT_FAILURE); }
        aresta->target = no_target;

        dados->distancia = distance;
        dados->tempo_viagem = travel_time;
        aresta->dados = dados;

        // Dobra a capacidade do vetor dinâmico de arestas se necessário
        if(count_lines + 1 >= array_arestas_length){
            array_arestas_length *= 2;
            array_arestas = (Aresta**)realloc(array_arestas, sizeof(Aresta*) * array_arestas_length);
            if(array_arestas == NULL){
                perror("Erro ao realocar mais memória para {array_arestas}");
                exit(EXIT_FAILURE);
            }
        }
        array_arestas[count_lines - 2] = aresta;

        // Cria o nó referente ao destino e insere no fim da lista do nó de origem
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

    fclose(edges);

    ResponseObjectLength *response = (ResponseObjectLength*)malloc(sizeof(ResponseObjectLength));

    response->object = (void**)array_arestas;
    response->length = count_lines - 1;

    return response;
}


// Realiza a busca linear pelo código da estação para obter o seu nó de origem
No* get_station(No** stations, int stations_length, char *code){
    for(int idx_station = 0; idx_station < stations_length; idx_station++){
        if(!strcmp(stations[idx_station]->dados->code, code)){
            return stations[idx_station];
        }
    }
    return NULL;
}