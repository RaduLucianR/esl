#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <timers.h>
#include <xil_printf.h>
#include <platform.h>
#include <vep_shared_memory_regions.h>
#include <image.h>

#define L_MEM vep_tile1_partition2_shared_region->frame

int main (void)
{
  int a = 1;
  xil_printf("Image processing started\n");

  while (1)
  {
    while (MEM->sync != CONSUMER_IS_OWNER);

    uint32_t xsize = MEM->xsize;
    uint32_t ysize = MEM->ysize;
    uint32_t bitsperpixel = MEM->bitsperpixel;
    uint32_t fixedPixels = 8;

    greyscale(MEM->frame, xsize, ysize, bitsperpixel, L_MEM);

    // convolution (average or gaussian blur)
    double const * filter;
    uint32_t size;
    if (bitsperpixel == 8) {
      filter = conv_avgxy3;
      size = 3;
    } else {
      filter = conv_gaussianblur5;
      size = 5;
    }

    xil_printf("Convolution started with size %u\n", size);
    convolution(L_MEM, xsize, ysize, fixedPixels, filter, size, size, MEM->frame);

    // sobel
    uint8_t const threshold = (bitsperpixel == 8 ? 100 : 128);
    xil_printf("Sobel started with threshold: %u\n", threshold);
    sobel(MEM->frame, xsize, ysize, fixedPixels, threshold, L_MEM);

    // overlay
    xil_printf("Overlay started\n");
    overlay(MEM->frame, xsize, ysize, fixedPixels, L_MEM, xsize, ysize, fixedPixels, 0, 0, 0.7, MEM->frame);

    MEM->sync = PRODUCER_IS_OWNER;
  }
}
