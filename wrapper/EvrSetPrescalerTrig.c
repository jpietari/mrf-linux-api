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
  int              prescaler, trigs;

  if (argc < 5)
    {
      printf("Usage: %s /dev/era3 <prescaler> <trigs>\n", argv[0]);
      return -1;
    }

  fdEr = EvrOpen(&pEr, argv[1]);
  if (fdEr == -1)
    return errno;

  if (argc > 3)
    {
      prescaler = atoi(argv[2]);
      trigs = atoi(argv[3]);
      i = EvrSetPrescalerTrig(pEr, prescaler, trigs);
    }

  EvrClose(fdEr);

  return i;
}
