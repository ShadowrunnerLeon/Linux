#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#define UN_SOCK_PATH "/tmp/un_serv"
#define BUF_SIZE 255
#define BACKLOG 5

void err_msg(char *msg) 
{
    perror(msg);
    exit(EXIT_FAILURE);
}

void sighandler(int sig) 
{
    printf("Server terminated succesfully");
    exit(EXIT_SUCCESS);
}

int main() {
    struct sigaction sa;
    sa.sa_handler = sighandler;
    sa.sa_flags = 0;
    if (sigaction(SIGINT, &sa, NULL) == -1) err_msg("sigaction");

    struct sockaddr_un addr;
    int sfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sfd == -1) err_msg("socket");

    if (remove(UN_SOCK_PATH) == -1 && errno != ENOENT) err_msg("remove");

    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, UN_SOCK_PATH, sizeof addr.sun_path - 1);

    if (bind(sfd, (struct sockaddr*)&addr, sizeof(struct sockaddr_un)) == -1) err_msg("bind");

    if (listen(sfd, BACKLOG) == -1) err_msg("listen");

    char buf[BUF_SIZE];
    while (true) 
    {
        int new_fd;
        if ((new_fd = accept(sfd, NULL, NULL)) == -1) err_msg("accept");

        int numrd;
        while ((numrd = read(new_fd, buf, BUF_SIZE)) > 0) 
        {
            if (write(STDOUT_FILENO, buf, numrd) == -1) err_msg("write");
            printf("\n");
        }        
            
        if (numrd == -1) err_msg("read");
        if (close(new_fd) == -1) err_msg("close");    
    }

    if (close(sfd) == -1) err_msg("close");   
}