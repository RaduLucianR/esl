#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <timers.h>
#include <xil_printf.h>
#include <platform.h>
#include <vep_shared_memory_regions.h>
#include <image.h>
#include <myfifo.h>
#include <math.h>

#define BUFFER (((vep_tile1_partition1_shared_region_t*)VEP_TILE1_PARTITION1_SHARED_REGION_REMOTE_START)->buffer)
#define INPUT  (&vep_tile0_partition2_shared_region->fifo)
#define OUTPUT (&((vep_tile1_partition1_shared_region_t*)VEP_TILE1_PARTITION1_SHARED_REGION_REMOTE_START)->fifo)

#define I_BUFFER (vep_tile1_partition1_shared_region->i_buffer)

uint8_t const indices3[] = {
    1, 2, 0,
    2, 0, 1,
    0, 1, 2,
};

uint8_t const indices5[] = {
    1, 2, 3, 4, 0,
    2, 3, 4, 0, 1,
    3, 4, 0, 1, 2,
    4, 0, 1, 2, 3,
    0, 1, 2, 3, 4,
};

int main()
{
  xil_printf("Convolution running\n") ;

  if (myfifo_init(OUTPUT, BUFFER, BUFFER_SIZE, sizeof(token_t)) == NULL)
    return -1;

  while (!myfifo_initialized(INPUT));

  while(1)
  {
    xil_printf("Convolution new image\n");
    while (MEM->sync != 1);

    uint32_t lines_per_pass = 1;
    uint32_t bitsperpixel = MEM->bitsperpixel;

    uint32_t xsize = MEM->xsize;
    uint32_t ysize = MEM->ysize;
    uint32_t img_size = MEM->xsize;

    // convolution (average or gaussian blur)
    uint8_t offset;
    uint32_t size;
    uint8_t index = 0;
    uint32_t fixedPixels = 8;
    double const * filter;
    uint8_t const * indices;

    if (bitsperpixel == fixedPixels) {
      filter = conv_avgxy3;
      size = 3;
      offset = 1;
      indices = indices3;
    } else {
      filter = conv_gaussianblur5;
      size = 5;
      offset = 2;
      indices = indices5;
    }

    token_t* token = NULL;
    token_t* tokenOut = NULL;

    for (uint32_t i = 0; i < ysize + offset; ++i)
    {
      if (i < ysize)
        token = (token_t*) myfifo_claim_token(INPUT);

      if (i >= offset)
      {
        tokenOut = (token_t*) myfifo_claim_space(OUTPUT);

        tokenOut->xsize = xsize;
        tokenOut->ysize = 1;
        tokenOut->bitsperpixel = fixedPixels;
      }

      // Copy to the internal buffer
      for (int j = 0; j < xsize; ++j)
        (I_BUFFER + (index * LINE_SIZE))[j] = token->frame[j];

      // Always take the middle line
      if (i >= offset)
      {
        convolution_line(
          I_BUFFER + (indices[(size * index) + 0] * LINE_SIZE),
          I_BUFFER + (indices[(size * index) + 1] * LINE_SIZE),
          I_BUFFER + (indices[(size * index) + 2] * LINE_SIZE),
          I_BUFFER + (indices[(size * index) + 3] * LINE_SIZE),
          I_BUFFER + (indices[(size * index) + 4] * LINE_SIZE),
          xsize, ysize, i - offset,
          filter, size, size,
          tokenOut->frame);
        
        myfifo_release_token(OUTPUT);
      }

      if (i < ysize)
        myfifo_release_space(INPUT);

      index = (index + 1 > (size - 1)) ? 0 : index + 1;
    }

    xil_printf("Convolution done\n");
    while (MEM->sync == 1);
  }

  xil_printf("Stopped convolution\n");

  return 0;
}

