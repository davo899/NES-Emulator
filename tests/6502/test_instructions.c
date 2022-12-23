#include "test_instructions.h"
#include "../test.h"
#include "../../src/instructions.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>

static struct cpu cpu;

static uint8_t read(uint16_t address) {
  return cpu.memory.memory[address];
}

static void write(uint8_t value, uint16_t address) {
  cpu.memory.memory[address] = value;
}

static const struct cpu blank_cpu = {.memory = {.read = &read, .write = &write}};

static void reset_cpu(void) {
  cpu = blank_cpu;
}

static void test_NOP();

static void test_ADC();
static void test_SBC();

static void test_AND();
static void test_EOR();
static void test_ORA();

static void test_ASL();
static void test_LSR();
static void test_ROL();
static void test_ROR();

static void test_BCC();
static void test_BCS();
static void test_BEQ();
static void test_BNE();
static void test_BMI();
static void test_BPL();
static void test_BVC();
static void test_BVS();

static void test_BIT();
static void test_CMP();
static void test_CPX();
static void test_CPY();

static void test_BRK();
static void test_RTI();

static void test_CLC();
static void test_CLD();
static void test_CLI();
static void test_CLV();
static void test_SEC();
static void test_SED();
static void test_SEI();

static void test_INC();
static void test_INX();
static void test_INY();
static void test_DEC();
static void test_DEX();
static void test_DEY();

static void test_JMP();
static void test_JSR();
static void test_RTS();

static void test_LDA();
static void test_LDX();
static void test_LDY();
static void test_STA();
static void test_STX();
static void test_STY();

static void test_PHA();
static void test_PHP();
static void test_PLA();
static void test_PLP();

static void test_TAX();
static void test_TAY();
static void test_TSX();
static void test_TXA();
static void test_TXS();
static void test_TYA();
static void test_set_NZ_flags();

void test_instructions() {
  unit_test("NOP", &test_NOP);

  unit_test("ADC", &test_ADC);
  unit_test("SBC", &test_SBC);

  unit_test("AND", &test_AND);
  unit_test("EOR", &test_EOR);
  unit_test("ORA", &test_ORA);

  unit_test("ASL", &test_ASL);
  unit_test("LSR", &test_LSR);
  unit_test("ROL", &test_ROL);
  unit_test("ROR", &test_ROR);

  unit_test("BCC", &test_BCC);
  unit_test("BCS", &test_BCS);
  unit_test("BEQ", &test_BEQ);
  unit_test("BNE", &test_BNE);
  unit_test("BMI", &test_BMI);
  unit_test("BPL", &test_BPL);
  unit_test("BVC", &test_BVC);
  unit_test("BVS", &test_BVS);

  unit_test("BIT", &test_BIT);
  unit_test("CMP", &test_CMP);
  unit_test("CPX", &test_CPX);
  unit_test("CPY", &test_CPY);

  unit_test("BRK", &test_BRK);
  unit_test("RTI", &test_RTI);

  unit_test("CLC", &test_CLC);
  unit_test("CLD", &test_CLD);
  unit_test("CLI", &test_CLI);
  unit_test("CLV", &test_CLV);
  unit_test("SEC", &test_SEC);
  unit_test("SED", &test_SED);
  unit_test("SEI", &test_SEI);

  unit_test("INC", &test_INC);
  unit_test("INX", &test_INX);
  unit_test("INY", &test_INY);
  unit_test("DEC", &test_DEC);
  unit_test("DEX", &test_DEX);
  unit_test("DEY", &test_DEY);

  unit_test("JMP", &test_JMP);
  unit_test("JSR", &test_JSR);
  unit_test("RTS", &test_RTS);

  unit_test("LDA", &test_LDA);
  unit_test("LDX", &test_LDX);
  unit_test("LDY", &test_LDY);
  unit_test("STA", &test_STA);
  unit_test("STX", &test_STX);
  unit_test("STY", &test_STY);

  unit_test("PHA", &test_PHA);
  unit_test("PHP", &test_PHP);
  unit_test("PLA", &test_PLA);
  unit_test("PLP", &test_PLP);

  unit_test("TAX", &test_TAX);
  unit_test("TAY", &test_TAY);
  unit_test("TSX", &test_TSX);
  unit_test("TXA", &test_TXA);
  unit_test("TXS", &test_TXS);
  unit_test("TYA", &test_TYA);
  unit_test("set_NZ_flags", &test_set_NZ_flags);

  printf("All instruction tests passed.\n");
}

static inline uint8_t *register_from_offset(struct cpu *cpu, int offset) {
  return (uint8_t *)cpu + offset;
}

static void test_NOP() {
  reset_cpu();
  perform_instruction(0xEA, &cpu);
}

static void test_ADC() {
  reset_cpu();
  cpu.status = 0b00000001;
  cpu.accumulator = 34;
  cpu.memory.memory[1] = 45;
  perform_instruction(0x69, &cpu);
  test_bytes_equal(cpu.accumulator, 1 + 34 + 45);

  cpu.program_counter = 0;
  cpu.accumulator = 0x40;
  cpu.memory.memory[1] = 0x40;
  perform_instruction(0x69, &cpu);
  test_bytes_equal(cpu.accumulator, 0x80);
  test_bytes_equal(cpu.status, 0b11000000);

  cpu.program_counter = 0;
  cpu.status = 0b00001000;
  cpu.accumulator = 0x98;
  cpu.memory.memory[1] = 0x21;
  perform_instruction(0x69, &cpu);
  test_bytes_equal(cpu.accumulator, 0x19);
  test_bytes_equal(cpu.status, 0b00001001);
}

static void test_SBC() {
  reset_cpu();
  cpu.accumulator = 45;
  cpu.memory.memory[1] = 34;
  perform_instruction(0xE9, &cpu);
  test_bytes_equal(cpu.accumulator, 45 - 34 - 1);

  cpu.program_counter = 0;
  cpu.accumulator = 0x40;
  cpu.memory.memory[1] = 0xc0;
  perform_instruction(0xE9, &cpu);
  test_bytes_equal(cpu.accumulator, 0x80);
  test_bytes_equal(cpu.status, 0b11000000);

  cpu.program_counter = 0;
  cpu.status = 0b10001001;
  cpu.accumulator = 0x48;
  cpu.memory.memory[1] = 0x29;
  perform_instruction(0xE9, &cpu);
  test_bytes_equal(cpu.accumulator, 0x19);
  test_bytes_equal(cpu.status, 0b00001001);
}

static void test_AND() {
  reset_cpu();
  cpu.accumulator = 0b10100101;
  cpu.memory.memory[1] = 0b11110000;
  perform_instruction(0x29, &cpu);
  test_bytes_equal(cpu.accumulator, 0b10100000);
}

static void test_EOR() {
  reset_cpu();
  cpu.memory.memory[1] = 0b11110000;
  cpu.accumulator = 0b10101010;
  perform_instruction(0x49, &cpu);
  test_bytes_equal(cpu.accumulator, 0b01011010);
}

static void test_ORA() {
  reset_cpu();
  cpu.memory.memory[1] = 0b11110000;
  cpu.accumulator = 0b10101010;
  perform_instruction(0x09, &cpu);
  test_bytes_equal(cpu.accumulator, 0b11111010);
}

static void test_ASL() {
  reset_cpu();
  cpu.accumulator = 0b10100101;
  perform_instruction(0x0A, &cpu);
  test_bytes_equal(cpu.accumulator, 0b01001010);
  test_bytes_equal(cpu.status, 0b00000001);
}

static void test_LSR() {
  reset_cpu();
  cpu.status = 0b10000000;
  cpu.accumulator = 1;
  perform_instruction(0x4A, &cpu);
  test_bytes_equal(cpu.accumulator, 0);
  test_bytes_equal(cpu.status, 0b00000011);
}

static void test_ROL() {
  reset_cpu();
  cpu.accumulator = 0b10101010;
  perform_instruction(0x2A, &cpu);
  test_bytes_equal(cpu.accumulator, 0b01010100);
  test_bytes_equal(cpu.status, 0b00000001);
  perform_instruction(0x2A, &cpu);
  test_bytes_equal(cpu.accumulator, 0b10101001);
  test_bytes_equal(cpu.status, 0b10000000);
}

static void test_ROR() {
  reset_cpu();
  cpu.accumulator = 0b10101010;
  perform_instruction(0x6A, &cpu);
  test_bytes_equal(cpu.accumulator, 0b01010101);
  test_bytes_equal(cpu.status, 0b00000000);
  perform_instruction(0x6A, &cpu);
  test_bytes_equal(cpu.accumulator, 0b00101010);
  test_bytes_equal(cpu.status, 0b00000001);
}

static void test_flag_branch(uint8_t opcode, int flag, bool should_branch_when_set) {
  reset_cpu();
  cpu.memory.memory[1] = 10;
  perform_instruction(opcode, &cpu);
  test_bytes_equal(cpu.program_counter, should_branch_when_set ? 2 : 12);

  cpu.program_counter = 0;
  cpu.status = (uint8_t)1 << flag;
  perform_instruction(opcode, &cpu);
  test_bytes_equal(cpu.program_counter, should_branch_when_set ? 12 : 2);
}

static void test_BCC() { test_flag_branch(0x90, 0, false); }
static void test_BCS() { test_flag_branch(0xB0, 0, true); }
static void test_BNE() { test_flag_branch(0xD0, 1, false); }
static void test_BEQ() { test_flag_branch(0xF0, 1, true); }
static void test_BVC() { test_flag_branch(0x50, 6, false); }
static void test_BVS() { test_flag_branch(0x70, 6, true); }
static void test_BPL() { test_flag_branch(0x10, 7, false); }
static void test_BMI() { test_flag_branch(0x30, 7, true); }

static void test_compare_register(uint8_t opcode, int reg) {
  reset_cpu();
  *register_from_offset(&cpu, reg) = 77;

  cpu.memory.memory[1] = 77;
  perform_instruction(opcode, &cpu);
  test_bytes_equal(cpu.status, 0b00000011);
  
  cpu.program_counter = 0;
  cpu.memory.memory[1] = 76;
  perform_instruction(opcode, &cpu);
  test_bytes_equal(cpu.status, 0b00000001);
  
  cpu.program_counter = 0;
  cpu.memory.memory[1] = 78;
  perform_instruction(opcode, &cpu);
  test_bytes_equal(cpu.status, 0b10000000);
}

static void test_BIT() {
  reset_cpu();
  cpu.memory.memory[0] = 0b11111111;
  cpu.memory.memory[1] = 0;
  perform_instruction(0x24, &cpu);
  test_bytes_equal(cpu.status, 0b11000010);

  cpu.program_counter = 0;
  cpu.accumulator = 0b11111111;
  cpu.memory.memory[0] = 0b00111111;
  cpu.memory.memory[1] = 0;
  perform_instruction(0x24, &cpu);
  test_bytes_equal(cpu.status, 0b00000000);
}

static void test_CMP() { test_compare_register(0xC9, offsetof(struct cpu, accumulator)); }
static void test_CPX() { test_compare_register(0xE0, offsetof(struct cpu, x)); }
static void test_CPY() { test_compare_register(0xC0, offsetof(struct cpu, y)); }

static void test_BRK() {
  reset_cpu();
  cpu.program_counter = 0x4629;
  cpu.memory.memory[0xFFFF] = 0x23;
  cpu.memory.memory[0xFFFE] = 0x54;
  perform_instruction(0x00, &cpu);
  test_bytes_equal(cpu.memory.memory[0x1FF], 0x46);
  test_bytes_equal(cpu.memory.memory[0x1FE], 0x29 + 2);
  test_bytes_equal(cpu.memory.memory[0x1FD], 0b00010000);
  test_bytes_equal(cpu.status, 0b00000100);
  test_bytes_equal(cpu.stack_pointer, 0xFD);
  if (cpu.program_counter != 0x2354) fail("Incorrect PC value");
}

static void test_RTI() {
  reset_cpu();
  cpu.memory.memory[0x1FF] = 0xAD;
  cpu.memory.memory[0x1FE] = 0x53;
  cpu.memory.memory[0x1FD] = 0b11111111;
  cpu.stack_pointer = 0xFD;
  perform_instruction(0x40, &cpu);
  test_bytes_equal(cpu.stack_pointer, 0);
  test_bytes_equal(cpu.status, 0b11001111);
  if (cpu.program_counter != 0xAD53) fail("Incorrect PC value");
}

static void test_flag_clear(uint8_t opcode, int flag) {
  reset_cpu();
  cpu.status = (uint8_t)1 << flag;
  perform_instruction(opcode, &cpu);
  test_bytes_equal(cpu.status, 0);
}

static void test_CLC() { test_flag_clear(0x18, 0); }
static void test_CLD() { test_flag_clear(0xD8, 3); }
static void test_CLI() { test_flag_clear(0x58, 2); }
static void test_CLV() { test_flag_clear(0xB8, 6); }

static void test_flag_set(uint8_t opcode, int flag) {
  reset_cpu();
  perform_instruction(opcode, &cpu);
  test_bytes_equal(cpu.status, (uint8_t)1 << flag);
}

static void test_SEC() { test_flag_set(0x38, 0); }
static void test_SED() { test_flag_set(0xF8, 3); }
static void test_SEI() { test_flag_set(0x78, 2); }

static void test_inc_dec(uint8_t opcode, struct cpu *cpu, uint8_t *byte, bool inc) {
  reset_cpu();
  *byte = 42;
  perform_instruction(opcode, cpu);
  test_bytes_equal(*byte, inc ? 43 : 41);
}

static void test_INC() { test_inc_dec(0xE6, &cpu, &cpu.memory.memory[0], true); }
static void test_INX() { test_inc_dec(0xE8, &cpu, &cpu.x, true); }
static void test_INY() { test_inc_dec(0xC8, &cpu, &cpu.y, true); }
static void test_DEC() { test_inc_dec(0xC6, &cpu, &cpu.memory.memory[0], false); }
static void test_DEX() { test_inc_dec(0xCA, &cpu, &cpu.x, false); }
static void test_DEY() { test_inc_dec(0x88, &cpu, &cpu.y, false); }

static void test_JMP() {
  reset_cpu();
  cpu.memory.memory[1] = 0x9F;
  cpu.memory.memory[2] = 0x8C;
  perform_instruction(0x4C, &cpu);
  if (cpu.program_counter != 0x8C9F) fail("Incorrect PC value");
}

static void test_JSR() {
  reset_cpu();
  cpu.memory.memory[0xFE] = 0x9F;
  cpu.memory.memory[0xFF] = 0x8C;
  cpu.program_counter = 0xFD;
  perform_instruction(0x20, &cpu);
  if (cpu.program_counter != 0x8C9F) fail("Incorrect PC value");
  test_bytes_equal(cpu.memory.memory[0x1FF], 0x00);
  test_bytes_equal(cpu.memory.memory[0x1FE], 0xFF);
}

static void test_RTS() {
  reset_cpu();
  cpu.memory.memory[0x1FF] = 0xAD;
  cpu.memory.memory[0x1FE] = 0x53;
  cpu.stack_pointer = 0xFE;
  perform_instruction(0x60, &cpu);
  test_bytes_equal(cpu.stack_pointer, 0);
  if (cpu.program_counter != 0xAD53) fail("Incorrect PC value");
}

static void test_load_register(uint8_t opcode, int reg) {
  reset_cpu();
  cpu.memory.memory[1] = 0xCC;
  perform_instruction(opcode, &cpu);
  test_bytes_equal(*register_from_offset(&cpu, reg), 0xCC);
}

static void test_LDA() { test_load_register(0xA9, offsetof(struct cpu, accumulator)); }
static void test_LDX() { test_load_register(0xA2, offsetof(struct cpu, x)); }
static void test_LDY() { test_load_register(0xA0, offsetof(struct cpu, y)); }

static void test_store_register(uint8_t opcode, int reg) {
  reset_cpu();
  *register_from_offset(&cpu, reg) = 0xCC;
  cpu.memory.memory[1] = 0;
  perform_instruction(opcode, &cpu);
  test_bytes_equal(cpu.memory.memory[0], 0xCC);
}

static void test_STA() { test_store_register(0x85, offsetof(struct cpu, accumulator)); }
static void test_STX() { test_store_register(0x86, offsetof(struct cpu, x)); }
static void test_STY() { test_store_register(0x84, offsetof(struct cpu, y)); }

static void test_PHA() {
  reset_cpu();
  cpu.accumulator = 157;
  perform_instruction(0x48, &cpu);
  test_bytes_equal(cpu.stack_pointer, 0xFF);
  test_bytes_equal(cpu.memory.memory[0x1FF], 157);
}

static void test_PHP() {
  reset_cpu();
  cpu.status = 0b11001111;
  perform_instruction(0x08, &cpu);
  test_bytes_equal(cpu.stack_pointer, 0xFF);
  test_bytes_equal(cpu.memory.memory[0x1FF], 0b11111111);
}

static void test_PLA() {
  reset_cpu();
  cpu.memory.memory[0x1FF] = 202;
  cpu.stack_pointer = 0xFF;
  perform_instruction(0x68, &cpu);
  test_bytes_equal(cpu.stack_pointer, 0);
  test_bytes_equal(cpu.accumulator, 202);
}

static void test_PLP() {
  reset_cpu();
  cpu.memory.memory[0x1FF] = 0b11111111;
  cpu.stack_pointer = 0xFF;
  perform_instruction(0x28, &cpu);
  test_bytes_equal(cpu.stack_pointer, 0);
  test_bytes_equal(cpu.status, 0b11001111);
}

static void test_transfer(uint8_t opcode, int from, int to) {
  reset_cpu();
  *register_from_offset(&cpu, from) = 77;
  perform_instruction(opcode, &cpu);
  test_bytes_equal(*register_from_offset(&cpu, to), 77);
}

static void test_TAX() { test_transfer(0xAA, offsetof(struct cpu, accumulator), offsetof(struct cpu, x)); }
static void test_TXA() { test_transfer(0x8A, offsetof(struct cpu, x), offsetof(struct cpu, accumulator)); }

static void test_TAY() { test_transfer(0xA8, offsetof(struct cpu, accumulator), offsetof(struct cpu, y)); }
static void test_TYA() { test_transfer(0x98, offsetof(struct cpu, y), offsetof(struct cpu, accumulator)); }

static void test_TSX() { test_transfer(0xBA, offsetof(struct cpu, stack_pointer), offsetof(struct cpu, x)); }
static void test_TXS() { test_transfer(0x9A, offsetof(struct cpu, x), offsetof(struct cpu, stack_pointer)); }

static void test_set_NZ_flags() {
  reset_cpu();
  cpu.accumulator = 0;
  perform_instruction(0xAA, &cpu);
  test_bytes_equal(cpu.status, 0b00000010);
  
  cpu.accumulator = (int8_t)-1;
  perform_instruction(0xAA, &cpu);
  test_bytes_equal(cpu.status, 0b10000000);
  
  cpu.accumulator = 1;
  perform_instruction(0xAA, &cpu);
  test_bytes_equal(cpu.status, 0b00000000);
}
