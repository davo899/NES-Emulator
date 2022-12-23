#include <stdint.h>

struct memory_mapping {
  uint8_t memory[0x10000];
  uint8_t (*read)(uint16_t address);
  void (*write)(uint8_t value, uint16_t address);
};
