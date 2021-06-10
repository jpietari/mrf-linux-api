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
  int              ch, i;

  if (argc < 2)
    {
      printf("Usage: %s /dev/era3 <channel> [<phase>]\n", argv[0]);
      printf("Assuming: /dev/era3\n");
      argv[1] = "/dev/era3";
    }

  fdEr = EvrOpen(&pEr, argv[1]);
  if (fdEr == -1)
    return errno;

  if (argc > 3)
    {
      ch = atoi(argv[2]);
      i = strtol(argv[3], NULL, 0) & 0xFFF;
      i = EvrSetCMLPhaseOffset(pEr, ch, i);
      printf("%d\n", i);
    }
  
  EvrClose(fdEr);

  return 0;
}
