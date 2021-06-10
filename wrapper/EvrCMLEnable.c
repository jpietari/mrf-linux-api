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
      printf("Usage: %s /dev/era3 <channel> [<enable>]\n", argv[0]);
      printf("Assuming: /dev/era3\n");
      argv[1] = "/dev/era3";
    }

  fdEr = EvrOpen(&pEr, argv[1]);
  if (fdEr == -1)
    return errno;

  if (argc > 2)
    {
      ch = atoi(argv[2]);
    }
  if (argc > 3)
    {
      i = atoi(argv[3]);
  
      EvrCMLEnable(pEr, ch, i);
    }

  if (argc > 2)
    {
      i = EvrGetCMLEnable(pEr, ch);
      printf("%d\n", (i ? 1 : 0));
    }
      
  EvrClose(fdEr);

  return 0;
}
