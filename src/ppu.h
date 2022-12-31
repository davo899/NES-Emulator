#pragma once

#include <stdint.h>

struct sprite {
  uint8_t y;
  uint8_t tile_number;
  uint8_t attribute;
  uint8_t x;
};

struct tile {
  uint8_t low_plane[8];
  uint8_t high_plane[8];
};

struct ppu {
  uint8_t control;
  uint8_t mask;
  uint8_t status;
  uint8_t oam_address;
  uint8_t oam_data;
  uint8_t scroll;
  uint8_t address;
  uint8_t data;
  uint8_t oam_dma;

  struct sprite oam[64];
  struct tile pattern_table_left[256];
  struct tile pattern_table_right[256];
  uint8_t nametable_0[0x400];
  uint8_t nametable_1[0x400];
  uint8_t palette_ram[0x20];
};

uint8_t ppu_read(struct ppu *ppu, uint16_t address);
void ppu_write(struct ppu *ppu, uint16_t address, uint8_t data);
