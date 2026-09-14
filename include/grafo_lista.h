#ifndef GRAFO_LISTA
#define GRAFO_LISTA

#define ARESTA_DEFAULT_INCREASE 5

typedef struct Aresta Aresta;

typedef struct No{
    double longitude, latitude;
    char *code;
    char *nome;
    unsigned int qtd_alloc_aresta;
    struct Aresta** proximos;
} No;

typedef struct Aresta{
    float distance;
    No *source;
    No *target;
}Aresta;

typedef struct grafo{
    int qtd_no;
    No **lista;
}GrafoLista;

GrafoLista *inicializar_grafo_l();

void _Insert_Aresta(Aresta** no_proximos, Aresta* aresta);

#endif