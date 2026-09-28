# Imagem base
FROM ubuntu:26.04

# Instala as ferramentas necessárias
RUN apt update && apt install -y \
    gcc \
    make \
    gdb \
    && rm -rf /var/lib/apt/lists/*

# Define a pasta de trabalho
WORKDIR /app

# Copia o projeto para dentro do container
COPY . .