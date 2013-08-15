#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <memory.h>

#define BASE10_BIGGEST 10000000000000000000UL

int i2a(char *dst, size_t n, uint64_t num) {
    int i = 0;
    uint64_t mul = BASE10_BIGGEST;


    while (num / mul == 0) {
        mul /= 10;
    }

    if (mul < 1)
        return -1;

    for (; i < n && mul > 0; i++, mul /= 10) {
        uint64_t val = num / mul;
        *dst++ = val + '0';
        num -= val * mul;
    }

    return i;
}


 #define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })


int vsnprintf(char * restrict str, size_t n, const char * restrict format, va_list ap) {
    int i = 0;

    int v_i;
    char *v_s;

    for (const char *f = format; *f != '\0' && i < n; ) {
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

            case 's':
                f++;

                v_s = va_arg(ap, char *);
                int len = strlen(v_s);
                memcpy(&str[i], v_s, min(len, (n-i)));
                i += len;

                break;
        }

    }

    return 0;
}



int snprintf(char * restrict str, size_t n, const char * restrict format, ...) {
    va_list ap;
    va_start(ap, format);

    vsnprintf(str, n, format, ap);

    va_end(ap);

    return 0;
}
