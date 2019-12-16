#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"

int main(int argc, char *argv[])
{
    char *message;
    char *response;
    int sockfd;

    /*
    *   Ex 0: Get cs.curs.pub.ro
    *
    *   Pas 1: Se deschide conexiunea (open_connection)
    *   Pas 2: Se creaza mesajul de request (compute_get_request)
    *   Pas 3: Se trimite la server mesajul (send_to_server)
    *   Pas 4: Se primeste raspuns de la server (receive_from_server)
    *   Pas 5: Se inchide conexiunea cu serverul (close_connection)
    */

    /*
        Ex 1: Get videos
    */

    sockfd = open_connection(IP_SERVER, PORT_SERVER, AF_INET, SOCK_STREAM, 0);
    message = compute_get_request(IP_SERVER, "/data/2.5/weather", "q=bucharest&APPID=80f21fc11662726c0c3c8c5512d89b3a");
    puts(message);
    send_to_server(sockfd, message);

    response = receive_from_server(sockfd);
    puts(response);

    close_connection(sockfd);
    sockfd = open_connection(IP_SERVER, PORT_SERVER, AF_INET, SOCK_STREAM, 0);

    message = compute_post_request(IP_SERVER, "/videos", "id=13&name=Noroc!");
    puts(message);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    puts(response);

    /*
        Ex 3 Autentificare
    */

    // TODO Ex 3

    close_connection(sockfd);
    free(message);

    return 0;
}