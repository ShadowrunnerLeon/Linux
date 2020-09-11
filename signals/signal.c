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

//Глобальный флаг обработчика
//volatile sig_atomic_t flag; 

void sighandler(int sig) {
    if (sig==SIGSEGV)
       printf("Segfault!\n");
    else
       printf("SIGFPE!\n");

    exit(-1);
};

void err_msg(char *str) {
    perror(str);
    exit(-1);
};

int main() {

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
