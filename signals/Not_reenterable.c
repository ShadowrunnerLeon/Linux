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

char *text = "Hello!";

void sighandler(int sig) 
{
    text = "Sighandler!";
}

void err_msg(char *msg) 
{
    perror(msg);
    exit(EXIT_FAILURE);
}

int main() 
{
    if (signal(SIGUSR1, sighandler) == SIG_ERR) err_msg("signal");

    pid_t pid = fork();

    if (pid == -1)
    {
        err_msg("fork");
    }
    else if (!pid) 
    {
        if (kill(getppid(), SIGUSR1) == -1) err_msg("kill");
        exit(EXIT_SUCCESS);
     }

    printf("%s\n", text);
    printf("%s\n", text);
    printf("%s\n", text);

    int status, waitpid;

    while ((waitpid=wait(&status)) == -1 && errno == EINTR) {}

    if (waitpid == -1) err_msg("wait");

    if (WIFEXITED(status))
    {
        printf("Child returned\n");
    }
    else if (WIFSIGNALED(status))
    {
        printf("Child interrupted by signal");
    }
}
