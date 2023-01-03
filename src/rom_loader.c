#include "rom_loader.h"
#include "error.h"
#include <stdio.h>
#include <stdlib.h>

const uint8_t header_validation[4] = {0x4E, 0x45, 0x53, 0x1A};

void load_rom(char *path, uint8_t *prg_rom, uint8_t *chr_rom, int *prg_banks) {
  FILE *file = fopen(path, "rb");
  if (file == NULL) error("ROM file not found or could not be opened");

  uint8_t header[16];
  fread(header, 1, 16, file);
  for (int i = 0; i < 4; i++) if (header[i] != header_validation[i]) error("Invalid ROM file");
  uint8_t prg_rom_size = header[4];
  *prg_banks = prg_rom_size;
  uint8_t chr_rom_size = header[5];
  uint8_t flags_6 = header[6];

  uint8_t trainer[512];
  if (flags_6 & 0b00000100) fread(trainer, 1, 512, file);
  
  uint8_t *head = prg_rom;
  for (int i = 0; i < prg_rom_size; i++) {
    fread(head, 1, 16384, file);
    head += 16384;
  }

  head = chr_rom;
  for (int i = 0; i < chr_rom_size; i++) {
    fread(head, 1, 8192, file);
    head += 8192;
  }

  fclose(file);
}
