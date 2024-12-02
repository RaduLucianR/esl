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

#define BUFFER (((vep_tile0_partition2_shared_region_t*)VEP_TILE0_PARTITION2_SHARED_REGION_REMOTE_START)->buffer)
#define INPUT  (&MEM->fifo)
#define OUTPUT (&((vep_tile0_partition2_shared_region_t*)VEP_TILE0_PARTITION2_SHARED_REGION_REMOTE_START)->fifo)

int main()
{
  xil_printf("Greyscale running\n");

  if (myfifo_init(OUTPUT, BUFFER, BUFFER_SIZE, sizeof(token_t)) == NULL)
    return -1;

  if (myfifo_init(INPUT, MEM->buffer, BUFFER_SIZE_RGB, sizeof(token_t)) == NULL)
  {
    xil_printf("Failed to initialize input\n");
    return -1;
  }

  while(1)
  {
    // Space is released by the sobel function
    while (MEM->sync != 1);

    uint32_t xsize = MEM->xsize;
    uint32_t ysize = MEM->ysize;

    // How many lines to process at once
    uint32_t lines_per_pass = 1;
    uint32_t bitsperpixel = MEM->bitsperpixel;

    uint32_t img_size = xsize * (bitsperpixel / 8);

    xil_printf("Greyscale new image\n");
    for (uint32_t i = 0; i < ysize; ++i)
    {
      token_24_t* token = (token_24_t*) myfifo_claim_token(INPUT);
      token_t* tokenOut = (token_t*) myfifo_claim_space(OUTPUT);

      greyscale(token->frame, xsize, lines_per_pass, bitsperpixel, tokenOut->frame);

      tokenOut->xsize = xsize;
      tokenOut->ysize = lines_per_pass;
      tokenOut->bitsperpixel = bitsperpixel;
      
      myfifo_release_space(INPUT);
      myfifo_release_token(OUTPUT);
    }

    xil_printf("Greyscale done\n");
    while (MEM->sync == 1);
  }

  xil_printf("Stopped greyscale\n");

  return 0;
}


