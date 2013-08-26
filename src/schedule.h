#ifndef __SCHEDULE_H__
#define __SCHEDULE_H__

#include "process.h"
#include <stdbool.h>
#include "kernel.h"

#define K_STACK_SIZE 65536
#define U_STACK_SIZE 65536

void schedule_init();
void switch_to_process(process_t *process);
void schedule();
bool process_running();
void return_from_schedule(system_state_t *state);
void add_process(process_t *process);
process_t *current_process();
void spawn(void *text, int argc, char **argv);
void remove_process(process_t *process);
void schedule_wake_sleepers();

#endif
