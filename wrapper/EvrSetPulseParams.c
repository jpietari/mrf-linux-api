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
  int              pulse, presc, delay, width;

  if (argc < 5)
    {
      printf("Usage: %s /dev/era3 <pulse> <presc> <delay> <width>\n", argv[0]);
      return -1;
    }

  fdEr = EvrOpen(&pEr, argv[1]);
  if (fdEr == -1)
    return errno;

  if (argc > 5)
    {
      pulse = atoi(argv[2]);
      presc = atoi(argv[3]);
      delay = atoi(argv[4]);
      width = atoi(argv[5]);
      i = EvrSetPulseParams(pEr, pulse, presc, delay, width);
    }

  EvrClose(fdEr);

  return i;
}
