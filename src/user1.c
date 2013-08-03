#include "video.h"
#include "syscall.h"
#include "process.h"

void wait();

void _start() {
	sys_test();

	sys_add_pages(1);
	*(u64_t*)USER_HEAP = 50;

	for (int i = 0; i < 80; i++) {
		print(11, i, COLOR_RED, "\07");
		wait();
	}

	while (1) {}
}


void wait() {
	for (int i = 0; i < 10000000; i++) {}
}