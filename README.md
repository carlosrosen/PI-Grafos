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