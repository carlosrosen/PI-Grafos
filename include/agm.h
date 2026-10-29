#ifndef AGM_H
#define AGM_H

#define AGM_MAX_VERTICES 11
#define AGM_MAX_ARESTAS 25
#define AGM_TAMANHO_CODIGO 16

typedef struct ArestaAGM {
    int origem;
    int destino;
    char codigo_origem[AGM_TAMANHO_CODIGO];
    char codigo_destino[AGM_TAMANHO_CODIGO];
    char ancora[AGM_TAMANHO_CODIGO];
    double custo_brl;
    double custo_gbp;
} ArestaAGM;

typedef struct ResultadoAGM {
    ArestaAGM arestas[AGM_MAX_VERTICES - 1];
    int qtd_vertices;
    int qtd_arestas_candidatas;
    int qtd_arestas;
    double custo_total_brl;
    double custo_total_gbp;
    int conectado;
} ResultadoAGM;

int executar_agm_lista(const char *bairros_csv, const char *tuneis_csv,
                       ResultadoAGM *resultado);
int executar_agm_matriz(const char *bairros_csv, const char *tuneis_csv,
                        ResultadoAGM *resultado);
int exportar_agm(const ResultadoAGM *resultado, const char *txt_path,
                 const char *dot_path, const char *representacao);
int resultados_agm_iguais(const ResultadoAGM *a, const ResultadoAGM *b);
void exibir_resultado_agm(const ResultadoAGM *resultado, const char *representacao);

#endif
