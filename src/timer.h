#pragma once

#include <stdbool.h>
#include <stdint.h>

void timer_start(void);
void timer_wait(uint64_t nanoseconds);
