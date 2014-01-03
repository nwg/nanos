#include "syscall.h"
#include "kernel.h"
#include "process.h"
#include "schedule.h"
#include "asm.h"
#include "term.h"
#include "timer.h"
#include "pages.h"
#include "ata_nanos.h"

void handle_syscall(system_state_t *state) {

	return_from_schedule(state);

	process_t *process = current_process();

	syscall_code_e code = (syscall_code_e)state->registers.rdi;
	uint64_t p1 = state->registers.rsi;
	uint64_t p2 = state->registers.rdx;
	uint64_t p3 = state->registers.rcx;
	// uint64_t p4 = state->registers.r8;
	// uint64_t p5 = state->registers.r9;

	uint64_t retval = 0;

	switch (code) {

		case SYSCALL_TEST:
			kprintf("Syscall 0 Received!!\n");
			break;

		case SYSCALL_GET_TICKS:
			retval = g_timer_ticks;
			break;

		case SYSCALL_GETPID:
			retval = process->pid;
			break;

		case SYSCALL_YIELD:
			schedule();
			break;

		case SYSCALL_EXIT: {
			int exit_status = p1;

			if (process->parent) {
				process_child_finished(process->parent, process, exit_status);
			}

			remove_process(process);
			schedule();
			break;
		}

		case SYSCALL_SBRK:
			retval = (uint64_t)process_sbrk(process, p1);
			break;

		case SYSCALL_SLEEP:
			process_sleep(process, p1);
			break;

		case SYSCALL_READ: {
			int filedes = p1;
			char *buf = (char*)p2;
			size_t len = p3;

			buf = pt_walk(process->pages, buf);
			retval = process_read_file(process, filedes, buf, len);

			break;
		}

		case SYSCALL_WRITE: {
			int filedes = p1;
			const char *buf = (const char*)p2;
			size_t len = p3;

			retval = process_write_file(process, filedes, buf, len);

			break;
		}

		case SYSCALL_WAIT: {
			int *stat_loc = (int*)p1;

			retval = process_wait(process, stat_loc);
			break;
		}

		case SYSCALL_SPAWN: {
			void *text = (void*)p1;
			int argc = p2;
			char **argv = (char**)p3;

			process_t *spawned = spawn(text, argc, argv);
			process_add_child(process, spawned);

			retval = spawned->pid;

			break;
		}

	}

	state->registers.rax = retval;
}

