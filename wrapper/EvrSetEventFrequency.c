#include <stdint.h>
#include <endian.h>
#include <byteswap.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <signal.h>
#include "../api/erapi.h"
#include "../api/fracdiv.h"

int main(int argc, char *argv[])
{
  struct MrfErRegs *pEr;
  int              fdEr;
  int              i;
  double           freq;
  
  if (argc < 1)
    {
      printf("Usage: %s /dev/era3 [<event clock frequency (MHz)>]\n", argv[0]);
      printf("Assuming: /dev/era3\n");
      argv[1] = "/dev/era3";
    }

  fdEr = EvrOpen(&pEr, argv[1]);
  if (fdEr == -1)
    return errno;

  if (argc > 2)
    {
      freq = atof(argv[2]);
      EvrSetFracDiv(pEr, freq_to_cw(freq));
    }
  
  i = EvrGetFracDiv(pEr);
  printf("%f\n", cw_to_freq(i));
  
  EvrClose(fdEr);

  return 0;
}
