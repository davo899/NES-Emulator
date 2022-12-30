#pragma once

#include "cpu.h"
#include "memory.h"

void wait_instruction(uint8_t opcode, struct cpu *cpu);
void perform_instruction(uint8_t opcode, struct cpu *cpu);
char *get_current_instruction_name(struct cpu *cpu);
