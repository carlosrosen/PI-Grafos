#include "dijkstra.h"
#include "acesso_grafo.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct EstadoDijkstra{
    double *distancia;
    int *predecessor;
    int *finalizado;
} EstadoDijkstra;

typedef enum TipoGrafo{
    TIPO_GRAFO_LISTA,
    TIPO_GRAFO_MATRIZ
} TipoGrafo;

static MinHeap *criar_heap(int capacidade_inicial){
    MinHeap *heap = (MinHeap *)malloc(sizeof(MinHeap));
    if (heap == NULL){
        perror("Falha ao alocar memoria para o heap do Dijkstra");
        return NULL;
    }

    heap->itens = (NoHeap *)malloc(sizeof(NoHeap) * capacidade_inicial);
    if (heap->itens == NULL){
        perror("Falha ao alocar memoria para os itens do heap do Dijkstra");
        free(heap);
        return NULL;
    }

    heap->tamanho = 0;
    heap->capacidade = capacidade_inicial;
    return heap;
}

static void liberar_heap(MinHeap *heap){
    if (heap == NULL) return;
    free(heap->itens);
    free(heap);
}

static void trocar_entradas(NoHeap *a, NoHeap *b){
    NoHeap auxiliar = *a;
    *a = *b;
    *b = auxiliar;
}

static void subir_heap(MinHeap *heap, int indice){
    while (indice > 0){
        int pai = (indice - 1) / 2;
        if (heap->itens[pai].distancia <= heap->itens[indice].distancia) break;
        trocar_entradas(&heap->itens[pai], &heap->itens[indice]);
        indice = pai;
    }
}

// Retorna o indice do menor entre o no e seus dois filhos. 
static int encontrar_filho_menor(MinHeap *heap, int indice){
    int esquerda = 2 * indice + 1;
    int direita = 2 * indice + 2;
    int menor = indice;

    if (esquerda < heap->tamanho && heap->itens[esquerda].distancia < heap->itens[menor].distancia){
        menor = esquerda;
    }
    if (direita < heap->tamanho && heap->itens[direita].distancia < heap->itens[menor].distancia){
        menor = direita;
    }

    return menor;
}

static void descer_heap(MinHeap *heap, int indice){
    int menor = encontrar_filho_menor(heap, indice);

    while (menor != indice){
        trocar_entradas(&heap->itens[indice], &heap->itens[menor]);
        indice = menor;
        menor = encontrar_filho_menor(heap, indice);
    }
}

static int inserir_heap(MinHeap *heap, int vertice, double distancia){
    if (heap->tamanho >= heap->capacidade){
        int nova_capacidade = heap->capacidade * 2;
        NoHeap *novos_itens = (NoHeap *)realloc(
            heap->itens, sizeof(NoHeap) * nova_capacidade
        );
        if (novos_itens == NULL){
            perror("Falha ao realocar memoria para os itens do heap do Dijkstra");
            return 0;
        }
        heap->itens = novos_itens;
        heap->capacidade = nova_capacidade;
    }

    int indice = heap->tamanho;
    heap->itens[indice].vertice = vertice;
    heap->itens[indice].distancia = distancia;
    heap->tamanho++;
    subir_heap(heap, indice);
    return 1;
}

static NoHeap remover_min(MinHeap *heap){
    NoHeap minimo = heap->itens[0];
    heap->tamanho--;

    if (heap->tamanho > 0){
        heap->itens[0] = heap->itens[heap->tamanho];
        descer_heap(heap, 0);
    }

    return minimo;
}

static EstadoDijkstra *criar_estado(int quantidade_vertices){
    EstadoDijkstra *estado = (EstadoDijkstra *)malloc(sizeof(EstadoDijkstra));
    if (estado == NULL){
        perror("Falha ao alocar memoria para o estado do Dijkstra");
        return NULL;
    }

    estado->distancia = (double *)malloc(sizeof(double) * quantidade_vertices);
    estado->predecessor = (int *)malloc(sizeof(int) * quantidade_vertices);
    estado->finalizado = (int *)calloc(quantidade_vertices, sizeof(int));

    if (estado->distancia == NULL || estado->predecessor == NULL || estado->finalizado == NULL){
        perror("Falha ao alocar memoria para os vetores do Dijkstra");
        free(estado->distancia);
        free(estado->predecessor);
        free(estado->finalizado);
        free(estado);
        return NULL;
    }

    for (int i = 0; i < quantidade_vertices; i++){
        estado->distancia[i] = INFINITY;
        estado->predecessor[i] = -1;
    }

    return estado;
}

static void liberar_estado(EstadoDijkstra *estado){
    if (estado == NULL) return;
    free(estado->distancia);
    free(estado->predecessor);
    free(estado->finalizado);
    free(estado);
}

static ResultadoDijkstra *criar_resultado(int origem, int destino, int encontrado){
    ResultadoDijkstra *resultado = (ResultadoDijkstra *)calloc(1, sizeof(ResultadoDijkstra));
    if (resultado == NULL){
        perror("Falha ao alocar memoria para o resultado do Dijkstra");
        return NULL;
    }

    resultado->origem = origem;
    resultado->destino = destino;
    resultado->encontrado = encontrado;
    resultado->custo = 0.0;
    resultado->caminho = NULL;
    resultado->tamanho_caminho = 0;
    return resultado;
}

// Segue os predecessores do destino ate a origem e monta o caminho em ordem. Retorna 0 em falha de alocacao e 1 em sucesso. 
static int reconstruir_caminho(ResultadoDijkstra *resultado, const int *predecessor){
    int tamanho = 0;
    for (int v = resultado->destino; v != -1; v = predecessor[v]){
        tamanho++;
        if (v == resultado->origem) break;
    }

    resultado->caminho = (int *)malloc(sizeof(int) * tamanho);
    if (resultado->caminho == NULL){
        perror("Falha ao alocar memoria para o caminho do Dijkstra");
        return 0;
    }

    int indice = tamanho - 1;
    for (int v = resultado->destino; v != -1; v = predecessor[v]){
        resultado->caminho[indice] = v;
        indice--;
        if (v == resultado->origem) break;
    }

    resultado->tamanho_caminho = tamanho;
    return 1;
}

static int relaxar_aresta(EstadoDijkstra *estado, MinHeap *heap, int origem, int destino, double peso){
    if (estado->finalizado[destino]) return 1;

    double nova_distancia = estado->distancia[origem] + peso;
    if (nova_distancia < estado->distancia[destino]){
        estado->distancia[destino] = nova_distancia;
        estado->predecessor[destino] = origem;
        if (!inserir_heap(heap, destino, nova_distancia)) return 0;
    }
    return 1;
}

static int iterar_vizinhos_lista(void *grafo, int u, EstadoDijkstra *estado, MinHeap *heap){
    GrafoLista *grafo_lista = (GrafoLista *)grafo;

    /* A cabeca da lista representa o proprio vertice, os vizinhos comecam em proximo. */
    for (No *no = grafo_lista->lista[u]->proximo; no != NULL; no = no->proximo){
        int v = no->vertice - 1; /* lista usa indices 1-based */
        if (v < 0 || v >= (int)grafo_lista->qtd_no) continue;
        if (!relaxar_aresta(estado, heap, u, v, no->peso)) return 0;
    }
    return 1;
}

static int iterar_vizinhos_matriz(void *grafo, int u, EstadoDijkstra *estado, MinHeap *heap){
    GrafoMatriz *grafo_matriz = (GrafoMatriz *)grafo;

    for (int v = 0; v < grafo_matriz->qtd_estacoes; v++){
        double peso = grafo_matriz->matriz[u][v];
        if (peso > 0.0 && !relaxar_aresta(estado, heap, u, v, peso)) return 0;
    }
    return 1;
}

/* Itera os vizinhos de u conforme a representacao do grafo.
   Retorna 0 em falha de alocacao e 1 em sucesso. */
static int iterar_vizinhos(void *grafo, TipoGrafo tipo, int u, EstadoDijkstra *estado, MinHeap *heap){
    if (tipo == TIPO_GRAFO_LISTA){
        return iterar_vizinhos_lista(grafo, u, estado, heap);
    }
    return iterar_vizinhos_matriz(grafo, u, estado, heap);
}

static ResultadoDijkstra *executar_dijkstra(
    void *grafo,
    TipoGrafo tipo,
    int quantidade_vertices,
    int origem,
    int destino
){
    if (origem < 0 || origem >= quantidade_vertices || destino < 0 || destino >= quantidade_vertices){
        return criar_resultado(origem, destino, 0);
    }

    EstadoDijkstra *estado = criar_estado(quantidade_vertices);
    if (estado == NULL) return NULL;

    MinHeap *heap = criar_heap(quantidade_vertices);
    if (heap == NULL){
        liberar_estado(estado);
        return NULL;
    }

    estado->distancia[origem] = 0.0;
    if (!inserir_heap(heap, origem, 0.0)){
        liberar_estado(estado);
        liberar_heap(heap);
        return NULL;
    }

    while (heap->tamanho > 0){
        NoHeap atual = remover_min(heap);
        int u = atual.vertice;

        // ignora entradas antigas e vertices ja finalizados. 
        if (estado->finalizado[u] || atual.distancia > estado->distancia[u]) continue;

        estado->finalizado[u] = 1;
        if (u == destino) break;

        if (!iterar_vizinhos(grafo, tipo, u, estado, heap)){
            liberar_estado(estado);
            liberar_heap(heap);
            return NULL;
        }
    }

    ResultadoDijkstra *resultado;
    if (!estado->finalizado[destino]){
        resultado = criar_resultado(origem, destino, 0);
    } else{
        resultado = criar_resultado(origem, destino, 1);
        if (resultado != NULL){
            resultado->custo = estado->distancia[destino];
            if (!reconstruir_caminho(resultado, estado->predecessor)){
                liberar_resultado_dijkstra(&resultado);
            }
        }
    }

    liberar_estado(estado);
    liberar_heap(heap);
    return resultado;
}

ResultadoDijkstra *dijkstra_lista(GrafoLista *grafo, int origem, int destino){
    if (grafo == NULL) return criar_resultado(origem, destino, 0);
    return executar_dijkstra(grafo, TIPO_GRAFO_LISTA, (int)grafo->qtd_no, origem, destino);
}

ResultadoDijkstra *dijkstra_matriz(GrafoMatriz *grafo, int origem, int destino){
    if (grafo == NULL) return criar_resultado(origem, destino, 0);
    return executar_dijkstra(grafo, TIPO_GRAFO_MATRIZ, grafo->qtd_estacoes, origem, destino);
}

static void escrever_resultado_lista(FILE *saida, GrafoLista *grafo, ResultadoDijkstra *resultado){
    if (saida == NULL || grafo == NULL || resultado == NULL) return;

    fprintf(saida, "=== Rota ideal (lista de adjacencia) ===\n");

    if (resultado->origem < 0 || resultado->origem >= (int)grafo->qtd_no || resultado->destino < 0 || resultado->destino >= (int)grafo->qtd_no){
        fprintf(saida, "Origem ou destino invalido.\n");
        return;
    }

    DadosEstacao *origem = grafo->lista[resultado->origem]->dados;
    DadosEstacao *destino = grafo->lista[resultado->destino]->dados;

    fprintf(saida, "Origem: %s - %s\n", origem->code, origem->nome);
    fprintf(saida, "Destino: %s - %s\n", destino->code, destino->nome);

    if (!resultado->encontrado){
        fprintf(saida, "Nao existe caminho entre %s (%s) e %s (%s).\n",
                origem->code, origem->nome, destino->code, destino->nome);
        return;
    }

    fprintf(saida, "Caminho (%d estacoes): ", resultado->tamanho_caminho);
    for (int i = 0; i < resultado->tamanho_caminho; i++){
        DadosEstacao *estacao = grafo->lista[resultado->caminho[i]]->dados;
        fprintf(saida, "%s", estacao->code);
        if (i + 1 < resultado->tamanho_caminho) fprintf(saida, " -> ");
    }
    fprintf(saida, "\nCusto total: %.2f\n", resultado->custo);
}

static void escrever_resultado_matriz(FILE *saida, GrafoMatriz *grafo, ResultadoDijkstra *resultado){
    if (saida == NULL || grafo == NULL || resultado == NULL) return;

    fprintf(saida, "=== Rota ideal (matriz de adjacencia) ===\n");

    if (resultado->origem < 0 || resultado->origem >= grafo->qtd_estacoes || resultado->destino < 0 || resultado->destino >= grafo->qtd_estacoes){
        fprintf(saida, "Origem ou destino invalido.\n");
        return;
    }

    DadosEstacao *origem = grafo->dados_estacao[resultado->origem];
    DadosEstacao *destino = grafo->dados_estacao[resultado->destino];

    fprintf(saida, "Origem: %s - %s\n", origem->code, origem->nome);
    fprintf(saida, "Destino: %s - %s\n", destino->code, destino->nome);

    if (!resultado->encontrado){
        fprintf(saida, "Nao existe caminho entre %s (%s) e %s (%s).\n",
                origem->code, origem->nome, destino->code, destino->nome);
        return;
    }

    fprintf(saida, "Caminho (%d estacoes): ", resultado->tamanho_caminho);
    for (int i = 0; i < resultado->tamanho_caminho; i++){
        DadosEstacao *estacao = grafo->dados_estacao[resultado->caminho[i]];
        fprintf(saida, "%s", estacao->code);
        if (i + 1 < resultado->tamanho_caminho) fprintf(saida, " -> ");
    }
    fprintf(saida, "\nCusto total: %.2f\n", resultado->custo);
}

void imprimir_resultado_dijkstra_lista(GrafoLista *grafo, ResultadoDijkstra *resultado){
    escrever_resultado_lista(stdout, grafo, resultado);
}

void imprimir_resultado_dijkstra_matriz(GrafoMatriz *grafo, ResultadoDijkstra *resultado){
    escrever_resultado_matriz(stdout, grafo, resultado);
}

void exportar_resultado_dijkstra_lista(GrafoLista *grafo, ResultadoDijkstra *resultado, char *output_path){
    FILE *saida = fopen(output_path, "w");
    if (saida == NULL){
        perror("Falha ao abrir arquivo de saida do Dijkstra (lista)");
        return;
    }

    escrever_resultado_lista(saida, grafo, resultado);
    fclose(saida);
}

void exportar_resultado_dijkstra_matriz(GrafoMatriz *grafo, ResultadoDijkstra *resultado, char *output_path){
    FILE *saida = fopen(output_path, "w");
    if (saida == NULL){
        perror("Falha ao abrir arquivo de saida do Dijkstra (matriz)");
        return;
    }
    escrever_resultado_matriz(saida, grafo, resultado);
    fclose(saida);
}

void liberar_resultado_dijkstra(ResultadoDijkstra **resultado){
    if (resultado == NULL || *resultado == NULL) return;
    free((*resultado)->caminho);
    free(*resultado);
    *resultado = NULL;
}

static int imprimir_rota_por_codigo(void *grafo, TipoGrafo tipo, int qtd_vertices,const char *origem_codigo,const char *destino_codigo){
    AcessoGrafo acesso;
    ResultadoDijkstra *resultado;
    int origem;
    int destino;

    if (grafo == NULL) return -1;

    acesso = tipo == TIPO_GRAFO_LISTA ? criar_acesso_lista((GrafoLista *)grafo) : criar_acesso_matriz((GrafoMatriz *)grafo);

    origem = buscar_indice_estacao(&acesso, origem_codigo);
    destino = buscar_indice_estacao(&acesso, destino_codigo);

    if (origem < 0 || destino < 0) return 0;

    resultado = executar_dijkstra(grafo, tipo, qtd_vertices, origem, destino);
    if (resultado == NULL) return -1;

    if (tipo == TIPO_GRAFO_LISTA){
        imprimir_resultado_dijkstra_lista((GrafoLista *)grafo, resultado);
    } else {
        imprimir_resultado_dijkstra_matriz((GrafoMatriz *)grafo, resultado);
    }

    liberar_resultado_dijkstra(&resultado);
    return 1;
}

int imprimir_rota_por_codigo_lista(GrafoLista *grafo, const char *origem, const char *destino){
    if (grafo == NULL) return -1;
    return imprimir_rota_por_codigo(grafo, TIPO_GRAFO_LISTA, (int)grafo->qtd_no, origem, destino);
}

int imprimir_rota_por_codigo_matriz(GrafoMatriz *grafo, const char *origem, const char *destino){
    if (grafo == NULL) return -1;
    return imprimir_rota_por_codigo(grafo, TIPO_GRAFO_MATRIZ, grafo->qtd_estacoes, origem, destino);
}
