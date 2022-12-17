#pragma once

#include <stdint.h>

#define BITN(N, X) ((X >> N) & (uint8_t)1)
#define NEGATIVE(S)     BITN(7, S)
#define OVERFLOW(S)     BITN(6, S)
#define BREAK(S)        BITN(4, S)
#define DECIMAL(S)      BITN(3, S)
#define INTR_DISABLE(S) BITN(2, S)
#define ZERO(S)         BITN(1, S)
#define CARRY(S)        BITN(0, S)

struct registers {
  uint8_t accumulator;
  uint8_t x;
  uint8_t y;
  uint8_t status;
  uint8_t stack_pointer;
  uint16_t program_counter;
};

void panic(char *error);
