#include "common.h"

void panic(const char *msg) {
    perror(msg);
    exit(1);
}
