#ifndef __DEVIO__H__
#define __DEVIO__H__

#include "file.h"
#include "ata_nanos.h"

typedef struct {
    file_t *file;
    ata_drive_e drive;
} devio_t;

extern devio_t *g_devio;

void devio_init();
devio_t *devio_alloc(ata_drive_e drive);
void devio_free(devio_t *this);

#endif
