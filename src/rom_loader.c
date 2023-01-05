#include "rom_loader.h"
#include "error.h"
#include "mappers/mapper_0.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

const uint8_t header_validation[4] = {0x4E, 0x45, 0x53, 0x1A};

void load_rom(char *path, struct memory_mapping *mapper, struct ppu *ppu) {
  FILE *file = fopen(path, "rb");
  if (file == NULL) error("ROM file not found or could not be opened");

  uint8_t header[16];
  fread(header, 1, 16, file);
  for (int i = 0; i < 4; i++) if (header[i] != header_validation[i]) error("Invalid ROM file");

  uint8_t flags_6 = header[6];
  uint8_t flags_7 = header[7];
  ppu->mirror_mode = (flags_6 & 0b00000001) ? VERTICAL : HORIZONTAL;

  uint8_t mapper_number = (flags_7 & 0xF0) | (flags_6 >> 4);
  switch (mapper_number) {
  case 0:
    init_mapper_0(mapper, ppu, file, header);
    break;
  
  default:
    error("ROM uses an unsupported mapper");
  }

  fclose(file);
}
