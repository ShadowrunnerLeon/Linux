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

void sighandler(int sig) {
    printf("SIGUSR1\n");
};

int main() {
    sigset_t sigmask;
    sigemptyset(&sigmask);
    sigaddset(&sigmask, SIGUSR1);
    sigaddset(&sigmask, SIGUSR2);

    if (sigprocmask(SIG_SETMASK, &sigmask, NULL)==-1)
        err_msg("sigprocmask");

    int sigfd;
    if ((sigfd=signalfd(-1, &sigmask, 0))==-1)
        err_msg("signalfd");


    pid_t pid = fork();
    if (pid==-1)
        err_msg("fork");
    else if (pid==0) {
        if (kill(getppid(), SIGUSR1)==-1)
            err_msg("kill");
        if (kill(getppid(), SIGUSR2)==-1)
            err_msg("kill");
        exit(0);
    };


    struct signalfd_siginfo sigbuf;

    if (read(sigfd, &sigbuf, sizeof(sigbuf))==-1)
        err_msg("read");
    else
        printf("%d = %d\n", sigbuf.ssi_signo, SIGUSR1);

    if (read(sigfd, &sigbuf, sizeof(sigbuf))==-1)
        err_msg("read");
    else
        printf("%d = %d\n", sigbuf.ssi_signo, SIGUSR2);

    close(sigfd);


    int status, waitpid;
    while ((waitpid=wait(&status)==-1) && errno==EINTR) {};
    if (WIFEXITED(status))
        printf("Child returned!\n");
    else if (WIFSIGNALED(status))
        printf("Child interrupted by signal!");

}
