#ifndef __PROCESS_H__
#define __PROCESS_H__

#include "types.h"
#include "sizes.h"

#define K_STACK_SIZE 65536
#define U_STACK_SIZE 65536

typedef struct process_s {
  void *stack_k;
  void *stack_u;
  void *pages;
  void *saved_sp;
  void *saved_registers;
  void *text;
} process_t;

#define STACK_K_START(process) STACK_START(process->stack_k, K_STACK_SIZE)
#define STACK_U_START(process) STACK_START(process->stack_u, U_STACK_SIZE)
#define USER_TEXT_VMA 0x1c0000
#define USER_TEXT_SIZE (16*KB)

process_t *process_alloc(void *text);

void __attribute__ ((noreturn)) switch_to_process(process_t *process);
void return_from_process(process_t *process, void *sp);

#endif
