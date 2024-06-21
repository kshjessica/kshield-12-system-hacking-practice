#pragma once

#include <stdio.h>
#include <stdlib.h>

#define ROWS (24)
#define COLS (80)
#define NAME_LEN (40)

#define GET_ROW(buf, y) &buf[COLS * (y)]
#define RENDER(render_buffer, data, len) render_buffer ? append_buffer(render_buffer, data, len) : write(1, data, len)

#define CTRL_E (0x05) // Exit
#define CTRL_F (0x06) // Record
#define CTRL_G (0x07) // Playback
#define CTRL_N (0x0E) // Naming
#define CTRL_O (0x0F) // Insert
#define CTRL_P (0x10) // Paste
#define CTRL_X (0x18) // Delete
#define CTRL_Y (0x19) // Yank

#define CTRL_3 (0x1B) // Paste from the most recent typing
#define CTRL_5 (0x1D) // Paste from the most recent clipboard history
#define CTRL_6 (0x1E) // Paste from the second most recent clipboard history
#define CTRL_7 (0x1F) // Paste from the thrid most recent clipboard history

void panic(const char *);
