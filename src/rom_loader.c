#include "rom_loader.h"
#include "error.h"
#include <stdio.h>

uint8_t *load_rom(char *path) {
  FILE *file = fopen(path, "rb");
  if (file == NULL) error("ROM file not found or could not be opened");
}
