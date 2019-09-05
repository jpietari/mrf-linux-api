/*
  sfpdiag.c -- SFP Transceiver Diagnostics Tools
               for Micro-Research Event Generator/Receiver
               Application Programming Interface

  Author: Jukka Pietarinen (MRF)
  Date:   10.5.2012

*/

#ifdef __linux__
#include <sys/types.h>
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
#include <ctype.h>
#include "sfpdiag.h"

#ifndef be16_to_cpu
#if __BYTE_ORDER == __LITTLE_ENDIAN
#define be16_to_cpu(x) bswap_16(x)
#else
#define be16_to_cpu(x) ((unsigned short)(x))
#endif
#endif

void SFPSN(struct SFPDiag *pSFP)
{
  int i;

  printf(" ");
  for (i = 0; i < 16; i++)
    if (isprint(pSFP->sn[i]))
      printf("%c", pSFP->sn[i]);
}

void SFPDump(struct SFPDiag *pSFP)
{
  int i;

  printf("BASE ID FIELDS\n");
  printf("Type of serial transceiver:         %02x\n", pSFP->transceiver_type);
  printf("Extended identifier of type serial: %02x\n", pSFP->ext_id);
  printf("Code for connector type:            %02x\n", pSFP->connector_code);
  printf("Code for elec./opt. compatibility:  ");
  for (i = 0; i < 8; i++)
    printf("%02x", pSFP->compatibility_code[i]);
  printf("\n");
  printf("Code for serial encoding algorithm: %02x\n", pSFP->encoding);
  printf("Nominal bit rate                    %d00 MBits/s\n", pSFP->nominal_bitrate);
  printf("Link length for 9/125 um:           %d km\n", pSFP->link_len_9_125_km);
  printf("Link length for 9/125 um:           %d00 m\n", pSFP->link_len_9_125_100m);
  printf("Link length for 50/125 um:          %d0 m\n", pSFP->link_len_50_125_10m);
  printf("Link length for 62.5/125 um:        %d0 m\n", pSFP->link_len_625_125_10m);
  printf("Link length for copper:             %d m\n", pSFP->link_len_copper);
  printf("SFP transceiver vendor name:        ");
  for (i = 0; i < 16; i++)
    if (isprint(pSFP->vendor[i]))
      printf("%c", pSFP->vendor[i]);
  printf("\n");
  printf("SFP transceiver vendor IEEE:        ");
  for (i = 0; i < 3; i++)
    printf("%02x", pSFP->ieee_id[i]);
  printf("\n");
  printf("Part number provided by vendor:     ");
  for (i = 0; i < 16; i++)
    if (isprint(pSFP->part_number[i]))
      printf("%c", pSFP->part_number[i]);
  printf("\n");
  printf("Revision level for part number:     ");
  for (i = 0; i < 4; i++)
    if (isprint(pSFP->revision[i]))
      printf("%c", pSFP->revision[i]);
  printf("\n");
  printf("EXTENDED ID FIELDS\n");
  printf("Implemented optional SFP signals:   %04x\n", be16_to_cpu(pSFP->optional_signals));
  printf("Upper bit rate margin:              %d %%\n", pSFP->upper_br_margin);
  printf("Lower bit rate margin:              %d %%\n", pSFP->lower_br_margin);
  printf("Serial number provided by vendor:   ");
  for (i = 0; i < 16; i++)
    if (isprint(pSFP->sn[i]))
      printf("%c", pSFP->sn[i]);
  printf("\n");
  printf("Vendor's manufacturing date code:   ");
  for (i = 0; i < 8; i++)
    if (isprint(pSFP->date_code[i]))
      printf("%c", pSFP->date_code[i]);
  printf("\n");
  printf("ENHANCED FEATURE SET MEMORY\n");
  printf("Temp H Alarm:                       %.1f degC\n", ((signed short) be16_to_cpu(pSFP->temp_h_alarm))/256.0);
  printf("Temp L Alarm:                       %.1f degC\n", ((signed short) be16_to_cpu(pSFP->temp_l_alarm))/256.0);
  printf("Temp H Warning:                     %.1f degC\n", ((signed short) be16_to_cpu(pSFP->temp_h_warning))/256.0);
  printf("Temp L Warning:                     %.1f degC\n", ((signed short) be16_to_cpu(pSFP->temp_l_warning))/256.0);
  printf("VCC H Alarm:                        %.3f V\n", be16_to_cpu(pSFP->vcc_h_alarm)/10000.0);
  printf("VCC L Alarm:                        %.3f V\n", be16_to_cpu(pSFP->vcc_l_alarm)/10000.0);
  printf("VCC H Warning:                      %.3f V\n", be16_to_cpu(pSFP->vcc_h_warning)/10000.0);
  printf("VCC L Warning:                      %.3f V\n", be16_to_cpu(pSFP->vcc_l_warning)/10000.0);
  printf("Tx Bias H Alarm:                    %d uA\n", be16_to_cpu(pSFP->tx_bias_h_alarm)*2);
  printf("Tx Bias L Alarm:                    %d uA\n", be16_to_cpu(pSFP->tx_bias_l_alarm)*2);
  printf("Tx Bias H Warning:                  %d uA\n", be16_to_cpu(pSFP->tx_bias_h_warning)*2);
  printf("Tx Bias L Warning:                  %d uA\n", be16_to_cpu(pSFP->tx_bias_l_warning)*2);
  printf("Tx Power H Alarm:                   %.1f uW\n", be16_to_cpu(pSFP->tx_power_h_alarm)*0.1);
  printf("Tx Power L Alarm:                   %.1f uW\n", be16_to_cpu(pSFP->tx_power_l_alarm)*0.1);
  printf("Tx Power H Warning:                 %.1f uW\n", be16_to_cpu(pSFP->tx_power_h_warning)*0.1);
  printf("Tx Power L Warning:                 %.1f uW\n", be16_to_cpu(pSFP->tx_power_l_warning)*0.1);
  printf("Rx Power H Alarm:                   %.1f uW\n", be16_to_cpu(pSFP->rx_power_h_alarm)*0.1);
  printf("Rx Power L Alarm:                   %.1f uW\n", be16_to_cpu(pSFP->rx_power_l_alarm)*0.1);
  printf("Rx Power H Warning:                 %.1f uW\n", be16_to_cpu(pSFP->rx_power_h_warning)*0.1);
  printf("Rx Power L Warning:                 %.1f uW\n", be16_to_cpu(pSFP->rx_power_l_warning)*0.1);
  printf("Real Time Temperature:              %.1f degC\n", be16_to_cpu(pSFP->rt_temperature)/256.0);
  printf("Real Time VCC:                      %.3f V\n", be16_to_cpu(pSFP->rt_vcc)/10000.0);
  printf("Real Time Tx Bias:                  %d uA\n", be16_to_cpu(pSFP->rt_tx_bias)*2);
  printf("Real Time Tx Power:                 %.1f uW\n", be16_to_cpu(pSFP->rt_tx_power)*0.1);
  printf("Real Time Rx Power:                 %.1f uW\n", be16_to_cpu(pSFP->rt_rx_power)*0.1);
  printf("Status / Control:                   %02x\n", pSFP->status_control);
  printf("Alarm Flags:                        %02x %02x\n", pSFP->alarm_flags[0], pSFP->alarm_flags[1]);
  printf("Warning Flags:                      %02x %02x\n", pSFP->warning_flags[0], pSFP->warning_flags[1]);
}
