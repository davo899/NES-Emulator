#include "test.h"
#include <stdio.h>
#include <stdlib.h>

char *current_test = "(nameless)";

void fail(char *message) {
  printf("Test %s failed: %s\n", current_test, message);
  exit(-1);
}

void unit_test(char *test_name, void (*test)(void)) {
  current_test = test_name;
  test();
  printf("%s passed\n", test_name);
}

void test_bytes_equal(uint8_t actual, uint8_t expected) {
  if (actual != expected) {
    char fail_message[49];
    sprintf(fail_message, "Bytes not equal\nExpected: 0x%x\nActual:   0x%x", expected, actual);
    fail(fail_message);
  }
}
