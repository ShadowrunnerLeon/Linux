#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>

static int glob = 0;

pthread_mutex_t pmutex = PTHREAD_MUTEX_INITIALIZER;

void err_msg(char *str) {
    perror(str);
    exit(-1);
};

void *thread_func(void *arg) {

    for (int i=0; i<50000; i++) {

        if (pthread_mutex_lock(&pmutex))
            err_msg("pthread_mutex_lock");

        int loc = glob;
        loc++;
        glob = loc;
        printf("Pthread %d: glob = %d\n", (int)arg, glob);

        if (pthread_mutex_unlock(&pmutex))
            err_msg("pthread_mutex_unlock");

    };

}

int main() {

    pthread_t pthread1, pthread2, pthread3;

    if (pthread_create(&pthread1, NULL, thread_func, (void*)1))
        err_msg("pthread_create");

    if (pthread_create(&pthread2, NULL, thread_func, (void*)2))
        err_msg("pthread_create");

    if (pthread_create(&pthread3, NULL, thread_func, (void*)3))
        err_msg("pthread_create");

    if (pthread_join(pthread1, NULL))
        err_msg("pthread_join");

    if (pthread_join(pthread2, NULL))
        err_msg("pthread_join");

    if (pthread_join(pthread3, NULL))
        err_msg("pthread_join");

}