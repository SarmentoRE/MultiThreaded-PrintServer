#include <stdlib.h>
#include <stdio.h>
#include "printQueue.h"
int main(int argc, char* argv[])
{
    int producerThreads = atoi(argv[1]);
    int consumerThreads = atoi(argv[2]);

    printf("num producers:\t%i\n", producerThreads);
    printf("num consumers:\t%i\n", consumerThreads);
    return 1;
}

int userThreadFunc()
{
    return 1;
}

int printThreadFunc()
{
    return 1;
}
