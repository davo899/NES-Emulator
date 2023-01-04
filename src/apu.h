#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

struct sequencer {
  uint32_t sequence;
  uint16_t timer;
  uint16_t reload;
  uint8_t output;
};

struct apu {
  struct sequencer pulse1_sequencer;
  double pulse1_frequency;
  double pulse1_duty;
  bool pulse1_constant_volume;
  bool pulse1_envelope_looped;
  uint8_t pulse1_envelope;
  uint8_t pulse1_volume;
  uint8_t pulse1_length_counter;
  union {
    struct {
      uint8_t shift_count : 3;
      uint8_t negate : 1;
      uint8_t period : 3;
      uint8_t enabled : 1;
    };
    uint8_t byte;
  } pulse1_sweep;
  uint8_t pulse1_sweep_timer;
  uint16_t pulse1_sweep_target_period;

  struct sequencer pulse2_sequencer;
  double pulse2_frequency;
  double pulse2_duty;
  bool pulse2_constant_volume;
  bool pulse2_envelope_looped;
  uint8_t pulse2_envelope;
  uint8_t pulse2_volume;
  uint8_t pulse2_length_counter;

  struct sequencer triangle_sequencer;
  double triangle_frequency;
  uint8_t triangle_volume;
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

  uint32_t frame_clock_counter;
  uint32_t clock_counter;

  SDL_AudioDeviceID audio_device_id;
  uint64_t samples_played;
  bool enabled;
};

void init_apu(struct apu *apu);
void step_apu(struct apu *apu, int cycle);
void quit_apu(struct apu *apu);

uint8_t apu_read(struct apu *apu, uint16_t address);
void apu_write(struct apu *apu, uint16_t address, uint8_t data);
