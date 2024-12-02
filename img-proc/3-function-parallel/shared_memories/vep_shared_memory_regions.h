#ifndef __VEP_PRIVATE_MEMORY_H_
#define __VEP_PRIVATE_MEMORY_H_

#include <stddef.h>
#include <stdint.h>
#include "vep_memory_map.h"
#include "../libmyfifo/include/myfifo.h"

// IMPORTANT: to use a private memory region you must:
// 1- declare the shared memory region in the vep-config.txt file
//    without this, the region will not be declared in the memory map
// 2- modify the typedef of the struct containing all data to be placed in the region
// 
// all fields are set to 0 when the VEP is loaded
// (it may therefore be useful to have an 'initialized' as first field in the struct)
//
// to debug, use the print_memory_map() function to print the VEP memory map
// it should be the same as the vep-config.txt from which it is generated

#define N_THREADS 4
#define PRODUCER_IS_OWNER 0
#define CONSUMER_IS_OWNER 1
#define MAX_SHARED_SIZE (VEP_MEMSHARED0_SHARED_REGION_SIZE - 20)
#define LINE_SIZE_RGB 1200 
#define LINE_SIZE 420
#define BUFFER_SIZE_RGB 6
#define BUFFER_SIZE 10
#define MEM vep_memshared0_shared_region

typedef struct _token_t {
  uint32_t xsize;
  uint32_t ysize;
  uint8_t bitsperpixel;
  uint8_t frame[LINE_SIZE];
} token_t;

typedef struct _token_24_t {
  uint32_t xsize;
  uint32_t ysize;
  uint8_t bitsperpixel;
  uint8_t frame[LINE_SIZE_RGB];
} token_24_t;


typedef struct _token_sbl_t {
	uint32_t xsize;
	uint32_t ysize;
	uint8_t bitsperpixel;
	uint8_t frame[LINE_SIZE * 2];
} token_sbl_t;

#ifdef VEP_MEMSHARED0_SHARED_REGION_REMOTE_START
typedef volatile struct {
  uint32_t xsize;
  uint32_t ysize;
  uint8_t bitsperpixel;
  uint8_t sync;
  myfifo_t fifo;
  token_24_t buffer[BUFFER_SIZE_RGB];
  myfifo_t fifo1;
  token_t buffer1[BUFFER_SIZE];
} vep_memshared0_shared_region_t;
#endif

#ifdef VEP_TILE0_PARTITION1_SHARED_REGION_REMOTE_START
typedef volatile struct {
} vep_tile0_partition1_shared_region_t;
#endif
#ifdef VEP_TILE0_PARTITION2_SHARED_REGION_REMOTE_START
typedef volatile struct {
  // Output of greyscale
  myfifo_t fifo;
  token_t buffer[BUFFER_SIZE];
} vep_tile0_partition2_shared_region_t;
#endif
#ifdef VEP_TILE0_PARTITION3_SHARED_REGION_REMOTE_START
typedef volatile struct {
  uint32_t initialized;
} vep_tile0_partition3_shared_region_t;
#endif
#ifdef VEP_TILE0_PARTITION4_SHARED_REGION_REMOTE_START
typedef volatile struct {
  uint32_t initialized;
} vep_tile0_partition4_shared_region_t;
#endif
#ifdef VEP_TILE1_PARTITION1_SHARED_REGION_REMOTE_START
typedef volatile struct {
  // Output of convolution
  myfifo_t fifo;
  token_t buffer[BUFFER_SIZE];
  uint8_t i_buffer[5 * LINE_SIZE];
} vep_tile1_partition1_shared_region_t;
#endif
#ifdef VEP_TILE1_PARTITION2_SHARED_REGION_REMOTE_START
typedef volatile struct {
  uint32_t initialized;
} vep_tile1_partition2_shared_region_t;
#endif
#ifdef VEP_TILE1_PARTITION3_SHARED_REGION_REMOTE_START
typedef volatile struct {
  uint32_t initialized;
} vep_tile1_partition3_shared_region_t;
#endif
#ifdef VEP_TILE1_PARTITION4_SHARED_REGION_REMOTE_START
typedef volatile struct {
  uint8_t initialized;
} vep_tile1_partition4_shared_region_t;
#endif
#ifdef VEP_TILE2_PARTITION1_SHARED_REGION_REMOTE_START
typedef volatile struct {
  // Output of sobel, it has a buffer with twice the size
  myfifo_t fifo;
  token_sbl_t buffer[BUFFER_SIZE];
  uint8_t i_buffer[3 * LINE_SIZE];
} vep_tile2_partition1_shared_region_t;
#endif
#ifdef VEP_TILE2_PARTITION2_SHARED_REGION_REMOTE_START
typedef volatile struct {
  uint32_t initialized;
} vep_tile2_partition2_shared_region_t;
#endif
#ifdef VEP_TILE2_PARTITION3_SHARED_REGION_REMOTE_START
typedef volatile struct {
  uint32_t initialized;
} vep_tile2_partition3_shared_region_t;
#endif
#ifdef VEP_TILE2_PARTITION4_SHARED_REGION_REMOTE_START
typedef volatile struct {
  uint32_t initialized;
} vep_tile2_partition4_shared_region_t;
#endif



/***** DO NOT MODIFY THE CODE BELOW *****/

extern void print_vep_memory_map(void);

#ifdef VEP_MEMSHARED0_SHARED_REGION_REMOTE_START
extern vep_memshared0_shared_region_t volatile * const vep_memshared0_shared_region;
#endif

#ifdef VEP_TILE0_PARTITION1_SHARED_REGION_REMOTE_START
extern vep_tile0_partition1_shared_region_t volatile * const vep_tile0_partition1_shared_region;
#endif
#ifdef VEP_TILE0_PARTITION2_SHARED_REGION_REMOTE_START
extern vep_tile0_partition2_shared_region_t volatile * const vep_tile0_partition2_shared_region;
#endif
#ifdef VEP_TILE0_PARTITION3_SHARED_REGION_REMOTE_START
extern vep_tile0_partition3_shared_region_t volatile * const vep_tile0_partition3_shared_region;
#endif
#ifdef VEP_TILE0_PARTITION4_SHARED_REGION_REMOTE_START
extern vep_tile0_partition4_shared_region_t volatile * const vep_tile0_partition4_shared_region;
#endif
#ifdef VEP_TILE1_PARTITION1_SHARED_REGION_REMOTE_START
extern vep_tile1_partition1_shared_region_t volatile * const vep_tile1_partition1_shared_region;
#endif
#ifdef VEP_TILE1_PARTITION2_SHARED_REGION_REMOTE_START
extern vep_tile1_partition2_shared_region_t volatile * const vep_tile1_partition2_shared_region;
#endif
#ifdef VEP_TILE1_PARTITION3_SHARED_REGION_REMOTE_START
extern vep_tile1_partition3_shared_region_t volatile * const vep_tile1_partition3_shared_region;
#endif
#ifdef VEP_TILE1_PARTITION4_SHARED_REGION_REMOTE_START
extern vep_tile1_partition4_shared_region_t volatile * const vep_tile1_partition4_shared_region;
#endif
#ifdef VEP_TILE2_PARTITION1_SHARED_REGION_REMOTE_START
extern vep_tile2_partition1_shared_region_t volatile * const vep_tile2_partition1_shared_region;
#endif
#ifdef VEP_TILE2_PARTITION2_SHARED_REGION_REMOTE_START
extern vep_tile2_partition2_shared_region_t volatile * const vep_tile2_partition2_shared_region;
#endif
#ifdef VEP_TILE2_PARTITION3_SHARED_REGION_REMOTE_START
extern vep_tile2_partition3_shared_region_t volatile * const vep_tile2_partition3_shared_region;
#endif
#ifdef VEP_TILE2_PARTITION4_SHARED_REGION_REMOTE_START
extern vep_tile2_partition4_shared_region_t volatile * const vep_tile2_partition4_shared_region;
#endif

#endif
