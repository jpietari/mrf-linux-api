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
  int              i;

  if (argc < 1)
    {
      printf("Usage: %s /dev/ega3 <timestamp>\n", argv[0]);
      printf("Assuming: /dev/ega3\n");
      argv[1] = "/dev/ega3";
    }

  fdEg = EvgOpen(&pEg, argv[1]);
  if (fdEg == -1)
    return errno;

  if (argc > 2)
    {
      i = atoi(argv[2]);
      EvgTimestampLoad(pEg, i);
    }
  
  i = EvgTimestampGet(pEg);
  printf("%d\n", i);
  
  EvgClose(fdEg);

  return 0;
}
