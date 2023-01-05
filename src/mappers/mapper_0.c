#include "mapper_0.h"
#include "../error.h"
#include <stdlib.h>

static uint8_t *PRG_ROM;
static uint8_t *PRG_RAM;
static int prg_banks;
static int chr_present;

static uint8_t mapper_0_read(uint16_t address) {
  if      (0x6000 <= address && address <= 0x7FFF) return PRG_RAM[address & 0x1FFF];
  else if (0x8000 <= address) {
    address &= prg_banks == 1 ? 0x3FFF : 0x7FFF;
    return PRG_ROM[address];
  }
}

static void mapper_0_write(uint8_t data, uint16_t address) {
  if (0x6000 <= address && address <= 0x7FFF) PRG_RAM[address & 0x1FFF] = data;
}

void init_mapper_0(struct memory_mapping *mapper, struct ppu *ppu, FILE *file, uint8_t header[16]) {
  PRG_RAM = malloc(8192 * sizeof(uint8_t)); check_OOM(PRG_RAM);

  prg_banks = header[4];
  chr_present = header[5];
  uint8_t flags_6 = header[6];

  PRG_ROM = malloc(prg_banks * 16384 * sizeof(uint8_t)); check_OOM(PRG_ROM);

  uint8_t trainer[512];
  if (flags_6 & 0b00000100) fread(trainer, 1, 512, file);
  
  uint8_t *head = PRG_ROM;
  for (int i = 0; i < prg_banks; i++) {
    fread(head, 1, 16384, file);
    head += 16384;
  }

  if (chr_present > 0) fread((uint8_t *)&ppu->pattern_table_left, 1, 8192, file);

  mapper->read = &mapper_0_read;
  mapper->write = &mapper_0_write;
}
