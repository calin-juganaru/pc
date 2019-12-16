#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "lib.h"

#define HOST "127.0.0.1"
#define PORT 10000

int main(int argc, char *argv[])
{
	msg t, m;
	int i, j;
	
	printf("[SENDER] Starting.\n");	
	init(HOST, PORT);

	long BDP = 1000 * atol(argv[1]);
	long W = BDP / (sizeof(msg) * 8);

	if (W > COUNT)
		W = COUNT;

	//int MARE = 10000;

	for (j = 0; j < 10; ++j)
	{
		for (i = 0; i < W; ++i)
		{
			strcpy(t.payload, " ,   Tralalalala");
			t.len = strlen(t.payload);
			t.payload[0] = (j % 10) + '0';
			t.payload[3] = (i % 10) + '0';

			//memset(t.payload, 1, MARE);

			printf("SEND %s with length %d\n", t.payload, t.len);
			send_message(&t);
		}

		for (i = 0; i < W; ++i)
		{
			recv_message(&m);
			printf("RECV back %s with length %d\n", m.payload, m.len);
		}
	}

	memset(&m, 0, sizeof(m));
	send_message(&m);
	printf("[SENDER] Stop.\n");
	return 0;
}