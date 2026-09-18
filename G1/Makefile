main.o: main.c
	gcc -ggdb -c main.c

quad.o: quad.c
	gcc -ggdb -c quad.c

p1: main.o quad.o
	gcc -ggdb $^ -o $@ -lm

clean: 
	rm main.o quad.o p1
