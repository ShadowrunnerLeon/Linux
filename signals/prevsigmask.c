#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

void err_msg(char *str) {
    perror(str);
    exit(-1);
};

int main() {
    sigset_t set, prevset;

    if (sigemptyset(&set)==-1)
        err_msg("sigemptyset");

    if (sigemptyset(&prevset)==-1)
        err_msg("sigemptyset");

    if (sigaddset(&set, SIGFPE)==-1)
        err_msg("sigaddset");

    if (sigaddset(&set, SIGSEGV)==-1)
        err_msg("sigaddset");

    if (sigaddset(&set, SIGINT)==-1)
        err_msg("sigaddset");

    if (sigprocmask(SIG_SETMASK, &set, &prevset)==-1)
        err_msg("sigprocmask");

    printf("Set is empty\n");

    if (sigprocmask(SIG_UNBLOCK, &set, &prevset)==-1)
        err_msg("sigprocmask");

    int count = 0;
    for (int sig=1; sig<NSIG; sig++)
        if (sigismember(&prevset, sig))
            count++;

    if (count==3 && sigismember(&prevset, SIGFPE) && sigismember(&prevset, SIGSEGV) && sigismember(&prevset, SIGINT))
       printf("prevset=set\n");
}
