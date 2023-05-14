#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define UN_SERV_PATH "/tmp/un_serv"
#define UN_CLIENT_PATH "/tmp/un_client_connect"
#define BUF_SIZE 255

void err_msg(char *msg) 
{
    perror(msg);
    exit(EXIT_FAILURE);
}

int main() {
    struct sockaddr_un saddr, caddr;
    struct sockaddr ret_addr;
    socklen_t ret_addr_size = sizeof ret_addr;
    char buf[BUF_SIZE];

    int sfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sfd == -1) err_msg("socket");

    if (remove(UN_CLIENT_PATH) == -1 && errno != ENOENT) err_msg("remove");

    memset(&caddr, 0, sizeof(struct sockaddr_un));
    caddr.sun_family = AF_UNIX;
    strncpy(caddr.sun_path, UN_CLIENT_PATH, sizeof(caddr.sun_path) - 1);

    if (bind(sfd, (struct sockaddr*)&caddr, sizeof caddr) == -1) err_msg("bind");

    memset(&saddr, 0, sizeof(struct sockaddr_un));
    saddr.sun_family = AF_UNIX;
    strncpy(saddr.sun_path, UN_SERV_PATH, sizeof(saddr.sun_path) - 1);

    sleep(5);

    if (connect(sfd, (struct sockaddr*)&saddr, sizeof(saddr)) == -1) err_msg("connect");

    if (send(sfd, "Hello, server!", 15, 0) == -1) err_msg("sendto");

    if (recv(sfd, buf, BUF_SIZE, 0) == -1) err_msg("recvfrom");

    printf("#1: Client received: %s\n", buf);

    if (recv(sfd, buf, BUF_SIZE, 0) == -1) err_msg("recvfrom");

    printf("#2: Client received: %s\n", buf);

    if (close(sfd) == -1) err_msg("close");    
}