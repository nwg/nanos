#include "syscall.h"
#include "kernel.h"
#include "process.h"
#include "schedule.h"
#include "video.h"
#include "asm.h"

void handle_syscall(system_state_t *state) {
	process_t *process = current_process();

	switch (state->registers.rax) {

		case SYSCALL_TEST:
			kprintf("Syscall 0 Received!!\n");
			break;

		case SYSCALL_EXIT:
			remove_process(process);
			schedule();
			break;

		case SYSCALL_ADD_PAGES:
			process_add_pages(process, state->registers.rbx);
			break;

		case SYSCALL_SLEEP:
			process_sleep(process, state->registers.rbx);
			schedule();
			break;

	}
}

