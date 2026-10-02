# Compilador que será utilizado
CC = gcc

# Opções de compilação
# -Wall: ativa avisos importantes
# -Wextra: ativa avisos adicionais
# -std=c11: utiliza o padrão C11
CFLAGS = -Wall -Wextra -std=c11 -I include/ -lm

# Nome do arquivo executável que será gerado
TARGET = run.out

# Arquivos .c que serão compilados
SRC = src/*.c
LIBS = src/libs/*.c

LISTA_SPEC_TARGET = ./spec/grafo_lista/bin/run.out
LISTA_SPEC_MAIN = ./spec/grafo_lista/teste_grafo_lista.c

MATRIZ_SPEC_TARGET = ./spec/grafo_matriz/bin/run.out
MATRIZ_SPEC_MAIN = ./spec/grafo_matriz/teste_grafo_matriz.c

BICONEXO_MATRIZ_SPEC_TARGET = ./spec/grafo_biconexo_matriz/bin/run.out
BICONEXO_MATRIZ_SPEC_MAIN = ./spec/grafo_biconexo_matriz/teste_grafo_biconexo_matriz.c

BICONEXO_LISTA_SPEC_TARGET = ./spec/grafo_biconexo_lista/bin/run.out
BICONEXO_LISTA_SPEC_MAIN = ./spec/grafo_biconexo_lista/teste_grafo_biconexo_lista.c

PONTES_SPEC_TARGET = ./spec/pontes/bin/run.out
PONTES_SPEC_MAIN = ./spec/pontes/teste_pontes.c

# Alvo padrão
all:
	$(CC) $(CFLAGS) $(SRC) $(LIBS) -o $(TARGET)

# Roda o arquivo compilado
run: $(TARGET)
	./$(TARGET)

compile_testes_lista:
	$(CC) $(CFLAGS) $(LIBS) $(LISTA_SPEC_MAIN) -o $(LISTA_SPEC_TARGET)
    
run_testes_lista: $(LISTA_SPEC_TARGET)
	./$(LISTA_SPEC_TARGET)

compile_testes_matriz:
	$(CC) $(CFLAGS) $(LIBS) $(MATRIZ_SPEC_MAIN) -o $(MATRIZ_SPEC_TARGET)

run_testes_matriz: $(MATRIZ_SPEC_TARGET)
	./$(MATRIZ_SPEC_TARGET)

compile_testes_biconexo_matriz:
	$(CC) $(CFLAGS) $(LIBS) $(BICONEXO_MATRIZ_SPEC_MAIN) -o $(BICONEXO_MATRIZ_SPEC_TARGET)

run_testes_biconexo_matriz: $(BICONEXO_MATRIZ_SPEC_TARGET)
	./$(BICONEXO_MATRIZ_SPEC_TARGET)

compile_testes_biconexo_lista:
	$(CC) $(CFLAGS) $(LIBS) $(BICONEXO_LISTA_SPEC_MAIN) -o $(BICONEXO_LISTA_SPEC_TARGET)

run_testes_biconexo_lista: $(BICONEXO_LISTA_SPEC_TARGET)
	./$(BICONEXO_LISTA_SPEC_TARGET)

compile_testes_pontes:
	$(CC) $(CFLAGS) $(LIBS) $(PONTES_SPEC_MAIN) -o $(PONTES_SPEC_TARGET)

run_testes_pontes: $(PONTES_SPEC_TARGET)
	./$(PONTES_SPEC_TARGET)

compile_debug_lista:
	$(CC) -g $(CFLAGS) $(LIBS) $(LISTA_SPEC_MAIN) -o $(LISTA_SPEC_TARGET)

compile_debug_matriz:
	$(CC) -g $(CFLAGS) $(LIBS) $(MATRIZ_SPEC_MAIN) -o $(MATRIZ_SPEC_TARGET)

compile_debug_biconexo_matriz:
	$(CC) -g $(CFLAGS) $(LIBS) $(BICONEXO_MATRIZ_SPEC_MAIN) -o $(BICONEXO_MATRIZ_SPEC_TARGET)

compile_debug_biconexo_lista:
	$(CC) -g $(CFLAGS) $(LIBS) $(BICONEXO_LISTA_SPEC_MAIN) -o $(BICONEXO_LISTA_SPEC_TARGET)

# Remove os arquivos executáveis
clean:
	rm -f $(TARGET) $(LISTA_SPEC_TARGET) $(MATRIZ_SPEC_TARGET) $(BICONEXO_MATRIZ_SPEC_TARGET) $(BICONEXO_LISTA_SPEC_TARGET) $(PONTES_SPEC_TARGET)