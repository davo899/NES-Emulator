#pragma once

#include "cpu.h"
#include "memory.h"

enum addressing_mode {
  ACCUMULATOR, IMMEDIATE, IMPLIED, RELATIVE,
  ABSOLUTE, ABSOLUTE_X, ABSOLUTE_Y,
  INDIRECT, X_INDIRECT, INDIRECT_Y,
  ZERO_PAGE, ZERO_PAGE_X, ZERO_PAGE_Y
};

enum addressing_mode get_addressing_mode(uint8_t opcode);
uint16_t get_operand_as_address(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);
uint8_t get_operand_as_value(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);
