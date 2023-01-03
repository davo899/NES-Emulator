#include "operand.h"
#include "error.h"
#include <stdlib.h>
#include <stdio.h>

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
  uint8_t target;
  switch (addressing_mode) {
    case IMMEDIATE:
    case ZERO_PAGE:
      return next_byte(cpu);
    case RELATIVE:
      return cpu->program_counter + (int8_t)next_byte(cpu) + 1;
    case ABSOLUTE:
      return absolute(cpu);
    case ABSOLUTE_X:
      return absolute(cpu) + cpu->x;
    case ABSOLUTE_Y:
      return absolute(cpu) + cpu->y;
    case INDIRECT:
      address = absolute(cpu);
      return concat_bytes(cpu->memory.read(address), cpu->memory.read(address + 1));
    case X_INDIRECT:
      target = next_byte(cpu) + cpu->x;
      return concat_bytes(cpu->memory.read(target), cpu->memory.read((uint8_t)(target + 1)));
    case INDIRECT_Y:
      target = next_byte(cpu);
      return concat_bytes(cpu->memory.read(target), cpu->memory.read((uint8_t)(target + 1))) + cpu->y;
    case ZERO_PAGE_X:
      return (uint8_t)(next_byte(cpu) + cpu->x);
    case ZERO_PAGE_Y:
      return (uint8_t)(next_byte(cpu) + cpu->y);

    case IMPLIED:
    case ACCUMULATOR:
      error("Attempted to get operand on implied addressing mode");
    default:
      error("Unlisted addressing mode taken from opcode");
  }
}

char *disassemble_operand(struct cpu *cpu) {
  enum addressing_mode addressing_mode = get_addressing_mode(cpu->memory.read(cpu->program_counter));

  char *text = malloc(10 * sizeof(char));
  switch (addressing_mode) {
    case IMMEDIATE:
      sprintf(text, "#%02x", cpu->memory.read(cpu->program_counter + 1));
      break;
    case ZERO_PAGE:
      sprintf(text, "$%02x", cpu->memory.read(cpu->program_counter + 1));
      break;
    case RELATIVE:
      sprintf(text, ">%02x", cpu->program_counter + 2 + (int8_t)cpu->memory.read(cpu->program_counter + 1));
      break;
    case ABSOLUTE:
      sprintf(text, "$%02x%02x", cpu->memory.read(cpu->program_counter + 2), cpu->memory.read(cpu->program_counter + 1));
      break;
    case ABSOLUTE_X:
      sprintf(text, "$%02x%02x, X", cpu->memory.read(cpu->program_counter + 2), cpu->memory.read(cpu->program_counter + 1));
      break;
    case ABSOLUTE_Y:
      sprintf(text, "$%02x%02x, Y", cpu->memory.read(cpu->program_counter + 2), cpu->memory.read(cpu->program_counter + 1));
      break;
    case INDIRECT:
      sprintf(text, "(%02x%02x)", cpu->memory.read(cpu->program_counter + 2), cpu->memory.read(cpu->program_counter + 1));
      break;
    case X_INDIRECT:
      sprintf(text, "(%02x%02x, X)", cpu->memory.read(cpu->program_counter + 2), cpu->memory.read(cpu->program_counter + 1));
      break;
    case INDIRECT_Y:
      sprintf(text, "(%02x%02x), Y", cpu->memory.read(cpu->program_counter + 2), cpu->memory.read(cpu->program_counter + 1));
      break;
    case ZERO_PAGE_X:
      sprintf(text, "$%02x, X", cpu->memory.read(cpu->program_counter + 1));
      break;
    case ZERO_PAGE_Y:
      sprintf(text, "$%02x, Y", cpu->memory.read(cpu->program_counter + 1));
      break;

    case ACCUMULATOR:
      return "A";
    case IMPLIED:
    default:
      return "";
  }
  return text;
}

uint16_t get_operand_as_address(enum addressing_mode addressing_mode, struct cpu *cpu) {
  return get_operand(addressing_mode, cpu);
}

uint8_t get_operand_as_value(enum addressing_mode addressing_mode, struct cpu *cpu) {
  uint16_t operand = get_operand(addressing_mode, cpu);
  return addressing_mode == IMMEDIATE ? (uint8_t)operand : cpu->memory.read(operand);
}
