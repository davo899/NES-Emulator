#include "cpu.h"
#include <stdio.h>

void panic(char *error) {
  printf("PANIC: %s", error);
}