#include "cpu.h"
#include <stdio.h>
#include <stdlib.h>

void panic(char *error) {
  printf("PANIC: %s", error);
  exit(-1);
}