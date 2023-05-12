#define _GNU_SOURCE
#define BUFSIZE 512

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

pthread_rwlock_t rwlock;
FILE* frd;
static char buf[BUFSIZE];

void err_msg(const char* msg) 
{
    perror(msg);
    exit(EXIT_FAILURE);
}

void* pthread_rdfunc(void* arg) 
{
    for (int index = 0; index < 10; ++index) 
    {
        if (pthread_rwlock_rdlock(&rwlock)) err_msg("pthread_rwlock_rdlock");
        if (fread(buf, sizeof(char) * BUFSIZE, 1, frd) == -1) err_msg("fread");  
        if (pthread_rwlock_unlock(&rwlock)) err_msg("pthread_rwlock_unlock"); 
    }                   
}

int main() 
{
    if (!(frd = fopen("/dev/urandom", "r"))) err_msg("fopen");

    if (pthread_rwlock_init(&rwlock, NULL)) err_msg("pthread_rwlock_init");

    //-----------------------time------------------------------------

    struct timespec tp1, tp2, ret;

    if (clock_gettime(CLOCK_MONOTONIC, &tp1) == -1) err_msg("clock_gettime");

    pthread_t readers[3];

    for (int index = 0; index < 3; ++index)
    {
        if (pthread_create(&readers[index], NULL, pthread_rdfunc, NULL)) err_msg("pthread_create");
    }

    for (int index = 0; index < 3; ++index)
    {
        if (pthread_join(readers[index], NULL)) err_msg("pthread_join");
    }

    if (clock_gettime(CLOCK_MONOTONIC, &tp2) == -1) err_msg("clock_gettime");

    ret.tv_nsec = (tp2.tv_nsec < tp1.tv_nsec) ? 1000000000 - tp1.tv_nsec : tp2.tv_nsec - tp1.tv_nsec;
    ret.tv_sec = tp2.tv_sec - tp1.tv_sec;

    printf("pthread[%ld]: %lf\n", (long)pthread_self(), (double)ret.tv_sec + (double)ret.tv_nsec / 1000000000);

    //-----------------------time------------------------------------

    if (pthread_rwlock_destroy(&rwlock)) err_msg("pthread_rwlock_destroy");
    fclose(frd);
}