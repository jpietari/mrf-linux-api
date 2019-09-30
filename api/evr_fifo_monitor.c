/*
  evr_fifo_monitor.c -- Micro-Research Event Generator
  Application Programming Interface Test Application

  Author: Jukka Pietarinen (MRF)
  Date:   02.06.2019

*/

#include <stdint.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include "egcpci.h"
#include "erapi.h"

int main(int argc, char *argv[])
{
  struct MrfErRegs *pEr;
  int              fdEr;
  int              i;

  if (argc < 2)
    {
      printf("Usage: %s /dev/era3\n", argv[0]);
      return -1;
    }

  fdEr = EvrOpen(&pEr, argv[1]);
  if (fdEr < 0)
    {
      printf("EvrOpen returned %d, errno %d\n", fdEr, errno);
      return errno;
    }

  for (i = 1; i < 0x7e; i++)
    EvrSetFIFOEvent(pEr, 0, i, 1);

  EvrEnable (pEr, 1);
  EvrMapRamEnable(pEr, 0, 1);
  
  while (1)
    {
      EvrDumpFIFO(pEr);
      sleep(1);
    }

  EvrClose(fdEr);
}
