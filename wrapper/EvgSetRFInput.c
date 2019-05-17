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
  int              RFsel;
  int              div;

  if (argc < 4)
    {
      printf("Usage: %s /dev/ega3 <RFsel> <div>\n", argv[0]);
      return -1;
    }

  fdEg = EvgOpen(&pEg, argv[1]);
  if (fdEg == -1)
    return errno;

  if (argc > 3)
    {
      RFsel = atoi(argv[2]);
      div = atoi(argv[3]);
      i = EvgSetRFInput(pEg, RFsel, div);
    }

  EvgClose(fdEg);

  return i;
}
