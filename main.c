#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include "printQueue.h"
#define QUEUESIZE 15

int flag;
struct printRequest queue[QUEUESIZE];

int main(int argc, char* argv[])
{
    flag = 1;
    signal(SIGINT, signal_handler);
    if (argc < 3) {
        printf("Please enter threadCounts\n");
        return -5;
    }
    int producerThreadCount = atoi(argv[1]);
    int consumerThreadCount = atoi(argv[2]);

    printf("num producers:\t%i\n", producerThreadCount);
    printf("num consumers:\t%i\n", consumerThreadCount);

    pthread_t producerThreads[producerThreadCount];
    pthread_t consumerThreads[consumerThreadCount];
    int i;
    for (i = 0; i < producerThreadCount; i++) {
        pthread_create(&producerThreads[i], NULL, userThreadFunc, NULL);
    }
    for (i = 0; i < consumerThreadCount; i++) {
        pthread_create(&consumerThreads[i], NULL, printThreadFunc, NULL);
    }
    while (flag) {
        sleep(1);
    }

    // Ending those threads
    for (i = 0; i < producerThreadCount; i++) {
        pthread_join(producerThreads[i], NULL);
        printf("ending producer thread %i\n", i);
    }
    for (i = 0; i < consumerThreadCount; i++) {
        pthread_join(consumerThreads[i], NULL);
        printf("ending consumer thread %i\n", i);
    }

    return 0;
}

int userThreadFunc()
{
    return 1;
}

int printThreadFunc()
{
    return 1;
}

void signal_handler(int signo)
{
    if (signo == SIGINT) {
        flag = 0;
        printf("gracefully terminating\n");
    }
}
