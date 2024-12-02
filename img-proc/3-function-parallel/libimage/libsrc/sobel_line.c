#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include "../include/image.h"

void sobel_line(uint8_t const volatile * const line0, uint8_t const volatile * const line1, uint8_t const volatile * const line2,
           uint32_t const xsize_in, uint32_t const ysize_in, uint32_t const y,
           uint8_t const threshold, uint8_t volatile * const line_out)
{
  const int fxsize = 3, fysize = 3;
  int32_t const sobelx[] = {
    -1, 0, 1,
    -2, 0, 2,
    -1, 0, 1,
  };
  int32_t const sobely[] = {
    1,  2,  1,
    0,  0,  0,
   -1, -2, -1
  };


  for (uint32_t x = 0; x < xsize_in; x++) {
    // don't create edge at borders
    if (x < (fxsize+1)/2 || x >= xsize_in-(fxsize+1)/2 || y < (fysize+1)/2 || y >= ysize_in-(fysize+1)/2)
    {
      line_out[x] = 0;
    }
    else
    {
      int32_t xr = 0, yr = 0;
      for (int32_t tx=0; tx < fxsize; tx++)
      {
          xr += sobelx[0 * fxsize + tx] * line0[x + tx - fxsize/2];
          xr += sobelx[1 * fxsize + tx] * line1[x + tx - fxsize/2];
          xr += sobelx[2 * fxsize + tx] * line2[x + tx - fxsize/2];
          yr += sobely[0 * fxsize + tx] * line0[x + tx - fxsize/2];
          yr += sobely[1 * fxsize + tx] * line1[x + tx - fxsize/2];
          yr += sobely[2 * fxsize + tx] * line2[x + tx - fxsize/2];
      }

      // gradient magnitude
      double const r = sqrt(yr*yr + xr*xr);
      // clip/saturate from int32_t to uint8_t
      if (r < threshold) line_out[x] = 0;
      else if (r > UINT8_MAX) line_out[x] = UINT8_MAX;
      else line_out[x] = UINT8_MAX;
    }
  }
}
