#include "test_6502.h"
#include "../test.h"
#include "test_instructions.h"
#include "../../src/cpu.h"
#include <stdio.h>
#include <stdlib.h>

struct cpu cpu;

static uint8_t read(uint16_t address) {
  return cpu.memory.memory[address];
}

static void write(uint8_t value, uint16_t address) {
  cpu.memory.memory[address] = value;
}

static void test_cpu() {
  cpu.memory.read = &read;
  cpu.memory.write = &write;

  uint8_t test_program[] = {
    0xA9, 0x00,       // 0:LDA #0
    0xA6, 0x34,       // 2:LDX 0x34
    0xE0, 0x00,       // 4:CPX #0
    0xF0, 0x07,       // 6:BEQ E
    0x18,             // 8:CLC
    0x65, 0x33,       // 9:ADC 0x33
    0xCA,             // B:DEX
    0x4C, 0x04, 0x00, // C:JMP 4
    0x8D, 0x33, 0x28, // F:STA 0x2833
    0x4C, 0x12, 0x00  //12:JMP 12
  };

  cpu.memory.memory[0x33] = 24;
  cpu.memory.memory[0x34] = 7;
  load_program_at(0, test_program, 0x15, &cpu);
  while(cpu.program_counter != 0x12) step_cpu(&cpu);

  test_bytes_equal(cpu.memory.memory[0x2833], 168);
  printf("Test program passed.\n");
}

void test_6502() {
  test_instructions();
  test_cpu();

  printf("All 6502 tests passed.\n");
}
