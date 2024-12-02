#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <xil_printf.h>
#include <vep_shared_memory_regions.h>
#include <platform.h>
#include <myfifo.h>
#include <image.h>

#define INPUT  (&vep_tile0_partition3_shared_region->fifo)
#define OUTPUT (&vep_tile2_partition1_shared_region->fifo)
#define BUFFER (vep_tile2_partition1_shared_region->buffer)
#define I_BUFFER (vep_tile2_partition1_shared_region->i_buffer)

uint8_t const indices3[] = {
    1, 2, 0,
    2, 0, 1,
    0, 1, 2,
};

int main()
{
  xil_printf("Sobel top running\n");

  if (myfifo_init(OUTPUT, BUFFER, BUFFER_SIZE, sizeof(token_sbl_t)) == NULL)
    return -1;

  while (!myfifo_initialized(INPUT));

  uint8_t const size = 3;
	uint32_t lines_per_pass = 1;
  uint8_t const fixedPixels = 8;

  while(1)
  {
    while (MEM->sync != 1);


    uint32_t xsize = MEM->xsize;
    uint32_t ysize = MEM->ysize_1;
    uint8_t const threshold = (MEM->bitsperpixel == 8 ? 100 : 128);

    uint8_t index = 0;
    uint8_t offset = 1;

    token_t* token = NULL;
    token_sbl_t* sobelOut = NULL;

    xil_printf("Sobel top new image\n");
    for (uint32_t i = 0; i < ysize + offset; ++i)
    {
      if (i < ysize)
        token = (token_t*) myfifo_claim_token(INPUT);

      for (int j = 0; j < xsize; ++j)
        (I_BUFFER + (index * LINE_SIZE))[j] = token->frame[j];

      if (i >= offset)
      {
        // xil_printf("Sobel running on line: %u\n", i);
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

    xil_printf("Sobel top done\n");
    while (MEM->sync == 1);
  }

  return 0;
}
