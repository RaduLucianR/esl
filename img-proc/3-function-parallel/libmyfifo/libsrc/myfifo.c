#include <stdint.h>
#include <stdlib.h>
#include <limits.h>
#include <xil_printf.h>
#include <string.h>
#include <myfifo.h>

#ifdef ARM
#include <stdio.h>
#define xil_printf printf
#endif
// do not change this function
// but feel free to add your own fifo_print_debug that prints more information

void myfifo_print_status(myfifo_t volatile const * const fifo_admin)
{
  xil_printf("w: %u r: %u spaces=%u tokens=%u\n", fifo_admin->m_writeIdx, fifo_admin->m_readIdx, myfifo_spaces(fifo_admin), myfifo_tokens(fifo_admin));

  // Since we don't know the queue size, we must print item by item
  for (uint16_t i = 0; i < fifo_admin->m_size; i++)
   xil_printf("%u:0x%.4x ", i, *(uint32_t*)(fifo_admin->m_buffer + (i * fifo_admin->m_token_size)));

  xil_printf("\n");
}

// token_buffer must point to an array containing nr_tokens that are aligned
// function returns NULL on failure and fifo_admin on success
myfifo_t * myfifo_init(myfifo_t volatile * const fifo_admin, void volatile * const token_buffer, uint32_t const nr_tokens, uint32_t const token_size)
{
  if (fifo_admin == NULL)
  {
    xil_printf("Cannot create queue, invalid admin\n");
    return NULL;
  }

  if (token_buffer == NULL)
  {
    xil_printf("Cannot create queue, invalid buffer\n");
    return NULL;
  }

  if (nr_tokens < 1)
  {
    xil_printf("Cannot create queue, size must be bigger than 0\n");
    return NULL;
  }

  if (token_size < 1)
  {
    xil_printf("Cannot create queue, token size must be bigger than 0\n");
    return NULL;
  }

  fifo_admin->m_size = nr_tokens;
  fifo_admin->m_buffer = token_buffer;
  fifo_admin->m_token_size = token_size;

  // Make initialization explicit
  fifo_admin->m_writeIdx = 0;
  fifo_admin->m_readIdx = 0;

  return fifo_admin;
}

// if token_buffer was malloc'd before myfifo_init then it must be freed after myfifo_destroy
void myfifo_destroy(myfifo_t volatile * const fifo_admin)
{
  if (!myfifo_initialized(fifo_admin))
    return;

  fifo_admin->m_token_size = 0;
  fifo_admin->m_writeIdx = 0;
  fifo_admin->m_readIdx = 0;
  fifo_admin->m_size = 0;
  fifo_admin->m_buffer = NULL;
}

// myfifo_initialized must return false before myfifo_init has been called successfully
// or after myfifo_destroy has been called, and true otherwise
bool myfifo_initialized(myfifo_t volatile const * const fifo_admin)
{
  return fifo_admin != NULL && fifo_admin->m_size > 0;
}

// use myfifo_spaces/tokens to poll, for non-blocking behaviour
// myfifo_spaces/tokens returns the number of spaces/tokens that can still be claimed
uint32_t myfifo_spaces(myfifo_t volatile const * const fifo_admin)
{
  return fifo_admin->m_size - fifo_admin->m_usage; // myfifo_tokens(fifo_admin) - 1;
}

uint32_t myfifo_tokens(myfifo_t volatile const * const fifo_admin)
{
  return fifo_admin->m_usage;
}

// all following functions are blocking
#ifdef ARM
uint32_t myfifo_claim_space(myfifo_t volatile * const fifo_admin)
{
  // Wait until there are spaces to write
  while (fifo_admin->m_usage >= fifo_admin->m_size);

  return fifo_admin->m_writeIdx;
}

uint32_t myfifo_claim_token(myfifo_t volatile * const fifo_admin)
{
  while (fifo_admin->m_usage == 0);

  return fifo_admin->m_readIdx;
}

void myfifo_clean(myfifo_t volatile * const fifo_admin)
{
  fifo_admin->m_writeIdx = 0;
  fifo_admin->m_readIdx = 0;
}
#else
void volatile * myfifo_claim_space(myfifo_t volatile * const fifo_admin)
{
  // Wait until there are spaces to write
  while (fifo_admin->m_usage >= fifo_admin->m_size);

  return fifo_admin->m_buffer + (fifo_admin->m_writeIdx * fifo_admin->m_token_size);
}

void volatile * myfifo_claim_token(myfifo_t volatile * const fifo_admin)
{
  while (fifo_admin->m_usage == 0);

  return fifo_admin->m_buffer + (fifo_admin->m_readIdx * fifo_admin->m_token_size);
}
#endif

void myfifo_release_token(myfifo_t volatile * const fifo_admin)
{
  // Make sure it doesn't overflow
  if (fifo_admin->m_writeIdx + 1 >= fifo_admin->m_size)
    fifo_admin->m_writeIdx = 0;
  else
    ++(fifo_admin->m_writeIdx);

  ++(fifo_admin->m_usage);
}

void myfifo_release_space(myfifo_t volatile * const fifo_admin)
{
  // Make sure it doesn't overflow
  if (fifo_admin->m_readIdx + 1 >= fifo_admin->m_size)
    fifo_admin->m_readIdx = 0;
  else
    ++(fifo_admin->m_readIdx);

  --(fifo_admin->m_usage);
}

