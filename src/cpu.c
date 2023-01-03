#include "cpu.h"
#include "instructions.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void step_cpu(struct cpu *cpu) {
  if (cpu->instruction_cycles_remaining == 0) {
    if (cpu->trigger_non_maskable_interrupt) {
      cpu->trigger_non_maskable_interrupt = false;
      cpu->instruction_cycles_remaining = 6;

      push_byte_to_stack(cpu->program_counter >> 8, cpu);
      push_byte_to_stack((cpu->program_counter & 0xFF), cpu);
      push_byte_to_stack(cpu->status, cpu);
      SET(INTR_DISABLE_FLAG, &cpu->status);
      cpu->program_counter = ((uint16_t)cpu->memory.read(0xFFFB) << 8) | cpu->memory.read(0xFFFA);

    } else {
      perform_instruction(cpu->memory.read(cpu->program_counter), cpu);
      wait_instruction(cpu->memory.read(cpu->program_counter), cpu);
    }
  }
  cpu->instruction_cycles_remaining--;
}

void load_program_at(uint16_t offset, uint8_t program[], int size, struct cpu *cpu) {
  for (int i = 0; i < size; i++) cpu->memory.write(program[i], offset + i);
}
