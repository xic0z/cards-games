main.o: main.c
	gcc -c main.c

quad.o: quad.c
	gcc -c quad.c

p1: main.o quad.o
	gcc $^ -o $@ -lm

clean: 
	rm main.o quad.o p1
