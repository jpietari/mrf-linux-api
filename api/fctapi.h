/*
  fctapi.h -- Definitions for Micro-Research Event Master
              Fan-Out/Concentrator
              Application Programming Interface

  Author: Jukka Pietarinen (MRF)
  Date:   04.09.2019

*/

/*
  Note: Byte ordering is big-endian.
 */

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

struct MrfFctRegs {
  u32  Status;                               /* 0000: Status Register */
  u32  Control;                              /* 0004: Main Control Register */
  u32  Resv0x0008;                           /* 0008: Reserved */
  u32  Resv0x000C;                           /* 000C: Reserved */
  u32  UpDCValue;                            /* 0010: Upstream Data Compensation Delay Value */
  u32  FIFODCValue;                          /* 0014: Receive FIFO Data Compensation Delay Value */
  u32  IntDCValue;                           /* 0018: FCT Internal Datapath Data Compensation Delay Value */
  u32  RXFIFOTarget;                         /* 001C: Upstream RX FIFO Target */
  u32  Resv0x0020;                           /* 0020: Reserved */
  u32  Resv0x0024;                           /* 0024: Reserved */
  u32  Resv0x0028;                           /* 0028: Reserved */
  u32  TopologyID;                           /* 002C: Timing Node Topology ID */
  u32  Resv0x0030;                           /* 0030: Reserved */
  u32  Resv0x0034;                           /* 0034: Reserved */
  u32  Resv0x0038;                           /* 0038: Reserved */
  u32  Resv0x003C;                           /* 003C: Reserved */
  u32  PortDCValue[16];                      /* 0040: Port Loop Delay Values for Ports 1 through x */
  u32  Resv0x0080_0x013F[(0x0140-0x0080)/4]; /* 0080-013F: Reserved */
  u32  PortDCStatus[16];                     /* 0140: Port DC Status for Ports 1 through x */
  u32  Resv0x0180_0x0FFF[(0x1000-0x0180)/4]; /* 0180-0FFF: Reserved */
  u32  SFPDiag[8][0x80];                     /* 1000-1FFF: Port 1-8 SFP EEPROM and Diagnostics */
};

/* Status register bit mappings */
#define C_FCT_STATUS_LINK12     27
#define C_FCT_STATUS_LINK11     26
#define C_FCT_STATUS_LINK10     25
#define C_FCT_STATUS_LINK9      24
#define C_FCT_STATUS_LINK8      23
#define C_FCT_STATUS_LINK7      22
#define C_FCT_STATUS_LINK6      21
#define C_FCT_STATUS_LINK5      20
#define C_FCT_STATUS_LINK4      19
#define C_FCT_STATUS_LINK3      18
#define C_FCT_STATUS_LINK2      17
#define C_FCT_STATUS_LINK1      16
#define C_FCT_STATUS_VIO12      11
#define C_FCT_STATUS_VIO11      10
#define C_FCT_STATUS_VIO10      9
#define C_FCT_STATUS_VIO9       8
#define C_FCT_STATUS_VIO8       7
#define C_FCT_STATUS_VIO7       6
#define C_FCT_STATUS_VIO6       5
#define C_FCT_STATUS_VIO5       4
#define C_FCT_STATUS_VIO4       3
#define C_FCT_STATUS_VIO3       2
#define C_FCT_STATUS_VIO2       1
#define C_FCT_STATUS_VIO1       0

/* -- Control Register bit mappings */
#define C_FCT_CTRL_CLRV12       11
#define C_FCT_CTRL_CLRV11       10
#define C_FCT_CTRL_CLRV10       9
#define C_FCT_CTRL_CLRV9        8
#define C_FCT_CTRL_CLRV8        7
#define C_FCT_CTRL_CLRV7        6
#define C_FCT_CTRL_CLRV6        5
#define C_FCT_CTRL_CLRV5        4
#define C_FCT_CTRL_CLRV4        3
#define C_FCT_CTRL_CLRV3        2
#define C_FCT_CTRL_CLRV2        1
#define C_FCT_CTRL_CLRV1        0

void FctDumpStatus(volatile struct MrfFctRegs *pFct, double reffreq, int ports);
int FctGetViolation(volatile struct MrfFctRegs *pFct, int clear);

