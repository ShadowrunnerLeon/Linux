#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <pthread.h>
#include <errno.h>

void err_msg(char *str) {
    perror(str);
    exit(-1);
};

static __thread char *str_dirname;
static __thread char *str_basename;

char *pthread_dirname(char *pathname) {

    if (pathname == NULL)
        return ".";

    int path_step = 0, str_step = 0, path_len = strlen(pathname);
    bool SlashFlag = 0;

    str_dirname = malloc(path_len);
    if (str_dirname==NULL)
        err_msg("malloc");

    while (path_step < path_len) {
        if (pathname[path_step]=='/') {
            str_dirname[str_step] = '/';
            str_step++;
            while (pathname[path_step]=='/' && path_step<path_len)
                path_step++; 
        }
        else {
            while (pathname[path_step]!='/' && path_step<path_len) {
                str_dirname[str_step] = pathname[path_step];
                str_step++;
                path_step++;
            };
        };
    };

    if (path_len==1 && pathname[0]=='/')
        return "/";

    if (str_dirname[str_step-1]=='/')
        str_step--;
    
    int slash_step = 0;
    for (int step = 0; step<str_step; step++) {
        if (str_dirname[step]=='/') {
            SlashFlag = 1;
            slash_step = step;
        };
    };

    if (!SlashFlag)
        return ".";

    if (!slash_step)
        str_dirname[str_step] = '\0';
    else 
        str_dirname[slash_step] = '\0';

    return str_dirname;
};

char *pthread_basename(char *pathname) {

    if (pathname == NULL)
        return ".";

    int path_step = 0, str_step = 0, path_len = strlen(pathname);
    bool SlashFlag = 0;

    char *str = malloc(sizeof(pathname));
    if (str==NULL)
        err_msg("malloc");

    while (path_step < path_len) {
        if (pathname[path_step]=='/') {
            SlashFlag = 1;
            str[str_step] = '/';
            str_step++;
            while (pathname[path_step]=='/' && path_step<path_len)
                path_step++; 
        }
        else {
            while (pathname[path_step]!='/' && path_step<path_len) {
                str[str_step] = pathname[path_step];
                str_step++;
                path_step++;
            };
        };
    };


    if (path_len==1 && pathname[0]=='/') {
        str[str_step] = '/0';
        free(str);
        return "/";
    };

    if (str[str_step-1]=='/')
        str_step--;
    
    str[str_step] = '\0';
    
    int slash_step = -1;
    for (int step = 0; step<str_step; step++) {
        if (str[step]=='/')
            slash_step = step;
    };

    str_basename = malloc(sizeof(str));
    if (str_basename==NULL)
        err_msg("malloc");

    if (slash_step==-1) {
        for (int step = 0; step<str_step; step++)
            str_basename[step] = str[step]; 
        str_basename[str_step] = '\0';
    }
    else {
        int count = 0;
        for (int step = slash_step+1; step<str_step; step++) {
            str_basename[count] = str[step];
            count++;
        };
        str_basename[str_step-slash_step] = '\0';
    };

    free(str);
    return str_basename;
};

int main() {
    //char *str = NULL;
    char *pathname1 = "/home/leonid/.profile";
    char *pathname2 = "/home/leonid/pthread_search.c";

    pthread_t pthread1, pthread2;

    if (pthread_create(&pthread1, NULL, basename, pathname1))
        err_msg("pthread_create");

    if (pthread_create(&pthread2, NULL, basename, pathname2))
        err_msg("pthread_create");

    char *ret1, *ret2;
    if (pthread_join(pthread1, &ret1))
        err_msg("pthread_join");

    if (pthread_join(pthread2, &ret2))
        err_msg("pthread_join");

    printf("%s\n", ret1);
    printf("%s\n", ret2);
}