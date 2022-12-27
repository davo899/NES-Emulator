#include <stdio.h>
#include <stdlib.h>
#include "error.h"
#include "rom_loader.h"
#include "cpu.h"
#include "ppu.h"
#include "apu.h"

struct cpu *cpu;
struct ppu *ppu;
struct apu *apu;
uint8_t *ram;
uint8_t *rom;

static uint8_t NES_read(uint16_t address) {
  if      (address < 0x2000) return ram[address & 0b0000011111111111];
  else if (address < 0x4000) return ppu_read(ppu, address & 0b0000000000000111);
  else if (address < 0x4014) return apu_read(apu, address & 0b0000000000001111);
  else                       return rom[address - 0x4020];
}

static void NES_write(uint8_t data, uint16_t address) {
  if      (address < 0x2000) ram[address & 0b0000011111111111] = data;
  else if (address < 0x4000) ppu_write(ppu, address & 0b0000000000000111, data);
  else if (address < 0x4014) apu_write(apu, address & 0b0000000000001111, data);
  else                       error("Attempted to write to ROM");
}

int main(int argc, char *argv[]) {
  if (argc != 2) error("Incorrect number of arguments: nes <rom_path>");
  
  cpu = calloc(1, sizeof(struct cpu)); check_OOM(cpu);
  ppu = calloc(1, sizeof(struct ppu)); check_OOM(ppu);
  apu = calloc(1, sizeof(struct apu)); check_OOM(apu);
  ram = calloc(2048, 1); check_OOM(ram);
  rom = load_rom(argv[1]);

  cpu->memory.read = &NES_read;
  cpu->memory.write = &NES_write;

  return 0;
}