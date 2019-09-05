#include <stdint.h>
#include <endian.h>
#include <byteswap.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <signal.h>
#include "../api/egapi.h"
#include "../api/fracdiv.h"
#include "../api/fctapi.h"

int main(int argc, char *argv[])
{
  struct MrfEgRegs *pEg;
  int              fdEg;
  int              i;
  double           freq;

  if (argc < 1)
    {
      printf("Usage: %s /dev/ega3\n", argv[0]);
      printf("Assuming: /dev/ega3\n");
      argv[1] = "/dev/ega3";
    }

  fdEg = EvgOpen(&pEg, argv[1]);
  if (fdEg == -1)
    return errno;

  freq = cw_to_freq(EvgGetFracDiv(pEg));

  printf("Freq %lf\n", freq);
  
  FctDumpStatus((struct MrfFctRegs *) pEg->Fct, freq, 7);
  
  EvgClose(fdEg);

  return 0;
}
