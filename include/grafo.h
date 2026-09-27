#ifndef GRAFO_H
#define GRAFO_H

typedef struct DadosEstacao{
    int id;
    double longitude, latitude;
    double tempo_transferencia;
    char *code;
    char *nome;
}DadosEstacao;

typedef struct DadosAresta{
    char *source, *target;
    float distancia;
    float tempo_viagem;
} DadosAresta;

#endif