#pragma once

#include "cpu.h"
#include "memory.h"

void perform_instruction(uint8_t opcode, struct registers *registers, uint8_t *memory);
