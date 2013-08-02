#include "syscall.h"
#include "kernel.h"
#include "process.h"
#include "schedule.h"

int64_t handle_syscall(system_state_t *state) {
	switch (state->rax) {
		case INTERRUPT_TEST:
		  __asm__(
		  	"movq $0x3fe8c0, %rax\n\t"
		  	"movq $0x01690148, (%rax)\n\t"
		  );

			break;
		case INTERRUPT_ADD_PAGES:
			process_add_pages(current_process(), state->rbx);
			break;
	}

	return 0;
}