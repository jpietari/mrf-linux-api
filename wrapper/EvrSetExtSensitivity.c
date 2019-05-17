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
  int              input, edge, level;

  if (argc < 5)
    {
      printf("Usage: %s /dev/era3 <input> <edge> <level>\n", argv[0]);
      return -1;
    }

  fdEr = EvrOpen(&pEr, argv[1]);
  if (fdEr == -1)
    return errno;

  if (argc > 5)
    {
      input = atoi(argv[2]);
      edge = atoi(argv[3]);
      level = atoi(argv[4]);
      i = EvrSetExtEdgeSensitivity(pEr, input, edge);
      i = EvrSetExtLevelSensitivity(pEr, input, level);
    }

  EvrClose(fdEr);

  return i;
}
