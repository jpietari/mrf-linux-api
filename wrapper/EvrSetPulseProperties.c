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
  int              i;
  int              pulse, polarity, map_reset_ena, map_set_ena, map_trigger_ena, enable;

  if (argc < 5)
    {
      printf("Usage: %s /dev/era3 <pulse> <polarity> <map_reset_ena> <map_set_ena> <map_trigger_ena> <enable>\n", argv[0]);
      return -1;
    }

  fdEr = EvrOpen(&pEr, argv[1]);
  if (fdEr == -1)
    return errno;

  if (argc > 7)
    {
      pulse = atoi(argv[2]);
      polarity = atoi(argv[3]);
      map_reset_ena = atoi(argv[4]);
      map_set_ena = atoi(argv[5]);
      map_trigger_ena = atoi(argv[6]);
      enable = atoi(argv[7]);
      i = EvrSetPulseProperties(pEr, pulse, polarity, map_reset_ena, map_set_ena, map_trigger_ena, enable);
    }

  EvrClose(fdEr);

  return i;
}
