#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dijkstra.h"
#include "agm.h"
#include "grafo_lista.h"
#include "grafo_matriz.h"
#include "topologia.h"

#define TAMANHO_LINHA 32
#define TAMANHO_CODIGO 16

typedef enum {
    LISTA,
    MATRIZ
} Representacao;

typedef struct {
    Representacao representacao;
    GrafoLista *lista;
    GrafoMatriz *matriz;
} EstadoGrafos;

static const char *nome_representacao(Representacao representacao){
    return representacao == MATRIZ ? "Matriz de adjacencia" : "Lista de adjacencia";
}

static void limpar_tela(void){
#ifdef _WIN32
    system("cls");
#else
    printf("\033[2J\033[H");
    fflush(stdout);
#endif
}

static void limpar_entrada(void){
    int caractere;
    while ((caractere = getchar()) != '\n' && caractere != EOF){}
}

static int ler_opcao(int minimo, int maximo){
    char linha[TAMANHO_LINHA];
    char *fim;
    long valor;

    while (1){
        printf("\nEscolha uma opcao: ");
        fflush(stdout);

        if (fgets(linha, sizeof linha, stdin) == NULL){
            printf("\nEntrada encerrada.\n");
            return -1;
        }

        if (strchr(linha, '\n') == NULL && !feof(stdin)){
            limpar_entrada();
        } else{
            valor = strtol(linha, &fim, 10);

            if (fim != linha){
                while (*fim == ' ' || *fim == '\t' ||
                       *fim == '\r' || *fim == '\n'){
                    fim++;
                }

                if (*fim == '\0' && valor >= minimo && valor <= maximo){
                    return (int)valor;
                }
            }
        }

        printf("Opcao invalida. Digite um numero entre %d e %d.\n",minimo, maximo);
    }
}

// retorna -1 no EOF, 0 para codigo vazio ou longo demais e 1 em caso de sucesso.
static int ler_codigo(const char *mensagem, char *codigo, size_t tamanho){
    char linha[TAMANHO_LINHA];

    printf("%s", mensagem);
    fflush(stdout);

    if (fgets(linha, sizeof linha, stdin) == NULL){
        return -1;
    }

    if (strchr(linha, '\n') == NULL && !feof(stdin)){
        limpar_entrada();
    }

    linha[strcspn(linha, "\r\n")] = '\0';

    if (linha[0] == '\0' || strlen(linha) >= tamanho){
        return 0;
    }

    strcpy(codigo, linha);
    return 1;
}

static void pausar(void){
    printf("\nPressione ENTER para continuar...");
    fflush(stdout);
    limpar_entrada();
}

static void exibir_menu_principal(Representacao representacao){
    limpar_tela();
    printf("\n============== ANALISE DA MALHA FERROVIARIA DO REINO UNIDO ==============\n");
    printf("Representacao atual: %s\n\n", nome_representacao(representacao));
    printf("[1] Exportar estrutura do grafo\n");
    printf("[2] Trocar representacao de grafo\n");
    printf("[3] Exportar subgrafo de demonstracao\n");
    printf("[4] Parte 1 - Topologia da malha\n");
    printf("[5] Parte 2 - Otimizacao e expansao\n");
    printf("[0] Sair\n");
}

static void exibir_menu_topologia(Representacao representacao){
    limpar_tela();
    printf("\n============== PARTE 1 - TOPOLOGIA DA MALHA ==============\n");
    printf("Representacao atual: %s\n\n", nome_representacao(representacao));
    printf("[1] Trocar representacao de grafo\n");
    printf("[2] Exportar componentes biconexos, vertices de articulacao e pontes\n");
    printf("[3] Relatorio consolidado de pontos criticos\n");
    printf("[4] Simular remocao de uma ponte selecionada\n");
    printf("[0] Voltar ao menu principal\n");
}

static void exibir_menu_otimizacao(Representacao representacao){
    limpar_tela();
    printf("\n============== PARTE 2 - OTIMIZACAO E EXPANSAO ==============\n");
    printf("Representacao atual: %s\n\n", nome_representacao(representacao));
    printf("[1] Trocar representacao de grafo\n");
    printf("[2] Roteiro de melhor caminho entre estacoes\n");
    printf("[3] Planejar expansao da malha com AGM\n");
    printf("[0] Voltar ao menu principal\n");
}

static void trocar_representacao(Representacao *representacao){
    *representacao = *representacao == LISTA ? MATRIZ : LISTA;
}

static void liberar_grafos(EstadoGrafos *estado){
    if (estado->lista != NULL){
        Liberar_Grafo(estado->lista);
        estado->lista = NULL;
    }
    if (estado->matriz != NULL){
        liberar_grafo_matriz(&estado->matriz);
        estado->matriz = NULL;
    }
}

static int inicializar_grafos(EstadoGrafos *estado){
    estado->lista = inicializar_grafo_lista("./dataset/stations.csv", "./dataset/edges.csv");
    estado->matriz = inicializar_grafo_matriz("./dataset/stations.csv", "./dataset/edges.csv");
    if (estado->lista == NULL || estado->matriz == NULL){
        liberar_grafos(estado);
        return 0;
    }
    return 1;
}

static void calcular_rota(EstadoGrafos *estado){
    char codigo_origem[TAMANHO_CODIGO];
    char codigo_destino[TAMANHO_CODIGO];
    int status;

    status = ler_codigo("\nEstacao de origem: ",codigo_origem, sizeof codigo_origem);
    if (status < 0){
        return;
    }
    if (status == 0){
        printf("Codigo de origem invalido.\n");
        pausar();
        return;
    }

    status = ler_codigo("Estacao de destino: ", codigo_destino, sizeof codigo_destino);
    if (status < 0){
        return;
    }
    if (status == 0){
        printf("Codigo de destino invalido.\n");
        pausar();
        return;
    }

    if (estado->representacao == LISTA){
        status = imprimir_rota_por_codigo_lista(estado->lista, codigo_origem, codigo_destino);
    } else{
        status = imprimir_rota_por_codigo_matriz(estado->matriz, codigo_origem, codigo_destino);
    }

    if (status == 0){
        printf("\nEstacao de origem ou destino nao encontrada.\n");
    } else if (status < 0){
        printf("\nFalha ao calcular a rota.\n");
    }

    pausar();
}

static void planejar_expansao(EstadoGrafos *estado){
    ResultadoAGM lista, matriz;
    ResultadoAGM *resultado;
    const char *nome;

    int sucesso_lista = executar_agm_lista("./dataset/expansion_neighborhoods.csv",
                                           "./dataset/expansion_tunnels.csv", &lista);
    int sucesso_matriz = executar_agm_matriz("./dataset/expansion_neighborhoods.csv",
                                             "./dataset/expansion_tunnels.csv", &matriz);
    if (!sucesso_lista || !sucesso_matriz){
        printf("\nFalha ao carregar ou processar o grafo de expansao.\n");
        pausar();
        return;
    }

    if (estado->representacao == LISTA){
        resultado = &lista;
        nome = "lista de adjacencia";
    } else{
        resultado = &matriz;
        nome = "matriz de adjacencia";
    }
    exibir_resultado_agm(resultado, nome);
    int exportou_lista = exportar_agm(&lista, "output/agm_lista.txt",
                                      "output/agm_lista.dot", "lista de adjacencia");
    int exportou_matriz = exportar_agm(&matriz, "output/agm_matriz.txt",
                                       "output/agm_matriz.dot", "matriz de adjacencia");
    if (!exportou_lista || !exportou_matriz)
        printf("Aviso: nao foi possivel exportar todos os arquivos da AGM.\n");
    if (!resultados_agm_iguais(&lista, &matriz))
        printf("Aviso: as representacoes produziram resultados diferentes.\n");
    else
        printf("Resultado validado contra a outra representacao.\n");
    pausar();
}

static int menu_topologia(EstadoGrafos *estado){
    int opcao;

    do{
        exibir_menu_topologia(estado->representacao);
        opcao = ler_opcao(0, 4);

        if (opcao < 0){
            return -1;
        }

        switch (opcao){
            case 1:
                trocar_representacao(&estado->representacao);
                break;
            case 2:
                if (estado->representacao == LISTA){
                    exportar_topologia_lista(estado->lista);
                } else{
                    exportar_topologia_matriz(estado->matriz);
                }
                pausar();
                break;
            case 3:
                printf("\n[PLACEHOLDER] Relatorio consolidado de pontos criticos ");
                pausar();
                break;
            case 4:
                printf("\n[PLACEHOLDER] Simulacao de remocao de ponte ");
                pausar();
                break;
        }
    } while (opcao != 0);

    return 0;
}

static int menu_otimizacao(EstadoGrafos *estado){
    int opcao;

    do{
        exibir_menu_otimizacao(estado->representacao);
        opcao = ler_opcao(0, 3);

        if (opcao < 0){
            return -1;
        }

        switch (opcao){
            case 1:
                trocar_representacao(&estado->representacao);
                break;
            case 2:
                calcular_rota(estado);
                break;
            case 3:
                planejar_expansao(estado);
                break;
        }
    } while (opcao != 0);

    return 0;
}

int main(void){
    EstadoGrafos estado = {LISTA, NULL, NULL};
    int opcao;

    printf("Carregando malha ferroviaria...\n");
    fflush(stdout);

    if (!inicializar_grafos(&estado)){
        printf("Falha ao inicializar os grafos.\n");
        return 1;
    }

    do{
        exibir_menu_principal(estado.representacao);
        opcao = ler_opcao(0, 5);

        if (opcao < 0){
            break;
        }

        switch (opcao){
            case 1:
                if (estado.representacao == LISTA){
                    exportar_estrutura_lista(estado.lista);
                } else{
                    exportar_estrutura_matriz(estado.matriz);
                }
                pausar();
                break;
            case 2:
                trocar_representacao(&estado.representacao);
                break;
            case 3:
                printf("\n[PLACEHOLDER] Exportar subgrafo de demonstracao\n");
                pausar();
                break;
            case 4:
                if (menu_topologia(&estado) < 0){
                    opcao = 0;
                }
                break;
            case 5:
                if (menu_otimizacao(&estado) < 0){
                    opcao = 0;
                }
                break;
        }
    } while (opcao != 0);
    liberar_grafos(&estado);
    return 0;
}
