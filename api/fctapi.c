/**
@file fctapi.c
@brief MRF Event Master Fan-Out Concentrator specific functions.
@author Jukka Pietarinen
@date 8/12/2019
*/

#ifdef __unix__
#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <endian.h>
#include <byteswap.h>
#include <errno.h>
#include <signal.h>
#include <sys/ioctl.h>
#else /* If not Linux, assume VxWorks */
#ifndef VXWORKS
#define VXWORKS 1
#endif
#include <vxWorks.h>
#include <sysLib.h>
#include <vxLib.h>
#include <intLib.h>
#include <iosLib.h>
#include <taskLib.h>
#include <semLib.h>
#include <memLib.h>
#include <rebootLib.h>
#include <lstLib.h>
#include <vme.h>
#include <tickLib.h>
#include <iv.h>
#endif

#include <stdio.h>
#include <string.h>

#include "erapi.h"
#include "fracdiv.h"
#include "fctapi.h"
#include "sfpdiag.h"

/**
* Prints out Fan-Out Concentrator Delay Compensation status.
 * @param pFct MrfFctRegs structure.
 * @param reffreq Reference frequency in MHz.
 * @param ports Number of Concentrator ports.
*/ 
void FctDumpStatus(volatile struct MrfFctRegs *pFct, double reffreq, int ports)
{
  int i;
  
  printf("Up DC Value      %08lx (%3.6f ns)\n",
	 be32_to_cpu(pFct->UpDCValue),
	 be32_to_cpu(pFct->UpDCValue)/(reffreq*65.536));
  printf("RX FIFO DC Value %08lx (%3.6f ns)\n",
	 be32_to_cpu(pFct->FIFODCValue),
	 be32_to_cpu(pFct->FIFODCValue)/(reffreq*65.536));
  printf("Int. Delay Value %08lx (%3.6f ns)\n",
	 be32_to_cpu(pFct->IntDCValue),
	 be32_to_cpu(pFct->IntDCValue)/(reffreq*65.536));
  printf("RX FIFO Target   %08lx (%3.6f ns)\n",
	 be32_to_cpu(pFct->RXFIFOTarget),
	 be32_to_cpu(pFct->RXFIFOTarget)/(reffreq*65.536));
  printf("Topolygy ID      %08lx\n",
	 be32_to_cpu(pFct->TopologyID));
  
  printf("Link:     ");
  for(i = ports-1; i >= 0; i--)
    {
      if(be32_to_cpu(pFct->Status) & (1 << (C_FCT_STATUS_LINK1+i)))
	printf("1");
      else
	printf("0");
    }
  printf("\n");
  printf("Violation:");
  for(i = ports-1; i >= 0; i--)
    {
      if(be32_to_cpu(pFct->Status) & (1 << (C_FCT_STATUS_VIO1+i)))
	printf("1");
      else
	printf("0");
    }
  printf("\n");

  for(i = 0; i < ports; i++)
    {
      if (be32_to_cpu(pFct->PortDCStatus[i]) > 0)
	printf("Port %2d Status %d Loop Delay %08lx (%3.6f ns)\n", i+1,
	       be32_to_cpu(pFct->PortDCStatus[i]),
	       be32_to_cpu(pFct->PortDCValue[i]),
	       be32_to_cpu(pFct->PortDCValue[i])/(reffreq*65.536));
      else
	printf("Port %2d Status %d Loop Delay not valid\n", i+1,
	       be32_to_cpu(pFct->PortDCStatus[i]));
    }
  
  for(i = 0; i < 8; i++)
    {
      printf("SFP Port %d serial number: ", i+1);
      SFPSN((struct SFPDiag *) pFct->SFPDiag[i]);
      printf("\n");
    }
}

/**
 * Clear violation flags for concentrator input port.
 * @param pFct MrfFctRegs structure.
 * @param clear Bit mask for ports to try to clear the violation flag, LSB is for port 1.
 * @return Returns violation flag status before trying to clear flags.
*/ 
int FctGetViolation(volatile struct MrfFctRegs *pFct, int clear)
{
  int vio;

  vio = be32_to_cpu(pFct->Status) & 0xffff;
  if (clear)
    pFct->Control = be32_to_cpu(clear);

  return vio;
}
