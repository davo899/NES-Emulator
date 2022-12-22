#include "test_instructions.h"
#include "../test.h"
#include "../../src/instructions.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>

static struct cpu blank_cpu;
static struct registers blank_registers;

static void test_ADC();
static void test_AND();
static void test_ASL();

static void test_BCC();
static void test_BCS();
static void test_BEQ();
static void test_BIT();
static void test_BMI();
static void test_BNE();
static void test_BPL();
static void test_BRK();
static void test_BVC();
static void test_BVS();

static void test_CLC();
static void test_CLD();
static void test_CLI();
static void test_CLV();
static void test_CMP();
static void test_CPX();
static void test_CPY();

static void test_DEC();
static void test_DEX();
static void test_DEY();

static void test_EOR();

static void test_INC();
static void test_INX();
static void test_INY();

static void test_JMP();
static void test_JSR();

static void test_LDA();
static void test_LDX();
static void test_LDY();
static void test_LSR();

static void test_NOP();

static void test_ORA();

static void test_PHA();
static void test_PHP();
static void test_PLA();
static void test_PLP();

static void test_ROL();
static void test_ROR();
static void test_RTI();
static void test_RTS();

static void test_SBC();
static void test_SEC();
static void test_SED();
static void test_SEI();
static void test_STA();
static void test_STX();
static void test_STY();

static void test_TAX();
static void test_TAY();
static void test_TSX();
static void test_TXA();
static void test_TXS();
static void test_TYA();
static void test_set_NZ_flags();

void test_instructions() {
  unit_test("ADC", &test_ADC);
  unit_test("AND", &test_AND);
  unit_test("ASL", &test_ASL);

  unit_test("BCC", &test_BCC);
  unit_test("BCS", &test_BCS);
  unit_test("BEQ", &test_BEQ);
  unit_test("BIT", &test_BIT);
  unit_test("BMI", &test_BMI);
  unit_test("BNE", &test_BNE);
  unit_test("BPL", &test_BPL);
  unit_test("BRK", &test_BRK);
  unit_test("BVC", &test_BVC);
  unit_test("BVS", &test_BVS);

  unit_test("CLC", &test_CLC);
  unit_test("CLD", &test_CLD);
  unit_test("CLI", &test_CLI);
  unit_test("CLV", &test_CLV);
  unit_test("CMP", &test_CMP);
  unit_test("CPX", &test_CPX);
  unit_test("CPY", &test_CPY);

  unit_test("DEC", &test_DEC);
  unit_test("DEX", &test_DEX);
  unit_test("DEY", &test_DEY);

  unit_test("EOR", &test_EOR);

  unit_test("INC", &test_INC);
  unit_test("INX", &test_INX);
  unit_test("INY", &test_INY);

  unit_test("JMP", &test_JMP);
  unit_test("JSR", &test_JSR);

  unit_test("LDA", &test_LDA);
  unit_test("LDX", &test_LDX);
  unit_test("LDY", &test_LDY);
  unit_test("LSR", &test_LSR);

  unit_test("NOP", &test_NOP);

  unit_test("ORA", &test_ORA);

  unit_test("PHA", &test_PHA);
  unit_test("PHP", &test_PHP);
  unit_test("PLA", &test_PLA);
  unit_test("PLP", &test_PLP);

  unit_test("ROL", &test_ROL);
  unit_test("ROR", &test_ROR);
  unit_test("RTI", &test_RTI);
  unit_test("RTS", &test_RTS);

  unit_test("SBC", &test_SBC);
  unit_test("SEC", &test_SEC);
  unit_test("SED", &test_SED);
  unit_test("SEI", &test_SEI);
  unit_test("STA", &test_STA);
  unit_test("STX", &test_STX);
  unit_test("STY", &test_STY);

  unit_test("TAX", &test_TAX);
  unit_test("TAY", &test_TAY);
  unit_test("TSX", &test_TSX);
  unit_test("TXA", &test_TXA);
  unit_test("TXS", &test_TXS);
  unit_test("TYA", &test_TYA);
  unit_test("set_NZ_flags", &test_set_NZ_flags);

  printf("All instruction tests passed.\n");
}

static inline uint8_t *register_from_offset(struct registers *registers, int offset) {
  return (uint8_t *)registers + offset;
}

static void test_ADC() {
  struct cpu cpu = blank_cpu;
  cpu.memory = calloc(2, 1);
  cpu.registers.status = 0b00000001;
  cpu.registers.accumulator = 34;
  cpu.memory[1] = 45;
  perform_instruction(0x69, &cpu.registers, cpu.memory);
  test_bytes_equal(cpu.registers.accumulator, 1 + 34 + 45);

  cpu.registers.program_counter = 0;
  cpu.registers.accumulator = 0x40;
  cpu.memory[1] = 0x40;
  perform_instruction(0x69, &cpu.registers, cpu.memory);
  test_bytes_equal(cpu.registers.accumulator, 0x80);
  test_bytes_equal(cpu.registers.status, 0b11000000);

  cpu.registers.program_counter = 0;
  cpu.registers.status = 0b00001000;
  cpu.registers.accumulator = 0x98;
  cpu.memory[1] = 0x21;
  perform_instruction(0x69, &cpu.registers, cpu.memory);
  test_bytes_equal(cpu.registers.accumulator, 0x19);
  test_bytes_equal(cpu.registers.status, 0b00001001);
  free(cpu.memory);
}

static void test_AND() {
  struct cpu cpu = blank_cpu;
  cpu.memory = calloc(2, 1);
  cpu.registers.accumulator = 0b10100101;
  cpu.memory[1] = 0b11110000;
  perform_instruction(0x29, &cpu.registers, cpu.memory);
  test_bytes_equal(cpu.registers.accumulator, 0b10100000);
  free(cpu.memory);
}

static void test_ASL() {
  struct cpu cpu = blank_cpu;
  cpu.registers.accumulator = 0b10100101;
  perform_instruction(0x0A, &cpu.registers, cpu.memory);
  test_bytes_equal(cpu.registers.accumulator, 0b01001010);
  test_bytes_equal(cpu.registers.status, 0b00000001);
}

static void test_flag_branch(uint8_t opcode, int flag, bool should_branch_when_set) {
  struct cpu cpu = blank_cpu;
  cpu.memory = calloc(2, 1);
  cpu.memory[1] = 10;
  perform_instruction(opcode, &cpu.registers, cpu.memory);
  test_bytes_equal(cpu.registers.program_counter, should_branch_when_set ? 1 : 10);

  cpu.registers.program_counter = 0;
  cpu.registers.status = (uint8_t)1 << flag;
  perform_instruction(opcode, &cpu.registers, cpu.memory);
  test_bytes_equal(cpu.registers.program_counter, should_branch_when_set ? 10 : 1);

  free(cpu.memory);
}

static void test_BCC() { test_flag_branch(0x90, 0, false); }
static void test_BCS() { test_flag_branch(0xB0, 0, true); }
static void test_BNE() { test_flag_branch(0xD0, 1, false); }
static void test_BEQ() { test_flag_branch(0xF0, 1, true); }
static void test_BVC() { test_flag_branch(0x50, 6, false); }
static void test_BVS() { test_flag_branch(0x70, 6, true); }
static void test_BPL() { test_flag_branch(0x10, 7, false); }
static void test_BMI() { test_flag_branch(0x30, 7, true); }

static void test_BIT() {
  struct cpu cpu = blank_cpu;
  cpu.memory = calloc(2, 1);
  cpu.memory[0] = 0b11111111;
  cpu.memory[1] = 0;
  perform_instruction(0x24, &cpu.registers, cpu.memory);
  test_bytes_equal(cpu.registers.status, 0b11000010);

  cpu.registers.program_counter = 0;
  cpu.registers.accumulator = 0b11111111;
  cpu.memory[0] = 0b00111111;
  cpu.memory[1] = 0;
  perform_instruction(0x24, &cpu.registers, cpu.memory);
  test_bytes_equal(cpu.registers.status, 0b00000000);

  free(cpu.memory);
}

static void test_BRK() {
  struct cpu cpu = blank_cpu;
  cpu.memory = calloc(0x10000, 1);
  cpu.registers.program_counter = 0x4629;
  cpu.memory[0xFFFF] = 0x23;
  cpu.memory[0xFFFE] = 0x54;
  perform_instruction(0x00, &cpu.registers, cpu.memory);
  test_bytes_equal(cpu.memory[0x1FF], 0x46);
  test_bytes_equal(cpu.memory[0x1FE], 0x29 + 2);
  test_bytes_equal(cpu.memory[0x1FD], 0b00010000);
  test_bytes_equal(cpu.registers.status, 0b00000100);
  test_bytes_equal(cpu.registers.stack_pointer, 0xFD);
  if (cpu.registers.program_counter != 0x2354) fail("Incorrect PC value");

  free(cpu.memory);
}

static void test_flag_clear(uint8_t opcode, int flag) {
  struct registers registers = blank_registers;
  registers.status = (uint8_t)1 << flag;
  perform_instruction(opcode, &registers, NULL);
  test_bytes_equal(registers.status, 0);
}

static void test_CLC() { test_flag_clear(0x18, 0); }
static void test_CLD() { test_flag_clear(0xD8, 3); }
static void test_CLI() { test_flag_clear(0x58, 2); }
static void test_CLV() { test_flag_clear(0xB8, 6); }

static void test_compare_register(uint8_t opcode, int reg) {
  struct cpu cpu = blank_cpu;
  cpu.memory = calloc(2, 1);
  *register_from_offset(&cpu.registers, reg) = 77;

  cpu.memory[1] = 77;
  perform_instruction(opcode, &cpu.registers, cpu.memory);
  test_bytes_equal(cpu.registers.status, 0b00000011);
  
  cpu.registers.program_counter = 0;
  cpu.memory[1] = 76;
  perform_instruction(opcode, &cpu.registers, cpu.memory);
  test_bytes_equal(cpu.registers.status, 0b00000001);
  
  cpu.registers.program_counter = 0;
  cpu.memory[1] = 78;
  perform_instruction(opcode, &cpu.registers, cpu.memory);
  test_bytes_equal(cpu.registers.status, 0b10000000);

  free(cpu.memory);
}

static void test_CMP() { test_compare_register(0xC9, offsetof(struct registers, accumulator)); }
static void test_CPX() { test_compare_register(0xE0, offsetof(struct registers, x)); }
static void test_CPY() { test_compare_register(0xC0, offsetof(struct registers, y)); }

static void test_inc_dec(uint8_t opcode, struct cpu *cpu, uint8_t *byte, bool inc) {
  *byte = 42;
  perform_instruction(opcode, &cpu->registers, cpu->memory);
  test_bytes_equal(*byte, inc ? 43 : 41);
}

static void test_DEC() {
  struct cpu cpu = blank_cpu;
  cpu.memory = calloc(2, 1);
  test_inc_dec(0xC6, &cpu, &cpu.memory[0], false);
  free(cpu.memory);
}

static void test_DEX() {
  struct cpu cpu = blank_cpu;
  test_inc_dec(0xCA, &cpu, &cpu.registers.x, false);
}

static void test_DEY() {
  struct cpu cpu = blank_cpu;
  test_inc_dec(0x88, &cpu, &cpu.registers.y, false);
}

static void test_EOR() {
  struct cpu cpu = blank_cpu;
  cpu.memory = calloc(2, 1);
  cpu.memory[1] = 0b11110000;
  cpu.registers.accumulator = 0b10101010;
  perform_instruction(0x49, &cpu.registers, cpu.memory);
  test_bytes_equal(cpu.registers.accumulator, 0b01011010);
  free(cpu.memory);
}

static void test_INC() {
  struct cpu cpu = blank_cpu;
  cpu.memory = calloc(2, 1);
  test_inc_dec(0xE6, &cpu, &cpu.memory[0], true);
  free(cpu.memory);
}

static void test_INX() {
  struct cpu cpu = blank_cpu;
  test_inc_dec(0xE8, &cpu, &cpu.registers.x, true);
}

static void test_INY() {
  struct cpu cpu = blank_cpu;
  test_inc_dec(0xC8, &cpu, &cpu.registers.y, true);
}

static void test_JMP() {
  struct cpu cpu = blank_cpu;
  cpu.memory = calloc(3, 1);
  cpu.memory[1] = 0x9F;
  cpu.memory[2] = 0x8C;
  perform_instruction(0x4C, &cpu.registers, cpu.memory);
  if (cpu.registers.program_counter != 0x8C9F) fail("Incorrect PC value");
  free(cpu.memory);
}

static void test_JSR() {
  struct cpu cpu = blank_cpu;
  cpu.memory = calloc(0x200, 1);
  cpu.memory[0xFE] = 0x9F;
  cpu.memory[0xFF] = 0x8C;
  cpu.registers.program_counter = 0xFD;
  perform_instruction(0x20, &cpu.registers, cpu.memory);
  if (cpu.registers.program_counter != 0x8C9F) fail("Incorrect PC value");
  test_bytes_equal(cpu.memory[0x1FF], 0x00);
  test_bytes_equal(cpu.memory[0x1FE], 0xFF);
  free(cpu.memory);
}

static void test_load_register(uint8_t opcode, int reg) {
  struct cpu cpu = blank_cpu;
  cpu.memory = calloc(2, 1);
  cpu.memory[1] = 0xCC;
  perform_instruction(opcode, &cpu.registers, cpu.memory);
  test_bytes_equal(*register_from_offset(&cpu.registers, reg), 0xCC);
  free(cpu.memory);
}

static void test_LDA() { test_load_register(0xA9, offsetof(struct registers, accumulator)); }
static void test_LDX() { test_load_register(0xA2, offsetof(struct registers, x)); }
static void test_LDY() { test_load_register(0xA0, offsetof(struct registers, y)); }

static void test_LSR() {
  struct cpu cpu = blank_cpu;
  cpu.registers.status = 0b10000000;
  cpu.registers.accumulator = 1;
  perform_instruction(0x4A, &cpu.registers, cpu.memory);
  test_bytes_equal(cpu.registers.accumulator, 0);
  test_bytes_equal(cpu.registers.status, 0b00000011);
}

static void test_NOP() { perform_instruction(0xEA, NULL, NULL); }

static void test_ORA() {
  struct cpu cpu = blank_cpu;
  cpu.memory = calloc(2, 1);
  cpu.memory[1] = 0b11110000;
  cpu.registers.accumulator = 0b10101010;
  perform_instruction(0x09, &cpu.registers, cpu.memory);
  test_bytes_equal(cpu.registers.accumulator, 0b11111010);
  free(cpu.memory);
}

static void test_PHA() {
  struct cpu cpu = blank_cpu;
  cpu.memory = calloc(0x200, 1);
  cpu.registers.accumulator = 157;
  perform_instruction(0x48, &cpu.registers, cpu.memory);
  test_bytes_equal(cpu.registers.stack_pointer, 0xFF);
  test_bytes_equal(cpu.memory[0x1FF], 157);
  free(cpu.memory);
}

static void test_PHP() {
  struct cpu cpu = blank_cpu;
  cpu.memory = calloc(0x200, 1);
  cpu.registers.status = 0b11001111;
  perform_instruction(0x08, &cpu.registers, cpu.memory);
  test_bytes_equal(cpu.registers.stack_pointer, 0xFF);
  test_bytes_equal(cpu.memory[0x1FF], 0b11111111);
  free(cpu.memory);
}

static void test_PLA() {
  struct cpu cpu = blank_cpu;
  cpu.memory = calloc(0x200, 1);
  cpu.memory[0x1FF] = 202;
  cpu.registers.stack_pointer = 0xFF;
  perform_instruction(0x68, &cpu.registers, cpu.memory);
  test_bytes_equal(cpu.registers.stack_pointer, 0);
  test_bytes_equal(cpu.registers.accumulator, 202);
  free(cpu.memory);
}

static void test_PLP() {
  struct cpu cpu = blank_cpu;
  cpu.memory = calloc(0x200, 1);
  cpu.memory[0x1FF] = 0b11111111;
  cpu.registers.stack_pointer = 0xFF;
  perform_instruction(0x28, &cpu.registers, cpu.memory);
  test_bytes_equal(cpu.registers.stack_pointer, 0);
  test_bytes_equal(cpu.registers.status, 0b11001111);
  free(cpu.memory);
}

static void test_ROL() {
  struct cpu cpu = blank_cpu;
  cpu.registers.accumulator = 0b10101010;
  perform_instruction(0x2A, &cpu.registers, cpu.memory);
  test_bytes_equal(cpu.registers.accumulator, 0b01010100);
  test_bytes_equal(cpu.registers.status, 0b00000001);
  perform_instruction(0x2A, &cpu.registers, cpu.memory);
  test_bytes_equal(cpu.registers.accumulator, 0b10101001);
  test_bytes_equal(cpu.registers.status, 0b10000000);
}

static void test_ROR() {
  struct cpu cpu = blank_cpu;
  cpu.registers.accumulator = 0b10101010;
  perform_instruction(0x6A, &cpu.registers, cpu.memory);
  test_bytes_equal(cpu.registers.accumulator, 0b01010101);
  test_bytes_equal(cpu.registers.status, 0b00000000);
  perform_instruction(0x6A, &cpu.registers, cpu.memory);
  test_bytes_equal(cpu.registers.accumulator, 0b00101010);
  test_bytes_equal(cpu.registers.status, 0b00000001);
}

static void test_RTI() {
  struct cpu cpu = blank_cpu;
  cpu.memory = calloc(0x200, 1);
  cpu.memory[0x1FF] = 0xAD;
  cpu.memory[0x1FE] = 0x53;
  cpu.memory[0x1FD] = 0b11111111;
  cpu.registers.stack_pointer = 0xFD;
  perform_instruction(0x40, &cpu.registers, cpu.memory);
  test_bytes_equal(cpu.registers.stack_pointer, 0);
  test_bytes_equal(cpu.registers.status, 0b11001111);
  if (cpu.registers.program_counter != 0xAD53) fail("Incorrect PC value");
}

static void test_RTS() {
  struct cpu cpu = blank_cpu;
  cpu.memory = calloc(0x200, 1);
  cpu.memory[0x1FF] = 0xAD;
  cpu.memory[0x1FE] = 0x53;
  cpu.registers.stack_pointer = 0xFE;
  perform_instruction(0x60, &cpu.registers, cpu.memory);
  test_bytes_equal(cpu.registers.stack_pointer, 0);
  if (cpu.registers.program_counter != 0xAD53) fail("Incorrect PC value");
  free(cpu.memory);
}

static void test_SBC() {
  struct cpu cpu = blank_cpu;
  cpu.memory = calloc(2, 1);
  cpu.registers.status = 0b00000000;
  cpu.registers.accumulator = 45;
  cpu.memory[1] = 34;
  perform_instruction(0xE9, &cpu.registers, cpu.memory);
  test_bytes_equal(cpu.registers.accumulator, 45 - 34 - 1);

  cpu.registers.program_counter = 0;
  cpu.registers.accumulator = 0x80;
  cpu.memory[1] = 1;
  perform_instruction(0xE9, &cpu.registers, cpu.memory);
  test_bytes_equal(cpu.registers.accumulator, 0x80);
  test_bytes_equal(cpu.registers.status, 0b01000001);

  cpu.registers.program_counter = 0;
  cpu.registers.status = 0b10001000;
  cpu.registers.accumulator = 0x98;
  cpu.memory[1] = 0x29;
  perform_instruction(0xE9, &cpu.registers, cpu.memory);
  test_bytes_equal(cpu.registers.accumulator, 0x69);
  test_bytes_equal(cpu.registers.status, 0b00001001);
  free(cpu.memory);
}

static void test_flag_set(uint8_t opcode, int flag) {
  struct registers registers = blank_registers;
  perform_instruction(opcode, &registers, NULL);
  test_bytes_equal(registers.status, (uint8_t)1 << flag);
}

static void test_SEC() { test_flag_set(0x38, 0); }
static void test_SED() { test_flag_set(0xF8, 3); }
static void test_SEI() { test_flag_set(0x78, 2); }

static void test_store_register(uint8_t opcode, int reg) {
  struct cpu cpu = blank_cpu;
  cpu.memory = calloc(2, 1);
  *register_from_offset(&cpu.registers, reg) = 0xCC;
  cpu.memory[1] = 0;
  perform_instruction(opcode, &cpu.registers, cpu.memory);
  test_bytes_equal(cpu.memory[0], 0xCC);
  free(cpu.memory);
}

static void test_STA() { test_store_register(0x85, offsetof(struct registers, accumulator)); }
static void test_STX() { test_store_register(0x86, offsetof(struct registers, x)); }
static void test_STY() { test_store_register(0x84, offsetof(struct registers, y)); }

static void test_transfer(uint8_t opcode, int from, int to) {
  struct registers registers = blank_registers;
  *register_from_offset(&registers, from) = 77;
  perform_instruction(opcode, &registers, NULL);
  test_bytes_equal(*register_from_offset(&registers, to), 77);
}

static void test_TAX() { test_transfer(0xAA, offsetof(struct registers, accumulator), offsetof(struct registers, x)); }
static void test_TXA() { test_transfer(0x8A, offsetof(struct registers, x), offsetof(struct registers, accumulator)); }

static void test_TAY() { test_transfer(0xA8, offsetof(struct registers, accumulator), offsetof(struct registers, y)); }
static void test_TYA() { test_transfer(0x98, offsetof(struct registers, y), offsetof(struct registers, accumulator)); }

static void test_TSX() { test_transfer(0xBA, offsetof(struct registers, stack_pointer), offsetof(struct registers, x)); }
static void test_TXS() { test_transfer(0x9A, offsetof(struct registers, x), offsetof(struct registers, stack_pointer)); }

static void test_set_NZ_flags() {
  struct registers registers = blank_registers;
  registers.accumulator = 0;
  perform_instruction(0xAA, &registers, NULL);
  test_bytes_equal(registers.status, 0b00000010);
  
  registers.accumulator = (int8_t)-1;
  perform_instruction(0xAA, &registers, NULL);
  test_bytes_equal(registers.status, 0b10000000);
  
  registers.accumulator = 1;
  perform_instruction(0xAA, &registers, NULL);
  test_bytes_equal(registers.status, 0b00000000);
}
