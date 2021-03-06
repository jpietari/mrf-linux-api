/**
@file
EvgSetBPoutMap <evg-device> <bp> <map> - Setup transition board output mapping.

@param <evg-device> Device name of EVG.
@param <output> Transition board output number
@param <map> Output map, see table \ref evg_output_mapping
*/

/** @private */
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
  int              output;
  int              map;

  if (argc < 4)
    {
      printf("Usage: %s /dev/ega3 <output> <map>\n", argv[0]);
      return -1;
    }

  fdEg = EvgOpen(&pEg, argv[1]);
  if (fdEg == -1)
    return errno;

  if (argc > 3)
    {
      output = atoi(argv[2]);
      map = atoi(argv[3]);
      i = EvgSetTBOutMap(pEg, output, map);
    }

  EvgClose(fdEg);

  return i;
}
