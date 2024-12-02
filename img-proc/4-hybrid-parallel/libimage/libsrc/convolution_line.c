#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

void convolution_line(uint8_t const volatile *const line0, uint8_t const volatile *const line1, uint8_t const volatile *const line2,
                      uint8_t const volatile *const line3, uint8_t const volatile *const line4, uint32_t const volatile xsize_in,
                      uint32_t const volatile ysize_in, uint32_t const volatile y,
                      double const *const f, uint32_t const fxsize, uint32_t const fysize, uint8_t volatile *const line_out)
{
  if (fxsize == 5)
  {
    for (uint32_t x = 0; x < xsize_in; x++)
    {
      double r = 0;
      for (uint32_t tx = 0; tx < fxsize; tx++)
      {
        if (x + tx >= fxsize / 2 && x + tx - fxsize / 2 < xsize_in)
        {
          r += y >= fysize / 2               ? f[0 * fxsize + tx] * line0[x + tx - fxsize / 2] : f[0 * fxsize + tx] * line2[x];
          r += y >= fysize / 2 - 1           ? f[1 * fxsize + tx] * line1[x + tx - fxsize / 2] : f[1 * fxsize + tx] * line2[x];
          r += f[2 * fxsize + tx] * line2[x + tx - fxsize / 2];
          r += y + fysize / 2 - 1 < ysize_in ? f[3 * fxsize + tx] * line3[x + tx - fxsize / 2] : f[3 * fxsize + tx] * line2[x];
          r += y + fysize / 2 < ysize_in     ? f[4 * fxsize + tx] * line4[x + tx - fxsize / 2] : f[4 * fxsize + tx] * line2[x];
        }
        else
        {
          // use centre pixel when over the border
          r += f[0 * fxsize + tx] * line2[x];
          r += f[1 * fxsize + tx] * line2[x];
          r += f[2 * fxsize + tx] * line2[x];
          r += f[3 * fxsize + tx] * line2[x];
          r += f[4 * fxsize + tx] * line2[x];
        }
      }

      // clip/saturate to uint8_t
      if (r < 0)
        line_out[x] = 0;
      else if (r > UINT8_MAX)
        line_out[x] = UINT8_MAX;
      else
        line_out[x] = r;
    }
  }
  else
  {
    for (uint32_t x = 0; x < xsize_in; x++)
    {
      double r = 0;
      for (uint32_t tx = 0; tx < fxsize; tx++)
      {
        if (x + tx >= fxsize / 2 && x + tx - fxsize / 2 < xsize_in)
        {
          r += y >= fysize / 2           ? f[0 * fxsize + tx] * line0[x + tx - fxsize / 2] : f[0 * fxsize + tx] * line1[x];
          r += f[1 * fxsize + tx] * line1[x + tx - fxsize / 2];
          r += y + fysize / 2 < ysize_in ? f[2 * fxsize + tx] * line2[x + tx - fxsize / 2] : f[2 * fxsize + tx] * line1[x];
        }
        else
        {
          // use centre pixel when over the border
          r += f[0 * fxsize + tx] * line1[x];
          r += f[1 * fxsize + tx] * line1[x];
          r += f[2 * fxsize + tx] * line1[x];
        }
      }

      // clip/saturate to uint8_t
      if (r < 0)
        line_out[x] = 0;
      else if (r > UINT8_MAX)
        line_out[x] = UINT8_MAX;
      else
        line_out[x] = r;
    }
  }
}
