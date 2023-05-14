#include <iostream>
#include <thread>
#include <vector>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

#define PORT "80"
#define BACKLOG 5
#define BUFSIZE 255

void err_msg(std::string msg) 
{
    perror(msg.c_str());
    exit(EXIT_FAILURE);
}

void thread_func(int id, int sendfd) 
{
    std::cout << "thread[" << id << "]: server connected with client, sending message" << std::endl;
    if (send(sendfd, "Hello, client!", 15, 0) == -1) err_msg("send");

    std::cout << "thread[" << id << "]: server ready to receving" << std::endl;

    int numbytes;
    char buf[BUFSIZE];
    if ((numbytes = recv(sendfd, buf, BUFSIZE, 0)) == -1) err_msg("recv");

    buf[numbytes] = '\0';

    std::cout << "thread[" << id << "]: server received: " << buf << std::endl;
}

class Server 
{
    private:
        int sfd;
        struct addrinfo hints;
        unsigned int thread_count;
        char *port, *msg;

    public:
        Server() 
        {
            hints.ai_family = AF_UNSPEC;
            hints.ai_socktype = SOCK_STREAM;
            hints.ai_flags = AI_PASSIVE;
            thread_count = std::thread::hardware_concurrency();
        }

        Server(int ipv, char *addr) 
        {
            switch(ipv)
            {
                case 4: hints.ai_family = AF_INET; break;
                case 6: hints.ai_family = AF_INET6; break;
                default: hints.ai_family = AF_UNSPEC;
            }

            hints.ai_socktype = SOCK_STREAM;
        }

        void Start() 
        {
            struct addrinfo *res, *p;
            int rv;

            if (hints.ai_flags == AI_PASSIVE) 
            {
                if ((rv = getaddrinfo(NULL, PORT, &hints, &res)) != 0) 
                {
                    printf("getaddrinfo: %s\n", gai_strerror(rv));
                    exit(EXIT_FAILURE);
                }
            }
            else 
            {
                if ((rv = getaddrinfo((char*)hints.ai_addr->sa_data, port, &hints, &res)) != 0) 
                {
                    printf("getaddrinfo: %s\n", gai_strerror(rv));
                    exit(EXIT_FAILURE);
                }
            }


            for (p = res; p != nullptr; p = p->ai_next) 
            {
                if ((sfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) 
                {
                    perror("server: sfd");
                    continue;
                }

                int yes = 1;
                if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) err_msg("setsockopt");
                
        
                if (bind(sfd, p->ai_addr, p->ai_addrlen) == -1) 
                {
                    close(sfd);
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

            if (listen(sfd, BACKLOG) == -1) err_msg("listen");
        }

        void Service() 
        {
            std::vector<std::thread> threads;
            int sendfd[thread_count];

            for (int i = 0; i < thread_count; ++i) 
            {
                if ((sendfd[i] = accept(sfd, NULL, 0)) == -1) err_msg("accept");

                std::thread thr(thread_func, i, sendfd[i]);
                threads.emplace_back(std::move(thr));
            }

            for (auto &thr : threads) thr.join();

            for (int i = 0; i < thread_count; ++i)
            {
                if (close(sendfd[i]) == -1) err_msg("close");
            }
        }

        void Stop() 
        {
            if (close(sfd) == -1) err_msg("close");
        }
};

int main() 
{
    Server server;
    std::cout << "Server starts" << std::endl;
    server.Start();
    std::cout << "Server handles requests" << std::endl;
    server.Service();
    std::cout << "Server stops" << std::endl;
    server.Stop();
}