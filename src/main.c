#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "error.h"
#include "timer.h"
#include "rom_loader.h"
#include "operand.h"
#include "instructions.h"
#include "cpu.h"
#include "ppu.h"
#include "apu.h"

#define CLOCK_TIME (1000000000 / 5370000)

struct cpu *cpu;
struct ppu *ppu;
struct apu *apu;
uint8_t *ram;
uint8_t *rom;

uint8_t controller;
uint8_t controller_buffer;

uint8_t dma_high;
uint8_t dma_low;
uint8_t dma_data;
bool performing_dma;
bool syncing_dma;

int prg_banks = 1;

int cycle = 0;

const SDL_Color white = {255, 255, 255, 255};

static uint8_t NES_read(uint16_t address) {
  if      (0x0000 <= address && address <= 0x1FFF) return ram[address & 0b0000011111111111];
  else if (0x2000 <= address && address <= 0x3FFF) {
    uint8_t data;
    switch (address & 0b0000000000000111) {
    case PPU_STATUS:
      data = (ppu->status & 0b11100000) | (ppu->data_buffer & 0b00011111);
      ppu->status &= 0b01111111;
      ppu->address_latch = false;
      break;

    case OAM_DATA:
      data = ppu->oam_bytes[ppu->oam_address];
      break;

    case PPU_DATA:
      data = ppu->data_buffer;
      ppu->data_buffer = ppu_read(ppu, ppu->address);
      if (0x3F00 <= ppu->address) data = ppu->data_buffer;
      ppu->address += (ppu->control & 0b00000100) ? 32 : 1;
      break;

    default:
      data = ppu->data_buffer;
    }
    return data;
  }
  else if (0x4000 <= address && address <= 0x4014) return 0;
  else if (address == 0x4015) return 0;                      // APU SND_CHN
  else if (address == 0x4016) {
    uint8_t data = controller_buffer >> 7;
    controller_buffer <<= 1;
    return data;
  }
  else if (address == 0x4017) return 0;  // Second controller
  else if (0x4018 <= address && address <= 0x401F) return 0;
  else if (0x4020 <= address && address <= 0x7FFF) return 0; // Extra game space
  else {
    if (prg_banks == 1 && 0xC000 <= address) return rom[address & 0b0011111111111111];
    else                                     return rom[address & 0b0111111111111111];
  }
}

static void NES_write(uint8_t data, uint16_t address) {
  if      (0x0000 <= address && address <= 0x1FFF) ram[address & 0b0000011111111111] = data;
  else if (0x2000 <= address && address <= 0x3FFF) {
    switch (address & 0b0000000000000111) {
    case PPU_CONTROL:
      ppu->control = data;
      ppu->temp_address &= 0b1111001111111111;
      ppu->temp_address |= ((uint16_t)(data & 0b00000011)) << 10;
      break;

    case PPU_MASK:
      ppu->mask = data;
      break;

    case OAM_ADDRESS:
      ppu->oam_address = data;
      break;

    case OAM_DATA:
      ppu->oam_bytes[ppu->oam_address] = data;
      break;

    case PPU_SCROLL:
      if (ppu->address_latch) {
        ppu->address_latch = false;
        ppu->temp_address &= 0b1000111111111111;
        ppu->temp_address |= ((uint16_t)data & 0b00000111) << 12;
        ppu->temp_address &= 0b1111110000011111;
        ppu->temp_address |= ((uint16_t)data >> 3) << 5;
      } else {
        ppu->address_latch = true;
        ppu->fine_x = data & 0b00000111;
        ppu->temp_address &= 0b1111111111100000;
        ppu->temp_address |= data >> 3;
      }
      break;

    case PPU_ADDRESS:
      if (ppu->address_latch) {
        ppu->address_latch = false;
        ppu->temp_address = (ppu->temp_address & 0xFF00) | data;
        ppu->address = ppu->temp_address;
      } else {
        ppu->address_latch = true;
        ppu->temp_address = ((uint16_t)(data & 0x3F) << 8) | (ppu->temp_address & 0x00FF);
      }
      break;

    case PPU_DATA:
      ppu_write(ppu, ppu->address, data);
      ppu->address += (ppu->control & 0b00000100) ? 32 : 1;
      break;
    }
  }
  else if (address < 0x4014) apu_write(apu, address & 0b0000000000001111, data);
  else if (address == 0x4014) {
    dma_high = data;
    dma_low = 0;
    performing_dma = true;
    syncing_dma = true;
  }
  else if (address == 0x4016) controller_buffer = controller;
}

static void cycle_clock(struct cpu *cpu, struct ppu *ppu, SDL_Renderer *rend) {
  step_ppu(ppu, cpu, rend);

  if (cycle % 3 == 0) {
    if (performing_dma) {
      if (syncing_dma) {
        if (cycle % 2 == 1) syncing_dma = false;

      } else {
        if (cycle % 2 == 0) dma_data = cpu->memory.read(((uint16_t)dma_high << 8) | dma_low);
        else {
          ppu->oam_bytes[dma_low++] = dma_data;
          if (dma_low == 0) performing_dma = false;
        }
      }

    } else {
      step_cpu(cpu);
    }
  }

  cycle++;
  timer_wait(CLOCK_TIME);

  /*
  if (cycle % 3 == 0 && cpu->instruction_cycles_remaining == 1) {
    char current_instruction[27];
    sprintf(current_instruction, "INSTRUCTION: %s %s", get_current_instruction_name(cpu), disassemble_operand(cpu));
    printf("%s\n", current_instruction);

    char current_program_counter[22];
    sprintf(current_program_counter, "PROGRAM COUNTER: %04x", cpu->program_counter);
    printf("%s\n", current_program_counter);

    
    printf("%02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x\n",
      ram[0x100 + cpu->stack_pointer],
      ram[0x100 + cpu->stack_pointer + 1],
      ram[0x100 + cpu->stack_pointer + 2],
      ram[0x100 + cpu->stack_pointer + 3],
      ram[0x100 + cpu->stack_pointer + 4],
      ram[0x100 + cpu->stack_pointer + 5],
      ram[0x100 + cpu->stack_pointer + 6],
      ram[0x100 + cpu->stack_pointer + 7]
    );
    fflush(stdout);
  }*/
}

static void render_palettes(SDL_Renderer *rend, int atX, int atY, struct ppu *ppu) {
  SDL_Rect border = {atX - 2, atY - 2, (10 * 4) + 4, (10 * 16) + 4};
  SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);
  SDL_RenderFillRect(rend, &border);
  for (int i = 0; i < 64; i++) {
    uint8_t *colour = NES_palette[ppu_read(ppu, 0x3F00 + i)];
    SDL_Rect rect = {atX + ((i % 4) * 10), atY + ((i / 4) * 10), 10, 10};
    SDL_SetRenderDrawColor(rend, colour[0], colour[1], colour[2], 255);
    SDL_RenderFillRect(rend, &rect);
  }
}

static void render_pattern_table(SDL_Renderer *rend, int atX, int atY, struct tile (*pattern_table)[16], struct ppu *ppu) {
  SDL_Rect border = {atX - 2, atY - 2, (16 * 8) + 4, (16 * 8) + 4};
  SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);
  SDL_RenderFillRect(rend, &border);
  for (int i = 0; i < 16; i++) {
    for (int j = 0; j < 16; j++) {
      for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
          uint8_t pattern_pixel = (((pattern_table[j][i].high_plane[y] & (1 << x)) > 0) << 1)
            | ((pattern_table[j][i].low_plane[y] & (1 << x)) > 0);
          uint8_t *colour = NES_palette[ppu_read(ppu, 0x3F00 + pattern_pixel) & 0x3F];
          SDL_SetRenderDrawColor(rend, colour[0], colour[1], colour[2], 255);
          SDL_RenderDrawPoint(rend, atX + (8 - x) + (i * 8), atY + y + (j * 8));
        }
      }
    }
  }
}

static void render_text(char *text, SDL_Renderer *rend, int x, int y, TTF_Font *font) {
  SDL_Texture *texture = SDL_CreateTextureFromSurface(rend, TTF_RenderText_Solid(font, text, white));
  SDL_Point size;
  SDL_QueryTexture(texture, NULL, NULL, &size.x, &size.y);
  SDL_Rect opcode_quad = {x, y, size.x, size.y};
  SDL_RenderCopy(rend, texture, NULL, &opcode_quad);
  SDL_DestroyTexture(texture);
}

int main(int argc, char *argv[]) {
  if (argc != 2) error("Incorrect number of arguments: nes <rom_path>");
  
  cpu = calloc(1, sizeof(struct cpu)); check_OOM(cpu);
  ppu = calloc(1, sizeof(struct ppu)); check_OOM(ppu);
  apu = calloc(1, sizeof(struct apu)); check_OOM(apu);
  ram = calloc(2048, 1); check_OOM(ram);
  rom = calloc(0x7FFF, 1); check_OOM(rom);
  load_rom(argv[1], rom, (uint8_t *)&ppu->pattern_table_left);

  cpu->memory.read = &NES_read;
  cpu->memory.write = &NES_write;
  cpu->program_counter = ((uint16_t)cpu->memory.read(0xFFFD) << 8) | cpu->memory.read(0xFFFC);
  ppu->mirror_mode = HORIZONTAL;

  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) printf("error initializing SDL: %s\n", SDL_GetError());
  if (TTF_Init() != 0) printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());

  SDL_Window* win = SDL_CreateWindow("GAME", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1600, 900, 0);
  Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
  SDL_Renderer* rend = SDL_CreateRenderer(win, -1, render_flags);
  TTF_Font* font = TTF_OpenFont("Retro Gaming.ttf", 24);
  TTF_Font* small_font = TTF_OpenFont("Retro Gaming.ttf", 12);

  timer_start();
  bool close = false;
  while (!close) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        close = true;
        break;

      } else if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
        case SDLK_z:
          while (cpu->instruction_cycles_remaining != 0) cycle_clock(cpu, ppu, rend);
          cycle_clock(cpu, ppu, rend);
          cycle_clock(cpu, ppu, rend);
          cycle_clock(cpu, ppu, rend);
          break;

        case SDLK_y:
          while (cpu->y != 0) cycle_clock(cpu, ppu, rend);
          break;

        case SDLK_x:
          while (cpu->x != 0) cycle_clock(cpu, ppu, rend);
          break;
        
        default:
          break;
        }
      }
    }

    int numkeys;
    const uint8_t *keys = SDL_GetKeyboardState(&numkeys);
    controller = 0;
    if (keys[SDL_SCANCODE_SPACE])  controller |= 0b10000000;
    if (keys[SDL_SCANCODE_LSHIFT]) controller |= 0b01000000;
    if (keys[SDL_SCANCODE_X])      controller |= 0b00100000;
    if (keys[SDL_SCANCODE_C])      controller |= 0b00010000;
    if (keys[SDL_SCANCODE_W])      controller |= 0b00001000;
    if (keys[SDL_SCANCODE_S])      controller |= 0b00000100;
    if (keys[SDL_SCANCODE_A])      controller |= 0b00000010;
    if (keys[SDL_SCANCODE_D])      controller |= 0b00000001;

    SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
    SDL_RenderClear(rend);

    render_palettes(rend, 1300, 100, ppu);
    render_pattern_table(rend, 1400, 100, ppu->pattern_table_left, ppu);
    render_pattern_table(rend, 1400, 230, ppu->pattern_table_right, ppu);

    char current_byte[5];
    sprintf(current_byte, "0x%02x", cpu->memory.read(cpu->program_counter));
    render_text(current_byte, rend, 1200, 650, font);

    char current_axy[27];
    sprintf(current_axy, "A:0x%02x X:0x%02x Y:0x%02x", cpu->accumulator, cpu->x, cpu->y);
    render_text(current_axy, rend, 1200, 700, font);
    
    char current_program_counter[22];
    sprintf(current_program_counter, "PROGRAM COUNTER: %04x", cpu->program_counter);
    render_text(current_program_counter, rend, 1200, 750, font);

    char current_cycle[9];
    sprintf(current_cycle, "CYCLE: %d", cpu->instruction_cycles_remaining);
    render_text(current_cycle, rend, 1200, 800, font);

    char current_instruction[27];
    sprintf(current_instruction, "INSTRUCTION: %s %s", get_current_instruction_name(cpu), disassemble_operand(cpu));
    render_text(current_instruction, rend, 1200, 850, font);

    SDL_SetRenderDrawColor(rend, 255, 0, 0, 255);
    for (int i = 0; i < 8; i++) {
      if (controller & (1 << i)) {
        SDL_Rect rect = {1200 + (i * 10), 400, 10, 10};
        SDL_RenderFillRect(rend, &rect);
      }
    }

    for (int i = 0; i < 89342; i++) cycle_clock(cpu, ppu, rend);

    /*
    for (int i = 0; i < 0x400; i++) {
      char nametable_byte[3];
      sprintf(nametable_byte, "%02x", ppu_read(ppu, 0x2000 | i));
      render_text(nametable_byte, rend, ((i % 0x20) * 8 * 3), ((i / 0x20) * 8 * 3), small_font);
    }*/

    SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
    SDL_RenderDrawLine(rend, 3 * 256, 0, 3 * 256, 3 * 262);
    SDL_RenderDrawLine(rend, 0, 3 * 240, 3 * 341, 3 * 240);

    SDL_RenderPresent(rend);
  }
 
  SDL_DestroyRenderer(rend);
  SDL_DestroyWindow(win);
  TTF_Quit();
  SDL_Quit();

  return 0;
}