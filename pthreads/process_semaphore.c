#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <sys/wait.h>

int* counter;
sem_t* sem;

void err_msg(const char* msg) {
    perror(msg);
    exit(-1);
}

void change_counter(const char* process) {
    for (int index = 0; index < 100000; ++index) {
        if (sem_wait(sem) == -1)
            err_msg("sem_wait");

        int tmp = *counter;
        ++tmp;
        *counter = tmp;

        printf("%s: %d\n", process, *counter);

        if (sem_post(sem) == -1)
            err_msg("sem_post");
    }
}

int main() {
    counter = (int*)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);

    if (counter == MAP_FAILED)
        err_msg("mmap");

    *counter = 0;

    sem = (sem_t*)mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);

    if (sem == MAP_FAILED)
        err_msg("mmap");

    if (sem_init(sem, 1, 1) == -1)
        err_msg("sem_init");

    pid_t pid = fork();

    if (pid == -1)
        err_msg("fork");
    else if (!pid) {
        change_counter("child");
        exit(0);
    }

    change_counter("parent");

    int status;
    if (wait(&status) == (pid_t)-1)
        err_msg("wait");

    if (WIFEXITED(status))
        printf("Child returned successfully\n");
    else
        printf("Child returned not successfully\n");

    if (sem_destroy(sem) == -1)
        err_msg("sem_destroy");

    if (munmap(counter, sizeof(int)) == -1)
        err_msg("munmap");

    if (munmap(sem, sizeof(sem_t)) == -1)
        err_msg("munmap");
}
