#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>

#define BUF_SIZE 255

void err_msg(char *msg) {
    perror(msg);
    exit(-1);
}

int main() {
    int sfd;
    struct sockaddr_un addr, ret_addr;
    socklen_t ret_addr_size = sizeof ret_addr;

    if ((sfd = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1)
        err_msg("socket");

    memset(&addr, 0, sizeof addr);
    addr.sun_family = AF_UNIX;
    strncpy(&addr.sun_path[1], "xyz", sizeof addr.sun_path - 2);

    if (bind(sfd, (struct sockaddr*)&addr, sizeof addr) == -1)
        err_msg("bind");

    char buf[BUF_SIZE];
    while (1) {
        if (recvfrom(sfd, buf, BUF_SIZE, 0, (struct sockaddr*)&ret_addr, &ret_addr_size) == -1)
            err_msg("recvfrom");

        printf("server received: %s\n", buf);

        if (sendto(sfd, "Hello, client!", 15, 0, (struct sockaddr*)&ret_addr, ret_addr_size) == -1)
            err_msg("sendto"); 
    }

    if (close(sfd) == -1)
        err_msg("close");
}