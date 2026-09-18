#ifndef GRAFO_LISTA
#define GRAFO_LISTA

#define ARESTA_DEFAULT_INCREASE 5

typedef struct Aresta Aresta;

typedef struct Dados{
    double longitude, latitude;
    char *code;
    char *nome;
}Dados;

typedef struct No{
    int vertice;
    Dados *dados;
    struct No* proximo;
} No;

typedef struct Aresta{
    float distance;
    No *source;
    No *target;
}Aresta;

typedef struct grafo{
    unsigned int qtd_no;
    unsigned int qtd_arestas;
    Aresta** arestas;
    No **lista;
}GrafoLista;

GrafoLista *inicializar_grafo_l();

void Liberar_Grafo(GrafoLista* grafo);

#endif