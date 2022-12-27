#include "error.h"
#include <stdio.h>
#include <stdlib.h>

void error(char *error) {
  printf("ERROR: %s", error);
  exit(-1);
}

void check_OOM(void *pointer) {
  if (pointer == NULL) error("Out of memory");
}
