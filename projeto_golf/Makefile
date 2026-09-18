# Variáveis de Compilação
CC = gcc
CFLAGS = -Wall -Wextra -std=c11
LIBS = -lcunit

# Alvo padrão: Compila o jogo
all: golf

# Compilação do Executável do Jogo
golf: main.o logica.o
	$(CC) $(CFLAGS) -o golf main.o logica.o

# Compilação dos Testes Unitários
testes: testes.o logica.o
	$(CC) $(CFLAGS) -o executar_testes testes.o logica.o $(LIBS)
	./executar_testes

# Objetos Individuais
main.o: main.c golf.h
	$(CC) $(CFLAGS) -c main.c

logica.o: logica.c golf.h
	$(CC) $(CFLAGS) -c logica.c

testes.o: testes.c golf.h
	$(CC) $(CFLAGS) -c testes.c

# Limpeza de ficheiros temporários
clean:
	rm -f *.o golf executar_testes
