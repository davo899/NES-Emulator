#include <stdint.h>

struct registers {
  uint8_t accumulator;
  uint8_t x;
  uint8_t y;
  uint8_t status;
  uint8_t stack_pointer;
  uint16_t program_counter;
};
