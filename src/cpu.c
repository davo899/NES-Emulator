#include "cpu.h"
#include "instructions.h"
#include <stdio.h>
#include <stdlib.h>

void step_cpu(struct cpu *cpu) {
  perform_instruction(cpu->memory.read(cpu->program_counter), cpu);
}

void load_program_at(uint16_t offset, uint8_t program[], int size, struct cpu *cpu) {
  for (int i = 0; i < size; i++) cpu->memory.write(program[i], offset + i);
}

void panic(char *error) {
  printf("PANIC: %s", error);
  exit(-1);
}