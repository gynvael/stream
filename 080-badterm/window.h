#pragma once
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include <errno.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <vector>
#include <cstdint>
#include <memory>

#include "textbuffer.h"
#include "console.h"

struct Context;

class TermWindow {
  const int kDefaultWidth = 640 /*px*/;
  const int kDefaultHeight = 480 /*px*/;

  Context *ctx_ = nullptr;
  Window window_{};
  Display *display_ = nullptr;
  int screen_ = 0;
  Visual *visual_ = nullptr;
  XImage *surface_ = nullptr;
  uint32_t *frame_ = nullptr;

  Atom WM_DELETE_WINDOW;
  Atom WM_SIZE_HINTS;
  Atom WM_NORMAL_HINTS;
  Atom _NET_WM_ALLOWED_ACTIONS;
  Atom _NET_WM_ACTION_CLOSE;
  Atom _NET_WM_ACTION_MINIMIZE;
  Atom _NET_WM_ACTION_MOVE;

 public:
  TermWindow(Context *ctx) : ctx_(ctx) {}
  ~TermWindow();

  bool InitX11();
  void QuitX11();

  bool Create();
  void Destroy();

  bool HandleEvents();  // If false is returned, exit.
  void ResizeConsoles();

  void RedrawWindowIfConsoleActive(Console *console);

};
