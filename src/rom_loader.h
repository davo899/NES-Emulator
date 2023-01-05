#pragma once

#include "cpu.h"
#include "ppu.h"

void load_rom(char *path, struct memory_mapping *mapper, struct ppu *ppu);
