#include <stdint.h>
#include <endian.h>
#include <byteswap.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <signal.h>
#include "../api/egapi.h"

int main(int argc, char *argv[])
{
  struct MrfEgRegs *pEg;
  int              fdEg;
  int              i;
  int              mxc;
  int              map;

  if (argc < 3)
    {
      printf("Usage: %s /dev/ega3 <MXC> [<map_value>]\n", argv[0]);
      return -1;
    }

  fdEg = EvgOpen(&pEg, argv[1]);
  if (fdEg == -1)
    return errno;

  mxc = atoi(argv[2]);

  if (argc > 3)
    {
      map = atoi(argv[3]);
      i = EvgSetMxcTrigMap(pEg, mxc, map);
    }

  EvgClose(fdEg);

  return 0;
}
