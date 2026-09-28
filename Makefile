# Compilador que será utilizado
CC = gcc

# Opções de compilação
# -Wall: ativa avisos importantes
# -Wextra: ativa avisos adicionais
# -std=c11: utiliza o padrão C11
CFLAGS = -Wall -Wextra -std=c11

# Nome do arquivo executável que será gerado
TARGET = build

# Arquivos .c que serão compilados
SRC = src/main.c

# Alvo padrão
# Executado quando usamos apenas: make
all:
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

# Remove o arquivo executável
# Executado com: make clean
clean:
	rm -f $(TARGET)