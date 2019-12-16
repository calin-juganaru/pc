// Protocoale de comunicatii
// Laborator 9 - DNS
// dns.c

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

int usage(char* name)
{
	printf("Usage:\n\t%s -n <NAME>\n\t%s -a <IP>\n", name, name);
	return 1;
}

// Receives a name and prints IP addresses
void get_ip(char* name)
{
	int ret;
	char ip[100];
	struct addrinfo hints, *result, *p;

	// TODO: set hints
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_protocol = 0;
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_next = NULL;

	// TODO: get addresses
	ret = getaddrinfo(name, NULL, &hints, &result);
    if (ret)
	{
        printf("getaddrinfo: %s\n", gai_strerror(ret));
        exit(1);
    }

	// TODO: iterate through addresses and print them
	while (result)
	{
		p = result;
		struct sockaddr_in* saddr = p->ai_addr;
		result = NULL;

		if (inet_ntop(p->ai_family, &(saddr->sin_addr), ip, sizeof(ip)))
		{
        	printf("IP is %s\nPort is %d\n", ip, ntohs(saddr->sin_port));
        }
	}

	// TODO: free allocated data
	freeaddrinfo(result);
}

// Receives an address and prints the associated name and service
void get_name(char* ip)
{
	int ret;
	struct sockaddr_in addr;
	socklen_t size = sizeof(struct sockaddr);
	char host[1024];
	char serv[20];

	// TODO: fill in address data
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(ip);
	addr.sin_port = 80;

	// TODO: get name and service
	ret = getnameinfo((struct sockaddr*) &addr, sizeof(addr), host, sizeof(host), serv, sizeof(serv), 0);
	// TODO: print name and service
	printf("Host: %s  &  serv: %s\n", host, serv);
}

int main(int argc, char **argv)
{
	if (argc < 3) {
		return usage(argv[0]);
	}

	if (strncmp(argv[1], "-n", 2) == 0) {
		get_ip(argv[2]);
	} else if (strncmp(argv[1], "-a", 2) == 0) {
		get_name(argv[2]);
	} else {
		return usage(argv[0]);
	}

	return 0;
}
