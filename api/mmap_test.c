#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <endian.h>
#include <byteswap.h>
#include <errno.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <string.h>
#include <ctype.h>
#include "egcpci.h"
#include "egapi.h"
#include "erapi.h"

#define DEVICE "/dev/mrfevr3"

#define MEM_WINDOW_TG 0x00100000
#define MEM_WINDOW    0x00010000

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

int main(int argc, char *argv[])
{
  char s[20];
  int fdEvr;
  void *pa;
  int i, j;
  struct MrfErRegs *pEvr;
  int p = 0, d;
  int chmode;
  char command;
  int dmode = 4; /* access mode: 4 - long word,
		    2 - short, 1 - byte */
  int mmode = 4; /* access mode: 4 - long word,
		    2 - short, 1 - byte */
  char *sm;
  int mem_window_size;

  if (argc < 2)
    {
      printf("Usage: %s /dev/mrfevra3\n", argv[0]);
      return 0;
    }

  fdEvr = open(argv[1], O_RDWR);
  if (!fdEvr)
    {
      printf("Could not open %s\n", argv[1]);
      return 0;
    }
  if (fdEvr)
    {
      /* First we try to allocate TG window size */
      mem_window_size = MEM_WINDOW_TG;
      pa = mmap(0, mem_window_size, PROT_READ | PROT_WRITE, MAP_SHARED, fdEvr, 0);
      /* If this is not successful we try again with EVR window size */
      if (pa == MAP_FAILED)
	{
	  mem_window_size = MEM_WINDOW;
	  pa = mmap(0, mem_window_size, PROT_READ | PROT_WRITE, MAP_SHARED, fdEvr, 0);
	}
      if (pa == MAP_FAILED)
        {
          printf("mmap failed.\n");
        }
      if (pa != MAP_FAILED)
	{
	  printf("Buffer %p\n", pa);
	  pEvr = (struct MrfErRegs *) pa;
	  do
	    {
	      printf("test-> ");
	      fflush(stdout);
	      fgets(s, sizeof(s), stdin);

	      if (s[1] == ' ' || isdigit(s[1]) || isdigit(s[2]))
		p = strtoul(&s[1], NULL, 0);
	      
	      chmode = 0;
	      sm = strchr(s, ',');
	      if (sm != NULL)
		if (isdigit(sm[1]))
		  switch (sm[1])
		    {
		    case '1':
		      chmode = 1;
		      break;
		    case '2':
		      chmode = 2;
		      break;
		    case '4':
		      chmode = 4;
		      break;
		    }

	      command = s[0];
	      switch (command)
		{
		case 'd':
		  if (chmode)
		    dmode = chmode;
		  for (j = 0; j < 8; j++)
		    {
		      printf("%02x:", p);
		      switch (dmode)
			{
			case 1:
			  {
			    unsigned char *pEvrbyte = (unsigned char *) pEvr;
			    for (i = 0; i < 16; i++)
			      printf(" %02x", pEvrbyte[p+i]);
			    break;
			  }
			case 2:
			  {
			    p &= -2;
			    unsigned short *pEvrshort =
			      (unsigned short *) pEvr;
			    for (i = 0; i < 8; i++)
			      printf(" %04x", be16_to_cpu(pEvrshort[p/2+i]));
			    break;
			  }
			case 4:	
			  p &= -4;
			  uint32_t *pEvrlong =
			    (uint32_t *) pEvr;
			  for (i = 0; i < 4; i++)
			    printf(" %08x", be32_to_cpu(pEvrlong[p/4+i]));
			  break;
			}
		      printf("\n");
		      p += 16;
		      p &= mem_window_size - 1;
		    }
		  break;
		case 'm':
		case 'w':
		  if (chmode)
		    mmode = chmode;
		  do
		    {
		      switch (mmode)
			{
			case 1:
			  {
			    unsigned char *pEvrbyte = (unsigned char *) pEvr;
			    if (command == 'm')
			      printf("%08x: %02x ? ", p, pEvrbyte[p]);
			    else
			      printf("%08x: WO ? ", p);
			    fflush(stdout);
			    fgets(s, sizeof(s), stdin);
			    if (isxdigit(s[0]))
			      {
				d = strtoul(s, NULL, 16);
				pEvrbyte[p] = d;
			      }
			    p++;
			    break;
			  }
			case 2:
			  {
			    p &= -2;
			    unsigned short *pEvrshort =
			      (unsigned short *) pEvr;
			    if (command == 'm')
			      printf("%08x: %04x ? ", p,
				     be16_to_cpu(pEvrshort[p/2]));
			    else
			      printf("%08x: WO ? ", p);
			    fflush(stdout);
			    fgets(s, sizeof(s), stdin);
			    if (isxdigit(s[0]))
			      {
				d = strtoul(s, NULL, 16);
				pEvrshort[p/2] = be16_to_cpu(d);
			      }
			    p += 2;
			    break;
			  }
			case 4:
			  p &= -4;
			  uint32_t *pEvrlong =
			    (uint32_t *) pEvr;
			  if (command == 'm')
			    printf("%08x: %08x ? ", p & 0xfffffffc,
				   be32_to_cpu(pEvrlong[p/4]));
			  else
			    printf("%08x: WO ? ", p & 0xfffffffc);
			  fflush(stdout);
			  fgets(s, sizeof(s), stdin);
			  if (isxdigit(s[0]))
			    {
			      d = strtoul(s, NULL, 16);
			      pEvrlong[p/4] = be32_to_cpu(d);
			    }
			  p += 4;
			  break;
			}
		    }
		  while (s[0] != '.');
		  break;
		case 't':
		  {
		    int i;
		    for (i = 1; i <= 14; i++)
		      {
			EvrSetLedEvent(pEvr, 0, i, 1);
			EvrSetPulseMap(pEvr, 0, i, i-1, -1, -1);
			EvrSetPulseParams(pEvr, i-1, 1, 0, 100);
			EvrSetPulseProperties(pEvr, i-1, 0, 0, 0, 1, 1);
			EvrSetUnivOutMap(pEvr, i-1, i-1);
			EvrSetTBOutMap(pEvr, i-1, i-1);
		      }

		    EvrDumpPulses(pEvr, 14);
		    EvrDumpMapRam(pEvr, 0);
		    EvrDumpUnivOutMap(pEvr, 14);
		  }
		  break;
		case '#':
		  {
		    char dst[1024];
		    
		    do
		      {
			memcpy((void *) dst, (void *) &pEvr->Databuf[0], 32);
			memcpy((void *) &pEvr->Databuf[0], (void *) dst, 32);
		      }
		    while(1);
		  }
		  break;
		case 'b':
		  {
		    char dst[1024];
		    
		    pEvr->PCIIrqEnable = be32_to_cpu(EVR_IRQ_PCICORE_ENABLE | 0x80000000);
		    memcpy((void *) dst, (void *) &pEvr->Databuf[0], 1024);
		  }
		  break;
		case 'y':
		  {
		    char src[1024];
		    
		    pEvr->PCIIrqEnable = be32_to_cpu(EVR_IRQ_PCICORE_ENABLE | 0x80000000);
		    memcpy((void *) &pEvr->Databuf[0], (void *) src, 1024);
		  }
		  break;
		case '0':
		  {
		    EvrUnivDlyEnable(pEvr, 0, 1);
		    EvrUnivDlySetDelay(pEvr, 0, p, 0);
		  }
		  break;
		case '1':
		  {
		    EvrUnivDlyEnable(pEvr, 0, 1);
		    EvrUnivDlySetDelay(pEvr, 0, 0, p);
		  }
		  break;
		case '2':
		  {
		    EvrUnivDlyEnable(pEvr, 1, 1);
		    EvrUnivDlySetDelay(pEvr, 1, p, 0);
		  }
		  break;
		case '3':
		  {
		    EvrUnivDlyEnable(pEvr, 1, 1);
		    EvrUnivDlySetDelay(pEvr, 1, 0, p);
		  }
		  break;
		}
	    }
	  while (s[0] != 'q');

	  munmap(0, mem_window_size);
	}
      close(fdEvr);
    }
}
