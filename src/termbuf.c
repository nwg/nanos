#include "termbuf.h"
#include "kmem.h"

ssize_t termbuf_write(termbuf_t *this, const char *str, size_t nbytes);
ssize_t termbuf_read(termbuf_t *this, char *str, size_t nbytes);
bool termbuf_wake_read(termbuf_t *this);

termbuf_t *termbuf_alloc() {
    termbuf_t *termbuf = kalloc(sizeof(termbuf_t));

    inbuf_t *inbuf = inbuf_alloc(TERMBUF_INBUFSIZE);
    termbuf->inbuf = inbuf;
    file_t *file = file_alloc();
    file->ctx = termbuf;
    file->write_handler = (file_write_handler)termbuf_write;
    file->read_handler = (file_read_handler)termbuf_read;
    file->wake_read_handler = (file_wake_read_handler)termbuf_wake_read;
    termbuf->file = file;

    return termbuf;
}

bool termbuf_wake_read(termbuf_t *this) {
    return inbuf_wake_read(this->inbuf);
}

ssize_t termbuf_read(termbuf_t *this, char *str, size_t nbytes) {
    if (!inbuf_wake_read(this->inbuf)) {
        return FILE_ERROR_EAGAIN;
    }
    return inbuf_read(this->inbuf, str, nbytes);
}

ssize_t termbuf_write(termbuf_t *this, const char *str, size_t nbytes) {
    inbuf_t *inbuf = this->inbuf;
    ssize_t result = inbuf_write(inbuf, str, nbytes);
    if (result > 0) {
        return file_write(g_term->file, str, result);
    }

    return result;
}
