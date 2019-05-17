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
  int              code;
  int              enable;

  if (argc < 6)
    {
      printf("Usage: %s /dev/era3 <ram> <code> <enable>\n", argv[0]);
      return -1;
    }

  fdEr = EvrOpen(&pEr, argv[1]);
  if (fdEr == -1)
    return errno;

  if (argc > 4)
    {
      ram = atoi(argv[2]);
      code = atoi(argv[3]);
      enable = atoi(argv[4]);
      i = EvrSetFIFOEvent(pEr, ram, code, enable);
    }

  EvrClose(fdEr);

  return i;
}
