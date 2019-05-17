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
  int              output;
  int              map;

  if (argc < 6)
    {
      printf("Usage: %s /dev/era3 <output> <map>\n", argv[0]);
      return -1;
    }

  fdEr = EvrOpen(&pEr, argv[1]);
  if (fdEr == -1)
    return errno;

  if (argc > 3)
    {
      output = atoi(argv[2]);
      map = atoi(argv[3]);
      i = EvrSetBPOutMap(pEr, output, map);
    }

  EvrClose(fdEr);

  return i;
}
