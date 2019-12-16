#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "lib.h"

#define HOST "127.0.0.1"
#define PORT 10001

int main(void)
{
	msg r;
	
	printf("[RECEIVER] Starting.\n");
	init(HOST, PORT);

	do
	{
		recv_message(&r);
		printf("RECV %s with length %d\n", r.payload, r.len);
		send_message(&r);
		printf("SEND back %s with length %d\n", r.payload, r.len);
	}
	while (r.len > 0);

	printf("[RECEIVER] Finished receiving..\n");
	return 0;
}
