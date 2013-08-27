#include "syscall.h"
#include "kernel.h"
#include "process.h"
#include "schedule.h"
#include "video.h"
#include "asm.h"
#include "term.h"

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

		case SYSCALL_READ: {
			int filedes = state->registers.rbx;
			char *buf = (char*)state->registers.rcx;
			size_t len = state->registers.rdx;

			if (filedes != 0) PANIC("read from bad filedes");

			int readlen = term_read_stdin(buf, len);
			if (readlen > 0) {
				kprintf("Got %d bytes\n", readlen);
				state->registers.rdi = readlen;
				break;
			}

			kprintf("Waiting for read\n", readlen);
			process_wait_read(process, filedes, buf, len);
			schedule();

			break;
		}

		case SYSCALL_WRITE: {
			int filedes = state->registers.rbx;
			const char *buf = (const char*)state->registers.rcx;
			size_t len = state->registers.rdx;

			if (filedes != 1) PANIC("write to bad filedes");

			term_write(buf, len);
			break;
		}
	}
}

