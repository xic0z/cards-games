CC     = gcc
CFLAGS = -Wall -Wextra -std=c11 -g

OBJ        = main_gen.o parser.o logica_gen.o interface_gen.o saveload.o
OBJ_TESTES = testes.o parser.o logica_gen.o interface_gen.o saveload.o

.PHONY: all clean

all: paciencia testes

paciencia: $(OBJ)
	$(CC) $(CFLAGS) -o paciencia $(OBJ)

testes: $(OBJ_TESTES)
	$(CC) $(CFLAGS) -o testes $(OBJ_TESTES) -lcunit

main_gen.o: main_gen.c paciencia.h
	$(CC) $(CFLAGS) -c main_gen.c

parser.o: parser.c paciencia.h
	$(CC) $(CFLAGS) -c parser.c

logica_gen.o: logica_gen.c paciencia.h
	$(CC) $(CFLAGS) -c logica_gen.c

interface_gen.o: interface_gen.c paciencia.h
	$(CC) $(CFLAGS) -c interface_gen.c

saveload.o: saveload.c paciencia.h
	$(CC) $(CFLAGS) -c saveload.c

testes.o: testes.c paciencia.h
	$(CC) $(CFLAGS) -c testes.c

clean:
	rm -f *.o paciencia testes
