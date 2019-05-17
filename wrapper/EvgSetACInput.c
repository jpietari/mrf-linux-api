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
  int              bypass;
  int              sync;
  int              div;
  int              delay;

  if (argc < 6)
    {
      printf("Usage: %s /dev/ega3 <bypass> <sync> <div> <delay>\n", argv[0]);
      return -1;
    }

  fdEg = EvgOpen(&pEg, argv[1]);
  if (fdEg == -1)
    return errno;

  if (argc > 5)
    {
      bypass = atoi(argv[2]);
      sync = atoi(argv[3]);
      div = atoi(argv[4]);
      delay = atoi(argv[5]);
      i = EvgSetACInput(pEg, bypass, sync, div, delay);
    }

  EvgClose(fdEg);

  return i;
}
