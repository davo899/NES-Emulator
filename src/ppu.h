#pragma once

#include "cpu.h"
#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#define PPU_CONTROL 0
#define PPU_MASK 1
#define PPU_STATUS 2
#define OAM_ADDRESS 3
#define OAM_DATA 4
#define PPU_SCROLL 5
#define PPU_ADDRESS 6
#define PPU_DATA 7

enum mirror_mode {
  HORIZONTAL,
  VERTICAL
};

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
  uint16_t temp_address;
  uint16_t address;
  uint8_t data;
  uint8_t oam_dma;
  uint8_t data_buffer;
  bool address_latch;

  struct sprite oam[64];
  struct tile pattern_table_left[16][16];
  struct tile pattern_table_right[16][16];
  uint8_t nametable_0[0x400];
  uint8_t nametable_1[0x400];
  uint8_t palette_ram[0x20];

  uint8_t fine_x;
  uint8_t next_tile_low_plane;
  uint8_t next_tile_high_plane;
  uint8_t next_tile_id;
  uint8_t next_tile_attribute;

  uint16_t pattern_low_shifter;
  uint16_t pattern_high_shifter;
  uint16_t attribute_low_shifter;
  uint16_t attribute_high_shifter;

  enum mirror_mode mirror_mode;
  int scanline;
  int cycle;
};

void step_ppu(struct ppu *ppu, struct cpu *cpu, SDL_Renderer* rend);
uint8_t ppu_read(struct ppu *ppu, uint16_t address);
void ppu_write(struct ppu *ppu, uint16_t address, uint8_t data);

extern uint8_t NES_palette[64][3];
