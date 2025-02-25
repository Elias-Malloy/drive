

all: main.o glad.o
	cc -o bin $^ -lSDL2

main.o: main.c
	cc -c main.c

glad.o: glad.c
	cc -c glad.c

clean:
	rm -rf bin *.o
