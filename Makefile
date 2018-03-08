

all: main.o

main.o: main.c printQueue.h
	gcc $^ -g -o $@ -lpthread -lrt


clean:
	rm -f *.o

run: main.o
	./main.o 10 12
