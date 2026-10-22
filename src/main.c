#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAMANHO_LINHA 32

typedef enum {
    LISTA,
    MATRIZ
} Representacao;

static const char *nome_representacao(Representacao representacao) {
    return representacao == MATRIZ
        ? "Matriz de adjacencia"
        : "Lista de adjacencia";
}

static void limpar_entrada(void) {
    int caractere;
    while ((caractere = getchar()) != '\n' && caractere != EOF) {
    }
}

static int ler_opcao(int minimo, int maximo) {
    char linha[TAMANHO_LINHA];
    char *fim;
    long valor;

    while (1) {
        printf("\nEscolha uma opcao: ");
        fflush(stdout);

        if (fgets(linha, sizeof linha, stdin) == NULL) {
            printf("\nEntrada encerrada.\n");
            return -1;
        }

        if (strchr(linha, '\n') == NULL && !feof(stdin)) {
            limpar_entrada();
        } else {
            valor = strtol(linha, &fim, 10);

            if (fim != linha) {
                while (*fim == ' ' || *fim == '\t' ||
                       *fim == '\r' || *fim == '\n') {
                    fim++;
                }

                if (*fim == '\0' && valor >= minimo && valor <= maximo) {
                    return (int)valor;
                }
            }
        }

        printf("Opcao inválida. Digite um número entre %d e %d.\n",
               minimo, maximo);
    }
}

static void pausar() {
    printf("\nPressione ENTER para continuar...");
    fflush(stdout);
    limpar_entrada();
}

static void exibir_menu_principal(Representacao representacao) {
    printf("\n============== ANALISE DA MALHA FERROVIARIA DO REINO UNIDO ==============\n");
    printf("Representacao atual: %s\n\n", nome_representacao(representacao));
    printf("[1] Exportar estrutura do grafo\n");
    printf("[2] Trocar representacao de grafo\n");
    printf("[3] Exportar subgrafo de demonstracao\n");
    printf("[4] Parte 1 - Topologia da malha\n");
    printf("[5] Parte 2 - Otimizacao e expansao\n");
    printf("[0] Sair\n");
}

static void trocar_representacao(Representacao *representacao) {
    *representacao = *representacao == LISTA ? MATRIZ : LISTA;
}

static void exibir_menu_topologia(Representacao representacao) {
    printf("\n============== PARTE 1 - TOPOLOGIA DA MALHA ==============\n");
    printf("Representacao atual: %s\n\n", nome_representacao(representacao));
    printf("[1] Trocar representacao de grafo\n");
    printf("[2] Exportar componentes biconexos, vertices de articulacao e pontes\n");
    printf("[3] Relatorio consolidado de pontos criticos\n");
    printf("[4] Simular remocao de uma ponte selecionada\n");
    printf("[0] Voltar ao menu principal\n");
}

static int menu_topologia(Representacao *representacao) {
    int opcao;

    do {
        exibir_menu_topologia(*representacao);
        opcao = ler_opcao(0, 4);

        if (opcao < 0) {
            return -1;
        }

        switch (opcao) {
            case 1:
                trocar_representacao(representacao);
                break;
            case 2:
                printf("\n[PLACEHOLDER] Exportar componentes biconexos, "
                       "vertices de articulacao e pontes\n");
                pausar();
                break;
            case 3:
                printf("\n[PLACEHOLDER] Relatorio consolidado de pontos criticos\n");
                pausar();
                break;
            case 4:
                printf("\n[PLACEHOLDER] Simular remocao de uma ponte selecionada\n");
                pausar();
                break;
        }
    } while (opcao != 0);

    return 0;
}

static void exibir_menu_otimizacao(Representacao representacao) {
    printf("\n============== PARTE 2 - OTIMIZACAO E EXPANSAO ==============\n");
    printf("Representacao atual: %s\n\n", nome_representacao(representacao));
    printf("[1] Trocar representacao de grafo\n");
    printf("[2] Roteiro de melhor caminho entre estacoes\n");
    printf("[3] Planejar expansao da malha com AGM\n");
    printf("[0] Voltar ao menu principal\n");
}

static int menu_otimizacao(Representacao *representacao) {
    int opcao;

    do {
        exibir_menu_otimizacao(*representacao);
        opcao = ler_opcao(0, 3);

        if (opcao < 0) {
            return -1;
        }

        switch (opcao) {
            case 1:
                trocar_representacao(representacao);
                break;
            case 2:
                printf("\n[PLACEHOLDER] Roteiro de melhor caminho entre estacões\n");
                pausar();
                break;
            case 3:
                printf("\n[PLACEHOLDER] Planejar expansao da malha com AGM\n");
                pausar();
                break;
        }
    } while (opcao != 0);

    return 0;
}

int main(void) {
    Representacao representacao = LISTA;
    int opcao;

    do {
        exibir_menu_principal(representacao);
        opcao = ler_opcao(0, 5);

        if (opcao < 0) {
            return 0;
        }

        switch (opcao) {
            case 1:
                printf("\n[PLACEHOLDER] Exportar estrutura do grafo\n");
                pausar();
                break;
            case 2:
                trocar_representacao(&representacao);
                break;
            case 3:
                printf("\n[PLACEHOLDER] Exportar subgrafo de demonstracao ");
                pausar();
                break;
            case 4:
                if (menu_topologia(&representacao) < 0) {
                    return 0;
                }
                break;
            case 5:
                if (menu_otimizacao(&representacao) < 0) {
                    return 0;
                }
                break;
        }
    } while (opcao != 0);

    return 0;
}
