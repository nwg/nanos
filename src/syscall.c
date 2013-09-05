#include "syscall.h"
#include "kernel.h"
#include "process.h"
#include "schedule.h"
#include "asm.h"
#include "term.h"

void handle_syscall(system_state_t *state) {

	return_from_schedule(state);

	process_t *process = current_process();

	switch (SYS_CALLNUM(state->registers)) {

		case SYSCALL_TEST:
			kprintf("Syscall 0 Received!!\n");
			break;

		case SYSCALL_YIELD:
			schedule();
			break;

		case SYSCALL_EXIT:
			if (process->parent) {
				process_child_finished(process->parent, process);
			}

			remove_process(process);
			schedule();
			break;

		case SYSCALL_ADD_PAGES:
			process_add_pages(process, SYS_P1(state->registers));
			break;

		case SYSCALL_SLEEP:
			process_sleep(process, SYS_P1(state->registers));
			break;

		case SYSCALL_READ: {
			int filedes = SYS_P1(state->registers);
			char *buf = (char*)SYS_P2(state->registers);
			size_t len = SYS_P3(state->registers);

			if (filedes != 0) PANIC("read from bad filedes");

			SYS_RET(state->registers) = process_read_file(process, filedes, buf, len);

			break;
		}

		case SYSCALL_WRITE: {
			int filedes = SYS_P1(state->registers);
			const char *buf = (const char*)SYS_P2(state->registers);
			size_t len = SYS_P3(state->registers);

			SYS_RET(state->registers) = process_write_file(process, filedes, buf, len);

			break;
		}

		case SYSCALL_WAIT:
			SYS_RET(state->registers) = process_wait(process);
			break;

		case SYSCALL_SPAWN: {
			void *text = (void*)SYS_P1(state->registers);
			int argc = SYS_P2(state->registers);
			char **argv = (char**)SYS_P3(state->registers);

			process_t *spawned = spawn(text, argc, argv);
			process_add_child(process, spawned);

			SYS_RET(state->registers) = spawned->pid;

			break;
		}
	}
}

