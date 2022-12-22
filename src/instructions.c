#include "instructions.h"
#include "operand.h"
#include <stdbool.h>

#define BYTE_MAX 0xFF
#define IS_SET true
#define IS_CLEAR false

// Instructions
static void ADC(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);
static void AND(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);
static void ASL(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);

static void BCC(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);
static void BCS(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);
static void BEQ(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);
static void BIT(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);
static void BMI(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);
static void BNE(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);
static void BPL(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);
static void BRK(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);
static void BVC(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);
static void BVS(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);

static void CLC(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);
static void CLD(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);
static void CLI(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);
static void CLV(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);
static void CMP(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);
static void CPX(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);
static void CPY(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);

static void DEC(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);
static void DEX(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);
static void DEY(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);

static void EOR(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);

static void INC(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);
static void INX(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);
static void INY(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);

static void JMP(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);
static void JSR(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);

static void LDA(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);
static void LDX(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);
static void LDY(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);
static void LSR(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);

static void NOP(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);

static void ORA(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);

static void PHA(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);
static void PHP(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);
static void PLA(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);
static void PLP(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);

static void ROL(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);
static void ROR(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);
static void RTI(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);
static void RTS(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);

static void SBC(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);
static void SEC(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);
static void SED(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);
static void SEI(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);
static void STA(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);
static void STX(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);
static void STY(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);

static void TAX(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);
static void TAY(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);
static void TSX(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);
static void TXA(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);
static void TXS(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);
static void TYA(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);

// Illegal Opcode Instructions
static void ALR(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);
static void ANC(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);
static void ANE(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);
static void ARR(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);

static void DCP(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);

static void ISC(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);

static void JAM(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);

static void LAS(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);
static void LAX(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);
static void LXA(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);

static void RLA(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);
static void RRA(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);

static void SAX(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);
static void SBX(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);
static void SHA(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);
static void SHX(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);
static void SHY(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);
static void SLO(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);
static void SRE(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);

static void TAS(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);

//Unsafe SBC
static void UBC(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory);


void (*instruction_table[256]) (enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) = {
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

void perform_instruction(uint8_t opcode, struct registers *registers, uint8_t *memory) {
  instruction_table[opcode](get_addressing_mode(opcode), registers, memory);
}

static uint8_t pop_byte_from_stack(struct registers *registers, uint8_t *memory) {
  return memory[0x100 + registers->stack_pointer++];
}

static void push_byte_to_stack(uint8_t byte, struct registers *registers, uint8_t *memory) {
  memory[0x100 + --registers->stack_pointer] = byte;
}

static void push_PC_plus_two(struct registers *registers, uint8_t *memory) {
  uint16_t return_address = registers->program_counter + 2;
  push_byte_to_stack(return_address >> 8, registers, memory);
  push_byte_to_stack(return_address & 0xFF, registers, memory);
}

static void set_NZ_flags(int8_t value, uint8_t *status_register) {
  CLEAR(ZERO_FLAG, status_register);
  CLEAR(NEGATIVE_FLAG, status_register);

  if (value == 0)     SET(ZERO_FLAG, status_register);
  else if (value < 0) SET(NEGATIVE_FLAG, status_register);
}

static void set_VC_flags(uint8_t *status_register, uint8_t left, uint8_t right, uint8_t result) {
  if ((int)left + (int)right > BYTE_MAX)           SET(CARRY_FLAG, status_register);
  if (BITN(7, (left ^ result) & (right ^ result))) SET(OVERFLOW_FLAG, status_register);
}

static void shift(bool left, enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {
  CLEAR(CARRY_FLAG, &registers->status);
  uint8_t *target = addressing_mode == ACCUMULATOR ? &registers->accumulator : &memory[get_operand_as_address(addressing_mode, registers, memory)];
  if (BITN(left ? 7 : 0, *target)) SET(CARRY_FLAG, &registers->status);
  if (left) *target <<= 1; else *target >>= 1;
  set_NZ_flags(*target, &registers->status);
}

/* Add to Accumulator with Carry */
static void ADC(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {
  uint8_t operand = get_operand_as_value(addressing_mode, registers, memory);
  uint8_t result = registers->accumulator + operand;

  CLEAR(OVERFLOW_FLAG, &registers->status);
  set_VC_flags(&registers->status, registers->accumulator, operand, result);

  if (BITN(CARRY_FLAG, registers->status)) {
    CLEAR(CARRY_FLAG, &registers->status);
    set_VC_flags(&registers->status, result, 1, ++result);
  } else
    CLEAR(CARRY_FLAG, &registers->status);

  if (BITN(DECIMAL_FLAG, registers->status)) {
    if ((result & 0xF) > 9) {
      set_VC_flags(&registers->status, result, 6, result + 6);
      result += 6;
    }

    if ((result >> 4) > 9) {
      set_VC_flags(&registers->status, result, 0x60, result + 0x60);
      result += 0x60;
    }
  }

  registers->accumulator = result;
  set_NZ_flags(result, &registers->status);
}

/* AND with Accumulator */
static void AND(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {
  registers->accumulator &= get_operand_as_value(addressing_mode, registers, memory);
  set_NZ_flags(registers->accumulator, &registers->status);
}

/* Shift Left One Bit */
static void ASL(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {
  shift(true, addressing_mode, registers, memory);
}

static void branch_on_flag(bool set, int flag_bit, enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {
  bool flag_set = BITN(flag_bit, registers->status);
  if ((flag_set && set) || (!flag_set && !set))
    registers->program_counter = get_operand_as_address(addressing_mode, registers, memory);
  else
    registers->program_counter++;
}

/* Branch on Carry Clear */
static void BCC(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {
  branch_on_flag(IS_CLEAR, CARRY_FLAG, addressing_mode, registers, memory);
}

/* Branch on Carry Set */
static void BCS(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {
  branch_on_flag(IS_SET, CARRY_FLAG, addressing_mode, registers, memory);
}

/* Branch on Result Zero */
static void BEQ(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {
  branch_on_flag(IS_SET, ZERO_FLAG, addressing_mode, registers, memory);
}

/* Test Bits in Memory with Accumulator */
static void BIT(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {
  uint8_t operand = get_operand_as_value(addressing_mode, registers, memory);
  registers->status &= 0b00111111;
  registers->status |= operand & 0b11000000;

  CLEAR(ZERO_FLAG, &registers->status);
  if (!(registers->accumulator & operand)) SET(ZERO_FLAG, &registers->status);
}

/* Branch on Result Minus */
static void BMI(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {
  branch_on_flag(IS_SET, NEGATIVE_FLAG, addressing_mode, registers, memory);
}

/* Branch on Result not Zero */
static void BNE(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {
  branch_on_flag(IS_CLEAR, ZERO_FLAG, addressing_mode, registers, memory);
}

/* Branch on Result Plus */
static void BPL(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {
  branch_on_flag(IS_CLEAR, NEGATIVE_FLAG, addressing_mode, registers, memory);
}

/* Force Break */
static void BRK(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {
  push_PC_plus_two(registers, memory);
  push_byte_to_stack(registers->status | ((uint8_t)1 << BREAK_FLAG), registers, memory);
  SET(INTR_DISABLE_FLAG, &registers->status);
  registers->program_counter = concat_bytes(memory[0xFFFE], memory[0xFFFF]);
}

/* Branch on Overflow Clear */
static void BVC(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {
  branch_on_flag(IS_CLEAR, OVERFLOW_FLAG, addressing_mode, registers, memory);
}

/* Branch on Overflow Set */
static void BVS(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {
  branch_on_flag(IS_SET, OVERFLOW_FLAG, addressing_mode, registers, memory);
}

/* Clear Carry Flag */
static void CLC(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {
  CLEAR(CARRY_FLAG, &registers->status);
}

/* Clear Decimal Flag */
static void CLD(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {
  CLEAR(DECIMAL_FLAG, &registers->status);
}

/* Clear Interrupt Disable Flag */
static void CLI(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {
  CLEAR(INTR_DISABLE_FLAG, &registers->status);
}

/* Clear Overflow Flag */
static void CLV(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {
  CLEAR(OVERFLOW_FLAG, &registers->status);
}

static void compare(uint8_t left, uint8_t right, struct registers *registers) {
  CLEAR(CARRY_FLAG, &registers->status);
  CLEAR(NEGATIVE_FLAG, &registers->status);
  CLEAR(ZERO_FLAG, &registers->status);

  if (left == right) {
    SET(ZERO_FLAG, &registers->status);
    SET(CARRY_FLAG, &registers->status);
  } else {
    if (left > right) SET(CARRY_FLAG, &registers->status);
    if (BITN(7, left - right)) SET(NEGATIVE_FLAG, &registers->status);
  }
}

/* Compare with Accumulator */
static void CMP(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {
  compare(registers->accumulator, get_operand_as_value(addressing_mode, registers, memory), registers);
}

/* Compare with Index X */
static void CPX(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {
  compare(registers->x, get_operand_as_value(addressing_mode, registers, memory), registers);
}

/* Compare with Index Y */
static void CPY(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {
  compare(registers->y, get_operand_as_value(addressing_mode, registers, memory), registers);
}

/* Decrement Memory by One */
static void DEC(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {
  set_NZ_flags(--memory[get_operand_as_address(addressing_mode, registers, memory)], &registers->status);
}

/* Decrement Index X by One */
static void DEX(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {
  set_NZ_flags(--registers->x, &registers->status);
}

/* Decrement Index Y by One */
static void DEY(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {
  set_NZ_flags(--registers->y, &registers->status);
}

/* Exclusive-OR with Accumulator */
static void EOR(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {
  registers->accumulator ^= get_operand_as_value(addressing_mode, registers, memory);
  set_NZ_flags(registers->accumulator, &registers->status);
}

/* Increment Memory by One */
static void INC(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {
  set_NZ_flags(++memory[get_operand_as_address(addressing_mode, registers, memory)], &registers->status);
}

/* Increment Index X by One */
static void INX(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {
  set_NZ_flags(++registers->x, &registers->status);
}

/* Increment Index Y by One */
static void INY(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {
  set_NZ_flags(++registers->y, &registers->status);
}

/* Jump to New Location */
static void JMP(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {
  registers->program_counter = get_operand_as_address(addressing_mode, registers, memory);
}

/* Jump to New Location Saving Return Address */
static void JSR(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {
  push_PC_plus_two(registers, memory);
  registers->program_counter = get_operand_as_address(addressing_mode, registers, memory);
}

/* Load Accumulator */
static void LDA(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {
  registers->accumulator = get_operand_as_value(addressing_mode, registers, memory);
  set_NZ_flags(registers->accumulator, &registers->status);
}

/* Load Index X */
static void LDX(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {
  registers->x = get_operand_as_value(addressing_mode, registers, memory);
  set_NZ_flags(registers->x, &registers->status);
}

/* Load Index Y */
static void LDY(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {
  registers->y = get_operand_as_value(addressing_mode, registers, memory);
  set_NZ_flags(registers->y, &registers->status);
}

/* Shift One Bit Right */
static void LSR(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {
  shift(false, addressing_mode, registers, memory);
}

/* No Operation */
static void NOP(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {}

/* OR with Accumulator */
static void ORA(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {
  registers->accumulator |= get_operand_as_value(addressing_mode, registers, memory);
  set_NZ_flags(registers->accumulator, &registers->status);
}

/* Push Accumulator on Stack */
static void PHA(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {
  push_byte_to_stack(registers->accumulator, registers, memory);
}

/* Push Processor Status on Stack */
static void PHP(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {
  push_byte_to_stack(registers->status | 0b00110000, registers, memory);
}

/* Pull Accumulator from Stack */
static void PLA(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {
  registers->accumulator = pop_byte_from_stack(registers, memory);
}

/* Pull Processor Status from Stack */
static void PLP(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {
  registers->status = pop_byte_from_stack(registers, memory) & 0b11001111;
}

/* Rotate One Bit Left */
static void ROL(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {
  uint8_t *target = addressing_mode == ACCUMULATOR ? &registers->accumulator : &memory[get_operand_as_address(addressing_mode, registers, memory)];
  bool carry = BITN(7, *target);
  *target <<= 1;
  *target |= registers->status & 0b00000001;
  CLEAR(CARRY_FLAG, &registers->status);
  if (carry) SET(CARRY_FLAG, &registers->status);
  set_NZ_flags(*target, &registers->status);
}

static void ROR(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {
  uint8_t *target = addressing_mode == ACCUMULATOR ? &registers->accumulator : &memory[get_operand_as_address(addressing_mode, registers, memory)];
  bool carry = BITN(0, *target);
  *target >>= 1;
  *target |= (registers->status & 0b00000001) << 7;
  CLEAR(CARRY_FLAG, &registers->status);
  if (carry) SET(CARRY_FLAG, &registers->status);
  set_NZ_flags(*target, &registers->status);
}

static void RTI(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {}
static void RTS(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {}

static void SBC(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {}

/* Set Carry Flag */
static void SEC(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {
  SET(CARRY_FLAG, &registers->status);
}

/* Set Decimal Flag */
static void SED(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {
  SET(DECIMAL_FLAG, &registers->status);
}

/* Set Interrupt Disable Flag */
static void SEI(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {
  SET(INTR_DISABLE_FLAG, &registers->status);
}

static void STA(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {}
static void STX(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {}
static void STY(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {}

/* Transfer Accumulator to Index X */
static void TAX(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {
  set_NZ_flags(registers->accumulator, &registers->status);
  registers->x = registers->accumulator;
}

/* Transfer Accumulator to Index Y */
static void TAY(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {
  set_NZ_flags(registers->accumulator, &registers->status);
  registers->y = registers->accumulator;
}

/* Transfer Stack Pointer to Index X */
static void TSX(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {
  set_NZ_flags(registers->stack_pointer, &registers->status);
  registers->x = registers->stack_pointer;
}

/* Transfer Index X to Accumulator */
static void TXA(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {
  set_NZ_flags(registers->x, &registers->status);
  registers->accumulator = registers->x;
}

/* Transfer Index X to Stack Pointer */
static void TXS(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {
  registers->stack_pointer = registers->x;
}

/* Transfer Index Y to Accumulator */
static void TYA(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {
  set_NZ_flags(registers->y, &registers->status);
  registers->accumulator = registers->y;
}

// Illegal Opcode Instructions
static void ALR(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {}
static void ANC(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {}
static void ANE(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {}
static void ARR(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {}

static void DCP(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {}

static void ISC(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {}

static void JAM(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {}

static void LAS(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {}
static void LAX(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {}
static void LXA(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {}

static void RLA(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {}
static void RRA(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {}

static void SAX(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {}
static void SBX(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {}
static void SHA(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {}
static void SHX(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {}
static void SHY(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {}
static void SLO(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {}
static void SRE(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {}

static void TAS(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {}

//Unsafe SBC
static void UBC(enum addressing_mode addressing_mode, struct registers *registers, uint8_t *memory) {}
