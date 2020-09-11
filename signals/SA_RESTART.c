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

void sighandler (int sig) {
    printf("SIGUSR1!\n");
};


int main() {

    int fd = open("test.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd==-1)
        err_msg("open");


    struct sigaction sa;
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = sighandler;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGUSR1, &sa, NULL)==-1)
        err_msg("sigaction");


    pid_t pid = fork();
    if (pid==-1)
        err_msg("fork");
    else if (pid==0) {
        sleep(1);
        if (kill(getppid(), SIGUSR1)==-1)
            err_msg("kill");
        exit(0);
    };

    sleep(1);
    int wr = write(fd, "1po202898134809325085825835783bbbfjblsni4njbfbdhb2bb12nb4b43b5n3bnsdnfodnfobsbBSDB", sizeof("1po202898134809325085825835783bbbfjblsni4njbfbdhb2bb12nb4b43b5n3bnsdnfodnfobsbBSDB"));
    if (errno = EINTR)
        printf("Interrupted\n");
    else if (wr==-1)
        err_msg("write");

    close(fd);


    int status, waitpid;
    while ((waitpid=wait(&status)==-1) && errno==EINTR) {};
    if (WIFEXITED(status))
        printf("Child returned!\n");
    else if (WIFSIGNALED(status))
        printf("Child interrupted by signal!");

}
