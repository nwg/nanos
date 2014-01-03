#include "devio.h"
#include "ata_nanos.h"
#include <malloc.h>
#include "timer.h"
#include "kmem.h"

devio_t *g_devio = NULL;

bool devio_check_timeout(void *ctx);
ssize_t devio_read_handler(void *ctx, char *buf, size_t nbyte);
ssize_t devio_write_handler(void *ctx, const char *buf, size_t nbyte);

void devio_init() {
    g_devio = devio_alloc(ATA_DRIVE_PRIMARY_MASTER);
}

devio_t *devio_alloc(ata_drive_e drive) {
    devio_t *this = kalloc(sizeof(devio_t));
    this->file = file_alloc();
    this->file->ctx = this;
    this->file->read_handler = devio_read_handler;
    this->file->write_handler = devio_write_handler;
    this->file->wake_read_handler = devio_check_timeout;
    this->file->wake_write_handler = devio_check_timeout;
    this->drive = drive;
    return this;
}

void devio_free(devio_t *this) {
    file_dealloc(this->file);
    kfree(this);
}

bool devio_check_timeout(void *ctx) {
    devio_t *this = (devio_t*)ctx;
    return ata_drive_did_timeout(this->drive);
}

ssize_t devio_read_handler(void *ctx, char *buf, size_t nbyte) {
    devio_t *this = (devio_t*)ctx;
    if (ata_read(this->drive, this->file->pos, buf, nbyte)) {
        return FILE_ERROR_FAIL;
    }

    return 0;
}

ssize_t devio_write_handler(void *ctx, const char *buf, size_t nbyte) {
    return 0;
}

