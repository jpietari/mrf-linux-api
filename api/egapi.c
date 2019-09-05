/*
  egapi.c -- Functions for Micro-Research Event Generator
             Application Programming Interface

  Author: Jukka Pietarinen (MRF)
  Date:   05.12.2006

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

u32 EvgFWVersion(volatile struct MrfEgRegs *pEg)
{
  return be32_to_cpu(pEg->FPGAVersion);
}

int EvgEnable(volatile struct MrfEgRegs *pEg, int state)
{
  if (state)
    pEg->Control |= be32_to_cpu(1 << C_EVG_CTRL_MASTER_ENABLE);
  else
    pEg->Control &= be32_to_cpu(~(1 << C_EVG_CTRL_MASTER_ENABLE));
  
  return EvgGetEnable(pEg);
}

int EvgGetEnable(volatile struct MrfEgRegs *pEg)
{
  return be32_to_cpu(pEg->Control & be32_to_cpu(1 << C_EVG_CTRL_MASTER_ENABLE));
}

int EvgSystemMasterEnable(volatile struct MrfEgRegs *pEg, int state)
{
  if (state)
    pEg->Control |= be32_to_cpu(1 << C_EVG_CTRL_DCMASTER_ENABLE);
  else
    pEg->Control &= be32_to_cpu(~(1 << C_EVG_CTRL_DCMASTER_ENABLE));
  
  return EvgGetSystemMasterEnable(pEg);
}

int EvgGetSystemMasterEnable(volatile struct MrfEgRegs *pEg)
{
  return be32_to_cpu(pEg->Control & be32_to_cpu(1 << C_EVG_CTRL_DCMASTER_ENABLE));
}

int EvgBeaconEnable(volatile struct MrfEgRegs *pEg, int state)
{
  if (state)
    pEg->Control |= be32_to_cpu(1 << C_EVG_CTRL_BEACON_ENABLE);
  else
    pEg->Control &= be32_to_cpu(~(1 << C_EVG_CTRL_BEACON_ENABLE));
  
  return EvgGetBeaconEnable(pEg);
}

int EvgGetBeaconEnable(volatile struct MrfEgRegs *pEg)
{
  return be32_to_cpu(pEg->Control & be32_to_cpu(1 << C_EVG_CTRL_BEACON_ENABLE));
}

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

int EvgRxGetEnable(volatile struct MrfEgRegs *pEg)
{
  return be32_to_cpu(~(pEg->Control) &
		     be32_to_cpu((1 << C_EVG_CTRL_RX_DISABLE) |
				 (1 << C_EVG_CTRL_RX_PWRDOWN)));
}

int EvgGetViolation(volatile struct MrfEgRegs *pEg, int clear)
{
  int result;

  result = be32_to_cpu(pEg->IrqFlag & be32_to_cpu(1 << C_EVG_IRQFLAG_VIOLATION));
  if (clear && result)
    pEg->IrqFlag = be32_to_cpu(result);

  return result;
}

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

int EvgGetSWEventEnable(volatile struct MrfEgRegs *pEg)
{
  return be32_to_cpu(pEg->SWEvent & be32_to_cpu(1 << C_EVG_SWEVENT_ENABLE));
}

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

int EvgEvanEnable(volatile struct MrfEgRegs *pEg, int state)
{
  if (state)
    pEg->EvanControl |= be32_to_cpu(1 << C_EVG_EVANCTRL_ENABLE);
  else
    pEg->EvanControl &= be32_to_cpu(~(1 << C_EVG_EVANCTRL_ENABLE));
  
  return EvgEvanGetEnable(pEg);
}

int EvgEvanGetEnable(volatile struct MrfEgRegs *pEg)
{
  return be32_to_cpu(pEg->EvanControl & be32_to_cpu(1 << C_EVG_EVANCTRL_ENABLE)); 
}

void EvgEvanReset(volatile struct MrfEgRegs *pEg)
{
  struct EvanStruct evan;

  pEg->EvanControl |= be32_to_cpu(1 << C_EVG_EVANCTRL_RESET);
  /* Dummy read to clear FIFO */
  EvgEvanGetEvent(pEg, &evan);
}

void EvgEvanResetCount(volatile struct MrfEgRegs *pEg)
{
  pEg->EvanControl |= be32_to_cpu(1 << C_EVG_EVANCTRL_COUNTRES);
}

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

int EvgSetMXCPrescaler(volatile struct MrfEgRegs *pEg, int mxc, unsigned int presc)
{
  if (mxc < 0 || mxc >= EVG_MAX_MXCS)
    return -1;

  pEg->MXC[mxc].Prescaler = be32_to_cpu(presc);

  return 0;
}

unsigned int EvgGetMXCPrescaler(volatile struct MrfEgRegs *pEg, int mxc)
{
  if (mxc < 0 || mxc >= EVG_MAX_MXCS)
    return -1;

  return (unsigned int) be32_to_cpu(pEg->MXC[mxc].Prescaler);
}

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

void EvgSyncMxc(volatile struct MrfEgRegs *pEg)
{
  pEg->Control |= be32_to_cpu(1 << C_EVG_CTRL_MXC_RESET);
}

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

int EvgSetDBusEvent(volatile struct MrfEgRegs *pEg, int enable)
{
  pEg->DBusEvent = be32_to_cpu(enable);
}

int EvgGetDBusEvent(volatile struct MrfEgRegs *pEg)
{
  return be32_to_cpu(pEg->DBusEvent);
}

int EvgSetACInput(volatile struct MrfEgRegs *pEg, int bypass, int sync, int div, int delay)
{
  unsigned int result;

  result = be32_to_cpu(pEg->ACControl);

  if (bypass == 0)
    result &= ~(1 << C_EVG_ACCTRL_BYPASS);
  if (bypass == 1)
    result |= (1 << C_EVG_ACCTRL_BYPASS);

  if (sync == 0)
    result &= ~(1 << C_EVG_ACCTRL_ACSYNC);
  if (sync == 1)
    result |= (1 << C_EVG_ACCTRL_ACSYNC);

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

int EvgSetFracDiv(volatile struct MrfEgRegs *pEg, int fracdiv)
{
  pEg->UsecDiv = be32_to_cpu((int) cw_to_freq(fracdiv));

  return be32_to_cpu(pEg->FracDiv = be32_to_cpu(fracdiv));
}

int EvgGetFracDiv(volatile struct MrfEgRegs *pEg)
{
  return be32_to_cpu(pEg->FracDiv);
}

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

unsigned int EvgGetSeqRamTimestamp(volatile struct MrfEgRegs *pEg, int ram, int pos)
{
  if (ram < 0 || ram >= EVG_SEQRAMS)
    return -1;

  if (pos < 0 || pos >= EVG_MAX_SEQRAMEV)
    return -1;

  return be32_to_cpu(pEg->SeqRam[ram][pos].Timestamp);
}

int EvgGetSeqRamEvent(volatile struct MrfEgRegs *pEg, int ram, int pos)
{
  if (ram < 0 || ram >= EVG_SEQRAMS)
    return -1;

  if (pos < 0 || pos >= EVG_MAX_SEQRAMEV)
    return -1;

  return be32_to_cpu(pEg->SeqRam[ram][pos].EventCode);
}

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
					  
int EvgSeqRamSWTrig(volatile struct MrfEgRegs *pEg, int trig)
{
  if (trig < 0 || trig > 1)
    return -1;

  pEg->SeqRamControl[trig] |= be32_to_cpu(1 << C_EVG_SQRC_SWTRIGGER);
  
  return 0;
}

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

int EvgGetUnivinMapIrq(volatile struct MrfEgRegs *pEg, int univ)
{
  int mask;

  if (univ < 0 || univ >= EVG_MAX_UNIVIN_MAP)
    return -1;

  mask = (be32_to_cpu(pEg->UnivInMap[univ]) >> C_EVG_INMAP_IRQ) & 1;

  return mask;
}

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

int EvgGetFPinMapIrq(volatile struct MrfEgRegs *pEg, int fp)
{
  int mask;

  if (fp < 0 || fp >= EVG_MAX_FPIN_MAP)
    return -1;

  mask = (be32_to_cpu(pEg->FPInMap[fp]) >> C_EVG_INMAP_IRQ) & 1;

  return mask;
}

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

int EvgGetTBinMapIrq(volatile struct MrfEgRegs *pEg, int tb)
{
  int mask;

  if (tb < 0 || tb >= EVG_MAX_TBIN_MAP)
    return -1;

  mask = (be32_to_cpu(pEg->TBInMap[tb]) >> C_EVG_INMAP_IRQ) & 1;

  return mask;
}

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

int EvgGetTriggerEventCode(volatile struct MrfEgRegs *pEg, int trigger)
{
  return (be32_to_cpu(pEg->EventTrigger[trigger])
	  >> C_EVG_EVENTTRIG_CODE_LOW) & EVG_MAX_EVENT_CODE;
}

int EvgGetTriggerEventEnable(volatile struct MrfEgRegs *pEg, int trigger)
{
  return (be32_to_cpu(pEg->EventTrigger[trigger]) &
	  (1 << C_EVG_EVENTTRIG_ENABLE) ? 1 : 0);
}

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

int EvgSetUnivOutMap(volatile struct MrfEgRegs *pEg, int output, int map)
{
  pEg->UnivOutMap[output] = be16_to_cpu(map);

  return be16_to_cpu(pEg->UnivOutMap[output]);
}

int EvgGetUnivOutMap(volatile struct MrfEgRegs *pEg, int output)
{
  return be16_to_cpu(pEg->UnivOutMap[output]);
}

int EvgSetFPOutMap(volatile struct MrfEgRegs *pEg, int output, int map)
{
  pEg->FPOutMap[output] = be16_to_cpu(map);

  return be16_to_cpu(pEg->FPOutMap[output]);
}

int EvgGetFPOutMap(volatile struct MrfEgRegs *pEg, int output)
{
  return be16_to_cpu(pEg->FPOutMap[output]);
}

int EvgSetTBOutMap(volatile struct MrfEgRegs *pEg, int output, int map)
{
  pEg->TBOutMap[output] = be16_to_cpu(map);

  return be16_to_cpu(pEg->TBOutMap[output]);
}

int EvgGetTBOutMap(volatile struct MrfEgRegs *pEg, int output)
{
  return be16_to_cpu(pEg->TBOutMap[output]);
}

int EvgSetDBufMode(volatile struct MrfEgRegs *pEg, int enable)
{
  if (enable)
    pEg->DataBufControl = be32_to_cpu(1 << C_EVG_DATABUF_MODE);
  else
    pEg->DataBufControl = 0;

  return EvgGetDBufStatus(pEg);
}

int EvgGetDBufStatus(volatile struct MrfEgRegs *pEg)
{
  return be32_to_cpu(pEg->DataBufControl);
}

int EvgGetSegBufStatus(volatile struct MrfEgRegs *pEg)
{
  return be32_to_cpu(pEg->SegBufControl);
}

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

int EvgGetFormFactor(volatile struct MrfEgRegs *pEg)
{
  int stat;
  
  stat = be32_to_cpu(pEg->FPGAVersion);
  return ((stat >> 24) & 0x0f);
}

#ifdef __linux__
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

int EvgIrqEnable(volatile struct MrfEgRegs *pEg, int mask)
{
  int control = be32_to_cpu(pEg->IrqEnable) & EVG_IRQ_PCICORE_ENABLE;

  pEg->IrqEnable = be32_to_cpu(mask | control);
  return be32_to_cpu(pEg->IrqEnable);
}

int EvgGetIrqFlags(volatile struct MrfEgRegs *pEg)
{
  return be32_to_cpu(pEg->IrqFlag);
}

int EvgClearIrqFlags(volatile struct MrfEgRegs *pEg, int mask)
{
  pEg->IrqFlag = be32_to_cpu(mask);
  return be32_to_cpu(pEg->IrqFlag);
}

#ifdef __linux__
void EvgIrqHandled(int fd)
{
  ioctl(fd, EV_IOCIRQEN);
}
#endif

int EvgTimestampEnable(volatile struct MrfEgRegs *pEg, int enable)
{
  if (enable)
    pEg->TimestampCtrl |= be32_to_cpu(1 << C_EVG_TSCTRL_ENABLE);
  else
    pEg->TimestampCtrl &= be32_to_cpu(~(1 << C_EVG_TSCTRL_ENABLE));
    
  return EvgGetTimestampEnable(pEg);
}

int EvgGetTimestampEnable(volatile struct MrfEgRegs *pEg)
{
  return be32_to_cpu(pEg->TimestampCtrl & be32_to_cpu(1 << C_EVG_TSCTRL_ENABLE));
}

int EvgTimestampLoad(volatile struct MrfEgRegs *pEg, int timestamp)
{
  pEg->TimestampValue = be32_to_cpu(timestamp);
  pEg->TimestampCtrl |= be32_to_cpu(1 << C_EVG_TSCTRL_LOAD);
}

int EvgTimestampGet(volatile struct MrfEgRegs *pEg)
{
  return be32_to_cpu(pEg->TimestampValue);
}
