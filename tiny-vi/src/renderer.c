#include "renderer.h"

void move_renderer_cursor(Buffer *render_buffer, uint8_t x, uint8_t y) {
    char data[11] = { 0, };

    snprintf(data, sizeof(data), "\x1b[%d;%dH", y, x);

    RENDER(render_buffer, data, strlen(data));
}

void draw_outline(Buffer *render_buffer) {
    move_renderer_cursor(render_buffer, 0, 2);

    RENDER(render_buffer, "*", 1);
    for (int i = 0; i < COLS; i++)
        RENDER(render_buffer, "-", 1);
    RENDER(render_buffer, "*", 1);

    for (int i = 0; i < ROWS; i++) {
        move_renderer_cursor(render_buffer, 0, i+3);
        RENDER(render_buffer, "|", 1);
        move_renderer_cursor(render_buffer, COLS+2, i+3);
        RENDER(render_buffer, "|", 1);
    }

    move_renderer_cursor(render_buffer, 0, ROWS+3);
    RENDER(render_buffer, "*", 1);
    for (int i = 0; i < COLS; i++)
        RENDER(render_buffer, "-", 1);
    RENDER(render_buffer, "*", 1);
}

void draw_screen(Buffer *render_buffer, char *data) {
    char row[COLS];
    for (int i = 0; i < ROWS; i++) {
        memcpy(row, GET_ROW(data, i), COLS);
        for (size_t j = 0; j < COLS; j++)
            if (!row[j])
                row[j] = ' ';
        move_renderer_cursor(render_buffer, 2, i+3);
        clear_line(render_buffer);
        RENDER(render_buffer, row, COLS);
    }
}

void clear_line(Buffer *render_buffer) {
    RENDER(render_buffer, "\x1b[K", 3);
}

void clear_screen(Buffer *render_buffer) {
    RENDER(render_buffer, "\x1b[2J", 4);
}

void hide_cursor(Buffer *render_buffer) {
    RENDER(render_buffer, "\x1b[?25l", 6);
}

void show_cursor(Buffer *render_buffer) {
    RENDER(render_buffer, "\x1b[?25h", 6);
}

void paint_cursor(Buffer *render_buffer, uint8_t x, uint8_t y, char c) {
    move_renderer_cursor(NULL, x, y);

    RENDER(render_buffer, "\x1b[44m", strlen("\x1b[44m"));

    if (!c || c < 0x20 || 0x7e < c)
        RENDER(NULL, (char *)&" ", 1);
    else
        RENDER(NULL, &c, 1);

    RENDER(NULL, "\x1b[0m", strlen("\x1b[0m"));
}

void help(Buffer *render_buffer) {
    size_t y = ROWS+8;

    move_renderer_cursor(render_buffer, 0, y++);
    RENDER(render_buffer, "h, j, k, l: Arrow left, down, up and right (Only NORMAL)", 56);
    move_renderer_cursor(render_buffer, 0, y++);
    RENDER(render_buffer, "CTRL-O: Insert (Toggle)", 23);
    move_renderer_cursor(render_buffer, 0, y++);
    RENDER(render_buffer, "CTRL-X: Delete", 14);

    move_renderer_cursor(render_buffer, 0, y++);
    RENDER(render_buffer, "CTRL-Y: Yank (Copy) (Toggle)", 28);
    move_renderer_cursor(render_buffer, 0, y++);
    RENDER(render_buffer, "CTRL-P: Paste", 13);

    move_renderer_cursor(render_buffer, 0, y++);
    RENDER(render_buffer, "CTRL-F: Recrod (Toggle)", 23);
    move_renderer_cursor(render_buffer, 0, y++);
    RENDER(render_buffer, "CTRL-G: Playback", 16);

    move_renderer_cursor(render_buffer, 0, y++);
    RENDER(render_buffer, "CTRL-3: Paste from the most recent typing", 41);
    move_renderer_cursor(render_buffer, 0, y++);
    RENDER(render_buffer, "CTRL-5: Paste from the most recent clipboard history", 52);
    move_renderer_cursor(render_buffer, 0, y++);
    RENDER(render_buffer, "CTRL-6: Paste from the second most recent clipboard history", 59);
    move_renderer_cursor(render_buffer, 0, y++);
    RENDER(render_buffer, "CTRL-7: Paste from the thrid most recent clipboard history", 58);

    move_renderer_cursor(render_buffer, 0, y++);
    RENDER(render_buffer, "CTRL-N: Naming", 14);

    move_renderer_cursor(render_buffer, 0, y++);
    RENDER(render_buffer, "CTRL-E: Exit", 12);
}

void refresh_screen(char *data) {
    Buffer render_buffer = BUFFER_INIT;

    clear_screen(&render_buffer);

    hide_cursor(&render_buffer);

    draw_screen(&render_buffer, data);
    draw_outline(&render_buffer);

    help(&render_buffer);

    show_cursor(&render_buffer);

    // Render
    write(1, render_buffer.buf, render_buffer.len);

    init_buffer(&render_buffer);
}
