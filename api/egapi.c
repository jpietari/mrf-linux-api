/**
@file egapi.c
@brief Functions for Micro-Research Event Generator
       Application Programming Interface.
@author Jukka Pietarinen (MRF)
@date 12/5/2006
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
#include "egcpci.h"
#else/* assume VxWorks */
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
#include "egvme.h"
#endif
#include <stdio.h>
#include <string.h>
#include "egapi.h"
#include "erapi.h"
#include "fracdiv.h"

/*
#define DEBUG 1
*/
#define DEBUG_PRINTF printf

#ifdef __linux__
/**
Opens EVG device and mmaps the register map into user space.
@param pEg Pointer to pointer of memory mapped MrfEgRegs structure.
@param device_name Name of device e.g. /dev/ega3.
@return Returns file descriptor of opened file, -1 on error.
*/
int EvgOpen(struct MrfEgRegs **pEg, char *device_name)
{
  int fd;

  /* Open Event Generator device for read/write */
  fd = open(device_name, O_RDWR);
#ifdef DEBUG
  DEBUG_PRINTF("EvgOpen: open(\"%s\", O_RDWR) returned %d\n", device_name, fd);
#endif
  if (fd != -1)
    {
      /* Memory map Event Generator registers */
      *pEg = (struct MrfEgRegs *) mmap(0, EVG_MEM_WINDOW, PROT_READ | PROT_WRITE,
					MAP_SHARED, fd, 0);
#ifdef DEBUG
  DEBUG_PRINTF("EvgOpen: mmap returned %08x, errno %d\n", (int) *pEg,
	       errno);
#endif
      if (*pEg == MAP_FAILED)
	{
	  close(fd);
	  return -1;
	}
      /* Put device in BE mode */
      (*pEg)->Control = ((*pEg)->Control) & (~0x02000002);
    }

  return fd;
}
#else
int EvgOpen(struct MrfEgRegs **pEg, char *device_name)
{
  return 0;
}
#endif

#ifdef __linux__
/**
Close EVG device opened with EvrOpen.
@param fd File descriptor of EVG device returned by EvgOpen.
@return Returns 0 on successful completion.
*/
int EvgClose(int fd)
{
  int result;

  result = munmap(0, EVG_MEM_WINDOW);
  return close(fd);
}
#else
int EvgClose(int fd)
{
  return 0;
}
#endif

/**
Retrieve EVG firmware version.
@param pEg Pointer to MrfEgRegs structure
@return Returns firmware version
*/
u32 EvgFWVersion(volatile struct MrfEgRegs *pEg)
{
  return be32_to_cpu(pEg->FPGAVersion);
}

/**
Enable/disable EVG.
@param pEg Pointer to MrfEgRegs structure
@param state 0 - disable, 1 - enable
@return Returns state read back from EVG.
*/
int EvgEnable(volatile struct MrfEgRegs *pEg, int state)
{
  if (state)
    pEg->Control |= be32_to_cpu(1 << C_EVG_CTRL_MASTER_ENABLE);
  else
    pEg->Control &= be32_to_cpu(~(1 << C_EVG_CTRL_MASTER_ENABLE));
  
  return EvgGetEnable(pEg);
}

/**
Get EVG enable state.
@param pEg Pointer to MrfEgRegs structure
@return Returns EVG state, 0 - disabled, non-zero - enabled.
*/
int EvgGetEnable(volatile struct MrfEgRegs *pEg)
{
  return be32_to_cpu(pEg->Control & be32_to_cpu(1 << C_EVG_CTRL_MASTER_ENABLE));
}

/**
Enable/disable EVG as system master.
@param pEg Pointer to MrfEgRegs structure
@param state 0 - EVG is not timing system master, 1 - EVG is timing system master
@return Returns state read back from EVG.
*/
int EvgSystemMasterEnable(volatile struct MrfEgRegs *pEg, int state)
{
  if (state)
    pEg->Control |= be32_to_cpu(1 << C_EVG_CTRL_DCMASTER_ENABLE);
  else
    pEg->Control &= be32_to_cpu(~(1 << C_EVG_CTRL_DCMASTER_ENABLE));
  
  return EvgGetSystemMasterEnable(pEg);
}

/**
Get EVG as system master state.
@param pEg Pointer to MrfEgRegs structure
@return 0 - EVG is not timing system master, non-zero - EVG is timing system master
*/
int EvgGetSystemMasterEnable(volatile struct MrfEgRegs *pEg)
{
  return be32_to_cpu(pEg->Control & be32_to_cpu(1 << C_EVG_CTRL_DCMASTER_ENABLE));
}

/**
Enable/disable EVG delay compensation beacon generator.
@param pEg Pointer to MrfEgRegs structure
@param state 0 - disable beacon generator, 1 - enable beacon generator
@return Returns state read back from EVG.
*/
int EvgBeaconEnable(volatile struct MrfEgRegs *pEg, int state)
{
  if (state)
    pEg->Control |= be32_to_cpu(1 << C_EVG_CTRL_BEACON_ENABLE);
  else
    pEg->Control &= be32_to_cpu(~(1 << C_EVG_CTRL_BEACON_ENABLE));
  
  return EvgGetBeaconEnable(pEg);
}

/**
Get EVG delay compensation beacon generator state.
@param pEg Pointer to MrfEgRegs structure
@return 0 - beacon generator disabled, non-zero - beacon generator enabled
*/
int EvgGetBeaconEnable(volatile struct MrfEgRegs *pEg)
{
  return be32_to_cpu(pEg->Control & be32_to_cpu(1 << C_EVG_CTRL_BEACON_ENABLE));
}

/**
Enable/disable EVG upstream SFP receiver port.
@param pEg Pointer to MrfEgRegs structure
@param state 0 - disable upstream SFP receiver port, 1 - enable upstream SFP receiver port
@return Returns state read back from EVG.
*/
int EvgRxEnable(volatile struct MrfEgRegs *pEg, int state)
{
  if (!state)
    pEg->Control |= be32_to_cpu((1 << C_EVG_CTRL_RX_DISABLE) |
				(1 << C_EVG_CTRL_RX_PWRDOWN));
  else
    pEg->Control &= be32_to_cpu(~((1 << C_EVG_CTRL_RX_DISABLE) |
				  (1 << C_EVG_CTRL_RX_PWRDOWN)));
  
  return EvgRxGetEnable(pEg);
}

/**
Get EVG upstream SFP receiver port state.
@param pEg Pointer to MrfEgRegs structure
@return 0 - upstream SFP receiver port disabled, 1 - upstream SFP receiver port enabled
*/
int EvgRxGetEnable(volatile struct MrfEgRegs *pEg)
{
  return be32_to_cpu(~(pEg->Control) &
		     be32_to_cpu((1 << C_EVG_CTRL_RX_DISABLE) |
				 (1 << C_EVG_CTRL_RX_PWRDOWN)));
}

/**
Get/clear EVG upstream link violation flag. The upstream link violation flag is set when an event link receive error is detected and the upstream port is enabled.
@param pEg Pointer to MrfEgRegs structure
@param clear 0 - just read, do not try to clear, 1 - clear violation flag
@return Returns EVG upstream link violation flag state, 0 - no violation, non-zero - violation detected.
*/
int EvgGetViolation(volatile struct MrfEgRegs *pEg, int clear)
{
  int result;

  result = be32_to_cpu(pEg->IrqFlag & be32_to_cpu(1 << C_EVG_IRQFLAG_VIOLATION));
  if (clear && result)
    pEg->IrqFlag = be32_to_cpu(result);

  return result;
}

/**
Enable/disable EVG software events.
@param pEg Pointer to MrfEgRegs structure
@param state 0 - disable sofwtare events, 1 - enable software events
@return Returns state read back from EVG.
*/
int EvgSWEventEnable(volatile struct MrfEgRegs *pEg, int state)
{
  unsigned int mask = ~((1 << (C_EVG_SWEVENT_CODE_HIGH + 1)) -
    (1 << C_EVG_SWEVENT_CODE_LOW));
  int swe;

  swe = be32_to_cpu(pEg->SWEvent);
  if (state)
    pEg->SWEvent = be32_to_cpu(1 << C_EVG_SWEVENT_ENABLE | (swe & mask));
  else
    pEg->SWEvent = be32_to_cpu(~(1 << C_EVG_SWEVENT_ENABLE) & swe & mask);
  return EvgGetSWEventEnable(pEg);
}

/**
Get EVG software event state.
@param pEg Pointer to MrfEgRegs structure
@return 0 - sofwtare events disabled , non-zero - software events enabled
*/
int EvgGetSWEventEnable(volatile struct MrfEgRegs *pEg)
{
  return be32_to_cpu(pEg->SWEvent & be32_to_cpu(1 << C_EVG_SWEVENT_ENABLE));
}

/**
Send EVG software event.
@param pEg Pointer to MrfEgRegs structure
@param code Event code to send
*/
int EvgSendSWEvent(volatile struct MrfEgRegs *pEg, int code)
{
  unsigned int mask = ~((1 << (C_EVG_SWEVENT_CODE_HIGH + 1)) -
    (1 << C_EVG_SWEVENT_CODE_LOW));
  int swcode;

  swcode = be32_to_cpu(pEg->SWEvent);
  swcode &= mask;
  swcode |= (code & EVG_MAX_EVENT_CODE);

  pEg->SWEvent = be32_to_cpu(swcode);

  return be32_to_cpu(pEg->SWEvent);
}

/**
Enable/disable EVG event analyzer.
@param pEg Pointer to MrfEgRegs structure
@param state 0 - disable event analyzer, 1 - enable event analyzer
@return Returns state read back from EVG.
*/
int EvgEvanEnable(volatile struct MrfEgRegs *pEg, int state)
{
  if (state)
    pEg->EvanControl |= be32_to_cpu(1 << C_EVG_EVANCTRL_ENABLE);
  else
    pEg->EvanControl &= be32_to_cpu(~(1 << C_EVG_EVANCTRL_ENABLE));
  
  return EvgEvanGetEnable(pEg);
}

/**
Get EVG event analyzer state.
@param pEg Pointer to MrfEgRegs structure
@return 0 - event analyzer disabled, 1 - event analyzer enabled
*/
int EvgEvanGetEnable(volatile struct MrfEgRegs *pEg)
{
  return be32_to_cpu(pEg->EvanControl & be32_to_cpu(1 << C_EVG_EVANCTRL_ENABLE)); 
}

/**
Clear EVG event analyzer buffer.
@param pEg Pointer to MrfEgRegs structure
*/
void EvgEvanReset(volatile struct MrfEgRegs *pEg)
{
  struct EvanStruct evan;

  pEg->EvanControl |= be32_to_cpu(1 << C_EVG_EVANCTRL_RESET);
  /* Dummy read to clear FIFO */
  EvgEvanGetEvent(pEg, &evan);
}

/**
Reset EVG event analyzer time counter.
@param pEg Pointer to MrfEgRegs structure
*/
void EvgEvanResetCount(volatile struct MrfEgRegs *pEg)
{
  pEg->EvanControl |= be32_to_cpu(1 << C_EVG_EVANCTRL_COUNTRES);
}

/**
Get EVG event analyzer event.
@param pEg Pointer to MrfEgRegs structure
@param evan Pointer to EvanStruct structure to copy event to
@return -1 on error (no event available), 0 on success
*/
int EvgEvanGetEvent(volatile struct MrfEgRegs *pEg, struct EvanStruct *evan)
{
  if (pEg->EvanControl & be32_to_cpu(1 << C_EVG_EVANCTRL_NOTEMPTY))
    {
      /* Reading the event code & dbus data, pops the next item first from the event
	 analyzer fifo */
      evan->EventCode = be32_to_cpu(pEg->EvanCode);
      evan->TimestampHigh = be32_to_cpu(pEg->EvanTimeH);
      evan->TimestampLow = be32_to_cpu(pEg->EvanTimeL);
      return 0;
    }
  return -1;
}

/**
Dump event analyzer memory.
@param pEg Pointer to MrfEgRegs structure
*/
void EvgEvanDump(volatile struct MrfEgRegs *pEg)
{
  struct EvanStruct evan;
  double ts;

  while (EvgEvanGetEvent(pEg, &evan) == 0)
    {
      ts = (((double) evan.TimestampHigh) * 65536.0*65536.0 + evan.TimestampLow) / (499654000.0/4.0);
      printf("%08x:%08x %3.9g %02x\n", evan.TimestampHigh, evan.TimestampLow, ts, evan.EventCode); 
    }
}

/**
Set EVG multiplexed counter prescaler value.
@param pEg Pointer to MrfEgRegs structure
@param mxc Multiplexed counter number 0 - 7
@param presc Prescaler value
*/
int EvgSetMXCPrescaler(volatile struct MrfEgRegs *pEg, int mxc, unsigned int presc)
{
  if (mxc < 0 || mxc >= EVG_MAX_MXCS)
    return -1;

  pEg->MXC[mxc].Prescaler = be32_to_cpu(presc);

  return 0;
}

/**
Get EVG multiplexed counter prescaler value.
@param pEg Pointer to MrfEgRegs structure
@param mxc Multiplexed counter number 0 - 7
@return Prescaler value
*/
unsigned int EvgGetMXCPrescaler(volatile struct MrfEgRegs *pEg, int mxc)
{
  if (mxc < 0 || mxc >= EVG_MAX_MXCS)
    return -1;

  return (unsigned int) be32_to_cpu(pEg->MXC[mxc].Prescaler);
}

/**
Set EVG multiplexed counter to trigger event mapping
@param pEg Pointer to MrfEgRegs structure
@param mxc Multiplexed counter number 0 - 7
@param map Trigger event number to trigger from multiplexed counter
*/
int EvgSetMxcTrigMap(volatile struct MrfEgRegs *pEg, int mxc, int map)
{
  if (mxc < 0 || mxc >= EVG_MAX_MXCS)
    return -1;

  if (map < -1 || map >= EVG_MAX_TRIGGERS)
    return -1;

  if (map >= 0)
    pEg->MXC[mxc].Control = be32_to_cpu(1 << (map + C_EVG_MXCMAP_TRIG_BASE));
  else
    pEg->MXC[mxc].Control = be32_to_cpu(0);

  return be32_to_cpu(pEg->MXC[mxc].Control) & 0x7fffffff;
}

/**
Synchronize EVG multiplexed counters.
@param pEg Pointer to MrfEgRegs structure
*/
void EvgSyncMxc(volatile struct MrfEgRegs *pEg)
{
  pEg->Control |= be32_to_cpu(1 << C_EVG_CTRL_MXC_RESET);
}

/**
Show EVG multiplexed counter settings.
@param pEg Pointer to MrfEgRegs structure
*/
void EvgMXCDump(volatile struct MrfEgRegs *pEg)
{
  int mxc;

  for (mxc = 0; mxc < EVG_MXCS; mxc++)
    {
      DEBUG_PRINTF("MXC%d Prescaler %08x (%d) Trig %08x State %d\n",
		   mxc,
		   be32_to_cpu(pEg->MXC[mxc].Prescaler),
		   be32_to_cpu(pEg->MXC[mxc].Prescaler),
		   be32_to_cpu(pEg->MXC[mxc].Control) & 
		   ((1 << EVG_MAX_TRIGGERS) - 1),
		   (be32_to_cpu(pEg->MXC[mxc].Control) & (1 << C_EVG_MXC_READ))
		   ? 1 : 0);
    }
}

/**
Set EVG distributed bus bit mapping
@param pEg Pointer to MrfEgRegs structure
@param dbus Distributed bus bit number
@param map Distributed bus bit mapping \n 
           0 - no mapping, distributed bus bit low \n 
           1 - from external input \n 
	   2 - from multiplexed counter \n 
           3 - forward from upstream EVG
*/
int EvgSetDBusMap(volatile struct MrfEgRegs *pEg, int dbus, int map)
{
  int mask;

  if (dbus < 0 || dbus >= EVG_DBUS_BITS)
    return -1;

  if (map < 0 || map > C_EVG_DBUS_SEL_MASK)
    return -1;

  mask = ~(C_EVG_DBUS_SEL_MASK << (dbus*C_EVG_DBUS_SEL_BITS));
  pEg->DBusMap &= be32_to_cpu(mask);
  pEg->DBusMap |= be32_to_cpu(map << (dbus*C_EVG_DBUS_SEL_BITS));

  return 0;
}

/**
Show EVG distributed bus settings.
@param pEg Pointer to MrfEgRegs structure
*/
void EvgDBusDump(volatile struct MrfEgRegs *pEg)
{
  int dbus;

  for (dbus = 0; dbus < EVG_DBUS_BITS; dbus++)
    {
      DEBUG_PRINTF("DBUS%d ", dbus);
      switch ((be32_to_cpu(pEg->DBusMap) >> (dbus*C_EVG_DBUS_SEL_BITS)) &
	      ((1 << C_EVG_DBUS_SEL_BITS) - 1))
	{
	case C_EVG_DBUS_SEL_OFF:
	  DEBUG_PRINTF("OFF\n");
	  break;
	case C_EVG_DBUS_SEL_EXT:
	  DEBUG_PRINTF("EXT\n");
	  break;
	case C_EVG_DBUS_SEL_MXC:
	  DEBUG_PRINTF("MXC\n");
	  break;
	case C_EVG_DBUS_SEL_FORWARD:
	  DEBUG_PRINTF("FWD\n");
	  break;
	default:
	  DEBUG_PRINTF("Unknown\n");
	  break;
	}
    }
}

/**
Set EVG distributed bus timestamping events enable
@param pEg Pointer to MrfEgRegs structure
@param enable Enable mask, see table \ref dbusevents for bit mapping

<table>
<caption id="dbusevents">Distributed bus timestamping events mask</caption>
<tr><th>Bit<th>Event
<tr><td>7<td>enable for distributed bus input 7 to 0x71 seconds '1' event
<tr><td>6<td>enable for distributed bus input 6 to 0x70 seconds '0' event
<tr><td>5<td>enable for distributed bus input 5 to 0x7D timestamp reset event
</table>
*/
int EvgSetDBusEvent(volatile struct MrfEgRegs *pEg, int enable)
{
  pEg->DBusEvent = be32_to_cpu(enable);
}

/**
Get EVG distributed bus timestamping events enable
@param pEg Pointer to MrfEgRegs structure
@return Enable mask, see table \ref dbusevents for bit mapping
*/
int EvgGetDBusEvent(volatile struct MrfEgRegs *pEg)
{
  return be32_to_cpu(pEg->DBusEvent);
}

/**
Set EVG AC Input
@param pEg Pointer to MrfEgRegs structure
@param bypass 0 - Mains logic enabled, 1 - Mains logic bypassed
@param sync Synchronization select \n 
            0 - Synchronize to event clock \n 
            1 - Synchronize to multiplexed counter 7 \n 
            3 - Synchronize to TTL input 1 \n 
            5 - Synchronize to TTL input 2
@param div AC input divider 0 to 255 (value of 0 results in divide by 256)
@param delay delay in 0.1 ms steps, 0 to 25.5 ms
*/
int EvgSetACInput(volatile struct MrfEgRegs *pEg, int bypass, int sync, int div, int delay)
{
  unsigned int result;

  result = be32_to_cpu(pEg->ACControl);

  if (bypass == 0)
    result &= ~(1 << C_EVG_ACCTRL_BYPASS);
  if (bypass == 1)
    result |= (1 << C_EVG_ACCTRL_BYPASS);

  if ((sync & 1) == 0)
    result &= ~(1 << C_EVG_ACCTRL_ACSYNC);
  if ((sync & 1) == 1)
    result |= (1 << C_EVG_ACCTRL_ACSYNC);

  if ((sync & 2) == 0)
    result &= ~(1 << C_EVG_ACCTRL_ACSYNC_1);
  if ((sync & 2) != 0)
    result |= (1 << C_EVG_ACCTRL_ACSYNC_1);

  if ((sync & 4) == 0)
    result &= ~(1 << C_EVG_ACCTRL_ACSYNC_2);
  if ((sync & 4) != 0)
    result |= (1 << C_EVG_ACCTRL_ACSYNC_2);

  if (div > 0 && div < (2 << (C_EVG_ACCTRL_DIV_HIGH - C_EVG_ACCTRL_DIV_LOW)))
    {
      result &= ~((2 << C_EVG_ACCTRL_DIV_HIGH) - (1 << C_EVG_ACCTRL_DIV_LOW));
      result |= div << C_EVG_ACCTRL_DIV_LOW;
    }

  if (delay > 0 && delay < (2 << (C_EVG_ACCTRL_DELAY_HIGH - C_EVG_ACCTRL_DELAY_LOW)))
    {
      result &= ~((2 << C_EVG_ACCTRL_DELAY_HIGH) - (1 << C_EVG_ACCTRL_DELAY_LOW));
      result |= delay << C_EVG_ACCTRL_DELAY_LOW;
    }

  pEg->ACControl = be32_to_cpu(result);

  return 0;
}

/**
Set EVG AC Input
@param pEg Pointer to MrfEgRegs structure
@param map Event trigger number to map AC trigger to, -1 to disable
*/
int EvgSetACMap(volatile struct MrfEgRegs *pEg, int map)
{
  if (map > EVG_MAX_TRIGGERS)
    return -1;

  if (map >= 0)
    pEg->ACMap = be32_to_cpu(1 << map);
  else
    pEg->ACMap = 0;

  return 0;
}

/**
Show EVG AC Input settings
@param pEg Pointer to MrfEgRegs structure
*/
void EvgACDump(volatile struct MrfEgRegs *pEg)
{
  unsigned int result;

  result = be32_to_cpu(pEg->ACControl);
  DEBUG_PRINTF("AC Input div %d delay %d", (result &
					    ((2 << (C_EVG_ACCTRL_DIV_HIGH))
					     - (1 << (C_EVG_ACCTRL_DIV_LOW))))
	       >> (C_EVG_ACCTRL_DIV_LOW),
	       (result &
		((2 << (C_EVG_ACCTRL_DELAY_HIGH))
		 - (1 << (C_EVG_ACCTRL_DELAY_LOW))))
	       >> (C_EVG_ACCTRL_DELAY_LOW));
  if (result & (1 << (C_EVG_ACCTRL_BYPASS)))
    DEBUG_PRINTF(" BYPASS");
  if (result & (1 << (C_EVG_ACCTRL_ACSYNC)))
    DEBUG_PRINTF(" SYNCMXC7");
  DEBUG_PRINTF("\n");
}

/**
Set EVG RF Input
@param pEg Pointer to MrfEgRegs structure
@param RFsel Event clock source select \n 
             0 - Internal reference (fractional synthesizer) \n 
	     1 - External RF input \n 
	     2 - PXIe 100 MHz clock (PXIe-EVG-300 only) \n 
             4 - Recovered upstream EVG clock, Fan-Out mode \n 
             5 - External RF input for downstream ports, internal reference for upstream port, Fan-Out mode, event rate down conversion \n 
	     6 - PXIe 10 MHz clock through 10 x clock multiplier (PXIe-EVG-300 only) \n 
	     7 - Recovered upstream EVG clock /2 decimate mode, Fan-Out mode
@param div Divider \n
           0 - direct RF frequency \n 
	   1 - RF/2 \n 
	   2 - RF/3 \n 
	   3 - RF/4 \n 
	   4 - RF/5 \n 
	   5 - RF/6 \n 
	   6 - RF/7 \n 
	   7 - RF/8 \n 
	   8 - RF/9 \n 
	   9 - RF/10 \n 
	   10 - RF/11 \n 
	   11 - RF/12 \n 
	   12 - OFF \n 
	   13 - RF/14 \n 
	   14 - RF/15 \n 
	   15 - RF/16 \n 
	   16 - RF/17 \n 
	   17 - RF/18 \n 
	   18 - RF/19 \n 
	   19 - RF/20 \n 
	   20 - RF/21 \n 
	   21 - RF/22 \n 
	   22 - RF/23 \n 
	   23 - RF/24 \n 
	   24 - RF/25 \n 
	   25 - RF/26 \n 
	   26 - RF/27 \n 
	   27 - RF/28 \n 
	   28 - RF/29 \n 
	   29 - RF/30 \n 
	   30 - RF/31 \n 
	   31 - RF/32
*/
int EvgSetRFInput(volatile struct MrfEgRegs *pEg, int RFsel, int div)
{
  int rfdiv;

  rfdiv = be32_to_cpu(pEg->ClockControl);

  rfdiv &= ~(C_EVG_CLKCTRL_RFSELMASK);
  if (RFsel >= 0 && RFsel <= C_EVG_CLKCTRL_MAX_RFSEL)
    rfdiv |= (RFsel << C_EVG_CLKCTRL_RFSEL);

  if (div >= 0 && div <= C_EVG_RFDIV_MASK)
    {
      rfdiv &= ~(C_EVG_RFDIV_MASK << C_EVG_CLKCTRL_DIV_LOW);
      rfdiv |= div << C_EVG_CLKCTRL_DIV_LOW;
    }
    
  pEg->ClockControl = be32_to_cpu(rfdiv);

  return 0;
}

/**
Set up fractional synthesizer that generates reference clock for event clock

@param pEg Pointer to MrfEgRegs structure
@param fracdiv Control word

The control word can be generated from a reference frequency by using function freq_to_cw().
*/
int EvgSetFracDiv(volatile struct MrfEgRegs *pEg, int fracdiv)
{
  pEg->UsecDiv = be32_to_cpu((int) cw_to_freq(fracdiv));

  return be32_to_cpu(pEg->FracDiv = be32_to_cpu(fracdiv));
}

/**
Get fractional synthesizer control word

@param pEg Pointer to MrfEgRegs structure
@return fracdiv Control word

Use function cw_to_freq() to convert the control word to frequency.
*/
int EvgGetFracDiv(volatile struct MrfEgRegs *pEg)
{
  return be32_to_cpu(pEg->FracDiv);
}

/**
Set sequence RAM event. This function writes an event into the sequence RAM.

@param pEg Pointer to MrfEgRegs structure
@param ram RAM number
@param pos Sequence RAM position (0 to 2047)
@param timestamp 32 bit timestamp of event
@param code Event code
*/
int EvgSetSeqRamEvent(volatile struct MrfEgRegs *pEg, int ram, int pos, unsigned int timestamp, int code)
{
  if (ram < 0 || ram >= EVG_SEQRAMS)
    return -1;

  if (pos < 0 || pos >= EVG_MAX_SEQRAMEV)
    return -1;

  if (code < 0 || code > EVG_MAX_EVENT_CODE)
    return -1;

  pEg->SeqRam[ram][pos].Timestamp = be32_to_cpu(timestamp);
  pEg->SeqRam[ram][pos].EventCode = be32_to_cpu(code);

  return 0;
}

/**
Get sequence RAM event timestamp.

@param pEr Pointer to MrfEgRegs structure
@param ram RAM number
@param pos Sequence RAM position (0 to 2047)
@return 32 bit timestamp of event at RAM position pos
*/
unsigned int EvgGetSeqRamTimestamp(volatile struct MrfEgRegs *pEg, int ram, int pos)
{
  if (ram < 0 || ram >= EVG_SEQRAMS)
    return -1;

  if (pos < 0 || pos >= EVG_MAX_SEQRAMEV)
    return -1;

  return be32_to_cpu(pEg->SeqRam[ram][pos].Timestamp);
}

/**
Get sequence RAM event code.

@param pEg Pointer to MrfEgRegs structure
@param ram RAM number
@param pos Sequence RAM position (0 to 2047)
@return Event code at RAM position pos
*/
int EvgGetSeqRamEvent(volatile struct MrfEgRegs *pEg, int ram, int pos)
{
  if (ram < 0 || ram >= EVG_SEQRAMS)
    return -1;

  if (pos < 0 || pos >= EVG_MAX_SEQRAMEV)
    return -1;

  return be32_to_cpu(pEg->SeqRam[ram][pos].EventCode);
}

/**
Show sequence RAM contents.

@param pEg Pointer to MrfEgRegs structure
@param ram RAM number
*/
void EvgSeqRamDump(volatile struct MrfEgRegs *pEg, int ram)
{
  int pos;

  if (ram < 0 || ram >= EVG_SEQRAMS)
    return;
 
  for (pos = 0; pos < EVG_MAX_SEQRAMEV; pos++)
    if (pEg->SeqRam[ram][pos].EventCode)
      DEBUG_PRINTF("Ram%d: Timestamp %08x Code %02x\n",
		   ram,
		   be32_to_cpu(pEg->SeqRam[ram][pos].Timestamp),
		   be32_to_cpu(pEg->SeqRam[ram][pos].EventCode));
}

/**
Control sequence RAM.

@param pEg Pointer to MrfEgRegs structure
@param ram RAM number
@param enable 0 - disable sequence RAM, 1 - enable sequence RAM
@param single 1 - select single sequence mode
@param recycle 1 - select recycle sequence mode
@param reset 1 - reset sequence RAM
@param trigsel See table \ref evgtrigsel

<table>
<caption id="evgtrigsel">Sequence RAM trigger selection</caption>
<tr><th>ID<th>Trigger
<tr><td>0x00<td>Multiplexed counter 0
<tr><td>0x01<td>Multiplexed counter 1
<tr><td>...<td>...
<tr><td>0x07<td>Multiplexed counter 7
<tr><td>0x10<td>AC synchronization logic
<tr><td>0x11<td>Sequence RAM 0 software trigger
<tr><td>0x12<td>Sequence RAM 1 software trigger
<tr><td>0x13<td>Continuous trigger
<tr><td>0x18<td>Sequence RAM 0 external trigger
<tr><td>0x19<td>Sequence RAM 1 external trigger
<tr><td>0x1F<td>Trigger disabled
</table>
*/
int EvgSeqRamControl(volatile struct MrfEgRegs *pEg, int ram, int enable, int single, int recycle, int reset, int trigsel)
{
  int control;

  if (ram < 0 || ram >= EVG_SEQRAMS)
    return -1;

  control = be32_to_cpu(pEg->SeqRamControl[ram]);

  if (enable == 0)
    control |= (1 << C_EVG_SQRC_DISABLE);
  if (enable == 1)
    control |= (1 << C_EVG_SQRC_ENABLE);

  if (single == 0)
    control &= ~(1 << C_EVG_SQRC_SINGLE);
  if (single == 1)
    control |= (1 << C_EVG_SQRC_SINGLE);
  
  if (recycle == 0)
    control &= ~(1 << C_EVG_SQRC_RECYCLE);
  if (recycle == 1)
    control |= (1 << C_EVG_SQRC_RECYCLE);
  
  if (reset == 1)
    control |= (1 << C_EVG_SQRC_RESET);

  if (trigsel >= 0 && trigsel <= C_EVG_SEQTRIG_MAX)
    {
      control &= ~(C_EVG_SEQTRIG_MAX << C_EVG_SQRC_TRIGSEL_LOW);
      control |= trigsel << C_EVG_SQRC_TRIGSEL_LOW;
    }

  pEg->SeqRamControl[ram] = be32_to_cpu(control);

  return 0;
}
					  
/**
Software trigger sequence RAM.

@param pEg Pointer to MrfEgRegs structure
@param ram RAM number, 0 for EVR
*/
int EvgSeqRamSWTrig(volatile struct MrfEgRegs *pEg, int ram)
{
  if (ram < 0 || ram > 1)
    return -1;

  pEg->SeqRamControl[ram] |= be32_to_cpu(1 << C_EVG_SQRC_SWTRIGGER);
  
  return 0;
}

/**
Show sequence RAM status.

@param pEg Pointer to MrfEgRegs structure
@param ram RAM number
*/
void EvgSeqRamStatus(volatile struct MrfEgRegs *pEg, int ram)
{
  int control;

  if (ram < 0 || ram >= EVG_SEQRAMS)
    return;
  
  control = be32_to_cpu(pEg->SeqRamControl[ram]);
  
  DEBUG_PRINTF("RAM%d:", ram);
  if (control & (1 << C_EVG_SQRC_RUNNING))
    DEBUG_PRINTF(" RUN");
  if (control & (1 << C_EVG_SQRC_ENABLED))
    DEBUG_PRINTF(" ENA");
  if (control & (1 << C_EVG_SQRC_SINGLE))
    DEBUG_PRINTF(" SINGLE");
  if (control & (1 << C_EVG_SQRC_RECYCLE))
    DEBUG_PRINTF(" RECYCLE");
  DEBUG_PRINTF(" Trigsel %02x\n", (control >> C_EVG_SQRC_TRIGSEL_LOW) & C_EVG_SEQTRIG_MAX);
}

/**
Set up Universal I/O Input Mappings.

@param pEg Pointer to MrfEgRegs structure
@param univ Number of Universal I/O input
@param trig Number of Event trigger to trigger, -1 for no trigger
@param dbus Number of Distributed bus bit to map input to, -1 for no mapping
@param irq External interrupt mapping, 0 = no interrupt, 1 = mapped to interrupt
@param seqtrig Number of sequence RAM trigger, -1 for no trigger 
*/
int EvgSetUnivinMap(volatile struct MrfEgRegs *pEg, int univ, int trig, int dbus, int irq, int seqtrig)
{
  int map = 0;

  if (univ < 0 || univ >= EVG_MAX_UNIVIN_MAP)
    return -1;

  if (trig >= EVG_MAX_TRIGGERS)
    return -1;

  if (dbus >= EVG_DBUS_BITS)
    return -1;

  if (seqtrig >= EVG_MAX_SEQRAMS)
    return -1;

  if (trig >= 0)
    map |= (1 << (C_EVG_INMAP_TRIG_BASE + trig));

  if (dbus >= 0)
    map |= (1 << (C_EVG_INMAP_DBUS_BASE + dbus));

  if (irq)
    map |= (1 << (C_EVG_INMAP_IRQ));

  if (seqtrig >= 0)
    map |= (1 << (C_EVG_INMAP_SEQTRIG_BASE + seqtrig));

  pEg->UnivInMap[univ] = be32_to_cpu(map);

  return 0;
}

/**
Get Universal I/O Input to Event Trigger Mappings.

@param pEg Pointer to MrfEgRegs structure
@param univ Number of Universal I/O input
@return Number of Event trigger to trigger, -1 for no trigger
*/
int EvgGetUnivinMapTrigger(volatile struct MrfEgRegs *pEg, int univ)
{
  int mask, i;

  if (univ < 0 || univ >= EVG_MAX_UNIVIN_MAP)
    return -1;

  mask = (be32_to_cpu(pEg->UnivInMap[univ]) >> C_EVG_INMAP_TRIG_BASE) &
    ((1 << EVG_MAX_TRIGGERS) - 1);
  if (!mask)
    return -1;
  for (i = 0; !(mask & 1); i++)
    mask >>= 1;

  return i;
}

/**
Get Universal I/O Input to Distributed Bus Bit Mappings.

@param pEg Pointer to MrfEgRegs structure
@param univ Number of Universal I/O input
@return Number of Distributed bus bit input is mapped to, -1 for no trigger
*/
int EvgGetUnivinMapDBus(volatile struct MrfEgRegs *pEg, int univ)
{
  int mask, i;

  if (univ < 0 || univ >= EVG_MAX_UNIVIN_MAP)
    return -1;

  mask = (be32_to_cpu(pEg->UnivInMap[univ]) >> C_EVG_INMAP_DBUS_BASE) &
    ((1 << EVG_DBUS_BITS) - 1);
  if (!mask)
    return -1;
  for (i = 0; !(mask & 1); i++)
    mask >>= 1;

  return i;
}

/**
Get Universal I/O Input External Interrupt Mapping.

@param pEg Pointer to MrfEgRegs structure
@param univ Number of Universal I/O input
@return 0 = Interrupt not mapped, 1 = interrupt mapped.
*/
int EvgGetUnivinMapIrq(volatile struct MrfEgRegs *pEg, int univ)
{
  int mask;

  if (univ < 0 || univ >= EVG_MAX_UNIVIN_MAP)
    return -1;

  mask = (be32_to_cpu(pEg->UnivInMap[univ]) >> C_EVG_INMAP_IRQ) & 1;

  return mask;
}

/**
Get Universal I/O Input Sequence RAM Trigger Mapping.

@param pEg Pointer to MrfEgRegs structure
@param univ Number of Universal I/O input
@return Number of sequencer trigger, -1 no trigger.
*/
int EvgGetUnivinMapSeqtrig(volatile struct MrfEgRegs *pEg, int univ)
{
  int mask, i;

  if (univ < 0 || univ >= EVG_MAX_UNIVIN_MAP)
    return -1;

  mask = (be32_to_cpu(pEg->UnivInMap[univ]) >> C_EVG_INMAP_SEQTRIG_BASE) &
    ((1 << EVG_MAX_SEQRAMS) - 1);
  if (!mask)
    return -1;
  for (i = 0; !(mask & 1); i++)
    mask >>= 1;

  return i;
}

/**
Show Universal I/O Input Mappings.

@param pEg Pointer to MrfEgRegs structure
*/
void EvgUnivinDump(volatile struct MrfEgRegs *pEg)
{
  int univ;

  for (univ = 0; univ < EVG_MAX_UNIVIN_MAP; univ++)
    {
      int map = be32_to_cpu(pEg->UnivInMap[univ]); 
      DEBUG_PRINTF("UnivIn%d Mapped to Trig %08x, DBus %02x, IRQ %d, seqtrig %d\n", univ,
		   (map >> C_EVG_INMAP_TRIG_BASE)
		   & ((1 << EVG_MAX_TRIGGERS) - 1),
		   (map >> C_EVG_INMAP_DBUS_BASE)
		   & ((1 << EVG_DBUS_BITS) - 1),
		   (map >> C_EVG_INMAP_IRQ) & 1,
		   (map >> C_EVG_INMAP_SEQTRIG_BASE)
		   & ((1 << EVG_MAX_SEQRAMS) - 1));
    }
}

/**
Set up Front panel Input Mappings.

@param pEg Pointer to MrfEgRegs structure
@param univ Number of Front panel input
@param trig Number of Event trigger to trigger, -1 for no trigger
@param dbus Number of Distributed bus bit to map input to, -1 for no mapping
@param irq External interrupt mapping, 0 = no interrupt, 1 = mapped to interrupt
@param seqtrig Number of sequence RAM trigger, -1 for no trigger 
*/
int EvgSetFPinMap(volatile struct MrfEgRegs *pEg, int univ, int trig, int dbus, int irq, int seqtrig)
{
  int map = 0;

  if (univ < 0 || univ >= EVG_MAX_UNIVIN_MAP)
    return -1;

  if (trig >= EVG_MAX_TRIGGERS)
    return -1;

  if (dbus >= EVG_DBUS_BITS)
    return -1;

  if (seqtrig >= EVG_MAX_SEQRAMS)
    return -1;

  if (trig >= 0)
    map |= (1 << (C_EVG_INMAP_TRIG_BASE + trig));

  if (dbus >= 0)
    map |= (1 << (C_EVG_INMAP_DBUS_BASE + dbus));

  if (irq)
    map |= (1 << (C_EVG_INMAP_IRQ));

  if (seqtrig >= 0)
    map |= (1 << (C_EVG_INMAP_SEQTRIG_BASE + seqtrig));

  pEg->FPInMap[univ] = be32_to_cpu(map);

  return 0;
}

/**
Get Front panel Input to Event Trigger Mappings.

@param pEg Pointer to MrfEgRegs structure
@param univ Number of Front panel input
@return Number of Event trigger to trigger, -1 for no trigger
*/
int EvgGetFPinMapTrigger(volatile struct MrfEgRegs *pEg, int fp)
{
  int mask, i;

  if (fp < 0 || fp >= EVG_MAX_FPIN_MAP)
    return -1;

  mask = (be32_to_cpu(pEg->FPInMap[fp]) >> C_EVG_INMAP_TRIG_BASE) &
    ((1 << EVG_MAX_TRIGGERS) - 1);
  if (!mask)
    return -1;
  for (i = 0; !(mask & 1); i++)
    mask >>= 1;

  return i;
}

/**
Get Front panel Input to Distributed Bus Bit Mappings.

@param pEg Pointer to MrfEgRegs structure
@param univ Number of Front panel input
@return Number of Distributed bus bit input is mapped to, -1 for no trigger
*/
int EvgGetFPinMapDBus(volatile struct MrfEgRegs *pEg, int fp)
{
  int mask, i;

  if (fp < 0 || fp >= EVG_MAX_FPIN_MAP)
    return -1;

  mask = (be32_to_cpu(pEg->FPInMap[fp]) >> C_EVG_INMAP_DBUS_BASE) &
    ((1 << EVG_DBUS_BITS) - 1);
  if (!mask)
    return -1;
  for (i = 0; !(mask & 1); i++)
    mask >>= 1;

  return i;
}

/**
Get Front panel Input External Interrupt Mapping.

@param pEg Pointer to MrfEgRegs structure
@param univ Number of Front panel input
@return 0 = Interrupt not mapped, 1 = interrupt mapped.
*/
int EvgGetFPinMapIrq(volatile struct MrfEgRegs *pEg, int fp)
{
  int mask;

  if (fp < 0 || fp >= EVG_MAX_FPIN_MAP)
    return -1;

  mask = (be32_to_cpu(pEg->FPInMap[fp]) >> C_EVG_INMAP_IRQ) & 1;

  return mask;
}

/**
Get Front panel Input Sequence RAM Trigger Mapping.

@param pEg Pointer to MrfEgRegs structure
@param univ Number of Front panel input
@return Number of sequencer trigger, -1 no trigger.
*/
int EvgGetFPinMapSeqtrig(volatile struct MrfEgRegs *pEg, int fp)
{
  int mask, i;

  if (fp < 0 || fp >= EVG_MAX_FPIN_MAP)
    return -1;

  mask = (be32_to_cpu(pEg->FPInMap[fp]) >> C_EVG_INMAP_SEQTRIG_BASE) &
    ((1 << EVG_MAX_SEQRAMS) - 1);
  if (!mask)
    return -1;
  for (i = 0; !(mask & 1); i++)
    mask >>= 1;

  return i;
}

/**
Show Front panel Input Mappings.

@param pEg Pointer to MrfEgRegs structure
*/
void EvgFPinDump(volatile struct MrfEgRegs *pEg)
{
  int fp;

  for (fp = 0; fp < EVG_MAX_FPIN_MAP; fp++)
    {
      int map = be32_to_cpu(pEg->FPInMap[fp]); 
      DEBUG_PRINTF("FPIn%d Mapped to Trig %08x, DBus %02x, IRQ %d, seqtrig %d\n", fp,
		   (map >> C_EVG_INMAP_TRIG_BASE)
		   & ((1 << EVG_MAX_TRIGGERS) - 1),
		   (map >> C_EVG_INMAP_DBUS_BASE)
		   & ((1 << EVG_DBUS_BITS) - 1),
		   (map >> C_EVG_INMAP_IRQ) & 1,
		   (map >> C_EVG_INMAP_SEQTRIG_BASE)
		   & ((1 << EVG_MAX_SEQRAMS) - 1));
    }
}

/**
Set up Transition board Input Mappings.

@param pEg Pointer to MrfEgRegs structure
@param univ Number of Transition board input
@param trig Number of Event trigger to trigger, -1 for no trigger
@param dbus Number of Distributed bus bit to map input to, -1 for no mapping
@param irq External interrupt mapping, 0 = no interrupt, 1 = mapped to interrupt
@param seqtrig Number of sequence RAM trigger, -1 for no trigger 
*/
int EvgSetTBinMap(volatile struct MrfEgRegs *pEg, int tb, int trig, int dbus, int irq, int seqtrig)
{
  int map = 0;

  if (tb < 0 || tb >= EVG_MAX_TBIN_MAP)
    return -1;

  if (trig >= EVG_MAX_TRIGGERS)
    return -1;

  if (dbus >= EVG_DBUS_BITS)
    return -1;

  if (seqtrig >= EVG_MAX_SEQRAMS)
    return -1;

  if (trig >= 0)
    map |= (1 << (C_EVG_INMAP_TRIG_BASE + trig));

  if (dbus >= 0)
    map |= (1 << (C_EVG_INMAP_DBUS_BASE + dbus));

  if (irq)
    map |= (1 << (C_EVG_INMAP_IRQ));

  if (seqtrig >= 0)
    map |= (1 << (C_EVG_INMAP_SEQTRIG_BASE + seqtrig));

  pEg->TBInMap[tb] = be32_to_cpu(map);

  return 0;
}

/**
Get Transition board Input to Event Trigger Mappings.

@param pEg Pointer to MrfEgRegs structure
@param univ Number of Transition board input
@return Number of Event trigger to trigger, -1 for no trigger
*/
int EvgGetTBinMapTrigger(volatile struct MrfEgRegs *pEg, int tb)
{
  int mask, i;

  if (tb < 0 || tb >= EVG_MAX_TBIN_MAP)
    return -1;

  mask = (be32_to_cpu(pEg->TBInMap[tb]) >> C_EVG_INMAP_TRIG_BASE) &
    ((1 << EVG_MAX_TRIGGERS) - 1);
  if (!mask)
    return -1;
  for (i = 0; !(mask & 1); i++)
    mask >>= 1;

  return i;
}

/**
Get Transition board Input to Distributed Bus Bit Mappings.

@param pEg Pointer to MrfEgRegs structure
@param univ Number of Transition board input
@return Number of Distributed bus bit input is mapped to, -1 for no trigger
*/
int EvgGetTBinMapDBus(volatile struct MrfEgRegs *pEg, int tb)
{
  int mask, i;

  if (tb < 0 || tb >= EVG_MAX_TBIN_MAP)
    return -1;

  mask = (be32_to_cpu(pEg->TBInMap[tb]) >> C_EVG_INMAP_DBUS_BASE) &
    ((1 << EVG_DBUS_BITS) - 1);
  if (!mask)
    return -1;
  for (i = 0; !(mask & 1); i++)
    mask >>= 1;

  return i;
}

/**
Get Transition board Input External Interrupt Mapping.

@param pEg Pointer to MrfEgRegs structure
@param univ Number of Transition board input
@return 0 = Interrupt not mapped, 1 = interrupt mapped.
*/
int EvgGetTBinMapIrq(volatile struct MrfEgRegs *pEg, int tb)
{
  int mask;

  if (tb < 0 || tb >= EVG_MAX_TBIN_MAP)
    return -1;

  mask = (be32_to_cpu(pEg->TBInMap[tb]) >> C_EVG_INMAP_IRQ) & 1;

  return mask;
}

/**
Get Transition board Input Sequence RAM Trigger Mapping.

@param pEg Pointer to MrfEgRegs structure
@param univ Number of Transition board input
@return Number of sequencer trigger, -1 no trigger.
*/
int EvgGetTBinMapSeqtrig(volatile struct MrfEgRegs *pEg, int tb)
{
  int mask, i;

  if (tb < 0 || tb >= EVG_MAX_TBIN_MAP)
    return -1;

  mask = (be32_to_cpu(pEg->TBInMap[tb]) >> C_EVG_INMAP_SEQTRIG_BASE) &
    ((1 << EVG_MAX_SEQRAMS) - 1);
  if (!mask)
    return -1;
  for (i = 0; !(mask & 1); i++)
    mask >>= 1;

  return i;
}

/**
Show Transition board Input Mappings.

@param pEg Pointer to MrfEgRegs structure
*/
void EvgTBinDump(volatile struct MrfEgRegs *pEg)
{
  int tb;

  for (tb = 0; tb < EVG_MAX_TBIN_MAP; tb++)
    {
      int map = be32_to_cpu(pEg->TBInMap[tb]); 
      DEBUG_PRINTF("TBIn%d Mapped to Trig %08x, DBus %02x, IRQ %d, seqtrig %d\n", tb,
		   (map >> C_EVG_INMAP_TRIG_BASE)
		   & ((1 << EVG_MAX_TRIGGERS) - 1),
		   (map >> C_EVG_INMAP_DBUS_BASE)
		   & ((1 << EVG_DBUS_BITS) - 1),
		   (map >> C_EVG_INMAP_IRQ) & 1,
		   (map >> C_EVG_INMAP_SEQTRIG_BASE)
		   & ((1 << EVG_MAX_SEQRAMS) - 1));
    }
}

/**
Set up Event Trigger.

@param pEg Pointer to MrfEgRegs structure
@param trigger Number of Event Trigger
@param code Event Code
@param enable 0 = Event trigger disabled, 1 = event trigger enabled
*/
int EvgSetTriggerEvent(volatile struct MrfEgRegs *pEg, int trigger, int code, int enable)
{
  int result;

  if (trigger < 0 || trigger >= EVG_TRIGGERS)
    return 0;

  result = be32_to_cpu(pEg->EventTrigger[trigger]);
					     
  if (code >= 0 && code <= EVG_MAX_EVENT_CODE)
    {
      result &= ~(EVG_MAX_EVENT_CODE);
      result |= (code << C_EVG_EVENTTRIG_CODE_LOW);
    }

  if (enable == 0)
    result &= ~(1 << C_EVG_EVENTTRIG_ENABLE);
  if (enable == 1)
    result |= (1 << C_EVG_EVENTTRIG_ENABLE);

  pEg->EventTrigger[trigger] = be32_to_cpu(result);

  return 0;
}

/**
Get Event Trigger event code.

@param pEg Pointer to MrfEgRegs structure
@param trigger Number of Event Trigger
@return Event Code
*/
int EvgGetTriggerEventCode(volatile struct MrfEgRegs *pEg, int trigger)
{
  return (be32_to_cpu(pEg->EventTrigger[trigger])
	  >> C_EVG_EVENTTRIG_CODE_LOW) & EVG_MAX_EVENT_CODE;
}

/**
Get Event Trigger status.

@param pEg Pointer to MrfEgRegs structure
@param trigger Number of Event Trigger
@return 0 = Event trigger disabled, 1 = event trigger enabled.
*/
int EvgGetTriggerEventEnable(volatile struct MrfEgRegs *pEg, int trigger)
{
  return (be32_to_cpu(pEg->EventTrigger[trigger]) &
	  (1 << C_EVG_EVENTTRIG_ENABLE) ? 1 : 0);
}

/**
Show Event Triggers.

@param pEg Pointer to MrfEgRegs structure
*/
void EvgTriggerEventDump(volatile struct MrfEgRegs *pEg)
{
  int trigger, result;

  for (trigger = 0; trigger < EVG_TRIGGERS; trigger++)
    {
      result = be32_to_cpu(pEg->EventTrigger[trigger]);
      printf("Trigger%d code %02x %s\n",
	     trigger, (result & ((1 << (C_EVG_EVENTTRIG_CODE_HIGH + 1)) -
				 (1 << C_EVG_EVENTTRIG_CODE_LOW))) >>
	     C_EVG_EVENTTRIG_CODE_LOW, result & (1 << C_EVG_EVENTTRIG_ENABLE) ?
	     "ENABLED" : "DISABLED");
    }
}

/**
Set up output mapping for Universal Output.

<table>
<caption id="evg_output_mapping">Output mapping</caption>
<tr><th>ID (dec)<th>ID (hex)<th>Signal
<tr><td>0<td>0x00<td>Reserved
<tr><td>...<td>...<td>...
<tr><td>31<td>0x1F<td>Reserved
<tr><td>32<td>0x20<td>Distributed bus bit 0
<tr><td>33<td>0x21<td>Distributed bus bit 1
<tr><td>...<td>...<td>...
<tr><td>39<td>0x27<td>Distributed bus bit 7
<tr><td>40<td>0x28<td>Multiplexed Counter 0
<tr><td>41<td>0x29<td>Multiplexed Counter 1
<tr><td>...<td>...<td>...
<tr><td>47<td>0x2F<td>Multiplexed Counter 7
<tr><td>48<td>0x30<td>AC trigger logic output
<tr><td>49<td>0x31<td>Reserved
<tr><td>...<td>...<td>...
<tr><td>60<td>0x3D<td>Reserved
<tr><td>61<td>0x3D<td>Three-state output (for bi-dir ports)
<tr><td>62<td>62<td>High 1
<tr><td>63<td>63<td>Low 0
</table>
@param pEg Pointer to MrfEgRegs structure
@param output Number of Universal Output
@param map Output map
*/
int EvgSetUnivOutMap(volatile struct MrfEgRegs *pEg, int output, int map)
{
  pEg->UnivOutMap[output] = be16_to_cpu(map);

  return be16_to_cpu(pEg->UnivOutMap[output]);
}

/**
Retrieve output mapping for Universal Output.

Please see \ref evg_output_mapping for details about the mapping.

@param pEg Pointer to MrfEgRegs structure
@param output Number of Universal Output
@return Output map for output
*/
int EvgGetUnivOutMap(volatile struct MrfEgRegs *pEg, int output)
{
  return be16_to_cpu(pEg->UnivOutMap[output]);
}

/**
Set up output mapping for Front panel Output.

Please see \ref evg_output_mapping for details about the mapping.

@param pEg Pointer to MrfEgRegs structure
@param output Number of Front panel Output
@param map Output map
*/
int EvgSetFPOutMap(volatile struct MrfEgRegs *pEg, int output, int map)
{
  pEg->FPOutMap[output] = be16_to_cpu(map);

  return be16_to_cpu(pEg->FPOutMap[output]);
}

/**
Retrieve output mapping for Front panel Output.

Please see \ref evg_output_mapping for details about the mapping.

@param pEg Pointer to MrfEgRegs structure
@param output Number of Front panel Output
@return Output map for output
*/
int EvgGetFPOutMap(volatile struct MrfEgRegs *pEg, int output)
{
  return be16_to_cpu(pEg->FPOutMap[output]);
}

/**
Set up output mapping for Transition board Output.

Please see \ref evg_output_mapping for details about the mapping.

@param pEg Pointer to MrfEgRegs structure
@param output Number of Transition board Output
@param map Output map
*/
int EvgSetTBOutMap(volatile struct MrfEgRegs *pEg, int output, int map)
{
  pEg->TBOutMap[output] = be16_to_cpu(map);

  return be16_to_cpu(pEg->TBOutMap[output]);
}

/**
Retrieve output mapping for Transition board Output.

Please see \ref evg_output_mapping for details about the mapping.

@param pEg Pointer to MrfEgRegs structure
@param output Number of Transition board Output
@return Output map for output
*/
int EvgGetTBOutMap(volatile struct MrfEgRegs *pEg, int output)
{
  return be16_to_cpu(pEg->TBOutMap[output]);
}

/**
Set Data Buffer mode.

For EVM's the data buffer mode is always enabled and cannot be disabled.

@param pEg Pointer to MrfEgRegs structure
@param enable 0 = disable, 1 = enable
*/
int EvgSetDBufMode(volatile struct MrfEgRegs *pEg, int enable)
{
  if (enable)
    pEg->DataBufControl = be32_to_cpu(1 << C_EVG_DATABUF_MODE);
  else
    pEg->DataBufControl = 0;

  return EvgGetDBufStatus(pEg);
}

/**
Get Data Buffer mode.

For EVM's the data buffer mode is always enabled and cannot be disabled.

@param pEg Pointer to MrfEgRegs structure
@return 0 = data buffer mode disabled, 1 = data buffer mode enabled
*/
int EvgGetDBufStatus(volatile struct MrfEgRegs *pEg)
{
  return be32_to_cpu(pEg->DataBufControl);
}

/**
Get segmented data buffer transmitter control register.

@param pEg Pointer to MrfEgRegs structure
@return Segmented data buffer transmitter control register.
*/
int EvgGetSegBufStatus(volatile struct MrfEgRegs *pEg)
{
  return be32_to_cpu(pEg->SegBufControl);
}

/**
Send data buffer.

@param pEg Pointer to MrfEgRegs structure
@param dbuf Pointer to data buffer to send
@param size Number of bytes to send (4 to 2048)
@return Returns -1 on error, number of bytes sent on success.

The function does not wait for the transmission to be completed. If the
previous transfer is still in progress the function returns -1.
*/
int EvgSendDBuf(volatile struct MrfEgRegs *pEg, char *dbuf, int size)
{
  int stat;

  stat = EvgGetDBufStatus(pEg);
  /* Check that DBUF mode enabled */
  if (!(stat & (1 << C_EVG_DATABUF_MODE)))
    return -1;
  /* Check that previous transfer is completed */
  if (!(stat & (1 << C_EVG_DATABUF_COMPLETE)))
    return -1;
  /* Check that size is valid */
  if (size & 3 || size > EVG_MAX_BUFFER || size < 4)
    return -1;

#ifdef __linux__
  memcpy((void *) &pEg->Databuf[0], (void *) dbuf, size);
#else
  memcpy((void *) &pEg->Databuf[0], (void *) dbuf, size);
  /* {
   int i;
   int *p = (int *) dbuf;

   for (i = 0; i < size/4; i++)
     pEg->Databuf[i] = be32_to_cpu(p[i]);
     } */
#endif

  /* Enable and set size */
  stat &= ~((EVG_MAX_BUFFER-1) | (1 << C_EVG_DATABUF_TRIGGER));
  stat |= (1 << C_EVG_DATABUF_ENA) | size;
  pEg->DataBufControl = be32_to_cpu(stat);

  /* Trigger */
  pEg->DataBufControl = be32_to_cpu(stat | (1 << C_EVG_DATABUF_TRIGGER));

  return size;
}

/**
Send segmented data buffer.

@param pEg Pointer to MrfEgRegs structure
@param dbuf Pointer to data buffer to send
@param segment Starting segment number
@param size Number of bytes to send (4 to max. 2048 depending on segment number)
@return Returns -1 on error, number of bytes sent on success.

The function does not wait for the transmission to be completed. If the
previous transfer is still in progress the function returns -1.
*/
int EvgSendSegBuf(volatile struct MrfEgRegs *pEg, char *dbuf, int segment, int size)
{
  int stat, dummy;

  stat = EvgGetSegBufStatus(pEg);
  /* Check that DBUF mode enabled
  if (!(stat & (1 << C_EVG_DATABUF_MODE)))
    return -1; */
  /* Check that previous transfer is completed */
  if (!(stat & (1 << C_EVG_DATABUF_COMPLETE)))
    return -1;
  /* Check that segment is valid */
  if (segment < EVG_MIN_BUF_SEGMENT || segment > EVG_MAX_BUF_SEGMENT)
    return -1;
  /* Check that size is valid */
  if (size & 3 || size > EVG_MAX_BUFFER || size < 4)
    return -1;

#ifdef LINUX
  memcpy((void *) &pEg->Segbuf[segment*4], (void *) dbuf, size);
#else
  memcpy((void *) &pEg->Segbuf[segment*4], (void *) dbuf, size);
  /* {
   int i;
   int *p = (int *) dbuf;

   for (i = 0; i < size/4; i++)
     pEg->Databuf[i] = be32_to_cpu(p[i]);
     } */
#endif

  /* Enable and set size */
  stat &= ~((EVG_MAX_BUF_SEGMENT << C_EVG_DATABUF_SEGSHIFT) | (EVG_MAX_BUFFER-1) | (1 << C_EVG_DATABUF_TRIGGER));
  stat |= (1 << C_EVG_DATABUF_ENA) | size;
  stat |= (segment << C_EVG_DATABUF_SEGSHIFT);
  /*
  printf("SegDatabuf control %08x\r\n", stat);
  */
  pEg->SegBufControl = be32_to_cpu(stat);
  dummy = EvgGetSegBufStatus(pEg);

  /* Trigger */
  pEg->SegBufControl = be32_to_cpu(stat | (1 << C_EVG_DATABUF_TRIGGER));
  dummy = EvgGetSegBufStatus(pEg);

  return size;
}

/**
Retrieve form factor of EVG/EVM device.

@param pEg Pointer to MrfEgRegs structure
@return Form Factor
<table>
<caption id="form_factor">Form factor</caption>
<tr><th>ID<th>Form Factor
<tr><td>0<td>CompactPCI 3U
<tr><td>2<td>VME64x
<tr><td>4<td>CompactPCI 6U
<tr><td>6<td>PXIe 3U
<tr><td>8<td>mTCA.4
</table>
*/
int EvgGetFormFactor(volatile struct MrfEgRegs *pEg)
{
  int stat;
  
  stat = be32_to_cpu(pEg->FPGAVersion);
  return ((stat >> 24) & 0x0f);
}

#ifdef __linux__
/**
Assign user space interrupt handler for EVG interrupts.

@param pEg Pointer to MrfEgRegs structure
@param fd File descriptor of EVR device
@param handler Pointer to user space interrupt handler
*/
void EvgIrqAssignHandler(volatile struct MrfEgRegs *pEg, int fd,
			 void (*handler)(int))
{
  struct sigaction act;
  int oflags;
  int result;

  act.sa_handler = handler;
  sigemptyset(&act.sa_mask);
  act.sa_flags = 0;

  result = sigaction(SIGIO, &act, NULL);
  /*
  printf("sigaction returned %d\n", result);
  */
  fcntl(fd, F_SETOWN, getpid());
  oflags = fcntl(fd, F_GETFL);
  fcntl(fd, F_SETFL, oflags | FASYNC);
  /* Now enable handler */
  EvgIrqHandled(fd);
}

/**
@private
*/
void EvgIrqUnassignHandler(int vector,
			 void (*handler)(int))
{
}
#endif

#ifdef VXWORKS
void EvgIrqAssignHandler(volatile struct MrfEgRegs *pEg, int vector,
			 void (*handler)(int))
{
  return intConnect(INUM_TO_IVEC(vector), handler, pEg);
}

void EvgIrqUnassignHandler(int vector,
			 void (*handler)(int))
{
  ppcDisconnectVec(vector, handler);
}
#endif

/**
Enable interrupts for EVG.

@param pEg Pointer to MrfEgRegs structure
@param mask Interrupt mask

<table>
<caption id="evginterrupts">Interrupt mask</caption>
<tr><th>Bit<th>Function
<tr><td>0<td>Receiver violation interrupt
<tr><td>1<td>Event FIFO full interrupt
<tr><td>5<td>Data buffer interrupt
<tr><td>6<td>External interrupt
<tr><td>8<td>Sequence RAM 0 start interrupt
<tr><td>9<td>Sequence RAM 1 start interrupt
<tr><td>12<td>Sequence RAM 0 stop interrupt
<tr><td>13<td>Sequence RAM 1 stop interrupt
<tr><td>16<td>Sequence RAM 0 halfway through interrupt
<tr><td>17<td>Sequence RAM 1 halfway through interrupt
<tr><td>20<td>Sequence RAM 0 roll over interrupt
<tr><td>21<td>Sequence RAM 1 roll over interrupt
</table>
*/
int EvgIrqEnable(volatile struct MrfEgRegs *pEg, int mask)
{
  int control = be32_to_cpu(pEg->IrqEnable) & EVG_IRQ_PCICORE_ENABLE;

  pEg->IrqEnable = be32_to_cpu(mask | control);
  return be32_to_cpu(pEg->IrqEnable);
}

/**
Get interrupt flags for EVR.

@param pEg Pointer to MrfEgRegs structure
@return Interrupt flags

Please see \ref evginterrupts for flag bit definitions.
*/
int EvgGetIrqFlags(volatile struct MrfEgRegs *pEg)
{
  return be32_to_cpu(pEg->IrqFlag);
}

/**
Clear EVG interrupt flags.

@param pEg Pointer to MrfEgRegs structure
@param mask Bit mask to clear interrupts
@return Interrupt flags

Please see \ref evginterrupts for interrupt bit definitions.
*/
int EvgClearIrqFlags(volatile struct MrfEgRegs *pEg, int mask)
{
  pEg->IrqFlag = be32_to_cpu(mask);
  return be32_to_cpu(pEg->IrqFlag);
}

#ifdef __linux__
/**
Function to call when interrupt handler exits.

@param fd File descriptor of EVG device opened.
*/
void EvgIrqHandled(int fd)
{
  ioctl(fd, EV_IOCIRQEN);
}
#endif

/**
Enable/disable Timstamp generator.

@param pEg Pointer to MrfEgRegs structure
@param enable 0 = disable, 1 = enable
*/
int EvgTimestampEnable(volatile struct MrfEgRegs *pEg, int enable)
{
  if (enable)
    pEg->TimestampCtrl |= be32_to_cpu(1 << C_EVG_TSCTRL_ENABLE);
  else
    pEg->TimestampCtrl &= be32_to_cpu(~(1 << C_EVG_TSCTRL_ENABLE));
    
  return EvgGetTimestampEnable(pEg);
}

/**
Get Timstamp generator state.

@param pEg Pointer to MrfEgRegs structure
@return 0 = Timestamp generator disabled, 1 = timestamp generator enabled
*/
int EvgGetTimestampEnable(volatile struct MrfEgRegs *pEg)
{
  return be32_to_cpu(pEg->TimestampCtrl & be32_to_cpu(1 << C_EVG_TSCTRL_ENABLE));
}

/**
Load new seconds value into timestamp generator.

@param pEg Pointer to MrfEgRegs structure
@param timestamp New seconds value
*/
int EvgTimestampLoad(volatile struct MrfEgRegs *pEg, int timestamp)
{
  pEg->TimestampValue = be32_to_cpu(timestamp);
  pEg->TimestampCtrl |= be32_to_cpu(1 << C_EVG_TSCTRL_LOAD);
}

/**
Get seconds value from timestamp generator.

@param pEg Pointer to MrfEgRegs structure
@return Timestamp generator seconds value.
*/
int EvgTimestampGet(volatile struct MrfEgRegs *pEg)
{
  return be32_to_cpu(pEg->TimestampValue);
}
