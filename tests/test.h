#pragma once

#include <stdint.h>

extern char *current_test;

void fail(char *message);
void unit_test(char *test_name, void (*test)(void));
void test_bytes_equal(uint8_t actual, uint8_t expected);
