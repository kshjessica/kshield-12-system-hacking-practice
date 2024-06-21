#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "account.h"
#include "group.h"
#include "packet.h"

#define BUFFER_SIZE (0x100)

void init() {
  setvbuf(stdin, NULL, _IONBF, 0);
  setvbuf(stdout, NULL, _IONBF, 0);
  setvbuf(stderr, NULL, _IONBF, 0);
}

int main() {
  init();

  while (1) {
    unsigned char buf[BUFFER_SIZE + 2]; // 2 for unicode null byte
    memset(buf, 0x0, BUFFER_SIZE + 2);

    // Receive packet
    uint32_t len = read(0, buf, BUFFER_SIZE);

    process_packet(buf, len);
  }

  return 0;
}
