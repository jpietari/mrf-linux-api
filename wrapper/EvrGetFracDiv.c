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
  unsigned int     result;

  if (argc != 2)
  {
    printf("Usage: %s <evr>\n", argv[0]);
    return 1;
  }

  fdEg = EvrOpen(&pEr, argv[1]);
  if (fdEg == -1)
  {
    printf("Failed to open device: %s\n", argv[1]);
    return errno;
  }

  result = EvrGetFracDiv(pEr);
  printf("%08x\n", result);
  
  EvrClose(fdEg);

  return 0;
}
