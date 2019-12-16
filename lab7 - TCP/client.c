/*
 * Protocoale de comunicatii
 * Laborator 7 - TCP
 * Echo Server
 * client.c
 */

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

#include "helpers.h"

#define PORT 8080

typedef enum {false, true} bool;

int main(int argc, char *argv[])
{
    int sock_fd = 0, res;
    char buff[BUFLEN];
    struct sockaddr_in server_addr;

    if(argc != 3)
    {
        printf("\n Usage: %s <ip> <port>\n", argv[0]);
        return 1;
    }

    // deschidere socket
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // completare informatii despre adresa serverului
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(atoi(argv[2]));
    socklen_t addrlen = sizeof(server_addr);

    // conectare la server
    res = connect(sock_fd, (struct sockaddr*)&server_addr, addrlen);
    //DIE(res < 0, "nu-i de bine...");

    // citire de la tastatura, trimitere de cereri catre server, asteptare raspuns
    while (true)
    {
        scanf("%s", buff);
        printf("trimit %s\n", buff);
        if (!strcmp(buff, "exit")) break;
        res = send(sock_fd, buff, strlen(buff), 0);

        recv(sock_fd, buff, sizeof(buff), 0);
        printf("a mers %s\n", buff);
    }

    // inchidere socket
    memset(buff, 0, sizeof(buff));
    res = send(sock_fd, buff, 0, 0);
    close(sock_fd);
    return 0;
}
