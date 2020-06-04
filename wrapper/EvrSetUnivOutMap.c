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
  int              map0, map1;

  if (argc < 4)
    {
      printf("Usage: %s /dev/era3 <output> <map 0> [<map 1>]\n", argv[0]);
      return -1;
    }

  fdEr = EvrOpen(&pEr, argv[1]);
  if (fdEr == -1)
    return errno;

  if (argc > 4)
    map1 = strtol(argv[4], NULL, 0);
  else
    map1 = 0x3d;
  
  if (argc > 3)
    {
      output = strtol(argv[2], NULL, 0);
      map0 = strtol(argv[3], NULL, 0);
      i = EvrSetUnivOutMap(pEr, output, map0 + ((map1 & 0xff) << 8));
    }

  EvrClose(fdEr);

  return i;
}
