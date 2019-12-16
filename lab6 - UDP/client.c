/*
 * Protocoale de comunicatii:
 * Laborator 6: UDP
 * client mini-server de backup fisiere
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

void usage(char *file)
{
	fprintf(stderr, "Usage: %s ip_server port_server file\n", file);
	exit(0);
}

int main(int argc, char **argv)
{
    puts("Pornire client");
    FILE* fd = fopen("fileX", "r");

    int sock_fd;
    struct sockaddr_in server_addr;
    socklen_t addrlen = sizeof(server_addr);
    int status = 0;
    int yes = 1;

    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    status = inet_aton("127.0.0.1", &server_addr.sin_addr);
    if (status <= 0)
    {
        perror("inet_aton");
        exit(-1);
    }

    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) < 0)
    {
        perror("setsockopt(SO_REUSEADDR) failed");
    }

    while (1)
    {
        int size = 50;
        char* buff = (char*)malloc(size);
        size_t result = fread(buff, 1, size, fd);
        if (buff[0] == '\0')
            break;

        sendto(sock_fd, buff, strlen(buff), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
        free(buff);

        char buf[size];
        recvfrom(sock_fd, (char *)buf, size, 0, (struct sockaddr *)&server_addr, &addrlen);

        //puts("Clientul a primit pomana");
        puts(buf);
    }

    puts("Oprire client");
    fclose(fd);

    return 0;
}
