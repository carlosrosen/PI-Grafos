#include "grafo_biconexo.h"
#include <stdio.h>
#include <stdlib.h>

// Estado compartilhado entre as chamadas recursivas da DFS
typedef struct EstadoDFS{
    int *visitado, *discovery, *low, *articulacao;
    int tempo;

    ArestaSimples *pilha;
    int topo_pilha, capacidade_pilha;

    ComponenteBiconexo *componentes;
    int qtd_componentes, capacidade_componentes;
} EstadoDFS;

// Existe ligação entre i e j em qualquer sentido? (grafo tratado como não dirigido)
static int existe_aresta(GrafoMatriz* grafo, int i, int j){
    return grafo->matriz[i][j] || grafo->matriz[j][i];
}


// Empilha a aresta (u,v) para uso posterior por registrar_componente,
// que vai desempilhar até essa aresta para formar um componente biconexo.
static void empilhar_aresta(EstadoDFS* estado, int u, int v){

    // Pilha cheia? Dobra o tamanho do vetor antes de escrever
    if(estado->topo_pilha >= estado->capacidade_pilha){
        estado->capacidade_pilha *= 2;
        estado->pilha = realloc(estado->pilha, sizeof(ArestaSimples) * estado->capacidade_pilha);
    }

    // Escreve a aresta na próxima posição livre (topo_pilha)
    estado->pilha[estado->topo_pilha].u = u;
    estado->pilha[estado->topo_pilha].v = v;

    // Avança o topo, já que essa posição foi ocupada
    estado->topo_pilha++;
}


// Desempilha arestas até (e incluindo) a aresta (u,v), e guarda esse grupo
// como um novo componente biconexo. É chamada quando a DFS detecta que
// low[v] >= discovery[u], ou seja, que (u,v) fecha um bloco do grafo.
static void registrar_componente(EstadoDFS* estado, int u, int v){
    ArestaSimples *temp = malloc(sizeof(ArestaSimples) * estado->topo_pilha);
    int n = 0;

    // Desempilha uma aresta por vez e guarda em temp, até achar (u,v)
    while(estado->topo_pilha > 0){
        ArestaSimples e = estado->pilha[--estado->topo_pilha];
        temp[n++] = e;

        // Achou a aresta (u,v), em qualquer ordem? Para por aqui:
        // esse é o limite do componente

        if((e.u == u && e.v == v) || (e.u == v && e.v == u)) break;
    }

     // Vetor de componentes cheio? Dobra a capacidade antes de adicionar

    if(estado->qtd_componentes >= estado->capacidade_componentes){
        estado->capacidade_componentes *= 2;
        estado->componentes = realloc(estado->componentes, sizeof(ComponenteBiconexo) * estado->capacidade_componentes);
    }
    
     // Encolhe temp para o tamanho real usado (n) e salva como um novo componente

    estado->componentes[estado->qtd_componentes].arestas = realloc(temp, sizeof(ArestaSimples) * n);
    estado->componentes[estado->qtd_componentes].qtd_arestas = n;
    estado->qtd_componentes++;
}


// DFS com low-link (algoritmo de Tarjan) para achar vértices de articulação
// e componentes biconexos. u = vértice atual sendo visitado; pai = de onde
// a DFS veio (-1 se u for a raiz da árvore de busca).
static void dfs_biconexo(GrafoMatriz* grafo, EstadoDFS* estado, int u, int pai){

    // Marca u como visitado e registra seu tempo de descoberta.
    // low[u] começa igual a discovery[u]: por enquanto, u só alcança a si mesmo.
    estado->visitado[u] = 1;
    estado->discovery[u] = estado->low[u] = estado->tempo++;
    int filhos = 0; // quantos filhos u tem na árvore da DFS (importa para a raiz)

    for(int v = 0; v < grafo->qtd_estacoes; v++){ // Percorre todos os possíveis vizinhos de u

        // Pula: o próprio u, quem mandou a DFS pra cá (pai), e quem não é vizinho

        if(v == u || v == pai || !existe_aresta(grafo, u, v)) continue;

        if(!estado->visitado[v]){ // v ainda não foi visitado: é um filho novo de u na árvore da DFS
            
            filhos++;

            // guarda a aresta (u,v) para um possível componente
            empilhar_aresta(estado, u, v);
            // desce recursivamente por v
            dfs_biconexo(grafo, estado, v, u);


            // Ao voltar de v, atualiza low[u]: se v alcança algo mais "antigo"
            // que o que u já alcançava, u herda esse alcance
            if(estado->low[v] < estado->low[u])
                estado->low[u] = estado->low[v];

            // Condições clássicas de vértice de articulação:
            // (1) u é raiz e tem 2+ filhos na árvore, OU
            // (2) u não é raiz e o filho v não consegue alcançar acima de u
            if((pai == -1 && filhos > 1) ||
               (pai != -1 && estado->low[v] >= estado->discovery[u])){
                estado->articulacao[u] = 1;
            }

             // Se v não alcança acima de u, o ramo de v não depende de mais
            // nada além de u: fecha um componente biconexo aqui
            if(estado->low[v] >= estado->discovery[u]){
                registrar_componente(estado, u, v);
            }

        }else if(estado->discovery[v] < estado->discovery[u]){
             /* v já foi visitado e é mais "antigo" que u: é uma aresta de retorno
             (liga u de volta a um ancestral seu na árvore da DFS)*/
            empilhar_aresta(estado, u, v);
            if(estado->discovery[v] < estado->low[u])
                estado->low[u] = estado->discovery[v];
        }
    }
}

ResultadoBiconexo* encontrar_biconexos(GrafoMatriz* grafo){
    if(!grafo) return NULL;
    int n = grafo->qtd_estacoes;

    EstadoDFS estado = {0};
    estado.visitado    = calloc(n, sizeof(int));
    estado.discovery   = calloc(n, sizeof(int));
    estado.low         = calloc(n, sizeof(int));
    estado.articulacao = calloc(n, sizeof(int));

    estado.capacidade_pilha = 64;
    estado.pilha = malloc(sizeof(ArestaSimples) * estado.capacidade_pilha);

    estado.capacidade_componentes = 16;
    estado.componentes = malloc(sizeof(ComponenteBiconexo) * estado.capacidade_componentes);

    for(int i = 0; i < n; i++){
        if(!estado.visitado[i]){
            dfs_biconexo(grafo, &estado, i, -1);

            if(estado.topo_pilha > 0){
                ArestaSimples primeira = estado.pilha[0];
                ArestaSimples ultima = estado.pilha[estado.topo_pilha - 1];
                registrar_componente(&estado, primeira.u, ultima.v);
            }
        }
    }

    ResultadoBiconexo* resultado = malloc(sizeof(ResultadoBiconexo));
    resultado->articulacoes = estado.articulacao;
    resultado->qtd_articulacoes = 0;
    for(int i = 0; i < n; i++)
        if(estado.articulacao[i]) resultado->qtd_articulacoes++;

    resultado->componentes = estado.componentes;
    resultado->qtd_componentes = estado.qtd_componentes;

    free(estado.visitado);
    free(estado.discovery);
    free(estado.low);
    free(estado.pilha);

    return resultado;
}

void imprimir_resultado_biconexo(GrafoMatriz* grafo, ResultadoBiconexo* resultado){
    if(!resultado) return;

    printf("Vértices de articulação:\n");
    for(int i = 0; i < grafo->qtd_estacoes; i++)
        if(resultado->articulacoes[i])
            printf("  %s\n", grafo->dados_estacao[i]->code);

    printf("\nComponentes biconexos (%d):\n", resultado->qtd_componentes);
    for(int c = 0; c < resultado->qtd_componentes; c++){
        printf("  Componente %d:", c + 1);
        for(int e = 0; e < resultado->componentes[c].qtd_arestas; e++){
            int u = resultado->componentes[c].arestas[e].u;
            int v = resultado->componentes[c].arestas[e].v;
            printf(" (%s-%s)", grafo->dados_estacao[u]->code, grafo->dados_estacao[v]->code);
        }
        printf("\n");
    }
}

void liberar_resultado_biconexo(ResultadoBiconexo** resultado){
    if(!resultado || !*resultado) return;

    for(int c = 0; c < (*resultado)->qtd_componentes; c++)
        free((*resultado)->componentes[c].arestas);

    free((*resultado)->componentes);
    free((*resultado)->articulacoes);
    free(*resultado);
    *resultado = NULL;
}