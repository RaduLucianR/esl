#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <xil_printf.h>
#include <vep_shared_memory_regions.h>
#include <platform.h>
#include <myfifo.h>
#include <image.h>

#define INPUT  (&vep_tile0_partition1_shared_region->fifo)
#define OUTPUT (&vep_tile0_partition3_shared_region->fifo)
#define BUFFER (vep_tile0_partition3_shared_region->buffer)
#define I_BUFFER (vep_tile0_partition3_shared_region->i_buffer)

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
  xil_printf("Convolution top running\n");

	if (myfifo_init(OUTPUT, BUFFER, BUFFER_SIZE, sizeof(token_t)) == NULL)
    return -1;

  while (!myfifo_initialized(INPUT));

	uint32_t fixedPixels = 8;
	uint32_t lines_per_pass = 1;

  while(1)
  {
    while (MEM->sync != 1);

    uint32_t lines_per_pass = 1;
    uint32_t bitsperpixel = MEM->bitsperpixel;

    uint32_t xsize = MEM->xsize;
    uint32_t ysize = MEM->ysize_1;

    // convolution (average or gaussian blur)
    uint8_t offset;
    uint32_t size;
    uint8_t index = 0;
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

    xil_printf("Convolution new top image\n");
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
        // xil_printf("Convolution running line: %u\n", i);
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

    xil_printf("Convolution top done\n");
    while (MEM->sync == 1);
  }


  return 0;
}
