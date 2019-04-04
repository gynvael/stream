// g++ -Wall -Wextra badterm.cc -lSDL2
#ifndef _GNU_SOURCE
#  define _GNU_SOURCE
#endif
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include <errno.h>
#ifdef __unix__
#  include <unistd.h>
#  include <sys/types.h>
#  include <sys/stat.h>
#  include <fcntl.h>
#  include <sys/wait.h>
#  include <signal.h>
#endif
#include <SDL2/SDL.h>
#include <vector>
#include <cstdint>
#include <memory>

#include "textbuffer.h"
#include "console.h"
#include "context.h"

#ifdef _WIN32
#  undef main
#endif

int main() {
  Context ctx;

  ctx.wnd = std::make_unique<Window>(&ctx);
  if (!ctx.wnd->InitSDL() ||
      !ctx.wnd->Create()) {
    return 1;
  }

  // TODO: Move this to a function called "add new console"
  ctx.consoles.emplace_back(
    std::make_unique<Console>(&ctx)
  );

  ctx.wnd->ResizeConsoles();

  if (!ctx.consoles[0]->SpawnChild()) {
    return 2;
  }
  // TODO: ---

  bool end = false;
  while (!end) {
    for (auto& console : ctx.consoles) {
#ifdef __unix__
      int status;
      if (waitpid(console->GetPid(), &status, WNOHANG) != 0) {
        console->ResetPid();
        end = true;
        break;
      }
#endif
    }

    if (!ctx.wnd->HandleEvents()) {
      end = true;
      break;
    }
  }

  ctx.wnd->Destroy();
  ctx.wnd->QuitSDL();

  for (auto& console : ctx.consoles) {
    // This should force the child to exit.
    console->CloseMaster();
  }

  // Keep this as a separate loop.
  for (auto& console : ctx.consoles) {
#ifdef __unix__
    if (console->GetPid() != -1) {
      // Wait for the child.
      waitpid(console->GetPid(), nullptr, 0);
    }
#else
    SDL_Delay(100);  // ms
#endif
  }
}

