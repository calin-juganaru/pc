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

int main(int argc, char **argv)
{
  init(HOST, PORT);
  msg t, r;
  
  int fd = open("test.in", O_RDONLY);
  if (fd < 0)
  {
    perror("");
    exit(-1);
  }

  strcpy(t.payload, "test.out");
  t.len = strlen("test.out");
  send_message(&t);

  do
  {
    memset(t.payload, 0, sizeof(t.payload));
    strcpy(r.payload, "test.out");
    t.len = read(fd, t.payload, sizeof(t.payload));
    printf("[%s] Send %d BYTES with payload: \n", argv[0], t.len);
    send_message(&t);
    
    
    if (recv_message(&r)<0)
    {
      perror("receive error");
      break;
    }
    else
    {
      printf("[%s] Got reply with payload: \n", argv[0], r.payload);
    }
    
  }
  while (t.len > 0);
  
  memset(t.payload, 0, sizeof(t.payload));
  t.len = 0;
  close(fd);
  return 0;
}
