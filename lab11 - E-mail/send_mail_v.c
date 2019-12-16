#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#define SMTP_PORT 25
#define MAXLEN 500

/**
 * Citeste maxim maxlen octeti din socket-ul sockfd. Intoarce
 * numarul de octeti cititi.
 */
ssize_t Readline(int sockd, void *vptr, size_t maxlen)
{
    ssize_t n, rc;
    char    c, *buffer;

    buffer = vptr;

    for ( n = 1; n < maxlen; n++ ) {
	if ( (rc = read(sockd, &c, 1)) == 1 ) {
	    *buffer++ = c;
	    if ( c == '\n' )
		break;
	}
	else if ( rc == 0 ) {
	    if ( n == 1 )
		return 0;
	    else
		break;
	}
	else {
	    if ( errno == EINTR )
		continue;
	    return -1;
	}
    }

    *buffer = 0;
    return n;
}

void send_command(int sockfd, char sendbuf[], char *expected)
{
  char recvbuf[MAXLEN];
  int nbytes;
  char CRLF[3];

  CRLF[0] = 13; CRLF[1] = 10; CRLF[2] = 0;
  strcat(sendbuf, CRLF);
  printf("Trimit: %s", sendbuf);
  write(sockfd, sendbuf, strlen(sendbuf));
  nbytes = Readline(sockfd, recvbuf, MAXLEN - 1);
  recvbuf[nbytes] = 0;
  printf("Am primit: %s", recvbuf);
  if (strstr(recvbuf, expected) != recvbuf) {
    printf("Am primit mesaj de eorare de la server!\n");
    exit(-1);
  }
}

int main(int argc, char **argv)
{
  int port = SMTP_PORT;

  char sendbuf[MAXLEN];
  char recvbuf[MAXLEN];

  auto server_ip = string(argv[1]);
  auto sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
  {
	  printf("Eroare la  creare socket.\n");
	  exit(-1);
  }

  auto servaddr = sockaddr_in();
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(port);

  auto return_val = inet_aton(server_ip, &servaddr.sin_addr);
  if (return_val <= 0)
  {
    printf("Adresa IP invalida.\n");
    exit(-1);
  }

  return_val = connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
  if (return_val < 0)
  {
    printf("Eroare la conectare\n");
    exit(-1);
  }

  Readline(sockfd, recvbuf, MAXLEN -1);
  printf("Am primit: %s\n", recvbuf);

  sprintf(sendbuf, "HELLO <adresa_ip_aici>");
  send_command(sockfd, sendbuf, "250");

  close(sockfd);
}

