/**
@file
EvgSetACInput <evg-device> <bypass> <sync> <div> <delay> - Setup EVG AC input.

@param <evg-device> Device name of EVG (defaults to /dev/ega3) if left blank.
@param <bypass> 0 - Enable divider and phase shifter, 1 - Bypass divider and phase shifter
@param <sync> Synchronization select \n 
            0 - Synchronize to event clock \n 
            1 - Synchronize to multiplexed counter 7 \n 
            3 - Synchronize to TTL input 1 \n 
            5 - Synchronize to TTL input 2
@param <div> AC input divider 0 to 255 (value of 0 results in divide by 256)
@param <delay> delay in 0.1 ms steps, 0 to 25.5 ms (value 0 to 255)
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
