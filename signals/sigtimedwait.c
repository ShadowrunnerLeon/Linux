#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <setjmp.h>

void err_msg(char *msg) 
{
    perror(msg);
    exit(EXIT_FAILURE);
}

void sighandler(int sig) 
{
    printf("SIGUSR1\n");
}

int main() 
{
    struct sigaction sa;
    sa.sa_flags = 0;
    sa.sa_handler = sighandler;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGUSR1, &sa, NULL) == -1) err_msg("sigaction");

    sigset_t sigmask;
    sigemptyset(&sigmask);

    if (sigaddset(&sigmask, SIGUSR1) == -1) err_msg("sigaddset");

    siginfo_t info;
    struct timespec timeout;
    timeout.tv_sec = 1;
    timeout.tv_nsec = 0;

    pid_t pid = fork();
    if (pid == -1)
    {
        err_msg("fork");
    }
    else if (!pid) 
    {
        sleep(2);
        if (kill(getppid(), SIGUSR1) == -1) err_msg("kill");
        exit(0);
    }


    if (sigtimedwait(&sigmask, &info, &timeout) == -1 && errno == EAGAIN)
    {
        printf("Reboot\n");
    }
    else if (errno != EAGAIN)
    {
        err_msg("sigtimedwait");
    }

    int status, waitpid;
    while ((waitpid=wait(&status) == -1) && errno == EINTR) {}

    if (WIFEXITED(status))
    {
        printf("Child returned!\n");
    }
    else if (WIFSIGNALED(status))
    {
        printf("Child interrupted by signal!");
    }
}
