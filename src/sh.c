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
#include "nanos_user.h"

#define MAX_ARGS 16
static char *DELIM = " \t";

typedef int (*cmdfunc)(int argc, char **argv);

typedef struct {
	const char *name;
	const char *help;
	cmdfunc f;
} cmd_t;

char * strsep (char ** sp, const char * delim);
cmd_t *find_command(char *name);
int run(void *text, int argc, char **argv);
int run_race(int argc, char **argv);
int run_help(int argc, char **argv);

cmd_t commands[] = {
	{ "race", "a test program", run_race },
	{ "help", "list commands", run_help },
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

int run_race(int argc, char **argv) {
	return run(RACE_TEXT, argc, argv);
}

int quick_help() {
	printf("\n");
	printf("Commands\n");
	printf("--------\n");
	for (int i = 0; i < num_commands; i++) {
		cmd_t *cmd = &commands[i];
		printf("%s: %s\n", cmd->name, cmd->help);
	}
	return 0;
}

int run_help(int argc, char **argv) {
	return quick_help();
}

int main(int argc, char **argv) {

	// Test a disk read
	uint8_t readbuf[1024];
	memset(readbuf, 0, sizeof(readbuf));
	if (read(3, readbuf, 1024) >= 0) {
		if (readbuf[510] != 0x55 || readbuf[511] != 0xAA) {
			printf("Read test expected boot sector but found %x\n", readbuf[511]);
		} else {
			printf("Read test succeeded\n");
		}
	} else {
		printf("Read test failed\n");
	}

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
		if (!tok || tok[0] == '\0')
			continue;

		cmd_t *cmd = find_command(tok);
		if (!cmd) {
			printf("Command %s not found.\n", tok);
			quick_help();
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

