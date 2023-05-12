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

static pthread_mutex_t pmutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t pcond = PTHREAD_COND_INITIALIZER;

static int count_joined = 0;

enum state 
{
    PTH_ALIVE,
    PTH_TERMINATED,
    PTH_JOINED
};

static struct thread_state 
{
    pthread_t id;
    enum state pstate;
} pthread[5];

void err_msg(char *msg) 
{
    perror(msg);
    exit(EXIT_FAILURE);
}

void *thread_func(void *arg) 
{
    sleep(2);

    if (pthread_mutex_lock(&pmutex)) err_msg("pthread_mutex_lock");

    ++count_joined;
    pthread[(int)arg].pstate = PTH_TERMINATED;
    printf("Pthread %d terminated\n", (int)arg);

    if (pthread_mutex_unlock(&pmutex)) err_msg("pthread_mutex_unlock");

    if (pthread_cond_signal(&pcond)) err_msg("pthread_cond_signal");
}

int main() 
{
    for (int i = 0; i < 5; ++i) 
    {
        if (pthread_create(&pthread[i].id, NULL, thread_func, (void*)i)) err_msg("pthread_create");
        pthread[i].pstate = PTH_ALIVE;
    }

    int count = count_joined;

    while (count < 5) 
    {
        if (pthread_mutex_lock(&pmutex)) err_msg("pthread_mutex_lock");
        
        while (!count_joined)
        {
            if (pthread_cond_wait(&pcond, &pmutex)) err_msg("pthread_cond_wait");
        }

        while (count_joined) 
        {
            for (int i = 0; i < 5; ++i)
            {
                if (pthread[i].pstate == PTH_TERMINATED) 
                {
                    if (pthread_join(pthread[i].id, NULL)) err_msg("pthread_join");

                    pthread[i].pstate = PTH_JOINED;
                    --count_joined;
                    ++count;
                    printf("Pthread %d joined\n", i);
                }
            }
        }

        if (pthread_mutex_unlock(&pmutex)) err_msg("pthread_mutex_unlock");
    }
}