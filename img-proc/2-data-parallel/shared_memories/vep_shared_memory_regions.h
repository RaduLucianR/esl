#ifndef __VEP_PRIVATE_MEMORY_H_
#define __VEP_PRIVATE_MEMORY_H_

#include <stddef.h>
#include <stdint.h>
#include <mutex.h>
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

#define N_THREADS 3
#define PRODUCER_IS_OWNER 0
#define CONSUMER_IS_OWNER 1
#define MAX_SIZE (VEP_TILE0_PARTITION2_SHARED_REGION_SIZE / 2)
#define MAX_SHARED_SIZE (MAX_SIZE * N_THREADS)
#define MEM vep_memshared0_shared_region


#ifdef VEP_MEMSHARED0_SHARED_REGION_REMOTE_START
typedef volatile struct {
  uint32_t xsize;
  uint32_t ysize;
  uint8_t bitsperpixel;
  uint8_t sync;
  uint8_t frame[MAX_SHARED_SIZE];
  mutex_t mutex;
} vep_memshared0_shared_region_t;
#endif

#ifdef VEP_TILE0_PARTITION1_SHARED_REGION_REMOTE_START
typedef volatile struct {
  uint32_t initialized;
} vep_tile0_partition1_shared_region_t;
#endif
#ifdef VEP_TILE0_PARTITION2_SHARED_REGION_REMOTE_START
typedef volatile struct {
  uint8_t frame0[MAX_SIZE];
  uint8_t frame1[MAX_SIZE];
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
  uint32_t initialized;
} vep_tile1_partition1_shared_region_t;
#endif
#ifdef VEP_TILE1_PARTITION2_SHARED_REGION_REMOTE_START
typedef volatile struct {
  uint8_t frame0[MAX_SIZE];
  uint8_t frame1[MAX_SIZE];
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
  uint32_t initialized;
} vep_tile2_partition1_shared_region_t;
#endif
#ifdef VEP_TILE2_PARTITION2_SHARED_REGION_REMOTE_START
typedef volatile struct {
  uint8_t frame0[MAX_SIZE];
  uint8_t frame1[MAX_SIZE];
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
