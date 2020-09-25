#define _GNU_SOURCE
#define MAX_DESCRIPTORS 30

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/capability.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <dirent.h>
#include <ftw.h>

static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

static char *keyword;
static bool found = 0;

void err_msg(char *pathname) {
    perror(pathname);
    exit(-1);
}

/**обработчик ресурсов**/
void CleanUp(void *arg) {
    if (pthread_mutex_unlock(&mtx))
        err_msg("pthread_mutex_unlock");
};

static int pthread_search(const char *pathname, const struct stat *statbuf, int typeflag, struct FTW *ftwbuf) {
    printf("%s\n", pathname);
    pthread_testcancel();

    if (typeflag==FTW_F && !strcmp(basename(pathname), keyword)) {
        
        pthread_cleanup_push(CleanUp, NULL);

        if (pthread_mutex_lock(&mtx))
            err_msg("pthread_mutex_lock");

        found = 1;

        if (pthread_mutex_unlock(&mtx))
            err_msg("pthread_mutex_unlock");

        if (pthread_cond_broadcast(&cond))
            err_msg("pthread_cond_signal");

        pthread_testcancel();

        pthread_cleanup_pop(1);

        printf("File found %s\n", pathname);
        return FTW_STOP;
    }
    else
        return FTW_CONTINUE;
}

static void *threadfunc(void *arg) {
    pthread_testcancel();
    if (nftw((char*)arg, pthread_search, MAX_DESCRIPTORS, FTW_MOUNT | FTW_ACTIONRETVAL)==-1)
        err_msg("nftw");
}

static int firstcall(const char *pathname, const struct stat *statbuf, int typeflag, struct FTW *ftwbuf) {
    pthread_t pthread;
    if (typeflag==FTW_D) {
        if (pthread_create(&pthread, NULL, threadfunc, (void*)pathname))
            err_msg("pthread_create");

        while (!found) 
            if (pthread_cond_wait(&cond, &mtx))
                err_msg("pthread_cond_wait");

        if (pthread_cancel(pthread))
            err_msg("pthread_cancel");

        if (pthread_join(pthread, NULL))
            err_msg("pthread_join");
    }
    else if (typeflag==FTW_F && pathname==keyword) {
        printf("File found %s\n", pathname);
        return FTW_STOP;
    }
    else
        return FTW_CONTINUE;
}

int main(int argc, char *argv[]) {
    /**--Настройка возможностей--
    cap_t cap;
    cap_value_t caplist = CAP_DAC_READ_SEARCH;

    cap = cap_get_proc();
    if (cap==NULL)
        err_msg("cap_get_proc");

    if (cap_set_flag(cap, CAP_EFFECTIVE, 1, &caplist, CAP_SET)==-1)
        err_msg("cap_set_flag");

    if (cap_set_proc(cap)==-1)
        err_msg("cap_set_proc");

    if (cap_free(cap)==-1)
        err_msg("cap_free");
    **/

    keyword = argv[2];

    /**Первый вызов - создаются потоки для обработки каталгов главной директории "/"
     * Далее каждый поток обходит своё дерево с помощью nftw()**/

    struct timespec tm1, tm2, fulltime;

    if (clock_gettime(CLOCK_MONOTONIC, &tm1)==-1)
        err_msg("clock_gettime");

    int res;
    if ((res=nftw(argv[1], firstcall, MAX_DESCRIPTORS, FTW_MOUNT | FTW_ACTIONRETVAL))==-1)
        err_msg("nftw");
    else if (res==0)
        printf("file not founded\n");

    if (clock_gettime(CLOCK_MONOTONIC, &tm2)==-1)
        err_msg("clock_gettime");

    fulltime.tv_sec  = ((tm2.tv_sec*1000000000 + tm2.tv_nsec) - (tm1.tv_sec*1000000000 + tm1.tv_nsec)) / 1000000000;
    fulltime.tv_nsec = ((tm2.tv_sec*1000000000 + tm2.tv_nsec) - (tm1.tv_sec*1000000000 + tm1.tv_nsec)) % 1000000000;    

    printf("Time: %ld.%ld\n", fulltime.tv_sec, fulltime.tv_nsec);    
}
