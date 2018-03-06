

all: main.o

main.o: main.c printQueue.h
	gcc $^ -g -o $@ -lpthread -lrt

