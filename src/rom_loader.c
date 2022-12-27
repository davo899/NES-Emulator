#include "rom_loader.h"
#include <stdio.h>

uint8_t *load_rom(char *path) {
  FILE *file = fopen(path, "rb");
  if (file == NULL) exit(-1);
}
