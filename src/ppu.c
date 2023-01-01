#include "ppu.h"
#include <stdint.h>
#include <stdio.h>

uint8_t NES_palette[64][3] = {
  {84, 84, 84},
	{0, 30, 116},
	{8, 16, 144},
	{48, 0, 136},
	{68, 0, 100},
	{92, 0, 48},
	{84, 4, 0},
	{60, 24, 0},
	{32, 42, 0},
	{8, 58, 0},
	{0, 64, 0},
	{0, 60, 0},
	{0, 50, 60},
	{0, 0, 0},
	{0, 0, 0},
	{0, 0, 0},

	{152, 150, 152},
	{8, 76, 196},
	{48, 50, 236},
	{92, 30, 228},
	{136, 20, 176},
	{160, 20, 100},
	{152, 34, 32},
	{120, 60, 0},
	{84, 90, 0},
	{40, 114, 0},
	{8, 124, 0},
	{0, 118, 40},
	{0, 102, 120},
	{0, 0, 0},
	{0, 0, 0},
	{0, 0, 0},

	{236, 238, 236},
	{76, 154, 236},
	{120, 124, 236},
	{176, 98, 236},
	{228, 84, 236},
	{236, 88, 180},
	{236, 106, 100},
	{212, 136, 32},
	{160, 170, 0},
	{116, 196, 0},
	{76, 208, 32},
	{56, 204, 108},
	{56, 180, 204},
	{60, 60, 60},
	{0, 0, 0},
	{0, 0, 0},

	{236, 238, 236},
	{168, 204, 236},
	{188, 188, 236},
	{212, 178, 236},
	{236, 174, 236},
	{236, 174, 212},
	{236, 180, 176},
	{228, 196, 144},
	{204, 210, 120},
	{180, 222, 120},
	{168, 226, 144},
	{152, 226, 180},
	{160, 214, 228},
	{160, 162, 160},
	{0, 0, 0},
	{0, 0, 0},
};

static void increment_scroll_x(struct ppu *ppu) {
  if (ppu->mask & 0b00011000) {
    if ((ppu->address & 0b0000000000011111) == 31) {
      ppu->address ^= 0b0000010000000000;
      ppu->address &= 0b1111111111100000;
    } else {
      ppu->address++;
    }
  }
}

static void increment_scroll_y(struct ppu *ppu) {
  if (ppu->mask & 0b00011000) {
    if (((ppu->address & 0b0111000000000000) >> 12) < 7) {
      ppu->address += 0b0001000000000000;
    } else {
      ppu->address &= 0b1000111111111111;

      uint8_t coarse_y = (ppu->address & 0b0000001111100000) >> 5;
      if (coarse_y == 29) {
        ppu->address ^= 0b0000100000000000;
        ppu->address &= 0b1111110000011111;
      } else if (coarse_y == 31) {
        ppu->address &= 0b1111110000011111;
      } else {
        ppu->address += 0b0000000000100000;
      }
    }
  }
}

static void transfer_address_x(struct ppu *ppu) {
  if (ppu->mask & 0b00011000) {
    ppu->address &= 0b1111101111100000;
    ppu->address |= (ppu->temp_address & 0b0000010000011111);
  }
}

static void transfer_address_y(struct ppu *ppu) {
  if (ppu->mask & 0b00011000) {
    ppu->address &= 0b1000010000011111;
    ppu->address |= (ppu->temp_address & 0b0111101111100000);
  }
}

static void load_background_shifters(struct ppu *ppu) {
  ppu->pattern_low_shifter &= 0xFF00;
  ppu->pattern_low_shifter |= ppu->next_tile_low_plane;

  ppu->pattern_high_shifter &= 0xFF00;
  ppu->pattern_high_shifter |= ppu->next_tile_high_plane;

  ppu->attribute_low_shifter &= 0xFF00;
  ppu->attribute_low_shifter |= (ppu->next_tile_attribute & 0b01) ? 0xFF : 0x00;

  ppu->attribute_high_shifter &= 0xFF00;
  ppu->attribute_high_shifter |= (ppu->next_tile_attribute & 0b10) ? 0xFF : 0x00;
}

static void update_background_shifters(struct ppu *ppu) {
  if (ppu->mask & 0b00001000) {
    ppu->pattern_low_shifter <<= 1;
    ppu->pattern_high_shifter <<= 1;
    ppu->attribute_low_shifter <<= 1;
    ppu->attribute_high_shifter <<= 1;
  }
}

void step_ppu(struct ppu *ppu, struct cpu *cpu, SDL_Renderer* rend) {
  if (ppu->scanline >= -1 && ppu->scanline < 240) {

    if (ppu->scanline == 0 && ppu->cycle == 0) ppu->cycle = 1;

    if (ppu->scanline == -1 && ppu->cycle == 1) ppu->status &= 0b01111111;

    if ((2 <= ppu->cycle && ppu->cycle < 258) || (321 <= ppu->cycle && ppu->cycle < 338)) {
      update_background_shifters(ppu);

      switch ((ppu->cycle - 1) % 8) {
      case 0:
        load_background_shifters(ppu);
        ppu->next_tile_id = ppu_read(ppu, 0x2000 | (ppu->address & 0xFFF));
        break;

      case 2:
        ppu->next_tile_attribute = ppu_read(
          ppu,
          0x23C0 | (ppu->address & 0b0000110000000000)
          | (((ppu->address >> 7) << 3) & 0b111000)
          | ((ppu->address & 0b0000000000011111) >> 2)
        );

        if (ppu->address & 0b0000000001000000) ppu->next_tile_attribute >>= 4;
        if (ppu->address & 0b0000000000000010) ppu->next_tile_attribute >>= 2;
        ppu->next_tile_attribute &= 0b00000011;
        break;
      
      case 4:
        ppu->next_tile_low_plane = ppu_read(
          ppu, 
          ((ppu->control & 0b00010000) << 8)
          + ((uint16_t)ppu->next_tile_id << 4)
          + ((ppu->address & 0b0111000000000000) >> 12)
        );
        break;

      case 6:
        ppu->next_tile_high_plane = ppu_read(
          ppu, 
          ((ppu->control & 0b00010000) << 8)
          + ((uint16_t)ppu->next_tile_id << 4)
          + ((ppu->address & 0b0111000000000000) >> 12)
          + 8
        );
        break;

      case 7:
        increment_scroll_x(ppu);
        break;
      }
    }

    if (ppu->cycle == 256) increment_scroll_y(ppu);
    else if (ppu->cycle == 257) {
      load_background_shifters(ppu);
      transfer_address_x(ppu);
    }

    if (ppu->cycle == 338 || ppu->cycle == 340) ppu->next_tile_id = ppu_read(ppu, 0x2000 | (ppu->address & 0xFFF));
    if (ppu->scanline == -1 && 280 <= ppu->cycle && ppu->cycle < 305) transfer_address_y(ppu);

  } else if (ppu->scanline == 241 && ppu->cycle == 1) {
    ppu->status |= 0b10000000;
    if (ppu->control & 0b10000000) cpu->trigger_non_maskable_interrupt = true;
  }

  uint8_t background_pixel = 0;
  uint8_t background_palette = 0;
  if (ppu->mask & 0b00001000) {
    uint16_t pixel_bit = 0b1000000000000000 >> ppu->fine_x;

    uint8_t pixel_pattern_low = (ppu->pattern_low_shifter & pixel_bit) > 0;
    uint8_t pixel_pattern_high = (ppu->pattern_high_shifter & pixel_bit) > 0;
    background_pixel = (pixel_pattern_high << 1) | pixel_pattern_low;

    uint8_t pixel_palette_low = (ppu->attribute_low_shifter & pixel_bit) > 0;
    uint8_t pixel_palette_high = (ppu->attribute_high_shifter & pixel_bit) > 0;
    background_palette = (pixel_palette_high << 1) | pixel_palette_low;
  }

  uint8_t *colour = NES_palette[ppu_read(ppu, 0x3F00 + (background_palette << 2) + background_pixel) & 0x3F];
  SDL_SetRenderDrawColor(rend, colour[0], colour[1], colour[2], 255);
  SDL_Rect rect = {(ppu->cycle - 1) * 3, ppu->scanline * 3, 3, 3};
  SDL_RenderFillRect(rend, &rect);

	if (++ppu->cycle >= 341) {
		ppu->cycle = 0;
		if (++ppu->scanline >= 261) ppu->scanline = -1;
	}
}

uint8_t *ppu_target(struct ppu *ppu, uint16_t address) {
  address &= 0x3FFF;

  if      (0x0000 <= address && address <= 0x0FFF) return &((uint8_t *)ppu->pattern_table_left)[address & 0xFFF];
  else if (0x1000 <= address && address <= 0x1FFF) return &((uint8_t *)ppu->pattern_table_right)[address & 0xFFF];
  else if (0x2000 <= address && address <= 0x3EFF) {
    address &= 0xFFF;
    switch (ppu->mirror_mode) {
    case HORIZONTAL:
      if (0x000 <= address && address <= 0x7FF) return &ppu->nametable_0[address & 0x3FF];
      else return &ppu->nametable_1[address & 0x3FF];
      break;
    case VERTICAL:
      if      (0x000 <= address && address <= 0x3FF) return &ppu->nametable_0[address & 0x3FF];
      else if (0x400 <= address && address <= 0x7FF) return &ppu->nametable_1[address & 0x3FF];
      else if (0x800 <= address && address <= 0xBFF) return &ppu->nametable_0[address & 0x3FF];
      else return &ppu->nametable_1[address & 0x3FF];
      break;
    
    default:
      break;
    }
  } else {
    address &= 0x1F;
		if (address == 0x0010) return &ppu->palette_ram[0x0];
		if (address == 0x0014) return &ppu->palette_ram[0x4];
		if (address == 0x0018) return &ppu->palette_ram[0x8];
		if (address == 0x001C) return &ppu->palette_ram[0xC];
    return &ppu->palette_ram[address];
  }
}

uint8_t ppu_read(struct ppu *ppu, uint16_t address) {
  return *ppu_target(ppu, address);
}

void ppu_write(struct ppu *ppu, uint16_t address, uint8_t data) {
  *ppu_target(ppu, address) = data;
}
