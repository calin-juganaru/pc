#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "link_emulator/lib.h"

#define HOST "127.0.0.1"
#define PORT 10000

int seq_parity(char*, int);

void send_with_parity(char *payload, int size)
{
    msg_par m_par;
    msg m, r;

    ///*
    int fd = open("test.in", O_RDONLY);
    if (fd < 0)
    {
      perror("");
      exit(-1);
    }

    strcpy(m.payload, "test.out");
    m.len = strlen("test.out");
    send_message(&m);
    //*/

    memcpy(m_par.content, payload, size);
    m_par.parity = seq_parity(m_par.content, size);
    memcpy(m.payload, &m_par, sizeof(m_par));
    m.len = sizeof(int) + size;

    do
    {
      printf("Send %d BYTES with payload: %s\n", size, payload);
      send_message(&m);
      recv_message(&r);
    }
    while (r.len == NACK);
  
    memset(m.payload, 0, sizeof(m.payload));
    m.len = 0;
    close(fd);
}

int main(int argc, char **argv)
{
  init(HOST, PORT);

  for (int i = 0; i < 3; i++)
  {
    send_with_parity("Anaaaaaaa", strlen("Anaaaaaaa") + 1);
  }
  return 0;
}
