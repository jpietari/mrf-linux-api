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

  if (argc < 3)
    {
      printf("Usage: %s /dev/era3 <ram> <enable>\n", argv[0]);
      printf("Assuming: /dev/era3\n");
      argv[1] = "/dev/era3";
      ram = 0;
    }

  fdEr = EvrOpen(&pEr, argv[1]);
  if (fdEr == -1)
    return errno;

  if (argc > 3)
    {
      ram = atoi(argv[2]);
      enable = atoi(argv[3]);
      EvrMapRamEnable(pEr, ram, enable);
    }
  
  EvrClose(fdEr);

  return 0;
}
