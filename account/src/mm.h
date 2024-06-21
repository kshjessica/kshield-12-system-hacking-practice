#pragma once

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

#define MEMORY_SIZE (0x1000)

#define CHUNK_STATUS_NONE (0)
#define CHUNK_STATUS_ALLOCATE (1)
#define CHUNK_STATUS_FREE (2)

typedef struct _Chunk {
  uint8_t status;
  uint32_t size;
  void *ptr;
  struct _Chunk *next;
} Chunk;

typedef struct _MemoryManager {
  void *memory;
  uint32_t cursor;
  Chunk *head;
  Chunk *tail;
} MemoryManager;

MemoryManager *get_mm();
void *mm_alloc(uint32_t size);
int mm_free(void *ptr);
