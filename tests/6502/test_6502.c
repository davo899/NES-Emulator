#include "test_6502.h"
#include "../test.h"
#include "test_instructions.h"
#include "../../src/cpu.h"
#include <stdio.h>
#include <stdlib.h>

static void test_cpu() {
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

  struct cpu *cpu = calloc(1, sizeof(struct cpu));
  cpu->memory = calloc(0x10000, 1);
  cpu->memory[0x33] = 24;
  cpu->memory[0x34] = 7;
  load_program_at(0, test_program, 0x15, cpu);
  while(cpu->registers.program_counter != 0x12) step_cpu(cpu);

  test_bytes_equal(cpu->memory[0x2833], 168);
  free(cpu->memory);
  free(cpu);

  printf("Test program passed.\n");
}

void test_6502() {
  test_instructions();
  test_cpu();

  printf("All 6502 tests passed.\n");
}
