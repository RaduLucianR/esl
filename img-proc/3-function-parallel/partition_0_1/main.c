#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <xil_printf.h>
#include <vep_shared_memory_regions.h>
#include <platform.h>
#include <myfifo.h>
#include <image.h>

#define INPUT  (&vep_tile2_partition1_shared_region->fifo)
#define OUTPUT (&(MEM->fifo1))

int main()
{
  xil_printf("Overlay running\n");

  if (myfifo_init(OUTPUT, MEM->buffer1, BUFFER_SIZE, sizeof(token_t)) == NULL)
  {
    xil_printf("Failed to initialize output\n");
    return -1;
  }

  while(!myfifo_initialized(INPUT));

  while (1)
  {
    // Space is released by the sobel function
    xil_printf("Overlay new image\n");
    while (MEM->sync != 1);

    uint32_t lines_per_pass = 1;
    uint32_t xsize = MEM->xsize;
    uint32_t ysize = MEM->ysize;

    uint32_t img_size = MEM->xsize;

    uint8_t const fixedPixels = 8;

    for (uint32_t i = 0; i < ysize; ++i)
    {
      token_t* token = (token_t*) myfifo_claim_token(INPUT);
      token_t* tokenOut = (token_t*) myfifo_claim_space(OUTPUT);

      overlay(token->frame, xsize, lines_per_pass, fixedPixels,
              token->frame + LINE_SIZE, xsize, lines_per_pass, fixedPixels,
              0, 0, 0.7, tokenOut->frame);
      
      myfifo_release_token(OUTPUT);
      myfifo_release_space(INPUT);
    }

    MEM->sync = 2;
    xil_printf("Overlay done\n");
  }

  xil_printf("Overlay stopped\n");
  return 0;
}
