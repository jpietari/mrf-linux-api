/**
@file
FctGetViolation [ <evm-device> ] [ <clear> ] - Get/clear EVM Concentrator RX violation flags.

@param <evm-device> Device name of evm (defaults to /dev/ega3) if left blank.
@param <clear> Bit mask to clear flags, Bit 0 for port 1, bit 1 for port 2, ...
@return Violation flags. Bit 0 for RX port 1, bit 1 for RX port 2, ...
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
#include "../api/fracdiv.h"
#include "../api/fctapi.h"

/** @private */
int main(int argc, char *argv[])
{
  struct MrfEgRegs *pEg;
  int              fdEg;
  int              i;

  if (argc < 1)
    {
      printf("Usage: %s /dev/ega3\n", argv[0]);
      printf("Assuming: /dev/ega3\n");
      argv[1] = "/dev/ega3";
    }

  fdEg = EvgOpen(&pEg, argv[1]);
  if (fdEg == -1)
    return errno;

  if (argc > 2)
    {
      i = atoi(argv[2]);
      FctGetViolation((struct MrfFctRegs *) pEg->Fct, i);
    }

  i = FctGetViolation((struct MrfFctRegs *) pEg->Fct, 0);
  printf("%d\n", i);
  
  EvgClose(fdEg);

  return 0;
}
