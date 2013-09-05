#ifndef __INBUF_H__
#define __INBUF_H__

#include <stddef.h>
#include "file.h"
#include <stdbool.h>

/*
 * Line-oriented input buffer
 * Forwards input on to terminal
 * Can be written by any input source such as a keyboard
 * Can be read once at least one line is available
 */
struct inbuf_s;

typedef struct inbuf_s {
    char *buf;
    size_t nbytes;
    size_t offset;
    size_t last_newline;
} inbuf_t;

void *inbuf_alloc(size_t nbytes);
void inbuf_dealloc(inbuf_t *this);
size_t inbuf_read(inbuf_t *this, char *str, size_t nbytes);
bool inbuf_can_read(inbuf_t *this);
ssize_t inbuf_write(inbuf_t *this, const char *str, size_t nbytes);

#endif
