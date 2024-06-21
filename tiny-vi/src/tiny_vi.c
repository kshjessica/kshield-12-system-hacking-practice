#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>
#include "renderer.h"
#include "common.h"
#include "input_handler.h"

char buf[ROWS][COLS];
Buffer clipboard;
Buffer macro;
Buffer typing_history;
Buffer clipboard_history[3];

char *name;
size_t name_idx;

uint8_t cursor_x;
uint8_t cursor_y;

uint8_t current_mode;
const uint8_t MODE_RAW    = 0b00000001;
const uint8_t MODE_NORMAL = 0b00000010;
const uint8_t MODE_INSERT = 0b00000100;
const uint8_t MODE_YANK   = 0b00001000;
const uint8_t MODE_RECORD = 0b00010000;
const uint8_t MODE_NAMING = 0b00100000;

void init() {
    setvbuf(stdin, 0, 2, 0);
    setvbuf(stdout, 0, 2, 0);
    setvbuf(stderr, 0, 2, 0);

    memset(buf, 0x00, ROWS * COLS);
    init_buffer(&clipboard);
    init_buffer(&macro);
    init_buffer(&typing_history);
    for (int i = 0 ; i < 3; i++)
        init_buffer(&clipboard_history[i]);
    name = (char *)malloc(NAME_LEN);
    memset(name, 0x00, NAME_LEN);

    cursor_x = 0;
    cursor_y = 0;

    current_mode = MODE_NORMAL;
}

int is_valid_to_process(char key) {
    if (current_mode & MODE_NORMAL) {
        switch (key) {
            case 'h':
                if (cursor_x > 0)
                    return 1;
                return 0;

            case 'j':
                if (cursor_y < ROWS-1)
                    return 1;
                return 0;

            case 'k':
                if (cursor_y > 0)
                    return 1;
                return 0;

            case 'l':
                if (cursor_x < COLS-1)
                    return 1;
                return 0;
        }
    }

    switch (key) {
        case CTRL_O:
            if (current_mode & MODE_NAMING)
                return 0;
            return 1;

        case CTRL_P:
            if (current_mode & MODE_YANK)
                return 0;
            return 1;

        case CTRL_G:
            if (current_mode & MODE_RECORD)
                return 0;
            return 1;

        case CTRL_N:
            if (current_mode & MODE_INSERT)
                return 0;
            return 1;
    }

    return 1;
}

int is_valid_key(char key) {
    if (0x20 <= key && key <= 0x7e)
        return 1;

    switch (key) {
        case CTRL_E: // Exit
        case CTRL_F: // Record
        case CTRL_G: // Playback
        case CTRL_N: // Naming
        case CTRL_O: // Insert
        case CTRL_P: // Paste
        case CTRL_X: // Delete
        case CTRL_Y: // Yank (Copy)

        case CTRL_3: // Paste from the most recent typing
        case CTRL_5: // Paste from the most recent clipboard history
        case CTRL_6: // Paste from the second most recent clipboard history
        case CTRL_7: // Paste from the thrid most recent clipboard history
            return 1;

        default:
            return 0;
    }
}

int is_valid_cursor() {
    if (cursor_x <= COLS-1 && cursor_y <= ROWS-1)
        return 1;
    return 0;
}

void insert_data(char data) {
    buf[cursor_y][cursor_x] = data;

    if (cursor_x < COLS-1)
        cursor_x++;
    else if (cursor_y < ROWS-1) {
        cursor_x = 0;
        cursor_y++;
    }
}

void delete_data() {
    buf[cursor_y][cursor_x] = '\0';

    if (cursor_x > 0)
        cursor_x--;
    else if (cursor_y > 0) {
        cursor_x = COLS-1;
        cursor_y--;
    }

}

void process_key(char key) {
    // Only Normal mode
    if (current_mode & MODE_NORMAL) {
        switch (key) {
            // Move cursor
            case 'h':
                cursor_x--;
                return;

            case 'j':
                cursor_y++;
                return;

            case 'k':
                cursor_y--;
                return;

            case 'l':
                cursor_x++;
                return;
        }
    }

    switch (key) {
        // Insert
        case CTRL_O:
            if (current_mode & MODE_INSERT) {
                current_mode |= MODE_NORMAL;
                current_mode &= ~MODE_INSERT;
            } else {
                current_mode &= ~MODE_NORMAL;
                current_mode |= MODE_INSERT;
                init_buffer(&typing_history);
            }
            break;

        // Delete
        case CTRL_X:
            delete_data();
            break;

        // Yank
        case CTRL_Y:
            if (current_mode & MODE_YANK) {
                char *start_position = ((char **)clipboard.buf)[0];
                char *end_position = &buf[cursor_y][cursor_x];

                if (start_position > end_position) {
                    char *tmp = end_position;
                    end_position = start_position;
                    start_position = tmp;
                }

                init_buffer(&clipboard);
                append_buffer(&clipboard, start_position, end_position-start_position+1);

                current_mode &= ~MODE_YANK;
            } else {
                if (clipboard_history[2].buf)
                    free(clipboard_history[2].buf);
                clipboard_history[2] = clipboard_history[1];
                clipboard_history[1] = clipboard_history[0];
                clipboard_history[0] = clipboard;

                clipboard.buf = NULL;
                clipboard.len = 0;
                append_pointer(&clipboard, &buf[cursor_y][cursor_x]);
                current_mode |= MODE_YANK;
            }

            break;

        // Paste
        case CTRL_P:
            for (size_t i = 0; i < clipboard.len; i++)
                insert_data(((char *)clipboard.buf)[i]);
            break;

        // Paste from the most recent typing
        case CTRL_3:
            for (size_t i = 0; i < typing_history.len; i++)
                insert_data(((char *)typing_history.buf)[i]);
            break;

        // Paste from the most recent clipboard history
        case CTRL_5:
            for (size_t i = 0; i < clipboard_history[0].len; i++)
                insert_data(((char *)clipboard_history[0].buf)[i]);
            break;

        // Paste from the second most recent clipboard history
        case CTRL_6:
            for (size_t i = 0; i < clipboard_history[1].len; i++)
                insert_data(((char *)clipboard_history[1].buf)[i]);
            break;

        // Paste from the thrid most recent clipboard history
        case CTRL_7:
            for (size_t i = 0; i < clipboard_history[2].len; i++)
                insert_data(((char *)clipboard_history[2].buf)[i]);
            break;

        // Record
        case CTRL_F:
            if (current_mode & MODE_RECORD) {
                // Replace CTRL_F to NULL
                ((char *)macro.buf)[macro.len-1] = '\0';

                current_mode &= ~MODE_RECORD;
            } else {
                init_buffer(&macro);
                current_mode |= MODE_RECORD;
            }

            break;

        // Playback
        case CTRL_G:
            for (size_t i = 0; i < macro.len; i++)
                process_key(((char *)macro.buf)[i]);

            if (!is_valid_cursor()) {
                cursor_x = 0;
                cursor_y = 0;
            }

            break;

        // Naming
        case CTRL_N:
            if (current_mode & MODE_NAMING) {
                current_mode |= MODE_NORMAL;
                current_mode &= ~MODE_NAMING;
            } else {
                current_mode &= ~MODE_NORMAL;
                current_mode |= MODE_NAMING;
                name_idx = 0;
            }

            break;

        default:
            if (current_mode & MODE_INSERT) {
                append_buffer(&typing_history, &key, 1);
                insert_data(key);
            } else if (current_mode & MODE_NAMING) {
                if (name_idx < NAME_LEN)
                    name[name_idx++] = key;
            }
    }
}

int main() {
    init();

    while (1) {
        refresh_screen((char *)buf);
        paint_cursor(NULL, cursor_x + 2, cursor_y + 3, buf[cursor_y][cursor_x]);

        // Display name
        if (name_idx) {
            move_renderer_cursor(NULL, 0, 0);
            RENDER(NULL, "[ ", 2);
            RENDER(NULL, name, name_idx);
            RENDER(NULL, " ]", 2);
        } else {
            move_renderer_cursor(NULL, 0, 0);
            RENDER(NULL, "[ No Name ]", 11);
        }

        // Display current_mode
        move_renderer_cursor(NULL, 0, ROWS+4);
        if (current_mode & MODE_RAW)
            RENDER(NULL, "[ RAW ] ", 8);

        if (current_mode & MODE_NORMAL)
            RENDER(NULL, "[ NORMAL ]", 10);
        else if (current_mode & MODE_INSERT)
            RENDER(NULL, "[ INSERT ]", 10);
        else if (current_mode & MODE_NAMING)
            RENDER(NULL, "[ NAMING ]", 10);

        if (current_mode & MODE_YANK)
            RENDER(NULL, " [ YANK ]", 9);
        if (current_mode & MODE_RECORD)
            RENDER(NULL, " [ RECORD ]", 11);

        move_renderer_cursor(NULL, 0, ROWS+6);
        RENDER(NULL, "> ", 2);
        Buffer key = BUFFER_INIT;
        capture_input(&key);

        // Exit
        if (key.len > 0 && ((char *)key.buf)[0] == CTRL_E) {
            clear_screen(NULL);
            move_renderer_cursor(NULL, 0, 0);
            break;
        }

        // Process...
        for (size_t i = 0; i < key.len; i++) {
            if (!(current_mode & MODE_RAW))
                if (!is_valid_key(((char *)key.buf)[i]))
                    continue;

            if (is_valid_to_process(((char *)key.buf)[i])) {
                // Recording...
                if (current_mode & MODE_RECORD)
                    append_buffer(&macro, &((char *)key.buf)[i], 1);

                process_key(((char *)key.buf)[i]);
            }
        }

        init_buffer(&key);
    }

    return 0;
}
