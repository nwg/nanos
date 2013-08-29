#include "file.h"
#include "kmem.h"
#include <memory.h>

static uint64_t next_global_id = 0;

file_t *file_alloc() {
    file_t *file = kalloc(sizeof(file_t));
    memset(file, 0, sizeof(file_t));
    file->global_id = next_global_id++;
    file->flags = 0;

    return file;
}

void file_dealloc(file_t *this) {
    kfree(this);
}

ssize_t file_write(file_t *this, const char *buf, size_t nbyte) {
    return this->write_handler(this->ctx, buf, nbyte);
}

ssize_t file_read(file_t *this, char *buf, size_t nbyte) {
    return this->read_handler(this->ctx, buf, nbyte);
}

bool file_can_read(file_t *this) {
    if (!this->can_read_handler) return true;
    return this->can_read_handler(this->ctx);
}

bool file_can_write(file_t *this) {
    if (!this->can_write_handler) return true;
    return this->can_write_handler(this->ctx);
}
