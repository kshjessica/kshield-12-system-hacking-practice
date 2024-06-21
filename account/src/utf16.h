#pragma once

#include <stdio.h>

typedef unsigned short Utf16;
size_t utf16_strlen(const Utf16 *s);
Utf16 *utf16_strcpy(void *dst, const Utf16 *src);