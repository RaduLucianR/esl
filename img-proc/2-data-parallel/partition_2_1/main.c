/*#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <timers.h>
#include <xil_printf.h>
#include <platform.h>
#include <vep_shared_memory_regions.h>
#include <image.h>

#include <math.h>
*/
#define L_MEM_0 (vep_tile2_partition2_shared_region->frame0)
#define L_MEM_1 (vep_tile2_partition2_shared_region->frame1)

#include "../partition_0_1/main.c"
/*
uint64_t volatile const* const g_timer = (uint64_t*) GLOBAL_TIMER;
uint64_t volatile const* const p_timer = (uint64_t*) PARTITION_TIMER;

int main()
{
  xil_printf("Partition running\n") ;

while(1){
  while (MEM->sync != 1);

  uint64_t g_start = *g_timer;
  uint64_t p_start = *p_timer;

  uint32_t lines_per_pass = floor(MEM->ysize / N_THREADS);
  uint32_t pre_offset_lines = lines_per_pass;
  uint32_t line_size = (MEM->xsize * (MEM->bitsperpixel / 8));

  // -1 in RISC
  uint32_t index = TILE_ID * (lines_per_pass * line_size);
  uint32_t start = 0;

  if (TILE_ID == N_THREADS - 1 && MEM->ysize % N_THREADS != 0)
  {
    lines_per_pass += (MEM->ysize % N_THREADS);
  }

  uint32_t orig_lines = lines_per_pass;

  uint32_t xsize = MEM->xsize;
  uint32_t bitsperpixel = MEM->bitsperpixel;
  uint32_t fixedPixels = 8;

  uint32_t offset = bitsperpixel == 8 ? 2 : 3;
  if (TILE_ID == 0)
  {
    lines_per_pass += offset;
  }
  // Last
  else if (TILE_ID == N_THREADS - 1)
  {
    start = offset * line_size;
    index = index - start;
    lines_per_pass += offset;
  }
  else
  {
    start = offset * line_size;
    index = index - start;
    // Go up 1 so need to go down 2
    lines_per_pass += 2 * offset;
  }

  xil_printf("Starting greyscale\n");
  greyscale(MEM->frame + index, xsize, lines_per_pass, bitsperpixel, L_MEM_0(TILE_ID));

  // convolution (average or gaussian blur)
  double const * filter;
  uint32_t size;
  if (bitsperpixel == 8) {
    filter = conv_avgxy3;
    size = 3;
  } else {
    filter = conv_gaussianblur5;
    size = 5;

    // Since the image is now greyscale, we only need 1 third of the space
    line_size = MEM->xsize;

    if (TILE_ID != 0)
      start = offset * line_size;
  }

  index = TILE_ID * (pre_offset_lines * line_size);

  // Wait until all partitions are done with greyscale before continuing
  ++MEM->sync;
  xil_printf("Wait until all partitions are done with greyscale before continuing: %u\n", MEM->sync);
  while(MEM->sync != (N_THREADS + 1));

  xil_printf("Convolution started with size %u\n", size);
  convolution(L_MEM_0(TILE_ID), xsize, lines_per_pass, fixedPixels, filter, size, size, L_MEM_1(TILE_ID));

  // sobel
  uint8_t const threshold = (bitsperpixel == 8 ? 100 : 128);
  xil_printf("Sobel started with threshold: %u\n", threshold);
  sobel(L_MEM_1(TILE_ID), xsize, lines_per_pass, fixedPixels, threshold, L_MEM_0(TILE_ID));

  // overlay
  xil_printf("%u Overlay started at: %u lines: %u original: %u\n", TILE_ID, index, lines_per_pass, orig_lines);
  overlay(L_MEM_1(TILE_ID) + start, xsize, orig_lines, fixedPixels,
          L_MEM_0(TILE_ID) + start, xsize, orig_lines, fixedPixels, \
          0, 0, 0.7, MEM->frame + index);

  uint32_t diff_p = *p_timer - p_start;
  uint32_t diff_g = *g_timer - g_start;
  xil_printf("Tile 2 frame: %u %u\n", diff_g, diff_p);
  ++MEM->sync;
  } 
return 0;
}
*/
