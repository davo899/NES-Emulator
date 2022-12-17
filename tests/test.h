#pragma once

extern char *current_test;

void fail(char *message);
void unit_test(char *test_name, void (*test)(void));
