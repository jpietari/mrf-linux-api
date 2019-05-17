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
  int              ram;
  int              enable;
  int              single;
  int              recycle;
  int              reset;
  int              trigsel;

  if (argc < 6)
    {
      printf("Usage: %s /dev/era3 <ram> <enable> <single> <recycle> <reset> <trigsel>\n", argv[0]);
      return -1;
    }

  fdEr = EvrOpen(&pEr, argv[1]);
  if (fdEr == -1)
    return errno;

  if (argc > 7)
    {
      ram = atoi(argv[2]);
      enable = atoi(argv[3]);
      single = atoi(argv[4]);
      recycle = atoi(argv[5]);
      reset = atoi(argv[6]);
      trigsel = atoi(argv[7]);
      i = EvrSeqRamControl(pEr, ram, enable, single, recycle, reset, trigsel);
    }

  EvrClose(fdEr);

  return i;
}
