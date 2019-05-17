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
  int              pos;
  int              timestamp;
  int              event;

  if (argc < 6)
    {
      printf("Usage: %s /dev/ega3 <ram> <pos> <timestamp> <event code>\n", argv[0]);
      return -1;
    }

  fdEg = EvgOpen(&pEg, argv[1]);
  if (fdEg == -1)
    return errno;

  if (argc > 5)
    {
      ram = atoi(argv[2]);
      pos = atoi(argv[3]);
      timestamp = atoi(argv[4]);
      event = atoi(argv[5]);
      i = EvgSetSeqRamEvent(pEg, ram, pos, timestamp, event);
    }

  EvgClose(fdEg);

  return i;
}
