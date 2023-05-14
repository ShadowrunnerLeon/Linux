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

struct pthread_arg 
{
    int id, fd;
}; 

void *pthread_func(void *arg) 
{
    int id = ((struct pthread_arg*)arg)->id;
    int fd = ((struct pthread_arg*)arg)->fd;

    printf("pthread[%d]: server connected with client, sending message\n", id);

    if (send(fd, "Hello, client!", 15, 0) == -1) 
    {
        perror("send");
        exit(EXIT_FAILURE);
    }

    printf("pthread[%d]: server ready to receving\n", id);

    int numbytes;
    char buf[BUFSIZE];
    if ((numbytes = recv(fd, buf, BUFSIZE, 0)) == -1) 
    {
        perror("recv");
        exit(EXIT_FAILURE);
    }

    buf[numbytes] = '\0';

    printf("pthread[%d]: server received: %s\n", id, buf);
}

int main() {
    int sockfd;
    struct addrinfo hints, *res, *p;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int rv;
    if ((rv = getaddrinfo(NULL, PORT, &hints, &res)) != 0) 
    {
        printf("getaddrinfo: %s\n", gai_strerror(rv));
        exit(EXIT_FAILURE);
    }

    for (p = res; p != NULL; p = p->ai_next) 
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) 
        {
            perror("server: sockfd");
            continue;
        }

        int yes = 1;
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) 
        {
            perror("setsockopt");
            exit(EXIT_FAILURE);
        }
        
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) 
        {
            close(sockfd);
            perror("server: bind");
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

    if (listen(sockfd, BACKLOG) == -1) 
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    pthread_t pthreads[4];
    int new_fd[4];
    struct pthread_arg arg;

    for (int i=0; i<4; ++i) 
    {
        //returned struct sockaddr* doesn`t matter in this case
        if ((new_fd[i] = accept(sockfd, NULL, 0)) == -1) 
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        struct pthread_arg arg = {i, new_fd[i]};

        if (pthread_create(&pthreads[i], NULL, pthread_func, (void*)&arg) != 0) 
        {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }

    }

    //case of memory overflow because not closed fds in time
    for (int i = 0; i < 4; ++i) 
    {
        if (pthread_join(pthreads[i], NULL) != 0) 
        {
            perror("pthread_join");
            exit(EXIT_FAILURE);
        }

        close(new_fd[i]);
    }

    close(sockfd);
}