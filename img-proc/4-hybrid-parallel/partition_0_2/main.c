#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <xil_printf.h>
#include <vep_shared_memory_regions.h>
#include <platform.h>
#include <myfifo.h>
#include <image.h>

#define OUTPUT (&vep_tile0_partition2_shared_region->fifo)
#define BUFFER (vep_tile0_partition2_shared_region->buffer)
#define INPUT (&MEM->fifo_in2)

int main()
{
  xil_printf("Greyscale bottom running\n");

  if (myfifo_init(OUTPUT, BUFFER, BUFFER_SIZE, sizeof(token_t)) == NULL)
  {
    xil_printf("Failed to initialize output\n");
    return -1;
  }
  
  xil_printf("Greyscale bottom output initialized\n");
  if (myfifo_init(INPUT, MEM->buffer_in2, BUFFER_SIZE_RGB, sizeof(token_24_t)) == NULL)
  {
    xil_printf("Failed to initialize input fifo 2\n");
    return -1;
  }

  xil_printf("Greyscale bottom input initialized: %u\n", INPUT->m_size);
  // How many lines to process at once
	uint32_t lines_per_pass = 1;

	while(1)
  {
    while (MEM->sync != 1);

    uint32_t xsize = MEM->xsize;
    uint32_t ysize = MEM->ysize_2;

    // How many lines to process at once
    uint32_t bitsperpixel = MEM->bitsperpixel;
    uint32_t img_size = xsize * (bitsperpixel / 8);

    xil_printf("Greyscale bottom new image\n");
    for (uint32_t i = 0; i < ysize; ++i)
    {
      token_24_t* token = (token_24_t*) myfifo_claim_token(INPUT);
      token_t* tokenOut = (token_t*) myfifo_claim_space(OUTPUT);

      greyscale(token->frame, xsize, lines_per_pass, bitsperpixel, tokenOut->frame);

      tokenOut->xsize = xsize;
      tokenOut->ysize = lines_per_pass;
      tokenOut->bitsperpixel = 8;

      myfifo_release_space(INPUT);
      myfifo_release_token(OUTPUT);
    }

    xil_printf("Greyscale bottom done\n");
    while (MEM->sync == 1);
  }

  return 0;
}
