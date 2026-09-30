#ifndef GRAFO_H
#define GRAFO_H

//Estrutura de dados que representa um vértice do grafo(uma estação)

typedef struct DadosEstacao{
    //Identificador da estação
    int id;
    //Coordenadas geográficas
    double longitude, latitude;
    //Tempo gasto para fazer baldeação na estação
    double tempo_transferencia;
    //Código da estação
    char *code;
    char *nome;
}DadosEstacao;

//Estrutura de dados que representa uma aresta dirigida do grafo(uma ligação entre 2 estações)

typedef struct DadosAresta{
    //Índices das estações de origem e destino no vetor de estações.
    int id_source, id_target;
    //Código da estação de origem e destino de uma aresta.
    char *source, *target;
    //Distância entre estações
    float distancia;
    //Tempo de viagem entre elas
    float tempo_viagem;
} DadosAresta;

#endif