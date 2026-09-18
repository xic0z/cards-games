# Variáveis de compilação
CC = gcc
CFLAGS = -Wall -Wextra -std=c11
LIBS = -lcunit
TARGET = simplesimon
TEST_TARGET = executar_testes

# Lista de ficheiros de objeto para o jogo
OBJ = main.o logica.o interface.o

# Regra principal: Compila o jogo
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

# Regra para compilar os testes
# Nota: O testes.c não usa o main.o porque o CUnit tem o seu próprio main
test: logica.o testes.c
	$(CC) $(CFLAGS) testes.c logica.o -o $(TEST_TARGET) $(LIBS)
	./$(TEST_TARGET)

# Como compilar o main.o
main.o: main.c simon.h
	$(CC) $(CFLAGS) -c main.c

# Como compilar a logica.o
logica.o: logica.c simon.h
	$(CC) $(CFLAGS) -c logica.c

# Como compilar a interface.o
interface.o: interface.c simon.h
	$(CC) $(CFLAGS) -c interface.c

# Limpeza dos ficheiros temporários e dos dois executáveis
clean:
	rm -f *.o $(TARGET) $(TEST_TARGET)

# Comando para correr o jogo
run: $(TARGET)
	./$(TARGET)
