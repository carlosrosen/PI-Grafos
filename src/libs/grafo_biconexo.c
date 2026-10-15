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

// Estado de iteracao dos vizinhos de um vertice. Cada representacao usa o
// campo que lhe interessa: a lista avanca pela cadeia de nos e a matriz
// avanca pelas colunas da linha.
typedef struct CursorVizinhos{
    No *no;      // lista: proximo no da cadeia de adjacencia
    int coluna;  // matriz: proxima coluna a examinar
} CursorVizinhos;

// Interface comum que abstrai a representacao do grafo. O nucleo do
// algoritmo so conhece estes campos, preenchidos pelos adaptadores.
typedef struct AcessoGrafo{
    void *grafo;
    int qtd_vertices;
    const char *descricao;  // usada nos cabecalhos dos arquivos de saida

    DadosEstacao* (*obter_dados_estacao)(void *grafo, int indice);
    int (*iniciar_vizinhos)(void *grafo, int u, CursorVizinhos *cursor);
    int (*proximo_vizinho)(void *grafo, int u, CursorVizinhos *cursor);
} AcessoGrafo;


// ============================ Nucleo do algoritmo ============================


// Empilha a aresta (u,v) para uso posterior por registrar_componente,
// que vai desempilhar até essa aresta para formar um componente biconexo.
static void empilhar_aresta(EstadoDFS* estado, int u, int v){

    // Caso a pilha esteja cheia, ele dobra o tamanho do vetor antes de escrever
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
static void dfs_biconexo(const AcessoGrafo* acesso, EstadoDFS* estado, int u, int pai){

    // Marca u como visitado e registra seu tempo de descoberta.
    // low[u] começa igual a discovery[u]: por enquanto, u só alcança a si mesmo.
    estado->visitado[u] = 1;
    estado->discovery[u] = estado->low[u] = estado->tempo++;
    int filhos = 0; // quantos filhos u tem na árvore da DFS (importa para a raiz)

    CursorVizinhos cursor;
    for(int v = acesso->iniciar_vizinhos(acesso->grafo, u, &cursor); v != -1;
        v = acesso->proximo_vizinho(acesso->grafo, u, &cursor)){

        // Pula: o próprio u e quem mandou a DFS pra cá (pai)
        if(v == u || v == pai) continue;

        if(!estado->visitado[v]){ // v ainda não foi visitado: é um filho novo de u na árvore da DFS

            filhos++;

            // guarda a aresta (u,v) para um possível componente
            empilhar_aresta(estado, u, v);
            // desce recursivamente por v
            dfs_biconexo(acesso, estado, v, u);


            // Ao voltar de v, atualiza low[u]: se v alcança algo mais "antigo"
            // que o que u já alcançava, u herda esse alcance
            if(estado->low[v] < estado->low[u])
                estado->low[u] = estado->low[v];

            // Condições clássicas de vértice de articulação:
            // u é raiz e tem 2+ filhos na árvore, OU
            // u não é raiz e o filho v não consegue alcançar acima de u
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


// Roda a DFS a partir de todos os vértices ainda não visitados e consolida
// articulações e componentes biconexos em um ResultadoBiconexo.
static ResultadoBiconexo* executar_biconexos(const AcessoGrafo* acesso){
    int n = acesso->qtd_vertices;

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
            dfs_biconexo(acesso, &estado, i, -1);

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


// ====================== Adaptadores: lista de adjacencia ======================


static DadosEstacao* obter_dados_estacao_lista(void* grafo_generico, int indice){
    GrafoLista* grafo = (GrafoLista*)grafo_generico;
    return grafo->lista[indice]->dados;
}

static int proximo_vizinho_lista(void* grafo_generico, int u, CursorVizinhos* cursor){
    (void)grafo_generico;
    (void)u;

    if(cursor->no == NULL) return -1;

    // Converte o identificador base-1 do nó para índice base-0
    int v = cursor->no->vertice - 1;
    cursor->no = cursor->no->proximo;
    return v;
}

static int iniciar_vizinhos_lista(void* grafo_generico, int u, CursorVizinhos* cursor){
    GrafoLista* grafo = (GrafoLista*)grafo_generico;

    // O nó cabeça representa a própria estação; a adjacência começa no próximo
    cursor->no = grafo->lista[u] != NULL ? grafo->lista[u]->proximo : NULL;
    return proximo_vizinho_lista(grafo_generico, u, cursor);
}

static AcessoGrafo criar_acesso_lista(GrafoLista* grafo){
    AcessoGrafo acesso = {
        .grafo = grafo,
        .qtd_vertices = grafo != NULL ? (int)grafo->qtd_no : 0,
        .descricao = "Lista de adjacencia",
        .obter_dados_estacao = obter_dados_estacao_lista,
        .iniciar_vizinhos = iniciar_vizinhos_lista,
        .proximo_vizinho = proximo_vizinho_lista,
    };
    return acesso;
}


// ====================== Adaptadores: matriz de adjacencia ======================


// Existe ligação entre i e j em qualquer sentido? (grafo tratado como não dirigido)
static int existe_aresta(GrafoMatriz* grafo, int i, int j){
    return grafo->matriz[i][j] || grafo->matriz[j][i];
}

static DadosEstacao* obter_dados_estacao_matriz(void* grafo_generico, int indice){
    GrafoMatriz* grafo = (GrafoMatriz*)grafo_generico;
    return grafo->dados_estacao[indice];
}

static int proximo_vizinho_matriz(void* grafo_generico, int u, CursorVizinhos* cursor){
    GrafoMatriz* grafo = (GrafoMatriz*)grafo_generico;

    while(cursor->coluna < grafo->qtd_estacoes){
        int v = cursor->coluna++;
        if(existe_aresta(grafo, u, v)) return v;
    }
    return -1;
}

static int iniciar_vizinhos_matriz(void* grafo_generico, int u, CursorVizinhos* cursor){
    cursor->coluna = 0;
    return proximo_vizinho_matriz(grafo_generico, u, cursor);
}

static AcessoGrafo criar_acesso_matriz(GrafoMatriz* grafo){
    AcessoGrafo acesso = {
        .grafo = grafo,
        .qtd_vertices = grafo != NULL ? grafo->qtd_estacoes : 0,
        .descricao = "Matriz de adjacencia",
        .obter_dados_estacao = obter_dados_estacao_matriz,
        .iniciar_vizinhos = iniciar_vizinhos_matriz,
        .proximo_vizinho = proximo_vizinho_matriz,
    };
    return acesso;
}


// ============================ Entradas públicas ============================


ResultadoBiconexo* encontrar_biconexos_lista(GrafoLista* grafo){
    if(!grafo) return NULL;
    AcessoGrafo acesso = criar_acesso_lista(grafo);
    return executar_biconexos(&acesso);
}

ResultadoBiconexo* encontrar_biconexos(GrafoMatriz* grafo){
    if(!grafo) return NULL;
    AcessoGrafo acesso = criar_acesso_matriz(grafo);
    return executar_biconexos(&acesso);
}


// ============================== Exportação ==============================


static void exportar_componentes_biconexos(const AcessoGrafo* acesso, ResultadoBiconexo* resultado, char* output_path){
    if(!acesso || !acesso->grafo || !resultado || !output_path) return;

    FILE* fp = fopen(output_path, "w");

    if(!fp){
        perror("Falha ao abrir arquivo de saida dos componentes biconexos");
        return;
    }

    fprintf(fp, "========= Componentes Biconexos (%s) ============\n", acesso->descricao);
    fprintf(fp, "Componentes biconexos encontrados: %d\n\n", resultado->qtd_componentes);

    for(int c = 0; c < resultado->qtd_componentes; c++){
        fprintf(fp, "Componente %d:\n", c + 1);
        for(int e = 0; e < resultado->componentes[c].qtd_arestas; e++){
            int u = resultado->componentes[c].arestas[e].u;
            int v = resultado->componentes[c].arestas[e].v;
            DadosEstacao* du = acesso->obter_dados_estacao(acesso->grafo, u);
            DadosEstacao* dv = acesso->obter_dados_estacao(acesso->grafo, v);
            fprintf(fp, "[%d <-> %d] (%s) %s <-> (%s) %s\n", u, v, du->code, du->nome, dv->code, dv->nome);
        }
        fprintf(fp, "\n");
    }

    fclose(fp);
}

static void exportar_vertices_articulacao(const AcessoGrafo* acesso, ResultadoBiconexo* resultado, char* output_path){
    if(!acesso || !acesso->grafo || !resultado || !output_path) return;

    FILE* fp = fopen(output_path, "w");

    if(!fp){
        perror("Falha ao abrir arquivo de saida dos vertices de articulacao");
        return;
    }

    fprintf(fp, "========= Vertices de articulacao (%s) ============\n", acesso->descricao);
    fprintf(fp, "Vertices de articulacao encontrados: %d\n\n", resultado->qtd_articulacoes);

    for(int i = 0; i < acesso->qtd_vertices; i++){
        if(resultado->articulacoes[i]){
            DadosEstacao* d = acesso->obter_dados_estacao(acesso->grafo, i);
            fprintf(fp, "[%d] (%s) %s\n", i, d->code, d->nome);
        }
    }

    fclose(fp);
}

void exportar_componentes_biconexos_lista(GrafoLista* grafo, ResultadoBiconexo* resultado, char* output_path){
    AcessoGrafo acesso = criar_acesso_lista(grafo);
    exportar_componentes_biconexos(&acesso, resultado, output_path);
}

void exportar_vertices_articulacao_lista(GrafoLista* grafo, ResultadoBiconexo* resultado, char* output_path){
    AcessoGrafo acesso = criar_acesso_lista(grafo);
    exportar_vertices_articulacao(&acesso, resultado, output_path);
}

void exportar_componentes_biconexos_matriz(GrafoMatriz* grafo, ResultadoBiconexo* resultado, char* output_path){
    AcessoGrafo acesso = criar_acesso_matriz(grafo);
    exportar_componentes_biconexos(&acesso, resultado, output_path);
}

void exportar_vertices_articulacao_matriz(GrafoMatriz* grafo, ResultadoBiconexo* resultado, char* output_path){
    AcessoGrafo acesso = criar_acesso_matriz(grafo);
    exportar_vertices_articulacao(&acesso, resultado, output_path);
}


// ============================== Liberação ==============================


// A estrutura de ResultadoBiconexo é a mesma para as duas representações, então
// a liberação é compartilhada.
void liberar_resultado_biconexo(ResultadoBiconexo** resultado){
    if(!resultado || !*resultado) return;

    for(int c = 0; c < (*resultado)->qtd_componentes; c++)
        free((*resultado)->componentes[c].arestas);

    free((*resultado)->componentes);
    free((*resultado)->articulacoes);
    free(*resultado);
    *resultado = NULL;
}

void liberar_resultado_biconexo_lista(ResultadoBiconexo** resultado){
    liberar_resultado_biconexo(resultado);
}
