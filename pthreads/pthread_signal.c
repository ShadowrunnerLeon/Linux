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
#include <signal.h>

void err_msg(char *str) {
    perror(str);
    exit(-1);
};

pthread_t pthread1, pthread2;
sigset_t set;

void *threadFunc1(void *arg) {
    sleep(1);
    
    if (pthread_kill(pthread2, SIGUSR1))
        err_msg("pthread_kill");

    sleep(1);
    sigset_t sigpend;
    if (sigpending(&sigpend)==-1)
        err_msg("sigpending");

    for (int sig = 1; sig<NSIG; sig++) 
        if (sigismember(&sigpend, sig))
            printf("Pthread 1: sig %d blocked or in waiting\n", sig);

    int sig;
    if (sigwait(&set, &sig))
        err_msg("sigwait");

    if (sig==SIGUSR2)
        printf("SIGUSR2 delivered\n");
};

void *threadFunc2(void *arg) {
    sleep(1);

    if (pthread_kill(pthread1, SIGUSR2))
        err_msg("pthread_kill");

    sleep(1);
    sigset_t sigpend;
    if (sigpending(&sigpend)==-1)
        err_msg("sigpending");

    for (int sig = 1; sig<NSIG; sig++) 
        if (sigismember(&sigpend, sig))
            printf("Pthread 2: sig %d blocked or in waiting\n", sig);

    int sig;
    if (sigwait(&set, &sig))
        err_msg("sigwait");

    if (sig==SIGUSR1)
        printf("SIGUSR1 delivered\n");
};

int main() {
    if (sigfillset(&set)==-1)
        err_msg("sigfillset");

    if (sigprocmask(SIG_SETMASK, &set, NULL)==-1)
        err_msg("sigprocmask");

    if (pthread_create(&pthread1, NULL, threadFunc1, NULL))
        err_msg("pthread_create");

    if (pthread_create(&pthread2, NULL, threadFunc2, NULL))
        err_msg("pthread_create");

    sleep(2);

    if (pthread_join(pthread1, NULL))
        err_msg("pthread_join");

    if (pthread_join(pthread2, NULL))
        err_msg("pthread_join");
}