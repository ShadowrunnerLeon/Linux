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
#include <limits.h>

void err_msg(char *msg) 
{
    perror(msg);
    exit(EXIT_FAILURE);
}

void sighandler(int sig, siginfo_t *siginfo, void *ucontext) 
{
    printf("Sighandler!\n");
    printf("Signal number: %d\n", siginfo->si_signo);

    if (siginfo->si_code == FPE_INTDIV) printf("Signal code: Int/0\n");
        
    exit(EXIT_FAILURE);
}

int main() 
{
    struct sigaction sa;
    sa.sa_sigaction = sighandler;
    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGFPE, &sa, NULL) == -1) err_msg("sigaction");

    int dig1 = 5, dig2 = 0;
    printf("%d", dig1/dig2);
}
