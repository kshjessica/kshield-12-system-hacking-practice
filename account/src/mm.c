#include "mm.h"

static MemoryManager *mm = NULL;

MemoryManager *get_mm() {
  if (mm == NULL) {
    mm = (MemoryManager *)malloc(sizeof(MemoryManager));

    void *mem = mmap(0, MEMORY_SIZE, PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (mem == MAP_FAILED) {
      fprintf(stderr, "fail to mmap\n");
      return NULL;
    }

    mm->memory = mem;
    mm->cursor = 0;
    mm->head = NULL;
    mm->tail = NULL;
  }
  return mm;
}

void *mm_alloc(uint32_t size) {
  if (size == 0) {
    fprintf(stderr, "invalid size\n");
    return NULL;
  }

  MemoryManager *mm = get_mm();

  for (Chunk *c = mm->head; c != NULL; c = c->next) {
    if (c->status == CHUNK_STATUS_FREE && c->size == size) {
      c->status = CHUNK_STATUS_ALLOCATE;
      return c->ptr;
    }
  }

  if (mm->cursor > mm->cursor + size) {
    fprintf(stderr, "overflow\n");
    return NULL;
  }

  if (mm->cursor + size > MEMORY_SIZE) {
    fprintf(stderr, "too big\n");
    return NULL;
  }

  Chunk *c = malloc(sizeof(Chunk));
  c->status = CHUNK_STATUS_ALLOCATE;
  c->size = size;
  c->ptr = mm->memory + mm->cursor;
  c->next = NULL;

  if (mm->head == NULL) {
    mm->head = c;
    mm->tail = c;
  } else {
    mm->tail->next = c;
    mm->tail = c;
  }

  mm->cursor += size;

  return c->ptr;
}

int mm_free(void *ptr) {
  for (Chunk *c = mm->head; c != NULL; c = c->next) {
    if (c->ptr == ptr) {
      c->status = CHUNK_STATUS_FREE;
      return 0;
    }
  }

  fprintf(stderr, "invalid ptr\n");
  return -1;
}