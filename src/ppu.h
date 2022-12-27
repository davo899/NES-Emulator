#pragma once

#include <stdint.h>

struct ppu {
  uint8_t oam[256];
  uint8_t *vram;
};

uint8_t ppu_read(struct ppu *ppu, uint16_t address);
void ppu_write(struct ppu *ppu, uint16_t address, uint8_t data);
