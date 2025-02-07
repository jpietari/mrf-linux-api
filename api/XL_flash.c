#ifdef __linux__
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
#endif
#include <stdio.h>
#include <string.h>

#define XL_CMD_READ_ARRAY      0xff
#define XL_CMD_READ_STATUS     0x70
#define XL_CMD_READ_SIGNATURE  0x90
#define XL_CMD_CLEAR_STATUS    0x50
#define XL_CMD_BLOCK_ERASE     0x20
#define XL_CMD2_BLOCK_ERASE    0xD0
#define XL_CMD_BUFFER_PROGRAM  0xE8
#define XL_END_BUFFER_PROGRAM  0xD0
#define XL_CMD_SET_CONFIG      0x60
#define XL_CMD2_SET_CONFIG     0x03
#define XL_CMD_BLOCK_LOCK      0x60
#define XL_CMD2_BLOCK_LOCK     0x01
#define XL_CMD_BLOCK_UNLOCK    0x60
#define XL_CMD2_BLOCK_UNLOCK   0xD0
#define XL_CMD_BLANK_CHECK     0xBC
#define XL_CMD2_BLANK_CHECK    0xCB

#define XL_CRD                 0x12BBE

#define XL_BLOCK_SIZE          0x20000
#define XL_MEM_WINDOW          0x01000000

#ifndef be16_to_cpu
#if __BYTE_ORDER == __LITTLE_ENDIAN
#define be16_to_cpu(x) bswap_16(x)
#else
#define be16_to_cpu(x) ((unsigned short)(x))
#endif
#endif

static const unsigned char bit_reverse_byte[] = 
{
  0x00, 0x80, 0x40, 0xC0, 0x20, 0xA0, 0x60, 0xE0, 0x10, 0x90, 0x50, 0xD0, 0x30, 0xB0, 0x70, 0xF0, 
  0x08, 0x88, 0x48, 0xC8, 0x28, 0xA8, 0x68, 0xE8, 0x18, 0x98, 0x58, 0xD8, 0x38, 0xB8, 0x78, 0xF8, 
  0x04, 0x84, 0x44, 0xC4, 0x24, 0xA4, 0x64, 0xE4, 0x14, 0x94, 0x54, 0xD4, 0x34, 0xB4, 0x74, 0xF4, 
  0x0C, 0x8C, 0x4C, 0xCC, 0x2C, 0xAC, 0x6C, 0xEC, 0x1C, 0x9C, 0x5C, 0xDC, 0x3C, 0xBC, 0x7C, 0xFC, 
  0x02, 0x82, 0x42, 0xC2, 0x22, 0xA2, 0x62, 0xE2, 0x12, 0x92, 0x52, 0xD2, 0x32, 0xB2, 0x72, 0xF2, 
  0x0A, 0x8A, 0x4A, 0xCA, 0x2A, 0xAA, 0x6A, 0xEA, 0x1A, 0x9A, 0x5A, 0xDA, 0x3A, 0xBA, 0x7A, 0xFA,
  0x06, 0x86, 0x46, 0xC6, 0x26, 0xA6, 0x66, 0xE6, 0x16, 0x96, 0x56, 0xD6, 0x36, 0xB6, 0x76, 0xF6, 
  0x0E, 0x8E, 0x4E, 0xCE, 0x2E, 0xAE, 0x6E, 0xEE, 0x1E, 0x9E, 0x5E, 0xDE, 0x3E, 0xBE, 0x7E, 0xFE,
  0x01, 0x81, 0x41, 0xC1, 0x21, 0xA1, 0x61, 0xE1, 0x11, 0x91, 0x51, 0xD1, 0x31, 0xB1, 0x71, 0xF1,
  0x09, 0x89, 0x49, 0xC9, 0x29, 0xA9, 0x69, 0xE9, 0x19, 0x99, 0x59, 0xD9, 0x39, 0xB9, 0x79, 0xF9, 
  0x05, 0x85, 0x45, 0xC5, 0x25, 0xA5, 0x65, 0xE5, 0x15, 0x95, 0x55, 0xD5, 0x35, 0xB5, 0x75, 0xF5,
  0x0D, 0x8D, 0x4D, 0xCD, 0x2D, 0xAD, 0x6D, 0xED, 0x1D, 0x9D, 0x5D, 0xDD, 0x3D, 0xBD, 0x7D, 0xFD,
  0x03, 0x83, 0x43, 0xC3, 0x23, 0xA3, 0x63, 0xE3, 0x13, 0x93, 0x53, 0xD3, 0x33, 0xB3, 0x73, 0xF3, 
  0x0B, 0x8B, 0x4B, 0xCB, 0x2B, 0xAB, 0x6B, 0xEB, 0x1B, 0x9B, 0x5B, 0xDB, 0x3B, 0xBB, 0x7B, 0xFB,
  0x07, 0x87, 0x47, 0xC7, 0x27, 0xA7, 0x67, 0xE7, 0x17, 0x97, 0x57, 0xD7, 0x37, 0xB7, 0x77, 0xF7, 
  0x0F, 0x8F, 0x4F, 0xCF, 0x2F, 0xAF, 0x6F, 0xEF, 0x1F, 0x9F, 0x5F, 0xDF, 0x3F, 0xBF, 0x7F, 0xFF
};


void XL_initialize(volatile short *flash)
{
  *(flash + (XL_CRD >> 1)) = be16_to_cpu(XL_CMD_SET_CONFIG);
  *(flash + (XL_CRD >> 1)) = be16_to_cpu(XL_CMD2_SET_CONFIG);
}

void XL_clear_status_register(volatile short *flash)
{
  *(flash) = be16_to_cpu(XL_CMD_CLEAR_STATUS);
}

int XL_read_status_register(volatile short *flash, int bka)
{
  *(flash + (bka >> 1)) = be16_to_cpu(XL_CMD_READ_STATUS);

  return be16_to_cpu(*(flash + bka));
}

int XL_check_device(volatile short *flash)
{
  int manufacturer_code, device_code;
  int i;

  *(flash) = be16_to_cpu(XL_CMD_READ_SIGNATURE);

  manufacturer_code = be16_to_cpu(*(flash));
  device_code = be16_to_cpu(*(flash+1));
  
  if (manufacturer_code != 0x0049 ||
      device_code != 0x506B)
    {
      fprintf(stderr, "Invalid Manufacturer %04X, Device %04X\n",
	      manufacturer_code, device_code);
      return -1;
    }

  fprintf(stderr, "Unique Device Number: ");
  for (i = 0x81; i <= 0x84; i++)
    {
      fprintf(stderr, "%04X", be16_to_cpu(*(flash+i)));
    }
  fprintf(stderr, "\n");

  return 0;  
}

void XL_dump_block(volatile short *flash, int ba)
{
  int i;
  short d;

  ba &= ~(XL_BLOCK_SIZE - 1);

  fprintf(stderr, "Reading block at %08X\n", ba);

  *(flash + (ba >> 1)) = be16_to_cpu(XL_CMD_READ_ARRAY);

  for (i = 0; i < XL_BLOCK_SIZE; i +=2 )
    {
      d = be16_to_cpu(*(flash + ((ba + i) >> 1)));
      putchar(d >> 8);
      putchar(d & 0x00ff);
    }
}

int XL_block_erase(volatile short *flash, int ba)
{
  int i;
  short d;
  volatile short *bank;

  XL_clear_status_register(flash);

  ba &= ~(XL_BLOCK_SIZE - 1);
  
#ifdef DEBUG
  fprintf(stderr, "XL_block_erase %08X\n", ba);
#endif

  /* First check if block is already blank */

#if 0
  *(flash + (ba >> 1)) = be16_to_cpu(XL_CMD_BLANK_CHECK);
  *(flash + (ba >> 1)) = be16_to_cpu(XL_CMD2_BLANK_CHECK);

  i = 0;
  do
    {
      d = be16_to_cpu(*(bank + (i & 0xffe)));
      i++;
    }
  while (!(d & 0x0080) || d == -1);

  fprintf(stderr, "Blank check took %d loops, status %02x\n", i, d);

  if (d & 0x20)
#endif

  bank = flash + (ba >> 1);

  *(flash + (ba >> 1)) = be16_to_cpu(XL_CMD_READ_ARRAY);

  for (i = 0; i < XL_BLOCK_SIZE; i += 2)
    {
      d = *(bank++);
      if (d != -1)
	break;
    }

  if (i != XL_BLOCK_SIZE)
    {
      fprintf(stderr, "Blank check failed on block %08X, erasing block\n", ba);

      /* Unlock Block */

      *(flash + (ba >> 1)) = be16_to_cpu(XL_CMD_BLOCK_UNLOCK);
      *(flash + (ba >> 1)) = be16_to_cpu(XL_CMD2_BLOCK_UNLOCK);     

      *(flash + (ba >> 1)) = be16_to_cpu(XL_CMD_BLOCK_ERASE);
      *(flash + (ba >> 1)) = be16_to_cpu(XL_CMD2_BLOCK_ERASE);

      i = 0;
      do
	{
	  d = be16_to_cpu(*(flash + (ba >> 1)));
	  i++;
	}
      while (!(d & 0x80));

      fprintf(stderr, "Block erase command took %d loops, status %02x\n", i, d);

      if (d & 0x0008)
	{
	  fprintf(stderr, "Invalid VPP.\n");
	  return -1;
	}
      if (d & 0x0030 == 0x0030)
	{
	  fprintf(stderr, "Invalid command sequence.\n");
	  return -1;
	}
      if (d & 0x0020 == 0x0020)
	{
	  fprintf(stderr, "Erase error.\n");
	  return -1;
	}
      if (d & 0x0002 == 0x0002)
	{
	  fprintf(stderr, "Block Protected.\n");
	  return -1;
	}
    }

  return 0;
}

int XL_buffer_program(volatile short *flash, int ba, char *buffer)
{
  int i;
  short d;

  XL_clear_status_register(flash);

  /* Unlock Block */

  *(flash + (ba >> 1)) = be16_to_cpu(XL_CMD_BLOCK_UNLOCK);
  *(flash + (ba >> 1)) = be16_to_cpu(XL_CMD2_BLOCK_UNLOCK);     

  i = 0;
  do
    {
      *(flash + (ba >> 1)) = be16_to_cpu(XL_CMD_BUFFER_PROGRAM);
      d = be16_to_cpu(*(flash + (ba >> 1)));
      if (d & 0x80)
	break;
    }
  while (i < 1000);

  if (!(d & 0x80))
    {
      fprintf(stderr, "Buffer not available for programming.\n");
      return -1;
    }

  *(flash + (ba >> 1)) = be16_to_cpu(31);

  for (i = 0; i < 32; i++)
    {
      d = (buffer[i << 1] << 8) + (buffer[(i << 1)+1] & 0x00ff);
      *(flash + (ba >> 1) + i) = be16_to_cpu(d);
    }

  *(flash + (ba >> 1)) = be16_to_cpu(XL_END_BUFFER_PROGRAM);

  i = 0;
  do
    {
      d = be16_to_cpu(*(flash + (ba >> 1)));
      i++;
    }
  while (!(d & 0x80));

  fprintf(stderr, "Buffer program took %d loops, status %02x\r", i, d);

  if (d & 0x0008)
    {
      fprintf(stderr, "Invalid VPP.\n");
      return -1;
    }
  if (d & 0x0010 == 0x0010)
    {
      fprintf(stderr, "Program error.\n");
      return -1;
    }
  if (d & 0x0002 == 0x0002)
    {
      fprintf(stderr, "Block Protected.\n");
      return -1;
    }

  return 0;
}

void help(char *cmd)
{
  fprintf(stderr, "Usage: %s [command] /dev/er3a1\n", cmd);
  fprintf(stderr, "-h Display help\n");
  fprintf(stderr, "-d Dump flash contents to stdio (binary)\n");
  fprintf(stderr, "-e[0-3] Erase partition (0 = fallback image , 3 = primary image)\n");
  fprintf(stderr, "-p[0-3] Program buffer (0 = fallback image , 3 = primary image)\n");
}

int main(int argc, char *argv[])
{
  short *flash;
  int   fdFlash;

  if (argc < 2 || (argc >= 2 && argv[1][0] != '-'))
    {
      argv[1] = "-h";
      argc = 2;
    }

  if (argc < 3)
    argv[2] = "/dev/er3a1";

  fdFlash = open(argv[2], O_RDWR);
  if (fdFlash != -1)
    {
      flash = (short *) mmap(0, XL_MEM_WINDOW, PROT_READ | PROT_WRITE,
			     MAP_SHARED, fdFlash, 0);
      if (flash == MAP_FAILED)
	{
	  close(fdFlash);
	  fprintf(stderr, "Could not mmap flash device to CPU memory.\n");
	  return -1;
	}
    }
  else
    {
      fprintf(stderr, "Could not open device %s.\n", argv[2]);
      return -1;
    }

  XL_initialize(flash);
  /*
  if (XL_check_device(flash))
    {
      munmap(flash, XL_MEM_WINDOW);
      close(fdFlash);
      return -1;
    }
  */

  switch (argv[1][1])
    {
    case 'h':
      help(argv[0]);
      break;
    case 'd':
      {
	int ba;
	for (ba = 0; ba < XL_MEM_WINDOW; ba += 0x20000)
	  XL_dump_block(flash, ba);
	break;
      }
    case 'e':
      {
	int bka, ba;
	if (argv[1][2] < '0' && argv[1][2] > '3')
	  {
	    help(argv[0]);
	    break;
	  }
	bka = ((argv[1][2] - '0') & 3) << 22;
	for (ba = 0; ba < XL_MEM_WINDOW/4; ba += 0x20000)
	  XL_block_erase(flash, bka + ba);
	break;
      }
    case 'p':
      {
	int bka, ba, d, i;
	char buffer[64];

	if (argv[1][2] < '0' && argv[1][2] > '3')
	  {
	    help(argv[0]);
	    break;
	  }
	bka = ((argv[1][2] - '0') & 3) << 22;

	for (ba = 0; ba < XL_MEM_WINDOW/4; ba += 64)
	  {
	    for (i = 0; i < 64; i++)
	      if (!feof(stdin))
		buffer[i] = bit_reverse_byte[(unsigned) getchar()];
	      else
		buffer[i] = 0xff;
	    i = XL_buffer_program(flash, bka + ba, buffer);
	    if (i || feof(stdin))
	      break;
	  }
      }
    }

  munmap(flash, XL_MEM_WINDOW);
  close(fdFlash);
  return 0;
}
