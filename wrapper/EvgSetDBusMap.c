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
  int              dbus;
  int              map;

  if (argc < 1)
    {
      printf("Usage: %s /dev/ega3 [<dbus> <map>]\n", argv[0]);
      printf("Assuming: /dev/ega3\n");
      argv[1] = "/dev/ega3";
    }

  fdEg = EvgOpen(&pEg, argv[1]);
  if (fdEg == -1)
    return errno;

  if (argc > 3)
    {
      dbus = atoi(argv[2]);
      map = atoi(argv[3]);
      EvgSetDBusMap(pEg, dbus, map);
    }
  else  
    EvgDBusDump(pEg);
  
  EvgClose(fdEg);

  return 0;
}
