

all: main.o

main.o: main.c printQueue.h
	gcc $^ -g -o $@ -lpthread -lrt


clean:
	rm -f *.o

run: main.o
	stdbuf -i0 -o0 -e0 ./main.o 10 12 | tee log.txt
