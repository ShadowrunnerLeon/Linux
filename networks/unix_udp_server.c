#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#define UN_SOCK_PATH "/tmp/un_serv"
#define UN_CLIENT_PATH "/tmp/un_client"
#define BUF_SIZE 255

void err_msg(char *msg) {
    perror(msg);
    exit(-1);
}

void sighandler(int sig) {
    printf("\nServer terminated succesfully\n");
    exit(0);
}

int main() {
    struct sigaction sa;
    sa.sa_handler = sighandler;
    sa.sa_flags = 0;
    if (sigaction(SIGINT, &sa, NULL) == -1)
        err_msg("sigaction");

    struct sockaddr_un addr, ret_addr;
    socklen_t ret_addr_size = sizeof ret_addr;
    char buf[BUF_SIZE];

    int sfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sfd == -1)
        err_msg("socket");

    if (remove(UN_SOCK_PATH) == -1 && errno != ENOENT)
        err_msg("remove");

    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, UN_SOCK_PATH, sizeof addr.sun_path - 1);

    if (bind(sfd, (struct sockaddr*)&addr, sizeof(struct sockaddr_un)) == -1)
        err_msg("bind");

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