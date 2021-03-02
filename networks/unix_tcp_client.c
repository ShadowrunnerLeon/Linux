#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define UN_SOCK_PATH "/tmp/un_serv"
#define BACKLOG 5

void err_msg(char *msg) {
    perror(msg);
    exit(-1);
}

int main() {
    struct sockaddr_un addr;
    int sfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sfd == -1)
        err_msg("socket");

    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, UN_SOCK_PATH, sizeof(addr.sun_path) - 1);

    if (connect(sfd, (struct sockaddr*)&addr, sizeof(struct sockaddr_un)) == -1)
        err_msg("connect");

    if (write(sfd, "Hello, server!", 15) == -1)
        err_msg("write");

    if (close(sfd) == -1)
        err_msg("close");    

}