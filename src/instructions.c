#include "instructions.h"
#include "operand.h"
#include <stdbool.h>

#define BYTE_MAX 0xFF
#define IS_SET true
#define IS_CLEAR false

// Instructions
static void NOP(enum addressing_mode addressing_mode, struct cpu *cpu);

static void ADC(enum addressing_mode addressing_mode, struct cpu *cpu);
static void SBC(enum addressing_mode addressing_mode, struct cpu *cpu);

static void AND(enum addressing_mode addressing_mode, struct cpu *cpu);
static void EOR(enum addressing_mode addressing_mode, struct cpu *cpu);
static void ORA(enum addressing_mode addressing_mode, struct cpu *cpu);

static void ASL(enum addressing_mode addressing_mode, struct cpu *cpu);
static void LSR(enum addressing_mode addressing_mode, struct cpu *cpu);
static void ROL(enum addressing_mode addressing_mode, struct cpu *cpu);
static void ROR(enum addressing_mode addressing_mode, struct cpu *cpu);

static void BCC(enum addressing_mode addressing_mode, struct cpu *cpu);
static void BCS(enum addressing_mode addressing_mode, struct cpu *cpu);
static void BEQ(enum addressing_mode addressing_mode, struct cpu *cpu);
static void BNE(enum addressing_mode addressing_mode, struct cpu *cpu);
static void BMI(enum addressing_mode addressing_mode, struct cpu *cpu);
static void BPL(enum addressing_mode addressing_mode, struct cpu *cpu);
static void BVC(enum addressing_mode addressing_mode, struct cpu *cpu);
static void BVS(enum addressing_mode addressing_mode, struct cpu *cpu);

static void BIT(enum addressing_mode addressing_mode, struct cpu *cpu);
static void CMP(enum addressing_mode addressing_mode, struct cpu *cpu);
static void CPX(enum addressing_mode addressing_mode, struct cpu *cpu);
static void CPY(enum addressing_mode addressing_mode, struct cpu *cpu);

static void BRK(enum addressing_mode addressing_mode, struct cpu *cpu);
static void RTI(enum addressing_mode addressing_mode, struct cpu *cpu);

static void CLC(enum addressing_mode addressing_mode, struct cpu *cpu);
static void CLD(enum addressing_mode addressing_mode, struct cpu *cpu);
static void CLI(enum addressing_mode addressing_mode, struct cpu *cpu);
static void CLV(enum addressing_mode addressing_mode, struct cpu *cpu);
static void SEC(enum addressing_mode addressing_mode, struct cpu *cpu);
static void SED(enum addressing_mode addressing_mode, struct cpu *cpu);
static void SEI(enum addressing_mode addressing_mode, struct cpu *cpu);

static void INC(enum addressing_mode addressing_mode, struct cpu *cpu);
static void INX(enum addressing_mode addressing_mode, struct cpu *cpu);
static void INY(enum addressing_mode addressing_mode, struct cpu *cpu);
static void DEC(enum addressing_mode addressing_mode, struct cpu *cpu);
static void DEX(enum addressing_mode addressing_mode, struct cpu *cpu);
static void DEY(enum addressing_mode addressing_mode, struct cpu *cpu);

static void JMP(enum addressing_mode addressing_mode, struct cpu *cpu);
static void JSR(enum addressing_mode addressing_mode, struct cpu *cpu);
static void RTS(enum addressing_mode addressing_mode, struct cpu *cpu);

static void LDA(enum addressing_mode addressing_mode, struct cpu *cpu);
static void LDX(enum addressing_mode addressing_mode, struct cpu *cpu);
static void LDY(enum addressing_mode addressing_mode, struct cpu *cpu);
static void STA(enum addressing_mode addressing_mode, struct cpu *cpu);
static void STX(enum addressing_mode addressing_mode, struct cpu *cpu);
static void STY(enum addressing_mode addressing_mode, struct cpu *cpu);

static void PHA(enum addressing_mode addressing_mode, struct cpu *cpu);
static void PHP(enum addressing_mode addressing_mode, struct cpu *cpu);
static void PLA(enum addressing_mode addressing_mode, struct cpu *cpu);
static void PLP(enum addressing_mode addressing_mode, struct cpu *cpu);

static void TAX(enum addressing_mode addressing_mode, struct cpu *cpu);
static void TAY(enum addressing_mode addressing_mode, struct cpu *cpu);
static void TSX(enum addressing_mode addressing_mode, struct cpu *cpu);
static void TXA(enum addressing_mode addressing_mode, struct cpu *cpu);
static void TXS(enum addressing_mode addressing_mode, struct cpu *cpu);
static void TYA(enum addressing_mode addressing_mode, struct cpu *cpu);

// Illegal Opcode Instructions
static void ALR(enum addressing_mode addressing_mode, struct cpu *cpu);
static void ANC(enum addressing_mode addressing_mode, struct cpu *cpu);
static void ANE(enum addressing_mode addressing_mode, struct cpu *cpu);
static void ARR(enum addressing_mode addressing_mode, struct cpu *cpu);

static void DCP(enum addressing_mode addressing_mode, struct cpu *cpu);

static void ISC(enum addressing_mode addressing_mode, struct cpu *cpu);

static void JAM(enum addressing_mode addressing_mode, struct cpu *cpu);

static void LAS(enum addressing_mode addressing_mode, struct cpu *cpu);
static void LAX(enum addressing_mode addressing_mode, struct cpu *cpu);
static void LXA(enum addressing_mode addressing_mode, struct cpu *cpu);

static void RLA(enum addressing_mode addressing_mode, struct cpu *cpu);
static void RRA(enum addressing_mode addressing_mode, struct cpu *cpu);

static void SAX(enum addressing_mode addressing_mode, struct cpu *cpu);
static void SBX(enum addressing_mode addressing_mode, struct cpu *cpu);
static void SHA(enum addressing_mode addressing_mode, struct cpu *cpu);
static void SHX(enum addressing_mode addressing_mode, struct cpu *cpu);
static void SHY(enum addressing_mode addressing_mode, struct cpu *cpu);
static void SLO(enum addressing_mode addressing_mode, struct cpu *cpu);
static void SRE(enum addressing_mode addressing_mode, struct cpu *cpu);

static void TAS(enum addressing_mode addressing_mode, struct cpu *cpu);

//Unsafe SBC
static void UBC(enum addressing_mode addressing_mode, struct cpu *cpu);

char disassembly_table[256][4] = {
  "BRK", "ORA", "JAM", "SLO", "NOP", "ORA", "ASL", "SLO", "PHP", "ORA", "ASL", "ANC", "NOP", "ORA", "ASL", "SLO",
  "BPL", "ORA", "JAM", "SLO", "NOP", "ORA", "ASL", "SLO", "CLC", "ORA", "NOP", "SLO", "NOP", "ORA", "ASL", "SLO",
  "JSR", "AND", "JAM", "RLA", "BIT", "AND", "ROL", "RLA", "PLP", "AND", "ROL", "ANC", "BIT", "AND", "ROL", "RLA",
  "BMI", "AND", "JAM", "RLA", "NOP", "AND", "ROL", "RLA", "SEC", "AND", "NOP", "RLA", "NOP", "AND", "ROL", "RLA",

  "RTI", "EOR", "JAM", "SRE", "NOP", "EOR", "LSR", "SRE", "PHA", "EOR", "LSR", "ALR", "JMP", "EOR", "LSR", "SRE",
  "BVC", "EOR", "JAM", "SRE", "NOP", "EOR", "LSR", "SRE", "CLI", "EOR", "NOP", "SRE", "NOP", "EOR", "LSR", "SRE",
  "RTS", "ADC", "JAM", "RRA", "NOP", "ADC", "ROR", "RRA", "PLA", "ADC", "ROR", "ARR", "JMP", "ADC", "ROR", "RRA",
  "BVS", "ADC", "JAM", "RRA", "NOP", "ADC", "ROR", "RRA", "SEI", "ADC", "NOP", "RRA", "NOP", "ADC", "ROR", "RRA",

  "NOP", "STA", "NOP", "SAX", "STY", "STA", "STX", "SAX", "DEY", "NOP", "TXA", "ANE", "STY", "STA", "STX", "SAX",
  "BCC", "STA", "JAM", "SHA", "STY", "STA", "STX", "SAX", "TYA", "STA", "TXS", "TAS", "SHY", "STA", "SHX", "SHA",
  "LDY", "LDA", "LDX", "LAX", "LDY", "LDA", "LDX", "LAX", "TAY", "LDA", "TAX", "LXA", "LDY", "LDA", "LDX", "LAX",
  "BCS", "LDA", "JAM", "LAX", "LDY", "LDA", "LDX", "LAX", "CLV", "LDA", "TSX", "LAS", "LDY", "LDA", "LDX", "LAX",

  "CPY", "CMP", "NOP", "DCP", "CPY", "CMP", "DEC", "DCP", "INY", "CMP", "DEX", "SBX", "CPY", "CMP", "DEC", "DCP",
  "BNE", "CMP", "JAM", "DCP", "NOP", "CMP", "DEC", "DCP", "CLD", "CMP", "NOP", "DCP", "NOP", "CMP", "DEC", "DCP",
  "CPX", "SBC", "NOP", "ISC", "CPX", "SBC", "INC", "ISC", "INX", "SBC", "NOP", "UBC", "CPX", "SBC", "INC", "ISC",
  "BEQ", "SBC", "JAM", "ISC", "NOP", "SBC", "INC", "ISC", "SED", "SBC", "NOP", "ISC", "NOP", "SBC", "INC", "ISC"
};

uint8_t page_boundary_check[] = {
  0x7D, 0x3D, 0xDD, 0x5D, 0xBD, 0x1D, 0xFD,
  0x79, 0x39, 0xD9, 0x59, 0xB9, 0x19, 0xF9,
  0x71, 0x31, 0xD1, 0x51, 0xB1, 0x11, 0xF1,
  0x70, 0x30, 0xD0, 0x50, 0xB0, 0x10, 0xF0,
  0xBC, 0xBE, 0x90
};

int cycles_table[256] = {
  7, 6, 0, 0, 0, 3, 5, 0, 3, 2, 2, 0, 0, 4, 6, 0,
  2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0,
  6, 6, 0, 0, 3, 3, 5, 0, 4, 2, 2, 0, 4, 4, 6, 0,
  2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0,

  6, 6, 0, 0, 0, 3, 5, 0, 3, 2, 2, 0, 3, 4, 6, 0,
  2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0,
  6, 6, 0, 0, 0, 3, 5, 0, 4, 2, 2, 0, 5, 4, 6, 0,
  2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0,

  0, 6, 2, 0, 3, 3, 3, 0, 2, 0, 2, 0, 4, 4, 4, 0,
  2, 6, 0, 0, 4, 4, 4, 0, 2, 5, 2, 0, 0, 5, 0, 0,
  2, 6, 2, 0, 3, 3, 3, 0, 2, 2, 2, 0, 4, 4, 4, 0,
  2, 5, 0, 0, 4, 4, 4, 0, 2, 4, 2, 0, 4, 4, 4, 0,

  2, 6, 0, 0, 3, 3, 5, 0, 2, 2, 2, 0, 4, 4, 6, 0,
  2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0,
  2, 6, 0, 0, 3, 3, 5, 0, 2, 2, 2, 0, 4, 4, 6, 0,
  2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0
};

void wait_instruction(uint8_t opcode, struct cpu *cpu) {
  cpu->instruction_cycles_remaining += cycles_table[opcode];
}

void (*instruction_table[256]) (enum addressing_mode addressing_mode, struct cpu *cpu) = {
  &BRK, &ORA, &JAM, &SLO, &NOP, &ORA, &ASL, &SLO, &PHP, &ORA, &ASL, &ANC, &NOP, &ORA, &ASL, &SLO,
  &BPL, &ORA, &JAM, &SLO, &NOP, &ORA, &ASL, &SLO, &CLC, &ORA, &NOP, &SLO, &NOP, &ORA, &ASL, &SLO,
  &JSR, &AND, &JAM, &RLA, &BIT, &AND, &ROL, &RLA, &PLP, &AND, &ROL, &ANC, &BIT, &AND, &ROL, &RLA,
  &BMI, &AND, &JAM, &RLA, &NOP, &AND, &ROL, &RLA, &SEC, &AND, &NOP, &RLA, &NOP, &AND, &ROL, &RLA,

  &RTI, &EOR, &JAM, &SRE, &NOP, &EOR, &LSR, &SRE, &PHA, &EOR, &LSR, &ALR, &JMP, &EOR, &LSR, &SRE,
  &BVC, &EOR, &JAM, &SRE, &NOP, &EOR, &LSR, &SRE, &CLI, &EOR, &NOP, &SRE, &NOP, &EOR, &LSR, &SRE,
  &RTS, &ADC, &JAM, &RRA, &NOP, &ADC, &ROR, &RRA, &PLA, &ADC, &ROR, &ARR, &JMP, &ADC, &ROR, &RRA,
  &BVS, &ADC, &JAM, &RRA, &NOP, &ADC, &ROR, &RRA, &SEI, &ADC, &NOP, &RRA, &NOP, &ADC, &ROR, &RRA,

  &NOP, &STA, &NOP, &SAX, &STY, &STA, &STX, &SAX, &DEY, &NOP, &TXA, &ANE, &STY, &STA, &STX, &SAX,
  &BCC, &STA, &JAM, &SHA, &STY, &STA, &STX, &SAX, &TYA, &STA, &TXS, &TAS, &SHY, &STA, &SHX, &SHA,
  &LDY, &LDA, &LDX, &LAX, &LDY, &LDA, &LDX, &LAX, &TAY, &LDA, &TAX, &LXA, &LDY, &LDA, &LDX, &LAX,
  &BCS, &LDA, &JAM, &LAX, &LDY, &LDA, &LDX, &LAX, &CLV, &LDA, &TSX, &LAS, &LDY, &LDA, &LDX, &LAX,

  &CPY, &CMP, &NOP, &DCP, &CPY, &CMP, &DEC, &DCP, &INY, &CMP, &DEX, &SBX, &CPY, &CMP, &DEC, &DCP,
  &BNE, &CMP, &JAM, &DCP, &NOP, &CMP, &DEC, &DCP, &CLD, &CMP, &NOP, &DCP, &NOP, &CMP, &DEC, &DCP,
  &CPX, &SBC, &NOP, &ISC, &CPX, &SBC, &INC, &ISC, &INX, &SBC, &NOP, &UBC, &CPX, &SBC, &INC, &ISC,
  &BEQ, &SBC, &JAM, &ISC, &NOP, &SBC, &INC, &ISC, &SED, &SBC, &NOP, &ISC, &NOP, &SBC, &INC, &ISC
};

void perform_instruction(uint8_t opcode, struct cpu *cpu) {
  instruction_table[opcode](get_addressing_mode(opcode), cpu);
  cpu->program_counter++;
}

char *get_current_instruction_name(struct cpu *cpu) {
  return disassembly_table[cpu->memory.read(cpu->program_counter)];
}

uint8_t pop_byte_from_stack(struct cpu *cpu) {
  return cpu->memory.read(0x100 + ++cpu->stack_pointer);
}

void push_byte_to_stack(uint8_t byte, struct cpu *cpu) {
  cpu->memory.write(byte, 0x100 + cpu->stack_pointer--);
}

void push_PC_plus_two(struct cpu *cpu) {
  uint16_t return_address = cpu->program_counter + 2;
  push_byte_to_stack(return_address >> 8, cpu);
  push_byte_to_stack(return_address & 0xFF, cpu);
}

static void set_NZ_flags(int8_t value, struct cpu *cpu) {
  cpu->status.zero = 0;
  cpu->status.negative = 0;

  if (value == 0)     cpu->status.zero = 1;
  else if (value < 0) cpu->status.negative = 1;
}

static inline uint8_t twos_complement_of(int8_t value) {
  return (uint8_t)(-value);
}

static void add_to_accumulator(uint8_t operand, struct cpu *cpu) {
  if ((int)cpu->accumulator + (int)operand > BYTE_MAX) cpu->status.carry = 1;

  uint8_t result = cpu->accumulator + operand;
  if ((cpu->accumulator ^ result) & (operand ^ result) & 0b10000000) cpu->status.overflow = 1;

  cpu->accumulator = result;
}

/* No Operation */
static void NOP(enum addressing_mode addressing_mode, struct cpu *cpu) {}

/* Add to Accumulator with Carry */
static void ADC(enum addressing_mode addressing_mode, struct cpu *cpu) {
  uint8_t operand = get_operand_as_value(addressing_mode, cpu);
  uint16_t result = (uint16_t)cpu->accumulator + (uint16_t)operand + (uint16_t)cpu->status.carry;
  cpu->status.carry = result > 255;
  cpu->status.overflow = ((result ^ (uint16_t)cpu->accumulator) & (result ^ (uint16_t)operand) & 0b0000000010000000) > 0;
  result &= 0b0000000011111111;
  set_NZ_flags(result, cpu);
  cpu->accumulator = result;
}

/* Subtract from Accumulator with Borrow */
static void SBC(enum addressing_mode addressing_mode, struct cpu *cpu) {
  uint8_t operand = get_operand_as_value(addressing_mode, cpu) ^ 0b11111111;
  uint16_t result = (uint16_t)cpu->accumulator + (uint16_t)operand + (uint16_t)cpu->status.carry;
  cpu->status.carry = result > 255;
  cpu->status.overflow = ((result ^ (uint16_t)cpu->accumulator) & (result ^ (uint16_t)operand) & 0b0000000010000000) > 0;
  result &= 0b0000000011111111;
  set_NZ_flags(result, cpu);
  cpu->accumulator = result;
}

/* AND with Accumulator */
static void AND(enum addressing_mode addressing_mode, struct cpu *cpu) {
  cpu->accumulator &= get_operand_as_value(addressing_mode, cpu);
  set_NZ_flags(cpu->accumulator, cpu);
}

/* Exclusive-OR with Accumulator */
static void EOR(enum addressing_mode addressing_mode, struct cpu *cpu) {
  cpu->accumulator ^= get_operand_as_value(addressing_mode, cpu);
  set_NZ_flags(cpu->accumulator, cpu);
}

/* OR with Accumulator */
static void ORA(enum addressing_mode addressing_mode, struct cpu *cpu) {
  cpu->accumulator |= get_operand_as_value(addressing_mode, cpu);
  set_NZ_flags(cpu->accumulator, cpu);
}

/* Shift Left One Bit */
static void ASL(enum addressing_mode addressing_mode, struct cpu *cpu) {
  if (addressing_mode == ACCUMULATOR) {
    cpu->status.carry = (cpu->accumulator & 0b10000000) > 0;
    cpu->accumulator <<= 1;
    set_NZ_flags(cpu->accumulator, cpu);

  } else {
    uint16_t address = get_operand_as_address(addressing_mode, cpu);
    uint8_t data = cpu->memory.read(address);
    cpu->status.carry = (data & 0b10000000) > 0;
    data <<= 1;
    set_NZ_flags(data, cpu);
    cpu->memory.write(data, address);
  }
}

/* Shift One Bit Right */
static void LSR(enum addressing_mode addressing_mode, struct cpu *cpu) {
  if (addressing_mode == ACCUMULATOR) {
    cpu->status.carry = (cpu->accumulator & 0b00000001) > 0;
    cpu->accumulator >>= 1;
    set_NZ_flags(cpu->accumulator, cpu);

  } else {
    uint16_t address = get_operand_as_address(addressing_mode, cpu);
    uint8_t data = cpu->memory.read(address);
    cpu->status.carry = (data & 0b00000001) > 0;
    data >>= 1;
    set_NZ_flags(data, cpu);
    cpu->memory.write(data, address);
  }
}

/* Rotate One Bit Left */
static void ROL(enum addressing_mode addressing_mode, struct cpu *cpu) {
  uint8_t previous_carry = cpu->status.carry;
  if (addressing_mode == ACCUMULATOR) {
    cpu->status.carry = (cpu->accumulator & 0b10000000) > 0;
    cpu->accumulator <<= 1;
    cpu->accumulator |= previous_carry;
    set_NZ_flags(cpu->accumulator, cpu);

  } else {
    uint16_t address = get_operand_as_address(addressing_mode, cpu);
    uint8_t data = cpu->memory.read(address);
    cpu->status.carry = (data & 0b10000000) > 0;
    data <<= 1;
    data |= previous_carry;
    set_NZ_flags(data, cpu);
    cpu->memory.write(data, address);
  }
}

/* Rotate One Bit Right */
static void ROR(enum addressing_mode addressing_mode, struct cpu *cpu) {
  uint8_t previous_carry = cpu->status.carry << 7;
  if (addressing_mode == ACCUMULATOR) {
    cpu->status.carry = (cpu->accumulator & 0b00000001) > 0;
    cpu->accumulator >>= 1;
    cpu->accumulator |= previous_carry;
    set_NZ_flags(cpu->accumulator, cpu);

  } else {
    uint16_t address = get_operand_as_address(addressing_mode, cpu);
    uint8_t data = cpu->memory.read(address);
    cpu->status.carry = (data & 0b00000001) > 0;
    data >>= 1;
    data |= previous_carry;
    set_NZ_flags(data, cpu);
    cpu->memory.write(data, address);
  }
}

static void branch_on_flag(bool should_branch, enum addressing_mode addressing_mode, struct cpu *cpu) {
  if (should_branch) cpu->program_counter = get_operand_as_address(addressing_mode, cpu);
  else               cpu->program_counter++;
}

/* Branch on Carry Clear */
static void BCC(enum addressing_mode addressing_mode, struct cpu *cpu) {
  branch_on_flag(cpu->status.carry == 0, addressing_mode, cpu);
}

/* Branch on Carry Set */
static void BCS(enum addressing_mode addressing_mode, struct cpu *cpu) {
  branch_on_flag(cpu->status.carry == 1, addressing_mode, cpu);
}

/* Branch on Result Zero */
static void BEQ(enum addressing_mode addressing_mode, struct cpu *cpu) {
  branch_on_flag(cpu->status.zero == 1, addressing_mode, cpu);
}

/* Branch on Result not Zero */
static void BNE(enum addressing_mode addressing_mode, struct cpu *cpu) {
  branch_on_flag(cpu->status.zero == 0, addressing_mode, cpu);
}

/* Branch on Result Minus */
static void BMI(enum addressing_mode addressing_mode, struct cpu *cpu) {
  branch_on_flag(cpu->status.negative == 1, addressing_mode, cpu);
}

/* Branch on Result Plus */
static void BPL(enum addressing_mode addressing_mode, struct cpu *cpu) {
  branch_on_flag(cpu->status.negative == 0, addressing_mode, cpu);
}

/* Branch on Overflow Clear */
static void BVC(enum addressing_mode addressing_mode, struct cpu *cpu) {
  branch_on_flag(cpu->status.overflow == 0, addressing_mode, cpu);
}

/* Branch on Overflow Set */
static void BVS(enum addressing_mode addressing_mode, struct cpu *cpu) {
  branch_on_flag(cpu->status.overflow == 1, addressing_mode, cpu);
}

/* Test Bits in Memory with Accumulator */
static void BIT(enum addressing_mode addressing_mode, struct cpu *cpu) {
  uint8_t operand = get_operand_as_value(addressing_mode, cpu);
  cpu->status.byte &= 0b00111111;
  cpu->status.byte |= operand & 0b11000000;
  cpu->status.zero = (cpu->accumulator & operand) == 0;
}

static void compare(uint8_t left, uint8_t right, struct cpu *cpu) {
  cpu->status.carry = left >= right;
  cpu->status.zero = left == right;
  cpu->status.negative = ((int8_t)(left - right)) < 0;
}

/* Compare with Accumulator */
static void CMP(enum addressing_mode addressing_mode, struct cpu *cpu) {
  compare(cpu->accumulator, get_operand_as_value(addressing_mode, cpu), cpu);
}

/* Compare with Index X */
static void CPX(enum addressing_mode addressing_mode, struct cpu *cpu) {
  compare(cpu->x, get_operand_as_value(addressing_mode, cpu), cpu);
}

/* Compare with Index Y */
static void CPY(enum addressing_mode addressing_mode, struct cpu *cpu) {
  compare(cpu->y, get_operand_as_value(addressing_mode, cpu), cpu);
}

/* Force Break */
static void BRK(enum addressing_mode addressing_mode, struct cpu *cpu) {
  push_PC_plus_two(cpu);
  push_byte_to_stack(cpu->status.byte | ((uint8_t)1 << 4), cpu);
  cpu->status.interrupt_disable = 1;
  cpu->program_counter = concat_bytes(cpu->memory.read(0xFFFE) - 1, cpu->memory.read(0xFFFF));
}

/* Return from Interrupt */
static void RTI(enum addressing_mode addressing_mode, struct cpu *cpu) {
  cpu->status.byte = pop_byte_from_stack(cpu) & 0b11001111;
  RTS(addressing_mode, cpu);
  cpu->program_counter--;
}

/* Clear Carry Flag */
static void CLC(enum addressing_mode addressing_mode, struct cpu *cpu) {
  cpu->status.carry = 0;
}

/* Clear Decimal Flag */
static void CLD(enum addressing_mode addressing_mode, struct cpu *cpu) {
  cpu->status.decimal = 0;
}

/* Clear Interrupt Disable Flag */
static void CLI(enum addressing_mode addressing_mode, struct cpu *cpu) {
  cpu->status.interrupt_disable = 0;
}

/* Clear Overflow Flag */
static void CLV(enum addressing_mode addressing_mode, struct cpu *cpu) {
  cpu->status.overflow = 0;
}

/* Set Carry Flag */
static void SEC(enum addressing_mode addressing_mode, struct cpu *cpu) {
  cpu->status.carry = 1;
}

/* Set Decimal Flag */
static void SED(enum addressing_mode addressing_mode, struct cpu *cpu) {
  cpu->status.decimal = 1;
}

/* Set Interrupt Disable Flag */
static void SEI(enum addressing_mode addressing_mode, struct cpu *cpu) {
  cpu->status.interrupt_disable = 1;
}

/* Increment Memory by One */
static void INC(enum addressing_mode addressing_mode, struct cpu *cpu) {
  uint16_t address = get_operand_as_address(addressing_mode, cpu);
  uint8_t value = cpu->memory.read(address);
  cpu->memory.write(++value, address);
  set_NZ_flags(value, cpu);
}

/* Increment Index X by One */
static void INX(enum addressing_mode addressing_mode, struct cpu *cpu) {
  set_NZ_flags(++cpu->x, cpu);
}

/* Increment Index Y by One */
static void INY(enum addressing_mode addressing_mode, struct cpu *cpu) {
  set_NZ_flags(++cpu->y, cpu);
}

/* Decrement Memory by One */
static void DEC(enum addressing_mode addressing_mode, struct cpu *cpu) {
  uint16_t address = get_operand_as_address(addressing_mode, cpu);
  uint8_t value = cpu->memory.read(address);
  cpu->memory.write(--value, address);
  set_NZ_flags(value, cpu);
}

/* Decrement Index X by One */
static void DEX(enum addressing_mode addressing_mode, struct cpu *cpu) {
  set_NZ_flags(--cpu->x, cpu);
}

/* Decrement Index Y by One */
static void DEY(enum addressing_mode addressing_mode, struct cpu *cpu) {
  set_NZ_flags(--cpu->y, cpu);
}

/* Jump to New Location */
static void JMP(enum addressing_mode addressing_mode, struct cpu *cpu) {
  cpu->program_counter = get_operand_as_address(addressing_mode, cpu) - 1;
}

/* Jump to New Location Saving Return Address */
static void JSR(enum addressing_mode addressing_mode, struct cpu *cpu) {
  push_PC_plus_two(cpu);
  JMP(addressing_mode, cpu);
}

/* Return from Subroutine */
static void RTS(enum addressing_mode addressing_mode, struct cpu *cpu) {
  uint8_t low_byte = pop_byte_from_stack(cpu);
  cpu->program_counter = concat_bytes(low_byte, pop_byte_from_stack(cpu));
}

/* Load Accumulator */
static void LDA(enum addressing_mode addressing_mode, struct cpu *cpu) {
  cpu->accumulator = get_operand_as_value(addressing_mode, cpu);
  set_NZ_flags(cpu->accumulator, cpu);
}

/* Load Index X */
static void LDX(enum addressing_mode addressing_mode, struct cpu *cpu) {
  cpu->x = get_operand_as_value(addressing_mode, cpu);
  set_NZ_flags(cpu->x, cpu);
}

/* Load Index Y */
static void LDY(enum addressing_mode addressing_mode, struct cpu *cpu) {
  cpu->y = get_operand_as_value(addressing_mode, cpu);
  set_NZ_flags(cpu->y, cpu);
}

/* Store Accumulator in Memory */
static void STA(enum addressing_mode addressing_mode, struct cpu *cpu) {
  cpu->memory.write(cpu->accumulator, get_operand_as_address(addressing_mode, cpu));
}

/* Store Index X in Memory */
static void STX(enum addressing_mode addressing_mode, struct cpu *cpu) {
  cpu->memory.write(cpu->x, get_operand_as_address(addressing_mode, cpu));
}

/* Store Index Y in Memory */
static void STY(enum addressing_mode addressing_mode, struct cpu *cpu) {
  cpu->memory.write(cpu->y, get_operand_as_address(addressing_mode, cpu));
}

/* Push Accumulator on Stack */
static void PHA(enum addressing_mode addressing_mode, struct cpu *cpu) {
  push_byte_to_stack(cpu->accumulator, cpu);
}

/* Push Processor Status on Stack */
static void PHP(enum addressing_mode addressing_mode, struct cpu *cpu) {
  push_byte_to_stack(cpu->status.byte | 0b00110000, cpu);
}

/* Pull Accumulator from Stack */
static void PLA(enum addressing_mode addressing_mode, struct cpu *cpu) {
  cpu->accumulator = pop_byte_from_stack(cpu);
  set_NZ_flags(cpu->accumulator, cpu);
}

/* Pull Processor Status from Stack */
static void PLP(enum addressing_mode addressing_mode, struct cpu *cpu) {
  cpu->status.byte = pop_byte_from_stack(cpu) & 0b11001111;
}

/* Transfer Accumulator to Index X */
static void TAX(enum addressing_mode addressing_mode, struct cpu *cpu) {
  set_NZ_flags(cpu->accumulator, cpu);
  cpu->x = cpu->accumulator;
}

/* Transfer Accumulator to Index Y */
static void TAY(enum addressing_mode addressing_mode, struct cpu *cpu) {
  set_NZ_flags(cpu->accumulator, cpu);
  cpu->y = cpu->accumulator;
}

/* Transfer Stack Pointer to Index X */
static void TSX(enum addressing_mode addressing_mode, struct cpu *cpu) {
  set_NZ_flags(cpu->stack_pointer, cpu);
  cpu->x = cpu->stack_pointer;
}

/* Transfer Index X to Accumulator */
static void TXA(enum addressing_mode addressing_mode, struct cpu *cpu) {
  set_NZ_flags(cpu->x, cpu);
  cpu->accumulator = cpu->x;
}

/* Transfer Index X to Stack Pointer */
static void TXS(enum addressing_mode addressing_mode, struct cpu *cpu) {
  cpu->stack_pointer = cpu->x;
}

/* Transfer Index Y to Accumulator */
static void TYA(enum addressing_mode addressing_mode, struct cpu *cpu) {
  set_NZ_flags(cpu->y, cpu);
  cpu->accumulator = cpu->y;
}

// Illegal Opcode Instructions
static void ALR(enum addressing_mode addressing_mode, struct cpu *cpu) {}
static void ANC(enum addressing_mode addressing_mode, struct cpu *cpu) {}
static void ANE(enum addressing_mode addressing_mode, struct cpu *cpu) {}
static void ARR(enum addressing_mode addressing_mode, struct cpu *cpu) {}

static void DCP(enum addressing_mode addressing_mode, struct cpu *cpu) {}

static void ISC(enum addressing_mode addressing_mode, struct cpu *cpu) {}

static void JAM(enum addressing_mode addressing_mode, struct cpu *cpu) {}

static void LAS(enum addressing_mode addressing_mode, struct cpu *cpu) {}
static void LAX(enum addressing_mode addressing_mode, struct cpu *cpu) {}
static void LXA(enum addressing_mode addressing_mode, struct cpu *cpu) {}

static void RLA(enum addressing_mode addressing_mode, struct cpu *cpu) {}
static void RRA(enum addressing_mode addressing_mode, struct cpu *cpu) {}

static void SAX(enum addressing_mode addressing_mode, struct cpu *cpu) {}
static void SBX(enum addressing_mode addressing_mode, struct cpu *cpu) {}
static void SHA(enum addressing_mode addressing_mode, struct cpu *cpu) {}
static void SHX(enum addressing_mode addressing_mode, struct cpu *cpu) {}
static void SHY(enum addressing_mode addressing_mode, struct cpu *cpu) {}
static void SLO(enum addressing_mode addressing_mode, struct cpu *cpu) {}
static void SRE(enum addressing_mode addressing_mode, struct cpu *cpu) {}

static void TAS(enum addressing_mode addressing_mode, struct cpu *cpu) {}

//Unsafe SBC
static void UBC(enum addressing_mode addressing_mode, struct cpu *cpu) {}
