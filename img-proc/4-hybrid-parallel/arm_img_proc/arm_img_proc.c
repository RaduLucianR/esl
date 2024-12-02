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

  myfifo_t volatile * const fifo_in1 = &shared_mem->fifo_in1;
  myfifo_t volatile * const fifo_in2 = &shared_mem->fifo_in2;
  token_24_t volatile * const buffer_in1 = shared_mem->buffer_in1;
  token_24_t volatile * const buffer_in2 = shared_mem->buffer_in2;
  
  myfifo_t volatile * const fifo_out1 = &shared_mem->fifo_out1;
  myfifo_t volatile * const fifo_out2 = &shared_mem->fifo_out2;
  token_t volatile * const buffer_out1 = shared_mem->buffer_out1;
  token_t volatile * const buffer_out2 = shared_mem->buffer_out2;

  printf("Waiting until the RISC-V has initialised top input fifo: %u\n", fifo_in1->m_size);
  while (!myfifo_initialized(fifo_in1));
  printf("Waiting until the RISC-V has initialised top output fifo: %u\n", fifo_out1->m_size);
  while (!myfifo_initialized(fifo_out1));
  printf("Waiting until the RISC-V has initialized bottom input fifo: %u\n", fifo_in2->m_size);
  while (!myfifo_initialized(fifo_in2));
  printf("Waiting until the RISC-V has initialised bottom output fifo: %u\n", fifo_out2->m_size);
  while (!myfifo_initialized(fifo_out2));


  if (fifo_in1 == NULL || fifo_in2 == NULL || fifo_out1 == NULL || fifo_out2 == NULL)
  {
    printf("Failed to initialize fifos\n");
    return -1;
  }

  printf("Fifos initiliazed\n");

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

    memset(shared_mem->buffer_in1, 0, sizeof(shared_mem->buffer_in1));
    memset(shared_mem->buffer_in2, 0, sizeof(shared_mem->buffer_in2));
    memset(shared_mem->buffer_out1, 0, sizeof(shared_mem->buffer_out1));
    memset(shared_mem->buffer_out2, 0, sizeof(shared_mem->buffer_out2));

    myfifo_clean(fifo_in1);
    myfifo_clean(fifo_in2);
    myfifo_clean(fifo_out1);
    myfifo_clean(fifo_out2);

    uint32_t const bytes_snd = xsize_snd * (bitsperpixel_snd / 8);

    // Determine image split and write general info in shared mem
    uint32_t start_1 = 0;
	  uint32_t end_1 = ysize_snd < TOP_OFFSET ? 0 : (ysize_snd / 2) + TOP_OFFSET;

  	uint32_t start_2 = ysize_snd < TOP_OFFSET ? 0 : (ysize_snd / 2) - TOP_OFFSET;
		uint32_t end_2 = ysize_snd;

    uint32_t offset = ysize_snd < TOP_OFFSET ? 0 : TOP_OFFSET;

    shared_mem->xsize = xsize_snd;
    shared_mem->ysize_1 = end_1;
    shared_mem->ysize_2 = end_2 - start_2;
    shared_mem->bitsperpixel = bitsperpixel_snd;
    shared_mem->sync = 1;

    uint8_t out_buffer[205000];
    
    uint32_t i = start_1 , k = start_2, index1 = start_1, index2 = start_2;
    
    // Sending and receiving tokens until finished
    while (index1 < end_1 || index2 < end_2)
    {
      if (i < end_1 && myfifo_spaces(fifo_in1) != 0)
      {
        uint32_t token_index = myfifo_claim_space(fifo_in1);

        printf("Claimed fifo_in1 space: %u\n", token_index);
        // send image to RISC-V
        buffer_in1[token_index].xsize = xsize_snd;
        buffer_in1[token_index].ysize = ysize_snd;
        buffer_in1[token_index].bitsperpixel = bitsperpixel_snd;

        printf("Writing line\n");
        for (uint32_t j = 0; j < bytes_snd; ++j)
          buffer_in1[token_index].frame[j] = (frame_snd + (i * bytes_snd))[j];

        myfifo_release_token(fifo_in1);
        printf("Released shared_mem\n");
        i++;
      }
      
      if (k < end_2 && myfifo_spaces(fifo_in2) != 0)
      {
        uint32_t token_index = myfifo_claim_space(fifo_in2);

        printf("Claimed space fifo_in2: %u\n", token_index);
        // send image to RISC-V
        buffer_in2[token_index].xsize = xsize_snd;
        buffer_in2[token_index].ysize = ysize_snd;
        buffer_in2[token_index].bitsperpixel = bitsperpixel_snd;

        printf("Writing line\n");
        for (uint32_t j = 0; j < bytes_snd; ++j)
          buffer_in2[token_index].frame[j] = (frame_snd + (k * bytes_snd))[j];

        myfifo_release_token(fifo_in2);
        printf("Released shared_mem\n");
        k++;
      }
      
      if (myfifo_tokens(fifo_out1) != 0)
      {
        uint32_t in = myfifo_claim_token(fifo_out1);

        printf("Read fifo_out1 token: %u, %u\n", in, index1);
        if (index1 <= end_1 - offset)
        {
          for (uint32_t j = 0; j < xsize_snd; ++j)
            (out_buffer + (index1 * xsize_snd))[j] = buffer_out1[in].frame[j];
        }
        index1++;
        myfifo_release_space(fifo_out1);
      }
      
      if (myfifo_tokens(fifo_out2) != 0)
      {
        uint32_t in = myfifo_claim_token(fifo_out2);

        printf("Read fifo_out2 token: %u, %u\n", in, index2);
        if (index2 >=  start_2 + offset)
        {
          for (uint32_t j = 0; j < xsize_snd; ++j)
            (out_buffer + (index2 * xsize_snd))[j] = buffer_out2[in].frame[j];
        } 
        index2++;
        myfifo_release_space(fifo_out2);
      }
      
    }
    
    
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
    if (writeBMP(outfile, (uint8_t*)out_buffer, xsize_snd, ysize_snd, 8)) {
      printf("Cannot write file %s\n",outfile);
      arm_shared_memory_close();
      return 1;
    }
  }

  arm_shared_memory_close();
  return 0;
}

