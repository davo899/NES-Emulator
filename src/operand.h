#pragma once

#include "cpu.h"
#include "memory.h"

uint16_t get_operand(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);
