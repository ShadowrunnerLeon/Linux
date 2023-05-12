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

void new_abort() 
{
    if (sigaction(SIGABRT, SIG_DFL, NULL) == -1) err_msg("sigaction");
    raise(SIGABRT);
}

void sighandler(int sig) 
{
    printf("Sighandler!");
    new_abort();
}

int main() 
{
    struct sigaction sa;
    sa.sa_flags = 0;
    sa.sa_handler = sighandler;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGUSR1, &sa, NULL) == -1) err_msg("sigaction");
    raise(SIGUSR1);
}
