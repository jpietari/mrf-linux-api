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

  if (argc < 1)
    {
      printf("Usage: %s /dev/era3\n", argv[0]);
      printf("Assuming: /dev/era3\n");
      argv[1] = "/dev/era3";
    }

  fdEr = EvrOpen(&pEr, argv[1]);
  if (fdEr == -1)
    return errno;

  for (i = 0; i < EVR_MAX_FPIN_MAP; i++)
    {
      printf("Front Panel Input %2d, state %d\n", i, EvrGetExtInStatus(pEr, i));
    }
  for (i = 0; i < EVR_MAX_UNIVIN_MAP; i++)
    {
      printf("Universal Input %2d,   state %d\n", i,
	     EvrGetExtInStatus(pEr, i+EVR_MAX_FPIN_MAP));
    }
  for (i = 0; i < EVR_MAX_BPIN_MAP; i++)
    {
      printf("Backplane Input %2d,   state %d\n", i,
	     EvrGetExtInStatus(pEr, i+EVR_MAX_FPIN_MAP+EVR_MAX_UNIVIN_MAP));
    }
	
  EvrClose(fdEr);

  return 0;
}
