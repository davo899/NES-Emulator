#include "memory.h"

uint8_t *memory;

void init_memory(int bytes) {
  memory = calloc(bytes, 1);
}
