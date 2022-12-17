#include "test_6502.h"
#include "../test.h"
#include "test_instructions.h"
#include <stdio.h>

void test_6502() {
  test_instructions();

  printf("All 6502 tests passed.\n");
}
