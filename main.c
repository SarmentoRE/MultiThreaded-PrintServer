#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include "printQueue.h"
#include "main.h"

int main(int argc, char* argv[])
{
    signal(SIGINT, signal_handler);
    if (argc < 3) {
        printf("Please enter threadCounts\n");
        return -5;
    }
    int producerThreads = atoi(argv[1]);
    int consumerThreads = atoi(argv[2]);

    printf("num producers:\t%i\n", producerThreads);
    printf("num consumers:\t%i\n", consumerThreads);

    while (1) {
        sleep(1);
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

int signal_handler(int signo)
{
    printf if (signo == SIGINT)
    {
        printf("gracefully terminating");
    }
}
