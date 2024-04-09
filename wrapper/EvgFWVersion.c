/**
@file
EvgFWVersion [ <evg-device> ] - Display EVG Firmware Version information

@param <evg-device> Device name of EVG (defaults to /dev/ega3) if left blank.
*/

#include <stdint.h>
#include <endian.h>
#include <byteswap.h>
#include <errno.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <signal.h>
#include "../api/egapi.h"

/** @private */
int main(int argc, char *argv[])
{
  struct MrfEgRegs *pEg;
  int              fdEg;
  int              i;
  char             c;

  if (argc < 2)
    {
      printf("Usage: %s /dev/ega3\n", argv[0]);
      printf("Assuming: /dev/ega3\n");
      argv[1] = "/dev/ega3";
    }

  fdEg = EvgOpen(&pEg, argv[1]);
  if (fdEg == -1)
    return errno;

  i = EvgFWVersion(pEg);
  printf("%08x\n", i);

  EvgClose(fdEg);

  return 0;
}
