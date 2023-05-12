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
    printf("Sighandler!");
    exit(EXIT_FAILURE);
}

void StackOverflow(int num) 
{
    int arr[100000];
    printf("Call %d: %p\n", num, &arr);
    StackOverflow(num+1);
}

int main() 
{
    stack_t alt_stack;
    alt_stack.ss_sp = malloc(SIGSTKSZ);

    if (!alt_stack.ss_sp) err_msg("malloc");

    alt_stack.ss_flags = 0;
    alt_stack.ss_size = SIGSTKSZ;

    if (sigaltstack(&alt_stack, NULL) == -1) err_msg("signalstack");

    struct sigaction sa;
    sa.sa_flags = SA_ONSTACK;
    sa.sa_handler = sighandler;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGSEGV, &sa, NULL) == -1) err_msg("sigaction");
    StackOverflow(1);
}
