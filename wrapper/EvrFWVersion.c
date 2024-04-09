/**
@file
EvrFWVersion [ <evr-device> ] - Display EVR Firmware Version information

@param <evr-device> Device name of EVR (defaults to /dev/era3) if left blank.
*/

#include <stdint.h>
#include <endian.h>
#include <byteswap.h>
#include <errno.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <signal.h>
#include "../api/erapi.h"

/** @private */
int main(int argc, char *argv[])
{
  struct MrfErRegs *pEr;
  int              fdEr;
  int              i;
  char             c;

  if (argc < 2)
    {
      printf("Usage: %s /dev/era3\n", argv[0]);
      printf("Assuming: /dev/era3\n");
      argv[1] = "/dev/era3";
    }

  fdEr = EvrOpen(&pEr, argv[1]);
  if (fdEr == -1)
    return errno;

  i = EvrFWVersion(pEr);
  printf("%08x\n", i);

  EvrClose(fdEr);

  return 0;
}
