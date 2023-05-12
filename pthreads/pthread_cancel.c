#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <pthread.h>
#include <errno.h>

void err_msg(char *msg) 
{
    perror(msg);
    exit(EXIT_SUCCESS);
}

void *threadFunc(void *arg) 
{
    int step = 1;
    while (1) 
    {
        printf("Step %d\n", step);
        step++;
        pthread_testcancel();
    }
}

int main() 
{
    pthread_t pthread;

    if (pthread_create(&pthread, NULL, threadFunc, NULL)) err_msg("pthread_create");

    sleep(3);

    if (pthread_cancel(pthread)) err_msg("pthread_cancel");

    int *res;
    if (pthread_join(pthread, &res)) err_msg("pthread_join");

    if (res == PTHREAD_CANCELED) printf("Pthread terminated\n");
}