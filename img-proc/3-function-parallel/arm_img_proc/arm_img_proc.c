#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <arm_shared_memory.h>
/* warning: this is the RISC-V memory map!
 * use only in conjuction with arm_shared_memory_write/read
  */
#include "vep_memory_map.h"
#include "myfifo.h"
#include <time.h>

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

  myfifo_t volatile * const fifo = &shared_mem->fifo;
  myfifo_t volatile * const fifo1 = &shared_mem->fifo1;
  token_t volatile * const buffer = shared_mem->buffer;
  token_t volatile * const buffer1 = shared_mem->buffer1;

  printf("Waiting until the RISC-V has initialised both fifos: %u\n", fifo->m_size);
  while (!myfifo_initialized(fifo));
  printf("Waiting until the RISC-V has initialised both fifos: %u\n", fifo1->m_size);
  while (!myfifo_initialized(fifo1));


  if (fifo == NULL || fifo1 == NULL)
  {
    printf("Failed to initialize fifos\n");
    return -1;
  }

  printf("Fifos initiliazed\n");

  // read BMP
  for (uint32_t f=1; f <= nr_files; f++) {
    uint32_t xsize_snd, ysize_snd, bitsperpixel_snd;
    uint8_t *frame_snd = NULL;


    if (!readBMP(argv[f], &frame_snd, &xsize_snd, &ysize_snd, &bitsperpixel_snd)) {
      printf("Cannot read file %s\n", argv[f]);
      arm_shared_memory_close();
      return 1;
    }

    clock_t t = clock();
    memset(shared_mem->buffer, 0, sizeof(shared_mem->buffer));
    memset(shared_mem->buffer1, 0, sizeof(shared_mem->buffer1));

    myfifo_clean(fifo);
    myfifo_clean(fifo1);

    uint32_t const bytes_snd = xsize_snd * (bitsperpixel_snd / 8);

    // Write general info in shared mem
    shared_mem->xsize = xsize_snd;
    shared_mem->ysize = ysize_snd;
    shared_mem->bitsperpixel = bitsperpixel_snd;
    shared_mem->sync = 1;

    uint8_t out_buffer[205000];
    uint32_t index = 0;
    uint32_t offset = 4;
    for (uint32_t i = 0; i < ysize_snd + offset; ++i)
    {
      if (i < ysize_snd)
      {
        uint32_t token_index = myfifo_claim_space(fifo);

        // send image to RISC-V
        buffer[token_index].xsize = xsize_snd;
        buffer[token_index].ysize = ysize_snd;
        buffer[token_index].bitsperpixel = bitsperpixel_snd;

        for (uint32_t j = 0; j < bytes_snd; ++j)
          buffer[token_index].frame[j] = (frame_snd + (i * bytes_snd))[j];

        myfifo_release_token(fifo);
      }

      // Risc informs that something was written to this token through the flag in bitsperpixel
      if (i >= offset)
      {
        uint32_t in = myfifo_claim_token(fifo1);

        for (uint32_t j = 0; j < xsize_snd; ++j)
          (out_buffer + (index * xsize_snd))[j] = buffer1[in].frame[j];

        index++;
        myfifo_release_space(fifo1);
      }
    }

    // wait for the RISC-V to produce the output data
    while (shared_mem->sync != 2);
    
    t = clock() - t;
    double time_taken = ((double)t)/CLOCKS_PER_SEC; // in seconds
    printf("%s %lf\n", argv[f], time_taken);

    // write BMP
    // note that reading & then writing doesn't always result in the same image
    // - a grey-scale (8-bit pixel) image will be written as a 24-bit pixel image too
    // - the header of colour images may change too
    char outfile[200];
    const char *prefix = "out-";
    strcpy(outfile, prefix);
    strcpy(&outfile[strlen(prefix)],argv[f]);
    outfile[strlen(argv[f])+strlen(prefix)] = '\0';
    if (writeBMP(outfile, (uint8_t*)out_buffer, xsize_snd, ysize_snd, 8)) {
      printf("Cannot write file %s\n",outfile);
      arm_shared_memory_close();
      return 1;
    }
  }

  arm_shared_memory_close();
  return 0;
}

