#include "user_vga.h"
#include "syscall.h"
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "user.h"
#include <stdlib.h>
#include "nanos_user.h"
#include <sys/wait.h>

int atoi(const char *a);

static char* colors[] = {
	"grey",
	"blue",
	"green",
	"cyan",
	"red",
	"purple",
	"yellow",
	"white"
};

#include "asm.h"
int main(int argc, char **argv) {
	if (argc != 2) {
		printf("Usage: %s [num_racers]\n", argv[0]);
		BOCHS_BRK();
		return 1;
	}

	int nracers = atoi(argv[1]);

	if (nracers < 0 || nracers > 8) {
		printf("%s: 0 <= num_racers <= 8\n", argv[0]);
		return 1;
	}


	pid_t pids[nracers];
	for (int i = 0; i < nracers; i++) {
		char color[3];
		char line[3];
		snprintf(color, sizeof(color), "%d", 8+i);
		snprintf(line, sizeof(color), "%d", 10+i);
		char *argv[] = { "racer", color, line };
		int argc = sizeof(argv) / sizeof(char*);
		pids[i] = spawn(LINE_TEXT, argc, argv);
	}

	pid_t winner = wait(NULL);
	for (int i = 0; i < nracers; i++) {
		if (winner == pids[i]) {
			char *color_name = colors[i];
			printf("%s wins!\n", color_name);
		}
	}
}

int atoi(const char *a) {
	int num = 0;
	int mult = 1;
	int len = strlen(a);

	for (int i = len - 1; i >= 0; i--) {
		char c = a[i];
		num += (c - '0') * mult;
		mult *= 10;
	}
	return num;
}
