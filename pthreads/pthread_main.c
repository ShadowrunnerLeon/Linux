#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>

void err_msg(char *str) {
    perror(str);
    exit(-1);
};

void *thread_func(void *arg) {
    for (int i=0; i<5; i++) {
        printf("Pthread %d\n", (int)arg);
        sleep(1);
    };
};

int main() {

    pthread_t pthread1, pthread2;

    if (pthread_create(&pthread1, NULL, thread_func, (void*)1))
        err_msg("pthread_create");

    if (pthread_detach(pthread1))  
        err_msg("pthread_detach");    

    if (pthread_create(&pthread2, NULL, thread_func, (void*)2))
        err_msg("pthread_create");

    if (pthread_detach(pthread2))  
        err_msg("pthread_detach");

    printf("Main thread\n");

    if (pthread_detach(pthread_self()))  
        err_msg("pthread_detach");

    pthread_exit(NULL);  

}