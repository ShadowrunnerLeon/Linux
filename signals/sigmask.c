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

void sighandler(int sig) {
    if (sig==SIGFPE)
        write(1, "SIGFPE!\n", sizeof("SIGFPE!\n"));
    else
        write(1, "SIGSEGV!\n", sizeof("SIGSEGV!\n"));
};

int main() {
    sigset_t set;

    if (sigemptyset(&set)==-1)
        err_msg("sigemptyset");

    if (sigaddset(&set, SIGFPE)==-1)
        err_msg("sigaddset");

    if (sigaddset(&set, SIGSEGV)==-1)
        err_msg("sigaddset");

    if (sigaddset(&set, SIGINT)==-1)
        err_msg("sigaddset");

    if (sigprocmask(SIG_SETMASK, &set, NULL)==-1)
        err_msg("sigprocmask");

    if (signal(SIGFPE, sighandler)==SIG_ERR)
        err_msg("signal");

    if (signal(SIGSEGV, sighandler)==SIG_ERR)
        err_msg("signal");

    raise(SIGFPE);

    raise(SIGSEGV);

    printf("SIGFPE and SIGSEGV blocked\n");

    if (sigprocmask(SIG_UNBLOCK, &set, NULL)==-1)
        err_msg("sigprocmask");

    printf("Signals unblocked\n");

}
