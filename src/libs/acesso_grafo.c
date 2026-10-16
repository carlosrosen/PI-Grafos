#include "acesso_grafo.h"

#include <stddef.h>

/*
    Adaptadores que permitem aos algoritmos de grafo percorrerem vizinhos e
    acessarem os dados das estacoes sem conhecer a representacao concreta
    (lista ou matriz de adjacencia).
*/

// ====================== Adaptadores: lista de adjacencia ======================


static DadosEstacao* obter_dados_estacao_lista(void* grafo_generico, int indice){
    GrafoLista* grafo = (GrafoLista*)grafo_generico;
    return grafo->lista[indice]->dados;
}

static int proximo_vizinho_lista(void* grafo_generico, int u, CursorVizinhos* cursor){
    (void)grafo_generico;
    (void)u;

    if(cursor->no == NULL) return -1;

    // Converte o identificador base-1 do nó para índice base-0
    int v = cursor->no->vertice - 1;
    cursor->no = cursor->no->proximo;
    return v;
}

static int iniciar_vizinhos_lista(void* grafo_generico, int u, CursorVizinhos* cursor){
    GrafoLista* grafo = (GrafoLista*)grafo_generico;

    // O nó cabeça representa a própria estação; a adjacência começa no próximo
    cursor->no = grafo->lista[u] != NULL ? grafo->lista[u]->proximo : NULL;
    return proximo_vizinho_lista(grafo_generico, u, cursor);
}

AcessoGrafo criar_acesso_lista(GrafoLista* grafo){
    AcessoGrafo acesso = {
        .grafo = grafo,
        .qtd_vertices = grafo != NULL ? (int)grafo->qtd_no : 0,
        .obter_dados_estacao = obter_dados_estacao_lista,
        .iniciar_vizinhos = iniciar_vizinhos_lista,
        .proximo_vizinho = proximo_vizinho_lista,
    };
    return acesso;
}


// ====================== Adaptadores: matriz de adjacencia ======================


// Existe ligação entre i e j em qualquer sentido? (grafo tratado como não dirigido)
static int existe_aresta(GrafoMatriz* grafo, int i, int j){
    return grafo->matriz[i][j] || grafo->matriz[j][i];
}

static DadosEstacao* obter_dados_estacao_matriz(void* grafo_generico, int indice){
    GrafoMatriz* grafo = (GrafoMatriz*)grafo_generico;
    return grafo->dados_estacao[indice];
}

static int proximo_vizinho_matriz(void* grafo_generico, int u, CursorVizinhos* cursor){
    GrafoMatriz* grafo = (GrafoMatriz*)grafo_generico;

    while(cursor->coluna < grafo->qtd_estacoes){
        int v = cursor->coluna++;
        if(existe_aresta(grafo, u, v)) return v;
    }
    return -1;
}

static int iniciar_vizinhos_matriz(void* grafo_generico, int u, CursorVizinhos* cursor){
    cursor->coluna = 0;
    return proximo_vizinho_matriz(grafo_generico, u, cursor);
}

AcessoGrafo criar_acesso_matriz(GrafoMatriz* grafo){
    AcessoGrafo acesso = {
        .grafo = grafo,
        .qtd_vertices = grafo != NULL ? grafo->qtd_estacoes : 0,
        .obter_dados_estacao = obter_dados_estacao_matriz,
        .iniciar_vizinhos = iniciar_vizinhos_matriz,
        .proximo_vizinho = proximo_vizinho_matriz,
    };
    return acesso;
}
