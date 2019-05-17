/*
  simple.c -- Micro-Research Event Generator
  Application Programming Interface Test Application

  Author: Jukka Pietarinen (MRF)
  Date:   21.3.2019

*/

#include <stdint.h>
#include <endian.h>
#include <byteswap.h>
#include <errno.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <signal.h>
#include "egcpci.h"
#include "egapi.h"
#include "erapi.h"

int main(int argc, char *argv[])
{
  struct MrfEgRegs *pEg;
  struct MrfErRegs *pEr;
  int              fdEg;
  int              fdEr;
  int              i;
  char             c;

  if (argc < 1)
    {
      printf("Usage: %s /dev/ega3\n", argv[0]);
      printf("Assuming: /dev/ega3\n");
      argv[1] = "/dev/ega3";
    }

  fdEg = EvgOpen(&pEg, argv[1]);
  if (fdEg == -1)
    return errno;

  /* User internal fractional synth */
  EvgSetRFInput(pEg, 0, C_EVG_RFDIV_4);

  EvgSystemMasterEnable(pEg, 1);
  EvgBeaconEnable(pEg, 1);
  
  EvgEnable(pEg, 1);
  printf("Enabling Evg: %08x\n", EvgGetEnable(pEg));

  EvgEvanResetCount(pEg);
  EvgSetTriggerEvent(pEg, 0, 0x01, 1);
  EvgSetTriggerEvent(pEg, 1, 0x02, 1);
  printf("Setting up multiplexed couters\n");
  /* 1 Hz on MXC0 */
  EvgSetMXCPrescaler(pEg, 0, 125000000);
  /* 0.5 Hz on MXC1 */
  EvgSetMXCPrescaler(pEg, 1, 250000000);
  /* Reset counters */
  EvgSyncMxc(pEg);
  EvgSetMxcTrigMap(pEg, 0, 0);
  EvgSetMxcTrigMap(pEg, 1, 1);  
  EvgSetDBusMap(pEg, 0, C_EVG_DBUS_SEL_MXC);
  EvgSetDBusMap(pEg, 1, C_EVG_DBUS_SEL_MXC);
  EvgMXCDump(pEg);
  printf("DBus Event Enable %08x\n", EvgGetDBusEvent(pEg));

  printf("Press any key to exit.\n");
  getchar();

  EvgEnable(pEg, 0);
  printf("Disabling Evg: %08x\n", EvgGetEnable(pEg));
  EvgClose(fdEg);
}
