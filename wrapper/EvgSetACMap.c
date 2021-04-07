/**
@file
EvgSetACMap <evg-device> <map> - Setup EVG AC input to event trigger map.

@param <evg-device> Device name of EVG (defaults to /dev/ega3) if left blank.
@param <map> trigger event to map AC Input to event triggers \n
            -1 - AC Input not mapped \n
            0 - AC Input mapped to event trigger 0 \n
            1 - AC Input mapped to event trigger 1 \n
            2 - AC Input mapped to event trigger 2 \n
            ...
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
  int              map;

  if (argc < 3)
    {
      printf("Usage: %s /dev/ega3 <map>\n", argv[0]);
      return -1;
    }

  fdEg = EvgOpen(&pEg, argv[1]);
  if (fdEg == -1)
    return errno;

  if (argc > 2)
    {
      map = atoi(argv[2]);
      i = EvgSetACMap(pEg, map);
    }

  EvgClose(fdEg);

  return i;
}
