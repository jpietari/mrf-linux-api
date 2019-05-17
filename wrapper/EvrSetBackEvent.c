#include <stdint.h>
#include <endian.h>
#include <byteswap.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <signal.h>
#include "../api/erapi.h"

int main(int argc, char *argv[])
{
  struct MrfErRegs *pEr;
  int              fdEr;
  int              i;
  int              input, code, edge, level;

  if (argc < 6)
    {
      printf("Usage: %s /dev/era3 <input> <code> <edge> <level>\n", argv[0]);
      return -1;
    }

  fdEr = EvrOpen(&pEr, argv[1]);
  if (fdEr == -1)
    return errno;

  if (argc > 5)
    {
      input = atoi(argv[2]);
      code = atoi(argv[3]);
      edge = atoi(argv[4]);
      level = atoi(argv[5]);
      i = EvrSetBackEvent(pEr, input, code, edge, level);
    }

  EvrClose(fdEr);

  return i;
}
