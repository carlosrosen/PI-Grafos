# Compilador que será utilizado
CC = gcc

# Opções de compilação
# -Wall: ativa avisos importantes
# -Wextra: ativa avisos adicionais
# -std=c11: utiliza o padrão C11
CFLAGS = -Wall -Wextra -std=c11 -I include/

# Nome do arquivo executável que será gerado
TARGET = run.out

# Arquivos .c que serão compilados
SRC = src/*.c
LIBS = src/libs/*.c


SPEC = spec/*.c
SPEC_TARGET = spec/run.out

# Alvo padrão
# Executado quando usamos apenas: make
all:
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

# Roda o arquivo compilado
run: $(TARGET)
	./$(TARGET)

compile_tests:
	$(CC) $(CFLAGS) $(LIBS) $(SPEC) -o $(SPEC_TARGET)
	
run_tests: $(SPEC_TARGET)
	./$(SPEC_TARGET)
	
compile_debug:
	$(CC) -g $(CFLAGS) $(LIBS) $(SPEC) -o $(SPEC_TARGET)

# Remove o arquivo executável
# Executado com: make clean
clean:
	rm -f $(TARGET)