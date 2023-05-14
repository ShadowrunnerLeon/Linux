//for demonstration behaivor of unix_udp_server which connected with unix_udp_client_connected and
//recieve message from unix_udp_client

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
#define UN_CLIENT_PATH "/tmp/un_client_connect"
#define BUF_SIZE 255

void err_msg(char *msg) 
{
    perror(msg);
    exit(EXIT_FAILURE);
}

void sighandler(int sig) 
{
    printf("\nServer terminated succesfully\n");
    exit(EXIT_SUCCESS);
}

int main() {
    struct sigaction sa;
    sa.sa_handler = sighandler;
    sa.sa_flags = 0;
    if (sigaction(SIGINT, &sa, NULL) == -1) err_msg("sigaction");

    struct sockaddr_un addr, ret_addr;
    socklen_t ret_addr_size = sizeof ret_addr;
    char buf[BUF_SIZE];

    int sfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sfd == -1) err_msg("socket");

    if (remove(UN_SOCK_PATH) == -1 && errno != ENOENT) err_msg("remove");

    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, UN_SOCK_PATH, sizeof addr.sun_path - 1);

    if (bind(sfd, (struct sockaddr*)&addr, sizeof(struct sockaddr_un)) == -1) err_msg("bind");

    sleep(5);

    struct sockaddr_un caddr;

    memset(&caddr, 0, sizeof(struct sockaddr_un));
    caddr.sun_family = AF_UNIX;
    strncpy(caddr.sun_path, UN_CLIENT_PATH, sizeof(caddr.sun_path) - 1);

    if (connect(sfd, (struct sockaddr*)&caddr, sizeof caddr) == -1) err_msg("connect");

    while (true) 
    {
        if (recv(sfd, buf, BUF_SIZE, 0) == -1) err_msg("recv");

        printf("server received: %s\n", buf);

        if (send(sfd, "Hello, client!", 15, 0) == -1) err_msg("send"); 
    }

    if (close(sfd) == -1) err_msg("close");
}