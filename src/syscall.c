#include "syscall.h"
#include "kernel.h"
#include "process.h"
#include "schedule.h"
#include "video.h"
#include "asm.h"

void handle_syscall(system_state_t *state) {
	process_t *process = current_process();

	switch (state->rax) {

		case SYSCALL_TEST:
			print(2, 2, COLOR_RED | COLOR_GREEN, "Syscall 0 Received!!");
			break;

		case SYSCALL_EXIT:
			remove_process(process);
			schedule(); // does not return

		case SYSCALL_ADD_PAGES:
			process_add_pages(process, state->rbx);
			break;

		case SYSCALL_SLEEP:
			process_sleep(process, state->rbx);
			schedule();

	}

    SET_RSP(process->saved_sp);
    RETURN_TO_PROCESS();
}

