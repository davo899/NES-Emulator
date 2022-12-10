#include "cpu.h"

void panic(char *error) {
  printf("PANIC: %s", error);
}