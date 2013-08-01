#include "syscall.h"
#include "kernel.h"

int64_t handle_syscall(system_state_t *state) {
	switch (state->rax) {
		case INTERRUPT_TEST:
		  __asm__(
		  	"movq $0x3fe8c0, %rax\n\t"
		  	"movq $0x01690148, (%rax)\n\t"
		  );

			break;
		case INTERRUPT_ADD_PAGES:
			break;
	}

	return 0;
}