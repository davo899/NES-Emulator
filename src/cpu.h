#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "memory.h"

struct memory_mapping {
  uint8_t (*read)(uint16_t address);
  void (*write)(uint8_t value, uint16_t address);
};

struct cpu {
  uint8_t accumulator;
  uint8_t x;
  uint8_t y;

  union {
    struct {
      uint8_t carry : 1;
      uint8_t zero : 1;
      uint8_t interrupt_disable : 1;
      uint8_t decimal : 1;
      uint8_t break_ : 1;
      uint8_t unused : 1;
      uint8_t overflow : 1;
      uint8_t negative : 1;
    };
    uint8_t byte;
  } status;

  uint8_t stack_pointer;
  uint16_t program_counter;
  int instruction_cycles_remaining;
  bool trigger_non_maskable_interrupt;
  struct memory_mapping memory;
};

void step_cpu(struct cpu *cpu);
void load_program_at(uint16_t offset, uint8_t program[], int size, struct cpu *cpu);
