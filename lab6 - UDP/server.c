/*
 * Protocoale de comunicatii:
 * Laborator 6: UDP
 * mini-server de backup fisiere
 */
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#include "helpers.h"
#define PORT 8080

int main(int argc, char **argv)
{
	puts("Pornire server");

    int socket_UDP;
    socklen_t addrlen;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    char buf[BUFLEN];
    int ret;
    int status = 0;
    int yes = 1;

    socket_UDP = socket(AF_INET, SOCK_DGRAM, 0);
    DIE(socket < 0, "Socket creation failed");

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (setsockopt(socket_UDP, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) < 0)
    {
        perror("setsockopt(SO_REUSEADDR) failed");
    }

    status = bind(socket_UDP, (struct sockaddr *)&server_addr, sizeof(struct sockaddr));
    if (status < 0)
    {
        perror("Socket Binding Failed");
        exit(EXIT_FAILURE);
    }

    addrlen = sizeof(client_addr);
    puts("Server asteapta pomana");

    while (1)
    {
        ret = recvfrom(socket_UDP, (char *)buf, sizeof(buf) - 1, 0, (struct sockaddr *)&client_addr, &addrlen);
        buf[ret] = '\0';
        if (!ret) break;

        puts("Clientul mi-a trimis pomana");
        puts(buf);

        sendto(socket_UDP, (char*)buf, ret, 0, (struct sockaddr *)&client_addr, addrlen);
    }

    puts("Oprire server");
    return 0;
}
