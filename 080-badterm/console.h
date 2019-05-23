#pragma once
#ifndef _GNU_SOURCE
#  define _GNU_SOURCE
#endif
#include <atomic>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include <errno.h>
#include <SDL2/SDL.h>
#include <vector>
#include <cstdint>
#include <thread>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>

#include "textbuffer.h"

struct Context;

class Console {
 public:
  static const uint32_t kCharWidth = 11;  // In pixels.
  static const uint32_t kCharHeight = 16;  // In pixels.

 private:
  enum class state_t {
    ST_NORMAL,
    ST_ESC,  // Received ESC code, awaiting next character.
    ST_CSI   // Received Control Sequence Introducer.
  };

  void ReaderWorker();
  void ProcessOutput(uint8_t *data, size_t size);
  bool ProcessOutputByte(uint8_t ch);
  bool ProcessStateNormal(uint8_t ch);
  bool ProcessStateEscape(uint8_t ch);
  bool ProcessStateCSI(uint8_t ch);

  void ProcessSGR(std::vector<uint8_t>& buffer);

  inline void ChangeState(state_t st) {
    last_state_ = state_;
    state_ = st;
  }

  std::vector<uint8_t> csi_buffer_;

  state_t state_ = state_t::ST_NORMAL;
  state_t last_state_ = state_t::ST_NORMAL;

  std::atomic<bool> end_threads_{false}; //
  XImage *surface_ = nullptr;
  TextBuffer buffer_;
  pid_t pid_ = -1;
  int master_ = -1;
  std::unique_ptr<std::thread> read_th_;

  uint32_t char_width_ = kCharWidth;
  uint32_t char_height_ = kCharHeight;

  uint32_t lines_ = 1;  // Actual console size.
  uint32_t scrollback_lines_ = 1000;

  Context* ctx_ = nullptr;  // Console is not the owner of this object.

 public:
  Console(Context* ctx) : ctx_(ctx) {}

  bool Initialize();

  pid_t GetPid() const;
  void ResetPid();

  void HandleSurfaceChange(XImage *surface);
  void ResizeTextBuffer(uint32_t w, uint32_t h);  // Pixels.

  bool SpawnChild();
  void CloseMaster();

  void UpdateSurface();
};


