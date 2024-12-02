#ifndef _MYFIFOH_
#define _MYFIFOH_

#include <stdint.h>
#include <stdbool.h>

typedef volatile struct _myfifo_t {
  // Actual fifo queue data
  // User must cast to whatever type they need
  volatile void * m_buffer;

  // Size of the buffer
  volatile uint16_t m_size;

  // Size of each element in m_buffer
  volatile uint16_t m_token_size;

  // Current usage of the queue
  volatile uint16_t m_usage;

  // Last written index
  volatile uint32_t m_writeIdx;

  // Last read index
  volatile uint32_t m_readIdx;
} myfifo_t;

// token_buffer must point to an array containing nr_tokens that are aligned
// function returns NULL on failure and fifo_admin on success
extern myfifo_t * myfifo_init(myfifo_t volatile * const fifo_admin, void volatile * const token_buffer, uint32_t const nr_tokens, uint32_t const token_size);

// if token_buffer was malloc'd before myfifo_init then it must be freed after myfifo_destroy
extern void myfifo_destroy(myfifo_t volatile * const fifo_admin);

// myfifo_initialized must return false before myfifo_init has been called successfully
// or after myfifo_destroy has been called, and true otherwise
extern bool myfifo_initialized(myfifo_t volatile const * const fifo_admin);
extern void myfifo_print_status(myfifo_t volatile const * const fifo_admin);

// use myfifo_spaces/tokens to poll, for non-blocking behaviour
// myfifo_spaces/tokens returns the number of spaces/tokens that can still be claimed
extern uint32_t myfifo_spaces(myfifo_t volatile const * const fifo_admin);
extern uint32_t myfifo_tokens(myfifo_t volatile const * const fifo_admin);

// all following functions are blocking
#ifdef ARM
extern uint32_t myfifo_claim_space(myfifo_t volatile * const fifo_admin);
extern uint32_t myfifo_claim_token(myfifo_t volatile * const fifo_admin);
extern void myfifo_clean(myfifo_t volatile * const fifo_admin);
#else
extern void volatile * myfifo_claim_space(myfifo_t volatile * const fifo_admin);
extern void volatile * myfifo_claim_token(myfifo_t volatile * const fifo_admin);
#endif

extern void myfifo_release_token(myfifo_t volatile * const fifo_admin);
extern void myfifo_release_space(myfifo_t volatile * const fifo_admin);

// write/read = claim + release
// can only write/read when no spaces/tokens have been claimed, to avoid confusion
// extern void myfifo_write_token(myfifo_t volatile * const fifo_admin, void const * const token_to_write);
// extern void myfifo_read_token(myfifo_t volatile * const fifo_admin, void * const new_token);

#endif

