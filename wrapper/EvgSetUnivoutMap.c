/**
@file
EvgSetBPoutMap <evg-device> <univ> <map> - Setup Universal I/O output mapping.

@param <evg-device> Device name of EVG.
@param <univ> Universal I/O output number
@param <map> Output map, see table \ref evg_output_mapping
*/

#include <stdint.h>
#include <endian.h>
#include <byteswap.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <signal.h>
#include "../api/egapi.h"

/** @private */
int main(int argc, char *argv[])
{
  struct MrfEgRegs *pEg;
  int              fdEg;
  int              i;
  int              output;
  int              map;

  if (argc < 6)
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
      i = EvgSetUnivOutMap(pEg, output, map);
    }

  EvgClose(fdEg);

  return i;
}
