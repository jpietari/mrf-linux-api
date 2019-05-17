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
  int              ram;
  int              enable;
  int              single;
  int              recycle;
  int              reset;
  int              trigsel;

  if (argc < 6)
    {
      printf("Usage: %s /dev/ega3 <ram> <enable> <single> <recycle> <reset> <trigsel>\n", argv[0]);
      return -1;
    }

  fdEg = EvgOpen(&pEg, argv[1]);
  if (fdEg == -1)
    return errno;

  if (argc > 7)
    {
      ram = atoi(argv[2]);
      enable = atoi(argv[3]);
      single = atoi(argv[4]);
      recycle = atoi(argv[5]);
      reset = atoi(argv[6]);
      trigsel = atoi(argv[7]);
      i = EvgSeqRamControl(pEg, ram, enable, single, recycle, reset, trigsel);
    }

  EvgClose(fdEg);

  return i;
}
