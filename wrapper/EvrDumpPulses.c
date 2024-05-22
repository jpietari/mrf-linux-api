#include <stdint.h>
#include <endian.h>
#include <byteswap.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <signal.h>
#include "../api/erapi.h"

int main(int argc, char *argv[])
{
  struct MrfErRegs *pEr;
  int              fdEr;
  int              i;

  if (argc < 2)
    {
      printf("Usage: %s /dev/era3 [<pulses>]\n", argv[0]);
      printf("Assuming: /dev/era3\n");
      argv[1] = "/dev/era3";
    }

  fdEr = EvrOpen(&pEr, argv[1]);
  if (fdEr == -1)
    return errno;

  i = 32;
  if (argc > 2)
    {
      i = atoi(argv[2]);
    }
 
  EvrDumpPulses(pEr, i);

  EvrClose(fdEr);

  return 0;
}
