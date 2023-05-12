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

void err_msg(char *msg) 
{
    perror(msg);
    exit(EXIT_FAILURE);
}

void sighandler(int sig) 
{
    printf("SIGCHLD!\n");
}

void *threadFunc(void *arg) 
{
    struct sigaction sa;
    sa.sa_handler = sighandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_NOCLDSTOP;

    if (sigaction(SIGCHLD, &sa, NULL) == -1) err_msg("sigaction");

    pid_t pid = fork();

    if (pid == -1)
    {
        err_msg("fork");
    }
    else if (!pid) 
    {
        printf("Child process created by pthread of parent process\n");
        sleep(3);
        exit(EXIT_SUCCESS);
    }

    sleep(5);

    int status, waitid;
    if (wait(&status) == -1) err_msg("wait");
    if (WIFEXITED(status)) printf("Child returned!\n"); 
}

int main() 
{
    pthread_t pthread;

    if (pthread_create(&pthread, NULL, threadFunc, NULL)) err_msg("pthread_create");
    if (pthread_join(pthread, NULL)) err_msg("pthread_join");

    printf("Pthread joined\n");
}