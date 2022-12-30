#include "timer.h"
#include <stdio.h>
#include <windows.h>

#define BILLION 1000000000

uint64_t ticksPerSecond;
uint64_t previous;

void timer_start(void) {
  LARGE_INTEGER tps, prev;
  QueryPerformanceFrequency(&tps);
  QueryPerformanceCounter(&prev);

  ticksPerSecond = tps.QuadPart;
  previous = prev.QuadPart;
}

void timer_wait(uint64_t nanoseconds) {
  LARGE_INTEGER time;

  do {
    QueryPerformanceCounter(&time);
  } while((time.QuadPart - previous) * (BILLION / ticksPerSecond) < nanoseconds);
  
  previous = time.QuadPart;
}
