/**
@file
EvgSetFPinMap <evg-device> <fp> <trig> <dbus> <irq> <seqtrig> - Setup front panel input mapping.

@param <evg-device> Device name of EVG.
@param <fp> Front panel input number
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
  int              fp;
  int              trig;
  int              dbus;
  int              irq;
  int              seqtrig;
  int              seqena;
  int              mask;

  if (argc < 9)
    {
      printf("Usage: %s /dev/ega3 <fp> <trig> <dbus> <irq> <seqtrig> <seqena> <mask>\n", argv[0]);
      return -1;
    }

  fdEg = EvgOpen(&pEg, argv[1]);
  if (fdEg == -1)
    return errno;

  if (argc > 7)
    {
      fp = atoi(argv[2]);
      trig = atoi(argv[3]);
      dbus = atoi(argv[4]);
      irq = atoi(argv[5]);
      seqtrig = atoi(argv[6]);
      seqena = atoi(argv[7]);
      mask = atoi(argv[8]);
      i = EvgSetFPinMap(pEg, fp, trig, dbus, irq, seqtrig, seqena, mask);
    }

  EvgClose(fdEg);

  return i;
}
