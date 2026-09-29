#include "../../include/grafo_matriz.h"
#include "../../include/aux_functions.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define BUFFER_SIZE 4096

typedef struct InitializeMatrizResponse{
    //Número de vértices
    int qtd_estacoes;
    //Número de arestas
    int qtd_arestas;
    //Vetor do tipo struct DadosAresta com os dados de cada aresta.
    DadosAresta **dados_aresta;
    //Vetor com os dados de cada estação.
    DadosEstacao **dados_estacao;
}InitMatrizResponse;

/*Inicialização da Matriz
char* path_stations: caminho do arquivo CSV das estações (vértices do grafo).
char* path_edges: caminho do arquivo CSV das arestas (ligações entre as estações).
*/

InitMatrizResponse* inicializar_matriz(char* path_stations, char* path_edges);

//Inicialização do GrafoMatriz 

GrafoMatriz* inicializar_grafo_matriz(char* path_stations, char* path_edges){

    //Alocação de memória
    GrafoMatriz* grafo = (GrafoMatriz*)malloc(sizeof(GrafoMatriz));
    //Verificação 
    if(grafo == NULL){
        perror("Erro ao alocar memória para o grafo matriz");
        exit(EXIT_FAILURE);
    }

    /* O Response lê os arquivos CSV e guarda os resultados (estações, arestas e
   quantidades) em uma struct temporária, o response */
    InitMatrizResponse* response = inicializar_matriz(path_stations,path_edges);

    //Tamanho total do número de vértices
    int capacidade = response->qtd_estacoes;

    //O grafo recebe dados sobre a aresta
    grafo->dados_arestas = response->dados_aresta;

    //O grafo recebe dados sobre o vértice
    grafo->dados_estacao = response->dados_estacao;

    //O grafo recebe o número total de arestas
    grafo->qtd_arestas = response->qtd_arestas;

    //Libera só a struct temporária; os vetores agora pertencem ao grafo
    free(response);response = NULL;

    /*Copia o número de estações para dentro do grafo. Assim, qualquer função que receba
    * o grafo sabe o tamanho da matriz (N × N) sem precisar de outro parâmetro.*/
    grafo->qtd_estacoes = capacidade;

    //Cria o vetor de linhas, cada indíce do vetor guarda um endereço de uma delas.
    grafo->matriz = (int**)calloc(capacidade,sizeof(int*));

    //Verificação
    if(grafo->matriz == NULL){
        free(grafo);
        perror("Erro ao alocar memória para a lista do grafo matriz");
        exit(EXIT_FAILURE);
    }

    //Para cada posição do vetor, aloca uma linha com N inteiros, todos iguais a 0. Depois disso a matriz N × N existe:
    for(int row = 0; row < capacidade; row++){
        grafo->matriz[row] = calloc(capacidade, sizeof(int));
        
        //Tratamento de erros.

        if(grafo->matriz[row] == NULL){
            for(int i = 0; i < row; i ++) free(grafo->matriz[i]);
            free(grafo->matriz);
            free(grafo);
            exit(EXIT_FAILURE);
        }
    }

    //Percorre todas as arestas no csv e marca elas na matriz.

    for(int i = 0; i < grafo->qtd_arestas; i++){

        DadosAresta* data = grafo->dados_arestas[i];

        //Se a aresta estiver vazia, marca como NULL e continua.
        if(data == NULL) continue;

        //Se não, coloca 1 na posição [origem][destino].
        /*Exemplo: se a aresta liga a estação de índice 0 à de índice 2, então matriz[0][2] = 1. 
        Como matriz[2][0] não é alterada, a ligação vale só em um sentido (grafo dirigido).*/
        grafo->matriz[data->id_source][data->id_target] = 1;

    }

    //Retorna o grafo.

    return grafo;
}

//Exibe a matriz:

void exibir_matriz(GrafoMatriz* grafo){ 
    if(!grafo)return; // grafo inexistente: nada a mostrar
    for(int row = 0; row < grafo->qtd_estacoes; row++){

        // Colchete de abertura na 1ª coluna, de fechamento na última
        for(int col = 0; col < grafo->qtd_estacoes; col++){
            if(col == 0){
                printf("[%d ", grafo->matriz[row][col]);
            }else if(col == grafo->qtd_estacoes - 1){
                printf("%d]", grafo->matriz[row][col]);
            }else{
                printf("%d ", grafo->matriz[row][col]);
            }
        }
        puts(""); // fim da linha da matriz
    }
    puts(""); // linha em branco após a matriz
}

// Escreve a matriz num arquivo texto, uma linha por estação,
// começando pelo código da estação

void imprimir_matriz(GrafoMatriz* grafo, char* output_path){
    if(!grafo)return;
    FILE* fp = fopen(output_path,"w"); // "w" apaga o conteúdo anterior
    if(!fp){
        fprintf(stderr, "Erro ao abrir o arquivo %s", output_path);
        return;
    }
    // for(int row = 0; row < grafo->qtd_estacoes; row++){
    //     fprintf(fp,"\t%s ", grafo->dados_estacao[row]->code);
    // }
    // fprintf(fp,"\n");
    for(int row = 0; row < grafo->qtd_estacoes; row++){
        for(int col = 0; col < grafo->qtd_estacoes; col++){
            if(col == 0){
                fprintf(fp,"%s [%d ",grafo->dados_estacao[row]->code,grafo->matriz[row][col]);
            }else if(col == grafo->qtd_estacoes - 1){
                fprintf(fp,"%d]", grafo->matriz[row][col]);
            }else{
                fprintf(fp,"%d ",grafo->matriz[row][col]);
            }
        }
        fprintf(fp,"\n");
    }
    fclose(fp);
}

//Gera um arquivo no formato DOT, que o Graphviz transforma em imagem do grafo:

void imprimir_matriz_dot(GrafoMatriz *grafo,char* output_path){

    // fopen abre o arquivo em output_path e devolve um FILE*,
    // o "identificador" usado depois por fprintf e fclose.
    // Modo "w": escrita; cria o arquivo se não existir e APAGA
    // o conteúdo se já existir.

    if(!grafo)return;
    FILE* fp = fopen(output_path, "w");
    if(!fp) return;
    fprintf(fp, "digraph G1{\n");

    // Declara os vértices (id da estação, desenhados como círculo)

    for(int i = 0; i < grafo->qtd_estacoes; i++){
        int vertice = grafo->dados_estacao[i]->id;
        fprintf(fp, "\t%d [shape=\"circle\"]\n",vertice);
    }
    fprintf(fp, "\n");

    // Cada 1 na matriz vira uma aresta; +1 converte índice (base 0) em id (base 1)
    
    for(int i = 0; i < grafo->qtd_estacoes; i++){
        for(int j = 0; j < grafo->qtd_estacoes; j++){
            if(grafo->matriz[i][j] == 1) fprintf(fp, "\t%d -> %d\n", i+1, j+1);
        }
    }
    fprintf(fp,"\n}");
    fclose(fp);
}



void liberar_grafo_matriz(GrafoMatriz** grafo){
    for(int i = 0; i < (*grafo)->qtd_estacoes; i++){
        free((*grafo)->matriz[i]);
        free((*grafo)->dados_estacao[i]->code);
        free((*grafo)->dados_estacao[i]->nome);
        free((*grafo)->dados_estacao[i]);
    }
    for(int i = 0; i < (*grafo)->qtd_arestas; i++){
        free((*grafo)->dados_arestas[i]);
    }
    free((*grafo)->matriz);
    free((*grafo)->dados_arestas);
    free((*grafo)->dados_estacao);
    free((*grafo));
    grafo = NULL;
}

// Procura a estação cujo code é igual a search.
// Devolve o índice no vetor, ou -1 se não existir.

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
    char* nome = NULL;
    char* code = NULL;
    
    // Realiza a leitura das estações e coloca os dados em array_dados_estacao para ser acessado como vetor;
    while(fgets(buffer,BUFFER_SIZE,stations) != NULL){
        count_lines++;
        if(count_lines == 1)continue;
        double latitude = -1, longitude = -1;
        nome = (char*)calloc(50, sizeof(char));
        if(nome == NULL){perror("Falha ao alocar memoria para \'nome\'"); exit(EXIT_FAILURE);}
        code = (char*)calloc(4,sizeof(char));
        if(code == NULL){perror("Falha ao alocar memoria para \'code\'"); exit(EXIT_FAILURE);}
        double tempo_transferencia = -1;
        
        sscanf(buffer,"%[^,],%lf,%lf,%[^,],%lf\n",code,&latitude,&longitude,nome,&tempo_transferencia);
        
        DadosEstacao* dados = (DadosEstacao*)malloc(sizeof(DadosEstacao));
        if(dados == NULL){
            perror("Falha ao alocar memoria para \'dados\'");
            exit(EXIT_FAILURE);
        }
        
        dados->id = count_lines- 1;
        dados->code = code;
        dados->latitude = latitude;
        dados->longitude = longitude;
        dados->nome = nome;
        dados->tempo_transferencia = tempo_transferencia;

        
        if(count_lines >= array_dados_length -1){
            array_dados_length *= 2;
            array_dados_estacao = (DadosEstacao**)realloc(array_dados_estacao,sizeof(DadosEstacao**) * array_dados_length);
            if(!array_dados_estacao){
                perror("não foi possivel realocar a memoria do array_dados_estacao");
                fclose(stations);
                for(size_t i = 0; i < count_lines; i ++){free(array_dados_estacao[i]->code);free(array_dados_estacao[i]->nome); free(array_dados_estacao[i]);}
                free(array_dados_estacao);free(dados);
                exit(EXIT_FAILURE);
            }
        }
        array_dados_estacao[count_lines -2] = dados;
    }
    fclose(stations);
    response->qtd_estacoes = count_lines-1;
    response->dados_estacao = array_dados_estacao;


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
            dados->id_source = idx;
        }
        dados->source = dados_source->code;
        
        if(dados_target == NULL || strcmp(dados_target->code, target)){
            int idx = get_station_matriz(array_dados_estacao, response->qtd_estacoes, target);
            if(idx == -1){printf("\nFalha ao encontrar a estação de codigo: %s\n",target); exit(EXIT_FAILURE);}
            dados_target = array_dados_estacao[idx];
            dados->id_target = idx;
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
        array_dados_aresta[count_lines - 2] = dados;
    }
    fclose(edges);

    response->dados_aresta = array_dados_aresta;
    response->qtd_arestas = count_lines -1;
    return response;
}