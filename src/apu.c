#include "apu.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define FREQ_CONST (1.85056892106)

uint8_t length_table[32] = {
  10, 254,
  20, 2,
  40, 4,
  80, 6,
  160, 8,
  60, 10,
  14, 12,
  26, 14,

  12, 16,
  24, 18,
  48, 20,
  96, 22,
  192, 24,
  72, 26,
  16, 28,
  32, 30
};

bool sound_enabled = false;
struct apu *player;

static uint8_t step_sequencer(struct sequencer *sequencer, void (*change_sequence)(uint32_t *s)) {
  if (--sequencer->timer == 0xFFFF) {
    sequencer->timer = sequencer->reload + 1;
    change_sequence(&sequencer->sequence);
    sequencer->output = sequencer->sequence & 0x00000001;
  }

  return sequencer->output;
}

static double noise_wave(void) {
  return (double)rand() / RAND_MAX;
}

static double triangle_wave(double time) {
  return 2 * fabs(time - floor(time + 0.5));
}

static void audio_callback(void* userdata, uint8_t* stream, int len) {
  if (sound_enabled) {
    uint64_t* samples_played = (uint64_t*)userdata;
    float* fstream = (float*)(stream);

    static const float volume = 0.2;

    for (int sid = 0; sid < (len / 8); ++sid) {
      double time = (*samples_played + sid) / 44100.0;
      double radian_time = 2.0 * M_PI * time;

      double pulse1 = (player->pulse1_volume / 15.0) * (sin(radian_time * player->pulse1_frequency * FREQ_CONST) > player->pulse1_duty);
      double pulse2 = (player->pulse2_volume / 15.0) * (sin(radian_time * player->pulse2_frequency * FREQ_CONST) > player->pulse2_duty);
      double triangle = (player->triangle_volume / 15.0) * triangle_wave(time * player->triangle_frequency * FREQ_CONST);
      double noise = (player->noise_volume / 15.0) * noise_wave();

      fstream[2 * sid] = volume * (pulse1 + pulse2 + triangle + noise);
    }

    *samples_played += (len / 8);
  }
}

static void rotate(uint32_t *sequence) {
  *sequence = ((*sequence & 0x0001) << 7) | ((*sequence & 0x00FE) >> 1);
}

void init_apu(struct apu *apu) {
  player = apu;
  sound_enabled = true;
  apu->samples_played = 0;

  SDL_AudioSpec audio_spec_want, audio_spec;
  SDL_memset(&audio_spec_want, 0, sizeof(audio_spec_want));
  audio_spec_want.freq     = 44100;
  audio_spec_want.format   = AUDIO_F32;
  audio_spec_want.channels = 1;
  audio_spec_want.samples  = 512;
  audio_spec_want.callback = audio_callback;
  audio_spec_want.userdata = (void*)&apu->samples_played;

  apu->audio_device_id = SDL_OpenAudioDevice(
    NULL, 0,
    &audio_spec_want, &audio_spec,
    SDL_AUDIO_ALLOW_FORMAT_CHANGE
  );
  if (!apu->audio_device_id) {
    sound_enabled = false;
    return;
  }
  SDL_PauseAudioDevice(apu->audio_device_id, 0);
}

void step_apu(struct apu *apu, int cycle) {
  bool quarter_frame_clock = false;
  bool half_frame_clock = false;

  if (cycle % 6 == 0) {
    apu->frame_clock_counter++;

    if (apu->frame_clock_counter == 3729)
      quarter_frame_clock = true;
    if (apu->frame_clock_counter == 7457) {
      quarter_frame_clock = true;
      half_frame_clock = true;
    }
    if (apu->frame_clock_counter == 11186)
      quarter_frame_clock = true;
    if (apu->frame_clock_counter == 14916) {
      quarter_frame_clock = true;
      half_frame_clock = true;
      apu->frame_clock_counter = 0;
    }

    if (quarter_frame_clock) {
      // Adjust volume envelope
      if (apu->pulse1_envelope > 0) {
        if (--apu->pulse1_envelope == 0 && apu->pulse1_envelope_looped) {
          apu->pulse1_envelope = 15;
        }
      }
      if (!apu->pulse1_constant_volume) apu->pulse1_volume = apu->pulse1_envelope;

      if (apu->pulse2_envelope > 0) {
        if (--apu->pulse2_envelope == 0 && apu->pulse2_envelope_looped) {
          apu->pulse2_envelope = 15;
        }
      }
      if (!apu->pulse2_constant_volume) apu->pulse2_volume = apu->pulse2_envelope;

      if (apu->noise_envelope > 0) {
        if (--apu->noise_envelope == 0 && apu->noise_envelope_looped) {
          apu->noise_envelope = 15;
        }
      }
      if (!apu->noise_constant_volume) apu->noise_volume = apu->noise_envelope;

      if (apu->triangle_linear_counter > 0) {
        if (--apu->triangle_linear_counter == 0) apu->triangle_volume = 0;
      }
    }

    if (half_frame_clock) {
      // Adjust note length and frequency sweep
      if (apu->pulse1_sweep.enabled) {
        if (apu->pulse1_sweep_timer > 0) apu->pulse1_sweep_timer--;

        if (apu->pulse1_sweep_timer == 0) {
          uint16_t change = apu->pulse1_sequencer.timer >> apu->pulse1_sweep.shift_count;
          if (apu->pulse1_sweep.negate) change = -((int16_t)change) - 1;
          apu->pulse1_sweep_target_period += change;
          if (apu->pulse1_sweep_target_period > 0x7FF) apu->pulse1_volume = 0;
          else apu->pulse1_frequency = (1789773) / (16 * (apu->pulse1_sweep_target_period + 1));
        }
      }

      if (apu->pulse2_sweep.enabled) {
        if (apu->pulse2_sweep_timer > 0) apu->pulse2_sweep_timer--;

        if (apu->pulse2_sweep_timer == 0) {
          uint16_t change = apu->pulse2_sequencer.timer >> apu->pulse2_sweep.shift_count;
          if (apu->pulse2_sweep.negate) change = -((int16_t)change);
          apu->pulse2_sweep_target_period += change;
          if (apu->pulse2_sweep_target_period > 0x7FF) apu->pulse2_volume = 0;
          else apu->pulse2_frequency = (1789773) / (16 * (apu->pulse2_sweep_target_period + 1));
        }
      }

      if (!apu->pulse1_envelope_looped) {
        if (apu->pulse1_length_counter > 0) {
          if (--apu->pulse1_length_counter == 0) apu->pulse1_volume = 0;
        }
      }

      if (!apu->pulse2_envelope_looped) {
        if (apu->pulse2_length_counter > 0) {
          if (--apu->pulse2_length_counter == 0) apu->pulse2_volume = 0;
        }
      }

      if (apu->triangle_length_counter > 0) {
        if (--apu->triangle_length_counter == 0) apu->triangle_volume = 0;
      }

      if (!apu->noise_envelope_looped) {
        if (apu->noise_length_counter > 0) {
          if (--apu->noise_length_counter == 0) apu->noise_volume = 0;
        }
      }
    }

    step_sequencer(&apu->pulse1_sequencer, &rotate);
    step_sequencer(&apu->pulse2_sequencer, &rotate);
  }

  apu->clock_counter++;
}

uint8_t apu_read(struct apu *apu, uint16_t address) {
  if (address == 0x4015) ;
}

void apu_write(struct apu *apu, uint16_t address, uint8_t data) {
  switch (address) {
  case 0x4000:
    apu->pulse1_volume = data & 0b00001111;
    apu->pulse1_envelope = 15;
    apu->pulse1_constant_volume = (data & 0b00010000) > 0;

    switch ((data & 0b11000000) >> 6) {
    case 0: apu->pulse1_duty = 0.92387953251; break;
    case 1: apu->pulse1_duty = 0.70710678118; break;
    case 2: apu->pulse1_duty = 0; break;
    case 3: apu->pulse1_duty = -0.70710678118; break;
    }
    break;

  case 0x4001:
    apu->pulse1_sweep.enabled = (data & 0b10000000) > 0;
    apu->pulse1_sweep.period = (data & 0b01110000) >> 4;
    apu->pulse1_sweep.negate = (data & 0b00001000) > 0;
    apu->pulse1_sweep.shift_count = data & 0b00000111;
    apu->pulse1_sweep_timer = apu->pulse1_sweep.period;
    break;

  case 0x4002:
    apu->pulse1_sequencer.reload = (apu->pulse1_sequencer.reload & 0xFF00) | data;
    break;

  case 0x4003:
    apu->pulse1_sequencer.reload = ((uint16_t)(data & 0b00000111) << 8) | (apu->pulse1_sequencer.reload & 0x00FF);
    apu->pulse1_sequencer.timer = apu->pulse1_sequencer.reload;
    apu->pulse1_sweep_target_period = apu->pulse1_sequencer.timer;
    apu->pulse1_length_counter = length_table[(data & 0b11111000) >> 3];
    apu->pulse1_volume = 15;

    apu->pulse1_frequency = (1789773) / (16.0 * (apu->pulse1_sequencer.timer + 1));
    break;

  case 0x4004:
    apu->pulse2_volume = data & 0b00001111;
    apu->pulse2_envelope = 15;
    apu->pulse2_constant_volume = (data & 0b00010000) > 0;

    switch ((data & 0b11000000) >> 6) {
    case 0: apu->pulse2_duty = 0.92387953251; break;
    case 1: apu->pulse2_duty = 0.70710678118; break;
    case 2: apu->pulse2_duty = 0; break;
    case 3: apu->pulse2_duty = -0.70710678118; break;
    }
    break;

  case 0x4005:
    apu->pulse2_sweep.enabled = (data & 0b10000000) > 0;
    apu->pulse2_sweep.period = ((data & 0b01110000) >> 4) + 1;
    apu->pulse2_sweep.negate = (data & 0b00001000) > 0;
    apu->pulse2_sweep.shift_count = data & 0b00000111;
    apu->pulse2_sweep_timer = apu->pulse2_sweep.period;
    break;

  case 0x4006:
    apu->pulse2_sequencer.reload = (apu->pulse2_sequencer.reload & 0xFF00) | data;
    break;

  case 0x4007:
    apu->pulse2_sequencer.reload = ((uint16_t)(data & 0b00000111) << 8) | (apu->pulse2_sequencer.reload & 0x00FF);
    apu->pulse2_sequencer.timer = apu->pulse2_sequencer.reload;
    apu->pulse2_sweep_target_period = apu->pulse2_sequencer.timer;
    apu->pulse2_length_counter = length_table[(data & 0b11111000) >> 3];
    apu->pulse2_volume = 15;

    apu->pulse2_frequency = (1789773) / (16.0 * (apu->pulse2_sequencer.timer + 1));
    break;

  case 0x4008:
    apu->triangle_linear_counter = data & 0b01111111;
    break;

  case 0x400A:
    apu->triangle_sequencer.reload = (apu->triangle_sequencer.reload & 0xFF00) | data;
    break;

  case 0x400B:
    apu->triangle_sequencer.reload = ((uint16_t)(data & 0b00000111) << 8) | (apu->triangle_sequencer.reload & 0x00FF);
    apu->triangle_sequencer.timer = apu->triangle_sequencer.reload;
    apu->triangle_length_counter = length_table[(data & 0b11111000) >> 3];
    apu->triangle_volume = 15;

    apu->triangle_frequency = (1789773) / (32.0 * (apu->triangle_sequencer.timer + 1));
    break;

  case 0x400C:
    apu->noise_volume = data & 0b00001111;
    apu->noise_envelope = 15;
    apu->noise_constant_volume = (data & 0b00010000) > 0;
    break;

  case 0x400E:
    apu->noise_period = data & 0x0F;
    break;

  case 0x400F:
    apu->noise_length_counter = data >> 3;
    break;

  case 0x4015:
    if (!(data & 0b00000001)) {
      apu->pulse1_volume = 0;
      apu->pulse1_length_counter = 0;
    }
    if (!(data & 0b00000010)) {
      apu->pulse2_volume = 0;
      apu->pulse2_length_counter = 0;
    }
    if (!(data & 0b00000100)) {
      apu->triangle_volume = 0;
      apu->triangle_length_counter = 0;
    }
    fflush(stdout);
    break;

  case 0x4017:
    apu->frame_clock_counter = (apu->clock_counter % 2 == 0) ? 14914 : 14913;
    break;
  }
}

void quit_apu(struct apu *apu) {
  SDL_CloseAudioDevice(apu->audio_device_id);
}
