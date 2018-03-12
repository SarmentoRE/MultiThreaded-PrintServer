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

int numJobsAdded = 0, numJobsRemoved = 0;
int sizeJobsAdded = 0, sizeJobsRemoved = 0;
// circular buffer vars
int size = 0, start = 0, end = 0;

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

    sem_init(&queueLock, 0, 1);
    sem_init(&readSem, 0, 0);
    sem_init(&writeSem, 0, QUEUESIZE);

    int i;
    for (i = 0; i < consumerThreadCount; i++) {
        pthread_create(&consumerThreads[i], NULL, printThreadFunc, (void*)i);
    }

    for (i = 0; i < producerThreadCount; i++) {
        pthread_create(&producerThreads[i], NULL, userThreadFunc, (void*)i);
    }

    // do it in this order so that we can end the program once the producers are done and the queue is empty
    for (i = 0; i < producerThreadCount; i++) {
        pthread_join(producerThreads[i], NULL);
        printf("ENDING PRODUCER THREADDDD %i\n", i);
    }
    printf("ALL PRODUCER THREADS DONE\n");
    producersDone = 1;

    while (flag && (!queueEmpty())) {
        sleep(1);
    }

    printf("Ending the threads\n");
    // Ending those threads
    int j;
    for (i = 0; i < consumerThreadCount; i++) {
        for (j = 0; j < QUEUESIZE; j++) {
            sem_post(&readSem);
        }
        pthread_join(consumerThreads[i], NULL);

        //printf("ending consumer thread %i\n", i);
    }

    sem_destroy(&queueLock);
    sem_destroy(&readSem);
    sem_destroy(&writeSem);

    printf("# jobs added:\t %i\n # jobs removed:\t %i\n", numJobsAdded, numJobsRemoved - abs(size));
    printf("size jobs added:\t %i\n size jobs removed:\t %i\n", sizeJobsAdded, sizeJobsRemoved);
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

void signal_handler(int signo)
{
    if (signo == SIGINT) {
        flag = 0;
        printf("gracefully terminating\n");
    }
}

void* userThreadFunc(int threadId)
{
    int numJobs = random_between_range(1, 20);
    /* printf("thread %i will be submitting %i jobs\n", threadId, numJobs); */

    int j;
    for (j = 0; j < numJobs; j++) {
        addJob(random_between_range(100, 1000), threadId);
    }
    /* pthread_exit(0); */
}

void addJob(int jobSize, int threadId)
{
    struct printRequest job;
    job.userId = threadId;
    job.size = jobSize;

    sem_wait(&writeSem);
    /* printf("user thread %i got the writeSem\n", threadId); */
    sem_wait(&queueLock);
    /* printf("print thread %i got the  queueLock\n", threadId); */

    /* printf("adding job of size %i for thread %i\n", jobSize, threadId); */

    queue[(start + size) % QUEUESIZE] = job;
    size++;
    sem_post(&queueLock);
    sem_post(&readSem);

    int val;
    sem_getvalue(&readSem, &val);
    printf("\tthread %i WRITESEM val:\t%i\n", threadId, val);

    numJobsAdded++;
    sizeJobsAdded += jobSize;
    printf("numJobsAdded thread:%i,\t%i\n", threadId, numJobsAdded);
}

void* printThreadFunc(int threadId)
{
    /* printf("before loop flag:\t%i producersDone:\t%i queueEmpty():\t%i \t %i\n", flag, !producersDone, !queueEmpty(), !(producersDone && queueEmpty())); */
    while (flag && !(producersDone && queueEmpty())) {
        /* printf("flag:\t%i producersDone:\t%i queueEmpty():\t%i\n", flag, producersDone, queueEmpty()); */
        removeJob(start, threadId);
    }

    printf("CONSUMER THREAD %i ENDING\n", threadId);
    /* pthread_exit(0); */
}

void removeJob(int index, int threadId)
{
    int val;
    sem_getvalue(&readSem, &val);
    printf("thread %i READSEM val:\t%i\n", threadId, val);
    sem_wait(&readSem);
    if (producersDone == 0 || !(queueEmpty())) {
        /* printf("print thread %i got the readSem\n", threadId); */
        sem_wait(&queueLock);
        /* printf("print thread %i got the  queueLock\n", threadId); */

        /* printf("removing job from queue at position: %i\n", start); */
        struct printRequest job = queue[start];

        sizeJobsRemoved += job.size;

        memset(&queue[start], 0, sizeof(job));
        start++;
        start %= 15;
        size--;

        sem_post(&queueLock);
        sem_post(&writeSem);
        numJobsRemoved++;
        printf("numJobsRemoved thread:%i,\t%i,\t\t size: %i\n", threadId, numJobsRemoved, size);
    }
}

int queueEmpty()
{
    int val;
    sem_getvalue(&readSem, &val);
    return val == 0;
}
