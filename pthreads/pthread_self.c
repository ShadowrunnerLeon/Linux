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

void err_msg(char *str) {
    perror(str);
    exit(-1);
};

void *thread_func(void *arg) {
    if (pthread_join(pthread_self(), NULL))
        err_msg("pthread_join");
}

int main() {
    pthread_t pthread;
    if (pthread_create(&pthread, NULL, thread_func, NULL))
        err_msg("pthread_create");

    pthread_exit(NULL);
}