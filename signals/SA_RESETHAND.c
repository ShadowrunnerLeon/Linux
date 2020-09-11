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
    write(1, "SIGFPE", sizeof("SIGFPE"));
};

struct sigaction sa;

int main() {
    sa.sa_handler = sighandler;
    sa.sa_flags = SA_RESETHAND;

    if (sigaction(SIGFPE, &sa, NULL)==-1)
       err_msg("sigaction");

    int a=0;
    printf("%d", 5/a);
}
