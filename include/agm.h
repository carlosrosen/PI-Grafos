#ifndef AGM_H
#define AGM_H

#include "kruskal.h"

int executar_agm_lista(const char *bairros_csv, const char *tuneis_csv,
                       ResultadoAGM *resultado);
int executar_agm_matriz(const char *bairros_csv, const char *tuneis_csv,
                        ResultadoAGM *resultado);
int exportar_agm(const ResultadoAGM *resultado, const char *txt_path,
                 const char *dot_path, const char *representacao);
void exibir_resultado_agm(const ResultadoAGM *resultado, const char *representacao);

#endif
