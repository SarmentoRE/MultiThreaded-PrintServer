#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include <time.h>
#include <string.h>

#include "printQueue.h"

#define QUEUESIZE 15

int flag;
int producersDone;
struct printRequest queue[QUEUESIZE];

sem_t queueLock;
sem_t readSem;
sem_t writeSem;

int main(int argc, char* argv[])
{
    producersDone = 0;
    flag = 1;
    signal(SIGINT, signal_handler);
    if (argc < 3) {
        printf("Please enter threadCounts\n");
        return -5;
    }

    int producerThreadCount = atoi(argv[1]);
    int consumerThreadCount = atoi(argv[2]);
    printf("starting %i producers and %i consumers\n", producerThreadCount, consumerThreadCount);

    pthread_t producerThreads[producerThreadCount];
    pthread_t consumerThreads[consumerThreadCount];

    int i;
    for (i = 0; i < producerThreadCount; i++) {
        pthread_create(&producerThreads[i], NULL, userThreadFunc, (void*)i);
    }

    for (i = 0; i < consumerThreadCount; i++) {
        pthread_create(&consumerThreads[i], NULL, printThreadFunc, (void*)i);
    }

    // do it in this order so that we can end the program once the producers are done and the queue is empty
    for (i = 0; i < producerThreadCount; i++) {
        pthread_join(producerThreads[i], NULL);
        printf("ending producer thread %i\n", i);
    }

    producersDone = 1;

    while (flag && (!queueEmpty())) {
        sleep(1);
    }

    printf("Ending the threads\n");
    // Ending those threads

    for (i = 0; i < consumerThreadCount; i++) {
        pthread_join(consumerThreads[i], NULL);
        //printf("ending consumer thread %i\n", i);
    }

    return 0;
}

// Thanks stackoverflow... Not plaigarizing just using something I don't want to rewrite myself
// because thread safety is a pain for getting random numbers.
// https://stackoverflow.com/a/36631869/5472958
size_t random_between_range(size_t min, size_t max)
{
    unsigned short state[3];
    unsigned int seed = time(NULL) + (unsigned int)pthread_self();
    memcpy(state, &seed, sizeof(seed));
    return min + nrand48(state) % (max - min);
}

void* userThreadFunc(int threadId)
{
    int numJobs = random_between_range(1, 20);
    printf("thread %i will be submitting %i jobs\n", threadId, numJobs);

    int j;
    struct printRequest job;
    for (j = 0; j < numJobs; j++) {
        addJob(random_between_range(100, 1000), threadId);
    }
}

void* printThreadFunc(int threadId)
{
    while (flag && (!producersDone && !(queueEmpty()))) {
        sleep(5);
        printf("flag:\t%i producersDone:\t%i queueEmpty():\t%i\n", flag, producersDone, queueEmpty());
    }
}

void signal_handler(int signo)
{
    if (signo == SIGINT) {
        flag = 0;
        printf("gracefully terminating\n");
    }
}

void addJob(int size, int threadId)
{
    printf("adding job of size %i for thread %i\n", size, threadId);
}

void removeJob(int index)
{
}

int queueEmpty()
{
    return 1;
}
