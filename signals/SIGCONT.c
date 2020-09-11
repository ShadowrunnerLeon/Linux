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

void err_msg(char *str) {
    perror(str);
    exit(-1);
};

void sighandler(int sig) {
    printf("Sighandler!\n");
};

int main() {

    struct sigaction sa;
    sa.sa_handler = sighandler;
    sa.sa_flags = 0;
    if (sigemptyset(&sa.sa_mask)==-1)
        err_msg("sigemptyset");

    if (sigaction(SIGCONT, &sa, NULL)==-1)
        err_msg("sigaction");


    sigset_t sigmask;
    if (sigemptyset(&sigmask)==-1)
        err_msg("sigemptyset");

    if (sigaddset(&sigmask, SIGCONT)==-1)
        err_msg("sigaddset");

    if (sigprocmask(SIG_SETMASK, &sigmask, NULL)==-1)
        err_msg("sigprocmask");


    pid_t pid = fork();

    if (pid==-1)
        err_msg("fork");
    else if (pid==0) {
        if (kill(getppid(), SIGCONT)==-1)
            err_msg("kill");
        exit(0);
    };


    raise(SIGSTOP);


    printf("SIGCONT\n");

    if (sigprocmask(SIG_UNBLOCK, &sigmask, NULL)==-1)
        err_msg("sigprocmask");

    raise(SIGCONT);

    int status, waitpid;

    while ((waitpid=wait(&status))==-1 && errno==EINTR) {};

    if (waitpid==-1)
        err_msg("wait");

    if (WIFEXITED(status))
        printf("Child returned");
    else if (WIFSIGNALED(status))
        printf("Child interrupted by signal");
}
