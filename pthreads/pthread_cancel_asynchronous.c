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

static int glob = 0;

void *threadFunc(void *arg) {
    if (pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL))
        err_msg("pthread_setcanceltype");

    while (1) {
        glob++;
        printf("glob: %d\n", glob);
    };
};

int main() {
    pthread_t pthread;

    if (pthread_create(&pthread, NULL, threadFunc, NULL))
        err_msg("pthread_create");

    sleep(1);

    if (pthread_cancel(pthread))
        err_msg("pthread_cancel");

    if (pthread_join(pthread, NULL))
        err_msg("pthread_join");
    
}