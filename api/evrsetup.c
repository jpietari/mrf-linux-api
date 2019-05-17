/*
  evrsetup.c -- Micro-Research Event Receiver
                DC EVR simple setup

  Author: Jukka Pietarinen (MRF)
  Date:   21.03.2019

*/

#include <stdint.h>
#include <errno.h>
#include <stdio.h>
#include "erapi.h"

#define ERROR_TEXT "*** "

int err_gen;

int pcieout(volatile struct MrfErRegs *pEr)
{
  int i;

  EvrEnable(pEr, 1);
  if (!EvrGetEnable(pEr))
    {
      printf(ERROR_TEXT "Could not enable EVR!\n");
      err_gen++;
      return -1;
    }
  
  EvrSetTargetDelay(pEr, 0x01000000);
  EvrDCEnable(pEr, 1);
  
  EvrGetViolation(pEr, 1);

  /* Build configuration for EVR map RAMS */

  {
    int ram,code;

    for (ram = 0; ram < 2; ram++)
      {
	for (i = 0; i < 16; i++)
	  {
	    code = 1;
	    EvrSetLedEvent(pEr, ram, code, 1);
	    /* Pulse Triggers start at code 1 */
	    EvrSetPulseMap(pEr, ram, code, i, -1, -1);
	  }
      }
    
    for (i = 0; i < 16; i++)
      {
	EvrSetPulseParams(pEr, i, 1, i*100, 100);
	EvrSetPulseProperties(pEr, i, 0, 0, 0, 1, 1);
	EvrSetUnivOutMap(pEr, i, i);
	EvrSetTBOutMap(pEr, i, i);
      }
  }

  EvrSetPrescaler(pEr, 0, 0x00ffff);
  EvrSetUnivOutMap(pEr, 0, C_EVR_SIGNAL_MAP_PRESC);

  EvrSetExtEvent(pEr, 0, 0x01, 1, 0);

  EvrMapRamEnable(pEr, 0, 1);

  EvrOutputEnable(pEr, 1);

  return 0;
}

int main(int argc, char *argv[])
{
  struct MrfErRegs *pEr;
  int              fdEr;

  if (argc < 2)
    {
      printf("Usage: %s /dev/er3a3\n", argv[0]);
      printf("Assuming: /dev/er3a3\n");
      argv[1] = "/dev/er3a3";
    }

  fdEr = EvrOpen(&pEr, argv[1]);
  if (fdEr == -1)
    {
      return errno;
    }

  pcieout(pEr);

  EvrClose(fdEr);
}
