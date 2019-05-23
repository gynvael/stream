#ifdef __unix__
#  include <poll.h>
#endif
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include "console.h"
#include "context.h"

pid_t Console::GetPid() const {
  return pid_;
}

void Console::ResetPid() {
  pid_ = -1;
}

void Console::HandleSurfaceChange(XImage *surface) {
  surface_ = surface;
}

void Console::ResizeTextBuffer(uint32_t w, uint32_t h) {
  unsigned int columns = std::max(w / char_width_, 1U);
  lines_ = std::max(h / char_height_, 1U);

  buffer_.Resize(columns, scrollback_lines_);

  // TODO: send signal to child
}

bool Console::SpawnChild() {
#ifdef __unix__
  master_ = getpt();
  if (master_ == -1) {
    perror("getpt");
    return false;
  }

  char slave_path[256]{};
  if (ptsname_r(master_, slave_path, sizeof(slave_path)) != 0) {
    perror("ptsname_r");
    close(master_);
    return false;
  }

  if (grantpt(master_) != 0) {
    perror("grantpt");
    close(master_);
    return false;
  }

  if (unlockpt(master_) != 0) {
    perror("unlockpt");
    close(master_);
    return false;
  }

  int slave = open(slave_path, O_RDWR);
  if (slave < 0) {
    perror("open");
    close(master_);
    return false;
  }

  pid_ = fork();
  if (pid_ == -1) {
    perror("fork");
    close(master_);
    close(slave);
    return false;
  }

  if (pid_ == 0) {
    // Child process.
    close(0);  // stdin
    close(1);  // stdout
    close(2);  // stderr
    close(master_);
    master_ = -1;

    dup2(slave, 0);
    dup2(slave, 1);
    dup2(slave, 2);

    close(slave);

    if (execl("/bin/bash", "/bin/bash", nullptr) == -1) {
      abort();
    }
  }

  // Parent process.
  close(slave);
#endif
  read_th_ = std::make_unique<std::thread>(&Console::ReaderWorker, this);
  return true;
}

bool Console::ProcessStateNormal(uint8_t ch) {
  switch (ch) {
    case '\n': buffer_.LineFeed(); break;
    case '\r': buffer_.CarriageReturn(); break;
    case '\x1b'/* ESC */: ChangeState(state_t::ST_ESC); break;

    default:
      buffer_.InsertCharacter(ch);
  }

  return true;
}

bool Console::ProcessStateEscape(uint8_t ch) {
  switch (ch) {
    case '[': ChangeState(state_t::ST_CSI); break;

    default:
      buffer_.InsertCharacter('\x1b');
      buffer_.InsertCharacter(ch);
  }
  return true;
}

void Console::ProcessSGR(std::vector<uint8_t>& buffer) {
  // Ubuntu colors (from Wikipedia)
  // TODO: move this to use Color from textbuffer.h
  int kANSIColors[][3] = {
    { 1, 1, 1 },
    { 222, 56, 43 },
    { 57, 181, 74 },
    { 255, 199, 6 },
    { 0, 111, 184 },
    { 118, 38, 113 },
    { 44, 181, 233 },
    { 204, 204, 204 },
    { 128, 128, 128 },
    { 255, 0, 0 },
    { 0, 255, 0 },
    { 255, 255, 0 },
    { 0, 0, 255 },
    { 255, 0, 255 },
    { 0, 255, 255 },
    { 255, 255, 255 }
  };

  buffer.push_back(0);

  // TODO: don't use pointers, reeally
  char *p = (char*)&buffer[0];
  char *end = (char*)&buffer[buffer.size() - 1];

  // TODO: move this away from here - intensity should be persistant
  bool intensity = false;

  while (p < end && p != nullptr) {
    long value = strtol(p, &p, 10);
    if (value == 0) {
      intensity = false;
    }

    if (value == 1) {
      intensity = true;
    }

    if (value >= 30 && value <= 37) {
      int *rgb = intensity ? kANSIColors[value - 30 + 8] :
                             kANSIColors[value - 30];
      buffer_.ChangeForegroundColor(rgb[2], rgb[1], rgb[0]);
    }

    if (*p == ';' || *p == ':') {
      p++;
    }
  }

}

bool Console::ProcessStateCSI(uint8_t ch) {
  if (last_state_ != state_t::ST_CSI) {
    csi_buffer_.clear();
  }

  // Received "\x1b[".
  switch (ch) {
    case 'm': ProcessSGR(csi_buffer_); break;

    default:
      csi_buffer_.push_back(ch);
      return true;
  }

  ChangeState(state_t::ST_NORMAL);
  return true;
}

bool Console::ProcessOutputByte(uint8_t ch) {
  switch (state_) {
    case state_t::ST_NORMAL: return ProcessStateNormal(ch);
    case state_t::ST_ESC: return ProcessStateEscape(ch);
    case state_t::ST_CSI: return ProcessStateCSI(ch);

    default:
      fprintf(stderr, "Unknown state!");
      abort();
  }
}

void Console::ProcessOutput(uint8_t *data, size_t size) {
  size_t i = 0;
  while (i < size) {
    // ProcessOutputByte will return true if the byte was fully processed,
    // otherwise it will return false, which means it must be further processed.
    while (1) {
      // Make sure that if state stays the same across ProcessOutputByte call,
      // the last_state_ gets updated as well.
      state_t current_state = state_;
      bool ret = ProcessOutputByte(data[i]);
      if (state_ == current_state) {
        last_state_ = current_state;
      }

      if (ret) {
        break;
      }
    }
    i++;
  }

  UpdateSurface();
}

void Console::UpdateSurface() {
  const auto chars = buffer_.GetCharacters();
  const auto attribs = buffer_.GetAttriubutes();
  const auto [w, h] = buffer_.GetSize();
  const uint32_t scroll_y = buffer_.GetScrollPosition();

  for (uint32_t j = scroll_y; j < h; j++) {
    for (uint32_t i = 0; i < w; i++) {
      const uint32_t x = i * char_width_;
      const uint32_t y = j * char_height_;
      const uint32_t ch = chars[i + j * w];
      const auto attrib = attribs[i + j * w];

      if (ch == 0 || ch == ' ') {
        continue;
      } // TODO: maybe clear cell

      ctx_->font_renderer->RenderGlyph(
          surface_, x, y, ch,
          attrib.fg.r, attrib.fg.g, attrib.fg.b);
    }
  }

  if (ctx_->wnd != nullptr) {
    ctx_->wnd->RedrawWindowIfConsoleActive(this);
  }


}

void Console::ReaderWorker() {
#ifdef __unix__
  uint8_t buf[4096];

  pollfd fds[] = {
    { master_, POLLIN | POLLPRI | POLLRDHUP | POLLERR, 0 /* ignored */ }
  };

  while (!end_threads_.load()) {

    int ret = poll(fds, 1, 50 /* ms */);
    if (ret == -1) {
      perror("poll");
      break;
    }

    if (ret == 0) {
      // Timeout.
      continue;
    }

    const int revents = fds[0].revents;

    if ((revents & POLLIN)) {
      ssize_t buf_read = read(master_, buf, sizeof(buf));
      if (buf_read == -1) {
        perror("ReaderWorker");
      }

      fwrite(buf, 1, buf_read, stdout);
      ProcessOutput(buf, (size_t)buf_read);

    } else {
      fprintf(stderr, "poll revents == %.x\n", revents);
    }
  }
#else
  SDL_Delay(3000);
  UpdateSurface();


#endif
}

void Console::CloseMaster() {
  end_threads_.store(true);
  read_th_->join();
#ifdef __unix__
  close(master_);
#endif
  master_ = -1;
}

