#include "grafo_lista.h"

#ifndef DATASET_H
#define DATASET_H

#define BUFFER_SIZE 4096

typedef struct response_object_length{
    void **object;
    unsigned int length;
} ResponseObjectLength;

No** get_stations();
Aresta** get_edges();
// int get_quantity_stations();

#endif