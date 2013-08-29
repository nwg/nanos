#ifndef __TERMBUF_H__
#define __TERMBUF_H__

#include "inbuf.h"
#include "term.h"

#define TERMBUF_INBUFSIZE 1024

typedef struct {
    inbuf_t *inbuf;
    file_t *file;
} termbuf_t;

termbuf_t *termbuf_alloc();

#endif
