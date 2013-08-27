#include "string.h"

size_t strlen(const char *s) {
	int i = 0;
	while (*s++) { i++; }
	return i;
}

char *strncpy(char *restrict s1, const char *restrict s2, size_t n) {
    char *p = s1;

    for (const char *q = s2; *q != '\0' && n > 0; n--) {
        *p++ = *q++;
    }

    return s1;
}
