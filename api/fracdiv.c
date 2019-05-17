/*
  fraqdiv.c -- Micro-Research Event Receiver
               Frequency reference control word computation

  Author: Jukka Pietarinen (MRF)
  Date:   16.12.2011

*/

#include <stdint.h>
#include <errno.h>
#include <stdio.h>
#include <math.h>
#include "erapi.h"

#define  REF_OSC  (24.000)

int tablePostDivSel[32] = {
  1,  3,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
  16, 18, 20, 22, 24, 26, 28, 30, 32, 36, 40, 44, 48, 52, 56, 60};
int tableMdivSel[8] = {
  16, 16, 18, 17, 31, 14, 32, 15};

double cw_to_freq(long cw)
{
  double freq;
  int DivSel, Qp, Qp_1, PostDivSel, MdivSel, NdivSel;

  DivSel = ((cw & 0x0003C000) >> 14) + 17;
  Qp = ((cw & 0x0F800000) >> 23);
  Qp_1 = ((cw & 0x007C0000) >> 18);
  PostDivSel = tablePostDivSel[((cw & 0x000007C0) >> 6)];
  MdivSel = tableMdivSel[(cw & 0x00000007)];
  NdivSel = tableMdivSel[(cw & 0x00000038) >> 3];

#ifdef DEBUG
  printf("Qp %d, Qp-1 %d, DivSel %d (0x%x), PostDivSel %d (0x%x), NdivSel %d, MdivSel %d\n",
	 Qp, Qp_1, DivSel, DivSel-17, tablePostDivSel[PostDivSel], PostDivSel,
	 NdivSel, MdivSel);
#endif

  freq = REF_OSC * ((double) DivSel - (double) Qp_1/((double) Qp_1+Qp));
#ifdef DEBUG
  printf("VCO freq %f\n", freq);
#endif
  if (freq < 540.0)
    {
      printf("VCO frequency too low %f < 540 MHz\n", freq);
      return -1.0;
    }
  if (freq > 729.0)
    {
      printf("VCO frequency too high %f > 729 MHz\n", freq);
      return -1.0;
    }
  if ((MdivSel <= 18 && NdivSel >= 31) ||
      (NdivSel <= 18 && MdivSel >= 31) ||
      (NdivSel == 18 && MdivSel == 14))
    {
      printf("Invalid MdivSel %d, Ndivsel %d combination.\n",
	     MdivSel, NdivSel);
      return -1.0;
    }
  freq = (freq / PostDivSel) * (NdivSel) / (MdivSel);
#ifdef DEBUG
  printf("Frequency set to %f MHz\n", freq);
#endif
  return freq;
}

long freq_to_cw(double freq)
{
  long cw;
  int i;
  int DivSel, Qp, Qp_1, PostDivSel, MdivSel, NdivSel;
  int tryDivSel, tryQp, tryQp_1;
  int minPostDivSel, maxPostDivSel;
  double best_err, err, f, fbest;
  int tryM, tryN;

#ifdef DEBUG
  printf("Frequency %f\n", freq);
#endif

  MdivSel = 5;
  NdivSel = 5;
  minPostDivSel = 31;
  maxPostDivSel = 31;
  for (i = 0; i < 32; i++)
    {
      if (i != 1) /* We skip value 3 at location 1 */
	{
	  if (freq * tablePostDivSel[i] < 540.0)
	    minPostDivSel = i;
	  if (freq * tablePostDivSel[i] < 729.0)
	    maxPostDivSel = i;
	}
    }
  minPostDivSel++;
#ifdef DEBUG
  printf("PostDivSel in range %d to %d\n",
	 tablePostDivSel[minPostDivSel],
	 tablePostDivSel[maxPostDivSel]);
#endif

  best_err = 1000.0;

  for (i = minPostDivSel; i <= maxPostDivSel; i++)
    {
      tryDivSel = ((freq * tablePostDivSel[i]) / REF_OSC) + 0.8;
#ifdef DEBUG
      printf("Trying PostDivSel %d and DivSel %d.\n", tablePostDivSel[i],
	     tryDivSel);
#endif
      for (tryQp = 1; tryQp < 32; tryQp++)
	for (tryQp_1 = 0; tryQp_1 < 32; tryQp_1++)
	  for (tryM = 0; tryM < 8; tryM++)
	    for (tryN = 0; tryN < 8; tryN++)
	      if (!((tryM <= 18 && tryN >= 31) ||
		    (tryN <= 18 && tryM >= 31) ||
		    (tryN == 18 && tryM == 14)))
		{
		  f = (REF_OSC * ((double) tryDivSel - (double) tryQp_1 / ((double) tryQp_1 + (double) tryQp)) / 
		    (double) tablePostDivSel[i]) * (double) tableMdivSel[tryN] / (double) tableMdivSel[tryM];
		  err = fabs(f - freq);
		  if (err < best_err)
		    {
#ifdef DEBUG
		      printf("Test freq %f, Err %f, Qp %d, Qp-1 %d, DivSel %d (0x%x), PostDivSel %d (0x%x), NdivSel %d (0x%x), MdivSel %d (0x%x)\n", f, err, tryQp, tryQp_1, tryDivSel, tryDivSel-17, tablePostDivSel[i], i,
			     tableMdivSel[tryN], tryN, tableMdivSel[tryM], tryM);
#endif
		      PostDivSel = i;
		      DivSel = tryDivSel;
		      Qp_1 = tryQp_1;
		      Qp = tryQp;
		      MdivSel = tryM;
		      NdivSel = tryN;
		      best_err = err;
		      fbest = f;
		    }
		}
    }

#ifdef DEBUG
  printf("Best result:\n");
  printf("Qp %d, Qp-1 %d, DivSel %d (0x%x), PostDivSel %d (0x%x), NdivSel %d (0x%x), MdivSel %d (0x%x)\n",
	 Qp, Qp_1, DivSel, DivSel-17, tablePostDivSel[PostDivSel], PostDivSel,
	 tableMdivSel[NdivSel], NdivSel, tableMdivSel[MdivSel], MdivSel);
#endif
  cw = (Qp << 23) +
    (Qp_1 << 18) +
    ((DivSel)-17 << 14) +
    (PostDivSel << 6) +
    (NdivSel << 3) + 
    MdivSel;

#ifdef DEBUG
  printf("Configuration word 0x%08x, error %f ppm\n", cw, (fbest / freq - 1.0) * 1.0E6);
#endif

  return cw;
}
