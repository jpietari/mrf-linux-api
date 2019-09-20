/**
@file
EvgSetUnivinMap <evg-device> <univin> <trig> <dbus> <irq> <seqtrig> - Setup Universal I/O input mapping.

@param <evg-device> Device name of EVG.
@param <univin> Universal I/O input number
@param <trig> Number of Event trigger to trigger, -1 for no trigger
@param <dbus> Number of Distributed bus bit to map input to, -1 for no mapping
@param <irq> External interrupt mapping, 0 = no interrupt, 1 = mapped to interrupt
@param <seqtrig> Number of sequence RAM trigger, -1 for no trigger 
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
  int              univ;
  int              trig;
  int              dbus;
  int              irq;
  int              seqtrig;

  if (argc < 6)
    {
      printf("Usage: %s /dev/ega3 <univ> <trig> <dbus> <irq> <seqtrig>\n", argv[0]);
      return -1;
    }

  fdEg = EvgOpen(&pEg, argv[1]);
  if (fdEg == -1)
    return errno;

  if (argc > 6)
    {
      univ = atoi(argv[2]);
      trig = atoi(argv[3]);
      dbus = atoi(argv[4]);
      irq = atoi(argv[5]);
      seqtrig = atoi(argv[6]);
      i = EvgSetUnivinMap(pEg, univ, trig, dbus, irq, seqtrig);
    }

  EvgClose(fdEg);

  return i;
}
