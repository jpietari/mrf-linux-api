/**
@file
EvgSetBPinMap <evg-device> <bp> <trig> <dbus> <irq> <seqtrig> - Setup backplane input mapping.

@param <evg-device> Device name of EVG.
@param <bp> Backplane input number
@param <trig> Number of Event trigger to trigger, -1 for no trigger
@param <dbus> Number of Distributed bus bit to map input to, -1 for no mapping
@param <irq> External interrupt mapping, 0 = no interrupt, 1 = mapped to interrupt
@param <seqtrig> Number of sequence RAM trigger, -1 for no trigger 
@param <mask> Sequence mask enable/disable field
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
  int              bp;
  int              trig;
  int              dbus;
  int              irq;
  int              seqtrig;
  int              mask;
  
  if (argc < 8)
    {
      printf("Usage: %s /dev/ega3 <bp> <trig> <dbus> <irq> <seqtrig> <mask>\n", argv[0]);
      return -1;
    }

  fdEg = EvgOpen(&pEg, argv[1]);
  if (fdEg == -1)
    return errno;

  if (argc > 7)
    {
      bp = atoi(argv[2]);
      trig = atoi(argv[3]);
      dbus = atoi(argv[4]);
      irq = atoi(argv[5]);
      seqtrig = atoi(argv[6]);
      mask = atoi(argv[7]);
      i = EvgSetBPinMap(pEg, bp, trig, dbus, irq, seqtrig, mask);
    }

  EvgClose(fdEg);

  return i;
}
