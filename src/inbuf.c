#include "inbuf.h"
#include <memory.h>
#include "num.h"
#include "kmem.h"
#include <string.h>
#include <stdbool.h>

ssize_t inbuf_write(inbuf_t *this, const char *str, size_t nbytes);

void *inbuf_alloc(size_t nbytes) {
    inbuf_t *this = kalloc(sizeof(inbuf_t));
    memset(this, 0, sizeof(inbuf_t));

    this->buf = kalloc(nbytes);
    this->nbytes = nbytes;
    memset(this->buf, 0, this->nbytes);
    this->offset = 0;
    this->last_newline = 0;

    return this;
}

void inbuf_dealloc(inbuf_t *this) {
    kfree(this->buf);
    kfree(this);
}

void inbuf_clear(inbuf_t *this) {
    memset(this->buf, 0, this->nbytes);
    this->offset = 0;
    this->last_newline = 0;
}

int inbuf_get_lines(inbuf_t *this, char *dst, size_t maxlen) {
    int size = min(maxlen, this->last_newline);
    if (size == 0) return 0;

    strncpy(dst, this->buf, size);

    if (size < this->offset) {
        memcpy(this->buf, &this->buf[this->offset], this->offset - size);
        this->offset = this->offset - size;
        this->last_newline = this->last_newline - size;
        memset(&this->buf[this->offset], 0, this->nbytes - this->offset);
    } else {
        inbuf_clear(this);
    }

    return size;
}

void inbuf_write_char(inbuf_t *this, char c) {
    if (this->offset >= this->nbytes - 1)
        return;

    // save char for last newline
    if (this->offset == this->nbytes - 2 && c != '\n')
        return;

    this->buf[this->offset++] = c;

    if (c == '\n') {
        this->last_newline = this->offset;
    }
}

ssize_t inbuf_write(inbuf_t *this, const char *str, size_t nbytes) {
    int i = 0;
    for (const char *p = str; *p != '\0' && i < nbytes; p++, i++) {
        inbuf_write_char(this, *p);
    }

    return i;
}

bool inbuf_can_read(inbuf_t *this) {
    return this->last_newline > 0;
}

size_t inbuf_read(inbuf_t *this, char *str, size_t nbytes) {
    return inbuf_get_lines(this, str, nbytes);
}
