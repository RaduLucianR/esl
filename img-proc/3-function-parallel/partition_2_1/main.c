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

#define BUFFER (((vep_tile2_partition1_shared_region_t*)VEP_TILE2_PARTITION1_SHARED_REGION_REMOTE_START)->buffer)
#define INPUT  (&vep_tile1_partition1_shared_region->fifo)
#define OUTPUT (&((vep_tile2_partition1_shared_region_t*)VEP_TILE2_PARTITION1_SHARED_REGION_REMOTE_START)->fifo)

#define I_BUFFER (vep_tile2_partition1_shared_region->i_buffer)

uint8_t const indices3[] = {
    1, 2, 0,
    2, 0, 1,
    0, 1, 2,
};

int main()
{
  xil_printf("Sobel running\n") ;
  if (myfifo_init(OUTPUT, BUFFER, BUFFER_SIZE, sizeof(token_sbl_t)) == NULL)
    return -1;

  while (!myfifo_initialized(INPUT));

  while(1)
  {
    xil_printf("Sobel new image\n");
    while (MEM->sync != 1);

    uint32_t lines_per_pass = 1;

    uint32_t xsize = MEM->xsize;
    uint32_t ysize = MEM->ysize;

    uint32_t img_size = MEM->xsize;

    uint8_t const size = 3;
    uint8_t const fixedPixels = 8;
    uint8_t const threshold = (MEM->bitsperpixel == 8 ? 100 : 128);

    uint8_t index = 0;
    uint8_t offset = 1;
    
    token_t* token = NULL;
    token_sbl_t* sobelOut = NULL;

    for (uint32_t i = 0; i < ysize + offset; ++i)
    {
      if (i < ysize)
        token = (token_t*) myfifo_claim_token(INPUT);

      for (int j = 0; j < xsize; ++j)
        (I_BUFFER + (index * LINE_SIZE))[j] = token->frame[j];

      if (i >= offset)
      {
        sobelOut = (token_sbl_t*) myfifo_claim_space(OUTPUT);
        sobelOut->xsize = xsize;
        sobelOut->ysize = 1;
        sobelOut->bitsperpixel = fixedPixels;

        // Just copy the convolution output
        for (int j = 0; j < xsize; ++j)
          sobelOut->frame[j] = (I_BUFFER + (indices3[(size * index) + 1] * LINE_SIZE))[j];

        sobel_line(
          I_BUFFER + (indices3[(size * index) + 0] * LINE_SIZE),
          I_BUFFER + (indices3[(size * index) + 1] * LINE_SIZE),
          I_BUFFER + (indices3[(size * index) + 2] * LINE_SIZE),
          xsize, ysize, i - offset,
          threshold, (sobelOut->frame + LINE_SIZE));
          
        myfifo_release_token(OUTPUT);
      }

      if (i < ysize)
        myfifo_release_space(INPUT);

      index = (index + 1 > (size - 1)) ? 0 : index + 1;
    }

    xil_printf("Sobel done\n");
    while (MEM->sync == 1);
  }

  xil_printf("Stopped sobel\n");

  return 0;
}
