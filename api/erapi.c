/**
@file erapi.c
@brief Functions for Micro-Research Event Receiver
       Application Programming Interface.
@author Jukka Pietarinen (MRF)
@date 12/8/2006
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
/** @private */
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

/**
Opens evr device and mmaps the register map into user space.
@param pEr Pointer to pointer of memory mapped MrfErRegs structure.
@param device_name Name of device e.g. /dev/era3. With EVM the two internal EVRs can be accessed with /dev/ega3.evrd and /dev/ega3.evru
@return Returns file descriptor of opened file, -1 on error.
*/
int EvrOpen(struct MrfErRegs **pEr, char *device_name)
{
  int fd;
  char *subdev;
  int offset = 0;

  subdev = strstr(device_name, ".evrd");
  if (subdev != NULL)
    {
      *subdev = 0; /* cut device name */
      offset = 0x20000;
    }
  else
    {
      subdev = strstr(device_name, ".evru");
      if (subdev != NULL)
	{
	  *subdev = 0; /* cut device name */
	  offset = 0x30000;
	}
    } 

  fd = EvrOpenWindow(pEr, device_name, EVR_CPCI300TG_MEM_WINDOW);
  if (fd == -1)
    {
      fd = EvrOpenWindow(pEr, device_name, EVR_MEM_WINDOW);
      if (fd == -1)
	{
	  fd = EvrOpenWindow(pEr, device_name, EVR_CPCI230_MEM_WINDOW);
	}
    }

  *pEr = (struct MrfErRegs *) ((void *) (*pEr) + offset); 
  
  if (fd != -1)
    {
      /* Put device in BE mode */
      (*pEr)->Control = ((*pEr)->Control) & ~0x02000002;
    }

  return fd;
}

/**
Opens evr device (cPCI-EVRTG-300) with larger register map and mmaps
the register map into user space.
@param pEr Pointer to pointer of memory mapped MrfErRegs structure.
@param device_name Name of device e.g. /dev/ertga3.
@return Returns file descriptor of opened file, -1 on error.
*/
int EvrTgOpen(struct MrfErRegs **pEr, char *device_name)
{
  int fd;

  fd = EvrOpenWindow(pEr, device_name, EVR_CPCI300TG_MEM_WINDOW);
  if (fd != -1)
    {
      /* Put device in BE mode */
      (*pEr)->Control = ((*pEr)->Control) & ~0x02000002;
    }
  return fd;
}

#else
int EvrOpen(struct MrfErRegs **pEg, char *device_name)
{
  return 0;
}
#endif

#ifdef __unix__
/** @private */
int EvrCloseWindow(int fd, int mem_window)
{
  int result;

  result = munmap(0, mem_window);
  return close(fd);
}

/**
Close evr device opened with EvrOpen.
@param fd File descriptor of evr device returned by EvrOpen.
@return Returns 0 on successful completion.
*/
int EvrClose(int fd)
{
  return EvrCloseWindow(fd, EVR_CPCI230_MEM_WINDOW);
}

/**
Close evr device opened with EvrTgOpen.
@param fd File descriptor of evr device returned by EvrTgOpen.
@return Returns 0 on successful completion.
*/
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

/**
Retrieve EVR firmware version.
@param pEr Pointer to MrfErRegs structure
@return Returns firmware version
*/
u32 EvrFWVersion(volatile struct MrfErRegs *pEr)
{
  return be32_to_cpu(pEr->FPGAVersion);
}

/**
Enable/disable EVR.
@param pEr Pointer to MrfErRegs structure
@param state 0 - disable, 1 - enable
@return Returns state read back from EVR.
*/
int EvrEnable(volatile struct MrfErRegs *pEr, int state)
{
  if (state)
    pEr->Control |= be32_to_cpu(1 << C_EVR_CTRL_MASTER_ENABLE);
  else
    pEr->Control &= be32_to_cpu(~(1 << C_EVR_CTRL_MASTER_ENABLE));
  
  return EvrGetEnable(pEr);
}

/**
Enable/disable EVR Delay Compensation. In delay compensation mode the EVR adjust its internal delay compensation FIFO so that the total system delay matches the set up target delay value.

When the EVR is not in delay compensation mode the FIFO depth is adjusted directly to match the target delay value. 

@param pEr Pointer to MrfErRegs structure
@param state 0 - disable, 1 - enable
@return Returns state read back from EVR.
*/
int EvrDCEnable(volatile struct MrfErRegs *pEr, int state)
{
  if (state)
    pEr->Control |= be32_to_cpu(1 << C_EVR_CTRL_DC_ENABLE);
  else
    pEr->Control &= be32_to_cpu(~(1 << C_EVR_CTRL_DC_ENABLE));
  
  return EvrGetDCEnable(pEr);
}

/**
Control output enable for external I/O box IFB-300.

@param pEr Pointer to MrfErRegs structure
@param state 0 - disable, 1 - enable
@return Returns state read back from EVR.
*/
int EvrOutputEnable(volatile struct MrfErRegs *pEr, int state)
{
  if (state)
    pEr->Control |= be32_to_cpu(1 << C_EVR_CTRL_OUTEN);
  else
    pEr->Control &= be32_to_cpu(~(1 << C_EVR_CTRL_OUTEN));

  return EvrGetEnable(pEr);
}

/**
Get EVR state.
@param pEr Pointer to MrfErRegs structure
@return Returns EVR state, 0 - disabled, non-zero - enabled.
*/
int EvrGetEnable(volatile struct MrfErRegs *pEr)
{
  return be32_to_cpu(pEr->Control & be32_to_cpu(1 << C_EVR_CTRL_MASTER_ENABLE));
}

/**
Get EVR delay compensation state.
@param pEr Pointer to MrfErRegs structure
@return Returns EVR delay compensation state, 0 - disabled, non-zero - enabled.
*/
int EvrGetDCEnable(volatile struct MrfErRegs *pEr)
{
  return be32_to_cpu(pEr->Control & be32_to_cpu(1 << C_EVR_CTRL_DC_ENABLE));
}

/**
Get/clear EVR link violation flag. The link violation flag is set when an event link receive error is detected. The EVR powers up with this flag set.
@param pEr Pointer to MrfErRegs structure
@param clear 0 - just read, do not try to clear, 1 - clear violation flag
@return Returns EVR link violation flag state, 0 - no violation, non-zero - violation detected.
*/
int EvrGetViolation(volatile struct MrfErRegs *pEr, int clear)
{
  int result;

  result = be32_to_cpu(pEr->IrqFlag & be32_to_cpu(1 << C_EVR_IRQFLAG_VIOLATION));
  if (clear && result)
    pEr->IrqFlag = be32_to_cpu(result);

  return result;
}

/**
Display EVR status.
@param pEr Pointer to MrfErRegs structure
*/
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

/**
Display EVR event mapping ram contents.
@param pEr Pointer to MrfErRegs structure
@param ram mapping ram number 0, 1.
*/
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

/**
Enable EVR event mapping ram.
@param pEr Pointer to MrfErRegs structure
@param ram mapping ram number 0 or 1.
@param enable 0 - disable, 1 - enable.
*/
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

/**
Set pulse generator action on received event code.

Each decoded event code can be programmed to trigger, set or clear any
pulse generator output. This function is used to set up mappings.

Please note that this function does not clear any existing mappings. To clear mapping see function EvrClearPulseMap().

@param pEr Pointer to MrfErRegs structure
@param ram Mapping RAM number 0 or 1.
@param code Event code to make changes to
@param trig Pulse generator number to trigger, -1 no action
@param set Pulse generator output number to set, -1 no action
@param clear Pulse genertor output number to clear, -1 no action
*/
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

/**
Set up event code forwarding.

This function sets up event code forwarding in a mapping RAM. To
enable/disable event code forwarding use function
EvrEnableEventForwarding().

@param pEr Pointer to MrfErRegs structure
@param ram Mapping RAM number 0 or 1.
@param code Event code to make changes to
@param enable 0 - disable event forwarding for event code, 1 - enable event forwarding for event code
*/
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

/**
Enable/disable event forwarding.

@param pEr Pointer to MrfErRegs structure
@param enable 0 - disable event forwarding, 1 - enable event forwarding
*/
int EvrEnableEventForwarding(volatile struct MrfErRegs *pEr, int enable)
{
  if (enable)
    pEr->Control |= be32_to_cpu(1 << C_EVR_CTRL_EVENT_FWD_ENA);
  else
    pEr->Control &= be32_to_cpu(~(1 << C_EVR_CTRL_EVENT_FWD_ENA));
  
  return EvrGetEventForwarding(pEr);
}

/**
Retrieve event forwarding state.

@param pEr Pointer to MrfErRegs structure
@return 0 - Event forwarding disabled, non-zero - Event forwarding enabled.
*/
int EvrGetEventForwarding(volatile struct MrfErRegs *pEr)
{
  return be32_to_cpu(pEr->Control & be32_to_cpu(1 << C_EVR_CTRL_EVENT_FWD_ENA));
}

/**
Set up event codes that flash the event LED.

This function sets up event codes in a mapping RAM that flash the event LED.

@param pEr Pointer to MrfErRegs structure
@param ram Mapping RAM number 0 or 1.
@param code Event code to make changes to
@param enable 0 - disable event LED flashing for event code, 1 - enable event LED flashing for event code
*/
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

/**
Set up event codes that get stored into the event FIFO.

This function sets up event codes in a mapping RAM that get stored into the event FIFO.

@param pEr Pointer to MrfErRegs structure
@param ram Mapping RAM number 0 or 1.
@param code Event code to store into the event FIFO
@param enable 0 - disable storing event code, 1 - enable storing event code
*/
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

/**
Set up event codes that get latch the timestamp.

This function sets up event codes in a mapping RAM that latch the timestamp into the timestamp latch register.

@param pEr Pointer to MrfErRegs structure
@param ram Mapping RAM number 0 or 1.
@param code Event code to latch timestamp
@param enable 0 - disable timestamp latching for event code, 1 - enable timestamp latching for event code
*/
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

/**
Set up event codes that get stored into the event log.

This function sets up event codes in a mapping RAM that get stored into the event log.

@param pEr Pointer to MrfErRegs structure
@param ram Mapping RAM number 0 or 1.
@param code Event code to store into the event log
@param enable 0 - disable storing event code, 1 - enable storing event code
*/
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

/**
Set up event codes that get stop the event log.

This function sets up event codes in a mapping RAM that get stop the event log.

@param pEr Pointer to MrfErRegs structure
@param ram Mapping RAM number 0 or 1.
@param code Event code to stop the event log
@param enable 0 - disable "stop log" for event code, 1 - enable "stop log" for event code
*/
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

/**
Clear the event FIFO.

@param pEr Pointer to MrfErRegs structure
*/
int EvrClearFIFO(volatile struct MrfErRegs *pEr)
{
  int ctrl;

  ctrl = be32_to_cpu(pEr->Control);
  ctrl |= (1 << C_EVR_CTRL_RESET_EVENTFIFO);
  pEr->Control = be32_to_cpu(ctrl);

  return be32_to_cpu(pEr->Control);
}

/**
Pull FIFOEvent entry from the event FIFO.

@param pEr Pointer to MrfErRegs structure
@param fe Pointer to FIFOEvent structure
@return 0 - on success, -1 on error (FIFO empty)
*/
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

/**
Enable/disable event log.

@param pEr Pointer to MrfErRegs structure
@param enable 0 - Disable logging of events, 1 - enable logging
*/
int EvrEnableLog(volatile struct MrfErRegs *pEr, int enable)
{
  if (enable)
    pEr->Control |= be32_to_cpu(1 << C_EVR_CTRL_LOG_ENABLE);
  else
    pEr->Control |= be32_to_cpu(1 << C_EVR_CTRL_LOG_DISABLE);
  
  return EvrGetLogState(pEr);
}

/**
Retrieve event log state.

@param pEr Pointer to MrfErRegs structure
@return 0 - event log disabled, 1 - event log enabled (logging events)
*/
int EvrGetLogState(volatile struct MrfErRegs *pEr)
{
  return be32_to_cpu(pEr->Status & be32_to_cpu(1 << C_EVR_STATUS_LOG_STOPPED));
}

/**
Retrieve event log start position in ring buffer.

@param pEr Pointer to MrfErRegs structure
@return Start position of event log (oldest entry in ring buffer)
*/
int EvrGetLogStart(volatile struct MrfErRegs *pEr)
{
  int pos;

  pos = be32_to_cpu(pEr->LogStatus);
  if (pos >= 0)
    return 0;
  else
    return (pos & (EVR_LOG_SIZE - 1));
}

/**
Retrieve number of entries in event log ring buffer.

@param pEr Pointer to MrfErRegs structure
@return Number of entries in event log. When return value equals length of log EVR_LOG_SIZE, the log may have rolled over and older events might be lost.
*/
int EvrGetLogEntries(volatile struct MrfErRegs *pEr)
{
  int pos;

  pos = be32_to_cpu(pEr->LogStatus);
  if (pos >= 0)
    return pos;
  else
    return EVR_LOG_SIZE;
}

/**
Enable "stop log" events.

@param pEr Pointer to MrfErRegs structure
@param enable 0 - disable stopping log by event codes that have the "stop log" bit set in the event mapping RAM, 1 - enable "stop log" event codes to stop the log.
*/
int EvrEnableLogStopEvent(volatile struct MrfErRegs *pEr, int enable)
{
  if (enable)
    pEr->Control |= be32_to_cpu(1 << C_EVR_CTRL_LOG_STOP_EV_EN);
  else
    pEr->Control &= be32_to_cpu(~(1 << C_EVR_CTRL_LOG_STOP_EV_EN));
  
  return EvrGetLogStopEvent(pEr);
}

/**
Retrieve state of "stop log" events.

@param pEr Pointer to MrfErRegs structure
@return 0 - log is not stopped by event codes that have the "stop log" bit set in the event mapping RAM, 1 - log gets stopped by "stop log" event codes.
*/
int EvrGetLogStopEvent(volatile struct MrfErRegs *pEr)
{
  return be32_to_cpu(pEr->Control & be32_to_cpu(1 << C_EVR_CTRL_LOG_STOP_EV_EN));
}

/**
Shows and purges the event FIFO contents.

@param pEr Pointer to MrfErRegs structure
*/
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

/**
Clears the event log.

@param pEr Pointer to MrfErRegs structure
*/
int EvrClearLog(volatile struct MrfErRegs *pEr)
{
  int ctrl;

  ctrl = be32_to_cpu(pEr->Control);
  ctrl |= (1 << C_EVR_CTRL_LOG_RESET);
  pEr->Control = be32_to_cpu(ctrl);

  return be32_to_cpu(pEr->Control);
}

/**
Shows the event log contents.

@param pEr Pointer to MrfErRegs structure
*/
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

/**
Clear pulse generator action on received event code.

@param pEr Pointer to MrfErRegs structure
@param ram Mapping RAM number 0 or 1.
@param code Event code
@param trig Pulse generator number to clear trigger mapping, -1 no action
@param set Pulse generator output number to reset set mapping, -1 no action
@param clear pulse genertor output number to reset clear mapping, -1 no action
*/
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

/**
Set up pulse generator time parameters.

@param pEr Pointer to MrfErRegs structure
@param pulse Number of pulse generator
@param presc Prescaler value
@param delay Pulse Delay value
@param width Pulse width value
*/
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

/**
Retrieve pulse generator prescaler value.

@param pEr Pointer to MrfErRegs structure
@param pulse Number of pulse generator
@return Pulse generator prescaler value.
*/
int EvrGetPulsePresc(volatile struct MrfErRegs *pEr, int pulse)
{
  if (pulse < 0 || pulse >= EVR_MAX_PULSES)
    return -1;

  return be32_to_cpu(pEr->Pulse[pulse].Prescaler);
}

/**
Retrieve pulse generator delay value.

@param pEr Pointer to MrfErRegs structure
@param pulse Number of pulse generator
@return Pulse generator delay value.
*/
int EvrGetPulseDelay(volatile struct MrfErRegs *pEr, int pulse)
{
  if (pulse < 0 || pulse >= EVR_MAX_PULSES)
    return -1;

  return be32_to_cpu(pEr->Pulse[pulse].Delay);
}

/**
Retrieve pulse generator width value.

@param pEr Pointer to MrfErRegs structure
@param pulse Number of pulse generator
@return Pulse generator width value.
*/
int EvrGetPulseWidth(volatile struct MrfErRegs *pEr, int pulse)
{
  if (pulse < 0 || pulse >= EVR_MAX_PULSES)
    return -1;

  return be32_to_cpu(pEr->Pulse[pulse].Width);
}

/**
Show pulse generator settings.

@param pEr Pointer to MrfErRegs structure
@param pulses Number of pulse generators
*/
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

/**
Set up pulse generator properties.

@param pEr Pointer to MrfErRegs structure
@param pulse Number of pulse generator
@param polarity 0 - positive pulse (active high), 1 - negative pulse (active low)
@param map_reset_ena 0 - disable flip-flop reset, 1 - enable flip-flop reset
@param map_set_ena 0 - disable flip-flop set, 1 - enable flip-flop set
@param map_trigger_ena 0 - disable triggering, 1 - enable triggering
@param enable 0 - disable pulse generator, 1 - enable pulse generator
*/
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

/**
Set up pulse generator hardware gates.

Pulse generators 28 through 31 have flip-flops only without
prescaler/delay/width counters. These flip-flop outputs can be used to
gate (mask/enable) pulse generator triggers.

As an example if we set up the mask field of pulse generator 0 to 0x01
and the enable field of pulse generator 1 to 0x01 we get pulse
generator 0 triggered only if the output of pulse generator 28 is
low. When the output of pulse generator 28 is low, pulse generator 1
gets triggered and pulse generator 0 not.

@param pEr Pointer to MrfErRegs structure
@param pulse Number of pulse generator
@param mask Bit mask for pulse generator hardware disable
@param enable Bit mask for pulse generator hardware enable
*/
int EvrSetPulseMask(volatile struct MrfErRegs *pEr, int pulse, int mask, int enable)
{
  int result;

  if (pulse < 0 || pulse >= EVR_MAX_PULSES)
    return -1;

  result = be32_to_cpu(pEr->Pulse[pulse].Control);

  result &= 0x0000ffff;
  result |= ((mask & 0x00ff) << 28);
  result |= ((enable & 0x00ff) << 20);

  pEr->Pulse[pulse].Control = be32_to_cpu(result);

  return 0;  
}

/**
Set up prescaler to trigger pulse generator.

Any of the prescalers can be set up to trigger any number of pulse generators.

@param pEr Pointer to MrfErRegs structure
@param prescaler Number of prescaler
@param trigs Bit mask of pulse generator triggers
*/
int EvrSetPrescalerTrig(volatile struct MrfErRegs *pEr, int prescaler, int trigs)
{
  int result;

  if (prescaler < 0 || prescaler >= EVR_MAX_PRESCALERS)
    return -1;

  pEr->PrescalerTrig[prescaler] = be32_to_cpu(trigs);
  return be32_to_cpu(pEr->PrescalerTrig[prescaler]);  
}

/**
Set up distributed bus signals to trigger pulse generator.

Any of the eight distibuted bus signals can be set up to trigger any number of pulse generators.

@param pEr Pointer to MrfErRegs structure
@param dbus Number of distributed bus bit
@param trigs Bit mask of pulse generator triggers
*/
int EvrSetDBusTrig(volatile struct MrfErRegs *pEr, int dbus, int trigs)
{
  int result;

  if (dbus < 0 || dbus >= 8)
    return -1;

  pEr->DBusTrig[dbus] = be32_to_cpu(trigs);
  return be32_to_cpu(pEr->DBusTrig[dbus]);  
}

/**
Set up output mapping for Universal Output.

Some form factors support dual outputs where two mappings are bitwise
or'ed together. In this case the map field consists of two bytes. If
only one output is to be mapped, set the upper byte to 0x3f.

<table>
<caption id="multi_row">Output mapping</caption>
<tr><th>ID<th>Signal
<tr><td>0x00<td>Pulse generator 0
<tr><td>0x01<td>Pulse generator 1
<tr><td>...<td>...
<tr><td>0x20<td>Distributed bus bit 0
<tr><td>0x21<td>Distributed bus bit 1
<tr><td>...<td>...
<tr><td>0x28<td>Prescaler 0
<tr><td>0x29<td>Prescaler 1
<tr><td>...<td>...
<tr><td>61<td>High-Impedance (only on I/O pins)
<tr><td>62<td>High 1
<tr><td>63<td>Low 0
</table>

@param pEr Pointer to MrfErRegs structure
@param output Number of Universal Output
@param map Output map
*/
int EvrSetUnivOutMap(volatile struct MrfErRegs *pEr, int output, int map)
{
  if (output < 0 || output >= EVR_MAX_UNIVOUT_MAP)
    return -1;

  pEr->UnivOutMap[output] = be16_to_cpu(map);

  return be16_to_cpu(pEr->UnivOutMap[output]);
}

/**
Retrieve output mapping for Universal Output.

Please see EvrSetUnivOutMap() for details about the mapping.

@param pEr Pointer to MrfErRegs structure
@param output Number of Universal Output
@return Output map for output
*/
int EvrGetUnivOutMap(volatile struct MrfErRegs *pEr, int output)
{
  if (output < 0 || output >= EVR_MAX_UNIVOUT_MAP)
    return -1;

  return be16_to_cpu(pEr->UnivOutMap[output]);
}

/**
Show output mapping for Universal Outputs.

Please see EvrSetUnivOutMap() for details about the mapping IDs.

@param pEr Pointer to MrfErRegs structure
@param output Number of outputs
*/
void EvrDumpUnivOutMap(volatile struct MrfErRegs *pEr, int outputs)
{
  int i;

  for (i = 0; i < outputs; i++)
    DEBUG_PRINTF("UnivOut[%d] %02x\n", i, be16_to_cpu(pEr->UnivOutMap[i]));
}

/**
Set up output mapping for Front panel outputs.

Please see EvrSetUnivOutMap() for details about the mapping.

@param pEr Pointer to MrfErRegs structure
@param output Number of output
@param map Output map
*/
int EvrSetFPOutMap(volatile struct MrfErRegs *pEr, int output, int map)
{
  if (output < 0 || output >= EVR_MAX_FPOUT_MAP)
    return -1;

  pEr->FPOutMap[output] = be16_to_cpu(map);

  return be16_to_cpu(pEr->FPOutMap[output]);
}

/**
Retrieve output mapping for Front panel outputs.

Please see EvrSetUnivOutMap() for details about the mapping.

@param pEr Pointer to MrfErRegs structure
@param output Number of Universal Output
@return Output map for output
*/
int EvrGetFPOutMap(volatile struct MrfErRegs *pEr, int output)
{
  if (output < 0 || output >= EVR_MAX_FPOUT_MAP)
    return -1;

  return be16_to_cpu(pEr->FPOutMap[output]);
}

/**
Show output mapping for Front panel outputs.

Please see EvrSetUnivOutMap() for details about the mapping IDs.

@param pEr Pointer to MrfErRegs structure
@param output Number of outputs
*/
void EvrDumpFPOutMap(volatile struct MrfErRegs *pEr, int outputs)
{
  int i;

  for (i = 0; i < outputs; i++)
    DEBUG_PRINTF("FPOut[%d] %02x\n", i, be16_to_cpu(pEr->FPOutMap[i]));
}

/**
Set up output mapping for Transition board outputs.

Please see EvrSetUnivOutMap() for details about the mapping.

@param pEr Pointer to MrfErRegs structure
@param output Number of output
@param map Output map
*/
int EvrSetTBOutMap(volatile struct MrfErRegs *pEr, int output, int map)
{
  if (output < 0 || output >= EVR_MAX_TBOUT_MAP)
    return -1;

  pEr->TBOutMap[output] = be16_to_cpu(map);

  return be16_to_cpu(pEr->TBOutMap[output]);
}

/**
Retrieve output mapping for Transition board outputs.

Please see EvrSetUnivOutMap() for details about the mapping.

@param pEr Pointer to MrfErRegs structure
@param output Number of Universal Output
@return Output map for output
*/
int EvrGetTBOutMap(volatile struct MrfErRegs *pEr, int output)
{
  if (output < 0 || output >= EVR_MAX_TBOUT_MAP)
    return -1;

  return be16_to_cpu(pEr->TBOutMap[output]);
}

/**
Show output mapping for Transition board outputs.

Please see EvrSetUnivOutMap() for details about the mapping IDs.

@param pEr Pointer to MrfErRegs structure
@param output Number of outputs
*/
void EvrDumpTBOutMap(volatile struct MrfErRegs *pEr, int outputs)
{
  int i;

  for (i = 0; i < outputs; i++)
    DEBUG_PRINTF("TBOut[%d] %02x\n", i, be16_to_cpu(pEr->TBOutMap[i]));
}

/**
Set up output mapping for Backplane outputs.

Please see EvrSetUnivOutMap() for details about the mapping.

@param pEr Pointer to MrfErRegs structure
@param output Number of output
@param map Output map
*/
int EvrSetBPOutMap(volatile struct MrfErRegs *pEr, int output, int map)
{
  if (output < 0 || output >= EVR_MAX_BPOUT_MAP)
    return -1;

  pEr->BPOutMap[output] = be16_to_cpu(map);

  return be16_to_cpu(pEr->BPOutMap[output]);
}

/**
Retrieve output mapping for Backplane outputs.

Please see EvrSetUnivOutMap() for details about the mapping.

@param pEr Pointer to MrfErRegs structure
@param output Number of Universal Output
@return Output map for output
*/
int EvrGetBPOutMap(volatile struct MrfErRegs *pEr, int output)
{
  if (output < 0 || output >= EVR_MAX_BPOUT_MAP)
    return -1;

  return be16_to_cpu(pEr->BPOutMap[output]);
}

/**
Show output mapping for Backplane outputs.

Please see EvrSetUnivOutMap() for details about the mapping IDs.

@param pEr Pointer to MrfErRegs structure
@param output Number of outputs
*/
void EvrDumpBPOutMap(volatile struct MrfErRegs *pEr, int outputs)
{
  int i;

  for (i = 0; i < outputs; i++)
    DEBUG_PRINTF("BPOut[%d] %02x\n", i, be16_to_cpu(pEr->BPOutMap[i]));
}

#ifdef __unix__
/**
Assign user space interrupt handler for EVR interrupts.

@param pEr Pointer to MrfErRegs structure
@param fd File descriptor of EVR device
@param handler Pointer to user space interrupt handler
*/
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

/**
@private
*/
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

/**
Enable interrupts for EVR.

@param pEr Pointer to MrfErRegs structure
@param mask Interrupt mask

<table>
<caption id="multi_row">Interrupt mask</caption>
<tr><th>Bit<th>Function
<tr><td>0<td>Receiver violation interrupt
<tr><td>1<td>Event FIFO full interrupt
<tr><td>2<td>Hearbeat interrupt
<tr><td>3<td>Event interrupt
<tr><td>4<td>Hardware interrupt
<tr><td>5<td>Data buffer interrupt
<tr><td>6<td>Link state change interrupt
<tr><td>7<td>Segmented data buffer interrupt
<tr><td>8<td>Sequence RAM start interrupt
<tr><td>12<td>Sequence RAM stop interrupt
<tr><td>16<td>Sequence RAM halfway through interrupt
<tr><td>20<td>Sequence RAM roll over interrupt
</table>
*/
int EvrIrqEnable(volatile struct MrfErRegs *pEr, int mask)
{
  int control = be32_to_cpu(pEr->IrqEnable) & EVR_IRQ_PCICORE_ENABLE;

  pEr->IrqEnable = be32_to_cpu(mask | control);
  return be32_to_cpu(pEr->IrqEnable);
}

/**
Get interrupt enable state for EVR.

@param pEr Pointer to MrfErRegs structure
@return Interrupt enable mask

Please see function EvrIrqEnable() for mask bit definitions.
*/
int EvrGetIrqEnable(volatile struct MrfErRegs *pEr)
{
  return be32_to_cpu(pEr->IrqEnable);
}

/**
Get interrupt flags for EVR.

@param pEr Pointer to MrfErRegs structure
@return Interrupt flags

Please see function EvrIrqEnable() for flag bit definitions.
*/
int EvrGetIrqFlags(volatile struct MrfErRegs *pEr)
{
  return be32_to_cpu(pEr->IrqFlag);
}

/**
Clear interrupt flags for EVR.

@param pEr Pointer to MrfErRegs structure
@param mask Bit mask to clear interrupts
@return Interrupt flags

Please see function EvrIrqEnable() for interrupt bit definitions.
*/
int EvrClearIrqFlags(volatile struct MrfErRegs *pEr, int mask)
{
  pEr->IrqFlag = be32_to_cpu(mask);
  return be32_to_cpu(pEr->IrqFlag);
}

#ifdef __unix__
/**
Function to call when interrupt handler exits.

@param fd File descriptor of EVR device opened.
*/
void EvrIrqHandled(int fd)
{
  ioctl(fd, EV_IOCIRQEN);
}
#endif

/**
Set up map for hardware interrupt.

@param pEr Pointer to MrfErRegs structure
@param map Output map for hardware interrupt.

Please see EvrSetUnivOutMap() for details about the mapping.
*/
int EvrSetPulseIrqMap(volatile struct MrfErRegs *pEr, int map)
{
  pEr->PulseIrqMap = be32_to_cpu(map);

  return be32_to_cpu(pEr->PulseIrqMap);
}

/** @private */
void EvrClearDiagCounters(volatile struct MrfErRegs *pEr)
{
  pEr->DiagReset = 0xffffffff;
  pEr->DiagReset = 0x0;
}

/** @private */
int EvrEnableDiagCounters(volatile struct MrfErRegs *pEr, int enable)
{
  if (enable)
    pEr->DiagCE = 0xffffffff;
  else
    pEr->DiagCE = 0;

  return be32_to_cpu(pEr->DiagCE);
}

/** @private */
u32 EvrGetDiagCounter(volatile struct MrfErRegs *pEr, int idx)
{
  return be32_to_cpu(pEr->DiagCounter[idx]);
}

/** @private */
int EvrSetGPIODir(volatile struct MrfErRegs *pEr, int dir)
{
  pEr->GPIODir = be32_to_cpu(dir);
  return be32_to_cpu(pEr->GPIODir);
}

/** @private */
int EvrSetGPIOOut(volatile struct MrfErRegs *pEr, int dout)
{
  pEr->GPIOOut = be32_to_cpu(dout);
  return be32_to_cpu(pEr->GPIOOut);
}

/** @private */
int EvrGetGPIOIn(volatile struct MrfErRegs *pEr)
{
  return be32_to_cpu(pEr->GPIOIn);
}

/**
Enable output on Universal I/O delay modules (-DLY)

@param pEr Pointer to MrfErRegs structure
@param dlymod Number od delay module
@param enable 0 - disable, 1 - enable
*/
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

/**
Set delay on Universal I/O delay modules (-DLY)

@param pEr Pointer to MrfErRegs structure
@param dlymod Number od delay module
@param dly0 Delay of first output 0 - 1023
@param dly1 Delay of seconds output 0 - 1023
*/
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

/** @private */
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

/**
Set up fractional synthesizer that generates reference clock for event clock

@param pEr Pointer to MrfErRegs structure
@param fracdiv Control word

The control word can be generated from a reference frequency by using function freq_to_cw().
*/
int EvrSetFracDiv(volatile struct MrfErRegs *pEr, int fracdiv)
{
  pEr->UsecDiv = be32_to_cpu((int) cw_to_freq(fracdiv));

  return be32_to_cpu(pEr->FracDiv = be32_to_cpu(fracdiv));
}

/**
Get fractional synthesizer control word

@param pEr Pointer to MrfErRegs structure
@return fracdiv Control word

Use function cw_to_freq() to convert the control word to frequency.
*/
int EvrGetFracDiv(volatile struct MrfErRegs *pEr)
{
  return be32_to_cpu(pEr->FracDiv);
}

/**
Set databuffer mode.

@param pEr Pointer to MrfErRegs structure
@param enable 0 - disable, 1 - enable
*/
int EvrSetDBufMode(volatile struct MrfErRegs *pEr, int enable)
{
  if (enable)
    pEr->DataBufControl = be32_to_cpu(1 << C_EVR_DATABUF_MODE);
  else
    pEr->DataBufControl = 0;

  return EvrGetDBufStatus(pEr);
}

/**
Get databuffer mode status.

@param pEr Pointer to MrfErRegs structure
@return 0 - disabled, non-zero - enabled
*/
int EvrGetDBufStatus(volatile struct MrfErRegs *pEr)
{
  volatile u32 *dbc = &(pEr->DataBufControl);

  return be32_to_cpu(*dbc);
}

/**
Arm/disarm data buffer.

The data buffer has to be armed before reception is possible. Upon
reception the data buffer is automatically disarmed and only the first
buffer is received.

@param pEr Pointer to MrfErRegs structure
@param enable 0 - disarm, 1 - arm
*/
int EvrReceiveDBuf(volatile struct MrfErRegs *pEr, int enable)
{
  if (enable)
    pEr->DataBufControl |= be32_to_cpu(1 << C_EVR_DATABUF_LOAD);
  else
    pEr->DataBufControl |= be32_to_cpu(1 << C_EVR_DATABUF_STOP);

  return EvrGetDBufStatus(pEr);
}

/**
Retrieve received data buffer.

@param pEr Pointer to MrfErRegs structure
@param dbuf Pointer to memory to copy data buffer to
@param size Size of buffer pointed to by dbuf
@return -1 on error, number of bytes copied on success.
*/
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

/**
Get segmented data buffer segment receive status.

@param pEr Pointer to MrfErRegs structure
@param segment Number of starting segment
@return -1 on error, 0 if segment not received, 1 if segment received
*/
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

/**
Get segmented data buffer segment overflow flag.

@param pEr Pointer to MrfErRegs structure
@param segment Number of starting segment
@return -1 on error, 0 no overflow, 1 segment overwritten
*/
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

/**
Get segmented data buffer segment checksum error flag.

@param pEr Pointer to MrfErRegs structure
@param segment Number of starting segment
@return -1 on error, 0 no error, 1 checksum error in segment
*/
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

/**
Clear segmented data buffer segment flags.

@param pEr Pointer to MrfErRegs structure
@param segment Number of starting segment to clear
*/
void EvrClearSegFlag(volatile struct MrfErRegs *pEr, int segment)
{
  u32 *rx_flag;

  if (segment < 0 || segment > 255)
    return;

  rx_flag = (u32 *) &(pEr->SegRXReg[segment/32]);
  
  *rx_flag = be32_to_cpu((0x80000000 >> (segment % 32)));
}

/**
Retrieve received segmented data buffer.

@param pEr Pointer to MrfErRegs structure
@param dbuf Pointer to memory to copy data buffer to
@param segment number of segment to retrieve
@return -1 on error, number of bytes copied on success.
*/
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

/**
Set local timestamp divider prescaler.

The timestamp event counter is incremented from the local counter or when the counter value is zero, either from the distributed bus or received timestamp event 0x7c. Please see function EvrSetTimestampDBus().

@param pEr Pointer to MrfErRegs structure
@param div Prescaler value.
*/
int EvrSetTimestampDivider(volatile struct MrfErRegs *pEr, int div)
{
  pEr->EvCntPresc = be32_to_cpu(div);

  return be32_to_cpu(pEr->EvCntPresc);
}

/**
Get timestamp event counter value.

@param pEr Pointer to MrfErRegs structure
@return Timestamp counter value.
*/
int EvrGetTimestampCounter(volatile struct MrfErRegs *pEr)
{
  return be32_to_cpu(pEr->TimestampEventCounter);
}

/**
Get timestamp seconds counter value.

@param pEr Pointer to MrfErRegs structure
@return Timestamp secnods counter value.
*/
int EvrGetSecondsCounter(volatile struct MrfErRegs *pEr)
{
  return be32_to_cpu(pEr->SecondsCounter);
}

/**
Get timestamp latch value (latched from timestamp event counter).

@param pEr Pointer to MrfErRegs structure
@return Timestamp latch value.
*/
int EvrGetTimestampLatch(volatile struct MrfErRegs *pEr)
{
  return be32_to_cpu(pEr->TimestampLatch);
}

/**
Get timestamp seconds latch value (latched from timestamp seconds register).

@param pEr Pointer to MrfErRegs structure
@return Timestamp seconds latch value.
*/
int EvrGetSecondsLatch(volatile struct MrfErRegs *pEr)
{
  return be32_to_cpu(pEr->SecondsLatch);
}

/**
Enable/disable timestamp counter running from distributed bus bit 4.
Please note that also the timestamp event counter has to be cleared to
0. Please see function EvrSetTimestampDivider().

@param pEr Pointer to MrfErRegs structure
@param enable 0 - use timestamp event 0x7c, 1 - use distributed bus bit 4.
*/
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

/**
Set up prescaler.

@param pEr Pointer to MrfErRegs structure
@param presc Number of prescaler
@param div Prescaler divider
*/
int EvrSetPrescaler(volatile struct MrfErRegs *pEr, int presc, int div)
{
  if (presc >= 0 && presc < EVR_MAX_PRESCALERS)
    {
      pEr->Prescaler[presc] = be32_to_cpu(div);

      return be32_to_cpu(pEr->Prescaler[presc]);
    }
  return -1;
}

/**
Get prescaler divider.

@param pEr Pointer to MrfErRegs structure
@param presc Number of prescaler
@return Prescaler divider
*/
int EvrGetPrescaler(volatile struct MrfErRegs *pEr, int presc)
{
  if (presc >= 0 && presc < EVR_MAX_PRESCALERS)
    {
      return be32_to_cpu(pEr->Prescaler[presc]);
    }
  return -1;
}

/**
Set up prescaler polarity.

@param pEr Pointer to MrfErRegs structure
@param presc Number of prescaler
@param polarity 0 - falling edge aligned on reset prescalers event, 1 - rising edge aligned on reset prescalers event
*/
int EvrSetPrescalerPolarity(volatile struct MrfErRegs *pEr, int polarity)
{
  if (polarity)
    pEr->Control |= be32_to_cpu(1 << C_EVR_CTRL_PRESC_POLARITY);
  else
    pEr->Control &= be32_to_cpu(~(1 << C_EVR_CTRL_PRESC_POLARITY));
  
  return be32_to_cpu(pEr->Control & (1 << C_EVR_CTRL_PRESC_POLARITY));
}

/**
Set up prescaler phase.

@param pEr Pointer to MrfErRegs structure
@param presc Number of prescaler
@param phase Phase offset (negative delay)
*/
int EvrSetPrescalerPhase(volatile struct MrfErRegs *pEr, int presc, int phase)
{
  if (presc >= 0 && presc < EVR_MAX_PRESCALERS)
    {
      pEr->PrescalerPhase[presc] = be32_to_cpu(phase);

      return be32_to_cpu(pEr->PrescalerPhase[presc]);
    }
  return -1;
}

/**
Set up external event.

External events are locally generated events that are inserted in the
received event stream.

<table>
<caption id="ext_event_input">Input mapping</caption>
<tr><th>Input number<th>Hardware input
<tr><td>0<td>Front panel TTL input 0
<tr><td>1<td>Front panel TTL input 1
<tr><td>...<td>...
<tr><td>4<td>Universal Input 0
<tr><td>5<td>Universal Input 1
<tr><td>...<td>...
<tr><td>24<td>Backplane Input 0
<tr><td>25<td>Backplane Input 1
<tr><td>...<td>... 
</table>

@param pEr Pointer to MrfErRegs structure
@param input Number of input, see table \ref ext_event_input
@param code Event code to generate
@param edge_enable 0 - edge sensitivity disabled, 1 - edge sensitivity enabled
@param level_enable 0 - level sensiticity disabled, 1 - level sensitivity enabled
*/
int EvrSetExtEvent(volatile struct MrfErRegs *pEr, int input, int code, int edge_enable, int level_enable)
{
  int fpctrl;

  if (input < 0 || input > EVR_MAX_EXTIN_MAP)
    return -1;

  fpctrl = be32_to_cpu(pEr->ExtinMap[input]);
  if (code >= 0 && code <= EVR_MAX_EVENT_CODE)
    {
      fpctrl &= ~(EVR_MAX_EVENT_CODE << C_EVR_EXTIN_EXTEVENT_BASE);
      fpctrl |= code << C_EVR_EXTIN_EXTEVENT_BASE;
    }
  fpctrl &= ~(1 << C_EVR_EXTIN_EXT_ENABLE);
  if (edge_enable)
    fpctrl |= (1 << C_EVR_EXTIN_EXT_ENABLE);

  fpctrl &= ~(1 << C_EVR_EXTIN_EXTLEV_ENABLE);
  if (level_enable)
    fpctrl |= (1 << C_EVR_EXTIN_EXTLEV_ENABLE);

  pEr->ExtinMap[input] = be32_to_cpu(fpctrl);
  if (pEr->ExtinMap[input] == be32_to_cpu(fpctrl))
    return 0;
  return -1;
}

/**
Get external event code for input.

@param pEr Pointer to MrfErRegs structure
@param input Number of input, see table \ref ext_event_input
@return Event code assigned to input.
*/
int EvrGetExtEventCode(volatile struct MrfErRegs *pEr, int input)
{
  int fpctrl;

  if (input < 0 || input > EVR_MAX_EXTIN_MAP)
    return -1;

  fpctrl = be32_to_cpu(pEr->ExtinMap[input]);
  return (fpctrl >> C_EVR_EXTIN_EXTEVENT_BASE) & EVR_MAX_EVENT_CODE;
}

/**
Set up backward event.

Backward events are locally generated events that are sent out through
the SFP TX port.

@param pEr Pointer to MrfErRegs structure
@param input Number of input, see table \ref ext_event_input
@param code Event code to generate
@param edge_enable 0 - edge sensitivity disabled, 1 - edge sensitivity enabled
@param level_enable 0 - level sensiticity disabled, 1 - level sensitivity enabled
*/
int EvrSetBackEvent(volatile struct MrfErRegs *pEr, int input, int code, int edge_enable, int level_enable)
{
  int fpctrl;

  if (input < 0 || input > EVR_MAX_EXTIN_MAP)
    return -1;

  fpctrl = be32_to_cpu(pEr->ExtinMap[input]);
  if (code >= 0 && code <= EVR_MAX_EVENT_CODE)
    {
      fpctrl &= ~(EVR_MAX_EVENT_CODE << C_EVR_EXTIN_BACKEVENT_BASE);
      fpctrl |= code << C_EVR_EXTIN_BACKEVENT_BASE;
    }
  fpctrl &= ~(1 << C_EVR_EXTIN_BACKEV_ENABLE);
  if (edge_enable)
    fpctrl |= (1 << C_EVR_EXTIN_BACKEV_ENABLE);

  fpctrl &= ~(1 << C_EVR_EXTIN_BACKLEV_ENABLE);
  if (level_enable)
    fpctrl |= (1 << C_EVR_EXTIN_BACKLEV_ENABLE);

  pEr->ExtinMap[input] = be32_to_cpu(fpctrl);
  if (pEr->ExtinMap[input] == be32_to_cpu(fpctrl))
    return 0;
  return -1;
}

/**
Set up external event edge sensitivity.

@param pEr Pointer to MrfErRegs structure
@param input Number of input, see table \ref ext_event_input
@param edge 0 - trigger event on rising edge, 1 - trigger event on falling edge
*/
int EvrSetExtEdgeSensitivity(volatile struct MrfErRegs *pEr, int input, int edge)
{
  int fpctrl;

  if (input < 0 || input > EVR_MAX_EXTIN_MAP)
    return -1;

  fpctrl = be32_to_cpu(pEr->ExtinMap[input]);
  fpctrl &= ~(1 << C_EVR_EXTIN_EXT_EDGE);
  if (edge)
    fpctrl |= (1 << C_EVR_EXTIN_EXT_EDGE);

  pEr->ExtinMap[input] = be32_to_cpu(fpctrl);
  if (pEr->ExtinMap[input] == be32_to_cpu(fpctrl))
    return 0;
  return -1;
}

/**
Set up external event level sensitivity.

@param pEr Pointer to MrfErRegs structure
@param input Number of input, see table \ref ext_event_input
@param level 0 - send events when input high, 1 - send events when input low
*/
int EvrSetExtLevelSensitivity(volatile struct MrfErRegs *pEr, int input, int level)
{
  int fpctrl;

  if (input < 0 || input > EVR_MAX_EXTIN_MAP)
    return -1;

  fpctrl = be32_to_cpu(pEr->ExtinMap[input]);
  fpctrl &= ~(1 << C_EVR_EXTIN_EXTLEV_ACT);
  if (level)
    fpctrl |= (1 << C_EVR_EXTIN_EXTLEV_ACT);

  pEr->ExtinMap[input] = be32_to_cpu(fpctrl);
  if (pEr->ExtinMap[input] == be32_to_cpu(fpctrl))
    return 0;
  return -1;
}

/**
Get external input state.

@param pEr Pointer to MrfErRegs structure
@param input Number of input, see table \ref ext_event_input
@return State of input, 0 - input low, 1 - input high
*/
int EvrGetExtInStatus(volatile struct MrfErRegs *pEr, int extin)
{
  int fpctrl;

  if (extin < 0 || extin > EVR_MAX_EXTIN_MAP)
    return -1;

  fpctrl = be32_to_cpu(pEr->ExtinMap[extin]);

  if (fpctrl & (1 << C_EVR_EXTIN_STATUS))
    return 1;
  else
    return 0;
}

/**
Set up external input to backward distributed bus mapping.

The backward distributed bus is sent out through the SFP TX port. Any
external input can be mapped to any backward distributed bus bit.

@param pEr Pointer to MrfErRegs structure
@param input Number of input, see table \ref ext_event_input
@param dbus Distributed bus mask to map input to
*/
int EvrSetBackDBus(volatile struct MrfErRegs *pEr, int input, int dbus)
{
  int fpctrl;

  if (input < 0 || input > EVR_MAX_EXTIN_MAP)
    return -1;

  if (dbus < 0 || dbus > 255)
    return -1;

  fpctrl = be32_to_cpu(pEr->ExtinMap[input]);
  fpctrl &= ~(255 << C_EVR_EXTIN_BACKDBUS_BASE);
  fpctrl |= dbus << C_EVR_EXTIN_BACKDBUS_BASE;

  pEr->ExtinMap[input] = be32_to_cpu(fpctrl);
  if (pEr->ExtinMap[input] == be32_to_cpu(fpctrl))
    return 0;
  return -1;

}

/** @private */
int EvrSetTxDBufMode(volatile struct MrfErRegs *pEr, int enable)
{
  if (enable)
    pEr->TxDataBufControl = be32_to_cpu(1 << C_EVR_TXDATABUF_MODE);
  else
    pEr->TxDataBufControl = 0;

  return EvrGetTxDBufStatus(pEr);
}

/** @private */
int EvrGetTxDBufStatus(volatile struct MrfErRegs *pEr)
{
  return be32_to_cpu(pEr->TxDataBufControl);
}

/**
Send data buffer through SFP TX port.

@param pEr Pointer to MrfErRegs structure
@param dbuf Pointer to data buffer to send
@param size Number of bytes to send (4 to 2048)
@return Returns -1 on error, number of bytes sent on success.

The function does not wait for the transmission to be completed. If the
previous transfer is still in progress the function returns -1.
*/
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

/**
Get segmented data buffer transmitter control register.

@param pEr Pointer to MrfErRegs structure
@return Segmented data buffer transmitter control register.
*/
int EvrGetTxSegBufStatus(volatile struct MrfErRegs *pEr)
{
  return be32_to_cpu(pEr->TxSegBufControl);
}

/**
Send segmented data buffer through SFP TX port.

@param pEr Pointer to MrfErRegs structure
@param dbuf Pointer to data buffer to send
@param segment Starting segment number
@param size Number of bytes to send (4 to max. 2048 depending on segment number)
@return Returns -1 on error, number of bytes sent on success.

The function does not wait for the transmission to be completed. If the
previous transfer is still in progress the function returns -1.
*/
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

/**
Retrieve form factor of EVR device.

@param pEr Pointer to MrfErRegs structure
@return Form Factor
<table>
<caption id="form_factor">Form factor</caption>
<tr><th>ID<th>Form Factor
<tr><td>0<td>CompactPCI 3U
<tr><td>1<td>PMC
<tr><td>2<td>VME64x
<tr><td>4<td>CompactPCI 6U
<tr><td>6<td>PXIe 3U
<tr><td>7<td>PCIe
<tr><td>8<td>mTCA.4
</table>
*/
int EvrGetFormFactor(volatile struct MrfErRegs *pEr)
{
  int stat;
  
  stat = be32_to_cpu(pEr->FPGAVersion);
  return ((stat >> 24) & 0x0f);
}

/** @private */
int EvrSetFineDelay(volatile struct MrfErRegs *pEr, int channel, int delay)
{
  if (channel < 0 || channel >= EVR_MAX_CML_OUTPUTS)
    return -1;

  pEr->FineDelay[channel] = be32_to_cpu(delay);
  return be32_to_cpu(pEr->FineDelay[channel]);
}

/** @private */
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

/** @private */
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

/**
Select event clock source.

@param pEr Pointer to MrfErRegs structure
@param enable 0 - Event clock locked to EVG, 1 - Event clock locally generated (stand-alone mode)
*/
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

/**
Set target delay. In delay compensation mode the target delay is the total system delay, in non-DC mode the target delay is the depth of the delay compensation FIFO.

@param pEr Pointer to MrfErRegs structure
@param delay Target delay value is a 32 bit fixed point number with the point in the middle of two 16 bit words. The value represent the delay in event clock cycles.
*/
int EvrSetTargetDelay(volatile struct MrfErRegs *pEr, int delay)
{
  pEr->dc_target = be32_to_cpu(delay);
  return EvrGetTargetDelay(pEr);
}

/**
Get target delay. In delay compensation mode the target delay is the total system delay, in non-DC mode the target delay is the depth of the delay compensation FIFO.

@param pEr Pointer to MrfErRegs structure
@return Target delay value is a 32 bit fixed point number with the point in the middle of two 16 bit words. The value represent the delay in event clock cycles.
*/
int EvrGetTargetDelay(volatile struct MrfErRegs *pEr)
{
  return (be32_to_cpu(pEr->dc_target));
}

/**
Enable local software events.

@param pEr Pointer to MrfErRegs structure
@param state 0 - disable, 1 - enable software events
*/
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

/**
Get local software event state.

@param pEr Pointer to MrfErRegs structure
@return 0 - software events disabled, non-zero - software events enabled
*/
int EvrGetSWEventEnable(volatile struct MrfErRegs *pEr)
{
  return be32_to_cpu(pEr->SWEvent & be32_to_cpu(1 << C_EVR_SWEVENT_ENABLE));
}

/**
Send local software event. This function inserts a software event in
an empty event slot in the received event stream.

@param pEr Pointer to MrfErRegs structure
@param code Event code to insert into received event stream
*/
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

/**
Set sequence RAM event. This function writes an event into the sequence RAM.

@param pEr Pointer to MrfErRegs structure
@param ram RAM number, 0 for EVR
@param pos Sequence RAM position (0 to 2047)
@param timestamp 32 bit timestamp of event
@param code Event code
*/
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

/**
Get sequence RAM event timestamp.

@param pEr Pointer to MrfErRegs structure
@param ram RAM number, 0 for EVR
@param pos Sequence RAM position (0 to 2047)
@return 32 bit timestamp of event at RAM position pos
*/
unsigned int EvrGetSeqRamTimestamp(volatile struct MrfErRegs *pEr, int ram, int pos)
{
  if (ram < 0 || ram >= EVR_SEQRAMS)
    return -1;

  if (pos < 0 || pos >= EVR_MAX_SEQRAMEV)
    return -1;

  return be32_to_cpu(pEr->SeqRam[ram][pos].Timestamp);
}

/**
Get sequence RAM event code.

@param pEr Pointer to MrfErRegs structure
@param ram RAM number, 0 for EVR
@param pos Sequence RAM position (0 to 2047)
@return Event code at RAM position pos
*/
int EvrGetSeqRamEvent(volatile struct MrfErRegs *pEr, int ram, int pos)
{
  if (ram < 0 || ram >= EVR_SEQRAMS)
    return -1;

  if (pos < 0 || pos >= EVR_MAX_SEQRAMEV)
    return -1;

  return be32_to_cpu(pEr->SeqRam[ram][pos].EventCode);
}

/**
Show sequence RAM contents.

@param pEr Pointer to MrfErRegs structure
@param ram RAM number, 0 for EVR
*/
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

/**
Control sequence RAM.

@param pEr Pointer to MrfErRegs structure
@param ram RAM number, 0 for EVR
@param enable 0 - disable sequence RAM, 1 - enable sequence RAM
@param single 1 - select single sequence mode
@param recycle 1 - select recycle sequence mode
@param reset 1 - reset sequence RAM
@param trigsel See table \ref trigsel

<table>
<caption id="trigsel">Sequence RAM trigger selection</caption>
<tr><th>ID<th>Trigger
<tr><td>0x00<td>Pulse generator 0
<tr><td>0x01<td>Pulse generator 1
<tr><td>...<td>...
<tr><td>0x20<td>Distributed bus bit 0
<tr><td>0x21<td>Distributed bus bit 1
<tr><td>...<td>...
<tr><td>0x28<td>Prescaler 0
<tr><td>0x29<td>Prescaler 1
<tr><td>...<td>...
<tr><td>0x30-0x3C<td>Reserved
<tr><td>0x3D<td>Software trigger
<tr><td>0x3E<td>Continuous trigger
<tr><td>0x3F<td>Trigger disabled
</table>
*/
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
					  
/**
Software trigger sequence RAM.

@param pEr Pointer to MrfErRegs structure
@param ram RAM number, 0 for EVR
*/
int EvrSeqRamSWTrig(volatile struct MrfErRegs *pEr, int ram)
{
  if (ram < 0 || ram > 1)
    return -1;

  pEr->SeqRamControl[ram] |= be32_to_cpu(1 << C_EVR_SQRC_SWTRIGGER);
  
  return 0;
}

/**
Show sequence RAM status.

@param pEr Pointer to MrfErRegs structure
@param ram RAM number, 0 for EVR
*/
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

/**
Get EVR Topology ID.

@param pEr Pointer to MrfErRegs structure
@return Topology ID
*/
int EvrGetTopologyID(volatile struct MrfErRegs *pEr)
{
  return be32_to_cpu(pEr->TopologyID);
}

/**
Get EVR Delay Compensation status.

@param pEr Pointer to MrfErRegs structure
@return Delay Compensation Status
*/
int EvrGetDCStatus(volatile struct MrfErRegs *pEr)
{
  return be32_to_cpu(pEr->dc_status);
}

/**
Get Delay Compensation delay value. In delay compensation mode the EVR
is trying to adjust the internal delay to get the delay compensation
value as close as possible to the target delay value. In non-DC mode
the internal FIFO depth is adjusted to match the target delay value.

@param pEr Pointer to MrfErRegs structure @return In delay
compensation mode returns the total delay value (internal delay + path
delay value). In non-DC mode returns the measured value of the
internal delay compensation FIFO.
*/
int EvrGetDCDelay(volatile struct MrfErRegs *pEr)
{
  return be32_to_cpu(pEr->dc_int_value) + be32_to_cpu(pEr->dc_value);
}

int EvrGetDCIntDelay(volatile struct MrfErRegs *pEr)
{
  return be32_to_cpu(pEr->dc_int_value);
}

int EvrGetDCPathValue(volatile struct MrfErRegs *pEr)
{
  return be32_to_cpu(pEr->dc_value);
}

int EvrRTMUnivSetDelay(volatile struct MrfErRegs *pEr, int dlymod, int dly)
{
  if (dlymod >= 0 && dlymod < 10)
    {
      pEr->RTMDelay[dlymod] = be32_to_cpu(dly);
      return dly; /* be32_to_cpu(pEr->RTMDelay[dlymod]); */
    }
  else
    return -1;
}
