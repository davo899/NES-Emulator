#pragma once

#include <stdint.h>

#define STATUS(S, C) ((S >> C) & (uint8_t)1)
#define NEGATIVE(S)     STATUS(S, 7)
#define OVERFLOW(S)     STATUS(S, 6)
#define BREAK(S)        STATUS(S, 4)
#define DECIMAL(S)      STATUS(S, 3)
#define INTR_DISABLE(S) STATUS(S, 2)
#define ZERO(S)         STATUS(S, 1)
#define CARRY(S)        (S & (uint8_t)1)

struct registers {
  uint8_t accumulator;
  uint8_t x;
  uint8_t y;
  uint8_t status;
  uint8_t stack_pointer;
  uint16_t program_counter;
};
