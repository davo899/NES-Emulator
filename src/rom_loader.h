#pragma once

#include <stdint.h>

void load_rom(char *path, uint8_t *prg_rom, uint8_t *chr_rom, int *prg_banks);
