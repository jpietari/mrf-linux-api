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
  int              enable;
  int              event;
  int              trig;

  if (argc < 3)
    {
      printf("Usage: %s /dev/ega3 <trigger event> [<event code> <enable>]\n", argv[0]);
      return -1;
    }

  fdEg = EvgOpen(&pEg, argv[1]);
  if (fdEg == -1)
    return errno;

  trig = atoi(argv[2]);

  if (argc > 4)
    {
      event = atoi(argv[3]);
      enable = atoi(argv[3]);
      i = EvgSetTriggerEvent(pEg, trig, event, enable);
    }

  event = EvgGetTriggerEventCode(pEg, trig);
  enable = EvgGetTriggerEventEnable(pEg, trig);
  printf("%d %d\n", event, enable);
  
  EvgClose(fdEg);

  return 0;
}
