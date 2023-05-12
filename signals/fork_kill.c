#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

void sighandler(int sig) 
{
    printf("SIGFPE in child process!\n");
}

void err_msg(char *msg) 
{
    perror(msg);
    exit(EXIT_FAILURE);
}

int main() 
{
    if (signal(SIGFPE, sighandler) == SIG_ERR) err_msg("signal");

    pid_t pid = fork();

    if (!pid) 
    {
        printf("Hello!\n");
        exit(EXIT_SUCCESS);
    }
    else if (pid == -1)
    {
        err_msg("fork");
    }

    if (kill(pid, SIGFPE) == -1) err_msg("kill");

    int status;
    if (wait(&status) == -1)
    {
        err_msg("wait");
    }
    if (WIFEXITED(status))
    {
        printf("Child process returned!\n");
    }
}
