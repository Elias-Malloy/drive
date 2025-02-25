

all: main.o
	cc -o bin $^

main.o: main.c
	cc -c main.c
