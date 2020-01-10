/**
@file
EvrDumpDC [ <evr-device> ] - Display EVR Delay Compensation Status.

@param <evr-device> Device name of evr (defaults to /dev/era3) if left blank.
*/

#include <stdint.h>
#include <endian.h>
#include <byteswap.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <signal.h>
#include "../api/erapi.h"
#include "../api/fracdiv.h"

/** @private */
int main(int argc, char *argv[])
{
  struct MrfErRegs *pEr;
  int              fdEr;
  int              i;
  double           reffreq;

  if (argc < 1)
    {
      printf("Usage: %s /dev/era3\n", argv[0]);
      printf("Assuming: /dev/era3\n");
      argv[1] = "/dev/era3";
    }

  fdEr = EvrOpen(&pEr, argv[1]);
  if (fdEr == -1)
    return errno;

  reffreq = cw_to_freq(EvrGetFracDiv(pEr));
  printf("DC Enable %d\n", (EvrGetDCEnable(pEr) ? 1 : 0));
  printf("DC Status 0x%04x\n", EvrGetDCStatus(pEr));
  printf("DC Delay Target %08lx, (%3.6f ns)\n",
	 EvrGetTargetDelay(pEr), EvrGetTargetDelay(pEr)/(reffreq*65.536));
  printf("DC Delay Value %08lx, (%3.6f ns)\n",
	 EvrGetDCDelay(pEr), EvrGetDCDelay(pEr)/(reffreq*65.536));
 
  EvrClose(fdEr);

  return 0;
}
