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
  int              dlymod;
  int              dly0, dly1;

  if (argc < 5)
    {
      printf("Usage: %s /dev/era3 <dlymod> <dly0> <dly1>\n", argv[0]);
      return -1;
    }

  fdEr = EvrOpen(&pEr, argv[1]);
  if (fdEr == -1)
    return errno;

  if (argc > 3)
    {
      dlymod = atoi(argv[2]);
      dly0 = atoi(argv[3]);
      dly1 = atoi(argv[4]);
      i = EvrUnivDlySetDelay(pEr, dlymod, dly0, dly1);
    }

  EvrClose(fdEr);

  return i;
}
