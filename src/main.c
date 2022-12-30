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

#define FRAME_TIME (1000000000 / 5370000)

struct cpu *cpu;
struct ppu *ppu;
struct apu *apu;
uint8_t *ram;
uint8_t *rom;
int cycle = 0;

const SDL_Color white = {255, 255, 255, 255};

static uint8_t NES_read(uint16_t address) {
  if      (address < 0x2000) return ram[address & 0b0000011111111111];
  else if (address < 0x4000) return ppu_read(ppu, address & 0b0000000000000111);
  else if (address < 0x4014) return apu_read(apu, address & 0b0000000000001111);
  else                       return rom[address - 0x8000];
}

static void NES_write(uint8_t data, uint16_t address) {
  if      (address < 0x2000) ram[address & 0b0000011111111111] = data;
  else if (address < 0x4000) ppu_write(ppu, address & 0b0000000000000111, data);
  else if (address < 0x4014) apu_write(apu, address & 0b0000000000001111, data);
}

static void cycle_clock(struct cpu *cpu, struct ppu *ppu) {
  if (cycle % 30 == 0) step_cpu(cpu);
  cycle++;
  timer_wait(FRAME_TIME);
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
  load_rom(argv[1], rom, ppu->chr_rom);

  cpu->memory.read = &NES_read;
  cpu->memory.write = &NES_write;
  cpu->program_counter = ((uint16_t)cpu->memory.read(0xFFFD) << 8) | cpu->memory.read(0xFFFC);

  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) printf("error initializing SDL: %s\n", SDL_GetError());
  if (TTF_Init() != 0) printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());

  SDL_Window* win = SDL_CreateWindow("GAME", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1600, 900, 0);
  Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
  SDL_Renderer* rend = SDL_CreateRenderer(win, -1, render_flags);
  TTF_Font* montserrat = TTF_OpenFont("Montserrat-Regular.ttf", 24);

  timer_start();
  bool close = false;
  while (!close) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        close = true;
        break;
      }
    }
    SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
    SDL_RenderClear(rend);
  
    char current_instruction[27];
    sprintf(current_instruction, "INSTRUCTION: %s %s", get_current_instruction_name(cpu), disassemble_operand(cpu));
    render_text(current_instruction, rend, 1250, 850, montserrat);

    SDL_RenderPresent(rend);

    //for (int i = 0; i < 89605; i++) 
    cycle_clock(cpu, ppu);
  }
 
  SDL_DestroyRenderer(rend);
  SDL_DestroyWindow(win);
  TTF_Quit();
  SDL_Quit();

  return 0;
}