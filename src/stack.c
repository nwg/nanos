#include "stack.h"
#include "string.h"

stackptr_t push_string(stackptr_t s, const char *str) {
    int len = strlen(str);
    STACK_INC(s, len+1);
    memcpy(s, str, len);
    ((char*)s)[len] = '\0';

    return s;
}
