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
  int              fdEg;
  int              code;
  unsigned int     count;

  if (argc != 3)
  {
    printf("Usage: %s <evr> <code>\n", argv[0]);
    return 1;
  }

  fdEg = EvrOpen(&pEr, argv[1]);
  if (fdEg == -1)
  {
    printf("Failed to open device: %s\n", argv[1]);
    return errno;
  }

  code   = atoi(argv[2]);
  count = EvrGetEventCount(pEr, code);
  printf("%d\n", count);
  
  EvrClose(fdEg);

  return 0;
}
