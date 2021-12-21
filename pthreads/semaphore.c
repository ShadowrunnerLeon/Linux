#define _GNU_SOURCE
#define BUFSIZE 512

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

sem_t sem;
FILE* frd;
static char buf[BUFSIZE];

void err_msg(const char* msg) {
    perror(msg);
    exit(-1);
}

void* pthread_rdfunc(void* arg) {

    if (sem_wait(&sem) == -1)
        err_msg("sem_wait");

    for (int index = 0; index < 200; ++index)
        if (fread(buf, sizeof(char), BUFSIZE, frd) == -1)
            err_msg("fread");                       

    if (sem_post(&sem) == -1)
        err_msg("sem_post");
}

int main() {
    if ((frd = fopen("/dev/urandom", "r")) == NULL)
        err_msg("fopen");

    if (sem_init(&sem, 0, 1) == -1)
        err_msg("sem_init");

    //-----------------------time------------------------------------

    struct timespec tp1, tp2, ret;

    if (clock_gettime(CLOCK_MONOTONIC, &tp1) == -1)
        err_msg("clock_gettime");

    pthread_t readers[3];

    for (int index = 0; index < 3; ++index)
        if (pthread_create(&readers[index], NULL, pthread_rdfunc, NULL))
            err_msg("pthread_create");

    for (int index = 0; index < 3; ++index)
        if (pthread_join(readers[index], NULL))
            err_msg("pthread_join");

    if (clock_gettime(CLOCK_MONOTONIC, &tp2) == -1)
        err_msg("clock_gettime");

    if (tp2.tv_nsec < tp1.tv_nsec)
        ret.tv_nsec = 1000000000 - tp1.tv_nsec;
    else
        ret.tv_nsec = tp2.tv_nsec - tp1.tv_nsec;

    ret.tv_sec = tp2.tv_sec - tp1.tv_sec;

    printf("pthread[%ld]: %lf\n", (long)pthread_self(), (double)ret.tv_sec + (double)ret.tv_nsec/1000000000);

    //-----------------------time------------------------------------

    if (sem_destroy(&sem) == -1)
        err_msg("sem_destroy");

    fclose(frd);
}