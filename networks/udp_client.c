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
#define PORT    "80"

void *get_addr(struct sockaddr *sa) 
{
    if (sa->sa_family==AF_INET)
        return &(((struct sockaddr_in*)sa)->sin_addr);
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
};

int main(int argc, char* argv[]) 
{
    int sockfd;
    struct addrinfo hints, *res, *p;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    int rv;
    if ((rv = getaddrinfo(INADDR_ANY, PORT, &hints, &res)) != 0) 
    {
        printf("getaddrinfo: %s\n", gai_strerror(rv));
        exit(EXIT_FAILURE);
    }

    for (p = res; p != NULL; p = p->ai_next) 
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) 
        {
            perror("client: sockfd");
            continue;
        }

        break;
    }

    if (!p) 
    {
        printf("p: null pointer\n");
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(res);

    printf("Client send message\n"); 
    
    if (sendto(sockfd, "Hello, server!", 15, 0, p->ai_addr, p->ai_addrlen) == -1) 
    {
        perror("sendto");
        exit(EXIT_FAILURE);
    }

    char str_to[INET_ADDRSTRLEN];
    if (!inet_ntop(AF_INET, get_addr((struct sockaddr*)&p), str_to, INET_ADDRSTRLEN)) 
    {
        perror("inet_ntop");
        exit(EXIT_FAILURE);
    }

    printf("Sending to %s terminated\n", str_to);

    struct sockaddr addr_from;
    socklen_t addr_size = sizeof addr_from;
    char buf[BUFSIZE];
    int numbytes;
    if ((numbytes = recvfrom(sockfd, buf, BUFSIZE-1, 0, &addr_from, &addr_size)) == -1) 
    {
        perror("recvfrom");
        exit(EXIT_FAILURE);
    }

    char str_from[INET_ADDRSTRLEN];
    if (!inet_ntop(AF_INET, get_addr(&addr_from), str_from, INET_ADDRSTRLEN)) 
    {
        perror("inet_ntop");
        exit(EXIT_FAILURE);
    }
    
    buf[BUFSIZE] = '\0';
    printf("Client received from %s: %s\n", str_from, buf);

    close(sockfd);
}