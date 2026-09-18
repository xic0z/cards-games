CC=gcc
CFLAGS= -Wall -ggdb
LDLIBS= -lm

main: main.o quad.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS) 

main.o: main.c
	$(CC) $(CFLAGS)  -c main.c

quad.o: quad.c 
	$(CC) $(CFLAGS) -c quad.c

clean:
	rm -f main.o quad.o main
