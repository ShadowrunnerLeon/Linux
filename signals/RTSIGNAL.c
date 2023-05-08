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

static volatile sig_atomic_t ind = 0;

void err_msg(char *msg) 
{
    perror(msg);
    exit(EXIT_FAILURE);
}

void sighandler(int sig, siginfo_t *siginfo, void *ucontext) 
{
    ++ind;

    if (sig == SIGRTMIN + 1)
    {
        printf("%d: Delivery %d: %d\n", SIGRTMIN + 1, ind, siginfo->si_value);
    }
    else if (sig == SIGRTMIN + 2)
    {
        printf("%d: Delivery %d: %d\n", SIGRTMIN + 2, ind, siginfo->si_value);
    }
    else
    {
        printf("%d: Delivery %d: %d\n", SIGRTMIN + 3, ind, siginfo->si_value);
    }
}

int main() 
{
    struct sigaction sa;
    sa.sa_sigaction = sighandler;
    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGRTMIN+1, &sa, NULL) == -1) err_msg("sigaction");
    if (sigaction(SIGRTMIN+2, &sa, NULL) == -1) err_msg("sigaction");
    if (sigaction(SIGRTMIN+3, &sa, NULL) == -1) err_msg("sigaction");

    pid_t pid = fork();
    if (pid == -1)
    {
        err_msg("fork");
    }
    else if (!pid) 
    {
        union sigval value;
        value.sival_int = 10;

        for (int i = 0; i < 5; ++i) 
        {
            if (sigqueue(getppid(), SIGRTMIN + 1, value) == -1) err_msg("sigqueue");
            ++value.sival_int;
            if (sigqueue(getppid(), SIGRTMIN + 2, value) == -1) err_msg("sigqueue");
            ++value.sival_int;
            if (sigqueue(getppid(), SIGRTMIN + 3, value) == -1) err_msg("sigqueue");
            ++value.sival_int;
        }

        exit(0);
    }

    sleep(2);

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
