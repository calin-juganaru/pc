#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "link_emulator/lib.h"

#define HOST "127.0.0.1"
#define PORT 10001

int main(int argc, char **argv)
{
  init(HOST, PORT);
  msg r, t;

  if (recv_message(&r) < 0)
  {
    perror("[RECEIVER] Recv error");
    return -1;
  }

  int fd = open(r.payload, O_WRONLY | O_CREAT | O_TRUNC, 0600);
  if (fd < 0)
  {
    perror("");
    exit(-1);
  }
  
  do
  {
    memset(r.payload, 0, sizeof(r.payload));
    
    if (recv_message(&r) < 0)
    {
      perror("Receive message");
      return -1;
    }

    printf("[%s] Got msg len %d with payload: \n",argv[0], r.len, r.payload);
    write(fd, r.payload, r.len);

    sprintf(t.payload,"ACK(%s)",r.payload);
    t.len = strlen(t.payload+1);
    send_message(&t);
  }
  while (r.len > 0);

  close(fd);
  memset(r.payload, 0, sizeof(r.payload));
  r.len = 0;
  return 0;
}
