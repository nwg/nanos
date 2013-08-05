#include "string.h"

size_t strlen(const char *s) {
	int i = 0;
	while (*s++) { i++; }
	return i;
}