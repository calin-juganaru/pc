#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "link_emulator/lib.h"

#define HOST "127.0.0.1"
#define PORT 10001

int seq_parity(char*, int);

void recv_with_parity()
{
    msg_par m_par;
    msg m, r;

    memset(r.payload, 0, sizeof(r.payload));
    int parity = 0;

    //*/
    int fd = open(r.payload, O_WRONLY | O_CREAT | O_TRUNC, 0600);
    if (fd < 0)
    {
      perror("");
      exit(-1);
    }
    //*/

    do
    {
      recv_message(&r);
      memcpy(&m_par, r.payload, r.len);

      parity = seq_parity(m_par.content, r.len - sizeof(int));     
      printf(" Got %d BYTES with payload: %s\n", strlen(m_par.content) + 1, m_par.content);

      m.len = NACK; 
      send_message(&m);
    }
    while (m_par.parity != parity);
    
    m.len = ACK;
    send_message(&m);
}

int main(int argc, char **argv)
{
  init(HOST, PORT);

  for (int i = 0; i < 3; i++)
  {
    recv_with_parity();
  }

  return 0;
}
