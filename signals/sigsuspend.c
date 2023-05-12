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
#include <time.h>

static volatile sig_atomic_t sigFlag = 1;
sigjmp_buf senv;

void err_msg(char *msg) 
{
    perror(msg);
    exit(EXIT_FAILURE);
}

void sighandler(int sig) 
{
    if (sig == SIGUSR1)
    {
        printf("SIGUSR1\n");
    }
    else 
    {
        sigFlag = 0;
        siglongjmp(senv, 1);
    }
}

int main() 
{
    struct sigaction sa1;
    sa1.sa_handler = sighandler;
    sa1.sa_flags = SA_RESTART;
    sigemptyset(&sa1.sa_mask);

    if (sigaction(SIGUSR1, &sa1, NULL) == -1) err_msg("sigaction");

    struct sigaction sa2;
    sa2.sa_handler = sighandler;
    sa2.sa_flags = 0;
    sigemptyset(&sa2.sa_mask);

    if (sigaction(SIGINT, &sa2, NULL) == -1) err_msg("sigaction");

    sigset_t sigmask, prevmask;
    sigemptyset(&sigmask);
    sigaddset(&sigmask, SIGUSR1);

    if (sigprocmask(SIG_SETMASK, &sigmask, &prevmask) == -1) err_msg("sigprocmask");

    pid_t pid = fork();
    if (pid == -1)
    {
        err_msg("fork");
    }
    else if (!pid) 
    {
        for (int i = 0; i < 1000000; ++i)
        {
            if (kill(getppid(), SIGUSR1) == -1) err_msg("kill");
        }

        exit(EXIT_SUCCESS);
    }

    sigset_t sigpend;
    if (sigpending(&sigpend) == -1) err_msg("sigpending");

    struct timespec fulltime;
    fulltime.tv_sec = 0;
    fulltime.tv_nsec = 0;

    sigsetjmp(senv, 1);

    while (sigFlag && !sigisemptyset(&sigpend)) 
    {

        struct timespec tm1, tm2;

        if (clock_gettime(CLOCK_MONOTONIC, &tm1) == -1) err_msg("clock_gettime");

        //siginfo_t siginfo;
        //if (sigwaitinfo(&sigmask, &siginfo) == -1 && errno!=EINTR)
        //    err_msg("sigwaitinfo");

        if (sigsuspend(&prevmask) == -1 && errno!=EINTR) err_msg("sigsuspend");
        if (clock_gettime(CLOCK_MONOTONIC, &tm2) == -1) err_msg("clock_gettime");

        fulltime.tv_sec  += ((tm2.tv_sec*1000000000 + tm2.tv_nsec) - (tm1.tv_sec*1000000000 + tm1.tv_nsec)) / 1000000000;
        fulltime.tv_nsec += ((tm2.tv_sec*1000000000 + tm2.tv_nsec) - (tm1.tv_sec*1000000000 + tm1.tv_nsec)) % 1000000000;
        
        if (fulltime.tv_nsec > 1000000000) 
        {
            fulltime.tv_nsec /= 10;
            ++fulltime.tv_sec;
        }

        if (sigpending(&sigpend) == -1) err_msg("sigpending");
    }

    printf("Time: %ld.%ld\n", fulltime.tv_sec, fulltime.tv_nsec);

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
