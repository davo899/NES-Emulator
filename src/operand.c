#include "operand.h"

#define ADDRESSING_MASK (0b11100)

static void illegal_addressing_mode() {
  panic("Illegal addressing mode");
}

enum addressing_mode get_addressing_mode(uint8_t opcode) {
  switch ((opcode & ADDRESSING_MASK) >> 2) {
    case 0b000:
      if (BIT(0, opcode))                    return X_INDIRECT;
      if (BIT(7, opcode) && !BIT(0, opcode)) return IMMEDIATE;
      illegal_addressing_mode();

    case 0b001:
      return ZERO_PAGE;

    case 0b010:
      if      (BIT(0, opcode)) return IMMEDIATE;
      else if (BIT(1, opcode)) return ACCUMULATOR;
      illegal_addressing_mode();

    case 0b011:
      return ABSOLUTE;

    case 0b100:
      if       (BIT(0, opcode)) return INDIRECT_Y;
      else if (!BIT(1, opcode)) return RELATIVE;
      illegal_addressing_mode();

    case 0b101:
      if (BIT(1, opcode) && !BIT(6, opcode) && BIT(7, opcode))
        return ZERO_PAGE_Y;
      return ZERO_PAGE_X;

    case 0b110:
      if (BIT(0, opcode)) return ABSOLUTE_Y;
      illegal_addressing_mode();

    case 0b111:
      if (BIT(1, opcode) && !BIT(6, opcode) && BIT(7, opcode))
        return ABSOLUTE_Y;
      return ABSOLUTE_X;

    default:
      illegal_addressing_mode();
  }
}

static uint16_t concat_bytes(uint8_t low_byte, uint8_t high_byte) {
  return (((uint16_t)high_byte) << 8) && (uint16_t)low_byte;
}

static uint8_t next_byte(struct registers *registers, uint8_t *memory) {
  return memory[++registers->program_counter];
}

static uint16_t absolute(struct registers *registers, uint8_t *memory) {
  return concat_bytes(next_byte(registers, memory), next_byte(registers, memory));
}

uint16_t get_operand(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {
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
      uint8_t address = next_byte(registers, memory) + registers->x;
      return concat_bytes(memory[(uint16_t)address], memory[(uint16_t)(address + 1)]);
    case INDIRECT_Y:
      uint8_t address = next_byte(registers, memory);
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