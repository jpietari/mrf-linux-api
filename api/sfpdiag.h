/*
  sfpdiag.h -- SFP Transceiver Diagnostics Register Definitions
               for Micro-Research Event Generator/Receiver
               Application Programming Interface

  Author: Jukka Pietarinen (MRF)
  Date:   10.5.2012

*/

/*
  Note: Byte ordering is big-endian.
 */

#ifndef u8
#define u8 unsigned char
#endif
#ifndef u16
#define u16 unsigned short
#endif
#ifndef u32
#define u32 unsigned long
#endif
#ifndef s16
#define s16 signed short
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
#define be32_to_cpu(x) ((unsigned long)(x))
#endif
#endif

struct SFPDiag {
  /* Base ID Fields */
  u8  transceiver_type;
  u8  ext_id;
  u8  connector_code;
  u8  compatibility_code[8];
  u8  encoding;
  u8  nominal_bitrate;
  u8  reserved_13;
  u8  link_len_9_125_km;
  u8  link_len_9_125_100m;
  u8  link_len_50_125_10m;
  u8  link_len_625_125_10m;
  u8  link_len_copper;
  u8  reserved_19;
  u8  vendor[16];
  u8  reserved_36;
  u8  ieee_id[3];
  u8  part_number[16];
  u8  revision[4];
  u8  reserved_60[3];
  u8  cc_base;
  /* Extended ID Fields */
  u16 optional_signals;
  u8  upper_br_margin;
  u8  lower_br_margin;
  u8  sn[16];
  u8  date_code[8];
  u8  reserved_92[3];
  u8  cc_ext;
  /* Vendor specific ID Fields */
  u8  vendor_data[32];
  u8  reserved_128[128];
  /* Enhanced Feature Set Memory */
  s16 temp_h_alarm;
  s16 temp_l_alarm;
  s16 temp_h_warning;
  s16 temp_l_warning;
  u16 vcc_h_alarm;
  u16 vcc_l_alarm;
  u16 vcc_h_warning;
  u16 vcc_l_warning;
  u16 tx_bias_h_alarm;
  u16 tx_bias_l_alarm;
  u16 tx_bias_h_warning;
  u16 tx_bias_l_warning;
  u16 tx_power_h_alarm;
  u16 tx_power_l_alarm;
  u16 tx_power_h_warning;
  u16 tx_power_l_warning;
  u16 rx_power_h_alarm;
  u16 rx_power_l_alarm;
  u16 rx_power_h_warning;
  u16 rx_power_l_warning;
  u8  reserved_296[16];
  u8  ext_calib_const[39];
  u8  cs_enh;
  s16 rt_temperature;
  u16 rt_vcc;
  u16 rt_tx_bias;
  u16 rt_tx_power;
  u16 rt_rx_power;
  u8  reserved_362[4];
  u8  status_control;
  u8  reserved_367;
  u8  alarm_flags[2];
  u8  reserved_370[2];
  u8  warning_flags[2];
  u8  reserved_374[138];
};

void SFPSN(struct SFPDiag *pSFP);
void SFPDump(struct SFPDiag *pSFP);
