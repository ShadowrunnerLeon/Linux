#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/socket.h> //bind()
#include <netdb.h>      //getaddrinfo(), freeaddrinfo(), gai_strerror()
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define BUFSIZE 255
#define BACKLOG 10
#define PORT    "80"

void err_msg(char *msg) {
    perror(msg);
    exit(-1);
}

int main() {
    int sockfd;
    struct addrinfo hints, *res, *p; 
    
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int rv;
    if ((rv = getaddrinfo(INADDR_ANY, PORT, &hints, &res))!=0) {
        printf("getaddrinfo: %s\n", gai_strerror(rv));
        exit(-1);
    }

    for (p = res; p!=NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol))==-1) {
            perror("server: sockfd");
            continue;
        }

        if (connect(sockfd, (struct sockaddr*)p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("connect");
            continue;
        }

        break;
    }

    if (p==NULL) {
        printf("p: null pointer\n");
        exit(-1);
    }

    freeaddrinfo(res);

    int numbytes;
    char buf[BUFSIZE];
    if ((numbytes = recv(sockfd, buf, BUFSIZE, 0))==-1) {
        perror("recv");
        exit(-1);
    }

    buf[numbytes] = '\0';

    printf("Client received: %s\n", buf);

    if (send(sockfd, "Hello, server!", 15, 0)==-1) {
        perror("send");
        exit(-1);
    }

    if (close(sockfd) == -1)
        err_msg("close");

}