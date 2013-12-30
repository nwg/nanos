#ifndef __FILE_H__
#define __FILE_H__

#include <stddef.h>
#include <sys/types.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>

struct file_s;
typedef ssize_t (*file_read_handler)(void *ctx, char *buf, size_t nbyte);
typedef ssize_t (*file_write_handler)(void *ctx, const char *buf, size_t nbyte);
typedef bool (*file_can_read_handler)(void *ctx);
typedef bool (*file_can_write_handler)(void *ctx);
typedef off_t (*file_size_handler)(void *ctx);

typedef enum {
    FILE_BUSY_WRITE,
    FILE_BUSY_READ,
} file_flags_e;

typedef enum {
    FILE_ERROR_EAGAIN = -1,
} file_error_e;

typedef struct file_s {
    uint64_t global_id;
    off_t pos;
    file_read_handler read_handler;
    file_write_handler write_handler;
    file_can_read_handler can_read_handler;
    file_can_read_handler can_write_handler;
    file_size_handler size_handler;
    void *ctx;
    file_flags_e flags;
} file_t;

file_t *file_alloc();
void file_dealloc(file_t *this);
ssize_t file_write(file_t *this, const char *buf, size_t nbyte);
ssize_t file_read(file_t *this, char *buf, size_t nbyte);
bool file_can_read(file_t *this);
bool file_can_write(file_t *this);
off_t file_size(file_t *this);

#endif
