#pragma once

#include <stdint.h>

struct apu {
  uint8_t pulse1_envelope;
  uint8_t pulse1_sweep;
  uint8_t pulse1_timer;
  uint8_t pulse1_length_counter;

  uint8_t pulse2_envelope;
  uint8_t pulse2_sweep;
  uint8_t pulse2_timer;
  uint8_t pulse2_length_counter;

  uint8_t triangle_linear_counter;
  uint8_t triangle_timer;
  uint8_t triangle_length_counter;

  uint8_t noise_envelope;
  uint8_t noise_period;
  uint8_t noise_length_counter;

  uint8_t dmc_sample_address;
  uint8_t dmc_sample_length;
  uint8_t dmc_sample_buffer;
  uint8_t dmc_timer;

  uint8_t control;
  uint8_t status;
  uint8_t frame_counter;
};

uint8_t apu_read(struct apu *apu, uint16_t address);
void apu_write(struct apu *apu, uint16_t address, uint8_t data);
