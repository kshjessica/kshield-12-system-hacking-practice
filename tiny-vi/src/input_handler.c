#include "input_handler.h"
#include "buffer.h"

void capture_input(Buffer *buffer) {
    init_buffer(buffer);

    while (1) {
        char tmp[COLS];
        size_t nread = read(STDIN_FILENO, tmp, COLS);
        append_buffer(buffer, tmp, nread);

        if (nread < COLS)
            break;
    }

    if (((char *)buffer->buf)[buffer->len-1] == '\n')
        ((char *)buffer->buf)[--buffer->len] = '\0';
}
