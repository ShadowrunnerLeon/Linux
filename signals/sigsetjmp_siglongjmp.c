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

static volatile sig_atomic_t flagJump = 0;
static sigjmp_buf senv;
//static jmp_buf env;

void err_msg(char *msg) 
{
    perror(msg);
    exit(EXIT_FAILURE);
}

void sighandler(int sig) 
{
    if (!flagJump)
    {
        printf("flagJump = 0\n");
    }
    else 
    {
        printf("flagJump = 1\n");
        siglongjmp(senv, 1);
        //longjmp(env, 1);
    }
}

int main() 
{
    struct sigaction sa;
    sa.sa_handler = sighandler;
    sa.sa_flags = 0;

    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGUSR1, &sa, NULL) == -1) err_msg("sigaction");

    printf("Sigmask is empty\n");

    pid_t pid = fork();

    if (pid == -1)
    {
        err_msg("fork");
    }
    else if (!pid) 
    {
        for (int i = 0; i < 2; ++i) 
        {
            if (kill(getppid(), SIGUSR1) == -1) err_msg("kill");
            sleep(1);
        }

        exit(EXIT_SUCCESS);
    }

    if (!sigsetjmp(senv, 1)) flagJump = 1;

    //if (!setjmp(env)) flagJump = 1;

    sigset_t current_mask;
    if (sigprocmask(0, NULL, &current_mask) == -1) err_msg("sigprocmask");

    for (int n = 1; n < NSIG; ++n) 
    {
        int sigstatus = sigismember(&current_mask, n);

        if (sigstatus == -1)
        {
            err_msg("sigismember");
        }
        else if (sigstatus == 1)
        {
            printf("%s\n", strsignal(n));
        }
    }

    int status, waitpid;

    while ((waitpid = wait(&status)) == -1 && errno == EINTR) {}

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
