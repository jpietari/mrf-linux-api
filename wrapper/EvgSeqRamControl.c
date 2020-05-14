/**
@file
EvgSeqRamControl <evg-device> <ram> <enable> <single> <recycle> <reset> <trigsel> <mask> - Control EVG Sequencer.

@param <evg-device> Device name of EVG (defaults to /dev/ega3) if left blank.
@param <ram> Sequence RAM number 0, 1
@param <enable> 0 - disable, 1 - enable
@param <single> 0 - do not disable RAM after end-of-sequence event, 1 - single sequence mode
@param <recycle> 0 - stop after end-of-sequence event (wait for next trigger), 1 - immediately restart sequence (without waiting for trigger)
@param <reset> 0 - do nothing, 1 - stop, reset and disable sequence
@param <trigsel> Trigger select. See table \ref evgtrigsel
@param <mask> event software enable/mask register

<table>
<caption id="evgtrigsel">Sequence RAM trigger selection</caption>
<tr><th>ID (dec)<th>ID (hex)<th>Trigger
<tr><td>0<td>0x00<td>Multiplexed counter 0
<tr><td>1<td>0x01<td>Multiplexed counter 1
<tr><td>...<td>...<td>...
<tr><td>7<td>0x07<td>Multiplexed counter 7
<tr><td>16<td>0x10<td>AC synchronization logic
<tr><td>17<td>0x11<td>Sequence RAM 0 software trigger
<tr><td>18<td>0x12<td>Sequence RAM 1 software trigger
<tr><td>19<td>0x13<td>Continuous trigger
<tr><td>24<td>0x18<td>Sequence RAM 0 external trigger
<tr><td>25<td>0x19<td>Sequence RAM 1 external trigger
<tr><td>31<td>0x1F<td>Trigger disabled
</table>
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
  int              ram;
  int              enable;
  int              single;
  int              recycle;
  int              reset;
  int              trigsel;
  int              mask;
  
  if (argc < 9)
    {
      printf("Usage: %s /dev/ega3 <ram> <enable> <single> <recycle> <reset> <trigsel> <mask>\n", argv[0]);
      return -1;
    }

  fdEg = EvgOpen(&pEg, argv[1]);
  if (fdEg == -1)
    return errno;

  if (argc > 8)
    {
      ram = atoi(argv[2]);
      enable = atoi(argv[3]);
      single = atoi(argv[4]);
      recycle = atoi(argv[5]);
      reset = atoi(argv[6]);
      trigsel = atoi(argv[7]);
      mask = atoi(argv[8]);
      i = EvgSeqRamControl(pEg, ram, enable, single, recycle, reset, trigsel, mask);
    }

  EvgClose(fdEg);

  return i;
}
