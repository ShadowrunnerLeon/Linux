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

int sockfd;

void *get_addr(struct sockaddr *sa) {
    if (sa->sa_family==AF_INET)
        return &(((struct sockaddr_in*)sa)->sin_addr);
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
};

void *pthread_func(void *arg) {
    int numbytes;
    char buf[BUFSIZE];
    struct sockaddr addr_from;
    socklen_t addr_size = sizeof addr_from;

    if ((numbytes = recvfrom(sockfd, buf, BUFSIZE-1, 0, &addr_from, &addr_size))==-1) {
        perror("recvfrom");
        exit(-1);
    }

    char str[INET_ADDRSTRLEN];
    if (inet_ntop(AF_INET, get_addr(&addr_from), str, INET_ADDRSTRLEN)==NULL) {
        perror("inet_ntop");
        exit(-1);
    }
    buf[numbytes] = '\0';
    printf("pthread[%d]: server received from %s: %s\n", (int)arg, str, buf);

    printf("pthread[%d]: server connected with client, sending message to %s\n", (int)arg, str);

    if (sendto(sockfd, "Hello, client!", 15, 0, &addr_from, addr_size)==-1) {
        perror("sendto");
        exit(-1);
    }
};

int main() {
    struct addrinfo hints, *res, *p;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

    int rv;
    if ((rv = getaddrinfo(NULL, PORT, &hints, &res))!=0) {
        printf("getaddrinfo: %s\n", gai_strerror(rv));
        exit(-1);
    }

    for (p = res; p!=NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol))==-1) {
            perror("server: sockfd");
            continue;
        }

        int yes = 1;
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int))==-1) {
            perror("setsockopt");
            exit(-1);
        }
        
        if (bind(sockfd, p->ai_addr, p->ai_addrlen)==-1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    if (p==NULL) {
        printf("p: null pointer\n");
        exit(-1);
    }

    freeaddrinfo(res);

    printf("Server ready to receving\n");
    pthread_t pthreads[4];

    for (int i=0; i<4; ++i) 
        if (pthread_create(&pthreads[i], NULL, pthread_func, (void*)i)!=0) {
            perror("pthread_create");
            exit(-1);
        }

    for (int i=0; i<4; ++i) 
        if (pthread_join(pthreads[i], NULL)!=0) {
            perror("pthread_join");
            exit(-1);
        }

    close(sockfd);
}