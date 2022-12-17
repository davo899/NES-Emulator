#include "test.h"

char *current_test = "(nameless)";

void fail(char *message) {
  printf("Test %s failed: %s\n", current_test, message);
  exit(-1);
}

void unit_test(char *test_name, void (*test)(void)) {
  current_test = test_name;
  test();
  printf("Passed Test: %s\n", test_name);
}
