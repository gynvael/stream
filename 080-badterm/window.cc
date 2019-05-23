#include "window.h"
#include "context.h"

bool TermWindow::InitX11() {
  static bool X11_initialized;  // Not multi-threading safe.

  if (X11_initialized) {
    return true;
  }

  XInitThreads();

  display_ = XOpenDisplay(NULL);
  if (display_ == nullptr) {
    fprintf(stderr, "XOpenDisplay: error\n");
    return false;
  }

  WM_DELETE_WINDOW = XInternAtom(display_, "WM_DELETE_WINDOW", false);
  WM_SIZE_HINTS = XInternAtom(display_, "WM_SIZE_HINTS", false);
  WM_NORMAL_HINTS = XInternAtom(display_, "WM_NORMAL_HINTS", false);
  _NET_WM_ALLOWED_ACTIONS = XInternAtom(
      display_, "_NET_WM_ALLOWED_ACTIONS", False);
  _NET_WM_ACTION_CLOSE = XInternAtom(display_, "_NET_WM_ACTION_CLOSE", False);
  _NET_WM_ACTION_MINIMIZE = XInternAtom(
      display_, "_NET_WM_ACTION_MINIMIZE", False);
  _NET_WM_ACTION_MOVE = XInternAtom(display_, "_NET_WM_ACTION_MOVE", False);

  screen_ = DefaultScreen(display_);
  visual_ = DefaultVisual(display_, screen_);

  X11_initialized = true;
  return true;
}

TermWindow::~TermWindow() {
  Destroy();
}

void TermWindow::Destroy() {
  //TODO: if (window_ != nullptr) {
  XDestroyWindow(display_, window_);
  //}
}

void TermWindow::QuitX11() {
  XCloseDisplay(display_);
}

void TermWindow::ResizeConsoles() {
  uint32_t w = (uint32_t)surface_->width;
  uint32_t h = (uint32_t)surface_->height;

  for (auto& console : ctx_->consoles) {
    console->ResizeTextBuffer(w, h);

    // TODO(gynvael):
    console->HandleSurfaceChange(surface_);
  }
}

bool TermWindow::Create() {
  // TODO: SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
  window_ = XCreateSimpleWindow(
    display_,
    RootWindow(display_, screen_),
    /*x=*/10, /*y=*/10, kDefaultWidth, kDefaultHeight,
    /*border_width=*/1,
    /*border=*/BlackPixel(display_, screen_),
    /*background=*/BlackPixel(display_, screen_));

  /*
  TODO: window_ isn't a pointer, figure out how to get info about an erro
  if (window_ == nullptr) {
    fprintf(stderr, "XCreateSimpleWindow: error\n");
    return false;
  }*/

  XStoreName(display_, window_, "badterm");
  XSetWMProtocols(display_, window_, &WM_DELETE_WINDOW, 1);
  XSelectInput(display_, window_, KeyPressMask | ExposureMask);

  XSizeHints xsh;
  xsh.flags = PMinSize | PMaxSize;
  xsh.min_width = xsh.max_width = kDefaultWidth;
  xsh.min_height = xsh.max_height = kDefaultHeight;
  XSetWMSizeHints(display_, window_, &xsh, WM_SIZE_HINTS);
  XSetWMNormalHints(display_, window_, &xsh);

  const Atom allowed[] = {
    _NET_WM_ACTION_CLOSE,
    _NET_WM_ACTION_MINIMIZE,
    _NET_WM_ACTION_MOVE
  };

  XChangeProperty(display_, window_, _NET_WM_ALLOWED_ACTIONS, XA_ATOM, 32,
                  PropertyNewValue, (unsigned char*)&allowed, 3);

  XMapWindow(display_, window_);
  XFlush(display_);

  frame_ = new uint32_t[kDefaultWidth * kDefaultHeight];

  //
  surface_ = XCreateImage(
      display_, visual_, /*depth=*/24,
      ZPixmap, /*offset=*/0, (char*)frame_, kDefaultWidth, kDefaultHeight,
      /*bitmap_pad=*/32, /*bytes_per_line*/0
  );
  if (surface_ == nullptr) {
    fprintf(stderr, "XCreateImage: error\n");
    return false;
  }

  ResizeConsoles();

  return true;
}

bool TermWindow::HandleEvents() {

  while (XPending(display_)) {
    XEvent ev;
    XNextEvent(display_, &ev);

    if (ev.type == ClientMessage &&
        (Atom)ev.xclient.data.l[0] == WM_DELETE_WINDOW) {
      return false;
    }

    if (ev.type == Expose) {
      RedrawWindowIfConsoleActive(nullptr);
      continue;
    }

    /*if (ev.type == SDL_WINDOWEVENT) {
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
    }*/
  }


  return true;
}

void TermWindow::RedrawWindowIfConsoleActive(Console */*console*/) {
  GC ctx;
  ctx = DefaultGC(display_, screen_);
  XPutImage(
      display_, window_, ctx, surface_,
      /*src_x=*/0, /*src_y=*/0, /*dst_x=*/0, /*dst_y=*/0,
      surface_->width, surface_->height);
  XFlush(display_);
}

