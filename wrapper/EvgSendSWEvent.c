/**
@file
EvgSendSWEvent [ <evg-device> ] [ <event code> ] - Send software event.

@param <evg-device> Device name of EVG (defaults to /dev/ega3) if left blank.
@param <event code> Event code to send.
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

  if (argc < 2)
    {
      printf("Usage: %s /dev/ega3 <event_code>\n", argv[0]);
      return -1;
    }

  fdEg = EvgOpen(&pEg, argv[1]);
  if (fdEg == -1)
    return errno;

  if (argc > 2)
    {
      i = atoi(argv[2]);
      EvgSendSWEvent(pEg, i);
    }
  
  i = EvgGetSWEventEnable(pEg);
  printf("%d\n", (i ? 1 : 0));
  
  EvgClose(fdEg);

  return 0;
}
