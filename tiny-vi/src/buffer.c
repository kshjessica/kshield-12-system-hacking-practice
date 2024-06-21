#include "buffer.h"

void append_buffer(Buffer *buffer, void *append_data, size_t append_len) {
    buffer->buf = realloc(buffer->buf, buffer->len + append_len);
    memcpy(&buffer->buf[buffer->len], append_data, append_len);
    buffer->len += append_len;
}

void append_pointer(Buffer *buffer, void *append_pointer) {
    buffer->buf = realloc(buffer->buf, buffer->len + sizeof(void *));
    memcpy(&buffer->buf[buffer->len], &append_pointer, sizeof(void *));
    buffer->len += sizeof(void *);
}

void init_buffer(Buffer *buffer) {
    if (buffer->buf)
        free(buffer->buf);
    buffer->buf = NULL;
    buffer->len = 0;
}
