#pragma once

#include <stdint.h>

#define BITN(N, X)   ((X >> N) & (uint8_t)1)
#define SET(N, X)    (X |= ((uint8_t)1 << N))
#define CLEAR(N, X)  (X &= ~((uint8_t)1 << N))

#define NEGATIVE_FLAG 7
#define OVERFLOW_FLAG 6
#define BREAK_FLAG 4
#define DECIMAL_FLAG 3
#define INTR_DISABLE_FLAG 2
#define ZERO_FLAG 1
#define CARRY_FLAG 0

struct registers {
  uint8_t accumulator;
  uint8_t x;
  uint8_t y;
  uint8_t status;
  uint8_t stack_pointer;
  uint16_t program_counter;
};

struct cpu {
  struct registers registers;
  uint8_t *memory;
};

void panic(char *error);
