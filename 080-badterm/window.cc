#include "window.h"
#include "context.h"

bool Window::InitSDL() {
  static bool SDL_initialized;  // Not multi-threading safe.

  if (SDL_initialized) {
    return true;
  }

  if (SDL_Init(SDL_INIT_VIDEO |
               SDL_INIT_TIMER |
               SDL_INIT_EVENTS) != 0) {
    fprintf(stderr, "SDL_Init: error\n");
    return false;
  }

  SDL_initialized = true;
  return true;
}

Window::~Window() {
  Destroy();
}

void Window::Destroy() {
  if (window_ != nullptr) {
    SDL_DestroyWindow(window_);
  }
}

void Window::QuitSDL() {
  SDL_Quit();
}

void Window::ResizeConsoles() {
  uint32_t w = (uint32_t)surface_->w;
  uint32_t h = (uint32_t)surface_->h;

  for (auto& console : ctx_->consoles) {
    console->ResizeTextBuffer(w, h);

    // TODO(gynvael):
    console->HandleSurfaceChange(surface_);
  }
}

bool Window::Create() {
  window_ = SDL_CreateWindow(
    "badterm", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
    640, 480,
    SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
  if (window_ == nullptr) {
    fprintf(stderr, "SDL_CreateWindow: error\n");
    return false;
  }

  surface_ = SDL_GetWindowSurface(window_);
  if (surface_ == nullptr) {
    fprintf(stderr, "SDL_GetWindowSurface: error\n");
    return false;
  }

  ResizeConsoles();

  return true;
}

bool Window::HandleEvents() {
  SDL_PumpEvents();
  SDL_Event ev;
  while (SDL_PeepEvents(
    &ev, 1, SDL_GETEVENT, SDL_FIRSTEVENT, SDL_LASTEVENT) > 0) {

    if (ev.type == SDL_QUIT) {
      return false;
    }

    if (ev.type == SDL_WINDOWEVENT) {
      if (ev.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
        surface_ = SDL_GetWindowSurface(window_);
        if (surface_ == nullptr) {
          fprintf(stderr, "SDL_GetWindowSurface: error\n");
          return false;
        }
        ResizeConsoles();
        //SDL_UpdateWindowSurface(window_);
        continue;
      }

      if (ev.window.event == SDL_WINDOWEVENT_CLOSE) {
        SDL_Event quit_ev;
        quit_ev.type = SDL_QUIT;
        SDL_PushEvent(&quit_ev);
        continue;
      }

      if (ev.window.event == SDL_WINDOWEVENT_MOVED ||
          ev.window.event == SDL_WINDOWEVENT_SHOWN ||
          ev.window.event == SDL_WINDOWEVENT_EXPOSED ||
          ev.window.event == SDL_WINDOWEVENT_RESIZED ||
          ev.window.event == SDL_WINDOWEVENT_MAXIMIZED ||
          ev.window.event == SDL_WINDOWEVENT_RESTORED) {
        printf("%p\n", window_);
        SDL_UpdateWindowSurface(window_);
        continue;
      }
    }
  }


  return true;
}

void Window::RedrawWindowIfConsoleActive(Console */*console*/) {
  // TODO: check if this console is actually active
  SDL_UpdateWindowSurface(window_);
}

