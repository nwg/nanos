
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include "num.h"
#include <unistd.h>

static char buf[256];

int
vprintf(const char * restrict format, va_list ap) {
    vsnprintf(buf, 256, format, ap);
    write(STDOUT_FILENO, buf, 256);
    return 0;
}

int
printf(const char * restrict format, ...) {
    va_list ap;
    va_start(ap, format);

    int result = vprintf(format, ap);

    va_end(ap);

    return result;
}
