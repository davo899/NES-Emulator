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

static uint16_t concat_bytes(uint8_t low_byte, uint8_t high_byte) {
  return (((uint16_t)high_byte) << 8) && (uint16_t)low_byte;
}

static uint8_t next_byte(struct registers *registers, uint8_t *memory) {
  return memory[++registers->program_counter];
}

static uint16_t absolute(struct registers *registers, uint8_t *memory) {
  return concat_bytes(next_byte(registers, memory), next_byte(registers, memory));
}

enum addressing_mode get_addressing_mode(uint8_t opcode) {
  return addressing_mode_table[opcode];
}

uint16_t get_operand(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {
  uint8_t address;
  switch (addressing_mode) {
    case IMMEDIATE:
      return next_byte(registers, memory);
    case RELATIVE:
      return registers->program_counter + (int8_t)next_byte(registers, memory);
    case ABSOLUTE:
      return absolute(registers, memory);
    case ABSOLUTE_X:
      return absolute(registers, memory) + registers->x;
    case ABSOLUTE_Y:
      return absolute(registers, memory) + registers->y;
    case INDIRECT:
      return memory[absolute(registers, memory)];
    case X_INDIRECT:
      address = next_byte(registers, memory) + registers->x;
      return concat_bytes(memory[(uint16_t)address], memory[(uint16_t)(address + 1)]);
    case INDIRECT_Y:
      address = next_byte(registers, memory);
      return concat_bytes(memory[(uint16_t)address], memory[(uint16_t)(address + 1)]) + registers->y;
    case ZERO_PAGE:
      return next_byte(registers, memory);
    case ZERO_PAGE_X:
      return next_byte(registers, memory) + registers->x;
    case ZERO_PAGE_Y:
      return next_byte(registers, memory) + registers->y;

    case IMPLIED:
    case ACCUMULATOR:
      panic("Attempted to get operand on implied addressing mode");
    default:
      panic("Unlisted addressing mode taken from opcode");
  }
}
