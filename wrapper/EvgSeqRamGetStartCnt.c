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
  int              ram;
  unsigned int     count;

  if (argc != 3)
  {
    printf("Usage: %s <evg> <ram>\n", argv[0]);
    return 1;
  }

  fdEg = EvgOpen(&pEg, argv[1]);
  if (fdEg == -1)
  {
    printf("Failed to open device: %s\n", argv[1]);
    return errno;
  }

  ram   = atoi(argv[2]);
  count = EvgSeqRamGetStartCnt(pEg, ram);
  printf("%d\n", count);
  
  EvgClose(fdEg);

  return 0;
}
