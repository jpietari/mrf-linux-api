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
  int              presc;
  int              value;
  int              pol, phase;

  if (argc < 4)
    {
      printf("Usage: %s /dev/era3 <presc> <value> [<phase>]\n", argv[0]);
      return -1;
    }

  fdEr = EvrOpen(&pEr, argv[1]);
  if (fdEr == -1)
    return errno;

  if (argc > 3)
    {
      presc = atoi(argv[2]);
      value = atoi(argv[3]);
      i = EvrSetPrescaler(pEr, presc, value);
    }

  if (argc > 4)
    {
      phase = atoi(argv[4]);
      EvrSetPrescalerPhase(pEr, presc, phase);
    }
  
  EvrClose(fdEr);

  return i;
}
