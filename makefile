

all: main.o emm_vulkan.o
	cc -o bin $^ -lSDL2 -lvulkan

main.o: main.c
	cc -c main.c

emm_vulkan.o: emm_vulkan.c
	cc -c emm_vulkan.c

clean:
	rm -rf bin *.o
