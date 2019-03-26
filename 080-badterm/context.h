#pragma once
// g++ -Wall -Wextra badterm.cc -lSDL2
#ifndef _GNU_SOURCE
#  define _GNU_SOURCE
#endif
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <SDL2/SDL.h>
#include <signal.h>
#include <vector>
#include <cstdint>
#include <memory>

#include "textbuffer.h"
#include "console.h"
#include "window.h"

struct Context {
  std::vector<std::unique_ptr<Console>> consoles;
  std::unique_ptr<Window> wnd;
};


