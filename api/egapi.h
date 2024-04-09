/*
@file egapi.h
@brief Definitions for Micro-Research Event Generator
       Application Programming Interface.
@author Jukka Pietarinen (MRF)
@date 12/5/2006
*/

/*
  Note: Byte ordering is big-endian.
 */

#define EVG_MEM_WINDOW      0x00040000

#ifndef u16
#define u16 unsigned short
#endif
#ifndef u32
#define u32 uint32_t
#endif

#ifndef be16_to_cpu
#if __BYTE_ORDER == __LITTLE_ENDIAN
#define be16_to_cpu(x) bswap_16(x)
#else
#define be16_to_cpu(x) ((unsigned short)(x))
#endif
#endif

#ifndef be32_to_cpu
#if __BYTE_ORDER == __LITTLE_ENDIAN
#define be32_to_cpu(x) bswap_32(x)
#else
#define be32_to_cpu(x) ((uint32_t)(x))
#endif
#endif

#define EVG_MAX_FPOUT_MAP   16
#define EVG_MAX_UNIVOUT_MAP 32
#define EVG_MAX_BPOUT_MAP   16
#define EVG_MAX_BPIN_MAP    16
#define EVG_MAX_TBOUT_MAP   64
#define EVG_MAX_FPIN_MAP    16
#define EVG_MAX_UNIVIN_MAP  16
#define EVG_MAX_TBIN_MAP    64
#define EVG_MAX_BUFFER      2048
#define EVG_MIN_BUF_SEGMENT 0
#define EVG_MAX_BUF_SEGMENT 127
#define EVG_MAX_SEQRAMEV    2048
#define EVG_MAX_SEQRAMS     4
#define EVG_MAX_EVENT_CODE  255
#define EVG_MAX_MXCS        16
#define EVG_MAX_TRIGGERS    16
#define EVG_SEQRAM_CLKSEL_BITS 5
#define C_EVG_SEQRAM_MAX_TRIG_BITS 5
#define EVG_DBUS_BITS       8
#define EVG_SIGNAL_MAP_BITS 6

#ifndef EVG_SEQRAMS
#define EVG_SEQRAMS 2
#endif

#ifndef SEQRAMITEMSTRUCT
#define SEQRAMITEMSTRUCT
struct SeqRamItemStruct {
  u32  Timestamp;
  u32  EventCode;
};
#endif

struct EvanStruct {
  u32 TimestampHigh;
  u32 TimestampLow;
  u32 EventCode;
};

struct MXCStruct {
  u32 Control;
  u32 Prescaler;
};

struct MrfEgRegs {
  u32  Status;                              /* 0000: Status Register */
  u32  Control;                             /* 0004: Main Control Register */
  u32  IrqFlag;                             /* 0008: Interrupt Flags */
  u32  IrqEnable;                           /* 000C: Interrupt Enable */
  u32  ACControl;                           /* 0010: AC divider control */
  u32  ACMap;                               /* 0014: AC event mapping */
  u32  SWEvent;                             /* 0018: Software Event */
  u32  SegBufControl;                       /* 001C: Segmented Data Buffer Control */
  u32  DataBufControl;                      /* 0020: Data Buffer Control */
  u32  DBusMap;                             /* 0024: Distributed Bus Mapping */
  u32  DBusEvent;                           /* 0028: Distributed Bus Event Enable */
  u32  FPGAVersion;                         /* 002C: FPGA version */
  u32  Resv0x0030;                          /* 0030: Reserved */
  u32  TimestampCtrl;                       /* 0034: Reserved */
  u32  TimestampValue;                      /* 0038: Reserved */
  u32  Resv0x003C[(0x048-0x03C)/4];         /* 003C-0047: Reserved */
  u32  TbIn;                                /* 0048: TBIN */
  u32  UsecDiv;                             /* 004C: round(Event clock/1 MHz) */
  u32  ClockControl;                        /* 0050: Clock Control */
  u32  Resv0x0054[(0x060-0x054)/4];         /* 0054-005F: Reserved */
  u32  EvanControl;                         /* 0060: Event Analyzer Control */
  u32  EvanCode;                            /* 0064: Event Analyzer Event Code
					       and DBus data */
  u32  EvanTimeH;                           /* 0068: Event Analyzer Timestamp
					       higher long word */
  u32  EvanTimeL;                           /* 006C: Event Analyzer Timestamp
					       lower long word */
  u32  SeqRamControl[EVG_MAX_SEQRAMS];      /* 0070-007F: Sequence RAM Control Registers */
  u32  FracDiv;                             /* 0080: Fractional Synthesizer SY87739L Control Word */
  u32  Resv0x0084;                          /* 0084: Reserved */
  u32  RxInitPS;                            /* 0088: Initial value for RF Recovery DCM phase */
  u32  Resv0x008Cto0x0100[(0x100-0x08C)/4]; /* 008C-00FF: Reserved */
  u32  EventTrigger[EVG_MAX_TRIGGERS];      /* 0100-013F: Event Trigger Registers */
  u32  SeqRamStartCnt[EVG_MAX_SEQRAMS];     /* 0140-014F: Sequence Start Counters */
  u32  SeqRamEndCnt[EVG_MAX_SEQRAMS];       /* 0150-015F: Sequence End Counters */
  u32  SeqRamRepeatLow[EVG_MAX_SEQRAMS];    /* 0160-016F: Sequence repetition settings (LSB) */
  u32  SeqRamRepeatHigh[EVG_MAX_SEQRAMS];   /* 0170-017F: Sequence repetition settings (MSB) */
  struct MXCStruct MXC[EVG_MAX_MXCS];       /* 0180-01FF: Multiplexed Counter Prescalers */
  u32  Resv0x01C0to0x03FC[(0x400-0x200)/4]; /* 0200-03FF: Reserved */
  u16  FPOutMap[EVG_MAX_FPOUT_MAP];         /* 0400-041F: Front panel output mapping */
  u16  BPOutMap[EVG_MAX_BPOUT_MAP];         /* 0420-043F: Front panel output mapping */
  u16  UnivOutMap[EVG_MAX_UNIVOUT_MAP];     /* 0440-047F: Universal I/O output mapping */
  u16  TBOutMap[EVG_MAX_TBOUT_MAP];         /* 0480-04FF: TB output mapping */
  u32  FPInMap[EVG_MAX_FPIN_MAP];           /* 0500-053F: Front panel input mapping */
  u32  UnivInMap[EVG_MAX_UNIVIN_MAP];       /* 0540-057F: Universal I/O input mapping */
  u32  BPInMap[EVG_MAX_BPIN_MAP];           /* 0580-05BF: Backplane input mapping */
  u32  Resv0x0580[(0x600-0x5C0)/4];         /* 05C0-05FF: Reserved */
  u32  TBInMap[EVG_MAX_TBIN_MAP];           /* 0600-06FF: TB input mapping */
  u32  Resv0x0700[(0x800-0x700)/4];         /* 0700-07FF: Reserved */
  u32  Databuf[EVG_MAX_BUFFER/4];           /* 0800-0FFF: Data Buffer */
  u32  Resv0x1000[(0x2000-0x1000)/4];       /* 1000-1FFF: Reserved */
  u32  Segbuf[EVG_MAX_BUFFER/4];            /* 2000-27FF: Segmented Data Buffer */
  u32  Resv0x2800[(0x8000-0x2800)/4];       /* 2800-7FFF: Reserved */
  struct SeqRamItemStruct SeqRam[EVG_SEQRAMS][EVG_MAX_SEQRAMEV];
                                            /* 8000-BFFF: Sequence RAM 1 */
                                            /* C000-FFFF: Sequence RAM 2 */
  u32  Fct[0x4000];
  u32  EvrD[0x4000];
  u32  EvrU[0x4000];
};

/* -- Status Register bit mappings */
#define C_EVG_STATUS_RXDBUS_HIGH    31
#define C_EVG_STATUS_RXDBUS_LOW     24
#define C_EVG_STATUS_TXDBUS_HIGH    23
#define C_EVG_STATUS_TXDBUS_LOW     16
/* -- Control Register bit mappings */
#define C_EVG_CTRL_MASTER_ENABLE    31
#define C_EVG_CTRL_RX_DISABLE       30
#define C_EVG_CTRL_RX_PWRDOWN       29
#define C_EVG_CTRL_MXC_RESET        24
#define C_EVG_CTRL_BEACON_ENABLE    23
#define C_EVG_CTRL_DCMASTER_ENABLE  22
#define C_EVG_CTRL_SEQRAM_ALT       16
/* -- Interrupt Flag/Enable Register bit mappings */
#define C_EVG_IRQ_MASTER_ENABLE  31
#define C_EVG_IRQ_PCICORE_ENABLE 30
#define C_EVG_NUM_IRQ            16
#define C_EVG_IRQFLAG_SEQSTOP    12
#define C_EVG_IRQFLAG_SEQSTART   8
#define C_EVG_IRQFLAG_EXTERNAL   6
#define C_EVG_IRQFLAG_DATABUF    5
#define C_EVG_IRQFLAG_RXFIFOFULL 1
#define C_EVG_IRQFLAG_VIOLATION  0
#define EVG_IRQ_PCICORE_ENABLE    (1 << C_EVG_IRQ_PCICORE_ENABLE)
/* -- SW Event Register bit mappings */
#define C_EVG_SWEVENT_PENDING    9
#define C_EVG_SWEVENT_ENABLE     8
#define C_EVG_SWEVENT_CODE_HIGH  7
#define C_EVG_SWEVENT_CODE_LOW   0
/* -- AC Input Control Register bit mappings */
#define C_EVG_ACCTRL_ACSYNC_2    19
#define C_EVG_ACCTRL_ACSYNC_1    18
#define C_EVG_ACCTRL_BYPASS      17
#define C_EVG_ACCTRL_ACSYNC      16
#define C_EVG_ACCTRL_DIV_HIGH    15
#define C_EVG_ACCTRL_DIV_LOW     8
#define C_EVG_ACCTRL_DELAY_HIGH  7
#define C_EVG_ACCTRL_DELAY_LOW   0
/* -- AC Input Mapping Register bit mappings */
#define C_EVG_ACMAP_TRIG_BASE   0
/* -- Databuffer Control Register bit mappings */
#define C_EVG_DATABUF_SEGSHIFT   24
#define C_EVG_DATABUF_COMPLETE   20
#define C_EVG_DATABUF_RUNNING    19
#define C_EVG_DATABUF_TRIGGER    18
#define C_EVG_DATABUF_ENA        17
#define C_EVG_DATABUF_MODE       16
#define C_EVG_DATABUF_SIZEHIGH   11
#define C_EVG_DATABUF_SIZELOW    2
/* -- Timestamp Generator Control Register bit mappings */
#define C_EVG_TSCTRL_LOAD        1
#define C_EVG_TSCTRL_ENABLE      0
/* -- Clock Control Register bit mappings */
#define C_EVG_CLKCTRL_PLLL         31
#define C_EVG_CLKCTRL_BWSEL        28
#define C_EVG_CLKCTRL_RFSEL        24
#define C_EVG_CLKCTRL_RFSELMASK    0x07000000
#define C_EVG_CLKCTRL_MAX_RFSEL    7
#define C_EVG_CLKCTRL_EXTRF        24
#define C_EVG_CLKCTRL_PHTOGG       23
#define C_EVG_CLKCTRL_DIV_HIGH     21
#define C_EVG_CLKCTRL_DIV_LOW      16
#define C_EVG_CLKCTRL_RECDCM_RUN    15
#define C_EVG_CLKCTRL_RECDCM_INITD  14
#define C_EVG_CLKCTRL_RECDCM_PSDONE 13
#define C_EVG_CLKCTRL_EVDCM_STOPPED 12
#define C_EVG_CLKCTRL_EVDCM_LOCKED 11
#define C_EVG_CLKCTRL_EVDCM_PSDONE 10
#define C_EVG_CLKCTRL_CGLOCK       9
#define C_EVG_CLKCTRL_RECDCM_PSDEC 8
#define C_EVG_CLKCTRL_RECDCM_PSINC 7
#define C_EVG_CLKCTRL_RECDCM_RESET 6
#define C_EVG_CLKCTRL_EVDCM_PSDEC  5
#define C_EVG_CLKCTRL_EVDCM_PSINC  4
#define C_EVG_CLKCTRL_EVDCM_SRUN   3
#define C_EVG_CLKCTRL_EVDCM_SRES   2
#define C_EVG_CLKCTRL_EVDCM_RES    1
#define C_EVG_CLKCTRL_USE_RXRECCLK 0
/* RF source select */
#define C_EVG_RFSEL_INT     0
#define C_EVG_RFSEL_EXT     1
#define C_EVG_RFSEL_FANOUT  4
/* -- RF Divider Settings */
#define C_EVG_RFDIV_MASK 0x003F
#define C_EVG_RFDIV_1    0x00
#define C_EVG_RFDIV_2    0x01
#define C_EVG_RFDIV_3    0x02
#define C_EVG_RFDIV_4    0x03
#define C_EVG_RFDIV_5    0x04
#define C_EVG_RFDIV_6    0x05
#define C_EVG_RFDIV_7    0x06
#define C_EVG_RFDIV_8    0x07
#define C_EVG_RFDIV_9    0x08
#define C_EVG_RFDIV_10   0x09
#define C_EVG_RFDIV_11   0x0A
#define C_EVG_RFDIV_12   0x0B
#define C_EVG_RFDIV_OFF  0x0C
#define C_EVG_RFDIV_14   0x0D
#define C_EVG_RFDIV_15   0x0E
#define C_EVG_RFDIV_16   0x0F
#define C_EVG_RFDIV_17   0x10
#define C_EVG_RFDIV_18   0x11
#define C_EVG_RFDIV_19   0x12
#define C_EVG_RFDIV_20   0x13
/* -- Event Analyser Control Register bit mappings */
#define C_EVG_EVANCTRL_RESET       3
#define C_EVG_EVANCTRL_NOTEMPTY    3
#define C_EVG_EVANCTRL_CLROVERFLOW 2
#define C_EVG_EVANCTRL_OVERFLOW    2
#define C_EVG_EVANCTRL_ENABLE      1
#define C_EVG_EVANCTRL_COUNTRES    0
/* -- Sequence RAM Control Register bit mappings */
#define C_EVG_SQRC_RUNNING         25
#define C_EVG_SQRC_ENABLED         24
#define C_EVG_SQRC_SWTRIGGER       21
#define C_EVG_SQRC_SINGLE          20
#define C_EVG_SQRC_RECYCLE         19
#define C_EVG_SQRC_RESET           18
#define C_EVG_SQRC_DISABLE         17
#define C_EVG_SQRC_ENABLE          16
#define C_EVG_SQRC_TRIGSEL_LOW     0
/* -- Sequence RAM Triggers */
#define C_EVG_SEQTRIG_MAX          31
#define C_EVG_SEQTRIG_SWTRIGGER1   18
#define C_EVG_SEQTRIG_SWTRIGGER0   17
#define C_EVG_SEQTRIG_ACINPUT      16
#define C_EVG_SEQTRIG_MXC_BASE     0
#define EVG_SEQTRIG_SWTRIGGER1     (1 << C_EVG_SEQTRIG_SWTRIGGER1)
#define EVG_SEQTRIG_SWTRIGGER0     (1 << C_EVG_SEQTRIG_SWTRIGGER0)
#define EVG_SEQTRIG_ACINPUT        (1 << C_EVG_SEQTRIG_ACINPUT)
/* -- Event Trigger bit mappings */
#define C_EVG_EVENTTRIG_ENABLE     8
#define C_EVG_EVENTTRIG_CODE_HIGH  7
#define C_EVG_EVENTTRIG_CODE_LOW   0
/* -- Input mapping bits */
#define C_EVG_INMAP_TRIG_BASE      0
#define C_EVG_INMAP_SEQTRIG_BASE   8
#define C_EVG_INMAP_SEQENA_BASE    12
#define C_EVG_INMAP_DBUS_BASE      16
#define C_EVG_INMAP_IRQ            24
#define C_EVG_INMAP_SEQMASK        24
/* -- Multiplexed Counter Mapping Bits */
#define C_EVG_MXC_READ             31
#define C_EVG_MXCMAP_TRIG_BASE     0
/* -- Distributed Bus Mapping Bits */
#define C_EVG_DBUS_SEL_BITS        4
#define C_EVG_DBUS_SEL_MASK        3
#define C_EVG_DBUS_SEL_OFF         0
#define C_EVG_DBUS_SEL_EXT         1
#define C_EVG_DBUS_SEL_MXC         2
#define C_EVG_DBUS_SEL_FORWARD     3
/* -- C_SIGNAL_MAP_DBUS defines the starting index of DBUS bit 0 */
#define C_SIGNAL_MAP_DBUS          32
/* -- C_SIGNAL_MAP_PRESC defines the starting index of the prescaler outputs */
#define C_SIGNAL_MAP_PRESC         40
/* -- C_SIGNAL_MAP_HIGH defines the index for state high output */
/* -- undefined indexes drive the output low */
#define C_SIGNAL_MAP_HIGH          62
#define C_SIGNAL_MAP_LOW           63

/* Function prototypes */
int EvgOpen(struct MrfEgRegs **pEg, char *device_name);
int EvgClose(int fd);
u32 EvgFWVersion(volatile struct MrfEgRegs *pEg);
int EvgEnable(volatile struct MrfEgRegs *pEg, int state);
int EvgGetEnable(volatile struct MrfEgRegs *pEg);
int EvgSystemMasterEnable(volatile struct MrfEgRegs *pEg, int state);
int EvgGetSystemMasterEnable(volatile struct MrfEgRegs *pEg);
int EvgBeaconEnable(volatile struct MrfEgRegs *pEg, int state);
int EvgGetBeaconEnable(volatile struct MrfEgRegs *pEg);
int EvgRxEnable(volatile struct MrfEgRegs *pEg, int state);
int EvgRxGetEnable(volatile struct MrfEgRegs *pEg);
int EvgGetViolation(volatile struct MrfEgRegs *pEg, int clear);
int EvgSWEventEnable(volatile struct MrfEgRegs *pEg, int state);
int EvgGetSWEventEnable(volatile struct MrfEgRegs *pEg);
int EvgSendSWEvent(volatile struct MrfEgRegs *pEg, int code);
int EvgEvanEnable(volatile struct MrfEgRegs *pEg, int state);
int EvgEvanGetEnable(volatile struct MrfEgRegs *pEg);
void EvgEvanReset(volatile struct MrfEgRegs *pEg);
void EvgEvanResetCount(volatile struct MrfEgRegs *pEg);
int EvgEvanGetEvent(volatile struct MrfEgRegs *pEg, struct EvanStruct *evan);
void EvgEvanDump(volatile struct MrfEgRegs *pEg);
int EvgSetMXCPrescaler(volatile struct MrfEgRegs *pEg, int mxc, unsigned int presc);
unsigned int EvgGetMXCPrescaler(volatile struct MrfEgRegs *pEg, int mxc);
int EvgSetMxcTrigMap(volatile struct MrfEgRegs *pEg, int mxc, int map);
void EvgSyncMxc(volatile struct MrfEgRegs *pEg);
void EvgMXCDump(volatile struct MrfEgRegs *pEg);
int EvgSetDBusMap(volatile struct MrfEgRegs *pEg, int dbus, int map);
void EvgDBusDump(volatile struct MrfEgRegs *pEg);
int EvgSetDBusEvent(volatile struct MrfEgRegs *pEg, int enable);
int EvgGetDBusEvent(volatile struct MrfEgRegs *pEg);
int EvgSetACInput(volatile struct MrfEgRegs *pEg, int bypass, int sync, int div, int delay);
int EvgSetACMap(volatile struct MrfEgRegs *pEg, int map);
void EvgACDump(volatile struct MrfEgRegs *pEg);
int EvgSetRFInput(volatile struct MrfEgRegs *pEg, int RFsel, int div);
int EvgSetFracDiv(volatile struct MrfEgRegs *pEg, int fracdiv);
int EvgGetFracDiv(volatile struct MrfEgRegs *pEg);
int EvgSetSeqRamEvent(volatile struct MrfEgRegs *pEg, int ram, int pos, unsigned int timestamp, int code, int mask);
unsigned int EvgGetSeqRamTimestamp(volatile struct MrfEgRegs *pEg, int ram, int pos);
int EvgGetSeqRamEvent(volatile struct MrfEgRegs *pEg, int ram, int pos);
void EvgSeqRamDump(volatile struct MrfEgRegs *pEg, int ram);
int EvgSeqRamControl(volatile struct MrfEgRegs *pEg, int ram, int enable, int single, int recycle, int reset, int trigsel, int mask);
int EvgSeqRamSetRepeat(volatile struct MrfEgRegs *pEg, int ram, unsigned int count);
int EvgSeqRamSetRepeatHigh(volatile struct MrfEgRegs *pEg, int ram, unsigned int count);
unsigned int EvgSeqRamGetRepeat(volatile struct MrfEgRegs *pEg, int ram);
unsigned int EvgSeqRamGetRepeatHigh(volatile struct MrfEgRegs *pEg, int ram);
int EvgSeqRamSWTrig(volatile struct MrfEgRegs *pEg, int ram);
void EvgSeqRamStatus(volatile struct MrfEgRegs *pEg, int ram);
unsigned int EvgSeqRamGetStartCnt(volatile struct MrfEgRegs *pEg, int ram);
unsigned int EvgSeqRamGetEndCnt(volatile struct MrfEgRegs *pEg, int ram);
int EvgSetUnivinMap(volatile struct MrfEgRegs *pEg, int univ, int trig, int dbus, int irq, int seqtrig, int mask);
int EvgGetUnivinMapTrigger(volatile struct MrfEgRegs *pEg, int univ);
int EvgGetUnivinMapDBus(volatile struct MrfEgRegs *pEg, int univ);
int EvgGetUnivinMapIrq(volatile struct MrfEgRegs *pEg, int univ);
int EvgGetUnivinMapSeqtrig(volatile struct MrfEgRegs *pEg, int univ);
void EvgUnivinDump(volatile struct MrfEgRegs *pEg);
int EvgSetFPinMap(volatile struct MrfEgRegs *pEg, int univ, int trig, int dbus, int irq, int seqtrig, int seqena, int mask);
int EvgGetFPinMapTrigger(volatile struct MrfEgRegs *pEg, int univ);
int EvgGetFPinMapDBus(volatile struct MrfEgRegs *pEg, int univ);
int EvgGetFPinMapIrq(volatile struct MrfEgRegs *pEg, int univ);
int EvgGetFPinMapSeqtrig(volatile struct MrfEgRegs *pEg, int univ);
void EvgFPinDump(volatile struct MrfEgRegs *pEg);
int EvgSetBPinMap(volatile struct MrfEgRegs *pEg, int bp, int trig, int dbus, int irq, int seqtrig, int mask);
int EvgGetBPinMapTrigger(volatile struct MrfEgRegs *pEg, int bp);
int EvgGetBPinMapDBus(volatile struct MrfEgRegs *pEg, int bp);
int EvgGetBPinMapIrq(volatile struct MrfEgRegs *pEg, int bp);
int EvgGetBPinMapSeqtrig(volatile struct MrfEgRegs *pEg, int bp);
void EvgBPinDump(volatile struct MrfEgRegs *pEg);
int EvgSetTBinMap(volatile struct MrfEgRegs *pEg, int tb, int trig, int dbus, int irq, int seqtrig, int mask);
int EvgGetTBinMapTrigger(volatile struct MrfEgRegs *pEg, int tb);
int EvgGetTBinMapDBus(volatile struct MrfEgRegs *pEg, int tb);
int EvgGetTBinMapIrq(volatile struct MrfEgRegs *pEg, int tb);
int EvgGetTBinMapSeqtrig(volatile struct MrfEgRegs *pEg, int tb);
void EvgTBinDump(volatile struct MrfEgRegs *pEg);
int EvgSetTriggerEvent(volatile struct MrfEgRegs *pEg, int trigger, int code, int enable);
int EvgGetTriggerEventCode(volatile struct MrfEgRegs *pEg, int trigger);
int EvgGetTriggerEventEnable(volatile struct MrfEgRegs *pEg, int trigger);
void EvgTriggerEventDump(volatile struct MrfEgRegs *pEg);
int EvgSetUnivOutMap(volatile struct MrfEgRegs *pEg, int output, int map);
int EvgGetUnivOutMap(volatile struct MrfEgRegs *pEg, int output);
int EvgSetFPOutMap(volatile struct MrfEgRegs *pEg, int output, int map);
int EvgGetFPOutMap(volatile struct MrfEgRegs *pEg, int output);
int EvgSetBPOutMap(volatile struct MrfEgRegs *pEg, int output, int map);
int EvgGetBPOutMap(volatile struct MrfEgRegs *pEg, int output);
int EvgSetTBOutMap(volatile struct MrfEgRegs *pEg, int output, int map);
int EvgGetTBOutMap(volatile struct MrfEgRegs *pEg, int output);
int EvgSetDBufMode(volatile struct MrfEgRegs *pEg, int enable);
int EvgGetDBufStatus(volatile struct MrfEgRegs *pEg);
int EvgGetSegBufStatus(volatile struct MrfEgRegs *pEg);
int EvgSendDBuf(volatile struct MrfEgRegs *pEg, char *dbuf, int size);
int EvgSendSegBuf(volatile struct MrfEgRegs *pEg, char *dbuf, int segment, int size);
int EvgGetFormFactor(volatile struct MrfEgRegs *pEg);
void EvgIrqAssignHandler(volatile struct MrfEgRegs *pEg, int fd, void (*handler)(int));
void EvgIrqUnassignHandler(int vector, void (*handler)(int));
int EvgIrqEnable(volatile struct MrfEgRegs *pEg, int mask);
int EvgGetIrqFlags(volatile struct MrfEgRegs *pEg);
int EvgClearIrqFlags(volatile struct MrfEgRegs *pEg, int mask);
void EvgIrqHandled(int fd);
int EvgTimestampEnable(volatile struct MrfEgRegs *pEg, int enable);
int EvgGetTimestampEnable(volatile struct MrfEgRegs *pEg);
int EvgTimestampLoad(volatile struct MrfEgRegs *pEg, int timestamp);
int EvgTimestampGet(volatile struct MrfEgRegs *pEg);
void EvgDumpClockControl(volatile struct MrfEgRegs *pEg);
void EvgDumpStatus(volatile struct MrfEgRegs *pEg);
