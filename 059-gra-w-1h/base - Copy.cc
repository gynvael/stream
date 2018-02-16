#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <SDL2/SDL.h>

#undef main
SDL_Surface *framebuffer;

double gNow;   // Timer.
double gDiff;  // Difference from last frame.

bool key[4096];

void HandleRendering() {
  int w = framebuffer->w;
  int h = framebuffer->h;
  (void)h;
  uint8_t *px = (uint8_t*)framebuffer->pixels;

  if (key[SDL_SCANCODE_DOWN]) {
    px[(320 + 240 * w) * 4 + 1] = 0xff;
  } else {
    px[(320 + 240 * w) * 4 + 0] = 0xff;
  }
}

int main(int, char **) {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS) != 0) {
    puts("SDL failed");
    return 3;
  }

  // Initialize timer.
  gNow = (double)SDL_GetTicks() / 1000.0;
  gDiff = 0.001;

  SDL_Window *win = SDL_CreateWindow(
    "Base Code", SDL_WINDOWPOS_CENTERED, 1200 + 200,
    1280, 720, SDL_WINDOW_SHOWN);
  if (win == nullptr) {
    puts("SDL failed");
    SDL_Quit();
    return 4;
  }

  SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
  if (ren == nullptr){
    puts("SDL failed");
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 1;
  }

  framebuffer = SDL_GetWindowSurface(win);

  SDL_Event e;
  bool end = false;
  while (!end) {
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) {
        end = true;
        break;
      }

      if (e.type == SDL_KEYDOWN) {
        //printf("%i\n", e.key.keysym.scancode); fflush(stdout);
        key[e.key.keysym.scancode] = true;
      }

      if (e.type == SDL_KEYUP) {
        //printf("%i\n", e.key.keysym.scancode); fflush(stdout);
        key[e.key.keysym.scancode] = false;
      }
    }

    // Update timer.
    double now = (double)SDL_GetTicks() / 1000.0;
    gDiff = now - gNow;
    gNow = now;

    HandleRendering();
    SDL_UpdateWindowSurface(win);
  }

  framebuffer = nullptr;
  SDL_DestroyRenderer(ren);
  SDL_DestroyWindow(win);
  SDL_Quit();
  return 0;
}
