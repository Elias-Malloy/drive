

all: main.o 
	cc -o bin $^ -lSDL2 -lvulkan

main.o: main.c
	cc -c main.c

clean:
	rm -rf bin *.o
