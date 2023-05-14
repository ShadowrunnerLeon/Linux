#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>

void err_msg(char *msg) 
{
    perror(msg);
    exit(EXIT_FAILURE);
}

int main() 
{
    struct ifaddrs *addrs;

    if (getifaddrs(&addrs) == -1) err_msg("getifaddrs");

    struct ifaddrs *addr = addrs;
    while (addr) 
    {
        int family = addr->ifa_addr->sa_family;
        if (family == AF_INET || family == AF_INET6)  
        {
            printf("%s - %s", addr->ifa_name, family == AF_INET ? "IPV4" : "IPV6");
            char ap[100];
            const int family_size = family == AF_INET ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6);
            getnameinfo(addr->ifa_addr, family_size, ap, sizeof(ap), 0, 0, NI_NUMERICHOST);
            printf(" - %s\n", ap);
        }
        addr = addr->ifa_next;
    }

    freeifaddrs(addrs);
}