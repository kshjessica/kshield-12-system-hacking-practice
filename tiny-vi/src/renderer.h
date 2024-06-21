#pragma once

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include "buffer.h"
#include "common.h"

void move_renderer_cursor(Buffer *, uint8_t, uint8_t);
void draw_outline(Buffer *);
void draw_screen(Buffer *, char *);

void clear_line(Buffer *);
void clear_screen(Buffer *);

void hide_cursor(Buffer *);
void show_cursor(Buffer *);
void paint_cursor(Buffer *, uint8_t, uint8_t, char);

void help(Buffer *);
void refresh_screen(char *);
