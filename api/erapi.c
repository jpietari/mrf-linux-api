/*
  erapi.c -- Functions for Micro-Research Event Receiver
             Application Programming Interface

  Author: Jukka Pietarinen (MRF)
  Date:   08.12.2006

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
#else /* assume VxWorks */
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

/*
#define DEBUG 1
*/
#define DEBUG_PRINTF printf

#ifdef __unix__
int EvrOpenWindow(struct MrfErRegs **pEr, char *device_name, int mem_window)
{
  int fd;

  /* Open Event Receiver device for read/write */
  fd = open(device_name, O_RDWR);
#ifdef DEBUG
  DEBUG_PRINTF("EvrOpen: open(\"%s\", O_RDWR) returned %d\n", device_name, fd);
#endif
  if (fd != -1)
    {
      /* Memory map Event Receiver registers */
      *pEr = (struct MrfErRegs *) mmap(0, mem_window, PROT_READ | PROT_WRITE,
					MAP_SHARED, fd, 0);
#ifdef DEBUG
  DEBUG_PRINTF("EvrOpen: mmap returned %08x, errno %d\n", (int) *pEr,
	       errno);
#endif
      if (*pEr == MAP_FAILED)
	{
	  close(fd);
	  return -1;
	}
    }

  return fd;
}

int EvrOpen(struct MrfErRegs **pEr, char *device_name)
{
  return EvrOpenWindow(pEr, device_name, EVR_CPCI230_MEM_WINDOW);
}

int EvrTgOpen(struct MrfErRegs **pEr, char *device_name)
{
  return EvrOpenWindow(pEr, device_name, EVR_CPCI300TG_MEM_WINDOW);
}

#else
int EvrOpen(struct MrfErRegs **pEg, char *device_name)
{
  return 0;
}
#endif

#ifdef __unix__
int EvrCloseWindow(int fd, int mem_window)
{
  int result;

  result = munmap(0, mem_window);
  return close(fd);
}

int EvrClose(int fd)
{
  return EvrCloseWindow(fd, EVR_CPCI230_MEM_WINDOW);
}

int EvrTgClose(int fd)
{
  return EvrCloseWindow(fd, EVR_CPCI300TG_MEM_WINDOW);
}

#else
int EvrClose(int fd)
{
  return 0;
}
#endif

u32 EvrFWVersion(volatile struct MrfErRegs *pEr)
{
  return be32_to_cpu(pEr->FPGAVersion);
}

int EvrEnable(volatile struct MrfErRegs *pEr, int state)
{
  if (state)
    pEr->Control |= be32_to_cpu(1 << C_EVR_CTRL_MASTER_ENABLE);
  else
    pEr->Control &= be32_to_cpu(~(1 << C_EVR_CTRL_MASTER_ENABLE));
  
  return EvrGetEnable(pEr);
}

int EvrDCEnable(volatile struct MrfErRegs *pEr, int state)
{
  if (state)
    pEr->Control |= be32_to_cpu(1 << C_EVR_CTRL_DC_ENABLE);
  else
    pEr->Control &= be32_to_cpu(~(1 << C_EVR_CTRL_DC_ENABLE));
  
  return EvrGetDCEnable(pEr);
}

int EvrOutputEnable(volatile struct MrfErRegs *pEr, int state)
{
  if (state)
    pEr->Control |= be32_to_cpu(1 << C_EVR_CTRL_OUTEN);
  else
    pEr->Control &= be32_to_cpu(~(1 << C_EVR_CTRL_OUTEN));

  return EvrGetEnable(pEr);
}

int EvrGetEnable(volatile struct MrfErRegs *pEr)
{
  return be32_to_cpu(pEr->Control & be32_to_cpu(1 << C_EVR_CTRL_MASTER_ENABLE));
}

int EvrGetDCEnable(volatile struct MrfErRegs *pEr)
{
  return be32_to_cpu(pEr->Control & be32_to_cpu(1 << C_EVR_CTRL_DC_ENABLE));
}

int EvrGetViolation(volatile struct MrfErRegs *pEr, int clear)
{
  int result;

  result = be32_to_cpu(pEr->IrqFlag & be32_to_cpu(1 << C_EVR_IRQFLAG_VIOLATION));
  if (clear && result)
    pEr->IrqFlag = be32_to_cpu(result);

  return result;
}

void EvrDumpStatus(volatile struct MrfErRegs *pEr)
{
  int result;

  result = be32_to_cpu(pEr->Status);
  DEBUG_PRINTF("Status %08x ", result);
  if (result & (1 << C_EVR_STATUS_LEGACY_VIO))
    DEBUG_PRINTF("LEGVIO ");
  if (result & (1 << C_EVR_STATUS_LOG_STOPPED))
    DEBUG_PRINTF("LOGSTOP ");
  DEBUG_PRINTF("\n");
  result = be32_to_cpu(pEr->Control);
  DEBUG_PRINTF("Control %08x: ", result);
  if (result & (1 << C_EVR_CTRL_MASTER_ENABLE))
    DEBUG_PRINTF("MSEN ");
  if (result & (1 << C_EVR_CTRL_EVENT_FWD_ENA))
    DEBUG_PRINTF("FWD ");
  if (result & (1 << C_EVR_CTRL_TXLOOPBACK))
    DEBUG_PRINTF("TXLP ");
  if (result & (1 << C_EVR_CTRL_RXLOOPBACK))
    DEBUG_PRINTF("RXLP ");
  if (result & (1 << C_EVR_CTRL_TS_CLOCK_DBUS))
    DEBUG_PRINTF("DSDBUS ");
  if (result & (1 << C_EVR_CTRL_MAP_RAM_ENABLE))
    DEBUG_PRINTF("MAPENA ");
  if (result & (1 << C_EVR_CTRL_MAP_RAM_SELECT))
    DEBUG_PRINTF("MAPSEL ");
  DEBUG_PRINTF("\n");
  result = be32_to_cpu(pEr->IrqFlag);
  DEBUG_PRINTF("IRQ Flag %08x: ", result);
  if (result & (1 << C_EVR_IRQ_MASTER_ENABLE))
    DEBUG_PRINTF("IRQEN ");
  if (result & (1 << C_EVR_IRQFLAG_SEGBUF))
    DEBUG_PRINTF("SEGBUF ");
  if (result & (1 << C_EVR_IRQFLAG_DATABUF))
    DEBUG_PRINTF("DBUF ");
  if (result & (1 << C_EVR_IRQFLAG_PULSE))
    DEBUG_PRINTF("PULSE ");
  if (result & (1 << C_EVR_IRQFLAG_EVENT))
    DEBUG_PRINTF("EVENT ");
  if (result & (1 << C_EVR_IRQFLAG_HEARTBEAT))
    DEBUG_PRINTF("HB ");
  if (result & (1 << C_EVR_IRQFLAG_FIFOFULL))
    DEBUG_PRINTF("FF ");
  if (result & (1 << C_EVR_IRQFLAG_VIOLATION))
    DEBUG_PRINTF("VIO ");
  result = be32_to_cpu(pEr->IrqEnable);
  DEBUG_PRINTF("IRQ Enable %08x: ", result);
  if (result & (1 << C_EVR_IRQ_MASTER_ENABLE))
    DEBUG_PRINTF("IRQEN ");
  if (result & (1 << C_EVR_IRQFLAG_SEGBUF))
    DEBUG_PRINTF("SEGBUF ");
  if (result & (1 << C_EVR_IRQFLAG_DATABUF))
    DEBUG_PRINTF("DBUF ");
  if (result & (1 << C_EVR_IRQFLAG_PULSE))
    DEBUG_PRINTF("PULSE ");
  if (result & (1 << C_EVR_IRQFLAG_EVENT))
    DEBUG_PRINTF("EVENT ");
  if (result & (1 << C_EVR_IRQFLAG_HEARTBEAT))
    DEBUG_PRINTF("HB ");
  if (result & (1 << C_EVR_IRQFLAG_FIFOFULL))
    DEBUG_PRINTF("FF ");
  if (result & (1 << C_EVR_IRQFLAG_VIOLATION))
    DEBUG_PRINTF("VIO ");
  DEBUG_PRINTF("\n");
  result = be32_to_cpu(pEr->DataBufControl);
  DEBUG_PRINTF("DataBufControl %08x\n", result);
}

int EvrDumpMapRam(volatile struct MrfErRegs *pEr, int ram)
{
  uint32_t code;
  uint32_t intev;
  uint32_t ptrig, pset, pclr;

  for (code = 0; code <= EVR_MAX_EVENT_CODE; code++)
    {
      intev = be32_to_cpu(pEr->MapRam[ram][code].IntEvent);
      ptrig = be32_to_cpu(pEr->MapRam[ram][code].PulseTrigger);
      pset = be32_to_cpu(pEr->MapRam[ram][code].PulseSet);
      pclr = be32_to_cpu(pEr->MapRam[ram][code].PulseClear);

      if (intev ||
	  ptrig ||
	  pset ||
	  pclr)
	{
	  DEBUG_PRINTF("Code 0x%02x (%3d): ", code, code);
	  if (intev & (1 << C_EVR_MAP_SAVE_EVENT))
	    DEBUG_PRINTF("SAVE ");
	  if (intev & (1 << C_EVR_MAP_LATCH_TIMESTAMP))
	    DEBUG_PRINTF("LTS ");
	  if (intev & (1 << C_EVR_MAP_LED_EVENT))
	    DEBUG_PRINTF("LED ");
	  if (intev & (1 << C_EVR_MAP_FORWARD_EVENT))
	    DEBUG_PRINTF("FWD ");
	  if (intev & (1 << C_EVR_MAP_STOP_LOG))
	    DEBUG_PRINTF("STOPLOG ");
	  if (intev & (1 << C_EVR_MAP_LOG_EVENT))
	    DEBUG_PRINTF("LOG ");
	  if (intev & (1 << C_EVR_MAP_HEARTBEAT_EVENT))
	    DEBUG_PRINTF("HB ");
	  if (intev & (1 << C_EVR_MAP_RESETPRESC_EVENT))
	    DEBUG_PRINTF("RESPRSC ");
	  if (intev & (1 << C_EVR_MAP_TIMESTAMP_RESET))
	    DEBUG_PRINTF("RESTS ");
	  if (intev & (1 << C_EVR_MAP_TIMESTAMP_CLK))
	    DEBUG_PRINTF("TSCLK ");
	  if (intev & (1 << C_EVR_MAP_SECONDS_1))
	    DEBUG_PRINTF("SEC1 ");
	  if (intev & (1 << C_EVR_MAP_SECONDS_0))
	    DEBUG_PRINTF("SEC0 ");
	  if (ptrig)
	    DEBUG_PRINTF("Trig %08x", ptrig);
	  if (pset)
	    DEBUG_PRINTF("Set %08x", pset);
	  if (pclr)
	    DEBUG_PRINTF("Clear %08x", pclr);
	  DEBUG_PRINTF("\n");
	}
    }
  return 0;
}

int EvrMapRamEnable(volatile struct MrfErRegs *pEr, int ram, int enable)
{
  int result;

  if (ram < 0 || ram > 1)
    return -1;

  result = be32_to_cpu(pEr->Control);
  result &= ~((1 << C_EVR_CTRL_MAP_RAM_ENABLE) | (1 << C_EVR_CTRL_MAP_RAM_SELECT));
  if (ram == 1)
    result |= (1 << C_EVR_CTRL_MAP_RAM_SELECT);
  if (enable == 1)
    result |= (1 << C_EVR_CTRL_MAP_RAM_ENABLE);
  pEr->Control = be32_to_cpu(result);

  return result;
}

int EvrSetPulseMap(volatile struct MrfErRegs *pEr, int ram, int code, int trig,
		   int set, int clear)
{
  if (ram < 0 || ram >= EVR_MAPRAMS)
    return -1;

  if (code <= 0 || code > EVR_MAX_EVENT_CODE)
    return -1;

  if (trig >= 0 && trig < EVR_MAX_PULSES)
    pEr->MapRam[ram][code].PulseTrigger |= be32_to_cpu(1 << trig);
  if (set >= 0 && set < EVR_MAX_PULSES)
    pEr->MapRam[ram][code].PulseSet |= be32_to_cpu(1 << set);
  if (clear >= 0 && clear < EVR_MAX_PULSES)
    pEr->MapRam[ram][code].PulseClear |= be32_to_cpu(1 << clear);

  return 0;
}

int EvrSetForwardEvent(volatile struct MrfErRegs *pEr, int ram, int code, int enable)
{
  if (ram < 0 || ram >= EVR_MAPRAMS)
    return -1;

  if (code <= 0 || code > EVR_MAX_EVENT_CODE)
    return -1;

  if (!enable)
    pEr->MapRam[ram][code].IntEvent &= be32_to_cpu(~(1 << C_EVR_MAP_FORWARD_EVENT));
  if (enable)
    pEr->MapRam[ram][code].IntEvent |= be32_to_cpu(1 << C_EVR_MAP_FORWARD_EVENT);
    
  return 0;
}

int EvrEnableEventForwarding(volatile struct MrfErRegs *pEr, int enable)
{
  if (enable)
    pEr->Control |= be32_to_cpu(1 << C_EVR_CTRL_EVENT_FWD_ENA);
  else
    pEr->Control &= be32_to_cpu(~(1 << C_EVR_CTRL_EVENT_FWD_ENA));
  
  return EvrGetEventForwarding(pEr);
}

int EvrGetEventForwarding(volatile struct MrfErRegs *pEr)
{
  return be32_to_cpu(pEr->Control & be32_to_cpu(1 << C_EVR_CTRL_EVENT_FWD_ENA));
}

int EvrSetLedEvent(volatile struct MrfErRegs *pEr, int ram, int code, int enable)
{
  if (ram < 0 || ram >= EVR_MAPRAMS)
    return -1;

  if (code <= 0 || code > EVR_MAX_EVENT_CODE)
    return -1;

  if (!enable)
    pEr->MapRam[ram][code].IntEvent &= be32_to_cpu(~(1 << C_EVR_MAP_LED_EVENT));
  if (enable)
    pEr->MapRam[ram][code].IntEvent |= be32_to_cpu(1 << C_EVR_MAP_LED_EVENT);
    
  return 0;
}

int EvrSetFIFOEvent(volatile struct MrfErRegs *pEr, int ram, int code, int enable)
{
  if (ram < 0 || ram >= EVR_MAPRAMS)
    return -1;

  if (code <= 0 || code > EVR_MAX_EVENT_CODE)
    return -1;

  if (!enable)
    pEr->MapRam[ram][code].IntEvent &= be32_to_cpu(~(1 << C_EVR_MAP_SAVE_EVENT));
  if (enable)
    pEr->MapRam[ram][code].IntEvent |= be32_to_cpu(1 << C_EVR_MAP_SAVE_EVENT);
    
  return 0;
}

int EvrSetLatchEvent(volatile struct MrfErRegs *pEr, int ram, int code, int enable)
{
  if (ram < 0 || ram >= EVR_MAPRAMS)
    return -1;

  if (code <= 0 || code > EVR_MAX_EVENT_CODE)
    return -1;

  if (!enable)
    pEr->MapRam[ram][code].IntEvent &= be32_to_cpu(~(1 << C_EVR_MAP_LATCH_TIMESTAMP));
  if (enable)
    pEr->MapRam[ram][code].IntEvent |= be32_to_cpu(1 << C_EVR_MAP_LATCH_TIMESTAMP);
    
  return 0;
}

int EvrSetLogEvent(volatile struct MrfErRegs *pEr, int ram, int code, int enable)
{
  if (ram < 0 || ram >= EVR_MAPRAMS)
    return -1;

  if (code <= 0 || code > EVR_MAX_EVENT_CODE)
    return -1;

  if (!enable)
    pEr->MapRam[ram][code].IntEvent &= be32_to_cpu(~(1 << C_EVR_MAP_LOG_EVENT));
  if (enable)
    pEr->MapRam[ram][code].IntEvent |= be32_to_cpu(1 << C_EVR_MAP_LOG_EVENT);
    
  return 0;
}

int EvrSetLogStopEvent(volatile struct MrfErRegs *pEr, int ram, int code, int enable)
{
  if (ram < 0 || ram >= EVR_MAPRAMS)
    return -1;

  if (code <= 0 || code > EVR_MAX_EVENT_CODE)
    return -1;

  if (!enable)
    pEr->MapRam[ram][code].IntEvent &= be32_to_cpu(~(1 << C_EVR_MAP_STOP_LOG));
  if (enable)
    pEr->MapRam[ram][code].IntEvent |= be32_to_cpu(1 << C_EVR_MAP_STOP_LOG);
    
  return 0;
}

int EvrClearFIFO(volatile struct MrfErRegs *pEr)
{
  int ctrl;

  ctrl = be32_to_cpu(pEr->Control);
  ctrl |= (1 << C_EVR_CTRL_RESET_EVENTFIFO);
  pEr->Control = be32_to_cpu(ctrl);

  return be32_to_cpu(pEr->Control);
}

int EvrGetFIFOEvent(volatile struct MrfErRegs *pEr, struct FIFOEvent *fe)
{
  int stat;

  stat = be32_to_cpu(pEr->IrqFlag);
  if (stat & (1 << C_EVR_IRQFLAG_EVENT))
    {
      fe->EventCode = be32_to_cpu(pEr->FIFOEvent);
      fe->TimestampHigh = be32_to_cpu(pEr->FIFOSeconds);
      fe->TimestampLow = be32_to_cpu(pEr->FIFOTimestamp);
      return 0;
    }
  else
    return -1;
}

int EvrEnableLog(volatile struct MrfErRegs *pEr, int enable)
{
  if (enable)
    pEr->Control |= be32_to_cpu(1 << C_EVR_CTRL_LOG_ENABLE);
  else
    pEr->Control |= be32_to_cpu(1 << C_EVR_CTRL_LOG_DISABLE);
  
  return EvrGetLogState(pEr);
}

int EvrGetLogState(volatile struct MrfErRegs *pEr)
{
  return be32_to_cpu(pEr->Status & be32_to_cpu(1 << C_EVR_STATUS_LOG_STOPPED));
}

int EvrGetLogStart(volatile struct MrfErRegs *pEr)
{
  int pos;

  pos = be32_to_cpu(pEr->LogStatus);
  if (pos >= 0)
    return 0;
  else
    return (pos & (EVR_LOG_SIZE - 1));
}

int EvrGetLogEntries(volatile struct MrfErRegs *pEr)
{
  int pos;

  pos = be32_to_cpu(pEr->LogStatus);
  if (pos >= 0)
    return pos;
  else
    return EVR_LOG_SIZE;
}

int EvrEnableLogStopEvent(volatile struct MrfErRegs *pEr, int enable)
{
  if (enable)
    pEr->Control |= be32_to_cpu(1 << C_EVR_CTRL_LOG_STOP_EV_EN);
  else
    pEr->Control &= be32_to_cpu(~(1 << C_EVR_CTRL_LOG_STOP_EV_EN));
  
  return EvrGetLogStopEvent(pEr);
}

int EvrGetLogStopEvent(volatile struct MrfErRegs *pEr)
{
  return be32_to_cpu(pEr->Control & be32_to_cpu(1 << C_EVR_CTRL_LOG_STOP_EV_EN));
}

int EvrDumpFIFO(volatile struct MrfErRegs *pEr)
{
  struct FIFOEvent fe;
  int i;

  do
    {
      i = EvrGetFIFOEvent(pEr, &fe);
      if (!i)
	{
	  printf("FIFO Code %08x, %08x:%08x\n",
		 fe.EventCode, fe.TimestampHigh, fe.TimestampLow);
	}
    }
  while (!i);

  return 0;
}

int EvrClearLog(volatile struct MrfErRegs *pEr)
{
  int ctrl;

  ctrl = be32_to_cpu(pEr->Control);
  ctrl |= (1 << C_EVR_CTRL_LOG_RESET);
  pEr->Control = be32_to_cpu(ctrl);

  return be32_to_cpu(pEr->Control);
}

int EvrDumpLog(volatile struct MrfErRegs *pEr)
{
  int pos, i, j;

  pos = EvrGetLogStart(pEr);
  i = EvrGetLogEntries(pEr);
  j = 1;
  if (i > 512) i = 512;
  for (; i; i--)
    {
      printf("%02x Log Code %08x, %08x:%08x\n", j++,
	     be32_to_cpu(pEr->Log[pos].EventCode),
	     be32_to_cpu(pEr->Log[pos].TimestampHigh),
	     be32_to_cpu(pEr->Log[pos].TimestampLow));
      pos++;
      pos &= (EVR_LOG_SIZE - 1);
    }

  return 0;
}

int EvrClearPulseMap(volatile struct MrfErRegs *pEr, int ram, int code, int trig,
		     int set, int clear)
{
  if (ram < 0 || ram >= EVR_MAPRAMS)
    return -1;

  if (code <= 0 || code > EVR_MAX_EVENT_CODE)
    return -1;

  if (trig >= 0 && trig < EVR_MAX_PULSES)
    pEr->MapRam[ram][code].PulseTrigger &= be32_to_cpu(~(1 << trig));
  if (set >= 0 && set < EVR_MAX_PULSES)
    pEr->MapRam[ram][code].PulseSet &= be32_to_cpu(~(1 << set));
  if (clear >= 0 && clear < EVR_MAX_PULSES)
    pEr->MapRam[ram][code].PulseClear &= be32_to_cpu(~(1 << clear));

  return 0;
}

int EvrSetPulseParams(volatile struct MrfErRegs *pEr, int pulse, int presc,
		      int delay, int width)
{
  if (pulse < 0 || pulse >= EVR_MAX_PULSES)
    return -1;

  pEr->Pulse[pulse].Width = be32_to_cpu(width);
  pEr->Pulse[pulse].Delay = be32_to_cpu(delay);
  pEr->Pulse[pulse].Prescaler = be32_to_cpu(presc);

  return 0;
}

int EvrGetPulsePresc(volatile struct MrfErRegs *pEr, int pulse)
{
  if (pulse < 0 || pulse >= EVR_MAX_PULSES)
    return -1;

  return be32_to_cpu(pEr->Pulse[pulse].Prescaler);
}

int EvrGetPulseDelay(volatile struct MrfErRegs *pEr, int pulse)
{
  if (pulse < 0 || pulse >= EVR_MAX_PULSES)
    return -1;

  return be32_to_cpu(pEr->Pulse[pulse].Delay);
}

int EvrGetPulseWidth(volatile struct MrfErRegs *pEr, int pulse)
{
  if (pulse < 0 || pulse >= EVR_MAX_PULSES)
    return -1;

  return be32_to_cpu(pEr->Pulse[pulse].Width);
}

void EvrDumpPulses(volatile struct MrfErRegs *pEr, int pulses)
{
  int i, control;

  for (i = 0; i < pulses; i++)
    {
      DEBUG_PRINTF("Pulse %02x Presc %08x Delay %08x Width %08x", i,
		   be32_to_cpu(pEr->Pulse[i].Prescaler), 
		   be32_to_cpu(pEr->Pulse[i].Delay), 
		   be32_to_cpu(pEr->Pulse[i].Width));
      control = be32_to_cpu(pEr->Pulse[i].Control);
      DEBUG_PRINTF(" Output %d", control & (1 << C_EVR_PULSE_OUT) ? 1 : 0);
      if (control & (1 << C_EVR_PULSE_POLARITY))
	DEBUG_PRINTF(" NEG");
      if (control & (1 << C_EVR_PULSE_MAP_RESET_ENA))
	DEBUG_PRINTF(" MAPRES");
      if (control & (1 << C_EVR_PULSE_MAP_SET_ENA))
	DEBUG_PRINTF(" MAPSET");
      if (control & (1 << C_EVR_PULSE_MAP_TRIG_ENA))
	DEBUG_PRINTF(" MAPTRIG");
      if (control & (1 << C_EVR_PULSE_ENA))
	DEBUG_PRINTF(" ENA");
      DEBUG_PRINTF("\n");
    }
}

int EvrSetPulseProperties(volatile struct MrfErRegs *pEr, int pulse, int polarity,
			  int map_reset_ena, int map_set_ena, int map_trigger_ena,
			  int enable)
{
  int result;

  if (pulse < 0 || pulse >= EVR_MAX_PULSES)
    return -1;

  result = be32_to_cpu(pEr->Pulse[pulse].Control);

  /* 0 clears, 1 sets, others don't change */
  if (polarity == 0)
    result &= ~(1 << C_EVR_PULSE_POLARITY);
  if (polarity == 1)
    result |= (1 << C_EVR_PULSE_POLARITY);

  if (map_reset_ena == 0)
    result &= ~(1 << C_EVR_PULSE_MAP_RESET_ENA);
  if (map_reset_ena == 1)
    result |= (1 << C_EVR_PULSE_MAP_RESET_ENA);

  if (map_set_ena == 0)
    result &= ~(1 << C_EVR_PULSE_MAP_SET_ENA);
  if (map_set_ena == 1)
    result |= (1 << C_EVR_PULSE_MAP_SET_ENA);

  if (map_trigger_ena == 0)
    result &= ~(1 << C_EVR_PULSE_MAP_TRIG_ENA);
  if (map_trigger_ena == 1)
    result |= (1 << C_EVR_PULSE_MAP_TRIG_ENA);

  if (enable == 0)
    result &= ~(1 << C_EVR_PULSE_ENA);
  if (enable == 1)
    result |= (1 << C_EVR_PULSE_ENA);

#ifdef DEBUG
  DEBUG_PRINTF("Pulse[%d].Control %08x\n", pulse, result);
#endif

  pEr->Pulse[pulse].Control = be32_to_cpu(result);

  return 0;
}

int EvrSetPulseMask(volatile struct MrfErRegs *pEr, int pulse, int mask, int enable)
{
  int result;

  if (pulse < 0 || pulse >= EVR_MAX_PULSES)
    return -1;

  result = be32_to_cpu(pEr->Pulse[pulse].Control);

  result &= 0x0000ffff;
  result |= ((mask & 0x00ff) << 24);
  result |= ((enable & 0x00ff) << 16);

  pEr->Pulse[pulse].Control = be32_to_cpu(result);

  return 0;  
}

int EvrSetPrescalerTrig(volatile struct MrfErRegs *pEr, int prescaler, int trigs)
{
  int result;

  if (prescaler < 0 || prescaler >= EVR_MAX_PRESCALERS)
    return -1;

  pEr->PrescalerTrig[prescaler] = be32_to_cpu(trigs);
  return be32_to_cpu(pEr->PrescalerTrig[prescaler]);  
}

int EvrSetDBusTrig(volatile struct MrfErRegs *pEr, int dbus, int trigs)
{
  int result;

  if (dbus < 0 || dbus >= 8)
    return -1;

  pEr->DBusTrig[dbus] = be32_to_cpu(trigs);
  return be32_to_cpu(pEr->DBusTrig[dbus]);  
}

int EvrSetUnivOutMap(volatile struct MrfErRegs *pEr, int output, int map)
{
  if (output < 0 || output >= EVR_MAX_UNIVOUT_MAP)
    return -1;

  pEr->UnivOutMap[output] = be16_to_cpu(map);

  return be16_to_cpu(pEr->UnivOutMap[output]);
}

int EvrGetUnivOutMap(volatile struct MrfErRegs *pEr, int output)
{
  if (output < 0 || output >= EVR_MAX_UNIVOUT_MAP)
    return -1;

  return be16_to_cpu(pEr->UnivOutMap[output]);
}

void EvrDumpUnivOutMap(volatile struct MrfErRegs *pEr, int outputs)
{
  int i;

  for (i = 0; i < outputs; i++)
    DEBUG_PRINTF("UnivOut[%d] %02x\n", i, be16_to_cpu(pEr->UnivOutMap[i]));
}

int EvrSetFPOutMap(volatile struct MrfErRegs *pEr, int output, int map)
{
  if (output < 0 || output >= EVR_MAX_FPOUT_MAP)
    return -1;

  pEr->FPOutMap[output] = be16_to_cpu(map);

  return be16_to_cpu(pEr->FPOutMap[output]);
}

int EvrGetFPOutMap(volatile struct MrfErRegs *pEr, int output)
{
  if (output < 0 || output >= EVR_MAX_FPOUT_MAP)
    return -1;

  return be16_to_cpu(pEr->FPOutMap[output]);
}

void EvrDumpFPOutMap(volatile struct MrfErRegs *pEr, int outputs)
{
  int i;

  for (i = 0; i < outputs; i++)
    DEBUG_PRINTF("FPOut[%d] %02x\n", i, be16_to_cpu(pEr->FPOutMap[i]));
}

int EvrSetTBOutMap(volatile struct MrfErRegs *pEr, int output, int map)
{
  if (output < 0 || output >= EVR_MAX_TBOUT_MAP)
    return -1;

  pEr->TBOutMap[output] = be16_to_cpu(map);

  return be16_to_cpu(pEr->TBOutMap[output]);
}

int EvrGetTBOutMap(volatile struct MrfErRegs *pEr, int output)
{
  if (output < 0 || output >= EVR_MAX_TBOUT_MAP)
    return -1;

  return be16_to_cpu(pEr->TBOutMap[output]);
}

void EvrDumpTBOutMap(volatile struct MrfErRegs *pEr, int outputs)
{
  int i;

  for (i = 0; i < outputs; i++)
    DEBUG_PRINTF("TBOut[%d] %02x\n", i, be16_to_cpu(pEr->TBOutMap[i]));
}

int EvrSetBPOutMap(volatile struct MrfErRegs *pEr, int output, int map)
{
  if (output < 0 || output >= EVR_MAX_BPOUT_MAP)
    return -1;

  pEr->BPOutMap[output] = be16_to_cpu(map);

  return be16_to_cpu(pEr->BPOutMap[output]);
}

int EvrGetBPOutMap(volatile struct MrfErRegs *pEr, int output)
{
  if (output < 0 || output >= EVR_MAX_BPOUT_MAP)
    return -1;

  return be16_to_cpu(pEr->BPOutMap[output]);
}

void EvrDumpBPOutMap(volatile struct MrfErRegs *pEr, int outputs)
{
  int i;

  for (i = 0; i < outputs; i++)
    DEBUG_PRINTF("BPOut[%d] %02x\n", i, be16_to_cpu(pEr->BPOutMap[i]));
}

#ifdef __unix__
void EvrIrqAssignHandler(volatile struct MrfErRegs *pEr, int fd,
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
  EvrIrqHandled(fd);
}

void EvrIrqUnassignHandler(int vector,
			 void (*handler)(int))
{
}
#endif

#ifdef VXWORKS
void EvrIrqAssignHandler(volatile struct MrfErRegs *pEr, int vector,
			 void (*handler)(int))
{
  return intConnect(INUM_TO_IVEC(vector), handler, pEr);
}

void EvrIrqUnassignHandler(int vector,
			 void (*handler)(int))
{
  ppcDisconnectVec(vector, handler);
}
#endif

int EvrIrqEnable(volatile struct MrfErRegs *pEr, int mask)
{
  int control = be32_to_cpu(pEr->IrqEnable) & EVR_IRQ_PCICORE_ENABLE;

  pEr->IrqEnable = be32_to_cpu(mask | control);
  return be32_to_cpu(pEr->IrqEnable);
}

int EvrGetIrqEnable(volatile struct MrfErRegs *pEr)
{
  return be32_to_cpu(pEr->IrqEnable);
}

int EvrGetIrqFlags(volatile struct MrfErRegs *pEr)
{
  return be32_to_cpu(pEr->IrqFlag);
}

int EvrClearIrqFlags(volatile struct MrfErRegs *pEr, int mask)
{
  pEr->IrqFlag = be32_to_cpu(mask);
  return be32_to_cpu(pEr->IrqFlag);
}

#ifdef __unix__
void EvrIrqHandled(int fd)
{
  ioctl(fd, EV_IOCIRQEN);
}
#endif

int EvrSetPulseIrqMap(volatile struct MrfErRegs *pEr, int map)
{
  pEr->PulseIrqMap = be32_to_cpu(map);

  return be32_to_cpu(pEr->PulseIrqMap);
}

void EvrClearDiagCounters(volatile struct MrfErRegs *pEr)
{
  pEr->DiagReset = 0xffffffff;
  pEr->DiagReset = 0x0;
}

int EvrEnableDiagCounters(volatile struct MrfErRegs *pEr, int enable)
{
  if (enable)
    pEr->DiagCE = 0xffffffff;
  else
    pEr->DiagCE = 0;

  return be32_to_cpu(pEr->DiagCE);
}

u32 EvrGetDiagCounter(volatile struct MrfErRegs *pEr, int idx)
{
  return be32_to_cpu(pEr->DiagCounter[idx]);
}

int EvrSetGPIODir(volatile struct MrfErRegs *pEr, int dir)
{
  pEr->GPIODir = be32_to_cpu(dir);
  return be32_to_cpu(pEr->GPIODir);
}

int EvrSetGPIOOut(volatile struct MrfErRegs *pEr, int dout)
{
  pEr->GPIOOut = be32_to_cpu(dout);
  return be32_to_cpu(pEr->GPIOOut);
}

int EvrGetGPIOIn(volatile struct MrfErRegs *pEr)
{
  return be32_to_cpu(pEr->GPIOIn);
}

int EvrUnivDlyEnable(volatile struct MrfErRegs *pEr, int dlymod, int enable)
{
  u32 gpio;
  int sh = 0;

  switch (dlymod)
    {
    case 0:
      sh = 0;
      break;
    case 1:
      sh = 4;
      break;
    case 2:
      sh = 8;
      break;
    case 3:
      sh = 12;
      break;
    default:
      return -1;
    }
  
  /* Setup outputs for both slots */
  pEr->GPIODir = be32_to_cpu(((EVR_UNIV_DLY_DIN | EVR_UNIV_DLY_SCLK |
	    EVR_UNIV_DLY_LCLK | EVR_UNIV_DLY_DIS) |
	   ((EVR_UNIV_DLY_DIN | EVR_UNIV_DLY_SCLK |
            EVR_UNIV_DLY_LCLK | EVR_UNIV_DLY_DIS) << 4) |
           ((EVR_UNIV_DLY_DIN | EVR_UNIV_DLY_SCLK |
	    EVR_UNIV_DLY_LCLK | EVR_UNIV_DLY_DIS) << 8) |
	   ((EVR_UNIV_DLY_DIN | EVR_UNIV_DLY_SCLK |
	    EVR_UNIV_DLY_LCLK | EVR_UNIV_DLY_DIS) << 12)));
  gpio = be32_to_cpu(pEr->GPIOOut) & ~(EVR_UNIV_DLY_DIS << sh);
  if (!enable)
    gpio |= (EVR_UNIV_DLY_DIS << sh);
  pEr->GPIOOut = be32_to_cpu(gpio);

  return 0;
}

int EvrUnivDlySetDelay(volatile struct MrfErRegs *pEr, int dlymod, int dly0, int dly1)
{
  u32 gpio;
  int sh = 0;
  int sd;
  int sr, i, din, sclk, lclk, dbit;

  switch (dlymod)
    {
    case 0:
      sh = 0;
      break;
    case 1:
      sh = 4;
      break;
    case 2:
      sh = 8;
      break;
    case 3:
      sh = 12;
      break;
    default:
      return -1;
    }
  
  din = EVR_UNIV_DLY_DIN << sh;
  sclk = EVR_UNIV_DLY_SCLK << sh;
  lclk = EVR_UNIV_DLY_LCLK << sh;

  gpio = be32_to_cpu(pEr->GPIOOut) & ~((EVR_UNIV_DLY_DIN | EVR_UNIV_DLY_SCLK |
					EVR_UNIV_DLY_LCLK) | 
				      ((EVR_UNIV_DLY_DIN | EVR_UNIV_DLY_SCLK |
                                        EVR_UNIV_DLY_LCLK) << 4) | 
                                      ((EVR_UNIV_DLY_DIN | EVR_UNIV_DLY_SCLK |
                                        EVR_UNIV_DLY_LCLK) << 8) | 
                                      ((EVR_UNIV_DLY_DIN | EVR_UNIV_DLY_SCLK |
                                        EVR_UNIV_DLY_LCLK) << 12));
  /* Limit delay values */
  dly0 &= 0x03ff;
  dly1 &= 0x03ff;

  /* We have to shift in the bits in following order:
     DA7, DA6, DA5, DA4, DA3, DA2, DA1, DA0,
     DB3, DB2, DB1, DB0, LENA, 0, DA9, DA8,
     LENB, 0, DB9, DB8, DB7, DB6, DB5, DB4 */

  sd = ((dly1 & 0x0ff) << 16) |
    ((dly0 & 0x00f) << 12) | (dly1 & 0x300) | 
    (dly0 >> 4);

  sr = sd;
  for (i = 24; i; i--)
    {
      dbit = 0;
      if (sr & 0x00800000)
	dbit = din;
      pEr->GPIOOut = be32_to_cpu(gpio | dbit);
      pEr->GPIOOut = be32_to_cpu(gpio | dbit | sclk);
      pEr->GPIOOut = be32_to_cpu(gpio | dbit);
      sr <<= 1;
    }

  pEr->GPIOOut = be32_to_cpu(gpio | lclk);
  pEr->GPIOOut = be32_to_cpu(gpio);

  /* Latch enables active */
  sr = sd | 0x000880;
  for (i = 24; i; i--)
    {
      dbit = 0;
      if (sr & 0x00800000)
	dbit = din;
      pEr->GPIOOut = be32_to_cpu(gpio | dbit);
      pEr->GPIOOut = be32_to_cpu(gpio | dbit | sclk);
      pEr->GPIOOut = be32_to_cpu(gpio | dbit);
      sr <<= 1;
    }

  pEr->GPIOOut = be32_to_cpu(gpio | lclk);
  pEr->GPIOOut = be32_to_cpu(gpio);

  sr = sd;
  for (i = 24; i; i--)
    {
      dbit = 0;
      if (sr & 0x00800000)
	dbit = din;
      pEr->GPIOOut = be32_to_cpu(gpio | dbit);
      pEr->GPIOOut = be32_to_cpu(gpio | dbit | sclk);
      pEr->GPIOOut = be32_to_cpu(gpio | dbit);
      sr <<= 1;
    }

  pEr->GPIOOut = be32_to_cpu(gpio | lclk);
  pEr->GPIOOut = be32_to_cpu(gpio);

  return 0;
}

void EvrDumpHex(volatile struct MrfErRegs *pEr)
{
  u32 *p = (u32 *) pEr;
  int i,j;

  for (i = 0; i < 0x600; i += 0x20)
    {
      printf("%08x: ", i);
      for (j = 0; j < 8; j++)
	printf("%08x ", be32_to_cpu(*p++));
      printf("\n");
    }
}

int EvrSetFracDiv(volatile struct MrfErRegs *pEr, int fracdiv)
{
  pEr->UsecDiv = be32_to_cpu((int) cw_to_freq(fracdiv));

  return be32_to_cpu(pEr->FracDiv = be32_to_cpu(fracdiv));
}

int EvrGetFracDiv(volatile struct MrfErRegs *pEr)
{
  return be32_to_cpu(pEr->FracDiv);
}

int EvrSetDBufMode(volatile struct MrfErRegs *pEr, int enable)
{
  if (enable)
    pEr->DataBufControl = be32_to_cpu(1 << C_EVR_DATABUF_MODE);
  else
    pEr->DataBufControl = 0;

  return EvrGetDBufStatus(pEr);
}

int EvrGetDBufStatus(volatile struct MrfErRegs *pEr)
{
  volatile u32 *dbc = &(pEr->DataBufControl);

  return be32_to_cpu(*dbc);
}

int EvrReceiveDBuf(volatile struct MrfErRegs *pEr, int enable)
{
  if (enable)
    pEr->DataBufControl |= be32_to_cpu(1 << C_EVR_DATABUF_LOAD);
  else
    pEr->DataBufControl |= be32_to_cpu(1 << C_EVR_DATABUF_STOP);

  return EvrGetDBufStatus(pEr);
}

int EvrGetDBuf(volatile struct MrfErRegs *pEr, char *dbuf, int size)
{
  int stat, rxsize;

  stat = EvrGetDBufStatus(pEr);
  /* Check that DBUF mode enabled */
  if (!(stat & (1 << C_EVR_DATABUF_MODE)))
    return -1;
  /* Check that transfer is completed */
  if (!(stat & (1 << C_EVR_DATABUF_RXREADY)))
    return -1;

  rxsize = stat & (EVR_MAX_BUFFER-1);

  if (size < rxsize)
    return -1;

#ifdef __unix__
  memcpy((void *) dbuf, (void *) &pEr->Databuf[0], rxsize);
#else
  memcpy((void *) dbuf, (void *) &pEr->Databuf[0], rxsize);
  /*  {
    int i;
    int *p = (int *) dbuf;
    
    for (i = 0; i < size/4; i++)
      p[i] = be32_to_cpu(pEr->Databuf[i]);
      } */
#endif

  if (stat & (1 << C_EVR_DATABUF_CHECKSUM))
    return -1;

  return rxsize;
}

int EvrGetSegRx(volatile struct MrfErRegs *pEr, int segment)
{
  u32 *rx_flag;

  if (segment < 0 || segment > 255)
    return -1;

  rx_flag = (u32 *) &(pEr->SegRXReg[segment/32]);
  
  if (be32_to_cpu(*rx_flag) & (0x80000000 >> (segment % 32)))
    return 1;
  else
    return 0;
}

int EvrGetSegOv(volatile struct MrfErRegs *pEr, int segment)
{
  u32 *ov_flag;

  if (segment < 0 || segment > 255)
    return -1;

  ov_flag = (u32 *) &(pEr->SegOVReg[segment/32]);
  
  if (be32_to_cpu(*ov_flag) & (0x80000000 >> (segment % 32)))
    return 1;
  else
    return 0;
}

int EvrGetSegCs(volatile struct MrfErRegs *pEr, int segment)
{
  u32 *cs_flag;

  if (segment < 0 || segment > 255)
    return -1;

  cs_flag = (u32 *) &(pEr->SegCSReg[segment/32]);
  
  if (be32_to_cpu(*cs_flag) & (0x80000000 >> (segment % 32)))
    return 1;
  else
    return 0;
}

void EvrClearSegFlag(volatile struct MrfErRegs *pEr, int segment)
{
  u32 *rx_flag;

  if (segment < 0 || segment > 255)
    return;

  rx_flag = (u32 *) &(pEr->SegRXReg[segment/32]);
  
  *rx_flag = be32_to_cpu((0x80000000 >> (segment % 32)));
}

int EvrGetSegBuf(volatile struct MrfErRegs *pEr, char *dbuf, int segment)
{
  int stat, rxsize;

  if (segment < 0 || segment > 255)
    return -1;

  if (EvrGetSegRx(pEr, segment) != 1)
    {
      printf("RX segment not ready.\r\n");
      return -1;
    }

  rxsize = be32_to_cpu(pEr->SegBufSize[segment]);

  if (rxsize >= 0 && rxsize <= EVR_MAX_BUFFER)
    {

#ifdef __unix__
      memcpy((void *) dbuf, (void *) (&pEr->SegBuf[segment * 4]), rxsize);
#else
      memcpy((void *) dbuf, (void *) (&pEr->SegBuf[segment * 4]), rxsize);
#endif
    }

  if (EvrGetSegCs(pEr, segment))
    {
      printf("RX segment checksum error.\r\n");
      return -1;
    }

  if (EvrGetSegOv(pEr, segment))
    {
      printf("RX segment overflow error.\r\n");
    }

  EvrClearSegFlag(pEr, segment);

  return rxsize;
}

int EvrSetTimestampDivider(volatile struct MrfErRegs *pEr, int div)
{
  pEr->EvCntPresc = be32_to_cpu(div);

  return be32_to_cpu(pEr->EvCntPresc);
}

int EvrGetTimestampCounter(volatile struct MrfErRegs *pEr)
{
  return be32_to_cpu(pEr->TimestampEventCounter);
}

int EvrGetSecondsCounter(volatile struct MrfErRegs *pEr)
{
  return be32_to_cpu(pEr->SecondsCounter);
}

int EvrGetTimestampLatch(volatile struct MrfErRegs *pEr)
{
  return be32_to_cpu(pEr->TimestampLatch);
}

int EvrGetSecondsLatch(volatile struct MrfErRegs *pEr)
{
  return be32_to_cpu(pEr->SecondsLatch);
}

int EvrSetTimestampDBus(volatile struct MrfErRegs *pEr, int enable)
{
  int ctrl;

  ctrl = be32_to_cpu(pEr->Control);
  if (enable)
    ctrl |= (1 << C_EVR_CTRL_TS_CLOCK_DBUS);
  else
    ctrl &= ~(1 << C_EVR_CTRL_TS_CLOCK_DBUS);
  pEr->Control = be32_to_cpu(ctrl);

  return be32_to_cpu(pEr->Control);  
}

int EvrSetPrescaler(volatile struct MrfErRegs *pEr, int presc, int div)
{
  if (presc >= 0 && presc < EVR_MAX_PRESCALERS)
    {
      pEr->Prescaler[presc] = be32_to_cpu(div);

      return be32_to_cpu(pEr->Prescaler[presc]);
    }
  return -1;
}

int EvrGetPrescaler(volatile struct MrfErRegs *pEr, int presc)
{
  if (presc >= 0 && presc < EVR_MAX_PRESCALERS)
    {
      return be32_to_cpu(pEr->Prescaler[presc]);
    }
  return -1;
}

int EvrSetPrescalerPolarity(volatile struct MrfErRegs *pEr, int polarity)
{
  if (polarity)
    pEr->Control |= be32_to_cpu(1 << C_EVR_CTRL_PRESC_POLARITY);
  else
    pEr->Control &= be32_to_cpu(~(1 << C_EVR_CTRL_PRESC_POLARITY));
  
  return be32_to_cpu(pEr->Control & (1 << C_EVR_CTRL_PRESC_POLARITY));
}

int EvrSetPrescalerPhase(volatile struct MrfErRegs *pEr, int presc, int phase)
{
  if (presc >= 0 && presc < EVR_MAX_PRESCALERS)
    {
      pEr->PrescalerPhase[presc] = be32_to_cpu(phase);

      return be32_to_cpu(pEr->PrescalerPhase[presc]);
    }
  return -1;
}

int EvrSetExtEvent(volatile struct MrfErRegs *pEr, int ttlin, int code, int edge_enable, int level_enable)
{
  int fpctrl;

  if (ttlin < 0 || ttlin > EVR_MAX_FPIN_MAP)
    return -1;

  fpctrl = be32_to_cpu(pEr->FPInMap[ttlin]);
  if (code >= 0 && code <= EVR_MAX_EVENT_CODE)
    {
      fpctrl &= ~(EVR_MAX_EVENT_CODE << C_EVR_FPIN_EXTEVENT_BASE);
      fpctrl |= code << C_EVR_FPIN_EXTEVENT_BASE;
    }
  fpctrl &= ~(1 << C_EVR_FPIN_EXT_ENABLE);
  if (edge_enable)
    fpctrl |= (1 << C_EVR_FPIN_EXT_ENABLE);

  fpctrl &= ~(1 << C_EVR_FPIN_EXTLEV_ENABLE);
  if (level_enable)
    fpctrl |= (1 << C_EVR_FPIN_EXTLEV_ENABLE);

  pEr->FPInMap[ttlin] = be32_to_cpu(fpctrl);
  if (pEr->FPInMap[ttlin] == be32_to_cpu(fpctrl))
    return 0;
  return -1;
}

int EvrGetExtEventCode(volatile struct MrfErRegs *pEr, int ttlin)
{
  int fpctrl;

  if (ttlin < 0 || ttlin > EVR_MAX_FPIN_MAP)
    return -1;

  fpctrl = be32_to_cpu(pEr->FPInMap[ttlin]);
  return (fpctrl >> C_EVR_FPIN_EXTEVENT_BASE) & EVR_MAX_EVENT_CODE;
}

int EvrSetBackEvent(volatile struct MrfErRegs *pEr, int ttlin, int code, int edge_enable, int level_enable)
{
  int fpctrl;

  if (ttlin < 0 || ttlin > EVR_MAX_FPIN_MAP)
    return -1;

  fpctrl = be32_to_cpu(pEr->FPInMap[ttlin]);
  if (code >= 0 && code <= EVR_MAX_EVENT_CODE)
    {
      fpctrl &= ~(EVR_MAX_EVENT_CODE << C_EVR_FPIN_BACKEVENT_BASE);
      fpctrl |= code << C_EVR_FPIN_BACKEVENT_BASE;
    }
  fpctrl &= ~(1 << C_EVR_FPIN_BACKEV_ENABLE);
  if (edge_enable)
    fpctrl |= (1 << C_EVR_FPIN_BACKEV_ENABLE);

  fpctrl &= ~(1 << C_EVR_FPIN_BACKLEV_ENABLE);
  if (level_enable)
    fpctrl |= (1 << C_EVR_FPIN_BACKLEV_ENABLE);

  pEr->FPInMap[ttlin] = be32_to_cpu(fpctrl);
  if (pEr->FPInMap[ttlin] == be32_to_cpu(fpctrl))
    return 0;
  return -1;
}

int EvrSetExtEdgeSensitivity(volatile struct MrfErRegs *pEr, int ttlin, int edge)
{
  int fpctrl;

  if (ttlin < 0 || ttlin > EVR_MAX_FPIN_MAP)
    return -1;

  fpctrl = be32_to_cpu(pEr->FPInMap[ttlin]);
  fpctrl &= ~(1 << C_EVR_FPIN_EXT_EDGE);
  if (edge)
    fpctrl |= (1 << C_EVR_FPIN_EXT_EDGE);

  pEr->FPInMap[ttlin] = be32_to_cpu(fpctrl);
  if (pEr->FPInMap[ttlin] == be32_to_cpu(fpctrl))
    return 0;
  return -1;
}

int EvrSetExtLevelSensitivity(volatile struct MrfErRegs *pEr, int ttlin, int level)
{
  int fpctrl;

  if (ttlin < 0 || ttlin > EVR_MAX_FPIN_MAP)
    return -1;

  fpctrl = be32_to_cpu(pEr->FPInMap[ttlin]);
  fpctrl &= ~(1 << C_EVR_FPIN_EXTLEV_ACT);
  if (level)
    fpctrl |= (1 << C_EVR_FPIN_EXTLEV_ACT);

  pEr->FPInMap[ttlin] = be32_to_cpu(fpctrl);
  if (pEr->FPInMap[ttlin] == be32_to_cpu(fpctrl))
    return 0;
  return -1;
}

int EvrSetBackDBus(volatile struct MrfErRegs *pEr, int ttlin, int dbus)
{
  int fpctrl;

  if (ttlin < 0 || ttlin > EVR_MAX_FPIN_MAP)
    return -1;

  if (dbus < 0 || dbus > 255)
    return -1;

  fpctrl = be32_to_cpu(pEr->FPInMap[ttlin]);
  fpctrl &= ~(255 << C_EVR_FPIN_BACKDBUS_BASE);
  fpctrl |= dbus << C_EVR_FPIN_BACKDBUS_BASE;

  pEr->FPInMap[ttlin] = be32_to_cpu(fpctrl);
  if (pEr->FPInMap[ttlin] == be32_to_cpu(fpctrl))
    return 0;
  return -1;

}

int EvrSetTxDBufMode(volatile struct MrfErRegs *pEr, int enable)
{
  if (enable)
    pEr->TxDataBufControl = be32_to_cpu(1 << C_EVR_TXDATABUF_MODE);
  else
    pEr->TxDataBufControl = 0;

  return EvrGetTxDBufStatus(pEr);
}

int EvrGetTxDBufStatus(volatile struct MrfErRegs *pEr)
{
  return be32_to_cpu(pEr->TxDataBufControl);
}

int EvrSendTxDBuf(volatile struct MrfErRegs *pEr, char *dbuf, int size)
{
  int stat;

  stat = EvrGetTxDBufStatus(pEr);
  /* Check that DBUF mode enabled */
  if (!(stat & (1 << C_EVR_TXDATABUF_MODE)))
    return -1;
  /* Check that previous transfer is completed */
  if (!(stat & (1 << C_EVR_TXDATABUF_COMPLETE)))
    return -1;
  /* Check that size is valid */
  if (size & 3 || size > EVR_MAX_BUFFER || size < 4)
    return -1;

#ifdef __unix__
  memcpy((void *) &pEr->TxDatabuf[0], (void *) dbuf, size);
#else
  memcpy((void *) &pEr->TxDatabuf[0], (void *) dbuf, size);
#endif

  /* Enable and set size */
  stat &= ~((EVR_MAX_BUFFER-1) | (1 << C_EVR_TXDATABUF_TRIGGER));
  stat |= (1 << C_EVR_TXDATABUF_ENA) | size;
  pEr->TxDataBufControl = be32_to_cpu(stat);
  /*
  printf("TxDataBufControl %08x\n", pEr->TxDataBufControl);
  */
  /* Trigger */
  pEr->TxDataBufControl = be32_to_cpu(stat | (1 << C_EVR_TXDATABUF_TRIGGER));
  /*
  printf("TxDataBufControl %08x\n", pEr->TxDataBufControl);
  */
  return size;
}

int EvrGetTxSegBufStatus(volatile struct MrfErRegs *pEr)
{
  return be32_to_cpu(pEr->TxSegBufControl);
}

int EvrSendTxSegBuf(volatile struct MrfErRegs *pEr, char *dbuf, int segment, int size)
{
  int stat, dummy;

  stat = EvrGetTxSegBufStatus(pEr);
  /* Check that previous transfer is completed */
  if (!(stat & (1 << C_EVR_TXDATABUF_COMPLETE)))
    return -1;
  /* Check that segment is valid */
  if (segment < EVR_MIN_BUF_SEGMENT || segment > EVR_MAX_BUF_SEGMENT)
    return -1;
  /* Check that size is valid */
  if (size & 3 || size > EVR_MAX_BUFFER || size < 4)
    return -1;

#ifdef __unix__
  memcpy((void *) &pEr->TxSegBuf[segment*4], (void *) dbuf, size);
#else
  memcpy((void *) &pEr->TxSegBuf[segment*4], (void *) dbuf, size);
#endif

  /* Enable and set size */
  stat &= ~((EVR_MAX_BUF_SEGMENT << C_EVR_TXDATABUF_SEGSHIFT) | (EVR_MAX_BUFFER-1) | (1 << C_EVR_TXDATABUF_TRIGGER));
  stat |= (1 << C_EVR_TXDATABUF_ENA) | size;
  stat |= (segment << C_EVR_TXDATABUF_SEGSHIFT);
  pEr->TxSegBufControl = be32_to_cpu(stat);
  dummy = EvrGetTxSegBufStatus(pEr);

  /*
  printf("TxDataBufControl %08x\n", pEr->TxDataBufControl);
  printf("TxSegBufControl %08x\n", pEr->TxSegBufControl);
  */

  /* Trigger */
  pEr->TxSegBufControl = be32_to_cpu(stat | (1 << C_EVR_TXDATABUF_TRIGGER));
  dummy = EvrGetTxSegBufStatus(pEr);

  /*
  printf("TxDataBufControl %08x\n", pEr->TxDataBufControl);
  printf("TxSegBufControl %08x\n", pEr->TxSegBufControl);
  */

  return size;
}


int EvrGetFormFactor(volatile struct MrfErRegs *pEr)
{
  int stat;
  
  stat = be32_to_cpu(pEr->FPGAVersion);
  return ((stat >> 24) & 0x0f);
}

int EvrSetFineDelay(volatile struct MrfErRegs *pEr, int channel, int delay)
{
  if (channel < 0 || channel >= EVR_MAX_CML_OUTPUTS)
    return -1;

  pEr->FineDelay[channel] = be32_to_cpu(delay);
  return be32_to_cpu(pEr->FineDelay[channel]);
}

int EvrCMLEnable(volatile struct MrfErRegs *pEr, int channel, int state)
{
  int ctrl;

  if (channel < 0 || channel >= EVR_MAX_CML_OUTPUTS)
    return -1;

  ctrl = be16_to_cpu(pEr->CML[channel].Control);
  if (state)
    {
      ctrl &= ~((1 << C_EVR_CMLCTRL_RESET) | (1 << C_EVR_CMLCTRL_POWERDOWN));
      ctrl |= (1 << C_EVR_CMLCTRL_ENABLE);
    }
  else
    {
      ctrl |= (1 << C_EVR_CMLCTRL_RESET) | (1 << C_EVR_CMLCTRL_POWERDOWN);
      ctrl &= ~(1 << C_EVR_CMLCTRL_ENABLE);
    }


  pEr->CML[channel].Control = be16_to_cpu(ctrl);
  return be16_to_cpu(pEr->CML[channel].Control);
}

int EvrSetCMLMode(volatile struct MrfErRegs *pEr, int channel, int mode)
{
  int ctrl;

  if (channel < 0 || channel >= EVR_MAX_CML_OUTPUTS)
    return -1;

  ctrl = be16_to_cpu(pEr->CML[channel].Control);
  ctrl &= ~(C_EVR_CMLCTRL_MODE_GUNTX200 | C_EVR_CMLCTRL_MODE_GUNTX300 |
	    C_EVR_CMLCTRL_MODE_PATTERN);
  ctrl |= mode;

  pEr->CML[channel].Control = be16_to_cpu(ctrl);
  return be16_to_cpu(pEr->CML[channel].Control);
}

int EvrSetIntClkMode(volatile struct MrfErRegs *pEr, int enable)
{
  int ctrl;

  ctrl = be32_to_cpu(pEr->ClockControl);
  if (enable)
    ctrl |= (1 << C_EVR_CLKCTRL_INT_CLK_MODE);
  else
    ctrl &= ~(1 << C_EVR_CLKCTRL_INT_CLK_MODE);
  
  pEr->ClockControl = be32_to_cpu(ctrl);
  return (be32_to_cpu(pEr->ClockControl) & (1 << C_EVR_CLKCTRL_INT_CLK_MODE));
}

int EvrSetTargetDelay(volatile struct MrfErRegs *pEr, int delay)
{
  pEr->dc_target = be32_to_cpu(delay);
  return EvrGetTargetDelay(pEr);
}

int EvrGetTargetDelay(volatile struct MrfErRegs *pEr)
{
  return (be32_to_cpu(pEr->dc_target));
}

int EvrSWEventEnable(volatile struct MrfErRegs *pEr, int state)
{
  unsigned int mask = ~((1 << (C_EVR_SWEVENT_CODE_HIGH + 1)) -
    (1 << C_EVR_SWEVENT_CODE_LOW));
  int swe;

  swe = be32_to_cpu(pEr->SWEvent);
  if (state)
    pEr->SWEvent = be32_to_cpu(1 << C_EVR_SWEVENT_ENABLE | (swe & mask));
  else
    pEr->SWEvent = be32_to_cpu(~(1 << C_EVR_SWEVENT_ENABLE) & swe & mask);
  return EvrGetSWEventEnable(pEr);
}

int EvrGetSWEventEnable(volatile struct MrfErRegs *pEr)
{
  return be32_to_cpu(pEr->SWEvent & be32_to_cpu(1 << C_EVR_SWEVENT_ENABLE));
}

int EvrSendSWEvent(volatile struct MrfErRegs *pEr, int code)
{
  unsigned int mask = ~((1 << (C_EVR_SWEVENT_CODE_HIGH + 1)) -
    (1 << C_EVR_SWEVENT_CODE_LOW));
  int swcode;

  swcode = be32_to_cpu(pEr->SWEvent);
  swcode &= mask;
  swcode |= (code & EVR_MAX_EVENT_CODE);

  pEr->SWEvent = be32_to_cpu(swcode);

  return be32_to_cpu(pEr->SWEvent);
}

int EvrSetSeqRamEvent(volatile struct MrfErRegs *pEr, int ram, int pos, unsigned int timestamp, int code)
{
  if (ram < 0 || ram >= EVR_SEQRAMS)
    return -1;

  if (pos < 0 || pos >= EVR_MAX_SEQRAMEV)
    return -1;

  if (code < 0 || code > EVR_MAX_EVENT_CODE)
    return -1;

  pEr->SeqRam[ram][pos].Timestamp = be32_to_cpu(timestamp);
  pEr->SeqRam[ram][pos].EventCode = be32_to_cpu(code);

  return 0;
}

unsigned int EvrGetSeqRamTimestamp(volatile struct MrfErRegs *pEr, int ram, int pos)
{
  if (ram < 0 || ram >= EVR_SEQRAMS)
    return -1;

  if (pos < 0 || pos >= EVR_MAX_SEQRAMEV)
    return -1;

  return be32_to_cpu(pEr->SeqRam[ram][pos].Timestamp);
}

int EvrGetSeqRamEvent(volatile struct MrfErRegs *pEr, int ram, int pos)
{
  if (ram < 0 || ram >= EVR_SEQRAMS)
    return -1;

  if (pos < 0 || pos >= EVR_MAX_SEQRAMEV)
    return -1;

  return be32_to_cpu(pEr->SeqRam[ram][pos].EventCode);
}

void EvrSeqRamDump(volatile struct MrfErRegs *pEr, int ram)
{
  int pos;

  if (ram < 0 || ram >= EVR_SEQRAMS)
    return;
 
  for (pos = 0; pos < EVR_MAX_SEQRAMEV; pos++)
    if (pEr->SeqRam[ram][pos].EventCode)
      DEBUG_PRINTF("Ram%d: Timestamp %08x Code %02x\n",
		   ram,
		   be32_to_cpu(pEr->SeqRam[ram][pos].Timestamp),
		   be32_to_cpu(pEr->SeqRam[ram][pos].EventCode));
}

int EvrSeqRamControl(volatile struct MrfErRegs *pEr, int ram, int enable, int single, int recycle, int reset, int trigsel)
{
  int control;

  if (ram < 0 || ram >= EVR_SEQRAMS)
    return -1;

  control = be32_to_cpu(pEr->SeqRamControl[ram]);

  if (enable == 0)
    control |= (1 << C_EVR_SQRC_DISABLE);
  if (enable == 1)
    control |= (1 << C_EVR_SQRC_ENABLE);

  if (single == 0)
    control &= ~(1 << C_EVR_SQRC_SINGLE);
  if (single == 1)
    control |= (1 << C_EVR_SQRC_SINGLE);
  
  if (recycle == 0)
    control &= ~(1 << C_EVR_SQRC_RECYCLE);
  if (recycle == 1)
    control |= (1 << C_EVR_SQRC_RECYCLE);
  
  if (reset == 1)
    control |= (1 << C_EVR_SQRC_RESET);

  if (trigsel >= 0 && trigsel <= C_EVR_SEQTRIG_MAX)
    {
      control &= ~(C_EVR_SEQTRIG_MAX << C_EVR_SQRC_TRIGSEL_LOW);
      control |= trigsel << C_EVR_SQRC_TRIGSEL_LOW;
    }

  pEr->SeqRamControl[ram] = be32_to_cpu(control);

  return 0;
}
					  
int EvrSeqRamSWTrig(volatile struct MrfErRegs *pEr, int trig)
{
  if (trig < 0 || trig > 1)
    return -1;

  pEr->SeqRamControl[trig] |= be32_to_cpu(1 << C_EVR_SQRC_SWTRIGGER);
  
  return 0;
}

void EvrSeqRamStatus(volatile struct MrfErRegs *pEr, int ram)
{
  int control;

  if (ram < 0 || ram >= EVR_SEQRAMS)
    return;
  
  control = be32_to_cpu(pEr->SeqRamControl[ram]);
  
  DEBUG_PRINTF("RAM%d:", ram);
  if (control & (1 << C_EVR_SQRC_RUNNING))
    DEBUG_PRINTF(" RUN");
  if (control & (1 << C_EVR_SQRC_ENABLED))
    DEBUG_PRINTF(" ENA");
  if (control & (1 << C_EVR_SQRC_SINGLE))
    DEBUG_PRINTF(" SINGLE");
  if (control & (1 << C_EVR_SQRC_RECYCLE))
    DEBUG_PRINTF(" RECYCLE");
  DEBUG_PRINTF(" Trigsel %02x\n", (control >> C_EVR_SQRC_TRIGSEL_LOW) & C_EVR_SEQTRIG_MAX);
}
