#include "utf16.h"

size_t utf16_strlen(const Utf16 *s) {
  size_t len = 0;

  for (const char *p = (const char *)s; *p || *(p + 1); p += 2) {
    len++;
  }

  return len;
}

Utf16 *utf16_strcpy(void *dst, const Utf16 *src) {
  char *d = dst;
  const char *p = (const char *)src;
  while (*p || *(p + 1)) {
    *d = *p;
    *(d + 1) = *(p + 1);

    d += 2;
    p += 2;
  }

  // \x00\x00
  *d = 0;
  *(d + 1) = 0;

  return (Utf16 *)dst;
}