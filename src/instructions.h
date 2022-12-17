#pragma once

#include "cpu.h"
#include "memory.h"

extern void (*instruction_table[256]) (enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);
