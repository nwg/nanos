
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <memory.h>
#include "video.h"
#include "num.h"

#define BASE10_LEFT 1000000000000000000
#define BASE16_LEFT 0x1000000000000000

char digits[] = "0123456789abcdef";

int _i2a(char *dst, size_t n, int64_t num, int base, int64_t base_left) {
    int i = 0;
    uint64_t mul = base_left;
    char *p = dst;

    if (n == 0) {
        return 0;
    }

    if (num == 0) {
        *p++ = '0'; i++;
        return i;
    }

    if (num < 0) {
        *p++ = '-'; i++;
        num = -num;
    }

    while (num / mul == 0) {
        mul /= base;
    }

    for (; i < n && mul > 0; i++, mul /= base) {
        uint64_t val = num / mul;
        *p++ = digits[val];
        num -= val * mul;
    }

    return i;
}

#define i2a_dec(dst, n, num) _i2a(dst, n,num, 10, BASE10_LEFT)
#define i2a_hex(dst, n, num) _i2a(dst, n,num, 16, BASE16_LEFT)
#define i2a i2a_dec

int vsnprintf(char * restrict str, size_t n, const char * restrict format, va_list ap) {
    int i = 0;
    int v_i;
    unsigned int v_u;
    char *v_s;

    if (n < 1)
        return 0;

    for (const char *f = format; *f != '\0' && i < n - 1; ) {
        if (*f != '%') {
            str[i++] = *f++;
            continue;
        }

        f++;

        switch (*f) {
            case 'd':
                f++;

                v_i = va_arg(ap, int);
                i += i2a(&str[i], (n - i), v_i);
                break;

            case 'x':
                f++;

                v_u = va_arg(ap, unsigned int);
                i += i2a_hex(&str[i], (n - i), v_u);
                break;

            case 's':
                f++;

                v_s = va_arg(ap, char *);
                int len = strlen(v_s);
                memcpy(&str[i], v_s, min(len, (n-i)));
                i += len;

                break;
        }

    }

    str[i++] = '\0';

    return 0;
}



int snprintf(char * restrict str, size_t n, const char * restrict format, ...) {
    va_list ap;
    va_start(ap, format);

    vsnprintf(str, n, format, ap);

    va_end(ap);

    return 0;
}

static int row = 0;
static char buf[256];

int
vprintf(const char * restrict format, va_list ap) {
    vsnprintf(buf, 256, format, ap);
    print(row++, 0, COLOR_GREEN, buf);

    row = row  % 25;
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
