#include "video.h"
#include "syscall.h"
#include "process.h"
#include <string.h>

void wait();
void print_hex(int row, int col, int color, u64_t v);
int atoi(const char *a);

int main(int argc, char **argv) {

	int color = atoi(argv[1]) << 8;
	int row = atoi(argv[2]);

	sys_test();

	sys_add_pages(1);
	*(u64_t*)USER_HEAP = 50;

	for (int i = 0; i < 80; i++) {
		print(row, i, color, "\07");
		wait();
	}

	while (1) {}
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

void print_hex(int row, int col, int color, u64_t v) {
	char buf[2]; buf[1] = '\0';

	for (int i = 0; i < 16; i++, col++) {
		char c = v >> (4*(16 - i - 1)) & 0xF;
		if (c < 0xa) {
			c = c + '0';
		} else {
			c = c - 0xa + 'a';
		}
		buf[0] = c;
		print(row, col, color, buf);
	}
}

void wait() {
	for (int i = 0; i < 10000000; i++) {}
}