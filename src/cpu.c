#include "cpu.h"
#include <stdio.h>
#include <stdlib.h>

void load_program_at(uint16_t offset, uint8_t program[], int size, struct cpu *cpu) {
  for (int i = 0; i < size; i++) {
    cpu->memory[offset + i] = program[i];
  }
}

void panic(char *error) {
  printf("PANIC: %s", error);
  exit(-1);
}