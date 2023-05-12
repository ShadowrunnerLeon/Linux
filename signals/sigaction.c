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

void err_msg(char *str) 
{
    perror(str);
    exit(EXIT_FAILURE);
}

void sighandler(int sig) 
{
    for (int i = 1; i < 10; ++i) 
    {
        char s = i + '0';
        write(1, &s, sizeof(s));
        sleep(1);
    }
}

struct sigaction sa;

int main() 
{
    sa.sa_handler = sighandler;
    sa.sa_flags = SA_NODEFER;

    if (sigaction(SIGUSR1, &sa, NULL) == -1) err_msg("sigaction");

    pid_t pid = fork();

    if (pid == -1)
    {
        err_msg("fork");
    }
    else if (!pid) 
    {
        for (int i = 0; i < 15; ++i)
        {
            if (kill(getppid(), SIGUSR1) == -1) err_msg("kill");
        }

        exit(EXIT_SUCCESS);
    }

    raise(SIGUSR1);

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
