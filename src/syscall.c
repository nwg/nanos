#include "syscall.h"
#include "kernel.h"
#include "process.h"
#include "schedule.h"
#include "asm.h"
#include "term.h"

void handle_syscall(system_state_t *state) {

	return_from_schedule(state);

	process_t *process = current_process();

	switch (state->registers.rax) {

		case SYSCALL_TEST:
			kprintf("Syscall 0 Received!!\n");
			break;

		case SYSCALL_YIELD:
			schedule();
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
			break;

		case SYSCALL_READ: {
			int filedes = state->registers.rbx;
			char *buf = (char*)state->registers.rcx;
			size_t len = state->registers.rdx;

			if (filedes != 0) PANIC("read from bad filedes");

			state->registers.rdi = process_read_file(process, filedes, buf, len);

			break;
		}

		case SYSCALL_WRITE: {
			int filedes = state->registers.rbx;
			const char *buf = (const char*)state->registers.rcx;
			size_t len = state->registers.rdx;

			state->registers.rdi = process_write_file(process, filedes, buf, len);

			break;
		}

		case SYSCALL_SPAWN: {
			void *text = (void*)state->registers.rbx;
			int argc = state->registers.rcx;
			char **argv = (char**)state->registers.rdx;

			spawn(text, argc, argv);

			state->registers.rdi = 0;

			break;
		}
	}
}

