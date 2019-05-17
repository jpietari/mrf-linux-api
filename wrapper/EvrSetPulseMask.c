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
  int              pulse, mask, enable;

  if (argc < 5)
    {
      printf("Usage: %s /dev/era3 <pulse> <mask> <enable>\n", argv[0]);
      return -1;
    }

  fdEr = EvrOpen(&pEr, argv[1]);
  if (fdEr == -1)
    return errno;

  if (argc > 4)
    {
      pulse = atoi(argv[2]);
      mask = atoi(argv[3]);
      enable = atoi(argv[4]);
      i = EvrSetPulseMask(pEr, pulse, mask, enable);
    }

  EvrClose(fdEr);

  return i;
}
