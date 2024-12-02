#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <arm_shared_memory.h>
/* warning: this is the RISC-V memory map!
 * use only in conjuction with arm_shared_memory_write/read
  */
#include "vep_memory_map.h"

extern uint32_t readBMP(char const * const file, uint8_t ** outframe, uint32_t * const x_size, uint32_t * const y_size, uint32_t * const bitsperpixel);
extern uint32_t writeBMP(char const * const file, uint8_t const * const outframe, uint32_t const x_size, uint32_t const y_size, uint32_t const bitsperpixel);

int main(int argc, char ** argv)
{
  if (argc < 2) {
    fprintf(stderr, "Usage: %s infile ...\n" , argv[0]);
    return 1;
  }

  uint32_t nr_files;
  nr_files = argc -1;

  vep_memshared0_shared_region_t * shared_mem = arm_shared_memory_init();

  // read BMP
  for (uint32_t f=1; f <= nr_files; f++) {
    uint32_t xsize_snd, ysize_snd, bitsperpixel_snd;
    uint8_t *frame_snd = NULL;
  
    printf("Processing file %s\n", argv[f]);
  
    if (!readBMP(argv[f], &frame_snd, &xsize_snd, &ysize_snd, &bitsperpixel_snd)) {
      printf("Cannot read file %s\n", argv[f]);
      arm_shared_memory_close();
      return 1;
    }
  
    uint32_t const bytes_snd = xsize_snd * ysize_snd * (bitsperpixel_snd / 8);
    // check if it fits in the memory on the RISC-V side
    if (bytes_snd > MAX_SHARED_SIZE)
    {
      printf("Image of size %ux%ux%u does not fit in the shared mem %u %u\n", xsize_snd, ysize_snd, bitsperpixel_snd, bytes_snd, MAX_SHARED_SIZE);
      arm_shared_memory_close();
      return -1;
    }
    else
    {
      printf("Image of size %ux%ux%u does fit in the shared mem %u %u\n", xsize_snd, ysize_snd, bitsperpixel_snd, bytes_snd, MAX_SHARED_SIZE);
    }
  
    printf("Claimed space\n");
    // send image to RISC-V
    shared_mem->xsize = xsize_snd;
    shared_mem->ysize = ysize_snd;
    shared_mem->bitsperpixel = bitsperpixel_snd;
  
    printf("Writing image\n");
    for (uint32_t i = 0; i < bytes_snd; ++i)
      shared_mem->frame[i] = frame_snd[i];
  
    printf("Released shared_mem\n");
  
    shared_mem->sync = 1;
    // wait for the RISC-V to produce the output data
    while (shared_mem->sync != (2 * N_THREADS) + 1);
  
    printf("Done\n");
  
    // write BMP
    // note that reading & then writing doesn't always result in the same image
    // - a grey-scale (8-bit pixel) image will be written as a 24-bit pixel image too
    // - the header of colour images may change too
    char outfile[200];
    const char *prefix = "out-";
    strcpy(outfile, prefix);
    strcpy(&outfile[strlen(prefix)],argv[f]);
    outfile[strlen(argv[f])+strlen(prefix)] = '\0';
    if (writeBMP(outfile, (uint8_t*)shared_mem->frame, xsize_snd, ysize_snd, 8)) {
      printf("Cannot write file %s\n",outfile);
      arm_shared_memory_close();
      return 1;
    }
  }

  arm_shared_memory_close();
  return 0;
}
