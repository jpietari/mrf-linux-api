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

int main(int argc, char *argv[])
{
  struct MrfEgRegs *pEg;
  int              fdEg;
  int              i;
  double           freq;
  
  if (argc < 1)
    {
      printf("Usage: %s /dev/ega3 [<event clock frequency (MHz)>]\n", argv[0]);
      printf("Assuming: /dev/ega3\n");
      argv[1] = "/dev/ega3";
    }

  fdEg = EvgOpen(&pEg, argv[1]);
  if (fdEg == -1)
    return errno;

  if (argc > 2)
    {
      freq = atof(argv[2]);
      EvgSetFracDiv(pEg, freq_to_cw(freq));
    }
  
  i = EvgGetFracDiv(pEg);
  printf("%f\n", cw_to_freq(i));
  
  EvgClose(fdEg);

  return 0;
}
