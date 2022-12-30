#include "cpu.h"
#include "instructions.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void step_cpu(struct cpu *cpu) {
  if (cpu->instruction_cycles_remaining == 0)
    if (cpu->working) {
      perform_instruction(cpu->memory.read(cpu->program_counter), cpu);
      cpu->working = false;
    } else {
      wait_instruction(cpu->memory.read(cpu->program_counter), cpu);
      cpu->working = true;
    }
  else
    cpu->instruction_cycles_remaining--;
}

void load_program_at(uint16_t offset, uint8_t program[], int size, struct cpu *cpu) {
  for (int i = 0; i < size; i++) cpu->memory.write(program[i], offset + i);
}
