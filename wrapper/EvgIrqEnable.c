#include <stdint.h>
#include <endian.h>
#include <byteswap.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <signal.h>
#include "../api/egapi.h"

int main(int argc, char *argv[])
{
  struct MrfEgRegs *pEg;
  int              fdEg;
  int              mask;

  if (argc != 3)
  {
    printf("Usage: %s <evg> <mask>\n", argv[0]);
    return 1;
  }

  fdEg = EvgOpen(&pEg, argv[1]);
  if (fdEg == -1)
  {
    printf("Failed to open device: %s\n", argv[1]);
    return errno;
  }

  mask = atoi(argv[2]);
  EvgIrqEnable(pEg, mask);
  
  EvgClose(fdEg);

  return 0;
}
