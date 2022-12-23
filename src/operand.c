#include "operand.h"

#define ADDRESSING_MASK (0b11100)

static enum addressing_mode addressing_mode_table[256] = {
  IMPLIED,   X_INDIRECT, IMPLIED,   X_INDIRECT, ZERO_PAGE,   ZERO_PAGE,   ZERO_PAGE,   ZERO_PAGE,   IMPLIED, IMMEDIATE,  ACCUMULATOR, IMMEDIATE,  ABSOLUTE,   ABSOLUTE,   ABSOLUTE,   ABSOLUTE,
  RELATIVE,  INDIRECT_Y, IMPLIED,   INDIRECT_Y, ZERO_PAGE_X, ZERO_PAGE_X, ZERO_PAGE_X, ZERO_PAGE_X, IMPLIED, ABSOLUTE_Y, IMPLIED,     ABSOLUTE_Y, ABSOLUTE_X, ABSOLUTE_X, ABSOLUTE_X, ABSOLUTE_X,
  ABSOLUTE,  X_INDIRECT, IMPLIED,   X_INDIRECT, ZERO_PAGE,   ZERO_PAGE,   ZERO_PAGE,   ZERO_PAGE,   IMPLIED, IMMEDIATE,  ACCUMULATOR, IMMEDIATE,  ABSOLUTE,   ABSOLUTE,   ABSOLUTE,   ABSOLUTE,
  RELATIVE,  INDIRECT_Y, IMPLIED,   INDIRECT_Y, ZERO_PAGE_X, ZERO_PAGE_X, ZERO_PAGE_X, ZERO_PAGE_X, IMPLIED, ABSOLUTE_Y, IMPLIED,     ABSOLUTE_Y, ABSOLUTE_X, ABSOLUTE_X, ABSOLUTE_X, ABSOLUTE_X,
  IMPLIED,   X_INDIRECT, IMPLIED,   X_INDIRECT, ZERO_PAGE,   ZERO_PAGE,   ZERO_PAGE,   ZERO_PAGE,   IMPLIED, IMMEDIATE,  ACCUMULATOR, IMMEDIATE,  ABSOLUTE,   ABSOLUTE,   ABSOLUTE,   ABSOLUTE,
  RELATIVE,  INDIRECT_Y, IMPLIED,   INDIRECT_Y, ZERO_PAGE_X, ZERO_PAGE_X, ZERO_PAGE_X, ZERO_PAGE_X, IMPLIED, ABSOLUTE_Y, IMPLIED,     ABSOLUTE_Y, ABSOLUTE_X, ABSOLUTE_X, ABSOLUTE_X, ABSOLUTE_X,
  IMPLIED,   X_INDIRECT, IMPLIED,   X_INDIRECT, ZERO_PAGE,   ZERO_PAGE,   ZERO_PAGE,   ZERO_PAGE,   IMPLIED, IMMEDIATE,  ACCUMULATOR, IMMEDIATE,  INDIRECT,   ABSOLUTE,   ABSOLUTE,   ABSOLUTE,
  RELATIVE,  INDIRECT_Y, IMPLIED,   INDIRECT_Y, ZERO_PAGE_X, ZERO_PAGE_X, ZERO_PAGE_X, ZERO_PAGE_X, IMPLIED, ABSOLUTE_Y, IMPLIED,     ABSOLUTE_Y, ABSOLUTE_X, ABSOLUTE_X, ABSOLUTE_X, ABSOLUTE_X,
  IMMEDIATE, X_INDIRECT, IMMEDIATE, X_INDIRECT, ZERO_PAGE,   ZERO_PAGE,   ZERO_PAGE,   ZERO_PAGE,   IMPLIED, IMMEDIATE,  IMPLIED,     IMMEDIATE,  ABSOLUTE,   ABSOLUTE,   ABSOLUTE,   ABSOLUTE,
  RELATIVE,  INDIRECT_Y, IMPLIED,   INDIRECT_Y, ZERO_PAGE_X, ZERO_PAGE_X, ZERO_PAGE_Y, ZERO_PAGE_Y, IMPLIED, ABSOLUTE_Y, IMPLIED,     ABSOLUTE_Y, ABSOLUTE_X, ABSOLUTE_X, ABSOLUTE_Y, ABSOLUTE_Y,
  IMMEDIATE, X_INDIRECT, IMMEDIATE, X_INDIRECT, ZERO_PAGE,   ZERO_PAGE,   ZERO_PAGE,   ZERO_PAGE,   IMPLIED, IMMEDIATE,  IMPLIED,     IMMEDIATE,  ABSOLUTE,   ABSOLUTE,   ABSOLUTE,   ABSOLUTE,
  RELATIVE,  INDIRECT_Y, IMPLIED,   INDIRECT_Y, ZERO_PAGE_X, ZERO_PAGE_X, ZERO_PAGE_Y, ZERO_PAGE_Y, IMPLIED, ABSOLUTE_Y, IMPLIED,     ABSOLUTE_Y, ABSOLUTE_X, ABSOLUTE_X, ABSOLUTE_Y, ABSOLUTE_Y,
  IMMEDIATE, X_INDIRECT, IMMEDIATE, X_INDIRECT, ZERO_PAGE,   ZERO_PAGE,   ZERO_PAGE,   ZERO_PAGE,   IMPLIED, IMMEDIATE,  IMPLIED,     IMMEDIATE,  ABSOLUTE,   ABSOLUTE,   ABSOLUTE,   ABSOLUTE,
  RELATIVE,  INDIRECT_Y, IMPLIED,   INDIRECT_Y, ZERO_PAGE_X, ZERO_PAGE_X, ZERO_PAGE_X, ZERO_PAGE_X, IMPLIED, ABSOLUTE_Y, IMPLIED,     ABSOLUTE_Y, ABSOLUTE_X, ABSOLUTE_X, ABSOLUTE_X, ABSOLUTE_X,
  IMMEDIATE, X_INDIRECT, IMMEDIATE, X_INDIRECT, ZERO_PAGE,   ZERO_PAGE,   ZERO_PAGE,   ZERO_PAGE,   IMPLIED, IMMEDIATE,  IMPLIED,     IMMEDIATE,  ABSOLUTE,   ABSOLUTE,   ABSOLUTE,   ABSOLUTE,
  RELATIVE,  INDIRECT_Y, IMPLIED,   INDIRECT_Y, ZERO_PAGE_X, ZERO_PAGE_X, ZERO_PAGE_X, ZERO_PAGE_X, IMPLIED, ABSOLUTE_Y, IMPLIED,     ABSOLUTE_Y, ABSOLUTE_X, ABSOLUTE_X, ABSOLUTE_X, ABSOLUTE_X
};

uint16_t concat_bytes(uint8_t low_byte, uint8_t high_byte) {
  return (((uint16_t)high_byte) << 8) | (uint16_t)low_byte;
}

static inline uint8_t next_byte(struct cpu *cpu) {
  return cpu->memory.read(++cpu->program_counter);
}

static uint16_t absolute(struct cpu *cpu) {
  uint8_t low_byte = next_byte(cpu);
  return concat_bytes(low_byte, next_byte(cpu));
}

enum addressing_mode get_addressing_mode(uint8_t opcode) {
  return addressing_mode_table[opcode];
}

static uint16_t get_operand(enum addressing_mode addressing_mode, struct cpu *cpu) {
  uint16_t address;
  switch (addressing_mode) {
    case IMMEDIATE:
    case ZERO_PAGE:
      return next_byte(cpu);
    case RELATIVE:
      return cpu->program_counter + (int8_t)next_byte(cpu);
    case ABSOLUTE:
      return absolute(cpu);
    case ABSOLUTE_X:
      return absolute(cpu) + cpu->x;
    case ABSOLUTE_Y:
      return absolute(cpu) + cpu->y;
    case INDIRECT:
      return cpu->memory.read(absolute(cpu));
    case X_INDIRECT:
      address = next_byte(cpu) + cpu->x;
      return concat_bytes(cpu->memory.read(address), cpu->memory.read(address + 1));
    case INDIRECT_Y:
      address = next_byte(cpu);
      return concat_bytes(cpu->memory.read(address), cpu->memory.read(address + 1)) + cpu->y;
    case ZERO_PAGE_X:
      return next_byte(cpu) + cpu->x;
    case ZERO_PAGE_Y:
      return next_byte(cpu) + cpu->y;

    case IMPLIED:
    case ACCUMULATOR:
      panic("Attempted to get operand on implied addressing mode");
    default:
      panic("Unlisted addressing mode taken from opcode");
  }
}

uint16_t get_operand_as_address(enum addressing_mode addressing_mode, struct cpu *cpu) {
  return get_operand(addressing_mode, cpu);
}

uint8_t get_operand_as_value(enum addressing_mode addressing_mode, struct cpu *cpu) {
  uint16_t operand = get_operand(addressing_mode, cpu);
  return addressing_mode == IMMEDIATE ? (uint8_t)operand : cpu->memory.read(operand);
}
