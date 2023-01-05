#pragma once

#include "../cpu.h"
#include "../ppu.h"
#include <stdint.h>
#include <stdio.h>

void init_mapper_0(struct memory_mapping *mapper, struct ppu *ppu, FILE *file, uint8_t header[16]);
