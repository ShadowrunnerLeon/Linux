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

static int avail = 0;

static pthread_mutex_t pmutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t pcond = PTHREAD_COND_INITIALIZER;

void err_msg(char *msg) 
{
    perror(msg);
    exit(EXIT_FAILURE);
}

void *thread_func(void *arg) 
{
    for (int i = 0; i < 5; ++i) 
    {
        if (pthread_mutex_lock(&pmutex)) err_msg("pthread_mutex_lock");

        printf("Pthread %d\n", (int)arg);
        avail++;

        if (pthread_mutex_unlock(&pmutex)) err_msg("pthread_mutex_unlock");

        if (pthread_cond_signal(&pcond)) err_msg("pthread_cond_signal");
    }
}

int main() 
{
    pthread_t pthread[3];
    
    for (int i = 0; i < 3; ++i)
    {
        if (pthread_create(&pthread[i], NULL, thread_func, (void*)(i+1))) err_msg("pthread_create");
    }

    int count = avail;

    while (count < 15) 
    {
        if (pthread_mutex_lock(&pmutex)) err_msg("pthread_mutex_lock");

        while (!avail)
        {
            if (pthread_cond_wait(&pcond, &pmutex)) err_msg("pthread_cond_wait");
        }

        while (avail) 
        {
            printf("New value\n");
            ++count;
            --avail;
        }

        if (pthread_mutex_unlock(&pmutex)) err_msg("pthread_mutex_unlock");
    }

    for (int i = 0; i < 3; ++i)
    {
        if (pthread_join(pthread[i], NULL)) err_msg("pthread_join");
    }
}