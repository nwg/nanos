#include "user_vga.h"
#include "syscall.h"
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "user.h"
#include "sizes.h"
#include <sys/wait.h>
#include <stdlib.h>

#define MAX_ARGS 16
static char *DELIM = " \t";

typedef int (*cmdfunc)(int argc, char **argv);

typedef struct {
	char *name;
	cmdfunc f;
} cmd_t;

int spawn(void *text, int argc, char **argv);
char * strsep (char ** sp, const char * delim);
cmd_t *find_command(char *name);
int run(void *text, int argc, char **argv);
int run_test(int argc, char **argv);

cmd_t commands[] = {
	{ "test", run_test },
};

int num_commands = sizeof(commands) / sizeof(cmd_t);
static char *cmd_argv[MAX_ARGS];

cmd_t *find_command(char *name) {
	for (int i = 0; i < num_commands; i++) {
		cmd_t *cmd = &commands[i];
		if (strcmp(name, cmd->name) == 0) {
			return cmd;
		}
	}

	return NULL;
}

int run(void *text, int argc, char **argv) {
		spawn(text, argc, argv);

		int status;
		pid_t pid = wait(&status);
		printf("Finished running pid=%d, status=%d\n", pid, status);

		return 0;
}

int run_test(int argc, char **argv) {
	return run((void*)TEST_PROG_PMA + 16*K, argc, argv);
}

int spawn(void *text, int argc, char **argv) {
	return sys_spawn(text, argc, argv);
}

int main(int argc, char **argv) {

	char buf[256];
	memset(buf, 0, sizeof(buf));
	while (true) {
		printf("> ");
		ssize_t len = read(STDIN_FILENO, buf, 256);
		if (len <= 0) {
			printf("Got empty line %s", buf);
			continue;
		}

		buf[len - 1] = '\0';

		char *s = buf;
		char *tok = strtok(s, DELIM);
		cmd_t *cmd = find_command(tok);
		if (!cmd) {
			printf("Command %s not found.\n", tok);
			continue;
		}

		cmd_argv[0] = tok;
		int cmd_argc = 1;
		for (tok = strtok(NULL, DELIM); tok; tok = strtok(NULL, DELIM)) {
			cmd_argv[cmd_argc++] = tok;
		}

		cmd->f(cmd_argc, cmd_argv);
	}

	return 1;
}

