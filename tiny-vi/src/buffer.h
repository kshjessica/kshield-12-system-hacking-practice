#pragma once

#include <stdlib.h>
#include <string.h>

#define BUFFER_INIT { NULL, 0 }

typedef struct _Buffer {
    void *buf;
    size_t len;
} Buffer;

void append_buffer(Buffer *, void *, size_t);
void append_pointer(Buffer *, void *);
void init_buffer(Buffer *);
