#pragma once
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include <errno.h>
#include <SDL2/SDL.h>
#include <vector>
#include <cstdint>
#include <memory>

#include "textbuffer.h"
#include "console.h"

struct Context;

class Window {
  Context *ctx_ = nullptr;
  SDL_Window *window_ = nullptr;
  SDL_Surface *surface_ = nullptr;
 public:
  Window(Context *ctx) : ctx_(ctx) {}
  ~Window();

  bool InitSDL();
  void QuitSDL();

  bool Create();
  void Destroy();

  bool HandleEvents();  // If false is returned, exit.
  void ResizeConsoles();

  void RedrawWindowIfConsoleActive(Console *console);

};
