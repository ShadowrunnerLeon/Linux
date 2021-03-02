#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netdb.h>  //getaddrinfo(), freeaddrinfo(), gai_strerror()
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFSIZE 255
#define PORT    80

void err_msg(char *msg) {
    perror(msg);
    exit(-1);
}

int main() {
    int sockfd;
    struct sockaddr_in broad_struct;
    struct in_addr broad_addr = {INADDR_BROADCAST};

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        err_msg("socket");

    int broad_flag = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broad_flag, sizeof broad_flag) == -1)
        err_msg("setcockopt");

    broad_struct.sin_family = AF_INET;
    broad_struct.sin_port = htons(PORT);
    broad_struct.sin_addr = broad_addr;
    memset(&broad_struct.sin_zero, '\0', sizeof(broad_struct.sin_zero));

    int numbytes;
    if ((numbytes = sendto(sockfd, "Broadcast packet", 17, 0, (struct sockaddr*)&broad_struct, sizeof broad_struct)) == -1)
        err_msg("sendto");

    printf("Send message to 255.255.255.255\n");

    if (close(sockfd) == -1)
        err_msg("close");
}