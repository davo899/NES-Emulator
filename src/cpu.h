#pragma once

#include <stdint.h>

#define BIT(N, X) ((X >> N) & (uint8_t)1)
#define NEGATIVE(S)     BIT(7, S)
#define OVERFLOW(S)     BIT(6, S)
#define BREAK(S)        BIT(4, S)
#define DECIMAL(S)      BIT(3, S)
#define INTR_DISABLE(S) BIT(2, S)
#define ZERO(S)         BIT(1, S)
#define CARRY(S)        BIT(0, S)

struct registers {
  uint8_t accumulator;
  uint8_t x;
  uint8_t y;
  uint8_t status;
  uint8_t stack_pointer;
  uint16_t program_counter;
};

void panic(char *error);
