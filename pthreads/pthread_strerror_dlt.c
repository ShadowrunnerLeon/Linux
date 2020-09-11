#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <pthread.h>
#include <errno.h>

#define BUF_SIZE 32

void err_msg(char *str) {
    perror(str);
    exit(-1);
};

static pthread_once_t pth_once = PTHREAD_ONCE_INIT;
static pthread_key_t key;

/**
 * Alternative variant - use __thread before variable
 * For example: static __thread var;
 **/

void *destructor(void *buf) {
    free(buf);
};

void *createKey() {
    if (pthread_key_create(&key, destructor))
        err_msg("pthread_key_create");
};

char *pthread_strerror(int arg) {

    if (pthread_once(&pth_once, createKey))
        err_msg("pthread_once");

    char *msg;
    if ((msg = pthread_getspecific(key))==NULL) {
        if ((msg = malloc(BUF_SIZE))==NULL)
            err_msg("malloc");
        if (pthread_setspecific(key, msg))
            err_msg("pthread_setspecific");
    };

    snprintf(msg, BUF_SIZE, "%p: Hello, %d", &arg, arg);
    return msg;

};

void *threadFunc(void *arg) {
    printf("%s\n", pthread_strerror((int)arg));
};

int main() {
    pthread_t pthread1, pthread2, pthread3;

    if (pthread_create(&pthread1, NULL, threadFunc, (void*)1))
        err_msg("pthread_create");

    if (pthread_create(&pthread2, NULL, threadFunc, (void*)2))
        err_msg("pthread_create");

    if (pthread_create(&pthread3, NULL, threadFunc, (void*)3))
        err_msg("pthread_create");

    printf("%s\n", pthread_strerror(0));

    if (pthread_join(pthread1, NULL))
        err_msg("pthread_join");

    if (pthread_join(pthread2, NULL))
        err_msg("pthread_join");

    if (pthread_join(pthread3, NULL))
        err_msg("pthread_join");

}