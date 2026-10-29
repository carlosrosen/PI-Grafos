## Peso das arestas

Cada trecho recebe um peso em unidades de tempo, calculado na inicialização do grafo:

```text
peso = tempo_viagem + tempo_transferencia da estação de destino
```

* `tempo_viagem`: coluna `travel_time` de `dataset/edges.csv` (a distância não entra na fórmula, o peso é expresso em tempo).
* `tempo_transferencia`: coluna `transfer_time` de `dataset/stations.csv`, da estação de destino do trecho.
* Fórmula centralizada em `calcular_peso_aresta` (`src/libs/aux_functions.c`).
* Lista de adjacência: `Aresta.dados->peso` (`DadosAresta.peso`).
* Matriz de adjacência: `GrafoMatriz.matriz[origem][destino]`, onde `0` indica ausência de aresta.

Cada linha de `edges.csv` gera as duas orientações do trecho, e cada orientação usa o tempo de baldeação da sua estação de destino.

Exemplo reproduzível: para `AAP,BOP,1.0,0.75` em `edges.csv` e `transfer_time = 9` para `BOP` em `stations.csv`, o peso de `AAP -> BOP` é `0.75 + 9 = 9.75`.

## Planejamento da expansão com AGM

A opção `[3] Planejar expansao da malha com AGM`, dentro da Parte 2 do menu,
carrega os 10 bairros de `dataset/expansion_neighborhoods.csv` e as 25 conexões
candidatas de `dataset/expansion_tunnels.csv`. A malha existente é representada
pelo supernó `NETWORK`; as conexões com ele exibem as âncoras `CRH`, `NDL` ou
`DNM` informadas no dataset.

O algoritmo de Kruskal usa `cost_billions_brl` como peso, union-find com
compressão de caminho e união por rank. Empates são resolvidos pelo custo e,
depois, pelos códigos de origem e destino. O grafo de expansão é independente
do grafo operacional e pode ser executado como lista ou matriz de adjacência.
O carregamento e as saídas da expansão ficam em `src/libs/agm.c`, enquanto o
algoritmo e o union-find ficam isolados em `src/libs/kruskal.c`, com a interface
pública declarada em `include/kruskal.h`.

Ao executar a opção, o programa mostra cada túnel selecionado e os totais em
bilhões de reais e libras. Também valida o resultado contra a outra
representação e grava os arquivos correspondentes:

- `output/agm_lista.txt` e `output/agm_lista.dot`;
- `output/agm_matriz.txt` e `output/agm_matriz.dot`.

Para reproduzir automaticamente a validação das duas representações:

```bash
make compile_testes_agm run_testes_agm
```

## Compilação e execução

O projeto utiliza um `Makefile` para facilitar a compilação dos arquivos escritos em C.

### Adicionando novos arquivos

Sempre que um novo arquivo `.c` for criado dentro da pasta `src/`, ele deve ser adicionado à variável `SRC` do `Makefile`.

Por exemplo, inicialmente:

```makefile
SRC = src/main.c
```

Ao criar o arquivo `src/grafo.c`, altere para:

```makefile
SRC = src/main.c \
      src/grafo.c
```

Ao criar outros arquivos, continue adicionando-os:

```makefile
SRC = src/main.c \
      src/grafo.c \
      src/parser.c \
      src/busca.c
```

Os arquivos `.h` não precisam ser adicionados ao `SRC`, pois são incluídos pelos arquivos `.c` através de `#include`.

### Compilando o projeto

Na pasta raiz do projeto, execute:

```bash
make
```

O `Makefile` irá compilar os arquivos definidos em `SRC` e gerar o executável definido em `TARGET`.

Atualmente:

```makefile
TARGET = build
```

Portanto, o executável gerado será:

```text
build
```

### Executando o programa

Após a compilação, execute:

```bash
./build
```

### Compilando e executando de uma vez

Também é possível compilar e executar o programa em um único comando:

```bash
make && ./build
```

O operador `&&` faz com que `./build` só seja executado caso o `make` termine com sucesso.

Isso é útil durante o desenvolvimento, pois permite verificar rapidamente se as alterações compilam e se o programa funciona:

```bash
make && ./build
```

### Fluxo básico de desenvolvimento

Sempre que um novo arquivo `.c` for criado:

1. Criar o arquivo dentro de `src/`.
2. Adicionar o arquivo à variável `SRC` no `Makefile`.
3. Salvar as alterações.
4. Compilar com:

```bash
make
```

5. Executar com:

```bash
./build
```

Ou, para fazer os dois passos de uma vez:

```bash
make && ./build
```

### Limpando a compilação

Para remover o executável gerado:

```bash
make clean
```

Depois, basta executar `make` novamente para realizar uma nova compilação.

## Execução com Docker

O projeto utiliza Docker para padronizar o ambiente de compilação e execução. Dessa forma, não é necessário instalar manualmente GCC, Make ou GDB no sistema operacional.

### Requisitos

Antes de executar o projeto, instale:

* [Git](https://git-scm.com/)
* [Docker Desktop](https://www.docker.com/products/docker-desktop/)

No Windows, o Docker Desktop deve estar aberto e em execução.

Para verificar se o Docker está funcionando:

```bash
docker --version
```

Também é recomendado executar:

```bash
docker run hello-world
```

Caso a mensagem `Hello from Docker!` seja exibida, o Docker está funcionando corretamente.

### Clonando o projeto

Clone o repositório utilizando:

```bash
git clone <URL_DO_REPOSITORIO>
```

Entre na pasta do projeto:

```bash
cd PI-Grafos
```

### Construindo a imagem Docker

Na pasta raiz do projeto, onde está localizado o arquivo `Dockerfile`, execute:

```bash
docker build -t pi-grafos .
```

Esse comando cria uma imagem Docker chamada `pi-grafos` contendo o ambiente necessário para compilar e executar o projeto.

A imagem utiliza Ubuntu 26.04 e instala automaticamente:

* GCC
* Make
* GDB

### Executando o container

Após construir a imagem, execute:

```bash
docker run -it pi-grafos
```

Isso iniciará um container baseado na imagem criada e abrirá um terminal dentro do ambiente do projeto.

### Compilando o projeto

Dentro do container, execute:

```bash
make
```

O `Makefile` será utilizado para compilar o código-fonte.

### Executando o programa

Após a compilação:

```bash
./build
```

O programa deverá apresentar:

```text
Projeto Malha Metroviaria
```

### Encerrando o container

Para sair do container:

```bash
exit
```

### Fluxo resumido

```bash
# Clonar o projeto
git clone <URL_DO_REPOSITORIO>

# Entrar na pasta
cd PI-Grafos

# Construir a imagem
docker build -t pi-grafos .

# Criar e iniciar o container
docker run -it pi-grafos

# Dentro do container: compilar
make

# Executar
./build
```

> **Observação:** o Dockerfile fornece o ambiente de compilação do projeto. Portanto, no sistema operacional do usuário não é necessário instalar GCC, Make ou GDB separadamente.
