#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>

#define PORT "8080"
#define BACKLOG 15
#define BUFSIZE 1024

const char *status200 = "HTTP/1.1 200 OK\r\n"
        "Connection: close\r\n";

const char *status400 = "HTTP/1.1 400 Bad Request\r\n"
        "Connection: close\r\n"
        "Content-Length: 11\r\n\r\nBad Request";

const char *status404 = "HTTP/1.1 404 Not Found\r\n"
        "Connection: close\r\n"
        "Content-Length: 9\r\n\r\nNot Found";

int sockfdServer = -1, sockfdClient = -1;

/****************************************************/

static void sighandler(int sig) 
{
    if (sockfdServer != -1) close(sockfdServer);
    if (sockfdClient != -1) close(sockfdClient);
    exit(EXIT_SUCCESS);
}

static inline void printError(const char *msg) 
{
    perror(msg);
    exit(EXIT_FAILURE);
}

static inline void checkGetRequest(const char *request)
{
    if (strncmp("GET /", request, 5)) 
    {
        if (send(sockfdClient, status400, strlen(status400), 0) == -1) printError("send");
        printf("%s\n", status400);
    }
    else 
    {
        // extract path
        char *startPath = (char*)request + 4;
        char *endPath = strstr(request, "HTTP");
        char path[BUFSIZE / 8];
        int counter = 0;

        while (startPath + 1 != endPath) 
        {
            path[counter++] = *startPath++;
            if (counter == 128) break;
        }

        path[counter] = '\0';
        printf("%s\n", &path[1]);

        // case: "/"
        if (counter == 1)    
        {
            if (send(sockfdClient, status200, strlen(status200), 0) == -1) printError("send");
            if (send(sockfdClient, "Content-Length: 13\r\nContent-type: text/plain\r\n\r\nHello, world!", 62, 0) == -1) printError("send");
            printf("%s\n", status200);
        }
        else 
        {
            // &path[1] - path without '/' in beginning
            int fd;
            if ((fd = open(&path[1], O_RDONLY)) == -1) 
            {
                perror("open");
                if (send(sockfdClient, status404, strlen(status404), 0) == -1) printError("send");
                printf("%s\n", status404);
                return;
            }

            struct stat statStruct;
            if (fstat(fd, &statStruct) == -1) printError("fstat");
            int fileSize = statStruct.st_size;

            char buffer[fileSize];
            if (read(fd, buffer, fileSize) == -1 && errno != EINTR) printError("read");

            char httpResponse[BUFSIZE];
            strcat(httpResponse, status200);
            char contentLength[32];
            sprintf(contentLength, "Content-Length: %d\r\n", fileSize);
            strcat(httpResponse, contentLength);
            strcat(httpResponse, "Content-type: text/plain\r\n\r\n");
            strcat(httpResponse, buffer);
            if (send(sockfdClient, httpResponse, strlen(httpResponse), 0) == -1) printError("send");

            printf("%s\n", httpResponse);
        }
    }
}

int main() 
{
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = sighandler;
    sa.sa_flags = 0;

    if (sigaction(SIGINT, &sa, NULL) == -1) printError("sigaction");

    int reuseAddr = 1;
    struct addrinfo searchInfo, *resInfo, *iterator;

    memset(&searchInfo, 0, sizeof(searchInfo));
    searchInfo.ai_family = AF_UNSPEC;
    searchInfo.ai_socktype = SOCK_STREAM;
    searchInfo.ai_flags = AI_PASSIVE;

    int resGetAddrInfo;
    if (resGetAddrInfo = getaddrinfo(NULL, PORT, &searchInfo, &resInfo))
    {
        printf("getaddrinfo: %s\n", gai_strerror(resGetAddrInfo));
        exit(EXIT_FAILURE);
    }

    for (iterator = resInfo; iterator != NULL; iterator = iterator->ai_next) 
    {
        if ((sockfdServer = socket(iterator->ai_family, iterator->ai_socktype, iterator->ai_protocol)) == -1) 
        {
            perror("socket");
            continue;
        }

        if (setsockopt(sockfdServer, SOL_SOCKET, SO_REUSEADDR, &reuseAddr, sizeof(int)) == -1) printError("setsockopt");

        if (bind(sockfdServer, iterator->ai_addr, iterator->ai_addrlen) == -1) 
        {
            close(sockfdServer);
            perror("bind");
            continue;
        }

        break;
    }

    freeaddrinfo(resInfo);

    if (!iterator) 
    {
        printf("bind: failed\n");
        exit(EXIT_FAILURE);
    }

    printf("Server created socket and bound\n");

    if (listen(sockfdServer, BACKLOG) == -1) printError("listen");

    struct sockaddr_storage addrClient;
    int storageSize = sizeof(addrClient);
    char requestClient[BUFSIZE];

    printf("Server waits conection\n");

    while (1) 
    {
        if ((sockfdClient = accept(sockfdServer, (struct sockaddr*)&addrClient, &storageSize)) == -1) 
        {
            perror("accept");
            continue;
        }

        if ((recv(sockfdClient, requestClient, BUFSIZE, 0)) == -1 && errno != EINTR) printError("recv");

        checkGetRequest(requestClient);
    }
}