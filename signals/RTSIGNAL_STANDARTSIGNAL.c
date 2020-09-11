#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/signalfd.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <setjmp.h>

void err_msg(char *str) {
    perror(str);
    exit(-1);
};

void standart_sighandler(int sig) {
    if (sig==SIGUSR1)
        printf("SIGUSR1\n");
    else if (sig==SIGUSR2)
        printf("SIGUSR2\n");
    else if (sig==SIGINT)
        printf("SIGINT\n");
};

void rt_sighandler(int sig, siginfo_t *siginfo, void *ucontext) {
    if (sig==SIGRTMIN+1)
        printf("SIGRTMIN+1\n");
    else
        printf("SIGRTMIN+2\n");
};

int main() {
    struct sigaction st_sa;
    st_sa.sa_flags = SA_RESTART;
    st_sa.sa_handler = standart_sighandler;
    sigemptyset(&st_sa.sa_mask);

    if (sigaction(SIGUSR1, &st_sa, NULL)==-1)
        err_msg("sigaction");

    if (sigaction(SIGUSR2, &st_sa, NULL)==-1)
        err_msg("sigaction");

    if (sigaction(SIGINT, &st_sa, NULL)==-1)
        err_msg("sigaction");


    struct sigaction rt_sa;
    rt_sa.sa_flags = SA_RESTART;
    rt_sa.sa_sigaction = rt_sighandler;
    sigemptyset(&rt_sa.sa_mask);

    if (sigaction(SIGRTMIN+1, &rt_sa, NULL)==-1)
        err_msg("sigaction");

    if (sigaction(SIGRTMIN+2, &rt_sa, NULL)==-1)
        err_msg("sigaction");


    sigset_t sigmask;
    sigemptyset(&sigmask);
    sigaddset(&sigmask, SIGUSR1);
    sigaddset(&sigmask, SIGUSR2);
    sigaddset(&sigmask, SIGINT);
    sigaddset(&sigmask, SIGRTMIN+1);
    sigaddset(&sigmask, SIGRTMIN+2);

    if (sigprocmask(SIG_SETMASK, &sigmask, NULL)==-1)
        err_msg("sigprocmask");


    pid_t pid = fork();
    if (pid==-1)
        err_msg("fork");
    else if (pid==0) {
        if (kill(getppid(), SIGUSR1)==-1)
            err_msg("kill");

        if (kill(getppid(), SIGUSR2)==-1)
            err_msg("kill");

        if (kill(getppid(), SIGINT)==-1)
            err_msg("kill");

        union sigval value;
        value.sival_int = 10;

        if (sigqueue(getppid(), SIGRTMIN+1, value)==-1)
            err_msg("sigqueue");

        if (sigqueue(getppid(), SIGRTMIN+2, value)==-1)
            err_msg("sigqueue");

        exit(0);
    };


    sleep(1);
    if (sigprocmask(SIG_UNBLOCK, &sigmask, NULL)==-1)
        err_msg("sigprocmask");


    int status, waitpid;
    while ((waitpid=wait(&status)==-1) && errno==EINTR) {};
    if (WIFEXITED(status))
        printf("Child returned!\n");
    else if (WIFSIGNALED(status))
        printf("Child interrupted by signal!");

}
