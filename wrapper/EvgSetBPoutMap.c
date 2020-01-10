/**
@file
EvgSetBPoutMap <evg-device> <bp> <map> - Setup backplane output mapping.

@param <evg-device> Device name of EVG.
@param <bp> Backplane output number
@param <map> Output map, see table \ref evg_output_mapping

<table>
<caption id="evg_output_mapping">Output mapping</caption>
<tr><th>ID (dec)<th>ID (hex)<th>Signal
<tr><td>0<td>0x00<td>Reserved
<tr><td>...<td>...<td>...
<tr><td>31<td>0x1F<td>Reserved
<tr><td>32<td>0x20<td>Distributed bus bit 0
<tr><td>33<td>0x21<td>Distributed bus bit 1
<tr><td>...<td>...<td>...
<tr><td>39<td>0x27<td>Distributed bus bit 7
<tr><td>40<td>0x28<td>Multiplexed Counter 0
<tr><td>41<td>0x29<td>Multiplexed Counter 1
<tr><td>...<td>...<td>...
<tr><td>47<td>0x2F<td>Multiplexed Counter 7
<tr><td>48<td>0x30<td>AC trigger logic output
<tr><td>49<td>0x31<td>Reserved
<tr><td>...<td>...<td>...
<tr><td>60<td>0x3D<td>Reserved
<tr><td>61<td>0x3D<td>Three-state output (for bi-dir ports)
<tr><td>62<td>62<td>High 1
<tr><td>63<td>63<td>Low 0
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
      i = EvgSetBPOutMap(pEg, output, map);
    }

  EvgClose(fdEg);

  return i;
}
