#include "syscall.h"
#include "kernel.h"
#include "process.h"
#include "schedule.h"
#include "video.h"

int64_t handle_syscall(system_state_t *state) {
	switch (state->rax) {
		case SYSCALL_TEST:
			print(2, 2, COLOR_RED | COLOR_GREEN, "Syscall 0 Received!!");
			break;
		case SYSCALL_ADD_PAGES:
			process_add_pages(current_process(), state->rbx);
			break;
	}

	return 0;
}

