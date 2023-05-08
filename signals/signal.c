//SIGBUS, SIGSEGV
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

extern const char *const sys_siglist[];

void sighandler(int sig) 
{
    printf(sig == SIGSEGV ? "Segfault!\n" : "SIGFPE!\n");
    exit(EXIT_FAILURE);
}

void err_msg(char *msg) 
{
    perror(msg);
    exit(EXIT_FAILURE);
}

int main() 
{
    //-------SIGSEGV---------//
    //int *dig;
    //printf("%d", *dig);
    //----------------------//

    //-------SIGFPE--------//
    //int dig1 = 0;
    //printf("%d", 5/dig1);
    //--------------------//

    printf("%s\n", sys_siglist[SIGCONT]);
}
