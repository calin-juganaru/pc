/*
 * Protocoale de comunicatii
 * Laborator 7 - TCP
 * Echo Server
 * server.c
 */

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

#include "helpers.h"

int main(int argc, char *argv[])
{
    int listenfd = 0, connfd = 0, res;
    char buff[BUFLEN];
    struct sockaddr_in server_addr;
    socklen_t addrlen = sizeof(server_addr);

    if(argc != 3)
    {
        printf("\n Usage: %s <ip> <port>\n", argv[0]);
        return 1;
    }

   // deschidere socket
    connfd = socket(AF_INET, SOCK_STREAM, 0);
    if (connfd < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // completare informatii despre adresa serverului
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(atoi(argv[2]));

    // legare proprietati de socket
    int status = bind(connfd, (struct sockaddr*)&server_addr, addrlen);

    // ascultare de conexiuni
    listen(connfd, 22);
    listenfd = accept(connfd, (struct sockaddr*) &server_addr, &addrlen);
    DIE(listenfd < 0, "nu-i de bine...");

    // acceptarea unei conexiuni, primirea datelor, trimiterea raspunsului

    while (1)
    {
        res = recv(listenfd, buff, sizeof(buff), 0);
        if (res == 0)
        {
            break;
        }
        else if (res > 0)
        {
            printf("am primit %s\n", buff);
            send(listenfd, buff, strlen(buff), 0);
        }
        else
        {
            printf("Hopa, avem probleme\n");
        }
    }

    // inchidere socket(i)
    close(connfd);
    close(listenfd);
    return 0;
}
