#pragma once

#include "cpu.h"
#include "memory.h"

void wait_instruction(uint8_t opcode, struct cpu *cpu);
void perform_instruction(uint8_t opcode, struct cpu *cpu);
char *get_current_instruction_name(struct cpu *cpu);

inline uint8_t pop_byte_from_stack(struct cpu *cpu);
void push_byte_to_stack(uint8_t byte, struct cpu *cpu);
void push_PC_plus_two(struct cpu *cpu);
