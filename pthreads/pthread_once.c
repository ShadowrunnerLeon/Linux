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

static bool OnceFlag = 1;
static pthread_mutex_t once_pmutex = PTHREAD_MUTEX_INITIALIZER;

static void one_time_init(bool *flag, void(*init)(void)) {
    if (pthread_mutex_lock(&once_pmutex))
        err_msg("pthread_mutex_lock");

    if (*flag) {
        init();
        *flag = 0;
    };

    if (pthread_mutex_unlock(&once_pmutex))
        err_msg("pthread_mutex_unlock");
};

void init() {
    printf("Init\n");
};

void *threadFunc(void *arg) {
    one_time_init(&OnceFlag, init);
    printf("Thread %d\n", (int)arg);
};

int main() {
    pthread_t pthread1, pthread2;

    if (pthread_create(&pthread1, NULL, threadFunc, (void*)1))
        err_msg("pthread_create");

    if (pthread_create(&pthread2, NULL, threadFunc, (void*)2))
        err_msg("pthread_create");
    
    if (pthread_join(pthread1, NULL))
        err_msg("pthread_join");

    if (pthread_join(pthread2, NULL))
        err_msg("pthread_join");    
}