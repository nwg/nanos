#include "user_vga.h"
#include "syscall.h"
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "user.h"
#include "sizes.h"

int spawn(void *text, int argc, char **argv);

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
		printf("Got line %s\n", buf);

		// static char *argv2[] = {
		// 	"user2", "1", "12",
		// };
		// static int argc2 = sizeof(argv2) / sizeof(char*);

		// spawn((void*)TEST_PROG_PMA, argc2, argv2);
	}

	sys_exit();
}

int spawn(void *text, int argc, char **argv) {
	int result;
	sys_spawn(text, argc, argv, result);
	return result;
}
