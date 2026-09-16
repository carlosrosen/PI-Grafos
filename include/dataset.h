#include "grafo_lista.h"

#ifndef DATASET_H
#define DATASET_H

#define BUFFER_SIZE 4096

typedef struct response_object_length{
    void **object;
    unsigned int length;
} ResponseObjectLength;

ResponseObjectLength* inicializa_stations();
ResponseObjectLength* Inicializar_edges(No** stations,int stations_length);
No* get_station(No** stations, int stations_length, char *code);
// int get_quantity_stations();

#endif