#include "test_instructions.h"
#include "../test.h"
#include "../../src/instructions.h"
#include <stdio.h>

static void test_TAX();
static void test_TAY();
static void test_TSX();
static void test_TXA();
static void test_TXS();
static void test_TYA();
static void test_set_NZ_flags();

void test_instructions() {
  unit_test("TAX", &test_TAX);
  unit_test("TAY", &test_TAY);
  unit_test("TSX", &test_TSX);
  unit_test("TXA", &test_TXA);
  unit_test("TXS", &test_TXS);
  unit_test("TYA", &test_TYA);

  unit_test("set_NZ_flags", &test_set_NZ_flags);
  
  printf("All instruction tests passed.\n");
}

static void test_TAX() {
  struct cpu cpu;
  cpu.registers.accumulator = 77;
  perform_instruction(0xAA, &cpu.registers, cpu.memory);
  test_bytes_equal(cpu.registers.x, 77);
}

static void test_TAY() {
  struct cpu cpu;
  cpu.registers.accumulator = 66;
  perform_instruction(0xA8, &cpu.registers, cpu.memory);
  test_bytes_equal(cpu.registers.y, 66);
}

static void test_TSX() {
  struct cpu cpu;
  cpu.registers.stack_pointer = 55;
  perform_instruction(0xBA, &cpu.registers, cpu.memory);
  test_bytes_equal(cpu.registers.x, 55);
}

static void test_TXA() {
  struct cpu cpu;
  cpu.registers.x = 44;
  perform_instruction(0x8A, &cpu.registers, cpu.memory);
  test_bytes_equal(cpu.registers.accumulator, 44);
}

static void test_TXS() {
  struct cpu cpu;
  cpu.registers.x = 33;
  perform_instruction(0x9A, &cpu.registers, cpu.memory);
  test_bytes_equal(cpu.registers.stack_pointer, 33);
}

static void test_TYA() {
  struct cpu cpu;
  cpu.registers.y = 22;
  perform_instruction(0x98, &cpu.registers, cpu.memory);
  test_bytes_equal(cpu.registers.accumulator, 22);
}

static void test_set_NZ_flags() {
  struct cpu cpu;
  cpu.registers.accumulator = 0;
  perform_instruction(0xAA, &cpu.registers, cpu.memory);
  test_bytes_equal(cpu.registers.status, 0b00000010);
  
  cpu.registers.accumulator = (int8_t)-1;
  perform_instruction(0xAA, &cpu.registers, cpu.memory);
  test_bytes_equal(cpu.registers.status, 0b10000000);
  
  cpu.registers.accumulator = 1;
  perform_instruction(0xAA, &cpu.registers, cpu.memory);
  test_bytes_equal(cpu.registers.status, 0b00000000);
}
