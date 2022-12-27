#include "ppu.h"
#include <SDL2/SDL.h>

/*
#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 900

  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    printf("error initializing SDL: %s\n", SDL_GetError());
  }
  SDL_Window* win = SDL_CreateWindow("GAME",
                                     SDL_WINDOWPOS_CENTERED,
                                     SDL_WINDOWPOS_CENTERED,
                                     SCREEN_WIDTH, SCREEN_HEIGHT, 0);

  while (1) ;*/

uint8_t ppu_read(struct ppu *ppu, uint16_t address) {

}

void ppu_write(struct ppu *ppu, uint16_t address, uint8_t data) {

}
