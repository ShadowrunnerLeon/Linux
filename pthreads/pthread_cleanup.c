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

void err_msg(char *str) {
    perror(str);
    exit(-1);
};

pthread_mutex_t pmutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t pcond = PTHREAD_COND_INITIALIZER;
static int glob = 0;

void CleanUp(void *arg) {
    if (pthread_mutex_unlock(&pmutex))
        err_msg("pthread_mutex_unlock");
    printf("CleanUp\n");
};

void *threadFunc(void *arg) {

    if (pthread_mutex_lock(&pmutex))
        err_msg("pthread_mutex_lock");

    pthread_cleanup_push(CleanUp, NULL);

    while (glob==0) 
        if (pthread_cond_wait(&pcond, &pmutex))
            err_msg("pthread_cond_wait");

    pthread_cleanup_pop(glob);
    
};

int main(int argc, char *argv[]) {

    pthread_t pthread;

    if (pthread_create(&pthread, NULL, threadFunc, NULL))
        err_msg("pthread_create");

    sleep(1);

    if (argc==1) {
        if (pthread_cancel(pthread))
            err_msg("pthread_cancel");
    }
    else {
        glob = 1;
        if (pthread_cond_signal(&pcond))
            err_msg("pthread_cond_signal");
    };

    int *res;
    if (pthread_join(pthread, &res))
        err_msg("pthread_join");

    if (res==PTHREAD_CANCELED)
        printf("Pthread canceled\n");
    else
        printf("Pthread returned\n");

}